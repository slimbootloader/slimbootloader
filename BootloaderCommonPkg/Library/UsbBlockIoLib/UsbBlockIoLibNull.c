/** @file
  Null Base Debug Library instance with empty functions.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <BlockDevice.h>

/**
  The function will initialize Dummy device.

  @param[in]  NullHcPciBase      Dummy Host Controller's PCI ConfigSpace Base address
  @param[in]  DevInitPhase       For the performance optimization,
                                 Device initialization is separated to several phases.

  @retval EFI_UNSUPPORTED        The interface is not implemented yet.

**/
EFI_STATUS
EFIAPI
InitializeUsb (
  IN  UINTN                     NullHcPciBase,
  IN  DEVICE_INIT_PHASE         DevInitPhase
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Gets a block device's media information.

  This function will provide the caller with the specified block device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  DeviceIndex    Specifies the block device to which the function wants
                             to talk.
  @param[out] DevBlockInfo   The Block Io information of the specified block partition.

  @retval EFI_UNSUPPORTED    The interface is not implemented yet.

**/
EFI_STATUS
EFIAPI
UsbGetMediaInfo (
  IN  UINTN                          DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Reads the requested number of blocks from the specified block device.

  The function reads the requested number of blocks from the device. All the
  blocks are read, or an error is returned. If there is no media in the device,
  the function returns EFI_NO_MEDIA.

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      The starting logical block address (LBA) to read from
                            on the device
  @param[in]  BufferSize    The size of the Buffer in bytes. This number must be
                            a multiple of the intrinsic block size of the device.
  @param[out] Buffer        A pointer to the destination buffer for the data.
                            The caller is responsible for the ownership of the
                            buffer.

  @retval EFI_UNSUPPORTED   The interface is not implemented yet.

**/
EFI_STATUS
EFIAPI
UsbReadBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  )
{
  return EFI_UNSUPPORTED;
}
