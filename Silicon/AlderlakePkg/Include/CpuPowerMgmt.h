/** @file
  This file contains define definitions specific to processor


  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _POWER_MGMT_DEFINITIONS_H_
#define _POWER_MGMT_DEFINITIONS_H_

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
#define PPM_OC_UNLOCKED         0x1000000  ///< Bit 24: 1= Overclocking fully uncloked
#define PPM_TURBO_BOOST_MAX     0x2000000  ///< Bit 25: 1= Intel Turbo Boost Max Technology 3.0.
#else
#define PPM_EIST                BIT0   ///< Enhanced Intel Speed Step Technology.
#define PPM_C1                  BIT1   ///< C1 enabled, supported.
#define PPM_C1E                 BIT2   ///< C1E enabled.
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
#define PPM_OC_UNLOCKED         BIT24  ///< 1= Overclocking fully unlocked
#define PPM_TURBO_BOOST_MAX     BIT25  ///< 1= Intel Turbo Boost Max Technology 3.0
#endif

#define PPM_C_STATES            0x72    ///< PPM_C1 + PPM_C6 + PPM_C7 + PPM_C7S


//
// The following definitions are based on assumed location for the  ACPI
// Base Address.  Modify as necessary base on platform-specific requirements.
//
#define PCH_ACPI_PBLK 0x1810
#define PCH_ACPI_LV3  0x1815
#define PCH_ACPI_LV4  0x1816
#define PCH_ACPI_LV6  0x1818
#define PCH_ACPI_LV5  0x1817
#define PCH_ACPI_LV7  0x1819

//
// C-State Latency (us):
//  Though _CST object is core level, it is recommended that System BIOS report the
//  Package level C state latencies, so that Operating System can account for worst case
//  latency before using a specific C state. Unlike legacy platforms,
//  BIOS is no longer required to program the C_STATE_LATENCYCONTROL_n MSRs (0x60B, 0x60C, 0x633, 0x634, 0x635).
//
#define C1_LATENCY                        1
#define C6_LATENCY                        127
#define C7_LATENCY                        253
#define C8_LATENCY                        260
#define C9_LATENCY                        487
#define C10_LATENCY                       1048

//
// C-State Power (mW)
//
#define C1_POWER                          0x3E8
#define C6_POWER                          0x15E
#define C7_POWER                          0xC8
#define C8_POWER                          0xC8
#define C9_POWER                          0xC8
#define C10_POWER                         0xC8

#ifndef AUTO
#define AUTO                              0
#endif
#define END_OF_TABLE                      0xFF


///
/// Limit the number of P-states for different generation OS.
///
#define LPSS_FVID_MAX_STATES                   16


#define SVID_REGISTER_VR_ID_VIRTUAL_0  4
#define SVID_REGISTER_VR_ID_VIRTUAL_1  5

#define SVID_EXCLUSIVE_DISABLE  0


#endif
