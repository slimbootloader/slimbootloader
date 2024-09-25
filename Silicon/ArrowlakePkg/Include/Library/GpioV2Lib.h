/** @file
  The platform GPIO library header.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_V2_LIB_H_
#define _GPIO_V2_LIB_H_

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <GpioV2Config.h>

#pragma pack(push, 1)

#define GPIOV2_CONTROLLER_HID_LENGTH  8
#define GPIOV2_NAME_LENGTH_MAX        32

/**
  GPIO configuration structure used for pin programming.
  Structure contains fields that can be used to configure pad.
**/
typedef struct {
  /**
  Pad Mode
  Pad can be set as GPIO or one of its native functions.
  When in native mode setting Direction (except Inversion), OutputState,
  InterruptConfig, Host Software Pad Ownership and OutputStateLock are unnecessary.
  Refer to definition of GPIOV2_PAD_MODE.
  Refer to EDS for each native mode according to the pad.
  **/
  UINT32 PadMode             : 5;
  /**
  Host Software Pad Ownership
  Set pad to ACPI mode or GPIO Driver Mode.
  Refer to definition of GPIOV2_HOSTSW_OWN.
  **/
  UINT32 HostSwOwn           : 2;
  /**
  GPIO Direction
  Can choose between In, In with inversion, Out, both In and Out, both In with inversion and out or disabling both.
  Refer to definition of GPIOV2_DIRECTION for supported settings.
  **/
  UINT32 Direction           : 6;
  /**
  Output State
  Set Pad output value.
  Refer to definition of GPIOV2_PAD_STATE for supported settings.
  This setting takes place when output is enabled.
  **/
  UINT32 OutputState         : 2;
  /**
  GPIO Interrupt Configuration
  Set Pad to cause one of interrupts (IOxAPIC/SCI/SMI/NMI).
  This setting is applicable only if GPIO is in GpioMode with input enabled.
  Refer to definition of GPIOV2_INT_CONFIG for supported settings.
  **/
  UINT32 InterruptConfig     : 9;
  /**
  GPIO Reset Configuration.
  This setting controls Pad Reset Configuration.
  Refer to definition of GPIOV2_RESET_CONFIG for supported settings.
  **/
  UINT32 ResetConfig         : 8;

  /**
  GPIO Electrical Configuration
  This setting controls pads termination.
  Refer to definition of GPIOV2_TERMINATION_CONFIG for supported settings.
  **/
  UINT32 TerminationConfig   : 6;
  /**
  GPIO Lock Configuration
  This setting controls pads lock.
  Refer to definition of GPIOV2_PAD_LOCK for supported settings.
  **/
  UINT32 LockConfig          : 2;
  /**
  GPIO Lock Output State
  This setting controls pads lock.
  Refer to definition of GPIOV2_PAD_LOCK for supported settings.
  **/
  UINT32 LockTx              : 2;
  /**
  Reserved for additional GPIO configuration
  **/
  UINT32 Reserved            : 6;

  /**
  Reserved for SBL GPIO configuration data.
  The following 2 bytes are only used by SBL GPIO configuration data
  They should match with GPIO configuration data
  **/
  UINT32 PadIdx              : 5;
  UINT32 GrpIdx              : 3;
  UINT32 ComIdx              : 3;
  UINT32 ChipId              : 3;
  UINT32 Hide                : 1;
  UINT32 Skip                : 1;
} GPIOV2_CONFIG;

typedef struct {
  GPIOV2_PAD           GpioPad;
  GPIOV2_CONFIG        GpioConfig;
} GPIOV2_INIT_CONFIG;

#pragma pack(pop)

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
  );

/**
  This procedure retrieves register offset

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
  );

/**
  Read GPIO register

  @param[in] GpioPad             GPIO pad
  @param[in] RegOffset           The register offset

  @retval data             The data read from the given reg offset.
**/
UINT32
EFIAPI
GpioRead32 (
  IN GPIOV2_PAD             GpioPad,
  IN UINT32                 RegOffset
  );


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
  );


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
  );


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
  );


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
  );


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
  );


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
  );


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
  );

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
  );

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
  );

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
  );

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
  );

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
  );


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
  );

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
  );

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
  );


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
  );


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
  );


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
  );

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
  );


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
  );

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
  );

/**
  Configure the GPIO pins, available as part of platform specific GPIO CFG DATA.
  If the pins are not part of GPIO CFG DATA, pass CDATA_NO_TAG for Tag param.

  @param    Tag         Tag ID of the Gpio Cfg data item
  @param    Entries     Number of entries in Gpio Table
  @param    DataBuffer  Pointer to the Gpio Table to be programmed

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_NOT_FOUND                 If Gpio Config Data cant be found
**/
EFI_STATUS
EFIAPI
ConfigureGpioV2 (
  IN  UINT16              Tag,
  IN  GPIOV2_INIT_CONFIG  *GpioTable,
  IN  UINT16              GpioTableCount
  );

#endif // _GPIO_LIB_H_
