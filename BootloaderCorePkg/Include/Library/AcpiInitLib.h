/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __ACPI_INIT_LIB_H__
#define __ACPI_INIT_LIB_H__

/**
  This function is called on S3 boot flow only.

  It will locate the S3 waking vector from the ACPI table and then
  jump into it. The control will never return.

  @param  AcpiBase   ACPI table base address

**/
VOID
FindAcpiWakeVectorAndJump (
  IN  UINT32    AcpiBase
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

#endif
