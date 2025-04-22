/** @file

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GRAPHICS_INIT_LIB_H_
#define _GRAPHICS_INIT_LIB_H_

#include <Guid/GraphicsInfoHob.h>

typedef struct {
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

typedef struct {
  UINT32               PciTempResourceBase;
  DISPLAY_TIMING_INFO  DisaplayTimingInfo;
} GRAPHICS_INIT_POLICY;

/*
  This function performs the PEI module initialization.

  @param[in]   GraphicsPolicyPtr   Pointer to the graphics policy structure.
  @param[out]  GfxInfoHob          Poniter to receive the GFX info HOB.

  @retval   EFI_SUCCESS       If initialization is successful.
  @retval   EFI_DEVICE_ERROR  If initialization fails.
**/
EFI_STATUS
EFIAPI
GraphicsInit (
  IN  UINT32                       GfxDevPciBase,
  IN  GRAPHICS_INIT_POLICY        *GraphicsPolicyPtr
  );


/**
  Get GOP MODE Info

  @param[out] Mode          EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE data pointer to be updated

  @retval  EFI_SUCCESS      Getting GOP MODE Info Successfully
  @retval  others           Error happening

**/
EFI_STATUS
EFIAPI
GetGraphicOutputModeInfo (
  OUT EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode
  );

#endif
