/** @file
  Intel(R) SGX library.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <RegAccess.h>
#include <FspmUpd.h>
#include <FspsUpd.h>
#include <ConfigDataDefs.h>
#include <CpuDataStruct.h>
#include <Register/CpuRegs.h>
#include <PlatformData.h>
#include <Library/DebugLib.h>
#include <Library/RngLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/UefiVariableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SgxLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/TmeVarLib.h>
#include <Include/GlobalNvsArea.h>

#define CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS 0x7
#define SGX_EPC_CPUID                           0x12
//Uncomment SGX_DUMP_NVRAM_DATA to dump data from NVRAM
// #define SGX_DUMP_NVRAM_DATA

static const CHAR16 gSgxUefiRegistrationConfigName[]    = SGX_UEFI_REGISTRATION_CONFIG_VARIABLE_NAME;
static const CHAR16 gSgxRegistrationPackageInfoName[]   = SGX_REGISTRATION_PACKAGE_INFO_VARIABLE_NAME;
static const CHAR16 gSgxUefiRegistrationStatusName[]    = SGX_UEFI_REGISTRATION_STATUS_VARIABLE_NAME;
static const CHAR16 gSgxUefiRegistrationResponseName[]  = SGX_UEFI_REGISTRATION_RESPONSE_VARIABLE_NAME;
static const CHAR16 gSgxKeyBlobsName[]                  = SGX_UEFIFW_KEY_BLOB_VARIABLE_NAME;
static const CHAR16 gSgxUefiRegistrationStateName[]     = SGX_UEFIFW_NONVOLATILVE_REGISTRATION_STATE_VARIABLE_NAME;
static const CHAR16 gSecurityIpInterdependence[]        = SECURITY_IP_INTERDEPENDENCE_HASH_NAME;
static const CHAR16 gSgxUefiRegistrationServerRequest[] = SGX_UEFI_REGISTRATION_REQUEST_VARIABLE_NAME;
static const CHAR16 gSgxUefiDisabledScenario[]          = SGX_SBL_DISABLED_SCENARIO_NAME;

static UINT8   *mSgxRegistrationConfig = NULL; // SGX_REGISTRATION_CONFIG
static UINT8   *mSgxRegistrationPackageInfo = NULL; // SGX_REGISTRATION_PACKAGE_INFO
static UINT8   *mSgxRegistrationStatus = NULL; //SGX_REGISTRATION_STATUS
static UINT8   *mSgxRegistrationResponse = NULL; // SGX_REGISTRATION_RESPONSE
static UINT8   *mSgxRegistrationState = NULL; //SGX_UEFIFW_NONVOLATILVE_REGISTRATION_STAT
static UINT8   *mUefiFwKeyBlobs = NULL; //SGX_UEFIFW_KEY_BLOB
static UINT8   *mSecurityIpInterdependencePreviousBoot = NULL;
static UINT8   *mSgxRegistrationServerRequest = NULL; //SGX_REGISTRATION_REQUEST

//Holds the size of allocated pages
static UINT32 msizeofSgxRegistrationConfig;
static UINT32 msizeofSgxRegistrationStatus;
static UINT32 msizeofSgxRegistrationPackageInfo;
static UINT32 msizeofSgxRegistrationResponse;
static UINT32 msizeofSgxUefifwKeyBlob;
static UINT32 msizeofSgxUefifwNonVolatilveRegistrationState;
static UINT32 msizeofSgxRegistrationRequest;

///
/// Intel(R) SGX status (Enabled/Disabled) setting from config
///
enum SGX_STATUS {
  CONFIG_SGX_DISABLED                         = 0x0,
  CONFIG_SGX_ENABLED                          = 0x1
};

///
/// PRMRR sizes 1MB & 2MB are valid for C6DRAM, but not Intel(R) SGX
///
enum SUPPORTED_PRMRR_SIZES_FOR_SGX {
  PRMRR_32                                    = 0x20,
  PRMRR_32_64                                 = 0x60,
  PRMRR_32_64_128                             = 0xE0,
  PRMRR_32_64_128_256                         = 0x1E0
};

///
/// Intel(R) SGX owner epoch update setting from config
///
enum SGX_OWNER_EPOCH_UPDATE {
  CONFIG_SGX_NO_EPOCH_UPDATE                         = 0x0,
  CONFIG_SGX_MANUAL_EPOCH_UPDATE                     = 0x1
};

/**
  Get SGX Registration Config Variable

  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxRegistrationConfigVariable (
  VOID
)
{
  EFI_STATUS  Status;
  UINTN DataSize;

  DataSize = 0;

  Status = UefiGetVariable (gSgxUefiRegistrationConfigName,
                            &gSgxUefiRegistrationConfigVariableGuid,
                            NULL, &DataSize, mSgxRegistrationConfig);

  if (Status == EFI_BUFFER_TOO_SMALL && DataSize > 0) {
    mSgxRegistrationConfig  = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
    msizeofSgxRegistrationConfig = (UINT32) DataSize;
    Status = UefiGetVariable (gSgxUefiRegistrationConfigName,
                              &gSgxUefiRegistrationConfigVariableGuid,
                              NULL, &DataSize, mSgxRegistrationConfig);
  }

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "RdNvGetSgxRegistrationConfigVariable: Success\n"));
    #ifdef SGX_DUMP_NVRAM_DATA
    DumpHex (2, 0, DataSize, (VOID *)mSgxRegistrationConfig);
    #endif
  }
  DEBUG ((DEBUG_INFO, "%a(): Completed Status 0x%x\n", __FUNCTION__, Status));
  return Status;
}

/**
  Get SGX Registration Package Info Variable

  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxRegistrationPackageInfoVariable (
  VOID
)
{
  EFI_STATUS  Status;
  UINTN DataSize;

  DataSize = 0;

  Status = UefiGetVariable (gSgxRegistrationPackageInfoName,
                            &gSgxRegistrationPackageInfoVariableGuid,
                            NULL, &DataSize, mSgxRegistrationPackageInfo);

  if (Status == EFI_BUFFER_TOO_SMALL && DataSize > 0) {
    mSgxRegistrationPackageInfo = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
    msizeofSgxRegistrationPackageInfo = (UINT32) DataSize;
    Status = UefiGetVariable (gSgxRegistrationPackageInfoName,
                              &gSgxRegistrationPackageInfoVariableGuid,
                              NULL, &DataSize, mSgxRegistrationPackageInfo);
  }

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "RdNvGetSgxRegistrationPackageInfoVariable: Success\n"));
    #ifdef SGX_DUMP_NVRAM_DATA
    DumpHex (2, 0, DataSize, (VOID *)mSgxRegistrationPackageInfo);
    #endif
  }
  DEBUG ((DEBUG_INFO, "%a(): Completed Status 0x%x\n", __FUNCTION__, Status));
  return Status;
}

/**
  Get SGX Registration Package Info Variable

  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxUefiFwRegistrationStateVariableGuid (
  VOID
)
{
  EFI_STATUS  Status;
  UINTN DataSize;

  DataSize = 0;

  Status = UefiGetVariable (gSgxUefiRegistrationStateName,
                            &gSgxUefiFwRegistrationStateVariableGuid,
                            NULL, &DataSize, mSgxRegistrationState);
  if (Status == EFI_BUFFER_TOO_SMALL && DataSize > 0) {
    mSgxRegistrationState = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
    msizeofSgxUefifwNonVolatilveRegistrationState = (UINT32) DataSize;
    Status = UefiGetVariable (gSgxUefiRegistrationStateName,
                              &gSgxUefiFwRegistrationStateVariableGuid,
                              NULL, &DataSize, mSgxRegistrationState);
  }

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "RdNvGetSgxUefiFwRegistrationStateVariableGuid: Success\n"));
    #ifdef SGX_DUMP_NVRAM_DATA
    DumpHex (2, 0, DataSize, (VOID *)mSgxRegistrationState);
    #endif
  }
  DEBUG ((DEBUG_INFO, "%a(): Completed Status 0x%x\n", __FUNCTION__, Status));
  return Status;
}

/**
  Get SGX Registration Server Response Variable

  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxRegistrationServerResponseVariable (
  VOID
)
{
  EFI_STATUS  Status;
  UINTN DataSize;

  DataSize = 0;

  Status = UefiGetVariable (gSgxUefiRegistrationResponseName,
                            &gSgxUefiRegistrationServerResponseVariableGuid,
                            NULL, &DataSize, mSgxRegistrationResponse);

  if (Status == EFI_BUFFER_TOO_SMALL && DataSize > 0) {
    mSgxRegistrationResponse = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
    msizeofSgxRegistrationResponse = (UINT32) DataSize;
    Status = UefiGetVariable (gSgxUefiRegistrationResponseName,
                              &gSgxUefiRegistrationServerResponseVariableGuid,
                              NULL, &DataSize, mSgxRegistrationResponse);
  }

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "RdNvGetSgxRegistrationServerResponseVariable: Success\n"));
    #ifdef SGX_DUMP_NVRAM_DATA
    DumpHex (2, 0, DataSize, (VOID *)SgxRegistrationResponse);
    #endif
  }
  DEBUG ((DEBUG_INFO, "%a(): Completed Status 0x%x\n", __FUNCTION__, Status));
  return Status;
}

/**
  Get SGX Registration Status Variable

  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxRegistrationStatusVariable (
  VOID
)
{
  EFI_STATUS  Status;
  UINTN DataSize;

  DataSize = 0;

  Status = UefiGetVariable (gSgxUefiRegistrationStatusName,
                            &gSgxUefiRegistrationStatusVariableGuid,
                            NULL, &DataSize, mSgxRegistrationStatus);

  if (Status == EFI_BUFFER_TOO_SMALL && DataSize > 0) {
    mSgxRegistrationStatus  = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
    msizeofSgxRegistrationStatus = (UINT32) DataSize;
    Status = UefiGetVariable (gSgxUefiRegistrationStatusName,
                              &gSgxUefiRegistrationStatusVariableGuid,
                              NULL, &DataSize, mSgxRegistrationStatus);
  }

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "RdNvGetSgxRegistrationStatusVariable: Success\n"));
    #ifdef SGX_DUMP_NVRAM_DATA
    DumpHex (2, 0, DataSize, (VOID *)mSgxRegistrationStatus);
    #endif
  }
  DEBUG ((DEBUG_INFO, "%a(): Completed Status 0x%x\n", __FUNCTION__, Status));
  return Status;
}

/**
  Get SGX Keyblob

  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetKeyBlobs(
  VOID
)
{
  EFI_STATUS  Status;
  UINTN DataSize;

  DataSize = 0;

  Status = UefiGetVariable (gSgxKeyBlobsName,
                            &gSgxUefiFwKeyBlobsVariableGuid,
                            NULL, &DataSize, mUefiFwKeyBlobs);

  if (Status == EFI_BUFFER_TOO_SMALL && DataSize > 0) {
    mUefiFwKeyBlobs = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
    msizeofSgxUefifwKeyBlob = (UINT32) DataSize;
    Status = UefiGetVariable (gSgxKeyBlobsName,
                              &gSgxUefiFwKeyBlobsVariableGuid,
                              NULL, &DataSize, mUefiFwKeyBlobs);
  }

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "RdNvGetKeyBlobs: Success\n"));
    #ifdef SGX_DUMP_NVRAM_DATA
    DumpHex (2, 0, DataSize, (VOID *)mUefiFwKeyBlobs);
    #endif
  }
  DEBUG ((DEBUG_INFO, "%a(): Completed Status 0x%x\n", __FUNCTION__, Status));
  return Status;
}


/**
  Get SGX Security Ip Interdependence struct
  @param[out]  SecurityIpInterdependence    Security Ip Interdependence
  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSecurityIpInterdependence(
  OUT UINT8 *SecurityIpInterdependence
)
{
  EFI_STATUS  Status;
  UINTN DataSize;

  if (!SecurityIpInterdependence) {
    return EFI_INVALID_PARAMETER;
  }

  DataSize = sizeof (UINT8) * SHA384_DIGEST_SIZE;

  Status = UefiGetVariable (gSecurityIpInterdependence,
                            &gSecIpInterdependenceHashGuid,
                            NULL, &DataSize, SecurityIpInterdependence);

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "RdNvGetSecurityIpInterdependence: Success\n"));
    #ifdef SGX_DUMP_NVRAM_DATA
    DumpHex (2, 0, DataSize, (VOID *)SecurityIpInterdependence);
    #endif
  }
  DEBUG ((DEBUG_INFO, "%a(): Completed Status 0x%x\n", __FUNCTION__, Status));
  return Status;
}

/**
  Get SGX Registration Server Request

  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxRegistrationServerRequest (
  VOID
)
{
  EFI_STATUS  Status;
  UINTN DataSize;

  DataSize = 0;

  Status = UefiGetVariable (gSgxUefiRegistrationServerRequest,
                            &gSgxUefiRegistrationServerRequestVariableGuid,
                            NULL, &DataSize, mSgxRegistrationServerRequest);

  if (Status == EFI_BUFFER_TOO_SMALL && DataSize > 0) {
    mSgxRegistrationServerRequest = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
    msizeofSgxRegistrationRequest = (UINT32) DataSize;
    Status = UefiGetVariable (gSgxUefiRegistrationServerRequest,
                              &gSgxUefiRegistrationServerRequestVariableGuid,
                              NULL, &DataSize, mSgxRegistrationServerRequest);
  }

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "RdNvGetSgxRegistrationServerRequest: Success\n"));
    #ifdef SGX_DUMP_NVRAM_DATA
    DumpHex (2, 0, DataSize, (VOID *)SgxRegistrationServerRequest);
    #endif
  }
  DEBUG ((DEBUG_INFO, "%a(): Completed Status 0x%x\n", __FUNCTION__, Status));
  return Status;
}


/**
  Get SGX UEFI disabled nvram variable
  @param[out]  SblState    SGX state as read from the NVRAM.
  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxUefiDisabledScenario(
  OUT SGX_SBL_DISABLED_SCENARIO *SblState
)
{
  EFI_STATUS  Status;
  UINTN DataSize;

  if (!SblState) {
    return EFI_INVALID_PARAMETER;
  }

  DataSize = sizeof (SGX_SBL_DISABLED_SCENARIO);

  Status = UefiGetVariable (gSgxUefiDisabledScenario,
                            &gSgxUefiDisabledScenarioGuid, NULL, &DataSize, SblState);

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "RdNvGetSgxUefiDisabledScenario: Success\n"));
    #ifdef SGX_DUMP_NVRAM_DATA
    DumpHex (2, 0, DataSize, (VOID *)SblState);
    #endif
  }
  DEBUG ((DEBUG_INFO, "%a(): Completed Status 0x%x\n", __FUNCTION__, Status));
  return Status;
}

/**
  Computes the value of the SGX uefi disabled enum to be sent to UEFI payload.

  @param[in]   PlatformData The pointer to PLATFORM_DATA structure.
  @param[out]  SblState     SGX state as read from the NVRAM.
  @retval  EFI_STATUS       Result of nvm read
**/
VOID
EFIAPI
GetSgxUefiScenario(
  IN  PLATFORM_DATA   *PlatformData
)
{
  EFI_STATUS                Status;
  SGX_SBL_DISABLED_SCENARIO TempState;

  if (PlatformData != NULL) {
    Status = RdNvGetSgxUefiDisabledScenario(&TempState);
    if(!EFI_ERROR (Status)){
      if (TempState == SgxUefiDisabledFinal) {
        PlatformData->PlatformFeatures.SgxSblState = SgxUefiStatusRead;
      } else {
        PlatformData->PlatformFeatures.SgxSblState = TempState;
      }
    } else {
      PlatformData->PlatformFeatures.SgxSblState = SgxUefiEnabled;
    }
    DEBUG ((DEBUG_INFO, "%a(): Completed Sgx retry State:0x%x\n", __FUNCTION__,PlatformData->PlatformFeatures.SgxSblState));
  }
}

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
  Update FSP-M UPD SGX config data

  @param[in]  FspmUpd           The pointer to the FSP-M UPD to be updated.

**/
BOOLEAN
EFIAPI
UpdateFspmSgxConfig (
  IN FSPM_UPD *FspmUpd
)
{
  BOOLEAN           UpdateFspmSgxConfigStatus;
  SGX_CFG_DATA     *SgxCfgData;
  PLATFORM_DATA    *PlatformData;

  UpdateFspmSgxConfigStatus = FALSE;
  SgxCfgData                = NULL;
  PlatformData              = GetPlatformDataPtr ();

  if (GetBootMode () == BOOT_ON_FLASH_UPDATE) {
    FspmUpd->FspmConfig.PcdSgxEnable = CONFIG_SGX_DISABLED;
    return UpdateFspmSgxConfigStatus;
  }

  if (FspmUpd == NULL || PlatformData == NULL) {
    return UpdateFspmSgxConfigStatus;
  }

   if (IsSgxCapSupported () == FALSE) {
    return UpdateFspmSgxConfigStatus;
  }
  GetSgxUefiScenario (PlatformData);

  // Reading SGX config data

  SgxCfgData = (SGX_CFG_DATA *) FindConfigDataByTag (CDATA_SGX_TAG);
  if (SgxCfgData != NULL) {
    // GetPayloadId cant be checked in Stage1. So just check for SGX and TME here
    // and delay for PayloadId check for Stage2.
    if ((SgxCfgData->EnableSgx == CONFIG_SGX_ENABLED) && (IsTmeEnabled() == TRUE)) {
        // Setting EnableSgx and PrmrrSize from config data

        if (PlatformData->PlatformFeatures.SgxSblState == SgxUefiEnabled ||
            PlatformData->PlatformFeatures.SgxSblState == SgxUefiStatusRead) {
          FspmUpd->FspmConfig.PcdSgxEnable = SgxCfgData->EnableSgx;
        } else {
          FspmUpd->FspmConfig.PcdSgxEnable = CONFIG_SGX_DISABLED;
          DEBUG ((DEBUG_WARN, "Intel(R) SGX set to disabled Retry in cold Boot.\n"));
        }
        FspmUpd->FspmConfig.PcdPrmrrSize = SgxCfgData->PrmrrSize;
        FspmUpd->FspmConfig.PcdSgxAutoRegistrationAgent = SgxCfgData->SgxAutoRegistration;
        FspmUpd->FspmConfig.PcdSgxQoS = SgxCfgData->SgxQos;;
        FspmUpd->FspmConfig.PcdSgxDebugMode = SgxCfgData->SgxDebugMode;
        FspmUpd->FspmConfig.PcdSgxLeWr = SgxCfgData->SgxLeWr;
        FspmUpd->FspmConfig.PcdSgxLePubKeyHash0 = SgxCfgData->SgxLePubKeyHash0;
        FspmUpd->FspmConfig.PcdSgxLePubKeyHash1 = SgxCfgData->SgxLePubKeyHash1;
        FspmUpd->FspmConfig.PcdSgxLePubKeyHash2 = SgxCfgData->SgxLePubKeyHash2;
        FspmUpd->FspmConfig.PcdSgxLePubKeyHash3 = SgxCfgData->SgxLePubKeyHash3;
        UpdateFspmSgxConfigStatus = TRUE;
    } else if (SgxCfgData->EnableSgx == CONFIG_SGX_DISABLED) {
      DEBUG ((DEBUG_WARN, "Intel(R) SGX set to disabled in config.\n"));
      FspmUpd->FspmConfig.PcdSgxEnable = CONFIG_SGX_DISABLED;
    } else {
      DEBUG ((DEBUG_ERROR, "Invalid SGX config.\n"));
      FspmUpd->FspmConfig.PcdSgxEnable = CONFIG_SGX_DISABLED;
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to find Intel(R) SGX CFG!\n"));
  }

  PlatformData->PlatformFeatures.SgxSblExec = (UpdateFspmSgxConfigStatus == TRUE) ? 1 : 0;

  return UpdateFspmSgxConfigStatus;
}

/**
  Update FSP-S UPD SGX config data

  @param[in]  FspsUpd           The pointer to the FSP-S UPD to be updated.

**/
BOOLEAN
EFIAPI
UpdateFspsSgxConfig (
  IN FSPS_UPD *FspsUpd
)
{
  BOOLEAN           UpdateFspsSgxConfigStatus;
  SGX_CFG_DATA     *SgxCfgData;
  PLATFORM_DATA    *PlatformData;

  UpdateFspsSgxConfigStatus = FALSE;
  SgxCfgData                = NULL;
  PlatformData              = GetPlatformDataPtr ();

  if (GetBootMode () == BOOT_ON_FLASH_UPDATE) {
    return UpdateFspsSgxConfigStatus;
  }

  if (FspsUpd == NULL || PlatformData == NULL) {
    return UpdateFspsSgxConfigStatus;
  }

  if (PlatformData->PlatformFeatures.SgxSblExec == 0) {
    return UpdateFspsSgxConfigStatus;
  }

  // Check if processor supports SGX

  if (IsSgxCapSupported () == FALSE) {
    return UpdateFspsSgxConfigStatus;
  }

  // Reading SGX config data
  SgxCfgData = (SGX_CFG_DATA *) FindConfigDataByTag (CDATA_SGX_TAG);
  if (SgxCfgData != NULL) {

    // If SGX enabled and not UEFI Payload, invalid config. So Halt.
    if ((SgxCfgData->EnableSgx) && (GetPayloadId () != UEFI_PAYLOAD_ID_SIGNATURE)) {
      CpuHalt ("Invalid SGX Config\n");
    }

    // Setting SGX registration config. Defaults to zero
    if (EFI_SUCCESS != RdNvGetSgxRegistrationConfigVariable ()) {
      FspsUpd->FspsConfig.PcdSgxRegistrationConfigPtr = 0x00000000;
      mSgxRegistrationConfig = NULL;
    } else {
        FspsUpd->FspsConfig.PcdSgxRegistrationConfigPtr = (UINT32)(UINTN)mSgxRegistrationConfig;
    }

    // Setting SGX registration status. Defaults to zero
    if (EFI_SUCCESS != RdNvGetSgxRegistrationStatusVariable ()) {
      FspsUpd->FspsConfig.PcdSgxRegistrationStatusPtr = 0x00000000;
      mSgxRegistrationStatus = NULL;
    } else {
      FspsUpd->FspsConfig.PcdSgxRegistrationStatusPtr = (UINT32)(UINTN)mSgxRegistrationStatus;
    }

    // Setting SGX registration package info. Defaults to zero
    if (EFI_SUCCESS != RdNvGetSgxRegistrationPackageInfoVariable ()) {
      FspsUpd->FspsConfig.PcdSgxRegistrationPackageInfoPtr = 0;
      mSgxRegistrationPackageInfo = NULL;
    } else {
      FspsUpd->FspsConfig.PcdSgxRegistrationPackageInfoPtr = (UINT32)(UINTN)mSgxRegistrationPackageInfo;
    }

    // Setting SGX registration server response. Defaults to zero
    if (EFI_SUCCESS != RdNvGetSgxRegistrationServerResponseVariable ()) {
      FspsUpd->FspsConfig.PcdSgxRegistrationResponsePtr = 0;
      mSgxRegistrationResponse = NULL;
    } else {
      FspsUpd->FspsConfig.PcdSgxRegistrationResponsePtr = (UINT32)(UINTN)mSgxRegistrationResponse;
    }

    // Setting SGX key blobs Defaults to zero
    if (EFI_SUCCESS != RdNvGetKeyBlobs ()) {
      FspsUpd->FspsConfig.PcdSgxUefiFwKeyBlobsPtr = 0;
      mUefiFwKeyBlobs = NULL;
    } else {
      FspsUpd->FspsConfig.PcdSgxUefiFwKeyBlobsPtr = (UINT32)(UINTN)mUefiFwKeyBlobs;
    }

    // Setting SGX Registration State Defaults to zero
    if (EFI_SUCCESS != RdNvGetSgxUefiFwRegistrationStateVariableGuid ()) {
      FspsUpd->FspsConfig.PcdSgxUefiFwRegistrationStatePtr = 0;
      mSgxRegistrationState = NULL;
    } else {
      FspsUpd->FspsConfig.PcdSgxUefiFwRegistrationStatePtr = (UINT32)(UINTN)mSgxRegistrationState;
    }

    // Note this is a binary blob read, if there is a size change in the FSP. need to calculate the update this.
    // This is equivalent of updating the headerfiles. This needs a lot of header files. To avoid adding those
    // a binary blob is read/written by SBL/UEFI PLD.
    // UINT8 mSecurityIpInterdependencePreviousBoot[201];
    mSecurityIpInterdependencePreviousBoot = AllocatePages (EFI_SIZE_TO_PAGES (sizeof (UINT8)*SHA384_DIGEST_SIZE));
    if (mSecurityIpInterdependencePreviousBoot != NULL) {
      if (EFI_SUCCESS != RdNvGetSecurityIpInterdependence (mSecurityIpInterdependencePreviousBoot)) {
        FspsUpd->FspsConfig.PcdSecIpInterdepPrevHash = 0;
        FreePages (mSecurityIpInterdependencePreviousBoot, EFI_SIZE_TO_PAGES (sizeof (UINT8)*SHA384_DIGEST_SIZE));
        mSecurityIpInterdependencePreviousBoot = NULL;
      } else {
        FspsUpd->FspsConfig.PcdSecIpInterdepPrevHash = (UINT32)(UINTN)mSecurityIpInterdependencePreviousBoot;
      }
    }

    // Setting SGX Registration Server Response Defaults to zero
    if (EFI_SUCCESS != RdNvGetSgxRegistrationServerRequest ()) {
      FspsUpd->FspsConfig.PcdSgxUefiFwRegistrationStatePtr = 0;
      mSgxRegistrationServerRequest = NULL;
    } else {
      FspsUpd->FspsConfig.PcdSgxRegistrationServerRequestPtr = (UINT32)(UINTN)mSgxRegistrationServerRequest;
    }

    DEBUG ((DEBUG_INFO, "Updated the required SGX FSP-S config!\n"));
    UpdateFspsSgxConfigStatus = TRUE;
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to find Intel(R) SGX CFG!\n"));
  }
  return UpdateFspsSgxConfigStatus;
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
  SGX_SBL_DATA_INFO_HOB     *SgxSblDataInfoHOB;
  GLOBAL_NVS_AREA           *Gnvs;
  BIOS_ACPI_PARAM           *AcpiParameter;
  PLATFORM_DATA             *PlatformData;

  PlatformData = GetPlatformDataPtr ();

  Gnvs = (GLOBAL_NVS_AREA *)(UINTN)PcdGet32 (PcdAcpiGnvsAddress);
  AcpiParameter = &Gnvs->BiosAcpiParam;
  if (AcpiParameter == NULL) {
    DEBUG ((DEBUG_ERROR, "Unable to get BIOS_ACPI_PARAM.\n"));
    return;
  }

  SgxSblDataInfoHOB = BuildGuidHob (&gSgxSblDataInfoHobGuid, sizeof(SGX_SBL_DATA_INFO_HOB));
  if (SgxSblDataInfoHOB == NULL) {
    DEBUG ((DEBUG_ERROR, "Unable to create a ACPI HOB.\n"));
  }

  ZeroMem (SgxSblDataInfoHOB, sizeof(SGX_SBL_DATA_INFO_HOB));

  SgxSblDataInfoHOB->EpcAddress.EpcBaseAddress = (UINT64) (UINTN) AcpiParameter->EpcBaseAddress;
  SgxSblDataInfoHOB->EpcAddress.EpcLengthAddress = (UINT64) (UINTN) AcpiParameter->EpcLength;
  SgxSblDataInfoHOB->EpcAddress.NumOfEpc = (UINT32) sizeof(AcpiParameter->EpcBaseAddress)/sizeof(UINT64);
  SgxSblDataInfoHOB->SgxStatusSize = (UINT8)sizeof(AcpiParameter->SgxStatus);
  SgxSblDataInfoHOB->SgxStatusAddr = (UINT64) (UINTN) &AcpiParameter->SgxStatus;
  DEBUG ((DEBUG_INFO, "SgxSblDataInfoHOB NumberOfEpc%d\n",SgxSblDataInfoHOB->EpcAddress.NumOfEpc));

  //Also Adding the Sgx State variable.
  if (PlatformData != NULL) {
    SgxSblDataInfoHOB->SgxSblCfgState = (UINT8)PlatformData->PlatformFeatures.SgxSblState;
  }

  DEBUG ((DEBUG_INFO, "%a(): Completed\n", __FUNCTION__));
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
  PLATFORM_DATA   *PlatformData;

  PlatformData = GetPlatformDataPtr ();
  if ((PlatformData != NULL) && (PlatformData->PlatformFeatures.SgxSblExec == 1)) {
    if (mSgxRegistrationConfig != NULL) {
      FreePages (mSgxRegistrationConfig,
        EFI_SIZE_TO_PAGES (msizeofSgxRegistrationConfig));
    }
    if (mSgxRegistrationPackageInfo != NULL) {
      FreePages (mSgxRegistrationPackageInfo,
        EFI_SIZE_TO_PAGES (msizeofSgxRegistrationPackageInfo));
    }
    if (mSgxRegistrationStatus != NULL) {
      FreePages (mSgxRegistrationStatus,
        EFI_SIZE_TO_PAGES (msizeofSgxRegistrationStatus));
    }
    if (mSgxRegistrationResponse != NULL) {
      FreePages (mSgxRegistrationResponse,
        EFI_SIZE_TO_PAGES (msizeofSgxRegistrationResponse));
    }
    if (mUefiFwKeyBlobs != NULL) {
      FreePages (mUefiFwKeyBlobs,
        EFI_SIZE_TO_PAGES (msizeofSgxUefifwKeyBlob));
    }
    if (mSgxRegistrationState != NULL) {
        FreePages (mSgxRegistrationState,
        EFI_SIZE_TO_PAGES (msizeofSgxUefifwNonVolatilveRegistrationState));
    }
    if (mSecurityIpInterdependencePreviousBoot != NULL) {
        FreePages (mSecurityIpInterdependencePreviousBoot,
        EFI_SIZE_TO_PAGES (sizeof (UINT8)*SHA384_DIGEST_SIZE));
    }
    if (mSgxRegistrationServerRequest != NULL) {
        FreePages (mSgxRegistrationServerRequest,
        EFI_SIZE_TO_PAGES (msizeofSgxRegistrationRequest));
    }

    mSgxRegistrationConfig  = NULL;
    mSgxRegistrationPackageInfo = NULL;
    mSgxRegistrationStatus = NULL;
    mSgxRegistrationResponse = NULL;
    mUefiFwKeyBlobs = NULL;
    mSgxRegistrationState = NULL;
    mSgxRegistrationServerRequest = NULL;
    mSecurityIpInterdependencePreviousBoot = NULL;
  }

  DEBUG ((DEBUG_INFO, "%a(): Completed \n", __FUNCTION__));
}

