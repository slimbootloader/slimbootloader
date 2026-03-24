/** @file
  Intel CPUID leaf definitions.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
  CPUID Version Information returned in EAX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  SteppingId:4;        ///< [Bits   3:0] Stepping ID
    UINT32  Model:4;             ///< [Bits   7:4] Model
    UINT32  FamilyId:4;          ///< [Bits  11:8] Family
    UINT32  ProcessorType:2;     ///< [Bits 13:12] Processor Type
    UINT32  Reserved1:2;         ///< [Bits 15:14] Reserved
    UINT32  ExtendedModelId:4;   ///< [Bits 19:16] Extended Model ID
    UINT32  ExtendedFamilyId:8;  ///< [Bits 27:20] Extended Family ID
    UINT32  Reserved2:4;         ///< Reserved
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32    Uint32;
} CPUID_VERSION_INFO_EAX;


/**
  CPUID Version Information returned in EBX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 7:0] Provides an entry into a brand string table that contains
    /// brand strings for IA-32 processors.
    ///
    UINT32  BrandIndex:8;
    ///
    /// [Bits 15:8] Indicates the size of the cache line flushed by the CLFLUSH
    /// and CLFLUSHOPT instructions in 8-byte increments. This field was
    /// introduced in the Pentium 4 processor.
    ///
    UINT32  CacheLineSize:8;
    ///
    /// [Bits 23:16] Maximum number of addressable IDs for logical processors
    /// in this physical package.
    ///
    /// @note
    /// The nearest power-of-2 integer that is not smaller than EBX[23:16] is
    /// the number of unique initial APICIDs reserved for addressing different
    /// logical processors in a physical package. This field is only valid if
    /// CPUID.1.EDX.HTT[bit 28]= 1.
    ///
    UINT32  MaximumAddressableIdsForLogicalProcessors:8;
    ///
    /// [Bits 31:24] The 8-bit ID that is assigned to the local APIC on the
    /// processor during power up. This field was introduced in the Pentium 4
    /// processor.
    ///
    UINT32  InitialLocalApicId:8;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32    Uint32;
} CPUID_VERSION_INFO_EBX;

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
  CPUID Version Information returned in EDX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bit 0] Floating Point Unit On-Chip. The processor contains an x87 FPU.
    ///
    UINT32  FPU:1;
    ///
    /// [Bit 1] Virtual 8086 Mode Enhancements.  Virtual 8086 mode enhancements,
    /// including CR4.VME for controlling the feature, CR4.PVI for protected
    /// mode virtual interrupts, software interrupt indirection, expansion of
    /// the TSS with the software indirection bitmap, and EFLAGS.VIF and
    /// EFLAGS.VIP flags.
    ///
    UINT32  VME:1;
    ///
    /// [Bit 2] Debugging Extensions.  Support for I/O breakpoints, including
    /// CR4.DE for controlling the feature, and optional trapping of accesses to
    /// DR4 and DR5.
    ///
    UINT32  DE:1;
    ///
    /// [Bit 3] Page Size Extension.  Large pages of size 4 MByte are supported,
    /// including CR4.PSE for controlling the feature, the defined dirty bit in
    /// PDE (Page Directory Entries), optional reserved bit trapping in CR3,
    /// PDEs, and PTEs.
    ///
    UINT32  PSE:1;
    ///
    /// [Bit 4] Time Stamp Counter.  The RDTSC instruction is supported,
    /// including CR4.TSD for controlling privilege.
    ///
    UINT32  TSC:1;
    ///
    /// [Bit 5] Model Specific Registers RDMSR and WRMSR Instructions.  The
    /// RDMSR and WRMSR instructions are supported. Some of the MSRs are
    /// implementation dependent.
    ///
    UINT32  MSR:1;
    ///
    /// [Bit 6] Physical Address Extension.  Physical addresses greater than 32
    /// bits are supported: extended page table entry formats, an extra level in
    /// the page translation tables is defined, 2-MByte pages are supported
    /// instead of 4 Mbyte pages if PAE bit is 1.
    ///
    UINT32  PAE:1;
    ///
    /// [Bit 7] Machine Check Exception.  Exception 18 is defined for Machine
    /// Checks, including CR4.MCE for controlling the feature. This feature does
    /// not define the model-specific implementations of machine-check error
    /// logging, reporting, and processor shutdowns. Machine Check exception
    /// handlers may have to depend on processor version to do model specific
    /// processing of the exception, or test for the presence of the Machine
    /// Check feature.
    ///
    UINT32  MCE:1;
    ///
    /// [Bit 8] CMPXCHG8B Instruction.  The compare-and-exchange 8 bytes(64 bits)
    /// instruction is supported (implicitly locked and atomic).
    ///
    UINT32  CX8:1;
    ///
    /// [Bit 9] APIC On-Chip.  The processor contains an Advanced Programmable
    /// Interrupt Controller (APIC), responding to memory mapped commands in the
    /// physical address range FFFE0000H to FFFE0FFFH (by default - some
    /// processors permit the APIC to be relocated).
    ///
    UINT32  APIC:1;
    UINT32  Reserved1:1;
    ///
    /// [Bit 11] SYSENTER and SYSEXIT Instructions.  The SYSENTER and SYSEXIT
    /// and associated MSRs are supported.
    ///
    UINT32  SEP:1;
    ///
    /// [Bit 12] Memory Type Range Registers.  MTRRs are supported. The MTRRcap
    /// MSR contains feature bits that describe what memory types are supported,
    /// how many variable MTRRs are supported, and whether fixed MTRRs are
    /// supported.
    ///
    UINT32  MTRR:1;
    ///
    /// [Bit 13] Page Global Bit.  The global bit is supported in paging-structure
    /// entries that map a page, indicating TLB entries that are common to
    /// different processes and need not be flushed. The CR4.PGE bit controls
    /// this feature.
    ///
    UINT32  PGE:1;
    ///
    /// [Bit 14] Machine Check Architecture. A value of 1 indicates the Machine
    /// Check Architecture of reporting machine errors is supported. The MCG_CAP
    /// MSR contains feature bits describing how many banks of error reporting
    /// MSRs are supported.
    ///
    UINT32  MCA:1;
    ///
    /// [Bit 15] Conditional Move Instructions.  The conditional move instruction
    /// CMOV is supported. In addition, if x87 FPU is present as indicated by the
    /// CPUID.FPU feature bit, then the FCOMI and FCMOV instructions are supported.
    ///
    UINT32  CMOV:1;
    ///
    /// [Bit 16] Page Attribute Table.  Page Attribute Table is supported. This
    /// feature augments the Memory Type Range Registers (MTRRs), allowing an
    /// operating system to specify attributes of memory accessed through a
    /// linear address on a 4KB granularity.
    ///
    UINT32  PAT:1;
    ///
    /// [Bit 17] 36-Bit Page Size Extension.  4-MByte pages addressing physical
    /// memory beyond 4 GBytes are supported with 32-bit paging. This feature
    /// indicates that upper bits of the physical address of a 4-MByte page are
    /// encoded in bits 20:13 of the page-directory entry. Such physical
    /// addresses are limited by MAXPHYADDR and may be up to 40 bits in size.
    ///
    UINT32  PSE_36:1;
    ///
    /// [Bit 18] Processor Serial Number.  The processor supports the 96-bit
    /// processor identification number feature and the feature is enabled.
    ///
    UINT32  PSN:1;
    ///
    /// [Bit 19] CLFLUSH Instruction.  CLFLUSH Instruction is supported.
    ///
    UINT32  CLFSH:1;
    UINT32  Reserved2:1;
    ///
    /// [Bit 21] Debug Store.  The processor supports the ability to write debug
    /// information into a memory resident buffer.  This feature is used by the
    /// branch trace store (BTS) and precise event-based sampling (PEBS)
    /// facilities.
    ///
    UINT32  DS:1;
    ///
    /// [Bit 22] Thermal Monitor and Software Controlled Clock Facilities.  The
    /// processor implements internal MSRs that allow processor temperature to
    /// be monitored and processor performance to be modulated in predefined
    /// duty cycles under software control.
    ///
    UINT32  ACPI:1;
    ///
    /// [Bit 23] Intel MMX Technology.  The processor supports the Intel MMX
    /// technology.
    ///
    UINT32  MMX:1;
    ///
    /// [Bit 24] FXSAVE and FXRSTOR Instructions.  The FXSAVE and FXRSTOR
    /// instructions are supported for fast save and restore of the floating
    /// point context. Presence of this bit also indicates that CR4.OSFXSR is
    /// available for an operating system to indicate that it supports the
    /// FXSAVE and FXRSTOR instructions.
    ///
    UINT32  FXSR:1;
    ///
    /// [Bit 25] SSE.  The processor supports the SSE extensions.
    ///
    UINT32  SSE:1;
    ///
    /// [Bit 26] SSE2.  The processor supports the SSE2 extensions.
    ///
    UINT32  SSE2:1;
    ///
    /// [Bit 27] Self Snoop.  The processor supports the management of
    /// conflicting memory types by performing a snoop of its own cache
    /// structure for transactions issued to the bus.
    ///
    UINT32  SS:1;
    ///
    /// [Bit 28] Max APIC IDs reserved field is Valid.  A value of 0 for HTT
    /// indicates there is only a single logical processor in the package and
    /// software should assume only a single APIC ID is reserved. A value of 1
    /// for HTT indicates the value in CPUID.1.EBX[23:16] (the Maximum number of
    /// addressable IDs for logical processors in this package) is valid for the
    /// package.
    ///
    UINT32  HTT:1;
    ///
    /// [Bit 29] Thermal Monitor.  The processor implements the thermal monitor
    /// automatic thermal control circuitry (TCC).
    ///
    UINT32  TM:1;
    UINT32  Reserved3:1;
    ///
    /// [Bit 31] Pending Break Enable.  The processor supports the use of the
    /// FERR#/PBE# pin when the processor is in the stop-clock state (STPCLK# is
    /// asserted) to signal the processor that an interrupt is pending and that
    /// the processor should return to normal operation to handle the interrupt.
    /// Bit 10 (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability.
    ///
    UINT32  PBE:1;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32    Uint32;
} CPUID_VERSION_INFO_EDX;

#define CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS                0x07

///
/// CPUID bitfield not in SDM
///
/**
  CPUID Structured Extended Feature Flags Enumeration in EDX for CPUID leaf
  #CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS sub leaf
  #CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_SUB_LEAF_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bit 1:0] Reserved.
    ///
    UINT32  Reserved1:2;
    ///
    /// [Bit 2] AVX512_4VNNIW. (Intel Xeon Phi only.)
    ///
    UINT32  AVX512_4VNNIW:1;
    ///
    /// [Bit 3] AVX512_4FMAPS. (Intel Xeon Phi only.)
    ///
    UINT32  AVX512_4FMAPS:1;
    ///
    /// [Bit 14:4] Reserved.
    ///
    UINT32  Reserved2:11;
    ///
    /// [Bit 15] Supports Hetero if 1.
    ///
    UINT32  Hetero:1;
    ///
    /// [Bit 25:16] Reserved.
    ///
    UINT32  Reserved3:10;
    ///
    /// [Bit 26] Enumerates support for indirect branch restricted speculation
    /// (IBRS) and the indirect branch pre-dictor barrier (IBPB). Processors
    /// that set this bit support the IA32_SPEC_CTRL MSR and the IA32_PRED_CMD
    /// MSR. They allow software to set IA32_SPEC_CTRL[0] (IBRS) and
    /// IA32_PRED_CMD[0] (IBPB).
    ///
    UINT32  EnumeratesSupportForIBRSAndIBPB:1;
    ///
    /// [Bit 27] Enumerates support for single thread indirect branch
    /// predictors (STIBP). Processors that set this bit support the
    /// IA32_SPEC_CTRL MSR. They allow software to set IA32_SPEC_CTRL[1]
    /// (STIBP).
    ///
    UINT32  EnumeratesSupportForSTIBP:1;
    ///
    /// [Bit 28] Enumerates support for L1D_FLUSH. Processors that set this bit
    /// support the IA32_FLUSH_CMD MSR. They allow software to set
    /// IA32_FLUSH_CMD[0] (L1D_FLUSH).
    ///
    UINT32  EnumeratesSupportForL1D_FLUSH:1;
    ///
    /// [Bit 29] Enumerates support for the IA32_ARCH_CAPABILITIES MSR.
    ///
    UINT32  EnumeratesSupportForCapability:1;
    ///
    /// [Bit 30] Reserved.
    ///
    UINT32  Reserved4:1;
    ///
    /// [Bit 31] Enumerates support for Speculative Store Bypass Disable (SSBD).
    /// Processors that set this bit sup-port the IA32_SPEC_CTRL MSR. They allow
    /// software to set IA32_SPEC_CTRL[2] (SSBD).
    ///
    UINT32  EnumeratesSupportForSSBD:1;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
} CPUID_EXTENDED_FEATURE_FLAGS_EDX_HETERO;

/**
  CPUID Extended Topology Information

  @note
  CPUID leaf 1FH is a preferred superset to leaf 0BH. Intel recommends first
  checking for the existence of Leaf 1FH before using leaf 0BH.
  Most of Leaf 0BH output depends on the initial value in ECX.  The EDX output
  of leaf 0BH is always valid and does not vary with input value in ECX.  Output
  value in ECX[7:0] always equals input value in ECX[7:0].
  Sub-leaf index 0 enumerates SMT level. Each subsequent higher sub-leaf index
  enumerates a higher-level topological entity in hierarchical order.
  For sub-leaves that return an invalid level-type of 0 in ECX[15:8]; EAX and
  EBX will return 0.
  If an input value n in ECX returns the invalid level-type of 0 in ECX[15:8],
  other input values with ECX > n also return 0 in ECX[15:8].

  @param   EAX  CPUID_EXTENDED_TOPOLOGY (0x0B)
  @param   ECX  Level number

  @retval  EAX  Extended topology information described by the type
                CPUID_EXTENDED_TOPOLOGY_EAX.
  @retval  EBX  Extended topology information described by the type
                CPUID_EXTENDED_TOPOLOGY_EBX.
  @retval  ECX  Extended topology information described by the type
                CPUID_EXTENDED_TOPOLOGY_ECX.
  @retval  EDX  x2APIC ID the current logical processor.

  <b>Example usage</b>
  @code
  CPUID_EXTENDED_TOPOLOGY_EAX  Eax;
  CPUID_EXTENDED_TOPOLOGY_EBX  Ebx;
  CPUID_EXTENDED_TOPOLOGY_ECX  Ecx;
  UINT32                       Edx;
  UINT32                       LevelNumber;

  LevelNumber = 0;
  do {
    AsmCpuidEx (
      CPUID_EXTENDED_TOPOLOGY, LevelNumber,
      &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx
      );
    LevelNumber++;
  } while (Eax.Bits.ApicIdShift != 0);
  @endcode
**/
#define CPUID_EXTENDED_TOPOLOGY                             0x0B

/**
  CPUID Extended Topology Information EAX for CPUID leaf #CPUID_EXTENDED_TOPOLOGY.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 4:0] Number of bits to shift right on x2APIC ID to get a unique
    /// topology ID of the next level type.  All logical processors with the
    /// same next level ID share current level.
    ///
    /// @note
    /// Software should use this field (EAX[4:0]) to enumerate processor
    /// topology of the system.
    ///
    UINT32  ApicIdShift:5;
    UINT32  Reserved:27;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
} CPUID_EXTENDED_TOPOLOGY_EAX;

/**
  CPUID Processor Frequency Information

  @note
  Data is returned from this interface in accordance with the processor's
  specification and does not reflect actual values. Suitable use of this data
  includes the display of processor information in like manner to the processor
  brand string and for determining the appropriate range to use when displaying
  processor information e.g. frequency history graphs. The returned information
  should not be used for any other purpose as the returned information does not
  accurately correlate to information / counters returned by other processor
  interfaces.  While a processor may support the Processor Frequency Information
  leaf, fields that return a value of zero are not supported.

  @param   EAX  CPUID_TIME_STAMP_COUNTER (0x16)

  @retval  EAX  Returns processor base frequency information described by the
                type CPUID_PROCESSOR_FREQUENCY_EAX.
  @retval  EBX  Returns maximum frequency information described by the type
                CPUID_PROCESSOR_FREQUENCY_EBX.
  @retval  ECX  Returns bus frequency information described by the type
                CPUID_PROCESSOR_FREQUENCY_ECX.
  @retval  EDX  Reserved.

  <b>Example usage</b>
  @code
  CPUID_PROCESSOR_FREQUENCY_EAX  Eax;
  CPUID_PROCESSOR_FREQUENCY_EBX  Ebx;
  CPUID_PROCESSOR_FREQUENCY_ECX  Ecx;

  AsmCpuid (CPUID_PROCESSOR_FREQUENCY, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, NULL);
  @endcode
**/
#define CPUID_PROCESSOR_FREQUENCY                           0x16

/**
  CPUID Processor Frequency Information ECX for CPUID leaf
  #CPUID_PROCESSOR_FREQUENCY.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 15:0] Bus (Reference) Frequency (in MHz).
    ///
    UINT32  BusFrequency:16;
    UINT32  Reserved:16;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
} CPUID_PROCESSOR_FREQUENCY_ECX;

#define CPUID_NATIVE_MODEL_ID_INFO              0x1A

/**
  CPUID Native Model ID Information returned in EAX for CPUID leaf
  #CPUID_NATIVE_MODEL_ID_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  NativeModelId:24;        ///< [Bits   23:0] Native Model ID
    UINT32  CoreType:8;              ///< [Bits   31:24] Core Type
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32    Uint32;
} CPUID_NATIVE_MODEL_ID_INFO_EAX;

#endif
