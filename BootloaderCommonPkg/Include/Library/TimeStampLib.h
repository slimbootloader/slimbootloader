/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __TIME_STAMP_LIB_H__
#define __TIME_STAMP_LIB_H__

/**
  Read current timestamp.

  @return  64 bit current timestampe value.

**/
UINT64
EFIAPI
ReadTimeStamp (
  VOID
  );

/**
  Get timestamp frequency in KHZ.

  @return   Timestamp frequency in KHZ.

**/
UINT32
EFIAPI
GetTimeStampFrequency (
  VOID
  );

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
  );

/**
  Convert timestamp ticks to microseconds.

  @param[in]  Ticks  The number of timestamp ticks to convert.

  @retval     MicroSeconds

**/
UINT64
EFIAPI
TimeStampTickToMicroSecond (
  IN UINT64    Ticks
  );

/**
  Convert microseconds to timestamp ticks.

  @param[in]  MicroSeconds  The number of microseconds to convert.

  @retval     Timestamp ticks

**/
UINT64
EFIAPI
MicroSecondToTimeStampTick (
  IN UINT64    MicroSeconds
  );

#endif
