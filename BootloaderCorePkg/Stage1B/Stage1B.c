/** @file

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Stage1B.h"

/**
  Prepare and load Stage2 into proper location for execution.
  - Load stage2 and check if compressed, if not CPU halted.
  - Check if the image is in flash
  - Do hash verification if failed CPU halted
  - Check if stage2 need to be loaded to high mem.
  - uncompress stage2 and rebase if required.

  @param[in]  Stage1bHob    HOB pointer for Stage1B

  @retval     The base address of the stage.
              0 if loading fails

**/
UINT32
PrepareStage2 (
  IN STAGE1B_HOB   *Stage1bHob
  )
{
  UINT32                    Src;
  UINT32                    Dst;
  UINT32                    Scr;
  UINT32                    DstLen;
  UINT32                    ScrLen;
  EFI_STATUS                Status;
  UINT32                    Data;
  UINT32                    Length;
  UINT32                    Delta;
  STAGE_HDR                *StageHdr;
  LOADER_COMPRESSED_HEADER *Hdr;

  // Load Stage 2 (Compressed)
  Status = GetComponentInfo (FLASH_MAP_SIG_STAGE2, &Src, &Length);
  if (EFI_ERROR (Status)) {
    return 0;
  }
  Dst = PCD_GET32_WITH_ADJUST (PcdStage2LoadBase);

  LoadStage2 (Dst, Src, Length);
  AddMeasurePoint (0x2080);
  Src = Dst;

  if (!IS_COMPRESSED (Src)) {
    CpuHalt ("ERROR: Stage2 must be compressed.");
  }

  Hdr    = (LOADER_COMPRESSED_HEADER *)Src;
  Length = sizeof (LOADER_COMPRESSED_HEADER) + Hdr->CompressedSize;

  if (FixedPcdGetBool (PcdVerifiedBootEnabled)) {
    // Copy and Verify if Stage2 is in Flash
    if (IS_FLASH_SPACE (Src)) {
      Dst = (UINT32)AllocateTemporaryPages (EFI_SIZE_TO_PAGES (Length));
      CopyMem ((VOID *)Dst, (VOID *)Src, Length);
      AddMeasurePoint (0x2090);
      Src = Dst;
      Hdr = (LOADER_COMPRESSED_HEADER *)Src;
    }

    // Verify Stage2 image only.
    Status = DoHashVerify ((CONST UINT8 *)Src, Length, COMP_TYPE_STAGE_2);
    AddMeasurePoint (0x20A0);
    if (EFI_ERROR (Status)) {
      if (Status != RETURN_NOT_FOUND) {
        CpuHalt (NULL);
      }
    }

    // Extend hash of Stage2 image into TPM.
    if (MEASURED_BOOT_ENABLED() ) {
      if (GetBootMode() != BOOT_ON_S3_RESUME) {
        TpmExtendStageHash (COMP_TYPE_STAGE_2);
      }
    }
  }

  // Determine if Stage 2 needs to be loaded into high mem
  if (FixedPcdGetBool (PcdStage2LoadHigh)) {
    Dst = (UINT32)AllocatePages (EFI_SIZE_TO_PAGES (Hdr->Size));
  } else {
    Dst = PCD_GET32_WITH_ADJUST (PcdStage2FdBase);
  }

  // Reserve scratch space needed for decompression
  Status = DecompressGetInfo (Hdr->Signature, Hdr->Data, Hdr->CompressedSize, &DstLen, &ScrLen);
  if (!EFI_ERROR (Status)) {
    // Decompress Stage 2
    Scr    = (UINT32)AllocateTemporaryPages (EFI_SIZE_TO_PAGES (ScrLen));
    Status = Decompress (Hdr->Signature, Hdr->Data, Hdr->CompressedSize, (VOID *)Dst, (VOID *)Scr);
    AddMeasurePoint (0x20B0);
  }
  ASSERT_EFI_ERROR (Status);

  // Rebase Stage2 if required
  if (Dst != PCD_GET32_WITH_ADJUST (PcdStage2FdBase)) {
    // Fix Stage 2 entry and base
    Delta = Dst - PCD_GET32_WITH_ADJUST (PcdStage2FdBase);
    StageHdr = (STAGE_HDR *)Dst;
    StageHdr->Entry += Delta;
    StageHdr->Base  += Delta;
    PeCoffRelocateImage (StageHdr->Base);

    // Fix FSP-S Base Address
    RebaseFspComponent (Delta);

    AddMeasurePoint (0x20C0);
  }

  // Check if Stage 2 contains a compressed payload
  Data = (UINT32)Hdr->Data + Hdr->CompressedSize;
  Src  = ALIGN_UP (Data, 0x100);
  if (IS_COMPRESSED (Src)) {
    // Only compressed payload is supported for now
    Stage1bHob->PayloadBase = Src;
  }
  DEBUG ((DEBUG_INFO, "Loaded STAGE2 @ 0x%08X\n", Dst));

  return Dst;
}

/**
  Load configuration data.

  This function will find external and internal configuration data,
  verify it if verified boot is enabled and append it to config database.

  @param[in] LdrGlobal            Pointer to loader global data.
  @param[in] Stage1bHob           Pointer to Stage1b HOB.

**/
VOID
CreateConfigDatabase (
  IN LOADER_GLOBAL_DATA       *LdrGlobal,
  IN STAGE1B_HOB              *Stage1bHob
  )
{
  EFI_STATUS                Status;
  CDATA_BLOB               *CfgBlob;
  UINT8                    *ExtCfgAddPtr;
  UINT8                    *IntCfgAddPtr;
  UINT8                    *SigPtr;
  UINT8                    *KeyPtr;
  UINT32                    CfgDataBase;
  UINT32                    CfgDataLength;

  //
  // In the config data base, the config data near the data base heaser has high priority.
  // In order to avoid memory allocation and configuration data movement if possible,
  // Here add external config data firstly, then append internal config data.
  // PcdCfgDatabaseSize: It is the size for both internal + external cfg data
  //
  if (PcdGet32 (PcdCfgDataSize) > 0) {
    CfgBlob = (CDATA_BLOB *)LdrGlobal->ConfDataPtr;
    ExtCfgAddPtr = (UINT8 *)CfgBlob + CfgBlob->UsedLength;
    Status = GetComponentInfo (FLASH_MAP_SIG_CFGDATA, &CfgDataBase, &CfgDataLength);
    if (!EFI_ERROR (Status)) {
      Status = LoadExternalConfigData (
               (UINT32)ExtCfgAddPtr,
               CfgDataBase,
               PcdGet32 (PcdCfgDatabaseSize) - CfgBlob->UsedLength
               );
    }
    DEBUG ((
             DEBUG_INFO,
             "Load EXT CFG Data @ 0x%08X:0x%04X ... %r\n",
             ExtCfgAddPtr,
             ((CDATA_BLOB *)ExtCfgAddPtr)->UsedLength,
             Status
             ));
    if (!EFI_ERROR (Status)) {
      if (FeaturePcdGet (PcdVerifiedBootEnabled)) {
        // Verify CFG public key
        CfgBlob = (CDATA_BLOB *)ExtCfgAddPtr;
        SigPtr  = (UINT8 *)CfgBlob + CfgBlob->UsedLength;
        KeyPtr  = SigPtr + RSA2048NUMBYTES;
        Status  = DoRsaVerify ((UINT8 *)CfgBlob, CfgBlob->UsedLength, COMP_TYPE_PUBKEY_CFG_DATA,
                     SigPtr, KeyPtr, Stage1bHob->ConfigDataHash);
        if (EFI_ERROR (Status)) {
          ExtCfgAddPtr = NULL;
        }
      }
      if (ExtCfgAddPtr != NULL) {
        Status = AddConfigData (ExtCfgAddPtr);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_INFO, "Append EXT CFG Data ... %r\n", Status));
        } else {
          Stage1bHob->ConfigDataHashValid = 1;
        }
      }
    }

    IntCfgAddPtr = (UINT8 *)GetBuiltInConfigData ();
    if (IntCfgAddPtr != NULL) {
      Status = AddConfigData (IntCfgAddPtr);
      if (!EFI_ERROR (Status)) {
        // Update the built-in CFGDATA offset
        CfgBlob       = (CDATA_BLOB *) IntCfgAddPtr;
        CfgDataLength = CfgBlob->UsedLength - CfgBlob->HeaderLength;
        CfgBlob       = (CDATA_BLOB *) LdrGlobal->ConfDataPtr;
        CfgBlob->InternalDataOffset = (UINT16) ((CfgBlob->UsedLength - CfgDataLength) >> 2);
      } else {
        DEBUG ((DEBUG_INFO, "Append Built-In CFG Data ... %r\n", Status));
      }
    } else {
      DEBUG ((DEBUG_INFO,  "No built-in CFG Data found !\n"));
    }


  }

  return;
}

/**
  Entry point to the C language phase of Stage 1B.

  Stage1B will find memory initialization. It can be either executed from
  Flash or temporary memory.

  @param[in] Params            Pointer to stage specific parameters.

**/
VOID
EFIAPI
SecStartup2 (
  IN VOID   *Params
  )
{
  STAGE_IDT_TABLE          *IdtTablePtr;
  STAGE1A_HOB              *Stage1aHob;
  LOADER_GLOBAL_DATA       *LdrGlobal;
  LOADER_GLOBAL_DATA       *OldLdrGlobal;
  UINT32                    FspReservedMemBase;
  UINT64                    FspReservedMemSize;
  STAGE1B_HOB               Stage1bHob;
  STAGE1B_HOB              *Stage1bHobInMem;
  UINT32                    StackTop;
  VOID                     *HobList;
  EFI_STATUS                Status;
  UINT32                    MemPoolStart;
  UINT32                    MemPoolEnd;
  UINT32                    MemPoolCurrTop;
  UINT32                    AllocateLen;
  UINT32                    Delta;
  UINT32                    Idx;
  UINT8                    *BufPtr;
  UINT8                    *OldBufPtr;
  VOID                     *SavedLdrHobList;
  LIBRARY_DATA             *LibDataPtr;
  SERVICES_LIST            *ServiceList;
  UINT32                    PcdDatabaseLen;
  PEI_PCD_DATABASE         *PcdDatabaseBin;
  PLATFORMID_CFG_DATA      *PidCfgData;
  UINT8                     PlatformName[PLATFORM_NAME_SIZE + 1];
  DEBUG_LOG_BUFFER_HEADER  *NewLogBuf;
  DEBUG_LOG_BUFFER_HEADER  *OldLogBuf;
  BOOLEAN                   OldStatus;
  PLT_DEVICE_TABLE         *DeviceTable;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ();
  ASSERT (LdrGlobal != NULL);
  LdrGlobal->LoaderStage = LOADER_STAGE_1B;
  AddMeasurePoint (0x2000);

  DEBUG ((DEBUG_INFO, "\n============= Intel Slim Bootloader STAGE1B =============\n"));

  Stage1aHob = (STAGE1A_HOB *) Params;
  if (Stage1aHob->Stage1BBase != 0) {
    PeCoffFindAndReportImageInfo ((UINT32) (UINTN) GET_STAGE_MODULE_BASE (Stage1aHob->Stage1BBase));
  }

  // Reload Exception handler
  UpdateExceptionHandler (NULL);

  // Migrate data from Stage1A HOB to Stage1B HOB
  ZeroMem (&Stage1bHob, sizeof (STAGE1B_HOB));
  Stage1bHob.CarBase  = Stage1aHob->CarBase;
  Stage1bHob.CarTop   = Stage1aHob->CarTop;

  // Remove all services in previous stage
  ServiceList = (SERVICES_LIST *)LdrGlobal->ServicePtr;
  if (ServiceList != NULL) {
    ZeroMem (ServiceList->Header, sizeof (SERVICE_COMMON_HEADER *) * ServiceList->Count);
  }

  // Perform pre-config board init
  BoardInit (PreConfigInit);

  CreateConfigDatabase (LdrGlobal, &Stage1bHob);

  // Overwrite platform ID if CFGDATA contains it
  PidCfgData = (PLATFORMID_CFG_DATA *)FindConfigDataByTag (CDATA_PLATFORMID_TAG);
  if ((PidCfgData != NULL) && (PidCfgData->PlatformId < 32)) {
    DEBUG ((DEBUG_INFO, "Override Board ID to %04X\n", PidCfgData->PlatformId));
    SetPlatformId (PidCfgData->PlatformId);
  }

  BoardInit (PostConfigInit);

  //Get Platform ID and Boot Mode
  DEBUG ((DEBUG_INIT, "BOOT: BP%d \nMODE: %d\nBoardID: 0x%02X\n",
          GetCurrentBootPartition (), GetBootMode (), GetPlatformId ()));

  CopyMem (PlatformName, GetPlatformName (), PLATFORM_NAME_SIZE);
  PlatformName[PLATFORM_NAME_SIZE] = 0;
  DEBUG ((DEBUG_INFO, "PlatformName: %a\n", PlatformName));

  // Perform pre-memory board init
  BoardInit (PreMemoryInit);

  // Initialize memory
  DEBUG ((DEBUG_INIT, "Memory Init\n"));
  AddMeasurePoint (0x2020);
  Status = CallFspMemoryInit (PCD_GET32_WITH_ADJUST (PcdFSPMBase), &HobList);
  AddMeasurePoint (0x2030);
  ASSERT_EFI_ERROR (Status);

  //
  // Reset the system if FSP API returned FSP_STATUS_RESET_REQUIRED status
  //
  if ((Status >= FSP_STATUS_RESET_REQUIRED_COLD) && (Status <= FSP_STATUS_RESET_REQUIRED_8)) {
    DEBUG ((DEBUG_INIT, "FSP Reboot\n"));
    ResetSystem (EfiResetCold);
  }

  FspReservedMemBase = (UINT32)GetFspReservedMemoryFromGuid (
                         HobList,
                         &FspReservedMemSize,
                         &gFspReservedMemoryResourceHobGuid
                         );
  ASSERT (FspReservedMemBase > 0);

  // Prepare Global Data structure
  OldLdrGlobal   = LdrGlobal;
  MemPoolStart   = FspReservedMemBase - PcdGet32 (PcdLoaderReservedMemSize);
  MemPoolEnd     = FspReservedMemBase - PcdGet32 (PcdLoaderHobStackSize);
  MemPoolCurrTop = ALIGN_DOWN (MemPoolEnd - sizeof (LOADER_GLOBAL_DATA), 0x10);
  LdrGlobal      = (LOADER_GLOBAL_DATA *)MemPoolCurrTop;
  MemPoolCurrTop = ALIGN_DOWN (MemPoolCurrTop - sizeof (STAGE_IDT_TABLE), 0x10);
  IdtTablePtr    = (STAGE_IDT_TABLE *)MemPoolCurrTop;
  MemPoolCurrTop = ALIGN_DOWN (MemPoolCurrTop, EFI_PAGE_SIZE);

  if (FixedPcdGetBool (PcdS3DebugEnabled)) {
    SavedLdrHobList = LdrGlobal->LdrHobList;
  }
  CopyMem (LdrGlobal, OldLdrGlobal, sizeof (LOADER_GLOBAL_DATA));

  LdrGlobal->FspHobList        = HobList;
  LdrGlobal->LdrHobList        = NULL;
  LdrGlobal->StackTop          = FspReservedMemBase;
  LdrGlobal->MemPoolEnd        = MemPoolEnd;
  LdrGlobal->MemPoolStart      = MemPoolStart;
  LdrGlobal->MemPoolCurrTop    = MemPoolCurrTop;
  LdrGlobal->MemPoolCurrBottom = MemPoolStart;

  DEBUG_CODE_BEGIN ();
  // Initialize HOB/Stack region with known pattern so that the usage can be detected
  SetMem32 (
    (VOID *)LdrGlobal->MemPoolEnd,
    LdrGlobal->StackTop - LdrGlobal->MemPoolEnd,
    STACK_DEBUG_FILL_PATTERN
    );
  DEBUG_CODE_END ();

  // Setup global data in memory
  LoadIdt (IdtTablePtr, (UINT32)LdrGlobal);
  SetLoaderGlobalDataPointer (LdrGlobal);
  DEBUG ((DEBUG_INFO, "Loader global data @ 0x%08X\n", (UINT32)LdrGlobal));

  if (!FeaturePcdGet (PcdStage1BXip)) {
    // Reload GDT table into memory
    RemapStage ();
  }

  OldStatus = SaveAndSetDebugTimerInterrupt (FALSE);
  InitializeDebugAgent (DEBUG_AGENT_INIT_POSTMEM_SEC, NULL, NULL);
  SaveAndSetDebugTimerInterrupt (OldStatus);

  // Restore PlatformData in new LoaderGlobal
  LdrGlobal->S3DataPtr = AllocatePool (sizeof (S3_DATA));
  if (LdrGlobal->BootMode != BOOT_ON_S3_RESUME) {
    ZeroMem (LdrGlobal->S3DataPtr, sizeof (S3_DATA));
  }

  // Calculate and Save CRC32 if S3_DEBUG is enabled
  if (FixedPcdGetBool (PcdS3DebugEnabled)) {
    S3DebugSaveCRC32 (SavedLdrHobList);
  }

  // Migrate data from CAR to memory
  AllocateLen = Stage1aHob->AllocDataLen;
  OldBufPtr   = (UINT8 *)Stage1aHob->AllocDataBase;
  BufPtr = AllocatePool (AllocateLen);
  CopyMem (BufPtr, OldBufPtr, AllocateLen);
  Delta = BufPtr - OldBufPtr;
  if (LdrGlobal->VerInfoPtr) {
    LdrGlobal->VerInfoPtr = (UINT8 *)LdrGlobal->VerInfoPtr + Delta;
  }
  if (LdrGlobal->HashStorePtr) {
    LdrGlobal->HashStorePtr = (UINT8 *)LdrGlobal->HashStorePtr + Delta;
  }
  if (LdrGlobal->ConfDataPtr) {
    LdrGlobal->ConfDataPtr = (UINT8 *)LdrGlobal->ConfDataPtr + Delta;
  }
  if (LdrGlobal->FlashMapPtr) {
    LdrGlobal->FlashMapPtr = (UINT8 *)LdrGlobal->FlashMapPtr + Delta;
  }
  if (LdrGlobal->LibDataPtr) {
    LibDataPtr  = (LIBRARY_DATA *) ((UINT8 *)LdrGlobal->LibDataPtr + Delta);
    AllocateLen = 0;
    for (Idx = 0; Idx < PcdGet32 (PcdMaxLibraryDataEntry); Idx++) {
      AllocateLen += ALIGN_UP (LibDataPtr[Idx].BufSize, sizeof (UINTN));
    }
    if (AllocateLen > 0) {
      BufPtr = AllocatePool (AllocateLen);
      for (Idx = 0; Idx < PcdGet32 (PcdMaxLibraryDataEntry); Idx++) {
        if (LibDataPtr[Idx].BufBase != 0) {
          CopyMem (BufPtr, (VOID *)LibDataPtr[Idx].BufBase, LibDataPtr[Idx].BufSize);
          LibDataPtr[Idx].BufBase = (UINT32)BufPtr;
          BufPtr += ALIGN_UP (LibDataPtr[Idx].BufSize, sizeof (UINTN));
        }
      }
    }
    LdrGlobal->LibDataPtr = (VOID *)LibDataPtr;
  }
  if (LdrGlobal->PcdDataPtr) {
    LdrGlobal->PcdDataPtr = (UINT8 *)LdrGlobal->PcdDataPtr + Delta;
    PcdDatabaseBin = (PEI_PCD_DATABASE *) (LdrGlobal->PcdDataPtr);
    PcdDatabaseLen = PcdDatabaseBin->Length + PcdDatabaseBin->UninitDataBaseSize;
    SetLibraryData (PcdGet8 (PcdPcdLibId), LdrGlobal->PcdDataPtr, PcdDatabaseLen);
  }
  if (LdrGlobal->ServicePtr) {
    LdrGlobal->ServicePtr = (UINT8 *)LdrGlobal->ServicePtr + Delta;
  }

  if (LdrGlobal->PlatDataPtr) {
    LdrGlobal->PlatDataPtr = (UINT8 *)LdrGlobal->PlatDataPtr + Delta;
  }

  if (LdrGlobal->LogBufPtr) {
    if (PcdGet32 (PcdEarlyLogBufferSize) >= PcdGet32 (PcdLogBufferSize)) {
      LdrGlobal->LogBufPtr   = (UINT8 *)LdrGlobal->LogBufPtr + Delta;
    } else {
      NewLogBuf = (DEBUG_LOG_BUFFER_HEADER *)AllocatePool (PcdGet32 (PcdLogBufferSize));
      OldLogBuf = (DEBUG_LOG_BUFFER_HEADER *)LdrGlobal->LogBufPtr;
      CopyMem ((VOID *)NewLogBuf, (VOID *)OldLogBuf, OldLogBuf->UsedLength);
      NewLogBuf->TotalLength = PcdGet32 (PcdLogBufferSize);
      LdrGlobal->LogBufPtr = NewLogBuf;
    }
  }

  // Copy device table to memory
  if (LdrGlobal->DeviceTable != NULL) {
    DeviceTable = (PLT_DEVICE_TABLE *) LdrGlobal->DeviceTable;
    AllocateLen = DeviceTable->DeviceNumber * sizeof (PLT_DEVICE) + sizeof (PLT_DEVICE_TABLE);
    LdrGlobal->DeviceTable = AllocatePool (AllocateLen);
    CopyMem (LdrGlobal->DeviceTable, DeviceTable, AllocateLen);
  }

  BoardInit (PostMemoryInit);
  AddMeasurePoint (0x2040);

  // Switch to memory-based stack and continue execution at ContinueFunc
  StackTop  = LdrGlobal->StackTop - (sizeof (STAGE2_HOB) + sizeof (STAGE1B_HOB) + 0x40);
  StackTop  = ALIGN_DOWN (StackTop, 0x100);
  Stage1bHobInMem = (STAGE1B_HOB *)StackTop;
  CopyMem (Stage1bHobInMem, &Stage1bHob, sizeof (STAGE1B_HOB));

  DEBUG ((DEBUG_INFO, "Switch to memory stack @ 0x%08X\n", StackTop));
  SwitchStack (ContinueFunc, Stage1bHobInMem, (VOID *)OldLdrGlobal, (VOID *)StackTop);
}

/**
  Entry point to the C language phase of Stage 1B.

  Stage1B will find memory initialization. It can be either executed from
  Flash or temporary memory.

  @param[in] Params            Pointer to stage specific parameters.

**/
VOID
EFIAPI
SecStartup (
  IN VOID   *Params
  )
{
  InitializeDebugAgent (DEBUG_AGENT_INIT_PREMEM_SEC, Params, SecStartup2);
}

/**
  Continue Stage 1B execution.

  This function will continue Stage1B execution with a new memory-based stack.

  @param[in]  Context1        Pointer to STAGE1B_HOB in main memory.
  @param[in]  Context2        Unused.

**/
VOID
ContinueFunc (
  IN VOID                      *Context1,
  IN VOID                      *Context2
  )
{
  STAGE1B_HOB              *Stage1bHob;
  STAGE2_HOB               *Stage2Hob;
  UINT32                    StackBot;
  UINT32                    Dst;
  UINT32                    StackTop;
  EFI_STATUS                Status;
  LOADER_GLOBAL_DATA       *LdrGlobal;
  LOADER_GLOBAL_DATA       *OldLdrGlobal;

  Stage1bHob   = (STAGE1B_HOB *)Context1;
  OldLdrGlobal = (LOADER_GLOBAL_DATA *)Context2;

  BoardInit (PreTempRamExit);

  StackBot = 0;
  DEBUG_CODE_BEGIN ();
  // Print stack and heap usage in temporary memory
  StackBot = DetectUsedStackBottom (OldLdrGlobal->StackTop, PcdGet32 (PcdStage1StackSize));
  DEBUG ((
           DEBUG_INFO,
           "Stage1 stack: 0x%X (0x%X used)\n",
           PcdGet32 (PcdStage1StackSize),
           OldLdrGlobal->StackTop - StackBot
           ));
  DEBUG ((
           DEBUG_INFO,
           "Stage1 heap: 0x%X (0x%X used)\n",
           PcdGet32 (PcdStage1DataSize),
           OldLdrGlobal->MemPoolEnd - OldLdrGlobal->MemPoolCurrTop
           ));
  DEBUG_CODE_END ();

  AddMeasurePoint (0x2050);
  Status = CallFspTempRamExit (PCD_GET32_WITH_ADJUST (PcdFSPMBase), NULL);
  AddMeasurePoint (0x2060);
  ASSERT_EFI_ERROR (Status);

  BoardInit (PostTempRamExit);
  AddMeasurePoint (0x2070);

  // Extend External Config Data hash
  if (MEASURED_BOOT_ENABLED() ) {
    if (GetBootMode() != BOOT_ON_S3_RESUME) {
      if (Stage1bHob->ConfigDataHashValid == 1) {
        TpmExtendPcrAndLogEvent ( 1, TPM_ALG_SHA256, Stage1bHob->ConfigDataHash,
                EV_EFI_VARIABLE_DRIVER_CONFIG, sizeof("Ext Config Data"), (UINT8 *)"Ext Config Data");
      }
    }
  }

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ();
  DEBUG ((DEBUG_INFO, "Memory FSP @ 0x%08X\n", LdrGlobal->StackTop));
  DEBUG ((DEBUG_INFO, "Memory TOP @ 0x%08X\n", LdrGlobal->MemPoolStart));

  Dst = PrepareStage2 (Stage1bHob);
  ASSERT (Dst != 0);

  // Configure stack
  StackTop = ALIGN_DOWN (LdrGlobal->StackTop - sizeof (STAGE2_HOB), 0x10);

  // Build Stage2 HOB
  Stage2Hob = (STAGE2_HOB *)StackTop;
  SetMem (Stage2Hob, sizeof (STAGE2_HOB), 0);
  Stage2Hob->Stage2ExeBase = Dst;
  Stage2Hob->PayloadBase   = Stage1bHob->PayloadBase;

  SwitchStack ((SWITCH_STACK_ENTRY_POINT)GET_STAGE_MODULE_ENTRY (Dst), Stage2Hob, NULL, (VOID *)StackTop);

  // Error: Stage 2 returned!
  CpuHalt (NULL);
}
