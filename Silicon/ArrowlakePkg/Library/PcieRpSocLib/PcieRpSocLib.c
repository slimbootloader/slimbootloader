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
#include <Include/P2SbSidebandAccessLib.h>
#include <Include/PcieRegs.h>
#include <Register/PchRegs.h>
#include <Register/PchBdfAssignment.h>
#include <Register/PchPcieRpRegs.h>
#include <Library/MtlSocPcieRpLib.h>
#include <Register/PchRegsPcr.h>
#include <Library/P2SbSocLib.h>
#include <Library/PchInfoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Include/P2SbSidebandAccessLib.h>

typedef struct {
  UINT8     DevNum;
  P2SB_PID  Pid;
  UINTN     RpNumBase;
  UINT8     NumOfRp;
  BOOLEAN   OnIoe;
  BOOLEAN   IocDecoded;
} MTL_SOC_PCIE_CONTROLLER_INFO;

GLOBAL_REMOVE_IF_UNREFERENCED MTL_SOC_PCIE_CONTROLLER_INFO  mMtlSocMPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1, MTL_SOC_PID_SPA,  0, 4, FALSE, FALSE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_5, MTL_SOC_PID_SPB,  4, 4, FALSE, FALSE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9, MTL_SOC_PID_SPC,  8, 1, FALSE, TRUE },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10, MTL_SOC_PID_PXPD, 9, 1, TRUE, TRUE},
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_11, MTL_SOC_PID_PXPE, 10, 1, TRUE, TRUE},
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_12, MTL_SOC_PID_PXPF, 11, 1, TRUE, TRUE}
};

GLOBAL_REMOVE_IF_UNREFERENCED MTL_SOC_PCIE_CONTROLLER_INFO  mMtlSocSPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10, MTL_SOC_PID_PXPA, 0, 2, FALSE, TRUE},
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10, MTL_SOC_PID_PXPB, 2, 1, FALSE, TRUE},
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10, MTL_SOC_PID_PXPD, 3, 1, TRUE, TRUE},
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_12, MTL_SOC_PID_PXPF, 4, 1, TRUE, TRUE},
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_11, MTL_SOC_PID_PXPE, 5, 1, TRUE, TRUE}
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
  if (MtlIsSocM ()) {
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
  } else if (MtlIsSocS ()) {
    if (RpIndex < 2) {
      return 0;
    } else if (RpIndex <= 6) {
      return (UINT32) RpIndex - 1;
    }
  }
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
  if (MtlIsSocM ()) {
    if (RpIndex < 8) {
      return (UINT16)(RpIndex % 4);
    } else {
      return 0;
    }
  } else {
    if (RpIndex < 2) {
      return (UINT16) RpIndex;
    } else {
      return 0;
    }
  }
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
  P2SB_SIDEBAND_REGISTER_ACCESS  RpAccess;
  P2SB_SIDEBAND_ACCESS_METHOD    AccessMethod;
  P2SB_CONTROLLER                P2SbController;

  PcieInfo = NULL;
  if (MtlIsSocM ()) {
    PcieInfo = mMtlSocMPcieControllerInfo;
  } else if (MtlIsSocS ()) {
    PcieInfo = mMtlSocSPcieControllerInfo;
  }
  Index    = MtlSocRpIndexToControllerIndex (RpNumber);
  *RpDev   = 0xFF;
  *RpFun   = 0xFF;

  if (Index == 0xFF || PcieInfo == NULL) {
    return EFI_NOT_FOUND;
  }

  ZeroMem (&RpAccess, sizeof (P2SB_SIDEBAND_REGISTER_ACCESS));
  FuncIndex = RpNumber - PcieInfo[Index].RpNumBase;
  *RpDev = PcieInfo[Index].DevNum;
  if (PcieInfo[Index].OnIoe) {
    MtlIoeGetP2SbController (&P2SbController);
    if (MtlIoeIsP2SbHidden ()) {
      AccessMethod = P2SbMmioAccess;
    } else {
      AccessMethod = P2SbMsgAccess;
    }
    BuildP2SbSidebandAccess (
      &P2SbController,
      PcieInfo[Index].Pid,
      MtlSocPcieRpGetSbiFid (RpNumber),
      P2SbPrivateConfig,
      AccessMethod,
      FALSE,
      &RpAccess
      );
  } else {
    MtlSocGetP2SbController (&P2SbController);
    BuildP2SbSidebandAccess (
      &P2SbController,
      PcieInfo[Index].Pid,
      MtlSocPcieRpGetSbiFid (RpNumber),
      P2SbPrivateConfig,
      P2SbMmioAccess,
      FALSE,
      &RpAccess
      );
  }
  PciePcd = RpAccess.Access.Read32 (&RpAccess.Access, R_SPX_SIP16_PCR_PCD);
  if (PciePcd != 0xFFFFFFFF) {
    *RpFun = (PciePcd >> (FuncIndex * S_SPX_PCR_PCD_RP_FIELD)) & B_SPX_PCR_PCD_RP1FN;
  }
  return EFI_SUCCESS;
}

/**
  This function returns PID according to PCIe controller index

  @param[in] ControllerIndex     PCIe controller index

  @retval P2SB_PID    Returns PID for SBI Access
**/
P2SB_PID
MtlSocGetPcieControllerSbiPid (
  IN UINT32  ControllerIndex
  )
{
  if (MtlIsSocM()) {
    if (ControllerIndex < ARRAY_SIZE (mMtlSocMPcieControllerInfo)) {
      return mMtlSocMPcieControllerInfo[ControllerIndex].Pid;
    }
  } else if (MtlIsSocS()) {
    if (ControllerIndex < ARRAY_SIZE (mMtlSocSPcieControllerInfo)) {
      return mMtlSocSPcieControllerInfo[ControllerIndex].Pid;
    }
  }
  return 0xFF;
}

/**
  This function returns PID according to Root Port Number

  @param[in] RpIndex     Root Port Index (0-based)

  @retval P2SB_PID   Returns PID for SBI Access
**/
P2SB_PID
MtlSocGetRpSbiPid (
  IN UINTN  RpIndex
  )
{
  UINT32   ControllerIndex;

  ControllerIndex = MtlSocRpIndexToControllerIndex ((UINTN)RpIndex);
  return MtlSocGetPcieControllerSbiPid (ControllerIndex);
}

/**
  Checks whether RpIndex is located on IOE die.

  @param[in] RpIndex  Root port index.

  @return TRUE if root port is located on IOE die.
**/
BOOLEAN
MtlSocIsRpOnIoeDie (
  IN UINT32  RpIndex
  )
{
  UINT32  ControllerIndex;

  ControllerIndex = MtlSocRpIndexToControllerIndex ((UINTN)RpIndex);
  if (MtlIsSocM()) {
    if (ControllerIndex < ARRAY_SIZE (mMtlSocMPcieControllerInfo)) {
      return mMtlSocMPcieControllerInfo[ControllerIndex].OnIoe;
    }
  } else if (MtlIsSocS()) {
    if (ControllerIndex < ARRAY_SIZE (mMtlSocSPcieControllerInfo)) {
      return mMtlSocSPcieControllerInfo[ControllerIndex].OnIoe;
    }
  }
  return FALSE;
}

BOOLEAN
MtlSocIsRpIocDecoded (
  IN UINT32  RpIndex
  )
{
  UINT32  ControllerIndex;

  ControllerIndex = MtlSocRpIndexToControllerIndex ((UINTN)RpIndex);
  if (MtlIsSocM()) {
    if (ControllerIndex < ARRAY_SIZE (mMtlSocMPcieControllerInfo)) {
      return mMtlSocMPcieControllerInfo[ControllerIndex].IocDecoded;
    }
  } else if (MtlIsSocS()) {
    if (ControllerIndex < ARRAY_SIZE (mMtlSocSPcieControllerInfo)) {
      return mMtlSocSPcieControllerInfo[ControllerIndex].IocDecoded;
    }
  }

  return FALSE;
}
