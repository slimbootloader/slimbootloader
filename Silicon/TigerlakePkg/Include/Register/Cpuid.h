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
  CPUID Version Information returned in ECX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bit 0] Streaming SIMD Extensions 3 (SSE3).  A value of 1 indicates the
    /// processor supports this technology
    ///
    UINT32  SSE3:1;
    ///
    /// [Bit 1] A value of 1 indicates the processor supports the PCLMULQDQ
    /// instruction.  Carryless Multiplication
    ///
    UINT32  PCLMULQDQ:1;
    ///
    /// [Bit 2] 64-bit DS Area.  A value of 1 indicates the processor supports
    /// DS area using 64-bit layout.
    ///
    UINT32  DTES64:1;
    ///
    /// [Bit 3] MONITOR/MWAIT.  A value of 1 indicates the processor supports
    /// this feature.
    ///
    UINT32  MONITOR:1;
    ///
    /// [Bit 4] CPL Qualified Debug Store.  A value of 1 indicates the processor
    /// supports the extensions to the Debug Store feature to allow for branch
    /// message storage qualified by CPL
    ///
    UINT32  DS_CPL:1;
    ///
    /// [Bit 5] Virtual Machine Extensions.  A value of 1 indicates that the
    /// processor supports this technology.
    ///
    UINT32  VMX:1;
    ///
    /// [Bit 6] Safer Mode Extensions. A value of 1 indicates that the processor
    /// supports this technology
    ///
    UINT32  SMX:1;
    ///
    /// [Bit 7] Enhanced Intel SpeedStep(R) technology.  A value of 1 indicates
    /// that the processor supports this technology
    ///
    UINT32  EIST:1;
    ///
    /// [Bit 8] Thermal Monitor 2.  A value of 1 indicates whether the processor
    /// supports this technology
    ///
    UINT32  TM2:1;
    ///
    /// [Bit 9] A value of 1 indicates the presence of the Supplemental Streaming
    /// SIMD Extensions 3 (SSSE3). A value of 0 indicates the instruction
    /// extensions are not present in the processor.
    ///
    UINT32  SSSE3:1;
    ///
    /// [Bit 10] L1 Context ID.  A value of 1 indicates the L1 data cache mode
    /// can be set to either adaptive mode or shared mode. A value of 0 indicates
    /// this feature is not supported. See definition of the IA32_MISC_ENABLE MSR
    /// Bit 24 (L1 Data Cache Context Mode) for details
    ///
    UINT32  CNXT_ID:1;
    ///
    /// [Bit 11] A value of 1 indicates the processor supports IA32_DEBUG_INTERFACE
    /// MSR for silicon debug
    ///
    UINT32  SDBG:1;
    ///
    /// [Bit 12] A value of 1 indicates the processor supports FMA (Fused Multiple
    ///  Add) extensions using YMM state.
    ///
    UINT32  FMA:1;
    ///
    /// [Bit 13] CMPXCHG16B Available.  A value of 1 indicates that the feature
    /// is available.
    ///
    UINT32  CMPXCHG16B:1;
    ///
    /// [Bit 14] xTPR Update Control.  A value of 1 indicates that the processor
    /// supports changing IA32_MISC_ENABLE[Bit 23].
    ///
    UINT32  xTPR_Update_Control:1;
    ///
    /// [Bit 15] Perfmon and Debug Capability:  A value of 1 indicates the
    /// processor supports the performance and debug feature indication MSR
    /// IA32_PERF_CAPABILITIES.
    ///
    UINT32  PDCM:1;
    UINT32  Reserved:1;
    ///
    /// [Bit 17] Process-context identifiers.  A value of 1 indicates that the
    /// processor supports PCIDs and that software may set CR4.PCIDE to 1.
    ///
    UINT32  PCID:1;
    ///
    /// [Bit 18] A value of 1 indicates the processor supports the ability to
    /// prefetch data from a memory mapped device.  Direct Cache Access.
    ///
    UINT32  DCA:1;
    ///
    /// [Bit 19] A value of 1 indicates that the processor supports SSE4.1.
    ///
    UINT32  SSE4_1:1;
    ///
    /// [Bit 20] A value of 1 indicates that the processor supports SSE4.2.
    ///
    UINT32  SSE4_2:1;
    ///
    /// [Bit 21] A value of 1 indicates that the processor supports x2APIC
    /// feature.
    ///
    UINT32  x2APIC:1;
    ///
    /// [Bit 22] A value of 1 indicates that the processor supports MOVBE
    /// instruction.
    ///
    UINT32  MOVBE:1;
    ///
    /// [Bit 23] A value of 1 indicates that the processor supports the POPCNT
    /// instruction.
    ///
    UINT32  POPCNT:1;
    ///
    /// [Bit 24] A value of 1 indicates that the processor's local APIC timer
    /// supports one-shot operation using a TSC deadline value.
    ///
    UINT32  TSC_Deadline:1;
    ///
    /// [Bit 25] A value of 1 indicates that the processor supports the AESNI
    /// instruction extensions.
    ///
    UINT32  AESNI:1;
    ///
    /// [Bit 26] A value of 1 indicates that the processor supports the
    /// XSAVE/XRSTOR processor extended states feature, the XSETBV/XGETBV
    /// instructions, and XCR0.
    ///
    UINT32  XSAVE:1;
    ///
    /// [Bit 27] A value of 1 indicates that the OS has set CR4.OSXSAVE[Bit 18]
    /// to enable XSETBV/XGETBV instructions to access XCR0 and to support
    /// processor extended state management using XSAVE/XRSTOR.
    ///
    UINT32  OSXSAVE:1;
    ///
    /// [Bit 28] A value of 1 indicates the processor supports the AVX instruction
    /// extensions.
    ///
    UINT32  AVX:1;
    ///
    /// [Bit 29] A value of 1 indicates that processor supports 16-bit
    /// floating-point conversion instructions.
    ///
    UINT32  F16C:1;
    ///
    /// [Bit 30] A value of 1 indicates that processor supports RDRAND instruction.
    ///
    UINT32  RDRAND:1;
    ///
    /// [Bit 31] Always returns 0.
    ///
    UINT32  NotUsed:1;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32    Uint32;
} CPUID_VERSION_INFO_ECX;

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
