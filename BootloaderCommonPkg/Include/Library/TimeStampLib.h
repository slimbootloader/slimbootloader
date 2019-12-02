/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __TIME_STAMP_LIB_H__
#define __TIME_STAMP_LIB_H__

/**
  Read current timestamp.

  @return  64 bit current timestampe value.

**/
UINT64
EFIAPI
ReadTimeStamp (
  VOID
  );

/**
  Get timestamp frequency in KHZ.

  @return   Timestamp frequency in KHZ.

**/
UINT32
EFIAPI
GetTimeStampFrequency (
  VOID
  );

#endif
