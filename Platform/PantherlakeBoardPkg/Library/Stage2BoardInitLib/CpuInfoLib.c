/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
  // PTL U, H 4Xe/12Xe
  //
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_U_SA_DEVICE_ID_4C,             4, 0},  // PTL U      4+0
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_U_SA_DEVICE_ID_2C,             2, 0},  // PTL U      2+0
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_12XE_SA_DEVICE_ID_4C_8A,     4, 8},  // PTL H 12Xe 4+8
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_12XE_SA_DEVICE_ID_4C_4A,     4, 4},  // PTL H 12Xe 4+4
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_12XE_SA_DEVICE_ID_2C_8A,     2, 8},  // PTL H 12Xe 2+8
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_12XE_SA_DEVICE_ID_2C_4A,     2, 4},  // PTL H 12Xe 2+4
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_SA_DEVICE_ID_4C_8A,      4, 8},  // PTL H 4Xe  4+8
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_SA_DEVICE_ID_4C_4A,      4, 4},  // PTL H 4Xe  4+4
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_SA_DEVICE_ID_2C_8A,      2, 8},  // PTL H 4Xe  2+8
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_SA_DEVICE_ID_2C_4A,      2, 4},  // PTL H 4Xe  2+4
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_U_NEX_SA_DEVICE_ID_4C,         4, 0},  // PTL U      4+0 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_U_NEX_SA_DEVICE_ID_2C,         2, 0},  // PTL U      2+0 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_12XE_NEX_SA_DEVICE_ID_4C_8A, 4, 8},  // PTL H 12Xe 4+8 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_12XE_NEX_SA_DEVICE_ID_4C_4A, 4, 4},  // PTL H 12Xe 4+4 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_12XE_NEX_SA_DEVICE_ID_2C_8A, 2, 8},  // PTL H 12Xe 2+8 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_12XE_NEX_SA_DEVICE_ID_2C_4A, 2, 4},  // PTL H 12Xe 2+4 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_NEX_SA_DEVICE_ID_4C_8A,  4, 8},  // PTL H 4Xe  4+8 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_NEX_SA_DEVICE_ID_4C_4A,  4, 4},  // PTL H 4Xe  4+4 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_NEX_SA_DEVICE_ID_2C_8A,  2, 8},  // PTL H 4Xe  2+8 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_NEX_SA_DEVICE_ID_2C_4A,  2, 4},  // PTL H 4Xe  2+4 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_U_NEX_SA_DEVICE_ID_1C,         1, 0},  // PTL U      1+0 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_NEX_SA_DEVICE_ID_1C,     4, 0},  // PTL H 4Xe  4+0 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE,   PTL_H_4XE_NEX_SA_DEVICE_ID_4C,     4, 0},  // PTL H 4Xe  4+0 (NEX)
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_PANTHERLAKE_MOBILE_E, PTL_H_4XE_NEX_SA_DEVICE_ID_4C_2,   4, 0},  // PTL U      4+0 (NEX)

  //
  // WCL
  //
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_WILDCATLAKE_MOBILE, WCL_SA_DEVICE_ID_2C_4LP,           2, 0}, // WCL
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_WILDCATLAKE_MOBILE, WCL_SA_DEVICE_ID_2C_2LP,           2, 0}, // WCL
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_WILDCATLAKE_MOBILE, WCL_SA_DEVICE_ID_2C,               2, 0}, // WCL
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_WILDCATLAKE_MOBILE, WCL_SA_DEVICE_ID_1C_4LP,           1, 0}, // WCL
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_WILDCATLAKE_MOBILE, WCL_SA_DEVICE_ID_4LP,              0, 0}, // WCL
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_WILDCATLAKE_MOBILE, WCL_SA_DEVICE_ID_2C_4LP_NEX,       2, 0}, // WCL
  { EnumCpuUlt, CPUID_FULL_FAMILY_MODEL_WILDCATLAKE_MOBILE, WCL_SA_DEVICE_ID_1C_4LP_NEX,       1, 0}, // WCL
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
  @retval FALSE - Processor does not support HeteroCore
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
