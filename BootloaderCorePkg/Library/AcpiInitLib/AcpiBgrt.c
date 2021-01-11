/** @file

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GraphicsLib.h>
#include <Library/PcdLib.h>
#include "AcpiInitLibInternal.h"


//
// ACPI Boot Graphics Resource Table template
//
CONST
EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE mBootGraphicsResourceTableTemplate = {
  {
    EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE_SIGNATURE,
    sizeof (EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE),
    EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE_REVISION,     // Revision
    0x00,  // Checksum will be updated at runtime
    //
    // It is expected that these values will be updated at EntryPoint.
    //
    EFI_ACPI_OEM_ID,                 // OEM ID is a 6 bytes long field
    EFI_ACPI_OEM_TABLE_ID,           // OEM Table ID(8 bytes long)
    EFI_ACPI_OEM_REVISION,           // OEM Revision
    EFI_ACPI_CREATOR_ID,             // Creator ID
    EFI_ACPI_CREATOR_REVISION        // Creator Revision
  },
  EFI_ACPI_5_0_BGRT_VERSION,         // Version
  EFI_ACPI_5_0_BGRT_STATUS_VALID,    // Status
  EFI_ACPI_5_0_BGRT_IMAGE_TYPE_BMP,  // Image Type
  0,                                 // Image Address
  0,                                 // Image Offset X
  0                                  // Image Offset Y
};


/**
  Update Boot Graphics Resource Table (BGRT).

  @param[in] Table          Pointer of ACPI BGRT Table.

  @retval EFI_SUCCESS       Update ACPI BGRT table successfully.
  @retval Others            Failed to update FPDT table.
 **/
EFI_STATUS
UpdateBgrt (
  IN  UINT8                           *Table
  )
{
  UINT32       BmpBase;
  UINT32       OffX;
  UINT32       OffY;
  EFI_STATUS   Status;
  EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE  *Bgrt;
  EFI_PEI_GRAPHICS_INFO_HOB                  *GfxInfoHob;

  if (PcdGet32 (PcdSplashLogoAddress) == 0) {
    return EFI_UNSUPPORTED;
  }

  // Get framebuffer info
  GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (NULL, NULL, &gEfiGraphicsInfoHobGuid);
  if (GfxInfoHob == NULL) {
    return EFI_UNSUPPORTED;
  }

  BmpBase = PCD_GET32_WITH_ADJUST (PcdSplashLogoAddress);
  Status = GetBmpDisplayPos ((VOID *)(UINTN)BmpBase, &OffX, &OffY, GfxInfoHob);
  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }

  Bgrt = (EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE *)Table;
  Bgrt->ImageAddress = (UINTN)BmpBase;
  Bgrt->ImageOffsetX = OffX;
  Bgrt->ImageOffsetY = OffY;

  if (!FeaturePcdGet (PcdSplashEnabled)) {
    Bgrt->Status = 0;
  }

  return EFI_SUCCESS;
}
