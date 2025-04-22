/** @file
  EFI USB function IO Protocol
  This protocol supports Usb Function IO API.
  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __EFI_USB_FUNC_IO_H__
#define __EFI_USB_FUNC_IO_H__

#include <IndustryStandard/Usb.h>
#include <Library/UsbDeviceLib.h>

#define EFI_USBFN_IO_PROTOCOL_REVISION   0x00010001

//
// {32D2963A-FE5D-4f30-B633-6E5DC55803CC}
// #define EFI_USBFN_IO_PROTOCOL_GUID  {0x32d2963a, 0xfe5d, 0x4f30, 0xb6, 0x33, 0x6e, 0x5d, 0xc5, 0x58, 0x3, 0xcc };
//

typedef struct _EFI_USBFN_IO_PROTOCOL   EFI_USBFN_IO_PROTOCOL;

//
// USB standard descriptors and reqeust
//
typedef USB_DEVICE_REQUEST        EFI_USB_DEVICE_REQUEST;
typedef USB_DEVICE_DESCRIPTOR     EFI_USB_DEVICE_DESCRIPTOR;
typedef USB_CONFIG_DESCRIPTOR     EFI_USB_CONFIG_DESCRIPTOR;
typedef USB_INTERFACE_DESCRIPTOR  EFI_USB_INTERFACE_DESCRIPTOR;
typedef USB_ENDPOINT_DESCRIPTOR   EFI_USB_ENDPOINT_DESCRIPTOR;

typedef enum _EFI_USBFN_PORT_TYPE {
  EfiUsbUnknownPort = 0,
  EfiUsbStandardDownstreamPort,
  EfiUsbChargingDownstreamPort,
  EfiUsbDedicatedChargingPort,
  EfiUsbInvalidDedicatedChargingPort
} EFI_USBFN_PORT_TYPE;

/**
 USB_DEVICE_DESCRIPTOR, USB_CONFIG_DESCRIPTOR, USB_INTERFACE_DESCRIPTOR, and
 USB_ENDPOINT_DESCRIPTOR are already defined
 in UEFI spec 2.3, as par USB 2.0 spec.
**/

typedef struct {
  EFI_USB_INTERFACE_DESCRIPTOR        *InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR         **EndpointDescriptorTable;
} EFI_USB_INTERFACE_INFO;

typedef struct {
  EFI_USB_CONFIG_DESCRIPTOR           *ConfigDescriptor;
  EFI_USB_INTERFACE_INFO              **InterfaceInfoTable;
} EFI_USB_CONFIG_INFO;

typedef struct {
  EFI_USB_DEVICE_DESCRIPTOR           *DeviceDescriptor;
  EFI_USB_CONFIG_INFO                 **ConfigInfoTable;
} EFI_USB_DEVICE_INFO;


typedef enum _EFI_USB_ENDPOINT_TYPE {
  UsbEndpointControl     = 0x00,
  UsbEndpointIsochronous = 0x01,
  UsbEndpointBulk        = 0x02,
  UsbEndpointInterrupt   = 0x03
} EFI_USB_ENDPOINT_TYPE;


typedef enum _EFI_USBFN_DEVICE_INFO_ID {
  EfiUsbDeviceInfoUnknown = 0,
  EfiUsbDeviceInfoSerialNumber,
  EfiUsbDeviceInfoManufacturerName,
  EfiUsbDeviceInfoProductName
} EFI_USBFN_DEVICE_INFO_ID;


typedef enum _EFI_USBFN_ENDPOINT_DIRECTION {
  EfiUsbEndpointDirectionHostOut  = 0,
  EfiUsbEndpointDirectionHostIn,
  EfiUsbEndpointDirectionDeviceTx = EfiUsbEndpointDirectionHostIn,
  EfiUsbEndpointDirectionDeviceRx = EfiUsbEndpointDirectionHostOut
} EFI_USBFN_ENDPOINT_DIRECTION;


typedef enum _EFI_USBFN_MESSAGE {
  //
  // Nothing
  //
  EfiUsbMsgNone = 0,
  //
  // SETUP packet is received, returned Buffer contains
  // EFI_USB_DEVICE_REQUEST struct
  //
  EfiUsbMsgSetupPacket,
  //
  // Indicates that some of the requested data has been received from the
  // host. It is the responsibility of the class driver to determine if it
  // needs to wait for any remaining data. Returned Buffer contains
  // EFI_USBFN_TRANSFER_RESULT struct containing endpoint number, transfer
  // status and count of bytes received.
  //
  EfiUsbMsgEndpointStatusChangedRx,
  //
  // Indicates that some of the requested data has been transmitted to the
  // host. It is the responsibility of the class driver to determine if any
  // remaining data needs to be resent. Returned Buffer contains
  // EFI_USBFN_TRANSFER_RESULT struct containing endpoint number, transfer
  // status and count of bytes sent.
  //
  EfiUsbMsgEndpointStatusChangedTx,
  //
  // DETACH bus event signaled
  //
  EfiUsbMsgBusEventDetach,
  //
  // ATTACH bus event signaled
  //
  EfiUsbMsgBusEventAttach,
  //
  // RESET bus event signaled
  //
  EfiUsbMsgBusEventReset,
  //
  // SUSPEND bus event signaled
  //
  EfiUsbMsgBusEventSuspend,
  //
  // RESUME bus event signaled
  //
  EfiUsbMsgBusEventResume,
  //
  // Bus speed updated, returned buffer indicated bus speed using
  // following enumeration named EFI_USB_BUS_SPEED
  //
  EfiUsbMsgBusEventSpeed
} EFI_USBFN_MESSAGE;


typedef enum _EFI_USBFN_TRANSFER_STATUS {
  UsbTransferStatusUnknown = 0,
  UsbTransferStatusComplete,
  UsbTransferStatusAborted,
  UsbTransferStatusActive,
  UsbTransferStatusNone
} EFI_USBFN_TRANSFER_STATUS;


typedef struct _EFI_USBFN_TRANSFER_RESULT {
  UINTN                         BytesTransferred;
  EFI_USBFN_TRANSFER_STATUS     TransferStatus;
  UINT8                         EndpointIndex;
  EFI_USBFN_ENDPOINT_DIRECTION  Direction;
  VOID                          *Buffer;
} EFI_USBFN_TRANSFER_RESULT;

typedef enum _EFI_USB_BUS_SPEED {
  UsbBusSpeedUnknown = 0,
  UsbBusSpeedLow,
  UsbBusSpeedFull,
  UsbBusSpeedHigh,
  UsbBusSpeedSuper,
  UsbBusSpeedMaximum = UsbBusSpeedSuper
} EFI_USB_BUS_SPEED;

typedef union _EFI_USBFN_MESSAGE_PAYLOAD {
  EFI_USB_DEVICE_REQUEST      udr;
  EFI_USBFN_TRANSFER_RESULT   utr;
  EFI_USB_BUS_SPEED           ubs;
} EFI_USBFN_MESSAGE_PAYLOAD;

typedef enum _EFI_USBFN_POLICY_TYPE {
  EfiUsbPolicyUndefined = 0,
  EfiUsbPolicyMaxTransactionSize,
  EfiUsbPolicyZeroLengthTerminationSupport,
  EfiUsbPolicyZeroLengthTermination
} EFI_USBFN_POLICY_TYPE;


/**

 Allocates transfer buffer of the specified size that satisfies
 controller requirements.

 The AllocateTransferBuffer function allocates a memory region of Size bytes and
 returns the address of the allocated memory that satisfies underlying
 controller requirements in the location referenced by Buffer.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_ALLOCATE_TRANSFER_BUFFER) (
  IN EFI_USBFN_IO_PROTOCOL    *This,
  IN UINTN                    Size,
  OUT VOID                    **Buffer
  );

/**

  Deallocates the memory allocated for the transfer buffer by AllocateTransferBuffer function.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_FREE_TRANSFER_BUFFER) (
  IN EFI_USBFN_IO_PROTOCOL    *This,
  IN VOID                     *Buffer
  );

/**
  Returns information about what type of device was attached.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_DETECT_PORT) (
  IN EFI_USBFN_IO_PROTOCOL   *This,
  OUT EFI_USBFN_PORT_TYPE    *PortType
  );

/**
  Configure endpoints based on supplied device and configuration descriptors.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_CONFIGURE_ENABLE_ENDPOINTS) (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN EFI_USB_DEVICE_INFO           *DeviceInfo
  );


/**
  Returns the maximum packet size of the specified endpoint type for the supplied bus speed.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_GET_ENDPOINT_MAXPACKET_SIZE) (
  IN EFI_USBFN_IO_PROTOCOL       *This,
  IN EFI_USB_ENDPOINT_TYPE       EndpointType,
  IN EFI_USB_BUS_SPEED           BusSpeed,
  OUT UINT16                     *MaxPacketSize
  );

/**
  Returns the maximum supported transfer size.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_GET_MAXTRANSFER_SIZE) (
  IN EFI_USBFN_IO_PROTOCOL     *This,
  OUT UINTN                    *MaxTransferSize
  );

/**
  Returns device specific information based on the supplied identifier as a
  Unicode string.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_GET_DEVICE_INFO) (
  IN EFI_USBFN_IO_PROTOCOL      *This,
  IN EFI_USBFN_DEVICE_INFO_ID   Id,
  IN OUT UINTN                  *BufferSize,
  OUT VOID                      *Buffer OPTIONAL
  );

/**
  Returns vendor-id and product-id of the device.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_GET_VENDOR_ID_PRODUCT_ID) (
  IN EFI_USBFN_IO_PROTOCOL      *This,
  OUT UINT16                    *Vid,
  OUT UINT16                    *Pid
  );

/**
  Aborts transfer on the specified endpoint.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_ABORT_TRANSFER) (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction
  );

/**
  Returns the stall state on the specified endpoint.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_GET_ENDPOINT_STALL_STATE) (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
  IN OUT BOOLEAN                  *State
  );

/**
  Sets or clears the stall state on the specified endpoint.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_SET_ENDPOINT_STALL_STATE) (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
  IN BOOLEAN                      State
  );


/**
  This function is called repeatedly to receive updates on USB bus states,
  receive, transmit status changes on endpoints and setup packet on endpoint 0.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_EVENTHANDLER) (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  OUT EFI_USBFN_MESSAGE           *Message,
  IN OUT UINTN                    *PayloadSize,
  OUT EFI_USBFN_MESSAGE_PAYLOAD   *Payload
  );

/**
  Primary function to handle transfer in either direction based on specified
  direction and on the specified endpoint.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_TRANSFER) (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN OUT UINTN                     *BufferSize,
  IN OUT VOID                      *Buffer
  );

/**
  This function supplies power to the USB controller if needed,
  initialize hardware and internal data structures, and then return.

  The port must not be activated by this function.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_START_CONTROLLER) (
  IN EFI_USBFN_IO_PROTOCOL    *This
  );

/**
  This function disables the hardware device by resetting the run/stop bit and
  power off the USB controller if needed.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_STOP_CONTROLLER) (
  IN EFI_USBFN_IO_PROTOCOL    *This
  );

/**
  This function sets the configuration policy for the specified non-control endpoint.

  Refer to the description for calling restrictions.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_SET_ENDPOINT_POLICY) (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
  IN EFI_USBFN_POLICY_TYPE        PolicyType,
  IN UINTN                        BufferSize,
  IN VOID                         *Buffer
  );

/**
  This function retrieves the configuration policy for the specified non-control endpoint.

  There are no associated calling restrictions for this function.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_IO_GET_ENDPOINT_POLICY) (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
  IN EFI_USBFN_POLICY_TYPE        PolicyType,
  IN OUT UINTN                    *BufferSize,
  IN OUT VOID                     *Buffer
  );


struct _EFI_USBFN_IO_PROTOCOL {
  UINT32                                      Revision;
  EFI_USBFN_IO_DETECT_PORT                    DetectPort;
  EFI_USBFN_IO_CONFIGURE_ENABLE_ENDPOINTS     ConfigureEnableEndpoints;
  EFI_USBFN_IO_GET_ENDPOINT_MAXPACKET_SIZE    GetEndpointMaxPacketSize;
  EFI_USBFN_IO_GET_DEVICE_INFO                GetDeviceInfo;
  EFI_USBFN_IO_GET_VENDOR_ID_PRODUCT_ID       GetVendorIdProductId;
  EFI_USBFN_IO_ABORT_TRANSFER                 AbortTransfer;
  EFI_USBFN_IO_GET_ENDPOINT_STALL_STATE       GetEndpointStallState;
  EFI_USBFN_IO_SET_ENDPOINT_STALL_STATE       SetEndpointStallState;
  EFI_USBFN_IO_EVENTHANDLER                   EventHandler;
  EFI_USBFN_IO_TRANSFER                       Transfer;
  EFI_USBFN_IO_GET_MAXTRANSFER_SIZE           GetMaxTransferSize;
  EFI_USBFN_IO_ALLOCATE_TRANSFER_BUFFER       AllocateTransferBuffer;
  EFI_USBFN_IO_FREE_TRANSFER_BUFFER           FreeTransferBuffer;
  //
  // Valid for version EFI_USBFN_IO_PROTOCOL_REVISION2 and above
  //
  EFI_USBFN_IO_START_CONTROLLER               StartController;
  EFI_USBFN_IO_STOP_CONTROLLER                StopController;
  EFI_USBFN_IO_SET_ENDPOINT_POLICY            SetEndpointPolicy;
  EFI_USBFN_IO_GET_ENDPOINT_POLICY            GetEndpointPolicy;
};


extern EFI_GUID gEfiUsbFnIoProtocolGuid;
#endif

