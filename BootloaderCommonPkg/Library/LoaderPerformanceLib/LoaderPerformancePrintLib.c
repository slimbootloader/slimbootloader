/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Guid/CsmePerformanceInfoGuid.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>
#include <Guid/LoaderFspInfoGuid.h>
#include <Library/BaseMemoryLib.h>
#include "ExtendedFirmwarePerformance.h"

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
    return "Extend Stage2 hash";
  case 0x20D0:
    return "Rebase Stage2";
  case 0x3000:
    return "Stage2 entry point";
  case 0x3010:
    return "Board PreSiliconInit hook";
  case 0x3020:
    return "Save NVS data";
  case 0x3030:
    return "FSP SiliconInit";
  case 0x3040:
    return "Board PostSiliconInit hook";
  case 0x3050:
    return "Display splash";
  case 0x3060:
    return "MP wake up";
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
    return "Locate payload";
  case 0x3120:
    return "Copy payload to memory";
  case 0x3130:
    return "Verify payload";
  case 0x3140:
    return "Decompress payload";
  case 0x3150:
    return "Extend payload hash";
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
    return "End of stage2";
  }
  return NULL;
}

/**
  Provide description string corresponding to FSP performance Id.

  @param[in]  Id                MeasurePoint Id

  @retval Default description string
**/
CHAR8 *
FspPerfIdToStr (
  IN UINT32 Id
  )
{

  switch (Id) {
  case 0xF000:
    return "TempRamInit entry";
  case 0xF07F:
    return "TempRamInit exit";
  case 0xD000:
    return "MemoryInit entry";
  case 0xD07F:
    return "MemoryInit exit";
  case 0xB000:
    return "TempRamExit entry";
  case 0xB07F:
    return "TempRamExit exit";
  case 0x9000:
    return "SiliconInit entry";
  case 0x907F:
    return "SiliconInit exit";

  }
  return NULL;
}

/**
  Provide description string for csme perf data corresponding to Id.

  @param[in]  Id  MeasurePoint Id

  @retval Default description string
**/
CHAR8 *
EFIAPI
CsmePerfIdToStr (
  IN UINT32 Id
  )
{
  switch (Id) {
  case 0:
    return "CSME ROM start execution";
  case 1:
    return "EC Boot Load Done (CSME ROM starts main execution)";
  case 2:
    return "CSME ROM completed execution / CSME RBE started";
  case 3:
    return "CSME got ESE Init Done indication from ESE";
  case 4:
    return "CSME RBE start PMC patch/es loading";
  case 5:
    return "CSME RBE completed PMC patch/es loading";
  case 6:
    return "CSME RBE set Boot Stall Done indication to PMC";
  case 7:
    return "CSME start poll for PMC PPS register";
  case 8:
    return "PMC set PPS";
  case 9:
    return "CSME BUP start running";
  case 10:
    return "CSME set Host Boot Prep Done indication to PMC";
  case 11:
    return "CSME starts PHYs loading";
  case 12:
    return "CSME completed PHYs loading";
  case 13:
    return "PMC indicated CSME that xxPWRGOOD was asserted";
  case 14:
    return "PMC indicated CSME that SYS_PWROK was asserted";
  case 15:
    return "PMC sent CPU_BOOT_CONFIG start message to CSME";
  case 16:
    return "CSME sent CPU_BOOT_CONFIG done message to PMC";
  case 17:
    return "PMC indicated CSME that xxPLTRST was de-asserted";
  case 18:
    return "PMC indicated CSME that TCO_S0 was asserted";
  case 19:
    return "PMC sent Core Reset Done Ack - Sent message to CSME";
  case 20:
    return "ACM Active indication - ACM started its execution";
  case 21:
    return "ACM Done indication - ACM completed execution";
  case 22:
    return "BIOS sent DRAM Init Done message";
  case 23:
    return "CSME sent DRAM Init Done message back to BIOS";
  case 24:
    return "CSME completed loading TCSS";
  case 25:
    return "CSME started loading ISH Bringup module";
  case 26:
    return "CSME completed loading ISH Bringup module";
  case 27:
    return "CSME started loading ISH Main module";
  case 28:
    return "CSME completed loading Main module";
  case 29:
    return "BIOS sent End Of Post  message to CSME";
  case 30:
    return "CSME sent End Of Post ack message back to BIOS";
  case 31:
    return "BIOS sent Core BIOS Done message to CSME";
  case 32:
    return "CSME sent Core BIOS Done ack message back to BIOS";
  case 33:
    return "CSME reached Firmware Init Done";
  case 63:
    return "Timestamp when CSME responded to BupGetEarlyBootData message itself";
  default:
    return "Reserved";
  }
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
  Get Measurement form Fpdt records.

  @param[in]   RecordHeader        Pointer to the FPDT record.
  @param[out]  Measurement         Pointer to the measurement which need to be filled.

**/
VOID
GetMeasurementInfo (
  IN     EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER  *RecordHeader,
  OUT    MEASUREMENT_RECORD                           *Measurement
  )
{
  switch (RecordHeader->Type) {
  case FPDT_GUID_EVENT_TYPE:
    CopyMem (&(Measurement->ModuleGuid), &(((FPDT_GUID_EVENT_RECORD *)RecordHeader)->Guid), sizeof (EFI_GUID));
    Measurement->Identifier       = ((UINT32)((FPDT_GUID_EVENT_RECORD *)RecordHeader)->ProgressID);
    Measurement->StartTimeStamp   = ((FPDT_GUID_EVENT_RECORD *)RecordHeader)->Timestamp;
    Measurement->Token            = "PEIM";
    break;

  case FPDT_DYNAMIC_STRING_EVENT_TYPE:
    CopyMem (&(Measurement->ModuleGuid), &(((FPDT_DYNAMIC_STRING_EVENT_RECORD *)RecordHeader)->Guid), sizeof (EFI_GUID));
    Measurement->Identifier       = ((UINT32)((FPDT_DYNAMIC_STRING_EVENT_RECORD *)RecordHeader)->ProgressID);
    Measurement->StartTimeStamp   = ((FPDT_DYNAMIC_STRING_EVENT_RECORD *)RecordHeader)->Timestamp;
    Measurement->Token            = ((FPDT_DYNAMIC_STRING_EVENT_RECORD *)RecordHeader)->String;
    Measurement->Module           = ((FPDT_DYNAMIC_STRING_EVENT_RECORD *)RecordHeader)->String;

    break;

  case FPDT_GUID_QWORD_EVENT_TYPE:
    CopyMem (&(Measurement->ModuleGuid), &(((FPDT_GUID_QWORD_EVENT_RECORD *)RecordHeader)->Guid), sizeof (EFI_GUID));
    Measurement->Identifier       = ((UINT32)((FPDT_GUID_QWORD_EVENT_RECORD *)RecordHeader)->ProgressID);
    Measurement->StartTimeStamp   = ((FPDT_GUID_QWORD_EVENT_RECORD *)RecordHeader)->Timestamp;
    Measurement->Token            = "LoadImage";
    break;

  case  FPDT_GUID_QWORD_STRING_EVENT_TYPE:
    CopyMem (&(Measurement->ModuleGuid), &(((FPDT_GUID_QWORD_STRING_EVENT_RECORD *)RecordHeader)->Guid), sizeof (EFI_GUID));
    Measurement->Identifier       = ((UINT32)((FPDT_GUID_QWORD_STRING_EVENT_RECORD *)RecordHeader)->ProgressID);
    Measurement->StartTimeStamp   = ((FPDT_GUID_QWORD_STRING_EVENT_RECORD*)RecordHeader)->Timestamp;
    Measurement->Token            = "DB_start";
    break;

  case FPDT_DUAL_GUID_STRING_EVENT_TYPE:
    CopyMem (&(Measurement->ModuleGuid), &(((FPDT_DUAL_GUID_STRING_EVENT_RECORD *)RecordHeader)->Guid1), sizeof (EFI_GUID));
    Measurement->Identifier       = ((UINT32)((FPDT_DUAL_GUID_STRING_EVENT_RECORD *)RecordHeader)->ProgressID);
    Measurement->StartTimeStamp   = ((FPDT_DUAL_GUID_STRING_EVENT_RECORD *)RecordHeader)->Timestamp;
    Measurement->Token            = ((FPDT_DUAL_GUID_STRING_EVENT_RECORD *)RecordHeader)->String;
    break;

  default:
    break;
  }

  if (FspPerfIdToStr (Measurement->Identifier) != NULL) {
    Measurement->Token = FspPerfIdToStr (Measurement->Identifier);
  }
}

/**
  Print FSP Measure Point information.

**/
VOID
PrintFspPerfData (
  VOID
  )
{
  UINT8                                       *FspFirmwarePerformance;
  EFI_HOB_GUID_TYPE                           *GuidHob;
  FPDT_PEI_EXT_PERF_HEADER                    *FspPerformanceLogHeader;
  MEASUREMENT_RECORD                          Rec;
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER *RecordHeader;
  UINT8                                       *StartRecordEvent;
  UINT32                                      DataSize;
  UINT32                                      Time;
  LOADER_FSP_INFO                            *FspInfo;

  // Get FSP Info HOB
  FspInfo = GetFirstGuidHob (&gLoaderFspInfoGuid);
  if (FspInfo == NULL) {
    return;
  }
  FspInfo = GET_GUID_HOB_DATA (FspInfo);

  FspFirmwarePerformance = NULL;
  GuidHob = GetNextGuidHob (&gEdkiiFpdtExtendedFirmwarePerformanceGuid, FspInfo->FspHobList);
  while (GuidHob != NULL) {
    FspFirmwarePerformance   = (UINT8*)GET_GUID_HOB_DATA (GuidHob);
    FspPerformanceLogHeader = (FPDT_PEI_EXT_PERF_HEADER *)FspFirmwarePerformance;

    DEBUG ((DEBUG_INFO | DEBUG_EVENT, " FspPerfHob: HobIsFull             - 0x%x\n", FspPerformanceLogHeader->HobIsFull));
    DEBUG ((DEBUG_INFO | DEBUG_EVENT, " FspPerfHob: SizeOfAllEntries      - 0x%x\n", FspPerformanceLogHeader->SizeOfAllEntries));

    DEBUG ((DEBUG_INFO | DEBUG_EVENT, " Dump performance data for FSP HOB:\n"));
    DEBUG ((DEBUG_INFO | DEBUG_EVENT, " Id   | Time (ms)  |  Token                   |            Module        \n"));
    DEBUG ((DEBUG_INFO | DEBUG_EVENT, "------+------------+--------------------------|--------------------------\n"));

    DataSize = 0;
    RecordHeader = (EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER *)(FspFirmwarePerformance + sizeof (FPDT_PEI_EXT_PERF_HEADER));
    StartRecordEvent  = (UINT8 *)RecordHeader;
    while (DataSize < FspPerformanceLogHeader->SizeOfAllEntries) {
      RecordHeader = (EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER *)(StartRecordEvent + DataSize);
      GetMeasurementInfo (RecordHeader, &Rec);
      Time = (UINT32)DivU64x32 (Rec.StartTimeStamp, 1000000);
      DEBUG ((DEBUG_INFO | DEBUG_EVENT, " %4X | %7d ms | %-25a| %g\n", Rec.Identifier, Time, Rec.Token, &Rec.ModuleGuid));
      DataSize += RecordHeader->Length;
    }

    //
    // Previous HOB is used, then find next one.
    //
    GuidHob = GetNextGuidHob (&gEdkiiFpdtExtendedFirmwarePerformanceGuid, GET_NEXT_HOB (GuidHob));
  }

  DEBUG ((DEBUG_INFO | DEBUG_EVENT,   "------+------------+--------------------------|--------------------------\n"));
}


/**
  Print Bootloader Measure Point information.

  @param[in]  PerfData          A pointer indicating BL_PERF_DATA instance to print performance data
  @param[in]  PerfIdToStrTbl    A pointer to description table corresponding to Id

**/
VOID
PrintBootloaderPerfData (
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

/**
  Print CSME boot time performance data.

**/
VOID
PrintCsmePerfData (
  VOID
  )
{
  UINT8                 Index;
  const CHAR8           *Desc;
  UINT32                PrevTS;
  EFI_HOB_GUID_TYPE     *GuidHob;
  CSME_PERFORMANCE_INFO  *CsmePerformanceInfo;

  GuidHob = GetNextGuidHob (&gCsmePerformanceInfoGuid, GetHobList());
  if (GuidHob == NULL) {
    return;
  }

  CsmePerformanceInfo = (CSME_PERFORMANCE_INFO *)GET_GUID_HOB_DATA (GuidHob);
  if (CsmePerformanceInfo == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO | DEBUG_EVENT, "\n Csme Boot Time Performance information\n"));

  DEBUG ((DEBUG_INFO | DEBUG_EVENT, "------+------------+------------+----------------------------------\n"));
  DEBUG ((DEBUG_INFO | DEBUG_EVENT, " Id   | Time (ms)  | Delta (ms) | Description                      \n"));
  DEBUG ((DEBUG_INFO | DEBUG_EVENT, "------+------------+------------+----------------------------------\n"));

  //
  // Initialize previous time stamp to CSME ROM Start execution TS
  //
  PrevTS = CsmePerformanceInfo->BootPerformanceData[1];
  for (Index = 1; Index < CsmePerformanceInfo->BootDataLength; Index++) {
    //
    // Ignore timestamps with value as 0
    //
    if (CsmePerformanceInfo->BootPerformanceData[Index] == 0) {
      continue;
    }
    Desc = PerfIdToStr (Index, CsmePerfIdToStr);
    DEBUG ((DEBUG_INFO | DEBUG_EVENT, " %4X | %7d ms | %7d ms | %a\n", \
          Index,\
          CsmePerformanceInfo->BootPerformanceData[Index],\
          CsmePerformanceInfo->BootPerformanceData[Index] - PrevTS,\
          Desc));
    PrevTS = CsmePerformanceInfo->BootPerformanceData[Index];
  }
  DEBUG ((DEBUG_INFO | DEBUG_EVENT, "------+------------+------------+----------------------------------\n"));
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
  // Print bootloader performance
  if ((PcdGet32 (PcdBootPerformanceMask) & BIT0) != 0) {
    PrintBootloaderPerfData (PerfData, PerfIdToStrTbl);
  }

  // Print FSP boot performance
  if ((PcdGet32 (PcdBootPerformanceMask) & BIT1) != 0) {
    PrintFspPerfData ();
  }

  // Print CSME boot performance
  if ((PcdGet32 (PcdBootPerformanceMask) & BIT2) != 0) {
    PrintCsmePerfData ();
  }
}
