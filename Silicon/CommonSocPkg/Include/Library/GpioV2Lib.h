/** @file
  Header file for platform GPIO V2 library.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIOV2_LIB_H_
#define _GPIOV2_LIB_H_

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Include/GpioV2Config.h>
#include <Library/GpioV2ControllerInterface.h>
#include <Library/BlMemoryAllocationLib.h>

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
  );

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
  );

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
GetRegisterOffset (
  IN  GPIOV2_REGISTER     Register,
  IN  GPIOV2_PAD          GpioPad,
  OUT UINT32              *RegisterOffset
  );

/**
  This procedure retrieves register offset

  @param[in] CommunityIndex      GPIO community group index
  @retval P2sb side band register base address
**/
P2SB_SIDEBAND_REGISTER_ACCESS
GetP2sbAddress (
 IN UINT32  CommunityIndex
);

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );


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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

/**
  This procedure will confirm All pads in GpioPadsConfigTable belong to the same ChipsetId.

  @param[in] GpioPad     Gpio pad

  @retval Status
**/
EFI_STATUS
GetGpioV2ServicesFromPad (
  IN  GPIOV2_PAD                GpioPad
  );

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
  );

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
  );

/**
  Print the output of the GPIO Config table that was read from CfgData.

  @param GpioPinNum           Number of GPIO entries in the table.

  @param GpioConfData         GPIO Config Data that was read from the Configuration region either from internal or external source.

**/
VOID
PrintGpioV2ConfigTable (
  IN UINT32              GpioPinNum,
  IN VOID*               GpioConfData
);

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
  );

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
  );

#endif // _GPIOV2_LIB_H_