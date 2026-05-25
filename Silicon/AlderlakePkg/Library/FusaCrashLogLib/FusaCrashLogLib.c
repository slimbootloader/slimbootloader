/** @file
  This file contains the CrashLog feature support library.

  Copyright (c) 2022 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "FusaCrashLogLibPrivate.h"

CONST EFI_GUID mCrashLog_RecordId_Guid     = CRASHLOG_RECORDID_GUID;
CONST EFI_GUID mTelemetry_RecordId_Guid    = TELEMETRY_RECORDID_GUID;
CONST EFI_GUID mTrace_RecordId_Guid        = TRACE_RECORDID_GUID;

/**
  Get Cpu Crash log from HOB and use data to fill FW boot error records for BERT.

  @param[out] MainLogDestination       Destination FW error record for main log.
  @param[out] TelemetryDestination     Destination FW error record for Telemetry.
  @param[out] TraceDestination         Destination FW error record for Trace log.

  @retval EFI_SUCCESS             The function completes successfully

**/
EFI_STATUS
GetCpuCrashLog (
  OUT  UINT8          *MainLogDestination,
  OUT  UINT8          *TelemetryDestination,
  OUT  UINT8          *TraceDestination
  )
{
  UINT8               *CrashLogAddr = NULL;
  UINT32              CrashLogSize;
  EFI_HOB_GUID_TYPE   *RawHob;
  CPU_CRASHLOG_HOB    *CpuCrashLogData;
  UINTN               MainLogDataBuffer;
  UINTN               TelemetryDataBuffer;
  UINTN               TraceDataBuffer;
  UINT32              MainLogSize;
  UINT32              TelemetrySize;
  UINT32              TraceSize;
  UINT32              CpuCrashLogSize;
  VOID                *FspHobList;

  FspHobList = GetFspHobListPtr();
  if (FspHobList == NULL) {
    return EFI_NOT_FOUND;
  }
  RawHob = GetNextGuidHob (&gCpuCrashLogDataBufferHobGuid, FspHobList);
  if (RawHob == NULL) {
    DEBUG((DEBUG_VERBOSE, "CrashLog Data not present\n"));
    return EFI_NOT_FOUND;
  }
  CpuCrashLogData = (CPU_CRASHLOG_HOB*)(RawHob+1);

  DEBUG((DEBUG_VERBOSE, "CpuCrashLogData : %p\n", (VOID *)CpuCrashLogData));
  MainLogDataBuffer = (UINTN)CpuCrashLogData->Main.AllocateAddress;
  TelemetryDataBuffer = (UINTN)CpuCrashLogData->Telemetry.AllocateAddress;
  TraceDataBuffer = (UINTN)CpuCrashLogData->Trace.AllocateAddress;
  MainLogSize = CpuCrashLogData->Main.Size;
  TelemetrySize = CpuCrashLogData->Telemetry.Size;
  TraceSize = CpuCrashLogData->Trace.Size;
  CpuCrashLogSize = MainLogSize + TelemetrySize + TraceSize;

  if (CpuCrashLogSize > 0) {
    DEBUG ((DEBUG_VERBOSE, "Cpu CrashLog is present.\n"));
  } else {
    return EFI_NOT_FOUND;
  }

  if (((MainLogSize > 0) && (MainLogDestination == NULL)) ||
      ((TelemetrySize > 0) && (TelemetryDestination == NULL)) ||
      ((TraceSize > 0) && (TraceDestination == NULL))) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_VERBOSE, "Cpu Crash Data Collection\n"));

  if (MainLogSize > 0) {
    CrashLogAddr = (UINT8 *) (UINTN) MainLogDataBuffer;
    DEBUG ((DEBUG_VERBOSE, "Value at MainCrashLogAddress %p = 0x%x \n ", (VOID *)CrashLogAddr, (UINT32) *CrashLogAddr));
    CopyMem (MainLogDestination, CrashLogAddr, MainLogSize);
    for (CrashLogSize = 0; CrashLogSize < MIN (MainLogSize, CRASHLOG_SIZE_DEBUG_PURPOSE); CrashLogSize++) {
      DEBUG ((DEBUG_VERBOSE, "Main CrashData[%d] = 0x%x\n", CrashLogSize, MainLogDestination[CrashLogSize]));
    }
    DEBUG ((DEBUG_VERBOSE, "Copied Main CrashLog Size = 0x%x\n", MainLogSize));
  }

  if (TelemetrySize > 0) {
    CrashLogAddr = (UINT8 *) (UINTN) TelemetryDataBuffer;
    DEBUG ((DEBUG_VERBOSE, "Value at TelemetryCrashLogAddress %p = 0x%x \n ", (VOID *)CrashLogAddr, (UINT32) *CrashLogAddr));
    CopyMem (TelemetryDestination, CrashLogAddr, TelemetrySize);
    for (CrashLogSize = 0; CrashLogSize < MIN (TelemetrySize, CRASHLOG_SIZE_DEBUG_PURPOSE); CrashLogSize++) {
      DEBUG ((DEBUG_VERBOSE, "Telemetry CrashData[%d] = 0x%x\n", CrashLogSize, TelemetryDestination[CrashLogSize]));
    }
    DEBUG ((DEBUG_VERBOSE, "Copied Telemetry CrashLog Size = 0x%x\n", TelemetrySize));
  }

  if (TraceSize > 0) {
    CrashLogAddr = (UINT8 *) (UINTN) TraceDataBuffer;
    DEBUG ((DEBUG_VERBOSE, "Value at TraceCrashLogAddress %p = 0x%x \n ", (VOID *)CrashLogAddr, (UINT32) *CrashLogAddr));
    CopyMem (TraceDestination, CrashLogAddr, TraceSize);
    for (CrashLogSize = 0; CrashLogSize < MIN (TraceSize, CRASHLOG_SIZE_DEBUG_PURPOSE); CrashLogSize++) {
      DEBUG ((DEBUG_VERBOSE, "Trace CrashData[%d] = 0x%x\n", CrashLogSize, TraceDestination[CrashLogSize]));
    }
    DEBUG ((DEBUG_VERBOSE, "Copied Trace CrashLog Size = 0x%x\n", TraceSize));
  }

  return EFI_SUCCESS;
}

/**

  Add a new Firmware CrashLog Entry to existing Error Status Block.

  @param[in] ErrStsBlk                          BERT GENERIC_ERROR_STATUS instance.
  @param[in out] FirmwareGenericErrorAddr       Pointer to this FirmwareGenericError entry address, updated on return.
  @param[in out] FirmwareCrashLogPayloadAddr    Pointer to Firmware CrashLog Entry payload address, updated on return.
  @param[in] EntrySize                          Firmware CrashLog Entry payload size.
  @param[in] RecordIdGuid                       Firmware CrashLog Record ID GUID.

**/

VOID
AddFirmwareCrashLogEntry (
  IN     EFI_ACPI_6_4_GENERIC_ERROR_STATUS_STRUCTURE    *ErrStsBlk,
  IN OUT FIRMWARE_GENERIC_ERROR                         **FirmwareGenericErrorAddr,
  IN OUT UINT8                                          **FirmwareCrashLogPayloadAddr,
  IN     UINT32                                         EntrySize,
  IN     EFI_GUID                                       RecordIdGuid
  )
{
  FIRMWARE_GENERIC_ERROR  *FirmwareGenericError = NULL;

  FirmwareGenericError = (FIRMWARE_GENERIC_ERROR*)((UINT8*)(void*)ErrStsBlk + ErrStsBlk->DataLength + sizeof (EFI_ACPI_6_4_GENERIC_ERROR_STATUS_STRUCTURE));
  ZeroMem (FirmwareGenericError, sizeof (FIRMWARE_GENERIC_ERROR));

  CopyMem (&FirmwareGenericError->GenericErrorDataEntry.SectionType, &gEfiFirmwareErrorSectionGuid, sizeof (EFI_GUID));
  FirmwareGenericError->GenericErrorDataEntry.ErrorSeverity = EFI_ACPI_6_4_ERROR_SEVERITY_FATAL;
  FirmwareGenericError->GenericErrorDataEntry.Revision = EFI_ACPI_6_4_GENERIC_ERROR_DATA_ENTRY_REVISION;
  FirmwareGenericError->GenericErrorDataEntry.ValidationBits = 0;
  FirmwareGenericError->GenericErrorDataEntry.ErrorDataLength = EntrySize + sizeof (EFI_FIRMWARE_ERROR_DATA);
  FirmwareGenericError->EfiFirmwareErrorData.ErrorType = 2; //FW CrashLog Error Record
  FirmwareGenericError->EfiFirmwareErrorData.Revision = EFI_2_7_FIRMWARE_ERROR_RECORD_REVISION;
  FirmwareGenericError->EfiFirmwareErrorData.RecordIdGuid = RecordIdGuid;

  ErrStsBlk->DataLength += FirmwareGenericError->GenericErrorDataEntry.ErrorDataLength + sizeof (EFI_ACPI_6_4_GENERIC_ERROR_DATA_ENTRY_STRUCTURE);
  ErrStsBlk->BlockStatus.ErrorDataEntryCount++;

  *FirmwareGenericErrorAddr = FirmwareGenericError;
  *FirmwareCrashLogPayloadAddr = (UINT8*)(void*) FirmwareGenericError + sizeof (FIRMWARE_GENERIC_ERROR);
}

/**
  Get CPU Crash Log, and initialize the APEI BERT GENERIC_ERROR_STATUS structure

  @param[in out] ErrStsBlk            BERT GENERIC_ERROR_STATUS instance.
  @param[in out] Bert                 ACPI BERT table containing error record.

  @retval EFI_SUCCESS             The function completes successfully
**/
EFI_STATUS
GenFwBootErrorlog (
  IN OUT EFI_ACPI_6_4_GENERIC_ERROR_STATUS_STRUCTURE    *ErrStsBlk,
  IN OUT EFI_ACPI_6_4_BOOT_ERROR_RECORD_TABLE_HEADER    *Bert
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  FIRMWARE_GENERIC_ERROR        *MainFirmwareGenericError = NULL;
  FIRMWARE_GENERIC_ERROR        *TelemetryFirmwareGenericError = NULL;
  FIRMWARE_GENERIC_ERROR        *TraceFirmwareGenericError = NULL;
  UINT8                         *MainDest = NULL;
  UINT8                         *TelemetryDest = NULL;
  UINT8                         *TraceDest = NULL;
  EFI_HOB_GUID_TYPE             *RawHob;
  CPU_CRASHLOG_HOB              *CpuCrashLogData;
  UINT32                        MainLogSize;
  UINT32                        TelemetrySize;
  UINT32                        TraceSize;
  VOID                          *FspHobList;

  if (ErrStsBlk == NULL) {
    return EFI_NOT_FOUND;
  }

  FspHobList = GetFspHobListPtr();
  if (FspHobList == NULL) {
    return EFI_NOT_FOUND;
  }
  RawHob = GetNextGuidHob (&gCpuCrashLogDataBufferHobGuid, FspHobList);
  if (RawHob == NULL) {
    DEBUG((DEBUG_VERBOSE, "CrashLog Data not present\n"));
    return EFI_NOT_FOUND;
  }
  CpuCrashLogData = (CPU_CRASHLOG_HOB*)(RawHob+1);

  MainLogSize = CpuCrashLogData->Main.Size;
  TelemetrySize = CpuCrashLogData->Telemetry.Size;
  TraceSize = CpuCrashLogData->Trace.Size;

  ErrStsBlk->RawDataOffset = 0;
  ErrStsBlk->RawDataLength = 0;
  ErrStsBlk->ErrorSeverity = EFI_ACPI_6_4_ERROR_SEVERITY_FATAL;
  ErrStsBlk->BlockStatus.ErrorDataEntryCount = 0;
  ErrStsBlk->DataLength = 0;

  //
  // Getting Cpu Crashlog and adding entry
  //
  if (MainLogSize) {
    AddFirmwareCrashLogEntry (ErrStsBlk, &MainFirmwareGenericError, &MainDest, MainLogSize, mCrashLog_RecordId_Guid);
    DEBUG ((DEBUG_VERBOSE, "ErrStsBlk = %p, MainFirmwareGenericError = %p, MainDest = %p, MainLogSize = 0x%x\n", (VOID *)ErrStsBlk, (VOID *)MainFirmwareGenericError, (VOID *)MainDest, MainLogSize));

    if (TelemetrySize) {
      AddFirmwareCrashLogEntry (ErrStsBlk, &TelemetryFirmwareGenericError, &TelemetryDest, TelemetrySize, mTelemetry_RecordId_Guid);
      DEBUG ((DEBUG_VERBOSE, "ErrStsBlk = %p, TelemetryFirmwareGenericError = %p, TelemetryDest = %p, TelemetrySize = 0x%x\n", (VOID *)ErrStsBlk, (VOID *)TelemetryFirmwareGenericError, (VOID *)TelemetryDest, TelemetrySize));
    }

    if (TraceSize) {
      AddFirmwareCrashLogEntry (ErrStsBlk, &TraceFirmwareGenericError, &TraceDest, TraceSize, mTrace_RecordId_Guid);
      DEBUG ((DEBUG_VERBOSE, "ErrStsBlk = %p, TraceFirmwareGenericError = %p, TraceDest = %p, TraceSize = 0x%x\n", (VOID *)ErrStsBlk, (VOID *)TraceFirmwareGenericError, (VOID *)TraceDest, TraceSize));
    }
  }

  Status = GetCpuCrashLog (MainDest, TelemetryDest, TraceDest);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "CPU CrashLog data are not collected\n"));
  }

  ErrStsBlk->BlockStatus.MultipleUncorrectableErrors = 1;
  ErrStsBlk->BlockStatus.UncorrectableErrorValid = 1;

  Bert->BootErrorRegionLength = ErrStsBlk->DataLength + sizeof (EFI_ACPI_6_4_GENERIC_ERROR_STATUS_STRUCTURE);

  DEBUG ((DEBUG_VERBOSE, " ErrStsBlk->DataLength = 0x%x\n", ErrStsBlk->DataLength));
  DEBUG ((DEBUG_VERBOSE, " ErrorStatusBlock Size = 0x%x\n", Bert->BootErrorRegionLength));

  return EFI_SUCCESS;
}

/**
  Log firmware boot error log  in APEI BERT.

  @param[in out] BootErrorTable       APEI BERT address.

  @retval EFI_SUCCESS             The function completes successfully
**/
EFI_STATUS
LogFwBootErrorlog (
  IN OUT  EFI_ACPI_6_4_BOOT_ERROR_RECORD_TABLE_HEADER    *BootErrorTable
  )
{
  EFI_ACPI_6_4_GENERIC_ERROR_STATUS_STRUCTURE       *ErrStsBlk;

  if (BootErrorTable == NULL) {
    return EFI_NOT_FOUND;
  }

  ErrStsBlk = (EFI_ACPI_6_4_GENERIC_ERROR_STATUS_STRUCTURE *) (UINTN) BootErrorTable->BootErrorRegion;
  if ((ErrStsBlk->DataLength + sizeof (EFI_ACPI_6_4_GENERIC_ERROR_STATUS_STRUCTURE)) > BootErrorTable->BootErrorRegionLength) {
    return EFI_OUT_OF_RESOURCES;
  }
  return GenFwBootErrorlog (ErrStsBlk, BootErrorTable);
}

/**
  Update BERT Table with Crash Log Telemetry SRAM data

  @param[in out] Bert                   Pointer to BERT table to update.

  @retval  EFI_SUCCESS:              Driver initialized successfully
           EFI_LOAD_ERROR:           Failed to Initialize or to Load the driver
           EFI_OUT_OF_RESOURCES:     Could not allocate needed resources
**/
EFI_STATUS
EFIAPI
UpdateCrashLogBertTable (
  IN OUT  EFI_ACPI_6_4_BOOT_ERROR_RECORD_TABLE_HEADER *Bert
  )
{
  EFI_STATUS                  Status;
  UINT32                      BootErrorRegionSize;
  CPU_CRASHLOG_HOB            *CpuCrashLogData;
  EFI_HOB_GUID_TYPE           *RawHob;
  BOOLEAN                     CpuCrashLogPresent;
  UINT32                      CrashLogCount = 0;
  UINTN                       MainLogDataBuffer;
  UINTN                       TelemetryDataBuffer;
  UINTN                       TraceDataBuffer;
  UINT32                      MainLogSize;
  UINT32                      TelemetrySize;
  UINT32                      TraceSize;
  UINT32                      CpuCrashLogSize;
  VOID                        *FspHobList;

  DEBUG ((DEBUG_VERBOSE, "CrashLog Entry Point\n"));

  FspHobList = GetFspHobListPtr();
  if (FspHobList == NULL) {
    return EFI_NOT_FOUND;
  }
  RawHob = GetNextGuidHob (&gCpuCrashLogDataBufferHobGuid, FspHobList);
  if (RawHob == NULL) {
    DEBUG((DEBUG_VERBOSE, "CrashLog Data not present\n"));
    return EFI_NOT_FOUND;
  }
  CpuCrashLogData = (CPU_CRASHLOG_HOB*)(RawHob+1);

  MainLogDataBuffer = (UINTN)CpuCrashLogData->Main.AllocateAddress;
  TelemetryDataBuffer = (UINTN)CpuCrashLogData->Telemetry.AllocateAddress;
  TraceDataBuffer = (UINTN)CpuCrashLogData->Trace.AllocateAddress;
  MainLogSize = CpuCrashLogData->Main.Size;
  TelemetrySize = CpuCrashLogData->Telemetry.Size;
  TraceSize = CpuCrashLogData->Trace.Size;
  CpuCrashLogSize = MainLogSize + TelemetrySize + TraceSize;
  DEBUG ((DEBUG_VERBOSE, "MainLogDataBuffer = %p\n", (VOID *)MainLogDataBuffer));
  DEBUG ((DEBUG_VERBOSE, "MainLogSize = 0x%x\n", MainLogSize));
  DEBUG ((DEBUG_VERBOSE, "TelemetryDataBuffer = %p\n", (VOID *)TelemetryDataBuffer));
  DEBUG ((DEBUG_VERBOSE, "TelemetrySize = 0x%x\n", TelemetrySize));
  DEBUG ((DEBUG_VERBOSE, "TraceDataBuffer = %p\n", (VOID *)TraceDataBuffer));
  DEBUG ((DEBUG_VERBOSE, "TraceSize = 0x%x\n", TraceSize));
  DEBUG ((DEBUG_VERBOSE, "CpuCrashLogSize = 0x%x\n", CpuCrashLogSize));

  if (MainLogSize) {
    DEBUG ((DEBUG_ERROR, "Cpu CrashLog data is present\n"));
    CpuCrashLogPresent = TRUE;
    CrashLogCount++;
    if (TelemetrySize) {
      CrashLogCount++;
    }
    if (TraceSize) {
      CrashLogCount++;
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Cpu CrashLog data is not present\n"));
    CpuCrashLogPresent = FALSE;
  }

  if ((CpuCrashLogPresent == FALSE)) {
    DEBUG ((DEBUG_ERROR, "CrashLog is not present. Skip BERT creation \n"));
    return EFI_NOT_READY;
  }

  DEBUG ((DEBUG_VERBOSE, "BERT Init\n"));

  //
  // Update the OEMID, OEM revision, OEM TABLE ID, Creator ID and Creator revision
  //
  CopyMem(&(Bert->Header.OemId), EFI_ACPI_BERT_OEM_ID, 6);

  Bert->Header.OemTableId = EFI_ACPI_BERT_OEM_TABLE_ID;
  Bert->Header.OemRevision = EFI_ACPI_BERT_OEM_REVISION;
  Bert->Header.CreatorId = EFI_ACPI_BERT_CREATOR_ID;
  Bert->Header.CreatorRevision = EFI_ACPI_BERT_CREATOR_REVISION;

  //
  // Allocate memory space Error status blocks
  // Allocate memory space for Errorlog Address range, Fill EFI_ACPI_6_4_BOOT_ERROR_RECORD_TABLE_HEADER(Bert) structure
  //
  BootErrorRegionSize = sizeof (EFI_ACPI_6_4_GENERIC_ERROR_STATUS_STRUCTURE) + sizeof (FIRMWARE_GENERIC_ERROR) * CrashLogCount + CpuCrashLogSize;
  Bert->BootErrorRegion = (UINT64)(UINTN)AllocateZeroPool ((UINTN) BootErrorRegionSize);

  DEBUG ((DEBUG_VERBOSE, "Bert->BootErrorRegion     = %x\n", (UINTN)Bert->BootErrorRegion));
  DEBUG ((DEBUG_VERBOSE, "Bert->BootErrorRegionSize = %x\n", BootErrorRegionSize));

  if (Bert->BootErrorRegion == 0) {
    DEBUG ((DEBUG_ERROR, "Unable to Allocate Memory for Boot Error Record Data\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Bert->BootErrorRegionLength = BootErrorRegionSize;

  Status = LogFwBootErrorlog (Bert);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "No Cpu error log found\n"));
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_VERBOSE, "CrashLog Exit \n"));
  return Status;
}
