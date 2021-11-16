/** @file

  Copyright (c) 2019 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SMBIOS_TABLES_H__
#define __SMBIOS_TABLES_H__

#include <Library/BaseLib.h>
#include <IndustryStandard/SmBios.h>
#include <Register/Intel/Cpuid.h>
#include <Register/Intel/ArchitecturalMsr.h>

#define SMBIOS_STRING_UNKNOWN           "Unknown"
#define SMBIOS_STRING_UNKNOWN_VERSION   "XXXX.XXX.XXX.XXX"
#define SMBIOS_STRING_VENDOR            "Intel Corporation"
#define SMBIOS_STRING_PLATFORM          "Intel Platform"

#define BRAND_STRING_UNSUPPORTED        "CPU Brand String Not Supported"
#define INTEL_CORPORATION_STRING        "Intel(R) Corporation"

#define TYPE_TERMINATOR_SIZE          2     // Each Type is terminated by 0000 - 2 bytes
#define TO_BE_FILLED                  0

#define SMBIOS_STRING_INDEX_NULL      0
#define SMBIOS_STRING_INDEX_1         1
#define SMBIOS_STRING_INDEX_2         2
#define SMBIOS_STRING_INDEX_3         3
#define SMBIOS_STRING_INDEX_4         4
#define SMBIOS_STRING_INDEX_5         5
#define SMBIOS_STRING_INDEX_6         6
#define SMBIOS_STRING_INDEX_7         7
#define SMBIOS_STRING_INDEX_8         8
#define SMBIOS_STRING_INDEX_9         9

#define EfiProcessorFamilyIntelAtomProcessor    0x2B
#define EfiProcessorFamilyIntelXeonProcessor    0xB3

typedef struct {
  CHAR8  *String;
  UINT16  Family;
} PROCESSOR_FAMILY_FIELD;

///
/// This data record refer
/// length.
///
typedef enum {
  EfiProcessorOther    = 1,
  EfiProcessorUnknown  = 2,
  EfiCentralProcessor  = 3,
  EfiMathProcessor     = 4,
  EfiDspProcessor      = 5,
  EfiVideoProcessor    = 6
} EFI_PROCESSOR_TYPE_DATA;

typedef struct {
  UINT32                            ProcessorSteppingId:4;
  UINT32                            ProcessorModel:     4;
  UINT32                            ProcessorFamily:    4;
  UINT32                            ProcessorType:      2;
  UINT32                            ProcessorReserved1: 2;
  UINT32                            ProcessorXModel:    4;
  UINT32                            ProcessorXFamily:   8;
  UINT32                            ProcessorReserved2: 4;
} EFI_PROCESSOR_SIGNATURE;


///
/// Inconsistent with specification here:
/// The name of third field in ProcSubClass specification 0.9 is LogicalProcessorCount.
/// Keep it unchanged for backward compatibility.
///
typedef struct {
  UINT32                            ProcessorBrandIndex    :8;
  UINT32                            ProcessorClflush       :8;
  UINT32                            ProcessorReserved      :8;
  UINT32                            ProcessorDfltApicId    :8;
} EFI_PROCESSOR_MISC_INFO;

typedef struct {
  UINT32                            ProcessorFpu:       1;
  UINT32                            ProcessorVme:       1;
  UINT32                            ProcessorDe:        1;
  UINT32                            ProcessorPse:       1;
  UINT32                            ProcessorTsc:       1;
  UINT32                            ProcessorMsr:       1;
  UINT32                            ProcessorPae:       1;
  UINT32                            ProcessorMce:       1;
  UINT32                            ProcessorCx8:       1;
  UINT32                            ProcessorApic:      1;
  UINT32                            ProcessorReserved1: 1;
  UINT32                            ProcessorSep:       1;
  UINT32                            ProcessorMtrr:      1;
  UINT32                            ProcessorPge:       1;
  UINT32                            ProcessorMca:       1;
  UINT32                            ProcessorCmov:      1;
  UINT32                            ProcessorPat:       1;
  UINT32                            ProcessorPse36:     1;
  UINT32                            ProcessorPsn:       1;
  UINT32                            ProcessorClfsh:     1;
  UINT32                            ProcessorReserved2: 1;
  UINT32                            ProcessorDs:        1;
  UINT32                            ProcessorAcpi:      1;
  UINT32                            ProcessorMmx:       1;
  UINT32                            ProcessorFxsr:      1;
  UINT32                            ProcessorSse:       1;
  UINT32                            ProcessorSse2:      1;
  UINT32                            ProcessorSs:        1;
  UINT32                            ProcessorReserved3: 1;
  UINT32                            ProcessorTm:        1;
  UINT32                            ProcessorReserved4: 2;
} EFI_PROCESSOR_FEATURE_FLAGS;

///
/// This structure provides a calculation for base-10 representations.
///
/// Not consistent with PI 1.2 Specification.
/// This data type is not defined in the PI 1.2 Specification, but is
/// required by several of the other data structures in this file.
///
typedef struct {
  ///
  /// The INT16 number by which to multiply the base-2 representation.
  ///
  INT16                            Value;
  ///
  /// The INT16 number by which to raise the base-2 calculation.
  ///
  INT16                            Exponent;
} EFI_EXP_BASE10_DATA;

///
/// This data record refers to the unique ID that identifies a set of processors. This data record is 16
/// bytes in length. The data in this structure is processor specific and reserved values can be defined
/// for future use. The consumer of this data should not make any assumption and should use this data
/// with respect to the processor family defined in the Family record number.
///
typedef struct {
  ///
  /// Identifies the processor.
  ///
  EFI_PROCESSOR_SIGNATURE           Signature;
  ///
  /// Provides additional processor information.
  ///
  EFI_PROCESSOR_MISC_INFO           MiscInfo;
  ///
  /// Reserved for future use.
  ///
  UINT32                            Reserved;
  ///
  /// Provides additional processor information.
  ///
  EFI_PROCESSOR_FEATURE_FLAGS       FeatureFlags;
} EFI_PROCESSOR_ID_DATA;


///
/// This data record refers to the core voltage of the processor being defined. The unit of measurement
/// of this data record is in volts.
///
typedef EFI_EXP_BASE10_DATA         EFI_PROCESSOR_VOLTAGE_DATA;

extern  SMBIOS_TABLE_TYPE0    mBiosInfo;
extern  SMBIOS_TABLE_TYPE1    mSystemInfo;
extern  SMBIOS_TABLE_TYPE2    mBaseBoardInfo;
extern  SMBIOS_TABLE_TYPE4    mProcessorInfo;
extern  SMBIOS_TABLE_TYPE19   mMemArrayMappedAddr;

/**
  This function builds required processor info SMBIOS type.
**/
EFI_STATUS
BuildProcessorInfo (
  VOID
  );

#endif /* __SMBIOS_TABLES_H__ */
