/** @file
  UI Setup common definitions - colors, key codes, shared types.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _UI_COMMON_H_
#define _UI_COMMON_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/GraphicsLib.h>
#include <Library/ConsoleInLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/VariableLib.h>
#include <Library/PcdLib.h>
#include "Include/UiDescriptor.h"

//
// Console dimensions
//
#define UI_MAX_COLS  240
#define UI_MAX_ROWS  56

//
// Color indices (matching mColors[] in GraphicsLib)
//
#define COLOR_BLACK           0x00
#define COLOR_BLUE            0x01
#define COLOR_GREEN           0x02
#define COLOR_CYAN            0x03
#define COLOR_RED             0x04
#define COLOR_MAGENTA         0x05
#define COLOR_BROWN           0x06
#define COLOR_LIGHT_GRAY      0x07
#define COLOR_DARK_GRAY       0x08
#define COLOR_LIGHT_BLUE      0x09
#define COLOR_LIGHT_GREEN     0x0A
#define COLOR_LIGHT_CYAN      0x0B
#define COLOR_LIGHT_RED       0x0C
#define COLOR_LIGHT_MAGENTA   0x0D
#define COLOR_YELLOW          0x0E
#define COLOR_WHITE           0x0F

//
// Make attribute from FG/BG: bit 11:8 = FG, bit 15:12 = BG
//
#define MAKE_ATTR(Fg, Bg)  (UINT8)(((Bg) << 4) | (Fg))

//
// UI Color scheme attributes
//
#define ATTR_TITLE       MAKE_ATTR(COLOR_YELLOW, COLOR_BLUE)
#define ATTR_BORDER      MAKE_ATTR(COLOR_LIGHT_CYAN, COLOR_BLACK)
#define ATTR_PAGE        MAKE_ATTR(COLOR_WHITE, COLOR_DARK_GRAY)
#define ATTR_PAGE_SEL    MAKE_ATTR(COLOR_WHITE, COLOR_BLUE)
#define ATTR_PAGE_PARENT MAKE_ATTR(COLOR_YELLOW, COLOR_DARK_GRAY)
#define ATTR_PAGE_LEAF   MAKE_ATTR(COLOR_WHITE, COLOR_DARK_GRAY)
#define ATTR_PAGE_PARENT_SEL MAKE_ATTR(COLOR_WHITE, COLOR_BLUE)
#define ATTR_PAGE_LEAF_SEL   MAKE_ATTR(COLOR_YELLOW, COLOR_BLUE)
#define ATTR_PAGE_PARENT_SEL_INACTIVE MAKE_ATTR(COLOR_LIGHT_CYAN, COLOR_DARK_GRAY)
#define ATTR_PAGE_LEAF_SEL_INACTIVE   MAKE_ATTR(COLOR_LIGHT_CYAN, COLOR_DARK_GRAY)
#define ATTR_FIELD_NAME  MAKE_ATTR(COLOR_LIGHT_GRAY, COLOR_DARK_GRAY)
#define ATTR_FIELD_VAL   MAKE_ATTR(COLOR_LIGHT_GRAY, COLOR_DARK_GRAY)
#define ATTR_FIELD_SEL   MAKE_ATTR(COLOR_BLACK, COLOR_CYAN)
#define ATTR_FIELD_SEL_INACTIVE MAKE_ATTR(COLOR_LIGHT_GRAY, COLOR_DARK_GRAY)
#define ATTR_HELP        MAKE_ATTR(COLOR_YELLOW, COLOR_DARK_GRAY)
#define ATTR_LEGEND      MAKE_ATTR(COLOR_CYAN, COLOR_BLACK)
#define ATTR_POPUP       MAKE_ATTR(COLOR_WHITE, COLOR_BLUE)
#define ATTR_POPUP_SEL   MAKE_ATTR(COLOR_YELLOW, COLOR_LIGHT_BLUE)
#define ATTR_MODIFIED    MAKE_ATTR(COLOR_YELLOW, COLOR_BLACK)

//
// Box-drawing characters (from SBL glyph table)
//
#define BOX_ULCORNER   0xC9
#define BOX_URCORNER   0xBB
#define BOX_LLCORNER   0xC8
#define BOX_LRCORNER   0xBC
#define BOX_HLINE      0xCD
#define BOX_VLINE      0xBA
#define BOX_LTEE       0xCC
#define BOX_RTEE       0xB9
#define BOX_TTEE       0xCB
#define BOX_BTEE       0xCA
#define BOX_CROSS      0xCE

//
// Single-line box chars
//
#define BOX_S_ULCORNER 0xDA
#define BOX_S_URCORNER 0xBF
#define BOX_S_LLCORNER 0xC0
#define BOX_S_LRCORNER 0xD9
#define BOX_S_HLINE    0xC4
#define BOX_S_VLINE    0xB3

//
// UI Key codes (returned by InputGetKey)
//
#define UI_KEY_NONE    0x0000
#define UI_KEY_UP      0x0100
#define UI_KEY_DOWN    0x0101
#define UI_KEY_LEFT    0x0102
#define UI_KEY_RIGHT   0x0103
#define UI_KEY_HOME    0x0104
#define UI_KEY_END     0x0105
#define UI_KEY_PGUP    0x0106
#define UI_KEY_PGDN    0x0107
#define UI_KEY_ENTER   0x000D
#define UI_KEY_ESC     0x001B
#define UI_KEY_TAB     0x0009
#define UI_KEY_BKSP    0x0008
#define UI_KEY_F1      0x0111
#define UI_KEY_F2      0x0112
#define UI_KEY_F9      0x0119
#define UI_KEY_F10     0x011A
#define UI_KEY_QUIT    0x0120  // Direct exit from anywhere

//
// UI layout constants
//
#define UI_PAGE_PANEL_WIDTH    28
#define UI_HELP_ROWS           4
#define UI_LEGEND_ROWS         3
#define UI_TITLE_ROWS          2
#define UI_FIELD_VALUE_WIDTH   20

//
// Setup states
//
typedef enum {
  UI_STATE_INIT = 0,
  UI_STATE_MAIN_MENU,
  UI_STATE_PAGE,
  UI_STATE_EDIT_COMBO,
  UI_STATE_EDIT_NUM,
  UI_STATE_CONFIRM_SAVE,
  UI_STATE_LOAD_DEFAULTS,
  UI_STATE_EXIT
} UI_STATE;

//
// Config delta format for variable storage
//
#define UI_CFG_DELTA_SIGNATURE   SIGNATURE_32('C','D','L','T')
#define UI_CFG_DELTA_VERSION     1
#define UI_CFG_DELTA_MAX_ENTRIES 50
#define UI_CFG_DELTA_VAR_NAME    L"CfgDelta"

#pragma pack(1)

typedef struct {
  UINT16  TagId;
  UINT16  BitOffset;
  UINT16  BitLength;
  UINT16  Reserved;
  UINT32  Value;
} UI_CFG_DELTA_ENTRY;

typedef struct {
  UINT32             Signature;
  UINT8              Version;
  UINT8              EntryCount;
  UINT16             Reserved;
  UI_CFG_DELTA_ENTRY Entries[0];
} UI_CFG_DELTA_HDR;

#pragma pack()

#endif // _UI_COMMON_H_
