/** @file

  Copyright (c) 2020-2021, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi/UefiBaseType.h>

#ifndef _ACPI_PSD_TABLE_UPDATE_H_
#define _ACPI_PSD_TABLE_UPDATE_H_


#define EFI_ACPI_PSD_SIGNATURE            SIGNATURE_32('P', 'S', 'D', 'S')  // 'PSDS'
#define PSDS_EFI_ACPI_OEM_ID              "INTEL "  // OEMID 6 bytes long
#define PSDS_EFI_ACPI_OEM_TABLE_ID        SIGNATURE_64('E','D','K','2',' ',' ',' ',' ') // OEM table id 8 bytes long
#define PSDS_EFI_ACPI_OEM_REVISION        0x00000005
#define PSDS_EFI_ACPI_CREATOR_ID          SIGNATURE_32('I','N','T','L')
#define PSDS_EFI_ACPI_CREATOR_REVISION    0x0100000D
#define EFI_ACPI_PSD_TABLE_REVISION       0x1
#define EFI_ACPI_PSD_FW_VENDOR_SIZE       16
#define EFI_ACPI_PSD_FW_VENDOR            "INTEL           "   // 16 bytes long


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
  }Bits;


  UINT16  Sgx16;

}PSD_SGX_CAPS;

//
//FW Version information
//
typedef struct {
  UINT32 CodeMinor;
  UINT32 CodeMajor;
  UINT32 CodeBuildNo;
  UINT32 CodeHotFix;
}SEC_VERSION_INFO;

//
//PSD Version information
//
typedef struct {
  UINT16 PsdVerMinor;
  UINT16 PsdVerMajor;
}PSD_VERSION_INFO;

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

  Update Platform Service Discovery Table.
  @param[in] Table Pointer of ACPI Table Data.
  @retval EFI_SUCCESS Installed PSD ACPI table successfully.
  @retval EFI_ ERROR.

 **/
EFI_STATUS
EFIAPI
UpdateAcpiPsdTable (
  IN VOID* Table
);

#endif //_ACPI_PSD_TABLE_UPDATE_H_
