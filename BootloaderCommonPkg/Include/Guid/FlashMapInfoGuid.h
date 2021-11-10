/** @file
  This file defines the hob structure for the Flash Map entries.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FLASH_MAP_INFO_GUID_H__
#define __FLASH_MAP_INFO_GUID_H__

#include <Uefi/UefiBaseType.h>

extern EFI_GUID gFlashMapInfoGuid;

#define FLASH_MAP_ADDRESS       0xFFFFFFF8
#define FLASH_MAP_HEADER_SIZE   16

#define FLASH_MAP_SIG_HEADER       SIGNATURE_32 ('F', 'L', 'M', 'P')
#define FLASH_MAP_SIG_STAGE1A      SIGNATURE_32 ('S', 'G', '1', 'A')
#define FLASH_MAP_SIG_STAGE1B      SIGNATURE_32 ('S', 'G', '1', 'B')
#define FLASH_MAP_SIG_STAGE2       SIGNATURE_32 ('S', 'G', '0', '2')
#define FLASH_MAP_SIG_ACM          SIGNATURE_32 ('A', 'C', 'M', '0')
#define FLASH_MAP_SIG_UCODE        SIGNATURE_32 ('U', 'C', 'O', 'D')
#define FLASH_MAP_SIG_MRCDATA      SIGNATURE_32 ('M', 'R', 'C', 'D')
#define FLASH_MAP_SIG_VARIABLE     SIGNATURE_32 ('V', 'A', 'R', 'S')
#define FLASH_MAP_SIG_UEFIVARIABLE SIGNATURE_32 ('U', 'V', 'A', 'R')
#define FLASH_MAP_SIG_PAYLOAD      SIGNATURE_32 ('P', 'Y', 'L', 'D')
#define FLASH_MAP_SIG_EPAYLOAD     SIGNATURE_32 ('E', 'P', 'L', 'D')
#define FLASH_MAP_SIG_SPI_IAS1     SIGNATURE_32 ('I', 'A', 'S', '1')
#define FLASH_MAP_SIG_SPI_IAS2     SIGNATURE_32 ('I', 'A', 'S', '2')
#define FLASH_MAP_SIG_FWUPDATE     SIGNATURE_32 ('F', 'W', 'U', 'P')
#define FLASH_MAP_SIG_CFGDATA      SIGNATURE_32 ('C', 'N', 'F', 'G')
#define FLASH_MAP_SIG_BLRESERVED   SIGNATURE_32 ('R', 'S', 'V', 'D')
#define FLASH_MAP_SIG_EMPTY        SIGNATURE_32 ('E', 'M', 'T', 'Y')
#define FLASH_MAP_SIG_UNKNOWN      SIGNATURE_32 ('U', 'N', 'K', 'N')
#define FLASH_MAP_SIG_BPM          SIGNATURE_32 ('_', 'B', 'P', 'M')

#define FLASH_MAP_FLAGS_TOP_SWAP               0x00000001
#define FLASH_MAP_FLAGS_REDUNDANT_REGION       0x00000002
#define FLASH_MAP_FLAGS_NON_REDUNDANT_REGION   0x00000004
#define FLASH_MAP_FLAGS_NON_VOLATILE_REGION    0x00000008
#define FLASH_MAP_FLAGS_COMPRESSED             0x00000010
#define FLASH_MAP_FLAGS_BACKUP                 0x00000040

#define FLASH_MAP_ATTRIBUTES_PRIMARY_REGION    0x00000000
#define FLASH_MAP_ATTRIBUTES_BACKUP_REGION     0x00000001

#pragma pack(1)
typedef struct {
  UINT32    Signature;
  UINT32    Flags;
  UINT32    Offset;
  UINT32    Size;
} FLASH_MAP_ENTRY_DESC;

typedef struct {
  UINT32                Signature;
  UINT16                Version;
  UINT16                Length;
  UINT8                 Attributes;
  UINT8                 Reserved[3];
  UINT32                RomSize;
  FLASH_MAP_ENTRY_DESC  EntryDesc[];
} FLASH_MAP;
#pragma pack()

#endif
