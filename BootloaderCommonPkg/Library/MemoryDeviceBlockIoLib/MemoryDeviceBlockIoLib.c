/** @file
  This file provides some helper functions via run time service support
  specific to Memory device.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryDeviceBlockIoLib.h>

/**
  Initialize a Memory instance.

  @param[in]  MemoryDevicePciBase         The PCI base of the Memory flash device.
  @param[in]  DevInitPhase       The initialization phase to prepare the device.

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval EFI_UNSUPPORTED        The call is unsupported.
  @retval Others                 Can't initialize the Memory device.
**/
EFI_STATUS
EFIAPI
InitializeMemoryDevice (
  IN  UINTN               MemoryDevicePciBase,
  IN  DEVICE_INIT_PHASE   DevInitPhase
  )
{
  return EFI_SUCCESS;
}

/**
  This function reads blocks from the Memory slave device.

  @param[in]  RegionType          The region type of the Memory area to read from.
  @param[in]  Address             The block address in the RegionType to read from on the Memory.
  @param[in]  BufferSize          Size of the Buffer in bytes.
  @param[out] Buffer              Pointer to caller-allocated buffer containing the data received during the Memory cycle.

  @retval EFI_SUCCESS             Memory command completes successfully.

**/
EFI_STATUS
EFIAPI
MemoryDeviceReadBlocks (
  IN  UINTN                         RegionType,
  IN  EFI_PEI_LBA                   Address,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  )
{
  if ((Buffer == NULL) || (BufferSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (Buffer, (VOID *) (UINTN)Address, BufferSize);

  return EFI_SUCCESS;
}

/**
  Gets a Memory device's media information.

  This function will provide the caller with the specified Memory device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  RegionType     The region type of the Memory area to access.
  @param[out] DevBlockInfo   The Block Io information of the Memory device.

  @retval EFI_SUCCESS        The Block Io information about the specified block device
                             was obtained successfully.
  @retval EFI_UNSUPPORTED    The call is unsupported.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.
**/
EFI_STATUS
EFIAPI
MemoryDeviceGetMediaInfo (
  UINTN                          RegionType,
  DEVICE_BLOCK_INFO              *DevBlockInfo
  )
{
  if (DevBlockInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DevBlockInfo->BlockNum  = 0x100000000;
  DevBlockInfo->BlockSize = RAM_BLOCK_SIZE;

  return EFI_SUCCESS;
}
