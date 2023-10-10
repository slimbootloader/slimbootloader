/** @file
  Routines for IBL GPIO virtual pads

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef GPIO_V2_VIRTUAL_H
#define GPIO_V2_VIRTUAL_H

#include "Library/GpioV2Lib.h"

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

#endif // _GPIO_V2_VIRTUAL_H_
