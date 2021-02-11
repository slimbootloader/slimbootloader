/** @file
  This file contains routines for GPIO

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <GpioLibConfig.h>
#include <Library/GpioSocLib.h>
#include <Library/GpioPlatformLib.h>
#include <GpioLibInternal.h>

/**
  This procedure will check if GpioPad is owned by host.

  @param[in] GpioPad       GPIO pad

  @retval TRUE             GPIO pad is owned by host
  @retval FALSE            GPIO pad is not owned by host and should not be used with GPIO lib API
**/
BOOLEAN
GpioIsPadHostOwned (
  IN GPIO_PAD             GpioPad
  )
{
  UINT32               GroupIndex;
  UINT32               PadNumber;
  GPIO_PAD_OWN         PadOwnVal;

  //
  // Check if selected GPIO Pad is not owned by CSME/ISH
  // If GPIO is not owned by Host all access to PadCfg will be dropped
  //
  GpioGetPadOwnership (GpioPad, &PadOwnVal);
  if (PadOwnVal != GpioPadOwnHost) {
    GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
    PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);
    DEBUG ((DEBUG_ERROR, "GPIO ERROR: Gpio pad not owned by host (Group=%d, Pad=%d)!\n", GroupIndex, PadNumber));
    return FALSE;
  }

  return TRUE;
}

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
  )
{
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 PadNumber;
  UINT32                 GroupIndex;

  if (!GpioIsCorrectPadForThisChipset (GpioPad)) {
    DEBUG ((DEBUG_ERROR, "GPIO ERROR: Incorrect GpioPad (0x%08x) used on this chipset!\n", GpioPad));
    goto Error;
  }

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Check if legal pin number
  //
  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);
  if (PadNumber >= GpioGroupInfo[GroupIndex].PadPerGroup) {
    DEBUG ((DEBUG_ERROR, "GPIO ERROR: Pin number (%d) exceeds possible range for this group\n", PadNumber));
    goto Error;
  }

  return TRUE;
Error:
  ASSERT (FALSE);
  return FALSE;
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
  PadCfgReg = GpioGroupInfo[GroupIndex].PadCfgOffset + GPIO_PCR_PADCFG * PadNumber + 0x4 * DwReg;

  return MmioRead32 (GetPchPcrAddress (GpioGroupInfo[GroupIndex].Community, PadCfgReg));
}

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
  )
{
  UINT32                 PadCfgReg;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;
  UINT32                 PadCfgLock;
  UINT32                 PadCfgLockTx;

  PadCfgLock = 0;
  PadCfgLockTx = 0;

  //
  // Check if Pad Configuration (except output state) is to be changed.
  // If AND and OR masks will indicate that configuration fields (other than output control)
  // are to be modified it means that there is a need to perform an unlock (if set)
  //
  if ((~PadCfgAndMask | PadCfgOrMask) & (UINT32)~B_GPIO_PCR_TX_STATE) {
    GpioGetPadCfgLock (GpioPad, &PadCfgLock);
    if (PadCfgLock) {
      GpioUnlockPadCfg (GpioPad);
    }
  }

  //
  // Check if Pad Output state is to be changed
  // If AND and OR masks will indicate that output control
  // is to be modified it means that there is a need to perform an unlock (if set)
  //
  if ((~PadCfgAndMask | PadCfgOrMask) & B_GPIO_PCR_TX_STATE) {
    GpioGetPadCfgLockTx (GpioPad, &PadCfgLockTx);
    if (PadCfgLockTx) {
      GpioUnlockPadCfgTx (GpioPad);
    }
  }

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Create Pad Configuration register offset
  //
  PadCfgReg = GpioGroupInfo[GroupIndex].PadCfgOffset + GPIO_PCR_PADCFG * PadNumber + 0x4 * DwReg;

  MmioAndThenOr32 (
    GetPchPcrAddress (GpioGroupInfo[GroupIndex].Community, PadCfgReg),
    PadCfgAndMask,
    PadCfgOrMask
    );
  DEBUG((DEBUG_INFO, "GPIO Address: 0x%08x, PadCfgAndMask = 0x%08x, PadCfgOrMask = 0x%08x\n", GetPchPcrAddress(GpioGroupInfo[GroupIndex].Community, PadCfgReg), PadCfgAndMask, PadCfgOrMask));
  if (PadCfgLock) {
    GpioLockPadCfg (GpioPad);
  }
  if (PadCfgLockTx) {
    GpioLockPadCfgTx (GpioPad);
  }
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
  GpioPadLockOutputRegister
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

  *ReadVal = MmioRead32 (GetPchPcrAddress (GpioGroupInfo[GroupIndex].Community, RegOffset));

  return EFI_SUCCESS;
}

/**
  This procedure will write GPIO register

  @param[in] RegType              GPIO register type
  @param[in] Group                GPIO group
  @param[in] DwNum                Register number for current group (parameter applicable in accessing whole register).
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in] RegAndMask           Mask which will be AND'ed with register value
  @param[in] RegOrMask            Mask which will be OR'ed with register value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_UNSUPPORTED         Feature is not supported for this group or pad
**/
STATIC
EFI_STATUS
GpioWriteReg (
  IN GPIO_REG               RegType,
  IN GPIO_GROUP             Group,
  IN UINT32                 DwNum,
  IN UINT32                 RegAndMask,
  IN UINT32                 RegOrMask
  )
{
  UINT32                 RegOffset;
  UINT32                 GroupIndex;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 PadCfgLock;
  BOOLEAN                Lockable;
  EFI_STATUS             Status;

  Lockable = FALSE;
  PadCfgLock = 0;
  RegOffset = NO_REGISTER_FOR_PROPERTY;
  GroupIndex = GpioGetGroupIndexFromGroup (Group);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  switch (RegType) {
    case GpioHostOwnershipRegister:
      RegOffset = GpioGroupInfo[GroupIndex].HostOwnOffset;
      break;
    case GpioGpeEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].GpiGpeEnOffset;
      Lockable = TRUE;
      break;
    case GpioGpeStatusRegister:
      RegOffset = GpioGroupInfo[GroupIndex].GpiGpeStsOffset;
      break;
    case GpioSmiEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].SmiEnOffset;
      Lockable = TRUE;
      break;
    case GpioSmiStatusRegister:
      RegOffset = GpioGroupInfo[GroupIndex].SmiStsOffset;
      break;
    case GpioNmiEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].NmiEnOffset;
      Lockable = TRUE;
      break;
    case GpioPadConfigLockRegister:
    case GpioPadLockOutputRegister:
    default:
      break;
  }

  //
  // Check if selected register exists
  //
  if (RegOffset == NO_REGISTER_FOR_PROPERTY) {
    return EFI_UNSUPPORTED;
  }

  if (Lockable) {
    GpioGetPadCfgLockForGroupDw (Group, DwNum, &PadCfgLock);
    if (PadCfgLock) {
      //
      // Check if for pads which are going to be reconfigured lock is set.
      //
      if ((~RegAndMask | RegOrMask) & PadCfgLock) {
        //
        // Unlock all pads for this Group DW reg for simplicity
        // even if not all of those pads will have their settings reprogrammed
        //
        Status = GpioUnlockPadCfgForGroupDw (Group, DwNum, PadCfgLock);
        if (EFI_ERROR (Status)) {
          ASSERT (FALSE);
          return Status;
        }
      } else {
        //
        // No need to perform an unlock as pads which are going to be reconfigured
        // are not in locked state
        //
        PadCfgLock = 0;
      }
    }
  }

  //
  // If there are more then 32 pads per group then certain
  // group information would be split into more then one DWord register.
  //
  RegOffset += DwNum * 0x4;

  MmioAndThenOr32 (
    GetPchPcrAddress (GpioGroupInfo[GroupIndex].Community, RegOffset),
    RegAndMask,
    RegOrMask
    );

  if (Lockable && PadCfgLock) {
    //
    // Lock previously unlocked pads
    //
    Status = GpioLockPadCfgForGroupDw (Group, DwNum, PadCfgLock);
    if (EFI_ERROR (Status)) {
      ASSERT (FALSE);
      return Status;
    }
  }

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

  Status = GpioPchSbiExecutionEx (
             GpioGroupInfo[GroupIndex].Community,
             RegOffset,
             (GPIO_PCH_SBI_OPCODE) GpioGetLockOpcode (),
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
  )
{
  GPIO_GROUP           Group;

  static GPIO_RESET_CONFIG  GppPadRstCfgToGpioResetConfigMap[] = {
                              GpioResumeReset,
                              GpioHostDeepReset,
                              GpioPlatformReset};
  static GPIO_RESET_CONFIG  GpdPadRstCfgToGpioResetConfigMap[] = {
                              GpioDswReset,
                              GpioHostDeepReset,
                              GpioPlatformReset,
                              GpioResumeReset};

  Group = GpioGetGroupFromGpioPad (GpioPad);

  if (GpioIsDswGroup (Group) && PadRstCfg < 4) {
    return GpdPadRstCfgToGpioResetConfigMap[PadRstCfg];
  } else if (PadRstCfg < 3) {
    return GppPadRstCfgToGpioResetConfigMap[PadRstCfg];
  } else {
    ASSERT (FALSE);
    return GpioResetDefault;
  }
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
        DEBUG ((DEBUG_ERROR, "GPIO ERROR: Only GPD group pads can use GpioDswReset (Group=%d, Pad=%d)!\n", GpioGetGroupIndexFromGpioPad (GpioPad), GpioGetPadNumberFromGpioPad (GpioPad)));
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
  This internal procedure will get GPIO_CONFIG data from PADCFG registers value

  @param[in]  GpioPad                   GPIO Pad
  @param[in]  PadCfgDwReg               PADCFG DWx register values
  @param[out] GpioData                  GPIO Configuration data

  @retval Status
**/
STATIC
VOID
GpioConfigFromPadCfgRegValue (
  IN GPIO_PAD      GpioPad,
  IN CONST UINT32  *PadCfgDwReg,
  OUT GPIO_CONFIG  *GpioConfig
  )
{
  UINT32               PadRstCfg;

  //
  // Get Reset Type (PadRstCfg)
  //
  PadRstCfg = (PadCfgDwReg[0] & B_GPIO_PCR_RST_CONF) >> N_GPIO_PCR_RST_CONF;

  GpioConfig->PowerConfig = GpioResetConfigFromPadRstCfg (
                              GpioPad,
                              PadRstCfg
                              );

  //
  // Get how interrupt is triggered (RxEvCfg)
  //
  GpioConfig->InterruptConfig = ((PadCfgDwReg[0] & B_GPIO_PCR_RX_LVL_EDG) >> (N_GPIO_PCR_RX_LVL_EDG - (N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS + 1))) | (0x1 << N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS);

  //
  // Get interrupt generation (GPIRoutIOxAPIC/SCI/SMI/NMI)
  //
  GpioConfig->InterruptConfig |= ((PadCfgDwReg[0] & (B_GPIO_PCR_RX_NMI_ROUTE | B_GPIO_PCR_RX_SCI_ROUTE | B_GPIO_PCR_RX_SMI_ROUTE | B_GPIO_PCR_RX_APIC_ROUTE)) >> (N_GPIO_PCR_RX_NMI_ROUTE - (N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS + 1))) | (0x1 << N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS);

  //
  // Get GPIO direction (GPIORxDis and GPIOTxDis)
  //
  GpioConfig->Direction = ((PadCfgDwReg[0] & (B_GPIO_PCR_RXDIS | B_GPIO_PCR_TXDIS)) >> (N_GPIO_PCR_TXDIS - (N_GPIO_DIRECTION_DIR_BIT_POS + 1))) | (0x1 << N_GPIO_DIRECTION_DIR_BIT_POS);

  //
  // Get GPIO input inversion (RXINV)
  // (Only meaningful if input enabled)
  //
  if((PadCfgDwReg[0] & B_GPIO_PCR_RXDIS) == 0) {
    GpioConfig->Direction |= ((PadCfgDwReg[0] & B_GPIO_PCR_RXINV) >> (N_GPIO_PCR_RXINV - (N_GPIO_DIRECTION_INV_BIT_POS + 1))) | (0x1 << N_GPIO_DIRECTION_INV_BIT_POS);
  }

  //
  // Get GPIO output state (GPIOTxState)
  //
  GpioConfig->OutputState = ((PadCfgDwReg[0] & B_GPIO_PCR_TX_STATE) << (N_GPIO_PCR_TX_STATE + (N_GPIO_OUTPUT_BIT_POS + 1))) | (0x1 << N_GPIO_OUTPUT_BIT_POS);

  //
  // Configure GPIO RX raw override to '1' (RXRAW1)
  //
  GpioConfig->OtherSettings = ((PadCfgDwReg[0] & B_GPIO_PCR_RX_RAW1) >> (N_GPIO_PCR_RX_RAW1 - (N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS + 1))) | (0x1 << N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS);

  //
  // Get GPIO Pad Mode (PMode)
  //
  GpioConfig->PadMode = ((PadCfgDwReg[0] & B_GPIO_PCR_PAD_MODE) >> (N_GPIO_PCR_PAD_MODE - (N_GPIO_PAD_MODE_BIT_POS + 1))) | (0x1 << N_GPIO_PAD_MODE_BIT_POS);

  //
  // Get GPIO termination (Term)
  //
  GpioConfig->ElectricalConfig = ((PadCfgDwReg[1] & B_GPIO_PCR_TERM) >> (N_GPIO_PCR_TERM - (N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS + 1))) | (0x1 << N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS);
}

/**
  This procedure will read multiple GPIO settings

  @param[in]  GpioPad                   GPIO Pad
  @param[out] GpioData                  GPIO data structure

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioGetPadConfig (
  IN  GPIO_PAD               GpioPad,
  OUT GPIO_CONFIG            *GpioData
  )
{
  UINT32               PadCfgDwReg[GPIO_PADCFG_DW_REG_NUMBER];
  UINT32               RegVal;
  GPIO_GROUP           Group;
  UINT32               PadNumber;
  UINT32               PadBitPosition;

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);
  PadBitPosition = GPIO_GET_PAD_POSITION (PadNumber);

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Read PADCFG DW0 register
  //
  PadCfgDwReg[0] = GpioReadPadCfgReg (GpioPad, 0);

  //
  // Read PADCFG DW1 register
  //
  PadCfgDwReg[1] = GpioReadPadCfgReg (GpioPad, 1);

  //
  // Read PADCFG DW2 register
  //
  PadCfgDwReg[2] = GpioReadPadCfgReg (GpioPad, 2);

  GpioConfigFromPadCfgRegValue (
    GpioPad,
    PadCfgDwReg,
    GpioData
    );

  //
  // Read HOSTSW_OWN registers
  //
  GpioReadReg (
    GpioHostOwnershipRegister,
    Group,
    GPIO_GET_DW_NUM (PadNumber),
    &RegVal
    );

  //
  // Get Host Software Ownership
  //
  GpioData->HostSoftPadOwn = (((RegVal >> PadBitPosition) & 0x1) << (N_GPIO_HOSTSW_OWN_BIT_POS + 1)) | (0x1 << N_GPIO_HOSTSW_OWN_BIT_POS);

  //
  // Read PADCFGLOCK register
  //
  GpioReadReg (
    GpioPadConfigLockRegister,
    Group,
    GPIO_GET_DW_NUM (PadNumber),
    &RegVal
    );

  //
  // Get Pad Configuration Lock state
  //
  GpioData->LockConfig = ((!((RegVal >> PadBitPosition) & 0x1)) << 1) | 0x1;

  //
  // Read PADCFGLOCKTX register
  //
  GpioReadReg (
    GpioPadLockOutputRegister,
    Group,
    GPIO_GET_DW_NUM (PadNumber),
    &RegVal
    );

  //
  // Get Pad Configuration Lock Tx state
  //
  GpioData->LockConfig |= ((!((RegVal >> PadBitPosition) & 0x1)) << 2) | 0x1;

  return EFI_SUCCESS;
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
  )
{
  if (Value > 1) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  GpioWritePadCfgReg (
    GpioPad,
    0,
    (UINT32)~B_GPIO_PCR_TX_STATE,
    Value << N_GPIO_PCR_TX_STATE
    );

  return EFI_SUCCESS;
}

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
  )
{
  UINT32      PadCfgReg;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  PadCfgReg = GpioReadPadCfgReg (GpioPad, 0);

  *OutputVal = (PadCfgReg & B_GPIO_PCR_TX_STATE) >> N_GPIO_PCR_TX_STATE;

  return EFI_SUCCESS;
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
EFIAPI
GpioGetInputValue (
  IN GPIO_PAD                  GpioPad,
  OUT UINT32                   *InputVal
  )
{
  UINT32      PadCfgReg;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  PadCfgReg = GpioReadPadCfgReg (GpioPad, 0);

  *InputVal = (PadCfgReg & B_GPIO_PCR_RX_STATE) >> N_GPIO_PCR_RX_STATE;

  return EFI_SUCCESS;
}

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
  )
{
  UINT32      PadCfgReg;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  PadCfgReg = GpioReadPadCfgReg (GpioPad, 1);

  *IrqNum = (PadCfgReg & B_GPIO_PCR_INTSEL) >> N_GPIO_PCR_INTSEL;

  return EFI_SUCCESS;
}

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
  )
{
  if (Value > 1) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  GpioWritePadCfgReg (
    GpioPad,
    0,
    (UINT32)~B_GPIO_PCR_RXINV,
    Value << N_GPIO_PCR_RXINV
    );

  return EFI_SUCCESS;
}

/**
  This procedure will get GPIO pad input inversion value

  @param[in]  GpioPad             GPIO pad
  @param[out] InvertState         GPIO inversion state
                                  0: No input inversion, 1: Inverted input

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioGetInputInversion (
  IN  GPIO_PAD                 GpioPad,
  OUT UINT32                   *InvertState
  )
{
  UINT32      PadCfgReg;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  PadCfgReg = GpioReadPadCfgReg (GpioPad, 0);

  *InvertState = (PadCfgReg & B_GPIO_PCR_RXINV) >> N_GPIO_PCR_RXINV;
  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS  Status;
  UINT32      RxLvlEdgeValue;
  UINT32      IntRouteValue;
  UINT32      PadNumber;
  UINT32      GpeEnable;
  UINT32      NmiEnable;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_SUCCESS;

  if (((Value & B_GPIO_INT_CONFIG_INT_TYPE_MASK) >> N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS) != GpioHardwareDefault) {
    RxLvlEdgeValue = ((Value & B_GPIO_INT_CONFIG_INT_TYPE_MASK) >> (N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS + 1)) << N_GPIO_PCR_RX_LVL_EDG;

    GpioWritePadCfgReg (
      GpioPad,
      0,
      (UINT32)~B_GPIO_PCR_RX_LVL_EDG,
      RxLvlEdgeValue
      );
  }

  if (((Value & B_GPIO_INT_CONFIG_INT_SOURCE_MASK) >> N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS) != GpioHardwareDefault) {

    IntRouteValue = ((Value & B_GPIO_INT_CONFIG_INT_SOURCE_MASK) >> (N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS + 1)) << N_GPIO_PCR_RX_NMI_ROUTE;

    GpioWritePadCfgReg (
      GpioPad,
      0,
      (UINT32)~(B_GPIO_PCR_RX_NMI_ROUTE | B_GPIO_PCR_RX_SCI_ROUTE | B_GPIO_PCR_RX_SMI_ROUTE | B_GPIO_PCR_RX_APIC_ROUTE),
      IntRouteValue
      );

    if ((Value & GpioIntSci) == GpioIntSci) {
      GpeEnable = 0x1;
    } else {
      GpeEnable = 0x0;
    }

    PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

    GpioWriteReg (
      GpioGpeEnableRegister,
      GpioGetGroupFromGpioPad (GpioPad),
      GPIO_GET_DW_NUM (PadNumber),
      ~(1 << GPIO_GET_PAD_POSITION (PadNumber)),
      GpeEnable << GPIO_GET_PAD_POSITION (PadNumber)
      );

    if ((Value & GpioIntNmi) == GpioIntNmi) {
      NmiEnable = 0x1;
    } else {
      NmiEnable = 0x0;
    }

    Status = GpioWriteReg (
               GpioNmiEnableRegister,
               GpioGetGroupFromGpioPad (GpioPad),
               GPIO_GET_DW_NUM (PadNumber),
               ~(1 << GPIO_GET_PAD_POSITION (PadNumber)),
               NmiEnable << GPIO_GET_PAD_POSITION (PadNumber)
               );
    if (Status == EFI_UNSUPPORTED) {
      if (NmiEnable == 0) {
        //
        // Not all GPIO have NMI capabilities. Since we always try to program this register,
        // even when not enabling NMI for a pad so do not report such access as an error
        //
        return EFI_SUCCESS;
      } else {
        DEBUG ((DEBUG_ERROR, "GPIO ERROR: Group %d has no pads supporting NMI\n", GpioGetGroupIndexFromGpioPad (GpioPad)));
      }
    }
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}

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
  )
{
  UINT32      TermValue;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  if (((Value & B_GPIO_ELECTRICAL_CONFIG_TERMINATION_MASK) >> N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS) != GpioHardwareDefault) {
    TermValue = ((Value & B_GPIO_ELECTRICAL_CONFIG_TERMINATION_MASK) >> (N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS + 1)) << N_GPIO_PCR_TERM;

    GpioWritePadCfgReg (
      GpioPad,
      1,
      (UINT32)~B_GPIO_PCR_TERM,
      TermValue
      );
  }

  return EFI_SUCCESS;
}

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
  )
{
  UINT32      PadRstCfg;
  EFI_STATUS  Status;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  if (((Value & B_GPIO_RESET_CONFIG_RESET_MASK) >> N_GPIO_RESET_CONFIG_RESET_BIT_POS) != GpioHardwareDefault) {

    //
    // Reset configuration depends on group type.
    // This field requires support for new and deprecated settings.
    //
    Status = GpioPadRstCfgFromResetConfig (
               GpioPad,
               Value,
               &PadRstCfg
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    GpioWritePadCfgReg (
      GpioPad,
      0,
      (UINT32)~B_GPIO_PCR_RST_CONF,
      PadRstCfg << N_GPIO_PCR_RST_CONF
      );
  }
  return EFI_SUCCESS;
}

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
  )
{
  UINT32      PadRstCfg;
  UINT32      PadCfgDw0Reg;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  PadCfgDw0Reg = GpioReadPadCfgReg (GpioPad, 0);

  //
  // Get Reset Type (PadRstCfg)
  //
  PadRstCfg = (PadCfgDw0Reg & B_GPIO_PCR_RST_CONF) >> N_GPIO_PCR_RST_CONF;

  *Value = GpioResetConfigFromPadRstCfg (
             GpioPad,
             PadRstCfg
             );

  return EFI_SUCCESS;
}

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
  )
{
  if (!GpioIsGroupAndDwNumValid (Group, DwNum)) {
    return EFI_INVALID_PARAMETER;
  }

  return GpioReadReg (
           GpioHostOwnershipRegister,
           Group,
           DwNum,
           HostSwRegVal
           );
}

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
  )
{
  if (!GpioIsGroupAndDwNumValid (Group, DwNum)) {
    return EFI_INVALID_PARAMETER;
  }

  return GpioWriteReg (
           GpioHostOwnershipRegister,
           Group,
           DwNum,
           0,
           HostSwRegVal
           );
}

/**
  This procedure will get Gpio Pad Host Software Ownership

  @param[in]  GpioPad             GPIO pad
  @param[out] PadHostSwOwn        Value of Host Software Pad Owner
                                  0: ACPI Mode, 1: GPIO Driver mode

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetHostSwOwnershipForPad (
  IN GPIO_PAD                 GpioPad,
  OUT UINT32                  *PadHostSwOwn
  )
{
  UINT32      PadNumber;
  UINT32      HostSwRegVal;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioReadReg (
    GpioHostOwnershipRegister,
    GpioGetGroupFromGpioPad (GpioPad),
    GPIO_GET_DW_NUM (PadNumber),
    &HostSwRegVal
    );

  *PadHostSwOwn = (HostSwRegVal >> GPIO_GET_PAD_POSITION (PadNumber)) & 0x1;

  return EFI_SUCCESS;
}

/**
  This procedure will set Gpio Pad Host Software Ownership

  @param[in] GpioPad              GPIO pad
  @param[in] PadHostSwOwn         Pad Host Software Owner
                                  0: ACPI Mode, 1: GPIO Driver mode

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioSetHostSwOwnershipForPad (
  IN GPIO_PAD                  GpioPad,
  IN UINT32                    PadHostSwOwn
  )
{
  UINT32      PadNumber;

  if (!GpioIsPadValid (GpioPad) || (PadHostSwOwn > 1)) {
    return EFI_INVALID_PARAMETER;
  }

  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  return GpioWriteReg (
           GpioHostOwnershipRegister,
           GpioGetGroupFromGpioPad (GpioPad),
           GPIO_GET_DW_NUM (PadNumber),
           (UINT32)~(1 << GPIO_GET_PAD_POSITION (PadNumber)),
           PadHostSwOwn << GPIO_GET_PAD_POSITION (PadNumber)
           );
}

/**
  This procedure will get Gpio Pad Ownership

  @param[in] GpioPad              GPIO pad
  @param[out] PadOwnVal           Value of Pad Ownership

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioGetPadOwnership (
  IN  GPIO_PAD                GpioPad,
  OUT GPIO_PAD_OWN            *PadOwnVal
  )
{
  UINT32                 Mask;
  UINT32                 RegOffset;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 PadOwnRegValue;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Calculate RegOffset using Pad Ownership offset and GPIO Pad number.
  // One DWord register contains information for 8 pads.
  //
  RegOffset = GpioGroupInfo[GroupIndex].PadOwnOffset + (PadNumber >> 3) * 0x4;

  //
  // Calculate pad bit position within DWord register
  //
  PadNumber %= 8;
  Mask = (BIT1 | BIT0) << (PadNumber * 4);

  PadOwnRegValue = MmioRead32 (GetPchPcrAddress (GpioGroupInfo[GroupIndex].Community, RegOffset));

  *PadOwnVal = (GPIO_PAD_OWN) ((PadOwnRegValue & Mask) >> (PadNumber * 4));

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

/**
  This procedure will check state of Pad Config Lock for selected pad

  @param[in] GpioPad              GPIO pad
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
  )
{
  UINT32      PadNumber;
  UINT32      PadCfgLockRegVal;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioReadReg (
    GpioPadConfigLockRegister,
    GpioGetGroupFromGpioPad (GpioPad),
    GPIO_GET_DW_NUM (PadNumber),
    &PadCfgLockRegVal
    );

  *PadCfgLock = (PadCfgLockRegVal >> GPIO_GET_PAD_POSITION (PadNumber)) & 0x1;

  return EFI_SUCCESS;
}

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

/**
  This procedure will check state of Pad Config Tx Lock for selected pad

  @param[in] GpioPad              GPIO pad
  @param[out] PadCfgLock          PadCfgLockTx for selected pad
                                  0: NotLockedTx, 1: LockedTx

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
GpioGetPadCfgLockTx (
  IN GPIO_PAD                   GpioPad,
  OUT UINT32                    *PadCfgLockTx
  )
{
  UINT32      PadNumber;
  UINT32      PadCfgLockTxRegVal;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioReadReg (
    GpioPadLockOutputRegister,
    GpioGetGroupFromGpioPad (GpioPad),
    GPIO_GET_DW_NUM (PadNumber),
    &PadCfgLockTxRegVal
    );

  *PadCfgLockTx = (PadCfgLockTxRegVal >> GPIO_GET_PAD_POSITION (PadNumber)) & 0x1;

  return EFI_SUCCESS;
}

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
  This procedure will clear PadCfgLock for selected pad.
  This function should be used only inside SMI.

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioUnlockPadCfg (
  IN GPIO_PAD                   GpioPad
  )
{
  GPIO_GROUP   Group;
  UINT32       PadNumber;

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  return GpioUnlockPadCfgForGroupDw (
           Group,
           GPIO_GET_DW_NUM (PadNumber),
           1 << GPIO_GET_PAD_POSITION (PadNumber)
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
EFIAPI
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
  This procedure will clear PadCfgLockTx for selected pad.
  This function should be used only inside SMI.

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioUnlockPadCfgTx (
  IN GPIO_PAD                   GpioPad
  )
{
  GPIO_GROUP   Group;
  UINT32       PadNumber;

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  return GpioUnlockPadCfgTxForGroupDw (
           Group,
           GPIO_GET_DW_NUM (PadNumber),
           1 << GPIO_GET_PAD_POSITION (PadNumber)
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
  )
{
  UINT32     GroupDw[3];
  UINT32     Index;
  EFI_STATUS Status;

  Status = GpioGetGroupDwToGpeDwX (
             GroupToGpeDw0,
             &GroupDw[0],
             GroupToGpeDw1,
             &GroupDw[1],
             GroupToGpeDw2,
             &GroupDw[2]
             );

  for (Index = 0; Index < ARRAY_SIZE (GroupDw); Index++) {
    if (GroupDw[Index] != 0) {
      Status = EFI_UNSUPPORTED;
      ASSERT (FALSE);
    }
  }
  return Status;
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
EFIAPI
GpioGetGroupDwToGpeDwX (
  OUT GPIO_GROUP                *GroupToGpeDw0,
  OUT UINT32                    *GroupDwForGpeDw0,
  OUT GPIO_GROUP                *GroupToGpeDw1,
  OUT UINT32                    *GroupDwForGpeDw1,
  OUT GPIO_GROUP                *GroupToGpeDw2,
  OUT UINT32                    *GroupDwForGpeDw2
  )
{
  UINT32                     PmcGpeDwXValue[3];
  GPIO_GROUP                 GroupToGpeDwX[3];
  UINT32                     GroupDwForGpeDwX[3];
  UINT8                      GpeDwXIndex;
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
  IN  GPIO_PAD                  GpioPad,
  OUT UINT32                    *GpeNumber
  )
{
  GPIO_GROUP           GroupToGpeDwX[3];
  UINT32               GroupDw[3];
  GPIO_GROUP           Group;
  UINT32               PadNumber;
  UINT32               Index;
  EFI_STATUS           Status;

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get GPIO groups mapping to 1-tier GPE
  // This mapping is dependent on GPIO IP implementation
  // and may change between chipset generations
  //
  Status = GpioGetGroupDwToGpeDwX (
             &GroupToGpeDwX[0], &GroupDw[0],
             &GroupToGpeDwX[1], &GroupDw[1],
             &GroupToGpeDwX[2], &GroupDw[2]
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check if pad is routed to 1-Tier GPE
  //
  for (Index = 0; Index < 3; Index++) {
    if ((Group == GroupToGpeDwX[Index]) && (PadNumber >= (32 * GroupDw[Index])) && (PadNumber < (32 * (GroupDw[Index] + 1)))) {
      *GpeNumber = PadNumber + (32 * Index) - (32 * GroupDw[Index]);
      return EFI_SUCCESS;
    }
  }

  //
  // If Group number doesn't match any of above then
  // it means that the pad is routed to 2-tier GPE
  // which corresponds to  GPE_111 (0x6F)
  //
  *GpeNumber = PCH_GPIO_2_TIER_MASTER_GPE_NUMBER;

  return EFI_SUCCESS;
}

/**
  This procedure is used to check GPIO inputs belongs to 2 tier or 1 tier architecture

  @param[in]  GpioPad             GPIO pad

  @retval     Data                0 means 1-tier, 1 means 2-tier
**/
BOOLEAN
GpioCheckFor2Tier (
  IN GPIO_PAD                  GpioPad
  )
{
  UINT32               Data32;
  EFI_STATUS           Status;

  Status = GpioGetGpeNumber (GpioPad, &Data32);
  if (EFI_ERROR (Status)) {
    DEBUG (( DEBUG_ERROR, "GpioCheckFor2Tier: Failed to get GPE number. Status: %r\n", Status ));
    return FALSE;
  }

  if (Data32 == PCH_GPIO_2_TIER_MASTER_GPE_NUMBER) {
    return TRUE;
  }

  return FALSE;
}

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
  )
{
  GPIO_GROUP           Group;
  UINT32               PadNumber;
  UINT32               DwNum;
  UINT32               PadBitPosition;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check for 2-tier
  //
  if (!(GpioCheckFor2Tier (GpioPad))) {
    return EFI_INVALID_PARAMETER;
  }

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);
  DwNum = GPIO_GET_DW_NUM (PadNumber);
  PadBitPosition = GPIO_GET_PAD_POSITION (PadNumber);

  //
  // Clear GPI GPE Status bit by writing '1'
  //
  return GpioWriteReg (
           GpioGpeStatusRegister,
           Group,
           DwNum,
           0u,
           (UINT32) (BIT0 << PadBitPosition)
           );
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
EFIAPI
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
