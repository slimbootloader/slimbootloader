/** @file
  Register names for CPU registers

  <b>Conventions</b>
  - Definitions beginning with "MSR_" are MSRs
  - Definitions beginning with "R_" are registers
  - Definitions beginning with "B_" are bits within registers
  - Definitions beginning with "V_" are meaningful values of bits within the registers
  - Definitions beginning with "S_" are register sizes
  - Definitions beginning with "N_" are the bit position

  Copyright (c) 2004 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

#define MSR_TURBO_RATIO_LIMIT                                         0x000001AD
#define MSR_PACKAGE_POWER_SKU_UNIT                                    0x00000606
#define MSR_PACKAGE_POWER_LIMIT                                       0x00000610

#define MSR_PLATFORM_INFO                                             0x000000CE


#define MSR_FLEX_RATIO                                                0x00000194


#define MSR_BOOT_GUARD_SACM_INFO                                      0x0000013A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0


#define MSR_IA32_THERM_STATUS                                         0x0000019C
#define B_IA32_THERM_STATUS_VALID                                     BIT31

#define MSR_TEMPERATURE_TARGET                                        0x000001A2
#define V_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_OFFSET_MASK           0x3F

//
// Bit defines for MSR_IA32_MTRR_DEF_TYPE
//
#define B_CACHE_MTRR_VALID                                            BIT11


#define N_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFST_OFFSET 8
#define S_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFST_WIDTH 8
#define B_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFST_MASK (0x0000FF00)

#define N_MSR_TEMPERATURE_TARGET_REF_TEMP_OFFSET 16
#define S_MSR_TEMPERATURE_TARGET_REF_TEMP_WIDTH 8
#define B_MSR_TEMPERATURE_TARGET_REF_TEMP_MASK (0x00FF0000)

#define N_MSR_FLEX_RATIO_OC_BINS_OFFSET 17
#define S_MSR_FLEX_RATIO_OC_BINS_WIDTH 3
#define B_MSR_FLEX_RATIO_OC_BINS_MASK (0x00000000000E0000)

#define N_MSR_TEMPERATURE_TARGET_TJ_MAX_TCC_OFFSET_OFFSET 24
#define S_MSR_TEMPERATURE_TARGET_TJ_MAX_TCC_OFFSET_WIDTH 6
#define B_MSR_TEMPERATURE_TARGET_TJ_MAX_TCC_OFFSET_MASK (0x3F000000)

//
// Architectural MSR for Core Capabilities
//
#define MSR_CORE_CAPABILITIES                     0x000000CF
#define B_MSR_CORE_CAPABILITIES_FUSA_SUPPORTED    BIT2


#ifndef TRIGGER_MODE_EDGE
#define TRIGGER_MODE_EDGE             0x00
#endif
#ifndef TRIGGER_MODE_LEVEL
#define TRIGGER_MODE_LEVEL            0x01
#endif

#ifndef CPU_FEATURE_DISABLE
#define CPU_FEATURE_DISABLE  0
#endif
#ifndef CPU_FEATURE_ENABLE
#define CPU_FEATURE_ENABLE   1
#endif

#define CACHE_WRITEPROTECTED            5

#define MSR_CACHE_VARIABLE_MTRR_BASE                                  0x200
#define B_CACHE_MTRR_VALID                                            BIT11


//
// Processor Definitions
//
#define CPUID_FULL_STEPPING                        0x0000000F
#define CPUID_FULL_FAMILY_MODEL                    0x0FFF0FF0
#define CPUID_FULL_FAMILY_MODEL_COFFEELAKE_ULT_ULX 0x000806E0
#define CPUID_FULL_FAMILY_MODEL_ICELAKE_ULT_ULX    0x000706E0
#define CPUID_FULL_FAMILY_MODEL_ICELAKE_DT_HALO_R  0x000906F0
#define CPUID_FULL_FAMILY_MODEL_ICELAKE_DT_HALO    0x000706D0
#define CPUID_FULL_FAMILY_MODEL_ICELAKE_DT         0x000706D0
#define CPUID_FULL_FAMILY_MODEL_ICELAKE_AIPG       0x000906D0
#define CPUID_FULL_FAMILY_MODEL_ELHARTLAKE         0x00090660
#define CPUID_FULL_FAMILY_MODEL_JASPERLAKE         0x000906C0
#define CPUID_FULL_FAMILY_MODEL_TIGERLAKE_ULT_ULX  0x000806C0
#define CPUID_FULL_FAMILY_MODEL_TIGERLAKE_DT_HALO  0x000806D0

#define STALL_ONE_MICRO_SECOND 1
#define STALL_ONE_MILLI_SECOND 1000

#define BITS(x) (1 << (x))

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
  EnumCpuCflUltUlx    = CPUID_FULL_FAMILY_MODEL_COFFEELAKE_ULT_ULX,
  EnumCpuIclUltUlx    = CPUID_FULL_FAMILY_MODEL_ICELAKE_ULT_ULX,
  EnumCpuIclDtHaloR   = CPUID_FULL_FAMILY_MODEL_ICELAKE_DT_HALO_R,
  EnumCpuIclDtHalo    = CPUID_FULL_FAMILY_MODEL_ICELAKE_DT_HALO,
  EnumCpuIclDt        = CPUID_FULL_FAMILY_MODEL_ICELAKE_DT,
  EnumCpuIclAipg      = CPUID_FULL_FAMILY_MODEL_ICELAKE_AIPG,
  EnumCpuJsl          = CPUID_FULL_FAMILY_MODEL_JASPERLAKE,
  EnumCpuEhl          = CPUID_FULL_FAMILY_MODEL_ELHARTLAKE,
  EnumCpuTglUltUlx    = CPUID_FULL_FAMILY_MODEL_TIGERLAKE_ULT_ULX,
  EnumCpuTglDtHalo    = CPUID_FULL_FAMILY_MODEL_TIGERLAKE_DT_HALO,
  EnumCpuMax          = CPUID_FULL_FAMILY_MODEL
} CPU_FAMILY;

///
/// Enums for CPU Stepping IDs
///
typedef enum {
  /// Whiskey Lake ULT Steppings
  EnumCflW0         = 0xB,
  EnumCflV0         = 0xC,

  EnumCflMaxUltUlxStep = EnumCflW0,
  ///
  /// Icelake ULX/ULT Steppings
  ///
  EnumIclA0         = 0,
  EnumIclB0         = 1,
  EnumIclC0         = 2,
  EnumIclD0         = 4,


  ///
  /// Icelake DT/Halo Refresh Steppings
  ///
  EnumIclG0         = 0,
  EnumIclH0         = 1,

  ///
  /// Icelake DT/Halo Steppings
  ///
  EnumIclP0         = 1,
  EnumIclQ0         = 2,
  ///
  /// Icelake DT Steppings
  ///
  EnumIclS0         = 1,

  ///
  /// Icelake AIPG Steppings
  ///
  EnumIclI0         = 0,
  ///
  /// Jasperlake Steppings
  ///
  EnumJslA0 = 0,
  ///
  /// Tigerlake ULX/ULT Steppings
  ///
  EnumTglA0 = 0,
  EnumTglB0 = 1,

  ///
  /// Tigerlake DT/Halo Steppings
  ///
  EnumTglP0 = 1,
  EnumTglQ0 = 2,

  ///
  /// Max Stepping
  ///
  EnumCpuSteppingMax  = CPUID_FULL_STEPPING
} CPU_STEPPING;

///
/// Enums for CPU SKU IDs
///
typedef enum {
  EnumCpuUlt        = 0,
  EnumCpuTrad,
  EnumCpuUlx,
  EnumCpuHalo,
  EnumCpuUnknown
} CPU_SKU;

#endif
