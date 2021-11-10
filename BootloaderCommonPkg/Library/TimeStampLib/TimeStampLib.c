/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/Intel/Cpuid.h>
#include <Library/BaseLib.h>

/**
  Read current timestamp.

  @return  64 bit current timestampe value.

**/
UINT64
EFIAPI
ReadTimeStamp (
  VOID
  )
{
  return AsmReadTsc();
}

/**
  Get timestamp frequency in KHZ.

  @return   Timestamp frequency in KHZ.

**/
UINT32
EFIAPI
GetTimeStampFrequency (
  VOID
  )
{
  UINT32  Ratio;

  Ratio = ((UINT32)AsmReadMsr64 (0xCE) >> 8) & 0xFF;
  if (Ratio == 0) {
    // This might be QEMU case
    Ratio = 8;
  }
  // Ratio * 100000
  return (Ratio * 100000);
}

/**
  Get timestamp accurate frequency in HZ by CPUID.

  The TSC counting frequency is determined by using CPUID leaf 0x15. Frequency in MHz = Core XTAL frequency * EBX/EAX.
  In newer flavors of the CPU, core xtal frequency is returned in ECX or 0 if not supported.

  @retval The number of TSC counts per second.

**/
UINT64
EFIAPI
GetTimeStampAccurateFrequency (
  VOID
  )
{
  UINT64                 TscFrequency;
  UINT32                 RegEax;
  UINT32                 RegEbx;
  UINT32                 RegEcx;

  // Use CPUID leaf 0x15 Time Stamp Counter and Nominal Core Crystal Clock Information
  // EBX returns 0 if not supported. ECX, if non zero, provides Core Xtal Frequency in hertz.
  // TSC frequency = (ECX, Core Xtal Frequency) * EBX/EAX.
  AsmCpuid (CPUID_TIME_STAMP_COUNTER, &RegEax, &RegEbx, &RegEcx, NULL);

  // If EAX, EBX or ECX returns 0, the XTAL ratio is not enumerated.
  if ((RegEax == 0) || (RegEbx == 0 ) || (RegEcx == 0)) {
    // Fallback to use GetTimeStampFrequency() instead
    TscFrequency = MultU64x32 (GetTimeStampFrequency(), 1000);
  } else {
    // Calculate TSC frequency = (ECX, Core Xtal Frequency) * EBX/EAX
    TscFrequency = DivU64x32 (MultU64x32 (RegEcx, RegEbx) + (UINT64)(RegEax >> 1), RegEax);
  }

  return TscFrequency;
}

/**
  Convert microseconds to timestamp ticks.

  @param[in]  MicroSeconds  The number of microseconds to convert.

  @retval     Timestamp ticks

**/
UINT64
EFIAPI
MicroSecondToTimeStampTick (
  IN UINT64    MicroSeconds
  )
{
  return DivU64x32 (
                      MultU64x64 (
                        GetTimeStampAccurateFrequency (),
                        MicroSeconds
                      ),
                      1000000u
                   );
}

/**
  Convert timestamp ticks to microseconds.

  @param[in]  Ticks  The number of timestamp ticks to convert.

  @retval     MicroSeconds

**/
UINT64
EFIAPI
TimeStampTickToMicroSecond (
  IN UINT64    Ticks
  )
{
  return DivU64x64Remainder (
                               MultU64x32 (Ticks, 1000000u),
                               GetTimeStampAccurateFrequency (),
                               NULL
                            );
}

