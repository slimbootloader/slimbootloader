/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"


/**
Return Cpu stepping type

@retval UINT8                   Cpu stepping type
**/
CPU_STEPPING
EFIAPI
GetCpuStepping(
  VOID
  )
{
  CPUID_VERSION_INFO_EAX  Eax;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, NULL, NULL, NULL);
  return ((CPU_STEPPING) (Eax.Uint32 & CPUID_FULL_STEPPING));
}

/**
  Return CPU Sku

  @param[in]  UINT32             CpuFamilyModel
  @param[in]  UINT16             CpuDid

  @retval     UINT8              CPU Sku
**/
UINT8
GetCpuSkuInfo (
  IN UINT32 CpuFamilyModel,
  IN UINT16 CpuDid
  )
{
  UINT8              CpuType;
  BOOLEAN            SkuFound;

  SkuFound  = FALSE;
  CpuType   = EnumCpuUnknown;

  switch (CpuFamilyModel) {
    case CPUID_FULL_FAMILY_MODEL_ALDERLAKE_DT_HALO:
      switch (CpuDid) {
        case V_SA_DEVICE_ID_DT_1:        // AlderLake Desktop (8+8+GT) SA DID
        case V_SA_DEVICE_ID_DT_2:        // AlderLake Desktop (8+6(f)+GT) SA DID
        case V_SA_DEVICE_ID_DT_3:        // AlderLake Desktop (8+4(f)+GT) SA DID
        case V_SA_DEVICE_ID_DT_4:        // AlderLake Desktop (8+2(f)+GT) SA DID
        case V_SA_DEVICE_ID_DT_5:        // AlderLake Desktop (8+0(f)+GT) SA DID
        case V_SA_DEVICE_ID_DT_6:        // AlderLake Desktop (6(f)+8+GT) SA DID
        case V_SA_DEVICE_ID_DT_7:        // AlderLake Desktop (6(f)+6(f)+GT) SA DID
        case V_SA_DEVICE_ID_DT_8:        // AlderLake Desktop (6(f)+4(f)+GT) SA DID
        case V_SA_DEVICE_ID_DT_9:        // AlderLake Desktop (6(f)+2(f)+GT) SA DID
        case V_SA_DEVICE_ID_DT_10:       // AlderLake Desktop (6+0+GT) SA DID
        case V_SA_DEVICE_ID_DT_11:       // AlderLake Desktop (4(f)+0+GT) SA DID
        case V_SA_DEVICE_ID_DT_12:       // AlderLake Desktop (2(f)+0+GT) SA DID
        case V_SA_DEVICE_ID_DT_13:       // AlderLake Desktop (8+6(f)+GT) SA DID
        case V_SA_DEVICE_ID_DT_14:       // AlderLake Desktop (8+6(f)+GT) SA DID
//        case V_SA_DEVICE_ID_DT_15:       // WA ADL_CPU_PO
          CpuType  = EnumCpuTrad;
          SkuFound = TRUE;
          break;
        default:
          break;
      }
    break;
    case 0x000B0670:
    case 0x000B06A0:
        CpuType  = EnumCpuTrad;
        SkuFound = TRUE;
        DEBUG ((DEBUG_INFO, "CPU Device ID: 0x%02X, CPUID: 0x%08X!\n", CpuDid, CpuFamilyModel));
        break;
    case CPUID_FULL_FAMILY_MODEL_ALDERLAKE_MOBILE:
      switch (CpuDid) {
        case V_SA_DEVICE_ID_MB_ULT_1:    // AlderLake P (6+8+GT)
        case V_SA_DEVICE_ID_MB_ULT_2:    // AlderLake P (6+4(f)+GT)
        case V_SA_DEVICE_ID_MB_ULT_3:    // AlderLake P (4(f)+8+GT)
        case V_SA_DEVICE_ID_MB_ULT_4:    // AlderLake P (2(f)+4(f)+GT)
        case V_SA_DEVICE_ID_MB_ULT_5:    // AlderLake P (2+8+GT)
        case V_SA_DEVICE_ID_MB_ULT_6:    // AlderLake P (2+4(f)+GT)
          CpuType  = EnumCpuUlt;
          SkuFound = TRUE;
          break;
        default:
          break;
      }
    break;
  }

  if (!SkuFound) {
    DEBUG ((DEBUG_ERROR, "Unsupported CPU SKU, Device ID: 0x%02X, CPUID: 0x%08X!\n", CpuDid, CpuFamilyModel));
  }

  return CpuType;
}

/**
  Return CPU Sku

  @retval UINT8              CPU Sku
**/
UINT8
EFIAPI
GetCpuSku (
  VOID
  )
{
  UINT16                  CpuDid;
  UINT32                  CpuFamilyModel;
  CPUID_VERSION_INFO_EAX  Eax;

  ///
  /// Read the CPUID & DID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, NULL, NULL, NULL);
  CpuFamilyModel = Eax.Uint32 & CPUID_FULL_FAMILY_MODEL;
  CpuDid = PciRead16 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_MC_DEVICE_ID));

  return GetCpuSkuInfo (CpuFamilyModel, CpuDid);

}



/**
  Detect if Hetero Core is supported.

  Check for CPU whether is Hetrogenous or not.

  @retval TRUE - Processor support HeteroCore
  @retval FALSE - Processor doesnt support HeteroCore
**/
BOOLEAN
IsHeteroCoreSupported (
  VOID
  )
{
  CPUID_EXTENDED_FEATURE_FLAGS_EDX_HETERO Edx;

  //
  // Check Hetero feature is supported
  // with CPUID.(EAX=7,ECX=0):EDX[15]=1
  //
  AsmCpuidEx (CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS, 0, NULL, NULL, NULL, &Edx.Uint32);
  if (Edx.Bits.Hetero == 1) {
    return TRUE;
  }
  return FALSE;
}


/**
  Detect the type of core.

  @retval the core type which is running
    10h - Quark
    20h - Atom
    30H - Knights
    40H - Core
**/
UINT8
DetectCoreType (
  VOID
  )
{
  CPUID_NATIVE_MODEL_ID_INFO_EAX     Eax;

  //
  // Check which is the running core by reading CPUID.(EAX=1AH, ECX=00H):EAX
  //
  AsmCpuid (CPUID_NATIVE_MODEL_ID_INFO, &Eax.Uint32, NULL, NULL, NULL);
  return (UINT8) Eax.Bits.CoreType;
}

/**
  Check if this is non-core processor - HT AP thread

  @retval TRUE if this is HT AP thread
  @retval FALSE if this is core thread
**/
BOOLEAN
IsSecondaryThread (
  VOID
  )
{
  CPUID_VERSION_INFO_EDX      CpuidVersionInfoEdx;
  CPUID_EXTENDED_TOPOLOGY_EAX CpuIdExtendedTopologyEax;
  UINT32                      ApicId;
  UINT32                      MaskShift;
  UINT32                      Mask;

  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, NULL, &CpuidVersionInfoEdx.Uint32);
  if (CpuidVersionInfoEdx.Bits.HTT == 0) {
    return FALSE;
  }

  AsmCpuidEx (
    CPUID_EXTENDED_TOPOLOGY,
    0,  // Sub-leaf 0
    &CpuIdExtendedTopologyEax.Uint32,
    NULL,
    NULL,
    &ApicId
    );

  MaskShift = CpuIdExtendedTopologyEax.Bits.ApicIdShift;
  Mask = ~(0xffffffff << MaskShift);

  return (ApicId & Mask) > 0;
}


/**
  Return CPU Family ID

  @retval CPU_FAMILY              CPU Family ID
**/
CPU_FAMILY
EFIAPI
GetCpuFamily (
  VOID
  )
{
  CPUID_VERSION_INFO_EAX  Eax;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, NULL, NULL, NULL);
  return (Eax.Uint32 & CPUID_FULL_FAMILY_MODEL);
}

