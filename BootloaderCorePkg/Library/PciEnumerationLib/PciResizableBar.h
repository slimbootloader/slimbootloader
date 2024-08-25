/** @file

  Copyright (c) 2020 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCI_RESIZABLE_BAR_H__
#define __PCI_RESIZABLE_BAR_H__

VOID
EFIAPI
InitializeResizeBar (
  IN OUT  PCI_IO_DEVICE              *PciIoDevice,
  IN      UINT8                       Bus,
  IN      UINT8                       Device,
  IN      UINT8                       Func
  );

#endif
