/** @file

  Copyright (c) 2019 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/AcpiInitLib.h>
#include <Library/TimeStampLib.h>

BOOT_PERFORMANCE_TABLE mBootPerformanceTableTemplate = {
  {
    EFI_ACPI_5_0_FPDT_BOOT_PERFORMANCE_TABLE_SIGNATURE,
    sizeof (BOOT_PERFORMANCE_TABLE)
  },
  {
    {
      EFI_ACPI_5_0_FPDT_RUNTIME_RECORD_TYPE_FIRMWARE_BASIC_BOOT,    // Type
      sizeof (EFI_ACPI_5_0_FPDT_FIRMWARE_BASIC_BOOT_RECORD),        // Length
      EFI_ACPI_5_0_FPDT_RUNTIME_RECORD_REVISION_FIRMWARE_BASIC_BOOT // Revision
    },
    0,  // Reserved
    //
    // These values will be updated at runtime.
    //
    0,  // ResetEnd
    0,  // OsLoaderLoadImageStart
    0,  // OsLoaderStartImageStart
    0,  // ExitBootServicesEntry
    0   // ExitBootServicesExit
  }
};

S3_PERFORMANCE_TABLE        mS3PerformanceTableTemplate = {
  {
    EFI_ACPI_5_0_FPDT_S3_PERFORMANCE_TABLE_SIGNATURE,
    sizeof (S3_PERFORMANCE_TABLE)
  },
  {
    {
      EFI_ACPI_5_0_FPDT_RUNTIME_RECORD_TYPE_S3_RESUME,     // Type
      sizeof (EFI_ACPI_5_0_FPDT_S3_RESUME_RECORD),         // Length
      EFI_ACPI_5_0_FPDT_RUNTIME_RECORD_REVISION_S3_RESUME  // Revision
    },
    //
    // These values will be updated at runtime.
    //
    0,  // ResumeCount
    0,  // FullResume
    0   // AverageResume
  }
};

SBL_PERFORMANCE_TABLE       mSblPerfTableTemplate = {
  {
    SBL_PERFORMANCE_TABLE_SIGNATURE,
    sizeof (SBL_PERFORMANCE_TABLE)
  },
  {
    {
      SBL_PERFORMACE_TABLE_TYPE,        // Type
      sizeof (SBL_PERFORMANCE_RECORD),  // Length
      SBL_PERFORMACE_TABLE_REVISION     // Revision
    },
    0,  // Reserved
    //
    // These values will be updated during the execution of SBL
    //
    0,  // Stage 1 execution time in nanoseconds
    0,  // Stage 2 execution time in nanoseconds
    0   // OS Loader execution time in nanoseconds
  }
};


/**
  Update boot performance record table.

  @param[out] BootPerfTable     Pointer of boot performance record table.

  @retval EFI_SUCCESS           Update the boot performance table successfully.
  @retval Others                Failed to update the table.
 **/
EFI_STATUS
UpdateFpdtBootTable (
  OUT BOOT_PERFORMANCE_TABLE          *BootPerfTable
  )
{
  BL_PERF_DATA                        *PerfData;
  UINT64                              TscValue;
  UINT64                              TimeInMs;

  // update ResetEnd (in ns)
  PerfData = GetPerfDataPtr();
  TscValue = PerfData->TimeStamp[0];
  ((UINT16 *)&TscValue)[3] = 0;
  TimeInMs  = DivU64x32 (TscValue, PerfData->FreqKhz);
  BootPerfTable->BasicBoot.ResetEnd = MultU64x32 (TimeInMs, 1000000);

  return  EFI_SUCCESS;
}


/**
  Get FPDT firmware performance table by searching ACPI table

  @param[in]  AcpiTableBase    ACPI table base address

  @retval  FPDT firmware performance table address, return value NULL means not found.
**/
FIRMWARE_PERFORMANCE_TABLE *
GetFpdtTable (
  IN  UINT32                                   AcpiTableBase
  )
{
  EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                  *Rsdt;
  EFI_ACPI_COMMON_HEADER                       *Hdr;
  UINT32                                       *RsdtEntry;
  UINT32                                       NumEntries;
  FIRMWARE_PERFORMANCE_TABLE                   *Fpdt;
  UINT8                                        Index;
  BOOT_PERFORMANCE_TABLE                       *BootTable;

  Rsdp = (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER *)(UINTN)AcpiTableBase;
  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Rsdp->RsdtAddress;

  NumEntries = (Rsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT32);
  RsdtEntry  = (UINT32 *) ((UINT8 *)Rsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  for (Index = 0; Index < NumEntries; Index++) {
    Hdr = (EFI_ACPI_COMMON_HEADER *) (UINTN) RsdtEntry[Index];
    if (Hdr->Signature == EFI_ACPI_5_0_FIRMWARE_PERFORMANCE_DATA_TABLE_SIGNATURE) {
      Fpdt = (FIRMWARE_PERFORMANCE_TABLE *) Hdr;
      BootTable = (BOOT_PERFORMANCE_TABLE *)(UINTN)Fpdt->BootPointerRecord.BootPerformanceTablePointer;
      DEBUG ((DEBUG_VERBOSE, "FPDT: ResetEnd                = %ld\n", BootTable->BasicBoot.ResetEnd));
      DEBUG ((DEBUG_VERBOSE, "FPDT: OsLoaderLoadImageStart  = %ld\n", BootTable->BasicBoot.OsLoaderLoadImageStart));
      DEBUG ((DEBUG_VERBOSE, "FPDT: OsLoaderStartImageStart = %ld\n", BootTable->BasicBoot.OsLoaderStartImageStart));
      DEBUG ((DEBUG_VERBOSE, "FPDT: ExitBootServicesEntry   = %ld\n", BootTable->BasicBoot.ExitBootServicesEntry));
      DEBUG ((DEBUG_VERBOSE, "FPDT: ExitBootServicesExit    = %ld\n", BootTable->BasicBoot.ExitBootServicesExit));

      return Fpdt;
    }
  }

  return NULL;
}


/**
  Update ACPI FPDT S3 performance record table.

  @param[in] AcpiTableBase     ACPI base address

  @retval EFI_SUCCESS          Update the S3 performance table successfully.
  @retval Others               Failed to update the table.
 **/
EFI_STATUS
EFIAPI
UpdateFpdtS3Table (
  IN  UINT32                          AcpiTableBase
  )
{
  S3_PERFORMANCE_TABLE                *S3PerfTable;
  BL_PERF_DATA                        *PerfData;
  UINT64                              TscValue;
  UINT64                              TimeInMs;
  UINT64                              TotalResumeTime;
  EFI_ACPI_5_0_FPDT_S3_RESUME_RECORD  *S3Resume;
  FIRMWARE_PERFORMANCE_TABLE          *Fpdt;

  Fpdt = GetFpdtTable (AcpiTableBase);
  if (Fpdt == NULL) {
    return EFI_NOT_FOUND;
  }

  S3PerfTable = (S3_PERFORMANCE_TABLE *)(UINTN)Fpdt->S3PointerRecord.S3PerformanceTablePointer;
  ASSERT ((S3PerfTable != NULL) && (S3PerfTable->Header.Signature == EFI_ACPI_5_0_FPDT_S3_PERFORMANCE_TABLE_SIGNATURE));

  // Get current time
  TscValue = ReadTimeStamp();
  PerfData = GetPerfDataPtr();
  TimeInMs = DivU64x32 (TscValue, PerfData->FreqKhz);

  // Update S3 performance data
  S3Resume                = &S3PerfTable->S3Resume;
  S3Resume->FullResume    = MultU64x32 (TimeInMs, 1000000);
  TotalResumeTime         = MultU64x32 (S3Resume->AverageResume, S3Resume->ResumeCount);
  TotalResumeTime        += S3Resume->FullResume;
  S3Resume->ResumeCount++;
  S3Resume->AverageResume = DivU64x32 (TotalResumeTime, S3Resume->ResumeCount);

  DEBUG ((DEBUG_VERBOSE, "FPDT: S3Resume->AverageResume = %ld\n", S3Resume->AverageResume));
  DEBUG ((DEBUG_VERBOSE, "FPDT: S3Resume->ResumeCount   = %d\n",  S3Resume->ResumeCount));
  DEBUG ((DEBUG_VERBOSE, "FPDT: S3Resume->FullResume    = %ld\n", S3Resume->FullResume));

  return  EFI_SUCCESS;
}


/**
  This function updates SBL performance table in FPDT

  @retval  EFI_SUCCESS if operation is successful, EFI_NOT_FOUND if
           performance HOB is not found

**/
EFI_STATUS
EFIAPI
UpdateFpdtSblTable (
  VOID
  )
{
  UINT32                            PerfIdx;
  UINT64                            PerfTsc;
  UINT32                            Time;
  UINT64                            ResetVectorTime;
  UINT16                            Id;
  BL_PERF_DATA                      *PerfData;
  FIRMWARE_PERFORMANCE_TABLE        *Fpdt;
  SBL_PERFORMANCE_TABLE             *SblPerfTable;

  Fpdt = GetFpdtTable (PcdGet32 (PcdAcpiTablesRsdp));
  if (Fpdt == NULL) {
    return EFI_NOT_FOUND;
  }

  SblPerfTable = (SBL_PERFORMANCE_TABLE *)(UINTN)Fpdt->SblPerfPointerRecord.SblPerfTablePointer;
  ASSERT ((SblPerfTable != NULL) && (SblPerfTable->Header.Signature == SBL_PERFORMANCE_TABLE_SIGNATURE));

  PerfData = GetPerfDataPtr();
  if (PerfData == NULL) {
    return EFI_NOT_FOUND;
  }

  // Grab relevant performance metrics
  ResetVectorTime = 0;
  for (PerfIdx = 0; PerfIdx < MAX_TS_NUM; PerfIdx++) {
    Id   = (RShiftU64 (PerfData->TimeStamp[PerfIdx], 48)) & 0xFFFF;
    switch (Id) {
    case 0x1000:  // Reset vector time
    PerfTsc = PerfData->TimeStamp[PerfIdx] & 0x0000FFFFFFFFFFFFULL;
    Time = (UINT32)DivU64x32 (PerfTsc, PerfData->FreqKhz);
    ResetVectorTime = Time;
    break;
    case 0x3000:  // Stage 1 done (Stage 2 entry)
      PerfTsc = PerfData->TimeStamp[PerfIdx] & 0x0000FFFFFFFFFFFFULL;
      Time = (UINT32)DivU64x32 (PerfTsc, PerfData->FreqKhz);
      SblPerfTable->SblPerfRecord.Stage1Time= Time;
    break;
    case 0x31F0:  // Stage 2 done (End of stage 2)
      PerfTsc = PerfData->TimeStamp[PerfIdx] & 0x0000FFFFFFFFFFFFULL;
      Time = (UINT32)DivU64x32 (PerfTsc, PerfData->FreqKhz);
      SblPerfTable->SblPerfRecord.Stage2Time = Time;
    break;
    default:
    break;
    }

    // 0x31F0 is the last measure point we can get from BL_PERF_DATA
    // Get the current measure point by reading the timestamp
    // to get an accurate timing measurement for OsLoader. Then,
    // calculate deltas, convert timings to nanoseconds, and break
    if (Id == 0x31F0) {
      PerfTsc = ReadTimeStamp();
      Time = (UINT32)DivU64x32 (PerfTsc, PerfData->FreqKhz);
      SblPerfTable->SblPerfRecord.OsLoaderTime = Time;

      SblPerfTable->SblPerfRecord.OsLoaderTime -= SblPerfTable->SblPerfRecord.Stage2Time;
      SblPerfTable->SblPerfRecord.Stage2Time -= SblPerfTable->SblPerfRecord.Stage1Time;
      SblPerfTable->SblPerfRecord.Stage1Time -= ResetVectorTime;

      SblPerfTable->SblPerfRecord.Stage1Time = MultU64x32(SblPerfTable->SblPerfRecord.Stage1Time, 1000000);
      SblPerfTable->SblPerfRecord.Stage2Time = MultU64x32(SblPerfTable->SblPerfRecord.Stage2Time, 1000000);
      SblPerfTable->SblPerfRecord.OsLoaderTime = MultU64x32(SblPerfTable->SblPerfRecord.OsLoaderTime, 1000000);
      break;
    }
  }

  DEBUG((DEBUG_INFO, "Updated SBL Performance Table: S1 = %ldns, S2 = %ldns, OSL = %ldns\n",
        SblPerfTable->SblPerfRecord.Stage1Time, SblPerfTable->SblPerfRecord.Stage2Time,
        SblPerfTable->SblPerfRecord.OsLoaderTime));

  return EFI_SUCCESS;
}

/**
  Update Firmware Performance Data Table (FPDT).

  @param[in] Table          Pointer of ACPI FPDT Table.
  @param[out] ExtraSize     Extra size the table needed.

  @retval EFI_SUCCESS       Update ACPI FPDT table successfully.
  @retval Others            Failed to update FPDT table.
 **/
EFI_STATUS
UpdateFpdt (
  IN  UINT8                           *Table,
  OUT UINT32                          *ExtraSize
  )
{
  FIRMWARE_PERFORMANCE_TABLE          *Fpdt;
  UINT8                               BootMode;
  BOOT_PERFORMANCE_TABLE              *BootPerfTable;
  S3_PERFORMANCE_TABLE                *S3PerfTable;
  SBL_PERFORMANCE_TABLE               *SblPerfTable;

  if ( Table == NULL) {
    DEBUG((DEBUG_WARN, "TABLE is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  BootMode = GetBootMode ();
  if (BootMode != BOOT_ON_S3_RESUME) {
    Fpdt          = (FIRMWARE_PERFORMANCE_TABLE *)Table;
    BootPerfTable = (BOOT_PERFORMANCE_TABLE *) (Fpdt + 1);
    S3PerfTable   = (S3_PERFORMANCE_TABLE *) (BootPerfTable + 1);
    SblPerfTable  = (SBL_PERFORMANCE_TABLE *) (S3PerfTable + 1);

    Fpdt->BootPointerRecord.BootPerformanceTablePointer = (UINT64) (UINTN) BootPerfTable;
    Fpdt->S3PointerRecord.S3PerformanceTablePointer     = (UINT64) (UINTN) S3PerfTable;
    Fpdt->SblPerfPointerRecord.SblPerfTablePointer      = (UINT64) (UINTN) SblPerfTable;
    CopyMem (BootPerfTable, &mBootPerformanceTableTemplate, sizeof (mBootPerformanceTableTemplate));
    CopyMem (S3PerfTable, &mS3PerformanceTableTemplate, sizeof (mS3PerformanceTableTemplate));
    CopyMem (SblPerfTable, &mSblPerfTableTemplate, sizeof (mSblPerfTableTemplate));
    UpdateFpdtBootTable (BootPerfTable);

    if (ExtraSize != NULL) {
      *ExtraSize = (UINT32)((UINT8 *) (SblPerfTable + 1) - Table - Fpdt->Header.Length);
    }

  }

  return  EFI_SUCCESS;
}

