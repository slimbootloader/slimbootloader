/** @file
Header file for CalcuateCrc32 routine

Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CRC32_H_
#define _CRC32_H_

#include <PiPei.h>

typedef enum {
  Crc32TypeDefault,
  Crc32TypeCastagnoli,
  Crc32TypeMax
} CRC32_TYPE;

/**
  The CalculateCrc32WithType routine.
  @param  Data        - The buffer contaning the data to be processed
  @param  DataSize    - The size of data to be processed
  @param  Type        - Which CRC table should be used, default or Castagnoli
  @param  CrcOut      - A pointer to the caller allocated UINT32 that on
                contains the CRC32 checksum of Data

  @retval EFI_SUCCESS               - Calculation is successful.
  @retval EFI_INVALID_PARAMETER     - Data / CrcOut = NULL, or DataSize = 0

**/
EFI_STATUS
EFIAPI
CalculateCrc32WithType (
  IN  UINT8                             *Data,
  IN  UINTN                             DataSize,
  IN  CRC32_TYPE                        Type,
  IN OUT UINT32                         *CrcOut
  );

#endif
