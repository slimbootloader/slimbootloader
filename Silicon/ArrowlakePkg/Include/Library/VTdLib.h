/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _VT_D_LIB_H_
#define _VT_D_LIB_H_

#define VTD_ENGINE_NUMBER         2

#include <IndustryStandard/Acpi.h>

typedef UINT32  EFI_ACPI_TABLE_VERSION;
#define EFI_ACPI_TABLE_VERSION_1_0B (1 << 1)
#define EFI_ACPI_TABLE_VERSION_2_0  (1 << 2)
#define EFI_ACPI_TABLE_VERSION_3_0  (1 << 3)
/**
  Update DMAR ACPI table at the end of stage 2
**/
VOID
UpdateDmarAcpi (
  EFI_ACPI_DESCRIPTION_HEADER *Table
  );

/**
  Get VT-d BAR size.

  @return     VT-d BAR size
**/
UINT32
GetVtdBarSize (
  );

/**
  Read VTD Engine Base Address from VTD BAR Offsets.

  @param[in]  VtdEngineNumber        - Engine number for which VTD Base Adderess is required.

  @retval   VTD Engine Base Address
**/
UINT32
ReadVtdBaseAddress (
  IN UINT8        VtdEngineNumber
  );

typedef enum {
  IGD_VTD = 0, // Individual VT-d for GFX device
  IOP_VTD = 1  // Global VT-d for Segment 0
} VTD_ENGINE;

/**
  The functions returns the IOMMU instance which supports a given PCIe device.

  @attention
  The function doesn't check if a given PCIe device exists or is enabled. This is
  a responsibility of a caller.

  @param[in]  Bus          PCIe Bus number
  @param[in]  Device       PCIe Device number
  @param[in]  Function     PCIe Function number

  @retval  VT-d engine number
**/
VTD_ENGINE
GetVtdEngineForPciDevice (
  IN UINT8  Bus,
  IN UINT8  Device,
  IN UINT8  Function
  );

/**
  The functions returns the properties for a given PCIe device.

  @attention
  The function doesn't check if a given PCIe device exists or is enabled. This is
  a responsibility of a caller.

  @param[in]  Bus          PCIe Bus number
  @param[in]  Device       PCIe Device number
  @param[in]  Function     PCIe Function number

  @retval  PCIe device properties
**/
UINT8
GetPropertiesForPciDevice (
  IN UINT8  Bus,
  IN UINT8  Device,
  IN UINT8  Function
  );

#endif
