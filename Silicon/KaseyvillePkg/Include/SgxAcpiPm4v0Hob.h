/** @file
  Intel(R) SGX library.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef _SGX_ACPI_PM_4V0_HOB_H_
#define _SGX_ACPI_PM_4V0_HOB_H_

#include <Uefi/UefiBaseType.h>

#define MAX_PRMRR_CPU 8

#define SGX_ACPI_PM_4V0_GUID {0x534987c3, 0x5802, 0x468d, {0x8c, 0x33, 0x78, 0xfe, 0x68, 0x0a, 0x4e, 0x04}}

typedef struct {
  BOOLEAN        ResetRequired;
  EFI_RESET_TYPE ResetType;
  UINT32         ResetStatus;
} SGX_NOTIFY_RESET;

typedef enum {
  SgxUefiEnabled       = 0x00,
  SgxUefiDisabled      = 0x01,
  SgxUefiStatusRead    = 0x02,
  SgxUefiDisabledFinal = 0x03
} SGX_UEFI_DISABLED_SCENARIO;

typedef struct {
  BOOLEAN SgxStatus;
  UINT64  EpcBaseAddress[MAX_PRMRR_CPU];
  UINT64  EpcLength[MAX_PRMRR_CPU];
} SGX_ACPI_EPC_DATA;

typedef struct {
  SGX_ACPI_EPC_DATA          SgxAcpiEpc;
  SGX_NOTIFY_RESET           SgxResetFlow;
  SGX_UEFI_DISABLED_SCENARIO SgxFspRequest;
} SGX_ACPI_PM_4V0_HOB;

#endif // _SGX_ACPI_PM_4V0_HOB_H_
