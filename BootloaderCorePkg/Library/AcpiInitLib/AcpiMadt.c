/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BoardInitLib.h>
#include <IndustryStandard/Acpi50.h>
#include <Library/MpInitLib.h>

STATIC CONST EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER mMadtHeader = {
  {
    EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
    0,
    EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION,
    0x00,
    { ' ', ' ', ' ', ' ', ' ', ' ' },
    0,
    0,
    0,
    0,
  },
  FixedPcdGet32 (PcdLocalApicAddress),
  EFI_ACPI_4_0_PCAT_COMPAT
};

/**
  Update MADT a single LocalApicNmi entry

  @param[in] Entry                  Pointer to create a LocalApicNmi entry
  @param[in] AcpiProcessorId        ACPI Processor ID
  @param[in] Flags                  Flags for interrupt trigger and polarity
  @param[in] LocalApicLint          LINT# (0 or 1)

  @retval                           None

 **/
STATIC
VOID
UpdateLocalApicNmi (
  IN  EFI_ACPI_3_0_LOCAL_APIC_NMI_STRUCTURE *Entry,
  IN  UINT8                                  AcpiProcessorId,
  IN  UINT16                                 Flags,
  IN  UINT8                                  LocalApicLint
)
{
  Entry->Type            = EFI_ACPI_3_0_LOCAL_APIC_NMI;
  Entry->Length          = sizeof (EFI_ACPI_3_0_LOCAL_APIC_NMI_STRUCTURE);
  Entry->AcpiProcessorId = AcpiProcessorId;
  Entry->Flags           = Flags;
  Entry->LocalApicLint   = LocalApicLint;
}

/**
  Create MADT LocalApicNmi entries

  @param[in] LocalApicNmi   Pointer to create LocalApicNmi entries

  @retval                   The size of MADT LocalApicNmi entries

 **/
STATIC
UINT32
CreateLocalApicNmi (
  IN  EFI_ACPI_3_0_LOCAL_APIC_NMI_STRUCTURE *LocalApicNmi
)
{
  UINT32        Index;
  UINT32        Count;
  UINT16        Flags;
  UINT8         LintNo;

  Flags  = (BIT2 | BIT0);
  Flags |= (PcdGet16 (PcdLocalApicNmiTrigger) << 3);
  Flags |= (PcdGet16 (PcdLocalApicNmiPolarity) << 1);

  LintNo = PcdGet8 (PcdLocalApicNmiLintIdx);

  Count = PcdGet32 (PcdLocalApicNmiCount);
  for (Index = 0; Index < Count; Index++) {
    UpdateLocalApicNmi (&LocalApicNmi[Index], Index + 1, Flags, LintNo);
  }

  return sizeof (EFI_ACPI_3_0_LOCAL_APIC_NMI_STRUCTURE) * Count;
}

/**
  Update MADT a single InterruptSourceOverride entry

  @param[in] Entry                  Pointer to create a InterruptSourceOverride entry
  @param[in] Bus                    Bus number
  @param[in] Source                 IRQ#
  @param[in] GlobalSystemInterrupt  Global system interrupt to map IRQ#
  @param[in] Flags                  Flags for interrupt trigger and polarity

  @retval                           None

 **/
STATIC
VOID
UpdateInterruptSource (
  IN  EFI_ACPI_3_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE  *Entry,
  IN  UINT8                                              Bus,
  IN  UINT8                                              Source,
  IN  UINT32                                             GlobalSystemInterrupt,
  IN  UINT16                                             Flags
)
{
  Entry->Type                  = EFI_ACPI_3_0_INTERRUPT_SOURCE_OVERRIDE;
  Entry->Length                = sizeof (EFI_ACPI_3_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE);
  Entry->Bus                   = Bus;
  Entry->Source                = Source;
  Entry->GlobalSystemInterrupt = GlobalSystemInterrupt;
  Entry->Flags                 = Flags;
}

/**
  Create MADT InterruptSourceOverride entries

  @param[in] IntrSrcOverride  Pointer to create InterruptSourceOverride entries

  @retval                     The size of MADT InterruptSourceOverride entries

 **/
STATIC
UINT32
CreateInterruptSourceOverride (
  IN  EFI_ACPI_3_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE  *IntrSrcOverride
)
{
  UINT32        Index;
  UINT32        Count;
  UINT16        SciFlags;

  Count = PcdGet32 (PcdIntrSrcOverrideCount);
  ASSERT (Count >= 2);

  Index = 0;
  if (Index < Count) {
    // System Timer IRQ-0
    UpdateInterruptSource (&IntrSrcOverride[Index++], 0, 0, 2, 0);
  }

  if (Index < Count) {
    // ACPI SCI IRQ-9
    SciFlags  = (BIT2 | BIT0);
    SciFlags |= (PcdGet16 (PcdIntrSrcSciTrigger) << 3);
    SciFlags |= (PcdGet16 (PcdIntrSrcSciPolarity) << 1);
    UpdateInterruptSource (&IntrSrcOverride[Index++], 0, 9, 9, SciFlags);
  }

  // These should be updated in PlatformUpdateAcpiTable() if required
  for (;Index < Count; Index++) {
    UpdateInterruptSource (&IntrSrcOverride[Index++], 0, 0, 0, 0);
  }

  return sizeof (EFI_ACPI_3_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE) * Count;
}

/**
  Create MADT IoApic entries

  @param[in] IoApic       Pointer to create IoApic entries

  @retval                 The size of MADT IoApic entries

 **/
STATIC
UINT32
CreateIoApic (
  IN  EFI_ACPI_3_0_IO_APIC_STRUCTURE  *IoApic
)
{
  UINT32         Index;
  UINT32         Count;

  // Allow 1 IO APIC for now
  Count = PcdGet32 (PcdIoApicCount);
  ASSERT (Count == 1);
  for (Index = 0; Index < Count; Index++) {
    IoApic[Index].Type                      = EFI_ACPI_3_0_IO_APIC;
    IoApic[Index].Length                    = sizeof (EFI_ACPI_3_0_IO_APIC_STRUCTURE);
    IoApic[Index].IoApicId                  = PcdGet8 (PcdIoApicId);
    IoApic[Index].Reserved                  = EFI_ACPI_RESERVED_BYTE;
    IoApic[Index].IoApicAddress             = PcdGet32 (PcdIoApicAddress);
    IoApic[Index].GlobalSystemInterruptBase = 0x18 * Index;
  }

  return sizeof (EFI_ACPI_3_0_IO_APIC_STRUCTURE) * Count;
}

/**
  Create MADT ProcessorLocalApic entries

  @param[in] ProcessorLocalApic   Pointer to create ProcessorLocalApic entries

  @retval                         The size of MADT ProcessorLocalApic entries

 **/
STATIC
UINT32
CreateProcessorLocalApic (
  IN  EFI_ACPI_3_0_PROCESSOR_LOCAL_APIC_STRUCTURE *ProcessorLocalApic
)
{
  SYS_CPU_INFO  *SysCpuInfo;
  UINT32         Index;
  UINT32         Count;

  SysCpuInfo = MpGetInfo ();
  ASSERT (SysCpuInfo);
  if (SysCpuInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to get CPU count from MP Info!\n"));
    return 0;
  }

  Count = SysCpuInfo->CpuCount;
  for (Index = 0; Index < Count; Index++) {
    ProcessorLocalApic[Index].Type            = EFI_ACPI_3_0_PROCESSOR_LOCAL_APIC;
    ProcessorLocalApic[Index].Length          = sizeof (EFI_ACPI_3_0_PROCESSOR_LOCAL_APIC_STRUCTURE);
    ProcessorLocalApic[Index].AcpiProcessorId = Index + 1;
    ProcessorLocalApic[Index].ApicId          = SysCpuInfo->CpuInfo[Index].ApicId;
    ProcessorLocalApic[Index].Flags           = 1;
  }

  return sizeof (EFI_ACPI_3_0_PROCESSOR_LOCAL_APIC_STRUCTURE) * Count;
}

/**
  Create MADT Header

  @param[in] Header         Pointer to create MADT Header

  @retval                   The size of MADT Header

 **/
STATIC
UINT32
CreateMadtHeader (
  IN  EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *Header
)
{
  CopyMem (Header, &mMadtHeader, sizeof (EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER));

  return sizeof (EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);
}

/**
  Get the size of MADT

  @retval                   0 if CpuInfo is invalid, otherwise the size of MADT

 **/
UINT32
EFIAPI
GetMadtSize (
  IN  VOID
  )
{
  UINT32         TableSize;
  SYS_CPU_INFO  *SysCpuInfo;

  SysCpuInfo = MpGetInfo ();
  ASSERT (SysCpuInfo);
  if ((SysCpuInfo == NULL) || (SysCpuInfo->CpuCount == 0)) {
    DEBUG ((DEBUG_ERROR, "Failed to get the number of CPU from MP Info!\n"));
    return 0;
  }

  TableSize  = sizeof (EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);
  TableSize += sizeof (EFI_ACPI_3_0_PROCESSOR_LOCAL_APIC_STRUCTURE) * SysCpuInfo->CpuCount;
  TableSize += sizeof (EFI_ACPI_3_0_IO_APIC_STRUCTURE) * PcdGet32 (PcdIoApicCount);
  TableSize += sizeof (EFI_ACPI_3_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE) * PcdGet32 (PcdIntrSrcOverrideCount);
  TableSize += sizeof (EFI_ACPI_3_0_LOCAL_APIC_NMI_STRUCTURE) * PcdGet32 (PcdLocalApicNmiCount);

  return TableSize;
}

/**
  Create MADT(Multiple APIC Description Table).

  @param[in] TableAddress   Pointer to create ACPI MADT
  @param[in] TableLength    Size of MADT

  @retval EFI_SUCCESS       Created ACPI MADT successfully.
  @retval Others            Failed to create MADT

 **/
EFI_STATUS
EFIAPI
CreateMadt (
  IN  UINT8     *TableAddress,
  IN  UINT32     TableLength
  )
{
  EFI_STATUS                                           Status;
  UINT32                                               Length;
  UINT8                                               *Current;
  EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *Header;

  Current = TableAddress;
  ZeroMem (Current, TableLength);
  Header = (EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *)Current;

  // Create MADT Header
  Current += CreateMadtHeader (Header);

  // Create MADT ProcessorLocalApic
  Length = CreateProcessorLocalApic ((EFI_ACPI_3_0_PROCESSOR_LOCAL_APIC_STRUCTURE *)Current);
  if (Length == 0) {
    DEBUG ((DEBUG_ERROR, "Failed to create MADT ProcessorLocalApic!\n"));
    return EFI_ABORTED;
  }
  Current += Length;

  // Create MADT IoApic
  Current += CreateIoApic ((EFI_ACPI_3_0_IO_APIC_STRUCTURE *)Current);

  // Create MADT InterruptSourceOverrride
  Current += CreateInterruptSourceOverride ((EFI_ACPI_3_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE *)Current);

  // Create MADT LocalApicNmi
  Current += CreateLocalApicNmi ((EFI_ACPI_3_0_LOCAL_APIC_NMI_STRUCTURE *)Current);

  // Platform hook for Platform/Board specific MADT update
  Status = PlatformUpdateAcpiTable (TableAddress);
  ASSERT (!EFI_ERROR (Status));

  // Update Table length for checksum calculation
  Length = Current - TableAddress;
  ASSERT (Length == TableLength);
  Header->Header.Length = Length;

  return Status;
}
