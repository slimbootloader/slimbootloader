/** @file
  The platform GPIO library header.

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_LIB_INT_H_
#define _GPIO_LIB_INT_H_

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <GpioConfig.h>

#define  GPIO_DEBUG_INFO     DEBUG_VERBOSE
#define  GPIO_DEBUG_ERROR    DEBUG_VERBOSE

/**
  This procedure is used to clear GPE STS for a specified GpioPad

  @param[in]  GpioPad             GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioClearGpiGpeSts (
  IN GPIO_PAD                  GpioPad
  );

/**
  This procedure is used to check GPIO inputs belongs to 2 tier or 1 tier architecture

  @param[in]  GpioPad             GPIO pad

  @retval     Data                0 means 1-tier, 1 means 2-tier
**/
BOOLEAN
EFIAPI
GpioCheckFor2Tier (
  IN GPIO_PAD                  GpioPad
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
EFIAPI
GpioGetGpeNumber (
  IN GPIO_PAD                   GpioPad,
  OUT UINT32                    *GpeNumber
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
EFIAPI
GpioGetGroupToGpeDwX (
  IN GPIO_GROUP               *GroupToGpeDw0,
  IN GPIO_GROUP               *GroupToGpeDw1,
  IN GPIO_GROUP               *GroupToGpeDw2
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
EFIAPI
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
EFIAPI
GpioUnlockPadCfg (
  IN GPIO_PAD                   GpioPad
  );

/**
  This procedure will clear PadCfgLockTx for selected pad.
  Unlocking a pad will cause an SMI (if enabled)

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
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
EFIAPI
GpioLockPadCfgTxForGroupDw (
  IN GPIO_GROUP                   Group,
  IN UINT32                       DwNum,
  IN UINT32                       PadsToLockTx
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
EFIAPI
GpioLockPadCfgForGroupDw (
  IN GPIO_GROUP                   Group,
  IN UINT32                       DwNum,
  IN UINT32                       PadsToLock
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
EFIAPI
GpioGetPadCfgLockTx (
  IN GPIO_PAD                   GpioPad,
  OUT UINT32                    *PadCfgLockTx
  );

/**
  This procedure will get GPIO IOxAPIC interrupt number

  @param[in]  GpioPad             GPIO pad
  @param[out] IrqNum              IRQ number

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioGetPadIoApicIrqNumber (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32                   *IrqNum
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
EFIAPI
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
EFIAPI
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
EFIAPI
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
EFIAPI
GpioSetHostSwOwnershipForPad (
  IN GPIO_PAD                  GpioPad,
  IN UINT32                    PadHostSwOwn
  );

/**
  This procedure will get number of pads for certain GPIO group

  @param[in] Group            GPIO group number

  @retval Value               Pad number for group
                              If illegal group number then return 0
**/
UINT32
EFIAPI
GpioGetPadPerGroup (
  IN GPIO_GROUP        Group
  );


/**
  This procedure will get lowest group

  @param[in] none

  @retval Value               Lowest Group
**/
GPIO_GROUP
EFIAPI
GpioGetLowestGroup (
  VOID
  );


/**
  This procedure will get highest group

  @param[in] none

  @retval Value               Highest Group
**/
GPIO_GROUP
EFIAPI
GpioGetHighestGroup (
  VOID
  );

/**
  This procedure will get GPIO group data with pads, which PadConfig is supposed to be left unlock

  @param[in]  GroupIndex          GPIO group index
  @param[in]  DwNum               DWORD index for a group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @retval     UnlockedPads        DWORD bitmask for pads which are going to be left unlocked
                                  Bit position - PadNumber
                                  Bit value - 0: to be locked, 1: Leave unlocked
**/
UINT32
EFIAPI
GpioGetGroupDwUnlockPadConfigMask (
  IN UINT32                       GroupIndex,
  IN UINT32                       DwNum
  );


/**
  This procedure will get GPIO group data with pads, which Output is supposed to be left unlock

  @param[in]  GroupIndex          GPIO group index
  @param[in]  DwNum               DWORD index for a group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @retval     UnlockedPads        DWORD bitmask for pads which are going to be left unlocked
                                  Bit position - PadNumber
                                  Bit value - 0: to be locked, 1: Leave unlocked
**/
UINT32
EFIAPI
GpioGetGroupDwUnlockOutputMask (
  IN UINT32                       GroupIndex,
  IN UINT32                       DwNum
  );

/**
  This procedure stores GPIO group data about pads which PadConfig needs to be unlocked.

  @param[in]  GroupIndex          GPIO group index
  @param[in]  DwNum               DWORD index for a group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToLock          DWORD bitmask for pads which are going to be left unlocked
                                  Bit position - PadNumber
                                  Bit value - 0: Skip, 1: Leave unlocked

  @retval Status
**/
EFI_STATUS
EFIAPI
GpioStoreGroupDwUnlockPadConfigData (
  IN UINT32                       GroupIndex,
  IN UINT32                       DwNum,
  IN UINT32                       UnlockedPads
  );

/**
  This procedure stores GPIO group data about pads which Output state needs to be unlocked.

  @param[in]  GroupIndex          GPIO group index
  @param[in]  DwNum               DWORD index for a group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToLock          DWORD bitmask for pads which are going to be left unlocked
                                  Bit position - PadNumber
                                  Bit value - 0: Skip, 1: Leave unlocked

  @retval Status
**/
EFI_STATUS
EFIAPI
GpioStoreGroupDwUnlockOutputData (
  IN UINT32                       GroupIndex,
  IN UINT32                       DwNum,
  IN UINT32                       UnlockedPads
  );

/**
  This function is to be used In GpioLockPads() to override a lock request by SOC code.

  @param[in]  Group          GPIO group
  @param[in]  DwNum          Register number for current group (parameter applicable in accessing whole register).
                             For group which has less then 32 pads per group DwNum must be 0.
  @param[out] *UnlockCfgPad  DWORD bitmask for pads which are going to be left unlocked
                             Bit position - PadNumber
                             Bit value - 0: to be locked, 1: Leave unlocked
  @param[out] *UnlockTxPad   DWORD bitmask for pads which are going to be left unlocked
                             Bit position - PadNumber
                             Bit value - 0: to be locked, 1: Leave unlocked
  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid input parameter
**/
EFI_STATUS
EFIAPI
GpioUnlockOverride (
  IN  GPIO_GROUP  Group,
  IN  UINT32      DwNum,
  OUT UINT32      *UnlockCfgPad,
  OUT UINT32      *UnlockTxPad
  );


/**
  This procedure will set PadCfgLock for selected pad

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioLockPadCfg (
  IN GPIO_PAD                   GpioPad
  );


/**
  This procedure will set PadCfgLockTx for selected pad

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioLockPadCfgTx (
  IN GPIO_PAD                   GpioPad
  );

#endif // _GPIO_LIB_INT_H_
