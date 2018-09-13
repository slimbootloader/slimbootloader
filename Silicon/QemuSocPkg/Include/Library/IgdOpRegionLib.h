/** @file

  Copyright (c) 1999 - 2014, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:
  IgdOpRegionLib.h
Abstract:
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system BIOS, ASL code, and Graphics drivers.
  Supporting Specification: OpRegion / Software SCI SPEC 0.70
  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)
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

#endif
