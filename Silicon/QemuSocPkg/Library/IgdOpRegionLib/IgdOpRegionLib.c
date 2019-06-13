/**@file

  Copyright (c) 2011 - 2014, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:
  IgdOpRegionLib.c
Abstract:
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system BIOS, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface
  Supporting Specification: OpRegion / Software SCI SPEC 0.70
  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)
**/

/**
  Graphics OpRegion / Software SCI driver installation function.

  @param ImageHandle     Handle for this drivers loaded image protocol.
  @param SystemTable     EFI system table.

  @retval EFI_SUCCESS    The driver installed without error.
  @retval EFI_ABORTED    The driver encountered an error and could not complete
                         installation of the ACPI tables.

**/
EFI_STATUS
IgdOpRegionInit (
  VOID
  )
{
  return EFI_SUCCESS;
}
