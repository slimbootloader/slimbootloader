/** @file
  The Smm Base HOB is used to store the information of:
  The relocated SmBase address in array for each processor.

  The default Smbase for the x86 processor is 0x30000. When SMI happens, processor
  runs the SMI handler at Smbase+0x8000. Also, the SMM save state area is within
  Smbase+0x10000. Since it's the start address to store the processor save state
  and code for the SMI entry point, those info are tiled within an SMRAM allocated
  or reserved buffer. This tile size shall be enough to cover 3 parts:
  1. Processor SMRAM Save State Map starts at Smbase + 0xfc00
  2. Extra processor specific context start starts at Smbase + 0xfb00
  3. SMI entry point starts at Smbase + 0x8000.
  Besides, This size should be rounded up to nearest power of 2. The Smm Base HOB
  producer should be responsible for reserving enough size.

  One of the SMM initialization from processor perspective is to relocate and program
  the new Smbase (in TSEG range) for each processor thread.

   Copyright (c) 2023 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef SMM_BASE_HOB_H_
#define SMM_BASE_HOB_H_

#define SMM_BASE_HOB_DATA_GUID \
  { \
    0xc2217ba7, 0x03bb, 0x4f63, {0xa6, 0x47, 0x7c, 0x25, 0xc5, 0xfc, 0x9d, 0x73}  \
  }

#pragma pack(1)
typedef struct {
  ///
  /// ProcessorIndex tells which processor range this specific HOB instance described.
  /// If ProcessorIndex is set to 0, it indicats the HOB describes the processor from
  /// 0 to NumberOfProcessors - 1. The HOB list may contains multiple this HOB
  /// instances. Each HOB instances describe the information for processor from
  /// ProcessorIndex to ProcessorIndex + NumberOfProcessors - 1. The instance order in
  /// the HOB list is random so consumer can not assume the ProcessorIndex of first
  /// instance is 0.
  ///
  UINT32    ProcessorIndex;
  ///
  /// Describes the Number of all max supported processors.
  ///
  UINT32    NumberOfProcessors;
  ///
  /// Pointer to SmBase address for each processor.
  ///
  UINT64    SmBase[];
} SMM_BASE_HOB_DATA;
#pragma pack()

extern EFI_GUID  gSmmBaseHobGuid;

#endif
