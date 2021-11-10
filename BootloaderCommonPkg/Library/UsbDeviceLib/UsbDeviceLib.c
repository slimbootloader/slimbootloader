/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UsbDeviceDxe.h"
#include <IndustryStandard/Pci.h>

USB_XDCI_DEV_CONTEXT     *mUsbXdciDevContext;
USB_DEVICE_PLATFORM_INFO     mUsbDeviceInfo;

/**
  Start to process the controller by reading the PCI.

  @retval EFI_SUCCESS            The controller is controlled by the usb bus.
  @retval EFI_ALREADY_STARTED    The controller is already controlled by the usb
                                 bus.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate resources.

**/
EFI_STATUS
EFIAPI
UsbDeviceDriverSupported (
  VOID
  )
{
  EFI_STATUS                Status;
  UINT32                    Address;
  UINT32                    Data;
  USB_CLASSC                UsbClassCReg;
  USB_DEVICE_PLATFORM_INFO     *UsbInfo;

  Status = EFI_SUCCESS;

  UsbInfo = &mUsbDeviceInfo;

  Address = MmPciAddress ( 0, UsbInfo->XdciDeviceAddress.Bus, UsbInfo->XdciDeviceAddress.Device,
                           UsbInfo->XdciDeviceAddress.Func, 0);
  if (MmioRead16 (Address) == 0xFFFF) {
    return EFI_NOT_FOUND;
  }

  Address = MmPciAddress ( 0, UsbInfo->XdciDeviceAddress.Bus, UsbInfo->XdciDeviceAddress.Device,
                           UsbInfo->XdciDeviceAddress.Func, PCI_REVISION_ID_OFFSET);
  Data = MmioRead32 (Address) >> 8;
  CopyMem (&UsbClassCReg, &Data, sizeof (USB_CLASSC));

  //
  // Test whether the controller belongs to USB device type
  //
  // 0x0C03FE / 0x0C0380
  if ((UsbClassCReg.BaseCode != PCI_CLASS_SERIAL) ||
      (UsbClassCReg.SubClassCode != PCI_CLASS_SERIAL_USB) ||
      ((UsbClassCReg.ProgInterface  != PCI_IF_USBDEV) && (UsbClassCReg.ProgInterface != 0x80))) {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  USB driver setup to process the controller.

  @retval EFI_SUCCESS            The controller is controlled by the usb bus.
  @retval EFI_ALREADY_STARTED    The controller is already controlled by the usb
                                 bus.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate resources.

**/
EFI_STATUS
EFIAPI
UsbDeviceDriverStart (
  VOID
  )
{
  EFI_STATUS                Status;
  USB_XDCI_DEV_CONTEXT     *UsbXdciDevContext;
  UINT32                    Address;
  USB_DEVICE_PLATFORM_INFO     *UsbInfo;

  Status = EFI_SUCCESS;

  UsbInfo = &mUsbDeviceInfo;

  UsbXdciDevContext = AllocateZeroPool (sizeof (USB_XDCI_DEV_CONTEXT));

  if (UsbXdciDevContext == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  //
  // Initialize the driver context
  //
  UsbXdciDevContext->StartUpController = FALSE;
  UsbXdciDevContext->XdciHandle   = NULL;
  UsbXdciDevContext->FirstNodePtr = NULL;
  UsbXdciDevContext->Signature = EFI_USB_DEV_SIGNATURE;

  Address = MmPciAddress (0, UsbInfo->XdciDeviceAddress.Bus, UsbInfo->XdciDeviceAddress.Device,
                          UsbInfo->XdciDeviceAddress.Func, R_OTG_BAR0);
  UsbXdciDevContext->XdciMmioBarAddr = MmioRead32 (Address) & B_OTG_BAR0_BA;

  CopyMem (
    & (UsbXdciDevContext->UsbFunIoProtocol),
    &mUsbFunIoProtocol,
    sizeof (EFI_USBFN_IO_PROTOCOL)
    );

  CopyMem (
    & (UsbXdciDevContext->UsbDevModeProtocol),
    &mUsbDeviceModeProtocol,
    sizeof (EFI_USB_DEVICE_MODE_PROTOCOL)
    );

  mUsbXdciDevContext = UsbXdciDevContext;

  return Status;

ErrorExit:
  if (UsbXdciDevContext != NULL) {
    if (UsbXdciDevContext->XdciPollTimer != NULL) {
      UsbXdciDevContext->XdciPollTimer = NULL;
    }
    FreePool (UsbXdciDevContext);
  }

  return Status;
}

/**
  USB device initialization.

  @param[in, out] Prot

**/
EFI_STATUS
EFIAPI
UsbDeviceInitialization (
  IN OUT       VOID          **Prot
  )
{
  EFI_STATUS                       Status;
  USB_DEVICE_PLATFORM_INFO            UsbInfo;

  Status = UsbDeviceBoardLibInit (&UsbInfo);
  if (!EFI_ERROR (Status)) {
    CopyMem (&mUsbDeviceInfo, &UsbInfo, sizeof (USB_DEVICE_PLATFORM_INFO));
  } else {
    return Status;
  }

  Status  = UsbDeviceDriverSupported();
  if (!EFI_ERROR (Status)) {
    Status = UsbDeviceDriverStart();
    if (!EFI_ERROR (Status)) {
      *Prot = (VOID *)&mUsbXdciDevContext->UsbDevModeProtocol;
    }
  }

  return Status;
}
