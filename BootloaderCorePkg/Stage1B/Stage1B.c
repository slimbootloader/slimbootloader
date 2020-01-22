/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1B.h"

/**
  Function to extend stage2 hash

  @param[in]  CbInfo    Component Call Back Info

**/
VOID
ExtendStageHash (
  IN  COMPONENT_CALLBACK_INFO   *CbInfo
  )
{
  UINT8                BootMode;
  UINT8                DigestHash[HASH_DIGEST_MAX];
  HASH_ALG_TYPE        MbHashType;
  TPMI_ALG_HASH        MbTmpAlgHash;
  UINT8               *HashPtr;
  RETURN_STATUS        Status;

  //Convert Measured boot Hash Mask to HASH_ALG_TYPE (CryptoLib)
  MbHashType   = GetCryptoHashAlg(PcdGet32(PcdMeasuredBootHashMask));

  //Convert Measured boot Hash Mask to TPMI_ALG_HASH (TPM ALG ID)
  MbTmpAlgHash = (TPMI_ALG_HASH) GetTpmHashAlg(PcdGet32(PcdMeasuredBootHashMask));

  //Check the boot mode
  BootMode = GetBootMode();
  if (MEASURED_BOOT_ENABLED() && (BootMode != BOOT_ON_S3_RESUME)) {
    //Extend  hash if ComponentType is stage2
    if ((CbInfo != NULL ) && (CbInfo->ComponentType == COMP_TYPE_STAGE_2)) {
      // Check Hash data alg match to PcdMeasuredBootHashMask
      if ((CbInfo->HashAlg == MbHashType) && (CbInfo->HashData != NULL)) {
        // Extend CbInfo->HashData if hashalg is valid
        HashPtr = CbInfo->HashData;
        Status = EFI_SUCCESS;
      } else {
        // Get Hash to extend based on component type and component src addresss
        Status = GetHashToExtend ((UINT8) CbInfo->ComponentType,
                                    MbHashType, CbInfo->CompBuf, CbInfo->CompLen, DigestHash);
        HashPtr = DigestHash;
      }

      if ((Status == EFI_SUCCESS) && (HashPtr != NULL)) {
        TpmExtendPcrAndLogEvent (0, MbTmpAlgHash, HashPtr,
                                  EV_POST_CODE, POST_CODE_STR_LEN, (UINT8 *)EV_POSTCODE_INFO_POST_CODE);
      } else {
        DEBUG((DEBUG_INFO, "Stage2 TPM PCR(0) extend failed!! \n"));
      }
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

  switch (ProgressId) {
  case PROGESS_ID_COPY:
    AddMeasurePoint (0x2090);
    break;
  case PROGESS_ID_AUTHENTICATE:
    ExtendStageHash (CbInfo);
    AddMeasurePoint (0x20A0);
    break;
  case PROGESS_ID_DECOMPRESS:
    AddMeasurePoint (0x20B0);
    break;
  default:
    break;
  }


}

/**
  Prepare and load Stage2 into proper location for execution.
  - Load stage2 and check if compressed, if not CPU halted.
  - Check if the image is in flash
  - Do hash verification if failed CPU halted
  - Check if stage2 need to be loaded to high mem.
  - uncompress stage2 and rebase if required.

  @param[in]  Stage1bParam    Param pointer for Stage1B

  @retval     The base address of the stage.
              0 if loading fails

**/
UINT32
PrepareStage2 (
  IN STAGE1B_PARAM   *Stage1bParam
  )
{
  UINT32                    Dst;
  UINT32                    DstLen;
  EFI_STATUS                Status;
  UINT32                    Delta;
  STAGE_HDR                *StageHdr;


  if (FixedPcdGetBool (PcdStage2LoadHigh)) {
    Dst = 0;
  } else {
    Dst = PCD_GET32_WITH_ADJUST (PcdStage2FdBase);
  }

  AddMeasurePoint (0x2080);
  Status = LoadComponentWithCallback (COMP_TYPE_STAGE_2, FLASH_MAP_SIG_STAGE2,
                                     (VOID *)&Dst, &DstLen, LoadComponentCallback);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Loading Stage2 error - %r !", Status));
    return 0;
  }

   AddMeasurePoint (0x20C0);

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

    AddMeasurePoint (0x20D0);
  }

  DEBUG ((DEBUG_INFO, "Loaded STAGE2 @ 0x%08X\n", Dst));

  return Dst;
}

/**
  Load, verify and append a hashes from external hash store compoment.

  @param[in] LdrGlobal      Pointer to loader global data.
  @param[in] Stage1bParam   Pointer to Stage1B params.

  @retval   EFI_SUCCESS             Hash store was appended successfully.
  @retval   EFI_NOT_FOUND           Could not locate the external hash store.
  @retval   EFI_OUT_OF_RESOURCES    Not enough buffer to load the external hash store.
  @retval   EFI_UNSUPPORTED         Hash store format is not valid.
  @retval   EFI_SECURITY_VIOLATION  Hash store authentication failed.
**/
EFI_STATUS
AppendHashStore (
  IN LOADER_GLOBAL_DATA       *LdrGlobal,
  IN STAGE1B_PARAM            *Stage1bParam
  )
{
  EFI_STATUS           Status;
  HASH_STORE_TABLE    *LdrKeyHashBlob;
  HASH_STORE_TABLE    *OemKeyHashBlob;
  HASH_STORE_TABLE    *OemKeyHashComp;
  UINT32               OemKeyHashCompBase;
  UINT32               OemKeyHashUsedLength;
  INT32                KeyHashSize;
  UINT8                AuthInfo[SIGNATURE_AND_KEY_SIZE_MAX];
  SIGNATURE_HDR       *SignHdr;
  PUB_KEY_HDR         *PubKeyHdr;
  HASH_ALG_TYPE        MbHashType;


  Status = GetComponentInfo (FLASH_MAP_SIG_KEYHASH, &OemKeyHashCompBase, NULL);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  // Check used length before copying to temporary memory
  OemKeyHashComp = (HASH_STORE_TABLE *) OemKeyHashCompBase;
  LdrKeyHashBlob = (HASH_STORE_TABLE *) LdrGlobal->HashStorePtr;
  OemKeyHashUsedLength  = OemKeyHashComp->UsedLength;
  if (OemKeyHashUsedLength > LdrKeyHashBlob->TotalLength - LdrKeyHashBlob->UsedLength) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Copy to temporary memory
  OemKeyHashBlob = (HASH_STORE_TABLE *)((UINT8 *)LdrKeyHashBlob + LdrKeyHashBlob->UsedLength);
  CopyMem (OemKeyHashBlob, (UINT8 *)OemKeyHashComp, OemKeyHashUsedLength);

  // Check the header length
  KeyHashSize = OemKeyHashUsedLength - OemKeyHashBlob->HeaderLength;
  if (KeyHashSize <= 0) {
    return EFI_UNSUPPORTED;
  }

  // Copy anthentication info to stack
  if (!FeaturePcdGet (PcdVerifiedBootEnabled)) {
    Status = EFI_SUCCESS;
  } else {
    CopyMem (AuthInfo, (UINT8 *)OemKeyHashComp + OemKeyHashUsedLength, sizeof(AuthInfo));
    SignHdr   = (SIGNATURE_HDR *) AuthInfo;
    PubKeyHdr = (PUB_KEY_HDR *)((UINT8 *)SignHdr + sizeof(SIGNATURE_HDR) + SignHdr->SigSize);
    Status = DoRsaVerify ((UINT8 *)OemKeyHashBlob,
                          OemKeyHashBlob->UsedLength,
                          HASH_USAGE_PUBKEY_MASTER,
                          SignHdr, PubKeyHdr,
                          PcdGet8(PcdCompSignHashAlg),
                          NULL,
                          Stage1bParam->KeyHashManifestHash);
  }
  if (EFI_ERROR (Status)) {
    Stage1bParam->KeyHashManifestHashValid = 0;
    return EFI_SECURITY_VIOLATION;
  }

  if (MEASURED_BOOT_ENABLED()) {
    //Convert Measured boot Hash Mask to HASH_ALG_TYPE (CryptoLib)
    MbHashType   = GetCryptoHashAlg(PcdGet32(PcdMeasuredBootHashMask));

    if (PcdGet8(PcdCompSignHashAlg) == MbHashType) {
      Stage1bParam->KeyHashManifestHashValid = 1;
    } else {
    // Check validition of Stage1bParam->KeyHashManifestHash generated.
    // Calcluate the digest to extend if measured boot hash alg doesn't match
      Status = GetHashToExtend (COMP_TYPE_INVALID,
                                    MbHashType,
                                    (UINT8 *) OemKeyHashBlob,
                                    OemKeyHashBlob->UsedLength,
                                    Stage1bParam->KeyHashManifestHash);
      if (Status == EFI_SUCCESS) {
        Stage1bParam->KeyHashManifestHashValid = 1;
      }
    }
  }

  // Append hash to the end and adjust used length
  CopyMem ((UINT8 *)OemKeyHashBlob, (UINT8 *)OemKeyHashBlob + OemKeyHashBlob->HeaderLength, KeyHashSize);
  LdrKeyHashBlob->UsedLength += KeyHashSize;

  return EFI_SUCCESS;
}

/**
  Load configuration data.

  This function will find external and internal configuration data,
  verify it if verified boot is enabled and append it to config database.

  @param[in] LdrGlobal            Pointer to loader global data.
  @param[in] Stage1bParam           Pointer to Stage1b Param.

**/
VOID
CreateConfigDatabase (
  IN LOADER_GLOBAL_DATA       *LdrGlobal,
  IN STAGE1B_PARAM            *Stage1bParam
  )
{
  EFI_STATUS                Status;
  CDATA_BLOB               *CfgBlob;
  UINT8                    *ExtCfgAddPtr;
  UINT8                    *IntCfgAddPtr;
  PUB_KEY_HDR              *PubKeyHdr;
  SIGNATURE_HDR            *SignHdr;
  UINT32                    CfgDataBase;
  UINT32                    CfgDataLength;
  HASH_ALG_TYPE             MbHashType;

  //
  // In the config data base, the config data near the data base heaser has high priority.
  // In order to avoid memory allocation and configuration data movement if possible,
  // Here add external config data firstly, then append internal config data.
  // PcdCfgDatabaseSize: It is the size for both internal + external cfg data
  //
  if (PcdGet32 (PcdCfgDataSize) > 0) {
    CfgBlob = (CDATA_BLOB *)LdrGlobal->CfgDataPtr;
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
        CfgBlob   = (CDATA_BLOB *)ExtCfgAddPtr;
        SignHdr   = (SIGNATURE_HDR *)((UINT8 *) CfgBlob + CfgBlob->UsedLength);
        PubKeyHdr = (PUB_KEY_HDR *)((UINT8 *)SignHdr + sizeof(SIGNATURE_HDR) + SignHdr->SigSize);
        Status  = DoRsaVerify ((UINT8 *)CfgBlob, CfgBlob->UsedLength, HASH_USAGE_PUBKEY_CFG_DATA,
                    SignHdr, PubKeyHdr, PcdGet8(PcdCompSignHashAlg), NULL, Stage1bParam->ConfigDataHash);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_INFO, "EXT CFG Data ignored ... %r\n", Status));
          ExtCfgAddPtr = NULL;
        }
      }
      if (ExtCfgAddPtr != NULL) {
       if (MEASURED_BOOT_ENABLED()) {
          //Convert Measured boot Hash Mask to HASH_ALG_TYPE (CryptoLib)
          MbHashType   = GetCryptoHashAlg(PcdGet32(PcdMeasuredBootHashMask));

         // Check if Stage1bParam->ConfigDataHash generated matches PcdMeasuredBootHashMask
         if (PcdGet8(PcdCompSignHashAlg) == MbHashType) {
           Stage1bParam->ConfigDataHashValid = 1;
         } else {
            // Get config hash to extend if config data hash type do not match PcdMeasuredBootHashMask
            Status = GetHashToExtend (COMP_TYPE_INVALID,
                                  MbHashType,
                                  ExtCfgAddPtr,
                                  ((CDATA_BLOB *) ExtCfgAddPtr)->UsedLength,
                                  Stage1bParam->ConfigDataHash);
            if (!EFI_ERROR (Status)){
              Stage1bParam->ConfigDataHashValid = 1;
            }
          }
        }
        Status = AddConfigData (ExtCfgAddPtr);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_INFO, "Append EXT CFG Data ... %r\n", Status));
          Stage1bParam->ConfigDataHashValid = 0;
        }
      }
    }

    // Add internal CFGDATA at the end
    IntCfgAddPtr = (UINT8 *)PCD_GET32_WITH_ADJUST (PcdCfgDataIntBase);
    if (IntCfgAddPtr != NULL) {
      Status = AddConfigData (IntCfgAddPtr);
      if (!EFI_ERROR (Status)) {
        // Update the built-in CFGDATA offset
        CfgBlob       = (CDATA_BLOB *) IntCfgAddPtr;
        CfgDataLength = CfgBlob->UsedLength - CfgBlob->HeaderLength;
        CfgBlob       = (CDATA_BLOB *) LdrGlobal->CfgDataPtr;
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
  STAGE1A_PARAM            *Stage1aParam;
  LOADER_GLOBAL_DATA       *LdrGlobal;
  LOADER_GLOBAL_DATA       *OldLdrGlobal;
  UINT32                    FspReservedMemBase;
  UINT64                    FspReservedMemSize;
  STAGE1B_PARAM             Stage1bParam;
  STAGE1B_PARAM            *Stage1bParamInMem;
  UINT32                    StackTop;
  VOID                     *HobList;
  EFI_STATUS                Status;
  UINT32                    MemPoolStart;
  UINT32                    MemPoolEnd;
  UINT32                    MemPoolCurrTop;
  UINT32                    AllocateLen;
  UINT32                    Offset;
  UINT32                    Delta;
  UINT32                    Idx;
  UINT8                    *BufPtr;
  UINT8                    *OldBufPtr;
  VOID                     *SavedLdrHobList;
  LIBRARY_DATA             *LibDataPtr;
  SERVICES_LIST            *ServiceList;
  PLATFORMID_CFG_DATA      *PidCfgData;
  UINT8                     PlatformName[PLATFORM_NAME_SIZE + 1];
  DEBUG_LOG_BUFFER_HEADER  *NewLogBuf;
  DEBUG_LOG_BUFFER_HEADER  *OldLogBuf;
  BOOLEAN                   OldStatus;
  PLT_DEVICE_TABLE         *DeviceTable;
  CONTAINER_LIST           *ContainerList;
  VOID                    **FieldPtr;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ();
  ASSERT (LdrGlobal != NULL);
  LdrGlobal->LoaderStage = LOADER_STAGE_1B;
  AddMeasurePoint (0x2000);

  DEBUG ((DEBUG_INFO, "\n============= Intel Slim Bootloader STAGE1B =============\n"));

  Stage1aParam = (STAGE1A_PARAM *) Params;
  if (Stage1aParam->Stage1BBase != 0) {
    PeCoffFindAndReportImageInfo ((UINT32) (UINTN) GET_STAGE_MODULE_BASE (Stage1aParam->Stage1BBase));
  }

  // Reload Exception handler
  UpdateExceptionHandler (NULL);

  // Migrate data from Stage1A Param to Stage1B Param
  ZeroMem (&Stage1bParam, sizeof (STAGE1B_PARAM));

  // Remove all services in previous stage
  ServiceList = (SERVICES_LIST *)LdrGlobal->ServicePtr;
  if (ServiceList != NULL) {
    ZeroMem (ServiceList->Header, sizeof (SERVICE_COMMON_HEADER *) * ServiceList->Count);
  }

  // Perform pre-config board init
  BoardInit (PreConfigInit);

  Status = AppendHashStore (LdrGlobal, &Stage1bParam);
  DEBUG ((DEBUG_INFO,  "Append public key hash into store: %r\n", Status));

  CreateConfigDatabase (LdrGlobal, &Stage1bParam);

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
  FspResetHandler (Status);
  ASSERT_EFI_ERROR (Status);

  //
  // Reset the system if FSP API returned FSP_STATUS_RESET_REQUIRED status
  //
  if ((Status >= FSP_STATUS_RESET_REQUIRED_COLD) && (Status <= FSP_STATUS_RESET_REQUIRED_8)) {
    DEBUG ((DEBUG_INIT, "FSP Reboot\n"));
    if (Status == FSP_STATUS_RESET_REQUIRED_WARM) {
      ResetSystem(EfiResetWarm);
    } else {
      ResetSystem(EfiResetCold);
    }
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

  // Restore S3_DATA in new LoaderGlobal
  LdrGlobal->S3DataPtr = AllocatePool (sizeof (S3_DATA));
  if (LdrGlobal->BootMode != BOOT_ON_S3_RESUME) {
    ZeroMem (LdrGlobal->S3DataPtr, sizeof (S3_DATA));
  }

  // Calculate and Save CRC32 if S3_DEBUG is enabled
  if (FixedPcdGetBool (PcdS3DebugEnabled)) {
    S3DebugSaveCRC32 (SavedLdrHobList);
  }

  // Migrate data from CAR to memory
  AllocateLen = Stage1aParam->AllocDataLen;
  OldBufPtr   = (UINT8 *)Stage1aParam->AllocDataBase;
  BufPtr = AllocatePool (AllocateLen);
  CopyMem (BufPtr, OldBufPtr, AllocateLen);
  Delta = BufPtr - OldBufPtr;

  // Adjust buffer pointer
  for (Idx = 0; Idx < EnumBufMax; Idx++) {
    Offset   = (UINT8 *)Stage1aParam->BufInfo[Idx].DstBase - (UINT8 *)OldLdrGlobal;
    FieldPtr = (VOID **)((UINT8 *)LdrGlobal + Offset);
    if (*FieldPtr != NULL) {
      *FieldPtr = (UINT8 *)(*FieldPtr) + Delta;
    }
  }

  // Re-allocate Lib Data
  LibDataPtr  = (LIBRARY_DATA *) LdrGlobal->LibDataPtr;
  if (LibDataPtr != NULL) {
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
  }

  // Re-allocate Log Buf if required
  if (LdrGlobal->LogBufPtr != NULL) {
    if (PcdGet32 (PcdEarlyLogBufferSize) < PcdGet32 (PcdLogBufferSize)) {
      // If log buffer needs to be bigger post memory, increase it.
      OldLogBuf = (DEBUG_LOG_BUFFER_HEADER *)LdrGlobal->LogBufPtr;
      NewLogBuf = (DEBUG_LOG_BUFFER_HEADER *)AllocatePool (PcdGet32 (PcdLogBufferSize));
      if (NewLogBuf != NULL) {
        CopyMem ((VOID *)NewLogBuf, (VOID *)OldLogBuf, OldLogBuf->UsedLength);
        NewLogBuf->TotalLength = PcdGet32 (PcdLogBufferSize);
        LdrGlobal->LogBufPtr = NewLogBuf;
      }
    }
  }

  // Copy device table to memory
  DeviceTable = (PLT_DEVICE_TABLE *) LdrGlobal->DeviceTable;
  if (DeviceTable != NULL) {
    AllocateLen = DeviceTable->DeviceNumber * sizeof (PLT_DEVICE) + sizeof (PLT_DEVICE_TABLE);
    LdrGlobal->DeviceTable = AllocatePool (AllocateLen);
    CopyMem (LdrGlobal->DeviceTable, DeviceTable, AllocateLen);
  }

  // Allocate container list
  AllocateLen = PcdGet32 (PcdContainerMaxNumber) * sizeof (CONTAINER_ENTRY) + sizeof (CONTAINER_LIST);
  LdrGlobal->ContainerList = AllocateZeroPool (AllocateLen);
  ContainerList = (CONTAINER_LIST *) LdrGlobal->ContainerList;
  if (ContainerList != NULL) {
    ContainerList->Signature   = CONTAINER_LIST_SIGNATURE;
    ContainerList->TotalLength = AllocateLen;
  }

  // Call back into board hooks post memory
  BoardInit (PostMemoryInit);
  AddMeasurePoint (0x2040);

  // Switch to memory-based stack and continue execution at ContinueFunc
  StackTop  = LdrGlobal->StackTop - (sizeof (STAGE2_PARAM) + sizeof (STAGE1B_PARAM) + 0x40);
  StackTop  = ALIGN_DOWN (StackTop, 0x100);
  Stage1bParamInMem = (STAGE1B_PARAM *)StackTop;
  CopyMem (Stage1bParamInMem, &Stage1bParam, sizeof (STAGE1B_PARAM));

  DEBUG ((DEBUG_INFO, "Switch to memory stack @ 0x%08X\n", StackTop));
  SwitchStack (ContinueFunc, Stage1bParamInMem, (VOID *)OldLdrGlobal, (VOID *)StackTop);
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

  @param[in]  Context1        Pointer to STAGE1B_PARAM in main memory.
  @param[in]  Context2        Unused.

**/
VOID
ContinueFunc (
  IN VOID                      *Context1,
  IN VOID                      *Context2
  )
{
  STAGE1B_PARAM            *Stage1bParam;
  STAGE2_PARAM             *Stage2Param;
  UINT32                    StackBot;
  UINT32                    Dst;
  UINT32                    StackTop;
  EFI_STATUS                Status;
  LOADER_GLOBAL_DATA       *LdrGlobal;
  LOADER_GLOBAL_DATA       *OldLdrGlobal;
  TPMI_ALG_HASH             MbTmpAlgHash;

  Stage1bParam   = (STAGE1B_PARAM *)Context1;
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

  if (MEASURED_BOOT_ENABLED()) {
    if (GetBootMode() != BOOT_ON_S3_RESUME) {
        //Convert Measured boot Hash Mask to TPMI_ALG_HASH (TPM ALG ID)
        MbTmpAlgHash = (TPMI_ALG_HASH) GetTpmHashAlg(PcdGet32(PcdMeasuredBootHashMask));

        // Extend External Config Data hash
        if (Stage1bParam->ConfigDataHashValid == 1) {
          TpmExtendPcrAndLogEvent ( 1,
                    MbTmpAlgHash,
                    Stage1bParam->ConfigDataHash,
                    EV_EFI_VARIABLE_DRIVER_CONFIG,
                    sizeof("Ext Config Data"),
                     (UINT8 *)"Ext Config Data");
        }

        // Extend Key hash manifest digest
        if (Stage1bParam->KeyHashManifestHashValid == 1) {
          TpmExtendPcrAndLogEvent (1,
                    MbTmpAlgHash,
                    Stage1bParam->KeyHashManifestHash,
                    EV_EFI_VARIABLE_DRIVER_CONFIG,
                    sizeof("Key Manifest"),
                     (UINT8 *)"Key Manifest");
        }
    }
  }

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ();
  DEBUG ((DEBUG_INFO, "Memory FSP @ 0x%08X\n", LdrGlobal->StackTop));
  DEBUG ((DEBUG_INFO, "Memory TOP @ 0x%08X\n", LdrGlobal->MemPoolStart));

  Dst = PrepareStage2 (Stage1bParam);
  if (Dst == 0) {
    CpuHalt ("Failed to load Stage2!");
  }

  // Configure stack
  StackTop = ALIGN_DOWN (LdrGlobal->StackTop - sizeof (STAGE2_PARAM), 0x10);

  // Build Stage2 Param
  Stage2Param = (STAGE2_PARAM *)StackTop;
  SetMem (Stage2Param, sizeof (STAGE2_PARAM), 0);
  Stage2Param->Stage2ExeBase = Dst;
  Stage2Param->PayloadBase   = Stage1bParam->PayloadBase;

  SwitchStack ((SWITCH_STACK_ENTRY_POINT)GET_STAGE_MODULE_ENTRY (Dst), Stage2Param, NULL, (VOID *)StackTop);

  // Error: Stage 2 returned!
  CpuHalt (NULL);
}
