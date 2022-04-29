/** @file
  Register names for CPU registers

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_
#pragma pack (push, 1)

#ifndef TRIGGER_MODE_EDGE
#define TRIGGER_MODE_EDGE             0x00
#endif
#ifndef TRIGGER_MODE_LEVEL
#define TRIGGER_MODE_LEVEL            0x01
#endif


#define MSR_PLATFORM_INFO                                             0x000000CE
#define MSR_FLEX_RATIO                                                0x00000194
#define MSR_TEMPERATURE_TARGET                                        0x000001A2
#define MSR_TURBO_RATIO_LIMIT                                         0x000001AD
#define MSR_PACKAGE_POWER_SKU_UNIT                                    0x606
#define MSR_PACKAGE_POWER_LIMIT                                       0x610
#define MSR_PACKAGE_POWER_SKU                                         0x614


#define MSR_BOOT_GUARD_SACM_INFO                                      0x0000013A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0

///
/// Local APIC definitions
///
//
// Processor Definitions
//
#define CPUID_FULL_STEPPING                         0x0000000F
#define CPUID_FULL_FAMILY_MODEL                     0x0FFF0FF0
#define CPUID_FULL_FAMILY_MODEL_ALDERLAKE_DT_HALO   0x00090670
#define CPUID_FULL_FAMILY_MODEL_ALDERLAKE_MOBILE    0x000906A0

#define CACHE_WRITEPROTECTED            5

#define MSR_CACHE_VARIABLE_MTRR_BASE                                  0x200
#define B_CACHE_MTRR_VALID                                            BIT11

#ifndef SLE_FLAG
#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 1
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND 1000
#endif
#else // SLE FLAG
#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 0
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND 0
#endif
#endif // SLE_FLAG

#define BITS(x) (1 << (x))


/**
  This 'flexible boot' register is written by BIOS in order to modify the maximum non-turbo ratio on the next reset.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 OcExtraVoltage : 8;

                            /* Bits[7:0], Access Type=RWS, default=0x00000000*/

                            /*
                               Depracated. Extra voltage to be used for
                               Overclocking.The voltage is defined in units of
                               1/256 Volts.
                            */
    UINT32 FlexRatio : 8;

                            /* Bits[15:8], Access Type=RWS, default=0x00000000*/

                            /*
                               When enabled by the ENABLE bit, the value in
                               this field sets the maximum non-turbo ratio. If
                               this value is greater than the maximum ratio set
                               by hardware, this field is silently ignored. The
                               override value applies on the next reset and not
                               immediately.
                            */
    UINT32 Enable : 1;

                            /* Bits[16:16], Access Type=RWS, default=0x00000000*/

                            /*
                               When set to 1, indicates that the value
                               programmed in FLEX_RATIO field will be used to
                               override the maximum non-turbo ratio on next
                               reboot. When this bit is set to zero, all writes
                               to FLEX_RATIO field are silently ignored.
                            */
    UINT32 OcBins : 3;

                            /* Bits[19:17], Access Type=RW_L, default=0x00000000*/

                            /*
                               [p]If overclocking is enabled, this field
                               contains how many bins of Core ratio
                               overclocking are supported.[/p] [p]The encoding
                               is:[/p] [list] [*]0: No Core overclocking is
                               supported [*]1-6: Max 1-6 bins of Core ratio
                               overclocking is supported. [*]7: Unlimited
                               [/list] [p]If overclocking is disabled then this
                               field should be ignored.[/p] [p]Overclocking
                               bins imply programming the TURBO_RATIO_LIMIT MSR
                               above its hardware default value.[/p]
                               Overclocking enabled/disabled can be checked in
                               CAPID0_B_0_0_0_PCI.OC_ENABLED.
                            */
    UINT32 OcLock : 1;

                            /* Bits[20:20], Access Type=RW1S, default=0x00000000*/

                            /*
                               If set, all writes to overclocking limits are
                               disabled. Anyway modifications to overclocking
                               limits prior to setting this lock bit are
                               maintained. This field may only be set by
                               software and it is cleared by hardware only on
                               reset.
                            */
    UINT32 Rsvd21 : 11;

                            /* Bits[31:21], Access Type=RO, default=None*/

                            /* Reserved */
    UINT32 Rsvd32 : 32;

                            /* Bits[63:32], Access Type=RO, default=None*/

                            /* Reserved */

  } Bits;

  UINT32 Uint32;
  UINT64 Uint64;

} MSR_FLEX_RATIO_REGISTER;

typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 TccOffsetTimeWindow : 7;

                            /* Bits[6:0], Access Type=RO_V, default=0x00000000*/

                            /* Describes the RATL averaging time window */
    UINT32 TccOffsetClampingBit : 1;

                            /* Bits[7:7], Access Type=RO_V, default=0x00000000*/

                            /* When enabled will allow RATL throttling below P1 */
    UINT32 FanTempTargetOfst : 8;

                            /* Bits[15:8], Access Type=RW_L, default=0x00000000*/

                            /*
                               Fan Temperature Target Offset (a.k.a. T-Control)
                               indicates the relative offset from the Thermal
                               Monitor Trip Temperature at which fans should be
                               engaged.
                            */
    UINT32 RefTemp : 8;

                            /* Bits[23:16], Access Type=RW_L, default=0x00000000*/

                            /*
                               This field indicates the maximum junction
                               temperature, also referred to as the Throttle
                               Temperature, TCC Activation Temperature or
                               Prochot Temperature. This is the temperature at
                               which the Adaptive Thermal Monitor is activated.
                            */
    UINT32 TjMaxTccOffset : 6;

                            /* Bits[29:24], Access Type=RO_V, default=0x00000000*/

                            /*
                               Temperature offset in degrees (C) from the TJ
                               Max. Used for throttling temperature. Will not
                               impact temperature reading. If offset is allowed
                               and set - the throttle will occur and reported
                               at lower then Tj_max.
                            */
    UINT32 Rsvd30 : 1;

                            /* Bits[30:30], Access Type=RO, default=None*/

                            /* Reserved */
    UINT32 Locked : 1;

                            /* Bits[31:31], Access Type=RW_KL, default=0x00000000*/

                            /*
                               When set, this entire register becomes read-
                               only.
                            */

  } Bits;

  UINT32 Uint32;
  UINT64 Uint64;

} MSR_TEMPERATURE_TARGET_REGISTER;

/**
  This MSR indicates the factory configured values for active core ranges (1-8) and not active cores. Each field in MSR_TURBO_RATIO_LIMIT_CORES (MSR 1AEh) denotes
  the active core count.
  Software can configure these limits when PLATFORM_INFO MSR 0CEh[28]==1.
  Instead of specifying a ratio for each active core count (legacy behavior), active core counts with an identical turbo ratio limit belong to a single active core range that act as a turbo limit for that entire range of active cores.
  Example: configure turbo ratio limits for a 10-Core model:
  [list]
  [*]Active Cores 1,2,3 to turbo ratio 40
  [*]Active Cores 4,5,6,7 to turbo ratio 39
  [*]Active Cores 8 to turbo ratio 38
  [*]Active Cores 9,10 to turbo ratio 36
  [/list]
  The ranges are programmed as follows:
  [list]
  [*]1ADh[7:0] = 40, 1AEh[7:0] = 3
  [*]1ADh[15:8] = 39, 1AEh[15:8] = 7
  [*]1ADh[23:16] = 38, 1AEh[23:16] = 8
  [*]1ADh[31:24] = 36, 1AEh[31:24] = 10
  [*]Recommended: pad both registers with zero. 1ADh[63:32] = 0, 1AEh[63:32] = 0
  [/list]
**/
#define ADL_MSR_BIGCORE_TURBO_RATIO_LIMIT 0x000001AD

typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 MaxTurboGroup0 : 8;

                            /* Bits[7:0], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 1 core active. */
    UINT32 MaxTurboGroup1 : 8;

                            /* Bits[15:8], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 2 cores active. */
    UINT32 MaxTurboGroup2 : 8;

                            /* Bits[23:16], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 3 cores active. */
    UINT32 MaxTurboGroup3 : 8;

                            /* Bits[31:24], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */
    UINT32 MaxTurboGroup4 : 8;

                            /* Bits[39:32], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */
    UINT32 MaxTurboGroup5 : 8;

                            /* Bits[47:40], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */
    UINT32 MaxTurboGroup6 : 8;

                            /* Bits[55:48], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */
    UINT32 MaxTurboGroup7 : 8;

                            /* Bits[63:56], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */

  } Bits;

  UINT8  MaxTurboGroup[8];
  UINT32 Uint32;
  UINT64 Uint64;

} ADL_MSR_BIGCORE_TURBO_RATIO_LIMIT_REGISTER;

/**
  This register defines the active core ranges for each frequency point
  NUMCORE[0:7] must be populated in ascending order.
  NUMCORE[i+1] must be greater than NUMCORE[i]
  Entries with NUMCORE[i] == 0 will be ignored
  The last valid entry must have NUMCORE >= the number of cores in the SKU
  If any of the rules above are broken, we will silently reject the configuration.
**/
#define ADL_MSR_BIGCORE_TURBO_RATIO_LIMIT_CORES 0x000001AE

typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 Numcore0 : 8;

                            /* Bits[7:0], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore1 : 8;

                            /* Bits[15:8], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore2 : 8;

                            /* Bits[23:16], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore3 : 8;

                            /* Bits[31:24], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore4 : 8;

                            /* Bits[39:32], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore5 : 8;

                            /* Bits[47:40], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore6 : 8;

                            /* Bits[55:48], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore7 : 8;

                            /* Bits[63:56], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */

  } Bits;

  UINT8  Numcore[8];
  UINT32 Uint32;
  UINT64 Uint64;

} ADL_MSR_BIGCORE_TURBO_RATIO_LIMIT_CORES_REGISTER;

/**
  This MSR indicates the factory configured values for active core ranges (1-8) and not active cores. Each field in MSR_TURBO_RATIO_LIMIT_CORES (MSR 1AEh) denotes
  the active core count.
  Software can configure these limits when PLATFORM_INFO MSR 0CEh[28]==1.
  Instead of specifying a ratio for each active core count (legacy behavior), active core counts with an identical turbo ratio limit belong to a single active core range that act as a turbo limit for that entire range of active cores.
  Example: configure turbo ratio limits for a 10-Core model:
  [list]
  [*]Active Cores 1,2,3 to turbo ratio 40
  [*]Active Cores 4,5,6,7 to turbo ratio 39
  [*]Active Cores 8 to turbo ratio 38
  [*]Active Cores 9,10 to turbo ratio 36
  [/list]
  The ranges are programmed as follows:
  [list]
  [*]1ADh[7:0] = 40, 1AEh[7:0] = 3
  [*]1ADh[15:8] = 39, 1AEh[15:8] = 7
  [*]1ADh[23:16] = 38, 1AEh[23:16] = 8
  [*]1ADh[31:24] = 36, 1AEh[31:24] = 10
  [*]Recommended: pad both registers with zero. 1ADh[63:32] = 0, 1AEh[63:32] = 0
  [/list]
**/
#define ADL_MSR_ATOM_TURBO_RATIO_LIMIT 0x00000650

typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 MaxTurboGroup0 : 8;

                            /* Bits[7:0], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 1 core active. */
    UINT32 MaxTurboGroup1 : 8;

                            /* Bits[15:8], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 2 cores active. */
    UINT32 MaxTurboGroup2 : 8;

                            /* Bits[23:16], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 3 cores active. */
    UINT32 MaxTurboGroup3 : 8;

                            /* Bits[31:24], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */
    UINT32 MaxTurboGroup4 : 8;

                            /* Bits[39:32], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */
    UINT32 MaxTurboGroup5 : 8;

                            /* Bits[47:40], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */
    UINT32 MaxTurboGroup6 : 8;

                            /* Bits[55:48], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */
    UINT32 MaxTurboGroup7 : 8;

                            /* Bits[63:56], Access Type=RW_L, default=0x00000000*/

                            /* Maximum turbo ratio limit with 4 cores active. */

  } Bits;

  UINT8  MaxTurboGroup[8];
  UINT32 Uint32;
  UINT64 Uint64;

} ADL_MSR_ATOM_TURBO_RATIO_LIMIT_REGISTER;

/**
  This register defines the active core ranges for each frequency point
  NUMCORE[0:7] must be populated in ascending order.
  NUMCORE[i+1] must be greater than NUMCORE[i]
  Entries with NUMCORE[i] == 0 will be ignored
  The last valid entry must have NUMCORE >= the number of cores in the SKU
  If any of the rules above are broken, we will silently reject the configuration.
**/
#define ADL_MSR_ATOM_TURBO_RATIO_LIMIT_CORES 0x00000651

typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 Numcore0 : 8;

                            /* Bits[7:0], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore1 : 8;

                            /* Bits[15:8], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore2 : 8;

                            /* Bits[23:16], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore3 : 8;

                            /* Bits[31:24], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore4 : 8;

                            /* Bits[39:32], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore5 : 8;

                            /* Bits[47:40], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore6 : 8;

                            /* Bits[55:48], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */
    UINT32 Numcore7 : 8;

                            /* Bits[63:56], Access Type=RW, default=0x00000000*/

                            /*
                               defines the active core ranges for each
                               frequency point.
                            */

  } Bits;

  UINT8  Numcore[8];
  UINT32 Uint32;
  UINT64 Uint64;

} ADL_MSR_ATOM_TURBO_RATIO_LIMIT_CORES_REGISTER;

/**
Notes :
  1.  Bit position always starts at 0.
  2.  Following macros are applicable only for Word aligned integers.
**/
#define BIT(Pos, Value)               (1 << (Pos) & (Value))
#define BITRANGE(From, Width, Value)  (((Value) >> (From)) & ((1 << (Width)) - 1))
///
/// Enums for CPU Family IDs
///
typedef enum {

  EnumCpuAdlDtHalo    = CPUID_FULL_FAMILY_MODEL_ALDERLAKE_DT_HALO,
  EnumCpuAdlMobile    = CPUID_FULL_FAMILY_MODEL_ALDERLAKE_MOBILE,
  EnumCpuMax          = CPUID_FULL_FAMILY_MODEL
} CPU_FAMILY;

typedef enum {
  // AlderLake DT/HALO Steppings
  EnumAdlA0 = 0,
  EnumAdlB0 = 1,
  EnumAdlC0 = 2,
  EnumAdlD0 = 3,
  // Alderlake P Steppings
  EnumAdlJ0 = 0,
  /// Max Stepping
  EnumCpuSteppingMax  = CPUID_FULL_STEPPING
} CPU_STEPPING;

typedef enum {
  EnumCpuUlt        = 0,
  EnumCpuTrad,
  EnumCpuUlx,
  EnumCpuHalo,
  EnumCpuUnknown
} CPU_SKU;
#pragma pack (pop)
#endif // _CPU_REGS_H_
