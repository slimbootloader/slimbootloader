/** @file
  This file contains routines for GPIO

  Copyright (c) 2013 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "GpioLibrary.h"

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
  )
{
  UINT32                 PadCfgReg;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Create Pad Configuration register offset
  //
  PadCfgReg = GpioGroupInfo[GroupIndex].PadCfgOffset + S_GPIO_PCR_PADCFG * PadNumber + 0x4 * DwReg;

  return MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, PadCfgReg));
}

/**
  This procedure will get GPIO input level

  @param[in] GpioPad              GPIO pad
  @param[out] InputVal            GPIO Input value
                                  0: InputLow, 1: InpuHigh

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetInputValue (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32                   *InputVal
  )
{
  UINT32      PadCfgReg;

  PadCfgReg = GpioReadPadCfgReg (GpioPad, 0);

  *InputVal = (PadCfgReg & B_GPIO_PCR_RX_STATE) >> N_GPIO_PCR_RX_STATE;

  return EFI_SUCCESS;
}


/**
  This procedure will check if GpioGroup argument is correct and
  supplied DW reg number can be used for this group to access DW registers.
  Function will check below conditions:
   - Valid GpioGroup
   - DwNum is has valid value for this group

  @param[in] Group        GPIO group
  @param[in] DwNum        Register number for current group (parameter applicable in accessing whole register).
                          For group which has less then 32 pads per group DwNum must be 0.

  @retval TRUE             DW Reg number and GpioGroup is valid
  @retval FALSE            DW Reg number and GpioGroup is invalid
**/
STATIC
BOOLEAN
GpioIsGroupAndDwNumValid (
  IN GPIO_GROUP             Group,
  IN UINT32                 DwNum
  )
{
  UINT32                 GroupIndex;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  GroupIndex = GpioGetGroupIndexFromGroup (Group);

  if ((Group < GpioGetLowestGroup ()) || (Group > GpioGetHighestGroup ()) || (GroupIndex >= GpioGroupInfoLength)) {
    DEBUG ((DEBUG_ERROR, "GPIO ERROR: Group argument (%d) is not within range of possible groups for this PCH\n", GroupIndex));
    goto Error;
  }

  //
  // Check if DwNum argument does not exceed number of DWord registers
  // resulting from available pads for certain group
  //
  if (DwNum > GPIO_GET_DW_NUM (GpioGroupInfo[GroupIndex].PadPerGroup - 1)){
    goto Error;
  }

  return TRUE;
Error:
  ASSERT (FALSE);
  return FALSE;
}

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
  GpioPadLockOutputRegister,
  GpioGpiIntStatusRegister
} GPIO_REG;

/**
  This procedure will read GPIO register

  @param[in] RegType              GPIO register type
  @param[in] Group                GPIO group
  @param[in] DwNum                Register number for current group (parameter applicable in accessing whole register).
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[out] ReadVal             Read data

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_UNSUPPORTED         Feature is not supported for this group or pad
**/
STATIC
EFI_STATUS
GpioReadReg (
  IN GPIO_REG               RegType,
  IN GPIO_GROUP             Group,
  IN UINT32                 DwNum,
  OUT UINT32                *ReadVal
  )
{
  UINT32                 RegOffset;
  UINT32                 GroupIndex;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;

  RegOffset = NO_REGISTER_FOR_PROPERTY;
  GroupIndex = GpioGetGroupIndexFromGroup (Group);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  switch (RegType) {
    case GpioHostOwnershipRegister:
      RegOffset = GpioGroupInfo[GroupIndex].HostOwnOffset;
      break;
    case GpioGpeEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].GpiGpeEnOffset;
      break;
    case GpioGpeStatusRegister:
      RegOffset = GpioGroupInfo[GroupIndex].GpiGpeStsOffset;
      break;
    case GpioSmiEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].SmiEnOffset;
      break;
    case GpioSmiStatusRegister:
      RegOffset = GpioGroupInfo[GroupIndex].SmiStsOffset;
      break;
    case GpioNmiEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].NmiEnOffset;
      break;
    case GpioPadConfigLockRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockOffset;
      break;
    case GpioPadLockOutputRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockTxOffset;
      break;
    case GpioGpiIntStatusRegister:
      RegOffset = GpioGroupInfo[GroupIndex].GpiIsOffset;
      break;
    default:
      break;
  }

  //
  // Check if selected register exists
  //
  if (RegOffset == NO_REGISTER_FOR_PROPERTY) {
    return EFI_UNSUPPORTED;
  }

  //
  // If there are more then 32 pads per group then certain
  // group information would be split into more then one DWord register.
  //
  if ((RegType == GpioPadConfigLockRegister) || (RegType == GpioPadLockOutputRegister)) {
    //
    // PadConfigLock and OutputLock registers when used for group containing more than 32 pads
    // are not placed in a continuous way, e.g:
    // 0x0 - PadConfigLock_DW0
    // 0x4 - OutputLock_DW0
    // 0x8 - PadConfigLock_DW1
    // 0xC - OutputLock_DW1
    //
    RegOffset += DwNum * 0x8;
  } else {
    RegOffset += DwNum * 0x4;
  }

  *ReadVal = MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, RegOffset));

  return EFI_SUCCESS;
}


/**
  This procedure will write GPIO Lock/LockTx register using SBI.

  @param[in] RegType              GPIO register (Lock or LockTx)
  @param[in] Group                GPIO group number
  @param[in] DwNum                Register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in] LockRegAndMask       Mask which will be AND'ed with Lock register value
  @param[in] LockRegOrMask        Mask which will be Or'ed with Lock register value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_UNSUPPORTED         Feature is not supported for this group or pad
**/
STATIC
EFI_STATUS
GpioWriteLockReg (
  IN GPIO_REG                  RegType,
  IN GPIO_GROUP                Group,
  IN UINT32                    DwNum,
  IN UINT32                    LockRegAndMask,
  IN UINT32                    LockRegOrMask
  )
{
  UINT8                  Response;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 RegOffset;
  UINT32                 OldLockVal;
  UINT32                 NewLockVal;
  UINT32                 GroupIndex;
  EFI_STATUS             Status;
  PCH_SBI_OPCODE         Opcode;

  OldLockVal = 0;
  NewLockVal = 0;

  RegOffset = NO_REGISTER_FOR_PROPERTY;
  GroupIndex = GpioGetGroupIndexFromGroup (Group);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  switch (RegType) {
    case GpioPadConfigLockRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockOffset;
      GpioGetPadCfgLockForGroupDw (Group, DwNum, &OldLockVal);
      break;
    case GpioPadLockOutputRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockTxOffset;
      GpioGetPadCfgLockTxForGroupDw (Group, DwNum, &OldLockVal);
      break;
    default:
      break;
  }

  //
  // Check if selected register exists
  //
  if (RegOffset == NO_REGISTER_FOR_PROPERTY) {
    return EFI_UNSUPPORTED;
  }

  //
  // If there are more then 32 pads per group then certain
  // group information would be split into more then one DWord register.
  // PadConfigLock and OutputLock registers when used for group containing more than 32 pads
  // are not placed in a continuous way, e.g:
  // 0x0 - PadConfigLock_DW0
  // 0x4 - OutputLock_DW0
  // 0x8 - PadConfigLock_DW1
  // 0xC - OutputLock_DW1
  //
  RegOffset += DwNum *0x8;

  NewLockVal = (OldLockVal & LockRegAndMask) | LockRegOrMask;

  if (IsGpioLockOpcodeSupported ()) {
    Opcode = GpioLockUnlock;
  } else {
    Opcode = PrivateControlWrite;
  }

  Status = PchSbiExecutionEx (
             GpioGroupInfo[GroupIndex].Community,
             RegOffset,
             Opcode,
             FALSE,
             0x000F,
             0x0000,
             0x0000,
             &NewLockVal,
             &Response
             );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

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
  )
{
  GPIO_GROUP           Group;

  Group = GpioGetGroupFromGpioPad (GpioPad);

  switch (GpioResetConfig) {
    case GpioResetDefault:
      *PadRstCfg = 0x0;
      break;
    case GpioHostDeepReset:
      *PadRstCfg = V_GPIO_PCR_RST_CONF_DEEP_RST;
      break;
    case GpioPlatformReset:
      *PadRstCfg = V_GPIO_PCR_RST_CONF_GPIO_RST;
      break;
    case GpioResumeReset:
      if (GpioIsDswGroup (Group)) {
        *PadRstCfg = V_GPIO_PCR_RST_CONF_RESUME_RST;
      } else {
        *PadRstCfg = V_GPIO_PCR_RST_CONF_POW_GOOD;
      }
      break;
    case GpioDswReset:
      if (GpioIsDswGroup (Group)) {
        *PadRstCfg = V_GPIO_PCR_RST_CONF_POW_GOOD;
      } else {
        DEBUG ((DEBUG_ERROR, "GPIO ERROR\n"));
        goto Error;
      }
      break;
    default:
      goto Error;
  }

  return EFI_SUCCESS;
Error:
  ASSERT (FALSE);
  return EFI_INVALID_PARAMETER;
}


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
  )
{
  UINT32               PadRstCfg;

  //
  // Configure Reset Type (PadRstCfg)
  // Reset configuration depends on group type.
  // This field requires support for new and deprecated settings.
  //
  GpioPadRstCfgFromResetConfig (
    GpioPad,
    GpioConfig->PowerConfig,
    &PadRstCfg
    );

  PadCfgDwRegMask[0] |= ((((GpioConfig->PowerConfig & B_GPIO_RESET_CONFIG_RESET_MASK) >> N_GPIO_RESET_CONFIG_RESET_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_RST_CONF);
  PadCfgDwReg[0] |= PadRstCfg << N_GPIO_PCR_RST_CONF;

  //
  // Configure how interrupt is triggered (RxEvCfg)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->InterruptConfig & B_GPIO_INT_CONFIG_INT_TYPE_MASK) >> N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_RX_LVL_EDG);
  PadCfgDwReg[0] |= (((GpioConfig->InterruptConfig & B_GPIO_INT_CONFIG_INT_TYPE_MASK) >> (N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS + 1)) << N_GPIO_PCR_RX_LVL_EDG);

  //
  // Configure interrupt generation (GPIRoutIOxAPIC/SCI/SMI/NMI)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->InterruptConfig & B_GPIO_INT_CONFIG_INT_SOURCE_MASK) >> N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS) == GpioHardwareDefault)  ? 0x0 : (B_GPIO_PCR_RX_NMI_ROUTE | B_GPIO_PCR_RX_SCI_ROUTE | B_GPIO_PCR_RX_SMI_ROUTE | B_GPIO_PCR_RX_APIC_ROUTE));
  PadCfgDwReg[0] |= (((GpioConfig->InterruptConfig & B_GPIO_INT_CONFIG_INT_SOURCE_MASK) >> (N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS + 1)) << N_GPIO_PCR_RX_NMI_ROUTE);

  //
  // Configure GPIO direction (GPIORxDis and GPIOTxDis)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->Direction & B_GPIO_DIRECTION_DIR_MASK) >> N_GPIO_DIRECTION_DIR_BIT_POS) == GpioHardwareDefault) ? 0x0 : (B_GPIO_PCR_RXDIS | B_GPIO_PCR_TXDIS));
  PadCfgDwReg[0] |= (((GpioConfig->Direction & B_GPIO_DIRECTION_DIR_MASK) >> (N_GPIO_DIRECTION_DIR_BIT_POS + 1)) << N_GPIO_PCR_TXDIS);

  //
  // Configure GPIO input inversion (RXINV)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->Direction & B_GPIO_DIRECTION_INV_MASK) >> N_GPIO_DIRECTION_INV_BIT_POS) == GpioHardwareDefault) ?  0x0 : B_GPIO_PCR_RXINV);
  PadCfgDwReg[0] |= (((GpioConfig->Direction & B_GPIO_DIRECTION_INV_MASK) >> (N_GPIO_DIRECTION_INV_BIT_POS + 1)) << N_GPIO_PCR_RXINV);

  //
  // Configure GPIO output state (GPIOTxState)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->OutputState & B_GPIO_OUTPUT_MASK) >> N_GPIO_OUTPUT_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_TX_STATE);
  PadCfgDwReg[0] |= (((GpioConfig->OutputState & B_GPIO_OUTPUT_MASK) >> (N_GPIO_OUTPUT_BIT_POS + 1)) << N_GPIO_PCR_TX_STATE);

  //
  // Configure GPIO RX raw override to '1' (RXRAW1)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->OtherSettings & B_GPIO_OTHER_CONFIG_RXRAW_MASK) >> N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_RX_RAW1);
  PadCfgDwReg[0] |= (((GpioConfig->OtherSettings & B_GPIO_OTHER_CONFIG_RXRAW_MASK) >> (N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS + 1)) << N_GPIO_PCR_RX_RAW1);

  //
  // Configure GPIO Pad Mode (PMode)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->PadMode & B_GPIO_PAD_MODE_MASK) >> N_GPIO_PAD_MODE_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_PAD_MODE);
  PadCfgDwReg[0] |= (((GpioConfig->PadMode & B_GPIO_PAD_MODE_MASK) >> (N_GPIO_PAD_MODE_BIT_POS + 1)) << N_GPIO_PCR_PAD_MODE);

  //
  // Configure GPIO termination (Term)
  //
  PadCfgDwRegMask[1] |= ((((GpioConfig->ElectricalConfig & B_GPIO_ELECTRICAL_CONFIG_TERMINATION_MASK) >> N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_TERM);
  PadCfgDwReg[1] |= (((GpioConfig->ElectricalConfig & B_GPIO_ELECTRICAL_CONFIG_TERMINATION_MASK) >> (N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS + 1)) << N_GPIO_PCR_TERM);

  return EFI_SUCCESS;
}

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
  )
{
  if (!GpioIsGroupAndDwNumValid (Group, DwNum)) {
    return EFI_INVALID_PARAMETER;
  }

  return GpioReadReg (
           GpioPadConfigLockRegister,
           Group,
           DwNum,
           PadCfgLockRegVal
           );
}

EFI_STATUS
GpioGetPadCfgLockTxForGroupDw (
  IN  GPIO_GROUP                  Group,
  IN  UINT32                      DwNum,
  OUT UINT32                      *PadCfgLockTxRegVal
  )
{
  if (!GpioIsGroupAndDwNumValid (Group, DwNum)) {
    return EFI_INVALID_PARAMETER;
  }

  return GpioReadReg (
           GpioPadLockOutputRegister,
           Group,
           DwNum,
           PadCfgLockTxRegVal
           );
}

EFI_STATUS
GpioUnlockPadCfgForGroupDw (
  IN GPIO_GROUP                Group,
  IN UINT32                    DwNum,
  IN UINT32                    PadsToUnlock
  )
{
  if (!GpioIsGroupAndDwNumValid (Group, DwNum)) {
    return EFI_INVALID_PARAMETER;
  }

  return GpioWriteLockReg (
           GpioPadConfigLockRegister,
           Group,
           DwNum,
           ~PadsToUnlock,
           0
  );
}

/**
  This procedure will set PadCfgLock for selected pads within one group

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLock register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToLock          Bitmask for pads which are going to be locked
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
  )
{
  if (!GpioIsGroupAndDwNumValid (Group, DwNum)) {
    return EFI_INVALID_PARAMETER;
  }

  return GpioWriteLockReg (
           GpioPadConfigLockRegister,
           Group,
           DwNum,
           ~0u,
           PadsToLock
           );
}
/**
  This procedure will set PadCfgLock for selected pad

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioLockPadCfg (
  IN GPIO_PAD                   GpioPad
  )
{
  GPIO_GROUP   Group;
  UINT32       PadNumber;

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  return GpioLockPadCfgForGroupDw (
           Group,
           GPIO_GET_DW_NUM (PadNumber),
           1 << GPIO_GET_PAD_POSITION (PadNumber)
           );
}

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
  )
{
  if (!GpioIsGroupAndDwNumValid (Group, DwNum)) {
    return EFI_INVALID_PARAMETER;
  }

  return GpioWriteLockReg (
           GpioPadLockOutputRegister,
           Group,
           DwNum,
           ~PadsToUnlockTx,
           0
           );
}

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
  )
{
  if (!GpioIsGroupAndDwNumValid (Group, DwNum)) {
    return EFI_INVALID_PARAMETER;
  }

  return GpioWriteLockReg (
           GpioPadLockOutputRegister,
           Group,
           DwNum,
           ~0u,
           PadsToLockTx
           );
}
/**
  This procedure will set PadCfgLockTx for selected pad

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioLockPadCfgTx (
  IN GPIO_PAD                   GpioPad
  )
{
  GPIO_GROUP   Group;
  UINT32       PadNumber;

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  return GpioLockPadCfgTxForGroupDw (
           Group,
           GPIO_GET_DW_NUM (PadNumber),
           1 << GPIO_GET_PAD_POSITION (PadNumber)
           );
}

/**
  This function gets Group to GPE0 configuration

  @param[out] GpeDw0Value       GPIO Group to GPE_DW0 assignment
  @param[out] GpeDw1Value       GPIO Group to GPE_DW1 assignment
  @param[out] GpeDw2Value       GPIO Group to GPE_DW2 assignment
**/
VOID
PmcGetGpioGpe (
  OUT UINT32    *GpeDw0Value,
  OUT UINT32    *GpeDw1Value,
  OUT UINT32    *GpeDw2Value
  )
{
  UINT32 Data32;

  Data32 = MmioRead32 ((UINTN) (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GPIO_CFG));

  *GpeDw0Value = ((Data32 & B_PMC_PWRM_GPIO_CFG_GPE0_DW0) >> N_PMC_PWRM_GPIO_CFG_GPE0_DW0);
  *GpeDw1Value = ((Data32 & B_PMC_PWRM_GPIO_CFG_GPE0_DW1) >> N_PMC_PWRM_GPIO_CFG_GPE0_DW1);
  *GpeDw2Value = ((Data32 & B_PMC_PWRM_GPIO_CFG_GPE0_DW2) >> N_PMC_PWRM_GPIO_CFG_GPE0_DW2);
}


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
  )
{
  UINT32                 PmcGpeDwXValue[3];
  GPIO_GROUP             GroupToGpeDwX[3];
  UINT32                     GroupDwForGpeDwX[3];
  UINT8                  GpeDwXIndex;
  UINT32                     Index;
  GPIO_GROUP_TO_GPE_MAPPING  *GpioGpeMap;
  UINT32                     GpioGpeMapLength;

  ZeroMem (GroupToGpeDwX, sizeof (GroupToGpeDwX));
  ZeroMem (GroupDwForGpeDwX, sizeof (GroupDwForGpeDwX));

  PmcGetGpioGpe (&PmcGpeDwXValue[0], &PmcGpeDwXValue[1], &PmcGpeDwXValue[2]);
  GpioGetGroupToGpeMapping (&GpioGpeMap, &GpioGpeMapLength);
  for (GpeDwXIndex = 0; GpeDwXIndex < 3; GpeDwXIndex++) {
    for (Index = 0; Index < GpioGpeMapLength; Index++) {

      if (GpioGpeMap[Index].PmcGpeDwxVal == PmcGpeDwXValue[GpeDwXIndex]) {
        GroupToGpeDwX[GpeDwXIndex] = GpioGpeMap[Index].Group;
        GroupDwForGpeDwX[GpeDwXIndex] = GpioGpeMap[Index].GroupDw;
        break;
      }
    }
  }

  if ((GroupToGpeDwX[0] == 0) ||
      (GroupToGpeDwX[1] == 0) ||
      (GroupToGpeDwX[2] == 0)) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  *GroupToGpeDw0 = GroupToGpeDwX[0];
  *GroupDwForGpeDw0 = GroupDwForGpeDwX[0];
  *GroupToGpeDw1 = GroupToGpeDwX[1];
  *GroupDwForGpeDw1 = GroupDwForGpeDwX[1];
  *GroupToGpeDw2 = GroupToGpeDwX[2];
  *GroupDwForGpeDw2 = GroupDwForGpeDwX[2];

  return EFI_SUCCESS;
}
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
  )
{
  UINT32         DwNum;
  GPIO_GROUP     Group;
  GPIO_GROUP     GroupMin;
  GPIO_GROUP     GroupMax;
  UINT32         UnlockedPads;
  UINT32         OverrideCfgPads;
  UINT32         OverrideTxPads;
  EFI_STATUS     Status;

  GroupMin = GpioGetLowestGroup ();
  GroupMax = GpioGetHighestGroup ();

  for (Group = GroupMin; Group <= GroupMax; Group++) {
    for (DwNum = 0; DwNum <= GPIO_GET_DW_NUM (GpioGetPadPerGroup (Group)); DwNum++) {
      OverrideCfgPads = 0;
      OverrideTxPads = 0;
      Status = GpioUnlockOverride (Group, DwNum, &OverrideCfgPads, &OverrideTxPads);
      if (EFI_ERROR (Status)) {
        ASSERT (FALSE);
        return Status;
      }

      UnlockedPads = GpioGetGroupDwUnlockPadConfigMask (GpioGetGroupIndexFromGroup (Group), DwNum);

      Status = GpioLockPadCfgForGroupDw (Group, DwNum, (UINT32)~(UnlockedPads | OverrideCfgPads));
      if (EFI_ERROR (Status)) {
        ASSERT (FALSE);
        return Status;
      }

      UnlockedPads = GpioGetGroupDwUnlockOutputMask (GpioGetGroupIndexFromGroup (Group), DwNum);

      Status = GpioLockPadCfgTxForGroupDw (Group, DwNum, (UINT32)~(UnlockedPads | OverrideTxPads));
      if (EFI_ERROR (Status)) {
        ASSERT (FALSE);
        return Status;
      }
    }
  }
  return EFI_SUCCESS;
}
