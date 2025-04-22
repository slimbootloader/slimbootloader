/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __NVM_CTRL_LIB_H__
#define __NVM_CTRL_LIB_H__

#include <BlockDevice.h>

#pragma pack(1)
typedef struct {
  UINT8       Bus;
  UINT8       Device;
  UINT8       Func;
} PCI_ADDR_NVM;

typedef struct {
  PCI_ADDR_NVM        DeviceAddress;
} NVM_CTRL_PLATFORM_INFO;
#pragma pack()


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
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.

**/
EFI_STATUS
EFIAPI
NvmeGetMediaInfo (
  IN  UINTN                          DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  );

/**
  This function reads data from Nvme device to Memory.

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
NvmeReadBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_PEI_LBA                   StartLBA,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  );

/**
  This function writes data from Memory to Nvme device.

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      Target EMMC block number(LBA) where data will be written
  @param[in]  DataSize      Total data size to be written in bytes unit
  @param[in]  DataAddress   Data address in Memory to be copied to EMMC

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
NvmeWriteBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         DataSize,
  IN  VOID                          *DataAddress
  );


/**
  This function initializes Nvme device.
  @param[in]  NvmeHcPciBase Nvme Host Controller's PCI ConfigSpace Base address
  @param[in]  NvmeInitMode         Nvme mode this parameter to have compatibility with DevInit in
                                   LinuxLoader.


  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
EFIAPI
NvmeInitialize (
  IN  UINTN               NvmeHcPciBase,
  IN  DEVICE_INIT_PHASE   NvmeInitMode
  );

#endif
