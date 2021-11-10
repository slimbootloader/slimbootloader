/** @file
  The file provides Media block I/O interfaces.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/MediaAccessLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UfsBlockIoLib.h>
#include <Library/UsbBlockIoLib.h>
#include <Library/AhciBlockIoLib.h>
#include <Library/MmcAccessLib.h>
#include <Library/SpiBlockIoLib.h>
#include <Library/PciNvmCtrlLib.h>
#include <Library/MemoryDeviceBlockIoLib.h>
#include <Library/MmcTuningLib.h>

OS_BOOT_MEDIUM_TYPE   mCurrentMediaType = OsBootDeviceMax;
DEVICE_BLOCK_FUNC     mDeviceBlockFuncs[OsBootDeviceMax];

/**
  Get current media interface type.

  @retval    The current selected media type.

**/
OS_BOOT_MEDIUM_TYPE
EFIAPI
MediaGetInterfaceType (
  VOID
  )
{
  return mCurrentMediaType;
}

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
  )
{
  UINTN     Type;

  if (MediaType >= OsBootDeviceMax) {
    return EFI_INVALID_PARAMETER;
  }

  if (mCurrentMediaType == OsBootDeviceMax) {
    //
    // Init Boot device functions
    //
    Type = OsBootDeviceSata;
    if (FixedPcdGet32 (PcdSupportedMediaTypeMask) & (1 << Type)) {
      mDeviceBlockFuncs[Type].DevInit     = AhciInitialize;
      mDeviceBlockFuncs[Type].GetInfo     = AhciGetMediaInfo;
      mDeviceBlockFuncs[Type].ReadBlocks  = AhciReadBlocks;
      mDeviceBlockFuncs[Type].WriteBlocks = AhciWriteBlocks;
    }

    Type = OsBootDeviceEmmc;
    if (FixedPcdGet32 (PcdSupportedMediaTypeMask) & (1 << Type)) {
      mDeviceBlockFuncs[Type].DevInit     = MmcInitialize;
      mDeviceBlockFuncs[Type].GetInfo     = MmcGetMediaInfo;
      mDeviceBlockFuncs[Type].ReadBlocks  = MmcReadBlocks;
      mDeviceBlockFuncs[Type].WriteBlocks = MmcWriteBlocks;
      mDeviceBlockFuncs[Type].WriteBlocksExt = MmcWriteBlocksExt;
      mDeviceBlockFuncs[Type].DevTuning   = MmcTuning;
    }

    Type = OsBootDeviceSd;
    if (FixedPcdGet32 (PcdSupportedMediaTypeMask) & (1 << Type)) {
      mDeviceBlockFuncs[Type].DevInit     = SdInitialize;
      mDeviceBlockFuncs[Type].GetInfo     = MmcGetMediaInfo;
      mDeviceBlockFuncs[Type].ReadBlocks  = MmcReadBlocks;
      mDeviceBlockFuncs[Type].WriteBlocks = MmcWriteBlocks;
    }

    Type = OsBootDeviceUfs;
    if (FixedPcdGet32 (PcdSupportedMediaTypeMask) & (1 << Type)) {
      mDeviceBlockFuncs[Type].DevInit     = InitializeUfs;
      mDeviceBlockFuncs[Type].GetInfo     = UfsGetMediaInfo;
      mDeviceBlockFuncs[Type].ReadBlocks  = UfsReadBlocks;
      mDeviceBlockFuncs[Type].WriteBlocks = UfsWriteBlocks;
    }

    Type = OsBootDeviceUsb;
    if (FixedPcdGet32 (PcdSupportedMediaTypeMask) & (1 << Type)) {
      mDeviceBlockFuncs[Type].DevInit     = InitializeUsb;
      mDeviceBlockFuncs[Type].GetInfo     = UsbGetMediaInfo;
      mDeviceBlockFuncs[Type].ReadBlocks  = UsbReadBlocks;
      mDeviceBlockFuncs[Type].WriteBlocks = NULL;
    }

    Type = OsBootDeviceSpi;
    if (FixedPcdGet32 (PcdSupportedMediaTypeMask) & (1 << Type)) {
      // Only when images are in PDR region.
      mDeviceBlockFuncs[Type].DevInit     = InitializeSpi;
      mDeviceBlockFuncs[Type].GetInfo     = SpiGetMediaInfo;
      mDeviceBlockFuncs[Type].ReadBlocks  = SpiReadBlocks;
      mDeviceBlockFuncs[Type].WriteBlocks = NULL;
    }

    Type = OsBootDeviceNvme;
    if (FixedPcdGet32 (PcdSupportedMediaTypeMask) & (1 << Type)) {
      mDeviceBlockFuncs[Type].DevInit     = NvmeInitialize;
      mDeviceBlockFuncs[Type].GetInfo     = NvmeGetMediaInfo;
      mDeviceBlockFuncs[Type].ReadBlocks  = NvmeReadBlocks;
      mDeviceBlockFuncs[Type].WriteBlocks = NvmeWriteBlocks;
    }

    Type = OsBootDeviceMemory;
    if (FixedPcdGet32 (PcdSupportedMediaTypeMask) & (1 << Type)) {
      mDeviceBlockFuncs[Type].DevInit     = InitializeMemoryDevice;
      mDeviceBlockFuncs[Type].GetInfo     = MemoryDeviceGetMediaInfo;
      mDeviceBlockFuncs[Type].ReadBlocks  = MemoryDeviceReadBlocks;
      mDeviceBlockFuncs[Type].WriteBlocks = NULL;
    }
  }

  if (mDeviceBlockFuncs[MediaType].DevInit == NULL) {
    return EFI_UNSUPPORTED;
  }

  mCurrentMediaType = MediaType;
  return EFI_SUCCESS;
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
  )
{
  if (mCurrentMediaType >= OsBootDeviceMax) {
    return EFI_NOT_READY;
  }

  if (mDeviceBlockFuncs[mCurrentMediaType].ReadBlocks == NULL) {
    return EFI_UNSUPPORTED;
  }

  return mDeviceBlockFuncs[mCurrentMediaType].ReadBlocks (DeviceIndex, StartLBA, BufferSize, Buffer);
}

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
  )
{
  if (mCurrentMediaType >= OsBootDeviceMax) {
    return EFI_NOT_READY;
  }

  if (mDeviceBlockFuncs[mCurrentMediaType].WriteBlocks == NULL) {
    return EFI_UNSUPPORTED;
  }

  return mDeviceBlockFuncs[mCurrentMediaType].WriteBlocks (DeviceIndex, StartLBA, BufferSize, Buffer);
}

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
  )
{
  if (mCurrentMediaType >= OsBootDeviceMax) {
    return EFI_NOT_READY;
  }

  if (mDeviceBlockFuncs[mCurrentMediaType].GetInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  return mDeviceBlockFuncs[mCurrentMediaType].GetInfo (DeviceIndex, DevBlockInfo);
}

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
  )
{
  if (mCurrentMediaType >= OsBootDeviceMax) {
    return EFI_NOT_READY;
  }

  if (mDeviceBlockFuncs[mCurrentMediaType].DevInit == NULL) {
    return EFI_UNSUPPORTED;
  }

  return mDeviceBlockFuncs[mCurrentMediaType].DevInit (MediaHcPciBase, DevInitPhase);
}

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
  )
{
  if (mCurrentMediaType >= OsBootDeviceMax) {
    return EFI_NOT_READY;
  }

  if (mDeviceBlockFuncs[mCurrentMediaType].WriteBlocksExt == NULL) {
    return EFI_UNSUPPORTED;
  }

  return mDeviceBlockFuncs[mCurrentMediaType].WriteBlocksExt (DeviceIndex, StartLBA, BufferSize, Buffer, IsReliableWrite);
}


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
  )
{
  if (mCurrentMediaType >= OsBootDeviceMax) {
    return EFI_NOT_READY;
  }

  if (mDeviceBlockFuncs[mCurrentMediaType].DevTuning == NULL) {
    return EFI_UNSUPPORTED;
  }

  return mDeviceBlockFuncs[mCurrentMediaType].DevTuning (MediaHcPciBase);
}

