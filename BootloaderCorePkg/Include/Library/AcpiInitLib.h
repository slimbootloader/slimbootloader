/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ACPI_INIT_LIB_H__
#define __ACPI_INIT_LIB_H__

#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Acpi.h>

typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER     Header;
  UINT32                                          Reserved;
  ///
  /// 64-bit processor-relative physical address of the SBL Performance Table.
  ///
  UINT64                                          SblPerfTablePointer;
} SBL_PERFORMANCE_TABLE_POINTER_RECORD;

typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER     Header;
  UINT32                                          Reserved;
  ///
  /// Time taken for Stage 1 execution in nanoseconds
  ///
  UINT64                                          Stage1Time;
  ///
  /// Time taken for Stage 2 execution in nanoseconds
  ///
  UINT64                                          Stage2Time;
  ///
  /// Time taken for OsLoader execution in nanoseconds
  ///
  UINT64                                          OsLoaderTime;

} SBL_PERFORMANCE_RECORD;

#pragma pack(1)
///
/// Firmware Performance Data Table.
///
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER                             Header;                 ///< Common ACPI description table header.
  EFI_ACPI_5_0_FPDT_BOOT_PERFORMANCE_TABLE_POINTER_RECORD BootPointerRecord;      ///< Basic Boot Performance Table Pointer record.
  EFI_ACPI_5_0_FPDT_S3_PERFORMANCE_TABLE_POINTER_RECORD   S3PointerRecord;        ///< S3 Performance Table Pointer record.
  SBL_PERFORMANCE_TABLE_POINTER_RECORD                    SblPerfPointerRecord;   ///< SBL Performance Table Pointer record.
} FIRMWARE_PERFORMANCE_TABLE;

///
/// Basic Boot Performance Data Table.
/// This structure contains BasicBoot performance record.
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_TABLE_HEADER   Header;     ///< Common ACPI table header.
  EFI_ACPI_5_0_FPDT_FIRMWARE_BASIC_BOOT_RECORD BasicBoot;  ///< Basic Boot Resume performance record.
  //
  // one or more boot performance records.
  //
} BOOT_PERFORMANCE_TABLE;

///
/// S3 Performance Data Table.
/// This structure contains S3 performance records which will be updated in S3
/// suspend and S3 resume boot path.
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_TABLE_HEADER  Header;    ///< Common ACPI table header.
  EFI_ACPI_5_0_FPDT_S3_RESUME_RECORD          S3Resume;  ///< Basic S3 Resume performance record.
} S3_PERFORMANCE_TABLE;

///
/// SBL Performance Data Table.
/// This structure contains SBL performance records like Stage1 done time,
/// Stage2 done time, OsLoader done time
#define SBL_PERFORMANCE_TABLE_SIGNATURE       SIGNATURE_32('S', 'B', 'L', 'T')
#define SBL_PERFORMACE_TABLE_TYPE             0x3000
#define SBL_PERFORMACE_TABLE_REVISION         0x01
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_TABLE_HEADER  Header;         ///< Common ACPI table header.
  SBL_PERFORMANCE_RECORD                      SblPerfRecord;  ///< SBL performance record.
} SBL_PERFORMANCE_TABLE;

#pragma pack()

/**
  Locate the ACPI wake vector from FACS and jump to it on S3 resume.

  This function retrieves the FACS address from the TSEG S3 communication
  area (stored during normal boot via AppendS3Info), validates the FACS
  signature, extracts the FirmwareWakingVector, and transfers control to
  the OS wake entry point. This function does not return on success.

  @param[in]  FacsAddress   Address of FACS saved from normal boot.

**/
VOID
EFIAPI
FindAcpiWakeVectorAndJump (
  VOID    *FacsAddress
  );

/**
  This function creates necessary ACPI tables and puts the RSDP
  table in F segment so that OS can locate it.

  @param[in]  AcpiMemTop      ACPI memory top address.

  @retval     EFI_SUCCESS     ACPI tables are created successfully.
              EFI_NOT_FOUND   Required ACPI tables could not be found.

**/
EFI_STATUS
EFIAPI
AcpiInit (
  IN  UINT32    *AcpiMemTop
  );

/**
  Update ACPI FPDT S3 performance record table.

  @param[in] AcpiTableBase     ACPI base address

  @retval EFI_SUCCESS          Update the S3 performance table successfully.
  @retval Others               Failed to update the table.
 **/
EFI_STATUS
EFIAPI
UpdateFpdtS3Table (
  IN  UINT32                   AcpiTableBase
  );

/**
  This function updates SBL performance table in FPDT

  @retval  EFI_SUCCESS if operation is successful, EFI_NOT_FOUND if
           performance HOB is not found

**/
EFI_STATUS
EFIAPI
UpdateFpdtSblTable (
  VOID
  );

/**
  Discover the S3 required info via the live ACPI tables and save it
  to the TSEG S3 communication area for use on S3 resume.

  Save AcpiBase and FACS address into TSEG so that it can be retrieved
  on S3 resume without trusting DRAM-resident ACPI tables. The storage
  method depends on PcdBuildSmmHobs:
    BIT1 - copy BL_ACPI_S3_INFO at a fixed offset after PLD_TO_BL_SMM_INFO
           and per-CPU SMM base entries in TSEG
    BIT0 - append a BL_ACPI_S3_INFO structure via AppendS3Info()

  @retval EFI_SUCCESS         ACPI S3 data saved successfully.
  @retval EFI_NOT_FOUND       ACPI S3 data not found in the ACPI tables.
  @retval EFI_DEVICE_ERROR    CPU info unavailable.
  @retval EFI_UNSUPPORTED     No supported storage method available.

**/
EFI_STATUS
EFIAPI
SaveAcpiDataForS3 (
  VOID
  );

/**
  Retrieve the FACS address from the TSEG S3 communication area.

  This function reads the FACS physical address previously saved by
  SaveAcpiDataForS3() during normal boot. It is called on S3 resume
  to locate the FACS without relying on DRAM-resident ACPI tables.

  @retval  Non-zero   Physical address of the FACS table.
  @retval  0          FACS address not found.

**/
UINT64
EFIAPI
GetFacsAddressForS3 (
  VOID
  );

/**
  Retrieve the ACPI base address from the TSEG S3 communication area.

  Calls GetAcpiS3Info() and returns the AcpiBase field. Called on S3 resume
  to locate the ACPI tables without relying on DRAM-resident data.

  @retval  Non-zero   Physical address of the ACPI RSDP.
  @retval  0          ACPI base address not found.

**/
UINT64
EFIAPI
GetAcpiBaseForS3 (
  VOID
  );

/**
  Find the FACS physical address by walking the live ACPI XSDT.

  @param[in]  AcpiTableBase   Physical address of the RSDP.

  @retval  Non-zero   Physical address of the FACS structure.
  @retval  0          FACS not found.

**/
UINT64
EFIAPI
FindAcpiFacsAddress (
  IN  UINT32    AcpiTableBase
  );

#endif
