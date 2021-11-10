/** @file
  The internal header file includes the common header files, defines
  internal structure and functions used by PeiVariable module.

Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _UEFI_VARIABLE_H_
#define _UEFI_VARIABLE_H_

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>

#define UEFI_VARIABLE_INDEX_TABLE_VOLUME 122
///
/// Variable data start flag.
///
#define UEFI_VARIABLE_DATA                     0x55AA
///
/// Alignment of Variable Data Header in Variable Store region.
///
#define HEADER_ALIGNMENT  4
#define HEADER_ALIGN(Header)  (((UINTN) (Header) + HEADER_ALIGNMENT - 1) & (~(HEADER_ALIGNMENT - 1)))
///
/// Variable Store Header Format and State.
///
#define UEFI_VARIABLE_STORE_FORMATTED          0x5a
#define UEFI_VARIABLE_STORE_HEALTHY            0xfe
///
/// Variable State flags.
///
#define UEFI_VAR_IN_DELETED_TRANSITION     0xfe  ///< Variable is in obsolete transition.
#define UEFI_VAR_DELETED                   0xfd  ///< Variable is obsolete.
#define UEFI_VAR_HEADER_VALID_ONLY         0x7f  ///< Variable header has been valid.
#define UEFI_VAR_ADDED                     0x3f  ///< Variable has been completely added.

///
/// Alignment of variable name and data, according to the architecture:
/// * For IA-32 and Intel(R) 64 architectures: 1.
///
#define ALIGNMENT         1

//
// GET_PAD_SIZE calculates the miminal pad bytes needed to make the current pad size satisfy the alignment requirement.
//
#if (ALIGNMENT == 1)
#define GET_PAD_SIZE(a) (0)
#else
#define GET_PAD_SIZE(a) (((~a) + 1) & (ALIGNMENT - 1))
#endif

typedef enum {
  VariableStoreTypeHob,
  VariableStoreTypeNv,
  VariableStoreTypeMax
} UEFI_VARIABLE_STORE_TYPE;

///
/// Status of Variable Store Region.
///
typedef enum {
  EfiRaw,
  EfiValid,
  EfiInvalid,
  EfiUnknown
} UEFI_VARIABLE_STORE_STATUS;

#pragma pack(1)

///
/// Single Variable Data Header Structure.
///
typedef struct {
  ///
  /// Variable Data Start Flag.
  ///
  UINT16      StartId;
  ///
  /// Variable State defined above.
  ///
  UINT8       State;
  UINT8       Reserved;
  ///
  /// Attributes of variable defined in UEFI specification.
  ///
  UINT32      Attributes;
  ///
  /// Size of variable null-terminated Unicode string name.
  ///
  UINT32      NameSize;
  ///
  /// Size of the variable data without this header.
  ///
  UINT32      DataSize;
  ///
  /// A unique identifier for the vendor that produces and consumes this varaible.
  ///
  EFI_GUID    VendorGuid;
} UEFI_VARIABLE_HEADER;


///
/// Variable Store region header.
///
typedef struct {
  ///
  /// Variable store region signature.
  ///
  EFI_GUID  Signature;
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
  UINT16  Reserved;
  UINT32  Reserved1;
} UEFI_VARIABLE_STORE_HEADER;

///
/// Use this data structure to store variable-related info, which can decrease
/// the cost of access to NV.
///
typedef struct {
  UINT16                Length;
  UINT16                GoneThrough;
  UEFI_VARIABLE_HEADER *EndPtr;
  UEFI_VARIABLE_HEADER *StartPtr;
  ///
  /// This field is used to store the distance of two neighbouring VAR_ADDED type variables.
  /// The meaning of the field is implement-dependent.
  UINT16          Index[UEFI_VARIABLE_INDEX_TABLE_VOLUME];
} UEFI_VARIABLE_INDEX_TABLE;

//
// FTW Last write data. It will be used as gEdkiiFaultTolerantWriteGuid GUID hob data.
//
typedef struct {
  ///
  /// Target address to be updated in FTW last write.
  ///
  EFI_PHYSICAL_ADDRESS      TargetAddress;
  ///
  /// Spare address to back up the updated buffer.
  ///
  EFI_PHYSICAL_ADDRESS      SpareAddress;
  ///
  /// The length of data that have been backed up in spare block.
  /// It is also the length of target block that has been erased.
  ///
  UINT64                    Length;
} UEFI_FAULT_TOLERANT_WRITE_LAST_WRITE_DATA;

typedef struct {
  UEFI_VARIABLE_STORE_HEADER                   *VariableStoreHeader;
  UEFI_VARIABLE_INDEX_TABLE                    *IndexTable;
  //
  // If it is not NULL, it means there may be an inconsecutive variable whose
  // partial content is still in NV storage, but another partial content is backed up
  // in spare block.
  //
  UEFI_FAULT_TOLERANT_WRITE_LAST_WRITE_DATA    *FtwLastWriteData;
  BOOLEAN                                       AuthFlag;
} UEFI_VARIABLE_STORE_INFO;

///
/// Single Authenticated Variable Data Header Structure.
///
typedef struct {
  ///
  /// Variable Data Start Flag.
  ///
  UINT16      StartId;
  ///
  /// Variable State defined above.
  ///
  UINT8       State;
  UINT8       Reserved;
  ///
  /// Attributes of variable defined in UEFI specification.
  ///
  UINT32      Attributes;
  ///
  /// Associated monotonic count value against replay attack.
  ///
  UINT64      MonotonicCount;
  ///
  /// Associated TimeStamp value against replay attack.
  ///
  EFI_TIME    TimeStamp;
  ///
  /// Index of associated public key in database.
  ///
  UINT32      PubKeyIndex;
  ///
  /// Size of variable null-terminated Unicode string name.
  ///
  UINT32      NameSize;
  ///
  /// Size of the variable data without this header.
  ///
  UINT32      DataSize;
  ///
  /// A unique identifier for the vendor that produces and consumes this varaible.
  ///
  EFI_GUID    VendorGuid;
} UEFI_AUTHENTICATED_VARIABLE_HEADER;

typedef struct {
  UEFI_VARIABLE_HEADER *CurrPtr;
  UEFI_VARIABLE_HEADER *EndPtr;
  UEFI_VARIABLE_HEADER *StartPtr;
} UEFI_VARIABLE_POINTER_TRACK;

//
// Library Data to be stored for access across tages
//
typedef struct {
  UEFI_FAULT_TOLERANT_WRITE_LAST_WRITE_DATA   FtwLastWriteData;
  UEFI_VARIABLE_INDEX_TABLE                   IndexTable;
  UEFI_VARIABLE_HEADER                        VariableHdr;
  BOOLEAN                                     StoreLibVarHdrSet;
} UEFI_VAR_STORE_LIBRARY_DATA;

#pragma pack()

//
// Functions
//
/**
  This service retrieves a variable's value using its name and GUID.

  Read the specified variable from the UEFI variable store. If the Data
  buffer is too small to hold the contents of the variable, the error
  EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the required buffer
  size to obtain the data.

  @param  This                  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
  @param  VariableName          A pointer to a null-terminated string that is the variable's name.
  @param  VariableGuid          A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                VariableGuid and VariableName must be unique.
  @param  Attributes            If non-NULL, on return, points to the variable's attributes.
  @param  DataSize              On entry, points to the size in bytes of the Data buffer.
                                On return, points to the size of the data returned in Data.
  @param  Data                  Points to the buffer which will hold the returned variable value.
                                May be NULL with a zero DataSize in order to determine the size of the buffer needed.

  @retval EFI_SUCCESS           The variable was read successfully.
  @retval EFI_NOT_FOUND         The variable was not found.
  @retval EFI_BUFFER_TOO_SMALL  The DataSize is too small for the resulting data.
                                DataSize is updated with the size required for
                                the specified variable.
  @retval EFI_INVALID_PARAMETER VariableName, VariableGuid, DataSize or Data is NULL.
  @retval EFI_DEVICE_ERROR      The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
UefiGetVariable (
  IN CONST  CHAR16                          *VariableName,
  IN CONST  EFI_GUID                        *VariableGuid,
  OUT       UINT32                          *Attributes,
  IN OUT    UINTN                           *DataSize,
  OUT       VOID                            *Data OPTIONAL
  );

#endif
