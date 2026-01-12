/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciSegmentLib.h>
#include <Include/P2SbSidebandAccessLib.h>
#include <P2SbController.h>
#include <PcieRegs.h>
#include <Register/PchRegs.h>
#include <Register/PchPcieRpRegs.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchBdfAssignment.h>
#include <Include/PcrDefine.h>
#include <Library/PtlPcdGpioTopologyLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PchInfoLib.h>
#include <Library/P2SbSocLib.h>

#define PEG_PCIE_CONTROLLER_PORTS              2u

typedef struct {
  UINT8            DevNum;
  P2SB_PORT_16_ID  Pid;
  UINTN            RpNumBase;
  UINT8            NumOfRp;

} PTL_SOC_PCIE_CONTROLLER_INFO;

GLOBAL_REMOVE_IF_UNREFERENCED PTL_SOC_PCIE_CONTROLLER_INFO  mPtlPcdPPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1, .Pid.Pid16bit = PTL_SID_F2_PID_PXPA,      0, 4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_5, .Pid.Pid16bit = PTL_SID_F2_PID_PXPB,      4, 4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9, .Pid.Pid16bit = PTL_P_H_SID_F2_PID_PXPC,  8, 2 }
};

GLOBAL_REMOVE_IF_UNREFERENCED PTL_SOC_PCIE_CONTROLLER_INFO  mPtlPcdHPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1, .Pid.Pid16bit = PTL_SID_F2_PID_PXPA,      0,  4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_5, .Pid.Pid16bit = PTL_SID_F2_PID_PXPB,      4,  4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9, .Pid.Pid16bit = PTL_P_H_SID_F2_PID_PXPC,  8,  2 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_11, .Pid.Pid16bit = PTL_H_SID_F2_PID_PXPD,  10, 1 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_12, .Pid.Pid16bit = PTL_H_SID_F2_PID_PXPE,  11, 1 }
};

GLOBAL_REMOVE_IF_UNREFERENCED PTL_SOC_PCIE_CONTROLLER_INFO  mWclPcdPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1, .Pid.Pid16bit = PTL_SID_F2_PID_PXPA,  0, 4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9, .Pid.Pid16bit = PTL_P_H_SID_F2_PID_PXPC,  4, 2  }
};

/**
  Translate RpIndex to controller index.

  @param[in] RpIndex  Root port index

  @return ControllerIndex
  @retval 0xFF Failed to lookup controller index.
**/
UINT32
PtlPchRpIndexToControllerIndex (
  IN UINT32  RpIndex
  )
{
  return RpIndex / PCH_PCIE_CONTROLLER_PORTS;
}

/**
  Get the FID number of the PCIe root port.

  @param[in] RpIndex  Index of the root port

  @return FID number to use when sending SBI msg
**/
UINT16
PtlPchPcieRpGetSbiFid (
  IN UINT32  RpIndex
  )
{
  if (IsWclPcd ()) {
    if (RpIndex < 4) {
      return (UINT16)(RpIndex % PCH_PCIE_CONTROLLER_PORTS);
    } else if (RpIndex < 6) {
      return (UINT16)(RpIndex % PEG_PCIE_CONTROLLER_PORTS);
    } else {
      return 0;
    }
  } else {
    if (RpIndex < 8) {
      return (UINT16)(RpIndex % PCH_PCIE_CONTROLLER_PORTS);
    } else if (RpIndex < 10) {
      return (UINT16)(RpIndex % PEG_PCIE_CONTROLLER_PORTS);
    } else {
      return 0;
    }
  }
}

/**
  Get the FID number of the PCIe root port.

  @param[in] Index  Index of the root port

  @return PTL_SOC_PCIE_CONTROLLER_INFO of specified Index
**/
PTL_SOC_PCIE_CONTROLLER_INFO *GetPtlPcieControllerInfo (UINT32 Index)
{
  PTL_SOC_PCIE_CONTROLLER_INFO   *PcieInfo = NULL;

  if (IsWclPcd ()) {
    if (Index >= (sizeof(mWclPcdPcieControllerInfo)/sizeof(PTL_SOC_PCIE_CONTROLLER_INFO))) {
      return NULL;
    }
    PcieInfo = mWclPcdPcieControllerInfo;
  } else if (PtlIsPcdP ()) {
    if (Index >= (sizeof(mPtlPcdPPcieControllerInfo)/sizeof(PTL_SOC_PCIE_CONTROLLER_INFO))) {
      return NULL;
    }
    PcieInfo = mPtlPcdPPcieControllerInfo;
  } else {
     if (Index >= (sizeof(mPtlPcdHPcieControllerInfo)/sizeof(PTL_SOC_PCIE_CONTROLLER_INFO))) {
      return NULL;
     }
    PcieInfo = mPtlPcdHPcieControllerInfo;
  }
  return PcieInfo;
}


/**
  Get PTL SoC Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
PtlPchGetPcieRpDevFun (
  IN  UINTN                 RpNumber,
  OUT UINT16                 *RpDev,
  OUT UINT16                 *RpFun
  )
{
  UINT32                         Index = MAX_UINT32;
  UINTN                          FuncIndex;
  UINT32                         PciePcd;
  PTL_SOC_PCIE_CONTROLLER_INFO   *PcieInfo;
  P2SB_SIDEBAND_REGISTER_ACCESS  RpAccess;
  P2SB_CONTROLLER                P2SbController;
  P2SB_SIDEBAND_ACCESS_METHOD    AccessMethod;
  EFI_STATUS                    Status;

  Index = PtlPcdRpIndexToControllerIndex (RpNumber);
  if (Index == 0xFF) {
    DEBUG((DEBUG_WARN, "PtlPchGetPcieRpDevFun: Invalid RpNumber(0x%x)\n", RpNumber));
    return EFI_NOT_FOUND;
  }

  PcieInfo = GetPtlPcieControllerInfo(Index);
  if (PcieInfo == NULL) {
    DEBUG((DEBUG_WARN, "PtlPchGetPcieRpDevFun: Not a valid Pcie Controller Index(0x%x)\n", Index));
    return EFI_NOT_FOUND;
  }

  *RpDev   = 0;
  *RpFun   = 0;
  FuncIndex = RpNumber - PcieInfo[Index].RpNumBase;
  *RpDev = PcieInfo[Index].DevNum;

  Status = PtlPcdGetP2SbController(&P2SbController);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "Failed to GetP2SB Controller"));
    return Status;
  }

  //
  // P2SB is hidden at the end of PEI. In case we are executing after that event
  // use MMIO access method.
  //
  if (PciSegmentRead16 (P2SbController.PciCfgBaseAddr) == 0xFFFF) {
    AccessMethod = P2SbMmioAccess;
  } else {
    AccessMethod = P2SbMsgAccess;
  }

  ZeroMem (&RpAccess, sizeof (P2SB_SIDEBAND_REGISTER_ACCESS));

  BuildP2SbSidebandAccess (
    &P2SbController,
    PcieInfo[Index].Pid.PortId.LocalPid,
    PtlPchPcieRpGetSbiFid ((UINT32) RpNumber),
    P2SbPrivateConfig,
    AccessMethod,
    FALSE,
    &RpAccess
    );

  PciePcd = RpAccess.Access.Read32 (&RpAccess.Access, R_SPX_SIP16_PCR_PCD);
  if (PciePcd != 0xFFFFFFFF) {
    *RpFun = (PciePcd >> (FuncIndex * S_SPX_PCR_PCD_RP_FIELD)) & B_SPX_PCR_PCD_RP1FN;
  } else {
    *RpFun = RpNumber % 8;
  }
  return EFI_SUCCESS;
}