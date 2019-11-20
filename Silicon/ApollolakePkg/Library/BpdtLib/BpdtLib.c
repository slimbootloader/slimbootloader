/** @file
  This file contains the implementation of BpdtLib library.

  The library exposes an abstract interface for accessing boot data
  stored in the BPDT format on the Logical Boot Partition of the boot device.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/BootMediaLib.h>
#include <Library/BpdtLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/FlashDescriptorLib.h>

#define DIR_HDR_SIGNATURE     SIGNATURE_32('$', 'C', 'P', 'D')
#define DIR_ENTRY_NAME_SIZE   12   //This is spec defined to be bytes


/**
  Calculate the Address in SPI of the selected Boot Partition

  @param    BpSel                 Which BP to locate on the SPI
  @param    BpFlashLinearAddress  The linear flash address of selected BP
  @param    LbpPointer            A pointer to the selected Boot partition on this media

  @retval   EFI_SUCCESS           The operation completed successfully.
**/
EFI_STATUS
EFIAPI
InternalGetBpSpi (
  IN  BOOT_PARTITION_SELECT      BpSel,
  OUT UINT32                     *BpFlashLinearAddress,
  OUT VOID                       **LbpPointer OPTIONAL
  )
{
  UINT32                         BiosAddr;

  //
  // Compute BP FlashLinearAddress
  //
  switch (BpSel) {
    case BootPartition1:
      *BpFlashLinearAddress = 0x1000;
      break;
    case BootPartition2:
        //
        // BP2 linear address is the midpoint between BIOS base and expansion data base
        //
        *BpFlashLinearAddress = (GetSpiFlashRegionBase(BIOS) + GetSpiFlashRegionLimit(BIOS) + 1) / 2;
        DEBUG ((DEBUG_INFO, "BpFlashLinearAddress = %X\n", *BpFlashLinearAddress));
      break;
    default:
      return EFI_NOT_FOUND;
  }

  //
  // Calculate Boot partition physical address
  // FLA[26:0] <= (Flash_Region_Limit) - (FFFF_FFFCh - bios_address)
  //
  BiosAddr = GetSpiFlashRegionLimit(BIOS) - *BpFlashLinearAddress;
  if (LbpPointer != NULL) {
    *LbpPointer = (VOID *)((UINTN)0xFFFFFFFF - BiosAddr);
  }

  DEBUG ((DEBUG_INFO, "Flash address of BP%d = %X\n", BpSel+1, (VOID *)((UINTN)0xFFFFFFFF - BiosAddr)));

  // since BPDT is in front of BIOS region, need minus that one.
  *BpFlashLinearAddress -= BPDT_SIZE;
  DEBUG ((DEBUG_INFO, "BpFlashLinearAddress = %X\n", *BpFlashLinearAddress));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetBootPartitionLinearAddr (
  IN  BOOT_PARTITION_SELECT      LbpSel,
  OUT UINT32                     *BpLinearAddr
  )
{
  BOOT_MEDIA_TYPE                BootMedia;
  EFI_STATUS                     Status;

  Status = GetBootMediaType (&BootMedia);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (BootMedia) {
    case BootMediaEmmc:
    case BootMediaUfs:
      DEBUG ((DEBUG_INFO, "Boot Device is EMMC/UFS, not support now.\n"));
      Status = EFI_NOT_FOUND; //InternalGetBpEmmcUfs (LbpSel, BootMedia, LbpPointer);
      break;
    case BootMediaSpi:
      Status = InternalGetBpSpi (LbpSel, BpLinearAddr, NULL);
      break;
    default:
      DEBUG ((DEBUG_ERROR, "Unable to determine boot device!\n"));
      Status = EFI_NOT_FOUND;
      break;
  }
  return Status;
}

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
  )
{
    EFI_STATUS                     Status;
    VOID                           *BpPointer;
    BPDT_PAYLOAD_DATA              BpdtInfo;
    BOOT_PARTITION_SELECT          BootPartition;

    if (!BootMediaIsSpi()) {
      DEBUG ((DEBUG_ERROR, "LoadFirmware ERROR: ONLY support SPI boot now. could add others later\n"));
      ASSERT(FALSE);
      return EFI_ABORTED;
    }

    if (Partition >= BootPartitionMax) {
      Status = GetBootPartition (&BootPartition);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "GetBootPartition, Status=0x%x\n", Status));
        return Status;
      }
    } else {
      BootPartition = Partition;
    }
    DEBUG ((DEBUG_INFO, "BootPartition=0x%x\n", BootPartition));

    //
    // FwPayload exists in primary boot partion in both BP1 and BP2
    //
    Status = GetBootPartitionPointer(BootPartition, &BpPointer);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "GetBootPartitionPointer, Status=0x%x\n", Status));
      return Status;
    }

    DEBUG ((DEBUG_INFO, "BpPointer=0x%p\n", BpPointer));
    Status = GetBpdtDataByType (BpPointer, EntryType, &BpdtInfo);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "GetBpdtDataByType, Status=0x%x\n", Status));
      return Status;
    }

    DEBUG ((DEBUG_INFO, "DataPtr=0x%p, size=0x%x\n", BpdtInfo.DataPtr, BpdtInfo.Size));
    Status = GetSubPartitionDataByName (&BpdtInfo, Name, SubPartitoinInfo);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "GetSubPartitionDataByName, Status=0x%x\n", Status));
      return Status;
    }

    return Status;
}


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
  IN  BOOT_PARTITION_SELECT      LbpSel,
  OUT VOID                       **LbpPointer
  )
{
  BOOT_MEDIA_TYPE                BootMedia;
  UINT32                         BpFlashLinearAddress;
  EFI_STATUS                     Status;

  Status = GetBootMediaType (&BootMedia);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (BootMedia) {
    case BootMediaEmmc:
    case BootMediaUfs:
      DEBUG ((DEBUG_INFO, "Boot Device is EMMC/UFS, not support now.\n"));
      Status = EFI_NOT_FOUND; //InternalGetBpEmmcUfs (LbpSel, BootMedia, LbpPointer);
      break;
    case BootMediaSpi:
      DEBUG ((DEBUG_INFO, "Boot Device is SPI.\n"));
      Status = InternalGetBpSpi (LbpSel, &BpFlashLinearAddress, LbpPointer);
      break;
    default:
      DEBUG ((DEBUG_ERROR, "Unable to determine boot device!\n"));
      Status = EFI_NOT_FOUND;
      break;
  }
  return Status;
}

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
  IN  VOID                       *LbpPointer,
  IN  BPDT_ENTRY_TYPES           EntryType,
  OUT BPDT_PAYLOAD_DATA          *BpdtPayloadPtr
  )
{
  UINT8                          Index;
  VOID                           *LbpOffset;
  BPDT_HEADER                    *BpdtHeaderPtr;
  BPDT_HEADER                    *sBpdtHeader;
  BPDT_ENTRY                     *BpdtEntryPtr;


  BpdtHeaderPtr = (BPDT_HEADER *)LbpPointer;
  do {
    //reset secondary header each time to ensure proper exit
    sBpdtHeader = NULL;
    BpdtEntryPtr = (BPDT_ENTRY *)((UINTN)BpdtHeaderPtr + sizeof(BPDT_HEADER));

    DEBUG ((DEBUG_INFO, "BpdtHeaderPtr->Signature = %x\n", BpdtHeaderPtr->Signature));
    if (BpdtHeaderPtr->Signature != BPDT_SIGN_GREEN) {
      DEBUG ((DEBUG_ERROR, "Signature of BPDT Header is invalid - Stop Parsing RawBP.\n"));
      return EFI_NOT_FOUND;
    }
    for (Index=0; Index < BpdtHeaderPtr->DscCount; Index++, BpdtEntryPtr++) {
      if (BpdtEntryPtr->LbpOffset == 0) {
        DEBUG ((DEBUG_INFO, "Skipping Stub-entry for Bpdt type: %d\n", BpdtEntryPtr->Type));
        continue;
      }
      LbpOffset = (VOID *)((UINTN)LbpPointer + BpdtEntryPtr->LbpOffset);

      if (BpdtEntryPtr->Type == BpdtSbpdt) {
        sBpdtHeader = (BPDT_HEADER *)LbpOffset;
      }
      if (BpdtEntryPtr->Type == EntryType) {
        DEBUG ((DEBUG_INFO, "Found BPDT Entry. Type: %d, Addr: 0x%x\n", BpdtEntryPtr->Type, LbpOffset));
        BpdtPayloadPtr->DataPtr = LbpOffset;
        BpdtPayloadPtr->Size = BpdtEntryPtr->Size;
        return EFI_SUCCESS;
      }
    }
    if (sBpdtHeader != NULL) {
      BpdtHeaderPtr = sBpdtHeader;
    }
  } while (sBpdtHeader != NULL);

  BpdtPayloadPtr->DataPtr = NULL;
  BpdtPayloadPtr->Size = 0;
  return EFI_NOT_FOUND;
}


/**
  Search the Subpartition pointed to by BpdtPayloadPtr for a Directory entry with "EntryName",
  then fill in the Struct pointed to by SubPartPayloadPtr with the Addres and Size of the Dir Entry.
  Note: "payload" is the Data pointed to by the Directory Entry with the given name.

  @param   BpdtPayloadPtr     Location and Size of the Payload to search
  @param   EntryName          the String name of the Directory entry to look for
  @param   SubPartPayloadPtr  Address of the Struct to put the results into (MUST be allocated by caller)

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  Others             Error happens.
**/
EFI_STATUS
EFIAPI
GetSubPartitionDataByName (
  IN  BPDT_PAYLOAD_DATA          *BpdtPayloadPtr,
  IN  CONST CHAR8                *EntryName,
  OUT BPDT_PAYLOAD_DATA          *SubPartPayloadPtr
  )
{
  UINT8                          Index;
  CHAR8                          SearchName[DIR_ENTRY_NAME_SIZE];
  UINTN                          SearchNameSize;
  SUBPART_DIR_HEADER             *DirectoryHeader;
  SUBPART_DIR_ENTRY              *DirectoryEntry;

  //pad Name with zeros (per spec) to ensure correct match
  ZeroMem(SearchName, DIR_ENTRY_NAME_SIZE);
  //Safely handle the input string to ensure we dont copy garbage when EntryName is <12
  SearchNameSize = AsciiStrnLenS (EntryName, DIR_ENTRY_NAME_SIZE);
  CopyMem(SearchName, EntryName, SearchNameSize);

  DEBUG ((DEBUG_INFO, "Looking for Directory Entry with Name: %a\n", SearchName));
  DirectoryHeader = (SUBPART_DIR_HEADER *)BpdtPayloadPtr->DataPtr;
  DirectoryEntry = (SUBPART_DIR_ENTRY *)((UINTN)DirectoryHeader + sizeof(SUBPART_DIR_HEADER));

  if (DirectoryHeader->HeaderMarker != DIR_HDR_SIGNATURE) {
    DEBUG ((DEBUG_INFO, "Directory Header is invalid - return Not_Found.\n"));
    return EFI_NOT_FOUND;
  }
  for (Index = 0; Index < DirectoryHeader->NumOfEntries; Index++, DirectoryEntry++) {
    DEBUG ((DEBUG_INFO, " %a Entry is located at %x\n",
    DirectoryEntry->EntryName, (UINTN)DirectoryHeader+DirectoryEntry->EntryOffset));
    if (!CompareMem(DirectoryEntry->EntryName, SearchName, DIR_ENTRY_NAME_SIZE)) {
      SubPartPayloadPtr->DataPtr = (VOID *)((UINTN)DirectoryHeader + DirectoryEntry->EntryOffset);
      SubPartPayloadPtr->Size = DirectoryEntry->EntrySize;
      return EFI_SUCCESS;
    }
  }
  DEBUG ((DEBUG_ERROR, "Entry not found.\n"));
  return EFI_NOT_FOUND;
}


/**
  Get BPDT partition size.

  NOTE: Two BPDT partition size is same.

  @param[out]   PartitionSize    The partition size.

  @retval  EFI_SUCCESS           Get the partition size successfully.
  @retval  Others                Failed to get partition size.

**/
EFI_STATUS
EFIAPI
GetPartitionSize (
  OUT UINT32                     *PartitionSize
  )
{
  EFI_STATUS                     Status;
  VOID                           *BpPointer1;
  VOID                           *BpPointer2;

   Status = GetBootPartitionPointer(BootPartition1, &BpPointer1);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Get BootPartition1, Status=0x%x\n", Status));
    return Status;
  }

  Status = GetBootPartitionPointer(BootPartition2, &BpPointer2);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Get BootPartition2, Status=0x%x\n", Status));
    return Status;
  }
  DEBUG((DEBUG_INFO, "BpPointer2 =0x%p. BpPointer1=0x%p\n", BpPointer2, BpPointer1));
  *PartitionSize  = (UINT32)BpPointer2 - (UINT32)BpPointer1;

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
BootDeviceRead (
  IN     UINT32                  Address,
  IN     UINT32                  ByteCount,
  OUT    UINT8                   *Buffer
  )
{
  EFI_STATUS                     Status;
  BOOT_MEDIA_TYPE                BootMedia;

  Status = GetBootMediaType (&BootMedia);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (BootMedia) {
    case BootMediaEmmc:
    case BootMediaUfs:
      DEBUG ((DEBUG_INFO, "Boot Device is EMMC/UFS, not support now.\n"));
      Status = EFI_NOT_FOUND;
      break;
    case BootMediaSpi:
      Status = SpiFlashRead (BIOS, Address,  ByteCount, Buffer);
      break;
    default:
      DEBUG ((DEBUG_ERROR, "Unable to determine boot device!\n"));
      Status = EFI_NOT_FOUND;
      break;
  }
  return Status;
}

EFI_STATUS
EFIAPI
BootDeviceWrite (
  IN     UINT32                  Address,
  IN     UINT32                  ByteCount,
  IN     UINT8                   *Buffer
  )
{
  EFI_STATUS                     Status;
  BOOT_MEDIA_TYPE                BootMedia;

  Status = GetBootMediaType (&BootMedia);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (BootMedia) {
    case BootMediaEmmc:
    case BootMediaUfs:
      DEBUG ((DEBUG_INFO, "Boot Device is EMMC/UFS, not support now.\n"));
      Status = EFI_NOT_FOUND;
      break;
    case BootMediaSpi:
      Status = SpiFlashWrite (BIOS, Address,  ByteCount, Buffer);
      break;
    default:
      DEBUG ((DEBUG_ERROR, "Unable to determine boot device!\n"));
      Status = EFI_NOT_FOUND;
      break;
  }
  return Status;
}


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
  )
{
  EFI_STATUS                     Status;
  BOOT_MEDIA_TYPE                BootMedia;

  Status = GetBootMediaType (&BootMedia);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (BootMedia) {
    case BootMediaEmmc:
    case BootMediaUfs:
      DEBUG ((DEBUG_INFO, "Boot Device is EMMC/UFS, not support now.\n"));
      Status = EFI_NOT_FOUND;
      break;
    case BootMediaSpi:
      Status = SpiFlashErase (BIOS, Address, ByteCount);
      break;
    default:
      DEBUG ((DEBUG_ERROR, "BootDeviceErase: Unable to determine boot device!\n"));
      Status = EFI_NOT_FOUND;
      break;
  }
  return Status;
}

/**
  Initialize boot device.

**/
VOID
EFIAPI
InitializeBootDevice(
  VOID
  )
{
  EFI_STATUS                     Status;
  BOOT_MEDIA_TYPE                BootMedia;

  Status = GetBootMediaType (&BootMedia);
  ASSERT_EFI_ERROR (Status);

  switch (BootMedia) {
    case BootMediaEmmc:
    case BootMediaUfs:
      DEBUG ((DEBUG_INFO, "Boot Device is EMMC/UFS, not support now.\n"));
      ASSERT (FALSE);
      break;
    case BootMediaSpi:
      Status = SpiConstructor ();
      ASSERT_EFI_ERROR (Status);
      break;
    default:
      DEBUG ((DEBUG_ERROR, "Unable to determine boot device!\n"));
      ASSERT (FALSE);
      break;
  }

}

