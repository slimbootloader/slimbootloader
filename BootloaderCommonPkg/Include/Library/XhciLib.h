/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
  IN     UINT32                          BaseAddress,
  IN OUT EFI_HANDLE                     *UsbHostHandle
  );

#endif
