/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2.h"

/**
  Unmap the previous mapped stage images.

  It will reload GDT and disable paging so that the memory goes back to 1:1 mapping.

**/
VOID
UnmapStage (
  VOID
  )
{
  //
  // Reload Exception handler
  //
  UpdateExceptionHandler (NULL);

  if (AsmReadCr0 () & (1 << 31)) {
    DEBUG ((DEBUG_INFO, "Disable Paging ...\n"));
    DisablePaging32 ();
  }
}
