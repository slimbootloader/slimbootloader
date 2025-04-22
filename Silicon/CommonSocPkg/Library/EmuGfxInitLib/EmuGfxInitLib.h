/** @file
  This file contains QEMU graphics controller register info.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _QEMU_VIDEO_H_
#define _QEMU_VIDEO_H_

#define QEMU_VGA_VID_DID                 0x11111234
#define QEMU_VGA_VID_DID2                0x11114321

#define VBE_DISPI_IOPORT_INDEX           0x01CE
#define VBE_DISPI_IOPORT_DATA            0x01D0

#define ATT_ADDRESS_REGISTER             0x3C0

#define VBE_DISPI_INDEX_ID               0x0
#define VBE_DISPI_INDEX_XRES             0x1
#define VBE_DISPI_INDEX_YRES             0x2
#define VBE_DISPI_INDEX_BPP              0x3
#define VBE_DISPI_INDEX_ENABLE           0x4
#define VBE_DISPI_INDEX_BANK             0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH       0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT      0x7
#define VBE_DISPI_INDEX_X_OFFSET         0x8
#define VBE_DISPI_INDEX_Y_OFFSET         0x9
#define VBE_DISPI_INDEX_VIDEO_MEMORY_64K 0xa

#define VBE_DISPI_DISABLED               0x00
#define VBE_DISPI_ENABLED                0x01
#define VBE_DISPI_GETCAPS                0x02
#define VBE_DISPI_8BIT_DAC               0x20
#define VBE_DISPI_LFB_ENABLED            0x40
#define VBE_DISPI_NOCLEARMEM             0x80

#define GRAPHICS_DATA_SIG    SIGNATURE_32 ('Q', 'G', 'F', 'X')

typedef struct {
  VOID*      LogoPtr;
  UINT32     LogoSize;
  VOID*      GraphicsConfigPtr;
} GRAPHICS_CONFIG;

typedef struct {
  UINT64                                 Signature;
  UINT32                                 Bar2;
} QEMU_VIDEO_PRIVATE_DATA;

typedef struct {
  UINT32    ResX;
  UINT32    ResY;
} QEMU_VIDEO_BOCHS_MODES;

#endif
