/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include "AspeedGfxInternal.h"

STATIC CONST DISPLAY_TIMING_INFO mDefaultTiming = {
  40000, 60,
  800, 40, 128, 88, 1,
  600, 1, 4, 23, 1
};

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
  )
{
  DISPLAY_TIMING_INFO   *TimingInfo;

  TimingInfo = (DISPLAY_TIMING_INFO *)&mDefaultTiming;
  if (Timing != NULL) {
    TimingInfo = Timing;
  }
  return aspeed_gfx_init (DevPciBase, TimingInfo);
}

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
  )
{
  return aspeed_gop_mode_info (Mode);
}
