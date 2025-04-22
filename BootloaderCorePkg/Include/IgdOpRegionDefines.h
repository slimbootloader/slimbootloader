/** @file

  Copyright (c) 1999 - 2014, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:
  IgdOpRegionDefines.h
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

//
// OpRegion (Miscellaneous) #defines.
//
// OpRegion Header #defines.
//

#define HEADER_SIGNATURE    "IntelGraphicsMem"
#define HEADER_SIZE         0x2000
#define HEADER_OPREGION_VER 0x0200
#define HEADER_OPREGION_VER_GEN9  0x0200
#define HEADER_OPREGION_VER_GEN12 0x0201
#define HEADER_OPREGION_REV 0x00
#define IGD_OPREGION_VERSION  2

//
//For VLV Tablet, MailBOX2(SCI)is not supported.
//
#define HEADER_MBOX_SUPPORT (HD_MBOX4 + HD_MBOX3 + HD_MBOX1)
#define HD_MBOX1            BIT0
#define HD_MBOX2            BIT1
#define HD_MBOX3            BIT2
#define HD_MBOX4            BIT3
#define HD_MBOX5            BIT4
#define SVER_SIZE           32

//
//Audio Type support for VLV2 A0
//
#define AUDIO_TYPE_SUPPORT_MASK    0xFFFFFFF3
#define NO_AUDIO_SUPPORT           (0<<2)
#define HD_AUDIO_SUPPORT           (1<<2)
#define LPE_AUDIO_SUPPORT          (2<<2)
#define AUDIO_TYPE_FIELD_MASK      0xFFFFFFEF
#define AUDIO_TYPE_FIELD_VALID     (1<<4)
#define AUDIO_TYPE_FIELD_INVALID   (0<<4)

//
// OpRegion Mailbox 1 EQUates.
//
// OpRegion Mailbox 3 EQUates.
//
#define ALS_ENABLE            BIT0
#define BLC_ENABLE            BIT1
#define BACKLIGHT_BRIGHTNESS  0xFF
#define FIELD_VALID_BIT       BIT31
#define WORD_FIELD_VALID_BIT  BIT15
#define PFIT_ENABLE           BIT2
#define PFIT_OPRN_AUTO        0x00000000
#define PFIT_OPRN_SCALING     0x00000007
#define PFIT_OPRN_OFF         0x00000000
#define PFIT_SETUP_AUTO       0
#define PFIT_SETUP_SCALING    1
#define PFIT_SETUP_OFF        2
#define INIT_BRIGHT_LEVEL     0x64
#define PFIT_STRETCH          6
#define PFIT_CENTER           1

//
// GT RELATED EQUATES
//
#define GTT_MEM_ALIGN        22
#define GTTMMADR_SIZE_4MB    0x400000

#define IGD_BUS                  0x00
#define IGD_DEV                  0x02
#define IGD_FUN_0                0x00

#define IGD_R_VID                0x00
#define IGD_R_CMD                0x04
#define IGD_R_GTTMMADR           0x10

#define IGD_R_BGSM               0x70
#define LOCKBIT                  BIT0

#define IGD_VID             0x8086
#define IGD_DID             0xA001
#define IGD_MGGC_OFFSET     0x0050      //GMCH Graphics Control Register 0x50
#define IGD_BSM_OFFSET      0x005C      //Base of Stolen Memory
#define IGD_SWSCI_OFFSET    0x00E0      //Software SCI 0xE0 2
#define IGD_ASLE_OFFSET     0x00E4      //System Display Event Register 0xE4 4
#define IGD_ASLS_OFFSET     0x00FC      // ASL Storage

//
// Video BIOS / VBT #defines
//
#define IGD_DID_VLV             0x0F31
#define OPTION_ROM_SIGNATURE    0xAA55
#define VBIOS_LOCATION_PRIMARY  0xC0000

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
