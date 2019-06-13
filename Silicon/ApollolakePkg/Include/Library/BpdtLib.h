/** @file
    Bpdt library header

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __BPDT_HEADER_H__
#define __BPDT_HEADER_H__
#include <Library/BootMediaLib.h>

#define BPDT_SIGN_GREEN       0x000055AA    //Normal Boot
#define BPDT_SIGN_RED         0xFFFFFFFF    //any other value, here select to use this value.

#define MAX_PARTITION_NUM     0x10
#define BPDT_SIZE             0x1000

typedef struct {
  VOID   *DataPtr;
  UINT32  Size;
} BPDT_PAYLOAD_DATA;

typedef enum {
  BpdtOemSmip,      // 0
  BpdtCseRbe,       // 1
  BpdtCseBup,       // 2
  BpdtUcode,        // 3
  BpdtIbb,          // 4
  BpdtSbpdt,        // 5 - Secondary BPDT within a BPDT Entry
  BpdtObb,          // 6
  BpdtCseMain,      // 7
  BpdtIsh,          // 8
  BpdtCseIdlm,      // 9
  BpdtIfpOverride,  //10
  BpdtDebugTokens,  //11
  BpdtUfsPhyConfig, //12
  BpdtUfsGppLunId,  //13
  BpdtPmc,          //14
  BpdtIunit,        //15
  BpdtMaxType
} BPDT_ENTRY_TYPES;

typedef struct {
  UINT32  Signature;
  UINT16  DscCount;
  UINT16  BpdtVer;
  UINT32  RedundantBlockXorSum;
  UINT32  IfwiVer;
  UINT64  FitToolVer;
} BPDT_HEADER;


typedef struct {
  UINT16  Type;
  UINT16  Flags;
  UINT32  LbpOffset;  // Offset of Sub-Partition starting from base of LBP
  UINT32  Size;       // Size of the Sub-Partition
} BPDT_ENTRY;


typedef struct {
  UINT32  HeaderMarker;
  UINT32  NumOfEntries;
  UINT8   HeaderVersion;
  UINT8   EntryVersion;
  UINT8   HeaderLength;
  UINT8   Checksum;
  UINT32  SubPartName;
} SUBPART_DIR_HEADER;


typedef struct {
  CHAR8   EntryName[12];
  UINT32  EntryOffset:25;
  UINT32  Huffman:1;
  UINT32  Reserved1:6;
  UINT32  EntrySize;
  UINT32  Reserved2;
} SUBPART_DIR_ENTRY;


EFI_STATUS
EFIAPI
GetBootPartitionLinearAddr (
  IN  BOOT_PARTITION_SELECT LbpSel,
  OUT UINT32                *BpLinearAddr
  );

/**
  Locate and return a pointer to the selected Logical Boot Partition.

  For block devices, the address will be the location in memory that the BP was copied to.
  For memory mapped device (eg SPI), the address will be the location on the device.

  @param    LbpSel         Which BP to locate on the boot device
  @param    Address        The address of the selected BP on the boot device

  @retval   EFI_SUCCESS    The operation completed successfully.
  @retval   other          The selected BP could not be found.
**/
EFI_STATUS
EFIAPI
GetBootPartitionPointer (
  IN  BOOT_PARTITION_SELECT LbpSel,
  OUT VOID                  **LbpPointer
  );

/**
  Gets the address information from BPDT.

  NOTE: when Partition is not a valid boot partition, this function will load firmware
  from current boot partition.

  @param[in]  Partition          The boot partition to load firmware.
  @param[in]  EntryType          Identify primary BPDT (BpdtIbb) or second-BPDT partition (BpdtObb).
  @param[in]  Name               The file name in the BPDT section.

  @retval   EFI_SUCCESS          Address info found successfully.
  @retval   Others               Failed to get the info.

**/
EFI_STATUS
GetAddressInfoFromBpdt (
  BOOT_PARTITION_SELECT          Partition,
  BPDT_ENTRY_TYPES               EntryType,
  CONST CHAR8                    *Name,
  BPDT_PAYLOAD_DATA              *SubPartitoinInfo
  );

/**
  Parse the Boot Partition Descriptor Table of the provided Logical Boot Partition.
  Note: "payload" is the Sub-Partition pointed to by the BDPT Entry of the given type.

  @param   LbpPointer      Pointer to the start of the BootPartition to be parsed
  @param   EntryType       The entry type of the subparition(payload) to look for
  @param   BpdtPayloadPtr  Address of the Struct to put the results into (MUST be allocated by caller)

  @retval  EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBpdtDataByType (
  IN  VOID                *LbpPointer,
  IN  BPDT_ENTRY_TYPES    EntryType,
  OUT BPDT_PAYLOAD_DATA   *BpdtPayloadPtr
  );

/**
  Search the Subpartition pointed to by BpdtPayloadPtr for a Directory entry with "EntryName",
  then fill in the Struct pointed to by SubPartPayloadPtr with the Addres and Size of the Dir Entry.
  Note: "payload" is the Data pointed to by the Directory Entry with the given name.

  @param   BpdtPayloadPtr     Location and Size of the Payload to search
  @param   EntryName          the String name of the Directory entry to look for
  @param   SubPartPayloadPtr  Address of the Struct to put the results into (MUST be allocated by caller)

  @retval  EFI_SUCCESS        The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetSubPartitionDataByName (
  IN  BPDT_PAYLOAD_DATA     *BpdtPayloadPtr,
  IN  CONST CHAR8           *EntryName,
  OUT BPDT_PAYLOAD_DATA     *SubPartPayloadPtr
  );

EFI_STATUS
EFIAPI
GetPartitionSize (
  OUT UINT32                     *PartitionSize
  );

EFI_STATUS
EFIAPI
BootDeviceRead (
  IN     UINT32                  Address,
  IN     UINT32                  ByteCount,
  OUT    UINT8                   *Buffer
  );

EFI_STATUS
EFIAPI
BootDeviceWrite (
  IN     UINT32                  Address,
  IN     UINT32                  ByteCount,
  IN     UINT8                   *Buffer
  );

/**
  Erase some area on the flash part.

  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
BootDeviceErase (
  IN     UINT32                  Address,
  IN     UINT32                  ByteCount
  );

VOID
EFIAPI
InitializeBootDevice(
  VOID
  );
#endif
