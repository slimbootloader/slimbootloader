/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Stage2.h"

/**
  Prepare and load payload into proper location for execution.

  @param[in]  Stage2Hob    HOB pointer for Stage2

  @retval     The base address of the payload.
              0 if loading fails.

**/
UINT32
PreparePayload (
  IN STAGE2_HOB   *Stage2Hob
  )
{
  EFI_STATUS                     Status;
  UINT32                         Src;
  UINT32                         Dst;
  UINT32                         TmpDst;
  UINT32                         LoadBase;
  UINT32                         Length;
  UINT32                         ActualLength;
  LOADER_COMPRESSED_HEADER      *Hdr;
  UINT8                          BootMode;
  UINT8                          HashIdx;
  UINT8                          PldIdx;
  MULTI_PAYLOAD_HEADER          *PldHdr;
  MULTI_PAYLOAD_ENTRY           *PldEntry;
  BOOLEAN                        IsNormalPld;
  UINT32                         PayloadId;
  VOID                          *Scr;
  UINT8                         *PubKey;
  UINT32                         ScrLen;
  UINT32                         DstLen;
  UINT64                         Tmp64;

  // Load payload to PcdPayloadLoadBase.
  PayloadId   = GetPayloadId ();
  IsNormalPld = (PayloadId == 0) ? TRUE : FALSE;
  BootMode = GetBootMode();
  if (BootMode == BOOT_ON_FLASH_UPDATE) {
    Status = GetComponentInfo (FLASH_MAP_SIG_FWUPDATE, &Src, &Length);
    LoadBase = PcdGet32 (PcdFwuPayloadLoadBase);
  } else {
    if (IsNormalPld) {
      Status = GetComponentInfo (FLASH_MAP_SIG_PAYLOAD, &Src, &Length);
    } else {
      Status = GetComponentInfo (FLASH_MAP_SIG_EPAYLOAD, &Src, &Length);
    }
    LoadBase = PcdGet32 (PcdPayloadLoadBase);
  }
  if (EFI_ERROR (Status)) {
    return 0;
  }

  Dst = (LoadBase == 0) ? Src : LoadBase;
  if (Stage2Hob->PayloadBase != 0) {
    // Payload is loaded already by Stage1B
    DEBUG ((DEBUG_INFO, "Payload is loaded already\n"));
    Src = Stage2Hob->PayloadBase;
    if (Stage2Hob->PayloadId != 0) {
      Src += Stage2Hob->PayloadOffset;
    }
  } else {
    // Load payload from media
    if (Stage2Hob->PayloadId != 0) {
      // Load a payload from multi-payload image
      if (Src == Dst) {
        Dst += Stage2Hob->PayloadOffset;
      }
      Src += Stage2Hob->PayloadOffset;
      Length = Stage2Hob->PayloadLength;
    }
    Status = LoadPayload (Dst, Src, Length);
    ASSERT_EFI_ERROR (Status);
    AddMeasurePoint (0x3100);

    Src = Dst;
  }

  ActualLength = Length;
  Hdr = (LOADER_COMPRESSED_HEADER *) Src;
  if (FixedPcdGetBool (PcdVerifiedBootEnabled)) {
    // Copy and Verify if Payload is in Flash
    if (IS_COMPRESSED (Hdr)) {
      Length = sizeof (LOADER_COMPRESSED_HEADER) + Hdr->CompressedSize;
    }

    // Security requirement: Verify in memory only (Not in flash)
    if ((Stage2Hob->PayloadBase == 0) && IS_FLASH_SPACE (Src)) {
      TmpDst = (UINT32) AllocateTemporaryPages (EFI_SIZE_TO_PAGES (Length));
      CopyMem ((VOID *)TmpDst, (VOID *)Src, Length);
      AddMeasurePoint (0x3110);
      Src = TmpDst;
      Hdr = (LOADER_COMPRESSED_HEADER *)TmpDst;
    }

    if (BootMode == BOOT_ON_FLASH_UPDATE) {
      HashIdx = HASH_INDEX_FIRMWARE_UPDATE;
    } else {
      if (Stage2Hob->PayloadId == 0) {
        HashIdx = HASH_INDEX_PAYLOAD;
      } else {
        HashIdx = HASH_INDEX_PAYLOAD_DYNAMIC;
      }
    }

    if (IsNormalPld || (Stage2Hob->PayloadId != 0)) {
      // For standard payload or components inside multi-payload, do hash verififcation
      Status = DoHashVerify ((CONST UINT8 *)Src, Length, HashIdx);
    } else {
      // For multi-payload header, do signature verififcation
      PldHdr    = (MULTI_PAYLOAD_HEADER *)&Hdr[1];
      PldEntry  = (MULTI_PAYLOAD_ENTRY *)&PldHdr[1] + PldHdr->EntryNum;
      if (Hdr->Signature != LZDM_SIGNATURE) {
        Status = RETURN_UNSUPPORTED;
      } else if ((UINT32)((UINT8 *)PldEntry - (UINT8 *)Hdr) >= Length) {
        // Ensure the buffer is within the boundary since the header cannot be trusted.
        Status = RETURN_BAD_BUFFER_SIZE;
      } else {
        Length = (UINT32)((UINT8 *)PldEntry - (UINT8 *)Hdr);
        PubKey = (UINT8 *)PldEntry + RSA2048NUMBYTES;
        Status = DoRsaVerify ((CONST UINT8 *)Src,  Length, COMP_TYPE_PUBKEY_CFG_DATA,
                              (CONST UINT8 *)PldEntry, PubKey, NULL);
      }
    }
    AddMeasurePoint (0x3120);
    if (EFI_ERROR (Status)) {
      if (Status != RETURN_NOT_FOUND) {
        CpuHaltWithStatus (NULL, Status);
      }
    }
    // Extend hash of Payload into TPM.
    if (MEASURED_BOOT_ENABLED() ) {
      if (BootMode != BOOT_ON_S3_RESUME) {
        TpmExtendStageHash (HashIdx);
      }
    }
  }

  // Decompress payload
  if (IS_COMPRESSED (Hdr)) {
    // Determine if Payload needs to be loaded into high mem
    ActualLength = Hdr->Size;
    if (FixedPcdGetBool (PcdPayloadLoadHigh)) {
      Dst = (UINT32)AllocatePages (EFI_SIZE_TO_PAGES (ActualLength));
    } else {
      Dst = PcdGet32 (PcdPayloadExeBase);
    }

    // Reserve scratch space needed for decompression
    DEBUG ((DEBUG_INFO, "Load Payload ID 0x%08X @ 0x%08X\n", Stage2Hob->PayloadId, Dst));
    Status = DecompressGetInfo (Hdr->Signature, Hdr->Data, Hdr->CompressedSize, &DstLen, &ScrLen);
    if (!EFI_ERROR (Status)) {
      Scr    = AllocateTemporaryPages (EFI_SIZE_TO_PAGES (ScrLen));
      Status = Decompress (Hdr->Signature, Hdr->Data, Hdr->CompressedSize, (VOID *)Dst, Scr);
      FreeTemporaryMemory (Scr);
    }
    AddMeasurePoint (0x3130);
    ASSERT_EFI_ERROR (Status);
  } else {
    Dst = Src;
  }

  if (IS_MULTI_PAYLOAD (Dst)) {

    PldHdr   = (MULTI_PAYLOAD_HEADER *)Dst;
    PldEntry = (MULTI_PAYLOAD_ENTRY *)&PldHdr[1];
    for (PldIdx = 0; PldIdx < PldHdr->EntryNum; PldIdx++) {
      if (PayloadId == PldEntry->Name) {
        Stage2Hob->PayloadId     = PldEntry->Name;
        Stage2Hob->PayloadOffset = PldEntry->Offset;
        Stage2Hob->PayloadLength = PldEntry->Size;
        Stage2Hob->PayloadActualLength = PldEntry->Size;
        SetComponentHash (HASH_INDEX_PAYLOAD_DYNAMIC, PldEntry->Hash);
        break;
      }
      PldEntry++;
    }

    Tmp64 = PayloadId;
    if (PldIdx == PldHdr->EntryNum) {
      Dst = 0;
    } else {
      Dst = PreparePayload (Stage2Hob);
    }
    DEBUG ((DEBUG_INFO, "Load Multi-Payload name '%a' - %r\n", &Tmp64, Dst != 0 ? EFI_SUCCESS : EFI_NOT_FOUND));

  } else {
    Stage2Hob->PayloadId = 0;
    Stage2Hob->PayloadActualLength = ActualLength;
  }

  return Dst;
}

/**
  Normal boot flow.

  @param[in]   Stage2Hob            STAGE2_HOB HOB pointer.

**/
VOID
NormalBootPath (
  IN STAGE2_HOB    *Stage2Hob
  )
{
  UINT32                         *Dst;
  PAYLOAD_ENTRY                   PldEntry;
  VOID                           *PldHobList;
  UINT32                          KernelEntry;
  LOADER_GLOBAL_DATA             *LdrGlobal;
  EFI_STATUS                      Status;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();

  // Load payload
  Dst = (UINT32 *)PreparePayload (Stage2Hob);
  ASSERT (Dst != NULL);

  BoardInit (PostPayloadLoading);
  AddMeasurePoint (0x31A0);

  KernelEntry = 0;
  if (Dst[0] == 0x00005A4D) {
    // It is a PE format
    DEBUG ((DEBUG_INFO, "PE32 Format Payload\n"));
    Status = PeCoffRelocateImage ((UINT32)Dst);
    ASSERT_EFI_ERROR (Status);
    Status = PeCoffLoaderGetEntryPoint (Dst, (VOID *)&PldEntry);
    ASSERT_EFI_ERROR (Status);
  } else if (Dst[10] == EFI_FVH_SIGNATURE) {
    // It is a FV format
    DEBUG ((DEBUG_INFO, "FV Format Payload\n"));
    // If loading base is not as expected, copy it over
    PldEntry = (PAYLOAD_ENTRY)Dst[0];
    if ((UINT32)Dst != Dst[1]) {
      CopyMem ((VOID *)Dst[1], Dst, Stage2Hob->PayloadActualLength);
    }
  } else if (IsElfImage (Dst)) {
    PldEntry = (PAYLOAD_ENTRY) (UINTN) LoadElfImage (Dst);
    ASSERT (PldEntry != NULL);
  } else {
    // Assume RAW Binary Payload
    // PcdPayloadExeBase MUST be the same as payload binary's TEXT BASE
    // Jump to the entry point directly
    PldEntry = (PAYLOAD_ENTRY) (UINTN)Dst;
  }
  AddMeasurePoint (0x31B0);

  if (FixedPcdGetBool (PcdSmpEnabled)) {
    DEBUG ((DEBUG_INIT, "MP Init%a\n", DebugCodeEnabled() ? " (Done)" : ""));
    Status = MpInit (EnumMpInitDone);
    AddMeasurePoint (0x31C0);
  }

  BoardInit (EndOfStages);
  AddMeasurePoint (0x31F0);

  DEBUG ((DEBUG_INFO, "HOB @ 0x%08X\n", LdrGlobal->LdrHobList));
  PldHobList = BuildExtraInfoHob (Stage2Hob);

  DEBUG_CODE_BEGIN ();
  PrintStackHeapInfo ();
  DEBUG_CODE_END ();

  DEBUG ((DEBUG_INFO, "Payload entry: 0x%08X\n", PldEntry));
  DEBUG ((DEBUG_INIT, "Jump to payload\n"));
  PldEntry (PldHobList, (VOID *)KernelEntry);
}

/**
  S3 resume flow.

  @param Stage2Hob            STAGE2_HOB HOB pointer.

**/
VOID
S3ResumePath (
  STAGE2_HOB   *Stage2Hob
  )
{
  LOADER_GLOBAL_DATA             *LdrGlobal;
  S3_DATA                        *S3Data;
  EFI_STATUS                      Status;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  S3Data    = (S3_DATA *)LdrGlobal->S3DataPtr;

  if (FixedPcdGetBool (PcdSmpEnabled)) {
    DEBUG ((DEBUG_INFO, "MP Init (Done)\n"));
    Status = MpInit (EnumMpInitDone);
    AddMeasurePoint (0x31C0);
  }

  BoardInit (ReadyToBoot);

  // Call FspNotify ReadyToBoot
  DEBUG ((DEBUG_INFO, "Call FspNotifyPhase(ReadyToBoot) ... "));
  Status = CallFspNotifyPhase (EnumInitPhaseReadyToBoot);
  AddMeasurePoint (0x31D0);
  DEBUG ((DEBUG_INFO, "%X\n", Status));
  ASSERT_EFI_ERROR (Status);

  BoardInit (EndOfFirmware);

  // Call FspNotify EndOfFirmware
  DEBUG ((DEBUG_INFO, "Call FspNotifyPhase(EndOfFirmware) ... "));
  Status = CallFspNotifyPhase (EnumInitPhaseEndOfFirmware);
  AddMeasurePoint (0x31E0);
  DEBUG ((DEBUG_INFO, "%X\n", Status));
  ASSERT_EFI_ERROR (Status);

  AddMeasurePoint (0x31F0);

  DEBUG_CODE_BEGIN ();
  PrintStackHeapInfo ();
  DEBUG_CODE_END ();

  // Find Wake Vector and Jump to OS
  FindAcpiWakeVectorAndJump (S3Data->AcpiBase);

  ASSERT (FALSE);
}

/**
  Entry point to the C language phase of Stage2.

  Stage2 will complete the remaining system initialization and load payload.
  It will be executed from memory.

  @param Params            STAGE2_HOB HOB pointer.

**/
VOID
EFIAPI
SecStartup (
  VOID *Params
  )
{
  EFI_STATUS                      Status;
  EFI_STATUS                      SubStatus;
  STAGE2_HOB                     *Stage2Hob;
  VOID                           *NvsData;
  UINT32                          MrcDataLen;
  VOID                           *MemPool;
  UINT32                          Delta;
  UINT32                          AcpiGnvs;
  UINT32                          AcpiTop;
  LOADER_GLOBAL_DATA             *LdrGlobal;
  UINT8                           BootMode;
  S3_DATA                        *S3Data;
  PLATFORM_SERVICE               *PlatformService;

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
  Stage2Hob = (STAGE2_HOB *)Params;
  Delta = Stage2Hob->Stage2ExeBase - PCD_GET32_WITH_ADJUST (PcdStage2FdBase);
  PcdSet32 (PcdFSPSBase,             PCD_GET32_WITH_ADJUST (PcdFSPSBase) + Delta);
  PcdSet32 (PcdAcpiTablesAddress,    PCD_GET32_WITH_ADJUST (PcdAcpiTablesAddress) + Delta);
  PcdSet32 (PcdGraphicsVbtAddress,   PCD_GET32_WITH_ADJUST (PcdGraphicsVbtAddress) + Delta);
  PcdSet32 (PcdSplashLogoAddress,    PCD_GET32_WITH_ADJUST (PcdSplashLogoAddress) + Delta);

  LdrGlobal->LdrHobList = (VOID *)LdrGlobal->MemPoolEnd;
  BuildHobHandoffInfoTable (
    BootMode,
    (EFI_PHYSICAL_ADDRESS) (UINTN)LdrGlobal->LdrHobList,
    (UINTN)PcdGet32 (PcdLoaderHobStackSize)
    );

  InitializeDebugAgent (DEBUG_AGENT_INIT_DXE_LOAD, NULL, NULL);

  // Call FspSiliconInit
  BoardInit (PreSiliconInit);
  AddMeasurePoint (0x3020);

  // Save NVS data
  NvsData = GetFspNvsDataBuffer (LdrGlobal->FspHobList, &MrcDataLen);
  if ((NvsData != NULL) && (MrcDataLen > 0)) {
    DEBUG ((DEBUG_INFO, "Save MRC Training Data (0x%p 0x%06X) ... ", NvsData, MrcDataLen));
    SubStatus = SaveNvsData (NvsData, MrcDataLen);
    AddMeasurePoint (0x3070);
    DEBUG ((DEBUG_INFO, "%X\n", SubStatus));
  }

  Status = CallFspSiliconInit ();
  AddMeasurePoint (0x3030);
  ASSERT_EFI_ERROR (Status);

  BoardInit (PostSiliconInit);
  AddMeasurePoint (0x3040);

  // Create base HOB
  BuildBaseInfoHob (Stage2Hob);

  // Display splash
  if (FixedPcdGetBool (PcdSplashEnabled)) {
    DisplaySplash ();
    AddMeasurePoint (0x3050);
  }

  // MP Init phase 1
  if (FixedPcdGetBool (PcdSmpEnabled)) {
    DEBUG ((DEBUG_INFO, "MP Init (Wakeup)\n"));
    Status = MpInit (EnumMpInitWakeup);
    AddMeasurePoint (0x3060);
  }

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
    MemPool = AllocateTemporaryPages (0);
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
  if ( FixedPcdGetBool (PcdAcpiEnabled) && ACPI_FEATURE_ENABLED() ) {
    if (PcdGet32 (PcdLoaderAcpiNvsSize) < GetAcpiGnvsSize()) {
      AcpiGnvs = 0;
      AcpiTop  = 0;
    } else {
      AcpiGnvs = LdrGlobal->MemPoolStart - PcdGet32 (PcdLoaderAcpiNvsSize);
      AcpiTop  = AcpiGnvs;
    }

    S3Data   = (S3_DATA *)LdrGlobal->S3DataPtr;
    PcdSet32 (PcdAcpiGnvsAddress, AcpiGnvs);
    if (BootMode != BOOT_ON_S3_RESUME) {
      if ((AcpiGnvs > 0) && (AcpiTop > 0)) {
        PlatformUpdateAcpiGnvs ((VOID *)AcpiGnvs);
        S3Data->AcpiGnvs = AcpiGnvs;
        S3Data->AcpiTop  = AcpiTop;
        DEBUG ((DEBUG_INIT, "ACPI Init\n"));
        Status = AcpiInit (&AcpiTop);
        DEBUG ((DEBUG_INFO, "ACPI Ret: %r\n", Status));
        S3Data->AcpiBase = AcpiTop;
        if (!EFI_ERROR (Status) && ((S3Data->AcpiTop - S3Data->AcpiBase) >
             PcdGet32 (PcdLoaderAcpiReclaimSize))) {
          Status = EFI_OUT_OF_RESOURCES;
        }
      } else {
        Status = EFI_OUT_OF_RESOURCES;
      }
    } else {
      Status   = (S3Data->AcpiGnvs == AcpiGnvs) ? EFI_SUCCESS : EFI_ABORTED;
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

  // Continue boot flow
  if (FixedPcdGetBool (PcdAcpiEnabled) && (BootMode == BOOT_ON_S3_RESUME) && ACPI_FEATURE_ENABLED() ) {
    S3ResumePath (Stage2Hob);
  } else {
    NormalBootPath (Stage2Hob);
  }

  // Should not reach here!
  DEBUG ((DEBUG_ERROR, "\nBoot failed. System is halted.\n"));

  CpuHalt (NULL);
}
