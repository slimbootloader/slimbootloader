/** @file
  The variable data structures.

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __VARIABLE_FORMAT_H__
#define __VARIABLE_FORMAT_H__

#define  VARIABLE_STORE_SIZE             FixedPcdGet32(PcdVariableStoreSize)

#define  VARIABLE_STORE_SIGNATURE        SIGNATURE_32('V', 'A', 'R', 'S')

#define  VARIABLE_NAME_MAX_LEN           64

///
/// Variable data start flag.
///
#define VARIABLE_DATA                    0xA5

///
/// Variable State flags.
///
#define VAR_HEADER_VALID                 0x01
#define VAR_DATA_VALID                   0x02
#define VAR_IN_MIGRATION                 0x04
#define VAR_DELETED                      0x08

///
/// Variable Attribute combinations.
///
#define VAR_ATTR_READ_ONLY               0x10

///
/// Variable state macros
///
#define IS_HEADER_VALID(State)          (((State) & VAR_HEADER_VALID) == 0)
#define IS_DATA_VALID(State)            (((State) & VAR_DATA_VALID) == 0)
#define IS_IN_MIGRATION(State)          (((State) & VAR_IN_MIGRATION) == 0)
#define IS_DELETED(State)               (((State) & VAR_DELETED) == 0)

///
/// Variable Store region header.
///
typedef struct {
  ///
  /// Variable store region signature.
  ///
  UINT32  Signature;
  ///
  /// Size of entire variable store,
  /// including size of variable store header but not including the size of FvHeader.
  ///
  UINT32  Size;
  ///
  /// Variable region format state.
  ///
  UINT8   Format;
  ///
  /// Variable region healthy state.
  ///
  UINT8   State;
  UINT8   Reserved[6];
} VARIABLE_STORE_HEADER;

///
/// Single Variable Data Header Structure.
///
typedef struct {
  ///
  /// Variable Data Start Flag.
  ///
  UINT8       StartId;
  ///
  /// Variable State defined above.
  ///
  UINT8       State;
  ///
  /// Size of the variable data.
  ///
  UINT16      DataSize;
  ///
  /// Variable GUID
  ///
  EFI_GUID    VariableGuid;
} VARIABLE_HEADER;

#endif
