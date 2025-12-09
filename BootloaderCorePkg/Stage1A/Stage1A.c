/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1A.h"

CONST CHAR8*  mBootloaderName = "Intel Slim Bootloader";

CONST CDATA_BLOB mCfgBlobTmpl = {
  CFG_DATA_SIGNATURE,
  sizeof (CDATA_BLOB),
  0,
  {0},
  sizeof (CDATA_BLOB),
  FixedPcdGet32 (PcdCfgDatabaseSize)
};

CONST DEBUG_LOG_BUFFER_HEADER mLogBufHdrTmpl = {
  DEBUG_LOG_BUFFER_SIGNATURE,
  sizeof (DEBUG_LOG_BUFFER_HEADER),
  0,
  {0, 0},
  sizeof (DEBUG_LOG_BUFFER_HEADER),
  FixedPcdGet32 (PcdEarlyLogBufferSize)
};

//
// Global Descriptor Table (GDT)
//
STATIC
CONST IA32_SEGMENT_DESCRIPTOR
mGdtEntries[STAGE_GDT_ENTRY_COUNT] = {
  /* selector { Global Segment Descriptor                              } */
  /* 0x00 */  {{0,      0,  0,  0,    0,  0,  0,  0,    0,  0, 0,  0,  0}}, //null descriptor
  /* 0x08 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear data segment descriptor
  /* 0x10 */  {{0xffff, 0,  0,  0xb,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear code segment descriptor
  /* 0x18 */  {{0xffff, 0,  0,  0x3,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //system data segment descriptor
  /* 0x20 */  {{0xffff, 0,  0,  0xb,  1,  0,  1,  0xf,  0,  1, 0,  1,  0}}, //linear code (64-bit) segment descriptor
  /* 0x28 */  {{0xffff, 0,  0,  0xb,  1,  0,  1,  0x0,  0,  0, 0,  0,  0}}, //16-bit code segment descriptor
  /* 0x30 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0x0,  0,  0, 0,  0,  0}}, //16-bit data segment descriptor
};

//
// IA32 Gdt register
//
STATIC
CONST IA32_DESCRIPTOR mGdt = {
  sizeof (mGdtEntries) - 1,
  (UINTN) mGdtEntries
  };

/**
  Get flash map info.

  @param[out]  BufInfo    BUF_INFO structure pointer to return flash map info

  @retval      The flash map pointer.
               NULL if flash map is invalid.
**/
STATIC
FLASH_MAP *
GetFlashMapBufInfo (
  OUT            BUF_INFO     *BufInfo
)
{
  FLASH_MAP     *FlashMap;
  UINT32         FlashMapBase;
  UINT32         Stage1aFvBase;

  FlashMap      = NULL;
  Stage1aFvBase = PcdGet32 (PcdStage1AFdBase) + PcdGet32 (PcdFSPTSize);
  FlashMapBase  = (* (UINT32 *)(UINTN)(Stage1aFvBase + PcdGet32 (PcdStage1AFvSize) + FLASH_MAP_ADDRESS));
  if ( (FlashMapBase > Stage1aFvBase) && \
       (FlashMapBase + sizeof(FLASH_MAP) < Stage1aFvBase + PcdGet32 (PcdStage1AFvSize) - 1) ) {
    // Verify FLASH_MAP is valid before access
    if (((FLASH_MAP *)(UINTN)FlashMapBase)->Signature == FLASH_MAP_SIG_HEADER) {
      FlashMap = (FLASH_MAP *)(UINTN)FlashMapBase;
      BufInfo->SrcBase   = FlashMap;
      BufInfo->AllocLen  = FlashMap->Length;
    }
  }
  return FlashMap;
}


/**
  Allocate and copy required buffer for bootloader global data.

  @param[in]   Stage1aParam    Param pointer for Stage1A
**/
STATIC
VOID
AllocateCopyBuffer (
  IN STAGE1A_PARAM   *Stage1aParam
)
{
  UINT32                    Index;
  UINT32                    AllocLen;
  BUF_INFO                 *BufInfo;
  UINT8                    *BufPtr;
  LOADER_GLOBAL_DATA       *LdrGlobal;

  // Calculate allocation length in total
  AllocLen = 0;
  for (Index = 0; Index < EnumBufMax; Index++) {
    AllocLen += Stage1aParam->BufInfo[Index].AllocLen + sizeof(UINTN);
  }

  // Allocate buffer
  // Pre-check the buffer size first to avoid early ASSERT in AlloctePool
  // The error message will be deferred to be printed after debug port init
  LdrGlobal = GetLoaderGlobalDataPointer ();
  if (LdrGlobal->MemPoolCurrTop <= LdrGlobal->MemPoolCurrBottom + AllocLen) {
    BufPtr  = NULL;
  } else {
    BufPtr  = AllocateZeroPool (AllocLen);
  }

  if (BufPtr != NULL) {
    Stage1aParam->AllocDataLen  = AllocLen;
    Stage1aParam->AllocDataBase = (UINT32)(UINTN)BufPtr;

    // Copy buffer if required
    BufInfo = Stage1aParam->BufInfo;
    for (Index = 0; Index < EnumBufMax; Index++) {
      if (BufInfo->AllocLen > 0) {
        // Update the pointer in global data
        *((VOID **)BufInfo->DstBase) = BufPtr;
      }
      if (BufInfo->SrcBase != NULL) {
        if (BufInfo->CopyLen == 0) {
          BufInfo->CopyLen = BufInfo->AllocLen;
        }
        if (BufInfo->CopyLen <= BufInfo->AllocLen) {
          CopyMem (BufPtr, BufInfo->SrcBase, BufInfo->CopyLen);
        }
      }
      BufPtr += ALIGN_UP (BufInfo->AllocLen, sizeof(UINTN));
      BufInfo++;
    }
  }
}


/**
  Prepare and load Stage1B into proper location.
  - Load and uncompress the Stage1B image
  - do hash verification if hash verification fails then Halt CPU.

  Stage1B could be compressed or XIP. If compressed, it needs to be
  decompressed into temporary memory for execution.

  @param[in]   Stage1aParam    Param pointer for Stage1A

  @retval      The base address of the stage.
               0 if loading fails

**/
STATIC
UINT32
PrepareStage1B (
  IN STAGE1A_PARAM   *Stage1aParam
  )
{
  UINT32                    Src;
  UINT32                    Dst;
  UINT32                    Exe;
  UINT32                    Length;
  EFI_STATUS                Status;
  LOADER_COMPRESSED_HEADER *Hdr;
  UINT8                     SignHashAlg;

#if FixedPcdGetBool(PcdFipsSupport)
  /* If FIPS is enabled, run FIPS self tests before any crypto algorithm is run.
     Halt execution if any FIPS self test fails
  */
  Status = RunFipsSelftests ();
  if (EFI_ERROR(Status)) {
    CpuHalt ("FIPS Self Test Failed");
  }
  DEBUG((DEBUG_INFO, "FIPS Self-tests Status = Success\n"));
  AddMeasurePoint (0x10D0);
#endif

  // Load Stage 1B
  Status = GetComponentInfo (FLASH_MAP_SIG_STAGE1B, &Src, &Length);
  if (EFI_ERROR (Status)) {
    return 0;
  }
  Dst = PCD_GET32_WITH_ADJUST (PcdStage1BLoadBase);
  Exe = PCD_GET32_WITH_ADJUST (PcdStage1BFdBase);
  DEBUG ((DEBUG_INFO, "Load STAGE1B @ 0x%08X\n", Exe));

  Status = LoadStage1B (Dst, Src, Length);
  AddMeasurePoint (0x1080);
  if (EFI_ERROR (Status)) {
    return 0;
  }

  Src = Dst;
  Dst = Exe;
  Hdr = (LOADER_COMPRESSED_HEADER *)(UINTN)Src;

  // Verify Stage 1B
  if (FeaturePcdGet (PcdVerifiedBootEnabled)  && FixedPcdGetBool(PcdVerifiedBootStage1B)) {
    if (IS_COMPRESSED (Src)) {
      Length = sizeof (LOADER_COMPRESSED_HEADER) + Hdr->CompressedSize;
    }
    if(PcdGet8(PcdCompSignHashAlg) == HASH_TYPE_SHA256){
      SignHashAlg = HASH_TYPE_SHA256;
    } else if(PcdGet8(PcdCompSignHashAlg) == HASH_TYPE_SHA384){
      SignHashAlg = HASH_TYPE_SHA384;
    } else {
      SignHashAlg = HASH_TYPE_NONE;
    }

    Status = DoHashVerify ((CONST UINT8 *)(UINTN)Src, Length, HASH_USAGE_STAGE_1B, SignHashAlg, NULL);
    AddMeasurePoint (0x10A0);
    if (EFI_ERROR (Status)) {
      if (Status != RETURN_NOT_FOUND) {
        CpuHalt (NULL);
      }
    }
  }

  if (!FeaturePcdGet (PcdStage1BXip)) {
    // Decompress Stage 1B
    Status = Decompress (Hdr->Signature, Hdr->Data, Hdr->CompressedSize, (VOID *)(UINTN)Dst, NULL);
    AddMeasurePoint (0x10B0);
    if (EFI_ERROR (Status)) {
      Dst = 0;
    } else {
      // Silicon might need additional operation to enable execution
      EnableCodeExecution ();
    }
  }

  return Dst;
}

/**

  Entry point to the C language phase of Stage1A.

  After the Stage1A assembly code has initialized some temporary memory and set
  up the stack, control is transferred to this function.
  - Initialize the global data
  - Do post TempRaminit board initialization.
  - Relocate by itself stage1A code to temp memory and execute.
  - CPU halted if relocation fails.

  @param[in] Params            Pointer to stage specific parameters.

**/
VOID
EFIAPI
SecStartup2 (
  IN VOID  *Params
  )
{
  STAGE1A_PARAM             Stage1aParam;
  LOADER_GLOBAL_DATA       *LdrGlobal;
  STAGE1A_ASM_PARAM        *Stage1aAsmParam;
  STAGE_HDR                *StageHdr;
  STAGE_ENTRY               ContinueEntry;
  EFI_STATUS                Status;
  UINT32                    Delta;
  UINT32                    Dst;
  UINT32                    Src;
  UINT32                    Stage1aFvBase;
  FLASH_MAP                *FlashMap;
  PEI_PCD_DATABASE         *PcdDatabaseBin;
  HASH_STORE_TABLE         *HashStoreTable;
  SERVICES_LIST            *ServiceList;
  BUF_INFO                 *BufInfo;
  CONTAINER_LIST           *ContainerList;
  BOOT_PARTITION           Partition;

  Stage1aFvBase = PcdGet32 (PcdStage1AFdBase) + PcdGet32 (PcdFSPTSize);
  PeCoffFindAndReportImageInfo ((UINT32) (UINTN) GET_STAGE_MODULE_BASE (Stage1aFvBase));

  Stage1aAsmParam = (STAGE1A_ASM_PARAM *)Params;
  LdrGlobal = GetLoaderGlobalDataPointer ();

  // Init Stage1A Param block
  ZeroMem (&Stage1aParam, sizeof (STAGE1A_PARAM));

  // Flash Map
  BufInfo  = &Stage1aParam.BufInfo[EnumBufFlashMap];
  FlashMap = GetFlashMapBufInfo (BufInfo);
  BufInfo->DstBase = &LdrGlobal->FlashMapPtr;

  // Ver Info
  BufInfo = &Stage1aParam.BufInfo[EnumBufVerInfo];
  BufInfo->SrcBase   = (VOID *)(UINTN)PcdGet32 (PcdVerInfoBase);
  BufInfo->AllocLen  = sizeof (BOOT_LOADER_VERSION);
  BufInfo->DstBase   = &LdrGlobal->VerInfoPtr;

  // Hash Store
  BufInfo = &Stage1aParam.BufInfo[EnumBufHashStore];
  HashStoreTable     = (HASH_STORE_TABLE *)(UINTN)PcdGet32 (PcdHashStoreBase);
  BufInfo->SrcBase   = HashStoreTable;
  BufInfo->AllocLen  = PcdGet32 (PcdHashStoreSize);
  BufInfo->CopyLen   = HashStoreTable->UsedLength;
  BufInfo->DstBase   = &LdrGlobal->HashStorePtr;

  // Library Data
  BufInfo = &Stage1aParam.BufInfo[EnumBufLibData];
  BufInfo->AllocLen  = PcdGet32 (PcdMaxLibraryDataEntry) * sizeof (LIBRARY_DATA);
  BufInfo->DstBase   = &LdrGlobal->LibDataPtr;

  // Service data
  BufInfo = &Stage1aParam.BufInfo[EnumBufService];
  BufInfo->AllocLen  = sizeof (SERVICES_LIST) + \
                       sizeof (SERVICE_COMMON_HEADER *) * PcdGet32 (PcdMaxServiceNumber);
  BufInfo->DstBase   = &LdrGlobal->ServicePtr;

  // Pcd Database, only copy initialized data
  BufInfo = &Stage1aParam.BufInfo[EnumBufPcdData];
  PcdDatabaseBin = (PEI_PCD_DATABASE *)(UINTN)PcdGet32 (PcdFileDataBase);
  BufInfo->SrcBase   = (VOID *)PcdDatabaseBin;
  BufInfo->AllocLen  = PcdDatabaseBin->Length + PcdDatabaseBin->UninitDataBaseSize;
  BufInfo->CopyLen   = PcdDatabaseBin->Length;
  BufInfo->DstBase   = &LdrGlobal->PcdDataPtr;

  // Platform Data
  BufInfo = &Stage1aParam.BufInfo[EnumBufPlatData];
  BufInfo->AllocLen  = GetPlatformDataSize ();
  BufInfo->DstBase   = &LdrGlobal->PlatDataPtr;

  // Config data
  BufInfo = &Stage1aParam.BufInfo[EnumBufCfgData];
  PcdDatabaseBin = (PEI_PCD_DATABASE *)(UINTN)PcdGet32 (PcdFileDataBase);
  BufInfo->SrcBase   = (VOID *)&mCfgBlobTmpl;
  BufInfo->AllocLen  = PcdGet32 (PcdCfgDatabaseSize);
  BufInfo->CopyLen   = sizeof(CDATA_BLOB);
  BufInfo->DstBase   = &LdrGlobal->CfgDataPtr;

  // Container list
  BufInfo = &Stage1aParam.BufInfo[EnumBufCtnList];
  BufInfo->AllocLen  = PcdGet32 (PcdContainerMaxNumber) * sizeof (CONTAINER_ENTRY) + sizeof (CONTAINER_LIST);
  BufInfo->DstBase   = &LdrGlobal->ContainerList;

  // Log Buffer
  BufInfo = &Stage1aParam.BufInfo[EnumBufLogBuf];
  BufInfo->SrcBase   = (VOID *)&mLogBufHdrTmpl;
  BufInfo->AllocLen  = PcdGet32 (PcdEarlyLogBufferSize);
  BufInfo->CopyLen   = sizeof(DEBUG_LOG_BUFFER_HEADER);
  BufInfo->DstBase   = &LdrGlobal->LogBufPtr;

  // Allocate buffer
  AllocateCopyBuffer (&Stage1aParam);
  if (Stage1aParam.AllocDataLen > 0) {
    ServiceList = (SERVICES_LIST *)LdrGlobal->ServicePtr;
    if (ServiceList != NULL) {
      ServiceList->Count = PcdGet32 (PcdMaxServiceNumber);
    }
    HashStoreTable = (HASH_STORE_TABLE *)LdrGlobal->HashStorePtr;
    if (HashStoreTable != NULL) {
      HashStoreTable->TotalLength = PcdGet32 (PcdHashStoreSize);
    }
    ContainerList = (CONTAINER_LIST *) LdrGlobal->ContainerList;
    if (ContainerList != NULL) {
      BufInfo = &Stage1aParam.BufInfo[EnumBufCtnList];
      ContainerList->Signature   = CONTAINER_LIST_SIGNATURE;
      ContainerList->TotalLength = BufInfo->AllocLen;
    }
    BufInfo = &Stage1aParam.BufInfo[EnumBufPcdData];
    SetLibraryData (PcdGet8 (PcdPcdLibId), LdrGlobal->PcdDataPtr, BufInfo->AllocLen);
  }

  if (PcdGetBool (PcdIdenticalTopSwapsBuilt)) {
    if (!EFI_ERROR (GetBootPartition (&Partition))) {
      SetCurrentBootPartition (Partition);
    }
  } else if (FlashMap != NULL) {
    SetCurrentBootPartition ((FlashMap->Attributes & FLASH_MAP_ATTRIBUTES_BACKUP_REGION) ? BackupPartition : PrimaryPartition);
  }

  // Call board hook to enable debug
  BoardInit (PostTempRamInit);
  AddMeasurePoint (0x1040);

  // Set DebugPrintErrorLevel to default PCD.
  SetDebugPrintErrorLevel (PcdGet32 (PcdDebugPrintErrorLevel));

  if (DebugCodeEnabled()) {
    DEBUG ((DEBUG_INFO, "\n============= %a STAGE1A =============\n",mBootloaderName));
  } else {
    DEBUG ((DEBUG_INIT, "\n%a\n", mBootloaderName));
  }

  if (Stage1aAsmParam->Status.CpuBist != 0) {
    CpuHalt ("CPU BIST failure!\n");
  }

  if (Stage1aAsmParam->Status.StackOutOfRange != 0) {
    CpuHalt ("Stack base offset is too big!\n");
  }

  if (FlashMap == NULL) {
    CpuHalt ("Invalid flash map!\n");
  }

  if (Stage1aParam.AllocDataLen == 0) {
    CpuHalt ("Insufficant memory pool!\n");
  }

  // Enable more CPU featurs
  AsmEnableAvx ();

  ContinueEntry = ContinueFunc;
  if (!FeaturePcdGet (PcdStage1AXip)) {
    // Need to relocate itself into temporary memory
    Dst = PcdGet32 (PcdStage1ALoadBase);
    Src = PcdGet32 (PcdStage1AFdBase) + PcdGet32 (PcdFSPTSize);
    CopyMem ((VOID *)(UINTN)Dst, (VOID *)(UINTN)Src, PcdGet32 (PcdStage1AFvSize));
    Delta    = Dst - Src;
    StageHdr = (STAGE_HDR *)(UINTN)Dst;
    StageHdr->Entry += Delta;
    StageHdr->Base  += Delta;
    Status = PeCoffRelocateImage (StageHdr->Base);
    if (!EFI_ERROR (Status)) {
      EnableCodeExecution ();
      ContinueEntry = (STAGE_ENTRY)(UINTN)((UINT32)(Delta + (UINTN)ContinueFunc));
    } else {
      CpuHalt ("Relocation failed!\n");
    }
  }

  // Control will get into ContinueFunc() for execution
  ContinueEntry (&Stage1aParam);
}

/**

  Entry point to the C language phase of Stage1A.

  After the Stage1A assembly code has initialized some temporary memory and set
  up the stack, control is transferred to this function.
  - Initialize the global data
  - Do post TempRaminit board initialization.
  - Relocate by itself stage1A code to temp memory and execute.
  - CPU halted if relocation fails.

  @param[in] Params            Pointer to stage specific parameters.

**/
VOID
EFIAPI
SecStartup (
  IN VOID  *Params
  )
{
  LOADER_GLOBAL_DATA        LdrGlobalData;
  STAGE_IDT_TABLE           IdtTable;
  STAGE_GDT_TABLE           GdtTable;
  LOADER_GLOBAL_DATA       *LdrGlobal;
  STAGE1A_ASM_PARAM        *Stage1aAsmParam;
  UINT32                    StackTop;
  UINT32                    PageTblSize;
  UINT64                    TimeStamp;

  TimeStamp   = ReadTimeStamp ();
  Stage1aAsmParam = (STAGE1A_ASM_PARAM *)Params;

  // Init global data
  PageTblSize = IS_X64 ? 8 * EFI_PAGE_SIZE : 0;
  LdrGlobal = &LdrGlobalData;
  ZeroMem (LdrGlobal, sizeof (LOADER_GLOBAL_DATA));
  StackTop = (UINT32)(UINTN)Params + sizeof (STAGE1A_ASM_PARAM);
  LdrGlobal->Signature             = LDR_GDATA_SIGNATURE;
  LdrGlobal->LoaderStage           = LOADER_STAGE_1A;
  LdrGlobal->StackTop              = StackTop;
  LdrGlobal->MemPoolEnd            = StackTop + PcdGet32 (PcdStage1DataSize) - PageTblSize;
  LdrGlobal->MemPoolStart          = StackTop;
  LdrGlobal->MemPoolCurrTop        = LdrGlobal->MemPoolEnd;
  LdrGlobal->MemPoolCurrBottom     = LdrGlobal->MemPoolStart;
  LdrGlobal->MemPoolMaxUsed        = 0;
  LdrGlobal->DebugPrintErrorLevel  = 0;
  LdrGlobal->PerfData.PerfIndex    = 2;
  LdrGlobal->PerfData.FreqKhz      = GetTimeStampFrequency ();
  LdrGlobal->PerfData.TimeStamp[0] = Stage1aAsmParam->TimeStamp | 0x1000000000000000ULL;
  LdrGlobal->PerfData.TimeStamp[1] = TimeStamp  | 0x1010000000000000ULL;
  // Set the Loader features to default here.
  // Any platform (board init lib) can update these according to
  // the config data passed in or these defaults remain
  LdrGlobal->LdrFeatures           = FEATURE_MEASURED_BOOT | FEATURE_ACPI;
  // TempRam Base and Size
  LdrGlobal->CarBase               = Stage1aAsmParam->CarBase;
  LdrGlobal->CarSize               = Stage1aAsmParam->CarTop - LdrGlobal->CarBase;

  LoadGdt (&GdtTable, (IA32_DESCRIPTOR *)&mGdt);
  UpdateSelectors();
  LoadIdt (&IdtTable, (UINT32)(UINTN)LdrGlobal);
  SetLoaderGlobalDataPointer (LdrGlobal);

  InitializeDebugAgent (DEBUG_AGENT_INIT_PREMEM_SEC, Params, SecStartup2);
}

/**
  Continue Stage 1A execution.

  This function will continue Stage1A execution for a new location.
  - Migrates the stage1A Param data to stage1B
  - Prints BootLoader Version information
  - Allocate Version, key store and config data and populate the data.
  - Load stage1B, Halt CPU if stage1B returns.

  @param[in]  Params          Pointer to STAGE1A_ASM_PARAM in main memory.

**/
VOID
EFIAPI
ContinueFunc (
  IN VOID  *Params
  )
{
  STAGE1A_PARAM            *Stage1aParam;
  LOADER_GLOBAL_DATA       *LdrGlobal;
  STAGE_ENTRY               StageEntry;
  UINT32                    StageBase;
  UINT8                     ImageId[9];
  BOOT_LOADER_VERSION      *VerInfoTbl;
  FSP_INFO_HEADER          *FspInfoHdr;
  UINT32                    CpuSig;

  LdrGlobal     = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  AddMeasurePoint (0x1060);

  if (!FeaturePcdGet (PcdStage1AXip)) {
    // Update exception handler in IDT
    UpdateExceptionHandler (NULL);
  }

  // Migrate data from Stage1A Param to Stage1B Param
  Stage1aParam = (STAGE1A_PARAM *)Params;

  // Print version info and
  VerInfoTbl = (BOOT_LOADER_VERSION *)LdrGlobal->VerInfoPtr;
  ImageId[8] = 0;
  CopyMem (ImageId, &VerInfoTbl->ImageId, sizeof (UINT64));
  DEBUG ((DEBUG_INIT,  "SBID: %a\n"
          "ISVN: %03d\n"
          "IVER: %03d.%03d.%03d.%03d.%05d\n",
          ImageId,
          VerInfoTbl->ImageVersion.SecureVerNum,
          VerInfoTbl->ImageVersion.CoreMajorVersion,
          VerInfoTbl->ImageVersion.CoreMinorVersion,
          VerInfoTbl->ImageVersion.ProjMajorVersion,
          VerInfoTbl->ImageVersion.ProjMinorVersion,
          VerInfoTbl->ImageVersion.BuildNumber));

  DEBUG ((DEBUG_INFO,  "SVER: %016lX%a\n"
          "FDBG: BLD(%c %a) FSP(%c)\n",
          VerInfoTbl->SourceVersion,
          VerInfoTbl->ImageVersion.Dirty ? "-dirty" : "",
          VerInfoTbl->ImageVersion.BldDebug ? 'D' : 'R',
          sizeof(UINTN) == sizeof(UINT32) ? "IA32" : "X64",
          VerInfoTbl->ImageVersion.FspDebug ? 'D' : 'R'));

  // Print FSP version
  FspInfoHdr = (FSP_INFO_HEADER *)(UINTN)(PcdGet32 (PcdFSPTBase) + FSP_INFO_HEADER_OFF);
  CopyMem (ImageId, &FspInfoHdr->ImageId, sizeof (UINT64));
  if (FspInfoHdr->HeaderRevision <= 5) {
    DEBUG ((DEBUG_INFO, "FSPV: ID(%a) REV(%08X)\n", ImageId, FspInfoHdr->ImageRevision));
  } else {
    DEBUG ((DEBUG_INFO, "FSPV: ID(%a) REV(%04X%04X%04X)\n", ImageId, \
            ((FspInfoHdr->ImageRevision & 0xFFFF0000) >> 16), \
            ((FspInfoHdr->ExtendedImageRevision & 0xFF00) | ((FspInfoHdr->ImageRevision & 0xFF00) >> 8)), \
            (((FspInfoHdr->ExtendedImageRevision & 0x00FF) << 8) | (FspInfoHdr->ImageRevision & 0x00FF))));
  }

  AsmCpuid (1, &CpuSig, NULL, NULL, NULL);
  DEBUG ((DEBUG_INFO, "CPUV: ID(%x) UCODE(%x)\n", CpuSig, \
          RShiftU64 (AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID), 32)));

  DEBUG ((DEBUG_INFO, "Loader global data @ 0x%08X\n", (UINT32)(UINTN)LdrGlobal));
  DEBUG ((DEBUG_INFO, "Run  STAGE1A @ 0x%08X\n", PcdGet32 (PcdStage1ALoadBase)));

  // Load Stage 1B if required
  StageBase = PrepareStage1B (Stage1aParam);

  // Jump into Stage 1B entry
  if (StageBase != 0) {
    PeCoffFindAndReportImageInfo ((UINT32)(UINTN)GET_STAGE_MODULE_BASE (StageBase));
    StageEntry = (STAGE_ENTRY) GET_STAGE_MODULE_ENTRY (StageBase);
    if (StageEntry != NULL) {
      Stage1aParam->Stage1BBase = StageBase;
      StageEntry (Stage1aParam);
    }
  }

  // Error: Stage 1B returned
  CpuHalt ("No valid Stage1B image!");
}
