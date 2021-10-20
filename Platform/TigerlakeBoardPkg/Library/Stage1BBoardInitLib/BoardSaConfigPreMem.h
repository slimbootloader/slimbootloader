/** @file
  PEI Boards Configurations for PreMem phase.

  Copyright (c) 2015 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _BOARD_SA_CONFIG_PRE_MEM_H_
#define _BOARD_SA_CONFIG_PRE_MEM_H_

//
// DDI defines
//
typedef enum {
  DdiDisable = 0x00,
  DdiDdcEnable = 0x01,
} DDI_DDC_TBT_VAL;

typedef enum {
  DdiHpdDisable = 0x00,
  DdiHpdEnable = 0x01,
} DDI_HPD_VAL;

typedef enum {
  DdiPortDisabled = 0x00,
  DdiPortEdp = 0x01,
  DdiPortAMipiDsi = 0x02,
} DDI_PORTA_SETTINGS;


//
// Display DDI settings for TGL-U DDR4
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 mTglUDdr4RowDisplayDdiConfig[16] = {
                                                                               DdiPortEdp,      // DDI Port A Config : DdiPortDisabled = No LFP is Connected, DdiPortEdp = eDP, DdiPortMipiDsi = MIPI DSI
                                                                               DdiPortDisabled, // DDI Port B Config : DdiPortDisabled = No LFP is Connected, DdiPortEdp = eDP, DdiPortMipiDsi = MIPI DSI
                                                                               DdiHpdDisable,   // DDI Port A HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdEnable,    // DDI Port B HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port C HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port 1 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port 2 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port 3 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port 4 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiDisable,      // DDI Port A DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDdcEnable,    // DDI Port B DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable,      // DDI Port C DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable,      // DDI Port 1 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable,      // DDI Port 2 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable,      // DDI Port 3 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable       // DDI Port 4 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
};

//
// Display DDI settings for TGL-U LP DDR4
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 mTglULpDdr4RowDisplayDdiConfig[16] = {
                                                                                DdiPortEdp,      // DDI Port A Config : DdiPortDisabled = No LFP is Connected, DdiPortEdp = eDP, DdiPortMipiDsi = MIPI DSI
                                                                                DdiPortDisabled, // DDI Port B Config : DdiPortDisabled = No LFP is Connected, DdiPortEdp = eDP, DdiPortMipiDsi = MIPI DSI
                                                                                DdiHpdDisable,   // DDI Port A HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                                DdiHpdEnable,    // DDI Port B HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                                DdiHpdDisable,   // DDI Port C HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                                DdiHpdDisable,   // DDI Port 1 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                                DdiHpdDisable,   // DDI Port 2 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                                DdiHpdDisable,   // DDI Port 3 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                                DdiHpdDisable,   // DDI Port 4 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                                DdiDisable,      // DDI Port A DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                                DdiDdcEnable,    // DDI Port B DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                                DdiDisable,      // DDI Port C DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                                DdiDisable,      // DDI Port 1 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                                DdiDisable,      // DDI Port 2 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                                DdiDisable,      // DDI Port 3 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                                DdiDisable       // DDI Port 4 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
};

//
// Display DDI settings for TGL-H DDR4
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 mTglHDdr4RowDisplayDdiConfig[16] = {
                                                                               DdiPortEdp,      // DDI Port A Config : DdiPortDisabled = No LFP is Connected, DdiPortEdp = eDP, DdiPortMipiDsi = MIPI DSI
                                                                               DdiPortDisabled, // DDI Port B Config : DdiPortDisabled = No LFP is Connected, DdiPortEdp = eDP, DdiPortMipiDsi = MIPI DSI
                                                                               DdiHpdDisable,   // DDI Port A HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdEnable,    // DDI Port B HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port C HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port 1 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port 2 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port 3 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiHpdDisable,   // DDI Port 4 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
                                                                               DdiDisable,      // DDI Port A DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDdcEnable,    // DDI Port B DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable,      // DDI Port C DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable,      // DDI Port 1 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable,      // DDI Port 2 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable,      // DDI Port 3 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
                                                                               DdiDisable       // DDI Port 4 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
};

#endif // _BOARD_SA_CONFIG_PRE_MEM_H_
