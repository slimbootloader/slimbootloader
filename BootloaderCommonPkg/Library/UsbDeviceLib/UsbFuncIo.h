/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __USB_FUNCTION_IO_H__
#define __USB_FUNCTION_IO_H__

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Protocol/EfiUsbFnIo.h>
#include <Library/UsbDeviceLib.h>
#include <Library/PrintLib.h>
#include "UsbDeviceLibPrivate.h"
#include "UsbIoNode.h"
#include "XdciDWC.h"
#include "UsbDeviceMode.h"

//
// Debug message setting
//
#define USB_FUIO_DEBUG_INFO              DEBUG_INFO
#define USB_FUIO_DEBUG_LOAD              DEBUG_LOAD
#define USB_FUIO_DEBUG_ERROR             DEBUG_ERROR
#define USB_FUIO_DEBUG_EVENT_I           0 //DEBUG_INIT
#define USB_FUIO_DEBUG_EVENT_D           DEBUG_ERROR
#define USB_FUIO_DEBUG_EVENT_NOTREADY_D  DEBUG_ERROR
#define USB_FUIO_DEBUG_EVENT_NOTREADY_I  0 //DEBUG_INIT

#define MAX_TRANSFER_PACKET     (8 * 1024 * 1024)

#define USBFU_VID     0x8086
#define USBFU_PID     0x0A65
#define DEVICE3_BASE_ADDRESS      0x5E
#define WKC_USBSRCDETSTATUS0_REG  0x29

#pragma pack(1)
typedef struct {
  UINT8                     ProgInterface;
  UINT8
  SubClassCode;
  UINT8
  BaseCode;
} USB_CLASSC;

//
// Event Buffer Struct
//
typedef
struct  {
  UINT32 Event;
  UINT32
  DevTstLmp1;
  UINT32
  DevTstLmp2;
  UINT32
  Reserved;
} USBD_EVENT_BUF;

typedef
struct  {
  UINT32                        EpNum;
  EFI_USBFN_ENDPOINT_DIRECTION
  Direction;
  UINTN
  XferAddress;
  UINT32
  XferLength;
  UINT8
  LogEpNum;
  BOOLEAN
  Complete;
  BOOLEAN
  ZlpFlag;
} USBD_EP_XFER_REC;

#pragma pack()

EFI_STATUS
UsbFnInitDevice (
  IN EFI_USBFN_IO_PROTOCOL        *This
  );

EFI_STATUS
UsbFnDeInitDevice (
  IN EFI_USBFN_IO_PROTOCOL        *This
  );

/**
  Returns information about what type of device was attached.

  @param[in]  This               A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[out] PortType           Returns the USB port type.


  @retval EFI_SUCCESS            The operation completed successfully.
  @retval EFI_INVALID_PARAMETER  A parameter is invalid.
  @retval EFI_DEVICE_ERROR       The physical device reported an error.
  @retval EFI_NOT_READY          The physical device is busy or not ready to
                                 process this request or the device is not
                                 attached to the host.


**/
EFI_STATUS
EFIAPI
DetectPort (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  OUT EFI_USBFN_PORT_TYPE          *PortType
  );

/**
  The AllocateTransferBuffer function allocates a memory region of Size bytes
  and returns the address of the allocated memory that satisfies underlying
  controller requirements in the location referenced by Buffer.

  @param[in] This               A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in] Size               The number of bytes to allocate for the transfer
                                Buffer.
  @param[out] Buffer            A pointer to a pointer to the allocated Buffer
                                if the call succeeds; undefined otherwise.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval The requested transfer Buffer could not be allocated.

**/
EFI_STATUS
EFIAPI
AllocateTransferBuffer (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINTN                         Size,
  OUT VOID                         **Buffer
  );

/**
  Deallocates the memory allocated for the transfer Buffer by
  AllocateTransferBuffer function.

  @param[in] This               A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in] Buffer             Buffer Pointer to the transfer Buffer
                                to deallocate.
  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.

**/
EFI_STATUS
EFIAPI
FreeTransferBuffer (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN VOID                          *Buffer
  );

/**
  Configure endpoints Based on supplied device and configuration descriptors.

  @param[in] This               A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in] DeviceInfo         A pointer to EFI_USBFN_DEVICE_INFO instance.

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_NOT_READY         The physical device is busy or not ready to
                                process this request.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to
                                lack of resources.

**/
EFI_STATUS
EFIAPI
ConfigureEnableEndpoints (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN EFI_USB_DEVICE_INFO           *DeviceInfo
  );

/**
  Returns the maximum packet size of the specified endpoint type for
  the supplied bus Speed.

  @param[in]  This               A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]  EndpointType       Endpoint type as defined as EFI_USB_ENDPOINT_TYPE.
  @param[in]  BusSpeed           Bus Speed as defined as EFI_USB_BUS_SPEED.
  @param[out] MaxPacketSize     The maximum packet size, in bytes,
                                of the specified endpoint type.

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_NOT_READY         The physical device is busy or not ready to
                                process this request.
**/
EFI_STATUS
EFIAPI
GetEndpointMaxPacketSize (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN EFI_USB_ENDPOINT_TYPE         EndpointType,
  IN EFI_USB_BUS_SPEED             BusSpeed,
  OUT UINT16                       *MaxPacketSize
  );

/**
  Returns the maximum supported transfer size.

  @param[in]  This               A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[out] MaxTransferSize    The maximum supported transfer size, in bytes.

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_NOT_READY         The physical device is busy or not ready to
                                process this request.
**/
EFI_STATUS
EFIAPI
GetMaxTransferSize (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  OUT UINTN                        *MaxTransferSize
  );

/**
  Returns device specific information Based on the supplied identifier as
  a Unicode string.

  @param[in]      This          A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]      Id            Requested information id.
  @param[in,out]  BufferSize    On input, the size of the Buffer in bytes.
                                On output, the amount of data returned in Buffer
                                in bytes.
  @param[out]     Buffer        A pointer to a Buffer to return the requested
                                information as a Unicode string. What string are
                                we talking about

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_NOT_READY         The physical device is busy or not ready to
                                process this request.
**/
EFI_STATUS
EFIAPI
GetDeviceInfo (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN EFI_USBFN_DEVICE_INFO_ID      Id,
  IN OUT UINTN                     *BufferSize,
  OUT VOID                         *Buffer OPTIONAL
  );

/**
  Returns vendor-id and product-id of the device.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[out] Vid               Returned vendor-id of the device.
  @param[out] Pid               Returned product-id of the device.

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         Unable to return vid or pid.

**/
EFI_STATUS
EFIAPI
GetVendorIdProductId (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  OUT UINT16                       *Vid,
  OUT UINT16                       *Pid
  );

/**
  Aborts transfer on the specified endpoint.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]  EndpointIndex     Indicates the endpoint on which the ongoing
                                transfer needs to be canceled.
  @param[in]  Direction         Direction of the endpoint.


  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_NOT_READY         The physical device is busy or not ready to
                                process this request.

**/
EFI_STATUS
EFIAPI
AbortTransfer (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction
  );

/**
  Returns the stall state on the specified endpoint.

  @param[in]       This                  A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]       EndpointIndex         Indicates the endpoint on which the ongoing
                                         transfer needs to be canceled.
  @param[in]       Direction             Direction of the endpoint.
  @param[in, out]  State                 Boolean, true value indicates that the endpoint
                                         is in a stalled state, false otherwise.

  @retval          EFI_SUCCESS           The function returned successfully.
  @retval          EFI_INVALID_PARAMETER A parameter is invalid.
  @retval          EFI_DEVICE_ERROR      The physical device reported an error.
  @retval          EFI_NOT_READY         The physical device is busy or not ready to
                                         process this request.

**/
EFI_STATUS
EFIAPI
GetEndpointStallState (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN OUT BOOLEAN                   *State
  );

/**
  Sets or clears the stall state on the specified endpoint.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]  EndpointIndex     Indicates the endpoint on which the ongoing
                                transfer needs to be canceled.
  @param[in]  Direction         Direction of the endpoint.
  @param[in]  State             Requested stall state on the specified endpoint.
                                True value causes the endpoint to stall;
                                false value clears an existing stall.

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_NOT_READY         The physical device is busy or not ready to
                                process this request.

**/
EFI_STATUS
EFIAPI
SetEndpointStallState (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN BOOLEAN                       State
  );

/**
  ISR inokes Event Handler.  Look at which interrupt has happened and see
  if there are event handler registerd and if so fire them 1 by one.

  @param[in]        This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[out]       Message           Indicates the event that initiated this
                                      notification.
  @param[in, out]   PayloadSize       On input, the size of the memory pointed by Payload.
                                      On output, the amount of data returned in Payload.
  @param[out]       Payload           A pointer to EFI_USBFN_MESSAGE_PAYLOAD instance to
                                      return additional payload for current message.




  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_NOT_READY         The physical device is busy or not ready to
                                process this request.
  @retval EFI_BUFFER_TOO_SMALL  Supplied Buffer not large enough to hold
                                the message payload.

**/
EFI_STATUS
EFIAPI
EventHandler (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  OUT EFI_USBFN_MESSAGE            *Message,
  IN OUT UINTN                     *PayloadSize,
  OUT EFI_USBFN_MESSAGE_PAYLOAD    *Payload
  );

/**
  Primary function to handle transfer in either direction Based on specified
  direction and on the specified endpoint.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]  EndpointIndex     Indicates the endpoint on which TX or RX transfer
                                needs to take place.
  @param[in]  Direction         Direction of the endpoint.
  @param[in, out]  BufferSize   If Direction is EfiUsbEndpointDirectionDeviceRx:
                                On input, the size of the Buffer in bytes.
                                On output, the amount of data returned in Buffer in bytes.
                                If Direction is EfiUsbEndpointDirectionDeviceTx:
                                On input, the size of the Buffer in bytes.
                                On output, the amount of data actually transmitted in bytes.
  @param[in, out]  Buffer       If Direction is EfiUsbEndpointDirectionDeviceRx:
                                The Buffer to return the received data.
                                If Direction is EfiUsbEndpointDirectionDeviceTx:
                                The Buffer that contains the data to be transmitted.

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_NOT_READY         The physical device is busy or not ready to
                                process this request.

**/
EFI_STATUS
EFIAPI
Transfer (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN OUT UINTN                     *BufferSize,
  IN OUT VOID                      *Buffer
  );

EFI_STATUS
EFIAPI
StartController (
  IN EFI_USBFN_IO_PROTOCOL         *This
  );

EFI_STATUS
EFIAPI
StopController (
  IN EFI_USBFN_IO_PROTOCOL         *This
  );

/**
  This function sets the configuration policy for the specified non-control endpoint.
  Refer to the description for calling restrictions

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]  EndpointIndex     Indicates the non-control endpoint for
                                which the policy needs to be set.
  @param[in]  Direction         Direction of the endpoint.
  @param[in]  PolicyType        Policy type the user is trying to set for
                                the specified non-control endpoint.
  @param[in]  BufferSize        The size of the Buffer in bytes.
  @param[in]  Buffer            The new value for the policy parameter that
                                PolicyType specifies.


  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_UNSUPPORTED       Changing this policy value is not supported.

**/
EFI_STATUS
EFIAPI
SetEndpointPolicy (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN EFI_USBFN_POLICY_TYPE         PolicyType,
  IN UINTN                         BufferSize,
  IN VOID                          *Buffer
  );

/**
  This function retrieves the configuration policy for the specified non-control
  endpoint. There are no associated calling restrictions for this function.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]  EndpointIndex     Indicates the non-control endpoint for
                                which the policy needs to be set.
  @param[in]  Direction         Direction of the endpoint.
  @param[in]  PolicyType        Policy type the user is trying to set for
                                the specified non-control endpoint.
  @param[in, out]  BufferSize   The size of the Buffer in bytes.
  @param[in, out]  Buffer       The new value for the policy parameter that
                                PolicyType specifies.


  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
  @retval EFI_UNSUPPORTED       Changing this policy value is not supported.
  @retval EFI_BUFFER_TOO_SMALL  Supplied Buffer is not large enough to
                                hold requested policy value.

**/
EFI_STATUS
EFIAPI
GetEndpointPolicy (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN EFI_USBFN_POLICY_TYPE         PolicyType,
  IN OUT UINTN                     *BufferSize,
  IN OUT VOID                      *Buffer
  );

/**
  Copies relevant endpoint data from standard USB endpoint descriptors
  to the usbEpInfo structure used by the XDCI.

  @param pEpDest   destination structure
  @param pEpSrc    source structure

  @return VOID

**/
VOID
UsbFnSetEpInfo (
  IN USB_EP_INFO                 *EpDest,
  IN USB_DEVICE_ENDPOINT_INFO    *EpSrc
  );

extern
EFI_USBFN_IO_PROTOCOL
mUsbFunIoProtocol;
#endif

