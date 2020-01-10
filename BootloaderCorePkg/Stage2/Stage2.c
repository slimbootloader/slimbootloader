/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2.h"

/**
  Function to extend Payload hash

  @param[in]  CbInfo    Component Call Back Info

**/
VOID
ExtendStage (
  IN  COMPONENT_CALLBACK_INFO   *CbInfo
  )
{
  UINT8                     BootMode;

  //Check the boot mode
   BootMode = GetBootMode();
  if (MEASURED_BOOT_ENABLED() && (BootMode != BOOT_ON_S3_RESUME)) {
    //Extend hash for Component Types  Payload, Epayload and FWPayload
    if ((CbInfo != NULL ) &&  ((CbInfo->ComponentType == COMP_TYPE_PAYLOAD)
                              || (CbInfo->ComponentType == FLASH_MAP_SIG_EPAYLOAD)
                              || (CbInfo->ComponentType == COMP_TYPE_PAYLOAD_FWU ))) {
      TpmExtendStageHash((UINT8) CbInfo->ComponentType, CbInfo->HashData, CbInfo->HashAlg,
                          CbInfo->CompBuf, CbInfo->CompLen);
    }
  }
}

/**
  Callback function to add performance measure point during component loading.

  @param[in]  ProgressId    Component loading progress ID code.
  @param[in]  CbInfo    Component Call Back Info

**/
VOID
LoadComponentCallback (
  IN  UINT32                     ProgressId,
  IN  COMPONENT_CALLBACK_INFO   *CbInfo
  )
{

  // Update Progress Info
  switch (ProgressId) {
  case PROGESS_ID_LOCATE:
    AddMeasurePoint (0x3110);
    break;
  case PROGESS_ID_COPY:
    AddMeasurePoint (0x3120);
    break;
  case PROGESS_ID_AUTHENTICATE:
    ExtendStage (CbInfo);
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
  BOOLEAN                        IsNormalPld;
  UINT32                         PayloadId;
  UINT32                         ContainerSig;
  UINT32                         ComponentName;
  UINT8                          BootMode;

  BootMode = GetBootMode();
  //
  // Force PayloadId to 0 during firmware update mode.
  //
  if (BootMode == BOOT_ON_FLASH_UPDATE) {
    SetPayloadId(0);
  }
  // Load payload to PcdPayloadLoadBase.
  PayloadId   = GetPayloadId ();
  DEBUG ((DEBUG_INFO, "Loading Payload ID 0x%08X\n", PayloadId));
  IsNormalPld = (PayloadId == 0) ? TRUE : FALSE;
  if (BootMode == BOOT_ON_FLASH_UPDATE) {
    ContainerSig  = COMP_TYPE_PAYLOAD_FWU;
    ComponentName = FLASH_MAP_SIG_FWUPDATE;
  } else {
    if (IsNormalPld) {
      ContainerSig  = COMP_TYPE_PAYLOAD;
      ComponentName = FLASH_MAP_SIG_PAYLOAD;
    } else {
      ContainerSig  = FLASH_MAP_SIG_EPAYLOAD;
      ComponentName = PayloadId;
    }
  }

  Dst = PcdGet32 (PcdPayloadExeBase);
  if (FixedPcdGetBool (PcdPayloadLoadHigh)) {
    PayloadId = GetPayloadId();
    if ((PayloadId != LINX_PAYLOAD_ID_SIGNATURE) && (PayloadId != UEFI_PAYLOAD_ID_SIGNATURE)) {
      Dst = 0;
    }
  }

  AddMeasurePoint (0x3100);
  DstLen = 0;
  Status = LoadComponentWithCallback (ContainerSig, ComponentName,
                                     (VOID *)&Dst, &DstLen, LoadComponentCallback);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Loading payload error - %r !", Status));
    return 0;
  }

  AddMeasurePoint (0x3150);

  Stage2Param->PayloadActualLength = DstLen;
  DEBUG ((DEBUG_INFO, "Load Payload ID 0x%08X @ 0x%08X\n", PayloadId, Dst));
  return Dst;
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
  UINT32                          PldBase;
  LOADER_GLOBAL_DATA             *LdrGlobal;
  EFI_STATUS                      Status;
  BOOLEAN                         CallBoardNotify;
  UINT32                          PayloadId;
  VOID                           *InitRd;
  UINT32                          InitRdLen;
  UINT8                          *CmdLine;
  UINT32                          CmdLineLen;
  UINT32                          UefiSig;
  UINT8                           OrgVal;


  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();

  // Load payload
  Dst = (UINT32 *)PreparePayload (Stage2Param);
  if (Dst == NULL) {
    CpuHalt ("Failed to load payload !");
  }

  BoardInit (PostPayloadLoading);
  AddMeasurePoint (0x31A0);

  UefiSig  = 0;
  PldBase  = 0;
  PldEntry = NULL;

  Status  = EFI_SUCCESS;
  if (Dst[0] == 0x00005A4D) {
    // It is a PE format
    DEBUG ((DEBUG_INFO, "PE32 Format Payload\n"));
    Status = PeCoffRelocateImage ((UINT32)Dst);
    if (!EFI_ERROR(Status)) {
      Status = PeCoffLoaderGetEntryPoint (Dst, (VOID *)&PldEntry);
    }
  } else if (Dst[10] == EFI_FVH_SIGNATURE) {
    // It is a FV format
    DEBUG ((DEBUG_INFO, "FV Format Payload\n"));
    UefiSig = Dst[0];
    Status  = LoadFvImage (Dst, Stage2Param->PayloadActualLength, (VOID **)&PldEntry);
  } else if (IsElfImage (Dst)) {
    Status = LoadElfImage (Dst, (VOID *)&PldEntry);
  } else {
    if (FeaturePcdGet (PcdLinuxPayloadEnabled)) {
      if (IsBzImage (Dst)) {
        // It is a Linux kernel image
        DEBUG ((DEBUG_INFO, "BzImage Format Payload\n"));
        InitRd     = NULL;
        InitRdLen  = 0;
        CmdLine    = NULL;
        CmdLineLen = 0;
        Status = LoadComponent (FLASH_MAP_SIG_EPAYLOAD, SIGNATURE_32 ('C', 'M', 'D', 'L'),
                                (VOID **)&CmdLine, &CmdLineLen);
        if (!EFI_ERROR (Status)) {
          OrgVal = CmdLine[CmdLineLen];
          CmdLine[CmdLineLen] = 0;
          DEBUG ((DEBUG_INFO, "Kernel command line: \n%a\n", CmdLine));
          CmdLine[CmdLineLen] = OrgVal;
        }
        PldEntry = (PAYLOAD_ENTRY)(UINTN)LinuxBoot;
        Status   = LoadBzImage (Dst, InitRd, InitRdLen, CmdLine, CmdLineLen);
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

  if (FixedPcdGetBool (PcdSmpEnabled)) {
    DEBUG ((DEBUG_INIT, "MP Init%a\n", DebugCodeEnabled() ? " (Done)" : ""));
    Status = MpInit (EnumMpInitDone);
    AddMeasurePoint (0x31C0);
  }

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
  } else {
    CallBoardNotify = TRUE;
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

  DEBUG_CODE_BEGIN ();
  PrintStackHeapInfo ();
  DEBUG_CODE_END ();

  DEBUG ((DEBUG_INFO, "Payload entry: 0x%08X\n", PldEntry));
  if (PldEntry != NULL) {
    DEBUG ((DEBUG_INIT, "Jump to payload\n\n"));
    PldEntry (PldHobList, (VOID *)PldBase);
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

  if (FixedPcdGetBool (PcdSmpEnabled)) {
    DEBUG ((DEBUG_INFO, "MP Init (Done)\n"));
    MpInit (EnumMpInitDone);
    AddMeasurePoint (0x31C0);
  }

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

  // Find Wake Vector and Jump to OS
  AddMeasurePoint (0x31F0);
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

  LdrGlobal->LdrHobList = (VOID *)LdrGlobal->MemPoolEnd;
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
    DEBUG ((DEBUG_INFO, "Save MRC Training Data (0x%p 0x%06X) ... ", NvsData, MrcDataLen));
    SubStatus = SaveNvsData (NvsData, MrcDataLen);
    DEBUG ((DEBUG_INFO, "%X\n", SubStatus));
  }

  DEBUG ((DEBUG_INIT, "Silicon Init\n"));
  AddMeasurePoint (0x3020);
  Status = CallFspSiliconInit ();
  AddMeasurePoint (0x3030);
  FspResetHandler (Status);
  ASSERT_EFI_ERROR (Status);

  BoardInit (PostSiliconInit);
  AddMeasurePoint (0x3040);

  // Create base HOB
  BuildBaseInfoHob (Stage2Param);

  // Display splash
  if (FixedPcdGetBool (PcdSplashEnabled)) {
    DisplaySplash ();
    AddMeasurePoint (0x3050);
  }

  // MP Init phase 1
  if (FixedPcdGetBool (PcdSmpEnabled)) {
    DEBUG ((DEBUG_INFO, "MP Init (Wakeup)\n"));
    Status = MpInit (EnumMpInitWakeup);
  } else {
    DEBUG ((DEBUG_INIT, "BSP Init\n"));
    BspInit ();
  }
  AddMeasurePoint (0x3060);

  // MP Init phase 2
  if (FixedPcdGetBool (PcdSmpEnabled) && !EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "MP Init (Run)\n"));
    Status = MpInit (EnumMpInitRun);
    AddMeasurePoint (0x3080);
  }
  ASSERT_EFI_ERROR (Status);

  // PCI Enumeration
  BoardInit (PrePciEnumeration);
  AddMeasurePoint (0x3090);

  if (FixedPcdGetBool (PcdPciEnumEnabled)) {
    MemPool = AllocateTemporaryMemory (0);
    DEBUG ((DEBUG_INIT, "PCI Enum\n"));
    Status = PciEnumeration (MemPool);
    AddMeasurePoint (0x30A0);

    BoardInit (PostPciEnumeration);
    AddMeasurePoint (0x30B0);

    if (!EFI_ERROR (Status)) {
      if (BootMode != BOOT_ON_FLASH_UPDATE) {
        BoardNotifyPhase (PostPciEnumeration);
        AddMeasurePoint (0x30C0);
      }
    }

    ASSERT_EFI_ERROR (Status);
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
        PlatformUpdateAcpiGnvs ((VOID *)AcpiGnvs);
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

  //
  // Allocate SMBIOS tables' memory, set Base and call Smbios init
  //
  if (FixedPcdGetBool (PcdSmbiosEnabled)) {
    SmbiosEntry = AllocateZeroPool (PcdGet16(PcdSmbiosTablesSize));
    Status = PcdSet32S (PcdSmbiosTablesBase, (UINT32)SmbiosEntry);
    Status = SmbiosInit ();
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "SMBIOS init Status = %r\n", Status));
    }
  }

  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL) {
    PlatformService->ResetSystem = ResetSystem;
  }

  BoardInit (PrePayloadLoading);
  AddMeasurePoint (0x30E0);

  // Continue boot flow
  if (ACPI_ENABLED() && (BootMode == BOOT_ON_S3_RESUME)) {
    S3ResumePath (Stage2Param);
  } else {
    NormalBootPath (Stage2Param);
  }

  // Should not reach here!
  CpuHalt ("Boot failed !");
}
