/** @file
  The platform GPIO init library header.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_INIT_LIB_H_
#define _GPIO_INIT_LIB_H_

typedef UINT8          PCH_SBI_PID;

#include <GpioConfig.h>
#include <Library/GpioLib.h>
#include <Library/GpioSiLib.h>
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/PchPcrLib.h>
#include <RegAccess.h>

#define GPIO_PAD_DEF(Group,Pad)               (UINT32)(((Group) << 16) + (Pad))
#define GPIO_GROUP_DEF(Index,ChipsetId)       ((Index) | ((ChipsetId) << 8))
#define GPIO_GET_GROUP_INDEX(Group)           ((Group) & 0xFF)
#define GPIO_GET_GROUP_FROM_PAD(Pad)          ((Pad) >> 16)
#define GPIO_GET_GROUP_INDEX_FROM_PAD(Pad)    GPIO_GET_GROUP_INDEX (((Pad) >> 16))
#define GPIO_GET_PAD_NUMBER(Pad)              ((Pad) & 0xFFFF)
#define GPIO_GET_CHIPSET_ID(Pad)              ((Pad) >> 24)

#define GPIO_GET_PAD_POSITION(PadNumber)      ((PadNumber) % 32)
#define GPIO_GET_DW_NUM(PadNumber)            ((PadNumber) / 32u)

//
// Number of PADCFG_DW registers
//
#define GPIO_PADCFG_DW_REG_NUMBER  4

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
#define B_GPIO_HOSTSW_OWN_MASK                          0x3
#define N_GPIO_HOSTSW_OWN_BIT_POS                       0
#define B_GPIO_DIRECTION_MASK                           0x1F
#define B_GPIO_DIRECTION_DIR_MASK                       0x7
#define N_GPIO_DIRECTION_DIR_BIT_POS                    0
#define B_GPIO_DIRECTION_INV_MASK                       0x18
#define N_GPIO_DIRECTION_INV_BIT_POS                    3
#define B_GPIO_OUTPUT_MASK                              0x3
#define N_GPIO_OUTPUT_BIT_POS                           0
#define N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS            0
#define N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS              5
#define B_GPIO_RESET_CONFIG_RESET_MASK                  0x3F
#define N_GPIO_RESET_CONFIG_OLD_RESET_TYPE              BIT1
#define B_GPIO_RESET_CONFIG_OLD_RESET_MASK              0xF
#define N_GPIO_RESET_CONFIG_RESET_BIT_POS               0
#define B_GPIO_RESET_CONFIG_GPD_RESET_MASK              (BIT5 | BIT4)
#define B_GPIO_RESET_CONFIG_GPP_RESET_MASK              (BIT3 | BIT2)
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

/**
  Generates GPIO group name from GroupIndex

  @param[in] GroupIndex  Gpio GroupIndex

  @retval CHAR8*  Pointer to the GPIO group name
**/
CONST
CHAR8*
GpioGetGroupName (
  IN UINT32  GroupIndex
  );

/**
  This procedure will return Port ID of GPIO Community from GpioPad

  @param[in] GpioPad            GpioPad

  @retval GpioCommunityPortId   Port ID of GPIO Community
**/
UINT8
GpioGetGpioCommunityPortIdFromGpioPad (
  IN GPIO_PAD        GpioPad
  );

/**
  This procedure will return PadCfg address from GpioPad

  @param[in] GpioPad            GpioPad

  @retval GpioPadCfgAddress     PadCfg Address of GpioPad
**/
UINT32
GpioGetGpioPadCfgAddressFromGpioPad (
  IN GPIO_PAD        GpioPad
  );

/**
  This procedure will check if GpioPad argument is valid.
  Function will check below conditions:
   - GpioPad represents a pad for current PCH
   - GpioPad belongs to valid GpioGroup
   - GPIO PadNumber is not greater than number of pads for this group

  @param[in] GpioPad       GPIO pad

  @retval TRUE             GPIO pad is valid and can be used with GPIO lib API
  @retval FALSE            GPIO pad is invalid and cannot be used with GPIO lib API
**/
BOOLEAN
GpioIsPadValid (
  IN GPIO_PAD             GpioPad
  );

/**
  This procedure will check if GpioPad is owned by host.

  @param[in] GpioPad       GPIO pad

  @retval TRUE             GPIO pad is owned by host
  @retval FALSE            GPIO pad is not owned by host and should not be used with GPIO lib API
**/
BOOLEAN
GpioIsPadHostOwned (
  IN GPIO_PAD             GpioPad
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
