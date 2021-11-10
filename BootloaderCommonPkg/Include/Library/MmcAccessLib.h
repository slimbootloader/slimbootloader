/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
 * MmcAccessLib.h
 */
#ifndef _MMC_ACCESS_LIB_H_
#define _MMC_ACCESS_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <BlockDevice.h>

typedef enum {
  Hs200 = 0,
  Hs400,
  Ddr50,
  Sdr25
} EMMC_MODE;

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
MmcGetMediaInfo (
  IN  UINTN                          DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  );

/**
  This function reads data from EMMC to Memory.

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
MmcReadBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_PEI_LBA                   StartLBA,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  );

/**
  This function writes data from Memory to EMMC

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      Target EMMC block number(LBA) where data will be written
  @param[in]  DataSize      Total data size to be written in bytes unit
  @param[in] DataAddress   Data address in Memory to be copied to EMMC

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
MmcWriteBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         DataSize,
  IN  VOID                          *DataAddress
  );


/**
  This function initializes EMMC device

  @param[in]  MmcHcPciBase   MMC Host Controller's PCI ConfigSpace Base address
  @param[in]  MmcInitMode    For the performance optimization,
                             EMMC initialization is separated to early init and the rest of init.

                             DevInitAll        : Execute generic MMC device initialization
                             DevInitOnlyPhase1 : Execute only early phase initialization
                             DevInitOnlyPhase2 : Skip early phase initialization,
                                                 and then initialize the rest of initialization


  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
EFIAPI
MmcInitialize (
  IN  UINTN               MmcHcPciBase,
  IN  DEVICE_INIT_PHASE   MmcInitMode
  );

/**
  This function initializes SD device

  @param[in]  SdHcPciBase    SD Host Controller's PCI ConfigSpace Base address
  @param[in]  SdInitMode     For the performance optimization,
                             SD initialization is separated to early init and the rest of init.

                             DevInitAll        : Execute generic SD device initialization
                             DevInitOnlyPhase1 : Execute only early phase initialization
                             DevInitOnlyPhase2 : Skip early phase initialization,
                                                 and then initialize the rest of initialization

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
EFIAPI
SdInitialize (
  IN  UINTN               SdHcPciBase,
  IN  DEVICE_INIT_PHASE   SdInitMode
  );

/**
  To select eMMC card operating mode HS200/HS400

  @param[in] EmmcMode                    To select HS200 or HS400 mode

  @retval EFI_SUCCESS                    Emmc Mode Select successful.
  @retval EFI_INVALID_PARAMETER          A parameter was incorrect.
**/
EFI_STATUS
EFIAPI
EmmcModeSelection (
  IN EMMC_MODE                          EmmcMode
  );

/**
  This function gets serial number of eMMC card.

  @param[out] SerialNumber              Serial Number buffer.
  @param[in]  Length                    Serial Number buffer length.

  @retval EFI_SUCCESS                   Serial Number Valid.
  @retval Others                        A parameter was incorrect.
**/
EFI_STATUS
EFIAPI
EmmcGetSerialNumber (
  IN  CHAR8*                            SerialNumber,
  IN  UINT32                            Length
  );

/**
  This function is an extention of MmcWriteBlocks API.

  @param[in]  DeviceIndex     Specifies the block device to which the function wants
                              to talk.
  @param[in]  StartLBA        Target EMMC block number(LBA) where data will be written
  @param[in]  DataSize        Total data size to be written in bytes unit
  @param[in]  DataAddress     Data address in Memory to be copied to EMMC
  @param[in]  IsReliableWrite Flag to specify whether the write needs to be reliable/authenticated
                              or not. This was introduced as part of RPMB library support

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.
**/
EFI_STATUS
EFIAPI
MmcWriteBlocksExt (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         DataSize,
  IN  VOID                          *DataAddress,
  IN  BOOLEAN                       IsReliableWrite
  );
#endif
