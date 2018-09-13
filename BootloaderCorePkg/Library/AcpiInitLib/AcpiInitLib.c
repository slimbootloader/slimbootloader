/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/AcpiInitLib.h>
#include <Library/DebugDataLib.h>
#include <Library/AcpiInitLib.h>
#include <Service/PlatformService.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/AcpiAml.h>
#include <Mcfg.h>
#include <Library/DebugDataLib.h>
#include <Library/MpInitLib.h>

#define  ACPI_ALLOC(x)       (Current = (UINT8 *)(((UINT32)Current - (x)) & ~0x0F))
#define  ACPI_ALLOC_PAGE(x)  (Current = (UINT8 *)(((UINT32)Current - (x)) & ~0x0FFF))

#define  EFI_ACPI_OEM_ID           {'O','E','M','I','D',' '}   // OEMID 6 bytes long
#define  EFI_ACPI_OEM_TABLE_ID     SIGNATURE_64('O','E','M','T','A','B','L','E') // OEM table id 8 bytes long
#define  EFI_ACPI_OEM_REVISION     0x00000005
#define  EFI_ACPI_CREATOR_ID       SIGNATURE_32('C','R','E','A')
#define  EFI_ACPI_CREATOR_REVISION 0x0100000D

#define  ACPI_SKIP             0
#define  ACPI_APPEND           1
#define  ACPI_REPLACE          2

extern  UINT32          WakeUpBuffer;
extern  CHAR8           WakeUp;
extern  UINT32          WakeUpSize;

const EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER RsdpTmp = {
  .Signature = EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE,
  .Checksum    = 0,
  .OemId       = EFI_ACPI_OEM_ID,
  .Revision    = EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION,
  .Length      = sizeof (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER),
  .ExtendedChecksum   = 0,
  .Reserved    = {0, 0, 0}
};

const EFI_ACPI_DESCRIPTION_HEADER XsdtTmp = {
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
  Update ACPI tables using the new table provided.

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
    CurrHdr = (EFI_ACPI_DESCRIPTION_HEADER *)RsdtEntry[Index];
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
  UINT8                            *NewTable;
  UINT32                           *RsdtEntry;
  UINT64                           *XsdtEntry;
  UINT32                            EntryIndex;
  UINT32                            Policy;
  UINT32                            Size;
  UINT32                            EntryNum;
  EFI_STATUS                        Status;

  if ((AcpiTable == NULL) || (Length < sizeof (EFI_ACPI_DESCRIPTION_HEADER))) {
    return EFI_INVALID_PARAMETER;
  }

  Rsdp = (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER *)PcdGet32 (PcdAcpiTablesRsdp);
  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Rsdp->RsdtAddress;
  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Rsdp->XsdtAddress;
  RsdtEntry = (UINT32 *) ((UINT8 *)Rsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  XsdtEntry = (UINT64 *) ((UINT8 *)Xsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  EntryNum  = (Rsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof (UINT32);

  Current   = (UINT8 *)Rsdp;

  Status = EFI_SUCCESS;
  Size   = 0;
  while (Size < Length) {
    AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER *)(AcpiTable + Size);
    if (CalculateCheckSum8 ((UINT8 *)AcpiHdr, AcpiHdr->Length) != 0) {
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
    NewTable = (UINT8 *)ACPI_ALLOC (AcpiHdr->Length);
    if ((UINT32)Current < PcdGet32 (PcdAcpiGnvsAddress) - PcdGet32 (PcdLoaderAcpiReclaimSize)) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }

    CopyMem  (NewTable, AcpiHdr, AcpiHdr->Length);

    // Update the ACPI header to pointer to the new copy
    // And then update the table if required
    AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER *)NewTable;
    Status  = PlatformUpdateAcpiTable (NewTable);
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
  }

  Rsdt->Length = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + EntryNum * sizeof (UINT32);
  AcpiPlatformChecksum ((UINT8 *)Rsdt, Rsdt->Length);

  Xsdt->Length = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + EntryNum * sizeof (UINT64);
  AcpiPlatformChecksum ((UINT8 *)Xsdt, Xsdt->Length);

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
EFIAPI
UpdateMadt (
  IN UINT8   *Current
  )
{
  EFI_ACPI_5_0_PROCESSOR_LOCAL_APIC_STRUCTURE *LocalApic;
  EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *Temp;
  SYS_CPU_INFO            *SysCpuInfo;
  UINT32                   CpuCount;
  UINT32                   MarkCpuCount;
  UINT32                   Idx;
  UINT32                   EndOfMadt;
  UINT8                   *Madt;

  Temp = (EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *)Current;
  EndOfMadt = Temp->Header.Length + (UINT32)Current;

  Madt = Current;
  //Skip the header
  Madt = Madt + sizeof (EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);

  //Get the Mp Table info
  SysCpuInfo = MpGetInfo ();
  if (SysCpuInfo != NULL) {
    CpuCount = SysCpuInfo->CpuCount;
    MarkCpuCount = CpuCount;
  } else {
    return EFI_UNSUPPORTED;
  }

  //Search for Processor Local APIC
  while ((UINT32)Madt < EndOfMadt) {
    LocalApic = (EFI_ACPI_5_0_PROCESSOR_LOCAL_APIC_STRUCTURE *)Madt;
    if (LocalApic->Type == EFI_ACPI_5_0_PROCESSOR_LOCAL_APIC) {
      LocalApic->Flags = 0;
      for (Idx = 0 ; Idx < CpuCount; Idx ++) {
        if ((LocalApic->ApicId == SysCpuInfo->CpuInfo[Idx].ApicId) && (LocalApic->Flags == 0)) {
          LocalApic->Flags = 1;
          MarkCpuCount--;
          break;
        }
      }
    }
    Madt = Madt + LocalApic->Length;
  }

  if (MarkCpuCount != 0) {
    DEBUG ((DEBUG_ERROR, "Less MADT entries than Number of cores..\n"));
    return EFI_OUT_OF_RESOURCES;
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
  IN  UINT32    *AcpiMemTop
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

  Facs = NULL;
  Dsdt = NULL;
  Facp = NULL;
  UpdateRdstXsdt = 0;

  Current = (UINT8 *) (*AcpiMemTop);

  //
  // Create RSDT structures and allocate buffers.
  //
  TotalSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + PcdGet32 (PcdAcpiTablesMaxEntry) * sizeof (UINT32);
  Rsdt      = (EFI_ACPI_DESCRIPTION_HEADER *) ACPI_ALLOC (TotalSize);
  CopyMem (Rsdt, &XsdtTmp, sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  Rsdt->Signature = EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE;
  Rsdt->Revision  = EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_TABLE_REVISION;
  RsdtEntry = (UINT32 *) ((UINT8 *)Rsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  SetMem (RsdtEntry, PcdGet32 (PcdAcpiTablesMaxEntry) * sizeof (UINT32), 0);

  //
  // Create XSDT structures and allocate buffers.
  //
  TotalSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER) + PcdGet32 (PcdAcpiTablesMaxEntry) * sizeof (UINT64);
  Xsdt      = (EFI_ACPI_DESCRIPTION_HEADER *) ACPI_ALLOC (TotalSize);
  CopyMem (Xsdt, &XsdtTmp, sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  XsdtEntry = (UINT64 *) ((UINT8 *)Xsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  XsdtIndex = 0;
  SetMem (XsdtEntry, PcdGet32 (PcdAcpiTablesMaxEntry) * sizeof (UINT64), 0);

  TblPtr  = (UINT8 *)PcdGet32 (PcdAcpiTablesAddress);
  EndPtr  = TblPtr + ((* ((UINT32 *) (TblPtr - 8)) & 0xFFFFFF) - 28);
  while (TblPtr < EndPtr) {
    Previous = Current;

    Table = (EFI_ACPI_COMMON_HEADER *)TblPtr;
    ACPI_ALLOC (Table->Length);
    CopyMem  (Current, Table, Table->Length);

    Status = PlatformUpdateAcpiTable (Current);
    if (Status != EFI_SUCCESS) {
      Current = Previous;
      DEBUG ((DEBUG_WARN, "Not adding ACPI table \n"));
      SectionLen = * (UINT32 *) (TblPtr - 4) & 0x00FFFFFF;
      TblPtr = TblPtr + ((SectionLen + 3) & ~3);
      continue;
    }

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
      // MCFG
      Mcfg = (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE *)Current;
      Mcfg->Segment.BaseAddress = PcdGet64 (PcdPciExpressBaseAddress);
      break;
    case EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE:
      // MADT
      Status = UpdateMadt (Current);
      if (Status != EFI_SUCCESS) {
        return Status;
      }
      break;
    default:
      // Misc
      break;
    }

    if (UpdateRdstXsdt == 1) {
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

    SectionLen = * (UINT32 *) (TblPtr - 4) & 0x00FFFFFF;
    TblPtr = TblPtr + ((SectionLen + 3) & ~3);
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
  // Make Rsdp page aligned
  //
  Rsdp    = (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER *) \
        ACPI_ALLOC_PAGE (sizeof (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER));
  CopyMem (Rsdp, &RsdpTmp, sizeof (RsdpTmp));
  Rsdp->RsdtAddress = (UINT32)Rsdt;
  Rsdp->XsdtAddress = (UINT64) (UINTN)Xsdt;
  Rsdp->Checksum    = CalculateCheckSum8 ((UINT8 *)Rsdp, sizeof (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER));
  Rsdp->ExtendedChecksum = CalculateCheckSum8 ((UINT8 *)Rsdp, Rsdp->Length);
  *AcpiMemTop = (UINT32)Current;

  //
  // Keep a copy at F segment so that non-UEFI OS will find ACPI tables
  //
  PcdSet32 (PcdAcpiTablesRsdp, (UINT32)Rsdp);
  CopyMem ((VOID *)0xFFF80, Rsdp, sizeof (RsdpTmp));

  // Update ACPI update service so that payload can have opportunity to update ACPI tables
  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL) {
    PlatformService->AcpiTableUpdate = AcpiTableUpdate;
  }

  return EFI_SUCCESS;
}

typedef VOID (*DOWAKEUP) (UINT32 WakeVector);


/**
  This function is called on S3 boot flow only.

  It will locate the S3 waking vector from the ACPI table and then
  jump into it. The control will never return.

  @param  AcpiTableBase   ACPI table base address

**/
VOID
FindAcpiWakeVectorAndJump (
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
  UINT32                                          WakeVector;
  DOWAKEUP                                        DoWake;

  Rsdp = (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER *) AcpiTableBase;
  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *) (UINTN)Rsdp->XsdtAddress;
  XsdtEntry = (UINT64 *) ((UINT8 *)Xsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));

  for (Index = 0; Index < PcdGet32 (PcdAcpiTablesMaxEntry); ++Index) {
    Hdr = (EFI_ACPI_COMMON_HEADER *) (UINTN)XsdtEntry[Index];
    if (Hdr->Signature == EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
      Facp = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *) Hdr;
      Facs = (EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *) Facp->FirmwareCtrl;
      if (Facs->Signature == EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
        WakeVector = Facs->FirmwareWakingVector;
        // Calculate CRC32 for 0x00000000 ---> BootLoaderRsvdMemBase and
        // compare with the one calculated and saved in Stage1B.
        if (PcdGetBool (PcdS3DebugEnabled)) {
          if (!S3DebugRestoreAndCompareCRC32 () ) {
            return;
          }
        }
        CopyMem ((VOID *)WakeUpBuffer, &WakeUp, WakeUpSize);
        DoWake = (DOWAKEUP)WakeUpBuffer;
        DEBUG ((DEBUG_INIT, "Jump to Wake vector = 0x%x\n", WakeVector));
        DoWake (WakeVector);
        //
        // Should never reach here!
        //
      }
    }
  }
}
