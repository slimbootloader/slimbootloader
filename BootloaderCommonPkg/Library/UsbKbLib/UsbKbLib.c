/** @file
  USB keyboard library implementation.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <UsbKbLibInternal.h>

CONST  CHAR8  *mKeyboardKeyMap[] = {
  "abcdefghijklmnopqrstuvwxyz1234567890\r\x1b\b\t -=[]\\ ;'`,./",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()\r\x1b\b\t _+{}| :\"~<>?",
  "/*-+\r1234567890."
};

USB_KB_DEV     mUsbKbDevice;

/**
  Set the idle rate of the specified USB HID report.

  Submit a USB set HID report idle request for the USB device specified by UsbIo,
  Interface, and ReportId, and set the idle rate to the value specified by Duration.
  If UsbIo is NULL, then ASSERT().

  @param  UsbIo      A pointer to the USB I/O Protocol instance for the specific USB target.
  @param  Interface  The index of the report interface on the USB target.
  @param  ReportId   The identifier of the report to retrieve.
  @param  Duration   The idle rate to set for the specified USB target.

  @retval  EFI_SUCCESS       The request executed successfully.
  @retval  EFI_TIMEOUT       A timeout occurred executing the request.
  @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
UsbSetIdleRequest (
  IN PEI_USB_IO_PPI         *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   ReportId,
  IN UINT8                   Duration
  )
{
  UINT32                  Status;
  EFI_USB_DEVICE_REQUEST  Request;

  ASSERT (UsbIo != NULL);
  //
  // Fill Device request packet
  //
  Request.RequestType = USB_HID_CLASS_SET_REQ_TYPE;
  Request.Request = EFI_USB_SET_IDLE_REQUEST;
  Request.Value   = (UINT16) ((Duration << 8) | ReportId);
  Request.Index   = Interface;
  Request.Length  = 0;

  Status = UsbIo->UsbControlTransfer (
             NULL,
             UsbIo,
             &Request,
             EfiUsbNoData,
             PcdGet32 (PcdUsbTransferTimeoutValue),
             NULL,
             0
             );
  return Status;
}

/**
  Set the HID protocol of the specified USB HID interface.

  Submit a USB set HID protocol request for the USB device specified by UsbIo
  and Interface and set the protocol to the value specified by Protocol.
  If UsbIo is NULL, then ASSERT().

  @param  UsbIo      A pointer to the USB I/O Protocol instance for the specific USB target.
  @param  Interface  The index of the report interface on the USB target.
  @param  Protocol   The protocol value to set for the specified USB target.

  @retval  EFI_SUCCESS       The request executed successfully.
  @retval  EFI_TIMEOUT       A timeout occurred executing the request.
  @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
UsbSetProtocolRequest (
  IN  PEI_USB_IO_PPI         *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   Protocol
  )
{
  EFI_STATUS              Status;
  EFI_USB_DEVICE_REQUEST  Request;

  ASSERT (UsbIo != NULL);

  //
  // Fill Device request packet
  //
  Request.RequestType = USB_HID_CLASS_SET_REQ_TYPE;
  Request.Request = EFI_USB_SET_PROTOCOL_REQUEST;
  Request.Value   = Protocol;
  Request.Index   = Interface;
  Request.Length  = 0;

  Status = UsbIo->UsbControlTransfer (
             NULL,
             UsbIo,
             &Request,
             EfiUsbNoData,
             PcdGet32 (PcdUsbTransferTimeoutValue),
             NULL,
             0
             );
  return Status;
}

/**
  Set the report descriptor of the specified USB HID interface.

  Submit a USB set HID report request for the USB device specified by UsbIo,
  Interface, ReportId, and ReportType, and set the report descriptor using the
  buffer specified by ReportLength and Report.
  If UsbIo is NULL, then ASSERT().
  If Report is NULL, then ASSERT().

  @param  UsbIo         A pointer to the USB I/O Protocol instance for the specific USB target.
  @param  Interface     The index of the report interface on the USB target.
  @param  ReportId      The identifier of the report to retrieve.
  @param  ReportType    The type of report to retrieve.
  @param  ReportLength  The size, in bytes, of Report.
  @param  Report        A pointer to the report descriptor buffer to set.

  @retval  EFI_SUCCESS       The request executed successfully.
  @retval  EFI_TIMEOUT       A timeout occurred executing the request.
  @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
UsbSetReportRequest (
  IN PEI_USB_IO_PPI         *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   ReportId,
  IN UINT8                   ReportType,
  IN UINT16                  ReportLen,
  IN UINT8                   *Report
  )
{
  EFI_STATUS              Status;
  EFI_USB_DEVICE_REQUEST  Request;

  ASSERT (UsbIo != NULL);
  ASSERT (Report != NULL);

  //
  // Fill Device request packet
  //
  Request.RequestType = USB_HID_CLASS_SET_REQ_TYPE;
  Request.Request = EFI_USB_SET_REPORT_REQUEST;
  Request.Value   = (UINT16) ((ReportType << 8) | ReportId);
  Request.Index   = Interface;
  Request.Length  = ReportLen;

  Status = UsbIo->UsbControlTransfer (
             NULL,
             UsbIo,
             &Request,
             EfiUsbDataOut,
             PcdGet32 (PcdUsbTransferTimeoutValue),
             Report,
             ReportLen
             );

  return Status;
}


/**
  Get the report descriptor of the specified USB HID interface.

  Submit a USB get HID report request for the USB device specified by UsbIo,
  Interface, ReportId, and ReportType, and return the report in the buffer
  specified by Report.
  If UsbIo is NULL, then ASSERT().
  If Report is NULL, then ASSERT().

  @param  UsbIo         A pointer to the USB I/O Protocol instance for the specific USB target.
  @param  Interface     The index of the report interface on the USB target.
  @param  ReportId      The identifier of the report to retrieve.
  @param  ReportType    The type of report to retrieve.
  @param  ReportLength  The size, in bytes, of Report.
  @param  Report        A pointer to the buffer to store the report descriptor.

  @retval  EFI_SUCCESS           The request executed successfully.
  @retval  EFI_OUT_OF_RESOURCES  The request could not be completed because the
                                 buffer specified by ReportLength and Report is not
                                 large enough to hold the result of the request.
  @retval  EFI_TIMEOUT           A timeout occurred executing the request.
  @retval  EFI_DEVICE_ERROR      The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
UsbGetReportRequest (
  IN  PEI_USB_IO_PPI         *UsbIo,
  IN  UINT8                   Interface,
  IN  UINT8                   ReportId,
  IN  UINT8                   ReportType,
  IN  UINT16                  ReportLen,
  OUT UINT8                   *Report
  )
{
  EFI_STATUS              Status;
  EFI_USB_DEVICE_REQUEST  Request;

  ASSERT (UsbIo != NULL);
  ASSERT (Report != NULL);

  //
  // Fill Device request packet
  //
  Request.RequestType = USB_HID_CLASS_GET_REQ_TYPE;
  Request.Request = EFI_USB_GET_REPORT_REQUEST;
  Request.Value   = (UINT16) ((ReportType << 8) | ReportId);
  Request.Index   = Interface;
  Request.Length  = ReportLen;

  Status = UsbIo->UsbControlTransfer (
             NULL,
             UsbIo,
             &Request,
             EfiUsbDataIn,
             PcdGet32 (PcdUsbTransferTimeoutValue),
             Report,
             ReportLen
             );

  return Status;
}


/**
  Sets USB keyboard LED state.

  @param  UsbKeyboardDevice  The USB_KB_DEV instance.

**/
VOID
SetKeyLedState (
  IN  USB_KB_DEV    *UsbKeyboardDevice
  )
{
  LED_MAP Led;
  UINT8   ReportId;

  //
  // Set each field in Led map.
  //
  Led.NumLock    = (UINT8) ((UsbKeyboardDevice->NumLockOn) ? 1 : 0);
  Led.CapsLock   = (UINT8) ((UsbKeyboardDevice->CapsOn) ? 1 : 0);
  Led.ScrollLock = (UINT8) ((UsbKeyboardDevice->ScrollOn) ? 1 : 0);
  Led.Resrvd     = 0;

  //
  // Call Set_Report Request to lighten the LED.
  //
  ReportId       = 0;
  UsbSetReportRequest (
    UsbKeyboardDevice->UsbIo,
    UsbKeyboardDevice->InterfaceDescriptor.InterfaceNumber,
    ReportId,
    HID_OUTPUT_REPORT,
    1,
    (UINT8 *) &Led
    );
}

/**
  Initialize the usb bot device.

  @param[in]  PeiServices   General-purpose services that are available to every
                            PEIM.
  @param[in]  UsbIoPpi      Indicates the PEI_USB_IO_PPI instance.

  @retval EFI_SUCCESS       The usb bot device is initialized successfully.
  @retval Other             Failed to initialize media.

**/
EFI_STATUS
UsbFindUsbKbDevice (
  IN  PEI_USB_IO_PPI       *UsbIoPpi,
  IN  USB_KB_DEV           *UsbKbDevice
  )
{
  EFI_STATUS                     Status;
  EFI_USB_INTERFACE_DESCRIPTOR  *InterfaceDesc;
  EFI_USB_ENDPOINT_DESCRIPTOR   *EndpointDescriptor;
  EFI_PEI_SERVICES             **PeiServices;
  UINT8                          EndpointIndex;

  PeiServices = NULL;

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

  // Check if it is the BOT device we support
  if ((InterfaceDesc->InterfaceClass != 0x03) || (InterfaceDesc->InterfaceSubClass != 0x01)
      || (InterfaceDesc->InterfaceProtocol != 0x01)) {
    return EFI_NOT_FOUND;
  }

  for (EndpointIndex = 0; EndpointIndex < InterfaceDesc->NumEndpoints; EndpointIndex++) {
    Status = UsbIoPpi->UsbGetEndpointDescriptor (PeiServices, UsbIoPpi, EndpointIndex, &EndpointDescriptor);
    if (EFI_ERROR (Status)) {
      continue;
    }
    if ( ((EndpointDescriptor->EndpointAddress & USB_ENDPOINT_DIR_IN) == USB_ENDPOINT_DIR_IN) &&
         ((EndpointDescriptor->Attributes & USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_INTERRUPT) ) {
      break;
    }
  }
  if (EndpointIndex == InterfaceDesc->NumEndpoints) {
    return EFI_UNSUPPORTED;
  }

  if (UsbKbDevice != NULL) {
    UsbKbDevice->Signature           = USB_KB_DEVICE_SIG;
    UsbKbDevice->UsbIo               = UsbIoPpi;
    UsbKbDevice->InterfaceDescriptor = *InterfaceDesc;
    UsbKbDevice->EndpointDescriptor  = *EndpointDescriptor;
  }

  Status = UsbSetProtocolRequest (
    UsbKbDevice->UsbIo,
    UsbKbDevice->InterfaceDescriptor.InterfaceNumber,
    0
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = UsbSetIdleRequest (
    UsbKbDevice->UsbIo,
    UsbKbDevice->InterfaceDescriptor.InterfaceNumber,
    0,
    10
    );

  return Status;
}

/**
  The function will initialize USB keyboard device.

  Based on UsbHcPciBase, this function will initialize USB host controller, allocate
  necessary resources, and enumarate all the USB bus.

  @param[in]  UsbHcPciBase       USB Host Controller's PCI ConfigSpace Base address

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval EFI_NOT_FOUND          Can't find any USB block devices for boot.

**/
EFI_STATUS
EFIAPI
InitUsbKeyBoard (
  IN  UINTN                     UsbHcPciBase
  )
{
  EFI_STATUS           Status;
  UINTN                Index;
  UINT32               UsbIoCount;
  PEI_USB_IO_PPI     **UsbIoArray;

  ZeroMem (&mUsbKbDevice, sizeof (mUsbKbDevice));

  Status = InitUsbDevices (UsbHcPciBase);
  if (!EFI_ERROR (Status)) {
    Status = GetUsbDevices ((PEI_USB_IO_PPI **)&UsbIoArray, &UsbIoCount);
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to initialize USB bus !\n"));
    return Status;
  }

  for (Index = 0; Index < UsbIoCount; Index++) {
    Status = UsbFindUsbKbDevice (UsbIoArray[Index], &mUsbKbDevice);
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Found keyboard on device %d\n", Index));
      break;
    }
  }

  if (mUsbKbDevice.Signature == 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


/**
  Enqueue the key.

  @param  Queue                 The queue to be enqueued.
  @param  KeyData               The key data to be enqueued.

  @retval EFI_NOT_READY         The queue is full.
  @retval EFI_SUCCESS           Successfully enqueued the key data.

**/
EFI_STATUS
Enqueue (
  IN SIMPLE_QUEUE         *Queue,
  IN CHAR8                 KeyData
  )
{
  if ((Queue->Rear + 1) % QUEUE_MAX_COUNT == Queue->Front) {
    return EFI_NOT_READY;
  }

  Queue->Buffer[Queue->Rear] = KeyData;
  Queue->Rear = (Queue->Rear + 1) % QUEUE_MAX_COUNT;

  return EFI_SUCCESS;
}

/**
  Dequeue the key.

  @param  Queue                 The queue to be dequeued.
  @param  KeyData               The key data to be dequeued.

  @retval EFI_NOT_READY         The queue is empty.
  @retval EFI_SUCCESS           Successfully dequeued the key data.

**/
EFI_STATUS
Dequeue (
  IN SIMPLE_QUEUE         *Queue,
  IN CHAR8                *KeyData
  )
{
  if (Queue->Front == Queue->Rear) {
    return EFI_NOT_READY;
  }

  if (KeyData != NULL) {
    *KeyData = Queue->Buffer[Queue->Front];
  }

  Queue->Front  = (Queue->Front + 1) % QUEUE_MAX_COUNT;

  return EFI_SUCCESS;
}

/**
  Check whether the queue is empty.

  @param  Queue                 The queue to be checked.

  @retval EFI_NOT_READY         The queue is empty.
  @retval EFI_SUCCESS           The queue is not empty.

**/
EFI_STATUS
CheckQueue (
  IN SIMPLE_QUEUE         *Queue
  )
{
  if (Queue->Front == Queue->Rear) {
    return EFI_NOT_READY;
  }

  return EFI_SUCCESS;
}

/**
  Return queue length.

  @param  Queue                 The queue to be checked.

  @retval                       The queue length.

**/
UINTN
GetQueueLength (
  IN SIMPLE_QUEUE         *Queue
  )
{
  if (Queue->Front == Queue->Rear) {
    return 0;
  }

  if (Queue->Front < Queue->Rear) {
    return Queue->Rear - Queue->Front;
  } else {
    return Queue->Rear - Queue->Front + QUEUE_MAX_COUNT;
  }
}

/**
  Convert a key code into ascii.

  @param  Modifier        The modifier state.
  @param  Key             The key code to be converted.

  @retval                 The ascii for the key code.

**/
CHAR8
ConvertKeyToAscii (
  IN  UINT8  Modifier,
  IN  UINT8  Key
  )
{
  UINT8    Shift;
  BOOLEAN  SetLed;

  SetLed = FALSE;
  Shift = ((Modifier & 0x22) != 0) ? 1 : 0;
  if (Key >= 0x04 && Key <= 0x38) {
    // Handle SHIFT key
    if ((Key <= 0x1D) && mUsbKbDevice.CapsOn) {
      Shift = 1 - Shift;
    }
    return mKeyboardKeyMap[Shift][Key - 0x04];
  }

  if (mUsbKbDevice.NumLockOn) {
     if (Key >= 0x54 && Key <= 0x63) {
       return mKeyboardKeyMap[2][Key - 0x54];
     }
  }

  if (Key == 0x53) {
    // Num Lock
    mUsbKbDevice.NumLockOn = mUsbKbDevice.NumLockOn ? FALSE : TRUE;
    SetLed = TRUE;
  } else if (Key == 0x39) {
    // Cap Lock
    mUsbKbDevice.CapsOn = mUsbKbDevice.CapsOn ? FALSE : TRUE;
    SetLed = TRUE;
  }

  if (SetLed) {
    SetKeyLedState (&mUsbKbDevice);
  }

  return 0;
}


/**
  Polls a keyboard to see if there is any data waiting to be read.

  @retval TRUE             Data is waiting to be read from the device.
  @retval FALSE            There is no data waiting to be read from the device.

**/
BOOLEAN
EFIAPI
KeyboardPoll (
  VOID
  )
{
  EFI_STATUS       Status;
  USB_KB_DEV      *UsbKbDevice;
  UINT8            KeyBuf[8];
  CHAR8            Char;
  UINTN            DataSize;
  UINTN            Index;

  UsbKbDevice = &mUsbKbDevice;
  if (UsbKbDevice->Signature != USB_KB_DEVICE_SIG) {
    return FALSE;
  }

  // Use interrupt transfer to get report
  Char     = 0;
  DataSize = sizeof (KeyBuf);
  Status   = UsbKbDevice->UsbIo->UsbBulkTransfer (
               NULL,
               UsbKbDevice->UsbIo,
               UsbKbDevice->EndpointDescriptor.EndpointAddress,
               KeyBuf,
               &DataSize,
               PcdGet32 (PcdUsbKeyboardPollingTimeout)
               );
  if (!EFI_ERROR (Status)) {
    if ((KeyBuf[2] == 0) || (KeyBuf[2] != UsbKbDevice->LastChar)) {
      UsbKbDevice->KeyRepeat = 0;
    }
    for (Index = 2; Index < DataSize; Index++) {
      if (KeyBuf[Index] == 0) {
        break;
      }
      // Key press
      UsbKbDevice->LastChar = KeyBuf[Index];
      if ((UsbKbDevice->KeyRepeat == 0) || (UsbKbDevice->KeyRepeat > 10)) {
        Char = ConvertKeyToAscii (KeyBuf[0], KeyBuf[Index]);
        if (Char > 0) {
          Enqueue (&UsbKbDevice->Queue, Char);
        }
      }
    }
    if (KeyBuf[2] != 0) {
      if (UsbKbDevice->KeyRepeat < 0xFF) {
        // Handle key repeat
        UsbKbDevice->KeyRepeat++;
      }
    }
  }

  return GetQueueLength (&UsbKbDevice->Queue) > 0 ? TRUE : FALSE;
}

/**
  Reads data from a USB keyboard into a buffer.
  This function will wait till specified number of bytes are filled.

  @param  Buffer           Pointer to the data buffer to store the data read from the device.
  @param  NumberOfBytes    Number of bytes to read from the device.

  @retval                  The number of bytes read from the device.

**/
UINTN
EFIAPI
KeyboardRead (
  OUT UINT8   *Buffer,
  IN  UINTN   NumberOfBytes
  )
{
  USB_KB_DEV      *UsbKbDevice;
  UINTN            Length;

  if (NULL == Buffer) {
    return 0;
  }

  UsbKbDevice = &mUsbKbDevice;
  if (UsbKbDevice->Signature != USB_KB_DEVICE_SIG) {
    return 0;
  }

  Length      = NumberOfBytes;
  while (Length--) {
    while (!KeyboardPoll ());
    Dequeue (&UsbKbDevice->Queue, (CHAR8 *)Buffer);
    Buffer++;
  }

  return NumberOfBytes;
}