/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Stage2.h"

//
// Global Descriptor Table (GDT)
//
CONST GLOBAL_REMOVE_IF_UNREFERENCED IA32_SEGMENT_DESCRIPTOR mGdtEntries[] = {
  /* selector { Global Segment Descriptor                              } */
  /* 0x00 */  {{0,      0,  0,  0,    0,  0,  0,  0,    0,  0, 0,  0,  0}}, //null descriptor
  /* 0x08 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear data segment descriptor
  /* 0x10 */  {{0xffff, 0,  0,  0xf,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear code segment descriptor
  /* 0x18 */  {{0xffff, 0,  0,  0x3,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //system data segment descriptor
  /* 0x20 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0x0,  0,  0, 0,  0,  0}}, //16-bit data segment descriptor
  /* 0x28 */  {{0xffff, 0,  0,  0xB,  1,  0,  1,  0x0,  0,  0, 0,  0,  0}}, //16-bit code segment descriptor
};


/**
  Unmap the previous mapped stage images.

  It will reload GDT and disable paging so that the memory goes back to 1:1 mapping.

**/
VOID
UnmapStage (
  VOID
  )
{
  IA32_DESCRIPTOR                 GdtDesc;

  //
  // Reload GDT table into Stage2 memory
  //
  GdtDesc.Base  = (UINTN)mGdtEntries;
  GdtDesc.Limit = sizeof (mGdtEntries) - 1;
  AsmWriteGdtr (&GdtDesc);

  //
  // Reload Exception handler
  //
  UpdateExceptionHandler (NULL);

  if (AsmReadCr0 () & (1 << 31)) {
    DEBUG ((DEBUG_INFO, "Disable Paging ...\n"));
    DisablePaging32 ();
  }
}
