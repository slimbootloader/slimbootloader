/** @file

  Copyright (c) 2018 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __VARIABLE_SERVICE_H__
#define __VARIABLE_SERVICE_H__

#include <Guid/BootLoaderServiceGuid.h>

#define VARIABLE_SERVICE_SIGNATURE  SIGNATURE_32 ('V', 'A', 'R', ' ')
#define VARIABLE_SERVICE_VERSION    1

/**

  This code gets variable in storage blocks.

  @param VariableName               Name of Variable to be found.
  @param VariableGuid               Variable GUID. Zero GUID is used if it is NULL.
  @param Attributes                 Attribute value of the variable found.
  @param DataSize                   Size of Data found. If size is less than the
                                    data, this value contains the required size.
  @param Data                       The buffer to return the contents of the variable. May be NULL
                                    with a zero DataSize in order to determine the size buffer needed.

  @retval EFI_INVALID_PARAMETER     Invalid parameter.
  @retval EFI_SUCCESS               Find the specified variable.
  @retval EFI_NOT_FOUND             Not found.
  @retval EFI_BUFFER_TOO_SMALL      DataSize is too small for the result.
  @retval EFI_VOLUME_CORRUPTED      Variable store is corrupted.

**/
typedef
EFI_STATUS
(EFIAPI *GET_VARIABLE) (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VariableGuid OPTIONAL,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data OPTIONAL
  );


/**

  This code Finds the Next available variable.

  Caution: This function may receive untrusted input.

  @param VariableNameSize           The size of the VariableName buffer. The size must be large
                                    enough to fit input string supplied in VariableName buffer.
  @param VariableName               Pointer to variable name.
  @param VariableGuid               Variable GUID.

  @retval EFI_SUCCESS               The function completed successfully.
  @retval EFI_NOT_FOUND             The next variable was not found.
  @retval EFI_BUFFER_TOO_SMALL      The VariableNameSize is too small for the result.
                                    VariableNameSize has been updated with the size needed to complete the request.
  @retval EFI_VOLUME_CORRUPTED      Variable store is corrupted.
  @retval EFI_INVALID_PARAMETER     VariableNameSize is NULL.
  @retval EFI_INVALID_PARAMETER     VariableName is NULL.
  @retval EFI_INVALID_PARAMETER     VendorGuid is NULL.
  @retval EFI_INVALID_PARAMETER     The input values of VariableName and VendorGuid are not a name and
                                    GUID of an existing variable.
  @retval EFI_INVALID_PARAMETER     Null-terminator is not found in the first VariableNameSize bytes of
                                    the input VariableName buffer.

**/
typedef
EFI_STATUS
(EFIAPI *GET_NEXT_VARIABLE_NAME) (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VariableGuid
  );


/**

  This code sets variable in storage blocks.

  @param VariableName                     Name of Variable to be found.
  @param VariableGuid                     Variable GUID. Zero GUID is used if it is NULL.
  @param Attributes                       Attribute value of the variable found
  @param DataSize                         Size of Data found. If size is less than the
                                          data, this value contains the required size.
  @param Data                             Data pointer.

  @return EFI_INVALID_PARAMETER           Invalid parameter.
  @return EFI_SUCCESS                     Set successfully.
  @return EFI_OUT_OF_RESOURCES            Resource not enough to set variable.
  @return EFI_NOT_FOUND                   Not found.
  @return EFI_WRITE_PROTECTED             Variable is read-only.
  @return EFI_DEVICE_ERROR                Variable storage is corrupted.

**/
typedef
EFI_STATUS
(EFIAPI *SET_VARIABLE) (
  IN CHAR16                 *VariableName,
  IN EFI_GUID               *VariableGuid OPTIONAL,
  IN UINT32                 Attributes OPTIONAL,
  IN UINTN                  DataSize,
  IN VOID                   *Data
  );


typedef struct {
  SERVICE_COMMON_HEADER              Header;
  GET_VARIABLE                       GetVariable;
  GET_NEXT_VARIABLE_NAME             GetNextVariableName;
  SET_VARIABLE                       SetVariable;
} VARIABLE_SERVICE;

#endif
