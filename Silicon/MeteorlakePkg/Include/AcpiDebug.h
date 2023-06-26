/** @file
  Header file for ACPI ADBG enable/disable.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ACPI_DEBUG_H_
#define _ACPI_DEBUG_H_

  #define ADBG(x)  //remove all ADBG and ASRT references
  #define ASRT(x, y)
  #define ACPI_DEBUG_EXTERNAL_REFERENCE //remove all external references for ADBG and ASRT
  #define ACPI_ASSERT_EXTERNAL_REFERENCE

#endif
