/** @file
  MTL PCH PCIe RP library.

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
#include <PcieRegs.h>
#include <Register/PchRegs.h>
#include <Register/PchPcieRpRegs.h>
#include <Register/PchRegsPcr.h>
#include <Register/MtlPchBdfAssignment.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PciLib.h>
#include <Guid/OsBootOptionGuid.h>

#define DEVICE_TABLE_P2SB_INSTANCE_PCH     2

typedef struct {
  UINT8     DevNum;
  P2SB_PID  Pid;
  UINTN     RpNumBase;
  UINT8     NumOfRp;
} MTL_PCH_PCIE_CONTROLLER_INFO;

GLOBAL_REMOVE_IF_UNREFERENCED MTL_PCH_PCIE_CONTROLLER_INFO  mMtlPchSPcieControllerInfo[] = {
  {MTL_PCH_PCIE_RP1_DEVICE_NUM, MTL_PCH_PID_SPA, 0, 4},
  {MTL_PCH_PCIE_RP5_DEVICE_NUM, MTL_PCH_PID_SPB, 4, 4},
  {MTL_PCH_PCIE_RP9_DEVICE_NUM, MTL_PCH_PID_SPC, 8, 4},
  {MTL_PCH_PCIE_RP13_DEVICE_NUM, MTL_PCH_PID_SPD, 12, 4},
  {MTL_PCH_PCIE_RP17_DEVICE_NUM, MTL_PCH_PID_SPE, 16, 4},
  {MTL_PCH_PCIE_RP21_DEVICE_NUM, MTL_PCH_PID_SPF, 20, 4}
};


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
GetP2sbDeviceAddr (
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
    DeviceBase = Device->Dev.DevAddr | ( (UINT64)Device->Reserved << 32);
  }

  return DeviceBase;
}


/**
  Get MTL PCH Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber           Root port physical number. (0-based)
  @param[out] RpDev              Return corresponding root port device number.
  @param[out] RpFun              Return corresponding root port function number.

  Rretval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
EFIAPI
MtlPchGetPcieRpDevFun (
  IN  UINTN    RpNumber,
  OUT UINT16   *RpDev,
  OUT UINT16   *RpFun
  )
{
  UINTN                          Index;
  UINTN                          FuncIndex;
  UINT32                         PciePcd;
  MTL_PCH_PCIE_CONTROLLER_INFO   *PcieInfo;
  UINT64                          P2sbBase;
  UINT16                          Fid;

  Index = RpNumber / PCH_PCIE_CONTROLLER_PORTS;
  if (Index >= ARRAY_SIZE (mMtlPchSPcieControllerInfo)) {
    return EFI_INVALID_PARAMETER;
  }

  P2sbBase = GetP2sbDeviceAddr (DEVICE_TABLE_P2SB_INSTANCE_PCH);
  if ((P2sbBase & 0x0FFFFFFF) != 0) {
    P2sbBase = TO_PCI_LIB_ADDRESS (P2sbBase);
  }

  PcieInfo  = mMtlPchSPcieControllerInfo;
  *RpDev    = PcieInfo[Index].DevNum;
  *RpFun    = 0xFF;
  FuncIndex = RpNumber - PcieInfo[Index].RpNumBase;
  Fid       = RpNumber % 4;
  PciePcd   = P2sbRead32 (P2sbBase, PcieInfo[Index].Pid, Fid, R_SPX_SIP16_PCR_PCD);
  if (PciePcd != 0xFFFFFFFF) {
    *RpFun = (PciePcd >> (FuncIndex * S_SPX_PCR_PCD_RP_FIELD)) & B_SPX_PCR_PCD_RP1FN;
  } else {
    *RpFun = RpNumber % 8;
  }

  return EFI_SUCCESS;
}

