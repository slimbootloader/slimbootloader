/** @file
Lite variable service library

Copyright (c) 2006 - 2023, Intel Corporation. All rights reserved.<BR>
Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <LiteVariableLib.h>
#include <Library/ExtraBaseLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/VariableLib.h>
#include <Service/VariableService.h>

const VARIABLE_SERVICE   mVariableService = {
  .Header.Signature     = VARIABLE_SERVICE_SIGNATURE,
  .Header.Version       = VARIABLE_SERVICE_VERSION,
  .GetVariable          = GetVariable,
  .GetNextVariableName  = GetNextVariableName,
  .SetVariable          = SetVariable
};

/**
  Get variable instance from library global data..

  @retval SpiInstance       Return SPI instance

**/
VARIABLE_INSTANCE *
GetVariableInstance (
  VOID
  )
{
  EFI_STATUS         Status;
  VARIABLE_INSTANCE *VarInstance;

  Status = GetLibraryData (PcdGet8 (PcdVariableLibId), (VOID **)&VarInstance);
  if (Status == EFI_NOT_FOUND) {
    VarInstance  = AllocateZeroPool (sizeof (VARIABLE_INSTANCE));
    Status = SetLibraryData (PcdGet8 (PcdVariableLibId), VarInstance, sizeof (VARIABLE_INSTANCE));
  }

  if (EFI_ERROR (Status)) {
    VarInstance = NULL;
  }

  return VarInstance;
}

/**
  This function retrieves the variable store region base and size.

  @param[in,out]  Size  Pointer to receive variable store size.

  @retval               Variable store region base address.

**/
VOID *
GetVariableStoreBase (
  IN OUT  UINT32 *Size      OPTIONAL
  )
{
  VARIABLE_INSTANCE  *VarInstance;

  VarInstance = GetVariableInstance ();
  if (VarInstance == NULL) {
    return NULL;
  }

  if (Size) {
    *Size = VarInstance->StoreSize;
  }

  return (VOID *)(UINTN)VarInstance->StoreBase;
}

/**
  This function erases the specified variable store region.

  @param[in]  VariableStore     Erasing region base, must be flash block aligned.
  @param[in]  Length            Erasing region size, must be flash block aligned.

  @retval     EFI_SUCCESS             Variable store region was erased successfully.
              EFI_NOT_AVAILABLE_YET   SPI service is not available.
              Others                  Erasing operation failed.
**/
EFI_STATUS
EraseVariableStore (
  IN VOID     *VariableStore,
  IN UINT32    Length
  )
{
  SPI_FLASH_SERVICE  *SpiService;
  EFI_STATUS          Status;
  UINT32              BiosRgnOffset;
  UINT32              RgnBase;
  UINT32              RgnSize;

  DEBUG ((DEBUG_INFO, "  SPI ERASE: %08X  %08X\n", (UINT32)(UINTN)VariableStore, Length));
  SpiService = (SPI_FLASH_SERVICE *)GetServiceBySignature (SPI_FLASH_SERVICE_SIGNATURE);
  if (SpiService != NULL) {
    Status = SpiService->SpiGetRegion (FlashRegionBios, &RgnBase, &RgnSize);
    if (!EFI_ERROR (Status)) {
      // BIOS region offset can be calculated by (HostAddress + BiosRgnLimit)
      BiosRgnOffset = (UINT32)((UINT32)(UINTN)VariableStore + RgnSize);
      Status = SpiService->SpiErase (FlashRegionBios, BiosRgnOffset, Length);
      AsmFlushCacheRange (VariableStore, Length);
    }
  } else {
    Status = EFI_NOT_AVAILABLE_YET;
  }
  return Status;
}

/**
  This function writs to the specified variable store region with data buffer.

  @param[in]  VariableStore     Writing region base.
  @param[in]  Length            Writing region size.
  @param[in]  Buffer            Data buffer to write.

  @retval     EFI_SUCCESS             Variable store region was written successfully.
              EFI_NOT_AVAILABLE_YET   SPI service is not available.
              Others                  Write operation failed.
**/
EFI_STATUS
WriteVariableStore (
  IN VOID     *VariableStore,
  IN UINT32    Length,
  IN VOID     *Buffer
  )
{
  SPI_FLASH_SERVICE  *SpiService;
  EFI_STATUS          Status;
  UINT32              BiosRgnOffset;
  UINT32              RgnBase;
  UINT32              RgnSize;

  DEBUG ((DEBUG_INFO, "  SPI WRITE: %08X  %08X\n", (UINT32)(UINTN)VariableStore, Length));
  SpiService = (SPI_FLASH_SERVICE *)GetServiceBySignature (SPI_FLASH_SERVICE_SIGNATURE);
  if (SpiService != NULL) {
    Status = SpiService->SpiGetRegion (FlashRegionBios, &RgnBase, &RgnSize);
    if (!EFI_ERROR (Status)) {
      // BIOS region offset can be calculated by (HostAddress + BiosRgnLimit)
      BiosRgnOffset = (UINT32)((UINT32)(UINTN)VariableStore + RgnSize);
      Status = SpiService->SpiWrite (FlashRegionBios, BiosRgnOffset, Length, Buffer);
      AsmFlushCacheRange (VariableStore, Length);
    }
  } else {
    Status = EFI_NOT_AVAILABLE_YET;
  }
  return Status;
}

/**

  This function checks fi the variable store is valid.

  @param    VarStoreHdrPtr      Variable store pointer to check

  @retval   TRUE          Variable store is valid.
  @retval   FALSE         Variable store is invalid.

**/
BOOLEAN
IsVariableStoreValid (
  IN  VARIABLE_STORE_HEADER  *VarStoreHdrPtr
  )
{
  if ((VarStoreHdrPtr != NULL) && (VarStoreHdrPtr->Signature == VARIABLE_STORE_SIGNATURE)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  This function returns the active variable store header pointer.

  @param    Size      Variable store size

  @retval   Active variable store header pointer.

**/
VARIABLE_STORE_HEADER *
GetActiveVaraibelStoreBase (
  IN UINT32    *Size
  )
{
  UINT32                  VarStoreLen;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr1;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr2;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr;

  VarStoreHdrPtr1 = (VARIABLE_STORE_HEADER *)GetVariableStoreBase (&VarStoreLen);
  if (VarStoreHdrPtr1 == NULL) {
    return NULL;
  }

  VarStoreHdrPtr2 = (VARIABLE_STORE_HEADER *) ((UINT8 *)VarStoreHdrPtr1 + (VarStoreLen >> 1));
  if (((VarStoreHdrPtr1->State & VAR_HEADER_VALID) == 0) && ((VarStoreHdrPtr1->State & VAR_DELETED) != 0)) {
    VarStoreHdrPtr = VarStoreHdrPtr1;
  } else if (((VarStoreHdrPtr2->State & VAR_HEADER_VALID) == 0) && ((VarStoreHdrPtr2->State & VAR_DELETED) != 0)) {
    VarStoreHdrPtr = VarStoreHdrPtr2;
  } else {
    VarStoreHdrPtr = NULL;
  }

  ASSERT ((VarStoreHdrPtr != NULL) && (VarStoreHdrPtr->Signature == VARIABLE_STORE_SIGNATURE));

  if (Size) {
    *Size = VarStoreHdrPtr->Size;
  }

  return VarStoreHdrPtr;
}

/**

  This function initilizes the variable store.

  If the variable store needs repair, it will fix previous error if required.

  @retval   EFI_DEVICE_ERROR    Device write failure.
  @retval   EFI_SUCCESS         Variable store has been initialized successfully.

**/
EFI_STATUS
EFIAPI
InitializeVariableStore (
  VOID
  )
{
  UINT32                  FullVarStoreLen;
  UINT32                  VarStoreLen;
  VARIABLE_STORE_HEADER   VarStoreHdr;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr1;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr2;
  VARIABLE_STORE_HEADER  *ActivateVarStoreHdrPtr;
  VARIABLE_STORE_HEADER  *InactiveVarStoreHdrPtr;
  UINT8                   State;
  EFI_STATUS              Status;

  VarStoreHdrPtr1 = (VARIABLE_STORE_HEADER *)GetVariableStoreBase (&FullVarStoreLen);
  if (VarStoreHdrPtr1 == NULL) {
    return EFI_NOT_READY;
  }

  VarStoreLen     = FullVarStoreLen >> 1;
  VarStoreHdrPtr2 = (VARIABLE_STORE_HEADER *) ((UINT8 *)VarStoreHdrPtr1 + VarStoreLen);

  if (((VarStoreHdrPtr1->State & VAR_HEADER_VALID) == 0) && ((VarStoreHdrPtr2->State & VAR_HEADER_VALID) == 0)) {
    //
    // Both headers are valid, check delete state
    //
    if ((VarStoreHdrPtr1->State & VAR_DELETED) == 0) {
      ActivateVarStoreHdrPtr = VarStoreHdrPtr2;
    } else if ((VarStoreHdrPtr2->State & VAR_DELETED) == 0) {
      ActivateVarStoreHdrPtr = VarStoreHdrPtr1;
    } else {
      //
      // Both headers are active, check migration state
      //
      if ((VarStoreHdrPtr1->State & VAR_IN_MIGRATION) == 0) {
        ActivateVarStoreHdrPtr = VarStoreHdrPtr2;
        InactiveVarStoreHdrPtr = VarStoreHdrPtr1;
      } else {
        ActivateVarStoreHdrPtr = VarStoreHdrPtr1;
        InactiveVarStoreHdrPtr = VarStoreHdrPtr2;
      }
      //
      // Mark migration copy as deleted
      //
      State  = InactiveVarStoreHdrPtr->State & ~VAR_DELETED;
      Status = WriteVariableStore (&InactiveVarStoreHdrPtr->State, sizeof (InactiveVarStoreHdrPtr->State), &State);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  } else if ((VarStoreHdrPtr2->State & VAR_HEADER_VALID) == 0) {
    ActivateVarStoreHdrPtr = VarStoreHdrPtr2;
  } else {
    ActivateVarStoreHdrPtr = VarStoreHdrPtr1;
  }

  if (!IsVariableStoreValid (ActivateVarStoreHdrPtr)) {
    //
    // Erase current partition
    //
    Status = EraseVariableStore (VarStoreHdrPtr1, VarStoreLen * 2);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
    ActivateVarStoreHdrPtr = VarStoreHdrPtr1;

    //
    // Write store header
    //
    SetMem (&VarStoreHdr, sizeof (VARIABLE_STORE_HEADER), 0);
    VarStoreHdr.Signature = VARIABLE_STORE_SIGNATURE;
    VarStoreHdr.State  = 0xFF;
    VarStoreHdr.Format = 0xFF;
    VarStoreHdr.Size   = VarStoreLen;
    Status = WriteVariableStore (ActivateVarStoreHdrPtr, sizeof (VARIABLE_STORE_HEADER), &VarStoreHdr);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Mark header as valid
    //
    State  = ActivateVarStoreHdrPtr->State & ~VAR_HEADER_VALID;
    Status = WriteVariableStore (&ActivateVarStoreHdrPtr->State, sizeof (ActivateVarStoreHdrPtr->State), &State);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**

  This internal function finds variable in storage blocks.

  Caution: This function may receive untrusted input.
  This function will do basic validation, before parse the data.

  @param VariableName               Name of Variable to be found.
  @param VariableGuid               Variable GUID. Zero GUID would be used if it is NULL.
  @param Attributes                 Attribute value of the variable found.
  @param DataSize                   Size of Data found. If size is less than the
                                    data, this value contains the required size.
  @param Data                       The buffer to return the contents of the variable. May be NULL
                                    with a zero DataSize in order to determine the size buffer needed.
  @param VariableHeader             Variable header pointer if the variable is found.

  @retval EFI_INVALID_PARAMETER     Invalid parameter.
  @retval EFI_SUCCESS               Find the specified variable.
  @retval EFI_NOT_FOUND             Not found.
  @retval EFI_BUFFER_TOO_SMALL      DataSize is too small for the result.
  @retval EFI_VOLUME_CORRUPTED      Variable store is corrupted.

**/
EFI_STATUS
EFIAPI
InternalGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VariableGuid OPTIONAL,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data OPTIONAL,
  IN OUT  VARIABLE_HEADER  **VariableHeader OPTIONAL
  )
{

  UINT32                  VarStoreLen;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr;
  VARIABLE_HEADER        *VarHdrPtr;
  VARIABLE_HEADER        *FindVarHdrPtr;
  UINT8                  *VarEndPtr;
  UINT8                   State;
  UINT32                  VariableNameLen;
  UINT32                  VariableDataLen;
  UINTN                   DataSizeIn;
  EFI_GUID                *VarGuid;

  if ((DataSize == NULL) || (VariableName == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  VarGuid = VariableGuid;
  if (VarGuid == NULL) {
    VarGuid = &gZeroGuid;
  }

  VarStoreHdrPtr = GetActiveVaraibelStoreBase (&VarStoreLen);
  if (!IsVariableStoreValid (VarStoreHdrPtr)) {
    return EFI_VOLUME_CORRUPTED;
  }

  VariableNameLen = (UINT32)StrSize (VariableName);
  VarHdrPtr = (VARIABLE_HEADER *)&VarStoreHdrPtr[1];
  VarEndPtr = (UINT8 *)VarStoreHdrPtr + VarStoreHdrPtr->Size;

  FindVarHdrPtr = NULL;
  while ((UINT8 *)VarHdrPtr < VarEndPtr) {
    State = VarHdrPtr->State;
    if (!IS_HEADER_VALID (State)) {
      break;
    }

    if (VarHdrPtr->StartId != VARIABLE_DATA) {
      VarHdrPtr = NULL;
      break;
    }

    if (IS_DATA_VALID (State) && !IS_DELETED (State)) {
      if ((StrCmp ((VOID *)&VarHdrPtr[1], VariableName) == 0) &&
           CompareGuid (VarGuid, &VarHdrPtr->VariableGuid)) {
        FindVarHdrPtr = VarHdrPtr;
        if (!IS_IN_MIGRATION (State)) {
          break;
        }
      }
    }

    VarHdrPtr = (VARIABLE_HEADER *) ((UINT8 *)&VarHdrPtr[1] + VarHdrPtr->DataSize);
  }

  if (VarHdrPtr == NULL) {
    return EFI_VOLUME_CORRUPTED;
  }

  if (FindVarHdrPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  DataSizeIn = *DataSize;
  VariableDataLen = FindVarHdrPtr->DataSize - VariableNameLen;
  *DataSize = VariableDataLen;
  if ((Data != NULL) && (DataSizeIn <  VariableDataLen)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  if (Data != NULL) {
    CopyMem (Data, (UINT8 *)&FindVarHdrPtr[1] + VariableNameLen, VariableDataLen);
  }

  if (VariableHeader) {
    *VariableHeader = FindVarHdrPtr;
  }

  return EFI_SUCCESS;

}

/**

  This code gets variable in storage blocks.

  @param VariableName               Name of Variable to be found.
  @param VariableGuid               Variable GUID. Zero GUID would be used if it is NULL.
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
  )
{
  return InternalGetVariable (VariableName, VariableGuid, Attributes, DataSize, Data, NULL);
}

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
  )
{
  UINT32                  VarStoreLen;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr;
  VARIABLE_HEADER        *VarHdrPtr;
  VARIABLE_HEADER        *FindVarHdrPtr;
  UINT8                  *VarEndPtr;
  UINT8                   State;
  UINT32                  VariableNameLen;

  if ((VariableNameSize == NULL) || (VariableName == NULL) || (VariableGuid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  VarStoreHdrPtr = GetActiveVaraibelStoreBase (&VarStoreLen);
  if (!IsVariableStoreValid (VarStoreHdrPtr)) {
    return EFI_VOLUME_CORRUPTED;
  }

  VarHdrPtr = (VARIABLE_HEADER *)&VarStoreHdrPtr[1];
  VarEndPtr = (UINT8 *)VarStoreHdrPtr + VarStoreHdrPtr->Size;

  if (VariableName[0] == 0) {
    // Return the first valid variable if VariableName starts with L"\0"
    FindVarHdrPtr = VarHdrPtr;
  } else {
    // Find current variable
    FindVarHdrPtr = NULL;
    while ((UINT8 *)VarHdrPtr < VarEndPtr) {
      State = VarHdrPtr->State;
      if (VarHdrPtr->StartId != VARIABLE_DATA) {
        break;
      }

      if (IS_HEADER_VALID (State) && IS_DATA_VALID (State) && !IS_DELETED (State)) {
        if ((StrCmp ((VOID *)&VarHdrPtr[1], VariableName) == 0) &&
             CompareGuid (VariableGuid, &VarHdrPtr->VariableGuid)) {
          FindVarHdrPtr = (VARIABLE_HEADER *) ((UINT8 *)&VarHdrPtr[1] + VarHdrPtr->DataSize);;
          break;
        }
      }

      VarHdrPtr = (VARIABLE_HEADER *) ((UINT8 *)&VarHdrPtr[1] + VarHdrPtr->DataSize);
    }
  }

  if (FindVarHdrPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  // Find the next valid variable
  VarHdrPtr     = FindVarHdrPtr;
  FindVarHdrPtr = NULL;
  while ((UINT8 *)VarHdrPtr < VarEndPtr) {
    State = VarHdrPtr->State;
    if (VarHdrPtr->StartId != VARIABLE_DATA) {
      break;
    }

    if (IS_HEADER_VALID (State) && IS_DATA_VALID (State) && !IS_DELETED (State)) {
      FindVarHdrPtr = VarHdrPtr;
      break;
    }

    VarHdrPtr = (VARIABLE_HEADER *) ((UINT8 *)&VarHdrPtr[1] + VarHdrPtr->DataSize);
  }

  if (FindVarHdrPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  VariableNameLen = (UINT32)StrSize ((CONST CHAR16 *)&FindVarHdrPtr[1]);
  if (*VariableNameSize < VariableNameLen) {
    *VariableNameSize = VariableNameLen;
    return EFI_BUFFER_TOO_SMALL;
  }

  CopyMem (VariableName, (UINT8 *)&FindVarHdrPtr[1], VariableNameLen);
  CopyGuid (VariableGuid, &FindVarHdrPtr->VariableGuid);
  return EFI_SUCCESS;

}

/**

  This function reclaim the variable store from active region to the alternative region.

  @param   ActiveVarStoreHdrPtr       The active variable store header pointer

  @retval  EFI_DEVICE_ERROR      Failed to erase device
  @retval  EFI_SUCCESS           Variable store was reclaimed successfully

**/

EFI_STATUS
Reclaim (
  IN VARIABLE_STORE_HEADER  *ActiveVarStoreHdrPtr
  )
{
  UINT32                  FullVarStoreLen;
  UINT32                  VarStoreLen;
  VARIABLE_STORE_HEADER   VarStoreHdr;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr1;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr2;
  VARIABLE_STORE_HEADER  *InactiveVarStoreHdrPtr;
  CHAR16                  VarName[VARIABLE_NAME_MAX_LEN];
  EFI_STATUS              Status;
  VARIABLE_HEADER        *VarHdrPtr;
  VARIABLE_HEADER         VarHdr;
  UINT8                  *CurPtr;
  UINTN                   DataLen;
  UINTN                   NameSize;
  UINT8                   ActiveState;
  UINT8                   InactiveState;
  EFI_GUID                VarGuid;

  DEBUG ((DEBUG_INFO, "Reclaiming variable storage\n"));

  VarStoreHdrPtr1 = (VARIABLE_STORE_HEADER *)GetVariableStoreBase (&FullVarStoreLen);
  if (VarStoreHdrPtr1 == NULL) {
    return EFI_NOT_READY;
  }

  VarStoreLen     = FullVarStoreLen >> 1;
  VarStoreHdrPtr2 = (VARIABLE_STORE_HEADER *) ((UINT8 *)VarStoreHdrPtr1 + VarStoreLen);

  if (VarStoreHdrPtr1 == ActiveVarStoreHdrPtr) {
    InactiveVarStoreHdrPtr = VarStoreHdrPtr2;
  } else {
    InactiveVarStoreHdrPtr = VarStoreHdrPtr1;
  }
  CurPtr = (UINT8 *) (InactiveVarStoreHdrPtr + 1);

  //
  // Erase InactiveVarStoreHdrPtr
  //
  Status = EraseVariableStore (InactiveVarStoreHdrPtr, VarStoreLen);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Copy variable over one by one
  //
  VarName[0] = 0;
  while (TRUE) {
    NameSize = sizeof (VarName);
    Status   = GetNextVariableName (&NameSize, VarName, &VarGuid);
    if (!EFI_ERROR (Status)) {
      Status = InternalGetVariable (VarName, &VarGuid, NULL, &DataLen, NULL, &VarHdrPtr);
      if (!EFI_ERROR (Status)) {
        CopyMem (&VarHdr, VarHdrPtr, sizeof (VarHdr));
        VarHdr.State |= VAR_IN_MIGRATION;
        Status  = WriteVariableStore (CurPtr, sizeof (VarHdr), &VarHdr);
        CurPtr += sizeof (VarHdr);
        if (!EFI_ERROR (Status)) {
          Status  = WriteVariableStore (CurPtr, VarHdrPtr->DataSize, (VOID *)&VarHdrPtr[1]);
          CurPtr += VarHdrPtr->DataSize;
        }
        if (EFI_ERROR (Status)) {
          return Status;
        }
      }
    } else {
      break;
    }
  }

  //
  // Write inactive store header
  //
  InactiveState = 0xFF;
  CopyMem (&VarStoreHdr, ActiveVarStoreHdrPtr, sizeof (VarStoreHdr));
  VarStoreHdr.State = InactiveState;
  Status = WriteVariableStore (InactiveVarStoreHdrPtr, sizeof (VarStoreHdr), (VOID *)&VarStoreHdr);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Mark active store in migration
  //
  ActiveState = ActiveVarStoreHdrPtr->State & ~VAR_IN_MIGRATION;;
  Status = WriteVariableStore (&ActiveVarStoreHdrPtr->State, sizeof (ActiveVarStoreHdrPtr->State), &ActiveState);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Mark inactive store as valid
  //
  InactiveState = InactiveState & ~VAR_HEADER_VALID;
  Status = WriteVariableStore (&InactiveVarStoreHdrPtr->State, sizeof (InactiveVarStoreHdrPtr->State), &InactiveState);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Delete active store
  //
  ActiveState = ActiveState & ~VAR_DELETED;
  Status = WriteVariableStore (&ActiveVarStoreHdrPtr->State, sizeof (ActiveVarStoreHdrPtr->State), &ActiveState);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**

  This code sets variable in storage blocks.

  @param VariableName                     Name of Variable to be found.
  @param VariableGuid                     Zero GUID would be used if it is NULL.
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
  )
{
  VARIABLE_HEADER         VarHdr;
  UINT32                  VarStoreLen;
  VARIABLE_STORE_HEADER  *VarStoreHdrPtr;
  VARIABLE_HEADER        *VarHdrPtr;
  VARIABLE_HEADER        *NextVarHdrPtr;
  VARIABLE_HEADER        *FindVarHdrPtr;
  UINT8                  *CurPtr;
  UINT8                  *VarEndPtr;
  EFI_STATUS              Status;
  UINT16                  Data16;
  UINT8                   State;
  UINT8                   FindVarState;
  UINTN                   Idx;
  UINT32                  VariableNameLen;
  UINT32                  TotalLen;
  BOOLEAN                 FoundSpace;
  BOOLEAN                 SkipVarWrite;
  BOOLEAN                 CheckVarDataValid;
  BOOLEAN                 NeedReclaim;
  EFI_GUID                *VarGuid;

  if ((VariableName == NULL) || (VariableName[0] == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Data == NULL) && (DataSize > 0)) {
    return EFI_INVALID_PARAMETER;
  }

  VarGuid = VariableGuid;
  if (VarGuid == NULL) {
    VarGuid = &gZeroGuid;
  }

  VarStoreHdrPtr = GetActiveVaraibelStoreBase (&VarStoreLen);
  if (!IsVariableStoreValid (VarStoreHdrPtr)) {
    return EFI_VOLUME_CORRUPTED;
  }

  VarHdrPtr = (VARIABLE_HEADER *)&VarStoreHdrPtr[1];
  VarEndPtr = (UINT8 *)VarStoreHdrPtr + VarStoreHdrPtr->Size;

  VariableNameLen = (UINT32)StrSize (VariableName);
  if (DataSize == 0) {
    //
    // Need to delete a variable
    //
    TotalLen = 0;
  } else {
    TotalLen = sizeof (VARIABLE_HEADER) + VariableNameLen + (UINT32)DataSize;
  }

  if (TotalLen > 0xFFFF) {
    return EFI_INVALID_PARAMETER;
  }

  NeedReclaim   = FALSE;
  SkipVarWrite  = FALSE;
  FoundSpace    = FALSE;
  FindVarHdrPtr = NULL;
  FindVarState  = 0;
  while (!FoundSpace) {
    CheckVarDataValid = FALSE;
    State = VarHdrPtr->State;

    if (!IS_HEADER_VALID (State)) {

      FoundSpace    = TRUE;

      //
      // Variable header is invalid, either
      //  - This area is empty
      //  - Previous variable header write failed
      //
      CurPtr = (UINT8 *)VarHdrPtr;
      for (Idx = 0; (Idx < sizeof (VARIABLE_HEADER)) && (CurPtr < VarEndPtr); Idx++) {
        if (CurPtr[Idx] != 0xFF) {
          CurPtr = NULL;
          break;
        }
      }

      if (CurPtr != NULL) {
        //
        // This area is empty
        //
        NextVarHdrPtr = VarHdrPtr;
      } else {
        //
        // Previous write header failed
        // Mark variable data length to 0
        //
        NeedReclaim = TRUE;
        Data16 = 0;
        Status = WriteVariableStore (&VarHdrPtr->DataSize, sizeof (VarHdrPtr->DataSize), &Data16);
        if (EFI_ERROR (Status)) {
          return Status;
        }

        //
        // Mark header valid
        //
        State &= ~VAR_HEADER_VALID;
        Status = WriteVariableStore (&VarHdrPtr->State, sizeof (VarHdrPtr->State), &State);
        if (EFI_ERROR (Status)) {
          return Status;
        }

        CheckVarDataValid = TRUE;
        NextVarHdrPtr = &VarHdrPtr[1];
      }
    } else {
      CheckVarDataValid = TRUE;
      if ((State & VAR_DELETED) == 0) {
        NeedReclaim = TRUE;
      }
      NextVarHdrPtr = (VARIABLE_HEADER *) ((UINT8 *)&VarHdrPtr[1] + VarHdrPtr->DataSize);
    }

    if (CheckVarDataValid) {
      if (!IS_DATA_VALID (State)) {
        //
        // Data is invalid, delete this variable
        //
        NeedReclaim = TRUE;
        State &= ~VAR_DELETED;
        Status = WriteVariableStore (&VarHdrPtr->State, sizeof (VarHdrPtr->State), &State);
        if (EFI_ERROR (Status)) {
          return Status;
        }
      }
    }

    if (IS_DATA_VALID (State) && !IS_DELETED (State)) {
      //
      // Check if variable match
      //
      if ((StrCmp ((VOID *)&VarHdrPtr[1], VariableName) == 0) &&
           CompareGuid (VarGuid, &VarHdrPtr->VariableGuid)) {
        if (FindVarHdrPtr != NULL) {
          //
          // 2nd match found, need to mark the previous one as invalid
          //
          NeedReclaim   = TRUE;
          FindVarState &= ~VAR_DELETED;
          Status = WriteVariableStore (&FindVarHdrPtr->State, sizeof (FindVarHdrPtr->State), &FindVarState);
          if (EFI_ERROR (Status)) {
            return Status;
          }
        }

        FindVarHdrPtr = VarHdrPtr;
        FindVarState  = State;
        if (DataSize > 0) {
          if (CompareMem ((UINT8 *)&VarHdrPtr[1] + VariableNameLen, Data, DataSize) == 0) {
            SkipVarWrite = TRUE;
          }
        } else {
          SkipVarWrite = FALSE;
        }
      }
    }

    if ((UINT8 *)NextVarHdrPtr + TotalLen > VarEndPtr) {
      FoundSpace = FALSE;
      break;
    }

    VarHdrPtr = NextVarHdrPtr;
  }

  if (SkipVarWrite) {
    return EFI_SUCCESS;
  }

  if (!FoundSpace) {
    //
    // No space left
    //
    if (NeedReclaim) {
      Status = Reclaim (VarStoreHdrPtr);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
      return SetVariable (VariableName, VariableGuid, Attributes, DataSize, Data);
    }
    return EFI_OUT_OF_RESOURCES;
  }

  if (DataSize > 0) {
    //
    // Write the variable header
    //
    SetMem (&VarHdr, sizeof (VARIABLE_HEADER), 0);
    VarHdr.StartId  = VARIABLE_DATA;
    VarHdr.State    = 0xFF;
    VarHdr.DataSize = (UINT16)TotalLen - sizeof (VARIABLE_HEADER);
    CopyGuid (&VarHdr.VariableGuid, VarGuid);
    Status = WriteVariableStore (VarHdrPtr, sizeof (VarHdr), &VarHdr);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Mark header valid
    //
    State  = VarHdr.State & ~VAR_HEADER_VALID;
    Status = WriteVariableStore (&VarHdrPtr->State, sizeof (VarHdrPtr->State), &State);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Write the variable name
    //
    Status = WriteVariableStore (&VarHdrPtr[1], VariableNameLen, VariableName);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Write the variable data
    //
    Status = WriteVariableStore ((UINT8 *)&VarHdrPtr[1] + VariableNameLen, (UINT32)DataSize, Data);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (FindVarHdrPtr != NULL) {
      //
      // Mark previous variable in migration
      //
      FindVarState &= ~VAR_IN_MIGRATION;
      Status = WriteVariableStore (&FindVarHdrPtr->State, sizeof (FindVarHdrPtr->State), &FindVarState);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    //
    // Mark data valid
    //
    State  = State & ~VAR_DATA_VALID;
    Status = WriteVariableStore (&VarHdrPtr->State, sizeof (VarHdrPtr->State), &State);
    if (EFI_ERROR (Status)) {
      return Status;
    }

  }

  if (FindVarHdrPtr != NULL) {
    //
    // Mark previous variable as invalid
    //
    FindVarState &= ~VAR_DELETED;
    Status = WriteVariableStore (&FindVarHdrPtr->State, sizeof (FindVarHdrPtr->State), &FindVarState);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    if (DataSize == 0) {
      return EFI_NOT_FOUND;
    }
  }

  return EFI_SUCCESS;
}

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
  )
{
  VARIABLE_STORE_HEADER   *VarStoreHdrPtr;
  VARIABLE_HEADER         *VarHdrPtr;
  UINT8                   *VarEndPtr;
  UINTN                   UsedSize;
  UINT8                   State;

  if (MaxVariableStorageSize == NULL || RemainingStorageSize == NULL || MaxVariableSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  VarStoreHdrPtr = GetActiveVaraibelStoreBase (NULL);
  if (!IsVariableStoreValid (VarStoreHdrPtr)) {
    return EFI_VOLUME_CORRUPTED;
  }

  // Loop through Variable entries looking for deleted vars and last entry
  UsedSize  = 0;
  VarHdrPtr = (VARIABLE_HEADER *)&VarStoreHdrPtr[1];
  VarEndPtr = (UINT8 *)VarStoreHdrPtr + VarStoreHdrPtr->Size;
  while ((UINT8 *)VarHdrPtr < VarEndPtr) {
    if (VarHdrPtr->StartId != VARIABLE_DATA) {
      break;
    }

    State = VarHdrPtr->State;
    if (IS_HEADER_VALID (State) && IS_DATA_VALID (State) && !IS_DELETED (State)) {
      UsedSize += VarHdrPtr->DataSize + sizeof(VARIABLE_HEADER);
    }

    VarHdrPtr = (VARIABLE_HEADER *) ((UINT8 *)&VarHdrPtr[1] + VarHdrPtr->DataSize);
  }

  *MaxVariableStorageSize = VarStoreHdrPtr->Size - sizeof(VARIABLE_STORE_HEADER);
  *RemainingStorageSize   = *MaxVariableStorageSize - UsedSize;
  *MaxVariableSize        = MAX_UINT16 - sizeof(VARIABLE_HEADER);

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS         Status;
  VARIABLE_INSTANCE *VarInstance;

  VarInstance = GetVariableInstance();
  ASSERT (VarInstance != NULL);

  if (VarInstance->Signature == VARIABLE_INSTANCE_SIGNATURE) {
    return EFI_SUCCESS;
  }

  // Base needs to be 4KB aligned
  // Size needs to be 8KB * n
  if (((Base & (SIZE_4KB - 1)) != 0) || ((Size & (SIZE_8KB - 1)) != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  VarInstance->Signature  = VARIABLE_INSTANCE_SIGNATURE;
  VarInstance->StoreBase  = Base;
  VarInstance->StoreSize  = Size;
  DEBUG ((DEBUG_INFO, "Variable region: 0x%08X:0x%X\n", Base, Size));
  Status = InitializeVariableStore ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Variable service init failed %r!\n", Status));
    return Status;
  }

  Status = RegisterService ((VOID *)&mVariableService);
  return Status;
}
