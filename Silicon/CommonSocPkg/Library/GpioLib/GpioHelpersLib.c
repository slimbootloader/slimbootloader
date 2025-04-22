/** @file
  This file contains routines for GPIO native and chipset specific usage

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/GpioLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioSiLib.h>
#include <GpioConfig.h>
#include <Library/MemoryAllocationLib.h>
#include "GpioLibInternal.h"

//
//  GPIO Lock HOB
//  Stores information on GPIO pads that should be left unlocked
//
typedef struct {
  //
  // GPIO PadConfig unlock data
  //
  UINT32  PadConfig;
  //
  // GPIO Output unlock data
  //
  UINT32  OutputState;
} GPIO_UNLOCK_HOB_DATA;

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_UNLOCK_HOB_DATA *mGpioUnlockData = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32 mGpioUnlockDataRecords = 0;

/**
  This procedure will get index of GPIO Unlock HOB structure for selected GroupIndex and DwNum.

  @param[in]  GroupIndex          GPIO group index
  @param[in]  DwNum               DWORD index for a group.
                                  For group which has less then 32 pads per group DwNum must be 0.

  @retval GpioUnlockHobIndex
**/
STATIC
UINT32
GpioUnlockDataIndex (
  IN UINT32                       GroupIndex,
  IN UINT32                       DwNum
  )
{
  UINT32         GpioUnlockDataIndex;
  UINT32         Index;

  GpioUnlockDataIndex = 0;

  for (Index = 0; Index < GroupIndex; Index++) {
    GpioUnlockDataIndex += GPIO_GET_DW_NUM (GpioGetPadPerGroup (GpioGetGroupFromGroupIndex (Index))) + 1;
  }

  GpioUnlockDataIndex += DwNum;
  return GpioUnlockDataIndex;
}

/**
  This procedure will create GPIO HOB for storing unlock data

  @retval Pointer to GPIO Unlock data structure
**/
STATIC
GPIO_UNLOCK_HOB_DATA*
GpioCreateUnlockData (
  VOID
  )
{
  GPIO_GROUP     Group;
  GPIO_GROUP     GroupMin;
  GPIO_GROUP     GroupMax;
  UINT32         GpioUnlockDataRecords;

  if (mGpioUnlockData == NULL) {
    GroupMin = GpioGetLowestGroup ();
    GroupMax = GpioGetHighestGroup ();
    GpioUnlockDataRecords = 0;

    for (Group = GroupMin; Group <= GroupMax; Group++) {
      GpioUnlockDataRecords += GPIO_GET_DW_NUM (GpioGetPadPerGroup (Group)) + 1;
    }

    mGpioUnlockData = (GPIO_UNLOCK_HOB_DATA *)AllocatePool (GpioUnlockDataRecords * sizeof (GPIO_UNLOCK_HOB_DATA));
    if (mGpioUnlockData != NULL) {
      mGpioUnlockDataRecords = GpioUnlockDataRecords;
      ZeroMem (mGpioUnlockData, mGpioUnlockDataRecords * sizeof (GPIO_UNLOCK_HOB_DATA));
    }
  }

  return (GPIO_UNLOCK_HOB_DATA*)mGpioUnlockData;
}

/**
  This procedure will Get GPIO Unlock data structure for storing unlock data.
  If HOB doesn't exist it will be created.

  @param[out] GpioUnlockData          pointer to GPIO Unlock data structure

  @retval Length                      number of GPIO unlock data records
**/
STATIC
UINT32
GpioGetUnlockData (
  GPIO_UNLOCK_HOB_DATA  **GpioUnlockData
  )
{
  if (mGpioUnlockData == NULL) {
    //
    // It is the first time this function is used so create the HOB
    //
    *GpioUnlockData = GpioCreateUnlockData ();
    if (*GpioUnlockData == NULL) {
      return 0;
    }
  } else {
    *GpioUnlockData = mGpioUnlockData;
  }
  return mGpioUnlockDataRecords;
}

/**
  This procedure will get pointer to GPIO Unlock data structure.

  @param[out] GpioUnlockData          pointer to GPIO Unlock data structure

  @retval Length                      number of GPIO unlock data records
**/
STATIC
UINT32
GpioLocateUnlockData (
  GPIO_UNLOCK_HOB_DATA  **GpioUnlockData
  )
{
  if (mGpioUnlockData == NULL) {
    *GpioUnlockData = NULL;
    return 0;
  }

  *GpioUnlockData = (GPIO_UNLOCK_HOB_DATA *) mGpioUnlockData;
  return mGpioUnlockDataRecords;
}

/**
  This procedure stores GPIO pad unlock information

  @param[in] GpioPad         GPIO pad
  @param[in] GpioLockConfig  GPIO Lock Configuration

  @retval Status
**/
EFI_STATUS
GpioStoreUnlockData (
  IN GPIO_PAD             GpioPad,
  IN GPIO_LOCK_CONFIG     GpioLockConfig
  )
{
  GPIO_UNLOCK_HOB_DATA *GpioUnlockData;
  UINT32               Length;
  UINT32               GroupIndex;
  UINT32               PadNumber;
  UINT32               Index;

  if (GpioLockConfig == GpioLockDefault) {
    return EFI_SUCCESS;
  }

  Length = GpioGetUnlockData (&GpioUnlockData);
  if (Length == 0) {
    return EFI_NOT_FOUND;
  }

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);
  Index = GpioUnlockDataIndex (GroupIndex, GPIO_GET_DW_NUM (PadNumber));

  if (Index >= Length) {
    return EFI_INVALID_PARAMETER;
  }

  if ((GpioLockConfig & B_GPIO_LOCK_CONFIG_PAD_CONF_LOCK_MASK) == GpioPadConfigUnlock) {
    GpioUnlockData[Index].PadConfig |= 1 << (GpioGetPadNumberFromGpioPad (GpioPad) % 32);
  }

  if ((GpioLockConfig & B_GPIO_LOCK_CONFIG_OUTPUT_LOCK_MASK) == GpioOutputStateUnlock) {
    GpioUnlockData[Index].OutputState |= 1 << (GpioGetPadNumberFromGpioPad (GpioPad) % 32);
  }

  return EFI_SUCCESS;
}

/**
  This procedure stores GPIO group data about pads which PadConfig needs to be unlocked.

  @param[in]  GroupIndex          GPIO group index
  @param[in]  DwNum               DWORD index for a group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  UnlockedPads        DWORD bitmask for pads which are going to be left unlocked
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
  )
{
  GPIO_UNLOCK_HOB_DATA *GpioUnlockData;
  UINT32               Length;
  UINT32               Index;

  if (UnlockedPads == 0) {
    //
    // No pads to be left unlocked
    //
    return EFI_SUCCESS;
  }

  Length = GpioGetUnlockData (&GpioUnlockData);
  if (Length == 0) {
    return EFI_NOT_FOUND;
  }

  Index = GpioUnlockDataIndex (GroupIndex, DwNum);
  if (Index >= Length) {
    return EFI_INVALID_PARAMETER;
  }

  GpioUnlockData[Index].PadConfig |= UnlockedPads;
  return EFI_SUCCESS;
}

/**
  This procedure stores GPIO group data about pads which Output state needs to be unlocked.

  @param[in]  GroupIndex          GPIO group index
  @param[in]  DwNum               DWORD index for a group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  UnlockedPads        DWORD bitmask for pads which are going to be left unlocked
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
  )
{
  GPIO_UNLOCK_HOB_DATA *GpioUnlockData;
  UINT32               Length;
  UINT32               Index;

  if (UnlockedPads == 0) {
    //
    // No pads to be left unlocked
    //
    return EFI_SUCCESS;
  }

  Length = GpioGetUnlockData (&GpioUnlockData);
  if (Length == 0) {
    return EFI_NOT_FOUND;
  }

  Index = GpioUnlockDataIndex (GroupIndex, DwNum);
  if (Index >= Length) {
    return EFI_INVALID_PARAMETER;
  }

  GpioUnlockData[Index].OutputState |= UnlockedPads;
  return EFI_SUCCESS;
}

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
  )
{
  GPIO_UNLOCK_HOB_DATA *GpioUnlockData;
  UINT32               Length;
  UINT32               Index;

  Length = GpioLocateUnlockData (&GpioUnlockData);
  if (Length == 0) {
    return 0;
  }

  Index = GpioUnlockDataIndex (GroupIndex, DwNum);
  if (Index >= Length) {
    return 0;
  }

  return GpioUnlockData[Index].PadConfig;
}

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
  )
{
  GPIO_UNLOCK_HOB_DATA *GpioUnlockData;
  UINT32               Length;
  UINT32               Index;

  Length = GpioLocateUnlockData (&GpioUnlockData);
  if (Length == 0) {
    return 0;
  }

  Index = GpioUnlockDataIndex (GroupIndex, DwNum);
  if (Index >= Length) {
    return 0;
  }

  return GpioUnlockData[Index].OutputState;
}


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
  )
{
  return ((GpioGetChipsetIdFromGpioPad (GpioPad) == GpioGetThisChipsetId ()) &&
         (GpioGetGroupIndexFromGpioPad (GpioPad) < GpioGetNumberOfGroups ()));
}



/**
  This procedure will get number of pads for certain GPIO group

  @param[in] Group            GPIO group number

  @retval Value               Pad number for group
                              If illegal group number then return 0
**/
UINT32
EFIAPI
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
EFIAPI
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
EFIAPI
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
EFIAPI
GpioGetHighestGroup (
  VOID
  )
{
  return GpioGetGroupFromGroupIndex (GpioGetNumberOfGroups () - 1);
}


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
EFIAPI
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
  GpioSbuPadTable = GpioGetTypeCSbuGpioPad(&MaxIndex);
  if (GpioSbuPadTable == NULL || MaxIndex == 0) {
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
