/** @file
  Intel(R) SGX library.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef SGXLIB_H
#define SGXLIB_H

#include <FspmUpd.h>
#include <FspsUpd.h>
#include <SgxAcpiPm4v0Hob.h>
//
// SGX OS Agent external interface
//
#define SGX_MANIFEST_GUID   {0xbba65596, 0x3937, 0x4f5c, {0xa1, 0xc5, 0x08, 0x33, 0x26, 0x82, 0x62, 0x06}}

#define MAX_VAR_DESCRIPTOR_NAME_LEN  64

typedef struct {
  CHAR16 Name[MAX_VAR_DESCRIPTOR_NAME_LEN];
  EFI_GUID Guid;
  UINT32 Attr;
  UINT64 DataSize;
  VOID* Data;
} VAR_DATA_INFO_HOB;

typedef struct {
  UINT8                      SgxSblCfgState;
  UINT8                      Reserved0[3];
  SGX_NOTIFY_RESET           SgxResetFlow;
  SGX_UEFI_DISABLED_SCENARIO SgxFspRequest;
} SGX_SBL_DATA_INFO_HOB;

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
  Check if SGX instruction set is supported by the processor

  @retval TRUE if SGX capability is supported by processor
  @retval FALSE if SGX capability is not supported by processor

**/
BOOLEAN
EFIAPI
IsSgxCapSupported (
  VOID
  );

/**
  Update FSP-M UPD SGX config data

  @param[in]  FspmUpd           Pointer to the FSP-m UPD to be updated

  @retval TRUE if SGX config was found and set
  @retval FALSE if SGX config was not found

**/
EFI_STATUS
EFIAPI
UpdateFspmSgxConfig (
  IN FSPM_UPD *FspmUpd
  );

/**
  Update SGX Meta data Parameters in stage2.

  @param[in]  FspsUpd           Pointer to the FSP-s UPD to be updated

  @retval TRUE if SGX config was found and set
  @retval FALSE if SGX config was not found

**/
EFI_STATUS
EFIAPI
UpdateFspsSgxConfig (
  IN FSPS_UPD *FspsUpd
);

/**
  Free Allocated memory for SGX variables
**/
VOID
EFIAPI
FreeSgxMem (
  VOID
);

/**
  Update SGX ACPI Parameters to ACPI Table.

  @param void

**/
EFI_STATUS
EFIAPI
UpdateSgxAcpiSblData (
  VOID
);

/**
  Create SBL to UEFI Payload HOB.

  @param void

**/
VOID
EFIAPI
BuildSgxSblDataInfoHob (
  VOID
);

#endif // SGXLIB_H
