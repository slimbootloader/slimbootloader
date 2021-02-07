/** @file

  Copyright (c) 2021 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/ContainerLib.h>
#include <Library/TccLib.h>
#include <Library/TccPlatformLib.h>
#include "TccInfo.h"

#define SIGNATURE_IPFW    SIGNATURE_32 ('I', 'P', 'F', 'W')
#define SIGNATURE_TCCT    SIGNATURE_32 ('T', 'C', 'C', 'T')
#define SIGNATURE_TCCB    SIGNATURE_32 ('T', 'C', 'C', 'B')
#define SIGNATURE_TCCC    SIGNATURE_32 ('T', 'C', 'C', 'C')
#define SIGNATURE_TCCM    SIGNATURE_32 ('T', 'C', 'C', 'M')

TCC_INFO_HOB    *mTccInfoHob    = NULL;
BIOS_SETTINGS   *PolicyConfig;
TCC_BINARIES_BASE *TccBinaries;
UINT8 mTccRtd3Support;
UINT8 mTccLowPowerS0Idle;

/**
  Update FSP-M UPD config data for TCC tuning, load TCC buffer & tuning data binaries from TempRam
**/

EFI_STATUS
EFIAPI
TccModePreMemConfig (
  OUT BIOS_SETTINGS*     PreMemPolicyConfig,
  OUT TCC_BINARIES_BASE* PreMemTccBinaries
)
{
  TCC_STREAM_CONFIGURATION              *PreMemStreamConfig;
  EFI_STATUS                            Status;
  VOID                                  *TccPreMemBufferBase;
  UINT32                                TccPreMemBufferSize;
  VOID                                  *TccPreMemTuningBase;
  UINT32                                TccPreMemTuningSize;

  DEBUG ((DEBUG_INFO, "TccModePreMemConfig () - Start\n"));

  // Load TCC tuning data from container
  Status = LoadComponent (SIGNATURE_IPFW, SIGNATURE_TCCT, &TccPreMemTuningBase, &TccPreMemTuningSize);
  if (EFI_ERROR (Status) || AsciiStrCmp ((CHAR8*) TccPreMemTuningBase, "dummy\0") == 0 ||
      TccPreMemTuningSize < sizeof (TCC_STREAM_CONFIGURATION)) {
    // Set default tuning values for TCC mode if TCC Tuning binary is not found
    DEBUG ((DEBUG_INFO, "TCC Tuning data not found! Using default TCC tuning policies.\n"));
    PreMemPolicyConfig               = NULL;
    TccPreMemTuningBase        = NULL;
    TccPreMemTuningSize        = 0;

  } else {
    // Consume TCC tuning binary settings
    PreMemStreamConfig = (TCC_STREAM_CONFIGURATION *) TccPreMemTuningBase;
    PreMemPolicyConfig = (BIOS_SETTINGS *) &PreMemStreamConfig->Info.Format1.BiosSettings;
  }

  // Load Tcc buffers data from container
  Status = LoadComponent (SIGNATURE_IPFW, SIGNATURE_TCCB, &TccPreMemBufferBase, &TccPreMemBufferSize);
  if (EFI_ERROR (Status) || AsciiStrCmp ((CHAR8*) TccPreMemBufferBase, "dummy\0") == 0) {
    DEBUG ((DEBUG_INFO, "TCC  Buffer not found! %r\n", Status));
    TccPreMemBufferBase = NULL;
    TccPreMemBufferSize = 0;
  }

  // Assign the Tcc buffer & tuning to FSP M UPDs
  PreMemTccBinaries->BufferBase       = TccPreMemBufferBase;
  PreMemTccBinaries->BufferSize       = TccPreMemBufferSize;
  PreMemTccBinaries->TuningBase       = TccPreMemTuningBase;
  PreMemTccBinaries->TuningSize       = TccPreMemTuningSize;

  DEBUG ((DEBUG_INFO, "TccModePreMemConfig () - End\n"));
  return EFI_SUCCESS;
}

/**
  Update FSP-S UPD config data for TCC tuning, load TCC related binaries into FSP
**/

EFI_STATUS
EFIAPI
TccModePostMemConfig (
  VOID
)
{
  TCC_STREAM_CONFIGURATION              *StreamConfig;
  EFI_STATUS                            Status;
  VOID                                  *TccBufferBase;
  UINT32                                TccBufferSize;
  VOID                                  *TccTuningBase;
  UINT32                                TccTuningSize;
  VOID                                  *TccCacheconfigBase;
  UINT32                                TccCacheconfigSize;
  VOID                                  *TccPtcmBase;
  UINT32                                TccPtcmSize;

  DEBUG ((DEBUG_INFO, "TccModePostMemConfig () - Start\n"));

  // Reload TCC tuning data from container
  Status = LoadComponent (SIGNATURE_IPFW, SIGNATURE_TCCT, &TccTuningBase, &TccTuningSize);
  if (EFI_ERROR (Status) || AsciiStrCmp ((CHAR8*) TccTuningBase, "dummy\0") == 0 ||
      TccTuningSize < sizeof (TCC_STREAM_CONFIGURATION)) {
    // Set default tuning values for TCC mode if TCC Tuning binary is not found
    DEBUG ((DEBUG_INFO, "TCC Tuning data not found! Using default TCC tuning policies.\n"));
    PolicyConfig        = NULL;
    TccTuningBase       = NULL;
    TccTuningSize       = 0;
    mTccRtd3Support     = 0;
    mTccLowPowerS0Idle  = 0;

  } else {
    // Consume TCC tuning binary settings
    StreamConfig = (TCC_STREAM_CONFIGURATION *) TccTuningBase;
    PolicyConfig = (BIOS_SETTINGS *) &StreamConfig->Info.Format1.BiosSettings;
    mTccRtd3Support    = PolicyConfig->Dstates;
    mTccLowPowerS0Idle = PolicyConfig->Sstates;
  }

  // Reload Tcc buffers data from container
  Status = LoadComponent (SIGNATURE_IPFW, SIGNATURE_TCCB, &TccBufferBase, &TccBufferSize);
  if (EFI_ERROR (Status) || AsciiStrCmp ((CHAR8*) TccBufferBase, "dummy\0") == 0) {
    DEBUG ((DEBUG_INFO, "TCC  Buffer not found! %r\n", Status));
    TccBufferBase = NULL;
    TccBufferSize = 0;
  }

  // Load Tcc cache config binary from container
  Status = LoadComponent (SIGNATURE_IPFW, SIGNATURE_TCCC, &TccCacheconfigBase, &TccCacheconfigSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TCC Cache config not found! %r\n", Status));
    TccCacheconfigBase = NULL;
    TccCacheconfigSize = 0;
  }

  // Load Tcc PTCM binary from container
  Status = LoadComponent (SIGNATURE_IPFW, SIGNATURE_TCCM, &TccPtcmBase, &TccPtcmSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TCC PTCM not found! %r\n", Status));
    TccPtcmBase = NULL;
    TccPtcmSize = 0;
  }

  // Save TCC binaries base and size
  TccBinaries->BufferBase       = TccBufferBase;
  TccBinaries->BufferSize       = TccBufferSize;
  TccBinaries->TuningBase       = TccTuningBase;
  TccBinaries->TuningSize       = TccTuningSize;
  TccBinaries->CacheconfigBase  = TccCacheconfigBase;
  TccBinaries->CacheconfigSize  = TccCacheconfigSize;
  TccBinaries->PtcmBase         = TccPtcmBase;
  TccBinaries->PtcmSize         = TccPtcmSize;

  Status = TccSetFspsConfig (PolicyConfig, TccBinaries);
  DEBUG ((DEBUG_INFO, "TccModePostMemConfig () - End\n"));
  return Status;
}

/*
  Get PTCT PTCD Limits entry data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PtctEntry   PTCT PTCD Limits entry

  @retval EFI_SUCCESS             Successful get PTCT PTCD Limits entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
EFIAPI
GetPtctPtcdLimits (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  )
{
  PTCT_PTCD_LIMITS                *PtctPtcdLimits;

  if ((PtctEntry ==NULL) || (*PtctEntry != NULL) || (PtctIndex >= 1)) {
    return EFI_INVALID_PARAMETER;
  }

  PtctPtcdLimits = AllocatePool (sizeof (PTCT_PTCD_LIMITS));
  if (PtctPtcdLimits == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PtctPtcdLimits->Header.Size = sizeof (PTCT_PTCD_LIMITS);
  PtctPtcdLimits->Header.Type = PtctPtcdLimitsType;
  PtctPtcdLimits->Header.Version = PTCT_PTCD_LIMITS_VERSION;

  PtctPtcdLimits->MaxIaL2Cos = GetMaxIaL2Cos ();
  PtctPtcdLimits->MaxIaL3Cos = MAX_IA_L3_CLOS;
  PtctPtcdLimits->MaxGtCos = MAX_GT_CLOS;
  PtctPtcdLimits->MaxWrcCos = MAX_WRC_CLOS;
  PtctPtcdLimits->MaxL2Instances = MAX_L2_INSTANCES;
  PtctPtcdLimits->MaxTccBuffers = MAX_TCC_BUFFERS;
  PtctPtcdLimits->MaxTccBufferApicIds = MAX_TCC_BUFFER_APIC_IDS;
  PtctPtcdLimits->MaxTccStreams = MAX_TCC_STREAMS;
  PtctPtcdLimits->MaxTccRegisters = MAX_TCC_REGISTERS;

  DEBUG ((DEBUG_INFO, "MaxIaL2Cos: %d\n", PtctPtcdLimits->MaxIaL2Cos));

  *PtctEntry = (PTCT_HEADER_ENTRY *)PtctPtcdLimits;
  return EFI_SUCCESS;
}

/*
  Get PTCT WRC L3 Way entry Data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PTCT_HEADER_ENTRY   PTCT WRC L3 Way entry

  @retval EFI_SUCCESS             Successful get PTCT WRC L3 Way entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
EFIAPI
GetPtctWrcL3WayMasks (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  )
{
  EFI_STATUS             Status;
  UINT32                 WrcL3WayMasks;
  PTCT_WRC_L3_WAY_MASKS  *PtctWrcL3WayMasks;

  if ((PtctEntry ==NULL) || (*PtctEntry != NULL) || (PtctIndex >= 1)) {
    return EFI_INVALID_PARAMETER;
  }

  Status =  EFI_SUCCESS;
  WrcL3WayMasks   = 0;

  Status = MsrGetWrcCos (&WrcL3WayMasks);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PtctWrcL3WayMasks = AllocatePool (sizeof (PTCT_WRC_L3_WAY_MASKS));
  if (PtctWrcL3WayMasks == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PtctWrcL3WayMasks->Header.Size = sizeof (PTCT_WRC_L3_WAY_MASKS);
  PtctWrcL3WayMasks->Header.Type = PtctWrcL3WayMasksType;
  PtctWrcL3WayMasks->Header.Version = PTCT_WRC_L3_WAY_MASKS_VERSION;
  PtctWrcL3WayMasks->WrcL3WayMasks = WrcL3WayMasks;
  DEBUG ((DEBUG_INFO, "WrcMask:0x%x\n", WrcL3WayMasks));

  *PtctEntry = (PTCT_HEADER_ENTRY *)PtctWrcL3WayMasks;
  return Status;
}

/*
  Get PTCT GT L3 Way entry Data

  @param[in]  PtctIndex         PTCT entry Index
  @param[out] PTCT_HEADER_ENTRY     PTCT GT L3 Way entry

  @retval EFI_SUCCESS             Successful get PTCT WRC L3 Way entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
EFIAPI
GetPtctGtL3WayMasks (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  )
{
  EFI_STATUS            Status;
  UINT32                GtL3WayMasks;
  UINT8                 Index;
  PTCT_GT_L3_WAY_MASKS  *PtctGtL3WayMasks;

  Status = EFI_SUCCESS;

  if ((PtctEntry ==NULL) || (*PtctEntry != NULL) || (PtctIndex >= 1)) {
    return EFI_INVALID_PARAMETER;
  }

  PtctGtL3WayMasks = AllocatePool (sizeof (PTCT_GT_L3_WAY_MASKS));
  if (PtctGtL3WayMasks == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PtctGtL3WayMasks->Header.Size = sizeof (PTCT_GT_L3_WAY_MASKS);
  PtctGtL3WayMasks->Header.Type = PtctGtL3WayMasksType;
  PtctGtL3WayMasks->Header.Version = PTCT_GT_L3_WAY_MASKS_VERSION;

  // Get all GT way masks
  for (Index = 0; Index < MAX_GT_L3_WAY_MASKS; Index++) {
    Status = GetGtCos (Index, &GtL3WayMasks);

    if (EFI_ERROR (Status)) {
      FreePool (PtctGtL3WayMasks);
      return Status;
    }

    PtctGtL3WayMasks->GtL3WayMasks[Index] = GtL3WayMasks;
    DEBUG ((DEBUG_INFO, "PtctGtL3WayMasks->GtL3WayMasks[0x%x] : 0x%X\n", Index, PtctGtL3WayMasks->GtL3WayMasks[Index] ));
  }

  *PtctEntry = (PTCT_HEADER_ENTRY *)PtctGtL3WayMasks;
  return Status;
}

/*
  Get PTCT Buffer entry Data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PtctEntry   PTCT Buffer entry

  @retval EFI_SUCCESS             Successful get PTCT Buffer entry
  @retval EFI_INVALID_PARAMETER   Input invalid
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
  @retval EFI_LOAD_ERROR          TCC Buffer Subregion is null
  @retval EFI_NOT_FOUND           Invalid Buffer IndexS
*/
EFI_STATUS
EFIAPI
GetPtctBuffer (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  )
{
  EFI_STATUS                   Status;
  TCC_BUFFER_CONFIGURATION     *BufferSubregion;
  PTCT_BUFFER                  *PtctBuffer;
  UINT8                        PtctEntrySize;
  UINT32                       NumOfApic;
  UINT32                       Index;
  TCC_BUFFER                   *TccBuffer;

  Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "Getting TCC Buffer for PTCT table.\n"));

  if ((PtctEntry == NULL) || (*PtctEntry != NULL) || (mTccInfoHob == NULL) || (PtctIndex >= MAX_TCC_BUFFERS)) {
    return EFI_INVALID_PARAMETER;
  }

  BufferSubregion = (TCC_BUFFER_CONFIGURATION *) TccBinaries->BufferBase;

  if (BufferSubregion == NULL) {
    return EFI_LOAD_ERROR;
  }

  DEBUG ((DEBUG_INFO, "TccBuffer loaded! Address: 0x%X\n", (UINT32) TccBinaries->BufferBase));
  DEBUG ((DEBUG_INFO, "TccBuffer: BufferSize: 0x%X\n", PtctBuffer->Size));

  if (PtctIndex >= BufferSubregion->Info.Format.ValidBuffers) {
    return  EFI_NOT_FOUND;
  }

  TccBuffer = &BufferSubregion->Info.Format.SBuffers[PtctIndex];
  NumOfApic = TccBuffer->Info.Format.ValidApicIds;

  // Calculate size of Buffer PTCT entry with number of Apic ids that be add to it
  PtctEntrySize = (UINT8) (sizeof (PTCT_BUFFER) + ((NumOfApic - 1) * sizeof (UINT32)));
  PtctBuffer = AllocatePool (PtctEntrySize);
  if (PtctBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PtctBuffer->Header.Size = PtctEntrySize;
  PtctBuffer->Header.Type = PtctBufferType;
  PtctBuffer->Header.Version = PTCT_BUFFER_VERSION;
  PtctBuffer->Base = mTccInfoHob->Data.BufferAddress[PtctIndex];
  PtctBuffer->Size = TccBuffer->Info.Format.Size;
  PtctBuffer->CacheLevel = TccBuffer->Info.Format.CacheLevel;
  PtctBuffer->WayMasks = TccBuffer->Info.Format.CacheWays;

  // add Apic ids to PTCT entry
  for (Index = 0; Index < NumOfApic; Index++) {
    PtctBuffer->ApicId[Index] = TccBuffer->Info.Format.ApicId[Index];
  }

  DEBUG ((DEBUG_INFO, "TccBuffer[%d] Address: 0x%X\n", PtctIndex, PtctBuffer->Base));
  DEBUG ((DEBUG_INFO, "TccBuffer: BufferSize: 0x%X\n", PtctBuffer->Size));

  *PtctEntry = (PTCT_HEADER_ENTRY *)PtctBuffer;

  return Status;
}

static MEMORY_HIERARCHY_LATENCY mMemoryHeirarchyLatency[MAX_MEMORY_HIERARCHY_LATENCY_ENTRIES] = {
                                        {MemHeirL2CacheType,      45, MAX_MEMORY_HIERARCHY_LATENCY_APIC_ID, {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7}},
                                        {MemHeirL3CacheType,     141, MAX_MEMORY_HIERARCHY_LATENCY_APIC_ID, {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7}},
                                        {MemHeirDramCacheType,   296, MAX_MEMORY_HIERARCHY_LATENCY_APIC_ID, {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7}}};

/*
  Get Memory Hierarchy Latency Info

  @param[in]  Index            Entry Index
  @param[out] MemHeirLatency   Pointer to Memory Hierarchy Latency entry

  @retval EFI_SUCCESS             Successful gets Memory Heirarchy Latency entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
*/
EFI_STATUS
GetMemoryLatency (
  IN UINT32 Index,
  OUT MEMORY_HIERARCHY_LATENCY **MemHeirLatency
)
{
  if ((Index >= MAX_MEMORY_HIERARCHY_LATENCY_ENTRIES) || (MemHeirLatency == NULL) || (*MemHeirLatency != NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  *MemHeirLatency = &mMemoryHeirarchyLatency[Index];

  return EFI_SUCCESS;
}

/*
  Get PTCT Memory Hierarchy Latency entry Data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PtctEntry   PTCT Memory Hierarchy Latency entry

  @retval EFI_SUCCESS             Successful get PTCT memory Hierarchy Latency entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
EFIAPI
GetPtctMemHierarchyLatency (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  )
{
  EFI_STATUS                      Status;
  PTCT_MEMORY_HIERARCHY_LATENCY   *PtctMemHierarchyLatency;
  MEMORY_HIERARCHY_LATENCY        *MemHeirLatency;
  UINT8                           PtctEntrySize;
  UINT32                          Index;


  Status = EFI_SUCCESS;

  if ((PtctEntry == NULL) || (*PtctEntry != NULL) || (PtctIndex >= MAX_MEMORY_HIERARCHY_LATENCY_ENTRIES)) {
    return EFI_INVALID_PARAMETER;
  }

  MemHeirLatency = NULL;
  Status = GetMemoryLatency (PtctIndex, &MemHeirLatency);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Calculate size of Memory Latency PTCT entry with number of Apic ids that be add to it
  PtctEntrySize = (UINT8) (sizeof (PTCT_MEMORY_HIERARCHY_LATENCY) + ((MemHeirLatency->NumOfValidApic - 1) * sizeof (UINT32)));

  PtctMemHierarchyLatency = AllocatePool (PtctEntrySize);
  if (PtctMemHierarchyLatency == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PtctMemHierarchyLatency->Header.Size = PtctEntrySize;
  PtctMemHierarchyLatency->Header.Type = PtctMemoryHierarchyLatencyType;
  PtctMemHierarchyLatency->Header.Version = PTCT_MEMORY_HIERARCHY_LATENCY_VERSION;
  PtctMemHierarchyLatency->Hierarchy = MemHeirLatency->Hierarchy;
  PtctMemHierarchyLatency->ClockCycles = MemHeirLatency->ClockCycles;

  // Add Apic ids to PTCT entry
  for (Index = 0; Index < MemHeirLatency->NumOfValidApic; Index++) {
    PtctMemHierarchyLatency->ApicId[Index] = MemHeirLatency->ApicId[Index];
  }

  *PtctEntry = (PTCT_HEADER_ENTRY *)PtctMemHierarchyLatency;
  return Status;
}

/*
  Get PTCT PTCM Binary entry data

  @param[out] PTCT_HEADER_ENTRY   PTCT PTCM Binary entry
  @param[in]  UINT32              PTCT entry Index

  @retval EFI_SUCCESS             Successful get PTCT PTCM Binary entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
  @retval EFI_NOT_FOUND           Unable to locate PTCM Binary
*/
EFI_STATUS
EFIAPI
GetPtctPtcmBinary (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  )
{
  PTCT_PTCM_BINARY                *PtctPtcmBinary;

  if ((PtctEntry ==NULL) || (*PtctEntry != NULL) || (mTccInfoHob == NULL) || (PtctIndex >= 1)) {
    return EFI_INVALID_PARAMETER;
  }

  // Skip making PTCM Binary PTCT entry if PTCM binary doesnt exist
  if ((mTccInfoHob->Data.PtcmBinaryAddress == 0) || (mTccInfoHob->Data.PtcmBinarySize == 0)) {
    DEBUG ((DEBUG_INFO, "PtcmBinary does not exist\n"));
    return EFI_NOT_FOUND;
  }

  PtctPtcmBinary = AllocatePool (sizeof (PTCT_PTCM_BINARY));
  if (PtctPtcmBinary == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PtctPtcmBinary->Header.Size = sizeof (PTCT_PTCM_BINARY);
  PtctPtcmBinary->Header.Type = PtctPtcmBinaryType;
  PtctPtcmBinary->Header.Version = PTCT_PTCM_BINARY_VERSION;

  PtctPtcmBinary->Address = mTccInfoHob->Data.PtcmBinaryAddress;
  PtctPtcmBinary->Size = mTccInfoHob->Data.PtcmBinarySize;

  *PtctEntry = (PTCT_HEADER_ENTRY *)PtctPtcmBinary;

  DEBUG ((DEBUG_INFO, "PtcmBinary[%d] Address: 0x%X,\n", PtctIndex, PtctPtcmBinary->Address));
  DEBUG ((DEBUG_INFO, "PtctPtcmBinary->Size: 0x%X\n", PtctPtcmBinary->Size));
  return EFI_SUCCESS;
}

const PtctCallEntry mPtctCallTable[] = {
  {GetPtctPtcdLimits, 1},
  {GetPtctGtL3WayMasks, 1},
  {GetPtctBuffer, MAX_TCC_BUFFERS},
  {GetPtctMemHierarchyLatency, MAX_MEMORY_HIERARCHY_LATENCY_ENTRIES},
  {GetPtctPtcmBinary, 1}
};

/**
  Update PTCT  (Platform Tuning Configuration Table) ACPI Table

  @param[in] EFI_ACPI_DESCRIPTION_HEADER   PTCT ACPI Table entry

  @retval EFI_STATUS            Status returned from PTCT Table entries
          EFI_NOT_FOUND         Loader Global Pointer not found
                                TccHob not found
          EFI_INVALID_PARAMETER Incorrect Ptct table entries
**/
EFI_STATUS
EFIAPI
UpdateAcpiPtctTable (
    IN EFI_ACPI_DESCRIPTION_HEADER *PtctTable
  )
{
  UINT8                         *DestAddr;
  EFI_STATUS                    Status;
  PTCT_HEADER_ENTRY             **PtctList;
  PTCT_HEADER_ENTRY             *PtctEntry;
  UINT32                        PtctTableLength;
  UINT32                        PtctCallTableCount;
  UINT32                        Index;
  UINT32                        MaxPtctEntries;
  UINT32                        PtctEntryListIndex;
  UINT32                        PtctCallIndex;
  LOADER_GLOBAL_DATA            *LdrGlobal;

  Status = EFI_SUCCESS;
  if (PtctTable == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  LdrGlobal   = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  if (LdrGlobal == NULL) {
    return EFI_NOT_FOUND;
  }

  mTccInfoHob = GetNextGuidHob (&gTccInfoHobGuid, LdrGlobal->FspHobList);
  if (mTccInfoHob == NULL) {
    return EFI_NOT_FOUND;
  }

  PtctTableLength = sizeof (EFI_ACPI_DESCRIPTION_HEADER);
  PtctEntry = NULL;

  PtctCallTableCount = sizeof (mPtctCallTable)/sizeof (mPtctCallTable[0]);

  MaxPtctEntries = 0;

  for (Index = 0; Index < PtctCallTableCount; Index++) {
    MaxPtctEntries += mPtctCallTable[Index].NumEntries;
  }

  PtctList = AllocatePool(MaxPtctEntries * sizeof (PTCT_HEADER_ENTRY*));
  if (PtctList == NULL) {
    DEBUG ((DEBUG_ERROR, "PTCT installation failed: out of resources\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (PtctList, (MaxPtctEntries*sizeof (PTCT_HEADER_ENTRY*)));
  PtctEntryListIndex = 0;

  // Get all PTCT Entries
  for (Index = 0; Index < PtctCallTableCount; Index++) {
    for (PtctCallIndex = 0; PtctCallIndex < mPtctCallTable[Index].NumEntries; PtctCallIndex++) {
      Status = mPtctCallTable[Index].ptct_entry_func (PtctCallIndex, &PtctEntry);
      if (Status == EFI_SUCCESS) {
        DEBUG ((DEBUG_INFO, "PTCT Entry %d installed\n", Index));
        PtctList[PtctEntryListIndex] = PtctEntry;
        PtctEntry = NULL;
        PtctTableLength += PtctList[PtctEntryListIndex]->Size;
        PtctEntryListIndex++;
      }
    }
  }
  DEBUG ((DEBUG_INFO, "%d PTCT entries installed\n", PtctEntryListIndex));
  PtctTable->Length          = PtctTableLength;

  // Add PTCT entries to ACPI table
  DestAddr = (UINT8 *) PtctTable;
  DestAddr += sizeof (EFI_ACPI_DESCRIPTION_HEADER);

  for (Index = 0; Index < MaxPtctEntries; Index++) {
    if (PtctList[Index] == NULL) {
      continue;
    }

  CopyMem (DestAddr, PtctList[Index], PtctList[Index]->Size);
  DestAddr += PtctList[Index]->Size;

  FreePool (PtctList[Index]);
  }
  return Status;
}

