/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>

/**
  Convert GUID to String.

  @param[in]  Guid                GUID to be converted
  @param[out] Buffer              Char buffer which will have GUID ascii string

**/
VOID
EFIAPI
GuidToStr (
  IN  EFI_GUID  *Guid,
  OUT CHAR8     *Buffer
  )
{
  AsciiSPrint (Buffer,  48, "%g", Guid);
}

/**
  Provide default description string corresponding to Id.

  @param[in]  Id                MeasurePoint Id

  @retval Default description string
**/
CHAR8 *
EFIAPI
DefPerfIdToStr (
  IN UINT32 Id
  )
{
  switch (Id) {
  case 0x1000:
    return "Reset vector";
  case 0x1010:
    return "Stage1A entry point";
  case 0x1040:
    return "Board PostTempRamInit hook";
  case 0x1060:
    return "Stage1A continuation";
  case 0x1080:
    return "Load Stage1B";
  case 0x10A0:
    return "Verify Stage1B";
  case 0x10B0:
    return "Decompress Stage1B";
  case 0x2000:
    return "Stage1B entry point";
  case 0x2020:
    return "Board PreMemoryInit hook";
  case 0x2030:
    return "FSP MemoryInit";
  case 0x2040:
    return "Board PostMemoryInit hook";
  case 0x2050:
    return "Board PreTempRamExit hook";
  case 0x2060:
    return "FSP TempRamExit";
  case 0x2070:
    return "Board PostTempRamExit hook";
  case 0x2080:
    return "Load Stage2";
  case 0x2090:
    return "Copy Stage2 to memory";
  case 0x20A0:
    return "Verify Stage2";
  case 0x20B0:
    return "Decompress Stage2";
  case 0x20C0:
    return "Rebase Stage2";
  case 0x3000:
    return "Stage2 entry point";
  case 0x3020:
    return "Board PreSiliconInit hook";
  case 0x3030:
    return "FSP SiliconInit";
  case 0x3040:
    return "Board PostSiliconInit hook";
  case 0x3050:
    return "Display splash";
  case 0x3060:
    return "MP wake up";
  case 0x3070:
    return "Save NVS data";
  case 0x3080:
    return "MP init run";
  case 0x3090:
    return "Board PrePciEnumeration hook";
  case 0x30A0:
    return "PCI enumeration";
  case 0x30B0:
    return "Board PostPciEnumeration hook";
  case 0x30C0:
    return "FSP PostPciEnumeration notify";
  case 0x30D0:
    return "ACPI init";
  case 0x30E0:
    return "Board PrePayloadLoading hook";
  case 0x3100:
    return "Load payload";
  case 0x3110:
    return "Copy payload to memory";
  case 0x3120:
    return "Verify payload";
  case 0x3130:
    return "Decompress payload";
  case 0x31A0:
    return "Board PostPayloadLoading hook";
  case 0x31B0:
    return "Decode payload format";
  case 0x31C0:
    return "MP init done";
  case 0x31D0:
    return "FSP ReadyToBoot notify";
  case 0x31E0:
    return "FSP EndOfFirmware notify";
  case 0x31F0:
    return "Board ReadyToBoot hook";
  }
  return NULL;
}

/**
  Provide description string corresponding to Id.

  If there is a description in default description table corresponding to Id,
  default description will be printed
  If there is no description in default description table,
    look for description in overrided description table once if it is provided
  Otherwise,
    just print empty string

  @param[in]  Id                MeasurePoint Id
  @param[in]  PerfIdToStrTbl    A pointer to description table

  @retval Description string
**/
CHAR8 *
EFIAPI
PerfIdToStr (
  IN UINT32         Id,
  IN PERF_ID_TO_STR PerfIdToStrTbl
  )
{
  CHAR8  *Desc;

  Desc = DefPerfIdToStr (Id);
  if ((Desc == NULL) && (PerfIdToStrTbl != NULL)) {
    Desc = PerfIdToStrTbl (Id);
  }

  if (Desc == NULL) {
    Desc = " ";
  }

  return Desc;
}

/**
  Print Bootloader Measure Point information.

  @param[in]  PerfData          A pointer indicating BL_PERF_DATA instance to print performance data
  @param[in]  PerfIdToStrTbl    A pointer to description table corresponding to Id

**/
VOID
EFIAPI
PrintMeasurePoint (
  IN BL_PERF_DATA   *PerfData,
  IN PERF_ID_TO_STR  PerfIdToStrTbl
  )
{
  UINT32      Idx;
  UINT32      Time;
  UINT32      PrevTime;
  UINT16      Id;
  UINT64      Tsc;
  const CHAR8 *Desc;

  PrevTime = 0;

  DEBUG ((DEBUG_INFO | DEBUG_EVENT, " Id   | Time (ms)  | Delta (ms) | Description                      \n"));
  DEBUG ((DEBUG_INFO | DEBUG_EVENT, "------+------------+------------+----------------------------------\n"));
  for (Idx = 0; Idx < PerfData->PerfIndex; Idx++) {
    Tsc  = PerfData->TimeStamp[Idx];
    Id   = ((UINT16 *)&Tsc)[3];
    ((UINT16 *)&Tsc)[3] = 0;
    Time = (UINT32)DivU64x32 (Tsc, PerfData->FreqKhz);
    Desc = PerfIdToStr (Id, PerfIdToStrTbl);
    DEBUG ((DEBUG_INFO | DEBUG_EVENT, " %4X | %7d ms | %7d ms | %a\n", Id, Time, Time - PrevTime, Desc));
    PrevTime = Time;
  }
  DEBUG ((DEBUG_INFO | DEBUG_EVENT, "------+------------+------------+----------------------------------\n"));
}
