/** @file
  Library instance for ACPI Watchdog Action Table (WDAT).

  Provides a header-only template plus helpers to dynamically build a
  WDAT table at runtime.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/WdatLib.h>

//
// ACPI WDAT table template.
//
// Header-only template: Length covers just the WDAT header. Platform code
// appends instruction entries at runtime, growing Header.Length and
// NumberWatchdogInstructionEntries as it goes.
//
CONST
EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE mAcpiWdatTableTemplate = {
  .Header.Signature                 = EFI_ACPI_6_5_WATCHDOG_ACTION_TABLE_SIGNATURE,
  .Header.Length                    = sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE),
  .Header.Revision                  = EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE_REVISION,
  .Header.OemTableId                = FixedPcdGet64 (PcdAcpiDefaultOemTableId),
  .Header.OemRevision               = FixedPcdGet32 (PcdAcpiDefaultOemRevision),
  .Header.CreatorId                 = FixedPcdGet32 (PcdAcpiDefaultCreatorId),
  .Header.CreatorRevision           = FixedPcdGet32 (PcdAcpiDefaultCreatorRevision),
  .WatchdogHeaderLength             = sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE) - sizeof (EFI_ACPI_DESCRIPTION_HEADER),
  .PCISegment                       = 0xFFFF,
  .PCIBusNumber                     = 0xFF,
  .PCIDeviceNumber                  = 0xFF,
  .PCIFunctionNumber                = 0xFF,
  .TimerPeriod                      = 0,
  .MaxCount                         = 0,
  .MinCount                         = 0,
  .WatchdogFlags                    = 0,
  .NumberWatchdogInstructionEntries = 0,
};

/**
  Initialize an ACPI WDAT (Watchdog Action Table) header.

  See WdatLib.h for parameter documentation.
**/
EFI_STATUS
EFIAPI
AddAcpiWdatHdr (
  OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN  UINT16                       PciSegment,
  IN  UINT8                        PciBusNumber,
  IN  UINT8                        PciDeviceNumber,
  IN  UINT8                        PciFunctionNumber,
  IN  UINT32                       TimerPeriod,
  IN  UINT32                       MaxCount,
  IN  UINT32                       MinCount,
  IN  UINT8                        WatchdogFlags
  )
{
  EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE  *Wdat;

  if (AcpiHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Wdat = (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE *)AcpiHeader;
  CopyMem (Wdat->Header.OemId, FixedPcdGetPtr (PcdAcpiDefaultOemId), sizeof (Wdat->Header.OemId));

  Wdat->WatchdogHeaderLength             = sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE) - sizeof (EFI_ACPI_DESCRIPTION_HEADER);
  Wdat->PCISegment                       = PciSegment;
  Wdat->PCIBusNumber                     = PciBusNumber;
  Wdat->PCIDeviceNumber                  = PciDeviceNumber;
  Wdat->PCIFunctionNumber                = PciFunctionNumber;
  Wdat->TimerPeriod                      = TimerPeriod;
  Wdat->MaxCount                         = MaxCount;
  Wdat->MinCount                         = MinCount;
  Wdat->WatchdogFlags                    = WatchdogFlags;
  Wdat->NumberWatchdogInstructionEntries = 0;

  ZeroMem (Wdat->Reserved_45, sizeof (Wdat->Reserved_45));
  ZeroMem (Wdat->Reserved_61, sizeof (Wdat->Reserved_61));

  DEBUG ((DEBUG_INFO,
    "WDAT: Hdr Seg=0x%04x B/D/F=0x%02x/0x%02x/0x%02x Period=0x%x Max=0x%x Min=0x%x Flags=0x%x\n",
    PciSegment, PciBusNumber, PciDeviceNumber, PciFunctionNumber,
    TimerPeriod, MaxCount, MinCount, WatchdogFlags));

  return EFI_SUCCESS;
}

/**
  Append a Watchdog Action Instruction Entry to a WDAT table.

  See WdatLib.h for parameter documentation.
**/
EFI_STATUS
EFIAPI
AddWdatInstructionEntry (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT8                        WatchdogAction,
  IN     UINT8                        InstructionFlags,
  IN     UINT8                        AddressSpaceId,
  IN     UINT8                        RegisterBitWidth,
  IN     UINT8                        RegisterBitOffset,
  IN     UINT8                        AccessSize,
  IN     UINT64                       Address,
  IN     UINT32                       Value,
  IN     UINT32                       Mask
  )
{
  EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE                              *Wdat;
  EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY  *Entry;

  if (AcpiHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Wdat  = (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE *)AcpiHeader;

  //
  // Length must cover the fixed header plus all existing entries so the new
  // entry is written past them; otherwise the table is inconsistent and the
  // write could land on top of an existing entry or before the header end.
  // This also rejects a zero Length.
  //
  if (AcpiHeader->Length !=
        sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE) +
        (UINT64)Wdat->NumberWatchdogInstructionEntries *
        sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY)) {
    return EFI_INVALID_PARAMETER;
  }

  Entry = (EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY *)
            ((UINT8 *)AcpiHeader + AcpiHeader->Length);

  ZeroMem (Entry, sizeof (*Entry));
  Entry->WatchdogAction                = WatchdogAction;
  Entry->InstructionFlags              = InstructionFlags;
  Entry->RegisterRegion.AddressSpaceId = AddressSpaceId;
  Entry->RegisterRegion.RegisterBitWidth  = RegisterBitWidth;
  Entry->RegisterRegion.RegisterBitOffset = RegisterBitOffset;
  Entry->RegisterRegion.AccessSize        = AccessSize;
  Entry->RegisterRegion.Address           = Address;
  Entry->Value                            = Value;
  Entry->Mask                             = Mask;

  Wdat->NumberWatchdogInstructionEntries++;
  AcpiHeader->Length += sizeof (*Entry);

  DEBUG ((DEBUG_INFO,
    "WDAT: Entry Action=0x%02x InsFlags=0x%02x AddrSpc=0x%02x Width=%d Offset=%d Access=%d Addr=0x%Lx Val=0x%x Mask=0x%x\n",
    WatchdogAction, InstructionFlags, AddressSpaceId,
    RegisterBitWidth, RegisterBitOffset, AccessSize,
    Address, Value, Mask));

  return EFI_SUCCESS;
}

/**
  Remove the last Watchdog Action Instruction Entry from a WDAT table.

  See WdatLib.h for parameter documentation.
**/
EFI_STATUS
EFIAPI
RemoveLastWdatInstructionEntry (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader
  )
{
  EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE  *Wdat;
  UINT32                              EntrySize;

  if (AcpiHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Wdat = (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE *)AcpiHeader;
  if (Wdat->NumberWatchdogInstructionEntries == 0) {
    return EFI_NOT_FOUND;
  }

  EntrySize = sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY);

  //
  // Length must at least cover the fixed WDAT header plus the entry being
  // removed; otherwise the table is malformed and shrinking it would
  // underflow Header.Length below the header size.
  //
  if (AcpiHeader->Length < sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE) + EntrySize) {
    return EFI_INVALID_PARAMETER;
  }

  Wdat->NumberWatchdogInstructionEntries--;
  Wdat->Header.Length -= EntrySize;

  DEBUG ((DEBUG_INFO,
    "WDAT: Removed last entry; remaining=%d Length=0x%x\n",
    Wdat->NumberWatchdogInstructionEntries, Wdat->Header.Length));

  return EFI_SUCCESS;
}

/**
  Overwrite the RegisterRegion of an existing WDAT instruction entry.

  See WdatLib.h for parameter documentation.
**/
EFI_STATUS
EFIAPI
PatchWdatInstructionEntry (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT32                       EntryIndex,
  IN     UINT8                        AddressSpaceId,
  IN     UINT8                        RegisterBitWidth,
  IN     UINT8                        RegisterBitOffset,
  IN     UINT8                        AccessSize,
  IN     UINT64                       Address,
  IN     UINT32                       Value,
  IN     UINT32                       Mask
  )
{
  EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE                              *Wdat;
  EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY  *Entry;

  if (AcpiHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Wdat = (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE *)AcpiHeader;
  if (EntryIndex >= Wdat->NumberWatchdogInstructionEntries) {
    return EFI_NOT_FOUND;
  }

  //
  // Ensure Length actually spans the entry being patched before indexing
  // into the array, in case the entry count and Length are inconsistent.
  //
  if (AcpiHeader->Length <
        sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE) +
        ((UINT64)EntryIndex + 1) *
        sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY)) {
    return EFI_INVALID_PARAMETER;
  }

  Entry = (EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY *)
            ((UINT8 *)AcpiHeader + sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE));
  Entry += EntryIndex;

  Entry->RegisterRegion.AddressSpaceId     = AddressSpaceId;
  Entry->RegisterRegion.RegisterBitWidth   = RegisterBitWidth;
  Entry->RegisterRegion.RegisterBitOffset  = RegisterBitOffset;
  Entry->RegisterRegion.AccessSize         = AccessSize;
  Entry->RegisterRegion.Address            = Address;
  Entry->Value                             = Value;
  Entry->Mask                              = Mask;

  return EFI_SUCCESS;
}

/**
  Add a base address to every System I/O instruction entry in a WDAT table.

  See WdatLib.h for parameter documentation.
**/
EFI_STATUS
EFIAPI
RebaseWdatIoEntries (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT64                       IoBase
  )
{
  EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE                              *Wdat;
  EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY  *Entry;
  UINT32                                                          Index;

  if (AcpiHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Wdat  = (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE *)AcpiHeader;

  //
  // Ensure Length actually spans all the entries the header claims before
  // iterating, in case the entry count and Length are inconsistent.
  //
  if (AcpiHeader->Length <
        sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE) +
        (UINT64)Wdat->NumberWatchdogInstructionEntries *
        sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY)) {
    return EFI_INVALID_PARAMETER;
  }

  Entry = (EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY *)
            ((UINT8 *)AcpiHeader + sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE));

  for (Index = 0; Index < Wdat->NumberWatchdogInstructionEntries; Index++) {
    if (Entry[Index].RegisterRegion.AddressSpaceId == EFI_ACPI_6_5_SYSTEM_IO) {
      Entry[Index].RegisterRegion.Address += IoBase;
    }
  }

  return EFI_SUCCESS;
}
