/** @file

  Copyright (c) 1999 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:
  IgdOpRegionDefinesMtl.h
Abstract:
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system BIOS, ASL code, and Graphics drivers.
  Supporting Specification: OpRegion / Software SCI SPEC 0.70
  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)
**/

#ifndef _IGD_OP_REGION_DEFINES_H_
#define _IGD_OP_REGION_DEFINES_H_


#define HEADER_SIGNATURE    "IntelGraphicsMem"
#define HEADER_SIZE         0x2000
#define HEADER_OPREGION_VER 0x0300
#define HEADER_OPREGION_REV 0x00
#define IGD_OPREGION_VERSION  2

//
//For VLV Tablet, MailBOX2(SCI)is not supported.
//
#define HEADER_MBOX_SUPPORT (HD_MBOX5 + HD_MBOX4 + HD_MBOX3 + HD_MBOX2 + HD_MBOX1)
#define HD_MBOX1            BIT0
#define HD_MBOX2            BIT5
#define HD_MBOX3            BIT2
#define HD_MBOX4            BIT3
#define HD_MBOX5            BIT4


//
// OpRegion Mailbox 1 EQUates.
//
// OpRegion Mailbox 3 EQUates.
//
#define BACKLIGHT_BRIGHTNESS  0xFF
#define FIELD_VALID_BIT       BIT31
#define INIT_BRIGHT_LEVEL     0x64


#define IGD_BUS                  0x00
#define IGD_DEV                  0x02
#define IGD_FUN_0                0x00


#define IGD_SWSCI_OFFSET    0x00E8      //Software SCI 0xE0 2
#define IGD_ASLS_OFFSET     0x00FC      // ASL Storage


#define VBT_SIGNATURE           SIGNATURE_32 ('$', 'V', 'B', 'T')

//
// Typedef stuctures
//
#pragma pack (1)
typedef struct {
  UINT8   HeaderSignature[20];
  UINT16  HeaderVersion;
  UINT16  HeaderSize;
  UINT16  HeaderVbtSize;
  UINT8   HeaderVbtCheckSum;
  UINT8   HeaderReserved;
  UINT32  HeaderOffsetVbtDataBlock;
  UINT32  HeaderOffsetAim1;
  UINT32  HeaderOffsetAim2;
  UINT32  HeaderOffsetAim3;
  UINT32  HeaderOffsetAim4;
  UINT8   DataHeaderSignature[16];
  UINT16  DataHeaderVersion;
  UINT16  DataHeaderSize;
  UINT16  DataHeaderDataBlockSize;
  UINT8   CoreBlockId;
  UINT16  CoreBlockSize;
  UINT16  CoreBlockBiosSize;
  UINT8   CoreBlockBiosType;
  UINT8   CoreBlockReleaseStatus;
  UINT8   CoreBlockHWSupported;
  UINT8   CoreBlockIntegratedHW;
  UINT8   CoreBlockBiosBuild[4];
  UINT8   CoreBlockBiosSignOn[155];
} VBIOS_VBT_STRUCTURE;
#pragma pack ()

#endif
