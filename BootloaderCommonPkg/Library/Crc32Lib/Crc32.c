/** @file
CalcuateCrc32 routine.

Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/Crc32Lib.h>

UINT32  mCrc32Table[256];
UINT32  mCrc32CastagnoliTable[256];

/**
  This internal function reverses bits for 32bit data.
  @param  Value                 The data to be reversed.
  @return                       Data reversed.
**/
UINT32
ReverseBits (
  UINT32  Value
  )
{
  UINTN   Index;
  UINT32  NewValue;

  NewValue = 0;
  for (Index = 0; Index < 32; Index++) {
    if ((Value & (1 << Index)) != 0) {
      NewValue = NewValue | (1 << (31 - Index));
    }
  }

  return NewValue;
}

/**
  Initialize CRC32 table.
  @param Type       Tpye of the polynomial to be used
  @param CrcTable   Pointer to the CRC table.
**/
VOID
RuntimeDriverInitializeCrc32Table (
  IN     CRC32_TYPE  Type,
  IN OUT UINT32      *CrcTable
  )
{
  UINTN   TableEntry;
  UINTN   Index;
  UINT32  Value;
  UINT32  Polynomial;

  if (Type == Crc32TypeCastagnoli) {
    Polynomial = 0x1EDC6F41;
  } else {
    Polynomial = 0x04C11DB7;
  }

  for (TableEntry = 0; TableEntry < 256; TableEntry++) {
    Value = ReverseBits ((UINT32) TableEntry);
    for (Index = 0; Index < 8; Index++) {
      if ((Value & 0x80000000) != 0) {
        Value = (Value << 1) ^ Polynomial;
      } else {
        Value = Value << 1;
      }
    }
    CrcTable[TableEntry] = ReverseBits (Value);
  }
}

/**
  The CalculateCrc32WithType routine.

  @param Data        - The buffer contaning the data to be processed
  @param DataSize    - The size of data to be processed
  @param Type        - Which CRC table should be used, default or Castagnoli
  @param CrcOut      - A pointer to the caller allocated UINT32 that on
                contains the CRC32 checksum of Data

  @retval EFI_SUCCESS               - Calculation is successful.
  @retval EFI_INVALID_PARAMETER     - Data / CrcOut = NULL, or DataSize = 0
**/
EFI_STATUS
EFIAPI
CalculateCrc32WithType (
  IN     UINT8       *Data,
  IN     UINTN       DataSize,
  IN     CRC32_TYPE  Type,
  IN OUT UINT32      *CrcOut
  )

{
  UINT32  Crc;
  UINTN   Index;
  UINT32  *CrcTable;
  UINT8   *Ptr;

  if (Type == Crc32TypeCastagnoli) {
    if (mCrc32CastagnoliTable[1] == 0) {
      RuntimeDriverInitializeCrc32Table (Type, mCrc32CastagnoliTable);
    }
    CrcTable = mCrc32CastagnoliTable;
  } else {
    if (mCrc32Table[1] == 0) {
      RuntimeDriverInitializeCrc32Table (Type, mCrc32Table);
    }
    CrcTable = mCrc32Table;
  }

  if ((DataSize == 0) || (Data == NULL) || (CrcOut == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Crc = 0xFFFFFFFF;
  for (Index = 0, Ptr = Data; Index < DataSize; Index++, Ptr++) {
    Crc = (Crc >> 8) ^ CrcTable[ (UINT8) Crc ^ *Ptr];
  }

  if (Type == Crc32TypeCastagnoli) {
    *CrcOut = Crc;
  } else {
    *CrcOut = Crc ^ 0xFFFFFFFF;
  }

  return EFI_SUCCESS;
}
