/** @file
  PCIe RP SoC library.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/P2SbLib.h>
#include <Include/PcieRegs.h>
#include <Register/PchRegs.h>
#include <Register/PchBdfAssignment.h>
#include <Register/PchPcieRpRegs.h>
#include <Library/MtlSocPcieRpLib.h>
#include <Register/PchRegsPcr.h>
#include <Library/P2SbSocLib.h>
#include <Library/PchInfoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PciLib.h>
#include <Guid/OsBootOptionGuid.h>

#define DEVICE_TABLE_P2SB_INSTANCE_SOC     0
#define DEVICE_TABLE_P2SB_INSTANCE_IOE     1
#define DEVICE_TABLE_P2SB_INSTANCE_PCH     2

typedef struct {
  UINT8     DevNum;
  P2SB_PID  Pid;
  UINT8     RpNumBase;
  UINT8     NumOfRp;
  BOOLEAN   OnIoe;
} MTL_SOC_PCIE_CONTROLLER_INFO;

GLOBAL_REMOVE_IF_UNREFERENCED MTL_SOC_PCIE_CONTROLLER_INFO  mMtlSocMPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1,  MTL_SOC_PID_SPA,  0,  4, FALSE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_5,  MTL_SOC_PID_SPB,  4,  4, FALSE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9,  MTL_SOC_PID_SPC,  8,  1, FALSE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10, MTL_SOC_PID_PXPD, 9,  1, TRUE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_11, MTL_SOC_PID_PXPE, 10, 1, TRUE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_12, MTL_SOC_PID_PXPF, 11, 1, TRUE }
};

GLOBAL_REMOVE_IF_UNREFERENCED MTL_SOC_PCIE_CONTROLLER_INFO  mMtlSocSPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10, MTL_SOC_PID_PXPA, 0, 2, FALSE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10, MTL_SOC_PID_PXPB, 2, 1, FALSE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10, MTL_SOC_PID_PXPD, 3, 1, TRUE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_12, MTL_SOC_PID_PXPF, 4, 1, TRUE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_11, MTL_SOC_PID_PXPE, 5, 1, TRUE }
};

/**
  Translate RpIndex to controller index.

  @param[in] RpIndex  Root port index

  @return ControllerIndex
  @retval 0xFF Failed to lookup controller index.
**/
UINT32
MtlSocRpIndexToControllerIndex (
  IN UINTN  RpIndex
  )
{
#if FixedPcdGetBool(PcdMtlSSupport)
  if (RpIndex < 2) {
    return 0;
  } else if (RpIndex <= 6) {
    return (UINT32) RpIndex - 1;
  }
#else
  if (RpIndex < 4) {
    return 0;
  } else if (RpIndex < 8) {
    return 1;
  } else if (RpIndex == 8) {
    return 2;
  } else if (RpIndex == 9) {
    return 3;
  } else if (RpIndex == 10) {
    return 4;
  } else if (RpIndex == 11) {
    return 5;
  }
#endif

  return 0xFF;
}

/**
  Get the FID number of the PCIe root port.

  @param[in] RpIndex  Index of the root port

  @return FID number to use when sending SBI msg
**/
UINT16
MtlSocPcieRpGetSbiFid (
  IN UINTN  RpIndex
  )
{
#if FixedPcdGetBool(PcdMtlSSupport)
  if (RpIndex < 2) {
    return (UINT16) RpIndex;
  } else {
    return 0;
  }
#else
  if (RpIndex < 8) {
    return (UINT16)(RpIndex % 4);
  } else {
    return 0;
  }
#endif
}


/**
  Get device address

  If the device is PCI device, the device address format is 0x00BBDDFF, where
  BB, DD and FF are PCI bus, device and function number.
  If the device is MMIO device, the device address format is 0x0000MMMMMM000000, where
  MM should be non-zero value.

  @param[in]  DeviceInstance     The device instance number starting from 0.

  @retval     Device address for a given device instance, return 0 if the device
              could not be found from device table.
**/
UINT64
GetP2sbDeviceAddr2 (
  IN  UINT8          DeviceInstance
  )
{
  PLT_DEVICE_TABLE   *DeviceTable;
  PLT_DEVICE         *Device;
  UINT64             DeviceBase;
  UINT32             Index;

  DeviceBase  = 0;
  Device      = NULL;
  DeviceTable = (PLT_DEVICE_TABLE *)GetDeviceTable();
  for (Index = 0; Index < DeviceTable->DeviceNumber; Index++) {
    Device = &DeviceTable->Device[Index];
    if ((Device->Type == PlatformDeviceP2sb) && (Device->Instance == DeviceInstance)){
      break;
    }
  }

  if (DeviceTable->DeviceNumber != Index) {
    DeviceBase = Device->Dev.DevAddr | ((UINT64)Device->Reserved << 32);
  }

  return DeviceBase;
}


/**
  Get MTL SoC Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber           Root port physical number. (0-based)
  @param[out] RpDev              Return corresponding root port device number.
  @param[out] RpFun              Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
EFIAPI
MtlSocGetPcieRpDevFun (
  IN  UINTN    RpNumber,
  OUT UINT16   *RpDev,
  OUT UINT16   *RpFun
  )
{
  UINT32                         Index;
  UINTN                          FuncIndex;
  UINT32                         PciePcd;
  MTL_SOC_PCIE_CONTROLLER_INFO   *PcieInfo;
  UINT64                          P2sbBase;
  UINT16                          Fid;

  Index = MtlSocRpIndexToControllerIndex (RpNumber);
  if (Index == 0xFF) {
    return EFI_INVALID_PARAMETER;
  }

#if FixedPcdGetBool(PcdMtlSSupport)
  PcieInfo = mMtlSocSPcieControllerInfo;
#else
  PcieInfo = mMtlSocMPcieControllerInfo;
#endif

  if (PcieInfo[Index].OnIoe) {
    P2sbBase = GetP2sbDeviceAddr2 (DEVICE_TABLE_P2SB_INSTANCE_IOE);
  } else {
    P2sbBase = GetP2sbDeviceAddr2 (DEVICE_TABLE_P2SB_INSTANCE_SOC);
  }
  DEBUG ((DEBUG_INFO, "P2sbBase 0x%llx\n",P2sbBase));

  if ((P2sbBase & 0x0FFFFFFF) != 0) {
    P2sbBase = TO_PCI_LIB_ADDRESS (P2sbBase);
  }

  *RpDev    = PcieInfo[Index].DevNum;
  *RpFun    = 0xFF;
  FuncIndex = RpNumber - PcieInfo[Index].RpNumBase;
  Fid       = MtlSocPcieRpGetSbiFid (RpNumber);
  PciePcd   = P2sbRead32 (P2sbBase, PcieInfo[Index].Pid, Fid, R_SPX_SIP16_PCR_PCD);
  if (PciePcd != 0xFFFFFFFF) {
    *RpFun = (PciePcd >> (FuncIndex * S_SPX_PCR_PCD_RP_FIELD)) & B_SPX_PCR_PCD_RP1FN;
  }
  DEBUG ((DEBUG_INFO, "%d, P2sbBase 0x%llx, PID=0x%x, Fid = 0x%x, PciePcd = 0x%x\n", Index, P2sbBase, PcieInfo[Index].Pid, Fid, PciePcd));

  return EFI_SUCCESS;
}

