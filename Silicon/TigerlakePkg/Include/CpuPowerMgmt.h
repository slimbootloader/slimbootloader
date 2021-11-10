/** @file
  This file contains define definitions specific to processor

  Copyright (c) 1999 - 2019, Intel Corporation. All rights reserved.<BR>
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

#define PPM_C_STATES            0x7A    ///< PPM_C1 + PPM_C3 + PPM_C6 + PPM_C7 + PPM_C7S

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
// C-State Latency (us) and Power (mW) for C1
//
#define C1_LATENCY                        1
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
/// For Desktop, default PL1 time window value is 8 second
///
#define DT_POWER_LIMIT1_TIME_DEFAULT  8


#define VR_IMON_SLOPE_OFFSET              16

#define SVID_REGISTER_VR_ID_VIRTUAL_0  4
#define SVID_REGISTER_VR_ID_VIRTUAL_1  5

#define SVID_EXCLUSIVE_DISABLE  0

#define ELIXIR_PRODUCT_REV_MAILBOX_ADDRESS  2


//24MHz XTAL Clock Frequency
#define N_RFI_FREQ_LO_PF2_OFFSET                 2
#define N_RFI_FREQ_HI_PF2_OFFSET                 16
#define B_RFI_FREQ_PF2_MASK                      (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT3 | BIT2)


#define N_RFI_FREQ_LO_PF2_OFFSET                 2
#define N_RFI_FREQ_HI_PF2_OFFSET                 16
#define B_RFI_FREQ_PF2_MASK                      (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT3 | BIT2)

///
/// Used to identify the CPU used for programming with the VR override table
///
typedef enum {
  EnumUnknownCpuId             = 0,
  EnumMinCpuId                 = 1,
  ///
  /// TGL-Y
  ///
  EnumTglY9WattES1NoExsCpuId   = 0x1,
  EnumTglY9Watt42fNoExsCpuId   = 0x2,
  EnumTglY9WattES1ExsCpuId     = 0x3,
  EnumTglY9Watt42fExsCpuId     = 0x4,
  EnumTglYMaxCpuId             = EnumTglY9Watt42fExsCpuId,
  ///
  /// TGL-U
  ///
  EnumTglU15WattES1NoExsCpuId  = 0x5,
  EnumTglU15Watt42fNoExsCpuId  = 0x6,
  EnumTglU28WattES1NoExsCpuId  = 0x7,
  EnumTglU28Watt42fNoExsCpuId  = 0x8,
  EnumTglU28Watt42fExsCpuId    = 0x9,
  EnumTglU28WattES1ExsCpuId    = 0xA,
  EnumTglU15WattES1ExsCpuId    = 0xB,
  EnumTglU15Watt42fExsCpuId    = 0xC,
  EnumTglUMaxCpuId             = EnumTglU15Watt42fExsCpuId,
  ///
  /// TGL-H
  ///
  EnumTglH45Watt81ExsCpuId     = 0xD,
  EnumTglH45Watt81NoExsCpuId   = 0xE,
  EnumTglH65Watt81ExsCpuId     = 0xF,
  EnumTglH65Watt81NoExsCpuId   = 0x10,
  EnumTglH95Watt81ExsCpuId     = 0x11,
  EnumTglH95Watt81NoExsCpuId   = 0x12,
  EnumTglHMaxCpuId             = EnumTglH95Watt81NoExsCpuId,
  ///
  /// WHL
  ///
  EnumWhlU15Watt42fCpuId       = 0x20,
  EnumWhlU15Watt22fCpuId       = 0x1E,
  EnumWhlU15Watt2f1fCpuId      = 0x21,
  EnumWhlUMaxCpuId             = EnumWhlU15Watt2f1fCpuId,

  EnumMaxCpuId                 = EnumWhlUMaxCpuId,
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

#pragma pack (1)
typedef struct {
  UINT8     NameOp;           // 12h ;First opcode is a NameOp.
  UINT8     PackageLead;      // 20h ;First opcode is a NameOp.
  UINT8     NumEntries;       // 06h ;First opcode is a NameOp.
  UINT8     DwordPrefix1;     // 0Ch
  UINT32    CoreFrequency;    // 00h
  UINT8     DwordPrefix2;     // 0Ch
  UINT32    Power;            // 00h
  UINT8     DwordPrefix3;     // 0Ch
  UINT32    TransLatency;     // 00h
  UINT8     DwordPrefix4;     // 0Ch
  UINT32    BmLatency;        // 00h
  UINT8     DwordPrefix5;     // 0Ch
  UINT32    Control;          // 00h
  UINT8     DwordPrefix6;     // 0Ch
  UINT32    Status;           // 00h
} PSS_PACKAGE_LAYOUT;
#pragma pack()

#endif
