/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GLOBAL_NVS_AREA_H_
#define _GLOBAL_NVS_AREA_H_

//
// Includes
//
#define GLOBAL_NVS_DEVICE_ENABLE 1
#define GLOBAL_NVS_DEVICE_DISABLE 0

//
// Global NVS Area Protocol GUID
//
#define EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID \
{ 0x74e1e48, 0x8132, 0x47a1, 0x8c, 0x2c, 0x3f, 0x14, 0xad, 0x9a, 0x66, 0xdc }
//
// Revision id - Added TPM related fields
//
#define GLOBAL_NVS_AREA_RIVISION_1       1
//
// Extern the GUID for protocol users.
//
//extern EFI_GUID gEfiGlobalNvsAreaProtocolGuid;

///
/// Global NVS Area definition
///
#pragma pack (1)

typedef struct {
  UINT64 Base;
  UINT64 End;
  UINT64 Length;
} PCI_WINDOW;

typedef struct {
/**
  Miscellaneous Dynamic Values, the definitions below need to be matched
  GNVS definitions in Platform.ASL
**/
  PCI_WINDOW PciWindow32;
  PCI_WINDOW PciWindow64;

} EFI_GLOBAL_NVS_AREA;
#pragma pack ()

///
/// Global NVS Area Protocol
///
typedef struct _EFI_GLOBAL_NVS_AREA_PROTOCOL {
  EFI_GLOBAL_NVS_AREA     *Area;
} EFI_GLOBAL_NVS_AREA_PROTOCOL;

#endif
