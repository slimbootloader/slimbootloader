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
#include <Include/P2SbSidebandAccessLib.h>
#include <PcieRegs.h>
#include <Register/PchRegs.h>
#include <Register/PchPcieRpRegs.h>
#include <Register/PchRegsPcr.h>
#include <Register/MtlPchBdfAssignment.h>
#include <Include/PcrDefine.h>
#include <Library/MtlPchGpioTopologyLib.h>
#include <Library/BaseMemoryLib.h>

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
  Translate RpIndex to controller index.

  @param[in] RpIndex  Root port index

  @return ControllerIndex
  @retval 0xFF Failed to lookup controller index.
**/
UINT32
MtlPchRpIndexToControllerIndex (
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
MtlPchPcieRpGetSbiFid (
  IN UINT32  RpIndex
  )
{
  return RpIndex % 4;
}

/**
  Get MTL SoC Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
MtlPchGetPcieRpDevFun (
  IN  UINT32                 RpNumber,
  OUT UINT16                 *RpDev,
  OUT UINT16                 *RpFun
  )
{
  UINT32                         Index = MAX_UINT32;
  UINTN                          FuncIndex;
  UINT32                         PciePcd;
  MTL_PCH_PCIE_CONTROLLER_INFO   *PcieInfo;
  P2SB_SIDEBAND_REGISTER_ACCESS  RpAccess;
  P2SB_CONTROLLER                P2SbController;
  P2SB_SIDEBAND_ACCESS_METHOD    AccessMethod;

  PcieInfo = mMtlPchSPcieControllerInfo;
  Index    = MtlPchRpIndexToControllerIndex (RpNumber);
  *RpDev   = 0;
  *RpFun   = 0;

  if (Index >= ARRAY_SIZE (mMtlPchSPcieControllerInfo) || PcieInfo == NULL || Index == MAX_UINT32) {
    return EFI_NOT_FOUND;
  }

  FuncIndex = RpNumber - PcieInfo[Index].RpNumBase;
  *RpDev = PcieInfo[Index].DevNum;
  MtlPchGetP2SbController (&P2SbController);
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
    PcieInfo[Index].Pid,
    MtlPchPcieRpGetSbiFid ((UINT32) RpNumber),
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


/**
  This function returns PID according to PCIe controller index

  @param[in] ControllerIndex     PCIe controller index

  @retval P2SB_PID    Returns PID for SBI Access
**/
P2SB_PID
MtlPchGetPcieControllerSbiPid (
  IN UINT32  ControllerIndex
  )
{
  return mMtlPchSPcieControllerInfo[ControllerIndex].Pid;
}

/**
  This function returns PID according to Root Port Number

  @param[in] RpIndex     Root Port Index (0-based)

  @retval P2SB_PID   Returns PID for SBI Access
**/
P2SB_PID
MtlPchGetRpSbiPid (
  IN UINT32  RpIndex
  )
{
  UINT32   ControllerIndex;

  ControllerIndex = MtlPchRpIndexToControllerIndex (RpIndex);
  return MtlPchGetPcieControllerSbiPid (ControllerIndex);
}
