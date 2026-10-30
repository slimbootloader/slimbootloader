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
#include "AcpiInitLibInternal.h"

CONST FIRMWARE_PERFORMANCE_TABLES mFpdtTemplate = {
  //
  // FPDT Header (per ACPI spec, Length only covers this portion)
  //
  {
    {
      EFI_ACPI_5_0_FIRMWARE_PERFORMANCE_DATA_TABLE_SIGNATURE,
      sizeof (FIRMWARE_PERFORMANCE_TABLE),
      EFI_ACPI_5_0_FIRMWARE_PERFORMANCE_DATA_TABLE_REVISION,
      0x00,
      EFI_ACPI_OEM_ID,
      EFI_ACPI_OEM_TABLE_ID,
      EFI_ACPI_OEM_REVISION,
      EFI_ACPI_CREATOR_ID,
      EFI_ACPI_CREATOR_REVISION,
    },
    {
      {
        EFI_ACPI_5_0_FPDT_RECORD_TYPE_FIRMWARE_BASIC_BOOT_POINTER,
        sizeof (EFI_ACPI_5_0_FPDT_BOOT_PERFORMANCE_TABLE_POINTER_RECORD),
        EFI_ACPI_5_0_FPDT_RECORD_REVISION_FIRMWARE_BASIC_BOOT_POINTER
      },
      0, 0
    },
    {
      {
        EFI_ACPI_5_0_FPDT_RECORD_TYPE_S3_PERFORMANCE_TABLE_POINTER,
        sizeof (EFI_ACPI_5_0_FPDT_S3_PERFORMANCE_TABLE_POINTER_RECORD),
        EFI_ACPI_5_0_FPDT_RECORD_REVISION_S3_PERFORMANCE_TABLE_POINTER
      },
      0, 0
    },
    {
      {
        SBL_PERFORMACE_TABLE_TYPE,
        sizeof (SBL_PERFORMANCE_TABLE_POINTER_RECORD),
        SBL_PERFORMACE_TABLE_REVISION
      },
      0, 0
    }
  },
  //
  // Boot Performance Table (FBPT)
  //
  {
    { EFI_ACPI_5_0_FPDT_BOOT_PERFORMANCE_TABLE_SIGNATURE, sizeof (BOOT_PERFORMANCE_TABLE) },
    {
      {
        EFI_ACPI_5_0_FPDT_RUNTIME_RECORD_TYPE_FIRMWARE_BASIC_BOOT,
        sizeof (EFI_ACPI_5_0_FPDT_FIRMWARE_BASIC_BOOT_RECORD),
        EFI_ACPI_5_0_FPDT_RUNTIME_RECORD_REVISION_FIRMWARE_BASIC_BOOT
      },
      0, 0, 0, 0, 0, 0
    }
  },
  //
  // S3 Performance Table (S3PT)
  //
  {
    { EFI_ACPI_5_0_FPDT_S3_PERFORMANCE_TABLE_SIGNATURE, sizeof (S3_PERFORMANCE_TABLE) },
    {
      {
        EFI_ACPI_5_0_FPDT_RUNTIME_RECORD_TYPE_S3_RESUME,
        sizeof (EFI_ACPI_5_0_FPDT_S3_RESUME_RECORD),
        EFI_ACPI_5_0_FPDT_RUNTIME_RECORD_REVISION_S3_RESUME
      },
      0, 0, 0
    }
  }
};


/**
  Get the total ACPI reclaim size needed for all FPDT tables.

  This includes the fixed tables (FPDT, FBPT, S3PT) plus the
  dynamically-sized SBL Performance Table (SBLT) based on MAX_TS_NUM.

  @retval  Total size in bytes.
**/
UINT32
GetFpdtTablesSize (
  VOID
  )
{
  return (UINT32)(sizeof (FIRMWARE_PERFORMANCE_TABLES) + OFFSET_OF (SBL_PERFORMANCE_TABLE, TimeStamp) + MAX_TS_NUM * sizeof (UINT64));
}


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
  Get FPDT firmware performance table by searching ACPI table. During
  S3 Resume all pointers need to be bounds checked.

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

  if (!S3_PTR_CHK(Rsdt) ||
      !S3_PTR_CHK((VOID*)((UINTN)Rsdt + sizeof(EFI_ACPI_DESCRIPTION_HEADER) - 1)) ||
      (!IS_X64 && ((UINT64)(UINTN)Rsdt + Rsdt->Length - 1) > MAX_UINT32) ||
      !S3_PTR_CHK((VOID*)((UINTN)Rsdt + Rsdt->Length - 1)) ||
      Rsdt->Length < sizeof(EFI_ACPI_DESCRIPTION_HEADER))
  {
    return NULL;
  }

  NumEntries = (Rsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT32);
  RsdtEntry  = (UINT32 *) ((UINT8 *)Rsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  for (Index = 0; Index < NumEntries; Index++) {
    Hdr = (EFI_ACPI_COMMON_HEADER *) (UINTN) RsdtEntry[Index];
    if (!S3_PTR_CHK(Hdr) ||
        !S3_PTR_CHK((VOID*)((UINTN)Hdr + sizeof(EFI_ACPI_COMMON_HEADER) - 1)) ||
        (!IS_X64 && ((UINT64)(UINTN)Hdr + Hdr->Length - 1) > MAX_UINT32) ||
        !S3_PTR_CHK((VOID*)((UINTN)Hdr + Hdr->Length - 1)) ||
        Hdr->Length < sizeof(EFI_ACPI_COMMON_HEADER))
    {
      return NULL;
    }
    if (Hdr->Signature == EFI_ACPI_5_0_FIRMWARE_PERFORMANCE_DATA_TABLE_SIGNATURE) {
      Fpdt = (FIRMWARE_PERFORMANCE_TABLE *) Hdr;
      BootTable = (BOOT_PERFORMANCE_TABLE *)(UINTN)Fpdt->BootPointerRecord.BootPerformanceTablePointer;
      if (!S3_PTR_CHK(BootTable) ||
          (!IS_X64 && ((UINT64)(UINTN)BootTable + sizeof(BOOT_PERFORMANCE_TABLE) - 1) > MAX_UINT32) ||
          !S3_PTR_CHK((VOID*)((UINTN)BootTable + sizeof(BOOT_PERFORMANCE_TABLE) - 1))) {
        return NULL;
      }
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
  if (!S3_PTR_CHK(S3PerfTable) ||

      !S3_PTR_CHK((VOID*)((UINTN)S3PerfTable + sizeof(S3_PERFORMANCE_TABLE) - 1))) {
    return EFI_INVALID_PARAMETER;
  }
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

  AcpiPlatformChecksum ((UINT8 *)Fpdt, Fpdt->Header.Length);
  return  EFI_SUCCESS;
}


/**
  This function updates SBL performance table in FPDT

  @param[in] PerfData  Live performance data from the bootloader or payload.

  @retval EFI_SUCCESS            Timestamps published.
  @retval EFI_INVALID_PARAMETER  Performance data is missing.
  @retval EFI_NOT_FOUND          FPDT is unavailable.
  @retval EFI_UNSUPPORTED        SBLT format is not supported.

**/
EFI_STATUS
EFIAPI
UpdateFpdtSblTable (
  IN CONST BL_PERF_DATA  *PerfData
  )
{
  UINT32                            Count;
  UINT32                            Capacity;
  FIRMWARE_PERFORMANCE_TABLE        *Fpdt;
  SBL_PERFORMANCE_TABLE             *SblPerfTable;

  if (PerfData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Fpdt = GetFpdtTable (PcdGet32 (PcdAcpiTablesRsdp));
  if (Fpdt == NULL) {
    return EFI_NOT_FOUND;
  }

  if ((Fpdt->Header.Length < sizeof (FIRMWARE_PERFORMANCE_TABLE)) ||
      (Fpdt->SblPerfPointerRecord.Header.Revision != SBL_PERFORMACE_TABLE_REVISION)) {
    return EFI_UNSUPPORTED;
  }

  SblPerfTable = (SBL_PERFORMANCE_TABLE *)(UINTN)Fpdt->SblPerfPointerRecord.SblPerfTablePointer;
  if ((SblPerfTable == NULL) ||
      (SblPerfTable->Header.Signature != SBL_PERFORMANCE_TABLE_SIGNATURE) ||
      (SblPerfTable->Header.Length < OFFSET_OF (SBL_PERFORMANCE_TABLE, TimeStamp))) {
    return EFI_UNSUPPORTED;
  }

  Capacity = (SblPerfTable->Header.Length - OFFSET_OF (SBL_PERFORMANCE_TABLE, TimeStamp)) / sizeof (UINT64);
  Count = MIN (PerfData->PerfIndex, MIN (MAX_TS_NUM, Capacity));

  SblPerfTable->Count     = (UINT16)Count;
  SblPerfTable->Frequency = PerfData->FreqKhz;
  CopyMem (SblPerfTable->TimeStamp, PerfData->TimeStamp, Count * sizeof (UINT64));

  DEBUG ((DEBUG_INFO, "Updated SBL Performance Table: %d timestamps, Freq = %d KHz\n", Count, PerfData->FreqKhz));

  AcpiPlatformChecksum ((UINT8 *)Fpdt, Fpdt->Header.Length);
  return EFI_SUCCESS;
}

/**
  Update Firmware Performance Data Table (FPDT).

  @param[in] Table          Pointer of ACPI FPDT Table.

  @retval EFI_SUCCESS       Update ACPI FPDT table successfully.
  @retval Others            Failed to update FPDT table.
 **/
EFI_STATUS
UpdateFpdt (
  IN  UINT8                           *Table
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

    //
    // Copy sub-tables (FBPT, S3PT) from template and zero SBLT area.
    // The FPDT header was already copied by the common CopyMem path.
    //
    CopyMem (BootPerfTable, &mFpdtTemplate.Boot, sizeof (BOOT_PERFORMANCE_TABLE));
    CopyMem (S3PerfTable, &mFpdtTemplate.S3, sizeof (S3_PERFORMANCE_TABLE));
    ZeroMem (SblPerfTable, GetFpdtTablesSize () - sizeof (FIRMWARE_PERFORMANCE_TABLES));

    //
    // Initialize SBLT header dynamically.
    //
    SblPerfTable->Header.Signature = SBL_PERFORMANCE_TABLE_SIGNATURE;
    SblPerfTable->Header.Length    = GetFpdtTablesSize () - sizeof (FIRMWARE_PERFORMANCE_TABLES);

    Fpdt->BootPointerRecord.BootPerformanceTablePointer = (UINT64) (UINTN) BootPerfTable;
    Fpdt->S3PointerRecord.S3PerformanceTablePointer     = (UINT64) (UINTN) S3PerfTable;
    Fpdt->SblPerfPointerRecord.SblPerfTablePointer      = (UINT64) (UINTN) SblPerfTable;
    UpdateFpdtBootTable (BootPerfTable);
  }

  return  EFI_SUCCESS;
}

