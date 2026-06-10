/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/DebugDataLib.h>
#include <Library/AcpiInitLib.h>
#include <Service/PlatformService.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/AcpiAml.h>
#include <Mcfg.h>
#include <Library/DebugDataLib.h>
#include <Library/MpInitLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <BootloaderCoreGlobal.h>
#include <Library/S3SaveRestoreLib.h>
#include <Guid/SmmInformationGuid.h>
#include "AcpiInitLibInternal.h"

STATIC
CONST EFI_ACPI_COMMON_HEADER *mAcpiTblTmpl[] = {
  (EFI_ACPI_COMMON_HEADER *)&mBootGraphicsResourceTableTemplate
};

CONST EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER RsdpTmp = {
  .Signature   = EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE,
  .Checksum    = 0,
  .OemId       = EFI_ACPI_OEM_ID,
  .Revision    = EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION,
  .Length      = sizeof (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER),
  .ExtendedChecksum   = 0,
  .Reserved    = {0, 0, 0}
};

CONST EFI_ACPI_DESCRIPTION_HEADER XsdtTmp = {
  .Signature = EFI_ACPI_5_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
  .Checksum    = 0,
  .Length   = 0,
  .Revision = EFI_ACPI_5_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_REVISION,
  .OemId = EFI_ACPI_OEM_ID,
  .OemTableId = EFI_ACPI_OEM_TABLE_ID,
  .OemRevision = EFI_ACPI_OEM_REVISION,
  .CreatorId = EFI_ACPI_CREATOR_ID,
  .CreatorRevision = EFI_ACPI_CREATOR_REVISION
};

/**
  This function calculates and updates an UINT8 checksum.

  @param[in]  Buffer          Pointer to buffer to checksum
  @param[in]  Size            Number of bytes to checksum

**/
VOID
AcpiPlatformChecksum (
  IN UINT8      *Buffer,
  IN UINTN      Size
  )
{
  UINTN ChecksumOffset;

  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);

  //
  // Set checksum to 0 first
  //
  Buffer[ChecksumOffset] = 0;

  //
  // Update checksum value
  //
  Buffer[ChecksumOffset] = CalculateCheckSum8 (Buffer, Size);
}

/**
  This function updates GNVS data structure base address dynamically.

  @param[in]  Dsdt          Pointer to DSDT table
  @param[in]  GnvsBase      Physical address of the GNVS data structure

**/
VOID
UpdateAcpiGnvs (
  IN EFI_ACPI_DESCRIPTION_HEADER   *Dsdt,
  IN UINT32                         GnvsBase
  )
{
  UINT8 *Ptr;
  UINT8 *End;

  Ptr = (UINT8 *)Dsdt;
  End = (UINT8 *)Dsdt + Dsdt->Length;

  /*
   * Loop through the ASL looking for values that we must fix up.
   */
  for (; Ptr < End; Ptr++) {
    if (* (UINT32 *)Ptr != SIGNATURE_32 ('G', 'N', 'V', 'S')) {
      continue;
    }
    if (* (Ptr - 1) != AML_EXT_REGION_OP) {
      continue;
    }
    * (UINT32 *) (Ptr + 6)  = GnvsBase;
    * (UINT16 *) (Ptr + 11) = (UINT16)GetAcpiGnvsSize();
    break;
  }
}

/**
  Find an ACPI table using the given signature.

  @param[in] Rsdt            ACPI table RSDT pointer.
  @param[in] Signature       ACPI table signature to find.
  @param[in] EntryIndex      Address to receive the entry index if found.

  @retval  ACPI table pointer if found.  And EntryIndex is updated.
           NULL if not found.

**/
EFI_ACPI_DESCRIPTION_HEADER *
FindAcpiTableBySignature (
  IN   EFI_ACPI_DESCRIPTION_HEADER  *Rsdt,
  IN   UINT32                        Signature,
  IN   UINT32                       *EntryIndex   OPTIONAL
  )
{
  EFI_ACPI_DESCRIPTION_HEADER      *CurrHdr;
  UINT32                           *RsdtEntry;
  UINT32                            EntryNum;
  UINT32                            Index;

  RsdtEntry = (UINT32 *) ((UINT8 *)Rsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  EntryNum  = (Rsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof (UINT32);
  for (Index = 0; Index < EntryNum; Index++) {
    CurrHdr = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)RsdtEntry[Index];
    if ((CurrHdr != NULL) && (CurrHdr->Signature == Signature)) {
      if (EntryIndex != NULL) {
        *EntryIndex = Index;
      }
      return CurrHdr;
    }
  }

  return NULL;
}

/**
  Update ACPI tables using the new table provided.

  @param[in] AcpiTable            ACPI table address to update.
  @param[in] Length               ACPI table buffer length.

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid region type given
  @retval EFI_UNSUPPORTED         Not supported
  @retval EFI_ABORTED             Error occrued
**/
EFI_STATUS
EFIAPI
AcpiTableUpdate (
  IN     UINT8                      *AcpiTable,
  IN     UINT32                      Length
  )
{
  EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER   *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                    *Xsdt;
  EFI_ACPI_DESCRIPTION_HEADER                    *Rsdt;
  EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE      *Facp;
  EFI_ACPI_DESCRIPTION_HEADER      *AcpiHdr;
  UINT8                            *Current;
  UINT8                            *Previous;
  UINT32                           *RsdtEntry;
  UINT64                           *XsdtEntry;
  UINT32                            EntryIndex;
  UINT32                            Policy;
  UINT32                            Size;
  UINT32                            EntryNum;
  EFI_STATUS                        Status;
  S3_DATA                          *S3Data;
  UINT32                            AcpiMax;

  if ((AcpiTable == NULL) || (Length < sizeof (EFI_ACPI_DESCRIPTION_HEADER))) {
    return EFI_INVALID_PARAMETER;
  }

  Rsdp = (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER *)(UINTN)PcdGet32 (PcdAcpiTablesRsdp);
  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Rsdp->RsdtAddress;
  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Rsdp->XsdtAddress;
  RsdtEntry = (UINT32 *) ((UINT8 *)Rsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  XsdtEntry = (UINT64 *) ((UINT8 *)Xsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  EntryNum  = (Rsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof (UINT32);
  S3Data    = (S3_DATA *) GetS3DataPtr();
  Current   = (UINT8 *)(UINTN)S3Data->AcpiTop;
  AcpiMax   = S3Data->AcpiBase + PcdGet32 (PcdLoaderAcpiReclaimSize);

  Status = EFI_SUCCESS;
  Size   = 0;
  while (Size < Length) {
    AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER *)(AcpiTable + Size);

    //
    // Verify Checksum
    //
    if (CalculateSum8 ((UINT8 *)AcpiHdr, AcpiHdr->Length) != 0) {
      Status = EFI_ABORTED;
      break;
    }

    if (Size + AcpiHdr->Length > Size) {
      Size +=  AcpiHdr->Length;
    } else {
      Status = EFI_ABORTED;
      break;
    }

    // Determine policy
    Previous = Current;
    ACPI_ALIGN ();
    if (((UINT32)(UINTN)Current + AcpiHdr->Length) > AcpiMax) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }

    CopyMem  (Current, AcpiHdr, AcpiHdr->Length);

    // Update the ACPI header to pointer to the new copy
    // And then update the table if required
    AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER *)Current;
    Status  = PlatformUpdateAcpiTable (Current);
    if (Status != EFI_SUCCESS) {
      Current = Previous;
      continue;
    }

    DEBUG ((DEBUG_INFO, "ACPI Table 0x%08X - ", AcpiHdr->Signature));
    if (AcpiHdr->Signature == EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
      Facp = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FindAcpiTableBySignature (
             Rsdt, EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE, &EntryIndex);
      if (Facp != NULL) {
        DEBUG ((DEBUG_INFO, "Replaced\n"));
        Facp->Dsdt  = (UINT32)(UINTN)AcpiHdr;
        Facp->XDsdt = (UINT64)(UINTN)AcpiHdr;
        AcpiPlatformChecksum ((UINT8 *)Facp, Facp->Header.Length);
        UpdateAcpiGnvs (AcpiHdr, PcdGet32 (PcdAcpiGnvsAddress));
      } else {
        Status = EFI_ABORTED;
        break;
      }
    } else {
      Policy = ACPI_APPEND;
      if ( (AcpiHdr->Signature == SIGNATURE_32 ('$', 'V', 'B', 'T')) ||
           (AcpiHdr->Signature != EFI_ACPI_5_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) ) {
        // Try to find the table
        if (FindAcpiTableBySignature (Rsdt, AcpiHdr->Signature, &EntryIndex) != NULL) {
          Policy = ACPI_REPLACE;
        }
      }
      if (Policy == ACPI_APPEND) {
        DEBUG ((DEBUG_INFO, "Appended\n"));
        RsdtEntry [EntryNum] = (UINT32)(UINTN)AcpiHdr;
        XsdtEntry [EntryNum] = (UINT32)(UINTN)AcpiHdr;
        EntryNum++;
        if (EntryNum > PcdGet32 (PcdAcpiTablesMaxEntry)) {
          Status = EFI_OUT_OF_RESOURCES;
          break;
        }
      } else if (Policy == ACPI_REPLACE) {
        DEBUG ((DEBUG_INFO, "Replaced\n"));
        RsdtEntry [EntryIndex] = (UINT32)(UINTN)AcpiHdr;
        XsdtEntry [EntryIndex] = (UINT32)(UINTN)AcpiHdr;
      }
    }
    AcpiPlatformChecksum ((UINT8 *)AcpiHdr, AcpiHdr->Length);
    Current += AcpiHdr->Length;
  }

  //
  // Check ACPI memory range again since a Table length may increase in hook
  //
  if ((UINT32)(UINTN)Current > AcpiMax) {
    Status = EFI_OUT_OF_RESOURCES;
  }

  Rsdt->Length = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + EntryNum * sizeof (UINT32);
  AcpiPlatformChecksum ((UINT8 *)Rsdt, Rsdt->Length);

  Xsdt->Length = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + EntryNum * sizeof (UINT64);
  AcpiPlatformChecksum ((UINT8 *)Xsdt, Xsdt->Length);

  //
  // Update AcpiTop for S3 resume
  //
  S3Data->AcpiTop = (UINT32)(UINTN)Current;

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Acpi update failed - %r\n", Status));
  }

  return Status;
}

/**
  This function updates MADT data structure based on Sysinfo APICID
  for number of active cores dynamically.

  @param[in]  Current               Pointer to MADT table
  @retval     EFI_SUCCESS           Patched the MADT
              EFI_UNSUPPORTED       Sysinfo table not found.
              EFI_OUT_OF_RESOURCES  If less MADT entries
**/
EFI_STATUS
UpdateMadt (
  IN UINT8   *Current
  )
{
  EFI_ACPI_DESCRIPTION_HEADER                   *Table;
  UINT8                                         *TempMadt;
  UINT8                                         *MadtPtr;
  UINT8                                         *MadtEnd;
  EFI_ACPI_MADT_ENTRY_COMMON_HEADER             *EntryHeader;
  EFI_ACPI_5_0_PROCESSOR_LOCAL_APIC_STRUCTURE   *LocalApic;
  EFI_ACPI_5_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE *LocalX2Apic;
  SYS_CPU_INFO                                  *SysCpuInfo;
  UINT32                                         Length;
  UINT32                                         Index;

  //
  // Get the number of detected CPUs
  //
  SysCpuInfo = MpGetInfo ();
  if ((SysCpuInfo == NULL) || (SysCpuInfo->CpuCount == 0)) {
    return EFI_ABORTED;
  }

  Table = (EFI_ACPI_DESCRIPTION_HEADER *)Current;

  //
  // Copy original MADT
  //
  TempMadt = (UINT8 *)AllocateTemporaryMemory (0);
  CopyMem ((VOID *)TempMadt, (VOID *)Table, Table->Length);

  MadtPtr = TempMadt;
  MadtEnd = MadtPtr + Table->Length;

  //
  // Copy MADT Header
  //
  Length = sizeof (EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);
  CopyMem ((VOID *)Current, (VOID *)MadtPtr, Length);
  MadtPtr += Length;
  Current += Length;

  //
  // Append Processor Local APIC info
  //
  if (FeaturePcdGet (PcdCpuX2ApicEnabled)) {
    LocalX2Apic = (EFI_ACPI_5_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE *)Current;
    Length = sizeof (EFI_ACPI_5_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE) * SysCpuInfo->CpuCount;
    ZeroMem (LocalX2Apic, Length);
    for (Index = 0; Index < SysCpuInfo->CpuCount; Index++) {
      LocalX2Apic[Index].Type             = EFI_ACPI_5_0_PROCESSOR_LOCAL_X2APIC;
      LocalX2Apic[Index].Length           = sizeof (EFI_ACPI_5_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE);
      LocalX2Apic[Index].AcpiProcessorUid = Index + PcdGet32 (PcdAcpiProcessorIdBase);
      LocalX2Apic[Index].X2ApicId         = SysCpuInfo->CpuInfo[Index].ApicId;
      LocalX2Apic[Index].Flags            = 1;
    }
  } else {
    LocalApic = (EFI_ACPI_5_0_PROCESSOR_LOCAL_APIC_STRUCTURE *)Current;
    Length = sizeof (EFI_ACPI_5_0_PROCESSOR_LOCAL_APIC_STRUCTURE) * SysCpuInfo->CpuCount;
    ZeroMem (LocalApic, Length);
    for (Index = 0; Index < SysCpuInfo->CpuCount; Index++) {
      LocalApic[Index].Type             = EFI_ACPI_5_0_PROCESSOR_LOCAL_APIC;
      LocalApic[Index].Length           = sizeof (EFI_ACPI_5_0_PROCESSOR_LOCAL_APIC_STRUCTURE);
      LocalApic[Index].AcpiProcessorId  = (UINT8)(Index + PcdGet32 (PcdAcpiProcessorIdBase));
      LocalApic[Index].ApicId           = (UINT8)SysCpuInfo->CpuInfo[Index].ApicId;
      LocalApic[Index].Flags            = 1;
    }
  }
  Current += Length;

  //
  // Copy rest of MADT entries
  //
  while (MadtPtr < MadtEnd) {
    EntryHeader = (EFI_ACPI_MADT_ENTRY_COMMON_HEADER *)MadtPtr;
    Length = EntryHeader->Length;
    if ((EntryHeader->Type != EFI_ACPI_5_0_PROCESSOR_LOCAL_APIC) && (EntryHeader->Type != EFI_ACPI_5_0_PROCESSOR_LOCAL_X2APIC)) {
      CopyMem ((VOID *)Current, (VOID *)MadtPtr, Length);
      Current += Length;
    }
    MadtPtr += Length;
  }

  //
  // Update Table Length
  //
  Length = (UINT32)(UINTN)(Current - (UINT8 *)Table);
  Table->Length = Length;

  return EFI_SUCCESS;
}

/**
  This function updates FWST ACPI data structure.

  @param[in]  Current               Pointer to FWST table
  @retval     EFI_SUCCESS           Patched the FWST
              EFI_NOT_FOUND         Reserved region component information not found.
**/
EFI_STATUS
UpdateFwst (
  IN UINT8   *Current
  )
{
  UINT8                           Count;
  UINT32                          RsvdBase;
  EFI_STATUS                      Status;
  EFI_FWST_ACPI_DESCRIPTION_TABLE *FwstAcpiTablePtr;
  FW_UPDATE_COMP_STATUS           *FwUpdCompStatus;

  //
  // We do not need to populate ACPI table during firmware update
  //
  if (GetBootMode () == BOOT_ON_FLASH_UPDATE) {
    return EFI_SUCCESS;
  }

  //
  // Get bootloader reserved region base
  //
  Status = GetComponentInfo (FLASH_MAP_SIG_BLRESERVED, &RsvdBase, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Could not get reserved region base\n"));
    return Status;
  }

  //
  // Fill in details for FWST ACPI table
  //
  FwstAcpiTablePtr = (EFI_FWST_ACPI_DESCRIPTION_TABLE *)Current;
  FwUpdCompStatus = (FW_UPDATE_COMP_STATUS *)(RsvdBase + sizeof(FW_UPDATE_STATUS));
  for (Count = 0; Count < MAX_FW_COMPONENTS; Count ++) {
    if (!CompareGuid(&FwUpdCompStatus->FirmwareId, &gCsmeFWUDriverImageFileGuid)) {
      CopyMem(&(FwstAcpiTablePtr->EsrtTableEntry[Count].FwClass), &(FwUpdCompStatus->FirmwareId), sizeof(EFI_GUID));
      FwstAcpiTablePtr->EsrtTableEntry[Count].FwType = ESRT_FW_TYPE_SYSTEMFIRMWARE;
      FwstAcpiTablePtr->EsrtTableEntry[Count].LastAttemptVersion = FwUpdCompStatus->LastAttemptVersion;
      FwstAcpiTablePtr->EsrtTableEntry[Count].LastAttemptStatus = FwUpdCompStatus->LastAttemptStatus;
    }
    FwUpdCompStatus = (FW_UPDATE_COMP_STATUS *)((UINT32)(UINTN)FwUpdCompStatus + sizeof(FW_UPDATE_COMP_STATUS));
  }

  return EFI_SUCCESS;
}

/**
  This function creates necessary ACPI tables and puts the RSDP
  table in F segment so that OS can locate it.

  @param[in]  AcpiMemTop      ACPI memory top address.

  @retval     EFI_SUCCESS     ACPI tables are created successfully.
              EFI_NOT_FOUND   Required ACPI tables could not be found.
              EFI_UNSUPPORTED Sysinfo table not found.

**/
EFI_STATUS
EFIAPI
AcpiInit (
  IN  UINT32    *AcpiMemBase
  )
{
  UINT8                   *TblPtr;
  UINT8                   *EndPtr;
  EFI_ACPI_COMMON_HEADER  *Table;
  UINT8                   *Current;
  UINT8                   *Previous;
  EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER   *Rsdp;
  EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE      *Facp;
  EFI_ACPI_DESCRIPTION_HEADER                    *Dsdt;
  EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE   *Facs;
  EFI_ACPI_DESCRIPTION_HEADER                    *Xsdt;
  EFI_ACPI_DESCRIPTION_HEADER                    *Rsdt;
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE *Mcfg;
  PLATFORM_SERVICE         *PlatformService;
  UINT32                   *RsdtEntry;
  UINT64                   *XsdtEntry;
  UINT32                    TotalSize;
  UINT32                    XsdtIndex;
  UINT32                    SectionLen;
  UINT32                    UpdateRdstXsdt;
  EFI_STATUS                Status;
  UINT32                    Round;
  UINT32                    StartIdx;
  UINT32                    EndIdx;
  BOOLEAN                   Loop;
  UINT64                    Signature;
  CONST EFI_ACPI_COMMON_HEADER  **AcpiTblTmpl;

  Facs = NULL;
  Dsdt = NULL;
  Facp = NULL;
  UpdateRdstXsdt = 0;

  Current = (UINT8 *)(UINTN)(*AcpiMemBase);

  //
  // Create RSDP
  //
  Rsdp = (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER *) ACPI_ALIGN_PAGE ();
  CopyMem (Rsdp, &RsdpTmp, sizeof (RsdpTmp));
  TotalSize = sizeof (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER);
  Current += TotalSize;

  //
  // Create RSDT structures and allocate buffers.
  //
  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *) ACPI_ALIGN ();
  CopyMem (Rsdt, &XsdtTmp, sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  Rsdt->Signature = EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE;
  Rsdt->Revision  = EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_TABLE_REVISION;
  RsdtEntry = (UINT32 *) ((UINT8 *)Rsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  SetMem (RsdtEntry, PcdGet32 (PcdAcpiTablesMaxEntry) * sizeof (UINT32), 0);
  TotalSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + PcdGet32 (PcdAcpiTablesMaxEntry) * sizeof (UINT32);
  Current += TotalSize;

  //
  // Create XSDT structures and allocate buffers.
  //
  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *) ACPI_ALIGN ();
  CopyMem (Xsdt, &XsdtTmp, sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  XsdtEntry = (UINT64 *) ((UINT8 *)Xsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  XsdtIndex = 0;
  SetMem (XsdtEntry, PcdGet32 (PcdAcpiTablesMaxEntry) * sizeof (UINT64), 0);
  TotalSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + PcdGet32 (PcdAcpiTablesMaxEntry) * sizeof (UINT64);
  Current += TotalSize;

  // 1st round: Copy existing ACPI tables from flash to memory and update
  // 2nd round: Create new common ACPI tables from template and update
  // 3rd round: Create platform ACPI tables from template and update
  for (Round = 0; Round < 3; Round++) {
    if (Round == 0) {
      TblPtr   = (UINT8 *)(UINTN)PcdGet32 (PcdAcpiTablesAddress);
      EndPtr   = TblPtr + ((* ((UINT32 *) (TblPtr - 8)) & 0xFFFFFF) - 28);
    } else if (Round == 1) {
      StartIdx    = 0;
      EndIdx      = ARRAY_SIZE (mAcpiTblTmpl);
      AcpiTblTmpl = &mAcpiTblTmpl[0];
    } else if (Round == 2) {
      StartIdx    = 0;
      EndIdx      = MAX_ACPI_TEMPLATE_NUM;
      AcpiTblTmpl = (CONST EFI_ACPI_COMMON_HEADER **)(UINTN)PcdGet32 (PcdAcpiTableTemplatePtr);
      if (AcpiTblTmpl == NULL) {
        continue;
      }
    }

    while (TRUE) {
      Loop = (Round == 0) ? (TblPtr < EndPtr) : (StartIdx < EndIdx);
      if (!Loop) {
        break;
      }

      Previous = Current;
      if (Round == 0) {
        Table = (EFI_ACPI_COMMON_HEADER *)TblPtr;
      } else {
        Table = (EFI_ACPI_COMMON_HEADER *)AcpiTblTmpl[StartIdx];
      }
      if (Table == NULL) {
        break;
      }

      ACPI_ALIGN ();
      CopyMem  (Current, Table, Table->Length);

      UpdateRdstXsdt = 0;
      Status = PlatformUpdateAcpiTable (Current);
      if (!EFI_ERROR(Status)) {
        UpdateRdstXsdt = 1;
        switch (Table->Signature) {
        case EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
          // FACP
          Facp = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)Current;
          break;
        case EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE:
          // FACS
          Facs = (EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)Current;
          UpdateRdstXsdt = 0;
          break;
        case EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
          // DSDT
          Dsdt = (EFI_ACPI_DESCRIPTION_HEADER *)Current;
          UpdateAcpiGnvs (Dsdt, PcdGet32 (PcdAcpiGnvsAddress));
          UpdateRdstXsdt = 0;
          break;
        case EFI_ACPI_5_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE:
          if (!FeaturePcdGet(PcdMcfgUsePlatformBaseAddr)) {
            // MCFG
            Mcfg = (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE *)Current;
            Mcfg->Segment.BaseAddress = PcdGet64 (PcdPciExpressBaseAddress);
          }
          break;
        case EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE:
          if (!FeaturePcdGet(PcdMadtUsePlatformLapic)) {
            // MADT
            Status = UpdateMadt (Current);
            if (EFI_ERROR(Status)) {
              // MADT is critical, if error occurs, abort.
              return Status;
            }
          }
          break;
        case EFI_ACPI_5_0_FIRMWARE_PERFORMANCE_DATA_TABLE_SIGNATURE:
          // FPDT
          Status = UpdateFpdt (Current);
          break;
        case EFI_FIRMWARE_UPDATE_STATUS_TABLE_SIGNATURE:
          // FWST
          Status = UpdateFwst (Current);
          break;
        case EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE_SIGNATURE:
          // BGRT
          Status = UpdateBgrt (Current);
          break;

        default:
          // Misc
          break;
        }
      }

      if (!EFI_ERROR(Status)) {
        if (UpdateRdstXsdt == 1) {
          Signature = Table->Signature;
          DEBUG ((DEBUG_INFO, "Publish ACPI table: %a @ 0x%08X\n", &Signature, (UINT32)(UINTN)Current));
          RsdtEntry[XsdtIndex]   = (UINT32) (UINTN)Current;
          XsdtEntry[XsdtIndex++] = (UINT64) (UINTN)Current;
        }
        ASSERT (XsdtIndex < PcdGet32 (PcdAcpiTablesMaxEntry));

        if (Current != (UINT8 *)Facp) {
          AcpiPlatformChecksum (
            Current,
            ((EFI_ACPI_COMMON_HEADER *)Current)->Length
            );
        }

        Current += ((EFI_ACPI_COMMON_HEADER *)Current)->Length;
      } else {
        DEBUG ((DEBUG_INFO, "Not adding ACPI table \n"));
        Current = Previous;
      }

      if (Round == 0) {
        SectionLen = * (UINT32 *) (TblPtr - 4) & 0x00FFFFFF;
        TblPtr = TblPtr + ((SectionLen + 3) & ~3);
      } else {
        StartIdx++;
      }
    }
  }

  if (Facp == NULL || Facs == NULL || Dsdt == NULL) {
    return EFI_NOT_FOUND;
  }

  Facp->FirmwareCtrl  = (UINT32)(UINTN)Facs;
  Facp->XFirmwareCtrl = (UINT64)(UINTN)Facs;
  Facp->Dsdt  = (UINT32)(UINTN)Dsdt;
  Facp->XDsdt = (UINT64)(UINTN)Dsdt;

  AcpiPlatformChecksum ((UINT8 *)Facp, Facp->Header.Length);

  Rsdt->Length = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + XsdtIndex * sizeof (UINT32);
  AcpiPlatformChecksum ((UINT8 *)Rsdt, Rsdt->Length);

  Xsdt->Length = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + XsdtIndex * sizeof (UINT64);
  AcpiPlatformChecksum ((UINT8 *)Xsdt, Xsdt->Length);

  //
  // Update RSDP
  //
  Rsdp->RsdtAddress = (UINT32)(UINTN)Rsdt;
  Rsdp->XsdtAddress = (UINT64)(UINTN)Xsdt;
  Rsdp->Checksum    = CalculateCheckSum8 ((UINT8 *)Rsdp, sizeof (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER));
  Rsdp->ExtendedChecksum = CalculateCheckSum8 ((UINT8 *)Rsdp, Rsdp->Length);
  *AcpiMemBase = (UINT32)(UINTN)Current;

  Status = PcdSet32S (PcdAcpiTablesRsdp, (UINT32)(UINTN)Rsdp);

  //
  // Keep a copy at F segment so that non-UEFI OS will find ACPI tables
  //
  if (FeaturePcdGet (PcdLegacyEfSegmentEnabled)) {
    CopyMem ((VOID *)0xFFF80, Rsdp, sizeof (RsdpTmp));
  }

  // Update ACPI update service so that payload can have opportunity to update ACPI tables
  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL) {
    PlatformService->AcpiTableUpdate = AcpiTableUpdate;
  }

  return EFI_SUCCESS;
}

typedef VOID (*DOWAKEUP) (UINT32 WakeVector);

/**
  Discover the S3 required info via the live ACPI tables and save it
  to the TSEG S3 communication area for use on S3 resume.

  Save AcpiBase and FACS address into TSEG so that it can be retrieved
  on S3 resume without trusting DRAM-resident ACPI tables. The storage
  method depends on PcdBuildSmmHobs:
    BIT1 - copy BL_ACPI_S3_INFO at a fixed offset after PLD_TO_BL_SMM_INFO
           and per-CPU SMM base entries in TSEG
    BIT0 - append a BL_ACPI_S3_INFO structure via AppendS3Info()

  @retval EFI_SUCCESS         ACPI S3 data saved successfully.
  @retval EFI_NOT_FOUND       ACPI S3 data not found in the ACPI tables.
  @retval EFI_DEVICE_ERROR    CPU info unavailable.
  @retval EFI_UNSUPPORTED     No supported storage method available.

**/
EFI_STATUS
EFIAPI
SaveAcpiDataForS3(
  VOID
  )
{
  LDR_SMM_INFO         LdrSmmInfo;
  BL_ACPI_S3_INFO      AcpiS3Info;
  EFI_STATUS           Status;
  SYS_CPU_INFO        *SysCpuInfo;
  VOID                *AcpiS3TsegPtr;

  //
  // Get the number of detected CPUs
  //
  SysCpuInfo = MpGetInfo ();
  if ((SysCpuInfo == NULL) || (SysCpuInfo->CpuCount == 0)) {
    return EFI_DEVICE_ERROR;
  }

  Status = EFI_UNSUPPORTED;

  AcpiS3Info.AcpiS3InfoHdr.Signature = BL_PLD_COMM_SIG;
  AcpiS3Info.AcpiS3InfoHdr.Id        = BL_FACS_ADDR_COMM_ID;
  AcpiS3Info.AcpiS3InfoHdr.Count     = 1;
  AcpiS3Info.AcpiS3InfoHdr.TotalSize = sizeof (BL_ACPI_S3_INFO);
  AcpiS3Info.AcpiBase                = PcdGet32 (PcdAcpiTablesRsdp);
  AcpiS3Info.FacsAddress             = FindAcpiFacsAddress(PcdGet32 (PcdAcpiTablesRsdp));

  if (AcpiS3Info.AcpiBase == 0 || AcpiS3Info.FacsAddress == 0) {
    DEBUG((DEBUG_ERROR, "ACPI info needed for S3 is Invalid!\n"));
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  if ((PcdGet8(PcdBuildSmmHobs) & BIT1) != 0) {
    PlatformUpdateHobInfo (&gSmmInformationGuid, &LdrSmmInfo);
    AcpiS3TsegPtr = (VOID *)(UINTN)(LdrSmmInfo.SmmBase + sizeof(PLD_TO_BL_SMM_INFO) + (sizeof(CPU_SMMBASE)*SysCpuInfo->CpuCount));
    if ((AcpiS3TsegPtr < (VOID*)(UINTN)LdrSmmInfo.SmmBase) ||
        (AcpiS3TsegPtr >= (VOID*)(UINTN)(LdrSmmInfo.SmmBase + LdrSmmInfo.SmmSize - sizeof(BL_ACPI_S3_INFO)))) {
      DEBUG((DEBUG_ERROR, "TSEG target address for ACPI S3 info is out of range!\n"));
      ASSERT (FALSE);
      return EFI_UNSUPPORTED;
    }
    DEBUG((DEBUG_INFO, "Copying ACPI info struct to 0x%08X in S3 data\n", (UINTN)AcpiS3TsegPtr));
    CopyMem (AcpiS3TsegPtr , &AcpiS3Info, sizeof(BL_ACPI_S3_INFO));
    Status = EFI_SUCCESS;
  }
  else if ((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) {
    Status = AppendS3Info(&AcpiS3Info, FALSE);
  }
  return Status;
}

/**
  Retrieve the pointer to BL_ACPI_S3_INFO structure from the TSEG S3 communication area.

  This function locates the BL_ACPI_S3_INFO structure stored in TSEG by
  SaveAcpiDataForS3() during normal boot. The retrieval method depends on
  PcdBuildSmmHobs:
    BIT1 - read from a fixed offset after PLD_TO_BL_SMM_INFO and per-CPU SMM base entries in TSEG
    BIT0 - find the structure appended via AppendS3Info()

  @retval Pointer to BL_ACPI_S3_INFO if found and valid, NULL if not found or invalid.

**/
BL_ACPI_S3_INFO*
EFIAPI
GetAcpiS3Info(
  VOID
)
{
  LDR_SMM_INFO         LdrSmmInfo;
  BL_ACPI_S3_INFO     *AcpiS3Info;
  SYS_CPU_INFO        *SysCpuInfo;

  //
  // Get the number of detected CPUs
  //
  SysCpuInfo = MpGetInfo ();
  if ((SysCpuInfo == NULL) || (SysCpuInfo->CpuCount == 0)) {
    return NULL;
  }

  PlatformUpdateHobInfo (&gSmmInformationGuid, &LdrSmmInfo);
  if ((PcdGet8(PcdBuildSmmHobs) & BIT1) != 0) {
    AcpiS3Info = (BL_ACPI_S3_INFO *)(UINTN)(LdrSmmInfo.SmmBase + sizeof(PLD_TO_BL_SMM_INFO) + (sizeof(CPU_SMMBASE)*SysCpuInfo->CpuCount));
    if ((AcpiS3Info < (BL_ACPI_S3_INFO *)(UINTN)LdrSmmInfo.SmmBase) ||
        (AcpiS3Info >= (BL_ACPI_S3_INFO *)(UINTN)(LdrSmmInfo.SmmBase + LdrSmmInfo.SmmSize - sizeof(BL_ACPI_S3_INFO)))) {
      DEBUG((DEBUG_ERROR, "TSEG target address for ACPI S3 info is out of range!\n"));
      ASSERT (FALSE);
      return NULL;
    }
    if (AcpiS3Info->AcpiS3InfoHdr.Signature != BL_PLD_COMM_SIG || AcpiS3Info->AcpiS3InfoHdr.Id != BL_FACS_ADDR_COMM_ID) {
      DEBUG((DEBUG_ERROR, "FACS address info not found in S3 data at expected location 0x%08X\n", AcpiS3Info));
      return NULL;
    }
    return AcpiS3Info;
  }
  else if ((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) {
    AcpiS3Info = FindS3Info (BL_FACS_ADDR_COMM_ID);
    if (AcpiS3Info == NULL) {
      DEBUG((DEBUG_ERROR, "FACS address info not found in S3 data\n"));
      return NULL;
    }
    return AcpiS3Info;
  }
  else {
    return NULL;
  }
}

/**
  Retrieve the FACS address from the TSEG S3 communication area.

  This function reads the FACS physical address previously saved by
  SaveAcpiDataForS3() during normal boot. It is called on S3 resume
  to locate the FACS without relying on DRAM-resident ACPI tables.

  @retval  Non-zero   Physical address of the FACS table.
  @retval  0          FACS address not found.

**/
UINT64
EFIAPI
GetFacsAddressForS3(
  VOID
  )
{
  BL_ACPI_S3_INFO     *AcpiS3Info;

  AcpiS3Info = GetAcpiS3Info();
  if (AcpiS3Info != NULL) {
    return AcpiS3Info->FacsAddress;
  }
  return 0;

}

/**
  Retrieve the ACPI base address from the TSEG S3 communication area.

  This function reads the ACPI base physical address previously saved by
  SaveAcpiDataForS3() during normal boot. It is called on S3 resume
  to locate the ACPI base without relying on DRAM-resident ACPI tables.

  @retval  Non-zero   Physical address of the ACPI base.
  @retval  0          ACPI base address not found.

**/
UINT64
EFIAPI
GetAcpiBaseForS3(
  VOID
)
{
  BL_ACPI_S3_INFO     *AcpiS3Info;

  AcpiS3Info = GetAcpiS3Info();
  if (AcpiS3Info != NULL) {
    return AcpiS3Info->AcpiBase;
  }
  return 0;

}

/**
  Locate the ACPI wake vector from FACS and jump to it on S3 resume.

  This function retrieves the FACS address from the TSEG S3 communication
  area (stored during normal boot via AppendS3Info), validates the FACS
  signature, extracts the FirmwareWakingVector, and transfers control to
  the OS wake entry point. This function does not return on success.

  @param[in]  FacsAddress   Address of FACS saved from normal boot.

**/
VOID
EFIAPI
FindAcpiWakeVectorAndJump (
  VOID    *FacsAddress
  )
{
  EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE   *Facs;
  UINT32                                          WakeVector;
  DOWAKEUP                                        DoWake;

  Facs = (EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)FacsAddress;
  if (Facs->Signature == EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
    WakeVector = Facs->FirmwareWakingVector;
    // Calculate CRC32 for 0x00000000 ---> BootLoaderRsvdMemBase and
    // compare with the one calculated and saved in Stage1B.
    if (PcdGetBool (PcdS3DebugEnabled)) {
      if (!S3DebugRestoreAndCompareCRC32 () ) {
        return;
      }
    }
    CopyMem ((VOID *)(UINTN)WakeUpBuffer, (VOID *)(UINTN)&WakeUp, WakeUpSize);
    DoWake = (DOWAKEUP)(UINTN)WakeUpBuffer;
    DEBUG ((DEBUG_INIT, "Jump to Wake vector = 0x%x\n", WakeVector));
    DoWake (WakeVector);
    //
    // Should never reach here!
    //
  }
}


/**
  Find the FACS physical address by walking the live ACPI XSDT.

  Scans the XSDT entries for FACP and resolves the FACS pointer via
  XFirmwareCtrl (64-bit) with fallback to FirmwareCtrl (32-bit).
  Used during normal boot to discover FACS after AcpiInit() so that the
  address can be stored for S3 resume.

  @param[in]  AcpiTableBase   Physical address of the RSDP.

  @retval  Non-zero   Physical address of the FACS structure.
  @retval  0          FACS not found.

**/
UINT64
EFIAPI
FindAcpiFacsAddress (
  IN  UINT32    AcpiTableBase
  )
{
  EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER   *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                    *Xsdt;
  EFI_ACPI_COMMON_HEADER                         *Hdr;
  UINT64                                         *XsdtEntry;
  UINT8                                           Index;
  EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE   *Facs;
  EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE      *Facp;

  Rsdp = (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER *)(UINTN)AcpiTableBase;
  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Rsdp->XsdtAddress;
  XsdtEntry = (UINT64 *) ((UINT8 *)Xsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));

  for (Index = 0; Index < PcdGet32 (PcdAcpiTablesMaxEntry); ++Index) {
    Hdr = (EFI_ACPI_COMMON_HEADER *) (UINTN)XsdtEntry[Index];
    if (Hdr->Signature == EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
      Facp = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *) Hdr;
      if (Facp->XFirmwareCtrl != 0) {
        Facs = (EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)Facp->XFirmwareCtrl;
      } else {
        Facs = (EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)Facp->FirmwareCtrl;
      }
      if (Facs->Signature == EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
        return (UINT64)(UINTN)Facs;
      }
    }
  }
  return 0;
}
