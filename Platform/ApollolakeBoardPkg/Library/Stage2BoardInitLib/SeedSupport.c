/** @file

  Copyright (c) 2018 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"

CONST STATIC UINT8 UsrInfo[]     = "oemseed";
CONST STATIC UINT8 DevInfo[]     = "deviceseed";
CONST STATIC UINT8 UserNonce[]   = "usernonce";
CONST STATIC UINT8 DeviceNonce[] = "devicenonce";
BOOLEAN  RpmbSerialNumberValid;

/**
  Debug function to dump the SEED list

  @param[in]  DumpSeedList  Pointer to the seed list to dump.

**/
VOID
DumpSeedValues (
  IN MKHI_BOOTLOADER_SEED_LIST *DumpSeedList
  )
{
  UINT8 Idx;
  UINT8 Count;

  if (DumpSeedList == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO, "Dump Seed values retrieved from HECI\n"));
  DEBUG ((DEBUG_INFO, "NumofSeeds = %u\n", DumpSeedList->NumOfSeeds));
  for (Idx = 0; Idx < DumpSeedList->NumOfSeeds; Idx++) {
    DEBUG ((DEBUG_INFO, "SeedList [%u], CseSvn = %u\n", Idx, DumpSeedList->List[Idx].CseSvn));
    DEBUG ((DEBUG_INFO, "SeedList[%x].seed: \n", Idx));
    for (Count = 0; Count < 32; Count++) {
      DEBUG ((DEBUG_INFO, "%02X", DumpSeedList->List[Idx].Seed[Count]));
    }
    DEBUG ((DEBUG_INFO, "\n"));
  }
}

/**
  Get the new Seed List info from HECI. Since this feature is not
  needed anymore for APL, we just return error.

  @param[out]  Returns SeedList Pointer retrieved from HECI.

  @retval EFI_UNSUPPORTED   Number of Seeds retrieved is zero.

**/
EFI_STATUS
GetSeedfromCSE (
  OUT MKHI_BOOTLOADER_SEED_LIST               *SeedList
  )
{
  // Enable this only for testing purposes.
  DumpSeedValues (NULL);

  return EFI_UNSUPPORTED;
}

/**
  Derive User and Device seeds using the HECI seedlist info.

  @param[in]   ListofSeeds  Pointer to the seed list.
  @param[out]  UseedList    Pointer to user seed list.
  @param[out]  DseedList    Pointer to device seed list.

  @retval     EFI_SUCCESS   Seed Derivation using IPP APIs is successful.
  @retval     EFI_ERROR     Failed to do seed derivation using IPP.

**/
EFI_STATUS
SeedDerivation (
  IN  MKHI_BOOTLOADER_SEED_LIST              *ListofSeeds,
  OUT MKHI_BOOTLOADER_SEED_INFO_EX           *UseedList,
  OUT MKHI_BOOTLOADER_SEED_INFO_EX           *DseedList
  )
{

  CONST UINT8                 *DeviceSalt;
  CONST UINT8                 *UserSalt;
  UINT8                        Idx;
  UINT8                        Index;
  EFI_STATUS                   Status;

  Status = EFI_SUCCESS;

  // Inputs
  // Nonces. Assign hard-coded values.
  UserSalt = UserNonce;
  DeviceSalt = DeviceNonce;

  // Derive User seeds list
  for (Idx = 0; Idx < ListofSeeds->NumOfSeeds; Idx++) {
    // Derive the key using Seed and UserSalt as inputs
    Status = HkdfExtractExpand (
               UserSalt, 64,
               ListofSeeds->List[Idx].Seed, MKHI_BOOTLOADER_SEED_LEN,
               UsrInfo, sizeof (UsrInfo) - 1,
               UseedList[Idx].Seed, MKHI_BOOTLOADER_SEED_LEN
               );

    // if HKDF returns without any errors
    if (!EFI_ERROR (Status)) {
      UseedList[Idx].CseSvn =  ListofSeeds->List[Idx].CseSvn;
    } else {
      DEBUG ((DEBUG_ERROR, "Error returned in HKDF for UserSeeds: %r\n", Status));
      UseedList[Idx].CseSvn = 0xFF;
      for (Index = 0; Index < sizeof (UseedList[Idx].Seed); Index++) {
        UseedList[Idx].Seed[Index] = 0xA5;
      }
    }
    // Derive the key using Seed and DeviceSalt as inputs
    Status = HkdfExtractExpand (
               DeviceSalt, 64,
               ListofSeeds->List[Idx].Seed, MKHI_BOOTLOADER_SEED_LEN,
               DevInfo, sizeof (DevInfo) - 1,
               DseedList[Idx].Seed, 32
               );

    // if HKDF returns without any errors
    if (!EFI_ERROR (Status)) {
      DseedList[Idx].CseSvn =  ListofSeeds->List[Idx].CseSvn;
    } else {
      DEBUG ((DEBUG_INFO, "Error returned in HKDF for DeviceSeeds: %r\n", Status));
      DseedList[Idx].CseSvn = 0xFF;
      for (Index = 0; Index < sizeof (DseedList[Idx].Seed); Index++) {
        DseedList[Idx].Seed[Index] = 0xA5;
      }
    }
  }
  return Status;
}

/**
  Seed Retrieval and Derivation. It does the following.
    Retrieve SVN SeedList from HECI.This contains SVN based RPMB seed.
    Retrieve User/Device Nonces from Cfg Data.
    Derive User/Device seeds using IPP HKDF.
    Retrive Fixed RPMB seed from HECI.
    Pass Derived seedlist and Fixed RPMB seed to OS Loader.

    @retval     EFI_SUCCESS        Successfully generated seeds to seedstore.
    @retval     EFI_OTHERS         Error in HECI API, or in deriving seeds.

**/
EFI_STATUS
SeedRetrievalAndDerivation (
  VOID
)
{
  PLATFORM_DATA                 *PlatformData;
  LOADER_SEED_LIST              *LdrSeedList;
  MKHI_BOOTLOADER_SEED_LIST     *SeedList;
  MKHI_BOOTLOADER_SEED_INFO_EX  *UseedList;
  MKHI_BOOTLOADER_SEED_INFO_EX  *DseedList;
  EFI_STATUS                    Status;
  UINT8                         Idx;
  UINT32                        Length;

  PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();
  ASSERT (PlatformData);

  LdrSeedList = (LOADER_SEED_LIST *)PlatformData->LoaderSeedList;
  if (LdrSeedList == NULL) {
    LdrSeedList = (LOADER_SEED_LIST *)AllocateZeroPool (sizeof (LOADER_SEED_LIST));
    PlatformData->LoaderSeedList = (VOID *)LdrSeedList;
  }

  // Step 1: Seed Retrieval from Heci
  SeedList  = AllocatePool (sizeof (MKHI_BOOTLOADER_SEED_LIST));
  ZeroMem (SeedList, sizeof (MKHI_BOOTLOADER_SEED_LIST));
  Status = GetSeedfromCSE (SeedList);
  if (!EFI_ERROR (Status)) {
    // If CseSVN = 1, it is RPMB seed. Populate RPMB seed
    for(Idx=0; Idx < SeedList->NumOfSeeds; Idx++) {
      if(SeedList->List[Idx].CseSvn == 1) {
        //Check serial number validity.
        if(!RpmbSerialNumberValid) {
          // If false, Rpmb key is not tied to actual card serial num due to issues in getting variable.
          // Indicate it in CseSvn saying that RPMBseed is not tied to serial num.
          SeedList->List[Idx].CseSvn = 0xFF;
        }
        // Populate orig pSeed[0] as-is. No need of derivation for RpmbSeed because this is already derived coming from HECI
        CopyMem (&LdrSeedList->RpmbHeciSeeds[0], SeedList->List[Idx].Seed, MKHI_BOOTLOADER_SEED_LEN);
      }
    }

    // Step 2: User/Device seed derivation
    Length = sizeof (MKHI_BOOTLOADER_SEED_INFO_EX) * SeedList->NumOfSeeds;
    UseedList = AllocatePool (Length);
    ZeroMem (UseedList, Length);

    DseedList = AllocatePool (Length);
    ZeroMem (DseedList, Length);

    Status = SeedDerivation (SeedList, UseedList, DseedList); //derive
    if (!EFI_ERROR (Status)) {
      // Step 3: Pass the HECI orig pSeed[0] and Derived Seedlist to OSLoader through PlatformInfo HOB.
      // Pass NumofSeeds
      LdrSeedList->NumofSeeds = SeedList->NumOfSeeds;

      for (Idx = 0; Idx < SeedList->NumOfSeeds; Idx++) {
        // Populate UserSeedList
        LdrSeedList->UseedList[Idx].CseSvn = UseedList[Idx].CseSvn;
        CopyMem (&LdrSeedList->UseedList[Idx].Seed, UseedList[Idx].Seed, sizeof (UseedList[Idx].Seed));
      }
      for (Idx = 0; Idx < SeedList->NumOfSeeds; Idx++) {
        // Populate DeviceSeedList
        LdrSeedList->DseedList[Idx].CseSvn = DseedList[Idx].CseSvn;
        CopyMem (&LdrSeedList->DseedList[Idx].Seed, DseedList[Idx].Seed, sizeof (DseedList[Idx].Seed));
      }
    } else {
      //UserSeeds and DeviceSeeds cannot be derived. Only RPMB Seed available.
      DEBUG ((DEBUG_ERROR, "USeeds/DSeeds cannot be derived from IPP\n"));
    }
  } else {
    DEBUG ((DEBUG_ERROR, "SeedList retrieval unsupported\n"));
    LdrSeedList->NumofSeeds = 0;
  }

  // Clear the original HECI list
  ZeroMem (SeedList, sizeof (MKHI_BOOTLOADER_SEED_LIST));

  return Status;
}

/**
  This is to generate seeds.

  @retval     EFI_SUCCESS          Successfully generated seeds to seedstore.
  @retval     EFI_PROTOCOL_ERROR   Successfully generated seeds to seedstore.
  @retval     EFI_OTHERS           Error in getting the serial number from SPI.
**/
EFI_STATUS
GenerateSeeds (
  VOID
  )
{
  EFI_STATUS              Status;
  EMMC_TUNING_DATA        EmmcTuningData;
  UINTN                   VariableLen;
  CHAR8                   SerialNum[MKHI_BOOTLOADER_PCR_LEN];

  ZeroMem(SerialNum, sizeof(SerialNum));

  // Get EmmcTuningData.SerialNumber from SPI using variable service
  VariableLen = sizeof (EmmcTuningData);
  Status = GetVariable (L"MMCDLL", NULL, NULL, &VariableLen, (void *)&EmmcTuningData);
  if ((Status != EFI_SUCCESS) || (AsciiStrnCmp (EmmcTuningData.SerialNumber, "badbadbadbadba", sizeof (EmmcTuningData.SerialNumber)) == 0)) {
    RpmbSerialNumberValid = FALSE;
  } else {
    AsciiStrCpyS (SerialNum, sizeof(SerialNum), EmmcTuningData.SerialNumber);
    RpmbSerialNumberValid = TRUE;
  }

  // SeedList retrieval depends on Serial Number. So pass it to PCR Extend first.
  Status = PcrExtend ((UINT8 *)SerialNum);
  if (Status == EFI_SUCCESS) {
    // Retrieve and derive seeds if able to extend serial number.
    SeedRetrievalAndDerivation();
  } else {
    return EFI_PROTOCOL_ERROR;
  }

  return Status;
}

/**
  Returns if Seed Type that is passed in is present or not.

  This is needed to ensure that only the correct seeds are appended to Stage2 HOB buffer
  based on the Image type( OS, Trusty flags) for current target boot index[0].

  @param[out] SeedType      Type of seed passed in.

  1.native android (AOS loader) without Trusty OS (evmm)  = only rpmb needed
  2.native android (AOS loader) with Trusty OS            = dseed + rpmb
  3.Clear linux without Trusty                            = useed + dseed
  4.Clear Linux with Trusty (no AOS loader)               = all (useed/dseed/rpmb keys)
  5.ACRN                                                  = all

  @retval  BOOLEAN           Seed Type present or not.
**/

BOOLEAN
SeedStatusBasedOnImageType (
  IN  SEED_TYPE   SeedType
  )
{
  OS_BOOT_OPTION_LIST   *OsBootOptionList;
  UINT8                 OsImageType;
  BOOLEAN               TrustyFlag;

  OsBootOptionList = GetBootOptionList ();
  if (OsBootOptionList == NULL || OsBootOptionList->OsBootOptionCount == 0) {
    return FALSE;
  }
  // First boot option should be the target OS.
  TrustyFlag  = (OsBootOptionList->OsBootOption[0].BootFlags & BOOT_FLAGS_PREOS) >> 2;
  OsImageType = OsBootOptionList->OsBootOption[0].ImageType;

  switch (SeedType) {
    case UserSeed:
      if((OsImageType == EnumImageTypeClearLinux) || (OsImageType == EnumImageTypeAcrn)) {
        return TRUE;
      }
      break;

    case DeviceSeed:
      if((OsImageType == EnumImageTypeClearLinux) || (OsImageType == EnumImageTypeAcrn) ||
        ((OsImageType == EnumImageTypeAdroid) && (TrustyFlag != 0))) {
        return TRUE;
      }
      break;

    case RpmbKey:
      if ((OsImageType == EnumImageTypeAdroid) || (OsImageType == EnumImageTypeAcrn) ||
        ((OsImageType == EnumImageTypeClearLinux) && (TrustyFlag != 0))) {
        return TRUE;
      }
      break;

    default:
      break;
  }
  return FALSE;
}

/**
  Append each seed into the Seed List HOB based on OS Image type

  @param[out] SeedListInfoHob      Pointer to SeedList HOB data structure.

  @retval     EFI_SUCCESS          Successfully appended seeds to seedlist HOB.
  @retval     EFI_UNSUPPORTED      Seed feature is not enabled.
  @retval     EFI_NOT_FOUND        Unable to find the necessary seed data.
  @retval     EFI_OTHERS           Error caused in appending seeds.
 **/
EFI_STATUS
UpdateSeedListInfo (
  OUT SEED_LIST_INFO_HOB  *SeedListInfoHob
)
{
  PLATFORM_DATA       *PlatformData;
  LOADER_SEED_LIST    *LdrSeedList;
  UINT8                Index;
  UINT8                RpmbSerialNumUsage;
  EFI_STATUS           Status;
  BOOLEAN              IsSeedPresent;

  if (PcdGetBool (PcdSeedListEnabled) == FALSE) {
    return EFI_UNSUPPORTED;
  }

  PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData == NULL) {
    return EFI_NOT_FOUND;
  }

  LdrSeedList = (LOADER_SEED_LIST *)PlatformData->LoaderSeedList;
  if (LdrSeedList == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;
  if (LdrSeedList->NumofSeeds > 0) {
    for (Index = 0; Index < LdrSeedList->NumofSeeds; Index++) {
      // Fill SeedListHOB with one USeed at a time
      // if CseSvn = 1, it is only for Rpmb Seed for APL. No need to derive for Useed
      // if CseSvn = 0xFF, its invalid entry. No need to Append.
      if ((LdrSeedList->UseedList[Index].CseSvn != 0x1) && (LdrSeedList->UseedList[Index].CseSvn != 0xFF)) {
        IsSeedPresent = SeedStatusBasedOnImageType(UserSeed);
        if( IsSeedPresent == TRUE) {
          // Append user seeds to Seed List Hob buffer based on OS Image type
          Status = AppendSeedData (SEED_ENTRY_TYPE_SVNSEED,SEED_ENTRY_USAGE_USEED, Index, sizeof (SVN_SEED_INFO), (UINT8 *)&LdrSeedList->UseedList[Index]);
          if (EFI_ERROR (Status)) {
            return Status;
          }
        }
      }
    }
    for (Index = 0; Index < LdrSeedList->NumofSeeds; Index++) {
      // Fill SeedListHOB with one DSeed at a time
      // if CseSvn = 1, it is only for Rpmb Seed for APL. No need to derive for Dseed
      // if CseSvn = 0xFF, its invalid entry. No need to Append.
      if ((LdrSeedList->DseedList[Index].CseSvn != 0x1) && (LdrSeedList->DseedList[Index].CseSvn != 0xFF)) {
        IsSeedPresent = SeedStatusBasedOnImageType(DeviceSeed);
        if( IsSeedPresent == TRUE) {
          // Append device seeds to Seed List Hob buffer based on OS Image type
          Status = AppendSeedData (SEED_ENTRY_TYPE_SVNSEED,SEED_ENTRY_USAGE_DSEED, Index, sizeof (SVN_SEED_INFO), (UINT8 *)&LdrSeedList->DseedList[Index]);
          if (EFI_ERROR (Status)) {
            return Status;
          }
        }
      }
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Useed/Dseed is not available!\n"));
  }
  // Fill RPMB Seed
  if (!RpmbSerialNumberValid) { // i.e; Rpmb is not tied to actual card serial number.
    RpmbSerialNumUsage = SEED_ENTRY_USAGE_NOT_BASED_ON_SERIALNO;
  } else {
    RpmbSerialNumUsage = SEED_ENTRY_USAGE_BASED_ON_SERIALNO;
  }
  IsSeedPresent = SeedStatusBasedOnImageType(RpmbKey);
  if( IsSeedPresent == TRUE) {
    // Append rpmb key to Seed List Hob buffer based on OS Image type
    Status = AppendSeedData (SEED_ENTRY_TYPE_RPMB, RpmbSerialNumUsage, SEED_ENTRY_IDX_RPMB_PART1, MKHI_BOOTLOADER_SEED_LEN, (UINT8 *)&LdrSeedList->RpmbHeciSeeds[0]);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  // clean-up temp pointer
  PlatformData->LoaderSeedList = NULL;

  return Status;
}




