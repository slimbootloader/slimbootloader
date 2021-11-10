/** @file
  Register names for CPU registers

  <b>Conventions</b>
  - Definitions beginning with "MSR_" are MSRs
  - Definitions beginning with "R_" are registers
  - Definitions beginning with "B_" are bits within registers
  - Definitions beginning with "V_" are meaningful values of bits within the registers
  - Definitions beginning with "S_" are register sizes
  - Definitions beginning with "N_" are the bit position

  Copyright (c) 2004 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_
#include "Pcucommon.h"

#define MSR_PLATFORM_INFO                                             0x000000CE


/**
  Available ways vectors for class of service for small core
**/


#define MSR_BOOT_GUARD_SACM_INFO                                      0x0000013A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0


/**
  Resource Association Register (R/W). If ( (CPUID.(EAX=07H, ECX=0):EBX[12]
  =1) or (CPUID.(EAX=07H, ECX=0):EBX[15] =1 ) ).

  @param  ECX  MSR_IA32_PQR_ASSOC (0x00000C8F)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_IA32_PQR_ASSOC_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_IA32_PQR_ASSOC_REGISTER.

  <b>Example usage</b>
  @code
  MSR_IA32_PQR_ASSOC_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_IA32_PQR_ASSOC);
  AsmWriteMsr64 (MSR_IA32_PQR_ASSOC, Msr.Uint64);
  @endcode
  @note MSR_IA32_PQR_ASSOC is defined as IA32_PQR_ASSOC in SDM.
**/
#define MSR_ATOM_CORE_IA32_PQR_ASSOC                       0x00000C8F

/**
  MSR information returned for MSR index #MSR_IA32_PQR_ASSOC
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  Reserved:32;
    ///
    /// [Bits 35:32] COS (R/W). The class of service (COS) to enforce (on
    /// writes); returns the current COS when read.
    ///
    UINT32  COS:4;
    UINT32  Reserved1:28;
  } Bits;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_ATOM_CORE_IA32_PQR_ASSOC_REGISTER;


//
// Bit defines for MSR_IA32_MTRR_DEF_TYPE
//
#define B_CACHE_MTRR_VALID                                            BIT11


#define B_MSR_PLATFORM_INFO_SMM_SAVE_CAP_MASK BIT16

#define B_MSR_PLATFORM_INFO_SAMPLE_PART_MASK BIT27


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
#define CPUID_FULL_FAMILY_MODEL_ELKHARTLAKE        0x00090660
#define CPUID_FULL_FAMILY_MODEL_JASPERLAKE         0x000906C0
#define CPUID_FULL_FAMILY_MODEL_TIGERLAKE_ULT_ULX  0x000806C0
#define CPUID_FULL_FAMILY_MODEL_TIGERLAKE_DT_HALO  0x000806D0

#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 0
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND 0
#endif

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
  EnumCpuEhl          = CPUID_FULL_FAMILY_MODEL_ELKHARTLAKE,
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
  EnumIclB4         = 2,
  EnumIclC0         = 3,
  EnumIclD0         = 4,
  EnumIclD1         = 5,


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
  /// Elkhartlake Steppings
  ///
  EnumEhlA0 = 0,
  EnumEhlA1 = 1,
  ///
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
///
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

///
/// Enums for CPU Generation
///
typedef enum {
  EnumCflCpu  = 0,
  EnumIclCpu,
  EnumTglCpu,
  EnumJslCpu,
  EnumEhlCpu,
  EnumRklCpu,
  EnumCpuUnknownGeneration = 255
} CPU_GENERATION;
#endif
