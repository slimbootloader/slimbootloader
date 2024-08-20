/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"


typedef struct {
  CPU_SKU   CpuSku;
  UINT32    CpuIdFamilyModel;
  UINT16    SaDeviceId;
  UINT8     BigCoreCount;
  UINT8     AtomCoreCount;
} CPU_SKU_INFO;

//
// "+ 2" in AtomCoreCount field in below array is to include the 2 atom cores in SOC die.
//
CPU_SKU_INFO mCpuSkuInfo[] = {
  //
  // CpuSku,    CpuIdFamilyModel,                          SaDeviceId, BigCoreCount, AtomCoreCount
  //
  //
  // M
  //
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULX_1, 2, 8},

  //
  // P
  //
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_1, 6, 8},
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_2, 2, 8},
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_3, 6, 8},
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_8, 4, 8},
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_9, 4, 4},
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_10, 2, 4},
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_11, 4, 0},

  //
  //PS
  //
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_12, 6, 8 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_13, 4, 8 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_14, 4, 4 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_15, 2, 8 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_16, 2, 4 },

  // S
  //
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_METEORLAKE_DT_HALO, V_SA_DEVICE_ID_DT_1, 8, 16},
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_METEORLAKE_DT_HALO, V_SA_DEVICE_ID_DT_2, 6, 8},

  //
  // P Validation
  //
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE, V_SA_DEVICE_ID_MB_ULT_5, 6, 8 + 2 },

  //
  // MTL S
  //
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_METEORLAKE_DT_HALO, MTL_SA_DEVICE_ID_S_6C_8A, 6, 8 },
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_METEORLAKE_DT_HALO, MTL_SA_DEVICE_ID_S_6C_4A, 6, 4 },
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_METEORLAKE_DT_HALO, MTL_SA_DEVICE_ID_S_4C_4A, 4, 4 },
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_METEORLAKE_DT_HALO, MTL_SA_DEVICE_ID_S_6C_16A, 6, 16 },

  //
  // ARL S
  //
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_S_6C_8A,    6,  8 },
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_S_8C_12A,   8, 12 },
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_S_8C_16A,   8, 16 },
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_S_8C_32A,   8, 32 },
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_S_6C_12A,   6, 12 },
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_S_6C_4A,    6,  4 },
  { EnumCpuTrad, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_S_4C_4A,    4,  4 },

  //
  // ARL U - MTL refresh
  //
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_U, ARL_SA_DEVICE_ID_U_2C_8A, 2, 8 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_U, ARL_SA_DEVICE_ID_U_2C_4A, 2, 4 },

  //
  // ARL H
  //
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_H, ARL_SA_DEVICE_ID_H_2C_4A, 2, 4 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_H, ARL_SA_DEVICE_ID_H_2C_8A, 2, 8 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_H, ARL_SA_DEVICE_ID_H_4C_4A, 4, 4 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_H, ARL_SA_DEVICE_ID_H_4C_8A, 4, 8 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_H, ARL_SA_DEVICE_ID_H_6C_4A, 6, 4 },
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_H, ARL_SA_DEVICE_ID_H_6C_8A, 6, 8 },

  //
  // ARL HX
  //
  { EnumCpuHalo, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_HX_8C_16A,  8, 16 },
  { EnumCpuHalo, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_HX_8C_12A,  8, 12 },
  { EnumCpuHalo, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_HX_6C_12A,  6, 12 },
  { EnumCpuHalo, CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO, ARL_SA_DEVICE_ID_HX_6C_8A,   6,  8 }

};

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
  UINTN              Index;

  CpuType   = EnumCpuUnknown;

  for (Index = 0; Index < ARRAY_SIZE (mCpuSkuInfo); Index++) {
    // cpudid 0x7D14, cpufamilymodel: 0x000A06A0!

    if ((CpuFamilyModel == mCpuSkuInfo[Index].CpuIdFamilyModel) && (CpuDid == mCpuSkuInfo[Index].SaDeviceId)) {
      return mCpuSkuInfo[Index].CpuSku;
    }
  }

  DEBUG ((DEBUG_ERROR, "Unsupported CPU SKU, Device ID: 0x%04X, CPUID: 0x%08X!\n", CpuDid, CpuFamilyModel));
  ASSERT (FALSE);

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
  CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_EDX Edx;

  //
  // Check Hetero feature is supported
  // with CPUID.(EAX=7,ECX=0):EDX[15]=1
  //
  AsmCpuidEx (CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS, 0, NULL, NULL, NULL, &Edx.Uint32);
  if (Edx.Bits.Hybrid == 1) {
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

/**
  Return CPU name

  @retval               CPU name string
**/
CHAR8 *
GetCpuName (
  VOID
  )
{
  UINT32                  CpuFamilyModel;
  CPUID_VERSION_INFO_EAX  Eax;

  ///
  /// Read the CPUID & DID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, NULL, NULL, NULL);
  CpuFamilyModel = Eax.Uint32 & CPUID_FULL_FAMILY_MODEL;

  switch (CpuFamilyModel) {
    case CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE:
    case CPUID_FULL_FAMILY_MODEL_METEORLAKE_DT_HALO:
      return "MeteorLake";
    case CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO:
    case CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_20A:
    case CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_H:
    case CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_U:
      return "ArrowLake";
    default:
      return "Unknown";
  }

}

