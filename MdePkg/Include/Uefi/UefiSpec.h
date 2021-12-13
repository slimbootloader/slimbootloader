/** @file
  Include file that supports UEFI.

  This include file must contain things defined in the UEFI 2.7 specification.
  If a code construct is defined in the UEFI 2.7 specification it must be included
  by this include file.

Copyright (c) 2006 - 2021, Intel Corporation. All rights reserved.<BR>
Portions Copyright (c) 2020, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __UEFI_SPEC_H__
#define __UEFI_SPEC_H__

#include <Uefi/UefiMultiPhase.h>

#include <Protocol/DevicePath.h>

///
/// Contains a set of GUID/pointer pairs comprised of the ConfigurationTable field in the
/// EFI System Table.
///
typedef struct {
  ///
  /// The 128-bit GUID value that uniquely identifies the system configuration table.
  ///
  EFI_GUID                          VendorGuid;
  ///
  /// A pointer to the table associated with VendorGuid.
  ///
  VOID                              *VendorTable;
} EFI_CONFIGURATION_TABLE;

#include <Uefi/UefiGpt.h>

#endif
