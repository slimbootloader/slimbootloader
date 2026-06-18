/** @file
  Library instances for ACPI Watchdog Action Table (WDAT) which describes
  the watchdog timer hardware to the operating system as defined by the
  Microsoft Hardware Watchdog Timers Design Specification.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _WDAT_LIB_H_
#define _WDAT_LIB_H_

#include <IndustryStandard/Acpi65.h>

//
// WDAT structure types are not provided by MdePkg's IndustryStandard headers,
// so the library declares them here for use by all consumers.
//
#pragma pack(1)
///
/// Watchdog Action Table.
///
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER    Header;
  UINT32                         WatchdogHeaderLength;
  UINT16                         PCISegment;
  UINT8                          PCIBusNumber;
  UINT8                          PCIDeviceNumber;
  UINT8                          PCIFunctionNumber;
  UINT8                          Reserved_45[3];
  UINT32                         TimerPeriod;
  UINT32                         MaxCount;
  UINT32                         MinCount;
  UINT8                          WatchdogFlags;
  UINT8                          Reserved_61[3];
  UINT32                         NumberWatchdogInstructionEntries;
} EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE;

///
/// Watchdog Instruction Entry.
///
typedef struct {
  UINT8                                     WatchdogAction;
  UINT8                                     InstructionFlags;
  UINT8                                     Reserved_2[2];
  EFI_ACPI_6_5_GENERIC_ADDRESS_STRUCTURE    RegisterRegion;
  UINT32                                    Value;
  UINT32                                    Mask;
} EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY;
#pragma pack()

///
/// WDAT Revision (per Microsoft WDAT specification).
///
#define EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE_REVISION  0x01

//
// WDAT 1.0 Watchdog Flags
//
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ENABLED                 0x1
#define EFI_ACPI_WDAT_1_0_WATCHDOG_STOPPED_IN_SLEEP_STATE  0x80

//
// WDAT 1.0 Watchdog Actions
//
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_RESET                           0x1
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_CURRENT_COUNTDOWN_PERIOD  0x4
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_COUNTDOWN_PERIOD          0x5
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_COUNTDOWN_PERIOD            0x6
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_RUNNING_STATE             0x8
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_RUNNING_STATE               0x9
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_STOPPED_STATE             0xA
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_STOPPED_STATE               0xB
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_REBOOT                    0x10
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_REBOOT                      0x11
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_SHUTDOWN                  0x12
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_SHUTDOWN                    0x13
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_WATCHDOG_STATUS           0x20
#define EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_WATCHDOG_STATUS             0x21

//
// WDAT 1.0 Watchdog Action Entry Instruction Flags
//
#define EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_VALUE         0x0
#define EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_COUNTDOWN     0x1
#define EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE        0x2
#define EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_COUNTDOWN    0x3
#define EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_PRESERVE_REGISTER  0x80

//
// Header-only WDAT template. Consumers copy this into their table buffer to
// seed the ACPI/WDAT header fields (Signature, Length, Revision, OEM/Creator
// IDs) before calling AddAcpiWdatHdr() and appending instruction entries.
//
extern CONST EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE  mAcpiWdatTableTemplate;

/**
  Initialize an ACPI WDAT (Watchdog Action Table) header.

  Fills the WDAT-specific header fields (PCI segment/bus/device/function,
  timer period, max/min count, and watchdog flags) and stamps the OEM ID
  from PCD. The instruction entry array starts empty; callers append
  entries via AddWdatInstructionEntry().

  @param[out] AcpiHeader        Pointer to the ACPI table header to be initialized.
                                Must point to a buffer holding a valid
                                EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE that has
                                already been seeded from mAcpiWdatTableTemplate.
  @param[in]  PciSegment        PCI segment number of the watchdog (or 0xFFFF if N/A).
  @param[in]  PciBusNumber      PCI bus number of the watchdog (or 0xFF if N/A).
  @param[in]  PciDeviceNumber   PCI device number of the watchdog (or 0xFF if N/A).
  @param[in]  PciFunctionNumber PCI function number of the watchdog (or 0xFF if N/A).
  @param[in]  TimerPeriod       Watchdog counter period in milliseconds.
  @param[in]  MaxCount          Maximum value the watchdog counter can hold.
  @param[in]  MinCount          Minimum value the watchdog counter can hold.
  @param[in]  WatchdogFlags     WDAT-spec watchdog flags
                                (e.g. EFI_ACPI_WDAT_1_0_WATCHDOG_ENABLED).

  @retval EFI_SUCCESS             Header initialized successfully.
  @retval EFI_INVALID_PARAMETER   AcpiHeader is NULL.
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
  );

/**
  Append a Watchdog Action Instruction Entry to a WDAT table.

  Appends one EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY
  immediately after the last existing entry, increments
  NumberWatchdogInstructionEntries, and grows AcpiHeader->Length.

  @param[in,out] AcpiHeader        Pointer to the WDAT table to extend.
  @param[in]     WatchdogAction    Action this entry implements
                                   (EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_*).
  @param[in]     InstructionFlags  Instruction flags
                                   (EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_*).
  @param[in]     AddressSpaceId    Generic Address Structure address space
                                   (EFI_ACPI_6_5_SYSTEM_IO/MEMORY/...).
  @param[in]     RegisterBitWidth  Register bit width.
  @param[in]     RegisterBitOffset Register bit offset.
  @param[in]     AccessSize        Register access size
                                   (EFI_ACPI_6_5_BYTE/WORD/DWORD/QWORD).
  @param[in]     Address           Register address (full 64-bit address).
  @param[in]     Value             Value written / expected.
  @param[in]     Mask              Mask applied before compare/write.

  @retval EFI_SUCCESS             Entry appended successfully.
  @retval EFI_INVALID_PARAMETER   AcpiHeader is NULL, or Length is inconsistent
                                  with the header plus existing instruction
                                  entries.
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
  );

/**
  Remove the last Watchdog Action Instruction Entry from a WDAT table.

  Decrements NumberWatchdogInstructionEntries and shrinks AcpiHeader->Length
  by sizeof(EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY).
  WatchdogHeaderLength is left unchanged: per the WDAT spec it describes only
  the fixed WDAT header size (excluding instruction entries), so it must stay
  constant for OS parsers to locate the instruction-entry array correctly.

  Useful when a platform must conditionally drop an entry (for example, the
  SET_REBOOT entry when the TCO NO_REBOOT bit is read-only); the entry must
  therefore be the last one in the array when this routine is called.

  @param[in,out] AcpiHeader  Pointer to the WDAT table to shrink.

  @retval EFI_SUCCESS             Entry removed successfully.
  @retval EFI_INVALID_PARAMETER   AcpiHeader is NULL, or Length is too small to
                                  contain the header plus the entry to remove.
  @retval EFI_NOT_FOUND           No instruction entries to remove.
**/
EFI_STATUS
EFIAPI
RemoveLastWdatInstructionEntry (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader
  );

/**
  Overwrite the RegisterRegion of an existing WDAT instruction entry.

  Used by platforms that must redirect a specific watchdog action to an
  alternate address space at runtime (for example, mapping QUERY/SET
  WATCHDOG STATUS to a memory-backed status byte).

  @param[in,out] AcpiHeader        Pointer to the WDAT table.
  @param[in]     EntryIndex        Zero-based index of the entry to patch.
                                   Must be < NumberWatchdogInstructionEntries.
  @param[in]     AddressSpaceId    New Generic Address Structure address space.
  @param[in]     RegisterBitWidth  New register bit width.
  @param[in]     RegisterBitOffset New register bit offset.
  @param[in]     AccessSize        New register access size.
  @param[in]     Address           New register address.
  @param[in]     Value             New value field.
  @param[in]     Mask              New mask field.

  @retval EFI_SUCCESS             Entry patched successfully.
  @retval EFI_INVALID_PARAMETER   AcpiHeader is NULL, or Length does not span
                                  the entry to patch.
  @retval EFI_NOT_FOUND           EntryIndex is out of range.
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
  );

/**
  Add a base address to every System I/O instruction entry in a WDAT table.

  Many platforms describe watchdog registers as offsets from a chipset base
  (for example, the TCO IO base). After all entries have been appended,
  this routine adds IoBase to RegisterRegion.Address for each entry whose
  AddressSpaceId == EFI_ACPI_6_5_SYSTEM_IO. Entries in other address spaces
  are left unchanged.

  @param[in,out] AcpiHeader  Pointer to the WDAT table.
  @param[in]     IoBase      Base address to add to System I/O entries.

  @retval EFI_SUCCESS             Rebase applied successfully.
  @retval EFI_INVALID_PARAMETER   AcpiHeader is NULL, or Length does not span
                                  the claimed instruction entries.
**/
EFI_STATUS
EFIAPI
RebaseWdatIoEntries (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT64                       IoBase
  );

#endif // _WDAT_LIB_H_
