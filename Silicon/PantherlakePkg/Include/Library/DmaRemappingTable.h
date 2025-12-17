/** @file

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _DMA_REMAPPING_TABLE_H_
#define _DMA_REMAPPING_TABLE_H_

#include <Base.h>
#include "DmaRemappingReportingTablePtl.h"
#include <IndustryStandard/Acpi.h>

#pragma pack(1)
///
/// DMAR table signature
///
#define EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE   0x52414D44  ///< "DMAR"
#define EFI_ACPI_DMAR_TABLE_REVISION        1
#define EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH  0x10


/**
  DMA-remapping hardware unit definition (DRHD) structure is defined in
  section 8.3. This uniquely represents a remapping hardware unit present
  in the platform. There must be at least one instance of this structure
  for each PCI segment in the platform.
**/
typedef struct {

  EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER DeviceScopeStructureHeader;
  EFI_ACPI_DMAR_PCI_PATH                      PciPath;     // device, function
} EFI_ACPI_DEV_SCOPE_STRUCTURE;

typedef struct {
  EFI_ACPI_DMAR_DRHD_HEADER               DrhdHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE            DeviceScope[1];
} EFI_ACPI_DRHD_ENGINE1_STRUCT;

typedef struct {
  EFI_ACPI_DMAR_DRHD_HEADER               DrhdHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE            DeviceScope[4];
} EFI_ACPI_DRHD_ENGINE2_STRUCT;

typedef struct {
  EFI_ACPI_DMAR_DRHD_HEADER               DrhdHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE            DeviceScope[2];
} EFI_ACPI_DRHD_ENGINE3_STRUCT;

typedef struct {
  EFI_ACPI_DMAR_SATC_HEADER     SatcHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[3];
} EFI_ACPI_SATC_STRUCT;

typedef struct {
  EFI_ACPI_DMAR_SIDP_HEADER     SidpHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[3];
} EFI_ACPI_SIDP_STRUCT;

typedef struct {

  EFI_ACPI_DMAR_HEADER          DmarHeader;
  EFI_ACPI_DRHD_ENGINE1_STRUCT  DrhdEngine1;
  EFI_ACPI_DRHD_ENGINE2_STRUCT  DrhdEngine2;
  EFI_ACPI_DRHD_ENGINE3_STRUCT  DrhdEngine3;
  #if 0
  EFI_ACPI_RMRR_IGD_STRUC       RmrrIgd;
  #endif
  EFI_ACPI_SATC_STRUCT          Satc;
  EFI_ACPI_SIDP_STRUCT          Sidp;
} EFI_ACPI_DMAR_TABLE;

#pragma pack()

#endif
