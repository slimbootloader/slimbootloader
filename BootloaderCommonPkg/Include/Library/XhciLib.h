/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __XHCI_LIB_H__
#define __XHCI_LIB_H__

#include <Ppi/Usb2HostController.h>

/**
  USB initialization interface.

  @param BaseAddress     Base address of the USB host controller.
  @param UsbHostHandle   Pointer to return the USB host controller handle.

  @retval EFI_SUCCESS    USB host controller was initialized successfully.

**/
EFI_STATUS
EFIAPI
UsbInitCtrl (
  IN     UINTN                           BaseAddress,
  IN OUT EFI_HANDLE                     *UsbHostHandle
  );

#endif
