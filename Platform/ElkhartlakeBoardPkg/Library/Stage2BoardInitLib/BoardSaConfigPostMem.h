/** @file
  PEI Boards Configurations for PostMem phase.

 Copyright (c) 2015 - 2020, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// DDI defines
//
typedef enum {
  DdiDisable       = 0x00,
  DdiDdcEnable     = 0x01,
} DDI_DDC_TBT_VAL;

typedef enum {
  DdiHpdDisable  = 0x00,
  DdiHpdEnable   = 0x01,
} DDI_HPD_VAL;

typedef enum {
  DdiPortADisabled = 0x00,
  DdiPortAEdp      = 0x01,
  DdiPortAMipiDsi  = 0x02,
  DdiPortADp       = 0x03,
} DDI_PORTA_SETTINGS;

typedef enum {
  DdiPortBDisabled = 0x00,
  DdiPortBDp       = 0x01,
  DdiPortBMipiDsi  = 0x02,
} DDI_PORTB_SETTINGS;

typedef enum {
  DdiPortCDisabled = 0x00,
  DdiPortCHdmi     = 0x01,
  DdiPortCDp       = 0x02,
} DDI_PORTC_SETTINGS;

//
// Display DDI settings for EHL RVP
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 mEhlRvpErbRowDisplayDdiConfig[17] = {
  DdiPortAEdp,     // DDI Port A Config : DdiPortADisabled = Disabled, DdiPortAEdp = eDP, DdiPortAMipiDsi = MIPI DSI, DdiPortADp = Dp
  DdiPortBDp,      // DDI Port B Config : DdiPortBDisabled = Disable, DdiPortBDp = Dp, DdiPortBMipiDsi = MIPI DSI
  DdiPortCHdmi,    // DDI Port C Config : DdiPortCDisabled = Disable, DdiPortCHdmi = Hdmi, DdiPortCDp = Dp
  DdiHpdDisable,   // DDI Port A HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdEnable,    // DDI Port B HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdEnable,    // DDI Port C HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdDisable,   // DDI Port 1 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdDisable,   // DDI Port 2 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdDisable,   // DDI Port 3 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdDisable,   // DDI Port 4 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiDisable,      // DDI Port A DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDisable,      // DDI Port B DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDdcEnable,    // DDI Port C DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDisable,      // DDI Port 1 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDisable,      // DDI Port 2 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDisable,      // DDI Port 3 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDisable       // DDI Port 4 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
};

//
// Display DDI settings for EHL Crb
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 mEhlCrbRowDisplayDdiConfig[17] = {
  DdiPortAEdp,     // DDI Port A Config : DdiPortADisabled = Disabled, DdiPortAEdp = eDP, DdiPortAMipiDsi = MIPI DSI, DdiPortADp = Dp
  DdiPortBDp,      // DDI Port B Config : DdiPortBDisabled = Disable, DdiPortBDp = Dp, DdiPortBMipiDsi = MIPI DSI
  DdiPortCHdmi,    // DDI Port C Config : DdiPortCDisabled = Disable, DdiPortCHdmi = Hdmi, DdiPortCDp = Dp
  DdiHpdDisable,   // DDI Port A HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdEnable,    // DDI Port B HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdEnable,    // DDI Port C HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdDisable,   // DDI Port 1 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdDisable,   // DDI Port 2 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdDisable,   // DDI Port 3 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiHpdDisable,   // DDI Port 4 HPD : DdiHpdDisable = Disable, DdiHpdEnable = Enable HPD
  DdiDisable,      // DDI Port A DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDdcEnable,    // DDI Port B DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDdcEnable,    // DDI Port C DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDisable,      // DDI Port 1 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDisable,      // DDI Port 2 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDisable,      // DDI Port 3 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
  DdiDisable       // DDI Port 4 DDC : DdiDisable = Disable, DdiDdcEnable = Enable DDC
};
