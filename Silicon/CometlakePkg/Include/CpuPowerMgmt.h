/** @file
  This file contains define definitions specific to processor

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _POWER_MGMT_DEFINITIONS_H_
#define _POWER_MGMT_DEFINITIONS_H_


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
#ifdef CPU_CML
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
#ifdef CPU_CML
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
#ifndef CPU_CML
#define PPM_C_STATES            0x72    ///< PPM_C1 + PPM_C6 + PPM_C7 + PPM_C7S
#else
#define PPM_C_STATES            0x7A    ///< PPM_C1 + PPM_C3 + PPM_C6 + PPM_C7 + PPM_C7S
#endif


//
// The following definitions are based on assumed location for the  ACPI
// Base Address.  Modify as necessary base on platform-specific requirements.
//
#define PCH_ACPI_PBLK 0x1810
#define PCH_ACPI_LV2  0x1814
#define PCH_ACPI_LV3  0x1815
#define PCH_ACPI_LV4  0x1816

//
// C-State Latency (us) and Power (mW) for C1
//
#define C1_LATENCY                        1
#define C1_POWER                          0x3E8
#define C3_POWER                          0x1F4
#define C6_POWER                          0x15E
#define C7_POWER                          0xC8

#ifndef AUTO
#define AUTO                              0
#endif
#define END_OF_TABLE                      0xFF


///
/// Limit the number of P-states for different generation OS.
///
#define TPSS_FVID_MAX_STATES                   40
#define LPSS_FVID_MAX_STATES                   16


#ifdef CPU_CML
#define VR_IMON_SLOPE_OFFSET              8
#else
#define VR_IMON_SLOPE_OFFSET              16
#endif


///
/// CML-H types
///
#define CML_H_45_WATT_4_2       0x17
#define CML_H_45_WATT_6_2       0x18
#define CML_H_65_WATT_6_2       0x19
#define CML_H_45_WATT_8_2       0x1F
#define CML_H_65_WATT_8_2       0x20

///
/// CML-S types
///
#define CML_S_35_WATT_2_2       0x1
#define CML_S_54_WATT_2_2       0x2
#define CML_S_58_WATT_2_2       0x3
#define CML_S_35_WATT_4_2       0x4
#define CML_S_62_WATT_4_2       0x5
#define CML_S_65_WATT_4_2       0x6
#define CML_S_95_WATT_4_2       0x7
#define CML_S_35_WATT_6_2       0x8
#define CML_S_65_WATT_6_2       0x9
#define CML_S_95_WATT_6_2       0xA
#define CML_S_35_WATT_8_2       0xB
#define CML_S_65_WATT_8_2       0xC
#define CML_S_95_WATT_8_2       0xD
#define CML_S_71_WATT_4_2       0xE   /// Workstation sku
#define CML_S_80_WATT_6_2       0xF   /// Workstation sku
#define CML_S_80_WATT_8_2       0x10  /// Workstation sku
#define CML_S_83_WATT_4_2       0x25   /// Workstation sku

///
/// Used to identify the CPU used for programming with the VR override table
///
typedef enum {
  EnumUnknownCpuId       = 0,
  EnumMinCpuId           = 1,

  ///
  /// CML-S
  ///
  EnumCmlSMinCpuId       = CML_S_35_WATT_2_2,
  EnumCmlS35Watt22CpuId  = CML_S_35_WATT_2_2,
  EnumCmlS54Watt22CpuId  = CML_S_54_WATT_2_2,
  EnumCmlS58Watt22CpuId  = CML_S_58_WATT_2_2,
  EnumCmlS35Watt42CpuId  = CML_S_35_WATT_4_2,
  EnumCmlS62Watt42CpuId  = CML_S_62_WATT_4_2,
  EnumCmlS65Watt42CpuId  = CML_S_65_WATT_4_2,
  EnumCmlS95Watt42CpuId  = CML_S_95_WATT_4_2,
  EnumCmlS35Watt62CpuId  = CML_S_35_WATT_6_2,
  EnumCmlS65Watt62CpuId  = CML_S_65_WATT_6_2,
  EnumCmlS95Watt62CpuId  = CML_S_95_WATT_6_2,
  EnumCmlS35Watt82CpuId  = CML_S_35_WATT_8_2,
  EnumCmlS65Watt82CpuId  = CML_S_65_WATT_8_2,
  EnumCmlS95Watt82CpuId  = CML_S_95_WATT_8_2,
  EnumCmlS71Watt42CpuId  = CML_S_71_WATT_4_2, /// Workstation sku
  EnumCmlS83Watt42CpuId  = CML_S_83_WATT_4_2, /// Workstation sku
  EnumCmlS80Watt62CpuId  = CML_S_80_WATT_6_2, /// Workstation sku
  EnumCmlS80Watt82CpuId  = CML_S_80_WATT_8_2, /// Workstation sku
  EnumCmlSMaxCpuId       = EnumCmlS80Watt82CpuId,

  ///
  /// CML-U
  ///
  EnumCmlU28Watt43eCpuId  = 0x11,
  EnumWhlU15Watt42fCpuId  = 0x12,
  EnumCmlU15Watt42fCpuId  = 0x13,
  EnumCmlU28Watt23eCpuId  = 0x24,
  EnumCmlUR15Watt43eCpuId = 0x22,
  EnumCmlUR28Watt43eCpuId = 0x23,
  EnumCmlUMaxCpuId        = EnumCmlU28Watt23eCpuId,

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
  /// CML-H
  ///
  EnumCmlH45Watt4f2CpuId  = CML_H_45_WATT_4_2,
  EnumCmlH45Watt62CpuId   = CML_H_45_WATT_6_2,
  EnumCmlH65Watt62CpuId   = CML_H_65_WATT_6_2,
  EnumCmlHMaxCpuId        = EnumCmlH65Watt62CpuId,

  ///
  /// CNL-H
  ///
  EnumCnlH45Watt82CpuId   = 0x1A,
  EnumCnlH65Watt82CpuId   = 0x1B,
  EnumCnlH95Watt82CpuId   = 0x1C,
  EnumCnlH120Watt82CpuId  = 0x1D,
  EnumCnlHMaxCpuId        = EnumCnlH95Watt82CpuId,

  ///
  /// CML-H
  ///
  EnumCmlH45Watt82CpuId   = CML_H_45_WATT_8_2,
  EnumCmlH65Watt82CpuId   = CML_H_65_WATT_8_2,
  EnumMaxCpuId            = EnumCmlH65Watt82CpuId,
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

//
// ASL PSS package structure layout
//
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
