/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ACPI_INIT_LIB_INTERNAL_H_
#define _ACPI_INIT_LIB_INTERNAL_H_


#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AcpiInitLib.h>
#include <Library/BootloaderCoreLib.h>
#include <IndustryStandard/Acpi.h>

#define  MAX_ACPI_TEMPLATE_NUM  16

#define  ACPI_ALIGN()       (Current = (UINT8 *)ALIGN_POINTER (Current, 0x10))
#define  ACPI_ALIGN_PAGE()  (Current = (UINT8 *)ALIGN_POINTER (Current, 0x1000))

#define  EFI_ACPI_OEM_ID           {'O','E','M','I','D',' '}   // OEMID 6 bytes long
#define  EFI_ACPI_OEM_TABLE_ID     SIGNATURE_64('O','E','M','T','A','B','L','E') // OEM table id 8 bytes long
#define  EFI_ACPI_OEM_REVISION     0x00000005
#define  EFI_ACPI_CREATOR_ID       SIGNATURE_32('C','R','E','A')
#define  EFI_ACPI_CREATOR_REVISION 0x0100000D

#define  ACPI_SKIP             0
#define  ACPI_APPEND           1
#define  ACPI_REPLACE          2

extern  UINT32          WakeUpBuffer;
extern  CHAR8           WakeUp;
extern  UINT32          WakeUpSize;

extern  CONST EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE mBootGraphicsResourceTableTemplate;

typedef struct {
  UINT8   Type;
  UINT8   Length;
} EFI_ACPI_MADT_ENTRY_COMMON_HEADER;

/**
  Update Boot Graphics Resource Table (BGRT).

  @param[in] Table          Pointer of ACPI BGRT Table.

  @retval EFI_SUCCESS       Update ACPI BGRT table successfully.
  @retval Others            Failed to update FPDT table.
 **/
EFI_STATUS
UpdateBgrt (
  IN  UINT8                           *Table
  );

/**
  Update Firmware Performance Data Table (FPDT).

  @param[in]  Table         Pointer of ACPI FPDT Table.
  @param[out] ExtraSize     Extra size the table needed.

  @retval EFI_SUCCESS       Update ACPI FPDT table successfully.
  @retval Others            Failed to update FPDT table.
 **/
EFI_STATUS
UpdateFpdt (
  IN  UINT8                             *Table,
  OUT UINT32                            *ExtraSize
  );

#endif
