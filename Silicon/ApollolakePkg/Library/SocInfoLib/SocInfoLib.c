/** @file
  The soc info library.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <RegAccess.h>
#include <CpuRegs.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Register/Intel/Cpuid.h>

typedef struct
{
  UINT8   RevId;
  UINT8   PlatformId;
  CHAR8   Device[5];
  CHAR8   Stepping[3];
} SOC_REV_INFO;

CONST SOC_REV_INFO mBxtSocRevInfo[] = { // new      old
  { 0x3,  0,  "BXTP",  "A0" },        // BXTP-A0  BXTP-A0
  { 0x9,  0,  "BXTP",  "B0" },        // BXTP-B0  BXTP-B0
  { 0xa,  0,  "APL",   "B0" },        // APL-B0   BXTP-B1
  { 0xb,  0,  "APL",   "B1" },        // APL-B1   BXTP-B2
  { 0xb,  1,  "APL",   "D0" },        // APL-D0  (BXTP-B2)
  { 0xd,  1,  "APL",   "F1" },        // APL-F1
  { 0x0,  0,  "INVD",  "XX" }         // Invalid
};

/**
  Get cpu turbo ratio.

  @retval   cpu turbo ratio
**/
UINT32
EFIAPI
GetCpuTurboRatio (
  VOID
)
{
  UINT64 MsrValue;
  MsrValue = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
  return (MsrValue & 0xff);
}

/**
  Get cpu max non-burst frequency.

  @retval   cpu max non-burst frequency
**/
UINT32
EFIAPI
GetCpuMaxNbFrequency (
  VOID
)
{
  UINT64 MsrValue;
  MsrValue = AsmReadMsr64 (MSR_PLATFORM_INFO);
  return (100 * (((UINT32)MsrValue >> 8) & 0xff));
}

/**
  Get cpu core count.

  @retval   cpu core count
**/
UINT32
EFIAPI
GetCpuNumCores (
  VOID
)
{
  UINT32 Ebx;
  AsmCpuidEx (CPUID_EXTENDED_TOPOLOGY, 1, NULL, &Ebx, NULL, NULL); // BWG 3.1.8
  return (Ebx & 0xffff);
}

/**
  Get cpu ucode revision.

  @retval   cpu ucode revision
**/
UINT32
EFIAPI
GetCpuUCodeRev (
  VOID
)
{
  UINT64 MsrValue;
  UINT32 UcodeRev;

  AsmWriteMsr64 (MSR_IA32_BIOS_SIGN_ID, 0LL);
  AsmCpuid (0, NULL, NULL, NULL, NULL);
  MsrValue = AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID);
  UcodeRev = RShiftU64 (MsrValue, 32) & 0xffffffff;

  return UcodeRev;
}

/**
  Get pch device name.

  @retval   pch device name
**/
CONST CHAR8*
EFIAPI
GetPchDeviceName (
  VOID
)
{
  UINT32 PciBase;
  UINT32 Stepping;
  UINT32 PlatformId;
  UINT32 Step;
  UINT64 MsrValue;

  //
  // Determine SoC (device) type and silicon stepping
  //
  PciBase = (UINT32)PcdGet64(PcdPciExpressBaseAddress);
  Stepping = MmioRead8 (PciBase + 8);
  MsrValue = AsmReadMsr64 (MSR_IA32_PLATFORM_ID);
  PlatformId = RShiftU64 (MsrValue, 50) & 7;

  //
  // Lookup silicon stepping in the dictionary
  //
  for (Step = 0 ; Step < ARRAY_SIZE(mBxtSocRevInfo)-1 ; Step += 1) {
    if (mBxtSocRevInfo[Step].RevId == Stepping && mBxtSocRevInfo[Step].PlatformId == PlatformId)
      break;
  }

  return mBxtSocRevInfo[Step].Device;
}

/**
  Get pch stepping name.

  @retval   pch stepping name
**/
CONST CHAR8*
EFIAPI
GetPchSteppingName (
  VOID
)
{
  UINT32 PciBase;
  UINT32 Stepping;
  UINT32 PlatformId;
  UINT32 Step;
  UINT64 MsrValue;

  //
  // Determine silicon stepping
  //
  PciBase = (UINT32)PcdGet64(PcdPciExpressBaseAddress);
  Stepping = MmioRead8 (PciBase + 8);
  MsrValue = AsmReadMsr64 (0x17);
  PlatformId = RShiftU64 (MsrValue, 50) & 7;

  //
  // Lookup silicon stepping in the dictionary
  //
  for (Step = 0 ; Step < ARRAY_SIZE(mBxtSocRevInfo)-1 ; Step += 1) {
    if (mBxtSocRevInfo[Step].RevId == Stepping && mBxtSocRevInfo[Step].PlatformId == PlatformId)
      break;
  }

  return mBxtSocRevInfo[Step].Stepping;
}

