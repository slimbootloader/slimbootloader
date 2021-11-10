/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
 * MemoryDeviceBlockIoLib.h
 */
#ifndef _MEM_ACCESS_LIB_H_
#define _MEM_ACCESS_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <BlockDevice.h>

#define RAM_BLOCK_SIZE 0x1

/**
  This function reads blocks from the Memory Device slave device.

  @param[in]  RegionType          The region type of the Memory Device area to read from.
  @param[in]  Address             The block address in the RegionType to read from on the Memory Device.
  @param[in]  BufferSize          Size of the Buffer in bytes.
  @param[out] Buffer              Pointer to caller-allocated buffer containing the data received during the Memory Device cycle.

  @retval EFI_SUCCESS             Memory Device command completes successfully.
  @retval EFI_DEVICE_ERROR        Device error, the command aborts abnormally.
  @retval EFI_ACCESS_DENIED       Some unrecognized or blocked command encountered in hardware sequencing mode
  @retval EFI_UNSUPPORTED         The specified Memory Device region is not supported for access.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
**/
EFI_STATUS
EFIAPI
MemoryDeviceReadBlocks (
  IN  UINTN                         RegionType,
  IN  EFI_PEI_LBA                   Address,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  );

/**
  Gets a Memory Device device's media information.

  This function will provide the caller with the specified Memory Device device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  RegionType     The region type of the Memory Device area to access.

  @param[out] DevBlockInfo   The Block Io information of the Memory Device device.

  @retval EFI_SUCCESS        The Block Io information about the specified block device
                             was obtained successfully.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.

**/
EFI_STATUS
EFIAPI
MemoryDeviceGetMediaInfo (
  IN  UINTN                          RegionType,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  );

/**
  Initialize a Memory Device instance.

  @param[in]  MemoryDevicePciBase The PCI base of the Memory Device flash device.
  @param[in]  DevInitPhase        The initialization phase to prepare the device.

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval Others                 Can't initialize the Memory Device device.
**/
EFI_STATUS
EFIAPI
InitializeMemoryDevice (
  IN  UINTN               MemoryDevicePciBase,
  IN  DEVICE_INIT_PHASE   DevInitPhase
  );

#endif
