/** @file

  Define BIOS Guard NVS Area operation region.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  //
  // Define BIOS Guard NVS Area operation region.
  //


  OperationRegion(BGNV,SystemMemory,0xFFFF0000,0xAA55)
  Field(BGNV,AnyAcc,Lock,Preserve)
  {
  BGMA, 64, // Offset(0),     BIOS Guard Memory Address for Tool Interface
  BGMS, 8,  // Offset(8),     BIOS Guard Memory Size for Tool Interface
  BGIA, 8,  // Offset(9),     BIOS Guard SW SMI value for Tool Interface
  }