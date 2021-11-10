/** @file
  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SSC_REG_H_
#define _SSC_REG_H_

// selection ID of the SSC selection table
typedef enum {
  No_SSC=0,
  M01_SSC,
  M02_SSC,
  M03_SSC,
  M04_SSC,
  M05_SSC
} SSC_SELECTION_TABLE;

// selection ID of the Clock bending selection table
typedef enum {
  Clk_Bending_13=0,
  Clk_Bending_06,
  No_Clk_Bending,
  Clk_Bending_M09
} CLK_BENDING_SELECTION_TABLE;

#define SSC_DEFAULT_SETTING          M05_SSC
#define CLK_BENDING_DEFAULT_SETTING  No_Clk_Bending

typedef struct _SSC_SETTING {
  UINT8 Ssc_Selection_ID;
  UINT32 Ssc_Cyc_To_Peak;
  UINT32 Ffs_Frac_Step;
} SSC_SETTING;

typedef struct _CLOCK_BENDING_SETTING {
  UINT8 Clk_Bending_Selection_ID;
  UINT32 Pll_Ratio_Frac;
  UINT32 Pll_Ratio_Int;
} CLOCK_BENDING_SETTING;

#define SSC_ENABLE         0x1
#define SSC_DISABLE        0x0

typedef union _LJ1PLL_CR_RW_CONTROL_1 {
  UINT32  Data;
  struct {
    UINT32  ssc_en_ovr : 1;        ///< [0] SSC enable override
    UINT32  ssc_en : 1;          ///< [1] SSC Enable
    UINT32  ssc_b_cycles : 5;         ///< [6:2] ssc_b_cycles
    UINT32  ssc_b_step : 10;         ///< [16:7] ssc_b_step
    UINT32  ssc_a_cycles : 5;         ///< [21:17] ssc_a_cycles
    UINT32  ssc_a_step : 10;         ///< [31:22] ssc_a_step
  } Fields;
} LJ1PLL_CR_RW_CONTROL_1;

typedef union _LJ1PLL_CR_RW_CONTROL_2 {
  UINT32  Data;
  struct {
    UINT32  ssc_cyc_to_peak_m1 : 9;        ///< [8:0] cycle to peak
    UINT32  ssc_mode : 2;          ///< [10:9] SSC Mode
    UINT32  spare : 1;         ///< [11] Spare
    UINT32  ssc_frac_step : 20;         ///< [31:12] fractional step configuration
  } Fields;
} LJ1PLL_CR_RW_CONTROL_2;

typedef union _LJ1PLL_CR_RW_CONTROL_5 {
  UINT32  Data;
  struct {
    UINT32  pll_ratio_frac : 24;        ///< [23:0] Clock Bending: fractional frequency multiplier
    UINT32  pll_ratio_int : 8;          ///< [31:24] Clock Bending: integer frequency multiplier
  } Fields;
} LJ1PLL_CR_RW_CONTROL_5;

//The IPC command  buffer for SSC
typedef struct _SSC_IPC_BUFFER {
  LJ1PLL_CR_RW_CONTROL_1 LJ1PLL_CTRL_1;
  LJ1PLL_CR_RW_CONTROL_2 LJ1PLL_CTRL_2;
  UINT32                 LJ1PLL_CTRL_3;
  LJ1PLL_CR_RW_CONTROL_5 LJ1PLL_CTRL_5;
} SSC_IPC_BUFFER;

typedef union _LCPLL_CR_RW_CONTROL_1 {
    UINT32  Data;
    struct {
        UINT32  ssc_en_ovr : 1;        /// [0] SSC enable override
        UINT32  ssc_en : 1;            /// [1] SSC Enable
        UINT32  RSVD : 30;             /// [31:2] Reserved
    } Fields;
} LCPLL_CR_RW_CONTROL_1;

typedef union _LCPLL_CR_RW_CONTROL_2 {
    UINT32  Data;
    struct {
        UINT32  ssc_cyc_to_peak_m1 : 9;     /// [8:0] cycle to peak
        UINT32  ssc_mode : 2;               /// [10:9] SSC Mode
        UINT32  spare : 1;                  /// [11] Spare
        UINT32  ssc_frac_step : 20;         /// [31:12] fractional step configuration
    } Fields;
} LCPLL_CR_RW_CONTROL_2;

typedef union _LCPLL_CR_RW_CONTROL_5 {
    UINT32  Data;
    struct {
        UINT32  pll_ratio_frac : 24;        /// [23:0] Clock Bending: fractional frequency multiplier
        UINT32  pll_ratio_int : 8;          /// [31:24] Clock Bending: integer frequency multiplier
    } Fields;
} LCPLL_CR_RW_CONTROL_5;

#endif
