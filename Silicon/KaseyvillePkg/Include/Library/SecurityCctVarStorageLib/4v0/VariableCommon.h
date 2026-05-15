/** @file
  Intel(R) SGX library.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CCT_VS_VAR_COMMON_H
#define _CCT_VS_VAR_COMMON_H

#include <Uefi.h>
#include <Uefi/UefiBaseType.h>

#define MAX_VAR_DESCRIPTOR_NAME_LEN  64

//
// Public data structures
//

typedef enum {
  CCT_VS_LIB_MODE_LEGACY_DIRECT,
  CCT_VS_LIB_MODE_DISPATCH,
  CCT_VS_LIB_MODE_API
} CCT_VS_LIB_MODE;

typedef enum {
  CCT_VS_VAR_VERSION_NONE = 0x00,
  CCT_VS_VAR_VERSION_5V0  = 0x10,
  CCT_VS_VAR_VERSION_5V1  = 0x11,
  CCT_VS_VAR_VERSION_5V2  = 0x12,
  CCT_VS_VAR_VERSION_MAX  = 0xFF
} CCT_VS_VAR_VERSION;

typedef enum {
  CCT_VS_VAR_DATA_INIT_PHASE_NONE     = 0x00,
  CCT_VS_VAR_DATA_INIT_PHASE_PRE_MEM  = 0x10,
  CCT_VS_VAR_DATA_INIT_PHASE_POST_MEM = 0x11,
  CCT_VS_VAR_DATA_INIT_PHASE_LATE     = 0x12,
  CCT_VS_VAR_DATA_INIT_PHASE_ANY      = 0xFE,
  CCT_VS_VAR_DATA_INIT_PHASE_MAX      = 0xFF
} CCT_VS_VAR_DATA_INIT_PHASE;

// Align to 2 bytes. Some of UEFI functions (f.ex. StrLen) require such aligment. Otherwise ASSERT may appear // Security: RestrictedContent
#pragma pack(2)

typedef struct {
  struct GLOBALLY_UNIQUE_IDENTITY_PART {
    EFI_GUID    DescriptorGuid;
    UINT32      HumanReadableVersion;
  } Identity;
  struct BIOS_OWNED_PART {
    CHAR16                        Name[MAX_VAR_DESCRIPTOR_NAME_LEN];
    EFI_GUID                      Guid;
    UINT32                        Attr;
    UINT64                        DataSize;
    CCT_VS_VAR_DATA_INIT_PHASE    Phase;
    BOOLEAN                       Available; // When TRUE shall be processed by consumer
    BOOLEAN                       IsPresent;
    BOOLEAN                       IsLocked;
  } Bios;
  struct BOOLOADER_OWNED_PART {
    UINT32    RetrievedAttr;
    UINT64    RetrievedDataSize;
  } Bootloader;
} CCT_VS_VAR_DESCRIPTOR;

typedef struct {
  EFI_GUID              ManifestVersion; // DescriptorGuid;
  UINT32                ArraySize;
  CCT_VS_VAR_DESCRIPTOR DescriptorArray[];
} CCT_VAR_MANIFEST;

#pragma pack()

#endif // _CCT_VS_VAR_COMMON_H
