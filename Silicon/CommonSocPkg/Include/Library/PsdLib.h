/** @file
  Header file for PsdLib

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Acpi.h>

#ifndef _ACPI_PSD_TABLE_UPDATE_H_
#define _ACPI_PSD_TABLE_UPDATE_H_


#define EFI_ACPI_PSD_SIGNATURE            SIGNATURE_32('P', 'S', 'D', 'S')  // 'PSDS'
#define PSDS_EFI_ACPI_OEM_ID              "INTEL "  // OEMID 6 bytes long
#define EFI_ACPI_PSD_TABLE_REVISION       0x1
#define EFI_ACPI_PSD_FW_VENDOR_SIZE       16
#define EFI_ACPI_PSD_FW_VENDOR            "INTEL           "   // 16 bytes long

// ACPI table structure definitions for legacy platforms
#define PSDS_EFI_ACPI_OEM_TABLE_ID        SIGNATURE_64('E','D','K','2',' ',' ',' ',' ') // OEM table id 8 bytes long
#define PSDS_EFI_ACPI_OEM_REVISION        0x00000005
#define PSDS_EFI_ACPI_CREATOR_ID          SIGNATURE_32('I','N','T','L')
#define PSDS_EFI_ACPI_CREATOR_REVISION    0x0100000D

#pragma pack(1)

//
//PSDT Hash indices
//
typedef union {
  struct {
    UINT16      Enabled:1;
    UINT16      SwCrtl:1;
    UINT16      PRMRR:4;
    UINT16      FlcEnabled:1;
    UINT16      Reserved:9;
  } Bits;
  UINT16  Sgx16;
} PSD_SGX_CAPS;

//
//FW Version information
//
typedef struct {
  UINT32 CodeMinor;
  UINT32 CodeMajor;
  UINT32 CodeBuildNo;
  UINT32 CodeHotFix;
} SEC_VERSION_INFO;

//
//PSD Version information
//
typedef struct {
  UINT16 PsdVerMinor;
  UINT16 PsdVerMajor;
} PSD_VERSION_INFO;

//
// PSDT Table which will be pupulated to OS,
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER      Header;
  PSD_VERSION_INFO                 PsdVersion;
  UINT32                           CsmeSecCapabilities; //in APL it is called TXE.
  UINT16                           SgxCapabilities;
  SEC_VERSION_INFO                 FwVer;
  UINT8                            FwVendor[EFI_ACPI_PSD_FW_VENDOR_SIZE];
  UINT8                            EomState;
  UINT8                            SecureBoot;
  UINT8                            MeasuredBoot;
  UINT8                            HwrotType;
} EFI_ACPI_PSD_TABLE;

#pragma pack()

/**
  Initialize and add an ACPI PSD (Platform Security Discovery) table.

  This function initializes the ACPI PSD table with standard fields such as
  signature, length, revision, OEM information, and creator details.

  @param[in]                   Table  Pointer to the ACPI table to be initialized.

  @retval EFI_SUCCESS            The header was initialized successfully.
  @retval EFI_INVALID_PARAMETER  Invalid parameter provided.
**/
EFI_STATUS
EFIAPI
UpdateAcpiPsdTable (
  IN VOID*               Table
  );

#endif //_ACPI_PSD_TABLE_UPDATE_H_
