/**@file
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system Bootloader, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface
  Supporting Specification: OpRegion / Software SCI SPEC 0.70

  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)
    IPU:        Image Processing Unit

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _IGD_OP_REGION_LIB_H_
#define _IGD_OP_REGION_LIB_H_

/**

  Graphics OpRegion / Software SCI driver installation function.

  @param Void

  @retval EFI_SUCCESS     The driver installed without error.
  @retval EFI_ABORTED     The driver encountered an error and could not complete
                          installation of the ACPI tables.
**/
EFI_STATUS
IgdOpRegionInit (
  void
  );

/**
  Update VBT data in Igd Op Region.

  @param[in] VbtTablePtr      Pointer to VBT_TABLE_DATA

  @retval    EFI_SUCCESS      VBT data was returned.
  @exception EFI_ABORTED      Intel VBT not found.
**/
EFI_STATUS
UpdateVbt (
  IN VOID   *VbtTablePtr
  );

#endif
