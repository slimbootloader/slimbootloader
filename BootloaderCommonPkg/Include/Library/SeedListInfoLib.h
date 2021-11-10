/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
 * SeedListInfoLib.h
 */
#ifndef _SEED_LIST_INFO_LIB_H_
#define _SEED_LIST_INFO_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <Guid/SeedInfoHobGuid.h>

/**
  Get Seed List Info from HOB.

  @param[out] Length     A pointer to the GUID HOB data buffer length.  If the
                         GUID HOB is located, the length will be updated.

  @retval     NULL       Failed to find the GUID HOB.
  @retval     Guid       A pointer to HOB GUID.
**/
SEED_LIST_INFO_HOB*
EFIAPI
GetSeedListInfoHOB (
  UINT32           *Length
  );

/**
  This function add individual Seed Entry Data to SeedListHOB.

  @param  IN Type                 Seed info struct: svn_seed_info or rpmbseed UINT8 array.
  @param  IN Usage                If same type, is it used or dseed
  @param  IN Idx                  MAX_ENTRIES: 4. Iff Same type and Usage, which seed Idx is this: {0,1,2,3}.
  @param  IN HeciSeedSize         Size of Heci Seed length. Ex: HECI_SEED_INFO or RPMB Seed size.
  @param  IN HeciSeedDataBuffer   Actual Seed Data: SVN_SEED_INFO or RPMB Seed.

  @retval EFI_NOT_FOUND         Failed to find the GUID HOB.
  @retval EFI_OUT_OF_RESOURCES  GUID HOB data buffer is out of space.

**/
EFI_STATUS
EFIAPI
AppendSeedData (
  IN UINT8 Type,
  IN UINT8 Usage,
  IN UINT8 Idx,
  IN UINT16 HeciSeedSize,
  IN UINT8 *HeciSeedDataBuffer
  );

/**
  This function add individual Seed Entry Data to SeedListHOB.

  @param  IN  Type                 Seed info struct: svn_seed_info or rpmbseed UINT8 array.
  @param  IN  Usage                If same type, is it used or dseed
  @param  IN  Idx                  MAX_ENTRIES: 4. Iff Same type and Usage, which seed Idx is this: {0,1,2,3}.
  @param  IN  HeciSeedSize         Size of Heci Seed length. Ex: HECI_SEED_INFO or RPMB Seed size.
  @param  OUT HeciSeedDataBuffer   Actual Seed Data: SVN_SEED_INFO or RPMB Seed to be returned.

  @retval EFI_NOT_FOUND             Failed to find the GUID HOB.
  @retval EFI_INVALID_PARAMETER     Failed to retrieve TotalSeedCount or TotalSeedCount is zero.
  @retval EFI_NOT_FOUND             Failed to find the Seed Entry from the GUID HOB data buffer.

**/
EFI_STATUS
EFIAPI
GetSeedData (
  IN UINT8 Type,
  IN UINT8 Usage,
  IN UINT8 Idx,
  IN UINT16 HeciSeedSize,
  OUT UINT8 *HeciSeedDataBuffer
  );

#endif
