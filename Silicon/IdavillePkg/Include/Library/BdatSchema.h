/** @file


  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi/UefiBaseType.h>

#ifndef _bdat_h
#define _bdat_h

#pragma pack(1)


//
// Memory training Data Header
//
typedef struct {
  EFI_GUID  MemDataGuid;  // GUID that uniquely identifies the memory training data revision
  UINT32    Size;         // Total size in bytes including the header and all memory training data
  UINT32    Crc;          // 32-bit CRC generated over the whole size minus this crc field
                          // Note: UEFI 32-bit CRC implementation (CalculateCrc32)
                          // Consumers can ignore CRC check if not needed.
  UINT32    Reserved;     // Reserved for future use, must be initialized to 0
} MEM_TRAINING_DATA_HEADER;

//
// Memory Training Raw Data
//
typedef struct {
  MEM_TRAINING_DATA_HEADER  Header;

  //
  // This is a dynamic region, where memory training data entries are filled out.
  //
} MEM_TRAINING_DATA_STRUCTURE;

typedef struct {
  BDAT_SCHEMA_HEADER_STRUCTURE  SchemaHeader;
  MEM_TRAINING_DATA_STRUCTURE   Data;
} BDAT_MEM_TRAINING_STRUCTURE;

//
// Memory training data HOB header
// This header contains the actual size of the training data in the HOB. The HOB data size is
// always mutiples of 8.
//
typedef struct {
  UINT32    Size;
} MEM_TRAINING_DATA_HOB_HEADER;


///
/// Enhanced Warning Log Header
///
typedef struct {
  EFI_GUID  EwlGuid;      /// GUID that uniquely identifies the EWL revision
  UINT32    Size;         /// Total size in bytes including the header and buffer
  UINT32    FreeOffset;   /// Offset of the beginning of the free space from byte 0
                          /// of the buffer immediately following this structure
                          /// Can be used to determine if buffer has sufficient space for next entry
  UINT32    Crc;          /// 32-bit CRC generated over the whole size minus this crc field
                          /// Note: UEFI 32-bit CRC implementation (CalculateCrc32) (References [7])
                          /// Consumers can ignore CRC check if not needed.
  UINT32    Reserved;     /// Reserved for future use, must be initialized to 0
} EWL_HEADER;

///
/// Enhanced Warning Log Spec defined data log structure
///
typedef struct {
  EWL_HEADER Header;          /// The size will vary by implementation and should not be assumed
  UINT8      Buffer[4 * 1024];  /// The spec requirement is that the buffer follow the header
} EWL_PUBLIC_DATA;

///
/// EWL private data structure.  This is going to be implementation dependent
///
typedef struct {
  UINT32            Rsvd1;  //
  UINT32            Rsvd2;  //
  EWL_PUBLIC_DATA   Status; // Spec defined EWL
} EWL_PRIVATE_DATA;

#pragma pack()
#endif // _bdat_h
