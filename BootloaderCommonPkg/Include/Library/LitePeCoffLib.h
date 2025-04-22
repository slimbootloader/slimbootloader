/** @file
  Base PE/COFF loader supports loading any PE32/PE32+ or TE image, but
  only supports relocating IA32, x64, IPF, and EBC images.

  Copyright (c) 2006 - 2019, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008-2009 Apple Inc. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PECOFF_RELOCATE_LIB_H__
#define __PECOFF_RELOCATE_LIB_H__

/**
  Check if the image is a TE or PE32 image.

  @param[in]       ImageBase    Memory address of an image
  @param[in, out]  Header       Pointer to EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION structure.
                                If it is TE/PE32 image, the structure will be updated.

  @retval TRUE                  Image is a TE/PE32 image
  @retval FALSE                 Not a TE/PE32 image

**/
BOOLEAN
EFIAPI
IsTePe32Image (
  IN      VOID                                   *ImageBase,
  IN  OUT EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION    *Header    OPTIONAL
  );


/**
  Retrieves and returns a pointer to the entry point to a PE/COFF image that has been loaded
  into system memory with the PE/COFF Loader Library functions.

  Retrieves the entry point to the PE/COFF image specified by Pe32Data and returns this entry
  point in EntryPoint.  If the entry point could not be retrieved from the PE/COFF image, then
  return RETURN_INVALID_PARAMETER.  Otherwise return RETURN_SUCCESS.

  @param  Pe32Data                  The pointer to the PE/COFF image that is loaded in system memory.
  @param  EntryPoint                The pointer to entry point to the PE/COFF image to return.

  @retval RETURN_SUCCESS            EntryPoint was returned.
  @retval RETURN_INVALID_PARAMETER  The entry point could not be found in the PE/COFF image.

**/
RETURN_STATUS
EFIAPI
PeCoffLoaderGetEntryPoint (
  IN  VOID  *Pe32Data,
  OUT VOID  **EntryPoint
  );


/**
  Performs an specific relocation fpr PECOFF images. The caller needs to
  allocate enough buffer at the PreferedImageBase

  @param  CurrentImageBase        Pointer to the current image base.

  @return Status code.

**/
RETURN_STATUS
EFIAPI
PeCoffRelocateImage (
  IN     UINT32   CurrentImageBase
  );


/**
  Get PE/TE image preferred image base address

  Based on current image location Pe32Data, check if it is same with expected location to run the
  image. If it is not same, it means the image need relocate to expected location. This function
  will return the preferred image base.

  @param[in]   Pe32Data             The pointer to the PE/COFF image that is loaded in system memory.
  @param[out]  Base                 The pointer to receive image base.

  @retval RETURN_SUCCESS            Base was returned successfully.
  @retval RETURN_UNSUPPORTED        It is not PE/COFF image.
**/
RETURN_STATUS
EFIAPI
PeCoffGetPreferredBase (
  IN  VOID                             *Pe32Data,
  OUT UINT32                           *Base       OPTIONAL
  );

/**
  Extract and return the machine type from the PE/COFF image.

  @param  Pe32Data                  The pointer to the PE/COFF image that is loaded in system memory.
  @param  MachinePtr                The pointer to machine type to return.

  @retval RETURN_SUCCESS            Machine was returned successfully.
  @retval RETURN_UNSUPPORTED        Unsupported image format.
  @retval RETURN_INVALID_PARAMETER  The Pe32Data pointer is NULL.

**/
RETURN_STATUS
EFIAPI
PeCoffLoaderGetMachine (
  IN  VOID     *Pe32Data,
  OUT UINT16   *MachinePtr      OPTIONAL
  );

#endif

