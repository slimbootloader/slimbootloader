/** @file
  Definition of BDAT data structures that are applicible for all schemas


  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef BDAT4_H_
#define BDAT4_H_

#include <Uefi/UefiBaseType.h>


#pragma pack(push, 1)

typedef struct {
  UINT8                       BiosDataSignature[8];  ///< "BDATHEAD"
  UINT32                      BiosDataStructSize;    ///< sizeof BDAT_STRUCTURE
  UINT16                      Crc16;                 ///< 16-bit CRC of BDAT_STRUCTURE (calculated with 0 in this field)
  UINT16                      Reserved;
  union {
    struct {
      UINT16                  Primary;
      UINT16                  Secondary;
    } Rev;
    UINT32                    Data32;
    UINT16                    Data16[2];
    UINT8                     Data8[4];
  }                           Version;               ///< Version, primary then secondary.
  UINT32                      OemOffset;             ///< Optional offset to OEM-defined structure
  UINT32                      Reserved1;
  UINT32                      Reserved2;
} BDAT_HEADER_STRUCTURE;

typedef struct {
  UINT16                      SchemaListLength;      ///< How many offsets are listed in the schema list.
  UINT16                      Reserved1;
  struct {
    UINT16                    Year;
    UINT8                     Month;
    UINT8                     Day;
    UINT8                     Hour;
    UINT8                     Minute;
    UINT8                     Second;
    UINT8                     Reserved2;
  } Time;                                            ///< Schema update time.
} BDAT_SCHEMA_LIST_STRUCTURE;

typedef struct {
  BDAT_HEADER_STRUCTURE       Header;
  BDAT_SCHEMA_LIST_STRUCTURE  Schemas;
} BDAT_STRUCTURE;


#define MAX_SCHEMA_LIST_LENGTH (10)

//
// Every BDAT schema should begin with this structure definition
//
typedef struct {
  EFI_GUID                    SchemaId;              ///< The GUID uniquely identifies the format of the data contained within the structure.
  UINT32                      DataSize;              ///< The total size of the memory block, including both the header as well as the schema specific data.
  UINT16                      Crc16;                 ///< Crc16 is computed in the same manner as the field in the BDAT_HEADER_STRUCTURE.
} BDAT_SCHEMA_HEADER_STRUCTURE;

#pragma pack (pop)

/**
  Not strictly part of the BDAT spec.  This HOB is used to identify which HOBs
  contain data that should be copied into the final BDAT structure as a schema
**/
typedef struct {
  EFI_HOB_GUID_TYPE           HobGuidType;
/**
  SchemaHobCount : Number of HOBs that exist which contain data that should be
                   copied to the BDAT structure.
**/
  UINT16                      SchemaHobCount;
  UINT16                      Reserved;
/**
  SchemaHobGuids : Array of GUIDs which identify HOBs that contain data
                   that should be copied into the BDAT structure.  The contents
                   of the HOB contains a schema.  The GUID for the HOB is the
                   same as the SchemaId GUID.
**/
  EFI_GUID                    SchemaHobGuids[MAX_SCHEMA_LIST_LENGTH];
} BDAT_SCHEMA_LIST_HOB;

#endif
