/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __VTD_LIB_H__
#define __VTD_LIB_H__

#define VTD_RMRR_USB_LENGTH                   0x20000

#define R_SA_MCHBAR_VTD1_OFFSET               0x6C88  ///< DMA Remapping HW UNIT1 for IGD
#define R_SA_MCHBAR_VTD2_OFFSET               0x6C80  ///< DMA Remapping HW UNIT2 for all other - PEG, USB, SATA etc

#include <IndustryStandard/Acpi.h>
#include <Library/DmaRemappingTable.h>

/**
  Update DMAR ACPI table at the end of stage 2
**/
VOID
UpdateDmarAcpi (
  EFI_ACPI_DESCRIPTION_HEADER *Table
  );

#endif /* __VTD_LIB_H__ */
