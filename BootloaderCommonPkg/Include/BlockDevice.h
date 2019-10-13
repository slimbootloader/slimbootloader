/** @file

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BLOCK_DEVICE_H_
#define _BLOCK_DEVICE_H_

#include <PiPei.h>
#include <Ppi/BlockIo.h>

typedef struct {
  UINT64   BlockNum;
  UINT32   BlockSize;
} DEVICE_BLOCK_INFO;

typedef enum {
  DevInitAll,
  DevInitOnlyPhase1,
  DevInitOnlyPhase2,
  DevDeinit
} DEVICE_INIT_PHASE;

/**
  This function initializes device

  @param[in]  DevHcPciBase  Device Host Controller's PCI ConfigSpace Base address
  @param[in]  DevInitPhase  For the performance optimization,
                            Device initialization is separated to several phases.

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
typedef
EFI_STATUS
(EFIAPI *DEVICE_INITIALIZE) (
  IN  UINTN               DevHcPciBase,
  IN  DEVICE_INIT_PHASE   DevInitPhase
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
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.

**/
typedef
EFI_STATUS
(EFIAPI *DEVICE_GET_INFO) (
  IN  UINTN                          DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  );

/**
  Reads the requested number of blocks from the specified block device.

  The function reads the requested number of blocks from the device. All the
  blocks are read, or an error is returned.

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
typedef
EFI_STATUS
(EFIAPI *DEVICE_READ_BLOCKS) (
  IN  UINTN                          DeviceIndex,
  IN  EFI_LBA                        StartLBA,
  IN  UINTN                          BufferSize,
  OUT VOID                           *Buffer
  );

/**
  This function writes data from Memory to device

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartBlock    Target block number(LBA) where data will be written
  @param[in]  DataSize      Total data size to be written in bytes unit
  @param[out] DataAddress   Data address in Memory to be copied to device

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
typedef
EFI_STATUS
(EFIAPI *DEVICE_WRITE_BLOCKS) (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         DataSize,
  IN  VOID                          *DataAddress
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

  @retval EFI_SUCCESS         The operation is done correctly.
  @retval Others              The operation fails.

**/
typedef
EFI_STATUS
(EFIAPI *DEVICE_WRITE_BLOCKS_EXT) (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         BufferSize,
  IN  VOID                          *Buffer,
  IN  BOOLEAN                       IsReliableWrite
  );

/**
  This function tuning the device.

  @param[in]  DevHcPciBase     Device Host Controller's PCI ConfigSpace Base address


  @retval EFI_SUCCESS           The request is executed successfully.
  @retval Others                The request could not be executed successfully.

**/
typedef
EFI_STATUS
(EFIAPI *DEVICE_TUNING) (
  IN  UINTN               DevHcPciBase
  );

typedef struct {
  DEVICE_INITIALIZE                  DevInit;
  DEVICE_GET_INFO                    GetInfo;
  DEVICE_READ_BLOCKS                 ReadBlocks;
  DEVICE_WRITE_BLOCKS                WriteBlocks;
  DEVICE_WRITE_BLOCKS_EXT            WriteBlocksExt;
  DEVICE_TUNING                      DevTuning;
} DEVICE_BLOCK_FUNC;

#endif
