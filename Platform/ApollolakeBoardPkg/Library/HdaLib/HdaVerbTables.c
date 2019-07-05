/** @file
  HD Audio Verb Table.

  Copyright (c) 2014 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/HdaVerbTables.h>

HDAUDIO_VERB_TABLE HdaVerbTableAlc662 = {
  {
    0x10EC0662,     // Vendor ID / Device ID
    0xFF,           // Revision ID
    0xFF,           // SDI number, 0xFF matches any SDI.
    15 * 4          // Number of data DWORDs following the header.
  },
  {

    0x00172000,
    0x00172100,
    0x00172286,
    0x00172380,

    //
    // Pin Widget Verb-table
    // Widget node 0x01 :
    //
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,

    //
    // Pin widget 0x12 - DMIC
    //
    0x01271C00,
    0x01271D00,
    0x01271E13,
    0x01271F40,

    //
    // Pin widget 0x14 - FRONT (Port-D)
    //
    0x01471C10,
    0x01471D40,
    0x01471E01,
    0x01471F01,

    //
    // Pin widget 0x15 - SURR (Port-A)
    //
    0x01571C12,
    0x01571D10,
    0x01571E01,
    0x01571F01,

    //
    // Pin widget 0x16 - CEN/LFE (Port-G)
    //
    0x01671C11,
    0x01671D60,
    0x01671E01,
    0x01671F01,

    //
    // Pin widget 0x18 - MIC1 (Port-B)
    //
    0x01871C30,
    0x01871D90,
    0x01871EA1,
    0x01871F01,

    //
    // Pin widget 0x19 - MIC2 (Port-F)
    //
    0x01971C40,
    0x01971D90,
    0x01971EA1,
    0x01971F02,

    //
    // Pin widget 0x1A - LINE1 (Port-C)
    //
    0x01A71C3F,
    0x01A71D30,
    0x01A71E81,
    0x01A71F01,

    //
    // Pin widget 0x1B - LINE2 (Port-E)
    //
    0x01B71C1F,
    0x01B71D40,
    0x01B71E21,
    0x01B71F02,

    //
    // Pin widget 0x1C - CD-IN
    //
    0x01C71CF0,
    0x01C71D11,
    0x01C71E11,
    0x01C71F41,

    //
    // Pin widget 0x1D - BEEP-IN
    //
    0x01D71C01,
    0x01D71DE6,
    0x01D71E45,
    0x01D71F40,

    //
    // Pin widget 0x1E - S/PDIF-OUT
    //
    0x01E71C20,
    0x01E71D11,
    0x01E71E44,
    0x01E71F01,

    //
    // Widget node 0x20 :
    //
    0x02050004,
    0x02040001,
    0x02050004,
    0x02040001,

    //
    // Widget node 0x20 - 1 :
    //
    0x02050005,
    0x02040080,
    0x02050001,
    0x0204A9B8
  }
};
