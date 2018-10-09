/** @file
  Base PE/COFF loader supports loading any PE32/PE32+ or TE image, but
  only supports relocating IA32, x64, IPF, and EBC images.

  Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008-2009 Apple Inc. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
  If Pe32Data is NULL, then ASSERT().
  If EntryPoint is NULL, then ASSERT().

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
PeCoffRelocateImage (
  IN     UINT32   CurrentImageBase
  );

/**
  Performs an specific relocation fpr PECOFF images. The caller needs to
  allocate enough buffer at the PreferedImageBase

  @param  CurrentImageBase        Pointer to the current image base.

  @return Status code.

**/
RETURN_STATUS
PeCoffRelocateImage (
  IN     UINT32   CurrentImageBase
  );

/**
  Returns PE/COFF image base is loaded in system memory where the input address is in.

  On DEBUG build, searches the PE/COFF image base forward the input address and
  returns it.

  @param  Address    Address located in one PE/COFF image.

  @retval 0          RELEASE build or cannot find the PE/COFF image base.
  @retval others     PE/COFF image base found.

**/
UINTN
EFIAPI
PeCoffSearchImageBase (
  IN UINTN    Address
  );

/**
  Returns a pointer to the PDB file name for a raw PE/COFF image that is not
  loaded into system memory with the PE/COFF Loader Library functions.

  Returns the PDB file name for the PE/COFF image specified by Pe32Data.  If
  the PE/COFF image specified by Pe32Data is not a valid, then NULL is
  returned.  If the PE/COFF image specified by Pe32Data does not contain a
  debug directory entry, then NULL is returned.  If the debug directory entry
  in the PE/COFF image specified by Pe32Data does not contain a PDB file name,
  then NULL is returned.
  If Pe32Data is NULL, then return NULL.

  @param  Pe32Data   Pointer to the PE/COFF image that is loaded in system
                     memory.

  @return The PDB file name for the PE/COFF image specified by Pe32Data or NULL
          if it cannot be retrieved.

**/
VOID *
EFIAPI
PeCoffLoaderGetPdbPointer (
  IN VOID  *Pe32Data
  );

#endif
