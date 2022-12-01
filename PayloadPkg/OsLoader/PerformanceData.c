/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"

/**
  Decodes the description as per the provided Id.

  @param[in]    PerfId  Id number to decode the string.

  @retval   String correspoding to the Id.
**/
STATIC
CHAR8 *
EFIAPI
LinuxPerfIdToStr (
  IN UINT32                  PerfId
  )
{
  switch (PerfId) {
  case 0x4000:
    return "Payload entry point";
  case 0x4010:
    return "OS loader main entry";
  case 0x4020:
    return "Shell exit";
  case 0x4040:
    return "Load image entry";
  case 0x4050:
    return "Boot device init";
  case 0x4055:
    return "Boot device tuning";
  case 0x4060:
    return "Parse partition info";
  case 0x4070:
    return "Load boot images";
  case 0x4080:
    return "Verify boot image";
  case 0x40A0:
    return "Process boot image type";
  case 0x40B0:
    return "Process ELF/MultiBoot";
  case 0x40E0:
    return "Kernel setup";
  case 0x40F0:
    return "FSP ReadyToBoot/EndOfFirmware notify";
  case 0x4100:
    return "TPM IndicateReadyToBoot";
  default:
    return NULL;
  }
}


/**
  Wrapper function to print LinuxLoader Measure Point information.
**/
VOID
PrintLinuxMeasurePoint (
  VOID
  )
{
  BL_PERF_DATA               *PerfData;

  PerfData = GetPerfDataPtr ();
  PrintMeasurePoint (PerfData, LinuxPerfIdToStr);
}

