/** @file
  The platform GPIO library header.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_LIB_H_
#define _GPIO_LIB_H_

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <GpioConfig.h>

#define GPIO_NAME_LENGTH_MAX  32

//
// Number of PADCFG_DW registers
//
#define GPIO_PADCFG_DW_REG_NUMBER  4

//
// Max supported platform ID for GPIO base table
//
#define GPIO_MAX_BASE_TABLE_PLATFORM_ID  32

/**
  This procedure will initialize multiple GPIO pins. Use GPIO_INIT_CONFIG structure.
  Structure contains fields that can be used to configure each pad.
  Pad not configured using GPIO_INIT_CONFIG will be left with hardware default values.
  Separate fields could be set to hardware default if it does not matter, except
  GpioPad and PadMode.
  Function will work in most efficient way if pads which belong to the same group are
  placed in adjacent records of the table.
  Although function can enable pads for Native mode, such programming is done
  by reference code when enabling related silicon feature.

  @param[in] NumberofItem               Number of GPIO pads to be updated
  @param[in] GpioInitTableAddress       GPIO initialization table

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioConfigurePads (
  IN UINT32                    NumberOfItems,
  IN VOID                     *GpioInitTableAddress
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
ConfigureGpio (
  IN  UINT16  Tag,
  IN  UINT32  Entries,
  IN  UINT8   *DataBuffer
  );


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
EFIAPI
GpioLockPads (
  VOID
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
EFIAPI
GpioGetGroupDwToGpeDwX (
  OUT GPIO_GROUP                *GroupToGpeDw0,
  OUT UINT32                    *GroupDwForGpeDw0,
  OUT GPIO_GROUP                *GroupToGpeDw1,
  OUT UINT32                    *GroupDwForGpeDw1,
  OUT GPIO_GROUP                *GroupToGpeDw2,
  OUT UINT32                    *GroupDwForGpeDw2
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
EFIAPI
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
EFIAPI
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
EFIAPI
GpioGetInputValue (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32                   *InputVal
  );


/**
  This procedure will get GPIO strap value

  @param[in]  GpioPad             GPIO pad
  @param[out] StrapVal            GPIO Strap value
                                  0: Low, 1: High

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioGetStrapValue (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32                   *StrapVal
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
EFIAPI
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
EFIAPI
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
EFIAPI
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
EFIAPI
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
EFIAPI
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
EFIAPI
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
EFIAPI
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
EFIAPI
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
EFIAPI
GpioGetPadConfig (
  IN  GPIO_PAD               GpioPad,
  OUT GPIO_CONFIG            *GpioData
  );


/**
  This procedure will get Gpio Pad Ownership

  @param[in] GpioPad              GPIO pad
  @param[out] PadOwnVal           Value of Pad Ownership

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioGetPadOwnership (
  IN  GPIO_PAD                GpioPad,
  OUT GPIO_PAD_OWN            *PadOwnVal
  );

/**
  This procedure will calculate GPIO_RESET_CONFIG value  (new type)
  based on provided PadRstCfg for a specific GPIO Pad.

  @param[in]  GpioPad               GPIO Pad
  @param[in]  PadRstCfg             GPIO PadRstCfg value

  @retval GpioResetConfig           GPIO Reset configuration (new type)
**/
GPIO_RESET_CONFIG
EFIAPI
GpioResetConfigFromPadRstCfg (
  IN  GPIO_PAD           GpioPad,
  IN  UINT32             PadRstCfg
  );

/**
  This procedure will calculate PadRstCfg register value based
  on provided GPIO Reset configuration for a certain pad.

  @param[in]  GpioPad                   GPIO Pad
  @param[in]  GpioResetConfig           GPIO Reset configuration
  @param[out] PadRstCfg                 GPIO PadRstCfg value

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid configuration
**/
EFI_STATUS
EFIAPI
GpioPadRstCfgFromResetConfig (
  IN  GPIO_PAD           GpioPad,
  IN  GPIO_RESET_CONFIG  GpioResetConfig,
  OUT UINT32             *PadRstCfg
  );

/**
  This procedure will calculate PADCFG register value based on GpioConfig data

  @param[in]  GpioPad                   GPIO Pad
  @param[in]  GpioConfig                GPIO Configuration data
  @param[out] PadCfgDwReg               PADCFG DWx register value
  @param[out] PadCfgDwRegMask           Mask with PADCFG DWx register bits to be modified

  @retval Status
**/
EFI_STATUS
EFIAPI
GpioPadCfgRegValueFromGpioConfig (
  IN  GPIO_PAD           GpioPad,
  IN  CONST GPIO_CONFIG  *GpioConfig,
  OUT UINT32             *PadCfgDwReg,
  OUT UINT32             *PadCfgDwRegMask
  );

/**
  This procedure will clear PadCfgLockTx for selected pads within one group.
  Unlocking a pad will cause an SMI (if enabled)

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLockTx register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToUnlockTx      Bitmask for pads which are going to be unlocked,
                                  Bit position - PadNumber
                                  Bit value - 0: DoNotUnLockTx, 1: LockTx

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioUnlockPadCfgTxForGroupDw (
  IN GPIO_GROUP                Group,
  IN UINT32                    DwNum,
  IN UINT32                    PadsToUnlockTx
  );

/**
  This procedure will check state of Pad Config Lock for pads within one group

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLock register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[out] PadCfgLockRegVal    Value of PadCfgLock register
                                  Bit position - PadNumber
                                  Bit value - 0: NotLocked, 1: Locked

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or DwNum parameter number
**/
EFI_STATUS
EFIAPI
GpioGetPadCfgLockForGroupDw (
  IN  GPIO_GROUP                  Group,
  IN  UINT32                      DwNum,
  OUT UINT32                      *PadCfgLockRegVal
  );

/**
  This procedure will check state of Pad Config Tx Lock for pads within one group

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLockTx register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[out] PadCfgLockTxRegVal  Value of PadCfgLockTx register
                                  Bit position - PadNumber
                                  Bit value - 0: NotLockedTx, 1: LockedTx

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or DwNum parameter number
**/
EFI_STATUS
EFIAPI
GpioGetPadCfgLockTxForGroupDw (
  IN  GPIO_GROUP                  Group,
  IN  UINT32                      DwNum,
  OUT UINT32                      *PadCfgLockTxRegVal
  );

/**
  This procedure will clear PadCfgLock for selected pads within one group.
  Unlocking a pad will cause an SMI (if enabled)

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLock register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToUnlock        Bitmask for pads which are going to be unlocked,
                                  Bit position - PadNumber
                                  Bit value - 0: DoNotUnlock, 1: Unlock

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioUnlockPadCfgForGroupDw (
  IN GPIO_GROUP                Group,
  IN UINT32                    DwNum,
  IN UINT32                    PadsToUnlock
  );

/**
  This procedure will get number of groups

  @param[in] none

  @retval Value               Group number
**/
UINT32
EFIAPI
GpioGetNumberOfGroups (
  VOID
  );

/**
  This procedure is used to check if GpioPad is valid for certain chipset

  @param[in]  GpioPad             GPIO pad

  @retval TRUE                    This pin is valid on this chipset
          FALSE                   Incorrect pin
**/
BOOLEAN
EFIAPI
GpioIsCorrectPadForThisChipset (
  IN  GPIO_PAD        GpioPad
  );


#endif // _GPIO_LIB_H_

