/** @file
  This file contains routines for GPIO native and chipset specific usage

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include "GpioLibrary.h"

//
// Chipset specific data
//

//SATA
extern GPIO_PAD_NATIVE_FUNCTION mPchLpSataGpToGpioMap[PCH_LP_AHCI_MAX_PORTS];
extern GPIO_PAD_NATIVE_FUNCTION mPchHSataGpToGpioMap[PCH_H_AHCI_MAX_PORTS];

extern GPIO_PAD_NATIVE_FUNCTION mCdfPchSata1GpToGpioMap[PCH_H_AHCI_MAX_PORTS];
extern GPIO_PAD_NATIVE_FUNCTION mCdfPchSata2GpToGpioMap[PCH_H_AHCI_MAX_PORTS];
extern GPIO_PAD_NATIVE_FUNCTION mCdfPchSata3GpToGpioMap[PCH_H_AHCI_MAX_PORTS];

extern GPIO_PAD_NATIVE_FUNCTION mPchLpSataDevSlpPinToGpioMap[PCH_LP_AHCI_MAX_PORTS];
extern GPIO_PAD_NATIVE_FUNCTION mPchHSataDevSlpPinToGpioMap[PCH_H_AHCI_MAX_PORTS];

extern GPIO_PAD_NATIVE_FUNCTION mCdfPchSata1DevSlpPinToGpioMap[PCH_H_AHCI_MAX_PORTS];
extern GPIO_PAD_NATIVE_FUNCTION mCdfPchSata2DevSlpPinToGpioMap[PCH_H_AHCI_MAX_PORTS];
extern GPIO_PAD_NATIVE_FUNCTION mCdfPchSata3DevSlpPinToGpioMap[PCH_H_AHCI_MAX_PORTS];

extern GPIO_GROUP_INFO mPchLpGpioGroupInfo[GPIO_VER2_PCH_LP_GPIO_GROUP_MAX];
extern GPIO_GROUP_INFO mPchHGpioGroupInfo[GPIO_VER2_PCH_H_GPIO_GROUP_MAX];
extern GPIO_GROUP_INFO mPchKGpioGroupInfo[GPIO_VER2_PCH_K_GPIO_GROUP_MAX];


/**
  This procedure is used to check if GpioPad is valid for certain chipset

  @param[in]  GpioPad             GPIO pad

  @retval TRUE                    This pin is valid on this chipset
          FALSE                   Incorrect pin
**/
BOOLEAN
GpioIsCorrectPadForThisChipset (
  IN  GPIO_PAD        GpioPad
  )
{
  return ((GPIO_GET_CHIPSET_ID (GpioPad) == GpioGetThisChipsetId ()) &&
         (GpioGetGroupIndexFromGpioPad (GpioPad) < GpioGetNumberOfGroups ()));
}

/**
  This procedure will retrieve address and length of GPIO info table

  @param[out]  GpioGroupInfoTableLength   Length of GPIO group table

  @retval Pointer to GPIO group table

**/
CONST GPIO_GROUP_INFO*
GpioGetGroupInfoTable (
  OUT UINT32              *GpioGroupInfoTableLength
  )
{
  if (IsPchLp ()) {
    *GpioGroupInfoTableLength = ARRAY_SIZE (mPchLpGpioGroupInfo);
    return mPchLpGpioGroupInfo;
  } else if (IsPchH ()) {
    *GpioGroupInfoTableLength = ARRAY_SIZE (mPchHGpioGroupInfo);
    return mPchHGpioGroupInfo;
  } else {
    *GpioGroupInfoTableLength = ARRAY_SIZE (mPchKGpioGroupInfo);
    return mPchKGpioGroupInfo;
  }
}

/**
  Get GPIO Chipset ID specific to PCH generation and series
**/
UINT32
GpioGetThisChipsetId (
  VOID
  )
{
  if (IsPchLp ()) {
    return GPIO_VER2_LP_CHIPSET_ID;
  } else if (IsPchH ()) {
    return GPIO_VER2_H_CHIPSET_ID;
  } else {
    return GPIO_VER2_K_CHIPSET_ID;
  }
}

GLOBAL_REMOVE_IF_UNREFERENCED PCH_SBI_PID mGpioComSbiIds[] =
{
  PID_GPIOCOM0, PID_GPIOCOM1, PID_GPIOCOM2, PID_GPIOCOM3, PID_GPIOCOM4, PID_GPIOCOM5
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_TO_GPE_MAPPING mPchLpGpioGroupToGpeMapping[] = {
    {GPIO_VER2_LP_GROUP_GPP_B,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_B,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_B},
    {GPIO_VER2_LP_GROUP_GPP_T,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_T,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_T},
    {GPIO_VER2_LP_GROUP_GPP_A,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_A,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_A},
    {GPIO_VER2_LP_GROUP_GPP_R,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_R,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_R},
    {GPIO_VER2_LP_GROUP_GPD,     0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPD,    V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPD},
    {GPIO_VER2_LP_GROUP_GPP_S,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_S,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_S},
    {GPIO_VER2_LP_GROUP_GPP_H,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_H,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_H},
    {GPIO_VER2_LP_GROUP_GPP_D,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_D,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_D},
    {GPIO_VER2_LP_GROUP_GPP_U,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_U,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_U},
    {GPIO_VER2_LP_GROUP_VGPIO,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_VGPIO,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_VGPIO},
    {GPIO_VER2_LP_GROUP_GPP_C,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_C,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_C},
    {GPIO_VER2_LP_GROUP_GPP_F,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_F,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_F},
    {GPIO_VER2_LP_GROUP_GPP_E,   0, V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_E,  V_GPIO_VER2_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_E}
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_TO_GPE_MAPPING mPchHGpioGroupToGpeMapping[] = {
  {GPIO_VER2_H_GROUP_GPD,   0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPD,   V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPD},
  {GPIO_VER2_H_GROUP_GPP_A, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_A, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_A},
  {GPIO_VER2_H_GROUP_GPP_R, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_R, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_R},
  {GPIO_VER2_H_GROUP_GPP_B, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_B, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_B},
  {GPIO_VER2_H_GROUP_GPP_D, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_D, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_D},
  {GPIO_VER2_H_GROUP_GPP_C, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_C, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_C},
  {GPIO_VER2_H_GROUP_GPP_S, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_S, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_S},
  {GPIO_VER2_H_GROUP_GPP_G, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_G, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_G},
  {GPIO_VER2_H_GROUP_VGPIO, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_VGPIO, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_VGPIO},
  {GPIO_VER2_H_GROUP_GPP_E, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_E, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_E},
  {GPIO_VER2_H_GROUP_GPP_F, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_F, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_F},
  {GPIO_VER2_H_GROUP_GPP_H, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_H, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_H},
  {GPIO_VER2_H_GROUP_GPP_K, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_K, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_K},
  {GPIO_VER2_H_GROUP_GPP_J, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_J, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_J},
  {GPIO_VER2_H_GROUP_GPP_I, 0, V_TGL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_I, V_GPIO_VER2_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_I}
};

/**
  Get information for GPIO Group required to program GPIO and PMC for desired 1-Tier GPE mapping

  @param[out] GpioGroupToGpeMapping        Table with GPIO Group to GPE mapping
  @param[out] GpioGroupToGpeMappingLength  GPIO Group to GPE mapping table length
**/
VOID
GpioGetGroupToGpeMapping (
  OUT GPIO_GROUP_TO_GPE_MAPPING  **GpioGroupToGpeMapping,
  OUT UINT32                     *GpioGroupToGpeMappingLength
  )
{
  if (IsPchLp ()) {
    *GpioGroupToGpeMapping = mPchLpGpioGroupToGpeMapping;
    *GpioGroupToGpeMappingLength = ARRAY_SIZE (mPchLpGpioGroupToGpeMapping);
  } else {
    *GpioGroupToGpeMapping = mPchHGpioGroupToGpeMapping;
    *GpioGroupToGpeMappingLength = ARRAY_SIZE (mPchHGpioGroupToGpeMapping);
  }
}

/**
  This function provides GPIO Community PortIDs

  @param[out] NativePinsTable                Table with GPIO COMMx SBI PortIDs

  @retval     Number of communities
**/
UINT32
GpioGetComSbiPortIds (
  OUT PCH_SBI_PID    **GpioComSbiIds
  )
{
  *GpioComSbiIds = mGpioComSbiIds;
  return ARRAY_SIZE (mGpioComSbiIds);
}

/**
  This procedure will get number of pads for certain GPIO group

  @param[in] Group            GPIO group number

  @retval Value               Pad number for group
                              If illegal group number then return 0
**/
UINT32
GpioGetPadPerGroup (
  IN GPIO_GROUP      Group
  )
{
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 GroupIndex;
  //
  // Check if group argument exceeds GPIO GROUP INFO array
  //
  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);
  GroupIndex = GpioGetGroupIndexFromGroup (Group);

  if ((UINTN) GroupIndex >= GpioGroupInfoLength) {
    return 0;
  } else {
    return GpioGroupInfo[GroupIndex].PadPerGroup;
  }
}

/**
  This procedure will get number of groups

  @param[in] none

  @retval Value               Group number
**/
UINT32
GpioGetNumberOfGroups (
  VOID
  )
{
  UINT32                 GpioGroupInfoLength;

  GpioGetGroupInfoTable (&GpioGroupInfoLength);
  return GpioGroupInfoLength;
}
/**
  This procedure will get lowest group

  @param[in] none

  @retval Value               Lowest Group
**/
GPIO_GROUP
GpioGetLowestGroup (
  VOID
  )
{
  return GpioGetGroupFromGroupIndex (0);
}

/**
  This procedure will get highest group

  @param[in] none

  @retval Value               Highest Group
**/
GPIO_GROUP
GpioGetHighestGroup (
  VOID
  )
{
  return GpioGetGroupFromGroupIndex (GpioGetNumberOfGroups () - 1);
}

/**
  This procedure will get group number

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group number
**/
GPIO_GROUP
GpioGetGroupFromGpioPad (
  IN GPIO_PAD         GpioPad
  )
{
  return GPIO_GET_GROUP_FROM_PAD (GpioPad);
}

/**
  This procedure will get group index (0 based)

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group Index
**/
UINT32
GpioGetGroupIndexFromGpioPad (
  IN GPIO_PAD        GpioPad
  )
{
  return (UINT32) GPIO_GET_GROUP_INDEX_FROM_PAD (GpioPad);
}

/**
  This procedure will get group index (0 based) from group

  @param[in] GpioGroup        Gpio Group

  @retval Value               Group Index
**/
UINT32
GpioGetGroupIndexFromGroup (
  IN GPIO_GROUP        GpioGroup
  )
{
  return (UINT32) GPIO_GET_GROUP_INDEX (GpioGroup);
}

/**
  This procedure will get group from group index (0 based)

  @param[in] GroupIndex        Group Index

  @retval GpioGroup            Gpio Group
**/
GPIO_GROUP
GpioGetGroupFromGroupIndex (
  IN UINT32        GroupIndex
  )
{
  return GPIO_GROUP_DEF (GroupIndex, GpioGetThisChipsetId ());
}

/**
  This procedure will get pad number (0 based) from Gpio Pad

  @param[in] GpioPad          Gpio Pad

  @retval Value               Pad Number
**/
UINT32
GpioGetPadNumberFromGpioPad (
  IN GPIO_PAD        GpioPad
  )
{
  return (UINT32) GPIO_GET_PAD_NUMBER (GpioPad);
}
/**
  This procedure will return GpioPad from Group and PadNumber

  @param[in] Group              GPIO group
  @param[in] PadNumber          GPIO PadNumber

  @retval GpioPad               GpioPad
**/
GPIO_PAD
GpioGetGpioPadFromGroupAndPadNumber (
  IN GPIO_GROUP      Group,
  IN UINT32          PadNumber
  )
{
  if (IsPchLp ()) {
    return GPIO_PAD_DEF (Group,PadNumber);
  } else {
    return GPIO_PAD_DEF (Group,PadNumber);
  }
}

/**
  This procedure will return GpioPad from GroupIndex and PadNumber

  @param[in] GroupIndex         GPIO GroupIndex
  @param[in] PadNumber          GPIO PadNumber

  @retval GpioPad               GpioPad
**/
GPIO_PAD
GpioGetGpioPadFromGroupIndexAndPadNumber (
  IN UINT32          GroupIndex,
  IN UINT32          PadNumber
  )
{
  GPIO_GROUP Group;

  Group = GPIO_GROUP_DEF (GroupIndex, GpioGetThisChipsetId ());
  return GPIO_PAD_DEF (Group, PadNumber);
}

/**
  Check if 0x13 opcode supported for writing to GPIO lock unlock register

  @retval TRUE                It's supported
  @retval FALSE               It's not supported
**/
BOOLEAN
IsGpioLockOpcodeSupported (
  VOID
  )
{
  return TRUE;
}


GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD mPchLpTypeCSbuGpioPad[] =
{
  GPIO_VER2_LP_GPP_E18, // BSSB_LS0_RX
  GPIO_VER2_LP_GPP_E19, // BSSB_LS0_TX
  GPIO_VER2_LP_GPP_E20, // BSSB_LS1_RX
  GPIO_VER2_LP_GPP_E21, // BSSB_LS1_TX
  GPIO_VER2_LP_GPP_D9,  // BSSB_LS2_RX
  GPIO_VER2_LP_GPP_D10, // BSSB_LS2_TX
  GPIO_VER2_LP_GPP_D11, // BSSB_LS3_RX
  GPIO_VER2_LP_GPP_D12  // BSSB_LS3_TX
};

/**
  This function is to be used In GpioLockPads() to override a lock request by SOC code.

  @param[in]  Group               GPIO group
  @param[in]  DwNum               Register number for current group (parameter applicable in accessing whole register).
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[out] *UnlockCfgPad       DWORD bitmask for pads which are going to be left unlocked
                                  Bit position - PadNumber
                                  Bit value - 0: to be locked, 1: Leave unlocked
  @param[out] *UnlockTxPad        DWORD bitmask for pads which are going to be left unlocked
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
  )
{
  GPIO_PAD            *GpioSbuPadTable;
  UINT32              SbuPadNum;
  UINT32              MaxIndex;
  GPIO_CONFIG         PadConfig;
  UINT32              Index;
  UINT32              SbuUnlockedPads;
  EFI_STATUS          Status;

  if ((UnlockCfgPad == NULL) || (UnlockTxPad == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  MaxIndex = 0;
  // SBU (Sideband use) pins are used as auxiliary signals for Type C connector,
  // which are hard-wired to BSSB_LS natively for debug function.
  // when USB-C needed, don't lock pins that control by PMC at runtime,
  if (IsPchLp ()) {
    GpioSbuPadTable = mPchLpTypeCSbuGpioPad;
    MaxIndex = ARRAY_SIZE (mPchLpTypeCSbuGpioPad);
  } else {
    *UnlockCfgPad = 0;
    *UnlockTxPad = 0;
    return EFI_SUCCESS;
  }
  SbuUnlockedPads = 0;

  for (Index = 0; Index < MaxIndex; Index++) {

    SbuPadNum = GpioGetPadNumberFromGpioPad (GpioSbuPadTable[Index]);

    if (Group != GpioGetGroupFromGpioPad (GpioSbuPadTable[Index])) {
      continue;
    }

    if (DwNum != GPIO_GET_DW_NUM (SbuPadNum)) {
      continue;
    }

    Status = GpioGetPadConfig (GpioSbuPadTable[Index], &PadConfig);
    if (EFI_ERROR(Status)) {
      continue;
    }

    //
    // GPIO, Native3, Native4 and Native5 control by PMC at runtime
    //
    if ((PadConfig.PadMode == GpioPadModeNative1) || (PadConfig.PadMode == GpioPadModeNative2)) {
      continue;
    }

    SbuUnlockedPads |= (1 << GPIO_GET_PAD_POSITION (SbuPadNum));
  }

  *UnlockCfgPad = SbuUnlockedPads;
  *UnlockTxPad = SbuUnlockedPads;
  return EFI_SUCCESS;
}
