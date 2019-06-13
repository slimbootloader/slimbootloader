/** @file
  Header file for CalcuateCrc32 routine

  Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _DEBUGDATA_LIB_H_
#define _DEBUGDATA_LIB_H_

#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/DebugLib.h>
#include <Library/Crc32Lib.h>
#include <Guid/MemoryMapInfoGuid.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/PrintLib.h>

// ShowBuffer related macros
#define LINE_BUFFER        51  //To construct a line needs 51 chars.
#define END_OF_LINE_OFFSET 47

//
// S3 DEBUG related info
//

#define S3_DEBUG_CRC32_BASE         0x00000000
#define S3_DEBUG_CRC32_LIMIT        0x80000000
#define S3_DEBUG_CRC32_REGION_SIZE  0x00100000


typedef struct {
  UINT32        RegionBase;
  UINT32        RegionLimit;
  UINT32        RegionSize;
  UINT32        RegionCrc32Value;
} S3_CRC_DATA;

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
  );

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
  );


//
// General Debug related info
//

typedef struct {
  S3_CRC_DATA  *S3CrcTable;
} DEBUG_DATA;

/**
  For serial debugger, it will show the buffer message line by line to serial console.

  @param[in] Message              the address point of buffer message
  @param[in] Length               message length

**/
VOID
ShowBuffer (
  IN  UINT8                       *Message,
  IN  UINT32                      Length
  );

#endif
