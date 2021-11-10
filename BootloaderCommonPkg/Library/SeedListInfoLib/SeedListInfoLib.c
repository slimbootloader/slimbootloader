/** @file
  This file provides helper functions to retrieve the seed data.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/SeedListInfoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLib.h>
#include <Guid/SeedInfoHobGuid.h>

#define DEBUG_SEED  0

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
  )
{
  EFI_HOB_GUID_TYPE          *GuidHob;

  GuidHob = GetNextGuidHob (&gSeedListInfoHobGuid, GetHobList());
  if (GuidHob == NULL) {
    return NULL;
  } else {
    if (Length != 0) {
      *Length = GET_GUID_HOB_DATA_SIZE (GuidHob);
    }
    return GET_GUID_HOB_DATA (GuidHob);
  }
}

/**
  This function add individual Seed Entry Data to SeedListHOB.

  @param  IN Type                 Seed info struct: svn_seed_info or rpmbseed UINT8 array.
  @param  IN Usage                If same type, is it used or dseed
  @param  IN Idx                  MAX_ENTRIES: 4. Iff Same type and Usage, which seed Idx is this: {0,1,2,3}.
  @param  IN HeciSeedSize         Size of Heci Seed length. Ex: HECI_SEED_INFO or RPMB Seed size.
  @param  IN HeciSeedDataBuffer   Actual Seed Data: SVN_SEED_INFO or RPMB Seed.

  @retval EFI_SUCCESS           Successfully Appended Seed Entry to SeedStore.
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
  )
{
  SEED_ENTRY          *SeedEntryData;
  SEED_LIST_INFO_HOB  *SeedListInfoHob;
  UINT32               SeedListHobLen;
  UINT16               NewSeedEntrySize;
  UINT32               TotalSeedEntryDataSize;
  UINT8                Index;
  UINT32               Offset;

  // Get Seed List HOB
  SeedListInfoHob = GetSeedListInfoHOB(&SeedListHobLen);
  if ((SeedListInfoHob == NULL) || (SeedListHobLen == 0)) {
    return EFI_NOT_FOUND;
  }

  NewSeedEntrySize = HeciSeedSize + sizeof(SEED_ENTRY);
  NewSeedEntrySize = ALIGN_UP( NewSeedEntrySize, sizeof(UINTN));

  // Check the existing SeedStore to see if new seed data fits into the Buffersize
  Offset        = sizeof(SEED_LIST_INFO_HOB);
  SeedEntryData = (SEED_ENTRY  *)((UINT8 *)SeedListInfoHob + Offset); //Start of SeedEntryData buffer

  // This is to get to the 'last' seed entry data pointer that was added so far
  for (Index = 0; Index <= SeedListInfoHob->TotalSeedCount; Index++) {
    SeedEntryData  = (SEED_ENTRY *)((UINT8 *)SeedEntryData + SeedEntryData->SeedEntrySize);
  }

  TotalSeedEntryDataSize = ((UINT32)(UINTN)SeedEntryData + (UINT32)NewSeedEntrySize - (UINT32)(UINTN)SeedListInfoHob);

  if ( TotalSeedEntryDataSize > SeedListInfoHob->BufferSize) {
    DEBUG ((DEBUG_WARN, "SeedLib HOB: Out of resources\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  SeedEntryData->Type   = Type;
  SeedEntryData->Usage  = Usage;
  SeedEntryData->Idx    = Idx;
  SeedEntryData->SeedEntrySize  = NewSeedEntrySize;
  if ( HeciSeedDataBuffer != NULL) {
    CopyMem(SeedEntryData->Seed, HeciSeedDataBuffer, HeciSeedSize );
  }

  SeedListInfoHob->TotalSeedCount += 1;

  return EFI_SUCCESS;
}

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
  )
{
  SEED_LIST_INFO_HOB  *SeedListInfoHob;
  SEED_ENTRY          *SeedEntryData;
  UINT32               SeedListHobLen;
  UINT8                Loop;
  EFI_STATUS           Status;
  UINT32               Offset;
#if DEBUG_SEED
  SVN_SEED_INFO       *SvnSeedInfo;
  UINT8               *RpmbSeedInfo;
  UINT8                Index;
#endif

  Status = EFI_NOT_FOUND;

  // Get Seed List HOB
  SeedListInfoHob = GetSeedListInfoHOB(&SeedListHobLen);
  if ((SeedListInfoHob == NULL) || (SeedListHobLen == 0)) {
    return EFI_NOT_FOUND;
  }

  if (SeedListInfoHob->TotalSeedCount == 0) {
    DEBUG ((DEBUG_WARN, "No seeds found\n"));
    return EFI_INVALID_PARAMETER;
  }

  Offset        = sizeof(SEED_LIST_INFO_HOB);
  SeedEntryData = (SEED_ENTRY  *)((UINT8 *)SeedListInfoHob + Offset); //Start of SeedEntryData buffer

  for (Loop = 0; Loop < SeedListInfoHob->TotalSeedCount; Loop++) {
    if((Type == SeedEntryData->Type) && (Usage == SeedEntryData->Usage)){
      if(Idx == SeedEntryData->Idx){
#if DEBUG_SEED
        if(Type == SEED_ENTRY_TYPE_SVNSEED) {
          SvnSeedInfo = (SVN_SEED_INFO *)SeedEntryData->Seed;
          DEBUG ((DEBUG_INFO, "SvnSeedInfo.CseSvn:0x%x\n",SvnSeedInfo->CseSvn));
          DEBUG ((DEBUG_INFO, "SvnSeedInfo.Seed: \n"));
          for (Index = 0; Index < BOOTLOADER_SEED_LEN; Index++) {
            DEBUG ((DEBUG_INFO, "%02X", SvnSeedInfo->Seed[Index]));
          }
          DEBUG ((DEBUG_INFO, "\n"));
        } else {
          RpmbSeedInfo = (UINT8 *)SeedEntryData->Seed;
          DEBUG ((DEBUG_INFO, "RpmbSeedInfo.Seed: \n"));
          for (Index = 0; Index < BOOTLOADER_SEED_LEN; Index++) {
            DEBUG ((DEBUG_INFO, "%02X", RpmbSeedInfo[Index]));
        }
        DEBUG ((DEBUG_INFO, "\n"));
        }
#endif
        if ( HeciSeedDataBuffer != NULL) {
          CopyMem(HeciSeedDataBuffer, SeedEntryData->Seed, HeciSeedSize );
          Status = EFI_SUCCESS;
        }
        break;
      }
    }
    SeedEntryData  = (SEED_ENTRY *)((UINT8 *)SeedEntryData + SeedEntryData->SeedEntrySize);
  }

  return Status;
}
