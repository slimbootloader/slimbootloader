/** @file
  Header file for GPIO Lib implementation.

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _GPIO_LIBRARY_H_
#define _GPIO_LIBRARY_H_

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <PchAccess.h>
#include <Library/SiGpioLib.h>
#include <Library/GpioNativeLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/GpioHelpersLib.h>
#include <Register/GpioRegs.h>
#include <GpioPinsVer2Lp.h>
#include <GpioPinsVer2H.h>
#include <GpioPinsVer2K.h>
#include <Register/GpioRegsVer2.h>
#include <PmcRegsVer2.h>

//
// Structure for native pin data
//
typedef struct {
  GPIO_PAD       Pad;
  GPIO_PAD_MODE  Mode;
} GPIO_PAD_NATIVE_FUNCTION;

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
  UINT16       PmcGpeDwx;
  UINT16       GpioGpeDwx;
} GPIO_GROUP_INFO;

//
// If in GPIO_GROUP_INFO structure certain register doesn't exist
// it will have value equal to NO_REGISTER_FOR_PROPERTY
//
#define NO_REGISTER_FOR_PROPERTY 0xFFFF

#define GPIO_PAD_DEF(Group,Pad)                (UINT32)(((Group) << 16) + (Pad))
#define GPIO_GROUP_DEF(GroupIndex,ChipsetId)   ((GroupIndex) | ((ChipsetId) << 8))
#define GPIO_GET_GROUP_INDEX(Group)            ((Group) & 0x1F)
#define GPIO_GET_GROUP_FROM_PAD(GpioPad)       (((GpioPad) & 0x0F1F0000) >> 16)
#define GPIO_GET_GROUP_INDEX_FROM_PAD(GpioPad) GPIO_GET_GROUP_INDEX (GPIO_GET_GROUP_FROM_PAD(GpioPad))
#define GPIO_GET_PAD_NUMBER(GpioPad)           ((GpioPad) & 0x1FF)
#define GPIO_GET_CHIPSET_ID(GpioPad)           (((GpioPad) >> 24) & 0xF)

#define GPIO_GET_PAD_POSITION(PadNumber)       ((PadNumber) % 32)
#define GPIO_GET_DW_NUM(PadNumber)             ((PadNumber) / 32u)

/**
  This procedure will retrieve address and length of GPIO info table

  @param[out]  GpioGroupInfoTableLength   Length of GPIO group table

  @retval Pointer to GPIO group table
**/
CONST GPIO_GROUP_INFO*
GpioGetGroupInfoTable (
  OUT UINT32              *GpioGroupInfoTableLength
  );

//
// Number of PADCFG_DW registers
//
#define GPIO_PADCFG_DW_REG_NUMBER  4

/**
  Get GPIO Chipset ID specific to PCH generation and series
**/
UINT32
GpioGetThisChipsetId (
  VOID
  );

/**
  This procedure is used to check if GpioPad is valid for certain chipset

  @param[in]  GpioPad             GPIO pad

  @retval TRUE                    This pin is valid on this chipset
          FALSE                   Incorrect pin
**/
BOOLEAN
GpioIsCorrectPadForThisChipset (
  IN  GPIO_PAD        GpioPad
  );

//
// Structure which stores information needed to map GPIO Group
// to 1-Tier GPE. Configuration is needed both in PMC and GPIO IP.
// Because GPE_DWx can handle only 32 pins only single double word can
// be mapped at a time. Each DW for a group has different configuration in PMC and GPIO
//
typedef struct {
  GPIO_GROUP  Group;
  UINT8       GroupDw;
  UINT8       PmcGpeDwxVal;
  UINT8       GpioGpeDwxVal;
} GPIO_GROUP_TO_GPE_MAPPING;

/**
  Get information for GPIO Group required to program GPIO and PMC for desired 1-Tier GPE mapping

  @param[out] GpioGroupToGpeMapping        Table with GPIO Group to GPE mapping
  @param[out] GpioGroupToGpeMappingLength  GPIO Group to GPE mapping table length
**/
VOID
GpioGetGroupToGpeMapping (
  OUT GPIO_GROUP_TO_GPE_MAPPING  **GpioGroupToGpeMapping,
  OUT UINT32                     *GpioGroupToGpeMappingLength
  );

/**
  This function provides GPIO Community PortIDs

  @param[out] NativePinsTable                Table with GPIO COMMx SBI PortIDs

  @retval      Number of communities
**/
UINT32
GpioGetComSbiPortIds (
  OUT PCH_SBI_PID    **GpioComSbiIds
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
  This procedure will set GPIO mode

  @param[in] GpioPad             GPIO pad
  @param[in] PadModeValue        GPIO pad mode value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioSetPadMode (
  IN GPIO_PAD                GpioPad,
  IN GPIO_PAD_MODE           PadModeValue
  );

/**
  This procedure will get GPIO mode

  @param[in]  GpioPad             GPIO pad
  @param[out] PadModeValue        GPIO pad mode value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioGetPadMode (
  IN  GPIO_PAD                 GpioPad,
  OUT GPIO_PAD_MODE            *PadModeValue
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
GpioUnlockOverride (
  IN  GPIO_GROUP  Group,
  IN  UINT32      DwNum,
  OUT UINT32      *UnlockCfgPad,
  OUT UINT32      *UnlockTxPad
  );

/**
  This internal procedure will calculate GPIO_RESET_CONFIG value  (new type)
  based on provided PadRstCfg for a specific GPIO Pad.

  @param[in]  GpioPad               GPIO Pad
  @param[in]  PadRstCfg             GPIO PadRstCfg value

  @retval GpioResetConfig           GPIO Reset configuration (new type)
**/
GPIO_RESET_CONFIG
GpioResetConfigFromPadRstCfg (
  IN  GPIO_PAD           GpioPad,
  IN  UINT32             PadRstCfg
  );

/**
  This internal procedure will calculate PadRstCfg register value based
  on provided GPIO Reset configuration for a certain pad.

  @param[in]  GpioPad                   GPIO Pad
  @param[in]  GpioResetConfig           GPIO Reset configuration
  @param[out] PadRstCfg                 GPIO PadRstCfg value

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid configuration
**/
EFI_STATUS
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
GpioPadCfgRegValueFromGpioConfig (
  IN  GPIO_PAD           GpioPad,
  IN  CONST GPIO_CONFIG  *GpioConfig,
  OUT UINT32             *PadCfgDwReg,
  OUT UINT32             *PadCfgDwRegMask
  );

/**
  Check if 0x13 opcode supported for writing to GPIO lock unlock register

  @retval TRUE                It's supported
  @retval FALSE               It's not supported
**/
BOOLEAN
IsGpioLockOpcodeSupported (
  VOID
  );
#endif // _GPIO_LIBRARY_H_
