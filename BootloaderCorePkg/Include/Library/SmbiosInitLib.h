/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SMBIOS_INIT_LIB_H__
#define __SMBIOS_INIT_LIB_H__

#include <IndustryStandard/SmBios.h>


#pragma pack(1)

typedef struct {
  UINT8     Type;
  UINT8     Idx;
  CHAR8     *String;
} SMBIOS_TYPE_STRINGS;

#pragma pack()

/**
  After adding and appending all Smbios Types, we need to do the following
    1. Check for the table overflow
    2. Add Type 127
    3. Adjust the TotalLength in entry point struct
    4. Update entry point & intermediate checksum

  @param[in]  NewMaxStructSize    Maximum type size of all the types added    , if called from SmbiosInit
                                  Type Length of the current table appended   , if called from AppendSmbiosType

  @retval                         EFI_DEVICE_ERROR, if Smbios Entry is NULL
                                  Overflow status , otherwise

**/
EFI_STATUS
FinalizeSmbios (
  VOID
  );

/**
  Append a string to an Smbios type header

  @param[in]  Type      Type to which a string is appended
  @param[in]  String    String literal to be appended

  @retval               EFI_SUCCESS, if string is appended successfully
                        EFI_ERROR,    otherwise

**/
EFI_STATUS
EFIAPI
AddSmbiosString (
  IN  UINT8     Type,
  IN  CHAR8    *String
  );

/**
  Add a particular Smbios type to the Smbios allocated region.

  @param[in]  HdrInfo     Address of the type being added

  @retval                 EFI_SUCCESS, if Type added successfully,
                          EFI_ERROR,   otherwise

**/
EFI_STATUS
EFIAPI
AddSmbiosType (
  IN  VOID   *HdrInfo
  );

/**
  Add a SMBIOS string entry to the destination buffer.

  @param[out]     Dest   Pointer to the destination buffer where the entry will be added.
  @param[in]      Type   SMBIOS type for the entry.
  @param[in]      Index  String index for the entry.
  @param[in]      String Null-terminated ASCII string to store.

  @retval Pointer to the next available position in the buffer.
**/
CHAR8 *
EFIAPI
AddSmbiosTypeString (
  OUT    CHAR8                 *Dest,
  IN     UINT8                 Type,
  IN     UINT8                 Index,
  IN     CHAR8                 *String
  );

/**
  Load and initialize customized SMBIOS string data.

  This function allocates a buffer for SMBIOS strings, populates it with BIOS and baseboard information,
  and loads additional SMBIOS string data from a specified component. The buffer pointer is stored in a PCD
  for later use. The function ensures that the buffer is not overrun when copying additional data.

  @param[in]  ContainerSig   Signature identifying the container to load SMBIOS data from.
  @param[in]  ComponentName  Name of the component containing SMBIOS string data.

  @retval EFI_SUCCESS            The SMBIOS string data was loaded and initialized successfully.
  @retval EFI_OUT_OF_RESOURCES   Memory allocation for the SMBIOS string table failed.
  @retval Other                  Error returned by LoadComponent if loading SMBIOS data fails.
**/
EFI_STATUS
EFIAPI
LoadSmbiosStringsFromComponent (
  IN  UINT32    ContainerSig,
  IN  UINT32    ComponentName
  );

/**
  This function is called to initialize the SMBIOS tables.

  @retval       EFI_DEVICE_ERROR, if Smbios Entry is NULL
                Status after finalizing the Smbios init

**/
EFI_STATUS
EFIAPI
SmbiosInit (
  VOID
  );

#endif
