/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <FspVariableServicesLibInternal.h>

/**
  Form the concatenated GUID + Varname string used by this LiteVariable wrapper.

  @param VariableName    Input unicode Variable name provided by FSP
  @param VariableGuid    Input Variable GUID provded by FSP
  @param FspVarName      Output ASCII Variable Name used by LiteVariable
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FormFspVarName (
  IN CHAR16   *VariableName,
  IN EFI_GUID *VariableGuid,
  OUT CHAR8   **FspVarName
  )
{
  CHAR8           *AsciiVarName;
  UINTN           AsciiVarNameLength;

  if (VariableName == NULL
    || VariableGuid == NULL
    || FspVarName == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  // Lite Variable doesn't support GUID natively, so we'll form
  // the ASCII name from the ASCII GUID contcatenated with Unicode name.
  AsciiVarNameLength = StrLen(VariableName);
  // number of characters in an ascii guid, plus null terminator
  AsciiVarNameLength += ASCII_GUID_LENGTH + 1;

  AsciiVarName = AllocateTemporaryMemory(AsciiVarNameLength * sizeof(CHAR8));
  ASSERT(AsciiVarName != NULL);
  if (AsciiVarName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  AsciiSPrint(AsciiVarName, AsciiVarNameLength, "%g%s", VariableGuid, VariableName);
  *FspVarName = AsciiVarName;
  return EFI_SUCCESS;
}

/**
  Split the ASCII name returned by LiteVariable calls into GUID and Unicode name expected by FSP.

  @param FspVarName          ASCII combined variable name returned from LiteVariableLib
  @param VariableNameSize    Max number of bytes VariableName can hold, including null-terminator
  @param VariableName
  @param VariableGuid
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
SplitFspVarName (
  IN CHAR8      *FspVarName,
  IN OUT UINTN  *VariableNameSize,
  OUT CHAR16    *VariableName,
  OUT EFI_GUID  *VariableGuid
  )
{
  EFI_STATUS  Status;
  UINTN       FspVarNameLength;
  CHAR8       AsciiGuid[MAXIMUM_VALUE_CHARACTERS];

  if (FspVarName == NULL
    || *FspVarName == '\0'
    || VariableNameSize == NULL
    || VariableName == NULL
    || VariableGuid == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  FspVarNameLength = AsciiStrLen(FspVarName);
  // Make sure FspVarName contains at least one character after GUID
  if (FspVarNameLength <= ASCII_GUID_LENGTH) {
    return EFI_INVALID_PARAMETER;
  }

  // Output buffer needs to hold FspVarName minus ASCII GUID size (including null)
  if (*VariableNameSize < sizeof(CHAR16)*(FspVarNameLength - ASCII_GUID_LENGTH + 1)) {
    *VariableNameSize = sizeof(CHAR16)*(FspVarNameLength - ASCII_GUID_LENGTH + 1);
    return EFI_BUFFER_TOO_SMALL;
  }

  Status = AsciiStrnCpyS(AsciiGuid, MAXIMUM_VALUE_CHARACTERS, FspVarName,ASCII_GUID_LENGTH);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  DEBUG((DEBUG_VERBOSE, "Extracted ASCII GUID %a\n", AsciiGuid));

  Status = AsciiStrToGuid(AsciiGuid, VariableGuid);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  DEBUG((DEBUG_VERBOSE, "Extracted GUID %g\n", VariableGuid));

  Status = AsciiStrToUnicodeStrS(&FspVarName[ASCII_GUID_LENGTH],VariableName,*VariableNameSize);
  ASSERT_EFI_ERROR(Status);
  DEBUG((DEBUG_VERBOSE, "Extracted VarName %s\n", VariableName));
  return Status;
}

/**
  FSP wrapper for SBL's Get Variable implementation.

  @param VariableName
  @param VariableGuid
  @param Attributes
  @param DataSize
  @param Data
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspGetVariable (
  IN  CHAR16      *VariableName,
  IN  EFI_GUID    *VariableGuid,
  OUT UINT32      *Attributes,
  IN OUT UINT64   *DataSize,
  IN  VOID        *Data
  )
{
  EFI_STATUS                Status;
  CHAR8                     *LiteVarName;
  UINTN                     LiteVarSize = 0;
  FSP_LITE_VARIABLE_HEADER  *VarHeader;

  if (VariableName == NULL
    || DataSize == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = FormFspVarName(VariableName, VariableGuid, &LiteVarName);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DEBUG((DEBUG_VERBOSE, "FspGetVariable: Passing %a to LiteVariableLib\n", LiteVarName));

  // Get variable size
  Status = GetVariable(LiteVarName, NULL, &LiteVarSize, NULL);
  // We expect buffer too small. Anything else means there was a problem with name/guid.
  if (Status != EFI_BUFFER_TOO_SMALL) {
    FreeTemporaryMemory(LiteVarName);
    return Status;
  }

  // LiteVariable stored data is encapsulated with a header for storing unsupported data.
  // We expect variable size to be passed in datasize plus FSP_LITE_VARIABLE_HEADER size
  if (LiteVarSize > *DataSize + sizeof(FSP_LITE_VARIABLE_HEADER)) {
    *DataSize = LiteVarSize - sizeof(FSP_LITE_VARIABLE_HEADER);
    FreeTemporaryMemory(LiteVarName);
    return EFI_BUFFER_TOO_SMALL;
  }

  // passed in data size is good. Allocate a pool and Get encapsulated variable.
  VarHeader = AllocateTemporaryMemory(LiteVarSize);
  ASSERT(VarHeader != NULL);
  if (VarHeader == NULL) {
    FreeTemporaryMemory(LiteVarName);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GetVariable(LiteVarName, NULL, &LiteVarSize, (VOID*)VarHeader);
  // Done with Var Name now.
  FreeTemporaryMemory(LiteVarName);
  if (EFI_ERROR(Status))
  {
    FreeTemporaryMemory(VarHeader);
    return Status;
  }
  // Sanity check returned data
  ASSERT(VarHeader->Signature == FSP_LITE_VAR_SIGNATURE);
  if (VarHeader->Signature != FSP_LITE_VAR_SIGNATURE)
  {
    FreeTemporaryMemory(VarHeader);
    return EFI_NOT_FOUND;
  }
  if (Data != NULL)
    CopyMem(Data, VarHeader->Data, LiteVarSize - sizeof(FSP_LITE_VARIABLE_HEADER));

  if (Attributes != NULL)
    *Attributes = VarHeader->Attributes;

  FreeTemporaryMemory(VarHeader);
  return EFI_SUCCESS;
}

/**
  FSP wrapper for SBL's Get Next Variable implementation.
  This must always be called successively without any other
  LiteVariable GetNextVariablName calls in between.

  @param VariableNameSize
  @param VariableName
  @param VariableGuid
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspGetNextVariableName (
  IN OUT  UINT64   *VariableNameSize,
  OUT     CHAR16   *VariableName,
  OUT     EFI_GUID *VariableGuid
  )
{
  EFI_STATUS                Status;
  static UINTN              VariableKey = 0;
  UINTN                     OldKey;
  UINTN                     LiteVarNameSize;
  UINTN                     NameSize;
  CHAR8                     *LiteVarName="";
  FSP_LITE_VARIABLE_HEADER  *VarHeader=NULL;
  UINTN                     VarSize;

  DEBUG((DEBUG_VERBOSE, "FspGetNextVariableName Entry\n"));

  if (VariableNameSize == NULL
    || ((VariableName == NULL
        || VariableGuid == NULL)
      && *VariableNameSize != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG((DEBUG_VERBOSE, "Parameter Check Pass\n"));

  // Reset variable key when FSP indicates first call.
  if (*VariableNameSize == sizeof(CHAR16) && *VariableName == L'\0') {
    VariableKey = 0;
  }

  NameSize = (UINTN)*VariableNameSize;
  DEBUG((DEBUG_VERBOSE, "Loop Init Done\n"));
  // Need to loop through LiteVariable GetNextVariableName and only return FSP created variables.
  do {
    DEBUG((DEBUG_VERBOSE, "Key: %d\n", VariableKey));
    LiteVarNameSize = 0;
    OldKey = VariableKey;
    Status = GetNextVariableName(&LiteVarNameSize, LiteVarName, &VariableKey);
    DEBUG((DEBUG_VERBOSE, "GetNextVariableName Status %r, Size 0x%X, Name %a\n", Status, LiteVarNameSize, LiteVarName));
    if (Status != EFI_BUFFER_TOO_SMALL) {
      return Status;
    }

    LiteVarName = AllocateTemporaryMemory(LiteVarNameSize);
    ASSERT(LiteVarName != NULL);
    if (LiteVarName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = GetNextVariableName(&LiteVarNameSize, LiteVarName, &VariableKey);
    DEBUG((DEBUG_VERBOSE, "GetNextVariableName Status %r, Size 0x%X, Name %a\n", Status, LiteVarNameSize, LiteVarName));
    if (Status == EFI_SUCCESS) {
      VarSize = 0;
      VarHeader = NULL;
      Status = GetVariable(LiteVarName, NULL, &VarSize, (VOID*)VarHeader);
      DEBUG((DEBUG_VERBOSE, "GetVariable Status %r, Size 0x%X, Name %a\n", Status, VarSize, LiteVarName));
      ASSERT(Status == EFI_BUFFER_TOO_SMALL);
      if (Status == EFI_BUFFER_TOO_SMALL
        && VarSize >= sizeof(VarHeader))
      {
        // Now we know the size. Allocate and get the variable.
        VarHeader = AllocateTemporaryMemory(VarSize);
        Status = GetVariable(LiteVarName, NULL, &VarSize, (VOID*)VarHeader);
        DEBUG((DEBUG_VERBOSE, "GetVariable Status %r, Size 0x%X, Name %a\n", Status, VarSize, LiteVarName));
        ASSERT_EFI_ERROR(Status);
        // Did we successfully get an FSP variable?
        if (!EFI_ERROR(Status)
          && (VarHeader->Signature == FSP_LITE_VAR_SIGNATURE))
        {
          // Don't need the actual variable anymore
          FreeTemporaryMemory(VarHeader);
          Status = SplitFspVarName(LiteVarName, &NameSize, VariableName, VariableGuid);
          DEBUG((DEBUG_VERBOSE, "SplitFspVarName Status %r, Size 0x%X, LiteVarName %a, VarName %s, GUID %g\n", Status, NameSize, LiteVarName, VariableName, VariableGuid));
          if (EFI_ERROR(Status)) {
            // Edge case:
            // if Caller VariableNameSize was too small, we need to reset key so caller can try again.
            VariableKey = OldKey;
            *VariableNameSize = sizeof(CHAR16)*(LiteVarNameSize - ASCII_GUID_LENGTH);
          }
          FreeTemporaryMemory(LiteVarName);
          return Status;
        }
      }
    }
    if (LiteVarName != NULL) {
      FreeTemporaryMemory(LiteVarName);
    }
    if (VarHeader != NULL) {
      FreeTemporaryMemory(VarHeader);
    }
  } while (Status != EFI_NOT_FOUND);
  return Status;
}

/**
  FSP wrapper for SBL's Set Variable implementation.

  @param VariableName
  @param VariableGuid
  @param Attributes
  @param DataSize
  @param Data
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspSetVariable (
  IN  CHAR16      *VariableName,
  IN  EFI_GUID    *VariableGuid,
  IN  UINT32      *Attributes,
  IN  UINT64      *DataSize,
  IN  VOID        *Data
  )
{
  EFI_STATUS                Status;
  CHAR8                     *LiteVarName;
  UINTN                     LiteVarSize;
  UINTN                     OldVarSize;
  FSP_LITE_VARIABLE_HEADER  *VarHeader, *OldVar;

  if (VariableName == NULL
    || DataSize == NULL
    || Attributes == NULL
    || (Data == NULL && *DataSize != 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // We don't support authenticated variables or volatile variables,
  // and FSP doesn't require them.
  if ((*Attributes & EFI_VARIABLE_NON_VOLATILE) == 0
    || *Attributes & (EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
      | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS))
  {
    return EFI_INVALID_PARAMETER;
  }

  // Concatenate GUID + Unicode name
  Status = FormFspVarName(VariableName, VariableGuid, &LiteVarName);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Check if this is an attempt to delete the variable
  if (*DataSize == 0
    && ((*Attributes & EFI_VARIABLE_APPEND_WRITE) == 0))
  {
    Status = SetVariable(LiteVarName, 0, 0, NULL);
    FreeTemporaryMemory(LiteVarName);
    return Status;
  }

  // check Attribute compatibility
  OldVarSize=0;
  OldVar = NULL;
  Status = GetVariable(LiteVarName, NULL, &OldVarSize, OldVar);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    OldVar = AllocateTemporaryMemory(OldVarSize);
    ASSERT(OldVar != NULL);
    if (OldVar == NULL) {
      FreeTemporaryMemory(LiteVarName);
      return EFI_OUT_OF_RESOURCES;
    }
    Status = GetVariable(LiteVarName, NULL, &OldVarSize, OldVar);
  }
  if (!EFI_ERROR(Status)) {
    if ((*Attributes & ~EFI_VARIABLE_APPEND_WRITE) != OldVar->Attributes) {
      FreeTemporaryMemory(LiteVarName);
      FreeTemporaryMemory(OldVar);
      return Status;
    }
  }

  // handle Append Write case
  if (OldVar != NULL
    && *Attributes & EFI_VARIABLE_APPEND_WRITE)
  {
    LiteVarSize = (UINTN)*DataSize + OldVarSize;
  } else {
    LiteVarSize = (UINTN)*DataSize + sizeof(FSP_LITE_VARIABLE_HEADER);
  }

  // Allocate a pool and create encapsulated variable.
  VarHeader = AllocateTemporaryMemory(LiteVarSize);
  ASSERT(VarHeader != NULL);
  if (VarHeader == NULL) {
    FreeTemporaryMemory(LiteVarName);
    if (OldVar != NULL) {
      FreeTemporaryMemory(OldVar);
    }
    return EFI_OUT_OF_RESOURCES;
  }

  VarHeader->Signature = FSP_LITE_VAR_SIGNATURE;
  VarHeader->Attributes = *Attributes & ~EFI_VARIABLE_APPEND_WRITE;
  if (OldVar != NULL && *Attributes & EFI_VARIABLE_APPEND_WRITE) {
    // Copy old variable data into new variable.
    CopyMem(VarHeader->Data, OldVar->Data, OldVarSize-sizeof(FSP_LITE_VARIABLE_HEADER));
    // Copy new data at the end
    CopyMem(((UINT8*)VarHeader)+OldVarSize, Data, (UINTN)*DataSize);
    FreeTemporaryMemory(OldVar);
  } else {
    // Copy new data into new variable
    CopyMem(VarHeader->Data, Data, (UINTN)*DataSize);
  }

  Status = SetVariable(LiteVarName, 0, LiteVarSize, (VOID*)VarHeader);
  // Done with temp buffers now.
  FreeTemporaryMemory(LiteVarName);
  FreeTemporaryMemory(VarHeader);
  return Status;
}

/**
  SBL implementation of FSP QueryVariableInfo API

  @param Attributes
  @param MaximumVariableStorageSize
  @param RemainingVariableStorageSize
  @param MaximumVariableSize
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspQueryVariableInfo (
  IN  UINT32      *Attributes,
  OUT UINT64      *MaximumVariableStorageSize,
  OUT UINT64      *RemainingVariableStorageSize,
  OUT UINT64      *MaximumVariableSize
  )
{
  EFI_STATUS      Status;
  UINTN           LiteMaxStoreSize;
  UINTN           LiteRemainingSize;
  UINTN           LiteMaxVarSize;

  // don't support authenticated or volatile variables
  if (*Attributes & (EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)
    || (*Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
    return EFI_UNSUPPORTED;
  }

  Status = QueryVariableInfo(
    &LiteMaxStoreSize,
    &LiteRemainingSize,
    &LiteMaxVarSize);
  if (!EFI_ERROR(Status)) {
    *MaximumVariableStorageSize = (UINT64)LiteMaxStoreSize;
    *RemainingVariableStorageSize = (UINT64)LiteRemainingSize - sizeof(FSP_LITE_VARIABLE_HEADER);
    *MaximumVariableSize = (UINT64)LiteMaxVarSize - sizeof(FSP_LITE_VARIABLE_HEADER);
  }
  return Status;
}
