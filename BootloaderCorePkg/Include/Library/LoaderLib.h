/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IBB_LOADER_H_
#define _IBB_LOADER_H_


/**
  Load Stage1B raw image to destination address.

  The destination should be temporary memory. It utilizes CSME to
  load Stage1B (IBBM) from boot media to temporary memory.

  @param[in]  IbbDst    The destination address for image loading.
  @param[in]  IbbSrc    Not used. CSME knows where to load image from.
  @param[in]  IbbSize   The size for destination buffer.

  @retval EFI_DEVICE_ERROR
  @retval EFI_SUCCESS  Always return success.

**/
EFI_STATUS
LoadStage1B (
  UINT32 Dst,
  UINT32 Src,
  UINT32 Size
  );

/**
 Initializes boot device and loads stage2 firmware to
 destination address provided.

 @param [in] Dst Destination address to load the Firmware.
 @param [in] Src Source address to get firmware.
 @param [in] Size size of the firmware to load.
 **/
EFI_STATUS
LoadStage2  (
  UINT32 Dst,
  UINT32 Src,
  UINT32 Size
  );

/**
  Based on the boot mode loads the payload image
  from flash or from the source address provided
  and loads on to the destination address.

  @param [in] Dst Destination address to load the payload.
  @param [in] Src Source address to get the payload.
  @param [in] Size size of the payload.
 **/
EFI_STATUS
LoadPayload (
  UINT32 Dst,
  UINT32 Src,
  UINT32 Size
  );

#endif
