/** @file
  This file provides some helper functions via run time service support
  specific to SPI flash device.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/SpiBlockIoLib.h>
#include <Library/BootloaderCommonLib.h>

SPI_FLASH_SERVICE *mSpiService = NULL;

/**
  Initialize a SPI instance.

  @param[in]  SpiPciBase         The PCI base of the SPI flash device.
  @param[in]  DevInitPhase       The initialization phase to prepare the device.

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval EFI_UNSUPPORTED        The call is unsupported.
  @retval Others                 Can't initialize the SPI device.
**/
EFI_STATUS
EFIAPI
InitializeSpi (
  IN  UINTN               SpiPciBase,
  IN  DEVICE_INIT_PHASE   DevInitPhase
  )
{
  EFI_STATUS Status;

  if (DevInitPhase == DevDeinit) {
    // Handle Deinit if required.
    return EFI_SUCCESS;
  }

  mSpiService = (SPI_FLASH_SERVICE *) GetServiceBySignature (SPI_FLASH_SERVICE_SIGNATURE);
  if (mSpiService == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = mSpiService->SpiInit ();

  return Status;
}

/**
  This function reads blocks from the SPI slave device.

  @param[in]  RegionType          The region type of the SPI area to read from.
  @param[in]  Address             The block address in the RegionType to read from on the SPI.
  @param[in]  BufferSize          Size of the Buffer in bytes.
  @param[out] Buffer              Pointer to caller-allocated buffer containing the data received during the SPI cycle.

  @retval EFI_SUCCESS             SPI command completes successfully.
  @retval EFI_DEVICE_ERROR        Device error, the command aborts abnormally.
  @retval EFI_ACCESS_DENIED       Some unrecognized or blocked command encountered in hardware sequencing mode.
  @retval EFI_UNSUPPORTED         The call is unsupported.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
**/
EFI_STATUS
EFIAPI
SpiReadBlocks (
  IN  UINTN                         RegionType,
  IN  EFI_PEI_LBA                   Address,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  )
{
  EFI_STATUS Status;

  if (mSpiService == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (Address == 0xFFFFFFFF) {
    return EFI_NO_MEDIA;
  }

  Status = mSpiService->SpiRead (RegionType, (UINT32) (Address), (UINT32)BufferSize, Buffer);

  return Status;
}

/**
  Gets a SPI device's media information.

  This function will provide the caller with the specified SPI device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  RegionType     Didn't use now.
  @param[out] DevBlockInfo   The Block Io information of the SPI device.

  @retval EFI_SUCCESS        The Block Io information about the specified block device
                             was obtained successfully.
  @retval EFI_UNSUPPORTED    The call is unsupported.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.
**/
EFI_STATUS
EFIAPI
SpiGetMediaInfo (
  UINTN                          RegionType,
  DEVICE_BLOCK_INFO              *DevBlockInfo
  )
{
  // This Lib will only support Images in PDR region
  // And need minor update this lib to support it.

  EFI_STATUS  Status;
  UINT32 FlashBase;
  UINT32 FlashSize;

  if (mSpiService == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (DevBlockInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = mSpiService->SpiGetRegion (FlashRegionAll, &FlashBase, &FlashSize);
  DevBlockInfo->BlockNum  = ((FlashSize % SPI_BLOCK_SIZE) == 0) ? FlashSize / SPI_BLOCK_SIZE :
                            (FlashSize / SPI_BLOCK_SIZE) + 1;
  DevBlockInfo->BlockSize = SPI_BLOCK_SIZE;

  return Status;
}
