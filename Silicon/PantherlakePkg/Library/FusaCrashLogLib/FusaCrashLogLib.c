/** @file
  This file contains the CrashLog feature support library.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "FusaCrashLogLibPrivate.h"

CRASHLOG_DXE_PRIVATE_DATA  mCrashlogData;

/**
  Calculate payload size in crash record.

  @param[in] HobData  Hob data of payload.

  @retval PayloadSize  Payload size.

**/
UINT32
GetPayloadSize (
  IN CRASHLOG_HOB_DATA  *HobData
  )
{
  UINT32                     Index;
  UINT32                     PayloadSize;
  CRASHLOG_HOB_RECORD_ENTRY  *CrashRecord;

  PayloadSize = 0;
  CrashRecord = (CRASHLOG_HOB_RECORD_ENTRY *)((UINTN)HobData + sizeof (CRASHLOG_HOB_DATA));

  for (Index = 0; Index < HobData->RecordCount; Index ++) {
    PayloadSize += CrashRecord[Index].Size;
  }

  return PayloadSize;
}

/**
  Get data from HOB and update private data structure

  @retval EFI_SUCCESS             The function completes successfully
  @retval EFI_NOT_FOUND           No crash log data found in HOB.

**/
EFI_STATUS
GetPayloadInfoFromHob (
  VOID
  )
{
  EFI_HOB_GUID_TYPE   *RawHob;
  CRASHLOG_HOB_DATA   *HobData;
  VOID                *FspHobList;
  UINT32              HobCount;
  UINT32              PayloadCount;
  UINT32              PayloadSize;

  HobCount     = 0;
  PayloadCount = 0;
  PayloadSize  = 0;

  FspHobList = GetFspHobListPtr();
  if (FspHobList == NULL) {
    return EFI_NOT_FOUND;
  }

  RawHob = GetNextGuidHob (&gCpuCrashLogDataBufferHobGuid, FspHobList);
  if (RawHob == NULL) {
    DEBUG((DEBUG_VERBOSE, "CrashLog Data not present\n"));
    return EFI_NOT_FOUND;
  }

  while (RawHob != NULL && HobCount < CrashlogAgentTypeMax) {
    HobData = (CRASHLOG_HOB_DATA *)GET_GUID_HOB_DATA (RawHob);
    mCrashlogData.HobDataPtr[HobCount] = HobData;

    PayloadCount += HobData->RecordCount;
    PayloadSize  += GetPayloadSize (HobData);
    DEBUG ((DEBUG_VERBOSE, " Find HOB %d, agent type= %d, current total payload count= %d, current total payload size= %d\n",
            HobCount, HobData->CrashlogAgent, PayloadCount, PayloadSize));

    RawHob = GetNextGuidHob (&gCpuCrashLogDataBufferHobGuid, GET_NEXT_HOB (RawHob));
    HobCount ++;
  }

  if (PayloadCount == 0 || PayloadSize == 0) {
    return EFI_NOT_FOUND;
  }

  mCrashlogData.HobCount     = HobCount;
  mCrashlogData.PayloadCount = PayloadCount;
  mCrashlogData.PayloadSize  = PayloadSize;

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
  UINT32                     HobIndex;
  UINT32                     Index;
  CRASHLOG_HOB_RECORD_ENTRY  *CrashRecord;
  FIRMWARE_GENERIC_ERROR     *GenericErrorData;
  UINT8                      *PayloadAddr;

  ErrStsBlk->RawDataOffset = 0;
  ErrStsBlk->RawDataLength = 0;
  ErrStsBlk->ErrorSeverity = EFI_ACPI_6_5_ERROR_SEVERITY_FATAL;
  ErrStsBlk->BlockStatus.ErrorDataEntryCount = 0;
  ErrStsBlk->DataLength = 0;

  for (HobIndex = 0; HobIndex < mCrashlogData.HobCount; HobIndex ++) {
    CrashRecord = (CRASHLOG_HOB_RECORD_ENTRY *)((UINTN)mCrashlogData.HobDataPtr[HobIndex] + sizeof (CRASHLOG_HOB_DATA));
    for (Index = 0; Index < mCrashlogData.HobDataPtr[HobIndex]->RecordCount; Index ++) {
      AddFirmwareCrashLogEntry (ErrStsBlk, &GenericErrorData, &PayloadAddr, CrashRecord[Index].Size, gCpuCrashLogDataBufferHobGuid);
      CopyMem (PayloadAddr, (VOID *)(UINTN)CrashRecord[Index].Address, CrashRecord[Index].Size);
      DEBUG ((DEBUG_VERBOSE, "ErrStsBlk = 0x%x, GenericErrorData = 0x%x, PayloadAddr = 0x%x, CrashRecordSize = 0x%x\n", ErrStsBlk, GenericErrorData, PayloadAddr, CrashRecord[Index].Size));
    }
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
  if (ErrStsBlk->DataLength >= BootErrorTable->BootErrorRegionLength) {
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

  DEBUG ((DEBUG_VERBOSE, "CrashLog Entry Point\n"));

  Status = GetPayloadInfoFromHob();
  if (EFI_ERROR(Status)) {
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
  // Allocate memort space for Errorlog Address range, Fill EFI_ACPI_6_4_BOOT_ERROR_RECORD_TABLE_HEADER(Bert) structure
  //
  BootErrorRegionSize = sizeof (EFI_ACPI_6_4_GENERIC_ERROR_STATUS_STRUCTURE) + sizeof (FIRMWARE_GENERIC_ERROR) * mCrashlogData.PayloadCount + mCrashlogData.PayloadSize;
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
