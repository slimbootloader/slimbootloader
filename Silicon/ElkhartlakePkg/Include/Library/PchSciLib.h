/** @file
  Header file for PchSciLib.

  Copyright (c) 2019 Intel Corporation., Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_SCI_LIB_H_
#define _PCH_SCI_LIB_H_

/**
  Check whether Sycamore Island (SCI) is supported by PCH Series.

  @retval TRUE                    SCI is supported in current PCH
  @retval FALSE                   SCI is not supported on current PCH
**/
BOOLEAN
EFIAPI
PchIsSciSupported (
  VOID
);

/**
  Query SCI boot status. This routine is only valid after
  PCI enumeration has completed since it relies on BAR0 to
  have been programmed already.

  @retval    BOOLEAN   TRUE if SCI boot successful, else FALSE
**/
BOOLEAN
EFIAPI
SciBootSuccess (
  VOID
);

#endif // _PCH_SCI_LIB_H_
