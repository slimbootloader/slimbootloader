/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Stage1A.h"

CONST CHAR8*  mBootloaderName = "Intel Slim Bootloader";

/**
  Prepare and load Stage1B into proper location.
  - Load and uncompress the Stage1B image
  - do hash verification if hash verification fails then Halt CPU.

  Stage1B could be compressed or XIP. If compressed, it needs to be
  decompressed into temporary memory for execution.

  @param[in]   Stage1aHob    HOB pointer for Stage1A

  @retval      The base address of the stage.
               0 if loading fails

**/
UINT32
PrepareStage1B (
  IN STAGE1A_HOB   *Stage1aHob
  )
{
  UINT32                    Src;
  UINT32                    Dst;
  UINT32                    Exe;
  UINT32                    Length;
  EFI_STATUS                Status;
  LOADER_COMPRESSED_HEADER *Hdr;

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
  Hdr = (LOADER_COMPRESSED_HEADER *) Src;

  // Verify Stage 1B
  if (FixedPcdGet32(PcdVerifiedBootHashMask) & (1 << COMP_TYPE_STAGE_1B)) {
    if (IS_COMPRESSED (Src)) {
      Length = sizeof (LOADER_COMPRESSED_HEADER) + Hdr->CompressedSize;
    }
    Status = DoHashVerify ((CONST UINT8 *)Src, Length, COMP_TYPE_STAGE_1B);
    AddMeasurePoint (0x10A0);
    if (EFI_ERROR (Status)) {
      if (Status != RETURN_NOT_FOUND) {
        CpuHalt (NULL);
      }
    }
  }

  if (!FeaturePcdGet (PcdStage1BXip)) {
    // Decompress Stage 1B
    Status = Decompress (Hdr->Signature, Hdr->Data, Hdr->CompressedSize, (VOID *)Dst, NULL);
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
  STAGE1A_HOB               Stage1aHob;
  LOADER_GLOBAL_DATA       *LdrGlobal;
  STAGE1A_ASM_HOB          *Stage1aAsmHob;
  STAGE_HDR                *StageHdr;
  STAGE_ENTRY               ContinueEntry;
  EFI_STATUS                Status;
  UINT32                    Delta;
  UINT32                    Dst;
  UINT32                    Src;
  UINT32                    AllocateLen;
  UINT32                    ServiceDataLen;
  UINT32                    LibDataLen;
  UINT32                    LogBufLen;
  CDATA_BLOB               *CfgBlob;
  UINT8                    *BufPtr;
  FLASH_MAP                *FlashMap;
  UINT32                    PcdDatabaseLen;
  UINT8                    *PcdDatabasePtr;
  PEI_PCD_DATABASE         *PcdDatabaseBin;
  UINT32                    PlatformDataLen;
  DEBUG_LOG_BUFFER_HEADER  *LogBufHdr;

  Stage1aAsmHob = (STAGE1A_ASM_HOB *)Params;

  Src = PcdGet32 (PcdStage1AFdBase) + PcdGet32 (PcdFSPTSize);
  PeCoffFindAndReportImageInfo ((UINT32) (UINTN) GET_STAGE_MODULE_BASE (Src));

  LdrGlobal = GetLoaderGlobalDataPointer ();

  // Allocate version info, key store, config data space and library data.
  // Adjust for alignments.
  ServiceDataLen = sizeof (SERVICES_LIST) + sizeof (SERVICE_COMMON_HEADER *) * PcdGet32 (PcdMaxServiceNumber);
  LibDataLen     = PcdGet32 (PcdMaxLibraryDataEntry) * sizeof (LIBRARY_DATA);
  LogBufLen      = PcdGet32 (PcdEarlyLogBufferSize);

  PcdDatabasePtr = (UINT8 *)PcdGet32 (PcdFileDataBase);
  PcdDatabaseBin = (PEI_PCD_DATABASE *)PcdDatabasePtr;
  PcdDatabaseLen = PcdDatabaseBin->Length + PcdDatabaseBin->UninitDataBaseSize;

  PlatformDataLen = GetPlatformDataSize ();

  AllocateLen = sizeof (BOOT_LOADER_VERSION) + PcdGet32 (PcdCfgDatabaseSize) + \
                + LibDataLen + ServiceDataLen + PcdDatabaseLen + PlatformDataLen \
                + LogBufLen + sizeof (UINTN) * 16;
  if (FeaturePcdGet (PcdVerifiedBootEnabled)) {
    AllocateLen += sizeof (HASH_STORE_TABLE);
  }
  if (FeaturePcdGet (PcdFlashMapEnabled) == TRUE) {
    FlashMap = (FLASH_MAP *) (* (UINT32 *)FLASH_MAP_ADDRESS);
    AllocateLen += FlashMap->Length;
  }
  BufPtr = AllocateZeroPool (AllocateLen);

  // Init Stage1A HOB
  ZeroMem (&Stage1aHob, sizeof (STAGE1A_HOB));
  Stage1aHob.CarBase       = Stage1aAsmHob->CarBase;
  Stage1aHob.CarTop        = Stage1aAsmHob->CarTop;
  Stage1aHob.AllocDataBase = (UINT32)BufPtr;
  Stage1aHob.AllocDataLen  = AllocateLen;

  // Update pointers in global data
  if (BufPtr != NULL) {
    // Version Info
    CopyMem (BufPtr, (VOID *)PcdGet32 (PcdVerInfoBase), sizeof (BOOT_LOADER_VERSION));
    LdrGlobal->VerInfoPtr = BufPtr;
    BufPtr += ALIGN_UP (sizeof (BOOT_LOADER_VERSION), sizeof (UINTN));

    // Flash Map
    if (FeaturePcdGet (PcdFlashMapEnabled) == TRUE) {
      CopyMem (BufPtr, (VOID *) (* (UINT32 *)FLASH_MAP_ADDRESS), FlashMap->Length);
      LdrGlobal->FlashMapPtr = BufPtr;
      BufPtr += ALIGN_UP (FlashMap->Length, sizeof (UINTN));
      SetCurrentBootPartition((FlashMap->Attributes & FLASH_MAP_ATTRIBUTES_BACKUP_REGION)? 1 : 0);
    }

    // Key Store
    if (FeaturePcdGet (PcdVerifiedBootEnabled)) {
      CopyMem (BufPtr, (VOID *)PcdGet32 (PcdHashStoreBase), sizeof (HASH_STORE_TABLE));
      LdrGlobal->HashStorePtr = BufPtr;
      BufPtr += ALIGN_UP (sizeof (HASH_STORE_TABLE), sizeof (UINTN));
    }

    // Library data
    LdrGlobal->LibDataPtr = BufPtr;
    BufPtr += ALIGN_UP (LibDataLen, sizeof (UINTN));

    // Service data
    LdrGlobal->ServicePtr = BufPtr;
    ((SERVICES_LIST *)LdrGlobal->ServicePtr)->Count = PcdGet32 (PcdMaxServiceNumber);
    BufPtr += ALIGN_UP (ServiceDataLen, sizeof (UINTN));

    // PcdDatabase, only copy initialized data
    CopyMem (BufPtr, (VOID *)PcdDatabasePtr, PcdDatabaseBin->Length);
    LdrGlobal->PcdDataPtr = BufPtr;
    BufPtr += ALIGN_UP (PcdDatabaseLen, sizeof (UINTN));

    // Log Buffer
    if (LogBufLen > 0) {
      LdrGlobal->LogBufPtr = BufPtr;
      LogBufHdr = (DEBUG_LOG_BUFFER_HEADER *)BufPtr;
      LogBufHdr->Signature        = DEBUG_LOG_BUFFER_SIGNATURE;
      LogBufHdr->HeaderLength     = sizeof (DEBUG_LOG_BUFFER_HEADER);
      LogBufHdr->UsedLength       = LogBufHdr->HeaderLength;
      LogBufHdr->TotalLength      = LogBufLen;
      BufPtr   += ALIGN_UP (PcdGet32 (PcdEarlyLogBufferSize), sizeof (UINTN));
    }

    // Platform specific data
    if (PlatformDataLen > 0) {
      ZeroMem (BufPtr, PlatformDataLen);
      LdrGlobal->PlatDataPtr = BufPtr;
      BufPtr += ALIGN_UP (PlatformDataLen, sizeof (UINTN));
    }

    // Config data
    if (PcdGet32 (PcdCfgDatabaseSize) > 0) {
      CfgBlob = (CDATA_BLOB *)BufPtr;
      CfgBlob->Signature          = CFG_DATA_SIGNATURE;
      CfgBlob->HeaderLength       = sizeof (CDATA_BLOB);
      CfgBlob->UsedLength         = sizeof (CDATA_BLOB);
      CfgBlob->InternalDataOffset = 0;
      CfgBlob->TotalLength        = PcdGet32 (PcdCfgDatabaseSize);
      LdrGlobal->ConfDataPtr      = BufPtr;
      BufPtr += ALIGN_UP (CfgBlob->TotalLength, sizeof (UINTN));
    }
  }

  // Call board hook to enable debug
  BoardInit (PostTempRamInit);
  AddMeasurePoint (0x1040);

  if (DebugCodeEnabled()) {
    DEBUG ((DEBUG_INFO, "\n============= %a STAGE1A =============\n",mBootloaderName));
  } else {
    DEBUG ((DEBUG_INIT, "\n%a\n", mBootloaderName));
  }

  if ( (BufPtr == NULL) ||
       ((Stage1aHob.AllocDataBase + Stage1aHob.AllocDataLen) < (UINT32)(UINTN)BufPtr) ) {
    CpuHalt ("Insufficant memory pool!\n");
  }

  ContinueEntry = ContinueFunc;
  if (!FeaturePcdGet (PcdStage1AXip)) {
    // Need to relocate itself into temporary memory
    Dst = PcdGet32 (PcdStage1ALoadBase);
    Src = PcdGet32 (PcdStage1AFdBase) + PcdGet32 (PcdFSPTSize);
    CopyMem ((VOID *)Dst, (VOID *)Src, PcdGet32 (PcdStage1AFvSize));
    Delta    = Dst - Src;
    StageHdr = (STAGE_HDR *)Dst;
    StageHdr->Entry += Delta;
    StageHdr->Base  += Delta;
    Status = PeCoffRelocateImage (StageHdr->Base);
    if (!EFI_ERROR (Status)) {
      EnableCodeExecution ();
      ContinueEntry = (STAGE_ENTRY) ((UINT32)ContinueFunc + Delta);
    } else {
      CpuHalt ("Relocation failed!\n");
    }
  }

  // Control will get into ContinueFunc() for execution
  ContinueEntry (&Stage1aHob);
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
  LOADER_GLOBAL_DATA       *LdrGlobal;
  STAGE1A_ASM_HOB          *Stage1aAsmHob;
  UINT32                    StackTop;
  UINT64                    TimeStamp;

  TimeStamp     = ReadTimeStamp ();
  Stage1aAsmHob = (STAGE1A_ASM_HOB *)Params;

  // Init global data
  LdrGlobal = &LdrGlobalData;
  ZeroMem (LdrGlobal, sizeof (LOADER_GLOBAL_DATA));
  StackTop = Stage1aAsmHob->CarBase + PcdGet32 (PcdStage1StackSize);
  LdrGlobal->Signature             = LDR_GDATA_SIGNATURE;
  LdrGlobal->LoaderStage           = LOADER_STAGE_1A;
  LdrGlobal->StackTop              = StackTop;
  LdrGlobal->MemPoolEnd            = StackTop + PcdGet32 (PcdStage1DataSize);
  LdrGlobal->MemPoolStart          = StackTop;
  LdrGlobal->MemPoolCurrTop        = LdrGlobal->MemPoolEnd;
  LdrGlobal->MemPoolCurrBottom     = LdrGlobal->MemPoolStart;
  LdrGlobal->DebugPrintErrorLevel  = PcdGet32 (PcdDebugPrintErrorLevel);
  LdrGlobal->PerfData.PerfIndex    = 2;
  LdrGlobal->PerfData.FreqKhz      = GetCpuTscFreqency ();
  LdrGlobal->PerfData.TimeStamp[0] = Stage1aAsmHob->TimeStamp | 0x1000000000000000ULL;
  LdrGlobal->PerfData.TimeStamp[1] = TimeStamp  | 0x1010000000000000ULL;
  // Set the Loader features to default here.
  // Any platform (board init lib) can update these according to
  // the config data passed in or these defaults remain
  LdrGlobal->LdrFeatures           = FEATURE_MEASURED_BOOT | FEATURE_ACPI;

  LoadIdt (&IdtTable, (UINT32)LdrGlobal);
  SetLoaderGlobalDataPointer (LdrGlobal);

  InitializeDebugAgent (DEBUG_AGENT_INIT_PREMEM_SEC, Params, SecStartup2);
}

/**
  Continue Stage 1A execution.

  This function will continue Stage1A execution for a new location.
  - Migrates the stage1A HOB data to stage1B HOB
  - Prints BootLoader Version information
  - Allocate Version, key store and config data and populate the data.
  - Load stage1B, Halt CPU if stage1B returns.

  @param[in]  Params          Pointer to STAGE1A_ASM_HOB in main memory.

**/
VOID
EFIAPI
ContinueFunc (
  IN VOID  *Params
  )
{
  STAGE1A_HOB              *Stage1aHob;
  LOADER_GLOBAL_DATA       *LdrGlobal;
  STAGE_ENTRY               StageEntry;
  UINT32                    StageBase;
  UINT8                     ImageId[9];
  BOOT_LOADER_VERSION      *VerInfoTbl;
  FSP_INFO_HEADER          *FspInfoHdr;

  LdrGlobal     = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  AddMeasurePoint (0x1060);

  if (!FeaturePcdGet (PcdStage1AXip)) {
    PostStageRelocation ();
  }

  // Migrate data from Stage1A HOB to Stage1B HOB
  Stage1aHob = (STAGE1A_HOB *)Params;

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
          "FDBG: BLD(%c) FSP(%c)\n",
          VerInfoTbl->SourceVersion,
          VerInfoTbl->ImageVersion.Dirty ? "-dirty" : "",
          VerInfoTbl->ImageVersion.BldDebug ? 'D' : 'R',
          VerInfoTbl->ImageVersion.FspDebug ? 'D' : 'R'));

  // Print FSP version
  FspInfoHdr = (FSP_INFO_HEADER *) (PcdGet32 (PcdFSPTBase) + FSP_INFO_HEADER_OFF);
  CopyMem (ImageId, &FspInfoHdr->ImageId, sizeof (UINT64));
  DEBUG ((DEBUG_INFO, "FSPV: ID(%a) REV(%08X)\n", ImageId, FspInfoHdr->ImageRevision));

  DEBUG ((DEBUG_INFO, "Loader global data @ 0x%08X\n", (UINT32)LdrGlobal));
  DEBUG ((DEBUG_INFO, "Run  STAGE1A @ 0x%08X\n", PcdGet32 (PcdStage1ALoadBase)));

  // Load Stage 1B if required
  StageBase = PrepareStage1B (Stage1aHob);

  // Jump into Stage 1B entry
  if (StageBase != 0) {
    PeCoffFindAndReportImageInfo ((UINT32) GET_STAGE_MODULE_BASE (StageBase));
    StageEntry = (STAGE_ENTRY) GET_STAGE_MODULE_ENTRY (StageBase);
    if (StageEntry != NULL) {
      Stage1aHob->Stage1BBase = StageBase;
      StageEntry (Stage1aHob);
    }
  }

  // Error: Stage 1B returned
  CpuHalt ("No valid Stage1B image!");
}
