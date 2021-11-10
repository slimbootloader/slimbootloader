/** @file

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
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
  Get FPDT S3 performance table by searching ACPI table

  @param[in]  AcpiTableBase    ACPI table base address

  @retval S3 performance table address     Value 0 means not found.
**/
UINTN
GetFpdtS3Table (
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

      return (UINTN)Fpdt->S3PointerRecord.S3PerformanceTablePointer;
    }
  }

  return 0;
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

  S3PerfTable = (S3_PERFORMANCE_TABLE *)GetFpdtS3Table (AcpiTableBase);
  if (S3PerfTable == NULL) {
    return EFI_NOT_FOUND;
  }

  ASSERT (S3PerfTable->Header.Signature == EFI_ACPI_5_0_FPDT_S3_PERFORMANCE_TABLE_SIGNATURE);

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

  if ( Table == NULL) {
    DEBUG((DEBUG_WARN, "TABLE is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  BootMode = GetBootMode ();
  if (BootMode != BOOT_ON_S3_RESUME) {
    Fpdt          = (FIRMWARE_PERFORMANCE_TABLE *)Table;
    BootPerfTable = (BOOT_PERFORMANCE_TABLE *) (Fpdt + 1);
    S3PerfTable   = (S3_PERFORMANCE_TABLE *) (BootPerfTable + 1);

    Fpdt->BootPointerRecord.BootPerformanceTablePointer = (UINT64) (UINTN) BootPerfTable;
    Fpdt->S3PointerRecord.S3PerformanceTablePointer     = (UINT64) (UINTN) S3PerfTable;
    CopyMem (BootPerfTable, &mBootPerformanceTableTemplate, sizeof (mBootPerformanceTableTemplate));
    CopyMem (S3PerfTable, &mS3PerformanceTableTemplate, sizeof (mS3PerformanceTableTemplate));
    UpdateFpdtBootTable (BootPerfTable);

    if (ExtraSize != NULL) {
      *ExtraSize = (UINT32)((UINT8 *) (S3PerfTable + 1) - Table - Fpdt->Header.Length);
    }

  }

  return  EFI_SUCCESS;
}

