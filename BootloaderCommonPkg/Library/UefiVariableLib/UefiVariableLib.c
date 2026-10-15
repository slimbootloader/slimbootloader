/** @file
  Implement ReadOnly Variable Services required by PEIM and install
  PEI ReadOnly Varaiable2 PPI. These services operates the non volatile storage space.

Copyright (c) 2006 - 2021, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include "FtwLastWrite.h"
#include <Guid/FlashMapInfoGuid.h>
#include <Library/BaseLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/UefiVariableLib.h>
#include <Library/BlMemoryAllocationLib.h>

/**

  Gets the pointer to the first variable header in given variable store area.

  @param VarStoreHeader  Pointer to the Variable Store Header.

  @return Pointer to the first variable header.

**/
UEFI_VARIABLE_HEADER *
GetStartPointer (
  IN UEFI_VARIABLE_STORE_HEADER       *VarStoreHeader
  )
{
  //
  // The start of variable store
  //
  return (UEFI_VARIABLE_HEADER *) HEADER_ALIGN (VarStoreHeader + 1);
}


/**

  Gets the pointer to the end of the variable storage area.

  This function gets pointer to the end of the variable storage
  area, according to the input variable store header.

  @param VarStoreHeader  Pointer to the Variable Store Header.

  @return Pointer to the end of the variable storage area.

**/
UEFI_VARIABLE_HEADER *
GetEndPointer (
  IN UEFI_VARIABLE_STORE_HEADER       *VarStoreHeader
  )
{
  //
  // The end of variable store
  //
  return (UEFI_VARIABLE_HEADER *) HEADER_ALIGN ((UINTN) VarStoreHeader + VarStoreHeader->Size);
}


/**
  This code checks if variable header is valid or not.

  @param  Variable  Pointer to the Variable Header.

  @retval TRUE      Variable header is valid.
  @retval FALSE     Variable header is not valid.

**/
BOOLEAN
IsValidVariableHeader (
  IN  UEFI_VARIABLE_HEADER   *Variable
  )
{
  if (Variable == NULL || Variable->StartId != UEFI_VARIABLE_DATA ) {
    return FALSE;
  }

  return TRUE;
}

/**
  This code gets the size of variable header.

  @param AuthFlag   Authenticated variable flag.

  @return Size of variable header in bytes in type UINTN.

**/
UINTN
GetVariableHeaderSize (
  IN  BOOLEAN       AuthFlag
  )
{
  UINTN Value;

  if (AuthFlag) {
    Value = sizeof (UEFI_AUTHENTICATED_VARIABLE_HEADER);
  } else {
    Value = sizeof (UEFI_VARIABLE_HEADER);
  }

  return Value;
}

/**
  This code gets the size of name of variable.

  @param  Variable  Pointer to the Variable Header.
  @param  AuthFlag  Authenticated variable flag.

  @return Size of variable in bytes in type UINTN.

**/
UINTN
NameSizeOfVariable (
  IN  UEFI_VARIABLE_HEADER   *Variable,
  IN  BOOLEAN                 AuthFlag
  )
{
  UEFI_AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (UEFI_AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (AuthFlag) {
    if (AuthVariable->State == (UINT8) (-1) ||
       AuthVariable->DataSize == (UINT32) (-1) ||
       AuthVariable->NameSize == (UINT32) (-1) ||
       AuthVariable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) AuthVariable->NameSize;
  } else {
    if (Variable->State == (UINT8) (-1) ||
       Variable->DataSize == (UINT32) (-1) ||
       Variable->NameSize == (UINT32) (-1) ||
       Variable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) Variable->NameSize;
  }
}


/**
  This code gets the size of data of variable.

  @param  Variable  Pointer to the Variable Header.
  @param  AuthFlag  Authenticated variable flag.

  @return Size of variable in bytes in type UINTN.

**/
UINTN
DataSizeOfVariable (
  IN  UEFI_VARIABLE_HEADER    *Variable,
  IN  BOOLEAN                  AuthFlag
  )
{
  UEFI_AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (UEFI_AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (AuthFlag) {
    if (AuthVariable->State == (UINT8) (-1) ||
       AuthVariable->DataSize == (UINT32) (-1) ||
       AuthVariable->NameSize == (UINT32) (-1) ||
       AuthVariable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) AuthVariable->DataSize;
  } else {
    if (Variable->State == (UINT8) (-1) ||
       Variable->DataSize == (UINT32) (-1) ||
       Variable->NameSize == (UINT32) (-1) ||
       Variable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) Variable->DataSize;
  }
}

/**
  This code gets the pointer to the variable name.

  @param   Variable  Pointer to the Variable Header.
  @param   AuthFlag  Authenticated variable flag.

  @return  A CHAR16* pointer to Variable Name.

**/
CHAR16 *
GetVariableNamePtr (
  IN UEFI_VARIABLE_HEADER    *Variable,
  IN BOOLEAN                  AuthFlag
  )
{
  return (CHAR16 *) ((UINTN) Variable + GetVariableHeaderSize (AuthFlag));
}

/**
  This code gets the pointer to the variable guid.

  @param Variable   Pointer to the Variable Header.
  @param AuthFlag   Authenticated variable flag.

  @return A EFI_GUID* pointer to Vendor Guid.

**/
EFI_GUID *
GetVendorGuidPtr (
  IN UEFI_VARIABLE_HEADER    *Variable,
  IN BOOLEAN                  AuthFlag
  )
{
  UEFI_AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (UEFI_AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (AuthFlag) {
    return &AuthVariable->VendorGuid;
  } else {
    return &Variable->VendorGuid;
  }
}

/**
  This code gets the pointer to the variable data.

  @param   Variable         Pointer to the Variable Header.
  @param   VariableHeader   Pointer to the Variable Header that has consecutive content.
  @param   AuthFlag         Authenticated variable flag.

  @return  A UINT8* pointer to Variable Data.

**/
UINT8 *
GetVariableDataPtr (
  IN  UEFI_VARIABLE_HEADER   *Variable,
  IN  UEFI_VARIABLE_HEADER   *VariableHeader,
  IN  BOOLEAN           AuthFlag
  )
{
  UINTN Value;

  //
  // Be careful about pad size for alignment
  //
  Value =  (UINTN) GetVariableNamePtr (Variable, AuthFlag);
  Value += NameSizeOfVariable (VariableHeader, AuthFlag);
  Value += GET_PAD_SIZE (NameSizeOfVariable (VariableHeader, AuthFlag));

  return (UINT8 *) Value;
}


/**
  Translate a variable address into the variable store address space.

  When an FTW last write is in progress, variables at or beyond TargetAddress live
  in the spare block. Such an address is mapped back so that all bounds checks can
  be done against the store's own start/end pointers.

  @param  StoreInfo    Pointer to variable store info structure.
  @param  Address      Address of a variable header.

  @return  The equivalent address within the variable store address space.

**/
STATIC
UINTN
GetVariableStoreAddress (
  IN  UEFI_VARIABLE_STORE_INFO   *StoreInfo,
  IN  UINTN                       Address
  )
{
  UINTN  SpareAddress;

  if (StoreInfo->FtwLastWriteData != NULL) {
    SpareAddress = (UINTN) StoreInfo->FtwLastWriteData->SpareAddress;
    if (Address >= SpareAddress) {
      return Address - SpareAddress + (UINTN) StoreInfo->FtwLastWriteData->TargetAddress;
    }
  }

  return Address;
}

/**
  Check that a variable of the given size lies completely inside the variable store.

  @param  StoreInfo    Pointer to variable store info structure.
  @param  Variable     Pointer to the Variable Header.
  @param  Size         Number of bytes required at Variable.

  @retval TRUE         The requested range is inside the variable store.
  @retval FALSE        The requested range is outside or straddles the end of the store.

**/
STATIC
BOOLEAN
IsVariableInStore (
  IN  UEFI_VARIABLE_STORE_INFO   *StoreInfo,
  IN  UEFI_VARIABLE_HEADER       *Variable,
  IN  UINTN                       Size
  )
{
  UINTN  StoreStart;
  UINTN  StoreEnd;
  UINTN  Address;

  if (StoreInfo->VariableStoreHeader == NULL) {
    return FALSE;
  }

  StoreStart = (UINTN) GetStartPointer (StoreInfo->VariableStoreHeader);
  StoreEnd   = (UINTN) GetEndPointer (StoreInfo->VariableStoreHeader);
  Address    = GetVariableStoreAddress (StoreInfo, (UINTN) Variable);

  if ((StoreEnd < StoreStart) || (Address < StoreStart) || (Address > StoreEnd)) {
    return FALSE;
  }

  return (BOOLEAN) (Size <= (StoreEnd - Address));
}

/**
  This code gets the pointer to the next variable header.

  @param  StoreInfo         Pointer to variable store info structure.
  @param  Variable          Pointer to the Variable Header.
  @param  VariableHeader    Pointer to the Variable Header that has consecutive content.

  @return  A VARIABLE_HEADER* pointer to next variable header.

**/
UEFI_VARIABLE_HEADER *
GetNextVariablePtr (
  IN  UEFI_VARIABLE_STORE_INFO   *StoreInfo,
  IN  UEFI_VARIABLE_HEADER       *Variable,
  IN  UEFI_VARIABLE_HEADER       *VariableHeader
  )
{
  EFI_PHYSICAL_ADDRESS  TargetAddress;
  EFI_PHYSICAL_ADDRESS  SpareAddress;
  UINTN                 Value;
  UINTN                 NameSize;
  UINTN                 DataSize;
  UINTN                 StoreEnd;
  UINT64                NextAddress;

  if (StoreInfo->VariableStoreHeader == NULL) {
    return Variable;
  }

  NameSize = NameSizeOfVariable (VariableHeader, StoreInfo->AuthFlag);
  DataSize = DataSizeOfVariable (VariableHeader, StoreInfo->AuthFlag);
  StoreEnd = (UINTN) GetEndPointer (StoreInfo->VariableStoreHeader);

  //
  // NameSize/DataSize are untrusted NV store fields; 64-bit math keeps the sum from
  // wrapping on IA32, where UINTN is 32 bits.
  //
  NextAddress = (UINT64) GetVariableStoreAddress (StoreInfo, (UINTN) Variable)
              + (UINT64) GetVariableHeaderSize (StoreInfo->AuthFlag)
              + (UINT64) NameSize + (UINT64) GET_PAD_SIZE (NameSize)
              + (UINT64) DataSize + (UINT64) GET_PAD_SIZE (DataSize);
  NextAddress = (NextAddress + (HEADER_ALIGNMENT - 1)) & ~(UINT64) (HEADER_ALIGNMENT - 1);

  if (!IsVariableInStore (StoreInfo, Variable, GetVariableHeaderSize (StoreInfo->AuthFlag)) ||
      (NextAddress > (UINT64) StoreEnd)) {
    //
    // Returning NULL stops the walk without aliasing the FTW spare address.
    //
    return NULL;
  }

  Value =  (UINTN) GetVariableDataPtr (Variable, VariableHeader, StoreInfo->AuthFlag);
  Value += DataSizeOfVariable (VariableHeader, StoreInfo->AuthFlag);
  Value += GET_PAD_SIZE (DataSizeOfVariable (VariableHeader, StoreInfo->AuthFlag));
  //
  // Be careful about pad size for alignment
  //
  Value = HEADER_ALIGN (Value);

  if (StoreInfo->FtwLastWriteData != NULL) {
    TargetAddress = StoreInfo->FtwLastWriteData->TargetAddress;
    SpareAddress = StoreInfo->FtwLastWriteData->SpareAddress;
    if (((UINTN) Variable < (UINTN) TargetAddress) && (Value >= (UINTN) TargetAddress)) {
      //
      // Next variable is in spare block.
      //
      Value = (UINTN) SpareAddress + (Value - (UINTN) TargetAddress);
    }
  }

  return (UEFI_VARIABLE_HEADER *) Value;
}

/**
  Get variable store status.

  @param  VarStoreHeader  Pointer to the Variable Store Header.

  @retval  EfiRaw      Variable store is raw
  @retval  EfiValid    Variable store is valid
  @retval  EfiInvalid  Variable store is invalid

**/
UEFI_VARIABLE_STORE_STATUS
GetVariableStoreStatus (
  IN UEFI_VARIABLE_STORE_HEADER *VarStoreHeader
  )
{
  if ((CompareGuid (&VarStoreHeader->Signature, &gEfiAuthenticatedVariableGuid) ||
       CompareGuid (&VarStoreHeader->Signature, &gEfiVariableGuid)) &&
      VarStoreHeader->Format == UEFI_VARIABLE_STORE_FORMATTED &&
      VarStoreHeader->State == UEFI_VARIABLE_STORE_HEALTHY
      ) {

    return EfiValid;
  }

  if (((UINT32 *)(&VarStoreHeader->Signature))[0] == 0xffffffff &&
      ((UINT32 *)(&VarStoreHeader->Signature))[1] == 0xffffffff &&
      ((UINT32 *)(&VarStoreHeader->Signature))[2] == 0xffffffff &&
      ((UINT32 *)(&VarStoreHeader->Signature))[3] == 0xffffffff &&
      VarStoreHeader->Size == 0xffffffff &&
      VarStoreHeader->Format == 0xff &&
      VarStoreHeader->State == 0xff
      ) {

    return EfiRaw;
  } else {
    return EfiInvalid;
  }
}

/**
  Compare two variable names, one of them may be inconsecutive.

  @param StoreInfo      Pointer to variable store info structure.
  @param Name1          Pointer to one variable name.
  @param Name2          Pointer to another variable name.
  @param NameSize       Variable name size.

  @retval TRUE          Name1 and Name2 are identical.
  @retval FALSE         Name1 and Name2 are not identical.

**/
BOOLEAN
CompareVariableName (
  IN UEFI_VARIABLE_STORE_INFO  *StoreInfo,
  IN CONST CHAR16              *Name1,
  IN CONST CHAR16              *Name2,
  IN UINTN                      NameSize
  )
{
  EFI_PHYSICAL_ADDRESS  TargetAddress;
  EFI_PHYSICAL_ADDRESS  SpareAddress;
  UINTN                 PartialNameSize;

  if (StoreInfo->FtwLastWriteData != NULL) {
    TargetAddress = StoreInfo->FtwLastWriteData->TargetAddress;
    SpareAddress = StoreInfo->FtwLastWriteData->SpareAddress;
    if (((UINTN) Name1 < (UINTN) TargetAddress) && (((UINTN) Name1 + NameSize) > (UINTN) TargetAddress)) {
      //
      // Name1 is inconsecutive.
      //
      PartialNameSize = (UINTN) TargetAddress - (UINTN) Name1;
      //
      // Partial content is in NV storage.
      //
      if (CompareMem ((UINT8 *) Name1, (UINT8 *) Name2, PartialNameSize) == 0) {
        //
        // Another partial content is in spare block.
        //
        if (CompareMem ((UINT8 *) (UINTN) SpareAddress, (UINT8 *) Name2 + PartialNameSize, NameSize - PartialNameSize) == 0) {
          return TRUE;
        }
      }
      return FALSE;
    } else if (((UINTN) Name2 < (UINTN) TargetAddress) && (((UINTN) Name2 + NameSize) > (UINTN) TargetAddress)) {
      //
      // Name2 is inconsecutive.
      //
      PartialNameSize = (UINTN) TargetAddress - (UINTN) Name2;
      //
      // Partial content is in NV storage.
      //
      if (CompareMem ((UINT8 *) Name2, (UINT8 *) Name1, PartialNameSize) == 0) {
        //
        // Another partial content is in spare block.
        //
        if (CompareMem ((UINT8 *) (UINTN) SpareAddress, (UINT8 *) Name1 + PartialNameSize, NameSize - PartialNameSize) == 0) {
          return TRUE;
        }
      }
      return FALSE;
    }
  }

  //
  // Both Name1 and Name2 are consecutive.
  //
  if (CompareMem ((UINT8 *) Name1, (UINT8 *) Name2, NameSize) == 0) {
    return TRUE;
  }
  return FALSE;
}

/**
  This function compares a variable with variable entries in database.

  @param  StoreInfo     Pointer to variable store info structure.
  @param  Variable      Pointer to the variable in our database
  @param  VariableHeader Pointer to the Variable Header that has consecutive content.
  @param  VariableName  Name of the variable to compare to 'Variable'
  @param  VendorGuid    GUID of the variable to compare to 'Variable'
  @param  PtrTrack      Variable Track Pointer structure that contains Variable Information.

  @retval EFI_SUCCESS    Found match variable
  @retval EFI_NOT_FOUND  Variable not found

**/
EFI_STATUS
CompareWithValidVariable (
  IN  UEFI_VARIABLE_STORE_INFO           *StoreInfo,
  IN  UEFI_VARIABLE_HEADER               *Variable,
  IN  UEFI_VARIABLE_HEADER               *VariableHeader,
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT UEFI_VARIABLE_POINTER_TRACK        *PtrTrack
  )
{
  VOID      *Point;
  EFI_GUID  *TempVendorGuid;
  UINTN      NameSize;

  TempVendorGuid = GetVendorGuidPtr (VariableHeader, StoreInfo->AuthFlag);

  if (VariableName[0] == 0) {
    PtrTrack->CurrPtr = Variable;
    return EFI_SUCCESS;
  } else {
    //
    // Don't use CompareGuid function here for performance reasons.
    // Instead we compare the GUID a UINT32 at a time and branch
    // on the first failed comparison.
    //
    if ((((INT32 *) VendorGuid)[0] == ((INT32 *) TempVendorGuid)[0]) &&
        (((INT32 *) VendorGuid)[1] == ((INT32 *) TempVendorGuid)[1]) &&
        (((INT32 *) VendorGuid)[2] == ((INT32 *) TempVendorGuid)[2]) &&
        (((INT32 *) VendorGuid)[3] == ((INT32 *) TempVendorGuid)[3])
        ) {
      //
      // The stored name must include exactly the caller's terminating NUL.
      //
      NameSize = NameSizeOfVariable (VariableHeader, StoreInfo->AuthFlag);
      if ((NameSize == 0) || (NameSize != StrSize (VariableName))) {
        return EFI_NOT_FOUND;
      }
      Point = (VOID *) GetVariableNamePtr (Variable, StoreInfo->AuthFlag);
      if (CompareVariableName (StoreInfo, VariableName, Point, NameSize)) {
        PtrTrack->CurrPtr = Variable;
        return EFI_SUCCESS;
      }
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Return the variable store header and the store info based on the Index.

  @param Type       The type of the variable store.
  @param StoreInfo  Return the store info.

  @return  Pointer to the variable store header.
**/
UEFI_VARIABLE_STORE_HEADER *
GetVariableStore (
  IN  UEFI_VARIABLE_STORE_TYPE         Type,
  OUT UEFI_VARIABLE_STORE_INFO        *StoreInfo
  )
{
  EFI_FIRMWARE_VOLUME_HEADER                 *FvHeader;
  UEFI_VARIABLE_STORE_HEADER                 *VariableStoreHeader;
  UINT32                                      NvStorageBase;
  UINT32                                      NvStorageSize;
  UINT32                                      BackUpOffset;
  UEFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER   *FtwWorkingHeader;
  UINTN                                       FtwWorkingSize;
  EFI_PHYSICAL_ADDRESS                        SpareAreaAddress;
  UINTN                                       SpareAreaLength;
  EFI_PHYSICAL_ADDRESS                        WorkSpaceInSpareArea;
  UEFI_FAULT_TOLERANT_WRITE_HEADER           *FtwLastWriteHeader;
  UEFI_FAULT_TOLERANT_WRITE_RECORD           *FtwLastWriteRecord;
  EFI_STATUS                                  Status;
  EFI_STATUS                                  LibStatus;
  UEFI_VAR_STORE_LIBRARY_DATA                *VarStoreLibData;
  UINT32                                      StoreSizeLimit;

  StoreInfo->VariableStoreHeader = NULL;
  StoreInfo->IndexTable = NULL;
  StoreInfo->FtwLastWriteData = NULL;
  StoreInfo->AuthFlag = FALSE;
  VariableStoreHeader = NULL;

  switch (Type) {
    case VariableStoreTypeNv:
      //
      // Works only if emulated non-volatile variable mode
      // is not enabled in UEFI payload.
      //
      GetComponentInfo (FLASH_MAP_SIG_UEFIVARIABLE, &NvStorageBase, &NvStorageSize);
      ASSERT (NvStorageBase != 0);

      //
      // First let FvHeader point to NV storage base.
      //
      FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) NvStorageBase;

      //
      // Check the FTW last write data hob.
      //
      BackUpOffset = 0;

      FtwWorkingHeader = (UEFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *) (UINTN)(NvStorageBase + (NvStorageSize / 2) - FTW_WORKING_SIZE);
      FtwWorkingSize   = FTW_WORKING_SIZE;
      SpareAreaAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) (NvStorageBase + (NvStorageSize / 2));
      SpareAreaLength  = NvStorageSize / 2;

      LibStatus = GetLibraryData (PcdGet8 (PcdUefiVariableLibId), (VOID **)&VarStoreLibData);
      if (LibStatus == EFI_NOT_FOUND) {
        VarStoreLibData = (UEFI_VAR_STORE_LIBRARY_DATA *) AllocatePool (sizeof(UEFI_VAR_STORE_LIBRARY_DATA));
        if (VarStoreLibData == NULL) {
          return NULL;
        }
        ZeroMem (VarStoreLibData, sizeof(UEFI_VAR_STORE_LIBRARY_DATA));
      }

      if (IsValidWorkSpace(FtwWorkingHeader, FtwWorkingSize)) {
        Status = FtwGetLastWriteHeader (
                  FtwWorkingHeader,
                  FtwWorkingSize,
                  &FtwLastWriteHeader
                  );
        if (!EFI_ERROR(Status)) {
          Status = FtwGetLastWriteRecord (
                    FtwLastWriteHeader,
                    &FtwLastWriteRecord
                    );
        }

        if (!EFI_ERROR(Status) && FtwLastWriteRecord != NULL) {
          if ((FtwLastWriteRecord->SpareComplete == FTW_VALID_STATE) && (FtwLastWriteRecord->DestinationComplete != FTW_VALID_STATE)) {
            if (VarStoreLibData != NULL) {
              if (LibStatus == EFI_NOT_FOUND) {
                VarStoreLibData->FtwLastWriteData.TargetAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) ((INT64) SpareAreaAddress + FtwLastWriteRecord->RelativeOffset);
                VarStoreLibData->FtwLastWriteData.SpareAddress  = SpareAreaAddress;
                VarStoreLibData->FtwLastWriteData.Length        = SpareAreaLength;
              }
              if (VarStoreLibData->FtwLastWriteData.TargetAddress == NvStorageBase) {
                //
                // Let FvHeader point to spare block.
                //
                FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) VarStoreLibData->FtwLastWriteData.SpareAddress;
                DEBUG ((DEBUG_INFO, "PeiVariable: NV storage is backed up in spare block: 0x%x\n", (UINTN) VarStoreLibData->FtwLastWriteData.SpareAddress));
              } else if ((VarStoreLibData->FtwLastWriteData.TargetAddress > NvStorageBase) && (VarStoreLibData->FtwLastWriteData.TargetAddress < (NvStorageBase + NvStorageSize))) {
                StoreInfo->FtwLastWriteData = &VarStoreLibData->FtwLastWriteData;
                //
                // Flash NV storage from the offset is backed up in spare block.
                //
                BackUpOffset = (UINT32) (VarStoreLibData->FtwLastWriteData.TargetAddress - NvStorageBase);
                DEBUG ((DEBUG_INFO, "PeiVariable: High partial NV storage from offset: %x is backed up in spare block: 0x%x\n", BackUpOffset, (UINTN) VarStoreLibData->FtwLastWriteData.SpareAddress));
                //
                // At least one block data in flash NV storage is still valid, so still leave FvHeader point to NV storage base.
                //
              }
            }
          }
        }
      } else {
        FtwWorkingHeader = NULL;
        //
        // If the working block workspace is not valid, try to find workspace in the spare block.
        //
        WorkSpaceInSpareArea = SpareAreaAddress + SpareAreaLength - FtwWorkingSize;

        // Ensure WorkSpaceInSpareArea is within the expected range
        ASSERT((WorkSpaceInSpareArea >= SpareAreaAddress) && (WorkSpaceInSpareArea <= (SpareAreaAddress + SpareAreaLength)));

        while (WorkSpaceInSpareArea >= SpareAreaAddress) {
          if (CompareGuid (&gEdkiiWorkingBlockSignatureGuid, (EFI_GUID *) (UINTN) WorkSpaceInSpareArea)) {
            //
            // Found the workspace.
            //
            DEBUG ((DEBUG_INFO, "FtwPei: workspace in spare block is at 0x%x.\n", (UINTN) WorkSpaceInSpareArea));
            FtwWorkingHeader = (UEFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *) (UINTN) WorkSpaceInSpareArea;
            break;
          }
          WorkSpaceInSpareArea = WorkSpaceInSpareArea - sizeof (EFI_GUID);
        }
        if ((FtwWorkingHeader != NULL) && IsValidWorkSpace (FtwWorkingHeader, FtwWorkingSize)) {
          //
          // It was workspace self reclaim.
          //
          if (LibStatus == EFI_NOT_FOUND && VarStoreLibData != NULL) {
            VarStoreLibData->FtwLastWriteData.TargetAddress = NvStorageBase - (WorkSpaceInSpareArea - SpareAreaAddress);
            VarStoreLibData->FtwLastWriteData.SpareAddress  = SpareAreaAddress;
            VarStoreLibData->FtwLastWriteData.Length        = SpareAreaLength;
          }
        } else {
          //
          // Both are invalid.
          //
          DEBUG ((DEBUG_INFO, "Both working and spare block are invalid.\n"));
        }

      }

      //
      // Retain newly allocated library data after FTW state is initialized, even if
      // subsequent FV/store validation rejects the flash contents. Pool allocations
      // cannot be freed in this phase, so future lookups must reuse this buffer.
      //
      if ((VarStoreLibData != NULL) && (LibStatus == EFI_NOT_FOUND)) {
        Status = SetLibraryData (PcdGet8 (PcdUefiVariableLibId), VarStoreLibData, sizeof (UEFI_VAR_STORE_LIBRARY_DATA));
        if (EFI_ERROR (Status)) {
          return NULL;
        }
      }

      //
      // Check if the Firmware Volume is not corrupted
      //
      if ((FvHeader->Signature != EFI_FVH_SIGNATURE) || (!CompareGuid (&gEfiSystemNvDataFvGuid, &FvHeader->FileSystemGuid))) {
        DEBUG ((DEBUG_ERROR, "Firmware Volume for Variable Store is corrupted\n"));
        break;
      }

      //
      // Bound the FV header offset before locating the variable store header.
      //
      StoreSizeLimit = NvStorageSize / 2;
      if ((StoreSizeLimit < sizeof (UEFI_VARIABLE_STORE_HEADER)) ||
          (FvHeader->HeaderLength < sizeof (*FvHeader)) ||
          (FvHeader->HeaderLength > StoreSizeLimit - sizeof (UEFI_VARIABLE_STORE_HEADER))) {
        VariableStoreHeader = NULL;
        break;
      }

      StoreSizeLimit -= FvHeader->HeaderLength;
      VariableStoreHeader = (UEFI_VARIABLE_STORE_HEADER *) ((UINT8 *) FvHeader + FvHeader->HeaderLength);

      if ((VariableStoreHeader->Size < sizeof (UEFI_VARIABLE_STORE_HEADER)) ||
          (VariableStoreHeader->Size > StoreSizeLimit) ||
          ((((UINTN) VariableStoreHeader + VariableStoreHeader->Size) & (HEADER_ALIGNMENT - 1)) != 0)) {
        DEBUG ((DEBUG_ERROR, "Variable store size 0x%x is out of range (limit 0x%x)\n",
                VariableStoreHeader->Size, StoreSizeLimit));
        VariableStoreHeader = NULL;
        break;
      }

      StoreInfo->AuthFlag = (BOOLEAN) (CompareGuid (&VariableStoreHeader->Signature, &gEfiAuthenticatedVariableGuid));

      if (VarStoreLibData != NULL) {
        if ((LibStatus == EFI_NOT_FOUND) ||
            (VarStoreLibData->IndexTable.StartPtr != GetStartPointer (VariableStoreHeader)) ||
            (VarStoreLibData->IndexTable.EndPtr != GetEndPointer (VariableStoreHeader))) {
          //
          // If it's the first time to access variable region in flash, create a guid hob to record
          // VAR_ADDED type variable info.
          // Note that as the resource of PEI phase is limited, only store the limited number of
          // VAR_ADDED type variables to reduce access time.
          //
          VarStoreLibData->IndexTable.Length      = 0;
          VarStoreLibData->IndexTable.StartPtr    = GetStartPointer (VariableStoreHeader);
          VarStoreLibData->IndexTable.EndPtr      = GetEndPointer   (VariableStoreHeader);
          VarStoreLibData->IndexTable.GoneThrough = 0;
          //
          // Set the Lib data after Ftw and Index table info is updated
          //
          VarStoreLibData->StoreLibVarHdrSet = FALSE;
          Status = SetLibraryData (PcdGet8 (PcdUefiVariableLibId), VarStoreLibData, sizeof (UEFI_VAR_STORE_LIBRARY_DATA));
          if (EFI_ERROR (Status)) {
            return NULL;
          }
        } else {
          StoreInfo->IndexTable = &VarStoreLibData->IndexTable;
        }
      }

      break;

    default:
      ASSERT (FALSE);
      break;
  }

  StoreInfo->VariableStoreHeader = VariableStoreHeader;
  return VariableStoreHeader;
}

/**
  Get variable header that has consecutive content.

  @param StoreInfo      Pointer to variable store info structure.
  @param Variable       Pointer to the Variable Header.
  @param VariableHeader Pointer to Pointer to the Variable Header that has consecutive content.

  @retval TRUE          Variable header is valid.
  @retval FALSE         Variable header is not valid.

**/
BOOLEAN
GetVariableHeader (
  IN UEFI_VARIABLE_STORE_INFO    *StoreInfo,
  IN UEFI_VARIABLE_HEADER        *Variable,
  OUT UEFI_VARIABLE_HEADER       **VariableHeader
  )
{
  EFI_PHYSICAL_ADDRESS            TargetAddress;
  EFI_PHYSICAL_ADDRESS            SpareAddress;
  UINTN                           PartialHeaderSize;
  EFI_STATUS                      LibStatus;
  UEFI_VAR_STORE_LIBRARY_DATA    *VarStoreLibData;
  UINT64                          RecordEnd;
  UINT64                          StoreEnd;
  UINT64                          RecordSize;
  UINTN                           NameSize;
  UINTN                           DataSize;
  UINTN                           Address;

  if (Variable == NULL) {
    return FALSE;
  }

  //
  // First assume variable header pointed by Variable is consecutive.
  //
  *VariableHeader = Variable;

  //
  // Covers the FTW path too, where the checks below only handle the spare block.
  //
  if (!IsVariableInStore (StoreInfo, Variable, GetVariableHeaderSize (StoreInfo->AuthFlag))) {
    return FALSE;
  }

  if (StoreInfo->FtwLastWriteData != NULL) {
    TargetAddress = StoreInfo->FtwLastWriteData->TargetAddress;
    SpareAddress = StoreInfo->FtwLastWriteData->SpareAddress;
    if (((UINTN) Variable > (UINTN) SpareAddress) &&
        (((UINTN) Variable - (UINTN) SpareAddress + (UINTN) TargetAddress) >= (UINTN) GetEndPointer (StoreInfo->VariableStoreHeader))) {
      //
      // Reach the end of variable store.
      //
      return FALSE;
    }
    if (((UINTN) Variable < (UINTN) TargetAddress) && (((UINTN) Variable + GetVariableHeaderSize (StoreInfo->AuthFlag)) > (UINTN) TargetAddress)) {
      //
      // Variable header pointed by Variable is inconsecutive,
      //
      LibStatus = GetLibraryData (PcdGet8 (PcdUefiVariableLibId), (VOID **)&VarStoreLibData);
      if (LibStatus == EFI_NOT_FOUND) {
        return FALSE;
      }

      if (VarStoreLibData->StoreLibVarHdrSet == FALSE) {
        *VariableHeader = &VarStoreLibData->VariableHdr;
        PartialHeaderSize = (UINTN) TargetAddress - (UINTN) Variable;
        //
        // Partial content is in NV storage.
        //
        CopyMem ((UINT8 *) *VariableHeader, (UINT8 *) Variable, PartialHeaderSize);
        //
        // Another partial content is in spare block.
        //
        CopyMem ((UINT8 *) *VariableHeader + PartialHeaderSize, (UINT8 *) (UINTN) SpareAddress, GetVariableHeaderSize (StoreInfo->AuthFlag) - PartialHeaderSize);
        VarStoreLibData->StoreLibVarHdrSet = TRUE;
      } else {
        *VariableHeader = &VarStoreLibData->VariableHdr;
      }
    }
  } else {
    if (Variable >= GetEndPointer (StoreInfo->VariableStoreHeader)) {
      //
      // Reach the end of variable store.
      //
      return FALSE;
    }
  }

  if (!IsValidVariableHeader (*VariableHeader)) {
    return FALSE;
  }

  //
  // Validate the complete record before callers compare its name or copy its data.
  // Normalize a spare-block cursor so the extent is checked in store address space.
  //
  Address    = GetVariableStoreAddress (StoreInfo, (UINTN) Variable);
  StoreEnd   = (UINT64) (UINTN) GetEndPointer (StoreInfo->VariableStoreHeader);
  NameSize   = NameSizeOfVariable (*VariableHeader, StoreInfo->AuthFlag);
  DataSize   = DataSizeOfVariable (*VariableHeader, StoreInfo->AuthFlag);
  RecordSize = (UINT64) GetVariableHeaderSize (StoreInfo->AuthFlag)
             + (UINT64) NameSize + (UINT64) GET_PAD_SIZE (NameSize)
             + (UINT64) DataSize + (UINT64) GET_PAD_SIZE (DataSize);

  RecordEnd = (UINT64) Address + RecordSize;
  if ((RecordEnd < (UINT64) Address) ||
      (RecordEnd > (~(UINT64) 0 - (HEADER_ALIGNMENT - 1)))) {
    return FALSE;
  }

  RecordEnd = (RecordEnd + (HEADER_ALIGNMENT - 1)) & ~(UINT64) (HEADER_ALIGNMENT - 1);
  return (BOOLEAN) (RecordEnd <= StoreEnd);
}

/**
  Get variable name or data to output buffer.

  @param  StoreInfo     Pointer to variable store info structure.
  @param  NameOrData    Pointer to the variable name/data that may be inconsecutive.
  @param  Size          Variable name/data size.
  @param  Buffer        Pointer to output buffer to hold the variable name/data.

**/
VOID
GetVariableNameOrData (
  IN UEFI_VARIABLE_STORE_INFO    *StoreInfo,
  IN UINT8                       *NameOrData,
  IN UINTN                        Size,
  OUT UINT8                      *Buffer
  )
{
  EFI_PHYSICAL_ADDRESS  TargetAddress;
  EFI_PHYSICAL_ADDRESS  SpareAddress;
  UINTN                 PartialSize;

  if (StoreInfo->FtwLastWriteData != NULL) {
    TargetAddress = StoreInfo->FtwLastWriteData->TargetAddress;
    SpareAddress = StoreInfo->FtwLastWriteData->SpareAddress;
    if (((UINTN) NameOrData < (UINTN) TargetAddress) && (((UINTN) NameOrData + Size) > (UINTN) TargetAddress)) {
      //
      // Variable name/data is inconsecutive.
      //
      PartialSize = (UINTN) TargetAddress - (UINTN) NameOrData;
      //
      // Partial content is in NV storage.
      //
      CopyMem (Buffer, NameOrData, PartialSize);
      //
      // Another partial content is in spare block.
      //
      CopyMem (Buffer + PartialSize, (UINT8 *) (UINTN) SpareAddress, Size - PartialSize);
      return;
    }
  }

  //
  // Variable name/data is consecutive.
  //
  CopyMem (Buffer, NameOrData, Size);
}

/**
  Discard cached variable lookup state and force FTW split headers to be rebuilt.

  @param StoreInfo  Pointer to variable store info structure.
  @param PtrTrack   Validated bounds for the current variable store.
**/
STATIC
VOID
ResetVariableIndexCache (
  IN UEFI_VARIABLE_STORE_INFO        *StoreInfo,
  IN UEFI_VARIABLE_POINTER_TRACK     *PtrTrack
  )
{
  EFI_STATUS                   Status;
  UEFI_VAR_STORE_LIBRARY_DATA *VarStoreLibData;

  if (StoreInfo->IndexTable != NULL) {
    StoreInfo->IndexTable->Length      = 0;
    StoreInfo->IndexTable->GoneThrough = 0;
    StoreInfo->IndexTable->StartPtr    = PtrTrack->StartPtr;
    StoreInfo->IndexTable->EndPtr      = PtrTrack->EndPtr;
  }

  if (StoreInfo->FtwLastWriteData != NULL) {
    Status = GetLibraryData (PcdGet8 (PcdUefiVariableLibId), (VOID **)&VarStoreLibData);
    if (!EFI_ERROR (Status) && (VarStoreLibData != NULL)) {
      VarStoreLibData->StoreLibVarHdrSet = FALSE;
    }
  }
}

/**
  Find the variable in the specified variable store.

  @param  StoreInfo           Pointer to the store info structure.
  @param  VariableName        Name of the variable to be found
  @param  VendorGuid          Vendor GUID to be found.
  @param  PtrTrack            Variable Track Pointer structure that contains Variable Information.

  @retval  EFI_SUCCESS            Variable found successfully
  @retval  EFI_NOT_FOUND          Variable not found
  @retval  EFI_INVALID_PARAMETER  Invalid variable name

**/
EFI_STATUS
FindVariableEx (
  IN UEFI_VARIABLE_STORE_INFO         *StoreInfo,
  IN CONST CHAR16                     *VariableName,
  IN CONST EFI_GUID                   *VendorGuid,
  OUT UEFI_VARIABLE_POINTER_TRACK     *PtrTrack
  )
{
  UEFI_VARIABLE_HEADER         *Variable;
  UEFI_VARIABLE_HEADER         *LastVariable;
  UEFI_VARIABLE_HEADER         *MaxIndex;
  UINTN                         Index;
  UINTN                         Offset;
  BOOLEAN                       StopRecord;
  UEFI_VARIABLE_HEADER         *InDeletedVariable;
  UEFI_VARIABLE_STORE_HEADER   *VariableStoreHeader;
  UEFI_VARIABLE_INDEX_TABLE    *IndexTable;
  UEFI_VARIABLE_HEADER         *VariableHeader;

  VariableStoreHeader = StoreInfo->VariableStoreHeader;

  if (VariableStoreHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (GetVariableStoreStatus (VariableStoreHeader) != EfiValid) {
    return EFI_UNSUPPORTED;
  }

  if (~VariableStoreHeader->Size == 0) {
    return EFI_NOT_FOUND;
  }

  IndexTable = StoreInfo->IndexTable;
  PtrTrack->StartPtr = GetStartPointer (VariableStoreHeader);
  PtrTrack->EndPtr   = GetEndPointer   (VariableStoreHeader);

  InDeletedVariable = NULL;

  //
  // No Variable Address equals zero, so 0 as initial value is safe.
  //
  MaxIndex   = NULL;
  VariableHeader = NULL;

  if (IndexTable != NULL) {
    //
    // traverse the variable index table to look for varible.
    // The IndexTable->Index[Index] records the distance of two neighbouring VAR_ADDED type variables.
    //
    if (IndexTable->Length > sizeof (IndexTable->Index) / sizeof (IndexTable->Index[0])) {
      //
      // Discard a corrupted cache before reading any of its entries.
      //
      ResetVariableIndexCache (StoreInfo, PtrTrack);
    }

    for (Offset = 0, Index = 0; Index < IndexTable->Length; Index++) {
      ASSERT (Index < sizeof (IndexTable->Index) / sizeof (IndexTable->Index[0]));
      Offset   += IndexTable->Index[Index];
      MaxIndex  = (UEFI_VARIABLE_HEADER *) ((UINT8 *) IndexTable->StartPtr + Offset);
      if (((Index != 0) && (IndexTable->Index[Index] == 0)) ||
          !GetVariableHeader (StoreInfo, MaxIndex, &VariableHeader) ||
          ((VariableHeader->State != UEFI_VAR_ADDED) &&
           (VariableHeader->State != (UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_ADDED)))) {
        //
        // Discard an invalid cached entry and rebuild from the validated store start.
        //
        ResetVariableIndexCache (StoreInfo, PtrTrack);
        InDeletedVariable = NULL;
        MaxIndex          = NULL;
        VariableHeader    = NULL;
        break;
      }

      if (CompareWithValidVariable (StoreInfo, MaxIndex, VariableHeader, VariableName, VendorGuid, PtrTrack) == EFI_SUCCESS) {
        if (VariableHeader->State == (UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_ADDED)) {
          InDeletedVariable = PtrTrack->CurrPtr;
        } else {
          return EFI_SUCCESS;
        }
      }
    }

    if (IndexTable->GoneThrough != 0) {
      //
      // If the table has all the existing variables indexed, return.
      //
      PtrTrack->CurrPtr = InDeletedVariable;
      return (PtrTrack->CurrPtr == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
    }
  }

  if (MaxIndex != NULL) {
    //
    // HOB exists but the variable cannot be found in HOB
    // If not found in HOB, then let's start from the MaxIndex we've found.
    //
    Variable     = GetNextVariablePtr (StoreInfo, MaxIndex, VariableHeader);
    LastVariable = MaxIndex;
  } else {
    //
    // Start Pointers for the variable.
    // Actual Data Pointer where data can be written.
    //
    Variable     = PtrTrack->StartPtr;
    LastVariable = PtrTrack->StartPtr;
  }

  //
  // Find the variable by walk through variable store
  //
  StopRecord = FALSE;
  while (GetVariableHeader (StoreInfo, Variable, &VariableHeader)) {
    if (VariableHeader->State == UEFI_VAR_ADDED || VariableHeader->State == (UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_ADDED)) {
      //
      // Record Variable in VariableIndex HOB
      //
      if ((IndexTable != NULL) && !StopRecord) {
        if (((UINTN) Variable < (UINTN) LastVariable) ||
            (((UINTN) Variable == (UINTN) LastVariable) && (IndexTable->Length != 0))) {
          //
          // A non-monotonic cursor cannot produce a usable distance.
          //
          StopRecord = TRUE;
        } else {
          Offset = (UINTN) Variable - (UINTN) LastVariable;
          if ((Offset > 0x0FFFF) || (IndexTable->Length >= sizeof (IndexTable->Index) / sizeof (IndexTable->Index[0]))) {
            //
            // Stop to record if the distance of two neighbouring VAR_ADDED variable is larger than the allowable scope(UINT16),
            // or the record buffer is full.
            //
            StopRecord = TRUE;
          } else {
            IndexTable->Index[IndexTable->Length++] = (UINT16) Offset;
            LastVariable = Variable;
          }
        }
      }

      if (CompareWithValidVariable (StoreInfo, Variable, VariableHeader, VariableName, VendorGuid, PtrTrack) == EFI_SUCCESS) {
        if (VariableHeader->State == (UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_ADDED)) {
          InDeletedVariable = PtrTrack->CurrPtr;
        } else {
          return EFI_SUCCESS;
        }
      }
    }

    Variable = GetNextVariablePtr (StoreInfo, Variable, VariableHeader);
  }
  //
  // If gone through the VariableStore, that means we never find in Firmware any more.
  //
  if ((IndexTable != NULL) && !StopRecord) {
    IndexTable->GoneThrough = 1;
  }

  PtrTrack->CurrPtr = InDeletedVariable;

  return (PtrTrack->CurrPtr == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
}

/**
  Find the variable in HOB and Non-Volatile variable storages.

  @param  VariableName  Name of the variable to be found
  @param  VendorGuid    Vendor GUID to be found.
  @param  PtrTrack      Variable Track Pointer structure that contains Variable Information.
  @param  StoreInfo     Return the store info.

  @retval  EFI_SUCCESS            Variable found successfully
  @retval  EFI_NOT_FOUND          Variable not found
  @retval  EFI_INVALID_PARAMETER  Invalid variable name
**/
EFI_STATUS
FindVariable (
  IN CONST  CHAR16                 *VariableName,
  IN CONST  EFI_GUID               *VendorGuid,
  OUT UEFI_VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT UEFI_VARIABLE_STORE_INFO     *StoreInfo
  )
{
  EFI_STATUS                      Status;

  if (VariableName[0] != 0 && VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  GetVariableStore (VariableStoreTypeNv, StoreInfo);
  Status = FindVariableEx (
              StoreInfo,
              VariableName,
              VendorGuid,
              PtrTrack
              );
  if (!EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_NOT_FOUND;
}

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
  @retval EFI_NOT_FOUND         The variable was be found.
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
  )
{
  UEFI_VARIABLE_POINTER_TRACK   Variable;
  UINTN                         VarDataSize;
  EFI_STATUS                    Status;
  UEFI_VARIABLE_STORE_INFO      StoreInfo;
  UEFI_VARIABLE_HEADER         *VariableHeader;
  UINT32                        NvStorageBase;
  UINT32                        NvStorageSize;
  EFI_FIRMWARE_VOLUME_HEADER   *FwVolHeader;

  if (VariableName == NULL || VariableGuid == NULL || DataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (VariableName[0] == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Check for signature/guid and return
  // if a valid NV area is absent
  //
  GetComponentInfo (FLASH_MAP_SIG_UEFIVARIABLE, &NvStorageBase, &NvStorageSize);
  ASSERT (NvStorageBase != 0);
  FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) NvStorageBase;
  if ((FwVolHeader->Signature != EFI_FVH_SIGNATURE) || (!CompareGuid (&gEfiSystemNvDataFvGuid, &FwVolHeader->FileSystemGuid))) {
    DEBUG ((DEBUG_INFO, "Invalid NV area!\n"));
    return EFI_DEVICE_ERROR;
  }

  VariableHeader = NULL;

  //
  // Find existing variable
  //
  Status = FindVariable (VariableName, VariableGuid, &Variable, &StoreInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (!GetVariableHeader (&StoreInfo, Variable.CurrPtr, &VariableHeader)) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Get data size
  //
  VarDataSize = DataSizeOfVariable (VariableHeader, StoreInfo.AuthFlag);
  if (*DataSize >= VarDataSize) {
    if (Data == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    GetVariableNameOrData (&StoreInfo, GetVariableDataPtr (Variable.CurrPtr, VariableHeader, StoreInfo.AuthFlag), VarDataSize, Data);

    if (Attributes != NULL) {
      *Attributes = VariableHeader->Attributes;
    }

    *DataSize = VarDataSize;
    return EFI_SUCCESS;
  } else {
    *DataSize = VarDataSize;
    return EFI_BUFFER_TOO_SMALL;
  }
}
