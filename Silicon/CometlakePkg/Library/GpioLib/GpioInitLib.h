/** @file
  The platform GPIO init library header.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_INIT_LIB_H_
#define _GPIO_INIT_LIB_H_

typedef UINT8          PCH_SBI_PID;

//
// If in GPIO_GROUP_INFO structure certain register doesn't exist
// it will have value equal to NO_REGISTER_FOR_PROPERTY
//
#define NO_REGISTER_FOR_PROPERTY 0xFFFF

//
// Below defines are based on GPIO_CONFIG structure fields
//
#define B_GPIO_PAD_MODE_MASK                            0xF
#define N_GPIO_PAD_MODE_BIT_POS                         0
#define N_GPIO_HOSTSW_OWN_BIT_POS                       0
#define B_GPIO_DIRECTION_DIR_MASK                       0x7
#define N_GPIO_DIRECTION_DIR_BIT_POS                    0
#define B_GPIO_DIRECTION_INV_MASK                       0x18
#define N_GPIO_DIRECTION_INV_BIT_POS                    3
#define B_GPIO_OUTPUT_MASK                              0x3
#define N_GPIO_OUTPUT_BIT_POS                           0
#define N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS            0
#define N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS              5
#define B_GPIO_RESET_CONFIG_RESET_MASK                  0x3F
#define N_GPIO_RESET_CONFIG_RESET_BIT_POS               0
#define N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS    0
#define N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS               0

//
// Possible registers to be accessed using GpioReadReg()/GpioWriteReg() functions
//
typedef enum {
  GpioHostOwnershipRegister = 0,
  GpioGpeEnableRegister,
  GpioGpeStatusRegister,
  GpioSmiEnableRegister,
  GpioSmiStatusRegister,
  GpioNmiEnableRegister,
  GpioPadConfigLockRegister,
  GpioPadLockOutputRegister
} GPIO_REG;

//
// Structure for storing information about registers offset, community,
// maximal pad number for available groups
//
typedef struct {
  PCH_SBI_PID  Community;
  UINT16       PadOwnOffset;
  UINT16       HostOwnOffset;
  UINT16       GpiIsOffset;
  UINT16       GpiIeOffset;
  UINT16       GpiGpeStsOffset;
  UINT16       GpiGpeEnOffset;
  UINT16       SmiStsOffset;
  UINT16       SmiEnOffset;
  UINT16       NmiStsOffset;
  UINT16       NmiEnOffset;
  UINT16       PadCfgLockOffset;
  UINT16       PadCfgLockTxOffset;
  UINT16       PadCfgOffset;
  UINT16       PadPerGroup;
} GPIO_GROUP_INFO;

/**
  This procedure will retrieve address and length of GPIO info table

  @param[out]  GpioGroupInfoTableLength   Length of GPIO group table

  @retval Pointer to GPIO group table

**/
CONST GPIO_GROUP_INFO*
GpioGetGroupInfoTable (
  OUT UINT32              *GpioGroupInfoTableLength
  );


/**
  This procedure will clear PadCfgLock for selected pads within one group.
  This function should be used only inside SMI.

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
  This procedure will clear PadCfgLockTx for selected pads within one group.
  This function should be used only inside SMI.

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
  This internal procedure will check if group is within DeepSleepWell.

  @param[in]  Group               GPIO Group

  @retval GroupWell               TRUE:  This is DSW Group
                                  FALSE: This is not DSW Group
**/
BOOLEAN
GpioIsDswGroup (
  IN  GPIO_GROUP         Group
  );

/**
  This procedure will read GPIO register

  @param[in] RegType              GPIO register type
  @param[in] Group                GPIO group
  @param[in] DwNum                Register number for current group (parameter applicable in accessing whole register).
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[out] ReadVal             Read data
**/
VOID
GpioReadReg (
  IN GPIO_REG               RegType,
  IN GPIO_GROUP             Group,
  IN UINT32                 DwNum,
  OUT UINT32                *ReadVal
  );

#endif //_GPIO_INIT_LIB_H_
