/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
