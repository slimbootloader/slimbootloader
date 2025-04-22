/** @file
  This file provides AHCI SATA device block access interfaces.

  Copyright (c) 2017 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "AhciDevice.h"

/**
  Calculate max transfer sector count.

  The function is designed to calculate disk max transfer sectpr count.

  @param[in]  AtaDevice    AHCI SATA device information pointer.

  @retval     AHCI_MAX_48_TRANSFER_SECTOR    Support 48bit LBA
  @retval     AHCI_MAX_28_TRANSFER_SECTOR    Support 28bit LBA

**/
UINT32
GetMaxTransferSector (
  IN EFI_ATA_DEVICE_INFO *AtaDevice
  )
{
  UINT32  BlkCnt;

  if (AtaDevice->DeviceFeature & DEVICE_LBA_48_SUPPORT) {
    if (FeaturePcdGet(PcdDmaProtectionEnabled)) {
      // When DMA protection is enabled, only tranfer less than DMA buffer size
      // Use half for safe.  Around 1MB will be used for CmdTable.
      BlkCnt = (PcdGet32 (PcdDmaBufferSize) >> 1) / AtaDevice->BlockSize;
      if (BlkCnt == 0) {
        BlkCnt = 1;
      } else if (BlkCnt > AHCI_MAX_48_TRANSFER_SECTOR) {
        BlkCnt = AHCI_MAX_48_TRANSFER_SECTOR;
      }
    } else {
      BlkCnt = AHCI_MAX_48_TRANSFER_SECTOR;
    }
  } else {
    BlkCnt = AHCI_MAX_28_TRANSFER_SECTOR;
  }
  return BlkCnt;
}

/**
  Block read/write to ATA device.

  The function performs read or write operation to the ATA device.

  @param[in]  AtaDevice     ATA device instance.
  @param[in]  Read          Read or write.
  @param[in]  StartLba      The starting logical block address (LBA) to read from
                            on the device.
  @param[in]  SectorCount   The sector count to read or write.
  @param[in, out] MemoryAddr    A pointer to the buffer for the data.
                            The caller is responsible for the ownership of the
                            buffer.

  @retval EFI_DEVICE_ERROR    The DMA data transfer abort with error occurs.
  @retval EFI_TIMEOUT         The operation is time out.
  @retval EFI_UNSUPPORTED     The device is not ready for transfer.
  @retval EFI_SUCCESS         The DMA data transfer executes successfully.

**/
EFI_STATUS
AhciAtaDeviceReadWrite (
  IN     EFI_ATA_DEVICE_INFO *AtaDevice,
  IN     BOOLEAN              Read,
  IN     EFI_LBA              StartLba,
  IN     UINT32               SectorCount,
  IN OUT VOID                *MemoryAddr
  )
{
  EFI_AHCI_CONTROLLER   *AhciController;
  EFI_ATA_COMMAND_BLOCK  AtaCmdBlk;

  AhciController     = AtaDevice->Controller;

  //
  // Prepare for ATA command block.
  //
  ZeroMem (&AtaCmdBlk, sizeof (EFI_ATA_COMMAND_BLOCK));

  if (Read) {
    AtaCmdBlk.AtaCommand = (AtaDevice->DeviceFeature & DEVICE_LBA_48_SUPPORT) ? ATA_CMD_READ_DMA_EXT : ATA_CMD_READ_DMA;
  } else {
    AtaCmdBlk.AtaCommand = (AtaDevice->DeviceFeature & DEVICE_LBA_48_SUPPORT) ? ATA_CMD_WRITE_DMA_EXT : ATA_CMD_WRITE_DMA;
  }
  AtaCmdBlk.AtaSectorNumber = (UINT8) StartLba;
  AtaCmdBlk.AtaCylinderLow = (UINT8) RShiftU64 (StartLba, 8);
  AtaCmdBlk.AtaCylinderHigh = (UINT8) RShiftU64 (StartLba, 16);
  AtaCmdBlk.AtaDeviceHead = (UINT8) (BIT7 | BIT6 | BIT5 | \
                                     (AtaDevice->PortMultiplier == 0xFFFF ? 0 : (AtaDevice->PortMultiplier << 4)));
  AtaCmdBlk.AtaSectorCount = (UINT8) SectorCount;

  if (AtaDevice->DeviceFeature & DEVICE_LBA_48_SUPPORT) {
    AtaCmdBlk.AtaSectorNumberExp = (UINT8) RShiftU64 (StartLba, 24);
    AtaCmdBlk.AtaCylinderLowExp = (UINT8) RShiftU64 (StartLba, 32);
    AtaCmdBlk.AtaCylinderHighExp = (UINT8) RShiftU64 (StartLba, 40);
    AtaCmdBlk.AtaSectorCountExp = (UINT8) (SectorCount >> 8);
  } else {
    AtaCmdBlk.AtaDeviceHead = (UINT8) (AtaCmdBlk.AtaDeviceHead | RShiftU64 (StartLba, 24));
  }

  return AhciDmaTransfer (
           AhciController,
           &AhciController->AhciRegisters,
           (UINT8)AtaDevice->Port,
           (UINT8)AtaDevice->PortMultiplier,
           NULL,
           0,
           Read,
           &AtaCmdBlk,
           NULL,
           MemoryAddr,
           SectorCount * AtaDevice->BlockSize,
           DMA_WAIT_TIMEOUT_MS * 1000 * 10
           );
}

/**
  Block read/write to ATA device.

  The function performs read or write operation to the ATA device.

  @param[in]  AtaDevice     ATA device instance.
  @param[in]  Read          Read or write.
  @param[in]  Lba           The starting logical block address (LBA) to read from
                            on the device.
  @param[in]  BufferSize    The size of the Buffer in bytes. This number must be
                            a multiple of the intrinsic block size of the device.
  @param[out] Buffer        A pointer to the destination buffer for the data.
                            The caller is responsible for the ownership of the
                            buffer.

  @retval EFI_INVALID_PARAMETER  Inpurt parameters are not valid.
  @retval EFI_DEVICE_ERROR       The DMA data transfer abort with error occurs.
  @retval EFI_TIMEOUT            The operation is time out.
  @retval EFI_UNSUPPORTED        The device is not ready for transfer.
  @retval EFI_SUCCESS            The DMA data transfer executes successfully.

**/
EFI_STATUS
AhciReadWriteBlock (
  IN     EFI_ATA_DEVICE_INFO  *AtaDevice,
  IN     BOOLEAN               Read,
  IN     EFI_LBA               Lba,
  IN     UINTN                 BufferSize,
  OUT    VOID                 *Buffer
  )
{
  EFI_STATUS                     Status;
  EFI_LBA                        LbaIndex;
  UINT8                          *ReadBuf;
  UINTN                          BlockSize;
  UINTN                          NumberOfBlocks;
  UINT32                         IoAlign;
  UINT32                         MaxTransferSector;
  UINT32                         RemainSectorCount;

  if (AtaDevice == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ReadBuf               = Buffer;
  LbaIndex              = Lba;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  //
  //  Get the intrinsic block size
  //
  BlockSize       = AtaDevice->BlockSize;
  NumberOfBlocks  = BufferSize / BlockSize;


  if (BufferSize % BlockSize != 0) {
    return EFI_BAD_BUFFER_SIZE;
  }

  if (Lba >= AtaDevice->TotalBlockNumber) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Lba + NumberOfBlocks - 1) >= AtaDevice->TotalBlockNumber) {
    return EFI_INVALID_PARAMETER;
  }

  IoAlign = 2;
  if ((IoAlign > 1) && (((UINTN) Buffer & (IoAlign - 1)) != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  MaxTransferSector = GetMaxTransferSector (AtaDevice);
  RemainSectorCount = (UINT32)NumberOfBlocks;
  while (RemainSectorCount != 0) {
    Status = AhciAtaDeviceReadWrite (
               AtaDevice,
               Read,
               LbaIndex,
               RemainSectorCount >= MaxTransferSector ? MaxTransferSector : RemainSectorCount,
               ReadBuf
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "AhciDeviceRead Status = %r\n", Status));
      return EFI_DEVICE_ERROR;
    }

    if (RemainSectorCount >= MaxTransferSector)  {
      ReadBuf           += ((UINTN)AtaDevice->BlockSize * MaxTransferSector);
      LbaIndex          += MaxTransferSector;
      RemainSectorCount -= MaxTransferSector;
    } else {
      RemainSectorCount = 0;
    }
  }

  return EFI_SUCCESS;
}

