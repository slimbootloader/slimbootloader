/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _DMA_REMAPPING_TABLE_H_
#define _DMA_REMAPPING_TABLE_H_

#include <Base.h>
#include "DmaRemappingReportingTableMtl.h"
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
  EFI_ACPI_DMAR_STRUCTURE_HEADER    Header;

  /**
    - Bit[0]: INCLUDE_PCI_ALL
              - If Set, this remapping hardware unit has under its scope all
                PCI compatible devices in the specified Segment, except devices
                reported under the scope of other remapping hardware units for
                the same Segment.
              - If Clear, this remapping hardware unit has under its scope only
                devices in the specified Segment that are explicitly identified
                through the DeviceScope field.
    - Bits[7:1] Reserved.
  **/
  UINT8     Flags;

  /**
    - Bit[3:0]: Indicates the size of the remapping hardware register set for this
                remapping unit. If the value in this field is N, the size of the
                register set is 2^N 4 KB pages (2^(N+12) bytes).
    - Bits[7:4] Reserved.
  **/
  UINT8     Size;
  ///
  /// The PCI Segment associated with this unit.
  ///
  UINT16    SegmentNumber;
  ///
  /// Base address of remapping hardware register-set for this unit.
  ///
  UINT64    RegisterBaseAddress;
} EFI_ACPI_DMAR_DRHD_HEADER_OVERWRITE;

typedef struct {
  EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER DeviceScopeStructureHeader;
  EFI_ACPI_DMAR_PCI_PATH                      PciPath;     // device, function
} EFI_ACPI_DEV_SCOPE_STRUCTURE;

typedef struct {
  EFI_ACPI_DMAR_DRHD_HEADER_OVERWRITE     DrhdHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE            DeviceScope[1];
} EFI_ACPI_DRHD_ENGINE1_STRUCT;

typedef struct {
  EFI_ACPI_DMAR_DRHD_HEADER_OVERWRITE     DrhdHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE            DeviceScope[2];
} EFI_ACPI_DRHD_ENGINE3_STRUCT;

typedef struct {
  EFI_ACPI_DMAR_RMRR_HEADER     RmrrHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[2];
} EFI_ACPI_RMRR_USB_STRUC;

typedef struct {
  EFI_ACPI_DMAR_RMRR_HEADER     RmrrHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[1];    // IGD
} EFI_ACPI_RMRR_IGD_STRUC;

typedef struct {
  EFI_ACPI_DMAR_SATC_HEADER     SatcHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[3];
} EFI_ACPI_SATC_STRUCT;

typedef struct {
  EFI_ACPI_DMAR_SIDP_HEADER     SidpHeader;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[3];
} EFI_ACPI_SIDP_STRUCT;

typedef struct {
  EFI_ACPI_DMAR_ANDD_HEADER     AnddHeader;
  UINT8                         AcpiObjectName[20];
} EFI_ACPI_ANDD_STRUC;

typedef struct {
  EFI_ACPI_DMAR_HEADER          DmarHeader;
  EFI_ACPI_DRHD_ENGINE1_STRUCT  DrhdEngine1;
  EFI_ACPI_DRHD_ENGINE3_STRUCT  DrhdEngine3;
  EFI_ACPI_SATC_STRUCT          Satc;
  EFI_ACPI_SIDP_STRUCT          Sidp;
} EFI_ACPI_DMAR_TABLE;

#pragma pack()

#endif
