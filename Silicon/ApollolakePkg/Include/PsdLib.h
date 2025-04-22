/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>

#ifndef _ACPI_PSD_TABLE_UPDATE_H_
#define _ACPI_PSD_TABLE_UPDATE_H_

#define EFI_ACPI_PSD_SIGNATURE            SIGNATURE_32('P', 'S', 'D', 'S')  // 'PSDS'
#define EFI_ACPI_OEM_REVISION             0x00000005
#define EFI_ACPI_CREATOR_REVISION         0x0100000D
#define EFI_ACPI_PSD_TABLE_REVISION       0x1
#define EFI_ACPI_PSD_FW_VENDOR_SIZE       16
#define EFI_ACPI_PSD_FW_VENDOR            "INTEL           "   // 16 bytes long
#define EFI_ACPI_PSD_FW_HASH_DATA_LEN     32


#pragma pack(1)

//
//PSDT Hash indeces
//
typedef enum {
  PsdCseHash=0,
  PsdIbblHash,
  PsdIbbHash,
  PsdObbHash,
  PsdFwHashMax,
}PSD_FW_HASH_INDEX;

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
  UINT16 PsdVerMajor;
  UINT16 PsdVerMinor;
}PSD_VERSION_INFO;

//
// PSDT Table which will be pupulated to OS,
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER      Header;
  PSD_VERSION_INFO                 PsdVersion;
  UINT32                           CsmeSecCapabilities; //in APL it is called TXE.
  SEC_VERSION_INFO                 FwVer;
  UINT8                            FwVendor[EFI_ACPI_PSD_FW_VENDOR_SIZE];
  UINT8                            EomState;
  UINT8                            SecureBoot;
  UINT8                            MeasuredBoot;
  UINT8                            PsdtType;
  UINT8                            FwHashIndex;
  UINT8                            FwHashDataLen;
  UINT8                            FwHashData[EFI_ACPI_PSD_FW_HASH_DATA_LEN*PsdFwHashMax];
} EFI_ACPI_PSD_TABLE;

///
/// Boot Policy Manifest (BPM) data structure
///
typedef struct {
  UINT32                    ExtType;       ///< 0x00000013  (Little-Endian)
  UINT32                    ExtLength;
  UINT32                    NemData;
  UINT32                    IbblHashAlgo;  ///< 0x00000002
  UINT32                    IbblHashSize;  ///< 0x00000020
  UINT8                     IbblHash[0x20];
  UINT32                    IbbmHashAlgo;
  UINT32                    IbbmHashSize;
  UINT8                     IbbmHash[0x20];
  UINT32                    ObbHashAlgo;
  UINT32                    ObbHashSize;
  UINT8                     ObbHash[0x20];
  UINT8                     Reserved[124]; ///< Others data
} BPMDATA;

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
