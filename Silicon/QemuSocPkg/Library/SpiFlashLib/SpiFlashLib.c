/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/SpiFlashLib.h>
#include <IndustryStandard/Pci30.h>
#include <SpiCommonLib.h>

#define WRITE_BYTE_CMD           0x10
#define BLOCK_ERASE_CMD          0x20
#define CLEAR_STATUS_CMD         0x50
#define READ_STATUS_CMD          0x70
#define READ_DEVID_CMD           0x90
#define BLOCK_ERASE_CONFIRM_CMD  0xd0
#define READ_ARRAY_CMD           0xff
#define CLEARED_ARRAY_STATUS     0x00

const SPI_FLASH_SERVICE   mSpiFlashService = {
  .Header.Signature = SPI_FLASH_SERVICE_SIGNATURE,
  .Header.Version   = SPI_FLASH_SERVICE_VERSION,
  .SpiInit          = SpiConstructor,
  .SpiRead          = SpiFlashRead,
  .SpiWrite         = SpiFlashWrite,
  .SpiErase         = SpiFlashErase,
  .SpiGetRegion     = SpiGetRegionAddress
};


/**
  Get SPI Instance from library global data..

  @retval SpiInstance       Return SPI instance
**/
SPI_INSTANCE *
GetSpiInstance (
  VOID
)
{
  EFI_STATUS         Status;
  SPI_INSTANCE      *SpiInstance;

  Status = GetLibraryData (PcdGet8(PcdSpiFlashLibId), (VOID **)&SpiInstance);
  if (Status == EFI_NOT_FOUND) {
    SpiInstance  = AllocatePool (sizeof(SPI_INSTANCE));
    ZeroMem (SpiInstance, sizeof(SPI_INSTANCE));
    Status = SetLibraryData (PcdGet8(PcdSpiFlashLibId), SpiInstance, sizeof(SPI_INSTANCE));
  }

  if (EFI_ERROR(Status)) {
    SpiInstance = NULL;
  }

  return SpiInstance;
}

/**
  Get Flash address from LBA and offset.

  @param[in]  Lba      The LBA block number for flash device.
  @param[int] Offset   The offset within the LBA block.

  @retval  The flash address pointer.

**/
STATIC
UINT8*
QemuFlashPtr (
  IN        EFI_LBA                             Lba,
  IN        UINTN                               Offset
  )
{
  return (UINT8 *)(UINTN)PcdGet32 (PcdFlashBaseAddress) + ((UINTN)Lba * PcdGet32 (PcdFlashBlockSize)) + Offset;
}

/**
  Determines if the QEMU flash memory device is present.

  @retval FALSE   The QEMU flash device is not present.
  @retval TRUE    The QEMU flash device is present.

**/
STATIC
BOOLEAN
QemuFlashDetected (
  VOID
  )
{
  BOOLEAN  FlashDetected;
  volatile UINT8  *Ptr;

  UINTN Offset;
  UINT8 OriginalUint8;
  UINT8 ProbeUint8;

  FlashDetected = FALSE;
  for (Offset = 0; Offset < PcdGet32 (PcdFlashBlockSize); Offset++) {
    Ptr = QemuFlashPtr (0, Offset);
    ProbeUint8 = *Ptr;
    if (ProbeUint8 != CLEAR_STATUS_CMD &&
        ProbeUint8 != READ_STATUS_CMD &&
        ProbeUint8 != CLEARED_ARRAY_STATUS) {
      break;
    }
  }

  if (Offset >= PcdGet32 (PcdFlashBlockSize)) {
    DEBUG ((DEBUG_INFO, "QEMU Flash: Failed to find probe location\n"));
    return FALSE;
  }

  DEBUG ((DEBUG_INFO, "QEMU Flash: Attempting flash detection at %p\n", Ptr));

  OriginalUint8 = *Ptr;
  *Ptr = CLEAR_STATUS_CMD;
  ProbeUint8 = *Ptr;
  if (OriginalUint8 != CLEAR_STATUS_CMD &&
      ProbeUint8 == CLEAR_STATUS_CMD) {
    DEBUG ((DEBUG_INFO, "QemuFlashDetected => FD behaves as RAM\n"));
    *Ptr = OriginalUint8;
  } else {
    *Ptr = READ_STATUS_CMD;
    ProbeUint8 = *Ptr;
    if (ProbeUint8 == OriginalUint8) {
      DEBUG ((DEBUG_INFO, "QemuFlashDetected => FD behaves as ROM\n"));
    } else if (ProbeUint8 == READ_STATUS_CMD) {
      DEBUG ((DEBUG_INFO, "QemuFlashDetected => FD behaves as RAM\n"));
      *Ptr = OriginalUint8;
    } else if (ProbeUint8 == CLEARED_ARRAY_STATUS) {
      DEBUG ((DEBUG_INFO, "QemuFlashDetected => FD behaves as FLASH\n"));
      FlashDetected = TRUE;
      *Ptr = READ_ARRAY_CMD;
    }
  }

  DEBUG ((DEBUG_INFO, "QemuFlashDetected => %a\n",
                      FlashDetected ? "Yes" : "No"));
  return FlashDetected;
}

/**
  Initialize an SPI instance.

  @retval    EFI_SUCCESS          The instance was properly initialized
  @retval    EFI_UNSUPPORTED      The flash is not supported by this module
**/
EFI_STATUS
EFIAPI
SpiConstructor (
  VOID
  )
{
  SPI_INSTANCE   *SpiInstance;
  EFI_STATUS      Status;

  //
  // Initialize the SPI instance
  //
  SpiInstance = GetSpiInstance();
  ASSERT (SpiInstance != NULL);
  if (SpiInstance->Signature == PCH_SPI_PRIVATE_DATA_SIGNATURE) {
    Status = RegisterService ((VOID *)&mSpiFlashService);
    return Status;
  }

  if (!QemuFlashDetected ()) {
    DEBUG ((
            DEBUG_ERROR,
            "QEMU CFI Flash device is not detected ! \n"
            "Please use '-pflash bios.bin' instead of '-bios bios.bin' in QEMU command line !\n"
          ));
    Status = EFI_UNSUPPORTED;
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  DEBUG ((DEBUG_INFO, "SpiInstance = %08X\n", SpiInstance));

  SpiInstance->Signature  = PCH_SPI_PRIVATE_DATA_SIGNATURE;
  SpiInstance->Handle     = NULL;
  SpiInstance->TotalFlashSize = PcdGet32 (PcdFlashSize);
  SpiInstance->BlockSize      = PcdGet32 (PcdFlashBlockSize);
  SpiInstance->StoreBase      = PcdGet32 (PcdFlashBaseAddress);

  Status = RegisterService ((VOID *)&mSpiFlashService);

  return Status;
}


/**
  Get the SPI region base and size, based on the enum type

  @param[in] FlashRegionType      The Flash Region type for for the base address which is listed in the Descriptor.
  @param[out] BaseAddress         The Flash Linear Address for the Region 'n' Base
  @param[out] RegionSize          The size for the Region 'n'

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid region type given
  @retval EFI_DEVICE_ERROR        The region is not used
**/
EFI_STATUS
EFIAPI
SpiGetRegionAddress (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  OUT    UINT32             *BaseAddress, OPTIONAL
  OUT    UINT32             *RegionSize OPTIONAL
  )
{
  SPI_INSTANCE   *SpiInstance;

  if ((FlashRegionType == FlashRegionAll) || (FlashRegionType == FlashRegionBios)) {
    if (BaseAddress != NULL) {
      *BaseAddress   = 0;
    }
    if (RegionSize  != NULL) {
      SpiInstance = GetSpiInstance();
      if (SpiInstance == NULL) {
        return EFI_DEVICE_ERROR;
      }

      *RegionSize    = SpiInstance->TotalFlashSize;
    }
    return EFI_SUCCESS;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  Read data from the flash part.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.
  @param[out] Buffer              The Pointer to caller-allocated buffer containing the dada received.
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashRead (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  OUT    UINT8              *Buffer
  )
{
  EFI_STATUS      Status;
  SPI_INSTANCE   *SpiInstance;

  if ((FlashRegionType != FlashRegionAll) && (FlashRegionType != FlashRegionBios)) {
    return EFI_INVALID_PARAMETER;
  }

  SpiInstance = GetSpiInstance();
  if (SpiInstance == NULL) {
    return EFI_DEVICE_ERROR;
  }

  Address = Address - SpiInstance->TotalFlashSize - SpiInstance->StoreBase;
  if ((Address >= SpiInstance->TotalFlashSize) ||
      (Address + ByteCount >= SpiInstance->TotalFlashSize)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  CopyMem (Buffer, (VOID *)(UINTN)(Address + SpiInstance->StoreBase), ByteCount);

  return Status;
}


/**
  Write data to the flash part.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.
  @param[in] Buffer               Pointer to caller-allocated buffer containing the data sent during the SPI cycle.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashWrite (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  IN     UINT8              *Buffer
  )
{
  UINTN           Idx;
  EFI_STATUS      Status;
  SPI_INSTANCE   *SpiInstance;
  UINT8           Old;
  volatile UINT8 *Ptr;

  if ((FlashRegionType != FlashRegionAll) && (FlashRegionType != FlashRegionBios)) {
    return EFI_INVALID_PARAMETER;
  }

  SpiInstance = GetSpiInstance();
  if (SpiInstance == NULL) {
    return EFI_DEVICE_ERROR;
  }

  Address = Address - SpiInstance->TotalFlashSize - SpiInstance->StoreBase;
  if ((Address >= SpiInstance->TotalFlashSize) ||
      (Address + ByteCount >= SpiInstance->TotalFlashSize)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  Ptr = (volatile UINT8 *)(UINTN)(Address + SpiInstance->StoreBase);
  for (Idx = 0; Idx < ByteCount; Idx++) {
    if (ByteCount < 0x1000) {
      *Ptr = READ_ARRAY_CMD;
      Old  = *Ptr;
    } else {
      Old  = 0xFF;
    }
    *Ptr = WRITE_BYTE_CMD;
    *Ptr = Buffer[Idx] & Old;
    Ptr++;
  }

  if (ByteCount > 0) {
    *(Ptr - 1) = READ_ARRAY_CMD;
  }

  return Status;
}


/**
  Erase some area on the flash part.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashErase (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount
  )
{
  SPI_INSTANCE   *SpiInstance;
  UINT32          Offset;
  volatile UINT8 *Ptr;

  if ((FlashRegionType != FlashRegionAll) && (FlashRegionType != FlashRegionBios)) {
    return EFI_INVALID_PARAMETER;
  }

  SpiInstance = GetSpiInstance();
  if (SpiInstance == NULL) {
    return EFI_DEVICE_ERROR;
  }

  Address = Address - SpiInstance->TotalFlashSize - SpiInstance->StoreBase;
  if ((Address >= SpiInstance->TotalFlashSize) ||
      (Address + ByteCount >= SpiInstance->TotalFlashSize)) {
    return EFI_INVALID_PARAMETER;
  }

  if (((Address & 0xFFF) > 0) || (((Address + ByteCount) & 0xFFF) > 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr = (volatile UINT8 *)(UINTN)(Address + SpiInstance->StoreBase);

  for (Offset = 0; Offset < ByteCount; Offset += 0x1000) {
    *Ptr = BLOCK_ERASE_CMD;
    *Ptr = BLOCK_ERASE_CONFIRM_CMD;
    Ptr += 0x1000;
  }

  if (ByteCount > 0) {
    *(Ptr - 1) = READ_ARRAY_CMD;
  }

  return EFI_SUCCESS;
}
