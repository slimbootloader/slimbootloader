/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef ASPEED_GFX_LIB_H_
#define ASPEED_GFX_LIB_H_

#include <Guid/GraphicsInfoHob.h>

typedef struct display_timing_info {
  UINT32  Clock;
  UINT32  RefreshRate;

  UINT32  Hdisp;
  UINT32  Hfp;
  UINT32  Hsync;
  UINT32  Hbp;
  UINT32  Phsync;

  UINT32  Vdisp;
  UINT32  Vfp;
  UINT32  Vsync;
  UINT32  Vbp;
  UINT32  Pvsync;
} DISPLAY_TIMING_INFO;

/**
  Initialize Aspeed Graphics

  @param[in]  DevPciBase    Aspeed Graphics PCI Info

  @retval  EFI_SUCCESS      Graphics initialization done properly
  @retval  others           Error happening when initializing.

**/
EFI_STATUS
EFIAPI
AspeedGraphicsInit (
  IN  UINT32                DevPciBase,
  IN  DISPLAY_TIMING_INFO   *Timing
  );

/**
  Get GOP MODE Info

  @param[out] Mode          EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE data pointer to be updated

  @retval  EFI_SUCCESS      Getting GOP MODE Info Successfully
  @retval  others           Error happening

**/
EFI_STATUS
EFIAPI
AspeedGetGraphicOutputProtocolModeInfo (
  OUT EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode
  );

#endif // ASPEED_GFX_LIB_H_
