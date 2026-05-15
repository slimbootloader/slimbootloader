/** @file
  Intel(R) SGX library.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <ConfigDataDefs.h>
#include <CpuDataStruct.h>
#include <Register/CpuRegs.h>
#include <PlatformData.h>
#include <Library/DebugLib.h>
#include <Library/RngLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/UefiVariableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Include/GlobalNvsArea.h>
#include <Library/SecurityCctVarStorageLib/4v0/MetadataNode.h>
#include <Library/SecurityCctVarStorageLib/4v0/VariableCommon.h>
#include <Library/PcdLib.h>
#include <Library/SgxLib22.h>
#include <SgxAcpiPm4v0Hob.h>
#include <Library/MemoryAllocationLib.h>


#define CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS 0x7
#define SGX_EPC_CPUID                           0x12

static const CHAR16 gSgxVariableManifestName[] = {'V','a','r','M','a','n','i','f','e','s','t','\0'};
static const CHAR16 gSgxUefiDisabledScenario[] = {'S','g','x','S','b','l','D','i','s','a','b','l','e','d','S','c','e','n','a','r','i','o','\0'};

static  CCT_VS_METADATA_NODE *mMetaDataNode = NULL;
static SGX_SBL_DATA_INFO_HOB  *SgxSblDataInfoHOB = NULL;

///
/// Intel(R) SGX status (Enabled/Disabled) setting from config
///
enum SGX_STATUS {
  CONFIG_SGX_DISABLED                         = 0x0,
  CONFIG_SGX_ENABLED                          = 0x1
};

/**
  Check on the processor if SGX is supported.

  @retval True if SGX supported or FALSE if not
**/
BOOLEAN
EFIAPI
IsSgxCapSupported (
  VOID
)
{
  EFI_CPUID_REGISTER CpuidRegs;

  ///
  /// Processor support SGX feature by reading CPUID.(EAX=7,ECX=0):EBX[2]
  ///
  AsmCpuidEx (CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS, 0,
              &CpuidRegs.RegEax,&CpuidRegs.RegEbx,&CpuidRegs.RegEcx,
              &CpuidRegs.RegEdx);

  ///
  /// SGX feature is supported only on SKL and later,
  /// with CPUID.(EAX=7,ECX=0):EBX[2]=1
  /// PRMRR configuration enabled, MSR IA32_MTRRCAP (FEh) [12] == 1
  ///
  if ((CpuidRegs.RegEbx & BIT2) && (AsmReadMsr64 (MSR_IA32_MTRRCAP) & BIT12)) {
    return TRUE;
  } else {
    DEBUG ((DEBUG_WARN, "\nIsSgxCapSupported(): Intel(R) SGX is not supported on this processor.\n"));
    return FALSE;
  }
}

/**
  Update FSP-M UPD SGX Persistent storage config data

  @param[in]  FspmUpd           The pointer to the FSP-M UPD to be updated.

**/
static EFI_STATUS
EFIAPI
SgxGetStorageVariables (
  VOID
)
{
  CCT_VS_METADATA_NODE *CctVsPrevNode = NULL;
  CCT_VS_METADATA_NODE *CctVsTempNode = NULL;
  UINT32 Index = 0;
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN DataSize = 0;
  VOID *Data = NULL;
  CCT_VAR_MANIFEST *SgxManifest = NULL;
  UINTN SgxManifestSize = 0;

  mMetaDataNode = NULL;

  /* Read Manifest from Guid*/
  Status = UefiGetVariable (gSgxVariableManifestName,
                            &gSgxVariableManifestGuid,
                            NULL, &SgxManifestSize, SgxManifest);

  if (Status == EFI_BUFFER_TOO_SMALL && SgxManifestSize > 0) {
    SgxManifest  = AllocatePages (EFI_SIZE_TO_PAGES (SgxManifestSize));
    if (SgxManifest == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }

    Status = UefiGetVariable (gSgxVariableManifestName,
                              &gSgxVariableManifestGuid,
                              NULL, &SgxManifestSize, SgxManifest);
  }

  if (Status != EFI_SUCCESS) {
    goto Exit;
  }

  if (SgxManifest->ArraySize == 0) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  for (Index = 0; Index < SgxManifest->ArraySize; Index++) {
    CctVsTempNode = (CCT_VS_METADATA_NODE *) AllocatePages
                      (EFI_SIZE_TO_PAGES (sizeof(CCT_VS_METADATA_NODE)));
    if (CctVsTempNode == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }

    ZeroMem (CctVsTempNode, sizeof(CCT_VS_METADATA_NODE));
    CctVsTempNode->Descriptor = SgxManifest->DescriptorArray[Index];
    DataSize = 0;
    Data = NULL;

    Status = UefiGetVariable (CctVsTempNode->Descriptor.Bios.Name,
                              &CctVsTempNode->Descriptor.Bios.Guid,
                              NULL, &DataSize, Data);

    if (Status == EFI_NOT_FOUND) {
      // Either Available or IsPresent flag was found unset in HOB.
      // hence the value is not set. Continue to read next manifest entry.
      Status = EFI_SUCCESS;
      continue;
    }

    if (Status == EFI_BUFFER_TOO_SMALL && DataSize > 0) {
      Data  = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
      if (Data == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }
      Status = UefiGetVariable (CctVsTempNode->Descriptor.Bios.Name,
                              &CctVsTempNode->Descriptor.Bios.Guid,
                              NULL, &DataSize, Data);
    }

    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "Meta Data read error. Exiting.\n"));
      goto Exit;
    }

    //EFI_PHYSICAL_ADDRESS is UINT64 while we are sending UINT32 address.
    //Hence first typecasted to UINT32 and then UINT64 to add leading 0.
    CctVsTempNode->DataPtr = (EFI_PHYSICAL_ADDRESS)(UINTN)Data;
    CctVsTempNode->NextNodePtr = (EFI_PHYSICAL_ADDRESS)0;

    if (Index == 0) {
      mMetaDataNode = CctVsTempNode;
    }

    if (CctVsPrevNode != NULL) {
      CctVsPrevNode->NextNodePtr = (EFI_PHYSICAL_ADDRESS)(UINTN)CctVsTempNode;
    }

    CctVsPrevNode = CctVsTempNode;
  }


Exit:
  //Free Memory SgxManifest
  if (SgxManifest != NULL)
    FreePages (SgxManifest, EFI_SIZE_TO_PAGES (SgxManifestSize));
  //Free Meta Node in case of failure only
  if (EFI_ERROR(Status)) {
    CctVsTempNode = mMetaDataNode;
    while (CctVsTempNode != NULL) {
      if (CctVsTempNode->DataPtr != (EFI_PHYSICAL_ADDRESS)0) {
        FreePages ((VOID *)((UINTN)CctVsTempNode->DataPtr), (UINTN)
          EFI_SIZE_TO_PAGES (CctVsTempNode->Descriptor.Bios.DataSize));
      }
      CctVsPrevNode = CctVsTempNode;
      CctVsTempNode = (VOID *)((UINTN)CctVsTempNode->NextNodePtr);

      FreePages (CctVsPrevNode, EFI_SIZE_TO_PAGES (sizeof(CCT_VS_METADATA_NODE)));
    }
    mMetaDataNode = NULL;
  }
  return Status;
}

/**
  Computes the value of the SGX uefi disabled enum to be sent to UEFI payload.

  @param[in]   PlatformData The pointer to PLATFORM_DATA structure.
  @param[out]  SblState     SGX state as read from the NVRAM.
  @retval  EFI_STATUS       Result of nvm read
**/
static VOID
EFIAPI
GetSgxUefiScenario(
  IN  PLATFORM_DATA_BL   *PlatformData
)
{
  EFI_STATUS                Status;
  UINTN DataSize = 0;
  SGX_UEFI_DISABLED_SCENARIO *SgxTempState = NULL;

  if (PlatformData != NULL) {
    Status = UefiGetVariable (gSgxUefiDisabledScenario,
                            &gSgxUefiDisabledScenarioGuid,
                            NULL, &DataSize, SgxTempState);

    if (Status == EFI_BUFFER_TOO_SMALL && DataSize > 0) {
      SgxTempState  = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
      Status = UefiGetVariable (gSgxUefiDisabledScenario,
                              &gSgxUefiDisabledScenarioGuid,
                              NULL, &DataSize, SgxTempState);
    }

    if(EFI_ERROR(Status)) {
      PlatformData->PlatformFeatures.SgxSblState = SgxUefiStatusRead;
      goto Exit;
    }

    switch(*SgxTempState) {
      // If the previous boot SGX state stored in NV variable is Disabled,
      // then set SGX to be disabled in this boot as well.
      // If any other state, then read the current state after sgx enabled execution
      // to be updated NV variable in Uefipayload.
      // If sgx state is SgxUefiDisabledFinal then SGX can be enabled after cold boot.
      case SgxUefiDisabledFinal:
      case SgxUefiEnabled:
      case SgxUefiStatusRead:
        PlatformData->PlatformFeatures.SgxSblState = SgxUefiStatusRead;
        break;
      case SgxUefiDisabled:
        PlatformData->PlatformFeatures.SgxSblState = SgxUefiDisabled;
        break;
      default:
        DEBUG ((DEBUG_WARN, "%a Invalid SGX state read from NVRAM\n"));
    }
  }

Exit:
  FreePages (SgxTempState, EFI_SIZE_TO_PAGES (DataSize));
}

/**
  Update FSP-M UPD SGX config data

  @param[in]  FspmUpd           The pointer to the FSP-M UPD to be updated.

**/
EFI_STATUS
EFIAPI
UpdateFspmSgxConfig (
  IN FSPM_UPD *FspmUpd
)
{
  EFI_STATUS         Status;
  SGX_CFG_DATA      *SgxCfgData;
  PLATFORM_DATA_BL  *PlatformData;

  Status = EFI_SUCCESS;
  SgxCfgData                = NULL;
  PlatformData              = GetPlatformDataPtr ();

  if (GetBootMode () == BOOT_ON_FLASH_UPDATE) {
    FspmUpd->FspmConfig.SecurityCctEnableSgx = CONFIG_SGX_DISABLED;
    Status = EFI_NOT_READY;
    goto Exit;
  }

  if (IsSgxCapSupported () == FALSE) {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  if (FspmUpd == NULL || PlatformData == NULL) {
    return EFI_NOT_FOUND;
  }

  GetSgxUefiScenario (PlatformData);

  // Reading SGX config data

  SgxCfgData = (SGX_CFG_DATA *) FindConfigDataByTag (CDATA_SGX_TAG);
  if (SgxCfgData == NULL) {
    Status = EFI_NOT_FOUND;
    goto Exit;
  }

  FspmUpd->FspmConfig.SecurityCctEnableSgx = SgxCfgData->EnableSgx;

  // GetPayloadId cant be checked in Stage1. So just check for SGX here
  // and delay for PayloadId check for Stage2.
  if (FspmUpd->FspmConfig.SecurityCctEnableSgx == CONFIG_SGX_ENABLED) {
    if (PlatformData->PlatformFeatures.SgxSblState != SgxUefiEnabled &&
      PlatformData->PlatformFeatures.SgxSblState != SgxUefiStatusRead) {
      FspmUpd->FspmConfig.SecurityCctEnableSgx = CONFIG_SGX_DISABLED;
      DEBUG ((DEBUG_WARN, "Intel(R) SGX set to disabled Retry in cold Boot.\n"));
      Status = EFI_NOT_READY;
      goto Exit;
    }

    // Setting EnableSgx and PrmrrSize from config data
    FspmUpd->FspmConfig.SecurityCctPrmrrSgxSize = SgxCfgData->PrmrrSize;
    FspmUpd->FspmConfig.SecurityCctSgxAutoRegistrationAgent = SgxCfgData->SgxAutoRegistration;
    FspmUpd->FspmConfig.SecurityCctSgxQoS = SgxCfgData->SgxQos;;
    FspmUpd->FspmConfig.SecurityCctSgxDebugMode = SgxCfgData->SgxDebugMode;
    FspmUpd->FspmConfig.SecurityCctSgxLeWr = SgxCfgData->SgxLeWr;
    FspmUpd->FspmConfig.SecurityCctSgxLePubKeyHash0 = SgxCfgData->SgxLePubKeyHash0;
    FspmUpd->FspmConfig.SecurityCctSgxLePubKeyHash1 = SgxCfgData->SgxLePubKeyHash1;
    FspmUpd->FspmConfig.SecurityCctSgxLePubKeyHash2 = SgxCfgData->SgxLePubKeyHash2;
    FspmUpd->FspmConfig.SecurityCctSgxLePubKeyHash3 = SgxCfgData->SgxLePubKeyHash3;
    FspmUpd->FspmConfig.SecurityCctEpochUpdate = SgxCfgData->EpochUpdate;
    FspmUpd->FspmConfig.SecurityCctSgxEpoch0 = SgxCfgData->SgxEpoch0;
    FspmUpd->FspmConfig.SecurityCctSgxEpoch1 = SgxCfgData->SgxEpoch1;

    /** updating Nvram variables here **/
    if (EFI_ERROR(SgxGetStorageVariables())) {
      FspmUpd->FspmConfig.SecurityCctVarStorageMetadataNodePtr = (EFI_PHYSICAL_ADDRESS)0;
    } else {
      FspmUpd->FspmConfig.SecurityCctVarStorageMetadataNodePtr =
                                        (EFI_PHYSICAL_ADDRESS)(UINTN)mMetaDataNode;
    }
  } else {
    DEBUG ((DEBUG_INFO, "SGX config is disabled\n"));
  }

Exit:
  PlatformData->PlatformFeatures.SgxSblExec = (Status == EFI_SUCCESS) ? 1 : 0;
  return Status;
}

/**
  Update SGX Meta data Parameters in stage2.

  @param[in]  FspsUpd           Pointer to the FSP-s UPD to be updated

  @retval TRUE if SGX config was found and set
  @retval FALSE if SGX config was not found

**/
EFI_STATUS
EFIAPI
UpdateFspsSgxConfig (
  IN FSPS_UPD *FspsUpd
)
{
  SGX_CFG_DATA      *SgxCfgData;

  SgxCfgData = (SGX_CFG_DATA *) FindConfigDataByTag (CDATA_SGX_TAG);
  if (SgxCfgData == NULL) {
    return EFI_NOT_FOUND;
  }

  if (SgxCfgData->EnableSgx == CONFIG_SGX_DISABLED) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR(SgxGetStorageVariables())) {
    FspsUpd->FspsConfig.SecurityCctVarStorageMetadataNodePtr = (EFI_PHYSICAL_ADDRESS)0;
  } else {
    FspsUpd->FspsConfig.SecurityCctVarStorageMetadataNodePtr =
                                      (EFI_PHYSICAL_ADDRESS)(UINTN)mMetaDataNode;
  }

  return EFI_SUCCESS;
}

/**
  Update SGX ACPI Parameters to ACPI Table.

  @param void

**/
EFI_STATUS
EFIAPI
UpdateSgxAcpiSblData (
  VOID
)
{
  GLOBAL_NVS_AREA           *Gnvs;
  SGX_ACPI_PM_4V0_HOB       *SgxAcpiParameter;
  UINT8                     Index;
  VOID                      *GuidHob = NULL;
  BIOS_ACPI_PARAM           *AcpiParameter;

  Gnvs = (GLOBAL_NVS_AREA *)(UINTN) PcdGet32 (PcdAcpiGnvsAddress);
  AcpiParameter = &Gnvs->BiosAcpiParam;
  if (AcpiParameter == NULL) {
    DEBUG ((DEBUG_ERROR, "%a(): Unable to get BIOS_ACPI_PARAM.\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  GuidHob = GetNextGuidHob (&gSgxAcpiVarGuid, GetFspHobListPtr());
  if(GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "%a(): Unable to get gSgxAcpiVarGuid: %g\n",
        __func__, &gSgxAcpiVarGuid));
    return EFI_NOT_FOUND;
  }

  SgxAcpiParameter  = (SGX_ACPI_PM_4V0_HOB *) GET_GUID_HOB_DATA (GuidHob);

  for (Index = 0; Index < MAX_PRMRR_CPU; Index++) {
    AcpiParameter->EpcBaseAddress[Index] = SgxAcpiParameter->SgxAcpiEpc.EpcBaseAddress[Index];
    AcpiParameter->EpcLength[Index] = SgxAcpiParameter->SgxAcpiEpc.EpcLength[Index];
  }
  AcpiParameter->SgxStatus = (UINT8) SgxAcpiParameter->SgxAcpiEpc.SgxStatus;

  return EFI_SUCCESS;
}

/**
  Create SBL to UEFI Payload HOB.

  @param void

**/
VOID
EFIAPI
BuildSgxSblDataInfoHob (
  VOID
)
{
  SGX_ACPI_PM_4V0_HOB       *SgxAcpiParameter;
  VAR_DATA_INFO_HOB         *SgxVarDataInfoHob;
  VOID                      *GuidHob = NULL;
  CCT_VS_VAR_DESCRIPTOR     *LocalMetaHob  = NULL;
  VOID                      *LocalGuidHob = NULL;
  PLATFORM_DATA_BL          *PlatformData;
  SGX_CFG_DATA              *SgxCfgData;

  PlatformData = GetPlatformDataPtr ();

  if (PlatformData == NULL) {
    DEBUG ((DEBUG_ERROR, "%a(): Platform Data is NULL.\n", __func__));
    return;
  }

  if (PlatformData->PlatformFeatures.SgxSblExec == 0) {
    DEBUG ((DEBUG_ERROR, "%a(): SBL FSPM upd exec has failed.\n", __func__));
    return;
  }

  // Check if processor supports SGX
  if (IsSgxCapSupported () == FALSE) {
    DEBUG ((DEBUG_ERROR, "%a(): SGX not supported.\n", __func__));
    return;
  }

  // Reading SGX config data
  SgxCfgData = (SGX_CFG_DATA *) FindConfigDataByTag (CDATA_SGX_TAG);
  if (SgxCfgData != NULL) {
    // If SGX enabled and not UEFI Payload, invalid config. So Halt.
    if ((SgxCfgData->EnableSgx) && (GetPayloadId () != UEFI_PAYLOAD_ID_SIGNATURE)) {
      CpuHalt ("Invalid SGX Config\n");
    }
  }

  //Building SgxSblDataInfoHOB
  SgxSblDataInfoHOB = BuildGuidHob (&gSgxSblDataInfoHobGuid, sizeof(SGX_SBL_DATA_INFO_HOB));
  if (SgxSblDataInfoHOB == NULL) {
    DEBUG ((DEBUG_ERROR, "%a(): Unable to create a ACPI HOB.\n", __func__));
  }

  ZeroMem (SgxSblDataInfoHOB, sizeof(SGX_SBL_DATA_INFO_HOB));

  GuidHob = GetNextGuidHob (&gSgxAcpiVarGuid, GetFspHobListPtr());
  if(GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "%a(): Unable to get gSgxAcpiVarGuid: %g\n",
          __func__, gSgxAcpiVarGuid));
  } else {
    SgxAcpiParameter  = (SGX_ACPI_PM_4V0_HOB *) GET_GUID_HOB_DATA (GuidHob);

    SgxSblDataInfoHOB->SgxResetFlow = SgxAcpiParameter->SgxResetFlow;
    SgxSblDataInfoHOB->SgxFspRequest = SgxAcpiParameter->SgxFspRequest;
  }

  //Also Adding the Sgx State variable.
  if (PlatformData != NULL) {
    SgxSblDataInfoHOB->SgxSblCfgState = (UINT8)PlatformData->PlatformFeatures.SgxSblState;
  }

  //Building Variable Hob
  GuidHob = GetNextGuidHob (&gSgxMetaHobGuid, GetFspHobListPtr());

  if (GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "%a(): Unable to get gSgxMetaHobGuid: %g.\n",
            __func__, &gSgxMetaHobGuid));
    return;
  }

  while (GuidHob != NULL) {
    LocalMetaHob  = (CCT_VS_VAR_DESCRIPTOR *) GET_GUID_HOB_DATA (GuidHob);
    //Reading data from Descriptor Hob Guid
    LocalGuidHob = GetNextGuidHob (&LocalMetaHob->Identity.DescriptorGuid, GetFspHobListPtr());

    if((LocalGuidHob != NULL) && (LocalMetaHob->Bios.Available == TRUE) &&
        (LocalMetaHob->Bios.IsPresent == TRUE)) {
      // Reading from Desc Guid and saving it in new HOB
      SgxVarDataInfoHob = BuildGuidHob (&gSgxSblVariableHobGuid, sizeof(VAR_DATA_INFO_HOB));
      if (SgxVarDataInfoHob == NULL) {
        DEBUG ((DEBUG_ERROR, "%a(): Unable to create a ACPI VAR HOB.\n", __func__));
        return;
      }

      ZeroMem (SgxVarDataInfoHob, sizeof(VAR_DATA_INFO_HOB));

      SgxVarDataInfoHob->Guid = LocalMetaHob->Bios.Guid;
      CopyMem (SgxVarDataInfoHob->Name, LocalMetaHob->Bios.Name,
                (MAX_VAR_DESCRIPTOR_NAME_LEN*sizeof(CHAR16)));
      SgxVarDataInfoHob->Attr = LocalMetaHob->Bios.Attr;
      SgxVarDataInfoHob->DataSize = LocalMetaHob->Bios.DataSize;
      SgxVarDataInfoHob->Data = (VOID *) GET_GUID_HOB_DATA (LocalGuidHob);
    }

    // Move to next HOB
    GuidHob = GET_NEXT_HOB (GuidHob);
    GuidHob = GetNextGuidHob (&gSgxMetaHobGuid, GuidHob);
  }

  DEBUG ((DEBUG_INFO, "%a(): Completed\n", __func__));
}

/**
  Free Allocated memory for SGX variables
**/
VOID
EFIAPI
FreeSgxMem (
  VOID
)
{
  CCT_VS_METADATA_NODE *TempNode = mMetaDataNode;
  CCT_VS_METADATA_NODE *NodeToDelete;

  while (TempNode != NULL) {
      if (TempNode->DataPtr != 0) {
      FreePages ((VOID*)((UINTN)TempNode->DataPtr), (UINTN)
        EFI_SIZE_TO_PAGES (TempNode->Descriptor.Bios.DataSize));
    }
    NodeToDelete = TempNode;
    TempNode = (VOID *)((UINTN)TempNode->NextNodePtr);

    FreePages (NodeToDelete,
        EFI_SIZE_TO_PAGES (sizeof(CCT_VS_METADATA_NODE)));

  }

  mMetaDataNode = NULL;
}
