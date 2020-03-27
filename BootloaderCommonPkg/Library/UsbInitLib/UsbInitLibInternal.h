/** @file
  USB init library header file.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  __USB_INIT_LIB_INTERNAL_H__
#define  __USB_INIT_LIB_INTERNAL_H__


#include <PiPei.h>
#include <IndustryStandard/Pci.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/XhciLib.h>
#include <Library/UsbBusLib.h>

#define  MAX_USB_DEVICE_NUMBER  32

typedef struct {
  EFI_HANDLE                      UsbHostHandle;
  UINT32                          UsbIoCount;
  PEI_USB_IO_PPI                 *UsbIoArray[MAX_USB_DEVICE_NUMBER];
} USB_INIT_INSTANCE;

#endif
