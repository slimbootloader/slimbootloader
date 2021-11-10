/** @file

  Copyright (c) 2011 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __LINUX_LIB_H__
#define __LINUX_LIB_H__

#define BOOTSIG               0x1FE
#define SETUP_HDR             0x53726448  // "HdrS"

#define BOOT_PARAMS_BASE      0x00090000
#define LINUX_KERNEL_BASE     0x00100000
#define CMDLINE_OFFSET        0xF000
#define CMDLINE_LENGTH_MAX    0x800

#define E820_RAM              1
#define E820_RESERVED         2
#define E820_ACPI             3
#define E820_NVS              4
#define E820_UNUSABLE         5

#define VIDEO_TYPE_EFI        0x70
#define GET_POS_FROM_MASK(mask)   (mask & 0x0FF)?0:((mask & 0x0FF00)?8:((mask & 0x0FF0000)?16:24))

#define VIDEO_CAPABILITY_SKIP_QUIRKS  (1 << 0)
#define VIDEO_CAPABILITY_64BIT_BASE   (1 << 1)  /* Frame buffer base is 64-bit */

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
  UINT32    ExtLfbBase;        /* 0x3a */
  UINT8     Reserved[2];       /* 0x3e */
} SCREEN_INFO;

typedef struct {
  SCREEN_INFO   ScreenInfo;
  UINT8         ApmBiosInfo[0x14];
  UINT8         Pad2[4];
  UINT64        TbootAddr;
  UINT8         IstInfo[0x10];
  UINT64        AcpiRsdpAddr;
  UINT8         Pad3[8];
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
  UINT16        Limit;
  UINT64       *Base;
} DT_ADDR;

#pragma pack()

/**
  Dumps kernel boot parameters.

  @param[in] Bp    Pointer to boot parameter structure.
 **/
VOID
DumpLinuxBootParams (
  IN BOOT_PARAMS             *Bp
  );

/**
  Return kernel boot parameters.

  @retval    Pointer to boot parameter structure.
 **/
BOOT_PARAMS *
EFIAPI
GetLinuxBootParams (
  VOID
  );

/**
  Check if the image is a bootable Linux image.

  @param[in]  ImageBase      Memory address of an image

  @retval     TRUE           Image is a bootable kernel image
  @retval     FALSE          Not a bootable kernel image
**/
BOOLEAN
EFIAPI
IsBzImage (
  IN  CONST VOID             *ImageBase
  );

/**
  Load linux kernel image to specified address and setup boot parameters.

  @param[in]  KernelBase     Memory address of an kernel image.
  @param[in]  InitRdBase     Memory address of an InitRd image.
  @param[in]  InitRdLen      InitRd image size.
  @param[in]  CmdLineBase    Memory address of command line buffer.
  @param[in]  CmdLineLen     Command line buffer size.

  @retval EFI_INVALID_PARAMETER   Input parameters are not valid.
  @retval EFI_UNSUPPORTED         Unsupported binary type.
  @retval EFI_SUCCESS             Kernel is loaded successfully.
**/
EFI_STATUS
EFIAPI
LoadBzImage (
  IN  CONST VOID                  *KernelBase,
  IN  CONST VOID                  *InitRdBase,
  IN      UINT32                   InitRdLen,
  IN  CONST VOID                  *CmdLineBase,
  IN      UINT32                   CmdLineLen
  );

/**
  Update linux kernel boot parameters.

  @param[in]  Bp             BootParams address to be updated

**/
VOID
EFIAPI
UpdateLinuxBootParams (
  IN  BOOT_PARAMS            *Bp
  );

/**
  Load linux kernel image to specified address and setup boot parameters.

  @param[in]  HobList    HOB list pointer. Not used for now.
  @param[in]  Params     Extra parameters. Not used for now.
**/
VOID
EFIAPI
LinuxBoot (
  IN VOID   *HobList,
  IN VOID   *Params
  );

#endif
