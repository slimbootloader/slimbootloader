/** @file
  This file contains define definitions specific to processor

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _POWER_MGMT_DEFINITIONS_H_
#define _POWER_MGMT_DEFINITIONS_H_

#define CSTATE_SUPPORTED          0x1
#define ENHANCED_CSTATE_SUPPORTED 0x2
#define C6_C7_SHORT_LATENCY_SUPPORTED 0x01
#define C6_C7_LONG_LATENCY_SUPPORTED  0x02
#define C7s_SHORT_LATENCY_SUPPORTED   0x03
#define C7s_LONG_LATENCY_SUPPORTED    0x04

#define NATIVE_PSTATE_LATENCY           10
#define PSTATE_BM_LATENCY               10

//
// Voltage offset definitions
//
#define OC_LIB_OFFSET_ADAPTIVE  0
#define OC_LIB_OFFSET_OVERRIDE  1
//
// Platform Power Management Flags Bit Definitions:
//   These defines are also used in CPU0CST.ASL to check platform configuration
//   and build C-state table accordingly.
//
#ifdef __GNUC__
//
// @todo Add workaround for GCC build, In GCC build the BIT can not be changed to value by GCC (ex: BIT0 can not be changed to 0x1)
//
#define PPM_EIST                0x1        ///< BIT 0 : Enhanced Intel Speed Step Technology.
#define PPM_C1                  0x2        ///< BIT 1 : C1 enabled, supported.
#define PPM_C1E                 0x4        ///< BIT 2 : C1E enabled.
#ifdef CPU_CFL
#define PPM_C3                  0x8        ///< BIT 3 : C3 enabled, supported.
#endif
#define PPM_C6                  0x10       ///< BIT 4 : C6 enabled, supported.
#define PPM_C7                  0x20       ///< BIT 5 : C7 enabled, supported.
#define PPM_C7S                 0x40       ///< BIT 6 : C7S enabled, supported
#define PPM_TM                  0x80       ///< BIT 7 : Adaptive Thermal Monitor.
#define PPM_TURBO               0x100      ///< BIT 8 : Long duration turbo mode
#define PPM_CMP                 0x200      ///< BIT 9 : CMP.
#define PPM_TSTATES             0x400      ///< BIT 10: CPU throttling states
#define PPM_MWAIT_EXT           0x800      ///< BIT 11: MONITIOR/MWAIT Extensions supported.
#define PPM_EEPST               0x1000     ///< BIT 12: Energy efficient P-State Feature enabled
#define PPM_TSTATE_FINE_GRAINED 0x2000     ///< BIT 13: Fine grained CPU Throttling states
#define PPM_CD                  0x4000     ///< BIT 14: Deep Cstate - C8/C9/C10
#define PPM_TIMED_MWAIT         0x8000     ///< BIT 15: Timed Mwait support
#define C6_LONG_LATENCY_ENABLE  0x10000    ///< BIT 16: 1=C6 Long and Short,0=C6 Short only
#define C7_LONG_LATENCY_ENABLE  0x20000    ///< BIT 17: 1=C7 Long and Short,0=C7 Short only
#define C7s_LONG_LATENCY_ENABLE 0x40000    ///< BIT 18: 1=C7s Long and Short,0=C7s Short only
#define PPM_C8                  0x80000    ///< Bit 19: 1= C8 enabled/supported
#define PPM_C9                  0x100000   ///< Bit 20: 1= C9 enabled/supported
#define PPM_C10                 0x200000   ///< Bit 21: 1= C10 enabled/supported
#define PPM_HWP                 0x400000   ///< Bit 22: 1= HWP enabled/supported
#define PPM_HWP_LVT             0x800000   ///< Bit 23: 1= HWP LVT enabled/supported
#define PPM_OC_UNLOCKED         0x1000000  ///< Bit 24: 1= Overclocking fully uncloked
#define PPM_TURBO_BOOST_MAX     0x2000000  ///< Bit 25: 1= Intel Turbo Boost Max Technology 3.0.
#else
#define PPM_EIST                BIT0   ///< Enhanced Intel Speed Step Technology.
#define PPM_C1                  BIT1   ///< C1 enabled, supported.
#define PPM_C1E                 BIT2   ///< C1E enabled.
#ifdef CPU_CFL
#define PPM_C3                  BIT3   ///< C3 enabled, supported.
#endif
#define PPM_C6                  BIT4   ///< C6 enabled, supported.
#define PPM_C7                  BIT5   ///< C7 enabled, supported.
#define PPM_C7S                 BIT6   ///< C7S enabled, supported
#define PPM_TM                  BIT7   ///< Adaptive Thermal Monitor.
#define PPM_TURBO               BIT8   ///< Long duration turbo mode
#define PPM_CMP                 BIT9   ///< CMP.
#define PPM_TSTATES             BIT10  ///< CPU throttling states
#define PPM_MWAIT_EXT           BIT11  ///< MONITIOR/MWAIT Extensions supported.
#define PPM_EEPST               BIT12  ///< Energy efficient P-State Feature enabled
#define PPM_TSTATE_FINE_GRAINED BIT13  ///< Fine grained CPU Throttling states
#define PPM_CD                  BIT14  ///< Deep Cstate - C8/C9/C10
#define PPM_TIMED_MWAIT         BIT15  ///< Timed Mwait support
#define C6_LONG_LATENCY_ENABLE  BIT16  ///< 1=C6 Long and Short,0=C6 Short only
#define C7_LONG_LATENCY_ENABLE  BIT17  ///< 1=C7 Long and Short,0=C7 Short only
#define C7s_LONG_LATENCY_ENABLE BIT18  ///< 1=C7s Long and Short,0=C7s Short only
#define PPM_C8                  BIT19  ///< 1= C8 enabled/supported
#define PPM_C9                  BIT20  ///< 1= C9 enabled/supported
#define PPM_C10                 BIT21  ///< 1= C10 enabled/supported
#define PPM_HWP                 BIT22  ///< 1= HWP enabled/supported
#define PPM_HWP_LVT             BIT23  ///< 1= HWP LVT enabled/supported
#define PPM_OC_UNLOCKED         BIT24  ///< 1= Overclocking fully unlocked
#define PPM_TURBO_BOOST_MAX     BIT25  ///< 1= Intel Turbo Boost Max Technology 3.0.
#endif
#ifndef CPU_CFL
#define PPM_C_STATES            0x72    ///< PPM_C1 + PPM_C6 + PPM_C7 + PPM_C7S
#else
#define PPM_C_STATES            0x7A    ///< PPM_C1 + PPM_C3 + PPM_C6 + PPM_C7 + PPM_C7S
#define C3_LATENCY              0x4E
#endif
#define C6_C7_SHORT_LATENCY     0x76
#define C6_C7_LONG_LATENCY      0x94
#define C8_LATENCY              0xFA
#define C9_LATENCY              0x14C
#define C10_LATENCY             0x3F2

#define FIVE_MS_RESPONSE_TIME   0x9D

//
// The following definitions are based on assumed location for the  ACPI
// Base Address.  Modify as necessary base on platform-specific requirements.
//
#define PCH_ACPI_PBLK 0x1810
#define PCH_ACPI_LV2  0x1814
#define PCH_ACPI_LV3  0x1815
#define PCH_ACPI_LV4  0x1816
#define PCH_ACPI_LV6  0x1818
#define PCH_ACPI_LV5  0x1817
#define PCH_ACPI_LV7  0x1819

//
// C-State Latency (us) and Power (mW) for C1
//
#define C1_LATENCY                        1
#define C1_POWER                          0x3E8
#define C3_POWER                          0x1F4
#define C6_POWER                          0x15E
#define C7_POWER                          0xC8
#define C8_POWER                          0xC8
#define C9_POWER                          0xC8
#define C10_POWER                         0xC8

#define MAX_POWER_LIMIT_1_TIME_IN_SECONDS 32767
#ifndef AUTO
#define AUTO                              0
#endif
#define END_OF_TABLE                      0xFF

#define CONFIG_TDP_DOWN                   1
#define CONFIG_TDP_UP                     2
#define CONFIG_TDP_DEACTIVATE             0xFF
//
// MMIO definitions
//
#define MMIO_DDR_RAPL_LIMIT     0x58e0
#define MMIO_TURBO_POWER_LIMIT  0x59A0

#define MAX_OVERCLOCKING_BINS   0x7

///
/// Default PL1 time window value is 28 seconds for all skus
///
#define POWER_LIMIT1_TIME_DEFAULT  28

#define PROCESSOR_FLAVOR_MOBILE       0x04
#define PROCESSOR_FLAVOR_DESKTOP      0x00
#define PROCESSOR_FLAVOR_MASK         (BIT3 | BIT2)

//
// Power definitions (Based on EMTS V1.0 for standard voltage 2.4-2.6 GHz dual-core parts.)
//
#define FVID_MAX_POWER      35000
#define FVID_TURBO_POWER    35000
#define FVID_SUPERLFM_POWER 12000
//
// Power definitions for LFM and Turbo mode TBD.
//
#define FVID_MIN_POWER  15000

///
/// Limit the number of P-states for different generation OS.
///
#define TPSS_FVID_MAX_STATES                   40
#define LPSS_FVID_MAX_STATES                   16

#define READ_PL1_DUTY_CYCLE_CLAMP_ENABLE  0x00000015
#define WRITE_PL1_DUTY_CYCLE_CLAMP_ENABLE 0x00000016

//
//  VR mailbox commands
//
#define READ_VR_STRAP_CONFIG_CMD          0x80000018
#define READ_ACDC_LOADLINE_CMD            0x80000118
#define WRITE_ACDC_LOADLINE_CMD           0x80000218
#define WRITE_PSI_CUTOFF_CMD              0x80000318
#define READ_PSI_CUTOFF_CMD               0x80001318
#define READ_IMON_CONFIG_CMD              0x80001418
#define WRITE_IMON_CONFIG_CMD             0x80000418
#define READ_VR_ICC_MAX_CMD               0x80000518
#define WRITE_VR_ICC_MAX_CMD              0x80000618
#define READ_VR_VOLTAGE_LIMIT_CMD         0x80000718
#define WRITE_VR_VOLTAGE_LIMIT_CMD        0x80000818
#define WRITE_PSYS_CONFIG_CMD             0x80000918
#define READ_PSYS_PMAX_CMD                0x80000B17
#define WRITE_PSYS_PMAX_CMD               0x80000B18
#define READ_VR_TDC_CONFIG_CMD            0x80000019
#define WRITE_VR_TDC_CONFIG_CMD           0x8000001A
#define WRITE_VOLTAGE_OPTIMIZATION_CMD    0x8000001F
#define READ_PLL_VOLTAGE_OFFSET_CMD       0x8000002C
#define WRITE_PLL_VOLTAGE_OFFSET_CMD      0x8000002D
#define WRITE_MPS_VR_IMPV8_CMD            0x80000E18
#define WRITE_PS4_EXIT_VR_CMD             0x80000034
#define WRITE_CPU_WAKEUP_TIMER_CMD        0x8000000C
#define WRITE_ISL_VR_MBX_CMD              0x80000012
#define ISL_VR_IA_GT_CMD_REQUESTED        0x01
#define ISL_VR_IA_GT_CMD_DATA             0x00
#define ISL_VR_IA_GT_SA_CMD_REQUESTED     0x02
#define ISL_VR_IA_GT_SA_CMD_DATA          0x01
#define MPS_VR_CMD_REQUESTED              0x01
#define PS4_EXIT_VR_CMD_REQUESTED         0x02

//
//  VR mailbox commands for Acoustic Noise Mitigation
//
#define WRITE_SVID_SET_VR_SLEW_RATE_CMD           0x80000C18
#define WRITE_SVID_SET_DISABLE_FAST_PKGC_RAMP_CMD 0x80000D18
#define READ_ACOUSTIC_MIGITATION_RANGE            0x80000007
#define WRITE_ACOUSTIC_MIGITATION_RANGE           0x80000008
#define NOISE_MIGITATION_RANGE_MASK               0xFF
#define RAMP_UP_OFFSET                            8
#define RAMP_DOWN_OFFSET                          16

#define VR_ADDRESS_MASK                   0xF
#define VR_ADDRESS_OFFSET                 16
#define VR_TDC_ADDRESS_OFFSET             8
#define DC_LOADLINE_OFFSET                16
#define AC_LOADLINE_MASK                  0xFFFF
#define DC_LOADLINE_MASK                  0xFFFF0000
#define AC_DC_LOADLINE_MAX                6249 ///< 62.49 mOhm max
#define PSI_THRESHOLD_MASK                0x3FF
#define PSI2_THRESHOLD_OFFSET_MASK        0xFFC00
#define PSI3_THRESHOLD_OFFSET_MASK        0x3FF00000
#define PSI2_THRESHOLD_OFFSET             10
#define PSI3_THRESHOLD_OFFSET             20
#define PSI3_ENABLE_OFFSET                30
#define PSI4_ENABLE_OFFSET                31
#ifdef CPU_CFL
#define VR_IMON_SLOPE_OFFSET              8
#define VR_IMON_OFFSET_HIGH_RES_OFFSET    18
#define VR_IMON_OFFSET_HIGH_RES_MASK      0xFFFC0000
#else
#define VR_IMON_SLOPE_OFFSET              16
#define VR_IMON_SLOPE_MASK                0xFFFF0000
#define VR_IMON_OFFSET_MASK               0xFFFF
#endif
#define VR_VOLTAGE_LIMIT_MASK             0xFFFF
#define PSYS_SLOPE_OFFSET                 8
#define VR_TDC_CURRENT_LIMIT_MASK         0x7FFF
#define VR_TDC_TIME_WINDOW_MASK           0x7F
#define VR_TDC_TIME_WINDOW_LOCATION_MASK  0xFE0000
#define VR_TDC_ENABLE_OFFSET              15
#define VR_TDC_TIME_WINDOW_OFFSET         17
#define VR_TDC_LOCK_OFFSET                31
#define VR_TDC_TIME_WINDOW_MAX            10

//
//  PID Tuning mailbox commands
//
#define WRITE_PID_RATL                    0x80000021
#define WRITE_PID_VR0_TDC                 0x81000021
#define WRITE_PID_VR1_TDC                 0x81010021
#define WRITE_PID_VR2_TDC                 0x81020021
#define WRITE_PID_VR3_TDC                 0x81030021
#define WRITE_PID_PSYS_PL1_MSR            0x82000021
#define WRITE_PID_PSYS_PL1_MMIO           0x82010021
#define WRITE_PID_PSYS_PL2_MSR            0x82020021
#define WRITE_PID_PSYS_PL2_MMIO           0x82030021
#define WRITE_PID_PKG_PL1_MSR             0x82040021
#define WRITE_PID_PKG_PL1_MMIO            0x82050021
#define WRITE_PID_PKG_PL2_MSR             0x82060021
#define WRITE_PID_PKG_PL2_MMIO            0x82070021
#define WRITE_PID_DDR_PL1_MSR             0x82080021
#define WRITE_PID_DDR_PL1_MMIO            0x82090021
#define WRITE_PID_DDR_PL2_MSR             0x820A0021
#define WRITE_PID_DDR_PL2_MMIO            0x820B0021
#define PID_DOMAIN_KP                     0
#define PID_DOMAIN_KI                     1
#define PID_DOMAIN_KD                     2
#define MAILBOX_PARAM_1_OFFSET            8

///
///  VR Domain Definitions
///
#define CPU_VR_DOMAIN_SA           0x0
#define CPU_VR_DOMAIN_IA           0x1
#define CPU_VR_DOMAIN_RING         0x2
#define CPU_VR_DOMAIN_GT           0x3
#define CPU_VR_DOMAIN_FIVR         0x4

///
///  RFI Definitions
///
#define CLOCK_FREQUENCY_19MHz                    19200000
#define CLOCK_FREQUENCY_24MHz                    24000000
#define R_EMI_CONTROL_0_0_0_MCHBAR_PCU           0x5A08
#define B_SPREAD_SPECTRUM_ENABLE                 BIT8
#define B_SPREAD_SPECTRUM_MASK                   0xFF
#define V_SPREAD_SPECTRUM_POINT_ONE_ENCODING_MAX 0x1C
#define V_FIVR_SPREAD_SPECTRUM_MAX               0x3F
#define R_RFI_CONTROL_0_0_0_MCHBAR_PCU           0x5A0C
#define B_RFI_FREQ_ADJ_MASK                      0xFF
#define V_RFI_FREQ_ADJ_MAX                       0x7F
#define B_RFI_FREQ_SPREAD_MASK                   0xFF00
#define V_MCIVR_SPREAD_SPECTRUM_MAX              8
#define B_RFI_FREQ_UPDATE_MASK                   0xFFFF
#define N_RFI_FREQ_LO_PF1_OFFSET                 0
#define N_RFI_FREQ_HI_PF1_OFFSET                 8
#define B_RFI_FREQ_PF1_MASK                      (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT1 | BIT0)
#define N_RFI_FREQ_LO_PF2_OFFSET                 2
#define N_RFI_FREQ_HI_PF2_OFFSET                 16
#define B_RFI_FREQ_PF2_MASK                      (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT3 | BIT2)
#define R_RFI_STATUS_0_0_0_MCHBAR_PCU            0x5A10
#define B_RFI_NOM_FREQ_MASK                      0x7FE
#define B_RFI_CURRENT_FREQ_MASK                  0xFFF
#define V_MAX_RFI_VALUE                          0x3FF

///
///  VR default override values
///

//
//  CNL-U
//
#define IA_AC_DC_LL_DEFAULT_CNL_U    280
#define GT_AC_DC_LL_DEFAULT_CNL_U    300
#define SA_AC_DC_LL_DEFAULT_CNL_U    960

#define IA_TDC_LIMIT_DEFAULT_CNL_U   176  // 22A
#define GT_TDC_LIMIT_DEFAULT_CNL_U   216  // 27A
#define SA_TDC_LIMIT_DEFAULT_CNL_U   92   // 11.5A

#define IA_ICC_MAX_DEFAULT_CNL_U     128  // 32A
#define GT_ICC_MAX_DEFAULT_CNL_U     148  // 37A
#define SA_ICC_MAX_DEFAULT_CNL_U     64   // 16A

//
//  CNL-Y
//
#define FIVR_AC_LL_DEFAULT_CNL_Y     650
#define FIVR_DC_LL_DEFAULT_CNL_Y     270

#define FIVR_TDC_LIMIT_DEFAULT_CNL_Y 160  // 20A

#define FIVR_ICC_MAX_DEFAULT_CNL_Y   116  // 29A

#define FIVR_VR_VOLTAGE_LIMIT_CNL_Y   1890 // 1.89 V

//
//  CFL-U 42
//
#define IA_AC_DC_LL_DEFAULT_CFL_U_4_2    240
#define GT_AC_DC_LL_DEFAULT_CFL_U_4_2    310
#define SA_AC_DC_LL_DEFAULT_CFL_U_4_2    1030

//
//  CFL-U 43e
//
#define IA_AC_DC_LL_DEFAULT_CFL_U_4_3e    240
#define GT_AC_DC_LL_DEFAULT_CFL_U_4_3e    200
#define SA_AC_DC_LL_DEFAULT_CFL_U_4_3e    1030

#define IA_TDC_LIMIT_DEFAULT_CFL_U_4_3e   352  // 44A
#define GT_TDC_LIMIT_DEFAULT_CFL_U_4_3e   304  // 38A

#define IA_ICC_MAX_DEFAULT_CFL_U_4_3e     256  // 64A
#define GT_ICC_MAX_DEFAULT_CFL_U_4_3e     256  // 64A
#define SA_ICC_MAX_DEFAULT_CFL_U_4_3e     34   // 8.5A

#define IA_TDC_LIMIT_DEFAULT_CFL_U_4_3e_TDP_28    360  // 45A

//
//  CFL-H 62
//
#define IA_AC_DC_LL_DEFAULT_CFL_H_6_2    180
#define GT_AC_DC_LL_DEFAULT_CFL_H_6_2    270
#define SA_AC_DC_LL_DEFAULT_CFL_H_6_2    1030

#define GT_TDC_LIMIT_DEFAULT_CFL_H_6_2   200  // 25A

#define IA_ICC_MAX_DEFAULT_CFL_H_6_2     512  // 128A
#define GT_ICC_MAX_DEFAULT_CFL_H_6_2     128  // 32A
#define SA_ICC_MAX_DEFAULT_CFL_H_6_2     44   // 11A

//
// 45 W part
//
#define IA_TDC_LIMIT_DEFAULT_CFL_H_6_2   640  // 80A

//
// 65 W part
//
#define IA_TDC_LIMIT_DEFAULT_CFL_H_6_2_TDP_65 728 // 91W

//
// CFL-H 82
//
#define IA_AC_DC_LL_DEFAULT_CFL_H_8_2    180
#define GT_AC_DC_LL_DEFAULT_CFL_H_8_2    270
#define SA_AC_DC_LL_DEFAULT_CFL_H_8_2    1030

#define IA_TDC_LIMIT_DEFAULT_CFL_H_8_2   688  // 86A
#define GT_TDC_LIMIT_DEFAULT_CFL_H_8_2   200  // 25A

#define IA_ICC_MAX_DEFAULT_CFL_H_8_2     560  // 140A
#define GT_ICC_MAX_DEFAULT_CFL_H_8_2     128  // 32A
#define SA_ICC_MAX_DEFAULT_CFL_H_8_2     44   // 11A


//
//  CFL-S 62
//
#define IA_AC_DC_LL_DEFAULT_CFL_S_6_2    210
#define GT_AC_DC_LL_DEFAULT_CFL_S_6_2    310
#define SA_AC_DC_LL_DEFAULT_CFL_S_6_2    1000

#define SA_TDC_LIMIT_DEFAULT_CFL_S_6_2   80 // 10A
//
// 35 W part
//
#define IA_TDC_LIMIT_DEFAULT_CFL_S_6_2_TDP_35   592  // 74A
#define GT_TDC_LIMIT_DEFAULT_CFL_S_6_2_TDP_35   200  // 25A

#define IA_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_35     416  // 104A
#define GT_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_35     140  // 35A
#define SA_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_35     44   // 11A

//
// 65 W part
//
#define IA_TDC_LIMIT_DEFAULT_CFL_S_6_2_TDP_65   728  // 91A
#define GT_TDC_LIMIT_DEFAULT_CFL_S_6_2_TDP_65   240  // 30A

#define IA_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_65     532  // 133A
#define GT_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_65     180  // 45A
#define SA_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_65     44   // 11A

//
// 80 W part
//
#define IA_TDC_LIMIT_DEFAULT_CFL_S_6_2_TDP_80   712  // 89A
#define GT_TDC_LIMIT_DEFAULT_CFL_S_6_2_TDP_80   240  // 30A

#define IA_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_80     532  // 133A
#define GT_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_80     180  // 45A
#define SA_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_80     44   // 11A

//
// 95 W part
//
#define IA_TDC_LIMIT_DEFAULT_CFL_S_6_2_TDP_95   800  // 100A
#define GT_TDC_LIMIT_DEFAULT_CFL_S_6_2_TDP_95   240  // 30A

#define IA_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_95     552  // 138A
#define GT_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_95     180  // 45A
#define SA_ICC_MAX_DEFAULT_CFL_S_6_2_TDP_95     44   // 11A

#define CPU_TDP_9_WATTS        900
#define CPU_TDP_12_WATTS       1200
#define CPU_TDP_15_WATTS       1500
#define CPU_TDP_28_WATTS       2800
#define CPU_TDP_35_WATTS       3500
#define CPU_TDP_45_WATTS       4500
#define CPU_TDP_54_WATTS       5400
#define CPU_TDP_58_WATTS       5800
#define CPU_TDP_62_WATTS       6200
#define CPU_TDP_65_WATTS       6500
#define CPU_TDP_71_WATTS       7100
#define CPU_TDP_83_WATTS       8300
#define CPU_TDP_80_WATTS       8000
#define CPU_TDP_91_WATTS       9100
#define CPU_TDP_95_WATTS       9500
#define CPU_TDP_120_WATTS      12000

//
//  CFL-X 62
//
#define IA_AC_DC_LL_DEFAULT_CFL_X_6_2    105

///
/// HDC Definitions
///
#define N_HDC_SUPPORT                 0
#define B_HDC_SUPPORT                 BIT0

///
/// CFL-H types
///
#define CFL_H_45_WATT_4_2       0x17
#define CFL_H_45_WATT_6_2       0x18
#define CFL_H_65_WATT_6_2       0x19
#define CFL_H_45_WATT_8_2       0x1F
#define CFL_H_65_WATT_8_2       0x20

///
/// CFL-S types
///
#define CFL_S_35_WATT_2_2       0x1
#define CFL_S_54_WATT_2_2       0x2
#define CFL_S_58_WATT_2_2       0x3
#define CFL_S_35_WATT_4_2       0x4
#define CFL_S_62_WATT_4_2       0x5
#define CFL_S_65_WATT_4_2       0x6
#define CFL_S_95_WATT_4_2       0x7
#define CFL_S_35_WATT_6_2       0x8
#define CFL_S_65_WATT_6_2       0x9
#define CFL_S_95_WATT_6_2       0xA
#define CFL_S_35_WATT_8_2       0xB
#define CFL_S_65_WATT_8_2       0xC
#define CFL_S_95_WATT_8_2       0xD
#define CFL_S_71_WATT_4_2       0xE   /// Workstation sku
#define CFL_S_80_WATT_6_2       0xF   /// Workstation sku
#define CFL_S_80_WATT_8_2       0x10  /// Workstation sku
#define CFL_S_83_WATT_4_2       0x25   /// Workstation sku

///
/// Used to identify the CPU used for programming with the VR override table
///
typedef enum {
  EnumUnknownCpuId       = 0,
  EnumMinCpuId           = 1,

  ///
  /// CFL-S
  ///
  EnumCflSMinCpuId       = CFL_S_35_WATT_2_2,
  EnumCflS35Watt22CpuId  = CFL_S_35_WATT_2_2,
  EnumCflS54Watt22CpuId  = CFL_S_54_WATT_2_2,
  EnumCflS58Watt22CpuId  = CFL_S_58_WATT_2_2,
  EnumCflS35Watt42CpuId  = CFL_S_35_WATT_4_2,
  EnumCflS62Watt42CpuId  = CFL_S_62_WATT_4_2,
  EnumCflS65Watt42CpuId  = CFL_S_65_WATT_4_2,
  EnumCflS95Watt42CpuId  = CFL_S_95_WATT_4_2,
  EnumCflS35Watt62CpuId  = CFL_S_35_WATT_6_2,
  EnumCflS65Watt62CpuId  = CFL_S_65_WATT_6_2,
  EnumCflS95Watt62CpuId  = CFL_S_95_WATT_6_2,
  EnumCflS35Watt82CpuId  = CFL_S_35_WATT_8_2,
  EnumCflS65Watt82CpuId  = CFL_S_65_WATT_8_2,
  EnumCflS95Watt82CpuId  = CFL_S_95_WATT_8_2,
  EnumCflS71Watt42CpuId  = CFL_S_71_WATT_4_2, /// Workstation sku
  EnumCflS83Watt42CpuId  = CFL_S_83_WATT_4_2, /// Workstation sku
  EnumCflS80Watt62CpuId  = CFL_S_80_WATT_6_2, /// Workstation sku
  EnumCflS80Watt82CpuId  = CFL_S_80_WATT_8_2, /// Workstation sku
  EnumCflSMaxCpuId       = EnumCflS80Watt82CpuId,

  ///
  /// CFL-U
  ///
  EnumCflU28Watt43eCpuId  = 0x11,
  EnumWhlU15Watt42fCpuId  = 0x12,
  EnumCflU15Watt42fCpuId  = 0x13,
  EnumCflU28Watt23eCpuId  = 0x24,
  EnumCflUR15Watt43eCpuId = 0x22,
  EnumCflUR28Watt43eCpuId = 0x23,
  EnumCflUMaxCpuId        = EnumCflU28Watt23eCpuId,

  ///
  /// CNL-U
  ///
  EnumCnlU15Watt22CpuId   = 0x14,
  EnumCnlU15Watt20CpuId   = 0x15,
  EnumCnlUMaxCpuId        = EnumCnlU15Watt20CpuId,

  ///
  /// CNL-Y
  ///
  EnumCnlY9Watt22CpuId    = 0x16,
  EnumCnlYMaxCpuId        = EnumCnlY9Watt22CpuId,

  ///
  /// CFL-H
  ///
  EnumCflH45Watt4f2CpuId  = CFL_H_45_WATT_4_2,
  EnumCflH45Watt62CpuId   = CFL_H_45_WATT_6_2,
  EnumCflH65Watt62CpuId   = CFL_H_65_WATT_6_2,
  EnumCflHMaxCpuId        = EnumCflH65Watt62CpuId,

  ///
  /// CNL-H
  ///
  EnumCnlH45Watt82CpuId   = 0x1A,
  EnumCnlH65Watt82CpuId   = 0x1B,
  EnumCnlH95Watt82CpuId   = 0x1C,
  EnumCnlH120Watt82CpuId  = 0x1D,
  EnumCnlHMaxCpuId        = EnumCnlH95Watt82CpuId,

  ///
  /// WHL
  ///
  EnumWhlU15Watt22fCpuId  = 0x1E,
  EnumWhlU15Watt2f1fCpuId = 0x21,
  ///
  /// CFL-H
  ///
  EnumCflH45Watt82CpuId   = CFL_H_45_WATT_8_2,
  EnumCflH65Watt82CpuId   = CFL_H_65_WATT_8_2,
  EnumMaxCpuId            = EnumCflH65Watt82CpuId,
} CPU_OVERRIDE_IDENTIFIER;

///
/// VR Override table structure
///
typedef struct {
  CPU_OVERRIDE_IDENTIFIER  CpuIdentifier;
  UINT16 IaIccMax;
  UINT16 GtIccMax;
  UINT16 SaIccMax;
  UINT16 VccInIccMax;
  UINT16 IaTdclimit;
  UINT16 GtTdclimit;
  UINT16 SaTdclimit;
  UINT16 VccInTdclimit;
  UINT16 IaAcLoadLine;
  UINT16 IaDcLoadLine;
  UINT16 GtAcLoadLine;
  UINT16 GtDcLoadLine;
  UINT16 SaAcLoadLine;
  UINT16 SaDcLoadLine;
  UINT16 VccInAcLoadLine;
  UINT16 VccInDcLoadLine;
  UINT16 VrVoltageLimit;
} CPU_VR_OVERRIDE_TABLE;

///
/// MSR_REGISTER definition as a Union of QWORDS, DWORDS and BYTES
///
typedef union _MSR_REGISTER {
  UINT64  Qword;       ///< MSR value in 64 bit QWORD.

  ///
  /// MSR value represented in two DWORDS
  ///
  struct {
    UINT32  Low;       ///< Lower DWORD of the 64 bit MSR value.
    UINT32  High;      ///< Higher DWORD of the 64 bit MSR value.
  } Dwords;

  ///
  /// MSR value represented in eight bytes.
  ///
  struct {
    UINT8 FirstByte;   ///< First byte of the 64 bit MSR value.
    UINT8 SecondByte;  ///< Second byte of the 64 bit MSR value.
    UINT8 ThirdByte;   ///< Third byte of the 64 bit MSR value.
    UINT8 FouthByte;   ///< Fourth byte of the 64 bit MSR value.
    UINT8 FifthByte;   ///< Fifth byte of the 64 bit MSR value.
    UINT8 SixthByte;   ///< Sixth byte of the 64 bit MSR value.
    UINT8 SeventhByte; ///< Seventh byte of the 64 bit MSR value.
    UINT8 EighthByte;  ///< Eigth byte of the 64 bit MSR value.
  } Bytes;
} MSR_REGISTER;
#pragma pack()

#endif

