/** @file
Common Libarary  for PEI USB.

Copyright (c) 2006 - 2014, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UsbPeim.h"
#include "PeiUsbLib.h"

/**
  Clear Endpoint Halt.

  @param  PeiServices       General-purpose services that are available to every PEIM.
  @param  UsbIoPpi          Indicates the PEI_USB_IO_PPI instance.
  @param  EndpointAddress   The endpoint address.

  @retval EFI_SUCCESS       Endpoint halt is cleared successfully.
  @retval EFI_DEVICE_ERROR  Cannot clear the endpoint halt status due to a hardware error.
  @retval Others            Other failure occurs.

**/
EFI_STATUS
PeiUsbClearEndpointHalt (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN PEI_USB_IO_PPI           *UsbIoPpi,
  IN UINT8                    EndpointAddress
  )
{
  EFI_STATUS                    Status;
  EFI_USB_INTERFACE_DESCRIPTOR  *InterfaceDesc;
  EFI_USB_ENDPOINT_DESCRIPTOR   *EndpointDescriptor;
  UINT8                         EndpointIndex;


  //
  // Check its interface
  //
  Status = UsbIoPpi->UsbGetInterfaceDescriptor (
             PeiServices,
             UsbIoPpi,
             &InterfaceDesc
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  for (EndpointIndex = 0; EndpointIndex < InterfaceDesc->NumEndpoints; EndpointIndex++) {
    Status = UsbIoPpi->UsbGetEndpointDescriptor (PeiServices, UsbIoPpi, EndpointIndex, &EndpointDescriptor);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    if (EndpointDescriptor->EndpointAddress == EndpointAddress) {
      break;
    }
  }

  if (EndpointIndex == InterfaceDesc->NumEndpoints) {
    return EFI_INVALID_PARAMETER;
  }

  Status = PeiUsbClearDeviceFeature (
             PeiServices,
             UsbIoPpi,
             EfiUsbEndpoint,
             EfiUsbEndpointHalt,
             EndpointAddress
             );

  return Status;
}

/**
  Judge if the port is connected with a low-speed usb device or not.

  @param  PortStatus  The usb port status gotten.

  @retval TRUE        A low-speed usb device is connected with the port.
  @retval FALSE       No low-speed usb device is connected with the port.

**/
BOOLEAN
IsPortLowSpeedDeviceAttached (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 9 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_LOW_SPEED) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

