/** @file
  The platform GPIO library header.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_LIBRARY_H_
#define _GPIO_LIBRARY_H_

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GpioPlatformLib.h>

/**
  This procedure is used to lock all GPIO pads except the ones
  which were requested during their configuration to be left unlocked.
  This function must be called before BIOS_DONE - before POSTBOOT_SAI is enabled.
    FSP - call this function from wrapper before transition to FSP-S
    UEFI/EDK - call this function before EndOfPei event

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioLockPads (
  VOID
  );


/**
  This procedure is used to clear GPE STS for a specified GpioPad

  @param[in]  GpioPad             GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioClearGpiGpeSts (
  IN GPIO_PAD                  GpioPad
  );


/**
  This procedure will get Group to GPE mapping. If group has more than 32 bits
  it is possible to map only single DW of pins (e.g. 0-31, 32-63) because
  ACPI GPE_DWx register is 32 bits large.

  @param[out]  GroupToGpeDw0       GPIO group mapped to GPE_DW0
  @param[out]  GroupDwForGpeDw0    DW of pins mapped to GPE_DW0
  @param[out]  GroupToGpeDw1       GPIO group mapped to GPE_DW1
  @param[out]  GroupDwForGpeDw1    DW of pins mapped to GPE_DW1
  @param[out]  GroupToGpeDw2       GPIO group mapped to GPE_DW2
  @param[out]  GroupDwForGpeDw2    DW of pins mapped to GPE_DW2

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioGetGroupDwToGpeDwX (
  OUT GPIO_GROUP                *GroupToGpeDw0,
  OUT UINT32                    *GroupDwForGpeDw0,
  OUT GPIO_GROUP                *GroupToGpeDw1,
  OUT UINT32                    *GroupDwForGpeDw1,
  OUT GPIO_GROUP                *GroupToGpeDw2,
  OUT UINT32                    *GroupDwForGpeDw2
  );


/**
  This procedure will set PadCfgLock for selected pad

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioLockPadCfg (
  IN GPIO_PAD                   GpioPad
  );


/**
  This procedure will check state of Pad Config Lock for selected pad

  @param[in]  GpioPad             GPIO pad
  @param[out] PadCfgLock          PadCfgLock for selected pad
                                  0: NotLocked, 1: Locked

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetPadCfgLock (
  IN GPIO_PAD                   GpioPad,
  OUT UINT32                    *PadCfgLock
  );


/**
  This procedure will clear PadCfgLock for selected pad.
  Unlocking a pad will cause an SMI (if enabled)

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioUnlockPadCfg (
  IN GPIO_PAD                   GpioPad
  );


/**
  This procedure will set GPIO output level

  @param[in] GpioPad              GPIO pad
  @param[in] Value                Output value
                                  0: OutputLow, 1: OutputHigh

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioSetOutputValue (
  IN GPIO_PAD                  GpioPad,
  IN UINT32                    Value
  );


/**
  This procedure will get GPIO output level

  @param[in]  GpioPad             GPIO pad
  @param[out] OutputVal           GPIO Output value
                                  0: OutputLow, 1: OutputHigh

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetOutputValue (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32                   *OutputVal
  );


/**
  This procedure will get GPIO input level

  @param[in]  GpioPad             GPIO pad
  @param[out] InputVal            GPIO Input value
                                  0: InputLow, 1: InputHigh

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetInputValue (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32                   *InputVal
  );


/**
  This procedure will configure GPIO input inversion

  @param[in] GpioPad              GPIO pad
  @param[in] Value                Value for GPIO input inversion
                                  0: No input inversion, 1: Invert input

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioSetInputInversion (
  IN GPIO_PAD                  GpioPad,
  IN UINT32                    Value
  );


/**
  This procedure will get GPIO pad input inversion value

  @param[in] GpioPad              GPIO pad
  @param[out] InvertState         GPIO inversion state
                                  0: No input inversion, 1: Inverted input

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetInputInversion (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32                   *InvertState
  );


/**
  This procedure will set GPIO interrupt settings

  @param[in] GpioPad              GPIO pad
  @param[in] Value                Value of Level/Edge
                                  use GPIO_INT_CONFIG as argument

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioSetPadInterruptConfig (
  IN GPIO_PAD                 GpioPad,
  IN GPIO_INT_CONFIG          Value
  );


/**
  This procedure will set GPIO electrical settings

  @param[in] GpioPad              GPIO pad
  @param[in] Value                Value of termination
                                  use GPIO_ELECTRICAL_CONFIG as argument

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioSetPadElectricalConfig (
  IN GPIO_PAD                  GpioPad,
  IN GPIO_ELECTRICAL_CONFIG    Value
  );


/**
  This procedure will set GPIO Reset settings

  @param[in] GpioPad              GPIO pad
  @param[in] Value                Value for Pad Reset Configuration
                                  use GPIO_RESET_CONFIG as argument

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioSetPadResetConfig (
  IN GPIO_PAD                  GpioPad,
  IN GPIO_RESET_CONFIG         Value
  );


/**
  This procedure will get GPIO Reset settings

  @param[in] GpioPad              GPIO pad
  @param[in] Value                Value of Pad Reset Configuration
                                  based on GPIO_RESET_CONFIG

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetPadResetConfig (
  IN GPIO_PAD                  GpioPad,
  IN GPIO_RESET_CONFIG         *Value
  );


/**
  This procedure will read GPIO Pad Configuration register

  @param[in] GpioPad          GPIO pad
  @param[in] DwReg            Choose PADCFG register: 0:DW0, 1:DW1

  @retval PadCfgRegValue      PADCFG_DWx value
**/
UINT32
GpioReadPadCfgReg (
  IN GPIO_PAD             GpioPad,
  IN UINT8                DwReg
  );


/**
  This procedure will write or read GPIO Pad Configuration register

  @param[in] GpioPad              GPIO pad
  @param[in] DwReg                Choose PADCFG register: 0:DW0, 1:DW1
  @param[in] PadCfgAndMask        Mask to be AND'ed with PADCFG reg value
  @param[in] PadCfgOrMask         Mask to be OR'ed with PADCFG reg value

  @retval none
**/
VOID
GpioWritePadCfgReg (
  IN GPIO_PAD             GpioPad,
  IN UINT8                DwReg,
  IN UINT32               PadCfgAndMask,
  IN UINT32               PadCfgOrMask
  );


/**
  This procedure will read multiple GPIO settings

  @param[in]  GpioPad                   GPIO Pad
  @param[out] GpioData                  GPIO data structure

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid GpioPad
**/
EFI_STATUS
GpioGetPadConfig (
  IN  GPIO_PAD               GpioPad,
  OUT GPIO_CONFIG            *GpioData
  );

#endif // _GPIO_LIBRARY_H_

