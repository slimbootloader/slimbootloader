/** @file
  This file defines the hob structure used for boot loader version.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __BOOT_LOADER_VERSION_GUID_H__
#define __BOOT_LOADER_VERSION_GUID_H__


///
/// Boot Loader Version GUID
///
extern EFI_GUID gBootLoaderVersionGuid;

#define  BOOT_LOADER_VERINFO_SIGNATURE                SIGNATURE_32('$', 'S', 'B', 'H')

#pragma pack(1)
typedef struct {
  UINT16             BuildNumber;
  UINT8              ProjMinorVersion;
  UINT8              ProjMajorVersion;
  UINT8              CoreMinorVersion;
  UINT8              CoreMajorVersion;
  UINT8              SecureVerNum;
  UINT8              Reserved : 4;
  UINT8              ImageArch: 1;
  UINT8              BldDebug : 1;
  UINT8              FspDebug : 1;
  UINT8              Dirty    : 1;
} IMAGE_BUILD_INFO;

typedef struct {
  UINT32             Signature;
  UINT16             HeaderLength;
  UINT8              HeaderRevision;
  UINT8              Reserved;
  UINT64             ImageId;
  IMAGE_BUILD_INFO   ImageVersion;
  UINT64             SourceVersion;
} BOOT_LOADER_VERSION;

typedef struct {
  UINT8                Revision;
  UINT8                Reserved0[3];
  BOOT_LOADER_VERSION  Version;
} EXT_BOOT_LOADER_VERSION;

#pragma pack()


#endif
