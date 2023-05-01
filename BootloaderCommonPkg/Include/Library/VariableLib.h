/** @file
  Lite variable service library

  Copyright (c) 2017 -2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _VAIRABLE_LIB_H_
#define _VAIRABLE_LIB_H_


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
EFI_STATUS
EFIAPI
GetVariable (
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
EFI_STATUS
EFIAPI
GetNextVariableName (
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
EFI_STATUS
EFIAPI
SetVariable (
  IN CHAR16                 *VariableName,
  IN EFI_GUID               *VariableGuid OPTIONAL,
  IN UINT32                 Attributes OPTIONAL,
  IN UINTN                  DataSize,
  IN VOID                   *Data
  );

/**

  This code returns information about the variables.

  @param Attributes                     Variable store type. It is not used.
  @param MaxVariableStorageSize         Pointer to the maximum size of the storage space available.
  @param RemainingStorageSize           Pointer to the remaining size of the storage space available.
  @param MaxVariableSize                Pointer to the maximum size of an individual EFI variables.


  @return EFI_INVALID_PARAMETER         An invalid combination of attribute bits was supplied.
  @return EFI_SUCCESS                   Query successfully.
  @return EFI_UNSUPPORTED               The attribute is not supported on this platform.

**/
EFI_STATUS
EFIAPI
QueryVariableInfo (
  IN  UINT32  Attributes OPTIONAL,
  OUT UINT64  *MaxVariableStorageSize,
  OUT UINT64  *RemainingStorageSize,
  OUT UINT64  *MaxVariableSize
  );

/**
  Initialize an varaible instance.
  Base needs to be 4KB aligned and Size needs to be 8KB aligned.

  @param     Base         Variable storage region base
  @param     Size         Variable storage region size
  @retval    EFI_SUCCESS  The instance was properly initialized
  @retval    EFI_INVALID_PARAMETER      Base or size is invalid
**/
EFI_STATUS
EFIAPI
VariableConstructor (
  IN  UINT32    Base,
  IN  UINT32    Size
  );

#endif
