/** @file
  Routines supporting partition discovery

Copyright (c) 2006 - 2022, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <IndustryStandard/Mbr.h>
#include <Uefi/UefiGpt.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/Crc32Lib.h>
#include <Library/DebugLib.h>
#include <Library/PartitionLib.h>
#include <Library/MediaAccessLib.h>

//
// Extract INT32 from char array
//
#define UNPACK_INT32(a) \
  (INT32) ((((UINT8 *) a)[0] << 0) | (((UINT8 *) a)[1] << 8) | (((UINT8 *) a)[2] << 16) | (((UINT8 *) a)[3] << 24))

//
// Extract UINT32 from char array
//
#define UNPACK_UINT32(a) \
  (UINT32) ((((UINT8 *) a)[0] << 0) | (((UINT8 *) a)[1] << 8) | (((UINT8 *) a)[2] << 16) | (((UINT8 *) a)[3] << 24))


CHAR8 *mPartTypeName[] = {
  "UNKNOWN",
  "MBR",
  "GPT"
};

extern
EFI_STATUS
FindSpiPartitions (
  IN OUT PART_BLOCK_DEVICE  *PartBlockDev
  );

/**
  Convert partition type into readable string.

  @param  Type              Partition type

  @retval                   Ascii string for partition type

**/
CHAR8 *
GetPartitionTypeName (
  IN UINTN    Type
  )
{
  if (Type >= EnumPartTypeMax) {
    Type = EnumPartTypeUnknown;
  }
  return mPartTypeName[Type];
}


/**
  Test to see if the Mbr buffer is a valid MBR.

  @param  Mbr               Parent Handle
  @param  LastLba           Last Lba address on the device.

  @retval TRUE              Mbr is a Valid MBR
  @retval FALSE             Mbr is not a Valid MBR

**/
BOOLEAN
PartitionValidMbr (
  IN  MASTER_BOOT_RECORD      *Mbr,
  IN  EFI_PEI_LBA             LastLba
  )
{
  UINT32  StartingLBA;
  UINT32  EndingLBA;
  UINT32  NewEndingLBA;
  INTN    Index1;
  INTN    Index2;
  BOOLEAN MbrValid;

  if (Mbr->Signature != MBR_SIGNATURE) {
    return FALSE;
  }

  //
  // The BPB also has this signature, so it can not be used alone.
  //
  MbrValid = FALSE;
  for (Index1 = 0; Index1 < MAX_MBR_PARTITIONS; Index1++) {
    if (Mbr->Partition[Index1].OSIndicator == 0x00 || UNPACK_UINT32 (Mbr->Partition[Index1].SizeInLBA) == 0) {
      continue;
    }

    MbrValid    = TRUE;
    StartingLBA = UNPACK_UINT32 (Mbr->Partition[Index1].StartingLBA);
    EndingLBA   = StartingLBA + UNPACK_UINT32 (Mbr->Partition[Index1].SizeInLBA) - 1;
    if (EndingLBA > LastLba) {
      //
      // Compatability Errata:
      //  Some systems try to hide drive space with thier INT 13h driver
      //  This does not hide space from the OS driver. This means the MBR
      //  that gets created from DOS is smaller than the MBR created from
      //  a real OS (NT & Win98). This leads to BlockIo->LastBlock being
      //  wrong on some systems FDISKed by the OS.
      //
      //  return FALSE Because no block devices on a system are implemented
      //  with INT 13h
      //
      return FALSE;
    }

    for (Index2 = Index1 + 1; Index2 < MAX_MBR_PARTITIONS; Index2++) {
      if (Mbr->Partition[Index2].OSIndicator == 0x00 || UNPACK_INT32 (Mbr->Partition[Index2].SizeInLBA) == 0) {
        continue;
      }

      NewEndingLBA = UNPACK_UINT32 (Mbr->Partition[Index2].StartingLBA) + UNPACK_UINT32 (Mbr->Partition[Index2].SizeInLBA) -
                     1;
      if (NewEndingLBA >= StartingLBA && UNPACK_UINT32 (Mbr->Partition[Index2].StartingLBA) <= EndingLBA) {
        //
        // This region overlaps with the Index1'th region
        //
        return FALSE;
      }
    }
  }
  //
  // Non of the regions overlapped so MBR is O.K.
  //
  return MbrValid;
}


/**
  This function finds Mbr partitions. Main algorithm
  is ported from DXE partition driver.

  @param[in]  PartBlockDev   Parition block device pointer

  @retval EFI_SUCCESS        New partitions are detected and logical block devices
                             are  added to block device array
  @retval EFI_NOT_FOUND      No New partitions are added
  @retval EFI_DEVICE_ERROR   Error occured for device access

**/
EFI_STATUS
FindMbrPartitions (
  IN  PART_BLOCK_DEVICE   *PartBlockDev
  )
{
  EFI_STATUS             Status;
  MASTER_BOOT_RECORD     *Mbr;
  UINTN                  Index;
  LOGICAL_BLOCK_DEVICE  *BlockDev;
  UINTN                  ParentBlockDevNo;
  DEVICE_BLOCK_INFO     *DevBlockInfo;

  DevBlockInfo = &PartBlockDev->BlockInfo;
  ParentBlockDevNo = PartBlockDev->HarewareDevice;
  Mbr    = (MASTER_BOOT_RECORD *) PartBlockDev->BlockData;
  Status = MediaReadBlocks (
             ParentBlockDevNo,
             0,
             DevBlockInfo->BlockSize,
             Mbr
             );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  Status = EFI_NOT_FOUND;
  if (!PartitionValidMbr (Mbr, DevBlockInfo->BlockNum - 1)) {
    return Status;
  }

  //
  // We have a valid mbr - add each partition
  //
  for (Index = 0; Index < MAX_MBR_PARTITIONS; Index++) {
    if (Mbr->Partition[Index].OSIndicator == 0x00 || UNPACK_INT32 (Mbr->Partition[Index].SizeInLBA) == 0) {
      //
      // Don't use null MBR entries
      //
      continue;
    }
    //
    // Register this partition
    //
    if (PartBlockDev->BlockDeviceCount < PART_MAX_BLOCK_DEVICE) {
      Status                      = EFI_SUCCESS;
      BlockDev                    = & (PartBlockDev->BlockDevice[PartBlockDev->BlockDeviceCount]);
      BlockDev->StartBlock        = UNPACK_UINT32 (Mbr->Partition[Index].StartingLBA);
      BlockDev->LastBlock         = BlockDev->StartBlock + UNPACK_INT32 (Mbr->Partition[Index].SizeInLBA) - 1;
      PartBlockDev->BlockDeviceCount++;
    }
  }

  if (!EFI_ERROR (Status)) {
    PartBlockDev->PartitionType = EnumPartTypeMbr;
  }

  PartBlockDev->PartitionChecked = TRUE;

  return Status;
}


/**
  Test to see if the GPT Entry Array CRC is valid.

  @param  GptEntries        GPT Entry Array Buffer
  @param  Gpt               GPT Header info
  @param  ReadSize          Size of the GPT Array Buffer in bytes
  @param  DevBlockInfo      Device block info
  @param  ParentBlockDevNo  Hardware partition of the device to read
                            from

  @retval TRUE              GPT Entry Array has a valid CRC
  @retval FALSE             GPT Entry Array does not have
                            a valid CRC

**/
BOOLEAN
PartitionValidGptEntryArray (
  IN OUT  EFI_PARTITION_ENTRY         *GptEntries,
  IN      EFI_PARTITION_TABLE_HEADER  *Gpt,
  IN      UINTN                        ReadSize,
  IN      DEVICE_BLOCK_INFO           *DevBlockInfo,
  IN      UINTN                        ParentBlockDevNo
  )
{
  UINT32 CrcOut;
  EFI_STATUS Status;
  EFI_LBA GptLba;

  GptLba = Gpt->PartitionEntryLBA;

  Status = MediaReadBlocks (ParentBlockDevNo, GptLba, ReadSize, GptEntries);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = CalculateCrc32WithType ((UINT8 *)GptEntries, ReadSize, Crc32TypeDefault, &CrcOut);
  if (EFI_ERROR (Status) || (CrcOut != Gpt->PartitionEntryArrayCRC32)) {
    return FALSE;
  }

  return TRUE;
}


/**
  Test to see if the GPT Header CRC is valid.

  @param  Gpt               GPT Header info
  @param  DevBlockInfo      Device block info
  @param  ParentBlockDevNo  Hardware partition of the device to read
                            from
  @param  Primary           Inidicator to check primary or secondary
                            GPT Header

  @retval TRUE              GPT Header has a valid CRC
  @retval FALSE             GPT Header does not have
                            a valid CRC

**/
BOOLEAN
PartitionValidGpt (
  IN OUT  EFI_PARTITION_TABLE_HEADER  *Gpt,
  IN      DEVICE_BLOCK_INFO           *DevBlockInfo,
  IN      UINTN                        ParentBlockDevNo,
  IN      BOOLEAN                      Primary
  )
{
  UINT32 CrcOut;
  EFI_STATUS Status;
  EFI_PARTITION_TABLE_HEADER GptBuff;

  Status = MediaReadBlocks (
             ParentBlockDevNo,
             (Primary) ? 1 : DevBlockInfo->BlockNum - 1,
             DevBlockInfo->BlockSize,
             Gpt
             );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  CopyMem (&GptBuff,  Gpt, sizeof (EFI_PARTITION_TABLE_HEADER));
  GptBuff.Header.CRC32 = 0;
  Status = CalculateCrc32WithType ((UINT8 *) (&GptBuff), sizeof (EFI_PARTITION_TABLE_HEADER), Crc32TypeDefault, &CrcOut);
  if (EFI_ERROR (Status) || (CrcOut != Gpt->Header.CRC32)) {
    return FALSE;
  }

  return TRUE;
}


/**
  This function finds Gpt partitions. Main algorithm
  is ported from DXE partition driver.

  @param[in]  PartBlockDev   Parition block device pointer

  @retval EFI_SUCCESS        New partitions are detected and logical block devices
                             are  added to block device array
  @retval EFI_NOT_FOUND      No New partitions are added
  @retval EFI_DEVICE_ERROR   Error occured for device access
  @retval EFI_CRC_ERROR      Error occured with primary and secondary GPT CRC
  @retval EFI_UNSUPPORTED    The number of GPT entries exceeds the maximum number allowed

**/
EFI_STATUS
FindGptPartitions (
  IN  PART_BLOCK_DEVICE   *PartBlockDev
  )
{
  EFI_STATUS                   Status;
  UINTN                        Index;
  UINTN                        ReadSize;
  UINTN                        GptPartCnt;
  UINTN                        ParentBlockDevNo;
  LOGICAL_BLOCK_DEVICE        *BlockDev;
  MASTER_BOOT_RECORD          *Mbr;
  UINTN                        ProtectMbrStartLba;
  UINTN                        ProtectMbrLbaSize;
  EFI_PARTITION_TABLE_HEADER  *Gpt;
  UINT64                       MaxBlkNum;
  BOOLEAN                      IsGptValid;
  BOOLEAN                      SecondaryHeader;
  EFI_PARTITION_ENTRY         *GptEntries;
  DEVICE_BLOCK_INFO           *DevBlockInfo;

  DevBlockInfo = &PartBlockDev->BlockInfo;
  ParentBlockDevNo = PartBlockDev->HarewareDevice;
  SecondaryHeader = FALSE;
  GptEntries = NULL;

  if (DevBlockInfo->BlockSize == 0 || DevBlockInfo->BlockNum == 0) {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  MaxBlkNum     = DevBlockInfo->BlockNum;

  Mbr    = (MASTER_BOOT_RECORD *) PartBlockDev->BlockData;
  Status = MediaReadBlocks (
             ParentBlockDevNo,
             0,
             DevBlockInfo->BlockSize,
             Mbr
             );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  if (!PartitionValidMbr (Mbr, DevBlockInfo->BlockNum - 1)) {
    DEBUG ((DEBUG_WARN, "Protective MBR is not valid format!\n"));
  } else {
    ProtectMbrStartLba = * ((UINT32 *) (Mbr->Partition[0].StartingLBA));
    ProtectMbrLbaSize  = * ((UINT32 *) (Mbr->Partition[0].SizeInLBA));

    if ((ProtectMbrStartLba != 1) || (ProtectMbrLbaSize != (MaxBlkNum - 1))) {
      DEBUG ((DEBUG_WARN, "Protective MBR does not protect the whole drive!\n"));
    }
  }

  Gpt = (EFI_PARTITION_TABLE_HEADER *) PartBlockDev->BlockData;

  IsGptValid = PartitionValidGpt (Gpt, DevBlockInfo, ParentBlockDevNo, TRUE);
  if (!IsGptValid) {
    SecondaryHeader = TRUE;
    DEBUG ((DEBUG_WARN, "Primary GPT Header CalculateCrc32 Error!\n"));
    IsGptValid = PartitionValidGpt (Gpt, DevBlockInfo, ParentBlockDevNo, FALSE);
    if (!IsGptValid) {
      DEBUG ((DEBUG_WARN, "Secondary GPT Header CalculateCrc32 Error!\n"));
      Status = EFI_CRC_ERROR;
      goto Done;
    }
  }

  ReadSize = Gpt->NumberOfPartitionEntries * Gpt->SizeOfPartitionEntry;
  ReadSize = (ReadSize % DevBlockInfo->BlockSize) == 0 ? ReadSize : DevBlockInfo->BlockSize * ((
               ReadSize / DevBlockInfo->BlockSize) + 1);
  GptEntries = (EFI_PARTITION_ENTRY *) AllocatePool (ReadSize);
  if (GptEntries == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  IsGptValid = PartitionValidGptEntryArray (GptEntries, Gpt, ReadSize, DevBlockInfo, ParentBlockDevNo);
  if (!IsGptValid) {
    DEBUG ((DEBUG_ERROR, "GPT Array CalculateCrc32 Error!\n"));
    if (SecondaryHeader) {
      DEBUG ((DEBUG_ERROR, "Primary Header and Secondary GPT Array data is corrupt!\n"));
      Status = EFI_CRC_ERROR;
      goto Done;
    }
    IsGptValid = PartitionValidGpt (Gpt, DevBlockInfo, ParentBlockDevNo, FALSE);
    if (!IsGptValid) {
      DEBUG ((DEBUG_ERROR, "Secondary GPT Header CalculateCrc32 Error!\n"));
      Status = EFI_CRC_ERROR;
      goto Done;
    }
    IsGptValid = PartitionValidGptEntryArray (GptEntries, Gpt, ReadSize, DevBlockInfo, ParentBlockDevNo);
    if (!IsGptValid) {
      DEBUG ((DEBUG_ERROR, "Secondary GPT Array CalculateCrc32 Error!\n"));
      Status = EFI_CRC_ERROR;
      goto Done;
    }
  }

  GptPartCnt = Gpt->NumberOfPartitionEntries;
  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < GptPartCnt; Index++) {
    if (CompareGuid (&GptEntries[Index].PartitionTypeGUID, &gEfiPartTypeUnusedGuid)) {
      continue;
    }
    if (PartBlockDev->BlockDeviceCount < PART_MAX_BLOCK_DEVICE) {
      Status  = EFI_SUCCESS;
      BlockDev                    = & (PartBlockDev->BlockDevice[PartBlockDev->BlockDeviceCount]);
      BlockDev->StartBlock        = GptEntries[Index].StartingLBA;
      BlockDev->LastBlock         = GptEntries[Index].EndingLBA;
      DEBUG ((DEBUG_INFO, "Part %02d: %12s ", PartBlockDev->BlockDeviceCount, GptEntries[Index].PartitionName));
      DEBUG ((DEBUG_INFO, "0x%08x--0x%08x, LBA count: 0x%x\n", (UINT32)BlockDev->StartBlock, \
              (UINT32)BlockDev->LastBlock, (UINT32) (BlockDev->LastBlock - BlockDev->StartBlock + 1)));

      PartBlockDev->BlockDeviceCount++;
    }
  }

Done:
  if (GptEntries != NULL) {
    FreePool (GptEntries);
  }

  if (!EFI_ERROR (Status)) {
    PartBlockDev->PartitionType = EnumPartTypeGpt;
  }

  PartBlockDev->PartitionChecked = TRUE;

  return Status;
}

/**
Get logical partition information.

This function will fill in logical partition information.

  @param[in]     SwPart           Software partition index.
  @param[in]     PartHandle       Partition handle.
  @param[in,out] LogicBlkDev      Pointer of the logic block device LOGICAL_BLOCK_DEVICE
                                  structure to receive block start and end information.
  @retval EFI_SUCCESS             The logic partition information was filled successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameters.

**/
EFI_STATUS
EFIAPI
GetLogicalPartitionInfo (
  IN     UINT32                 SwPart,
  IN     EFI_HANDLE             PartHandle,
  IN OUT LOGICAL_BLOCK_DEVICE  *LogicBlkDev
  )
{
  PART_BLOCK_DEVICE   *PartBlockDev;

  // Validate parameters
  PartBlockDev = (PART_BLOCK_DEVICE *)PartHandle;
  if ( (LogicBlkDev == NULL) || (PartBlockDev == NULL) || \
       (PartBlockDev->Signature != PART_INFO_SIGNATURE) ) {
    return EFI_INVALID_PARAMETER;
  }

  if ((SwPart >= PartBlockDev->BlockDeviceCount) || (SwPart >= PART_MAX_BLOCK_DEVICE)) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (LogicBlkDev, &PartBlockDev->BlockDevice[SwPart], sizeof (LOGICAL_BLOCK_DEVICE));

  return EFI_SUCCESS;
}

/**
Find partitions from OS boot medium

This function will check hardware partition for MBR, GPT or NONE parition.

  @param[in]   HwDevice      The hardware device index.
  @param[out]  PartHandle    The pointer to return parition handle

  @retval EFI_SUCCESS        The partition was found successfully.
  @retval EFI_NOT_FOUND      No partition was found.
  @retval EFI_DEVICE_ERROR   Error occured for device access.
  @retval EFI_INVALID_PARAMETER   Invalid parameters.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.

**/
EFI_STATUS
EFIAPI
FindPartitions (
  IN   UINT32               HwDevice,
  OUT  EFI_HANDLE          *PartHandle
  )
{
  EFI_STATUS                   Status;
  DEVICE_BLOCK_INFO            DevBlockInfo;
  UINT8                       *Buffer;
  LOGICAL_BLOCK_DEVICE        *BlockDev;
  EFI_PARTITION_TABLE_HEADER  *Gpt;
  PART_BLOCK_DEVICE           *PartBlockDev;
  OS_BOOT_MEDIUM_TYPE          CurrentMediaType;
  BOOLEAN                      IsGptValid;

  if (PartHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = MediaGetMediaInfo (HwDevice, &DevBlockInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  PartBlockDev = (PART_BLOCK_DEVICE *) AllocateZeroPool (sizeof (PART_BLOCK_DEVICE));
  if (PartBlockDev == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PartBlockDev->Signature      = PART_INFO_SIGNATURE;
  PartBlockDev->HarewareDevice = HwDevice;

  CopyMem (&PartBlockDev->BlockInfo, &DevBlockInfo, sizeof (DEVICE_BLOCK_INFO));

  CurrentMediaType = MediaGetInterfaceType();

  if (CurrentMediaType == OsBootDeviceMemory || CurrentMediaType == OsBootDeviceSpi) {
    // It is memory mapped SPI block device.
    Status = FindSpiPartitions (PartBlockDev);
    if (!EFI_ERROR (Status)) {
      *PartHandle = (EFI_HANDLE)PartBlockDev;
    }
    return Status;
  }

  // Check primary GPT Partition first
  Buffer = (UINT8 *) PartBlockDev->BlockData;
  Gpt    = (EFI_PARTITION_TABLE_HEADER *)Buffer;
  IsGptValid = PartitionValidGpt (Gpt, &DevBlockInfo, PartBlockDev->HarewareDevice, TRUE);
  if (!IsGptValid) {
    // Check backup GPT as well
    IsGptValid = PartitionValidGpt (Gpt, &DevBlockInfo, PartBlockDev->HarewareDevice, FALSE);
  }
  if (IsGptValid && ((UINT32)Gpt->Header.Signature == 0x20494645)) {
    Status = FindGptPartitions (PartBlockDev);
  } else {
    Status = FindMbrPartitions (PartBlockDev);
  }

  // Check result
  if (EFI_ERROR (Status)) {
    // Could not find any partition, so assume no partitions
    BlockDev              = & (PartBlockDev->BlockDevice[0]);
    BlockDev->StartBlock  = 0;
    BlockDev->LastBlock   = DevBlockInfo.BlockNum - 1;
    PartBlockDev->PartitionType    = EnumPartTypeUnknown;
    PartBlockDev->PartitionChecked = TRUE;
    PartBlockDev->BlockDeviceCount = 1;
    Status = EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "Partition type: %a  (%d logical partitions)\n", \
          GetPartitionTypeName (PartBlockDev->PartitionType), \
          PartBlockDev->BlockDeviceCount));

  *PartHandle = (EFI_HANDLE)PartBlockDev;

  return Status;
}

/**
  Clean-up allocated memory/etc. used for partitions

  @param[in]  PartHandle      The partition handle to clean-up

  @retval                     none

**/
VOID
EFIAPI
ClosePartitions (
  IN   EFI_HANDLE             PartHandle
  )
{
  PART_BLOCK_DEVICE           *PartBlockDev;

  PartBlockDev = NULL;
  if (PartHandle != NULL) {
    PartBlockDev = (PART_BLOCK_DEVICE *)PartHandle;
    ASSERT (PartBlockDev->Signature == PART_INFO_SIGNATURE);
  }

  if ((PartBlockDev != NULL) && (PartBlockDev->Signature == PART_INFO_SIGNATURE)) {
    FreePool (PartBlockDev);
  }
}

/**
  Get detected partition type

  @param[in]  PartHandle      The partition handle to clean-up

  @retval                     Partition type enum

**/
PARTITION_TYPE
EFIAPI
GetPartitionType (
  IN   EFI_HANDLE             PartHandle
  )
{
  PART_BLOCK_DEVICE           *PartBlockDev;
  PARTITION_TYPE               PartType;

  PartType = EnumPartTypeMax;
  PartBlockDev = (PART_BLOCK_DEVICE *)PartHandle;
  if ((PartBlockDev != NULL) && (PartBlockDev->Signature == PART_INFO_SIGNATURE)) {
    PartType = PartBlockDev->PartitionType;
  }

  return PartType;
}

/**
  Get HW part no of the detected partition

  @param[in]  PartHandle              The partition handle to clean-up
  @param[in]  HwPartNo                HW part no.

  @retval     EFI_SUCCESS             Found SW part no.
  @retval     EFI_INVALID_PARAMETER   Invalid PartHandle

**/
EFI_STATUS
EFIAPI
GetPartitionCurrentPartNo (
  IN  EFI_HANDLE            PartHandle,
  OUT UINT32               *HwPartNo
  )
{
  PART_BLOCK_DEVICE          *PartBlockDev;

  PartBlockDev = (PART_BLOCK_DEVICE *)PartHandle;
  if ((PartBlockDev == NULL) || (PartBlockDev->Signature != PART_INFO_SIGNATURE)) {
    return EFI_INVALID_PARAMETER;
  }

  *HwPartNo = PartBlockDev->HarewareDevice;
  return EFI_SUCCESS;
}
