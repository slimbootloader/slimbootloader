/** @file
  PE/Coff Extra Action library instances.

  Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PeCoffExtraActionLib.h>

/**
  Performs additional actions after a PE/COFF image has been loaded and relocated.

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that has already been loaded and relocated.

**/
VOID
EFIAPI
PeCoffLoaderRelocateImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  )
{
  ASSERT (ImageContext != NULL);
}

/**
  Performs additional actions just before a PE/COFF image is unloaded.  Any resources
  that were allocated by PeCoffLoaderRelocateImageExtraAction() must be freed.

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that is being unloaded.

**/
VOID
EFIAPI
PeCoffLoaderUnloadImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  )
{
  ASSERT (ImageContext != NULL);
}

/**
  Find and report image info to HOST.

  @param  ImageBase        Pointer to the current image base.

**/
VOID
PeCoffFindAndReportImageInfo (
  IN UINT32   ImageBase
  )
{
  return;
}
