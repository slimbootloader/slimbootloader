/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>

/**
  Read current timestamp.

  @return  64 bit current timestampe value.

**/
UINT64
EFIAPI
ReadTimeStamp (
  VOID
  )
{
  return AsmReadTsc();
}

/**
  Get timestamp frequency in KHZ.

  @return   Timestamp frequency in KHZ.

**/
UINT32
EFIAPI
GetTimeStampFrequency (
  VOID
  )
{
  UINT8  Ratio;

  Ratio = (UINT8)(AsmReadMsr64 (0xCE) >> 8);
  if (Ratio == 0) {
    // This might be QEMU case
    Ratio = 8;
  }
  // Ratio * 100000
  return (UINT32)(Ratio * 100000);
}
