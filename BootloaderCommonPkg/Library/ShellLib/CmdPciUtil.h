/** @file
  Utility functions for the `pci` shell command.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CMD_PCI_UTIL_H_
#define _CMD_PCI_UTIL_H_

#include <Library/ShellLib.h>
#include <IndustryStandard/Pci.h>

typedef struct {
  CHAR16  *BaseClass; // Pointer to the PCI base class string
  CHAR16  *SubClass;  // Pointer to the PCI sub class string
  CHAR16  *PIFClass;  // Pointer to the PCI programming interface string
} PCI_CLASS_STRINGS;

/**
  Generates printable Unicode strings that represent PCI device class,
  subclass and programmed I/F based on a value passed to the function.

  @param[in] ClassCode      Value representing the PCI "Class Code" register read from a
                 PCI device. The encodings are:
                     bits 23:16 - Base Class Code
                     bits 15:8  - Sub-Class Code
                     bits  7:0  - Programming Interface
  @param[in, out] ClassStrings   Pointer of PCI_CLASS_STRINGS structure, which contains
                 printable class strings corresponding to ClassCode. The
                 caller must not modify the strings that are pointed by
                 the fields in ClassStrings.
**/
VOID
PciGetClassStrings (
  IN      UINT32               ClassCode,
  IN OUT  PCI_CLASS_STRINGS    *ClassStrings
  );

#endif
