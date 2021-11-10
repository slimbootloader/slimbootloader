/** @file
  Routines supporting partition discovery

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PartitionLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLib.h>
#include <Guid/FlashMapInfoGuid.h>

/**
  This function adds a Flash region as partition.

  @param[in,out]  PartBlockDev   Parition block device pointer
  @param[in]      RegionBase     Flash region base address
  @param[in]      RegionSize     Flash region size

**/
VOID
AddSpiPartition (
  IN OUT PART_BLOCK_DEVICE     *PartBlockDev,
  IN     UINT32                RegionBase,
  IN     UINT32                RegionSize
  )
{
  UINT32                       BlockNum;
  LOGICAL_BLOCK_DEVICE         *BlockDev;
  UINT32                       BlkSize;

  BlockDev = &PartBlockDev->BlockDevice[PartBlockDev->BlockDeviceCount];
  DEBUG ((DEBUG_INFO, "Part %02d: ", PartBlockDev->BlockDeviceCount));
  if (RegionSize == 0) {
    // Use 0xFFFFFFFF in StartBlock to indicate it is not a valid software partition.
    BlockDev->StartBlock = 0xFFFFFFFF;
    BlockDev->LastBlock  = 0;
    DEBUG ((DEBUG_INFO, "Partition size is 0, no data in this partition\n"));
  } else {
    BlkSize  = PartBlockDev->BlockInfo.BlockSize;
    BlockNum = ((RegionSize % BlkSize) == 0) ? RegionSize / BlkSize : RegionSize / BlkSize + 1;
    BlockDev->StartBlock = RegionBase / BlkSize;
    BlockDev->LastBlock  = BlockDev->StartBlock + BlockNum;
    DEBUG ((DEBUG_INFO, "0x%08x--0x%08x, LBA count: 0x%x\n", (UINT32)BlockDev->StartBlock, \
            (UINT32)BlockDev->LastBlock, BlockNum));
  }
  PartBlockDev->BlockDeviceCount++;
}


/**
  This function finds SPI partitions from BIOS region.

  This function gets flash IAS regoins from flash map for block read/write.
  It will takes every region as a separate software partition.

  @param[in, out]  PartBlockDev   Parition block device pointer

  @retval EFI_SUCCESS        New partitions are detected and logical block devices
                             are  added to block device array
  @retval EFI_NOT_FOUND      No New partitions are added
  @retval EFI_INVALID_PARAMETER   The SPI flash block number or size is 0.
**/
EFI_STATUS
FindSpiPartitions (
  IN OUT PART_BLOCK_DEVICE  *PartBlockDev
  )
{
  EFI_STATUS                Status;
  UINT32                    IasRegionBase;
  UINT32                    IasRegionSize;
  DEVICE_BLOCK_INFO         *DevBlockInfo;

  DevBlockInfo = &PartBlockDev->BlockInfo;
  if (DevBlockInfo->BlockSize == 0 || DevBlockInfo->BlockNum == 0) {
    return EFI_INVALID_PARAMETER;
  }

  PartBlockDev->PartitionChecked = FALSE;
  PartBlockDev->BlockDeviceCount = 0;

  // Get first IAS region as partition
  Status = GetComponentInfo (FLASH_MAP_SIG_SPI_IAS1, &IasRegionBase, &IasRegionSize);
  if (!EFI_ERROR(Status)) {
    AddSpiPartition (PartBlockDev, IasRegionBase, IasRegionSize);
  } else {
    AddSpiPartition (PartBlockDev, IasRegionBase, 0);
  }

  // Check if the second IAS region exists.
  Status = GetComponentInfo (FLASH_MAP_SIG_SPI_IAS2, &IasRegionBase, &IasRegionSize);
  if (!EFI_ERROR(Status)) {
    AddSpiPartition (PartBlockDev, IasRegionBase, IasRegionSize);
  }

  DEBUG ((DEBUG_INFO, "SPI BIOS region: (%d logical partitions)\n", \
            PartBlockDev->BlockDeviceCount));
  PartBlockDev->PartitionChecked = TRUE;

  return EFI_SUCCESS;
}

