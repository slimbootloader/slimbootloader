/** @file
  Intel(R) SGX library.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <RegAccess.h>
#include <FspmUpd.h>
#include <FspsUpd.h>
#include <Library/RngLib.h>
#include <ConfigDataDefs.h>
#include <Library/ConfigDataLib.h>
#include <Library/VariableLib.h>
#include <Library/BaseLib.h>
#include <CpuNvsAreaDef.h>

#define CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS 0x7
#define SGX_EPC_CPUID                           0x12

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
  Check on the processor if SGX is supported.

  @retval True if SGX supported or FALSE if not
**/
BOOLEAN
IsSgxCapSupported (
  VOID
)
{
  EFI_CPUID_REGISTER CpuidRegs;

  ///
  /// Processor support SGX feature by reading CPUID.(EAX=7,ECX=0):EBX[2]
  ///
  AsmCpuidEx (CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS, 0, &CpuidRegs.RegEax,&CpuidRegs.RegEbx,&CpuidRegs.RegEcx,&CpuidRegs.RegEdx);

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
  Check if input PRMRR value is supported by the processor

  @param[in]  PrmrrSizeFromConfig  PrmrrSize value entered in SGX config

  @retval     TRUE if PrmrrSizeFromConfig is supported, FALSE otherwise

**/
BOOLEAN
IsPrmrrSizeSupported (
  IN UINT32 PrmrrSizeFromConfig
)
{
  ///
  /// Determine supported PRMRR sizes by reading
  /// MSR_PRMRR_VALID_CONFIG [8:5]
  ///
  UINT64 SupportedPrmrrSizes = AsmReadMsr64 (MSR_PRMRR_VALID_CONFIG) & 0x1E0;
  BOOLEAN SizeIsSupported = FALSE;
  switch (PrmrrSizeFromConfig) {
  case 0x2000000: // 32 MB
    if ((SupportedPrmrrSizes == PRMRR_32_64_128_256) ||
        (SupportedPrmrrSizes == PRMRR_32_64_128)     ||
        (SupportedPrmrrSizes == PRMRR_32_64)         ||
        (SupportedPrmrrSizes == PRMRR_32))            {
          SizeIsSupported = TRUE;
    }
    break;
  case 0x4000000: // 64 MB
    if ((SupportedPrmrrSizes == PRMRR_32_64_128_256) ||
        (SupportedPrmrrSizes == PRMRR_32_64_128)     ||
        (SupportedPrmrrSizes == PRMRR_32_64))         {
          SizeIsSupported = TRUE;
    }
    break;
  case 0x8000000: // 128 MB
    if ((SupportedPrmrrSizes == PRMRR_32_64_128_256) ||
        (SupportedPrmrrSizes == PRMRR_32_64_128))     {
          SizeIsSupported = TRUE;
    }
    break;
  case 0x10000000: // 256 MB
    if (SupportedPrmrrSizes == PRMRR_32_64_128_256)   {
          SizeIsSupported = TRUE;
    }
    break;
  default:
    DEBUG ((DEBUG_ERROR, "Unknown PRMRR size.\n"));
    break;
  }
  return SizeIsSupported;
}

/**
  Update FSP-M UPD SGX config data

  @param[in]  FspmUpd           The pointer to the FSP-M UPD to be updated.

**/
BOOLEAN
UpdateFspmSgxConfig (
  IN FSPM_UPD *FspmUpd
)
{
  BOOLEAN UpdateFspmSgxConfigStatus = FALSE;
  SGX_CFG_DATA *SgxCfgData = NULL;

  if (FspmUpd == NULL) {
    return UpdateFspmSgxConfigStatus;
  }
  if (IsSgxCapSupported() == FALSE) {
    return UpdateFspmSgxConfigStatus;
  }

  SgxCfgData = (SGX_CFG_DATA *) FindConfigDataByTag (CDATA_SGX_TAG);
  if (SgxCfgData != NULL) {
    if (SgxCfgData->EnableSgx == CONFIG_SGX_ENABLED) {
      if (IsPrmrrSizeSupported(SgxCfgData->PrmrrSize) == TRUE) {
        FspmUpd->FspmConfig.EnableSgx = SgxCfgData->EnableSgx;
        FspmUpd->FspmConfig.PrmrrSize = SgxCfgData->PrmrrSize;
        UpdateFspmSgxConfigStatus = TRUE;
      } else {
        DEBUG ((DEBUG_ERROR, "Invalid PrmrrSize value set in SGX config.\n"));
      }
    } else if (SgxCfgData->EnableSgx == CONFIG_SGX_DISABLED) {
      DEBUG ((DEBUG_WARN, "Intel(R) SGX set to disabled in config.\n"));
    } else {
      DEBUG ((DEBUG_ERROR, "Invalid EnableSgx value set in SGX config.\n"));
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to find Intel(R) SGX CFG!\n"));
  }
  return UpdateFspmSgxConfigStatus;
}

/**
  Update FSP-S UPD SGX config data

  @param[in]  FspsUpd           The pointer to the FSP-S UPD to be updated.

**/
BOOLEAN
UpdateFspsSgxConfig (
  IN FSPS_UPD *FspsUpd
)
{
  BOOLEAN UpdateFspsSgxConfigStatus = FALSE;
  SGX_CFG_DATA *SgxCfgData = NULL;

  if (FspsUpd == NULL) {
    DEBUG ((DEBUG_WARN, "NULL pointer passed.\n"));
    return UpdateFspsSgxConfigStatus;
  }
  if (IsSgxCapSupported() == FALSE) {
    DEBUG ((DEBUG_WARN, "Intel(R) SGX is not supported by this CPU.\n"));
    return UpdateFspsSgxConfigStatus;
  }

  SgxCfgData = (SGX_CFG_DATA *) FindConfigDataByTag (CDATA_SGX_TAG);
  if (SgxCfgData != NULL) {
    if (SgxCfgData->EnableSgx != CONFIG_SGX_ENABLED) {
      DEBUG ((DEBUG_WARN, "Intel(R) SGX set to disabled in config.\n"));
    } else if (IsPrmrrSizeSupported(SgxCfgData->PrmrrSize) == FALSE) {
      DEBUG ((DEBUG_WARN, "Invalid PRMRR size set in config.\n"));
    } else {
      if (SgxCfgData->EpochUpdate == CONFIG_SGX_MANUAL_EPOCH_UPDATE) {
        FspsUpd->FspsConfig.SgxEpoch0           = SgxCfgData->SgxEpoch0;
        FspsUpd->FspsConfig.SgxEpoch1           = SgxCfgData->SgxEpoch1;
        DEBUG ((DEBUG_INFO, "Owner Epoch for Intel(R) SGX was updated.\n"));
      }
      UpdateFspsSgxConfigStatus = TRUE;
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to find Intel(R) SGX CFG!\n"));
  }
  return UpdateFspsSgxConfigStatus;
}

/**
  Return TRUE if SGX in Feature Control MSR was set
  IA32_FEATURE_CONTROL MSR(3Ah) [18] == 1

  @retval TRUE if SGX in Feature Control MSR was set.
  @retval FALSE if SGX in Feature Control MSR was not set.
**/
BOOLEAN
IsSgxFeatureCtrlSet (
  VOID
  )
{
  UINT64 Msr;

  Msr = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);
  if (BIT18 == (Msr & BIT18)) {
    return TRUE;
  } else {
    DEBUG ((DEBUG_WARN, "\nIsSgxFeatureCtrlSet(): Intel(R) SGX bit in feature control MSR was NOT set!\n"));
    return FALSE;
  }
}

/**
  Return TRUE if PRMRR base was already set on this core
  @retval TRUE  PRMRR base was already set
  @retval FALSE PRMRR base wasn't set
**/
BOOLEAN
IsPrmrrAlreadySet (
  VOID
  )
{
  if ((AsmReadMsr64 (MSR_PRMRR_PHYS_BASE) != 0) &&
      ((AsmReadMsr64 (MSR_PRMRR_PHYS_MASK) & BIT10) != 0)) {
    return TRUE;
  } else {
    DEBUG ((DEBUG_WARN, "\nIsPrmrrAlreadySet(): PRMRR base was NOT set!\n"));
    return FALSE;
  }
}

/**
 Update SGX CPU NVS variable for EPC device in Platform.asl

 @param[in, out]  CpuNvs           Pointer to CPU NVS region
**/
VOID
UpdateSgxNvs (
  IN OUT CPU_NVS_AREA *CpuNvs
  )
{
  if (CpuNvs == NULL) {
    return;
  }
  EFI_CPUID_REGISTER CpuidSgxLeaf;
  EFI_CPUID_REGISTER Cpuid;

  DEBUG ((DEBUG_INFO, "\nUpdateSgxNvs started...\n"));

  CpuNvs->SgxStatus      = 0;
  CpuNvs->EpcBaseAddress = 0;
  CpuNvs->EpcLength      = 1;

  /**
    Check if all of these conditions are met
    - 1: EnableSgx policy was set to enable
    - 2: SGX feature is supported by CPU
    - 3: PRM was successfully allocated and PRMRRs were set
    - 4: SGX IA32_FEATURE_CONTROL MSR(3Ah) [18] == 1
  **/
  if (IsSgxCapSupported () && IsPrmrrAlreadySet () && IsSgxFeatureCtrlSet ()) {
    //
    // Read Cpuid.Sgx_LEAF.0x2 to get EPC Base and Size
    //
    CpuidSgxLeaf.RegEax = SGX_EPC_CPUID;
    AsmCpuidEx (CpuidSgxLeaf.RegEax, 0x2, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);

    //
    // Check if the first sub-leaf is a valid EPC section
    //
    if ((Cpuid.RegEax & 0xF) != 0x1) {
      DEBUG ((DEBUG_WARN, "\nUpdateSgxNvs(): Invalid EPC section!\n"));
      return;
    }
    DEBUG ((DEBUG_INFO, "\nUpdateSgxNvs(): Intel(R) SGX is ENABLED.\n"));
    CpuNvs->EpcBaseAddress = LShiftU64 ((UINT64) (Cpuid.RegEbx & 0xFFFFF), 32) + (UINT64) (Cpuid.RegEax & 0xFFFFF000);
    CpuNvs->EpcLength      = LShiftU64 ((UINT64) (Cpuid.RegEdx & 0xFFFFF), 32) + (UINT64) (Cpuid.RegEcx & 0xFFFFF000);
    CpuNvs->SgxStatus      = TRUE;
  } else {
    DEBUG ((DEBUG_WARN, "\nUpdateSgxNvs(): Intel(R) SGX is not supported!\n"));
  }
  DEBUG ((DEBUG_INFO, "CpuNvs->SgxStatus      = 0x%X\n",      CpuNvs->SgxStatus));
  DEBUG ((DEBUG_INFO, "CpuNvs->EpcBaseAddress = 0x%016llX\n", CpuNvs->EpcBaseAddress));
  DEBUG ((DEBUG_INFO, "CpuNvs->EpcLength      = 0x%016llX\n", CpuNvs->EpcLength));
  return;
}
