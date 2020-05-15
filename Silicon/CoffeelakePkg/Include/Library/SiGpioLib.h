/** @file
  Header file for GpioLib.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GPIO_LIB_H_
#define _GPIO_LIB_H_

#include <GpioConfig.h>

#define GPIO_NAME_LENGTH_MAX  32

typedef struct {
  GPIO_PAD           GpioPad;
  GPIO_CONFIG        GpioConfig;
} GPIO_INIT_CONFIG;

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
GpioConfigurePads (
  IN UINT32                    NumberOfItems,
  IN GPIO_INIT_CONFIG          *GpioInitTableAddress
  );

//
// Functions for setting/getting multiple GpioPad settings
//

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

/**
  This procedure will configure multiple GPIO settings

  @param[in] GpioPad                    GPIO Pad
  @param[in] GpioData                   GPIO data structure

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid GpioPad
**/
EFI_STATUS
GpioSetPadConfig (
  IN GPIO_PAD                  GpioPad,
  IN GPIO_CONFIG               *GpioData
  );

//
// Functions for setting/getting single GpioPad properties
//

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
  This procedure will get GPIO IOxAPIC interrupt number

  @param[in]  GpioPad             GPIO pad
  @param[out] IrqNum              IRQ number

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetPadIoApicIrqNumber (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32                   *IrqNum
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
  This procedure will get GPIO Host Software Pad Ownership for certain group

  @param[in]  Group               GPIO group
  @param[in]  DwNum               Host Ownership register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[out] HostSwRegVal        Value of Host Software Pad Ownership register
                                  Bit position - PadNumber
                                  Bit value - 0: ACPI Mode, 1: GPIO Driver mode

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or DwNum parameter number
**/
EFI_STATUS
GpioGetHostSwOwnershipForGroupDw (
  IN  GPIO_GROUP                  Group,
  IN  UINT32                      DwNum,
  OUT UINT32                      *HostSwRegVal
  );

/**
  This procedure will get GPIO Host Software Pad Ownership for certain group

  @param[in]  Group               GPIO group
  @param[in]  DwNum               Host Ownership register number for current group
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  HostSwRegVal        Value of Host Software Pad Ownership register
                                  Bit position - PadNumber
                                  Bit value - 0: ACPI Mode, 1: GPIO Driver mode

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or DwNum parameter number
**/
EFI_STATUS
GpioSetHostSwOwnershipForGroupDw (
  IN GPIO_GROUP                   Group,
  IN UINT32                       DwNum,
  IN UINT32                       HostSwRegVal
  );

/**
  This procedure will get Gpio Pad Host Software Ownership

  @param[in] GpioPad              GPIO pad
  @param[out] PadHostSwOwn        Value of Host Software Pad Owner
                                  0: ACPI Mode, 1: GPIO Driver mode

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetHostSwOwnershipForPad (
  IN GPIO_PAD                 GpioPad,
  OUT UINT32                  *PadHostSwOwn
  );

/**
  This procedure will set Gpio Pad Host Software Ownership

  @param[in] GpioPad              GPIO pad
  @param[in]  PadHostSwOwn        Pad Host Software Owner
                                  0: ACPI Mode, 1: GPIO Driver mode

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioSetHostSwOwnershipForPad (
  IN GPIO_PAD                  GpioPad,
  IN UINT32                    PadHostSwOwn
  );

///
/// Possible values of Pad Ownership
/// If Pad is not under Host ownership then GPIO registers
/// are not accessible by host (e.g. BIOS) and reading them
/// will return 0xFFs.
///
typedef enum {
  GpioPadOwnHost = 0x0,
  GpioPadOwnCsme = 0x1,
  GpioPadOwnIsh  = 0x2,
} GPIO_PAD_OWN;

/**
  This procedure will get Gpio Pad Ownership

  @param[in] GpioPad              GPIO pad
  @param[out] PadOwnVal           Value of Pad Ownership

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetPadOwnership (
  IN  GPIO_PAD                GpioPad,
  OUT GPIO_PAD_OWN            *PadOwnVal
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
GpioGetPadCfgLockForGroupDw (
  IN  GPIO_GROUP                  Group,
  IN  UINT32                      DwNum,
  OUT UINT32                      *PadCfgLockRegVal
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
GpioGetPadCfgLockTxForGroupDw (
  IN  GPIO_GROUP                  Group,
  IN  UINT32                      DwNum,
  OUT UINT32                      *PadCfgLockTxRegVal
  );

/**
  This procedure will check state of Pad Config Tx Lock for selected pad

  @param[in]  GpioPad             GPIO pad
  @param[out] PadCfgLock          PadCfgLockTx for selected pad
                                  0: NotLockedTx, 1: LockedTx

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetPadCfgLockTx (
  IN GPIO_PAD                   GpioPad,
  OUT UINT32                    *PadCfgLockTx
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
GpioUnlockPadCfgForGroupDw (
  IN GPIO_GROUP                Group,
  IN UINT32                    DwNum,
  IN UINT32                    PadsToUnlock
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
  This procedure will set PadCfgLock for selected pads within one group

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLock register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToLock          Bitmask for pads which are going to be locked,
                                  Bit position - PadNumber
                                  Bit value - 0: DoNotLock, 1: Lock

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or DwNum parameter number
**/
EFI_STATUS
GpioLockPadCfgForGroupDw (
  IN GPIO_GROUP                   Group,
  IN UINT32                       DwNum,
  IN UINT32                       PadsToLock
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
GpioUnlockPadCfgTxForGroupDw (
  IN GPIO_GROUP                Group,
  IN UINT32                    DwNum,
  IN UINT32                    PadsToUnlockTx
  );

/**
  This procedure will clear PadCfgLockTx for selected pad.
  Unlocking a pad will cause an SMI (if enabled)

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioUnlockPadCfgTx (
  IN GPIO_PAD                   GpioPad
  );

/**
  This procedure will set PadCfgLockTx for selected pads within one group

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLock register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToLockTx        Bitmask for pads which are going to be locked,
                                  Bit position - PadNumber
                                  Bit value - 0: DoNotLockTx, 1: LockTx

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or DwNum parameter number
**/
EFI_STATUS
GpioLockPadCfgTxForGroupDw (
  IN GPIO_GROUP                   Group,
  IN UINT32                       DwNum,
  IN UINT32                       PadsToLockTx
  );

/**
  This procedure will set PadCfgLockTx for selected pad

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioLockPadCfgTx (
  IN GPIO_PAD                   GpioPad
  );

/**
  This procedure will get Group to GPE mapping.
  It will assume that only first 32 pads can be mapped to GPE.
  To handle cases where groups have more than 32 pads and higher part of group
  can be mapped please refer to GpioGetGroupDwToGpeDwX()

  @param[out] GroupToGpeDw0       GPIO group to be mapped to GPE_DW0
  @param[out] GroupToGpeDw1       GPIO group to be mapped to GPE_DW1
  @param[out] GroupToGpeDw2       GPIO group to be mapped to GPE_DW2

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioGetGroupToGpeDwX (
  IN GPIO_GROUP               *GroupToGpeDw0,
  IN GPIO_GROUP               *GroupToGpeDw1,
  IN GPIO_GROUP               *GroupToGpeDw2
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
  This procedure will set Group to GPE mapping.
  It will assume that only first 32 pads can be mapped to GPE.
  To handle cases where groups have more than 32 pads and higher part of group
  can be mapped please refer to GpioSetGroupDwToGpeDwX()

  @param[in]  GroupToGpeDw0       GPIO group to be mapped to GPE_DW0
  @param[in]  GroupToGpeDw1       GPIO group to be mapped to GPE_DW1
  @param[in]  GroupToGpeDw2       GPIO group to be mapped to GPE_DW2

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioSetGroupToGpeDwX (
  IN GPIO_GROUP                GroupToGpeDw0,
  IN GPIO_GROUP                GroupToGpeDw1,
  IN GPIO_GROUP                GroupToGpeDw2
  );

/**
  This procedure will set Group to GPE mapping. If group has more than 32 bits
  it is possible to map only single DW of pins (e.g. 0-31, 32-63) because
  ACPI GPE_DWx register is 32 bits large.

  @param[in]  GroupToGpeDw0       GPIO group to be mapped to GPE_DW0
  @param[in]  GroupDwForGpeDw0    DW of pins to be mapped to GPE_DW0
  @param[in]  GroupToGpeDw1       GPIO group to be mapped to GPE_DW1
  @param[in]  GroupDwForGpeDw1    DW of pins to be mapped to GPE_DW1
  @param[in]  GroupToGpeDw2       GPIO group to be mapped to GPE_DW2
  @param[in]  GroupDwForGpeDw2    DW of pins to be mapped to GPE_DW2

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioSetGroupDwToGpeDwX (
  IN GPIO_GROUP                GroupToGpeDw0,
  IN UINT32                    GroupDwForGpeDw0,
  IN GPIO_GROUP                GroupToGpeDw1,
  IN UINT32                    GroupDwForGpeDw1,
  IN GPIO_GROUP                GroupToGpeDw2,
  IN UINT32                    GroupDwForGpeDw2
  );

/**
  This procedure will get GPE number for provided GpioPad.
  PCH allows to configure mapping between GPIO groups and related GPE (GpioSetGroupToGpeDwX())
  what results in the fact that certain Pad can cause different General Purpose Event. Only three
  GPIO groups can be mapped to cause unique GPE (1-tier), all others groups will be under one common
  event (GPE_111 for 2-tier).

  1-tier:
  Returned GpeNumber is in range <0,95>. GpioGetGpeNumber() can be used
  to determine what _LXX ACPI method would be called on event on selected GPIO pad

  2-tier:
  Returned GpeNumber is 0x6F (111). All GPIO pads which are not mapped to 1-tier GPE
  will be under one master GPE_111 which is linked to _L6F ACPI method. If it is needed to determine
  what Pad from 2-tier has caused the event, _L6F method should check GPI_GPE_STS and GPI_GPE_EN
  registers for all GPIO groups not mapped to 1-tier GPE.

  @param[in]  GpioPad             GPIO pad
  @param[out] GpeNumber           GPE number

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetGpeNumber (
  IN GPIO_PAD                   GpioPad,
  OUT UINT32                    *GpeNumber
  );

/**
  This procedure is used to clear SMI STS for a specified Pad

  @param[in]  GpioPad             GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioClearGpiSmiSts (
  IN GPIO_PAD                   GpioPad
  );

/**
  This procedure is used by Smi Dispatcher and will clear
  all GPI SMI Status bits

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
GpioClearAllGpiSmiSts (
  VOID
  );

/**
  This procedure is used to disable all GPI SMI

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
GpioDisableAllGpiSmi (
  VOID
  );

/**
  This procedure is used to register GPI SMI dispatch function.

  @param[in]  GpioPad             GPIO pad
  @param[out] GpiNum              GPI number

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetGpiSmiNum (
  IN GPIO_PAD          GpioPad,
  OUT UINTN            *GpiNum
  );

/**
  This procedure is used to check GPIO inputs belongs to 2 tier or 1 tier architecture

  @param[in]  GpioPad             GPIO pad

  @retval     Data                0 means 1-tier, 1 means 2-tier
**/
BOOLEAN
GpioCheckFor2Tier (
  IN GPIO_PAD                  GpioPad
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
  This procedure is used to read GPE STS for a specified Pad

  @param[in]  GpioPad             GPIO pad
  @param[out] Data                GPE STS data

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetGpiGpeSts (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32*                  Data
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
GpioLockPads (
  VOID
  );

/**
  Generates GPIO name from GpioPad

  @param[in]  GpioPad             GpioPad
  @param[out] GpioNameBuffer      Caller allocated buffer for GPIO name of GPIO_NAME_LENGTH_MAX size
  @param[in]  GpioNameBufferSize  Size of the buffer

  @retval CHAR8*  Pointer to the GPIO name
**/
CHAR8*
GpioGetPadName (
  IN  GPIO_PAD  GpioPad,
  OUT CHAR8*    GpioNameBuffer,
  IN  UINT32    GpioNameBufferSize
  );

#endif // _GPIO_LIB_H_
