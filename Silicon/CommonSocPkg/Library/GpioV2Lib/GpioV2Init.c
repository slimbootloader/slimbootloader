/** @file
  This file contains routines for GPIO V2 Library

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/GpioSiLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/PrintLib.h>
#include <Library/GpioTopologyLib.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/GpioV2Virtual.h>

#define  GPIO_DEBUG_INFO     DEBUG_VERBOSE
#define  GPIO_DEBUG_ERROR    DEBUG_VERBOSE

/**
  This procedure verifies if requested GpioGroup definition is ment for platform that it is used on

  @param[in]  GpioGroup           GPIO pad
  @param[out] IsValid             Pointer to a buffer for validation information, TRUE or FALSE

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
IsGroupValid (
  IN  GPIOV2_PAD_GROUP        GpioGroup,
  OUT BOOLEAN                 *IsValid
  )
{
  UINT32  ChipsetId;
  UINT32  CommunityIndex;
  UINT32  GroupIndex;

  ChipsetId = GPIOV2_PAD_GET_CHIPSETID (GpioGroup);
  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioGroup);
  GroupIndex = GPIOV2_PAD_GET_GROUP_INDEX (GpioGroup);

  *IsValid = FALSE;
  if (ChipsetId != GpioGetChipId()) {
    DEBUG((DEBUG_ERROR, "[GPIOV2][IsGroupValid] Error: wrong ChipsetId; com:%d,grp:%d id: 0x%x (exp: 0x%x)\n", CommunityIndex, GroupIndex, ChipsetId, GpioGetChipId()));
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }

  if (CommunityIndex >= GpioGetCommunitiesNum()) {
    DEBUG((DEBUG_ERROR, "[GPIOV2][IsGroupValid] Error: wrong community index; com:%d (max: %d)\n", CommunityIndex, GpioGetCommunitiesNum()));
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }

  if (GroupIndex >= GpioGetCommunities(CommunityIndex)->GroupsNum) {
    DEBUG((DEBUG_ERROR, "[GPIOV2][IsGroupValid] Error: wrong group index; g:%d (max: %d)\n", GroupIndex, GpioGetCommunities(CommunityIndex)->GroupsNum));
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }

  *IsValid = TRUE;

  return EFI_SUCCESS;
}

/**
  This procedure verifies if requested GpioPad definition is ment for platform that it is used on

  @param[in] GpioPad              GPIO pad
  @param[out] IsValid             Pointer to a buffer for validation information, TRUE or FALSE

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
IsPadValid (
  IN  GPIOV2_PAD              GpioPad,
  OUT BOOLEAN                 *IsValid
  )
{
  UINT32  CommunityIndex;
  UINT32  GroupIndex;
  UINT32  PadIndex;

  if ((IsValid == NULL)) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  IsGroupValid ((GPIOV2_PAD_GROUP)GpioPad, IsValid);
  if (*IsValid != TRUE) {
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }

  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  GroupIndex = GPIOV2_PAD_GET_GROUP_INDEX (GpioPad);
  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  if (PadIndex >= SocGpioGetGroups(CommunityIndex,GroupIndex).PadsNum) {
    DEBUG((DEBUG_WARN, "[GPIOV2][IsPadValid] Error: Pad 0x%x, wrong pad index\n", GpioPad));
    *IsValid = FALSE;
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}

/**
  This procedure retrieves name of requested Gpio Pad

  @param[in] GpioPad              Gpio Pad. Please refer to GpioPinsYYY.h - where YYY name of the platform (eg. MTL, EBG, ...)
  @param[in] NameBufferLength     Maximum number of characters to be stored in NameBuffer
  @param[out] NameBuffer          Pointer to a buffer for Gpio Pad name

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GetPadName (
  IN  GPIOV2_PAD            GpioPad,
  IN  UINT32                NameBufferLength,
  OUT CHAR8                 *NameBuffer
  )
{
  UINT32 PadIndex;
  UINT32 CommunityIndex;
  UINT32 GroupIndex;

  if ( (NameBuffer == NULL)) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  if (GpioPad == GPIOV2_PAD_NONE) {
    AsciiSPrint (NameBuffer, NameBufferLength, "No muxing");
    return EFI_SUCCESS;
  }

  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  GroupIndex = GPIOV2_PAD_GET_GROUP_INDEX (GpioPad);
  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  if (AsciiSPrint (NameBuffer, NameBufferLength, "%a_%02d",
          SocGpioGetGroups(CommunityIndex,GroupIndex).Name,
          PadIndex) == 0) {
            ASSERT(FALSE);
            return EFI_INVALID_PARAMETER;
          }

  return EFI_SUCCESS;
}

/**
  This procedure retrieves register offset

  @param[in]  Register            Register for which user want to retrieve offset. Please refer to GpioV2Pad.h
  @param[in]  GpioPad             Gpio Pad. Please refer to GpioPinsYYY.h - where YYY name of the platform (eg. MTL, EBG, ...)
  @param[out] RegisterOffset      Pointer to a buffer for register offset

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GetRegisterOffset (
  IN  GPIOV2_REGISTER     Register,
  IN  GPIOV2_PAD          GpioPad,
  OUT UINT32              *RegisterOffset
  )
{
  UINT32             CommunityIndex;
  UINT32             GroupIndex;
  UINT32             PadIndex;

  ASSERT (RegisterOffset != NULL);
  if (RegisterOffset == NULL)  {
    return EFI_INVALID_PARAMETER;
  }

  *RegisterOffset = 0;

  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  GroupIndex = GPIOV2_PAD_GET_GROUP_INDEX (GpioPad);
  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  switch (Register) {
    case GpioV2PadOwnReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadOwn + (PadIndex / 8) * 0x04;
      break;
    case GpioV2PadCfgLockReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadCfgLock;
      break;
    case GpioV2PadCfgLockTxReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadCfgLockTx;
      break;
    case GpioV2PadHostSwOwnReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.HostOwn;
      break;
    case GpioV2GpiIsReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiIs;
      break;
    case GpioV2GpiIeReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiIe;
      break;
    case GpioV2GpiGpeStsReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiGpeSts;
      break;
    case GpioV2GpiGpeEnReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiGpeEn;
      break;
    case GpioV2SmiStsReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.SmiSts;
      break;
    case GpioV2SmiEnReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.SmiEn;
      break;
    case GpioV2NmiStsReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.NmiSts;
      break;
    case GpioV2NmiEnReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.NmiEn;
      break;
    case GpioV2Dw0Reg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.Dw0 + PadIndex * 0x8;
      break;
    case GpioV2Dw1Reg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.Dw0 + 0x04 + PadIndex * 0x8;
      break;
    case GpioV2MiscCfg:
      *RegisterOffset = GpioGetCommunities(CommunityIndex)->RegisterOffsets.MiscCfg;
      break;
    default:
      ASSERT(FALSE);
      return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  This procedure reads current ownership configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Ownership            Pointer to a buffer for ownership, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GetOwnership (
  IN  GPIOV2_PAD              GpioPad,
  OUT GPIOV2_PAD_OWN          *Ownership
  )
{
  if (IsHardGpio (GpioPad)) {
    *Ownership = GpioV2PadOwnHost;
    return EFI_SUCCESS;
  } else {
    return VirtualGpioGetOwnership (GpioPad, Ownership);
  }
}

/**
  This procedure will set GPIO mode

  @param[in] GpioPad              Gpio Pad. Please refer to GpioPinsYYY.h - where YYY name of the platform (eg. MTL, EBG, ...)
  @param[in] PadModeValue         GPIO pad mode value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetPadMode (
  IN GPIOV2_PAD              GpioPad,
  IN GPIOV2_PAD_MODE         PadMode
  )
{
  UINT32  AndValue;
  UINT32  OrValue;
  UINT32  CommunityIndex;
  UINT32  RegisterOffset;
  BOOLEAN PadValid;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;
  GPIOV2_PAD_OWN  Ownership;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetPadMode (GpioPad, PadMode);
  } else {
    IsPadValid (GpioPad, &PadValid);
    if (PadValid != TRUE) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }

    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    if (PadMode != GpioV2PadModeHardwareDefault) {
      AndValue = (UINT32)~(GPIOV2_PAD_MODE_MASK << GPIOV2_PAD_MODE_DW0_POS);
      OrValue  = ((PadMode >> 1) & GPIOV2_PAD_MODE_MASK) << GPIOV2_PAD_MODE_DW0_POS;
      GetRegisterOffset (
        GpioV2Dw0Reg,
        GpioPad,
        &RegisterOffset
      );

      P2sbPtr.Access.AndThenOr32 (
        &(P2sbPtr.Access),
        RegisterOffset,
        AndValue,
        OrValue
      );
    }
  }
  return EFI_SUCCESS;
}

/**
  This procedure reads current GPIO Pad Mode

  @param[in] GpioPad             GPIO Pad. Please refer to GpioPinsYYY.h - where YYY name of the platform (eg. MTL, EBG, ...)
  @param[in] PadMode             Pointer to a buffer for GPIO Pad Mode

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetPadMode (
  IN GPIOV2_PAD            GpioPad,
  IN GPIOV2_PAD_MODE       *PadMode
  )
{
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS   P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetPadMode (GpioPad, PadMode);
  } else {
    if (PadMode == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

      GetRegisterOffset (
        GpioV2Dw0Reg,
        GpioPad,
        &RegisterOffset
      );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    *PadMode = GPIO_ASSIGN_VALUE (((RegisterValue >> GPIOV2_PAD_MODE_DW0_POS) & GPIOV2_PAD_MODE_MASK));
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets host ownership configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Ownership            Host ownership, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
SetHostOwnership (
  IN GPIOV2_PAD               GpioPad,
  IN GPIOV2_HOSTSW_OWN        HostOnwership
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS   P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetHostOwnership (GpioPad, HostOnwership);
  } else {
    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    if (HostOnwership != GpioV2HostOwnDefault) {
      GetRegisterOffset (
        GpioV2PadHostSwOwnReg,
        GpioPad,
        &RegisterOffset
      );

      AndValue = (UINT32)~(GPIOV2_PAD_HOST_OWNERSHIP_MASK << PadIndex);
      OrValue  = ((HostOnwership >> 1) & GPIOV2_PAD_HOST_OWNERSHIP_MASK) << PadIndex;

      P2sbPtr.Access.AndThenOr32 (
        &(P2sbPtr.Access),
        RegisterOffset,
        AndValue,
        OrValue
      );
    }
  }
  return EFI_SUCCESS;
}

/**
  This procedure reads current host ownership configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Ownership            Pointer to a buffer for ownership, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetHostOwnership (
  IN  GPIOV2_PAD              GpioPad,
  OUT GPIOV2_HOSTSW_OWN       *HostOwnership
  )
{
  GPIOV2_PAD_OWN Ownership;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetHostOwnership (GpioPad, HostOwnership);
  } else {
    if (HostOwnership == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2PadHostSwOwnReg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    // Please refer to enum GPIOV2_HOSTSW_OWN in GpioV2Pad.h file
    *HostOwnership = (((RegisterValue >> PadIndex) & GPIOV2_PAD_HOST_OWNERSHIP_MASK) << 1) | 0x01;
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets Gpio Pad output state

  @param[in] GpioPad             GPIO pad
  @param[in] OutputState         GpioV2StateLow - output state low, GpioV2StateHigh - output state high

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetTx (
  IN GPIOV2_PAD             GpioPad,
  IN GPIOV2_PAD_STATE       OutputState
  )
{
  GPIOV2_PAD_OWN Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS  P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetTx (GpioPad, OutputState);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    if (OutputState != GpioV2StateDefault) {
      GetRegisterOffset (
        GpioV2Dw0Reg,
        GpioPad,
        &RegisterOffset
      );

      AndValue = (UINT32)~(GPIOV2_PAD_OUTPUT_STATE_MASK << GPIOV2_PAD_OUTPUT_STATE_DW0_POS);
      OrValue  = ((OutputState >> 1) & GPIOV2_PAD_OUTPUT_STATE_MASK) << GPIOV2_PAD_OUTPUT_STATE_DW0_POS;

      P2sbPtr.Access.AndThenOr32 (
        &(P2sbPtr.Access),
        RegisterOffset,
        AndValue,
        OrValue
      );
    }
  }
  return EFI_SUCCESS;
}

/**
  This procedure reads current Gpio Pad output state

  @param[in] GpioPad             GPIO pad
  @param[in] OutputState         Pointer to a buffer for output state

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetTx (
  IN GPIOV2_PAD             GpioPad,
  IN GPIOV2_PAD_STATE       *OutputState
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS  P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetTx (GpioPad, OutputState);
  } else {
    if (OutputState == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }
    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((RegisterValue >> GPIOV2_PAD_OUTPUT_STATE_DW0_POS) & (GPIOV2_PAD_OUTPUT_STATE_MASK)) {
      *OutputState = GpioV2StateHigh;
    } else {
      *OutputState = GpioV2StateLow;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads current Gpio Pad input state

  @param[in] GpioPad             GPIO pad
  @param[in] InputState          Pointer to a buffer for input state

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetRx (
  IN GPIOV2_PAD           GpioPad,
  IN GPIOV2_PAD_STATE     *InputState
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS  P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetRx (GpioPad, InputState);
  } else {
    if (InputState == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((RegisterValue >> GPIOV2_PAD_INPUT_STATE_DW0_POS) & (GPIOV2_PAD_INPUT_STATE_MASK)) {
      *InputState = GpioV2StateHigh;
    } else {
      *InputState = GpioV2StateLow;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads if TX buffer for requested Gpio Pad is enabled or disabled

  @param[in] GpioPad             GPIO pad
  @param[in] TxDisabled          Pointer to a buffer for enabled/disabled information

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetTxDisable (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  *TxDisabled
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS  P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetTxDisable (GpioPad, TxDisabled);
  } else {
    if (TxDisabled == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((RegisterValue >> GPIOV2_PAD_TX_DISABLE_DW0_POS) & (GPIOV2_PAD_TX_DISABLE_MASK)) {
      *TxDisabled = TRUE;
    } else {
      *TxDisabled = FALSE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets TX buffer for requested Gpio Pad as enabled or disabled

  @param[in] GpioPad             GPIO pad
  @param[in] TxDisabled          True - TX buffer disabled, False - TX buffer enabled

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetTxDisable (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  TxDisabled
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetTxDisable (GpioPad, TxDisabled);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(GPIOV2_PAD_TX_DISABLE_MASK << GPIOV2_PAD_TX_DISABLE_DW0_POS);
    OrValue  = (TxDisabled & GPIOV2_PAD_TX_DISABLE_MASK) << GPIOV2_PAD_TX_DISABLE_DW0_POS;

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }
  return EFI_SUCCESS;
}

/**
  This procedure reads if RX buffer for requested Gpio Pad is enabled or disabled

  @param[in] GpioPad             GPIO pad
  @param[in] RxDisabled          Pointer to a buffer for enabled/disabled information

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetRxDisable (
  IN GPIOV2_PAD              GpioPad,
  IN BOOLEAN                 *RxDisabled
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetRxDisable (GpioPad, RxDisabled);
  } else {
    if (RxDisabled == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }

    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }
    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((RegisterValue >> GPIOV2_PAD_RX_DISABLE_DW0_POS) & (GPIOV2_PAD_RX_DISABLE_MASK)) {
      *RxDisabled = TRUE;
    } else {
      *RxDisabled = FALSE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets Rx buffer for requested Gpio Pad as enabled or disabled

  @param[in] GpioPad             GPIO pad
  @param[in] TxDisabled          True - TX buffer disabled, False - TX buffer enabled

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetRxDisable (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  RxDisabled
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetRxDisable (GpioPad, RxDisabled);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(GPIOV2_PAD_RX_DISABLE_MASK << GPIOV2_PAD_RX_DISABLE_DW0_POS);
    OrValue  = (RxDisabled & GPIOV2_PAD_RX_DISABLE_MASK) << GPIOV2_PAD_RX_DISABLE_DW0_POS;

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  return EFI_SUCCESS;
}

/**
  This procedure will set GPIO enable or disable input inversion on rquested pad

  @param[in] GpioPad             GPIO pad
  @param[in] InputInversion      GpioV2InputInversionEnable or GpioV2InputInversionDisable, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetInputInversion (
  IN GPIOV2_PAD                  GpioPad,
  IN GPIOV2_PAD_INPUT_INVERSION  InputInversion
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetInputInversion (GpioPad, InputInversion);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }
    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    if (InputInversion != GpioV2InputInversionHardwareDefault) {

      AndValue = (UINT32)~(GPIOV2_PAD_INPUT_INVERSION_MASK << GPIOV2_PAD_INPUT_INVERSION_DW0_POS);
      OrValue  = ((InputInversion >> 1) & GPIOV2_PAD_INPUT_INVERSION_MASK) << GPIOV2_PAD_INPUT_INVERSION_DW0_POS;

      GetRegisterOffset (
        GpioV2Dw0Reg,
        GpioPad,
        &RegisterOffset
      );

      P2sbPtr.Access.AndThenOr32 (
        &(P2sbPtr.Access),
        RegisterOffset,
        AndValue,
        OrValue
      );
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure will Get GPIO enable or disable input inversion on rquested pad

  @param[in]  GpioPad             GPIO pad
  @param[Out] Inverted            Buffer for Boolean value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetInputInversion (
  IN GPIOV2_PAD                  GpioPad,
  OUT BOOLEAN                    *Inverted
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32  CommunityIndex;
  UINT32  RegisterOffset;
  BOOLEAN PadValid;
  UINT32  Data;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetInputInversion (GpioPad, Inverted);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    IsPadValid (GpioPad, &PadValid);
    if (PadValid != TRUE) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
        GpioV2Dw0Reg,
        GpioPad,
        &RegisterOffset
      );

    Data = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((Data >> GPIOV2_PAD_INPUT_INVERSION_DW0_POS) & GPIOV2_PAD_INPUT_INVERSION_MASK) {
      *Inverted = TRUE;
    }
    else {
      *Inverted = FALSE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure will set GPIO Lock (register PADCFGLOCK)

  @param[in] GpioPad             GPIO pad
  @param[in] Lock                GpioV2Unlock - Unlock pad, GpioV2Lock - Lock pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
SetLock (
  IN GPIOV2_PAD                GpioPad,
  IN GPIOV2_PAD_LOCK           Lock
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 PadIndex;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetLock (GpioPad, Lock);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

    if (Lock != GpioV2LockHardwareDefault) {
      AndValue = ~(GPIOV2_PAD_LOCK_MASK << PadIndex);
      OrValue  = ((Lock >> 1) & GPIOV2_PAD_LOCK_MASK) << PadIndex;

      GetRegisterOffset (
        GpioV2PadCfgLockReg,
        GpioPad,
        &RegisterOffset
      );

      P2sbPtr = GetP2sbAddress(CommunityIndex);

      P2sbPtr.Access.AndThenOr32 (
        &(P2sbPtr.Access),
        RegisterOffset,
        AndValue,
        OrValue
      );
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure will get GPIO Lock (register PADCFGLOCK)

  @param[in]  GpioPad             GPIO pad
  @param[out] Lock                Buffer for GPIOV2_PAD_LOCK

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetLock (
  IN  GPIOV2_PAD                   GpioPad,
  OUT GPIOV2_PAD_LOCK              *Lock
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 PadIndex;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetLock (GpioPad, Lock);
  } else {
    if (Lock == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2PadCfgLockReg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((RegisterValue >> PadIndex) & GPIOV2_PAD_LOCK_MASK) {
      *Lock = GpioV2Lock;
    } else {
      *Lock = GpioV2Unlock;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure will set GPIO Lock Tx (register PADCFGLOCKTX)

  @param[in] GpioPad             GPIO pad
  @param[in] LockTx              GpioV2Unlock - Unlock output state of Gpio Pad, GpioV2Lock - Lock output state of Gpio Pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
SetLockTx (
  IN GPIOV2_PAD                  GpioPad,
  IN GPIOV2_PAD_LOCK             LockTx
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 PadIndex;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetLockTx (GpioPad, LockTx);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

    P2sbPtr = GetP2sbAddress(CommunityIndex);

    if (LockTx != GpioV2LockHardwareDefault) {
      GetRegisterOffset (
        GpioV2PadCfgLockTxReg,
        GpioPad,
        &RegisterOffset
      );

      AndValue = ~(GPIOV2_PAD_LOCK_MASK << PadIndex);
      OrValue  = ((LockTx >> 1) & GPIOV2_PAD_LOCK_MASK) << PadIndex;

      P2sbPtr.Access.AndThenOr32 (
        &(P2sbPtr.Access),
        RegisterOffset,
        AndValue,
        OrValue
      );
    }
  }
  return EFI_SUCCESS;
}

/**
  This procedure will get GPIO Lock Tx (register PADCFGLOCKTX)

  @param[in] GpioPad             GPIO pad
  @param[in] Lock                Buffer for GPIOV2_PAD_LOCK

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetLockTx (
  IN GPIOV2_PAD                   GpioPad,
  IN GPIOV2_PAD_LOCK              *Lock
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           PadIndex;
  UINT32           CommunityIndex;
  UINT32           RegisterOffset;
  UINT32           RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetLockTx (GpioPad, Lock);
  } else {
    if (Lock == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

    P2sbPtr = GetP2sbAddress(CommunityIndex);

      GetRegisterOffset (
        GpioV2PadCfgLockTxReg,
        GpioPad,
        &RegisterOffset
      );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((RegisterValue >> PadIndex) & GPIOV2_PAD_LOCK_MASK) {
      *Lock = GpioV2Lock;
    } else {
      *Lock = GpioV2Unlock;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets NMI Enable for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] NmiEn                NMI Enable, TRUE or FALSE

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetNmiEn (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  NmiEn
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetNmiEn (GpioPad, NmiEn);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2NmiEnReg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(GPIOV2_PAD_NMI_EN_MASK << PadIndex);
    if (NmiEn) {
      OrValue = 0x01 << PadIndex;
    } else {
      OrValue = 0x0;
    }

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets SMI Enable configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] SmiEn                SMI Enable, TRUE or FALSE

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetSmiEn (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  SmiEn
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetSmiEn (GpioPad, SmiEn);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2SmiEnReg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(GPIOV2_PAD_SMI_EN_MASK << PadIndex);
    OrValue  = (SmiEn & GPIOV2_PAD_SMI_EN_MASK) << PadIndex;

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets GPI GPE Enable configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] GpiIe                GPI GPE Enable, TRUE or FALSE

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetGpiGpeEn (
  IN GPIOV2_PAD              GpioPad,
  IN BOOLEAN                 GpiGpeEn
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetGpiGpeEn (GpioPad, GpiGpeEn);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2GpiGpeEnReg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(GPIOV2_PAD_GPI_GPE_EN_MASK << PadIndex);
    OrValue  = (GpiGpeEn & GPIOV2_PAD_GPI_GPE_EN_MASK) << PadIndex;

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads current GPI GPE Enable configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] GpiIe                GPI GPE Enable, TRUE or FALSE

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetGpiGpeEn (
  IN GPIOV2_PAD              GpioPad,
  IN BOOLEAN                 *GpiGpeEn
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetGpiGpeEn (GpioPad, GpiGpeEn);
  } else {
    if (GpiGpeEn == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }

    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2GpiGpeEnReg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((RegisterValue >> PadIndex) & (GPIOV2_PAD_GPI_GPE_EN_MASK)) {
      *GpiGpeEn = TRUE;
    } else {
      *GpiGpeEn = FALSE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets Gpi Enable configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] GpiIe                GPI Enable, TRUE or FALSE

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetGpiIe (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  GpiIe
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS  P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetGpiIe (GpioPad, GpiIe);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2GpiIeReg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(GPIOV2_PAD_GPI_IE_MASK << PadIndex);
    OrValue  = (GpiIe & GPIOV2_PAD_GPI_IE_MASK) << PadIndex;

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }
  return EFI_SUCCESS;
}

/**
  This procedure reads current Gpi Enable configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] GpiIe                Pointer to a buffer for GPI Enable

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetGpiIe (
  IN GPIOV2_PAD              GpioPad,
  IN BOOLEAN                 *GpiIe
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS   P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetGpiIe (GpioPad, GpiIe);
  } else {
    if (GpiIe == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }

    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2GpiIeReg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((RegisterValue >> PadIndex) & (GPIOV2_PAD_GPI_IE_MASK)) {
      *GpiIe = TRUE;
    } else {
      *GpiIe = FALSE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads current Gpi Status for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] GpiIe                Pointer to a buffer for GPI Enable

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetGpiIs (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  *GpiIs
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (IsHardGpio (GpioPad)) {
    return VirtualGpioGetGpiIs (GpioPad, GpiIs);
  } else {
    if (GpiIs == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }

    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2GpiIsReg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    if ((RegisterValue >> PadIndex) & (GPIOV2_PAD_GPI_IE_MASK)) {
      *GpiIs = TRUE;
    } else {
      *GpiIs = FALSE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets GPIROUTNMI bit (17th bit in DW0) for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Enable               TRUE or FALSE, either enable or disable 17 bit in DW0

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetRouteNmi (
  IN GPIOV2_PAD           GpioPad,
  IN BOOLEAN              Enable
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetRouteNmi (GpioPad, Enable);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }
    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    AndValue = (UINT32)~(GPIOV2_PAD_GPIROUTNMI_DW0_MASK << GPIOV2_PAD_GPIROUTNMI_DW0_POS);
    if (Enable) {
      OrValue = 0x1 << GPIOV2_PAD_GPIROUTNMI_DW0_POS;
    } else {
      OrValue = 0x0;
    }

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets GPIROUTSMI bit (18th bit in DW0) for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Enable               TRUE or FALSE, either enable or disable 18 bit in DW0

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetRouteSmi (
  IN GPIOV2_PAD           GpioPad,
  IN BOOLEAN              Enable
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetRouteSmi (GpioPad, Enable);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    AndValue = (UINT32)~(GPIOV2_PAD_GPIROUTSMI_DW0_MASK << GPIOV2_PAD_GPIROUTSMI_DW0_POS);
    if (Enable) {
      OrValue = 0x1 << GPIOV2_PAD_GPIROUTSMI_DW0_POS;
    } else {
      OrValue = 0x0;
    }

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets GPIROUTIOXAPIC bit (20th bit in DW0) for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Enable               TRUE or FALSE, either enable or disable 20th bit in DW0

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetRouteIoxApic (
  IN GPIOV2_PAD           GpioPad,
  IN BOOLEAN              Enable
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetRouteIoxApic (GpioPad, Enable);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    AndValue = (UINT32)~(GPIOV2_PAD_GPIROUTIOXAPIC_DW0_MASK << GPIOV2_PAD_GPIROUTIOXAPIC_DW0_POS);
    if (Enable) {
      OrValue = 0x1 << GPIOV2_PAD_GPIROUTIOXAPIC_DW0_POS;
    } else {
      OrValue = 0x0;
    }

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets RxEv configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] RxEvCfg              RxEv configuration

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetRxEvCfg (
  IN GPIOV2_PAD           GpioPad,
  IN GPIOV2_RXEVCFG       RxEvCfg
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetRxEvCfg (GpioPad, RxEvCfg);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    if (RxEvCfg != GpioV2IntRxEvCfgDefault) {
      CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
      P2sbPtr = GetP2sbAddress(CommunityIndex);

      AndValue = (UINT32)~(GPIOV2_PAD_RXEV_MASK << GPIOV2_PAD_RXEV_DW0_POS);
      OrValue  = ((RxEvCfg >> 1) & GPIOV2_PAD_RXEV_MASK) << GPIOV2_PAD_RXEV_DW0_POS;

      GetRegisterOffset (
        GpioV2Dw0Reg,
        GpioPad,
        &RegisterOffset
      );

      P2sbPtr.Access.AndThenOr32 (
        &(P2sbPtr.Access),
        RegisterOffset,
        AndValue,
        OrValue
      );
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets GPIROUTSCI bit (19th bit in DW0) for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Enable               TRUE or FALSE, either enable or disable 19 bit in DW0

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetRouteSci (
  IN GPIOV2_PAD           GpioPad,
  IN BOOLEAN              Enable
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetRouteSci (GpioPad, Enable);
  } else {
    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    AndValue = (UINT32)~(GPIOV2_PAD_GPIROUTSCI_DW0_MASK << GPIOV2_PAD_GPIROUTSCI_DW0_POS);
    if (Enable) {
      OrValue = 0x1 << GPIOV2_PAD_GPIROUTSCI_DW0_POS;
    } else {
      OrValue = 0x0;
    }

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    P2sbPtr.Access.AndThenOr32 (
      &(P2sbPtr.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets Reset Configuration for requested Gpio Pad

  @param[in] GpioPad             GPIO pad
  @param[in] ResetConfig         Reset Configuration - please refer to GpioV2Pad.h (GPIOV2_RESET_CONFIG)

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
SetResetConfig (
  IN GPIOV2_PAD               GpioPad,
  IN GPIOV2_RESET_CONFIG      ResetConfig
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  UINT8  PadRstIndex;
  GPIOV2_RESET_CONFIG *PadRstCfgToGpioResetConfigMap;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetResetConfig (GpioPad, ResetConfig);
  } else {
    static GPIOV2_RESET_CONFIG  GppPadRstCfgToGpioResetConfigMap[] = {
      GpioV2ResetResume,
      GpioV2ResetHostDeep,
      GpioV2ResetHost,
      GpioV2ResetGlobal
    };
    static GPIOV2_RESET_CONFIG  GpdPadRstCfgToGpioResetConfigMap[] = {
      GpioV2DswReset,
      GpioV2ResetHostDeep,
      GpioV2ResetHost,
      GpioV2ResetResume
    };

    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    if (ResetConfig != GpioV2ResetDefault) {
      GetRegisterOffset (
        GpioV2Dw0Reg,
        GpioPad,
        &RegisterOffset
      );
      if (GpioGetCommunities(CommunityIndex)->IsComDsw) {
        PadRstCfgToGpioResetConfigMap = GpdPadRstCfgToGpioResetConfigMap;
      } else {
        PadRstCfgToGpioResetConfigMap = GppPadRstCfgToGpioResetConfigMap;
      }
      for (PadRstIndex = 0; PadRstIndex <= (GPIOV2_PAD_RESET_CONFIG_MASK + 1); PadRstIndex++) {
        if (PadRstIndex == (GPIOV2_PAD_RESET_CONFIG_MASK + 1)) {
          DEBUG ((DEBUG_ERROR, "[%a] ResetConfig = %d has not been found in mapping table!\n", __FUNCTION__, ResetConfig));
          return EFI_NOT_FOUND;
        } else if (PadRstCfgToGpioResetConfigMap[PadRstIndex] == ResetConfig) {
          break;
        }
      }


      AndValue = (UINT32)~(GPIOV2_PAD_RESET_CONFIG_MASK << GPIOV2_PAD_RESET_CONFIG_DW0_POS);
      OrValue  = (PadRstIndex & GPIOV2_PAD_RESET_CONFIG_MASK) << GPIOV2_PAD_RESET_CONFIG_DW0_POS;

      P2sbPtr.Access.AndThenOr32 (
        &(P2sbPtr.Access),
        RegisterOffset,
        AndValue,
        OrValue
      );
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads current Reset Configuration for requested Gpio Pad

  @param[in] GpioPad             GPIO pad
  @param[in] ResetConfig         Pointer to a buffer for Reset Configuration

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
GetResetConfig (
  IN GPIOV2_PAD               GpioPad,
  IN GPIOV2_RESET_CONFIG      *ResetConfig
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  UINT32 RegisterValue;
  UINT8  Padrstcfg;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioGetResetConfig (GpioPad, ResetConfig);
  } else {

    static GPIOV2_RESET_CONFIG  GppPadRstCfgToGpioResetConfigMap[] = {
      GpioV2ResetResume,
      GpioV2ResetHostDeep,
      GpioV2ResetHost,
      GpioV2ResetGlobal
    };
    static GPIOV2_RESET_CONFIG  GpdPadRstCfgToGpioResetConfigMap[] = {
      GpioV2DswReset,
      GpioV2ResetHostDeep,
      GpioV2ResetHost,
      GpioV2ResetResume
    };

    if (ResetConfig == NULL) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }

    GetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
    P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    RegisterValue = P2sbPtr.Access.Read32 (
      &(P2sbPtr.Access),
      RegisterOffset
    );

    // Please refer to enum GPIOV2_RESET_CONFIG in GpioV2Pad.h file
    Padrstcfg = (RegisterValue >> GPIOV2_PAD_RESET_CONFIG_DW0_POS) & GPIOV2_PAD_RESET_CONFIG_MASK;
    if (GpioGetCommunities(CommunityIndex)->IsComDsw) {
      *ResetConfig = GpdPadRstCfgToGpioResetConfigMap[Padrstcfg];
    } else {
      *ResetConfig = GppPadRstCfgToGpioResetConfigMap[Padrstcfg];
    }
  }

  return EFI_SUCCESS;
}

/**
  This procedure sets termination configuration for requested Gpio Pad

  @param[in] GpioPad             GPIO pad
  @param[in] TerminationConfig   Termination configuration, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
  @retval EFI_ACCESS_DENIED       Pad is not owned by Host.
**/
EFI_STATUS
EFIAPI
SetTerminationConfig (
  IN GPIOV2_PAD                 GpioPad,
  IN GPIOV2_TERMINATION_CONFIG  TerminationConfig
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 RegisterOffset;
  UINT32 CommunityIndex;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  if (!IsHardGpio (GpioPad)) {
    return VirtualGpioSetTerminationConfig (GpioPad, TerminationConfig);
  } else {
      GetOwnership (GpioPad, &Ownership);
      if (Ownership != GpioV2PadOwnHost) {
        return EFI_ACCESS_DENIED;
      }

      if (TerminationConfig != GpioV2TermDefault) {
        GetRegisterOffset (
          GpioV2Dw1Reg,
          GpioPad,
          &RegisterOffset
        );

        CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
        P2sbPtr = GetP2sbAddress(CommunityIndex);

        AndValue = (UINT32)~(GPIOV2_PAD_TERMINATION_CONFIG_MASK << GPIOV2_PAD_TERMINATION_CONFIG_DW1_POS);
        OrValue  = ((TerminationConfig >> 1) & GPIOV2_PAD_TERMINATION_CONFIG_MASK) << GPIOV2_PAD_TERMINATION_CONFIG_DW1_POS;

        P2sbPtr.Access.AndThenOr32 (
          &(P2sbPtr.Access),
          RegisterOffset,
          AndValue,
          OrValue
        );
      }
  }

  return EFI_SUCCESS;
}

/**
  This function retreives Global Group Index from GPIOV2_PAD_GROUP.
  Global Group Index is used in ASL code in ACPI interface.
  Please refer to OneSiliconPkg\Fru\XXX\Include\AcpiTables\Dsdt\GpioGroupsXXX.asl file.

  @param[in]  GpioGroup           Gpio Group
  @param[out] GlobalGroupIndex    buffer for Global Group Index

  @retval EFI_SUCCESS                    The function completed successfully
  @retval EFI_INVALID_PARAMETER          Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GetGlobalGroupIndex (
  IN   GPIOV2_PAD_GROUP            GpioGroup,
  OUT  UINT32                      *GlobalGroupIndex
  )
{
  UINT32             CommunityIndex;
  UINT32             CommunityIndexMax;
  BOOLEAN            IsValid;

  if (GlobalGroupIndex == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  IsGroupValid (GpioGroup, &IsValid);
  if (IsValid != TRUE) {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }

  CommunityIndexMax = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioGroup);

  *GlobalGroupIndex = GPIOV2_PAD_GET_GROUP_INDEX (GpioGroup);
  for (CommunityIndex = 0; CommunityIndex < CommunityIndexMax; CommunityIndex++) {
    *GlobalGroupIndex += GpioGetCommunities(CommunityIndex)->GroupsNum;
  }

  return EFI_SUCCESS;
}

/**
  Store unlock data.

  @param[in] GpioPad        GPIO pad
  @param[in] LockCfg        Pad config lock policy
  @param[in] LockTx         Pad Tx lock policy
**/
VOID
GpioV2StoreUnlockData (
  IN GPIOV2_PAD        GpioPad,
  IN GPIOV2_PAD_LOCK   LockCfg,
  IN GPIOV2_PAD_LOCK   LockTx
  )
{
  UINT32 Index;
  UINT32 ComIndex;
  UINT32 GrpIndex;
  UINT32  GlobalGroupIndex;
  CHAR8   PadName[GPIOV2_NAME_LENGTH_MAX];

  ComIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  GrpIndex = GPIOV2_PAD_GET_GROUP_INDEX (GpioPad);

  GlobalGroupIndex = GrpIndex;
  for (Index = 0; Index < ComIndex; Index++) {
    GlobalGroupIndex += 1;
  }

  GetPadName (GpioPad, sizeof (PadName), PadName);
  DEBUG ((DEBUG_VERBOSE, "[GPIOV2] [UNLOCK] [SELECT] %a ", PadName));
  if (LockCfg == GpioV2Unlock) {
    DEBUG ((DEBUG_VERBOSE, "CFG"));
  }

  if (LockTx == GpioV2Unlock) {
    DEBUG ((DEBUG_VERBOSE, " TX"));
  }
  DEBUG ((DEBUG_VERBOSE, "\n"));
}

/**
  This procedure will set GPIO mode for HDA SNDW functionality

  @param[in] GpioPad              Gpio Pad
  @param[in] PadConfig            Gpio Pad configuration - please refer to GPIOV2_CONFIG in GpioV2Config.h

  @retval EFI_SUCCESS              The function completed successfully
  @retval EFI_INVALID_PARAMETER    Invalid group or pad number
**/
EFI_STATUS
EFIAPI
ConfigurePad (
  IN GPIOV2_PAD              GpioPad,
  IN GPIOV2_CONFIG           *PadConfig
  )
{
  GPIOV2_PAD_OWN   Ownership;
  EFI_STATUS       Status = EFI_SUCCESS;
  CHAR8            PadName[GPIOV2_NAME_LENGTH_MAX];
  BOOLEAN          PadValid;

  IsPadValid(GpioPad, &PadValid);

  if (!PadValid) {
    DEBUG ((DEBUG_WARN, "[GPIOV2] %a PadIsNot Valid\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  GetOwnership(GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  //
  // First, set proper CS# before programming Pad.
  //
  if (!IsHardGpio (GpioPad)) {
    Status = VirtualGpioSetCs (GpioPad, PadConfig);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  GetPadName (GpioPad, sizeof (PadName), PadName);
  SetPadMode (GpioPad, PadConfig->PadMode);

  if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntSmi >> 1)) {
    SetHostOwnership (GpioPad, GpioV2HostOwnGpio);
  } else {
    SetHostOwnership (GpioPad, PadConfig->HostOwn);
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

  if (PadConfig->Direction != GpioV2DirDefault) {
    switch (PadConfig->Direction) {
      case GpioV2DirInOut:
        SetTxDisable (GpioPad, FALSE);
        SetRxDisable (GpioPad, FALSE);
        SetInputInversion (GpioPad, GpioV2InputInversionDisable);
        break;
      case GpioV2DirInInvOut:
        SetTxDisable (GpioPad, FALSE);
        SetRxDisable (GpioPad, FALSE);
        SetInputInversion (GpioPad, GpioV2InputInversionEnable);
        break;
      case GpioV2DirIn:
        SetTxDisable (GpioPad, TRUE);
        SetRxDisable (GpioPad, FALSE);
        SetInputInversion (GpioPad, GpioV2InputInversionDisable);
        break;
      case GpioV2DirInInv:
        SetTxDisable (GpioPad, TRUE);
        SetRxDisable (GpioPad, FALSE);
        SetInputInversion (GpioPad, GpioV2InputInversionEnable);
        break;
      case GpioV2DirOut:
        SetTxDisable (GpioPad, FALSE);
        SetRxDisable ( GpioPad, TRUE);
        SetInputInversion (GpioPad, GpioV2InputInversionDisable);
        break;
      case GpioV2DirNone:
        SetTxDisable (GpioPad, TRUE);
        SetRxDisable (GpioPad, TRUE);
        SetInputInversion (GpioPad, GpioV2InputInversionDisable);
        break;
      case GpioV2DirDefault:
      default:
        break;
    }
  }
  if (PadConfig->InterruptConfig != GpioV2IntDefault) {

    //
    // All Interrupt/wake events turned off by default
    //
    SetNmiEn (GpioPad, FALSE);
    SetSmiEn (GpioPad, FALSE);
    SetGpiGpeEn (GpioPad, FALSE);
    SetGpiIe (GpioPad, FALSE);

    if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntNmi >> 1)) {
      SetNmiEn (GpioPad, TRUE);
      SetRouteNmi (GpioPad, TRUE);
    }

    if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntSmi >> 1)) {
      SetSmiEn (GpioPad, TRUE);
      SetRouteSmi (GpioPad, TRUE);
    }

    if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntSci >> 1)) {
      SetGpiGpeEn (GpioPad, TRUE);
      SetRouteSci (GpioPad, TRUE);
    }

    if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntApic >> 1)) {
      SetGpiIe (GpioPad, TRUE);
      SetRouteIoxApic (GpioPad, TRUE);
    }

    if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntLevel >> 1)) {
      SetRxEvCfg (GpioPad, GpioV2IntRxEvCfgLevel);

    } else if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntEdge >> 1)) {
      SetRxEvCfg (GpioPad, GpioV2IntRxEvCfgEdge);

    } else if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntLvlEdgDis >> 1)) {
      SetRxEvCfg (GpioPad, GpioV2IntRxEvCfgDisable);

    } else if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntBothEdge >> 1)) {
      SetRxEvCfg (GpioPad, GpioV2IntRxEvCfgLevelEdge);
    }
  }
  SetResetConfig (GpioPad, PadConfig->ResetConfig);

  SetTerminationConfig (GpioPad, PadConfig->TerminationConfig);
  //
  // Store unlock data
  //
  if ( ((PadConfig->Direction == GpioV2DirOut) || (PadConfig->Direction == GpioV2DirInOut) ||  (PadConfig->Direction == GpioV2DirInInvOut)) &&
       (PadConfig->PadMode == GpioV2PadModeGpio) &&
       (PadConfig->LockTx == GpioV2LockHardwareDefault) ) {
    GpioV2StoreUnlockData (GpioPad,
                           PadConfig->LockConfig,
                           GpioV2Unlock );
  } else if ( (PadConfig->LockConfig == GpioV2Unlock) ||
              (PadConfig->LockTx == GpioV2Unlock) ) {
    GpioV2StoreUnlockData (GpioPad,
                          PadConfig->LockConfig,
                          PadConfig->LockTx );
  }

  if (PadConfig->OtherSettings != GpioV2RxRaw1Default) {
    // TO BE DONE
  }

  return Status;
}

/**
  This procedure will configure all GPIO pads based on GpioPadsConfigTable

  @param[in] GpioPadsConfigTable     Pointer to PadInitConfigTable
  @param[in] GpioPadsConfigTableSize Size of PadInitConfigTable

  @retval Status
**/
EFI_STATUS
EFIAPI
ConfigurePads (
  IN GPIOV2_INIT_CONFIG       *GpioPadsConfigTable,
  IN UINT32                   GpioPadsConfigTableSize
  )
{
  UINT32       Index;
  EFI_STATUS   Status;
  Status = EFI_SUCCESS;

  for (Index = 0; Index < GpioPadsConfigTableSize; Index++) {
    Status = ConfigurePad (
               GpioPadsConfigTable[Index].GpioPad,
               &GpioPadsConfigTable[Index].GpioConfig
               );
    if (Status != EFI_SUCCESS) {
      ASSERT(FALSE);
      break;
    }
  }

  return Status;
}

/**
  This procedure will initialize configuration for all GPIO based on GpioPadsConfigTable

  @param[in] GpioPadsConfigTable     Pointer to PadInitConfigTable
  @param[in] GpioPadsConfigTableSize Size of PadInitConfigTable

  @retval Status
**/
EFI_STATUS
GpioV2ConfigurePads (
  IN GPIOV2_INIT_CONFIG       *GpioPadsConfigTable,
  IN UINT32                   GpioPadsConfigTableSize
  )
{
  EFI_STATUS                Status;

  if ((GpioPadsConfigTable == NULL) || (GpioPadsConfigTableSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // All pads in PadInitConfigTable should belong to the same ChipsetId.
  //
  Status = GetGpioV2ServicesFromPad (GpioPadsConfigTable[0].GpioPad);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return ConfigurePads (GpioPadsConfigTable, GpioPadsConfigTableSize);

}

/**
  Retreive PadInfo embedded inside DW of GPIO CFG DATA.
  Prepare a PadInfo DWORD first, add into the GpioTable,
  followed by DW0 and DW1 directly from GPIO CFG DATA.
  This format of GpioTable is what the Gpio library expects.

  @param    GpioTable   Pointer to the GpioTable to be updated
  @param    GpioCfgHdr  Pointer to the cfg data header
  @param    Offset      Index of a particulr pin's DW0, DW1 in GpioCfg

  @retval   GpioTable   Pointer to fill the next gpio item
**/
STATIC
UINT8 *
FillGpioTable (
  IN  UINT8          *GpioTable,
  IN  ARRAY_CFG_HDR  *GpioCfgHdr,
  IN  UINT32          Offset

)
{
  GPIO_PAD           *GpioPad;
  UINT8              *GpioData;
  UINT8              *GpioCfg;
  // UINT8              *print;

  GpioData  = ((UINT8 *)GpioCfgHdr) + GpioCfgHdr->HeaderSize + Offset;
  GpioCfg   = GpioTable + sizeof(GPIO_PAD);
  GpioPad   = (GPIO_PAD *) GpioTable;
  DEBUG((DEBUG_INFO, "Inside filltable, GpioCfgHdr->ItemSize: %d\n", GpioCfgHdr->ItemSize));
  CopyMem (GpioCfg, GpioData, GpioCfgHdr->ItemSize);

  //
  // Get the DW and extract PadInfo
  //
  GpioGetGpioPadFromCfgDw ((UINT32 *)GpioCfg, GpioPad);

  GpioTable += (sizeof(GPIO_PAD) + GpioCfgHdr->ItemSize);

  return GpioTable;
}

/**
  Print the output of the GPIO Config table that was read from CfgData.

  @param GpioPinNum           Number of GPIO entries in the table.

  @param GpioConfData         GPIO Config Data that was read from the Configuration region either from internal or external source.

**/
VOID
PrintGpioV2ConfigTable (
  IN UINT32              GpioPinNum,
  IN VOID*               GpioConfData
)
{
  GPIOV2_INIT_CONFIG  *GpioInitConf;
  UINT32            *PadDataPtr;
  UINT32             Index;

  GpioInitConf = (GPIOV2_INIT_CONFIG *)GpioConfData;
  for (Index  = 0; Index < GpioPinNum; Index++) {
    PadDataPtr = (UINT32 *)&GpioInitConf->GpioConfig;
    DEBUG ((DEBUG_INFO, "GPIO PAD: 0x%08X   DATA: 0x%08X 0x%08X\n", GpioInitConf->GpioPad, PadDataPtr[0], PadDataPtr[1]));
    GpioInitConf++;
  }
}

/**
  Configure the GPIO pins, available as part of platform specific GPIO CFG DATA.
  If the pins are not part of GPIO CFG DATA, call GpioConfigurePads() directly
  with the appropriate arguments.

  @param    Tag         Tag ID of the Gpio Cfg data item
  @param    Entries     Number of entries in Gpio Table
  @param    DataBuffer  Pointer to the Gpio Table to be programmed

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_NOT_FOUND                 If Gpio Config Data cant be found
**/
EFI_STATUS
EFIAPI
ConfigureGpioV2 (
  IN  UINT16  Tag,
  IN GPIOV2_INIT_CONFIG                 *DataBuffer,
  IN UINT16                             GpioTableCount
  )
{
  ARRAY_CFG_HDR  *GpioCfgCurrHdr;
  ARRAY_CFG_HDR  *GpioCfgBaseHdr;
  ARRAY_CFG_HDR  *GpioCfgHdr;
  UINT32         GpioEntries;
  UINT32         Index;
  UINT32         Offset;
  UINT8          *GpioCfgDataBuffer;
  UINT8          *GpioTable;
  EFI_STATUS     Status;

  //
  // If no Tag provided, check for GpioTable info;
  // If    Tag provided, GpioTable params are don't care
  //
  if (Tag == CDATA_NO_TAG) {
    Status = GpioV2ConfigurePads (DataBuffer, GpioTableCount);
    ASSERT_EFI_ERROR (Status);
    return EFI_SUCCESS;
  }

  //
  // Find the GPIO CFG HDR
  //
  GpioCfgCurrHdr = (ARRAY_CFG_HDR *)FindConfigDataByTag (Tag);
  if (GpioCfgCurrHdr == NULL) {
    return EFI_NOT_FOUND;
  }

  GpioEntries    = 0;
  GpioCfgBaseHdr = NULL;

  //
  // Find the GPIO CFG Data based on Platform ID
  // GpioTableData is the start of the GPIO entries
  //
  if (GpioCfgCurrHdr->BaseTableId < 16) {
    GpioCfgBaseHdr = (ARRAY_CFG_HDR *)FindConfigDataByPidTag (GpioCfgCurrHdr->BaseTableId, Tag);
    if (GpioCfgBaseHdr == NULL) {
      DEBUG ((GPIO_DEBUG_ERROR, "Cannot find base GPIO table for platform ID %d\n", GpioCfgCurrHdr->BaseTableId));
      return EFI_NOT_FOUND;
    }
    if (GpioCfgCurrHdr->ItemSize != GpioCfgBaseHdr->ItemSize) {
      DEBUG ((GPIO_DEBUG_ERROR, "Inconsistent GPIO item size\n"));
      return EFI_LOAD_ERROR;
    }
    GpioCfgHdr = GpioCfgBaseHdr;
  }
  else {
    GpioCfgHdr = GpioCfgCurrHdr;
  }

  Offset     = 0;
  GpioTable  = (UINT8 *)AllocateTemporaryMemory (0);  //allocate new buffer
  if (GpioTable == NULL) {
    DEBUG ((GPIO_DEBUG_ERROR, "Cannot allocate buffer for GpioTable\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  GpioCfgDataBuffer = GpioTable;
  for (Index = 0; Index  < GpioCfgHdr->ItemCount; Index++) {
    if (GpioCfgCurrHdr->BaseTableBitMask[Index >> 3] & (1 << (Index & 7))) {
      GpioTable = FillGpioTable (GpioTable, GpioCfgHdr, Offset);
      GpioEntries++;
    }
    Offset += GpioCfgHdr->ItemSize;
  }

  Offset = 0;
  if (GpioCfgBaseHdr != NULL) {
    for (Index = 0; Index  < GpioCfgCurrHdr->ItemCount; Index++) {
      GpioTable = FillGpioTable (GpioTable, GpioCfgCurrHdr, Offset);
      GpioEntries++;
      Offset += GpioCfgCurrHdr->ItemSize;
    }
  }

  DEBUG_CODE_BEGIN ();
  PrintGpioV2ConfigTable (GpioEntries, GpioCfgDataBuffer);
  DEBUG_CODE_END ();

  GpioV2ConfigurePads ((GPIOV2_INIT_CONFIG *)GpioCfgDataBuffer, GpioEntries);

  DEBUG ((GPIO_DEBUG_INFO, "GpioInit(0x%p:%d) Done\n", GpioCfgDataBuffer, GpioEntries));
  return EFI_SUCCESS;
}
