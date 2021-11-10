/** @file

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GraphicsLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <IndustryStandard/Bmp.h>
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
  UINT32                                      BmpBase;
  UINT32                                      OffX;
  UINT32                                      OffY;
  EFI_STATUS                                  Status;
  EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE  *Bgrt;
  EFI_PEI_GRAPHICS_INFO_HOB                  *GfxInfoHob;
  BMP_IMAGE_HEADER                           *BmpHdr;
  BMP_IMAGE_HEADER                           *OrgBmpHdr;
  UINT32                                      ImageLen;
  UINT32                                      FileLen;

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

  OrgBmpHdr = (BMP_IMAGE_HEADER *)(UINTN)BmpBase;
  Bgrt = (EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE *)Table;
  Bgrt->ImageAddress = (UINTN)OrgBmpHdr;
  if (!((OrgBmpHdr->BitPerPixel == 24) || (OrgBmpHdr->BitPerPixel == 32))) {
    // Need to convert the BMP image into 32bit BMP supported by BGRT
    ImageLen = (UINT32)MultU64x32 (OrgBmpHdr->PixelWidth << 2,  OrgBmpHdr->PixelHeight);
    FileLen  = sizeof(BMP_IMAGE_HEADER) + ImageLen;
    BmpHdr = (BMP_IMAGE_HEADER *)AllocatePages (EFI_SIZE_TO_PAGES (FileLen));
    if (BmpHdr != NULL) {
      CopyMem (BmpHdr, OrgBmpHdr, sizeof(BMP_IMAGE_HEADER));
      BmpHdr->Size = FileLen;
      BmpHdr->ImageOffset = sizeof(BMP_IMAGE_HEADER);
      BmpHdr->HeaderSize  = sizeof(BMP_IMAGE_HEADER) - OFFSET_OF(BMP_IMAGE_HEADER, HeaderSize);
      BmpHdr->BitPerPixel = 32;
      BmpHdr->CompressionType = 0;
      BmpHdr->ImageSize = 0;
      BmpHdr->NumberOfColors  = 0;
      BmpHdr->ImportantColors = 0;
      Status = DisplayBmpToFrameBuffer (OrgBmpHdr, &BmpHdr[1], ImageLen, GfxInfoHob);
      if (!EFI_ERROR(Status)) {
        Bgrt->ImageAddress = (UINTN)BmpHdr;
      }
    }
  }
  Bgrt->ImageOffsetX = OffX;
  Bgrt->ImageOffsetY = OffY;

  if (!FeaturePcdGet (PcdSplashEnabled)) {
    Bgrt->Status = 0;
  }

  return EFI_SUCCESS;
}
