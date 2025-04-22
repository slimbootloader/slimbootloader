/** @file
  Implement ReadOnly Variable Services required by PEIM and install
  PEI ReadOnly Varaiable2 PPI. These services operates the non volatile storage space.

Copyright (c) 2006 - 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include "FtwLastWrite.h"
#include <Library/BootloaderCommonLib.h>
#include <Library/UefiVariableLib.h>


/**
  Get the last Write Header pointer.
  The last write header is the header whose 'complete' state hasn't been set.
  After all, this header may be a EMPTY header entry for next Allocate.


  @param FtwWorkSpaceHeader Pointer of the working block header
  @param FtwWorkSpaceSize   Size of the work space
  @param FtwWriteHeader     Pointer to retrieve the last write header

  @retval  EFI_SUCCESS      Get the last write record successfully
  @retval  EFI_ABORTED      The FTW work space is damaged

**/
EFI_STATUS
FtwGetLastWriteHeader (
  IN  UEFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER  *FtwWorkSpaceHeader,
  IN  UINTN                                    FtwWorkSpaceSize,
  OUT UEFI_FAULT_TOLERANT_WRITE_HEADER         **FtwWriteHeader
  )
{
  UINTN                           Offset;
  UEFI_FAULT_TOLERANT_WRITE_HEADER *FtwHeader;

  *FtwWriteHeader = NULL;
  FtwHeader       = (UEFI_FAULT_TOLERANT_WRITE_HEADER *) (FtwWorkSpaceHeader + 1);
  Offset          = sizeof (UEFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER);

  while (FtwHeader->Complete == FTW_VALID_STATE) {
    Offset += FTW_WRITE_TOTAL_SIZE (FtwHeader->NumberOfWrites, FtwHeader->PrivateDataSize);
    //
    // If Offset exceed the FTW work space boudary, return error.
    //
    if (Offset >= FtwWorkSpaceSize) {
      *FtwWriteHeader = FtwHeader;
      return EFI_ABORTED;
    }

    FtwHeader = (UEFI_FAULT_TOLERANT_WRITE_HEADER *) ((UINT8 *) FtwWorkSpaceHeader + Offset);
  }
  //
  // Last write header is found
  //
  *FtwWriteHeader = FtwHeader;

  return EFI_SUCCESS;
}

/**
  Get the last Write Record pointer. The last write Record is the Record
  whose DestinationCompleted state hasn't been set. After all, this Record
  may be a EMPTY record entry for next write.


  @param FtwWriteHeader  Pointer to the write record header
  @param FtwWriteRecord  Pointer to retrieve the last write record

  @retval EFI_SUCCESS        Get the last write record successfully
  @retval EFI_ABORTED        The FTW work space is damaged

**/
EFI_STATUS
FtwGetLastWriteRecord (
  IN  UEFI_FAULT_TOLERANT_WRITE_HEADER          *FtwWriteHeader,
  OUT UEFI_FAULT_TOLERANT_WRITE_RECORD         **FtwWriteRecord
  )
{
  UINTN                           Index;
  UEFI_FAULT_TOLERANT_WRITE_RECORD *FtwRecord;

  *FtwWriteRecord = NULL;
  FtwRecord       = (UEFI_FAULT_TOLERANT_WRITE_RECORD *) (FtwWriteHeader + 1);

  //
  // Try to find the last write record "that has not completed"
  //
  for (Index = 0; Index < FtwWriteHeader->NumberOfWrites; Index += 1) {
    if (FtwRecord->DestinationComplete != FTW_VALID_STATE) {
      //
      // The last write record is found
      //
      *FtwWriteRecord = FtwRecord;
      return EFI_SUCCESS;
    }

    FtwRecord++;

    if (FtwWriteHeader->PrivateDataSize != 0) {
      FtwRecord = (UEFI_FAULT_TOLERANT_WRITE_RECORD *) ((UINTN) FtwRecord + (UINTN) FtwWriteHeader->PrivateDataSize);
    }
  }
  //
  //  if Index == NumberOfWrites, then
  //  the last record has been written successfully,
  //  but the Header->Complete Flag has not been set.
  //  also return the last record.
  //
  if (Index == FtwWriteHeader->NumberOfWrites) {
    *FtwWriteRecord = (UEFI_FAULT_TOLERANT_WRITE_RECORD *) ((UINTN) FtwRecord - FTW_RECORD_SIZE (FtwWriteHeader->PrivateDataSize));
    return EFI_SUCCESS;
  }

  return EFI_ABORTED;
}

/**
  Check to see if it is a valid work space.


  @param WorkingHeader   Pointer of working block header
  @param WorkingLength   Working block length

  @retval TRUE          The work space is valid.
  @retval FALSE         The work space is invalid.

**/
BOOLEAN
IsValidWorkSpace (
  IN UEFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER    *WorkingHeader,
  IN UINTN                                        WorkingLength
  )
{
  UINT8 Data;

  if (WorkingHeader == NULL) {
    return FALSE;
  }

  if ((WorkingHeader->WorkingBlockValid != FTW_VALID_STATE) || (WorkingHeader->WorkingBlockInvalid == FTW_VALID_STATE)) {
    DEBUG ((DEBUG_ERROR, "FtwPei: Work block header valid bit check error\n"));
    return FALSE;
  }

  if (WorkingHeader->WriteQueueSize != (WorkingLength - sizeof (UEFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER))) {
    DEBUG ((DEBUG_ERROR, "FtwPei: Work block header WriteQueueSize check error\n"));
    return FALSE;
  }

  //
  // Check signature with gEdkiiWorkingBlockSignatureGuid
  //
  if (!CompareGuid (&gEdkiiWorkingBlockSignatureGuid, &WorkingHeader->Signature)) {
    DEBUG ((DEBUG_ERROR, "FtwPei: Work block header signature check error, it should be gEdkiiWorkingBlockSignatureGuid\n"));
    //
    // To be compatible with old signature gEfiSystemNvDataFvGuid.
    //
    if (!CompareGuid (&gEfiSystemNvDataFvGuid, &WorkingHeader->Signature)) {
      return FALSE;
    } else {
      Data = *(UINT8 *) (WorkingHeader + 1);
      if (Data != 0xff) {
        DEBUG ((DEBUG_ERROR, "FtwPei: Old format FTW structure can't be handled\n"));
        ASSERT (FALSE);
        return FALSE;
      }
    }
  }

  return TRUE;

}
