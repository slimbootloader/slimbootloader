/** @file

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Register/SaRegs.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include "TccPtct.h"
#include <Guid/TccDataGuid.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/HobLib.h>
#include <Register/Cpuid.h>
#include <Library/BootloaderCommonLib.h>
#include <PlatformData.h>
#include <TccConfigSubRegions.h>

TCC_INFO_HOB *mTccInfoHob = NULL;

/**
  Gets WRC COS policy MSR Address

  @param[in] CosPolicy          Wrc COS Policy
  @param[out] MsrAddress        Cos Ways Mask

  @retval EFI_SUCCESS           Found WRC COS Address successful
  @retval EFI_INVALID_PARAMETER Invalid Cos Policy
  @retval EFI_UNSUPPORTED       Unsupported MSR
**/
EFI_STATUS
MsrGetWrcCosAddress (
  IN UINT8 CosPolicy,
  IN UINT32 *MsrAddress
  )
{
  switch (CosPolicy) {
    case 0:
      *MsrAddress = TGL_MSR_WRC_COS_WAY_MASK_0_0;
      break;
    case 1:
      *MsrAddress = TGL_MSR_WRC_COS_WAY_MASK_1_1;
      break;
    case 2:
      *MsrAddress = TGL_MSR_WRC_COS_WAY_MASK_2_2;
      break;
    case 3:
      *MsrAddress = TGL_MSR_WRC_COS_WAY_MASK_3_3;
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

CONST PtctCallEntry mPtctCallTable[] = { \
  {GetPtctPtcdLimits, 1},
  {GetPtctGtL3WayMasks, 1},
  {GetPtctWrcL3WayMasks, 1},
  {GetPtctBuffer, MAX_TCC_BUFFERS},
  {GetPtctMemHierarchyLatency, MAX_MEMORY_HIERARCHY_LATENCY_ENTRIES},
  {GetPtctPtcmBinary, 1}
};

/**
  Get WRC COS of specific policy

  @param[in]  CosPolicy    Wrc COS Policy to get ways
  @param[out] CosWaysMask  Cos Ways Mask

  @retval EFI_SUCCESS           get WRC COS successful
  @retval EFI_INVALID_PARAMETER invalid COS policy
  @retval EFI_UNSUPPORTED       Unsupported MSR
**/
EFI_STATUS
MsrGetWrcCos (
  IN UINT8     CosPolicy,
  OUT UINT32   *CosWaysMask
  )
{
  TGL_MSR_WRC_COS_WAY_MASK_0_0_REGISTER WrcCosMsr;
  UINT32                                MsrAddress;
  EFI_STATUS                            Status;

  Status = EFI_SUCCESS;

  if (CosWaysMask == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status =  MsrGetWrcCosAddress (CosPolicy, &MsrAddress);
  if(EFI_ERROR(Status)) {
    return Status;
  }

  WrcCosMsr.Uint64 = AsmReadMsr64 (MsrAddress);
  *CosWaysMask = WrcCosMsr.Uint32;

  return Status;
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
GetPtctWrcL3WayMasks (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  )
{
  EFI_STATUS             Status;
  UINT32                 WrcL3WayMasks;
  UINT64                 WrcVcCos;
  UINT32                 WrcVcEnable;
  UINT64_STRUCT          MchBar;
  PTCT_WRC_L3_WAY_MASKS  *PtctWrcL3WayMasks;

  Status = EFI_SUCCESS;

  if ((PtctEntry ==NULL) || (*PtctEntry != NULL) || (PtctIndex > 1)) {
    return EFI_INVALID_PARAMETER;
  }

  MchBar.Data32.High = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_MCHBAR + 4));
  MchBar.Data32.Low  = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_MCHBAR));
  MchBar.Data       &= (UINT64) ~BIT0;

  // WRC Enable VCrt
  WrcVcEnable = (MmioRead32 ((UINTN) MchBar.Data + R_SA_MCHBAR_WRC_VC_ENABLE) & (UINT32) B_SA_MCHBAR_WRC_VC_ENABLE_MASK);

  if (WrcVcEnable == 0){
    WrcL3WayMasks = 0;
  } else {
    // Find Vc Rt WRC Policy and get way mask
    // WrcVcCos should be non-zero.  bit 11 will be set . double check.
    WrcVcCos = (MmioRead64 ((UINTN) MchBar.Data + R_SA_MCHBAR_WRC_VC_CLOS) & B_SA_MCHBAR_WRC_VC_CLOS_MASK) >> N_SA_MCHBAR_WRC_VC_CLOS_OFFSET;
    Status = MsrGetWrcCos ((UINT8)WrcVcCos, &WrcL3WayMasks);

    if (EFI_ERROR (Status)) {
      return Status;
    }
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

/**
  Gets GT COS policy MSR Address

  @param[in] CosPolicy          GT COS Policy
  @param[out] MsrAddress        Cos Ways Mask

  @retval EFI_SUCCESS           Found GT COS Address successful
  @retval EFI_INVALID_PARAMETER Invalid COS policy
  @retval EFI_UNSUPPORTED       Unsupported MSR
**/
EFI_STATUS
MsrGetGtCosAddress (
  IN UINT8 CosPolicy,
  IN UINT32 *MsrAddress
  )
{
  switch (CosPolicy) {
    case 0:
      *MsrAddress = TGL_MSR_GT_COS_WAY_MASK_0_0;
      break;
    case 1:
      *MsrAddress = TGL_MSR_GT_COS_WAY_MASK_1_1;
      break;
    case 2:
      *MsrAddress = TGL_MSR_GT_COS_WAY_MASK_2_2;
      break;
    case 3:
      *MsrAddress = TGL_MSR_GT_COS_WAY_MASK_3_3;
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}


/**
  Get GT COS of specific policy

  @param[in] CosPolicy              GT COS Policy to get ways
  @param[out] CosWaysMask           Cos Ways Mask

  @retval EFI_SUCCESS               Got GT COS successful
  @retval EFI_INVALID_PARAMETER     Invalid COS policy
  @retval EFI_UNSUPPORTED           Unsupported MSR
**/
EFI_STATUS
MsrGetGtCos (
  IN UINT8     CosPolicy,
  OUT UINT32   *CosWaysMask
  )
{
  TGL_MSR_GT_COS_WAY_MASK_0_0_REGISTER  GtCosMsr;
  UINT32                                MsrAddress;
  EFI_STATUS                            Status;

  Status = EFI_SUCCESS;

  if (CosWaysMask == NULL) {
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }

  Status =  MsrGetGtCosAddress (CosPolicy, &MsrAddress);

  if(EFI_ERROR(Status)) {
    return Status;
  }

  GtCosMsr.Uint64 = AsmReadMsr64 (MsrAddress);
  *CosWaysMask = GtCosMsr.Uint32;

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
    Status = MsrGetGtCos (Index, &GtL3WayMasks);
    PtctGtL3WayMasks->GtL3WayMasks[Index] = GtL3WayMasks;
    DEBUG ((DEBUG_INFO, "PtctGtL3WayMasks->GtL3WayMasks[0x%x] : 0x%X\n", Index, PtctGtL3WayMasks->GtL3WayMasks[Index] ));
  }

  *PtctEntry = (PTCT_HEADER_ENTRY *)PtctGtL3WayMasks;
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

static MEMORY_HIERARCHY_LATENCY mMemoryHeirarchyLatency[MAX_MEMORY_HIERARCHY_LATENCY_ENTRIES] = {
                                        {MemHeirL2CacheType,     186, MAX_MEMORY_HIERARCHY_LATENCY_APIC_ID, {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7}},
                                        {MemHeirL3CacheType,     398, MAX_MEMORY_HIERARCHY_LATENCY_APIC_ID, {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7}},
                                        {MemHeirDramCacheType, 12402, MAX_MEMORY_HIERARCHY_LATENCY_APIC_ID, {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7}}};

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
  PLAT_FEATURES                   *PlatformFeatures;

  Status = EFI_SUCCESS;

  PlatformFeatures = &((PLATFORM_DATA *)GetPlatformDataPtr ())->PlatformFeatures;
  if (PlatformFeatures == NULL) {
    return EFI_NOT_FOUND;
  }
  if (PlatformFeatures->TccBufferBase == NULL) {
    return EFI_NOT_FOUND;
  }

  if ((PtctEntry == NULL) || (*PtctEntry != NULL) || (mTccInfoHob == NULL) || (PtctIndex >= MAX_TCC_BUFFERS)) {
    return EFI_INVALID_PARAMETER;
  }

  BufferSubregion = (TCC_BUFFER_CONFIGURATION *) PlatformFeatures->TccBufferBase;

  if (BufferSubregion == NULL) {
    return EFI_LOAD_ERROR;
  }

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

/*
  Get Max IA L2 Cache COS

  @retval UINT32    Max IA L2 Cache COS
*/
UINT32
GetMaxIaL2Cos (
  VOID
  )
{
  CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF_EDX L2CacheRdtAlloc;

  AsmCpuidEx (
    CPUID_INTEL_RDT_ALLOCATION,
    CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF,
    NULL,
    NULL,
    NULL,
    &L2CacheRdtAlloc.Uint32
    );

  // Add 1 for max cox
  return L2CacheRdtAlloc.Bits.HighestCosNumber + 1;
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

  *PtctEntry = (PTCT_HEADER_ENTRY *)PtctPtcdLimits;
  return EFI_SUCCESS;
}

/**
  Update PTCT  (Platform Tuning Configuration Table) ACPI Table

  @param[in] EFI_ACPI_DESCRIPTION_HEADER   PTCT ACPI Table entry

  @retval EFI_STATUS            Status returned from PTCT Table entries
          EFI_NOT_FOUND         Loader Global Pointer not found
                                TccHob not found
          EFI_INVALID_PARAMETER Incorrect Ptct table entries
**/
EFI_STATUS
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
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (PtctList, (MaxPtctEntries*sizeof (PTCT_HEADER_ENTRY*)));
  PtctEntryListIndex = 0;

  // Get all PTCT Entries
  for (Index = 0; Index < PtctCallTableCount; Index++) {
    for (PtctCallIndex = 0; PtctCallIndex < mPtctCallTable[Index].NumEntries; PtctCallIndex++) {
       Status = mPtctCallTable[Index].ptct_entry_func (PtctCallIndex, &PtctEntry);
      if (Status == EFI_SUCCESS) {
        PtctList[PtctEntryListIndex] = PtctEntry;
        PtctEntry = NULL;
        PtctTableLength += PtctList[PtctEntryListIndex]->Size;
        PtctEntryListIndex++;
      }
    }
  }
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
