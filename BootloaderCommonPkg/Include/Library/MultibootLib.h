/** @file
  Copyright (c) 2005, 2006 The NetBSD Foundation, Inc.
  All rights reserved.

  This code is derived from software contributed to The NetBSD Foundation
   by Julio M. Merino Vidal.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 **/

/*        [ORIGIN: src/sys/arch/i386/include/...                                */
/*        $NetBSD: multiboot.h,v 1.8 2009/02/22 18:05:42 ahoka Exp $        */

#ifndef __MULTIBOOT_LIB_H__
#define __MULTIBOOT_LIB_H__

#include <Library/IasImageLib.h>

#define KB_(n)  ((UINT32) (n) * 1024)
#define MB_(n)  ((UINT32) (n) * 1024 * 1024)

typedef VOID *P_ADDR;
typedef VOID *V_ADDR;

/*
 *  Machine state when jumping to a IA32 boot image:
 */
typedef struct {
  UINT32  EntryPoint;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Esi;
  UINT32  Edi;
} IA32_BOOT_STATE;

/*
 * Multiboot header structure.
 */
#define MULTIBOOT_HEADER_MAGIC          0x1BADB002
#define MULTIBOOT_HEADER_MODS_ALIGNED   0x00000001
#define MULTIBOOT_HEADER_WANT_MEMORY    0x00000002
#define MULTIBOOT_HEADER_HAS_VBE        0x00000004
#define MULTIBOOT_HEADER_HAS_ADDR       0x00010000

/* Indicator for ACPI binary blob inside IAS image */
#define MULTIBOOT_SPECIAL_MODULE_MAGIC  0xacb10086

typedef struct {
  UINT32  Magic;
  UINT32  Flags;
  UINT32  Checksum;

  /* Valid if mh_flags sets MULTIBOOT_HEADER_HAS_ADDR. */
  P_ADDR  HeaderAddr;
  P_ADDR  LoadAddr;
  P_ADDR  LoadEndAddr;
  P_ADDR  BssEndAddr;
  P_ADDR  EntryAddr;

  /* Valid if mh_flags sets MULTIBOOT_HEADER_HAS_VBE. */
  UINT32  ModeType;
  UINT32  Width;
  UINT32  Height;
  UINT32  Depth;
} MULTIBOOT_HEADER;

/*
 * Multiboot information structure.
 */
#define MULTIBOOT_INFO_MAGIC            0x2BADB002
#define MULTIBOOT_INFO_HAS_MEMORY       0x00000001
#define MULTIBOOT_INFO_HAS_BOOT_DEVICE  0x00000002
#define MULTIBOOT_INFO_HAS_CMDLINE      0x00000004
#define MULTIBOOT_INFO_HAS_MODS         0x00000008
#define MULTIBOOT_INFO_HAS_AOUT_SYMS    0x00000010
#define MULTIBOOT_INFO_HAS_ELF_SYMS     0x00000020
#define MULTIBOOT_INFO_HAS_MMAP         0x00000040
#define MULTIBOOT_INFO_HAS_DRIVES       0x00000080
#define MULTIBOOT_INFO_HAS_CONFIG_TABLE 0x00000100
#define MULTIBOOT_INFO_HAS_LOADER_NAME  0x00000200
#define MULTIBOOT_INFO_HAS_APM_TABLE    0x00000400
#define MULTIBOOT_INFO_HAS_VBE          0x00000800
#define MULTIBOOT_INFO_HAS_FB           0x00001000

typedef struct {
  UINT32  Flags;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_MEMORY. */
  UINT32  MemLower;
  UINT32  MemUpper;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_BOOT_DEVICE. */
  UINT8   BootDevicePart3;
  UINT8   BootDevicePart2;
  UINT8   BootDevicePart1;
  UINT8   BootDeviceDrive;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_CMDLINE. */
  UINT8  *Cmdline;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_MODS. */
  UINT32  ModsCount;
  V_ADDR  ModsAddr;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_{AOUT,ELF}_SYMS. */
  UINT32  ElfshdrNum;
  UINT32  ElfshdrSize;
  V_ADDR  ElfshdrAddr;
  UINT32  ElfshdrShndx;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_MMAP. */
  UINT32  MmapLength;
  V_ADDR  MmapAddr;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_DRIVES. */
  UINT32  DrivesLength;
  V_ADDR  DrivesAddr;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_CONFIG_TABLE. */
  VOID   *UnusedConfigTable;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_LOADER_NAME. */
  UINT8  *LoaderName;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_APM. */
  VOID   *UnusedApmTable;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_VBE. */
  VOID   *UnusedVbeControlInfo;
  VOID   *UnusedVbeModeInfo;
  UINT16  UnusedVbeMode;
  UINT16  UnusedVbeInterfaceSeg;
  UINT16  UnusedVbeInterfaceOff;
  UINT16  UnusedVbeInterfaceLen;

  /* Valid if mi_flags sets MULTIBOOT_INFO_HAS_FB. */
  UINT64  FramebufferAddr;
  UINT32  FramebufferPitch;
  UINT32  FramebufferWidth;
  UINT32  FramebufferHeight;
  UINT8   FramebufferBpp;
  UINT8   FramebufferType;
  UINT8   FramebufferRedFieldPosition;
  UINT8   FramebufferRedMaskSize;
  UINT8   FramebufferGreenFieldPosition;
  UINT8   FramebufferGreenMaskSize;
  UINT8   FramebufferBlueFieldPosition;
  UINT8   FramebufferBlueMaskSize;

} MULTIBOOT_INFO;

/*
 * Drive information.  This describes an entry in the drives table as
 * pointed to by mi_drives_addr.
 */
typedef struct {
  UINT32  Length;
  UINT8   Number;
  UINT8   Mode;
  UINT16  Cylinders;
  UINT8   Heads;
  UINT8   Sectors;

  /* The variable-sized 'ports' field comes here, so this structure
   * can be longer. */
} MULTIBOOT_DRIVE;

/*
 * Memory mapping.  This describes an entry in the memory mappings table
 * as pointed to by mi_mmap_addr.
 *
 * Be aware that mm_size specifies the size of all other fields *except*
 * for mm_size.  In order to jump between two different entries, you
 * have to count mm_size + 4 bytes.
 */
#pragma pack (1)
typedef struct {
  UINT32  Size;
  UINT64  BaseAddr;
  UINT64  Length;
  UINT32  Type;
} MULTIBOOT_MMAP;
#pragma pack()

/*
 * Modules. This describes an entry in the modules table as pointed
 * to by mi_mods_addr.
 */
typedef struct {
  UINT32  Start;
  UINT32  End;
  UINT8  *String;
  UINT32  Reserved;
} MULTIBOOT_MODULE;

typedef struct {
  IMAGE_DATA  CmdFile;
  IMAGE_DATA  ImgFile;
} MULTIBOOT_MODULE_DATA;

typedef struct {
  IMAGE_DATA              VmmBootParams;
  IMAGE_DATA              VmmHeapAddr;
  IMAGE_DATA              VmmRuntimeAddr;
} VMM_IMAGE_DATA;

typedef struct {
  IMAGE_DATA              Base;
  IMAGE_DATA              SeedList;
  IMAGE_DATA              PlatformInfo;
} BOOTPARAMS_IMAGE_DATA;

typedef struct {
  VMM_IMAGE_DATA          VmmImageData;
  BOOTPARAMS_IMAGE_DATA   BootParamsData;
} TRUSTY_IMAGE_DATA;

typedef struct multiboot2_start_tag MULTIBOOT2_START_TAG;  /* opaque type */

typedef struct {
  MULTIBOOT2_START_TAG   *StartTag;
} MULTIBOOT2_INFO;

#define MAX_MULTIBOOT_MODULE_NUMBER  16
typedef struct {
  IMAGE_DATA              BootFile;
  IMAGE_DATA              CmdFile;
  MULTIBOOT_INFO          MbInfo;
  MULTIBOOT2_INFO         Mb2Info;
  IA32_BOOT_STATE         BootState;
  UINT16                  CmdBufferSize;
  UINT16                  MbModuleNumber;
  MULTIBOOT_MODULE        MbModule[MAX_MULTIBOOT_MODULE_NUMBER];
  MULTIBOOT_MODULE_DATA   MbModuleData[MAX_MULTIBOOT_MODULE_NUMBER];
  TRUSTY_IMAGE_DATA       TrustyImageData;
} MULTIBOOT_IMAGE;

/**
  Check if it is Multiboot image

  @param[in]  ImageAddr    Memory address of an image

  @retval TRUE             Image is Multiboot 0.6.98 compliant image
  @retval FALSE            Not multiboot image
**/
BOOLEAN
EFIAPI
IsMultiboot (
  IN  VOID                   *ImageAddr
  );

/**
  Setup Multiboot image and its boot info.

  @param[in,out] MultiBoot   Point to loaded Multiboot image structure

  @retval  RETURN_SUCCESS    Setup Multiboot image successfully
  @retval  Others            There is error when setup image
**/
EFI_STATUS
EFIAPI
SetupMultibootImage (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  );

/**
  Setup the Multiboot info for boot usage.

  @param[in,out]   MultiBoot  Point to loaded Multiboot image structure
**/
VOID
EFIAPI
SetupMultibootInfo (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  );

/**
  ASM inline function that goes from payload to a Multiboot enabled OS.
  @param[in]  State  Boot state structure
 **/
VOID
EFIAPI
JumpToMultibootOs (
  IN IA32_BOOT_STATE *State  // esp + 4
  );

/* ======================================================================== */
/**
  Check if it is Multiboot-2 image

  @param[in]  ImageAddr    Memory address of an image

  @retval TRUE             Image is Multiboot 2.0 compliant image
  @retval FALSE            Not multiboot image
**/
BOOLEAN
EFIAPI
IsMultiboot2 (
  IN  VOID                   *ImageAddr
  );

/**
  Setup Multiboot-2 image and its boot info.

  @param[in,out] MultiBoot   Point to loaded Multiboot-2 image structure

  @retval  RETURN_SUCCESS    Setup Multiboot-2 image successfully
  @retval  Others            There is error when setup image
**/
EFI_STATUS
EFIAPI
SetupMultiboot2Image (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  );

/**
  Setup the Multiboot-2 info for boot usage.

  @param[in,out]   MultiBoot  Point to loaded Multiboot-2 image structure
**/
VOID
EFIAPI
SetupMultiboot2Info (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  );

/**
  Update the memory info inside the Multiboot-2 info.

  @param[in,out]   MultiBoot     Point to loaded Multiboot-2 image structure
  @param[in]       RsvdMemBase   Reserved memory base address
  @param[in]       RsvdMemSize   Reserved memory size
  @param[in]       RsvdMemExtra  Extra space to add to the reserved memory region.
**/
VOID
EFIAPI
UpdateMultiboot2MemInfo (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot,
  IN UINT64                   RsvdMemBase,
  IN UINT64                   RsvdMemSize,
  IN UINT32                   RsvdMemExtra
  );

#endif
