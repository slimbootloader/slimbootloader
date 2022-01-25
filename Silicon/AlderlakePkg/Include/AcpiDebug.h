/** @file
  Header file for ACPI ADBG enable/disable.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ACPI_DEBUG_H_
#define _ACPI_DEBUG_H_

// todo: add a PCD to control ACPI debug print
  #define ADBG(x) ADBG(x)  //leave it as it is
  #define ACPI_DEBUG_EXTERNAL_REFERENCE External(\ADBG, MethodObj)   //additional define statement to cover ADBG external references
#endif
