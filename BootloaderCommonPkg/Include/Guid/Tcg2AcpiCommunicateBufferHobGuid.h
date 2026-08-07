/** @file
  This file defines the hob structure for the TCG2 ACPI communicate buffer.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __TCG2_ACPI_COMMUNICATE_BUFFER_HOB_GUID_H__
#define __TCG2_ACPI_COMMUNICATE_BUFFER_HOB_GUID_H__

///
/// TCG2 ACPI communicate buffer Hob GUID
///
extern EFI_GUID gEdkiiTcg2AcpiCommunicateBufferHobGuid;

#pragma pack(1)

typedef struct {
  EFI_PHYSICAL_ADDRESS  Tcg2AcpiCommunicateBuffer;
  UINTN                 Pages;
} TCG2_ACPI_COMMUNICATE_BUFFER;

///
/// Placeholder structure for TCG NVS region.
/// The actual layout is defined by the TCG2 ACPI driver.
///
typedef struct {
  UINT8  Reserved[0x100];
} TCG_NVS;

#pragma pack()

#endif
