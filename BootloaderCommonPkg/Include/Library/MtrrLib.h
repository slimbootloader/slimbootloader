/** @file
  This library class defines a set of methods related with MTRR.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __MTRR_LIB_H__
#define __MTRR_LIB_H__

#include <PiPei.h>


/**
  Print MTRR settings.

  @param[in]  Str     A string will be print before MTRR data if it is not NULL.

**/
VOID
EFIAPI
PrintMtrr (
  IN  CHAR8   *Str
  );

#endif
