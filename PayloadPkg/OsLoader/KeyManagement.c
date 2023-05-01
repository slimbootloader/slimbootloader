/** @file

  Copyright (c) 2018 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"

extern CONST CHAR16   *mMmcDllStr;
UINT8                 mRpmbKeyCount = 0;

/**
  Check the eMMC storage serial number validity.

  If card serial number != SPI flash serial number, then
  save the new serial number from the card to SPI flash and
  warm reset the platform.

  @retval  EFI_SUCCESS       Successfully checked and saved the serial number.
  @retval  EFI_NOT_FOUND     Couldnt retrieve serial number either from card or flash.
  @retval  EFI_DEVICE_ERROR  Couldnt save the new serial number to SPI flash.
**/
EFI_STATUS
EmmcSerialNumCheck (
  VOID
  )
{
  EFI_STATUS                 Status;
  EMMC_TUNING_DATA           EmmcTuningData;
  UINTN                      VariableLen;
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;
  PLATFORM_SERVICE          *PlatformService;

  PlatformService = NULL;

  // Get serial number from the eMMC card
  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GetLoaderPlatformInfoPtr();
  if (LoaderPlatformInfo == NULL) {
    return EFI_NOT_FOUND;
  }

  // Get serial number from SPI flash
  VariableLen = sizeof (EmmcTuningData);
  Status = GetVariable ((CHAR16 *)mMmcDllStr, NULL, NULL, &VariableLen, (void *)&EmmcTuningData);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  // Compare serial number from the card and SPI flash
  if (AsciiStriCmp (LoaderPlatformInfo->SerialNumber, EmmcTuningData.SerialNumber) != 0) {
    AsciiStrCpyS (EmmcTuningData.SerialNumber, sizeof(EmmcTuningData.SerialNumber), LoaderPlatformInfo->SerialNumber);

    // Save new serial number into SPI flash
    Status = SetVariable ((CHAR16 *)mMmcDllStr, NULL, 0, sizeof (EMMC_TUNING_DATA), (VOID *)&EmmcTuningData);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "MMC serial number save to flash unsuccessful, Status = %r\n", Status));
      return EFI_DEVICE_ERROR;
    }

    // Reset platform
    PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
    if ((PlatformService != NULL) && (PlatformService->ResetSystem != NULL)) {
      DEBUG ((DEBUG_ERROR, "Initializing Platform Warm Reset due to inconsistent serial number\n"));
      PlatformService->ResetSystem (EfiResetWarm);
    }
  }

  return Status;
}

/**
  Seed Sanity check to check seed HOB validity before passing to OS.

  This is needed to ensure that only the correct seeds are passed from Stage2 HOB buffer
  to OS based on the Image type( OS, Trusty flags) for current target boot index[0].

  1.native android (AOS loader) without Trusty OS (evmm) = only rpmb needed
  2.native android (AOS loader) with Trusty OS           = dseed + rpmb
  3.Clear linux without Trusty                           = useed + dseed
  4.Clear Linux with Trusty (no AOS loader)              = all (useed/dseed/rpmb keys)
  5.ACRN                                                 = all

  @param[in]     CurrentBootOption        Current boot option
  @param[in,out]     SeedList             Pointer to the Seed list

  @retval        EFI_SUCCESS              Successfully checked the seed validity.
  @retval        EFI_NOT_FOUND            Couldnt retrieve seeds or couldnt find bootoptionslist.

**/
EFI_STATUS
SeedSanityCheck (
  IN     OS_BOOT_OPTION      *CurrentBootOption,
  IN OUT LOADER_SEED_LIST    *SeedList
  )
{
  OS_BOOT_OPTION_LIST   *OsBootOptionList;
  SEED_LIST_INFO_HOB    *SeedListInfo;
  UINT8                 OsImageType;
  UINT8                 CurrentOsImageType;
  BOOLEAN               TrustyFlag;
  BOOLEAN               CurrentTrustyFlag;
  BOOLEAN               ClearSeedHOB;
  UINT8                 Index;
  UINT8                 UseedCount;
  UINT8                 DseedCount;
  UINT32                SeedListLen;
  EFI_STATUS            Status;

  Status = EFI_SUCCESS;

  ClearSeedHOB = FALSE;
  UseedCount = 0;
  DseedCount = 0;
  SeedListLen = 0;

  OsBootOptionList = GetBootOptionList ();
  if ((OsBootOptionList == NULL) || (OsBootOptionList->OsBootOptionCount == 0)) {
    return EFI_NOT_FOUND;
  }

  SeedListInfo = GetSeedListInfoHOB (&SeedListLen);
  if ((SeedListInfo == NULL) || (SeedListLen == 0)) {
    return EFI_NOT_FOUND;
  }

  // Get OStype/ Trusty Flag from Osbootoptionlist for index: 0
  // as its assumed that target OS should always be at index:0
  OsImageType = OsBootOptionList->OsBootOption[0].ImageType;
  TrustyFlag  = (OsBootOptionList->OsBootOption[0].BootFlags & BOOT_FLAGS_PREOS) >> 2;

  // Get Os type/Trusty flag from current boot option
  CurrentOsImageType = CurrentBootOption->ImageType;
  CurrentTrustyFlag = (CurrentBootOption->BootFlags & BOOT_FLAGS_PREOS) >> 2;

  // Compare OS type/Trusty flags of current bootoption with bootoption index 0
  if((OsImageType == CurrentOsImageType) && (TrustyFlag == CurrentTrustyFlag)) {
    // Retrieve Seeds from HOB
    for(Index = 0; Index < SeedListInfo->TotalSeedCount; Index++) {

      Status = GetSeedData (SEED_ENTRY_TYPE_SVNSEED, SEED_ENTRY_USAGE_USEED, UseedCount, sizeof(SVN_SEED_INFO), (UINT8 *)&SeedList->UseedList[UseedCount]);
      if (!EFI_ERROR (Status)) {
        UseedCount++;
      }
      Status = GetSeedData (SEED_ENTRY_TYPE_SVNSEED, SEED_ENTRY_USAGE_DSEED, DseedCount, sizeof(SVN_SEED_INFO), (UINT8 *)&SeedList->DseedList[DseedCount]);
      if (!EFI_ERROR (Status)) {
        DseedCount++;
      }
      // First get RPMB seed based on card Serial Number.
      Status = GetSeedData (SEED_ENTRY_TYPE_RPMB, SEED_ENTRY_USAGE_BASED_ON_SERIALNO, mRpmbKeyCount, MKHI_BOOTLOADER_SEED_LEN, (UINT8 *)&SeedList->RpmbHeciSeeds[mRpmbKeyCount]);
      if (EFI_ERROR (Status)) {
        // If not found, then SBL has tied it with Zero based Serial Number.
        Status = GetSeedData (SEED_ENTRY_TYPE_RPMB, SEED_ENTRY_USAGE_NOT_BASED_ON_SERIALNO, mRpmbKeyCount, MKHI_BOOTLOADER_SEED_LEN, (UINT8 *)&SeedList->RpmbHeciSeeds[mRpmbKeyCount]);
      }
      if (!EFI_ERROR (Status)) {
        mRpmbKeyCount++;
      }
    }

    // Check Image Type and only then only pass seeds to OS. If not, zero out the HOB
    if(!((OsImageType == EnumImageTypeClearLinux) || (OsImageType == EnumImageTypeAcrn))) {
      if(UseedCount > 0) {
        // No other Image should have useeds. Zero out HOB buffer
        ClearSeedHOB = TRUE;
      }
    }
    if(!((OsImageType == EnumImageTypeClearLinux) || (OsImageType == EnumImageTypeAcrn) ||
    ((OsImageType == EnumImageTypeAdroid) && (TrustyFlag != 0)))) {
      if(DseedCount > 0) {
        // No other Image should have dseeds. Zero out HOB buffer
        ClearSeedHOB = TRUE;
      }
    }
    if (!((OsImageType == EnumImageTypeAdroid) || (OsImageType == EnumImageTypeAcrn) ||
    ((OsImageType == EnumImageTypeClearLinux) && (TrustyFlag != 0)))) {
      if(mRpmbKeyCount > 0) {
        // No other Image should have rpmb keys. Zero out HOB buffer
        ClearSeedHOB = TRUE;
      }
    }
  } else {
    // Current boot option Image type is not same as Bootoption[0] ImageType.
    // Donot send incorrect seeds to OS. Zero out the HOB
    ClearSeedHOB = TRUE;
  }
  if (ClearSeedHOB) {
    DEBUG ((DEBUG_ERROR, "Clear SeedList buffer due to OSImageType error\n"));
    ZeroMem(SeedListInfo,SeedListLen);
  }

  return EFI_SUCCESS;
}

/**
  Provision the RPMB key into RPMB partition.

  Currently, there is only eMMC RPMB support. So this flow works
  only if DeviceType = eMMC.

  @param[in]     CurrentBootOption        Current boot option
  @param[in]     SeedList                 Pointer to the Seed list

  @retval        EFI_SUCCESS              Successfully provisioned the RPMB key.
  @retval        EFI_NOT_FOUND            Couldn't find PlatformService->CfgData or PlatformInfo
                                          or SeedList not found.
  @retval        EFI_UNSUPPORTED          RpmbProvisioning CfgData flag not enabled or SeedList
                                          feature not enabled.
  @retval        EFI_OUT_OF_RESOURCES     Couldn't allocate memory.
  @retval        Others                   Error occurred inside RPMB library.

**/
EFI_STATUS
RpmbKeyProvisioning (
  IN     OS_BOOT_OPTION      *CurrentBootOption,
  IN     LOADER_SEED_LIST    *SeedList
  )
{
  LOADER_PLATFORM_INFO       *LoaderPlatformInfo;
  GEN_CFG_DATA               *GenCfgData;
  UINT8                      Index;
  BOOLEAN                    Eom;
  RPMB_RESPONSE_RESULT       Result;
  EFI_STATUS                 Status;
  UINT8                      *RpmbSeedInfo;

  Result       = 0;

  if((SeedList == NULL) || (mRpmbKeyCount == 0)){
    return EFI_NOT_FOUND;
  }

  // Check for the storage device medium: emmc/ufs/nvme.
  // Currently only eMMC rpmb provisioning is supported
  if (CurrentBootOption->DevType != OsBootDeviceEmmc) {
    return EFI_UNSUPPORTED;
  }

  // Get Rpmb Key provisioning flag from Cfg Data
  GenCfgData = (GEN_CFG_DATA *) FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenCfgData == NULL) {
    return EFI_NOT_FOUND;
  }

  if((!GenCfgData->RpmbKeyProvisioning) || (!PcdGetBool (PcdSeedListEnabled))) {
    return EFI_UNSUPPORTED;
  }

  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GetLoaderPlatformInfoPtr();
  if (LoaderPlatformInfo == NULL) {
    return EFI_NOT_FOUND;
  }

  // Get manufacturing state
  Eom = ((LoaderPlatformInfo->HwState & HWSTATE_IN_MANU_SECURE_DEBUG_MODE) == 0) ? TRUE : FALSE;

  // Proceed further only if its in production mode and if the verified boot is enabled
  if((Eom) || ((LoaderPlatformInfo->LdrFeatures & FEATURE_VERIFIED_BOOT) == FEATURE_VERIFIED_BOOT)) {
    return EFI_UNSUPPORTED;
  }

  // Copy 32 bytes for APL
  Status = EFI_UNSUPPORTED;
  for(Index = 0;Index < mRpmbKeyCount; Index++) {
    RpmbSeedInfo = (UINT8 *)&SeedList->RpmbHeciSeeds[Index];
    Status = RpmbProgramKey(CurrentBootOption->DevType, 0, RpmbSeedInfo, 32, &Result);
  }

  return Status;
}

