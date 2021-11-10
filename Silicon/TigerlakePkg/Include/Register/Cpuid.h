/** @file
  Intel CPUID leaf definitions.

  Provides defines for CPUID leaf indexes.  Data structures are provided for
  registers returned by a CPUID leaf that contain one or more bit fields.
  If a register returned is a single 32-bit value, then a data structure is
  not provided for that register.

  Copyright (c) 2015 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Specification Reference:
  Intel(R) 64 and IA-32 Architectures Software Developer's Manual, Volume 2A,
  November 2018, CPUID instruction.

**/

#ifndef __INTEL_CPUID_H__
#define __INTEL_CPUID_H__

/**
  CPUID Version Information

  @param   EAX  CPUID_VERSION_INFO (0x01)

  @retval  EAX  Returns Model, Family, Stepping Information described by the
                type CPUID_VERSION_INFO_EAX.
  @retval  EBX  Returns Brand, Cache Line Size, and Initial APIC ID described by
                the type CPUID_VERSION_INFO_EBX.
  @retval  ECX  CPU Feature Information described by the type
                CPUID_VERSION_INFO_ECX.
  @retval  EDX  CPU Feature Information described by the type
                CPUID_VERSION_INFO_EDX.

  <b>Example usage</b>
  @code
  CPUID_VERSION_INFO_EAX  Eax;
  CPUID_VERSION_INFO_EBX  Ebx;
  CPUID_VERSION_INFO_ECX  Ecx;
  CPUID_VERSION_INFO_EDX  Edx;

  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);
  @endcode
**/
#define CPUID_VERSION_INFO                      0x01

/**
  CPUID Intel Resource Director Technology (Intel RDT) Allocation Information

  @param   EAX  CPUID_INTEL_RDT_ALLOCATION (0x10).
  @param   ECX  CPUID_INTEL_RDT_ALLOCATION_ENUMERATION_SUB_LEAF (0x00).
                CPUID_INTEL_RDT_ALLOCATION_L3_CACHE_SUB_LEAF (0x01).
                CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF (0x02).
**/
#define CPUID_INTEL_RDT_ALLOCATION                          0x10

/**
  L2 Cache Allocation Technology Enumeration Sub-leaf

  @param   EAX  CPUID_INTEL_RDT_ALLOCATION (0x10)
  @param   ECX  CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF (0x02)

  @retval  EAX  RESID L2 Cache Allocation Technology information described by
                the type CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF_EAX.
  @retval  EBX  Bit-granular map of isolation/contention of allocation units.
  @retval  ECX  Reserved.
  @retval  EDX  RESID L2 Cache Allocation Technology information described by
                the type CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF_EDX.

  <b>Example usage</b>
  @code
  CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF_EAX  Eax;
  UINT32                                            Ebx;
  CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF_EDX  Edx;

  AsmCpuidEx (
    CPUID_INTEL_RDT_ALLOCATION, CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF,
    &Eax.Uint32, &Ebx, NULL, &Edx.Uint32
    );
  @endcode
**/
#define CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF        0x02

/**
  CPUID L2 Cache Allocation Technology Information EDX for CPUID leaf
  #CPUID_INTEL_RDT_ALLssOCATION, sub-leaf
  #CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 15:0] Highest COS number supported for this ResID.
    ///
    UINT32  HighestCosNumber:16;
    UINT32  Reserved:16;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
} CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF_EDX;

#endif
