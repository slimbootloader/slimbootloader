/** @file
  This file contains routines for GPIO

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Library/BaseLib.h>
#include <Uefi/UefiBaseType.h>
#include <Library/GpioV2Lib.h>
#include <Library/GpioV2SiLib.h>
#include <Include/GpioV2Config.h>
#include <Library/P2sbLib.h>
#include <Library/PciLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/ConfigDataLib.h>


/**
  This procedure verifies if requested GpioGroup definition is ment for platform that it is used on

  @param[in]  GpioPad             GPIO pad

  @retval TRUE                    The PAD has valid group info
  @retval FALSE                   The PAD has not valid group info
**/
BOOLEAN
IsPadValid (
  IN GPIOV2_PAD                   GpioPad
  )
{
  GPIOV2_CONTROLLER               *Controller;
  GPIOV2_COMMUNITY                *Community;
  UINT32                          ChipsetId;
  UINT32                          CommunityIndex;
  UINT32                          GroupIndex;
  UINT32                          PadIndex;

  Controller     = GpioGetController (GpioPad);
  ChipsetId      = GPIOV2_PAD_GET_CHIPSETID (GpioPad);
  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  GroupIndex     = GPIOV2_PAD_GET_GROUP_INDEX (GpioPad);
  PadIndex       = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  if ((ChipsetId & GpioGetThisChipsetId ()) == 0) {
    DEBUG((DEBUG_INFO, "[GPIO] PAD 0x%x invalid ChipsetId 0x%x (0x%x)\n", GpioPad, ChipsetId, GpioGetThisChipsetId ()));
    return FALSE;
  }

  if (CommunityIndex >= Controller->CommunityNum) {
    DEBUG((DEBUG_INFO, "[GPIO] PAD 0x%x invalid community %d (%d)\n", GpioPad, CommunityIndex, Controller->CommunityNum));
    return FALSE;
  }

  Community = &Controller->Communities[CommunityIndex];
  if (GroupIndex >= Community->GroupsNum) {
    DEBUG((DEBUG_INFO, "[GPIO] PAD 0x%x invalid group %d (%d)\n", GpioPad, GroupIndex, Community->GroupsNum));
    return FALSE;
  }

  if (PadIndex >= Community->Groups[GroupIndex].PadsNum) {
    DEBUG((DEBUG_INFO, "[GPIO] PAD 0x%x invalid pad idx %d (%d)\n", GpioPad, PadIndex,Community->Groups[GroupIndex].PadsNum));
    return FALSE;
  }

  return TRUE;
}

/**
  This procedure retrieves name of requested Gpio Pad

  @param[in]  GpioPad             Gpio Pad.
  @param[in]  NameBufferLength    Maximum number of characters to be stored in NameBuffer
  @param[out] NameBuffer          Pointer to a buffer for Gpio Pad name

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GetPadName (
  IN  GPIOV2_PAD                  GpioPad,
  IN  UINT32                      NameBufferLength,
  OUT CHAR8                       *NameBuffer
  )
{
  GPIOV2_CONTROLLER               *Controller;
  UINT32                          CommunityIndex;
  UINT32                          GroupIndex;
  UINT32                          PadIndex;

  Controller     = GpioGetController (GpioPad);
  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  GroupIndex     = GPIOV2_PAD_GET_GROUP_INDEX (GpioPad);
  PadIndex       = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  AsciiSPrint (
    NameBuffer,
    NameBufferLength,
    "%a_%02d",
    Controller->Communities[CommunityIndex].Groups[GroupIndex].Name,
    PadIndex
    );

  return EFI_SUCCESS;
}


/**
  Retrieves register offset

  @param[in]  GpioPad             Gpio Pad.
  @param[in]  Register            Register for which user want to retrieve offset.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
UINT32
EFIAPI
GetRegisterOffset (
  IN  GPIOV2_PAD                  GpioPad,
  IN  GPIOV2_REGISTER             Register
  )
{
  GPIOV2_CONTROLLER               *Controller;
  UINT32                          CommunityIndex;
  UINT32                          GroupIndex;
  UINT32                          PadIndex;
  UINT32                          RegisterOffset;
  GPIOV2_GROUP_REGISTERS_OFFSETS  *RegOffset;
  UINT8                           PadDwSize;

  Controller     = GpioGetController (GpioPad);
  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  GroupIndex     = GPIOV2_PAD_GET_GROUP_INDEX (GpioPad);
  PadIndex       = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);
  RegOffset      = &Controller->Communities[CommunityIndex].Groups[GroupIndex].RegisterOffsets;
  PadDwSize      = Controller->Communities[CommunityIndex].Groups[GroupIndex].PadDwSize;

  switch (Register) {
    case GpioV2PadOwnReg:
      RegisterOffset = RegOffset->PadOwn + (PadIndex / 8) * 0x04;
      break;
    case GpioV2PadCfgLockReg:
      RegisterOffset = RegOffset->PadCfgLock;
      break;
    case GpioV2PadCfgLockTxReg:
      RegisterOffset = RegOffset->PadCfgLockTx;
      break;
    case GpioV2PadHostOwnReg:
      RegisterOffset = RegOffset->HostOwn;
      break;
    case GpioV2GpiIsReg:
      RegisterOffset = RegOffset->GpiIs;
      break;
    case GpioV2GpiIeReg:
      RegisterOffset = RegOffset->GpiIe;
      break;
    case GpioV2GpiGpeStsReg:
      RegisterOffset = RegOffset->GpiGpeSts;
      break;
    case GpioV2GpiGpeEnReg:
      RegisterOffset = RegOffset->GpiGpeEn;
      break;
    case GpioV2SmiStsReg:
      RegisterOffset = RegOffset->SmiSts;
      break;
    case GpioV2SmiEnReg:
      RegisterOffset = RegOffset->SmiEn;
      break;
    case GpioV2NmiStsReg:
      RegisterOffset = RegOffset->NmiSts;
      break;
    case GpioV2NmiEnReg:
      RegisterOffset = RegOffset->NmiEn;
      break;
    case GpioV2Dw0Reg:
      RegisterOffset = RegOffset->Dw0 + PadIndex * PadDwSize;
      break;
    case GpioV2Dw1Reg:
      RegisterOffset = RegOffset->Dw0 + 0x04 + PadIndex * PadDwSize;
      break;
    case GpioV2Dw2Reg:
      RegisterOffset = RegOffset->Dw0 + 0x08 + PadIndex * PadDwSize;
      break;
    case GpioV2MiscCfg:
      RegisterOffset = Controller->Communities[CommunityIndex].RegisterOffsets.MiscCfg;
      break;
    default:
      DEBUG ((DEBUG_INFO, "Wrong Register Offset\n"));
      ASSERT(FALSE);
      return 0;
  }

  return RegisterOffset;
}


/**
  This procedure sets Gpio Pad output state

  @param[in] GpioPad             GPIO pad
  @param[in] RegOffset           The register offset
  @param[in] AndValue            And value for this operation
  @param[in] OrValue             Or value for this operation

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
GpioWriteAndThenOr32 (
  IN GPIOV2_PAD                   GpioPad,
  IN UINT32                       RegOffset,
  IN UINT32                       AndValue,
  IN UINT32                       OrValue
  )
{
  GPIOV2_CONTROLLER               *Controller;
  GPIOV2_COMMUNITY                *Community;
  UINTN                           CommunityIndex;
  UINT64                          P2sbBase;

  Controller     = GpioGetController (GpioPad);
  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  Community      = &Controller->Communities[CommunityIndex];

  if (Controller->SbRegBar != 0) {
    P2sbBase = Controller->SbRegBar;
  } else {
    if (PciRead32 ((UINTN)Controller->P2sbBase) == 0xFFFFFFFF) {
      DEBUG ((DEBUG_ERROR, "P2SB (0x%x) is hidden and SBREG_BAR is unknown.\n", Controller->P2sbBase));
      ASSERT (FALSE);
    }
    P2sbBase = (PciRead32 ((UINTN)Controller->P2sbBase + 0x10) & 0xFFFFFFF0) | PciRead32 ((UINTN)Controller->P2sbBase + 0x14);
    if (P2sbBase == 0) {
      P2sbBase = Controller->P2sbBase;
    }
  }

  P2SbAndThenOr32 (P2sbBase, (P2SB_PID)Community->Pid, 0, RegOffset, AndValue, OrValue);
  return EFI_SUCCESS;
}


/**
  Read GPIO register

  @param[in] GpioPad             GPIO pad
  @param[in] RegOffset           The register offset

  @retval data             The data read from the given reg offset.
**/
UINT32
EFIAPI
GpioRead32 (
  IN GPIOV2_PAD                   GpioPad,
  IN UINT32                       RegOffset
  )
{
  GPIOV2_CONTROLLER               *Controller;
  GPIOV2_COMMUNITY                *Community;
  UINTN                           CommunityIndex;
  UINT32                          Data32;
  UINT64                          P2sbBase;

  Controller     = GpioGetController (GpioPad);
  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  Community      = &Controller->Communities[CommunityIndex];

  if (Controller->SbRegBar != 0) {
    P2sbBase = Controller->SbRegBar;
  } else {
    if (PciRead32 ((UINTN)Controller->P2sbBase) == 0xFFFFFFFF) {
      DEBUG ((DEBUG_ERROR, "P2SB (0x%x) is hidden and SBREG_BAR is unknown.\n", Controller->P2sbBase));
      ASSERT (FALSE);
    }
    P2sbBase = (PciRead32 ((UINTN)Controller->P2sbBase + 0x10) & 0xFFFFFFF0) | PciRead32 ((UINTN)Controller->P2sbBase + 0x14);
    if (P2sbBase == 0) {
      P2sbBase = Controller->P2sbBase;
    }
  }

  Data32 = P2sbRead32 ((UINTN)P2sbBase, (P2SB_PID)Community->Pid, 0, RegOffset);
  return Data32;
}


/**
  Set the specified bits with the Value to the GpioPad Register

  @param[in] GpioPad              GPIO pad
  @param[in] Register             The register name to set
  @param[in] BitPosition          The bit position to set
  @param[in] BitMask              The bit mask to set
  @param[in] Value                The value to set

**/
VOID
SetRegBits (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_REGISTER              Register,
  IN UINT32                       BitPosition,
  IN UINT32                       BitMask,
  IN UINT32                       Value
  )
{
  UINT32                          RegOffset;
  UINT32                          AndValue;
  UINT32                          OrValue;

  AndValue  = (UINT32)~(BitMask << BitPosition);
  OrValue   = (Value & BitMask) << BitPosition;
  RegOffset = GetRegisterOffset (GpioPad, Register);
  GpioWriteAndThenOr32 (GpioPad, RegOffset, AndValue, OrValue);
}


/**
  Read specified bits from the given GpioPad Register

  @param[in] GpioPad              GPIO pad
  @param[in] Register             The register name
  @param[in] BitPosition          The bits start position
  @param[in] BitMask              The bits mask

  @retval    The register value from the specified bits.
**/
UINT32
GetRegBits (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_REGISTER              Register,
  IN UINT32                       BitPosition,
  IN UINT32                       BitMask
  )
{
  UINT32                          RegOffset;
  UINT32                          RegValue;

  RegOffset = GetRegisterOffset (GpioPad, Register);
  RegValue  = GpioRead32 (GpioPad, RegOffset);

  return (RegValue >> BitPosition) & BitMask;
}

/**
  Check if the specified bit is set or not for the given GpioPad Register

  @param[in] GpioPad              GPIO pad
  @param[in] Register             The register name to check
  @param[in] BitPosition          The bit position to check

  @retval    TRUE if the specified BIT is set or FALSE if it is not set.
**/
BOOLEAN
IsRegBitSet (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_REGISTER              Register,
  IN UINT32                       BitPosition
  )
{
  if (GetRegBits (GpioPad, Register, BitPosition, 1) == 1) {
    return TRUE;
  }
  return FALSE;
}

/**
  Set GpioPad PAD mode

  @param[in] GpioPad              Gpio Pad
  @param[in] PadMode              GPIO pad mode value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetPadMode (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_PAD_MODE              PadMode
  )
{
  if (!IsPadValid (GpioPad)) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  if (PadMode != GpioV2PadModeHardwareDefault) {
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_MODE_DW0_POS, GPIOV2_PAD_MODE_MASK, PadMode >> 1);
  }

  return EFI_SUCCESS;
}


/**
  Reads current GPIO Pad Mode

  @param[in]  GpioPad             GPIO Pad.
  @param[out] PadMode             Pointer to a buffer for GPIO Pad Mode

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetPadMode (
  IN GPIOV2_PAD                   GpioPad,
  OUT GPIOV2_PAD_MODE             *PadMode
  )
{
  UINT32                          BitsValue;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  BitsValue = GetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_MODE_DW0_POS, GPIOV2_PAD_MODE_MASK);
  *PadMode  = GPIO_ASSIGN_VALUE (BitsValue);

  return EFI_SUCCESS;
}


/**
  Sets GpioPad host software ownership configuration

  @param[in] GpioPad              GPIO pad
  @param[in] HostSwOwnership      Host software pad ownership value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetHostSwOwnership (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_HOSTSW_OWN            HostSwOwnership
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (HostSwOwnership != GpioV2HostOwnDefault) {
    SetRegBits (GpioPad, GpioV2PadHostOwnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), GPIOV2_PAD_HOST_OWNERSHIP_MASK, HostSwOwnership >> 1);
  }

  return EFI_SUCCESS;
}


/**
  Gets GpioPad host software ownership configuration

  @param[in]  GpioPad             GPIO pad
  @param[out] HostSwOwn           Pointer to a buffer for HostSwOwn

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetHostSwOwnership (
  IN  GPIOV2_PAD                  GpioPad,
  OUT GPIOV2_HOSTSW_OWN           *HostSwOwn
  )
{
  UINT32                          BitsValue;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  BitsValue  = GetRegBits (GpioPad, GpioV2PadHostOwnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), GPIOV2_PAD_HOST_OWNERSHIP_MASK);
  *HostSwOwn = GPIO_ASSIGN_VALUE (BitsValue);

  return EFI_SUCCESS;
}


/**
  Sets Gpio Pad output state

  @param[in] GpioPad              GPIO pad
  @param[in] OutputState          The GPIO PAD output state value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetTx (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_PAD_STATE             OutputState
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (OutputState != GpioV2StateDefault) {
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_OUTPUT_STATE_DW0_POS, GPIOV2_PAD_OUTPUT_STATE_MASK, OutputState >> 1);
  }
  return EFI_SUCCESS;
}


/**
  Get current Gpio Pad output state

  @param[in]  GpioPad             GPIO pad
  @param[out] OutputState         Pointer to a buffer for output state

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetTx (
  IN  GPIOV2_PAD                  GpioPad,
  OUT GPIOV2_PAD_STATE            *OutputState
  )
{
  UINT32                          BitsValue;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  BitsValue  = GetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_OUTPUT_STATE_DW0_POS, GPIOV2_PAD_OUTPUT_STATE_MASK);
  *OutputState = GPIO_ASSIGN_VALUE (BitsValue);

  return EFI_SUCCESS;
}

/**
  Get current Gpio Pad input state

  @param[in] GpioPad              GPIO pad
  @param[in] InputState           Pointer to a buffer for input state

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetRx (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_PAD_STATE             *InputState
  )
{
  UINT32                          BitsValue;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  BitsValue   = GetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_INPUT_STATE_DW0_POS, GPIOV2_PAD_INPUT_STATE_MASK);
  *InputState = GPIO_ASSIGN_VALUE (BitsValue);

  return EFI_SUCCESS;
}

/**
  Sets GpioPad Reset Configuration

  @param[in] GpioPad              GPIO pad
  @param[in] ResetConfig          Reset Configuration value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetResetConfig (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_RESET_CONFIG          ResetConfig
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (ResetConfig != GpioV2ResetDefault) {
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RESET_CONFIG_DW0_POS, GPIOV2_PAD_RESET_CONFIG_MASK, ResetConfig >> 1);
  }
  return EFI_SUCCESS;
}

/**
  Gets GpioPad Reset Configuration

  @param[in]  GpioPad             GPIO pad
  @param[out] ResetConfig         Reset Configuration buffer

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetResetConfig (
  IN  GPIOV2_PAD                  GpioPad,
  OUT GPIOV2_RESET_CONFIG         *ResetConfig
  )
{
  UINT32                          BitsValue;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  BitsValue    = GetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RESET_CONFIG_DW0_POS, GPIOV2_PAD_RESET_CONFIG_MASK);
  *ResetConfig = GPIO_ASSIGN_VALUE (BitsValue);

  return EFI_SUCCESS;
}

/**
  Sets GpioPad termination configuration

  @param[in] GpioPad              GPIO pad
  @param[in] TermConfig           Termination configuration value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetTerminationConfig (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_TERMINATION_CONFIG    TermConfig
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (TermConfig != GpioV2TermDefault) {
    SetRegBits (GpioPad, GpioV2Dw1Reg, GPIOV2_PAD_TERMINATION_CONFIG_DW1_POS, GPIOV2_PAD_TERMINATION_CONFIG_MASK, TermConfig >> 1);
  }
  return EFI_SUCCESS;
}

/**
  Gets GpioPad termination configuration

  @param[in]  GpioPad             GPIO pad
  @param[out] TermConfig          Termination configuration buffer

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetTerminationConfig (
  IN  GPIOV2_PAD                  GpioPad,
  OUT GPIOV2_TERMINATION_CONFIG   *TermConfig
  )
{
  UINT32                          BitsValue;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  BitsValue   = GetRegBits (GpioPad, GpioV2Dw1Reg, GPIOV2_PAD_TERMINATION_CONFIG_DW1_POS, GPIOV2_PAD_TERMINATION_CONFIG_MASK);
  *TermConfig = GPIO_ASSIGN_VALUE (BitsValue);

  return EFI_SUCCESS;
}

/**
  Set GpioPad GPIO PAD config Lock (register PADCFGLOCK)
  If pad configuration is locked. It would impact PAD configuration registers (except GPIO TX state)
  and GPI NMI EN register and HostSw OWN register.

  @param[in] GpioPad              GPIO pad
  @param[in] ConfigLock           Config Lock value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetPadConfigLock (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_PAD_LOCK              ConfigLock
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (ConfigLock != GpioV2LockHardwareDefault) {
    SetRegBits (GpioPad, GpioV2PadCfgLockReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), GPIOV2_PAD_LOCK_MASK, ConfigLock >> 1);
  }
  return EFI_SUCCESS;
}

/**
  Get GpioPad GPIO PAD config Lock value (register PADCFGLOCK)

  @param[in]  GpioPad             GPIO pad
  @param[out] ConfigLock          config lock value buffer

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetPadConfigLock (
  IN  GPIOV2_PAD                  GpioPad,
  OUT GPIOV2_PAD_LOCK             *ConfigLock
  )
{
  UINT32                          BitsValue;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  BitsValue   = GetRegBits (GpioPad, GpioV2PadCfgLockReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), GPIOV2_PAD_LOCK_MASK);
  *ConfigLock = GPIO_ASSIGN_VALUE (BitsValue);

  return EFI_SUCCESS;
}


/**
  Sets GpioPad Output state lock (register PADCFGLOCKTX)
  when Output status lock is set, GPIO TX state register is read-only.

  @param[in] GpioPad              GPIO pad
  @param[in] TxLock               Output state lock value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetTxLock (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_PAD_LOCK              TxLock
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (TxLock != GpioV2LockHardwareDefault) {
    SetRegBits (GpioPad, GpioV2PadCfgLockTxReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), GPIOV2_PAD_LOCK_MASK, TxLock >> 1);
  }
  return EFI_SUCCESS;
}

/**
  Get GpioPad Output State Lock (register PADCFGLOCKTX)

  @param[in]  GpioPad             GPIO pad
  @param[out] TxLock              Output state lock buffer

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetTxLock (
  IN  GPIOV2_PAD                  GpioPad,
  OUT GPIOV2_PAD_LOCK             *TxLock
  )
{
  UINT32                          BitsValue;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  BitsValue   = GetRegBits (GpioPad, GpioV2PadCfgLockTxReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), GPIOV2_PAD_LOCK_MASK);
  *TxLock = GPIO_ASSIGN_VALUE (BitsValue);

  return EFI_SUCCESS;
}

/**
  Set GPIO direction related registers RX/TX/RXINV

  @param[in] GpioPad              GPIO pad
  @param[in] Direction            GPIO direction setting

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetDirection (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_DIRECTION             Direction
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Direction == GpioV2DirDefault) {
    return EFI_SUCCESS;
  }

  switch (Direction) {
    case GpioV2DirInOut:
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_TX_DISABLE_DW0_POS, GPIOV2_PAD_TX_DISABLE_MASK, 0);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RX_DISABLE_DW0_POS, GPIOV2_PAD_RX_DISABLE_MASK, 0);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_INPUT_INVERSION_DW0_POS, GPIOV2_PAD_INPUT_INVERSION_MASK, 0);
      break;
    case GpioV2DirInInvOut:
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_TX_DISABLE_DW0_POS, GPIOV2_PAD_TX_DISABLE_MASK, 0);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RX_DISABLE_DW0_POS, GPIOV2_PAD_RX_DISABLE_MASK, 0);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_INPUT_INVERSION_DW0_POS, GPIOV2_PAD_INPUT_INVERSION_MASK, 1);
      break;
    case GpioV2DirIn:
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_TX_DISABLE_DW0_POS, GPIOV2_PAD_TX_DISABLE_MASK, 1);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RX_DISABLE_DW0_POS, GPIOV2_PAD_RX_DISABLE_MASK, 0);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_INPUT_INVERSION_DW0_POS, GPIOV2_PAD_INPUT_INVERSION_MASK, 0);
      break;
    case GpioV2DirInInv:
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_TX_DISABLE_DW0_POS, GPIOV2_PAD_TX_DISABLE_MASK, 1);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RX_DISABLE_DW0_POS, GPIOV2_PAD_RX_DISABLE_MASK, 0);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_INPUT_INVERSION_DW0_POS, GPIOV2_PAD_INPUT_INVERSION_MASK, 1);
      break;
    case GpioV2DirOut:
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_TX_DISABLE_DW0_POS, GPIOV2_PAD_TX_DISABLE_MASK, 0);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RX_DISABLE_DW0_POS, GPIOV2_PAD_RX_DISABLE_MASK, 1);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_INPUT_INVERSION_DW0_POS, GPIOV2_PAD_INPUT_INVERSION_MASK, 0);
      break;
    case GpioV2DirNone:
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_TX_DISABLE_DW0_POS, GPIOV2_PAD_TX_DISABLE_MASK, 1);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RX_DISABLE_DW0_POS, GPIOV2_PAD_RX_DISABLE_MASK, 1);
      SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_INPUT_INVERSION_DW0_POS, GPIOV2_PAD_INPUT_INVERSION_MASK, 0);
      break;
    default:
      break;
  }

  return EFI_SUCCESS;
}


/**
  Get GPIO direction settings from registers RX/TX/RXINV

  @param[in]  GpioPad             GPIO pad
  @param[out] Direction           GPIO direction setting

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetDirection (
  IN  GPIOV2_PAD                  GpioPad,
  OUT GPIOV2_DIRECTION            *Direction
  )
{
  BOOLEAN                         TxDisable;
  BOOLEAN                         RxDisable;
  BOOLEAN                         InputInversion;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  TxDisable      = IsRegBitSet (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_TX_DISABLE_DW0_POS);
  RxDisable      = IsRegBitSet (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RX_DISABLE_DW0_POS);
  InputInversion = IsRegBitSet (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_INPUT_INVERSION_DW0_POS);

  if (TxDisable) {
    if (RxDisable) {
      *Direction = GpioV2DirNone;
    } else {
      if (InputInversion) {
        *Direction = GpioV2DirInInv;
      } else {
        *Direction = GpioV2DirIn;
      }
    }
  } else {
    if (RxDisable) {
      *Direction = GpioV2DirOut;
    } else {
      if (InputInversion) {
        *Direction = GpioV2DirInInvOut;
      } else {
        *Direction = GpioV2DirInOut;
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Set GPIO interrupt related registers
  Includes INT_EN/GPE_EN/SMI_EN/NMI_EN/RXEVCFG and interrupt route registers.

  @param[in] GpioPad              GPIO pad
  @param[in] InterruptConfig      GPIO interrupt setting

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetInterrupt (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_INT_CONFIG            InterruptConfig
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (InterruptConfig == GpioV2IntDefault) {
    return EFI_SUCCESS;
  }

  //
  // All Interrupt/wake events turned off by default
  //
  SetRegBits (GpioPad, GpioV2NmiEnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), 1, 0);
  SetRegBits (GpioPad, GpioV2SmiEnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), 1, 0);
  SetRegBits (GpioPad, GpioV2GpiGpeEnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), 1, 0);
  SetRegBits (GpioPad, GpioV2GpiIeReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), 1, 0);

  if ((InterruptConfig & GpioV2IntNmi) >> 1 != 0) {
    SetRegBits (GpioPad, GpioV2NmiEnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), 1, 1);
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_GPIROUTNMI_DW0_POS, 1, 1);
  }
  if ((InterruptConfig & GpioV2IntSmi) >> 1 != 0) {
    SetRegBits (GpioPad, GpioV2SmiEnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), 1, 1);
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_GPIROUTSMI_DW0_POS, 1, 1);
  }
  if ((InterruptConfig & GpioV2IntSci) >> 1 != 0) {
    SetRegBits (GpioPad, GpioV2GpiGpeEnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), 1, 1);
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_GPIROUTSCI_DW0_POS, 1, 1);
  }
  if ((InterruptConfig & GpioV2IntApic) >> 1 != 0) {
    SetRegBits (GpioPad, GpioV2GpiIeReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad), 1, 1);
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_GPIROUTIOXAPIC_DW0_POS, 1, 1);
  }
  if ((InterruptConfig & GpioV2IntLevel) >> 1 != 0) {
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RXEV_DW0_POS, GPIOV2_PAD_RXEV_MASK, GpioV2IntRxEvCfgLevel >> 1);
  } else if ((InterruptConfig & GpioV2IntEdge) >> 1 != 0) {
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RXEV_DW0_POS, GPIOV2_PAD_RXEV_MASK, GpioV2IntRxEvCfgEdge >> 1);
  } else if ((InterruptConfig & GpioV2IntLvlEdgDis) >> 1 != 0) {
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RXEV_DW0_POS, GPIOV2_PAD_RXEV_MASK, GpioV2IntRxEvCfgDisable >> 1);
  } else if ((InterruptConfig & GpioV2IntBothEdge) >> 1 != 0) {
    SetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RXEV_DW0_POS, GPIOV2_PAD_RXEV_MASK, GpioV2IntRxEvCfgLevelEdge >> 1);
  }
  return EFI_SUCCESS;
}


/**
  Get GPIO interrupt settings
  Includes INT_EN/GPE_EN/SMI_EN/NMI_EN/RXEVCFG and interrupt route registers.

  @param[in]  GpioPad             GPIO pad
  @param[out] InterruptConfig     GPIO interrupt setting

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetInterrupt (
  IN  GPIOV2_PAD                  GpioPad,
  OUT GPIOV2_INT_CONFIG           *InterruptConfig
  )
{
  UINT32                          IntCfg;
  GPIOV2_RXEVCFG                  RxEvCfg;
  BOOLEAN                         NmiEn;
  BOOLEAN                         SmiEn;
  BOOLEAN                         GpiGpeEn;
  BOOLEAN                         GpiIntEn;
  BOOLEAN                         RouteNmiSet;
  BOOLEAN                         RouteSmiSet;
  BOOLEAN                         RouteSciSet;
  BOOLEAN                         RouteIoxApicSet;

  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  NmiEn           = IsRegBitSet (GpioPad, GpioV2NmiEnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad));
  SmiEn           = IsRegBitSet (GpioPad, GpioV2SmiEnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad));
  GpiGpeEn        = IsRegBitSet (GpioPad, GpioV2GpiGpeEnReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad));
  GpiIntEn        = IsRegBitSet (GpioPad, GpioV2GpiIeReg, GPIOV2_PAD_GET_PAD_INDEX (GpioPad));
  RouteNmiSet     = IsRegBitSet (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_GPIROUTNMI_DW0_POS);
  RouteSmiSet     = IsRegBitSet (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_GPIROUTSMI_DW0_POS);
  RouteSciSet     = IsRegBitSet (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_GPIROUTSCI_DW0_POS);
  RouteIoxApicSet = IsRegBitSet (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_GPIROUTIOXAPIC_DW0_POS);

  IntCfg = GpioV2IntDis;
  if (NmiEn && RouteNmiSet) {
    IntCfg |= GpioV2IntNmi;
  }
  if (SmiEn && RouteSmiSet) {
    IntCfg |= GpioV2IntSmi;
  }
  if (GpiGpeEn && RouteSciSet) {
    IntCfg |= GpioV2IntSci;
  }
  if (GpiIntEn && RouteIoxApicSet) {
    IntCfg |= GpioV2IntApic;
  }

  RxEvCfg = (GPIOV2_RXEVCFG) GetRegBits (GpioPad, GpioV2Dw0Reg, GPIOV2_PAD_RXEV_DW0_POS, GPIOV2_PAD_RXEV_MASK);
  switch (GPIO_ASSIGN_VALUE (RxEvCfg)) {
    case GpioV2IntRxEvCfgLevel:
      IntCfg |= GpioV2IntLevel;
      break;
    case GpioV2IntRxEvCfgEdge:
      IntCfg |= GpioV2IntEdge;
      break;
    case GpioV2IntRxEvCfgDisable:
      IntCfg |= GpioV2IntLvlEdgDis;
      break;
    case GpioV2IntRxEvCfgLevelEdge:
      IntCfg |= GpioV2IntBothEdge;
      break;
    default:
      break;
  }

  *InterruptConfig = IntCfg;

  return EFI_SUCCESS;
}


/**
  Set GPIO MiscCfg value

  @param[in]  GpioPad             GPIO pad
  @param[in]  MiscCfgValue        GPIO miscellaneous configuration value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
SetMiscCfg (
  IN  GPIOV2_PAD      GpioPad,
  IN  UINT32          MiscCfgValue
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  SetRegBits (GpioPad, GpioV2MiscCfg, 0, 0xFFFFFFFF, MiscCfgValue);

  return EFI_SUCCESS;
}

/**
  Get GPIO MiscCfg value

  @param[in]  GpioPad             GPIO pad
  @param[out] MiscCfgValue        GPIO miscellaneous configuration value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
GetMiscCfg (
  IN  GPIOV2_PAD      GpioPad,
  OUT UINT32          *MiscCfgValue
  )
{
  if (!IsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  *MiscCfgValue = GetRegBits (GpioPad, GpioV2MiscCfg, 0, 0xFFFFFFFF);

  return EFI_SUCCESS;
}


/**
  Gets group index to be mapped to GPE DWx.

  @param[in]  ChipsetId           Chipset ID of the GPIO controller
  @param[out] GpioIndex0          GPIO group index to be mapped to GPE DW0
  @param[out] GpioIndex1          GPIO group index to be mapped to GPE DW1
  @param[out] GpioIndex2          GPIO group index to be mapped to GPE DW2

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_DEVICE_ERROR        GPIO MISC register is not set correctly.
**/
EFI_STATUS
EFIAPI
GpioGetGpeMapping (
  IN  UINT16             ChipsetId,
  OUT UINT8              *GpioIndex0,
  OUT UINT8              *GpioIndex1,
  OUT UINT8              *GpioIndex2
  )
{
  EFI_STATUS             Status;
  UINT8                  MiscCfgDwX[3];
  GPIOV2_PAD_GROUP       GroupToGpeDwX[3] = {0};
  UINT32                 MiscCfgValue;
  UINT32                 Index;
  GPIOV2_PAD             GpioPad;
  GPIOV2_CONTROLLER      *Controller;
  UINT32                 CommunityIndex;
  GPIOV2_COMMUNITY       *Community;
  UINT32                 GroupIndex;
  GPIOV2_GROUP           *Group;

  GpioPad = GPIOV2_PAD_ID (0, ChipsetId, 0, 0, 0, 0);
  Status  = GetMiscCfg (GpioPad, &MiscCfgValue);
  ASSERT_EFI_ERROR (Status);

  MiscCfgDwX[0] = (UINT8) ((MiscCfgValue & GPIOV2_PAD_MISCCFG_GPE0_DW0_MASK) >> GPIOV2_PAD_MISCCFG_GPE0_DW0_POS);
  MiscCfgDwX[1] = (UINT8) ((MiscCfgValue & GPIOV2_PAD_MISCCFG_GPE0_DW1_MASK) >> GPIOV2_PAD_MISCCFG_GPE0_DW1_POS);
  MiscCfgDwX[2] = (UINT8) ((MiscCfgValue & GPIOV2_PAD_MISCCFG_GPE0_DW2_MASK) >> GPIOV2_PAD_MISCCFG_GPE0_DW2_POS);

  Controller = GpioGetController (GpioPad);
  for (Index = 0; Index < 3; Index++) {
    for (CommunityIndex = 0; CommunityIndex < Controller->CommunityNum; CommunityIndex++) {
      Community = &Controller->Communities[CommunityIndex];
      for (GroupIndex = 0; GroupIndex < Community->GroupsNum; GroupIndex++) {
        Group = &Community->Groups[GroupIndex];
        if (MiscCfgDwX[Index] == Group->GroupToGpeMapping.GpioGpeDwxVal) {
          GroupToGpeDwX[Index] = Group->GpioPadGroup;
          break;
        }
      }
    }
  }

  if ((GroupToGpeDwX[0] == GroupToGpeDwX[1]) || (GroupToGpeDwX[1] == GroupToGpeDwX[2]) || (GroupToGpeDwX[0] == GroupToGpeDwX[2])) {
    return EFI_DEVICE_ERROR;
  }

  *GpioIndex0 = (UINT8) GPIOV2_PAD_GET_GROUP_INDEX (GroupToGpeDwX[0]);
  *GpioIndex1 = (UINT8) GPIOV2_PAD_GET_GROUP_INDEX (GroupToGpeDwX[1]);
  *GpioIndex2 = (UINT8) GPIOV2_PAD_GET_GROUP_INDEX (GroupToGpeDwX[2]);

  return EFI_SUCCESS;
}


/**
  Set GpioPad using the PAD config data

  @param[in] GpioPad              Gpio Pad
  @param[in] PadConfig            Gpio Pad configuration

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid PAD (chipsetId, community, group or pad number)
**/
EFI_STATUS
EFIAPI
ConfigurePad (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_CONFIG                *PadConfig
  )
{
  EFI_STATUS                      Status;
  UINT32                          RegOffset;
  UINT32                          RegisterValue;

  Status = SetPadMode (GpioPad, PadConfig->PadMode);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((PadConfig->InterruptConfig & GpioV2IntSmi) >> 1 != 0) {
    SetHostSwOwnership (GpioPad, GpioV2HostOwnGpio);
  } else {
    SetHostSwOwnership (GpioPad, PadConfig->HostSwOwn);
  }

  //
  // Make sure we set a Tx state before setting a direction to avoid glitches on the GPIO line.
  // A glitch can happen if by default Tx of the GPIO pad is disabled and Tx state is set to drive
  // the pad low but signal on the board is pulled up. When SBL wants to take control over the GPIO pad
  // and enable the Tx and set the Tx state to high to match the platform default signal state we need to make
  // sure tha Tx is programed to high before we enable Tx otherwise signal on the board will experience a
  // very quick glitch which can lead to a number of problems.
  //
  SetTx (GpioPad, PadConfig->OutputState);
  SetDirection (GpioPad, PadConfig->Direction);

  SetInterrupt (GpioPad, PadConfig->InterruptConfig);
  SetResetConfig (GpioPad, PadConfig->ResetConfig);
  SetTerminationConfig (GpioPad, PadConfig->TerminationConfig);

  SetPadConfigLock (GpioPad, PadConfig->LockConfig);
  SetTxLock (GpioPad, PadConfig->LockTx);

  DEBUG_CODE_BEGIN ();
  RegOffset     = GetRegisterOffset (GpioPad, GpioV2Dw0Reg);
  RegisterValue = GpioRead32 (GpioPad, RegOffset);
  DEBUG ((DEBUG_INFO, "Pad:0x%x, DW0(0x%x)=0x%x  ", GpioPad, RegOffset, RegisterValue));

  RegOffset     = GetRegisterOffset (GpioPad, GpioV2Dw1Reg);
  RegisterValue = GpioRead32 (GpioPad, RegOffset);
  DEBUG ((DEBUG_INFO, "DW1(0x%x)=0x%x\n", RegOffset, RegisterValue));
  DEBUG_CODE_END ();

  return EFI_SUCCESS;
}


/**
  Program all the GPIO configurations based on GpioPadsConfigTable

  @param[in] GpioPadsConfigTable       Pointer to PadInitConfigTable
  @param[in] GpioPadsConfigTableSize   Size of PadInitConfigTable

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid input
**/
EFI_STATUS
GpioV2ConfigurePads (
  IN GPIOV2_INIT_CONFIG           *GpioConfigTable,
  IN UINT32                       GpioConfigTableSize
  )
{
  EFI_STATUS                      Status;
  UINT32                          Index;

  if ((GpioConfigTable == NULL) || (GpioConfigTableSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < GpioConfigTableSize; Index++) {
    Status = ConfigurePad (GpioConfigTable[Index].GpioPad, &GpioConfigTable[Index].GpioConfig);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Print the output of the GPIO Config table that was read from CfgData.

  @param[in] GpioCfgData          GPIO Config Data table.
  @param[in] GpioNum              Number of GPIO entries in the table.

**/
VOID
PrintGpioTable (
  IN GPIOV2_INIT_CONFIG           *GpioCfgData,
  IN UINT32                       GpioNum
  )
{
  UINT32            *DwData;
  UINT32             Index;

  for (Index = 0; Index < GpioNum; Index++) {
    DwData = (UINT32 *)&GpioCfgData[Index].GpioConfig;
    DEBUG ((DEBUG_INFO, "GPIO PAD: 0x%08X   DATA: 0x%08X 0x%08X\n", GpioCfgData[Index].GpioPad, DwData[0], DwData[1]));
  }
}


/**
  Convert GPIO info from config data to the format used by GPIO library

  SBL GPIO config data has DW0 and DW1.
  GPIO library used PAD, DW0 and DW1.
  Need get required info from SBL GPIO config data and fill into new
  GPIO table required by the GPIO library.

  @param[out] GpioTable           The GpioTable to be updated
  @param[in]  GpioCfgHdr          The GPIO cfg data header
  @param[in]  Offset              Offset of a particulr pin's DW0, DW1 in GpioCfg

  @retval   GpioTable            Pointer to fill the next gpio item
**/
UINT8 *
FillGpioTable (
  OUT  UINT8                      *GpioTable,
  IN  ARRAY_CFG_HDR               *GpioCfgHdr,
  IN  UINT32                      Offset
  )
{
  GPIOV2_PAD                      *GpioV2Pad;
  GPIOV2_CONFIG                   *DwCfg;
  UINT8                           *GpioData;

  // set DW1 and DW2
  GpioData = (UINT8 *)GpioCfgHdr + GpioCfgHdr->HeaderSize + Offset;
  DwCfg    = (GPIOV2_CONFIG *)(GpioTable + sizeof(GPIOV2_PAD));
  CopyMem (DwCfg, GpioData, GpioCfgHdr->ItemSize);

  // Set GPIO PAD
  GpioV2Pad = (GPIOV2_PAD *) GpioTable;
  *GpioV2Pad = GPIOV2_PAD_ID (0, 1 << DwCfg->ChipId, 0, DwCfg->ComIdx, DwCfg->GrpIdx, DwCfg->PadIdx);

  GpioTable += (sizeof(GPIOV2_PAD) + GpioCfgHdr->ItemSize);

  return GpioTable;
}


/**
  Configure the GPIO pins, available as part of platform specific GPIO CFG DATA.
  If the pins are not part of GPIO CFG DATA, call GpioConfigurePads() directly
  with the appropriate arguments.

  @param[in] Tag                  The Gpio Cfg data Tag ID
  @param[in] DataBuffer           Pointer to the Gpio Table to be programmed
  @param[in] Entries              Number of entries in Gpio Table

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_NOT_FOUND           If Gpio Config Data cant be found
**/
EFI_STATUS
EFIAPI
ConfigureGpioV2 (
  IN UINT16                       Tag,
  IN GPIOV2_INIT_CONFIG           *DataBuffer,
  IN UINT16                       Entries
  )
{
  ARRAY_CFG_HDR                   *GpioCfgCurrHdr;
  ARRAY_CFG_HDR                   *GpioCfgBaseHdr;
  ARRAY_CFG_HDR                   *GpioCfgHdr;
  EFI_STATUS                      Status;
  UINT32                          GpioEntries;
  UINT32                          Index;
  UINT32                          Offset;
  GPIOV2_INIT_CONFIG              *GpioCfgBuffer;
  UINT8                           *GpioTable;

  if ((Tag == CDATA_NO_TAG) && (Entries == 0 || DataBuffer == NULL) ) {
    DEBUG ((DEBUG_INFO, "Provide either Tag or Gpio Table info!\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // If no Tag provided, check for GpioTable info;
  // If    Tag provided, GpioTable params are don't care
  //
  if (Tag == CDATA_NO_TAG) {
    Status = GpioV2ConfigurePads (DataBuffer, Entries);
    return Status;
  }

  //
  // Find the GPIO CFG data for this platform
  //
  GpioCfgCurrHdr = (ARRAY_CFG_HDR *)FindConfigDataByTag (Tag);
  if (GpioCfgCurrHdr == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Find the base GPIO CFG Data
  //
  GpioCfgBaseHdr = NULL;
  if (GpioCfgCurrHdr->BaseTableId < 32) {
    GpioCfgBaseHdr = (ARRAY_CFG_HDR *)FindConfigDataByPidTag (GpioCfgCurrHdr->BaseTableId, Tag);
    if (GpioCfgBaseHdr == NULL) {
      return EFI_NOT_FOUND;
    }
    if (GpioCfgCurrHdr->ItemSize != GpioCfgBaseHdr->ItemSize) {
      return EFI_LOAD_ERROR;
    }
    GpioCfgHdr = GpioCfgBaseHdr;
  } else {
    GpioCfgHdr = GpioCfgCurrHdr;
  }

  GpioTable = (UINT8 *) AllocateTemporaryMemory (0);
  ASSERT (GpioTable != NULL);
  GpioCfgBuffer = (GPIOV2_INIT_CONFIG *) GpioTable;

  GpioEntries = 0;
  Offset      = 0;
  for (Index = 0; Index < GpioCfgHdr->ItemCount; Index++) {
    if (GpioCfgCurrHdr->BaseTableBitMask[Index >> 3] & (1 << (Index & 7))) {
      GpioTable = FillGpioTable (GpioTable, GpioCfgHdr, Offset);
      GpioEntries++;
    }
    Offset += GpioCfgHdr->ItemSize;
  }

  Offset = 0;
  if (GpioCfgBaseHdr != NULL) {
    for (Index = 0; Index < GpioCfgCurrHdr->ItemCount; Index++) {
      GpioTable = FillGpioTable (GpioTable, GpioCfgCurrHdr, Offset);
      Offset   += GpioCfgCurrHdr->ItemSize;
      GpioEntries++;
    }
  }

  DEBUG_CODE_BEGIN ();
  PrintGpioTable (GpioCfgBuffer, GpioEntries);
  DEBUG_CODE_END ();

  Status = GpioV2ConfigurePads (GpioCfgBuffer, GpioEntries);

  return Status;
}
