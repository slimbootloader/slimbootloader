/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  // Define a Global region of ACPI NVS Region that may be used for any
  // type of implementation.  The starting offset and size will be fixed
  // up by the System BIOS during POST.  Note that the Size must be a word
  // in size to be fixed up correctly.

  OperationRegion(GNVS,SystemMemory,0xFFFF0000,0xAA55)
  Field(GNVS,AnyAcc,Lock,Preserve)
  {
  Offset(0),      OSYS, 16, // Offset(0),     Operating System
  Offset(2),      ECR1, 8,  // Offset(2),
  Offset(4),                // Offset(3),     Reserved0
  Offset(4),      USDB, 32, // Offset(4),
  }

