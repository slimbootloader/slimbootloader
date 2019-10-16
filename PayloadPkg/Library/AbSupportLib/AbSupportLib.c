/** @file
  Payload implements one instance of Platform Hook Library.

  Copyright (c) 2015 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include   "AbSupport.h"


/**
  Check if slot is bootable.

  @param[in]  SlotData        Pointer to slot data

  @retval  TRUE               This slot is bootable
  @retval  FALSE              This slot is not bootable
**/
BOOLEAN
SlotBootable (
  IN  AB_SLOT_DATA           *SlotData
  )
{
  if ((SlotData->Priority > 0) && ((SlotData->SuccessBoot > 0) || (SlotData->TriesRemaining > 0))) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check which slot to boot.

  @param[in]  AbBootInfo     Pointer to AB boot info data

  @retval   0                Select first slot to boot
  @retval   1                Select second slot to boot
  @retval  -1                Magic doesn't match
  @retval  -2                SlotData CRC error
  @retval  -3                Both SlotA and SlotB are not bootable
**/
INT32
ParseBootSlot (
  IN AB_BOOT_INFO            *AbBootInfo
  )
{
  EFI_STATUS                 Status;
  UINT32                     CrcOut;
  UINTN                      DataSize;

  if (AbBootInfo->Magic != AB_MAGIC_SIGNATURE) {
    return -1;
  }

  DataSize = sizeof (AB_BOOT_INFO) - sizeof (UINT32);
  Status = CalculateCrc32WithType ((UINT8 *)AbBootInfo, DataSize, Crc32TypeDefault, &CrcOut);
  if (EFI_ERROR (Status) || (SwapBytes32 (CrcOut) != AbBootInfo->Crc32)) {
    DEBUG ((DEBUG_INFO, "BootSlot CRC error: 0x%x !=0x%x\n", SwapBytes32 (CrcOut), AbBootInfo->Crc32));
    return -2;
  }

  if (SlotBootable (&AbBootInfo->SlotData[0])) {
    if (SlotBootable (&AbBootInfo->SlotData[1])) {
      if (AbBootInfo->SlotData[1].Priority > AbBootInfo->SlotData[0].Priority) {
        return 1;
      }
    }
    return 0;
  } else if (SlotBootable (&AbBootInfo->SlotData[1])) {
    return 1;
  }

  return -3;
}


/**
  Load AB boot information from misc partition

  This function will read AB boot information from boot device.

  @param[in]  BootOption      Image information where to load image.
  @param[in]  HwPartHandle    The hardware partition handle
  @param[out] AbBootInfo      AB boot info loaded from misc partition.

  @retval  RETURN_SUCCESS     Read AB boot information successfully
  @retval  Others             There is error when read boot info
**/
EFI_STATUS
LoadMisc (
  IN  OS_BOOT_OPTION         *BootOption,
  IN  EFI_HANDLE             HwPartHandle,
  OUT AB_BOOT_INFO           *AbBootInfo
  )
{
  EFI_STATUS                 Status;
  MISC_PARTITION_DATA        *MiscPartitionData;
  DEVICE_BLOCK_INFO          BlockInfo;
  VOID                       *Buffer;
  LOGICAL_BLOCK_DEVICE       LogicBlkDev;
  UINTN                      ReadSize;
  UINT32                     BlockSize;
  LBA_IMAGE_LOCATION         *LbaImage;

  LbaImage = &BootOption->Image[LoadImageTypeMisc].LbaImage;
  Status = GetLogicalPartitionInfo (LbaImage->SwPart, HwPartHandle, &LogicBlkDev);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Get logical partition error, Status = %r\n", Status));
    return Status;
  }

  Status = MediaGetMediaInfo (BootOption->HwPart, &BlockInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetMediaInfo Error %r\n", Status));
    return Status;
  }

  //
  // Data is in raw block IO partition
  // Read size should be block aligned in bytes.
  //
  BlockSize = BlockInfo.BlockSize;
  ReadSize  = ((sizeof (MISC_PARTITION_DATA) % BlockSize) == 0) ? \
              sizeof (MISC_PARTITION_DATA) : \
              ((sizeof (MISC_PARTITION_DATA) / BlockSize) + 1) * BlockSize;

  Buffer = (UINT8 *) AllocatePages (EFI_SIZE_TO_PAGES (ReadSize));
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = MediaReadBlocks (
             BootOption->HwPart,
             LogicBlkDev.StartBlock + LbaImage->LbaAddr,
             ReadSize,
             Buffer
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read Mics error, Status = %r\n", Status));
  } else {
    MiscPartitionData = (MISC_PARTITION_DATA *)Buffer;
    CopyMem (AbBootInfo, &MiscPartitionData->AbBootInfo, sizeof (AB_BOOT_INFO));
  }

  //
  // Free temporary memory
  //
  if (Buffer != NULL) {
    FreePages (Buffer, EFI_SIZE_TO_PAGES (ReadSize));
  }

  return Status;
}


/**
  Check which slot to boot.

  @param[in]  BootOption      Image information where to load image.
  @param[in]  HwPartHandle    The hardware partition handle

  @retval  0                  Select first slot to boot
  @retval  1                  Select second slot to boot
**/
INT32
EFIAPI
GetBootSlot (
  IN OS_BOOT_OPTION          *BootOption,
  IN EFI_HANDLE              HwPartHandle
  )
{
  EFI_STATUS                 Status;
  AB_BOOT_INFO               AbBootInfo;
  INT32                      BootSlot;

  BootSlot = 0;

  if ((BootOption->BootFlags & BOOT_FLAGS_MISC) != 0) {
    Status   = LoadMisc (BootOption, HwPartHandle, &AbBootInfo);
    if (!EFI_ERROR (Status)) {
      BootSlot = ParseBootSlot (&AbBootInfo);
      if (BootSlot < 0) {
        DEBUG ((DEBUG_ERROR, "ERROR: boot slot error (%d)\n", BootSlot));
        BootSlot = 0;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "LoadMisc Status = %r\n", Status));
    }
  } else if ((BootOption->BootFlags & BOOT_FLAGS_MENDER) != 0) {
    //
    // TODO: Need to support env file parsing to determine which
    //       boot slot we should really be using for Mender boot
    //       support. For now, just return primary (e.g. 0)
    //
    BootSlot = 0;
  }

  return BootSlot;
}

