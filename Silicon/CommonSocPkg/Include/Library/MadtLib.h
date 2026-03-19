/** @file
  Library instances for Multiple APIC Description Table (MADT) which
  describes the APIC topology of the system.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _MADT_LIB_H_
#define _MADT_LIB_H_

#include <IndustryStandard/Acpi64.h>

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
  );

/**
  Add MADT Local APIC structures to an ACPI table.

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );
#endif

