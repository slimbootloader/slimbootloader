/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __BOOT_LOADER_SERVICES_GUID_H__
#define __BOOT_LOADER_SERVICES_GUID_H__

///
/// Boot Loader Service GUID
///
extern EFI_GUID gBootLoaderServiceGuid;

typedef struct {
  UINT32                   Signature;
  UINT16                   Version;
  UINT16                   Reserved;
} SERVICE_COMMON_HEADER;

typedef struct {
  UINT32                   Count;
  SERVICE_COMMON_HEADER   *Header[0];
} SERVICES_LIST;

typedef struct {
  UINT8                    Revision;
  UINT8                    Reserved[3];
  SERVICES_LIST            ServiceList;
} BOOT_LOADER_SERVICES_LIST;

#endif
