/** @file
  This file contains routines for GPIO

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <GpioConfig.h>
#include <Library/GpioV2Lib.h>
#include <Library/GpioSiLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/ConfigDataLib.h>
#include <Include/GpioV2Config.h>
#include <Library/PrintLib.h>
#include <Base.h>
#include <Library/BaseLib.h>
#if FixedPcdGet8(PcdMtlSSupport) == 1
  #include <GpioV2PinsMtlPchS.h>
  #include <Library/MtlPchGpioTopologyLib.h>
#else
  #include <GpioV2PinsMtlSoc.h>
  #include <Library/MtlSocGpioTopologyLib.h>
#endif

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
  DEBUG((DEBUG_INFO, "ChipsetId:0x%x\n", ChipsetId));
  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioGroup);
  GroupIndex = GPIOV2_PAD_GET_GROUP_INDEX (GpioGroup);

  *IsValid = FALSE;
#if FixedPcdGet8(PcdMtlSSupport) == 1
  if (ChipsetId != GPIOV2_MTL_PCH_S_CHIPSET_ID) {
    DEBUG((DEBUG_ERROR, "[GPIOV2][IsGroupValid] Error: wrong ChipsetId; com:%d,grp:%d id: 0x%x (exp: 0x%x)\n", CommunityIndex, GroupIndex, ChipsetId, GPIOV2_MTL_PCH_S_CHIPSET_ID));
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }
  if (CommunityIndex >= MtlPchGpioGetCommunitiesNum(0)) {
    DEBUG((DEBUG_ERROR, "[GPIOV2][IsGroupValid] Error: wrong community index; com:%d (max: %d)\n", CommunityIndex, MtlPchGpioGetCommunitiesNum(0)));
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }

  if (GroupIndex >= MtlPchGpioGetCommunities(CommunityIndex)->GroupsNum) {
    DEBUG((DEBUG_ERROR, "[GPIOV2][IsGroupValid] Error: wrong group index; g:%d (max: %d)\n", GroupIndex, MtlPchGpioGetCommunities(CommunityIndex)->GroupsNum));
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }
#else
  if (ChipsetId != GPIOV2_MTL_SOC_M_CHIPSET_ID) {
    DEBUG((DEBUG_ERROR, "[GPIOV2][IsGroupValid] Error: wrong ChipsetId; com:%d,grp:%d id: 0x%x (exp: 0x%x)\n", CommunityIndex, GroupIndex, ChipsetId, GPIOV2_MTL_SOC_M_CHIPSET_ID));
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }

  if (CommunityIndex >= MtlSocGpioGetCommunitiesNum(0)) {
    DEBUG((DEBUG_ERROR, "[GPIOV2][IsGroupValid] Error: wrong community index; com:%d (max: %d)\n", CommunityIndex, MtlSocGpioGetCommunitiesNum(0)));
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }

  if (GroupIndex >= MtlSocGpioGetCommunities(CommunityIndex)->GroupsNum) {
    DEBUG((DEBUG_ERROR, "[GPIOV2][IsGroupValid] Error: wrong group index; g:%d (max: %d)\n", GroupIndex, MtlSocGpioGetCommunities(CommunityIndex)->GroupsNum));
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }
#endif
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

#if FixedPcdGet8(PcdMtlSSupport) == 1
  if (PadIndex >= MtlPchGpioGetGroups(CommunityIndex,GroupIndex).PadsNum) {
    DEBUG((DEBUG_INFO, "[GPIOV2][IsPadValid] Error: Pad 0x%x, wrong pad index\n", GpioPad));
    *IsValid = FALSE;
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }
#else
  if (PadIndex >= MtlSocGpioGetGroups(CommunityIndex,GroupIndex).PadsNum) {
    DEBUG((DEBUG_INFO, "[GPIOV2][IsPadValid] Error: Pad 0x%x, wrong pad index\n", GpioPad));
    *IsValid = FALSE;
    ASSERT(FALSE);
    return EFI_SUCCESS;
  }
#endif
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
    DEBUG((DEBUG_INFO, "No muxing\n"));
    return EFI_SUCCESS;
  }

  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  GroupIndex = GPIOV2_PAD_GET_GROUP_INDEX (GpioPad);
  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

#if FixedPcdGet8(PcdMtlSSupport) == 1
  if (AsciiSPrint (NameBuffer, NameBufferLength, "%a_%02d",
          MtlPchGpioGetGroups(CommunityIndex,GroupIndex).Name,
          PadIndex) == 0) {
            ASSERT(FALSE);
            return EFI_INVALID_PARAMETER;
          }
#else
  if (AsciiSPrint (NameBuffer, NameBufferLength, "%a_%02d",
          MtlSocGpioGetGroups(CommunityIndex,GroupIndex).Name,
          PadIndex) == 0) {
            ASSERT(FALSE);
            return EFI_INVALID_PARAMETER;
          }
#endif
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
#if FixedPcdGet8(PcdMtlSSupport) == 1
    case GpioV2PadOwnReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadOwn + (PadIndex / 8) * 0x04;
      break;
    case GpioV2PadCfgLockReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadCfgLock;
      break;
    case GpioV2PadCfgLockTxReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadCfgLockTx;
      break;
    case GpioV2PadHostOwnReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.HostOwn;
      break;
    case GpioV2GpiIsReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiIs;
      break;
    case GpioV2GpiIeReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiIe;
      break;
    case GpioV2GpiGpeStsReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiGpeSts;
      break;
    case GpioV2GpiGpeEnReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiGpeEn;
      break;
    case GpioV2SmiStsReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.SmiSts;
      break;
    case GpioV2SmiEnReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.SmiEn;
      break;
    case GpioV2NmiStsReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.NmiSts;
      break;
    case GpioV2NmiEnReg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.NmiEn;
      break;
    case GpioV2Dw0Reg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.Dw0 + PadIndex * 0x10;
      break;
    case GpioV2Dw1Reg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.Dw0 + 0x04 + PadIndex * 0x10;
      break;
    case GpioV2Dw2Reg:
      *RegisterOffset = MtlPchGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.Dw0 + 0x08 + PadIndex * 0x10;
      break;
    case GpioV2MiscCfg:
      *RegisterOffset = MtlPchGpioGetCommunities(CommunityIndex)->RegisterOffsets.MiscCfg;
      break;
    case GpioV2AcReg:
      *RegisterOffset = MtlPchGpioGetCommunities(CommunityIndex)->RegisterOffsets.AcSaiGrup0RcpDw0;
      break;
#else
    case GpioV2PadOwnReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadOwn + (PadIndex / 8) * 0x04;
      break;
    case GpioV2PadCfgLockReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadCfgLock;
      break;
    case GpioV2PadCfgLockTxReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadCfgLockTx;
      break;
    case GpioV2PadHostOwnReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.HostOwn;
      break;
    case GpioV2GpiIsReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiIs;
      break;
    case GpioV2GpiIeReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiIe;
      break;
    case GpioV2GpiGpeStsReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiGpeSts;
      break;
    case GpioV2GpiGpeEnReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiGpeEn;
      break;
    case GpioV2SmiStsReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.SmiSts;
      break;
    case GpioV2SmiEnReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.SmiEn;
      break;
    case GpioV2NmiStsReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.NmiSts;
      break;
    case GpioV2NmiEnReg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.NmiEn;
      break;
    case GpioV2Dw0Reg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.Dw0 + PadIndex * 0x10;
      break;
    case GpioV2Dw1Reg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.Dw0 + 0x04 + PadIndex * 0x10;
      break;
    case GpioV2Dw2Reg:
      *RegisterOffset = MtlSocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.Dw0 + 0x08 + PadIndex * 0x10;
      break;
    case GpioV2MiscCfg:
      *RegisterOffset = MtlSocGpioGetCommunities(CommunityIndex)->RegisterOffsets.MiscCfg;
      break;
    case GpioV2AcReg:
      *RegisterOffset = MtlSocGpioGetCommunities(CommunityIndex)->RegisterOffsets.AcSaiGrup0RcpDw0;
      break;
#endif
    default:
      DEBUG ((GPIO_DEBUG_ERROR, "Wrong Register Offset\n", (UINTN)PcdGet32(PcdP2sb0Ptr)));
      ASSERT(FALSE);
      return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

P2SB_SIDEBAND_REGISTER_ACCESS
GetP2sbAddress (
 IN UINT32  CommunityIndex
)
{
  if (CommunityIndex == 0) {
    return *((P2SB_SIDEBAND_REGISTER_ACCESS*)(UINTN)PcdGet32(PcdP2sb0Ptr));
  } else if (CommunityIndex == 1) {
    return *((P2SB_SIDEBAND_REGISTER_ACCESS*)(UINTN)PcdGet32(PcdP2sb1Ptr));
  } else if (CommunityIndex == 2) {
    return *((P2SB_SIDEBAND_REGISTER_ACCESS*)(UINTN)PcdGet32(PcdP2sb2Ptr));
  } else if (CommunityIndex == 3) {
    return *((P2SB_SIDEBAND_REGISTER_ACCESS*)(UINTN)PcdGet32(PcdP2sb3Ptr));
  } else if (CommunityIndex == 4) {
    return *((P2SB_SIDEBAND_REGISTER_ACCESS*)(UINTN)PcdGet32(PcdP2sb4Ptr));
  } else if (CommunityIndex == 5) {
    return *((P2SB_SIDEBAND_REGISTER_ACCESS*)(UINTN)PcdGet32(PcdP2sb5Ptr));
  }
  else {
    DEBUG ((DEBUG_INFO, "Wrong Community Index, asserting to index 0 as a default\n"));
    return *((P2SB_SIDEBAND_REGISTER_ACCESS*)(UINTN)PcdGet32(PcdP2sb0Ptr));
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
  P2SB_SIDEBAND_REGISTER_ACCESS   P2sbPtr;

  IsPadValid (GpioPad, &PadValid);
  if (PadValid != TRUE) {
    DEBUG ((DEBUG_INFO, "PadValid != TRUE\n"));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  DEBUG ((DEBUG_INFO, "GpioPad: 0x%x\n", GpioPad));
  DEBUG ((DEBUG_INFO, "PadMode: 0x%x\n", PadMode));
  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  DEBUG ((DEBUG_INFO, "CommunityIndex: 0x%x\n", CommunityIndex));

  P2sbPtr = GetP2sbAddress(CommunityIndex);

  if (PadMode != GpioV2PadModeHardwareDefault) {

    AndValue = (UINT32)~(GPIOV2_PAD_MODE_MASK << GPIOV2_PAD_MODE_DW0_POS);
    DEBUG ((DEBUG_INFO, "AndValue: 0x%x\n", AndValue));
    OrValue  = ((PadMode >> 1) & GPIOV2_PAD_MODE_MASK) << GPIOV2_PAD_MODE_DW0_POS;
    DEBUG ((DEBUG_INFO, "OrValue: 0x%x\n", OrValue));

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    DEBUG ((DEBUG_INFO, "RegisterOffset: 0x%x\n", RegisterOffset));
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
  This procedure sets host ownership configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Ownership            Host ownership, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetHostOwnership (
  IN GPIOV2_PAD               GpioPad,
  IN GPIOV2_HOSTSW_OWN        HostOnwership
  )
{
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  P2sbPtr = GetP2sbAddress(CommunityIndex);

  if (HostOnwership != GpioV2HostOwnDefault) {
    GetRegisterOffset (
      GpioV2PadHostOwnReg,
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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS  P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 PadIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS  P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

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

  return EFI_SUCCESS;
}

/**
  This procedure sets Reset Configuration for requested Gpio Pad

  @param[in] GpioPad             GPIO pad
  @param[in] ResetConfig         Reset Configuration - please refer to GpioV2Pad.h (GPIOV2_RESET_CONFIG)

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetResetConfig (
  IN GPIOV2_PAD               GpioPad,
  IN GPIOV2_RESET_CONFIG      ResetConfig
  )
{
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);

  P2sbPtr = GetP2sbAddress(CommunityIndex);

  if (ResetConfig == GpioV2ResetHostDeep) {
  }

  if (ResetConfig != GpioV2ResetDefault) {
    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(GPIOV2_PAD_RESET_CONFIG_MASK << GPIOV2_PAD_RESET_CONFIG_DW0_POS);
    OrValue  = ((ResetConfig >> 1) & GPIOV2_PAD_RESET_CONFIG_MASK) << GPIOV2_PAD_RESET_CONFIG_DW0_POS;

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
  This procedure sets termination configuration for requested Gpio Pad

  @param[in] GpioPad             GPIO pad
  @param[in] TerminationConfig   Termination configuration, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
SetTerminationConfig (
  IN GPIOV2_PAD                 GpioPad,
  IN GPIOV2_TERMINATION_CONFIG  TerminationConfig
  )
{
  UINT32 AndValue;
  UINT32 OrValue;
  UINT32 CommunityIndex;
  UINT32 RegisterOffset;
  P2SB_SIDEBAND_REGISTER_ACCESS P2sbPtr;

  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);

  P2sbPtr = GetP2sbAddress(CommunityIndex);

  if (TerminationConfig == GpioV2TermDefault) {
  }

  if (TerminationConfig != GpioV2TermDefault) {
    GetRegisterOffset (
      GpioV2Dw1Reg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(GPIOV2_PAD_TERMINATION_CONFIG_MASK << GPIOV2_PAD_TERMINATION_CONFIG_DW1_POS);
    OrValue  = ((TerminationConfig >> 1) & GPIOV2_PAD_TERMINATION_CONFIG_MASK) << GPIOV2_PAD_TERMINATION_CONFIG_DW1_POS;

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
#if FixedPcdGet8(PcdMtlSSupport) == 1
    *GlobalGroupIndex += MtlPchGpioGetCommunities(CommunityIndex)->GroupsNum;
#else
    *GlobalGroupIndex += MtlSocGpioGetCommunities(CommunityIndex)->GroupsNum;
#endif
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
  DEBUG ((DEBUG_INFO, "[GPIOV2] [UNLOCK] [SELECT] %a ", PadName));
  if (LockCfg == GpioV2Unlock) {
    DEBUG ((DEBUG_INFO, "CFG"));
  }

  if (LockTx == GpioV2Unlock) {
    DEBUG ((DEBUG_INFO, " TX"));
  }
  DEBUG ((DEBUG_INFO, "\n"));
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
  EFI_STATUS       Status = EFI_SUCCESS;
  CHAR8            PadName[GPIOV2_NAME_LENGTH_MAX];
  UINT32           CommunityIndex;
  P2SB_SIDEBAND_REGISTER_ACCESS   P2sbPtr;
  UINT32           RegisterOffset;
  UINT32           RegisterValue;

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

  // For debug purpose - start
  // Read DW0 register offset
  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);
  P2sbPtr = GetP2sbAddress(CommunityIndex);

    GetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

  // Read DW0 register offset data
  RegisterValue = P2sbPtr.Access.Read32 (
    &(P2sbPtr.Access),
    RegisterOffset
  );
  DEBUG ((DEBUG_INFO, "DW0 RegisterOffset= 0x%x, RegisterValue= 0x%x\n", RegisterOffset, RegisterValue));

  // Read DW1 register offset
  GetRegisterOffset (
    GpioV2Dw1Reg,
    GpioPad,
    &RegisterOffset
  );
  // Read DW1 register offset data
  RegisterValue = P2sbPtr.Access.Read32 (
    &(P2sbPtr.Access),
    RegisterOffset
  );
  DEBUG ((DEBUG_INFO, "DW1 RegisterOffset= 0x%x, RegisterValue= 0x%x\n", RegisterOffset, RegisterValue));

  // Read DW2 register offset
  GetRegisterOffset (
    GpioV2Dw2Reg,
    GpioPad,
    &RegisterOffset
  );
  // Read DW2 register offset data
  RegisterValue = P2sbPtr.Access.Read32 (
    &(P2sbPtr.Access),
    RegisterOffset
  );
  DEBUG ((DEBUG_INFO, "DW2 RegisterOffset= 0x%x, RegisterValue= 0x%x\n", RegisterOffset, RegisterValue));
  // For debug purpose - end

  return Status;
}

EFI_STATUS
GetGpioV2ServicesFromPad (
  IN  GPIOV2_PAD                GpioPad
  )
{
  UINT8                   ChipsetId;
  CHAR8                   Hid[GPIOV2_CONTROLLER_HID_LENGTH + 1] = {0};
  UINT32                  Uid;

  ChipsetId = GPIOV2_PAD_GET_CHIPSETID (GpioPad);
  Uid = 0xFFFFFFFF;

#if FixedPcdGet8(PcdMtlSSupport) == 1
  //
  // MTL S MTL-S + MTP-S
  //
  switch (ChipsetId) {
  case GPIOV2_MTL_SOC_S_CHIPSET_ID:
    CopyMem (Hid, GPIO_HID_MTL_SOC_S, GPIOV2_CONTROLLER_HID_LENGTH);
    Uid = 0;
    DEBUG ((GPIO_DEBUG_INFO, "GetGpioV2ServicesFromPad:GPIOV2_MTL_SOC_S_CHIPSET_ID\n"));
    break;

  case GPIOV2_MTL_PCH_S_CHIPSET_ID:
    CopyMem (Hid, GPIOV2_MTL_PCH_S_HID, GPIOV2_CONTROLLER_HID_LENGTH);
    Uid = 0;
    DEBUG ((GPIO_DEBUG_INFO, "GetGpioV2ServicesFromPad:GPIOV2_MTL_PCH_S_CHIPSET_ID\n"));
    break;

  default:
    DEBUG ((GPIO_DEBUG_ERROR, "GetGpioV2ServicesFromPad:Incorrect ChipsetId found\n"));
    break;
  }
#else
  //
  // MTL M/P
  //
  switch (ChipsetId) {
  case GPIOV2_MTL_SOC_M_CHIPSET_ID:
    CopyMem (Hid, GPIO_HID_MTL_SOC_M, GPIOV2_CONTROLLER_HID_LENGTH);
    Uid = 0;
    break;

  default:
    break;
  }
#endif

  if (Uid == 0xFFFFFFFF) {
    //
    // Did not find a match Chipset ID from PAD.
    //
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
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
  IN GPIOV2_INIT_CONFIG                 *GpioTable,
  IN UINT16                             GpioTableCount
  )
{
  ARRAY_CFG_HDR  *GpioCfgCurrHdr;
  ARRAY_CFG_HDR  *GpioCfgBaseHdr;
  EFI_STATUS     Status;

  //
  // If no Tag provided, check for GpioTable info;
  // If    Tag provided, GpioTable params are don't care
  //
  if (Tag == CDATA_NO_TAG) {
    DEBUG ((DEBUG_INFO, "ConfigureGpio() Start\n"));
    Status = GpioV2ConfigurePads (GpioTable, GpioTableCount);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((DEBUG_INFO, "ConfigureGpio() End\n"));
    return EFI_SUCCESS;
  }

  //
  // Find the GPIO CFG HDR
  //
  GpioCfgCurrHdr = (ARRAY_CFG_HDR *)FindConfigDataByTag (Tag);
  if (GpioCfgCurrHdr == NULL) {
    return EFI_NOT_FOUND;
  }

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
  }
  return EFI_SUCCESS;
}
