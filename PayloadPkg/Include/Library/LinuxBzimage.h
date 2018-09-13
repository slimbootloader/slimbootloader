/** @file

  Copyright (c) 2011 - 2013, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __LINUX_BZIMAGE_H__
#define __LINUX_BZIMAGE_H__

#define BOOTSIG         0x1FE
#define SETUP_HDR       0x53726448  /* 0x53726448 == "HdrS" */

#define E820_RAM        1
#define E820_RESERVED   2
#define E820_ACPI       3
#define E820_NVS        4
#define E820_UNUSABLE   5

#define VIDEO_TYPE_EFI  0x70  /* EFI graphic mode   */

#pragma pack(1)

typedef struct {
  UINT8     SetupSectorss;   /* Sectors for setup code */
  UINT16    RootFlags;
  UINT32    SysSize;
  UINT16    RamSize;
  UINT16    VideoMode;
  UINT16    RootDev;
  UINT16    Signature;   /* Boot Signature */
  UINT16    Jump;
  UINT32    Header;
  UINT16    Version;
  UINT16    SuSwitch;
  UINT16    SetupSeg;
  UINT16    StartSys;
  UINT16    KernalVer;
  UINT8     LoaderId;
  UINT8     LoadFlags;
  UINT16    MoveSize;
  UINT32    Code32Start;    /* Start of code loaded high */
  UINT32    RamDiskStart;   /* Start of initial ramdisk */
  UINT32    RamDisklen; /* Length of initial ramdisk */
  UINT32    BootsectorKludge;
  UINT16    HeapEnd;
  UINT8     ExtLoaderVer;  /* Extended boot loader version */
  UINT8     ExtLoaderType; /* Extended boot loader ID */
  UINT32    CmdLinePtr;   /* 32-bit pointer to the kernel command line */
  UINT32    RamDiskMax;    /* Highest legal initrd address */
  UINT32    KernelAlignment; /* Physical addr alignment required for kernel */
  UINT8     RelocatableKernel; /* Whether kernel is relocatable or not */
  UINT8     MinAlignment;
  UINT16    XloadFlags;
  UINT32    CmdlineSize;
  UINT32    HardwareSubarch;
  UINT64    HardwareSubarchData;
  UINT32    PayloadOffset;
  UINT32    PayloadLength;
  UINT64    SetupData;
  UINT64    PrefAddress;
  UINT32    InitSize;
  UINT32    HandoverOffset;
} SETUP_HEADER;

typedef struct {
  UINT32    EfiLoaderSignature;
  UINT32    EfiSystab;
  UINT32    EfiMemdescSize;
  UINT32    EfiMemdescVersion;
  UINT32    EfiMemmap;
  UINT32    EfiMemmapSize;
  UINT32    EfiSystabHi;
  UINT32    EfiMemmapHi;
} EFI_INFO;

typedef struct {
  UINT64    Addr;        /* start of memory segment */
  UINT64    Size;        /* size of memory segment */
  UINT32    Type;        /* Type of memory segment */
} E820_ENTRY;

typedef struct {
  UINT8     OrigX;           /* 0x00 */
  UINT8     OrigY;           /* 0x01 */
  UINT16    ExtMemK;         /* 0x02 */
  UINT16    OrigVideoPage;   /* 0x04 */
  UINT8     OrigVideoMode;   /* 0x06 */
  UINT8     OrigVideoCols;   /* 0x07 */
  UINT8     Flags;           /* 0x08 */
  UINT8     Unused2;         /* 0x09 */
  UINT16    OrigVideoEgaBx;  /* 0x0a */
  UINT16    Unused3;         /* 0x0c */
  UINT8     OrigVideoLines;  /* 0x0e */
  UINT8     OrigVideoIsVGA;  /* 0x0f */
  UINT16    OrigVideoPoints; /* 0x10 */

  /* VESA graphic mode -- linear frame buffer */
  UINT16    LfbWidth;          /* 0x12 */
  UINT16    LfbHeight;         /* 0x14 */
  UINT16    LfbDepth;          /* 0x16 */
  UINT32    LfbBase;           /* 0x18 */
  UINT32    LfbSize;           /* 0x1c */
  UINT16    ClMagic, ClOffset; /* 0x20 */
  UINT16    LfbLinelength;     /* 0x24 */
  UINT8     RedSize;           /* 0x26 */
  UINT8     RedPos;            /* 0x27 */
  UINT8     GreenSize;         /* 0x28 */
  UINT8     GreenPos;          /* 0x29 */
  UINT8     BlueSize;          /* 0x2a */
  UINT8     BluePos;           /* 0x2b */
  UINT8     RsvdSize;          /* 0x2c */
  UINT8     RsvdPos;           /* 0x2d */
  UINT16    VesapmSeg;         /* 0x2e */
  UINT16    VesapmOff;         /* 0x30 */
  UINT16    Pages;             /* 0x32 */
  UINT16    VesaAttributes;    /* 0x34 */
  UINT32    Capabilities;      /* 0x36 */
  UINT8     Reserved[6];       /* 0x3a */
} SCREEN_INFO;

typedef struct {
  SCREEN_INFO   ScreenInfo;
  UINT8         ApmBiosInfo[0x14];
  UINT8         Pad2[4];
  UINT64        TbootAddr;
  UINT8         IstInfo[0x10];
  UINT8         Pad3[16];
  UINT8         Hd0Info[16];
  UINT8         Hd1Info[16];
  UINT8         SysDescTable[0x10];
  UINT8         OlpcOfwHeader[0x10];
  UINT8         Pad4[128];
  UINT8         EdidInfo[0x80];
  EFI_INFO      EfiInfo;
  UINT32        AltMemk;
  UINT32        Scratch;
  UINT8         E820Entries;
  UINT8         EddbufEntries;
  UINT8         EddMbrSigBufEntries;
  UINT8         Pad6[6];
  SETUP_HEADER  Hdr;
  UINT8         Pad7[0x290 - 0x1f1 - sizeof (SETUP_HEADER)];
  UINT32        EddMbrSigBuffer[16];
  E820_ENTRY    E820Map[128];
  UINT8         Pad8[48];
  UINT8         Eddbuf[0x1ec];
  UINT8         Pad9[276];
} BOOT_PARAMS;

typedef struct {
  UINT16    Limit;
  UINT64    *Base;
} DT_ADDR;

#pragma pack()

/**
  Setup graphics.

  @param Buf  Pointer to boot parameter structure.
 **/
extern EFI_STATUS
setup_graphics (
  BOOT_PARAMS *Buf
  );

#endif /* __LINUX_BZIMAGE_H__ */
