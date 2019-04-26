/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
  Return the pointer to the Smbios table spcified by 'Type'

  @param[in]  Type    Smbios type requested

  @retval             Pointer to the starting of the requested Smbios type
                      NULL, otherwise

**/
VOID *
EFIAPI
FindSmbiosType (
  IN  UINT8   Type
  );

/**
  Append an Smbios Type to an existing set of types
  Different from AddSmbiosType, which gets called during initial Smbios setup

  @param[in]  TypeData    pointer to the Type Data including strings
  @param[in]  TypeLength  Size of type data including strings and end terminator (0000)

  @retval                         EFI_DEVICE_ERROR, if Smbios Entry is NULL
                                  Overflow status for adding this type or Type 127, otherwise

**/
EFI_STATUS
EFIAPI
AppendSmbiosType (
  IN  VOID      *TypeData,
  IN  UINT16    TypeLength
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