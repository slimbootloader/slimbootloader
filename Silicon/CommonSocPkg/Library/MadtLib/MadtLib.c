/** @file
  Library instances for Multiple APIC Description Table (MADT) which
  describes the APIC topology of the system.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/MadtLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/MpInitLib.h>

//
// ACPI MADT table template
//
CONST
EFI_ACPI_6_4_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER mAcpiMadtTableTemplate = {
  .Header.Signature       = EFI_ACPI_6_4_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
  .Header.Length          = sizeof (EFI_ACPI_6_4_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER),
  .Header.Revision        = EFI_ACPI_6_4_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION,
  .Header.OemTableId      = FixedPcdGet64 (PcdAcpiDefaultOemTableId),
  .Header.OemRevision     = FixedPcdGet32 (PcdAcpiDefaultOemRevision),
  .Header.CreatorId       = FixedPcdGet32 (PcdAcpiDefaultCreatorId),
  .Header.CreatorRevision = FixedPcdGet32 (PcdAcpiDefaultCreatorRevision),
  .LocalApicAddress       = 0,
  .Flags                  = 0,
};

/**
  Initialize and add an ACPI MADT (Multiple APIC Description Table) header.

  This function initializes the ACPI MADT table header with standard fields such as
  signature, length, revision, OEM information, and creator details. It also sets
  the local APIC address and MADT-specific flags.

  @param[out] AcpiHeader        Pointer to the ACPI table header to be initialized.
  @param[in]  LocalApicAddress  The memory-mapped address of the local APIC.
  @param[in]  Flags             The flags for the MADT table

  @retval EFI_SUCCESS             The header was initialized successfully.
  @retval EFI_INVALID_PARAMETER   A parameter is invalid (e.g., NULL pointer for AcpiHeader).
**/
EFI_STATUS
EFIAPI
AddAcpiMadtHdr (
  OUT EFI_ACPI_DESCRIPTION_HEADER               *AcpiHeader,
  IN  UINT32                                    LocalApicAddress,
  IN  UINT8                                     Flags
  )
{
  EFI_ACPI_6_4_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *Madt;

  Madt = (EFI_ACPI_6_4_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *)AcpiHeader;
  CopyMem(Madt->Header.OemId, FixedPcdGetPtr(PcdAcpiDefaultOemId), 6);
  Madt->Flags = Flags;
  Madt->LocalApicAddress = LocalApicAddress;

  return EFI_SUCCESS;
}

/**
  Add All MADT Local APIC structures to an ACPI table.

  This function appends a Local APIC structure to the provided ACPI
  table for all detected processors. The function updates the ACPI table
  length accordingly.

  @param[in, out] AcpiHeader                Pointer to the ACPI table header to which the DRHD
                                            structure will be appended.
  @retval EFI_SUCCESS                 The structures were added successfully.
  @retval EFI_INVALID_PARAMETER       A parameter is invalid (e.g., NULL pointer for AcpiHeader).
**/
EFI_STATUS
EFIAPI
AddMadtAllLocalApics (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader
  )
{
  UINT32          CpuIndex;
  SYS_CPU_INFO   *SysCpuInfo;
  EFI_STATUS      Status;

  //
  // Get the number of detected CPUs
  //
  SysCpuInfo = MpGetInfo ();
  if ((SysCpuInfo == NULL) || (SysCpuInfo->CpuCount == 0)) {
    return EFI_ABORTED;
  }

  for (CpuIndex = 0; CpuIndex < SysCpuInfo->CpuCount; CpuIndex++) {
    if (FeaturePcdGet (PcdCpuX2ApicEnabled)) {
      Status = AddMadtLocalX2Apic(
        AcpiHeader,
        (UINT32)SysCpuInfo->CpuInfo[CpuIndex].ApicId,
        1,
        (UINT32)(CpuIndex + PcdGet32 (PcdAcpiProcessorIdBase))
        );
    } else {
      Status = AddMadtLocalApic(
        AcpiHeader,
        (UINT8)(CpuIndex + PcdGet32 (PcdAcpiProcessorIdBase)),
        (UINT8)SysCpuInfo->CpuInfo[CpuIndex].ApicId,
        1
        );
    }
    if (EFI_ERROR (Status)) {
      break;
    }
  }
  return Status;
}

/**
  Add MADT Local APIC structure to an ACPI table.

  This function appends a Local APIC structure to the provided ACPI
  table. The Local APIC structure describes a local APIC. The function
  updates the ACPI table length accordingly.

  @param[in, out] AcpiHeader                Pointer to the ACPI table header to which the DRHD
                                            structure will be appended.
  @param[in]      AcpiProcessorUid          The ID of the ACPI processor.
  @param[in]      LocalApicId               The ID of the local APIC.
  @param[in]      Flags                     The flags for the local APIC.

  @retval EFI_SUCCESS                 The structure was added successfully.
  @retval EFI_INVALID_PARAMETER       A parameter is invalid (e.g., NULL pointer for AcpiHeader).
**/
EFI_STATUS
EFIAPI
AddMadtLocalApic (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT8                         AcpiProcessorUid,
  IN     UINT8                         LocalApicId,
  IN     UINT32                        Flags
  )
{
  EFI_ACPI_6_4_PROCESSOR_LOCAL_APIC_STRUCTURE *LocalApicStruct;

  if (AcpiHeader == NULL || AcpiHeader->Length == 0) {
    return EFI_INVALID_PARAMETER;
  }

  LocalApicStruct = (EFI_ACPI_6_4_PROCESSOR_LOCAL_APIC_STRUCTURE *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);

  ZeroMem(LocalApicStruct, sizeof(EFI_ACPI_6_4_PROCESSOR_LOCAL_APIC_STRUCTURE));
  LocalApicStruct->Type         = EFI_ACPI_6_4_PROCESSOR_LOCAL_APIC;
  LocalApicStruct->Length       = sizeof(EFI_ACPI_6_4_PROCESSOR_LOCAL_APIC_STRUCTURE);
  LocalApicStruct->AcpiProcessorUid = AcpiProcessorUid;
  LocalApicStruct->ApicId       = LocalApicId;
  LocalApicStruct->Flags        = Flags;

  DEBUG((DEBUG_INFO,
    "Local APIC: ACPI Processor UID=0x%x, APIC ID=0x%x, Flags=0x%x\n",
    AcpiProcessorUid, LocalApicId, Flags));

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof (EFI_ACPI_6_4_PROCESSOR_LOCAL_APIC_STRUCTURE);
  return EFI_SUCCESS;
}


/**
  Add MADT Local X2APIC structures to an ACPI table.

  This function appends a Local X2APIC structure to the provided ACPI
  table. The Local X2APIC structure describes a local X2APIC. The function
  updates the ACPI table length accordingly.

  @param[in, out] AcpiHeader                Pointer to the ACPI table header to which the DRHD
                                            structure will be appended.
  @param[in]      X2ApicId                  The ID of the local X2APIC.
  @param[in]      Flags                     The flags for the local X2APIC.
  @param[in]      AcpiProcessorUid          The ID of the ACPI processor.

  @retval EFI_SUCCESS                 The structure was added successfully.
  @retval EFI_INVALID_PARAMETER       A parameter is invalid (e.g., NULL pointer for AcpiHeader).
**/
EFI_STATUS
EFIAPI
AddMadtLocalX2Apic (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT32                        X2ApicId,
  IN     UINT32                        Flags,
  IN     UINT32                        AcpiProcessorUid
  )
{
  EFI_ACPI_6_4_PROCESSOR_LOCAL_X2APIC_STRUCTURE *LocalX2ApicStruct;

  if (AcpiHeader == NULL || AcpiHeader->Length == 0) {
    return EFI_INVALID_PARAMETER;
  }

  LocalX2ApicStruct = (EFI_ACPI_6_4_PROCESSOR_LOCAL_X2APIC_STRUCTURE *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);

  ZeroMem(LocalX2ApicStruct, sizeof(EFI_ACPI_6_4_PROCESSOR_LOCAL_X2APIC_STRUCTURE));
  LocalX2ApicStruct->Type         = EFI_ACPI_6_4_PROCESSOR_LOCAL_X2APIC;
  LocalX2ApicStruct->Length       = sizeof(EFI_ACPI_6_4_PROCESSOR_LOCAL_X2APIC_STRUCTURE);
  LocalX2ApicStruct->AcpiProcessorUid = AcpiProcessorUid;
  LocalX2ApicStruct->X2ApicId     = X2ApicId;
  LocalX2ApicStruct->Flags        = Flags;

  DEBUG((DEBUG_INFO,
    "Local X2APIC: ACPI Processor UID=0x%x, X2APIC ID=0x%x, Flags=0x%x\n",
    AcpiProcessorUid, X2ApicId, Flags));

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof (EFI_ACPI_6_4_PROCESSOR_LOCAL_X2APIC_STRUCTURE);
  return EFI_SUCCESS;
}

/**
  Add MADT IO APIC structures to an ACPI table.

  This function appends an IO APIC structure to the provided ACPI table.
  The IO APIC structure describes an I/O APIC. The function updates the
  ACPI table length accordingly.

  @param[in, out] AcpiHeader                Pointer to the ACPI table header to which the DRHD
                                            structure will be appended.
  @param[in]      IoApicId                  The ID of the I/O APIC.
  @param[in]      IoApicAddress             The memory-mapped address of the I/O APIC registers.
  @param[in]      GlobalSystemInterruptBase The base of the global system interrupts for this I/O APIC.

  @retval EFI_SUCCESS                 The structure was added successfully.
  @retval EFI_INVALID_PARAMETER       A parameter is invalid (e.g., NULL pointer for AcpiHeader).
**/
EFI_STATUS
EFIAPI
AddMadtIoApic (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT8                         IoApicId,
  IN     UINT32                        IoApicAddress,
  IN     UINT32                        GlobalSystemInterruptBase
  )
{
  EFI_ACPI_6_4_IO_APIC_STRUCTURE           *IoApicStruct;

  if (AcpiHeader == NULL || AcpiHeader->Length == 0) {
    return EFI_INVALID_PARAMETER;
  }

  IoApicStruct = (EFI_ACPI_6_4_IO_APIC_STRUCTURE *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);

  ZeroMem(IoApicStruct, sizeof(EFI_ACPI_6_4_IO_APIC_STRUCTURE));
  IoApicStruct->Type         = EFI_ACPI_6_4_IO_APIC;
  IoApicStruct->Length       = sizeof(EFI_ACPI_6_4_IO_APIC_STRUCTURE);
  IoApicStruct->IoApicId     = IoApicId;
  IoApicStruct->IoApicAddress = IoApicAddress;
  IoApicStruct->GlobalSystemInterruptBase = GlobalSystemInterruptBase;

  DEBUG((DEBUG_INFO,
    "IO APIC: Id=0x%x, Address=0x%x, GSI Base=0x%x\n",
    IoApicId, IoApicAddress, GlobalSystemInterruptBase));

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof (EFI_ACPI_6_4_IO_APIC_STRUCTURE);
  return EFI_SUCCESS;
}

/**
  Add MADT Interrupt Source Override structure to an ACPI table.

  This function appends an Interrupt Source Override structure to the provided
  ACPI table. The Interrupt Source Override structure describes an interrupt
  source override. The function updates the ACPI table length accordingly.

  @param[in, out] AcpiHeader                Pointer to the ACPI table header to which the DRHD
                                            structure will be appended.
  @param[in]      Bus                       The bus type.
  @param[in]      Source                    The source of the interrupt.
  @param[in]      GlobalSystemInterrupt     The global system interrupt number.
  @param[in]      Flags                     The flags for the interrupt override.

  @retval EFI_SUCCESS                 The structure was added successfully.
  @retval EFI_INVALID_PARAMETER       A parameter is invalid (e.g., NULL pointer for AcpiHeader).
**/
EFI_STATUS
EFIAPI
AddMadtIntSrcOverride (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT8                         Bus,
  IN     UINT8                         Source,
  IN     UINT32                        GlobalSystemInterrupt,
  IN     UINT16                        Flags
  )
{
  EFI_ACPI_6_4_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE *IntSrcOvrStruct;

  if (AcpiHeader == NULL || AcpiHeader->Length == 0) {
    return EFI_INVALID_PARAMETER;
  }

  IntSrcOvrStruct = (EFI_ACPI_6_4_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);

  ZeroMem(IntSrcOvrStruct, sizeof(EFI_ACPI_6_4_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE));
  IntSrcOvrStruct->Type         = EFI_ACPI_6_4_INTERRUPT_SOURCE_OVERRIDE;
  IntSrcOvrStruct->Length       = sizeof(EFI_ACPI_6_4_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE);
  IntSrcOvrStruct->Bus          = Bus;
  IntSrcOvrStruct->Source       = Source;
  IntSrcOvrStruct->GlobalSystemInterrupt = GlobalSystemInterrupt;
  IntSrcOvrStruct->Flags        = Flags;

  DEBUG((DEBUG_INFO,
    "Interrupt Source Override: Bus=0x%x, Source=0x%x, GSI=0x%x, Flags=0x%x\n",
    Bus, Source, GlobalSystemInterrupt, Flags));

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof (EFI_ACPI_6_4_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE);
  return EFI_SUCCESS;

}

/**
  Add MADT Local APIC NMI structure to an ACPI table.

  This function appends a Local APIC NMI structure to the provided
  ACPI table. The Local APIC NMI structure describes a non-maskable
  interrupt for a local APIC. The function updates the ACPI table
  length accordingly.

  @param[in, out] AcpiHeader                Pointer to the ACPI table header to which the DRHD
                                            structure will be appended.
  @param[in]      AcpiProcessorUid          The ACPI processor UID for the local APIC.
  @param[in]      Flags                     The flags for the local APIC NMI.
  @param[in]      LocalApicLint             The local APIC's Local Interrupt number for the NMI.

  @retval EFI_SUCCESS                 The structure was added successfully.
  @retval EFI_INVALID_PARAMETER       A parameter is invalid (e.g., NULL pointer for AcpiHeader).
**/
EFI_STATUS
EFIAPI
AddLocalApicNmi (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT8                         AcpiProcessorUid,
  IN     UINT16                        Flags,
  IN     UINT8                         LocalApicLint
  )
{
  EFI_ACPI_6_4_LOCAL_APIC_NMI_STRUCTURE *LocalApicNmiStruct;

  if (AcpiHeader == NULL || AcpiHeader->Length == 0) {
    return EFI_INVALID_PARAMETER;
  }

  LocalApicNmiStruct = (EFI_ACPI_6_4_LOCAL_APIC_NMI_STRUCTURE *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);

  ZeroMem(LocalApicNmiStruct, sizeof(EFI_ACPI_6_4_LOCAL_APIC_NMI_STRUCTURE));
  LocalApicNmiStruct->Type             = EFI_ACPI_6_4_LOCAL_APIC_NMI;
  LocalApicNmiStruct->Length           = sizeof(EFI_ACPI_6_4_LOCAL_APIC_NMI_STRUCTURE);
  LocalApicNmiStruct->AcpiProcessorUid = AcpiProcessorUid;
  LocalApicNmiStruct->Flags            = Flags;
  LocalApicNmiStruct->LocalApicLint    = LocalApicLint;

  DEBUG((DEBUG_INFO,
    "Local APIC NMI: ACPI Processor UID=0x%x, Flags=0x%x, Local APIC LINT=0x%x\n",
    AcpiProcessorUid, Flags, LocalApicLint));

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof (EFI_ACPI_6_4_LOCAL_APIC_NMI_STRUCTURE);
  return EFI_SUCCESS;
}

/**
  Add MADT Local X2 APIC NMI structure to an ACPI table.

  This function appends a Local X2 APIC NMI structure to the provided
  ACPI table. The Local X2 APIC NMI structure describes a non-maskable
  interrupt for a local X2 APIC. The function updates the ACPI table
  length accordingly.

  @param[in, out] AcpiHeader                Pointer to the ACPI table header to which the DRHD
                                            structure will be appended.
  @param[in]      Flags                     The flags for the local X2 APIC NMI.
  @param[in]      AcpiProcessorUid          The ACPI processor UID for the local X2 APIC.
  @param[in]      LocalApicLint             The local X2 APIC's Local Interrupt number for the NMI.

  @retval EFI_SUCCESS                 The structure was added successfully.
  @retval EFI_INVALID_PARAMETER       A parameter is invalid (e.g., NULL pointer for AcpiHeader).
**/
EFI_STATUS
EFIAPI
AddLocalX2ApicNmi (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT16                        Flags,
  IN     UINT32                        AcpiProcessorUid,
  IN     UINT8                         LocalX2ApicLint
  )
{
  EFI_ACPI_6_4_LOCAL_X2APIC_NMI_STRUCTURE *LocalX2ApicNmiStruct;

  if (AcpiHeader == NULL || AcpiHeader->Length == 0) {
    return EFI_INVALID_PARAMETER;
  }

  LocalX2ApicNmiStruct = (EFI_ACPI_6_4_LOCAL_X2APIC_NMI_STRUCTURE *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);

  ZeroMem(LocalX2ApicNmiStruct, sizeof(EFI_ACPI_6_4_LOCAL_X2APIC_NMI_STRUCTURE));
  LocalX2ApicNmiStruct->Type             = EFI_ACPI_6_4_LOCAL_X2APIC_NMI;
  LocalX2ApicNmiStruct->Length           = sizeof(EFI_ACPI_6_4_LOCAL_X2APIC_NMI_STRUCTURE);
  LocalX2ApicNmiStruct->Flags            = Flags;
  LocalX2ApicNmiStruct->AcpiProcessorUid = AcpiProcessorUid;
  LocalX2ApicNmiStruct->LocalX2ApicLint  = LocalX2ApicLint;

  DEBUG((DEBUG_INFO,
    "Local X2 APIC NMI: ACPI Processor UID=0x%x, Flags=0x%x, Local X2 APIC LINT=0x%x\n",
    AcpiProcessorUid, Flags, LocalX2ApicLint));

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof (EFI_ACPI_6_4_LOCAL_X2APIC_NMI_STRUCTURE);
  return EFI_SUCCESS;
}
