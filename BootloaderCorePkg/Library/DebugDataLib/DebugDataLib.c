/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugDataLib.h>
#include <BootloaderCoreGlobal.h>

/**
  Allocate Memory for Debug Test Cases.

  Check if the DebugDataPtr is allocated memory by a
  debug test case already. If yes, do nothing. Else
  allocate memory for DebugDataPtr of size DEBUG_DATA.
**/
VOID
CheckAndAllocateDebugDataMemory (
  VOID
  )
{
  LOADER_GLOBAL_DATA   *LdrGlobal;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  if (LdrGlobal->DebugDataPtr == NULL) {
    LdrGlobal->DebugDataPtr = AllocatePool (sizeof (DEBUG_DATA));
    if (LdrGlobal->BootMode != BOOT_ON_S3_RESUME) {
      ZeroMem (LdrGlobal->DebugDataPtr, sizeof (DEBUG_DATA));
    }
  }
  // else, DebugData Memory is already
  // allocated by another test case.
}

/**
  Restore the CRC32 values saved and compare.

  Stage1B will calculate and save the CRC32 for the 1MiB memory ranges
  within Unreserved Memory Regions. Re-calculate the CRC32 for the same
  memory ranges and report the un-matched CRC regions. This routine is called
  right before jumping to S3 WakeUp trampoline.
**/
BOOLEAN
EFIAPI
S3DebugRestoreAndCompareCRC32 (
  VOID
  )
{
  LOADER_GLOBAL_DATA       *LdrGlobal;
  S3_CRC_DATA              *SavedS3CrcTable;
  UINT32                    CrcRegionIdx;
  UINT32                    NewCrcValue;
  BOOLEAN                   CrcMatching;

  CrcMatching = TRUE;
  LdrGlobal         = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  SavedS3CrcTable   = ((DEBUG_DATA *)LdrGlobal->DebugDataPtr)->S3CrcTable;

  DEBUG ((DEBUG_INFO, "Checking for unmatched-CRC Regions ...\n"));
  for (CrcRegionIdx = 0; SavedS3CrcTable[CrcRegionIdx].RegionSize != 0x00; CrcRegionIdx++) {
    CalculateCrc32WithType ((UINT8 *)(UINTN) SavedS3CrcTable[CrcRegionIdx].RegionBase,
                    (UINTN) (SavedS3CrcTable[CrcRegionIdx].RegionSize), Crc32TypeCastagnoli, &NewCrcValue);
    if (NewCrcValue != SavedS3CrcTable[CrcRegionIdx].RegionCrc32Value) {
      DEBUG ((DEBUG_INFO, "RegBase=0x%08X  RegLimit=0x%08X  RegSize=0x%08X  SavedS3CrcValue=0x%08X  NewCrcValue=0x%08X\n",
              SavedS3CrcTable[CrcRegionIdx].RegionBase,
              SavedS3CrcTable[CrcRegionIdx].RegionLimit,
              SavedS3CrcTable[CrcRegionIdx].RegionSize,
              SavedS3CrcTable[CrcRegionIdx].RegionCrc32Value,
              NewCrcValue));
      CrcMatching = FALSE;
    }
  }
  if (!CrcMatching) {
    return FALSE;
  } else {
    DEBUG ((DEBUG_INFO, "None\n"));
    return TRUE;
  }
}


/**
  Calculate CRC32 values for 1MiB memory ranges

  Calculate CRC32 values for 1MiB memory ranges
  within the UnReserved Memory Regions from MmeoryMap.
  This gets control only in S3Rseume path.

  @param S3CrcTable            Pointer to the CRC table to be stored for later comparison.
  @param S3CrcTableSize        CRC table size
  @param MemoryMapEntry        Pointer to an UnReserved Memory Region entry from MemoryMap.
  @param S3CrcRegionIdx        Address of the variable used for indexing S3CrcTable.

**/
VOID
S3DebugCalculateCRC32 (
  IN  OUT S3_CRC_DATA        *S3CrcTable,
  IN      UINT32              S3CrcTableSize,
  IN      MEMORY_MAP_ENTRY   *MemoryMapEntry,
  IN  OUT UINT32             *S3CrcRegionIdx
  )
{
  UINT32                      SubRegionBase;
  UINT32                      SubRegionSize;
  UINT32                      SubRegionCount;
  UINT32                      SubRegionIndex;
  UINT32                      CrcRegionIdx;

  CrcRegionIdx = *S3CrcRegionIdx;
  if ( CrcRegionIdx >= DivU64x32 (S3CrcTableSize, sizeof (S3_CRC_DATA)) ) {
    DEBUG ((DEBUG_INFO, "S3CrcTable to over-flow! Returning\n"));
    return;
  }

  SubRegionBase = (UINT32)MemoryMapEntry->Base;
  SubRegionSize = S3_DEBUG_CRC32_REGION_SIZE;
  SubRegionCount = (UINT32) DivU64x32 (MemoryMapEntry->Size + S3_DEBUG_CRC32_REGION_SIZE - 1, S3_DEBUG_CRC32_REGION_SIZE);

  for (SubRegionIndex = 0; SubRegionIndex < SubRegionCount; SubRegionIndex++) {
    S3CrcTable[CrcRegionIdx].RegionBase   = SubRegionBase;
    S3CrcTable[CrcRegionIdx].RegionSize   = SubRegionSize;
    if (S3CrcTable[CrcRegionIdx].RegionBase == 0x00000000) {
      S3CrcTable[CrcRegionIdx].RegionBase = 0x00000001;
      S3CrcTable[CrcRegionIdx].RegionSize -= 1;
    }
    if ( (S3CrcTable[CrcRegionIdx].RegionBase + S3CrcTable[CrcRegionIdx].RegionSize) >= ((UINT32)MemoryMapEntry->Base +
         (UINT32)MemoryMapEntry->Size) ) {
      S3CrcTable[CrcRegionIdx].RegionSize = (UINT32)MemoryMapEntry->Base + (UINT32)MemoryMapEntry->Size -
                                            S3CrcTable[CrcRegionIdx].RegionBase - 1;
    }
    S3CrcTable[CrcRegionIdx].RegionLimit = S3CrcTable[CrcRegionIdx].RegionBase + S3CrcTable[CrcRegionIdx].RegionSize - 1;
    CalculateCrc32WithType ((UINT8 *)(UINTN)S3CrcTable[CrcRegionIdx].RegionBase, (UINTN) (S3CrcTable[CrcRegionIdx].RegionSize),
                    Crc32TypeCastagnoli, & (S3CrcTable[CrcRegionIdx].RegionCrc32Value));
    DEBUG ((DEBUG_INFO, "RegBase=0x%08X RegLimit=0x%08X RegSize=0x%08X  CrcValue=0x%08X\n",
            S3CrcTable[CrcRegionIdx].RegionBase, S3CrcTable[CrcRegionIdx].RegionLimit, S3CrcTable[CrcRegionIdx].RegionSize,
            S3CrcTable[CrcRegionIdx].RegionCrc32Value ));
    CrcRegionIdx++;
    SubRegionBase += SubRegionSize;
  }
  *S3CrcRegionIdx = CrcRegionIdx;
}


/**
  Calculate and Save the CRC32 values.

  Stage1B will calculate and save the CRC32 for the 1MiB memory ranges
  for Unreserved Memory Regions. This gets control only in S3Rseume path.

  @param[in] LdrHobList   Pointer to saved/restored LdrHobList from NormalBoot path.

**/
VOID
EFIAPI
S3DebugSaveCRC32 (
  IN      VOID         *LdrHobList
  )
{
  UINT32                S3CrcRegionIdx;
  VOID                 *GuidHob;
  MEMORY_MAP_INFO      *MemoryMapInfo;
  UINT8                 MemoryMapIdx;
  LOADER_GLOBAL_DATA   *LdrGlobal;
  S3_CRC_DATA          *S3CrcTable;
  UINT32                S3CrcTableSize;

  S3CrcRegionIdx = 0;
  CheckAndAllocateDebugDataMemory ();
  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();

  // Allocating predefined amount of memory here due to lack of
  // Memory Map availablity in Stage1B in Normal Boot.
  S3CrcTableSize = sizeof (S3_CRC_DATA) * (S3_DEBUG_CRC32_LIMIT / S3_DEBUG_CRC32_REGION_SIZE);
  S3CrcTable = (S3_CRC_DATA *) AllocatePool (S3CrcTableSize);

  if (S3CrcTable != NULL) {
    if (LdrGlobal->BootMode != BOOT_ON_S3_RESUME) {
      ZeroMem (S3CrcTable, S3CrcTableSize);
    } else {
      GuidHob = GetNextGuidHob (&gLoaderMemoryMapInfoGuid, LdrHobList);
      ASSERT (GuidHob != NULL);
      MemoryMapInfo = (MEMORY_MAP_INFO *)GET_GUID_HOB_DATA (GuidHob);
      for (MemoryMapIdx = 0; MemoryMapIdx < MemoryMapInfo->Count; MemoryMapIdx++) {
        if (MemoryMapInfo->Entry[MemoryMapIdx].Base >= S3_DEBUG_CRC32_LIMIT) {
          break;
        }
        // Calculate CRC32 for this memory region if it is unreserved
        if (MemoryMapInfo->Entry[MemoryMapIdx].Type == 1) {
          S3DebugCalculateCRC32 (S3CrcTable, S3CrcTableSize, & (MemoryMapInfo->Entry[MemoryMapIdx]), &S3CrcRegionIdx);
        }
      }
    }
    ((DEBUG_DATA *)LdrGlobal->DebugDataPtr)->S3CrcTable = S3CrcTable;
  }

}

/**
  For serial debugger, it will show the buffer message line by line to serial console.

  @param[in] Message              the address point of buffer message
  @param[in] Length               message length

**/
VOID
ShowBuffer (
  IN  UINT8                       *Message,
  IN  UINT32                      Length
  )
{
  DEBUG_CODE_BEGIN ();
  UINT32                          Index;
  UINT32                          Offset;
  CHAR16                          Buffer[LINE_BUFFER];

  Index  = 0;
  Offset = 0;
  ZeroMem (Buffer, sizeof (Buffer));

  while (Length-- > 0) {
    //
    // Get the corresponding offset value from the index of buffer message.
    //
    Offset = ((Index & 0x0F) > 7) ? (((Index & 0x0F) * 3) + 2) : ((Index & 0x0F) * 3);

    //
    // Print "- " at the half of a line increases the readability of debug message.
    //
    if ((Index & 0x0F) == 0x08) {
      UnicodeSPrint (&Buffer [24], 3 * sizeof (CHAR16), L"- ");
    }

    //
    // Collect the data of buffer message.
    //
    UnicodeSPrint (&Buffer[Offset], 4 * sizeof (CHAR16), L"%02x ", Message [Index]);

    //
    // A line contains 16 bytes of buffer message. If a line is complete, it will be shown through DEBUG macro.
    //
    if (Offset == END_OF_LINE_OFFSET) {
      DEBUG ((DEBUG_INFO, "%02x: %s\n", (Index & 0xF0), Buffer));
    }

    Index++;
  }

  //
  // If a line isn't complete, show the remaining data.
  //
  if (Offset != END_OF_LINE_OFFSET) {
    DEBUG ((DEBUG_INFO, "%02x: %s\n", (Index & 0xF0), Buffer));
  }
  DEBUG_CODE_END ();
  return;
}
