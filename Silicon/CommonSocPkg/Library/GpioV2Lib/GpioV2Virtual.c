/** @file
  Routines for IBL GPIO virtual pads

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include "Library/GpioV2Virtual.h"
#include <Uefi/UefiBaseType.h>
#include <Library/GpioTopologyLib.h>
#include "Library/GpioV2Lib.h"
#include "Register/GpioV2VirtualRegs.h"
#include "Library/IoLib.h"


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
VirtualGpioGetRegisterOffset (
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
    case GpioV2PadCfgLockReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.PadCfgLock + (PadIndex / 32) * 0x4;
      break;
    case GpioV2GpiIsReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiIs + (PadIndex / 32) * 0x4;
      break;
    case GpioV2GpiIeReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiIe + (PadIndex / 32) * 0x4;
      break;
    case GpioV2GpiGpeStsReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiGpeSts + (PadIndex / 32) * 0x4;
      break;
    case GpioV2GpiGpeEnReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.GpiGpeEn + (PadIndex / 32) * 0x4;
      break;
    case GpioV2SmiStsReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.SmiSts + (PadIndex / 32) * 0x4;
      break;
    case GpioV2SmiEnReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.SmiEn + (PadIndex / 32) * 0x4;
      break;
    case GpioV2NmiStsReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.NmiSts + (PadIndex / 32) * 0x4;
      break;
    case GpioV2NmiEnReg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.NmiEn + (PadIndex / 32) * 0x4;
      break;
    case GpioV2Dw0Reg:
      *RegisterOffset = SocGpioGetGroups(CommunityIndex,GroupIndex).RegisterOffsets.Dw0 + PadIndex * 0x4;
      break;
    default:
      ASSERT(FALSE);
      return EFI_INVALID_PARAMETER;
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
VirtualGpioSetHostOwnership (
  IN GPIOV2_PAD               GpioPad,
  IN GPIOV2_HOSTSW_OWN        HostOnwership
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }


  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  AndValue = (UINT32)~(B_VGPIO_PCR_RX_PAD_HOSTSW_OWNERSHIP);
  OrValue  = ((HostOnwership >> 1) << N_VGPIO_PCR_RX_PAD_HOSTSW_OWNERSHIP) & B_VGPIO_PCR_RX_PAD_HOSTSW_OWNERSHIP;

  MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue, OrValue);

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
VirtualGpioGetOwnership (
  IN  GPIOV2_PAD              GpioPad,
  OUT GPIOV2_PAD_OWN          *Ownership
  )
{
  UINT32  RegisterOffset;
  UINT32  RegisterValue;

  if (Ownership == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  // Please refer to enum GPIOV2_PAD_OWN in GpioV2Pad.h file
  *Ownership = GPIO_ASSIGN_VALUE ((RegisterValue & B_VGPIO_PCR_RX_PAD_OWNERSHIP) >> N_VGPIO_PCR_RX_PAD_OWNERSHIP);

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
VirtualGpioSetTx (
  IN GPIOV2_PAD             GpioPad,
  IN GPIOV2_PAD_STATE       OutputState
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  if (OutputState != GpioV2StateDefault) {
    VirtualGpioGetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(B_VGPIO_PCR_TX_STATE);
    OrValue  = ((OutputState >> 1) << N_VGPIO_PCR_TX_STATE) & B_VGPIO_PCR_TX_STATE;

     MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue, OrValue);
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
VirtualGpioSetTxDisable (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  TxDisabled
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  AndValue = (UINT32)~(B_VGPIO_PCR_TX_DISABLE);
  OrValue  = (TxDisabled << N_VGPIO_PCR_TX_DISABLE) & B_VGPIO_PCR_TX_DISABLE;

  MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue,  OrValue);

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
VirtualGpioSetRxDisable (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  RxDisabled
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  AndValue = (UINT32)~(B_VGPIO_PCR_RX_DISABLE);
  OrValue  = (RxDisabled << N_VGPIO_PCR_RX_DISABLE) & B_VGPIO_PCR_RX_DISABLE;

  MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue,  OrValue);

  return EFI_SUCCESS;
}

/**
  This procedure will set GPIO enable or disable input inversion on requested pad

  @param[in] GpioPad             GPIO pad
  @param[in] InputInversion      GpioV2InputInversionEnable or GpioV2InputInversionDisable, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetInputInversion (
  IN GPIOV2_PAD                  GpioPad,
  IN GPIOV2_PAD_INPUT_INVERSION  InputInversion
  )
{
  return EFI_UNSUPPORTED;
}

/**
  This procedure sets NMI Enable for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] NmiEn                NMI Enable, TRUE or FALSE

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetNmiEn (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  NmiEn
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;
  UINT32           PadIndex;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  VirtualGpioGetRegisterOffset (
    GpioV2NmiEnReg,
    GpioPad,
    &RegisterOffset
    );

  AndValue = (UINT32)~(GPIOV2_PAD_NMI_EN_MASK << (PadIndex % 32));
  OrValue  = (NmiEn & GPIOV2_PAD_NMI_EN_MASK) << (PadIndex % 32);

  MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue,  OrValue);

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
VirtualGpioSetSmiEn (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  SmiEn
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;
  UINT32           PadIndex;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  VirtualGpioGetRegisterOffset (
    GpioV2SmiEnReg,
    GpioPad,
    &RegisterOffset
    );

  AndValue = (UINT32)~(GPIOV2_PAD_SMI_EN_MASK << (PadIndex % 32));
  OrValue  = (SmiEn & GPIOV2_PAD_SMI_EN_MASK) << (PadIndex % 32);

  MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue,  OrValue);

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
VirtualGpioSetGpiGpeEn (
  IN GPIOV2_PAD              GpioPad,
  IN BOOLEAN                 GpiGpeEn
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;
  UINT32           PadIndex;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  VirtualGpioGetRegisterOffset (
    GpioV2GpiGpeEnReg,
    GpioPad,
    &RegisterOffset
    );

  AndValue = (UINT32)~(GPIOV2_PAD_GPI_GPE_EN_MASK << (PadIndex % 32));
  OrValue  = (GpiGpeEn & GPIOV2_PAD_GPI_GPE_EN_MASK) << (PadIndex % 32);

  MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue,  OrValue);

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
VirtualGpioSetGpiIe (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  GpiIe
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;
  UINT32           PadIndex;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  VirtualGpioGetRegisterOffset (
    GpioV2GpiIeReg,
    GpioPad,
    &RegisterOffset
    );

  AndValue = (UINT32)~(GPIOV2_PAD_GPI_IE_MASK << (PadIndex % 32));
  OrValue  = (GpiIe & GPIOV2_PAD_GPI_IE_MASK) << (PadIndex % 32);

  MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue,  OrValue);

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
VirtualGpioSetRouteNmi (
  IN GPIOV2_PAD           GpioPad,
  IN BOOLEAN              Enable
  )
{
  GPIOV2_PAD_OWN   Ownership;
  GPIOV2_CONFIG    PadConfig;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  if (Enable) {
    PadConfig.InterruptConfig = GpioV2IntNmi;
  } else {
    PadConfig.InterruptConfig = GpioV2IntDis;
  }

  return VirtualGpioSetInterruptOrLevelType (GpioPad, &PadConfig);
}

/**
  This procedure will configure VGPIO INTERRUPT_OR_EVENT_TYPE field.

  @param[in] GpioPad              Gpio Pad
  @param[in] PadConfig            Gpio Pad configuration - please refer to GPIOV2_CONFIG in GpioV2Config.h

  @retval EFI_SUCCESS              The function completed successfully
  @retval EFI_INVALID_PARAMETER    Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetInterruptOrLevelType (
  IN GPIOV2_PAD              GpioPad,
  IN GPIOV2_CONFIG           *PadConfig
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterOffset;
  UINT32           OrValue;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
    );

  OrValue = 0;

  if (PadConfig->InterruptConfig != GpioV2IntDefault) {
    if (PadConfig->InterruptConfig == GpioV2IntDis) {
      OrValue |= V_VGPIO_PCR_INT_TYPE_DIS;
    } else if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntApic >> 1)) {
      OrValue |= V_VGPIO_PCR_INT_TYPE_IRQ;
    } else if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntNmi >> 1)) {
      OrValue |= V_VGPIO_PCR_INT_TYPE_NMI;
    } else if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntSmi >> 1)) {
      OrValue |= V_VGPIO_PCR_INT_TYPE_SMI;
    } else if ((PadConfig->InterruptConfig >> 1) & (GpioV2IntSci >> 1)) {
      OrValue |= V_VGPIO_PCR_INT_TYPE_SCI;
    }
    MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, (UINT32)~(B_VGPIO_PCR_INT_TYPE),  (OrValue << N_VGPIO_PCR_INT_TYPE));
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
VirtualGpioSetRouteSmi (
  IN GPIOV2_PAD           GpioPad,
  IN BOOLEAN              Enable
  )
{
  GPIOV2_PAD_OWN   Ownership;
  GPIOV2_CONFIG    PadConfig;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  if (Enable) {
    PadConfig.InterruptConfig = GpioV2IntSmi;
  } else {
    PadConfig.InterruptConfig = GpioV2IntDis;
  }

  return VirtualGpioSetInterruptOrLevelType (GpioPad, &PadConfig);
}

/**
  This procedure sets GPIROUTSCI bit (19th bit in DW0) for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Enable               TRUE or FALSE, either enable or disable 19 bit in DW0

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetRouteSci (
  IN GPIOV2_PAD           GpioPad,
  IN BOOLEAN              Enable
  )
{
  GPIOV2_PAD_OWN   Ownership;
  GPIOV2_CONFIG    PadConfig;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  if (Enable) {
    PadConfig.InterruptConfig = GpioV2IntSci;
  } else {
    PadConfig.InterruptConfig = GpioV2IntDis;
  }

  return VirtualGpioSetInterruptOrLevelType (GpioPad, &PadConfig);
}

/**
  This procedure sets GPIROUTIOXAPIC bit (20th bit in DW0) for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Enable               TRUE or FALSE, either enable or disable 20th bit in DW0

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetRouteIoxApic (
  IN GPIOV2_PAD           GpioPad,
  IN BOOLEAN              Enable
  )
{
  GPIOV2_PAD_OWN   Ownership;
  GPIOV2_CONFIG    PadConfig;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  if (Enable) {
    PadConfig.InterruptConfig = GpioV2IntApic;
  } else {
    PadConfig.InterruptConfig = GpioV2IntDis;
  }

  return VirtualGpioSetInterruptOrLevelType (GpioPad, &PadConfig);
}

/**
  This procedure sets RxEv configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] RxEvCfg              RxEv configuration

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetRxEvCfg (
  IN GPIOV2_PAD           GpioPad,
  IN GPIOV2_RXEVCFG       RxEvCfg
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;

  if (RxEvCfg != GpioV2IntRxEvCfgDefault) {

    if ((RxEvCfg != GpioV2IntRxEvCfgLevel) && (RxEvCfg != GpioV2IntRxEvCfgDisable)) {
      return EFI_UNSUPPORTED;
    }

    VirtualGpioGetOwnership (GpioPad, &Ownership);
    if (Ownership != GpioV2PadOwnHost) {
      return EFI_ACCESS_DENIED;
    }

    VirtualGpioGetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(B_VGPIO_PCR_RX_LVL_EDG);
    OrValue  = ((RxEvCfg >> 1) << N_VGPIO_PCR_RX_LVL_EDG) & B_VGPIO_PCR_RX_LVL_EDG;

    MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue,  OrValue);
  }

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
VirtualGpioSetResetConfig (
  IN GPIOV2_PAD               GpioPad,
  IN GPIOV2_RESET_CONFIG      ResetConfig
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           RegisterOffset;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  if (ResetConfig != GpioV2ResetDefault) {
    VirtualGpioGetRegisterOffset (
      GpioV2Dw0Reg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = (UINT32)~(B_VGPIO_PCR_PAD_RESET_CONFIG);
    OrValue  = ((ResetConfig >> 1) << N_VGPIO_PCR_PAD_RESET_CONFIG) & B_VGPIO_PCR_PAD_RESET_CONFIG;

    MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue,  OrValue);
  }

  return EFI_SUCCESS;
}

/**
  This procedure will set GPIO mode

  @param[in] GpioPad              Gpio Pad. Please refer to GpioPinsYYY.h - where YYY name of the platform (eg. MTL, EBG, ...)
  @param[in] PadModeValue         GPIO pad mode value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetPadMode (
  IN GPIOV2_PAD              GpioPad,
  IN GPIOV2_PAD_MODE         PadMode
  )
{
  return EFI_UNSUPPORTED;
}

/**
  This procedure reads current GPIO Pad Mode

  @param[in] GpioPad             GPIO Pad. Please refer to GpioPinsYYY.h - where YYY name of the platform (eg. MTL, EBG, ...)
  @param[in] PadMode             Pointer to a buffer for GPIO Pad Mode

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetPadMode (
  IN GPIOV2_PAD            GpioPad,
  IN GPIOV2_PAD_MODE       *PadMode
  )
{
  return EFI_UNSUPPORTED;
}

/**
  This procedure reads current host ownership configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] Ownership            Pointer to a buffer for ownership, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetHostOwnership (
  IN  GPIOV2_PAD              GpioPad,
  OUT GPIOV2_HOSTSW_OWN       *HostOwnership
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterOffset;
  UINT32           RegisterValue;

  if (HostOwnership == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  // Please refer to enum GPIOV2_HOSTSW_OWN in GpioV2Pad.h file
  *HostOwnership = GPIO_ASSIGN_VALUE ((RegisterValue & B_VGPIO_PCR_RX_PAD_HOSTSW_OWNERSHIP) >> N_VGPIO_PCR_RX_PAD_HOSTSW_OWNERSHIP);

  return EFI_SUCCESS;
}

/**
  This procedure reads current Gpio Pad output state

  @param[in] GpioPad             GPIO pad
  @param[in] OutputState         Pointer to a buffer for output state

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetTx (
  IN GPIOV2_PAD             GpioPad,
  IN GPIOV2_PAD_STATE       *OutputState
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterValue;
  UINT32           RegisterOffset;

  if (OutputState == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  if (RegisterValue & B_VGPIO_PCR_TX_STATE) {
    *OutputState = GpioV2StateHigh;
  } else {
    *OutputState = GpioV2StateLow;
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads current Gpio Pad input state

  @param[in] GpioPad             GPIO pad
  @param[in] InputState          Pointer to a buffer for input state

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetRx (
  IN GPIOV2_PAD           GpioPad,
  IN GPIOV2_PAD_STATE     *InputState
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterValue;
  UINT32           RegisterOffset;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  if (RegisterValue & B_VGPIO_PCR_RX_STATE) {
    *InputState = GpioV2StateHigh;
  } else {
    *InputState = GpioV2StateLow;
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads if TX buffer for requested Gpio Pad is enabled or disabled

  @param[in] GpioPad             GPIO pad
  @param[in] TxDisabled          Pointer to a buffer for enabled/disabled information

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetTxDisable (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  *TxDisabled
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterOffset;
  UINT32           RegisterValue;

  if (TxDisabled == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  if (RegisterValue & B_VGPIO_PCR_TX_DISABLE) {
    *TxDisabled = TRUE;
  } else {
    *TxDisabled = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads if RX buffer for requested Gpio Pad is enabled or disabled

  @param[in] GpioPad             GPIO pad
  @param[in] RxDisabled          Pointer to a buffer for enabled/disabled information

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetRxDisable (
  IN GPIOV2_PAD              GpioPad,
  IN BOOLEAN                 *RxDisabled
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterOffset;
  UINT32           RegisterValue;

  if (RxDisabled == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  if (RegisterValue & B_VGPIO_PCR_RX_DISABLE) {
    *RxDisabled = TRUE;
  } else {
    *RxDisabled = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  This procedure will Get GPIO enable or disable input inversion on requested pad

  IBL Virtaul GPIO does not support inversion, function return state as not inverted.

  @param[in]  GpioPad             GPIO pad
  @param[Out] Data                Enabled / Disabled

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetInputInversion (
  IN  GPIOV2_PAD                 GpioPad,
  OUT BOOLEAN                    *Inverted
  )
{
  *Inverted = FALSE;

  return EFI_SUCCESS;
}

/**
  This procedure will set GPIO Lock (register PADCFGLOCK)

  @param[in] GpioPad             GPIO pad
  @param[in] Lock                GpioV2Unlock - Unlock pad, GpioV2Lock - Lock pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetLock (
  IN GPIOV2_PAD                GpioPad,
  IN GPIOV2_PAD_LOCK           Lock
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           AndValue;
  UINT32           OrValue;
  UINT32           PadIndex;
  UINT32           RegisterOffset;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  if (Lock != GpioV2LockHardwareDefault) {

    VirtualGpioGetRegisterOffset (
      GpioV2PadCfgLockReg,
      GpioPad,
      &RegisterOffset
    );

    AndValue = ~(GPIOV2_PAD_LOCK_MASK << PadIndex % 32);
    OrValue  = ((Lock >> 1) & GPIOV2_PAD_LOCK_MASK) << PadIndex % 32;

    MmioAndThenOr32 (GetVgpioBaseAddress() + RegisterOffset, AndValue, OrValue);
  }

  return EFI_SUCCESS;
}

/**
  This procedure will get GPIO Lock (register PADCFGLOCK)

  @param[in]  GpioPad             GPIO pad
  @param[out] Lock                GpioV2Unlock - Unlock pad, GpioV2Lock - Lock pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetLock (
  IN  GPIOV2_PAD                GpioPad,
  OUT GPIOV2_PAD_LOCK           *Lock
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           PadIndex;
  UINT32           RegisterValue;
  UINT32           RegisterOffset;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  VirtualGpioGetRegisterOffset (
    GpioV2PadCfgLockReg,
    GpioPad,
    &RegisterOffset
  );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  if ((RegisterValue >> PadIndex) & GPIOV2_PAD_LOCK_MASK) {
    *Lock = GpioV2Lock;
  } else {
    *Lock = GpioV2Unlock;
  }

  return EFI_SUCCESS;
}

/**
  This procedure will set GPIO Lock Tx (register PADCFGLOCKTX)

  @param[in] GpioPad             GPIO pad
  @param[in] LockTx              GpioV2Unlock - Unlock output state of Gpio Pad, GpioV2Lock - Lock output state of Gpio Pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetLockTx (
  IN GPIOV2_PAD                  GpioPad,
  IN GPIOV2_PAD_LOCK             LockTx
  )
{
  //
  // VGPIOs have no seperate PADCFGLOCKTX register. TX is locked via PADCFGLOCK.
  // Use SetLock here to seamlessly handle this difference.
  //
  return VirtualGpioSetLock (GpioPad, LockTx);
}

/**
  This procedure will get GPIO Lock TX

  Virtual GPIO does not have Lock TX bit
  Lock TX state is based on Lock pad state

  @param[in]  GpioPad             GPIO pad
  @param[out] LockTx              GpioV2Unlock - Unlock output state of Gpio Pad, GpioV2Lock - Lock output state of Gpio Pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetLockTx (
  IN  GPIOV2_PAD                  GpioPad,
  OUT GPIOV2_PAD_LOCK             *LockTx
  )
{
  return VirtualGpioGetLock (GpioPad, LockTx);
}

/**
  This procedure reads current GPI GPE Enable configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] GpiIe                GPI GPE Enable, TRUE or FALSE

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetGpiGpeEn (
  IN GPIOV2_PAD              GpioPad,
  IN BOOLEAN                 *GpiGpeEn
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterOffset;
  UINT32           PadIndex;
  UINT32           RegisterValue;

  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2GpiGpeEnReg,
    GpioPad,
    &RegisterOffset
    );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  if ((RegisterValue >> (PadIndex % 32)) & GPIOV2_PAD_GPI_GPE_EN_MASK) {
    *GpiGpeEn = TRUE;
  } else {
    *GpiGpeEn = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads current Gpi Enable configuration for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] GpiIe                Pointer to a buffer for GPI Enable

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetGpiIe (
  IN GPIOV2_PAD              GpioPad,
  IN BOOLEAN                 *GpiIe
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterOffset;
  UINT32           PadIndex;
  UINT32           RegisterValue;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  VirtualGpioGetRegisterOffset (
    GpioV2GpiIeReg,
    GpioPad,
    &RegisterOffset
    );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  if ((RegisterValue >> (PadIndex % 32)) & GPIOV2_PAD_GPI_IE_MASK) {
    *GpiIe = TRUE;
  } else {
    *GpiIe = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads current Gpi Status for requested Gpio Pad

  @param[in] GpioPad              GPIO pad
  @param[in] GpiIe                Pointer to a buffer for GPI Enable

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetGpiIs (
  IN GPIOV2_PAD               GpioPad,
  IN BOOLEAN                  *GpiIs
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterOffset;
  UINT32           PadIndex;
  UINT32           RegisterValue;

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  PadIndex = GPIOV2_PAD_GET_PAD_INDEX (GpioPad);

  VirtualGpioGetRegisterOffset (
    GpioV2GpiIsReg,
    GpioPad,
    &RegisterOffset
    );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  if ((RegisterValue >> (PadIndex % 32)) & GPIOV2_PAD_GPI_IS_MASK) {
    *GpiIs = TRUE;
  } else {
    *GpiIs = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  This procedure reads current Reset Configuration for requested Gpio Pad

  @param[in] GpioPad             GPIO pad
  @param[in] ResetConfig         Pointer to a buffer for Reset Configuration

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
VirtualGpioGetResetConfig (
  IN GPIOV2_PAD               GpioPad,
  IN GPIOV2_RESET_CONFIG      *ResetConfig
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterOffset;
  UINT32           RegisterValue;

  if (ResetConfig == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  VirtualGpioGetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
  );

  RegisterValue = MmioRead32 (GetVgpioBaseAddress() + RegisterOffset);

  // Please refer to enum GPIOV2_RESET_CONFIG in GpioV2Pad.h file
  *ResetConfig = GPIO_ASSIGN_VALUE ((RegisterValue & B_VGPIO_PCR_PAD_RESET_CONFIG) >> N_VGPIO_PCR_PAD_RESET_CONFIG);

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
VirtualGpioSetTerminationConfig (
  IN GPIOV2_PAD                 GpioPad,
  IN GPIOV2_TERMINATION_CONFIG  TerminationConfig
  )
{
  return EFI_UNSUPPORTED;
}

/**
  This procedure will configure VGPIO CS setting.

  @param[in] GpioPad              Gpio Pad
  @param[in] PadConfig            Gpio Pad configuration - please refer to GPIOV2_CONFIG in GpioV2Config.h

  @retval EFI_SUCCESS              The function completed successfully
  @retval EFI_INVALID_PARAMETER    Invalid group or pad number
**/
EFI_STATUS
VirtualGpioSetCs (
  IN GPIOV2_PAD              GpioPad,
  IN GPIOV2_CONFIG           *PadConfig
  )
{
  GPIOV2_PAD_OWN   Ownership;
  UINT32           RegisterOffset;
  UINT32           Value;
  EFI_STATUS       Status;
  BOOLEAN          TxDisabled;

  if (PadConfig == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  GetOwnership (GpioPad, &Ownership);
  if (Ownership != GpioV2PadOwnHost) {
    return EFI_ACCESS_DENIED;
  }

  VirtualGpioGetRegisterOffset (
    GpioV2Dw0Reg,
    GpioPad,
    &RegisterOffset
    );

  Value = (MmioRead32 (GetVgpioBaseAddress() + RegisterOffset) & B_VGPIO_PCR_OUT_CS) >> N_VGPIO_PCR_OUT_CS;
  if (Value == PadConfig->VgpioCs) {
    return EFI_SUCCESS;
  }

  switch (PadConfig->VgpioCs) {
    case GpioV2VgpioCs0:
      Value = V_VGPIO_PCR_OUT_CS_0;
      break;
    case GpioV2VgpioCs1:
      Value = V_VGPIO_PCR_OUT_CS_1;
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }

  // To make CS selection, make sure TX Disable bit is set to 1
  Status = VirtualGpioGetTxDisable (GpioPad, &TxDisabled);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (!TxDisabled) {
    Status = VirtualGpioSetTxDisable (GpioPad, TRUE);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  MmioAndThenOr32 (
    RegisterOffset,
    (UINT32)~(B_VGPIO_PCR_OUT_CS),
    (Value << N_VGPIO_PCR_OUT_CS)
    );

  if (!TxDisabled) {
    VirtualGpioSetTxDisable (GpioPad, FALSE);
  }

  return EFI_SUCCESS;
}