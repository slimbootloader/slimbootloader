/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MEDIA_ACCESS_LIB_H_
#define _MEDIA_ACCESS_LIB_H_

#include <BlockDevice.h>
#include <Guid/OsBootOptionGuid.h>

/**
  Get current media interface type.

  @retval    The current selected media type.

**/
OS_BOOT_MEDIUM_TYPE
EFIAPI
MediaGetInterfaceType (
  VOID
  );


/**
  Select current media interface type.

  The function selects the requested media interface type. All following media
  access will be routed to this selected interface.

  @param[in]  MediaType     Specifies the media interface type to set.

  @retval EFI_INVALID_PARAMETER   MediaType is not a valid type.
  @retval EFI_UNSUPPORTED         The media type is not supported.
  @retval EFI_SUCCESS             The medis type was selected successfully.

**/
EFI_STATUS
EFIAPI
MediaSetInterfaceType (
  IN OS_BOOT_MEDIUM_TYPE  MediaType
  );


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

  @retval EFI_SUCCESS             The data was read correctly from the device.
  @retval EFI_UNSUPPORTED         The interface is not supported.
  @retval EFI_NOT_READY           The MediaSetInterfaceType() has not been called yet.
  @retval EFI_DEVICE_ERROR        The device reported an error while attempting
                                  to perform the read operation.
  @retval EFI_INVALID_PARAMETER   The read request contains LBAs that are not
                                  valid, or the buffer is not properly aligned.
  @retval EFI_NO_MEDIA            There is no media in the device.
  @retval EFI_BAD_BUFFER_SIZE     The BufferSize parameter is not a multiple of
                                  the intrinsic block size of the device.

**/
EFI_STATUS
EFIAPI
MediaReadBlocks (
  IN  UINTN                          DeviceIndex,
  IN  EFI_LBA                        StartLBA,
  IN  UINTN                          BufferSize,
  OUT VOID                          *Buffer
  );


/**
  This function writes data from Memory to media

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      Target media block number(LBA) where data will be written
  @param[in]  BufferSize    Total data size to be written in bytes unit
  @param[in]  Buffer        Data address in Memory to be copied to media.

  @retval EFI_SUCCESS            The operation is done correctly.
  @retval EFI_UNSUPPORTED        The interface is not supported.
  @retval EFI_NOT_READY          The MediaSetInterfaceType() has not been called yet.
  @retval EFI_NOT_FOUND          The device index cannot be found.
  @retval EFI_UNSUPPORTED        Media is not ready yet.
  @retval EFI_INVALID_PARAMETER  Input parameters are not valid.
  @retval EFI_DEVICE_ERROR       The write failed.
  @retval EFI_TIMEOUT            The operation is time out.

**/
EFI_STATUS
EFIAPI
MediaWriteBlocks (
  IN UINTN                         DeviceIndex,
  IN EFI_LBA                       StartLBA,
  IN UINTN                         BufferSize,
  IN VOID                         *Buffer
  );


/**
  Gets a block device's media information.

  This function will provide the caller with the specified block device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  DeviceIndex    Specifies the block device to which the function wants
                             to talk.
  @param[out] DevBlockInfo   The Block Io information of the specified block partition.

  @retval EFI_SUCCESS        The Block Io information about the specified block device
                             was obtained successfully.
  @retval EFI_UNSUPPORTED    Media is not ready yet.
  @retval EFI_NOT_FOUND      The device index cannot be found.
  @retval EFI_UNSUPPORTED    The interface is not supported.
  @retval EFI_NOT_READY      The MediaSetInterfaceType() has not been called yet.

**/
EFI_STATUS
EFIAPI
MediaGetMediaInfo (
  IN  UINTN                           DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  );


/**
  The function will initialize media device.

  Based on MediaHcPciBase, this function will initialize media controller, allocate
  necessary resources, and enumerate all the devices.

  @param[in]  MediaHcPciBase     Device host controller's PCI ConfigSpace Base address.
  @param[in]  DevInitPhase       For the performance optimization,
                                 Device initialization is separated to several phases.

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval EFI_NOT_FOUND          Can't find any block devices for boot.
  @retval EFI_OUT_OF_RESOURCES   Not sufficient memory.
  @retval EFI_DEVICE_ERROR       Hardware error during the initialization.
  @retval EFI_UNSUPPORTED        The interface is not supported.
  @retval EFI_NOT_READY          The MediaSetInterfaceType() has not been called yet.

**/
EFI_STATUS
EFIAPI
MediaInitialize (
  IN UINTN                     MediaHcPciBase,
  IN DEVICE_INIT_PHASE         DevInitPhase
  );

/**
  This function is an extended version of the WriteBloks API

  @param[in]  DeviceIndex     Specifies the block device to which the function wants
                              to talk.
  @param[in]  StartLBA        Target media block number(LBA) where data will be written
  @param[in]  BufferSize      Total data size to be written in bytes unit
  @param[in]  Buffer          Data address in Memory to be copied to media.
  @param[in]  IsReliableWrite Flag to specify whether the write needs to be reliable/authenticated
                              or not. This was introduced as part of RPMB library support

  @retval EFI_SUCCESS            The operation is done correctly.
  @retval EFI_UNSUPPORTED        The interface is not supported.
  @retval EFI_NOT_READY          The MediaSetInterfaceType() has not been called yet.
  @retval EFI_NOT_FOUND          The device index cannot be found.
  @retval EFI_UNSUPPORTED        Media is not ready yet.
  @retval EFI_INVALID_PARAMETER  Input parameters are not valid.
  @retval EFI_DEVICE_ERROR       The write failed.
  @retval EFI_TIMEOUT            The operation is time out.

**/
EFI_STATUS
EFIAPI
MediaWriteBlocksExt (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         BufferSize,
  IN  VOID                          *Buffer,
  IN  BOOLEAN                       IsReliableWrite
  );

/**
  This function tunes the device.

  @param[in]  DevHcPciBase     Device Host Controller's PCI ConfigSpace Base address


  @retval EFI_SUCCESS           The request is executed successfully.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
EFIAPI
MediaTuning (
  IN UINTN                     MediaHcPciBase
  );

#endif

