/** @file
  UfsPassThruDxe driver is used to produce EFI_EXT_SCSI_PASS_THRU protocol interface
  for upper layer application to execute UFS-supported SCSI cmds.

  Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FTW_LAST_WRITE_H_
#define _FTW_LAST_WRITE_H_

#include <PiPei.h>

#pragma pack(1)

///
/// The EDKII Fault tolerant working block header.
/// The header is immediately followed by the write queue data.
///
typedef struct {
  ///
  /// FTW working block signature.
  /// Its value has be updated from gEfiSystemNvDataFvGuid to gEdkiiWorkingBlockSignatureGuid,
  /// because its write queue data format has been updated to support the crossing archs.
  ///
  EFI_GUID  Signature;
  ///
  /// 32bit CRC calculated for this header.
  ///
  UINT32    Crc;
  ///
  /// Working block valid bit.
  ///
  UINT8     WorkingBlockValid : 1;
  UINT8     WorkingBlockInvalid : 1;
  UINT8     Reserved : 6;
  UINT8     Reserved3[3];
  ///
  /// Total size of the following write queue range.
  ///
  UINT64    WriteQueueSize;
  ///
  /// Write Queue data.
  ///
  /// EFI_FAULT_TOLERANT_WRITE_HEADER FtwHeader;
  /// EFI_FAULT_TOLERANT_WRITE_RECORD FtwRecord[FtwHeader.NumberOfWrites]
  /// EFI_FAULT_TOLERANT_WRITE_HEADER FtwHeader2;
  /// EFI_FAULT_TOLERANT_WRITE_RECORD FtwRecord2[FtwHeader2.NumberOfWrites]
  /// ...
  ///
} UEFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER;

#define FTW_VALID_STATE     0
#define FTW_INVALID_STATE   1

//
// EFI Fault tolerant block update write queue entry.
//
typedef struct {
  UINT8     HeaderAllocated : 1;
  UINT8     WritesAllocated : 1;
  UINT8     Complete : 1;
  UINT8     Reserved : 5;
  EFI_GUID  CallerId;
  UINT64    NumberOfWrites;
  UINT64    PrivateDataSize;
} UEFI_FAULT_TOLERANT_WRITE_HEADER;

//
// EFI Fault tolerant block update write queue record.
//
typedef struct {
  UINT8   BootBlockUpdate : 1;
  UINT8   SpareComplete : 1;
  UINT8   DestinationComplete : 1;
  UINT8   Reserved : 5;
  EFI_LBA Lba;
  UINT64  Offset;
  UINT64  Length;
  //
  // Relative offset to spare block.
  //
  INT64   RelativeOffset;
  //
  // UINT8    PrivateData[PrivateDataSize]
  //
} UEFI_FAULT_TOLERANT_WRITE_RECORD;

#pragma pack()

extern EFI_GUID gEdkiiWorkingBlockSignatureGuid;

#define FTW_WORKING_SIZE    0x2000

#define FTW_RECORD_SIZE(PrivateDataSize)  (sizeof (UEFI_FAULT_TOLERANT_WRITE_RECORD) + (UINTN) PrivateDataSize)

#define FTW_RECORD_TOTAL_SIZE(NumberOfWrites, PrivateDataSize) \
    ((UINTN) (NumberOfWrites) * (sizeof (UEFI_FAULT_TOLERANT_WRITE_RECORD) + (UINTN) PrivateDataSize))

#define FTW_WRITE_TOTAL_SIZE(NumberOfWrites, PrivateDataSize) \
    ( \
      sizeof (UEFI_FAULT_TOLERANT_WRITE_HEADER) + (UINTN) (NumberOfWrites) * \
      (sizeof (UEFI_FAULT_TOLERANT_WRITE_RECORD) + (UINTN) PrivateDataSize) \
    )

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
  );

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
  );

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
  );

#endif
