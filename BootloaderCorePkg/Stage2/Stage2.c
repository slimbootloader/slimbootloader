/** @file

  Copyright (c) 2016 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2.h"

/**
  Callback function to add performance measure point during component loading.

  @param[in]  ProgressId    Component loading progress ID code.
  @param[in]  CbInfo        Component Call Back Info

**/
VOID
LoadComponentCallback (
  IN  UINT32                     ProgressId,
  IN  COMPONENT_CALLBACK_INFO   *CbInfo
  )
{
  UINT8             BootMode;

  // Update Progress Info
  switch (ProgressId) {
  case PROGESS_ID_LOCATE:
    AddMeasurePoint (0x3110);
    break;
  case PROGESS_ID_COPY:
    AddMeasurePoint (0x3120);
    break;
  case PROGESS_ID_AUTHENTICATE:
    //Check the boot mode
    BootMode = GetBootMode();
    if (MEASURED_BOOT_ENABLED() && (BootMode != BOOT_ON_S3_RESUME)) {
      // Extend the stage hash
      ExtendStageHash (CbInfo);
    }
    AddMeasurePoint (0x3130);
    break;
  case PROGESS_ID_DECOMPRESS:
    AddMeasurePoint (0x3140);
    break;
  default:
    break;
  }


}

/**
  Prepare and load payload into proper location for execution.

  @param[in]  Stage2Param    Param pointer for Stage2

  @retval     The base address of the payload.
              0 if loading fails.

**/
UINT32
PreparePayload (
  IN STAGE2_PARAM   *Stage2Param
  )
{
  EFI_STATUS                     Status;
  UINT32                         Dst;
  UINT32                         DstLen;
  VOID                          *DstAdr;
  UINT32                         PayloadId;
  UINT32                         ContainerSig;
  UINT32                         ComponentName;
  UINT8                          BootMode;
  UINT64                         SignatureBuf;

  BootMode = GetBootMode();
  //
  // Force PayloadId to 0 during firmware update mode.
  //
  if (BootMode == BOOT_ON_FLASH_UPDATE) {
    SetPayloadId(0);
  }
  // Load payload to PcdPayloadLoadBase.
  PayloadId   = GetPayloadId ();
  if (BootMode == BOOT_ON_FLASH_UPDATE) {
    ContainerSig  = COMP_TYPE_PAYLOAD_FWU;
    ComponentName = FLASH_MAP_SIG_FWUPDATE;
  } else {
    if (PayloadId == 0) {
      ContainerSig  = COMP_TYPE_PAYLOAD;
      ComponentName = FLASH_MAP_SIG_PAYLOAD;
    } else {
      ContainerSig  = FLASH_MAP_SIG_EPAYLOAD;
      ComponentName = PayloadId;
    }
  }
  SignatureBuf = ComponentName;
  DEBUG ((DEBUG_INFO, "Loading Payload ID %4a\n", (CHAR8 *)&SignatureBuf));

  Dst = PcdGet32 (PcdPayloadExeBase);
  if (FixedPcdGetBool (PcdPayloadLoadHigh)) {
    PayloadId = GetPayloadId();
    if ((PayloadId != LINX_PAYLOAD_ID_SIGNATURE) && (PayloadId != UEFI_PAYLOAD_ID_SIGNATURE)) {
      Dst = 0;
    }
  }

  AddMeasurePoint (0x3100);
  DstLen = 0;
  DstAdr = (VOID *)(UINTN)Dst;
  Status = LoadComponentWithCallback (ContainerSig, ComponentName,
                                      &DstAdr, &DstLen, LoadComponentCallback);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Loading payload error - %r !", Status));
    return 0;
  }

  AddMeasurePoint (0x3150);

  Dst = (UINT32)(UINTN)DstAdr;
  Stage2Param->PayloadActualLength = DstLen;
  DEBUG ((DEBUG_INFO, "Load Payload ID 0x%08X @ 0x%08X\n", PayloadId, Dst));
  return Dst;
}

/**
  Load Linux kernel with command line and InitRD support.

  @param[in]  Dst           Pointer to the Linux kernel image
  @param[out] PldEntry      Pointer to store the payload entry point

  @retval EFI_SUCCESS       Linux kernel loaded successfully
  @retval Others            Error loading Linux kernel components

**/
EFI_STATUS
LoadLinuxKernel (
  IN  UINT32         *Dst,
  OUT PAYLOAD_ENTRY  *PldEntry
  )
{
  EFI_STATUS  Status;
  VOID       *InitRd;
  UINT32      InitRdLen;
  UINT8      *CmdLine;
  UINT32      CmdLineLen;

  DEBUG ((DEBUG_INFO, "BzImage Format Payload\n"));

  InitRd     = NULL;
  InitRdLen  = 0;
  CmdLine    = NULL;
  CmdLineLen = 0;

  Status = LoadComponent (FLASH_MAP_SIG_EPAYLOAD, SIGNATURE_32 ('C', 'M', 'D', 'L'),
                          (VOID **)&CmdLine, &CmdLineLen);
  if (!EFI_ERROR (Status)) {
    // Limit max command line length
    if (CmdLineLen > CMDLINE_LENGTH_MAX - 1) {
      CmdLineLen = CMDLINE_LENGTH_MAX - 1;
    }
    CmdLine[CmdLineLen] = 0;
    DEBUG ((DEBUG_INFO, "Kernel command line: \n%a\n", CmdLine));
  }

  // Try to load InitRd if it exists. If loading fails, continue booting
  Status = LoadComponent (FLASH_MAP_SIG_EPAYLOAD, SIGNATURE_32 ('I', 'N', 'R', 'D'),
                          (VOID **)&InitRd, &InitRdLen);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "InitRD is loaded at 0x%x:0x%x\n", InitRd, InitRdLen));
  }

  *PldEntry = (PAYLOAD_ENTRY)(UINTN)LinuxBoot;
  Status    = LoadBzImage (Dst, InitRd, InitRdLen, CmdLine, CmdLineLen);

  return Status;
}


/**
  Normal boot flow.

  @param[in]   Stage2Param            STAGE2_PARAM Param pointer.

**/
VOID
NormalBootPath (
  IN STAGE2_PARAM    *Stage2Param
  )
{
  UINT32                         *Dst;
  PAYLOAD_ENTRY                   PldEntry;
  VOID                           *PldHobList;
  UINTN                          PldBase;
  LOADER_GLOBAL_DATA             *LdrGlobal;
  EFI_STATUS                      Status;
  BOOLEAN                         CallBoardNotify;
  UINT32                          PayloadId;
  UINT32                          UefiSig;
  UINT32                          HobSize;
  UINT16                          PldMachine;
  LOADED_PAYLOAD_INFO             PayloadInfo;
  UNIVERSAL_PAYLOAD_EXTRA_DATA   *PldImgInfo;
  FIT_IMAGE_CONTEXT               Context;
  FIT_RELOCATE_ITEM              *RelocateTable;
  INTN                            Delta;
  UINTN                           Index;
  UNIVERSAL_PAYLOAD_BASE         *PayloadBase;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();

  // Load payload
  Dst = (UINT32 *)(UINTN)PreparePayload (Stage2Param);
  if (Dst == NULL) {
    // Unable to recover non-FWU payload, so avoid triggering of recovery flow
    if (PcdGetBool (PcdSblResiliencyEnabled) && GetBootMode () != BOOT_ON_FLASH_UPDATE) {
      StopTcoTimer ();
    }
    CpuHalt ("Failed to load payload !");
  }

  BoardInit (PostPayloadLoading);
  AddMeasurePoint (0x31A0);

  UefiSig  = 0;
  PldBase  = 0;
  PldEntry = NULL;
  PldMachine = IS_X64 ? IMAGE_FILE_MACHINE_X64 : IMAGE_FILE_MACHINE_I386;

  Status  = EFI_SUCCESS;
  if (FeaturePcdGet (PcdPe32SupportEnabled) && (Dst[0] == 0x00005A4D)) {
    // Check if this is actually a Linux kernel that happens to have PE32 signature
    if (FeaturePcdGet (PcdLinuxPayloadEnabled) && IsBzImage (Dst)) {
      DEBUG ((DEBUG_INFO, "Detected Linux kernel with PE32 signature - loading as Linux kernel\n"));
      Status = LoadLinuxKernel (Dst, &PldEntry);
    } else {
      // It is a PE format
      DEBUG ((DEBUG_INFO, "PE32 Format Payload\n"));
      Status = PeCoffRelocateImage ((UINT32)(UINTN)Dst);
      if (!EFI_ERROR(Status)) {
        Status = PeCoffLoaderGetMachine (Dst, &PldMachine);
        if (!EFI_ERROR(Status)) {
          Status = PeCoffLoaderGetEntryPoint (Dst, (VOID *)&PldEntry);
        }
      }
    }
  } else if (IsFitImage (Dst, &Context)) {
    DEBUG ((DEBUG_INFO, "FIT Format Payload\n"));
    Context.PayloadBaseAddress = (UINTN)Dst;
    if (Context.PayloadBaseAddress != Context.PayloadLoadAddress) {
      RelocateTable = (FIT_RELOCATE_ITEM *)(UINTN)(Context.PayloadBaseAddress + Context.RelocateTableOffset);
      Delta         = (INTN)(Context.PayloadBaseAddress - Context.PayloadLoadAddress);
      Context.PayloadEntryPoint += Delta;
      for (Index = 0; Index < Context.RelocateTableCount; Index++) {
        if ((RelocateTable[Index].RelocateType == 10) || (RelocateTable[Index].RelocateType == 3)) {
          *((UINT64 *)(UINTN)(Context.PayloadBaseAddress + RelocateTable[Index].Offset)) += Delta;
        }
      }
    }
    DEBUG ((DEBUG_INFO, "Image Base: 0x%08lx, EntryPoint: 0x%08lx\n", Context.PayloadLoadAddress, Context.PayloadEntryPoint));
    PayloadBase = BuildGuidHob (&gUniversalPayloadBaseGuid, sizeof (UNIVERSAL_PAYLOAD_BASE));
    PayloadBase->Entry = (EFI_PHYSICAL_ADDRESS)Context.ImageBase;

    // ASSUME 64bit payload. Need get arch info if need support 32bit payload
    PldMachine = IMAGE_FILE_MACHINE_X64;
    PldEntry   = (PAYLOAD_ENTRY)(UINTN)Context.PayloadEntryPoint;
  } else if (FeaturePcdGet (PcdFvSupportEnabled) && (Dst[10] == EFI_FVH_SIGNATURE)) {
    // It is a FV format
    DEBUG ((DEBUG_INFO, "FV Format Payload\n"));
    UefiSig = Dst[0];
    Status  = LoadFvImage (Dst, Stage2Param->PayloadActualLength, (VOID **)&PldEntry, &PldMachine);
  } else if (FeaturePcdGet (PcdElfSupportEnabled) && IsElfFormat ((CONST UINT8 *)Dst)) {
    DEBUG ((DEBUG_INFO, "ELF Format Payload\n"));
    // Assume Universal Payload first
    ZeroMem (&PayloadInfo, sizeof(PayloadInfo));
    Status = LoadElfPayload (Dst, &PayloadInfo);
    if (!EFI_ERROR(Status)) {
      if (PayloadInfo.Info.Identifier == UNIVERSAL_PAYLOAD_IDENTIFIER) {
        DEBUG ((DEBUG_INFO, "Universal Payload %a v%08X\n", PayloadInfo.Info.ImageId, PayloadInfo.Info.Revision));
        UefiSig    = UNIVERSAL_PAYLOAD_IDENTIFIER;
        HobSize    = sizeof (UNIVERSAL_PAYLOAD_EXTRA_DATA) + sizeof(UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY) * PayloadInfo.ImageCount;
        PldImgInfo = (UNIVERSAL_PAYLOAD_EXTRA_DATA *)BuildGuidHob (&gUniversalPayloadExtraDataGuid, HobSize);
        if (PldImgInfo != NULL) {
          ZeroMem (PldImgInfo, HobSize);
          PldImgInfo->Header.Revision = 0;
          PldImgInfo->Count = PayloadInfo.ImageCount;
          CopyMem (&PldImgInfo->Entry[0], &PayloadInfo.LoadedImage, sizeof(UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY) * PayloadInfo.ImageCount);
        }
      }
      PldMachine = (UINT16)PayloadInfo.Machine;
      PldEntry   = (PAYLOAD_ENTRY)PayloadInfo.EntryPoint;
    }
  } else {
    if (FeaturePcdGet (PcdLinuxPayloadEnabled)) {
      if (IsBzImage (Dst)) {
        Status = LoadLinuxKernel (Dst, &PldEntry);
      }
    }

    if (PldEntry == NULL) {
      // Assume RAW Binary Payload if no other format match
      // PcdPayloadExeBase MUST be the same as payload binary's TEXT BASE
      // Jump to the entry point directly
      DEBUG ((DEBUG_INFO, "Assume RAW Format Payload\n"));
      PldEntry = (PAYLOAD_ENTRY) (UINTN)Dst;
    }
  }
  AddMeasurePoint (0x31B0);
  ASSERT_EFI_ERROR (Status);

  BoardInit (EndOfStages);

  PayloadId = GetPayloadId ();
  if (PayloadId == 0) {
    // For built-in payload including OsLoader and FirmwareUpdate, it will handle
    // notification through SBL platform services, so do not call notifications
    // here.
    CallBoardNotify = FALSE;
  } else if ((PayloadId == UEFI_PAYLOAD_ID_SIGNATURE) && (UefiSig != 0)) {
    // Current open sourced UEFI payload does not call any FSP notifications,
    // but some customized UEFI payload will. The 1st DWORD in UEFI payload image
    // will be used to indicate if it will handle FSP notifications.
    CallBoardNotify = FALSE;
  } else if (UefiSig == UNIVERSAL_PAYLOAD_IDENTIFIER) {
    // Expect Universal UEFI payload would send FSP notifications.
    CallBoardNotify = FALSE;
  } else {
    CallBoardNotify = TRUE;
  }

  // Only delay MpInitDone for OsLoader
  if ((PayloadId != 0) || (GetBootMode() == BOOT_ON_FLASH_UPDATE)) {
    Status = MpInit (EnumMpInitDone);
    AddMeasurePoint (0x31C0);
  }

  if (CallBoardNotify) {
    BoardNotifyPhase (ReadyToBoot);
    AddMeasurePoint (0x31D0);

    BoardNotifyPhase (EndOfFirmware);
    AddMeasurePoint (0x31E0);
  }

  AddMeasurePoint (0x31F0);

  DEBUG ((DEBUG_INFO, "HOB @ 0x%08X\n", LdrGlobal->LdrHobList));
  PldHobList = BuildExtraInfoHob (Stage2Param);
  #if (FixedPcdGetBool (PcdHandOffFdtEnable))
    PldHobList = BuildFdtForUpl ();
    ASSERT (PldHobList != NULL);
  #endif

  DEBUG_CODE_BEGIN ();
  PrintStackHeapInfo ();
  DEBUG_CODE_END ();

  UpdateFpdtSblTable ();
  // FWU payload is the only payload in SBL scope, so stop TCO
  // timer if another payload is set to be launched
  if (PcdGetBool (PcdSblResiliencyEnabled) && GetBootMode () != BOOT_ON_FLASH_UPDATE) {
    StopTcoTimer ();
    ClearFailedBootCount ();
  }

  DEBUG ((DEBUG_INFO, "Payload entry: 0x%08X\n", PldEntry));
  if (PldEntry != NULL) {
    if (IS_X64) {
      if (PldMachine == IMAGE_FILE_MACHINE_I386) {
        DEBUG ((DEBUG_INFO, "Thunk back to x86 mode\n"));
      }
    } else {
      if (PldMachine == IMAGE_FILE_MACHINE_X64) {
        DEBUG ((DEBUG_INFO, "Switch to x64 mode\n"));
      }
    }
    DEBUG ((DEBUG_INIT, "Jump to payload\n\n"));
    if (PldMachine == IMAGE_FILE_MACHINE_X64) {
      // Need to call in x64 long mode
      Execute64BitCode ((UINT64)(UINTN)PldEntry, (UINT64)(UINTN)PldHobList,
                        (UINT64)(UINTN)PldBase, FALSE);
    } else {
      // Need to call in x86 compatible mode
      Execute32BitCode ((UINT64)(UINTN)PldEntry, (UINT64)(UINTN)PldHobList,
                        (UINT64)(UINTN)PldBase, FALSE);
    }

  }
}

/**
  S3 resume flow.

  @param Stage2Param            STAGE2_PARAM Param pointer.

**/
VOID
S3ResumePath (
  STAGE2_PARAM   *Stage2Param
  )
{
  LOADER_GLOBAL_DATA             *LdrGlobal;
  S3_DATA                        *S3Data;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  S3Data    = (S3_DATA *)LdrGlobal->S3DataPtr;

  MpInit (EnumMpInitDone);
  AddMeasurePoint (0x31C0);

  // Call the board notification
  BoardInit (EndOfStages);

  // Call board and FSP Notify ReadyToBoot
  BoardNotifyPhase (ReadyToBoot);
  AddMeasurePoint (0x31D0);

  // Call board and FSP Notify ReadyToBoot
  BoardNotifyPhase (EndOfFirmware);
  AddMeasurePoint (0x31E0);

  DEBUG_CODE_BEGIN ();
  PrintStackHeapInfo ();
  DEBUG_CODE_END ();

  // Update FPDT table
  UpdateFpdtS3Table (S3Data->AcpiBase);

  AddMeasurePoint (0x31F0);

  // No payload is executed in S3 resume, so stop TCO timer in all cases
  if (PcdGetBool (PcdSblResiliencyEnabled)) {
    StopTcoTimer ();
    ClearFailedBootCount ();
  }

  // Find Wake Vector and Jump to OS
  FindAcpiWakeVectorAndJump (S3Data->AcpiBase);
}

/**
  Entry point to the C language phase of Stage2.

  Stage2 will complete the remaining system initialization and load payload.
  It will be executed from memory.

  @param Params            STAGE2_PARAM Param pointer.

**/
VOID
EFIAPI
SecStartup (
  VOID *Params
  )
{
  EFI_STATUS                      Status;
  EFI_STATUS                      SubStatus;
  STAGE2_PARAM                   *Stage2Param;
  VOID                           *NvsData;
  UINT32                          MrcDataLen;
  VOID                           *MemPool;
  UINT32                          Delta;
  UINT32                          AcpiGnvs;
  UINT32                          AcpiBase;
  LOADER_GLOBAL_DATA             *LdrGlobal;
  UINT8                           BootMode;
  S3_DATA                        *S3Data;
  PLATFORM_SERVICE               *PlatformService;
  VOID                           *SmbiosEntry;
  BOOLEAN                         SplashPostPci;
  UINT8                           SmmRebaseMode;

  // Initialize HOB
  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  ASSERT (LdrGlobal != NULL);
  LdrGlobal->LoaderStage = LOADER_STAGE_2;

  DEBUG ((DEBUG_INFO, "\n============= Intel Slim Bootloader STAGE2 =============\n"));
  AddMeasurePoint (0x3000);

  // Unmap previous stage
  UnmapStage ();

  // Deallocate temporary memory used by previous stage
  FreeTemporaryMemory (NULL);

  if (IS_X64) {
    // Build full physical space 1:1 mapping page table
    CreateIdentityMappingPageTables (0);
  }

  // Init all services
  InitializeService ();

  BootMode = GetBootMode ();

  // Update Patchable PCD in case Stage2 is loaded into high mem
  Stage2Param = (STAGE2_PARAM *)Params;
  Delta = Stage2Param->Stage2ExeBase - PCD_GET32_WITH_ADJUST (PcdStage2FdBase);
  Status = PcdSet32S (PcdFSPSBase,             PCD_GET32_WITH_ADJUST (PcdFSPSBase) + Delta);
  Status = PcdSet32S (PcdAcpiTablesAddress,    PCD_GET32_WITH_ADJUST (PcdAcpiTablesAddress) + Delta);
  Status = PcdSet32S (PcdGraphicsVbtAddress,   PCD_GET32_WITH_ADJUST (PcdGraphicsVbtAddress) + Delta);
  Status = PcdSet32S (PcdSplashLogoAddress,    PCD_GET32_WITH_ADJUST (PcdSplashLogoAddress) + Delta);

  LdrGlobal->LdrHobList = (VOID *)(UINTN)LdrGlobal->MemPoolEnd;
  BuildHobHandoffInfoTable (
    BootMode,
    (EFI_PHYSICAL_ADDRESS) (UINTN)LdrGlobal->LdrHobList,
    (UINTN)PcdGet32 (PcdLoaderHobStackSize)
    );

  InitializeDebugAgent (DEBUG_AGENT_INIT_DXE_LOAD, NULL, NULL);

  // Call FspSiliconInit
  BoardInit (PreSiliconInit);

  AddMeasurePoint (0x3010);

  // Save NVS data
  NvsData = GetFspNvsDataBuffer (LdrGlobal->FspHobList, &MrcDataLen);
  if ((NvsData != NULL) && (MrcDataLen > 0)) {
    SubStatus = SaveNvsData (NvsData, MrcDataLen);
    DEBUG ((DEBUG_INFO, "Save MRC Training Data (0x%p 0x%06X) ... %r\n", NvsData, MrcDataLen, SubStatus));
  }

  DEBUG ((DEBUG_INIT, "Silicon Init\n"));
  AddMeasurePoint (0x3020);
  Status = CallFspSiliconInit ();

  FspResetHandler(Status);
  ASSERT_EFI_ERROR (Status);

  Status = FspVariableHandler(Status, CallFspMultiPhaseSiliconInit);
  ASSERT_EFI_ERROR(Status);

  Status = FspMultiPhaseSiliconInitHandler();
  if (Status == EFI_UNSUPPORTED) {
    DEBUG((DEBUG_INFO, "FspMultiPhaseSiliconInitHandler() returned EFI_UNSUPPORTED.\n"));
  } else {
    ASSERT_EFI_ERROR(Status);
  }

  AddMeasurePoint (0x3030);
  FspResetHandler (Status);

  BoardInit (PostSiliconInit);
  AddMeasurePoint (0x3040);

#if FixedPcdGetBool (PcdEnableCryptoPerfTest)
  CryptoPerfTest();
#endif

  // Create base HOB
  BuildBaseInfoHob (Stage2Param);

  // Display splash
  SplashPostPci = FALSE;
  if (FixedPcdGetBool (PcdSplashEnabled)) {
    Status = DisplaySplash ();
    AddMeasurePoint (0x3050);
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((DEBUG_INFO, "Display splash early setup failed, will retry after PCI enumeration.\n"));
      SplashPostPci = TRUE;
    }
  }

  //
  // Update SMM_REBASE_AUTO and SMM_REBASE_AUTO_NOSMRR to specific rebase modes
  // Currently, there are three types of payloads:
  //
  // Payload A: Payload without SMM support
  //            Examples: OsLoader payload, UEFI payload without SMM feature support.
  //
  // Payload B: Payload with full SMM support
  //            Examples: UEFI payload built with EDK2 PiSmmCpuDxeSmm that supports SMM relocation.
  //
  // Payload C: Payload with partial SMM support (excluding SMM rebase)
  //            Examples: UEFI payload with EDK2 PiSmmCpuDxeSmm relying on gSmmBaseHobGuid HOB.
  //                      Note: Starting in 2024, EDK2 PiSmmCpuDxeSmm removed SMM relocation
  //                      and now depends on the bootloader to handle SMM rebasing and to build
  //                      a gSmmBaseHobGuid HOB.
  //  +------------+--------------------------+---------------------------------------------------+
  //  |   Type     | Expected Rebase Mode     | Comments                                          |
  //  +------------+--------------------------+---------------------------------------------------+
  //  | Payload A  | SMM_REBASE_ENABLE        | SBL rebases SMM and configures SMRRs              |
  //  +------------+--------------------------+---------------------------------------------------+
  //  | Payload B  | SMM_REBASE_DISABLE       | Payload handles SMM rebasing and configures SMRRs |
  //  +------------+--------------------------+---------------------------------------------------+
  //  | Payload C  | SMM_REBASE_ENABLE_NOSMRR | SBL rebases SMM, while the payload configures SMRR|
  //  +------------+--------------------------+---------------------------------------------------+
  //
  // Based on the information above, the UEFI payload can be built into different types of payloads.
  // SBL must set PcdSmmRebaseMode appropriately according to the type of payload.
  //
  SmmRebaseMode = PcdGet8 (PcdSmmRebaseMode);
  if ((SmmRebaseMode == SMM_REBASE_AUTO) || (SmmRebaseMode == SMM_REBASE_AUTO_NOSMRR)) {
    if (GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE) {
      if (SmmRebaseMode == SMM_REBASE_AUTO_NOSMRR) {
        SmmRebaseMode = SMM_REBASE_ENABLE_NOSMRR;
      } else {
        SmmRebaseMode = SMM_REBASE_DISABLE;
      }
    } else {
      SmmRebaseMode = SMM_REBASE_ENABLE;
    }
    (VOID) PcdSet8S (PcdSmmRebaseMode, SmmRebaseMode);
  }

  // MP Init phase 1
  Status = MpInit (EnumMpInitWakeup);
  AddMeasurePoint (0x3060);

  // MP Init phase 2
  if (!EFI_ERROR (Status)) {
    Status = MpInit (EnumMpInitRun);
    AddMeasurePoint (0x3080);
  }
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate SMBIOS tables' memory, set Base and call Smbios init
  //
  if (FixedPcdGetBool (PcdSmbiosEnabled)) {
    SmbiosEntry = AllocateZeroPool (PcdGet16(PcdSmbiosTablesSize));
    Status = PcdSet32S (PcdSmbiosTablesBase, (UINT32)(UINTN)SmbiosEntry);
    Status = SmbiosInit ();
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "SMBIOS init Status = %r\n", Status));
    }
  }

  // PCI Enumeration
  BoardInit (PrePciEnumeration);
  AddMeasurePoint (0x3090);

  if (FixedPcdGetBool (PcdPciEnumEnabled)) {
    MemPool = AllocateTemporaryMemory (0);
    DEBUG ((DEBUG_INIT, "PCI Enum\n"));
    Status = PciEnumeration (MemPool);
    AddMeasurePoint (0x30A0);
    UpdateGraphicsHob ();
    BoardInit (PostPciEnumeration);
    AddMeasurePoint (0x30B0);

    if (!EFI_ERROR (Status)) {
      if (FeaturePcdGet(PcdEnableFwuNotify) || (BootMode != BOOT_ON_FLASH_UPDATE)) {
        BoardNotifyPhase (PostPciEnumeration);
        AddMeasurePoint (0x30C0);
      }
    }
    ASSERT_EFI_ERROR (Status);

    if (FixedPcdGetBool (PcdSplashEnabled)) {
      if (SplashPostPci) {
        DisplaySplash ();
      }
    }
  }

  // ACPI Initialization
  if (ACPI_ENABLED ()) {
    AcpiGnvs = 0;
    AcpiBase = 0;
    Status   = (PcdGet32 (PcdLoaderAcpiNvsSize) < GetAcpiGnvsSize ()) ? EFI_OUT_OF_RESOURCES : EFI_SUCCESS;
    if (!EFI_ERROR (Status)) {
      AcpiGnvs = LdrGlobal->MemPoolStart - PcdGet32 (PcdLoaderAcpiNvsSize);
      AcpiBase = AcpiGnvs - PcdGet32 (PcdLoaderAcpiReclaimSize);
      Status   = PcdSet32S (PcdAcpiGnvsAddress, AcpiGnvs);

      S3Data = (S3_DATA *)LdrGlobal->S3DataPtr;
      if (BootMode != BOOT_ON_S3_RESUME) {
        PlatformUpdateAcpiGnvs ((VOID *)(UINTN)AcpiGnvs);
        S3Data->AcpiGnvs = AcpiGnvs;
        S3Data->AcpiBase = AcpiBase;
        DEBUG ((DEBUG_INIT, "ACPI Init\n"));
        Status = AcpiInit (&AcpiBase);
        DEBUG ((DEBUG_INFO, "ACPI Ret: %r\n", Status));
        S3Data->AcpiTop = AcpiBase;
        if (!EFI_ERROR (Status) && ((S3Data->AcpiTop - S3Data->AcpiBase) >
             PcdGet32 (PcdLoaderAcpiReclaimSize))) {
          Status = EFI_OUT_OF_RESOURCES;
        }
      } else {
        Status = (S3Data->AcpiGnvs == AcpiGnvs) ? EFI_SUCCESS : EFI_ABORTED;
      }
    }

    AddMeasurePoint (0x30D0);
    if (EFI_ERROR (Status)) {
      CpuHaltWithStatus ("ACPI error !", Status);
    }
  }

  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL) {
    PlatformService->ResetSystem = ResetSystem;
  }

  BoardInit (PrePayloadLoading);
  AddMeasurePoint (0x30E0);

  // Trigger SMI to enable SMRR valid bit if required
  if (SmmRebaseMode == SMM_REBASE_ENABLE) {
    DEBUG ((DEBUG_INFO, "Enable SMRR\n"));
    SendSmiIpiAllExcludingSelf ();
    SendSmiIpi (GetApicId());
  }

  // Finalize Smbios (add Type127 and cheksum)
  if (FixedPcdGetBool (PcdSmbiosEnabled)) {
    FinalizeSmbios ();
  }

  // Continue boot flow
  if (ACPI_ENABLED() && (BootMode == BOOT_ON_S3_RESUME)) {
    S3ResumePath (Stage2Param);
  } else {
    NormalBootPath (Stage2Param);
  }

  // Should not reach here!
  CpuHalt ("Boot failed !");
}
