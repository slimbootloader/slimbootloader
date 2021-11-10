/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UsbDeviceDxe.h"

//
// 16 bytes in a guid x 2 characters per byte, 4 chars for dashes and a NUL
//
#define CHARS_IN_GUID (sizeof(GUID) * 2 + 4 + 1)

//
// Strings that get sent with the USB Connection
//
STATIC CHAR16 mUsbFnDxeMfgString[] = L"Intel Corporation";
STATIC CHAR16 mUsbFnDxeProductString[] = L"Broxton";
STATIC CHAR16 mUsbFnDxeSerialNumber[] = L"INT123456";

//
// Duplicated from MiscSystemManufacturerData.c Some parts of it will
// replaced with device-specific unique values.
//
STATIC
GUID
mSmBiosUniqueGuid = {
  0x5e24fe9c, 0xc8d0, 0x45bd, 0xa7, 0x9f, 0x54, 0xea, 0x5f, 0xbd, 0x3d, 0x97
};

EFI_USBFN_IO_PROTOCOL         mUsbFunIoProtocol = {
  EFI_USBFN_IO_PROTOCOL_REVISION,
  DetectPort,
  ConfigureEnableEndpoints,
  GetEndpointMaxPacketSize,
  GetDeviceInfo,
  GetVendorIdProductId,
  AbortTransfer,
  GetEndpointStallState,
  SetEndpointStallState,
  EventHandler,
  Transfer,
  GetMaxTransferSize,
  AllocateTransferBuffer,
  FreeTransferBuffer,
  StartController,
  StopController,
  SetEndpointPolicy,
  GetEndpointPolicy
};


EFI_STATUS
PrintEventBuffer (
  IN  EFI_USBFN_IO_PROTOCOL  *This
  )
{
  UINT32                          EventCount;
  USB_XDCI_DEV_CONTEXT            *UsbFuncIoDevPtr;
  XDCI_CORE_HANDLE                *XdciCorePtr;
  USB_DEV_CORE                    *UsbDeviceCorePtr;
  UINT32                          Index;
  UINT32                          *DbBufPtr;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;

  EventCount = UsbRegRead ((UINT32)UsbFuncIoDevPtr->XdciMmioBarAddr, DWC_XDCI_EVNTCOUNT_REG (0));

  DbBufPtr = (UINT32 *) (UINTN)XdciCorePtr->CurrentEventBuffer;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "FUEV:: XdciCorePtr->AlignedEventBuffers 0x%08x\n",
          (UINTN)XdciCorePtr->AlignedEventBuffers));
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "FUEV:: DUMP BUF_S\n"));
  for (Index = 0; Index < ((EventCount / 4) + 1); Index++) {
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "0x%08x\n", DbBufPtr[Index]));
  }
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "FUEV:: DUMP BUF_E\n"));

  return EFI_SUCCESS;
}

/**
Debug End
**/

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
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT EFI_USBFN_PORT_TYPE    *PortType
  )
{
  USB_XDCI_DEV_CONTEXT  *UsbFuncIoDevPtr;
  EFI_STATUS            Status;

  DEBUG ((USB_FUIO_DEBUG_INFO, "DetectPort - Entry\n"));

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  Status = GetUsbPortType (PortType);

  DEBUG ((USB_FUIO_DEBUG_INFO, "DetectPort - Exit\n"));
  return Status;
}


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
  IN EFI_USBFN_IO_PROTOCOL    *This,
  IN UINTN                    Size,
  OUT VOID                    **Buffer
  )
{
  EFI_STATUS            Status;
  USB_XDCI_DEV_CONTEXT  *UsbFuncIoDevPtr;
  VOID                  *AllocateBufferPtr;
  USB_MEM_NODE          *NodePtr;

  DEBUG ((USB_FUIO_DEBUG_INFO, "AllocateTransferBuffer - Entry\n"));

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  if (Size == 0) {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  AllocateBufferPtr = AllocateZeroPool (Size);

  if (AllocateBufferPtr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  //
  // Create new node
  //
  Status = InsertNewNodeToHead (This, &NodePtr);
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  NodePtr->Size = Size;
  NodePtr->AllocatePtr = AllocateBufferPtr;

  *Buffer = AllocateBufferPtr;

  DEBUG ((USB_FUIO_DEBUG_INFO, "AllocateTransferBuffer addr 0x%08x\n", AllocateBufferPtr));
  DEBUG ((USB_FUIO_DEBUG_INFO, "AllocateTransferBuffer - Exit\n"));
  return EFI_SUCCESS;

ErrorExit:

  DEBUG ((USB_FUIO_DEBUG_ERROR, "AllocateTransferBuffer - ERRROR %r\n", Status));
  return Status;
}


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
  IN EFI_USBFN_IO_PROTOCOL    *This,
  IN VOID                     *Buffer
  )
{
  EFI_STATUS            Status;
  USB_XDCI_DEV_CONTEXT  *UsbFuncIoDevPtr;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  DEBUG ((USB_FUIO_DEBUG_LOAD, "FreeTransferBuffer - Entry\n"));

  Status = RemoveNode (This, Buffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((USB_FUIO_DEBUG_LOAD, "FreeTransferBuffer - ERROR\n"));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((USB_FUIO_DEBUG_LOAD, "FreeTransferBuffer - Exit\n"));
  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                        Status;
  USB_XDCI_DEV_CONTEXT              *UsbFuncIoDevPtr;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  Status = EFI_SUCCESS;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "ConfigureEnableEndpoints - Entry\n"));
  //
  //Assuming that the hardware has already been initialized,
  //this function configures the endpoints using supplied
  //DeviceInfo, activates the port, and starts receiving USB events
  //
  Status = EFI_SUCCESS;
  if (DeviceInfo == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto FUNC_EXIT;
  }

  UsbFuncIoDevPtr->DevInfoPtr->DeviceDescriptor = DeviceInfo->DeviceDescriptor;

  //
  // Set Configure table
  //
  if (DeviceInfo->DeviceDescriptor->NumConfigurations > 1) {
    DEBUG ((DEBUG_ERROR, "!!!Error ConfigNum over '1' %d\n", DeviceInfo->DeviceDescriptor->NumConfigurations));
  }
  UsbFuncIoDevPtr->IndexPtrConfig.ConfigDescriptor = DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor;
  UsbFuncIoDevPtr->IndexPtrConfig.InterfaceInfoTable[0] = DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0];

  //
  // Set Interface
  //
  if (DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->NumInterfaces > 1) {
    DEBUG ((DEBUG_ERROR, "!!!Error NumInterfaces[0] over '1' %d\n",
            DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->NumInterfaces));
  }
  UsbFuncIoDevPtr->IndexPtrInteface.InterfaceDescriptor =
    DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->InterfaceDescriptor;

  //
  // Set Endpoint
  //
  if (UsbFuncIoDevPtr->IndexPtrInteface.InterfaceDescriptor->NumEndpoints > 2) {
    DEBUG ((DEBUG_ERROR, "!!!Error NumEndPoint[0] over '2' %d\n",
            UsbFuncIoDevPtr->IndexPtrInteface.InterfaceDescriptor->NumEndpoints));
  }

  UsbFuncIoDevPtr->IndexPtrInEp.EndpointCompDesc = NULL;
  UsbFuncIoDevPtr->IndexPtrOutEp.EndpointCompDesc = NULL;

  if ((DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->EndpointDescriptorTable[0]->EndpointAddress &
       USB_ENDPOINT_DIR_IN) != 0) {
    UsbFuncIoDevPtr->IndexPtrInEp.EndpointDesc =
      DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->EndpointDescriptorTable[0];
    UsbFuncIoDevPtr->IndexPtrOutEp.EndpointDesc =
      DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->EndpointDescriptorTable[1];
  } else {
    UsbFuncIoDevPtr->IndexPtrInEp.EndpointDesc =
      DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->EndpointDescriptorTable[1];
    UsbFuncIoDevPtr->IndexPtrOutEp.EndpointDesc =
      DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->EndpointDescriptorTable[0];
  }

  DEBUG ((USB_FUIO_DEBUG_LOAD, " In Ep Num 0x%02x\n", UsbFuncIoDevPtr->IndexPtrInEp.EndpointDesc->EndpointAddress));

  DEBUG ((USB_FUIO_DEBUG_LOAD, " Out Ep Num 0x%02x\n", UsbFuncIoDevPtr->IndexPtrOutEp.EndpointDesc->EndpointAddress));

FUNC_EXIT:
  DEBUG ((USB_FUIO_DEBUG_LOAD, "ConfigureEnableEndpoints - exit %r\n", Status));
  return Status;
}

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
  IN EFI_USBFN_IO_PROTOCOL      *This,
  IN EFI_USB_ENDPOINT_TYPE      EndpointType,
  IN EFI_USB_BUS_SPEED          BusSpeed,
  OUT UINT16                    *MaxPacketSize
  )
{
  EFI_STATUS                        Status;
  USB_XDCI_DEV_CONTEXT              *UsbFuncIoDevPtr;
  USB_DEV_CORE                      *DevCorePtr;
  XDCI_CORE_HANDLE                  *XdciCorePtr;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  DevCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = DevCorePtr->ControllerHandle;
  Status = EFI_SUCCESS;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "GetEndpointMaxPacketSize - Entry\n"));

  switch (EndpointType) {
  case UsbEndpointControl:
#ifdef SUPPORT_SUPER_SPEED
    *MaxPacketSize = USB_EP0_MAX_PKT_SIZE_SS; // Default to super Speed
#else
    *MaxPacketSize = USB_EP0_MAX_PKT_SIZE_HS; // Default to high Speed
#endif
    break;

  case UsbEndpointBulk:
#ifdef SUPPORT_SUPER_SPEED
    *MaxPacketSize = USB_BULK_EP_PKT_SIZE_SS; // Default to super Speed
#else
    *MaxPacketSize = USB_BULK_EP_PKT_SIZE_HS; // Default to high Speed
#endif
    break;

  case UsbEndpointInterrupt:
    *MaxPacketSize = 1;
    break;

  case UsbEndpointIsochronous:
  default:
    Status = EFI_DEVICE_ERROR;
    break;
  }

  DEBUG ((USB_FUIO_DEBUG_LOAD, "GetEndpointMaxPacketSize - Exit %r\n", Status));
  return Status;
}


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
  IN EFI_USBFN_IO_PROTOCOL     *This,
  OUT UINTN                    *MaxTransferSize
  )
{
  //
  // Need to check, Make max transfer package to 8MB
  //
  *MaxTransferSize = MAX_TRANSFER_PACKET;
  return EFI_SUCCESS;
}


/**
  This function returns the unique device ID of the device--this matches
  what is populated in the SMBIOS table.

  @param[in, out] BufferSize    On input, the size of the Buffer in bytes.
                                On output, the amount of data returned in Buffer
                                in bytes.

  @param[out]     Buffer        A pointer to a Buffer to return the requested
                                information as a Unicode string. What string are
                                we talking about

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_BUFFER_TOO_SMALL  A parameter is invalid.

**/
STATIC
EFI_STATUS
EFIAPI
GetDeviceSerialNumber (
  IN OUT UINTN *BufferSize,
  OUT VOID *Buffer OPTIONAL
  )
{
  EFI_STATUS Status;
  CHAR16 UuidString[CHARS_IN_GUID];
  UINTN CharsCopied;

  Status = EFI_SUCCESS;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "+GetDeviceSerialNumber\n"));
  //
  // check bounds
  //
  if (*BufferSize < sizeof (UuidString)) {
    Status = EFI_BUFFER_TOO_SMALL;
    *BufferSize = 0;
    goto Error;
  }

  //
  // The rest of mSmBiosUniqueGuid will be same. Note that we cannot
  // read the SMBIOS table directly, as it might not be ready by the time we
  // are to read it. The population of the data from the eMMC is ready
  // by the time we are here.
  //

  //
  // Print to to a string, and copy it off
  //
  CharsCopied = UnicodeSPrint (UuidString, sizeof (UuidString), L"%g", &mSmBiosUniqueGuid);
  if (CharsCopied != (CHARS_IN_GUID - 1)) {
    Status = EFI_BUFFER_TOO_SMALL;
    *BufferSize = 0;
    goto Error;
  }
  CopyMem (Buffer, UuidString, sizeof (UuidString));
  *BufferSize = sizeof (UuidString);

Error:

  DEBUG ((USB_FUIO_DEBUG_LOAD, "-GetDeviceSerialNumber, Status = 0x%08x\r\n", Status));

  return Status;
}


/**
  Returns device specific information Based on the supplied identifier as
  a Unicode string

  @param[in]       This          A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]       Id            Requested information id.
  @param[in, out]  BufferSize    On input, the size of the Buffer in bytes.
                                 On output, the amount of data returned in Buffer
                                 in bytes.
  @param[out]      Buffer        A pointer to a Buffer to return the requested
                                 information as a Unicode string. What string are
                                 we talking about

  @retval EFI_SUCCESS            The function returned successfully.
  @retval EFI_INVALID_PARAMETER  A parameter is invalid.
  @retval EFI_DEVICE_ERROR       The physical device reported an error.
  @retval EFI_NOT_READY          The physical device is busy or not ready to
                                 process this request.
**/
EFI_STATUS
EFIAPI
GetDeviceInfo (
  IN EFI_USBFN_IO_PROTOCOL      *This,
  IN EFI_USBFN_DEVICE_INFO_ID   Id,
  IN OUT UINTN                  *BufferSize,
  OUT VOID                      *Buffer OPTIONAL
  )
{
  EFI_STATUS                    Status;
  USB_XDCI_DEV_CONTEXT          *UsbFuncIoDevPtr;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  Status = EFI_SUCCESS;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "GetDeviceInfo - Entry\n"));

  if ((BufferSize == 0) || (Buffer == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    goto FUN_EXIT;
  }

  switch (Id) {

  //
  // FIXME: Get real serial number of board
  //
  case EfiUsbDeviceInfoSerialNumber:
    if (*BufferSize < sizeof (mUsbFnDxeSerialNumber)) {
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = 0;
      goto FUN_EXIT;
    }
    CopyMem (Buffer, mUsbFnDxeSerialNumber, sizeof (mUsbFnDxeSerialNumber));
    *BufferSize = sizeof (mUsbFnDxeSerialNumber);
    break;

  case EfiUsbDeviceInfoManufacturerName:
    if (*BufferSize < sizeof (mUsbFnDxeMfgString)) {
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = 0;
      goto FUN_EXIT;
    }
    CopyMem (Buffer, mUsbFnDxeMfgString, sizeof (mUsbFnDxeMfgString));
    *BufferSize = sizeof (mUsbFnDxeMfgString);
    break;

  case EfiUsbDeviceInfoProductName:
    if (*BufferSize < sizeof (mUsbFnDxeProductString)) {
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = 0;
      goto FUN_EXIT;
    }
    CopyMem (Buffer, mUsbFnDxeProductString, sizeof (mUsbFnDxeProductString));
    *BufferSize = sizeof (mUsbFnDxeProductString);
    break;

  case EfiUsbDeviceInfoUnknown:
  default:
    Status = EFI_UNSUPPORTED;
    *BufferSize = 0;
    DEBUG ((USB_FUIO_DEBUG_ERROR, "Unknown ID %d encountered.\r\n", Id));
    break;
  }

FUN_EXIT:
  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbSetconfigure - ConfigDescriptor addr 0x%08x \n",
          (UINTN)UsbFuncIoDevPtr->IndexPtrConfig.ConfigDescriptor));
  DEBUG ((USB_FUIO_DEBUG_LOAD, "GetDeviceInfo - Exit %r\n", Status));
  return Status;
}


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
  IN EFI_USBFN_IO_PROTOCOL      *This,
  OUT UINT16                    *Vid,
  OUT UINT16                    *Pid
  )
{
  USB_XDCI_DEV_CONTEXT              *UsbFuncIoDevPtr;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  //
  //   *Vid = 0x8086
  //   *Pid = 0x0A65
  //
  *Vid = UsbFuncIoDevPtr->VendorId;
  *Pid = UsbFuncIoDevPtr->DeviceId;
  return EFI_SUCCESS;
}

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
  )
{
  USB_XDCI_DEV_CONTEXT              *UsbFuncIoDevPtr;
  XDCI_CORE_HANDLE                  *XdciCorePtr;
  USB_DEV_CORE                      *UsbDeviceCorePtr;
  USB_EP_INFO                       EpInfo;
  EFI_STATUS                        Status;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "FU:AbortTransfer - Entry\n"));
  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  Status = EFI_SUCCESS;

  if (UsbFuncIoDevPtr->DevResetFlag == TRUE) {
    return Status;
  }

  EpInfo.EpNum = EndpointIndex;
  EpInfo.EpDir = (Direction != 0 ) ? UsbEpDirIn : UsbEpDirOut;

  Status = UsbDeviceEpCancelTransfer (UsbFuncIoDevPtr->DrvCore, &EpInfo);

  DEBUG ((USB_FUIO_DEBUG_LOAD, "FU:AbortTransfer - Exit %r\n", Status));
  return Status;
}

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
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
  IN OUT BOOLEAN                  *State
  )
{
  USB_XDCI_DEV_CONTEXT              *UsbFuncIoDevPtr;
  XDCI_CORE_HANDLE                  *XdciCorePtr;
  UINT32                            EndPoint;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  DEBUG ((USB_FUIO_DEBUG_LOAD, "GetEndpointStallState - Entry\n"));

  EndPoint = UsbGetPhysicalEpNum (EndpointIndex, (Direction != 0 ) ? UsbEpDirIn : UsbEpDirOut);

  XdciCorePtr = UsbFuncIoDevPtr->XdciDrvIfHandle;

  if (XdciCorePtr->EpHandles[EndPoint].State == USB_EP_STATE_STALLED) {
    *State = TRUE;
  } else {
    *State = FALSE;
  }

  DEBUG ((USB_FUIO_DEBUG_LOAD, "GetEndpointStallState - Exit\n"));
  return EFI_SUCCESS;
}


EFI_STATUS
UsbSetAddress (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT32                        Address
  )
{
  EFI_STATUS                        Status;
  USB_XDCI_DEV_CONTEXT              *UsbFuncIoDevPtr;
  XDCI_CORE_HANDLE                  *XdciCorePtr;
  USB_DEV_CORE                      *UsbDeviceCorePtr;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbSetAddress - 0x%04x Entry\n", Address));

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  Status = EFI_SUCCESS;

  Status = UsbDeviceSetAddress (UsbDeviceCorePtr, (UINT32)Address);

  if (Status != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
    goto EXIT_SET_ADDRESS;
  }

  Status = UsbDeviceEp0TxStatus (UsbDeviceCorePtr);

  if (Status != EFI_SUCCESS) {
    Status = EFI_NO_RESPONSE;
    goto EXIT_SET_ADDRESS;
  }

EXIT_SET_ADDRESS:

  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbSetAddress - Exit %r\n", Status));
  return Status;
}


EFI_STATUS
EFIAPI
UsbSetconfigure (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT32                       InterFaceIndex
  )
{
  EFI_STATUS                      Status;
  USB_XDCI_DEV_CONTEXT            *UsbFuncIoDevPtr;
  XDCI_CORE_HANDLE                *XdciCorePtr;
  USB_DEV_CORE                    *UsbDeviceCorePtr;
  UINT32                          InterfaceNum;
  UINT32                          EndPointNum;
  UINT32                          EndPointIndex;
  EFI_USB_INTERFACE_INFO          *InterfaceInfoPtr;
  USB_EP_INFO                     EpInfo;
  USB_DEVICE_ENDPOINT_INFO        EpDescInfo;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbSetconfigure - 0x%04x Entry\n", InterFaceIndex));

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  Status = EFI_SUCCESS;

  InterfaceNum = UsbFuncIoDevPtr->IndexPtrConfig.ConfigDescriptor->NumInterfaces;
  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbSetconfigure - ConfigDescriptor addr 0x%08x \n",
          (UINTN)UsbFuncIoDevPtr->IndexPtrConfig.ConfigDescriptor));

  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbSetconfigure - DescriptorType 0x%04x ; ConfigurationValue 0x%04x\n",
          UsbFuncIoDevPtr->IndexPtrConfig.ConfigDescriptor->DescriptorType,
          UsbFuncIoDevPtr->IndexPtrConfig.ConfigDescriptor->ConfigurationValue
          ));

  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbSetconfigure - InterfaceNum 0x%04x \n", InterfaceNum));
  if (InterfaceNum < InterFaceIndex) {
    Status = EFI_INVALID_PARAMETER;
    goto EXIT__SET_CONFIGURE;
  }

  //
  // Arry strart form '0', Index start from '1'.
  //
  InterfaceInfoPtr = UsbFuncIoDevPtr->IndexPtrConfig.InterfaceInfoTable[InterFaceIndex - 1];
  EndPointNum = InterfaceInfoPtr->InterfaceDescriptor->NumEndpoints;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbSetconfigure - Total EP NUM 0x%04x \n", EndPointNum));

  for (EndPointIndex = 0; EndPointIndex < EndPointNum; EndPointIndex++) {
    EpDescInfo.EndpointDesc = InterfaceInfoPtr->EndpointDescriptorTable[EndPointIndex];
    EpDescInfo.EndpointCompDesc = NULL;
    UsbFnSetEpInfo (&EpInfo, &EpDescInfo);
    DEBUG ((USB_FUIO_DEBUG_LOAD, "EndpointAddress 0x%02x\n", EpDescInfo.EndpointDesc->EndpointAddress));

    if (UsbDeviceInitEp (UsbDeviceCorePtr, &EpInfo) == EFI_SUCCESS) {
      if (UsbDeviceEpEnable (UsbDeviceCorePtr, &EpInfo) == EFI_SUCCESS) {
      } else {
        Status = EFI_DEVICE_ERROR;
        DEBUG ((DEBUG_INFO, "UsbDeviceEpEnable() - Failed to enable endpoint\n"));
      }
    } else {
      Status = EFI_DEVICE_ERROR;
      DEBUG ((DEBUG_INFO, "UsbDeviceInitEp() - Failed to initialize endpoint\n"));
    }
  }

  Status = UsbDeviceEp0TxStatus (UsbDeviceCorePtr);

  if (Status != EFI_SUCCESS) {
    Status = EFI_NO_RESPONSE;
    goto EXIT__SET_CONFIGURE;
  }


EXIT__SET_CONFIGURE:
  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbSetconfigure - Exit %r\n", Status));

  return Status;
}

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
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
  IN BOOLEAN                      State
  )
{
  EFI_STATUS                        Status;
  USB_XDCI_DEV_CONTEXT              *UsbFuncIoDevPtr;
  USB_EP_INFO                       pEpInfo;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  DEBUG ((USB_FUIO_DEBUG_LOAD, "SetEndpointStallState - Entry\n"));
  Status = EFI_SUCCESS;

  pEpInfo.EpNum = EndpointIndex;
  pEpInfo.EpDir = (Direction != 0 ) ? UsbEpDirIn : UsbEpDirOut;

  if (State) {
    Status = UsbDeviceEpStall (UsbFuncIoDevPtr->DrvCore, (VOID *) (UINTN) &pEpInfo);
  } else {
    Status = UsbDeviceEpClearStall (UsbFuncIoDevPtr->DrvCore, (VOID *) (UINTN) &pEpInfo);
  }

  if (Status != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
  }

  DEBUG ((USB_FUIO_DEBUG_LOAD, "SetEndpointStallState - Exit\n"));
  return Status;
}

EFI_STATUS
DeviceEventCheck (
  IN  EFI_USBFN_IO_PROTOCOL       *This,
  IN  USBD_EVENT_BUF              *EventIndex,
  OUT UINT32                      *ProcessSize,
  OUT EFI_USBFN_MESSAGE           *Message,
  OUT BOOLEAN                     *EventFlag
  )
{
  USB_XDCI_DEV_CONTEXT            *UsbFuncIoDevPtr;
  UINT32                          EventReg;
  USB_DEV_CORE                    *UsbDeviceCorePtr;
  XDCI_CORE_HANDLE                *XdciCorePtr;

  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "\n FUEV::DeviceEvent entry....\n"));
  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  EventReg = (EventIndex->Event & DWC_XDCI_EVENT_BUFF_DEV_EVT_MASK);
  EventReg >>= DWC_XDCI_EVENT_BUFF_DEV_EVT_BIT_POS;
  *EventFlag = FALSE;

  //
  // Assume default event size. Change it in switch case if
  // different
  //
  *ProcessSize =  DWC_XDCI_DEV_EVENT_DEFAULT_SIZE_IN_BYTES;

  switch (EventReg) {
  case DWC_XDCI_EVENT_BUFF_DEV_DISCONN_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_DISCONN_EVENT\n"));
    *Message = EfiUsbMsgBusEventDetach;
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_USB_RESET_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_USB_RESET_EVENT\n"));
    //
    //  In resetDet will prepare setup Xfer package
    //
    UsbFuncIoDevPtr->DevReConnect = FALSE;
    UsbFuncIoDevPtr->DevResetFlag = TRUE;

    UsbProcessDeviceResetDet (XdciCorePtr);
    UsbDeviceSetAddress (UsbDeviceCorePtr, 0);
    *Message = EfiUsbMsgBusEventReset;
    *EventFlag = TRUE;
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_CONN_DONE_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_CONN_DONE_EVENT\n"));
    UsbProcessDeviceResetDone (XdciCorePtr);
    UsbDeviceSetAddress (UsbDeviceCorePtr, 0);
    UsbFuncIoDevPtr->DevReConnect = TRUE;
    UsbFuncIoDevPtr->DevResetFlag = FALSE;
    *EventFlag = TRUE;
    *Message = EfiUsbMsgNone;
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_HBRNTN_REQ_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_HBRNTN_REQ_EVENT\n"));
    *Message = EfiUsbMsgBusEventSuspend;
    *EventFlag = TRUE;
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_WKUP_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_WKUP_EVENT\n"));
    *Message = EfiUsbMsgBusEventResume;
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_TST_LMP_RX_EVENT:
    *ProcessSize = DWC_XDCI_DEV_EVENT_TST_LMP_SIZE_IN_BYTES;
    *Message = EfiUsbMsgNone;
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFUDwcXdciProcessDeviceEvent: UNHANDLED device event: %x\n", EventReg));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_STATE_CHANGE_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_STATE_CHANGE_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_SOF_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_SOF_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_ERRATIC_ERR_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_ERRATIC_ERR_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_CMD_CMPLT_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_CMD_CMPLT_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_BUFF_OVFL_EVENT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_Device DWC_XDCI_EVENT_BUFF_DEV_BUFF_OVFL_EVENT\n"));
    break;

  default:
    *EventFlag = FALSE;
    *Message = EfiUsbMsgNone;
    DEBUG ((USB_FUIO_DEBUG_EVENT_I, "USBFUWcXdciProcessDeviceEvent: UNHANDLED device event: %x\n", EventReg));
    break;
  }

  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "\n FUEV::DeviceEvent entry exit.... \n"));
  return EFI_SUCCESS;
}


EFI_STATUS
Ep0XferDone (
  IN  EFI_USBFN_IO_PROTOCOL       *This,
  IN  UINT32                      EndPointNum,
  OUT EFI_USBFN_MESSAGE           *Message,
  IN OUT UINTN                    *PayloadSize,
  OUT EFI_USBFN_MESSAGE_PAYLOAD   *Payload
  )
{
  USB_DEV_CORE                    *UsbDeviceCorePtr;
  XDCI_CORE_HANDLE                *XdciCorePtr;
  USB_XDCI_DEV_CONTEXT            *UsbFuncIoDevPtr;
  DWC_XDCI_ENDPOINT               *EpHandle;
  DWC_XDCI_TRB                    *Trb;
  UINT32                          TrbCtrl;
  UINT32                          TrbSts;
  UINT32                          BufferLen;
  EFI_STATUS                      DevStatus;
  USB_EP_INFO                     EpInfo;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  EpHandle = &XdciCorePtr->EpHandles[EndPointNum];
  Trb = XdciCorePtr->Trbs + (EndPointNum * DWC_XDCI_TRB_NUM);

  if (Trb->TrbCtrl & DWC_XDCI_TRB_CTRL_HWO_MASK) {
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "Ep0XferDone. HW owns TRB: %x!!!\n", (UINT32) (UINTN)Trb));
  }

  DevStatus = EFI_SUCCESS;
  BufferLen = 0;

  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "EndPointNum:%d, TRB: Addr 0x%08x!!!\n", EndPointNum, (UINTN)Trb));
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "Ep0 done Trb->TrbCtrl: %x!!!\n", (UINT32)Trb->TrbCtrl));
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "Ep0 done Trb->LenXferParams: %x!!!\n", (UINT32)Trb->LenXferParams));
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "Ep0 done Trb->BuffPtrLow: %x!!!\n", (UINT32)Trb->BuffPtrLow));
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "Ep0 done Trb->BuffPtrHigh: %x!!!\n", (UINT32)Trb->BuffPtrHigh));

  //
  // Set CheckFlag to FALSE for 'DwcXdciEpRxData' function
  // check the RX request complete and continue next transfer request
  //
  EpHandle->CheckFlag = FALSE;
  EpHandle->CurrentXferRscIdx = 0;

  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "Ep0 done D01!!\n"));
  TrbCtrl = (Trb->TrbCtrl & DWC_XDCI_TRB_CTRL_TYPE_MASK) >> DWC_XDCI_TRB_CTRL_TYPE_BIT_POS;

  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "Ep0 done D02!!\n"));
  TrbSts = (Trb->LenXferParams & DWC_XDCI_TRB_STATUS_MASK) >> DWC_XDCI_TRB_STATUS_BIT_POS;

  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "Ep0 done D03!!\n" ));
  BufferLen = Trb->LenXferParams & DWC_XDCI_TRB_BUFF_SIZE_MASK;

  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "Ep0 done D04 TrbCtrl :: %x!!\n", TrbCtrl));
  switch (TrbCtrl) {
  case DWC_XDCI_TRB_CTRL_TYPE_SETUP:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "Ep0 done DWC_XDCI_TRB_CTRL_TYPE_SETUP!!\n"));
    //
    // This is delay for other host USB controller(none Intel), identify device get fail issue.
    //
    gBS->Stall (130);
    BufferLen = 8;

    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "DWC_XDCI_TRB_CTRL_TYPE_SETUP!!\n"));
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "AlignedSetupBuffer::0x%08x!!\n", XdciCorePtr->AlignedSetupBuffer));
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "Payload::0x%08x!!\n", (UINTN)Payload));
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "BufferLen::0x%08x!!\n", (UINTN)BufferLen));
    *Message = EfiUsbMsgSetupPacket;
    CopyMem (Payload, XdciCorePtr->AlignedSetupBuffer, BufferLen);

    DEBUG ((USB_FUIO_DEBUG_EVENT_I, "Ep0 done D06!!\n"));
    if (! (XdciCorePtr->AlignedSetupBuffer[0] & USB_SETUP_DATA_PHASE_DIRECTION_MASK)) {
      if ((XdciCorePtr->AlignedSetupBuffer[0]  == 0x00) ) {
        if ((XdciCorePtr->AlignedSetupBuffer[1]  == USB_DEV_SET_ADDRESS)) {
          //
          // set address
          //
          UsbSetAddress (
            This,
            (UINT32) (XdciCorePtr->AlignedSetupBuffer[3] << 8 | XdciCorePtr->AlignedSetupBuffer[2])
            );

          *Message = EfiUsbMsgNone;
        } else if ((XdciCorePtr->AlignedSetupBuffer[1]  == USB_DEV_SET_CONFIGURATION)) {
          DEBUG ((USB_FUIO_DEBUG_EVENT_I, "\n set configure !!!"));
          UsbSetconfigure (
            This,
            (UINT32) (XdciCorePtr->AlignedSetupBuffer[3] << 8 | XdciCorePtr->AlignedSetupBuffer[2])
            );
          *Message = EfiUsbMsgNone;
        }
      }
    }

    DEBUG ((USB_FUIO_DEBUG_EVENT_I, "Ep0 done D07!!\n"));
    break;

  case DWC_XDCI_TRB_CTRL_TYPE_DATA:
    DEBUG ((DEBUG_INFO, "Ep0 done DWC_XDCI_TRB_CTRL_TYPE_DATA!!\n"));
    //
    // Notify upper layer of control transfer completion
    // if a callback function was registerd
    //
    if ((EndPointNum & 0x01) == 0) {
      *Message = EfiUsbMsgEndpointStatusChangedRx;
    } else {
      *Message = EfiUsbMsgEndpointStatusChangedTx;
    }
    Payload->utr.EndpointIndex = (UINT8) (EndPointNum >> 1);
    Payload->utr.Direction = (UINT8) (EndPointNum & 0x01);
    Payload->utr.Buffer = (VOID *) (UINTN) (Trb->BuffPtrLow);

    DEBUG ((DEBUG_INFO, "Ep0 EndPointNum: %x!!!\n", (UINT32)EndPointNum));
    DEBUG ((DEBUG_INFO, "Ep0 done XferLength: %x!!!\n", (UINT32)UsbFuncIoDevPtr->EndPointXferRec[EndPointNum].XferLength));
    Payload->utr.Buffer = (VOID *)UsbFuncIoDevPtr->EndPointXferRec[EndPointNum].XferAddress;
    Payload->utr.BytesTransferred = UsbFuncIoDevPtr->EndPointXferRec[EndPointNum].XferLength;

    if (TrbSts == 0) {
      if ((Trb->LenXferParams & DWC_XDCI_TRB_BUFF_SIZE_MASK) == 0) {
        Payload->utr.TransferStatus = UsbTransferStatusComplete;
      } else {
        Payload->utr.TransferStatus = UsbTransferStatusActive;
      }
    } else if (TrbSts != 0) {
      Trb->TrbCtrl |= DWC_XDCI_TRB_CTRL_HWO_MASK;
      *Message = EfiUsbMsgNone;
      Payload->utr.TransferStatus = UsbTransferStatusAborted;
      DEBUG ((DEBUG_INFO, "Flush FIFO!!!\n" ));
      EpInfo.EpNum = 0;
      EpInfo.EpDir = UsbEpDirIn;
      UsbXdciCoreFlushEpFifo (XdciCorePtr, &EpInfo);
      EpInfo.EpNum = 0;
      EpInfo.EpDir = UsbEpDirOut;
      UsbXdciCoreFlushEpFifo (XdciCorePtr, &EpInfo);
      DevStatus = UsbDeviceEp0RxSetup (UsbDeviceCorePtr, XdciCorePtr->AlignedSetupBuffer);
    }

    break;

  case DWC_XDCI_TRB_CTRL_TYPE_STATUS2:
  case DWC_XDCI_TRB_CTRL_TYPE_STATUS3:
    Payload->utr.Buffer = (VOID *) UsbFuncIoDevPtr->EndPointXferRec[EndPointNum].XferAddress;
    Payload->utr.BytesTransferred = 0;
    Payload->utr.EndpointIndex = (UINT8) (EndPointNum >> 1);
    if ((EndPointNum & 0x01) == 0) {
      *Message = EfiUsbMsgEndpointStatusChangedRx;
    } else {
      *Message = EfiUsbMsgEndpointStatusChangedTx;
    }

    if (TrbSts == 0) {
      if ((Trb->LenXferParams & DWC_XDCI_TRB_BUFF_SIZE_MASK) == 0) {
        Payload->utr.TransferStatus = UsbTransferStatusComplete;
      } else {
        Payload->utr.TransferStatus = UsbTransferStatusActive;
      }
    } else if (TrbSts != 0) {
      Payload->utr.TransferStatus = UsbTransferStatusAborted;
    }

    DevStatus = UsbDeviceEp0RxSetup (UsbDeviceCorePtr, XdciCorePtr->AlignedSetupBuffer);

    if (DevStatus != EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "DwcXdciProcessEp0XferPhaseDone: FAILED to queue SETUP\n"));
    }
    DEBUG ((DEBUG_INFO, "Status phase done. Queue next SETUP packet==>\n"));
    break;

  default:
    *Message = EfiUsbMsgNone;
    DEBUG ((DEBUG_INFO, "DwcXdciProcessEp0XferPhaseDone: UNHANDLED STATE in TRB\n"));
    break;
  }
  return EFI_SUCCESS;
}


EFI_STATUS
NoneEp0XferDone (
  IN  EFI_USBFN_IO_PROTOCOL       *This,
  IN  UINT32                      EndPointNum,
  OUT EFI_USBFN_MESSAGE           *Message,
  IN OUT UINTN                    *PayloadSize,
  OUT EFI_USBFN_MESSAGE_PAYLOAD   *Payload
  )
{
  USB_DEV_CORE                    *UsbDeviceCorePtr;
  XDCI_CORE_HANDLE                *XdciCorePtr;
  USB_XDCI_DEV_CONTEXT            *UsbFuncIoDevPtr;
  DWC_XDCI_ENDPOINT               *EpHandle;
  DWC_XDCI_TRB                    *Trb;
  UINT32                          TrbCtrl;
  UINT32                          TrbSts;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  EpHandle = &XdciCorePtr->EpHandles[EndPointNum];
  Trb = XdciCorePtr->Trbs + (EndPointNum * DWC_XDCI_TRB_NUM);

  if (Trb->TrbCtrl & DWC_XDCI_TRB_CTRL_HWO_MASK) {
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "NoneEp0XferDone. HW owns TRB: %x!!!, EndPointNum: %x\n", (UINT32) (UINTN)Trb,
            EndPointNum));
  }

  DEBUG ((USB_FUIO_DEBUG_EVENT_D, " TRB: Addr 0x%08x!!!\n", (UINTN)Trb));
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, " Xfer done Trb->BuffPtrLow: %x!!!\n", (UINT32)Trb->BuffPtrLow));
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, " Xfer done Trb->BuffPtrHigh: %x!!!\n", (UINT32)Trb->BuffPtrHigh));
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, " Xfer done Trb->LenXferParams: %x!!!\n", (UINT32)Trb->LenXferParams));
  DEBUG ((USB_FUIO_DEBUG_EVENT_D, " Xfer done Trb->TrbCtrl: %x!!!\n", (UINT32)Trb->TrbCtrl));

  //
  // Set CheckFlag to FALSE for 'DwcXdciEpRxData' function
  // check the RX request complete and continue next transfer request
  //
  EpHandle->CheckFlag = FALSE;
  EpHandle->CurrentXferRscIdx = 0;
  *Message = EfiUsbMsgNone;

  TrbCtrl = (Trb->TrbCtrl & DWC_XDCI_TRB_CTRL_TYPE_MASK) >> DWC_XDCI_TRB_CTRL_TYPE_BIT_POS;
  TrbSts = (Trb->LenXferParams & DWC_XDCI_TRB_STATUS_MASK) >> DWC_XDCI_TRB_STATUS_BIT_POS;

  Payload->utr.BytesTransferred = UsbFuncIoDevPtr->EndPointXferRec[EndPointNum].XferLength;
  Payload->utr.EndpointIndex = UsbFuncIoDevPtr->EndPointXferRec[EndPointNum].LogEpNum;
  Payload->utr.Direction = UsbFuncIoDevPtr->EndPointXferRec[EndPointNum].Direction;
  Payload->utr.Buffer = (VOID *) (UINTN) (Trb->BuffPtrLow);
  UsbFuncIoDevPtr->EndPointXferRec[EndPointNum].Complete = TRUE;

  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "EndPointAddress = 0x%08x\n", Payload->utr.EndpointIndex));
  if (Payload->utr.Direction == EfiUsbEndpointDirectionDeviceTx) {
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, " Direction::EfiUsbEndpointDirectionDeviceTx\n"));
    *Message = EfiUsbMsgEndpointStatusChangedTx;
  } else {
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, " Direction::EfiUsbEndpointDirectionDeviceRx\n"));
    *Message = EfiUsbMsgEndpointStatusChangedRx;
  }

  if (TrbSts == 0) {
    if ((Trb->LenXferParams & DWC_XDCI_TRB_BUFF_SIZE_MASK) == 0) {
      Payload->utr.TransferStatus = UsbTransferStatusComplete;
      DEBUG ((USB_FUIO_DEBUG_EVENT_D, "XferStatus::UsbTransferStatusComplete\n"));
    } else {
      Payload->utr.TransferStatus = UsbTransferStatusComplete;
      Payload->utr.BytesTransferred -= (Trb->LenXferParams & DWC_XDCI_TRB_BUFF_SIZE_MASK);
      DEBUG ((USB_FUIO_DEBUG_EVENT_D, "XferStatus::UsbTransferStatusComplete\n"));
      DEBUG ((USB_FUIO_DEBUG_EVENT_D, "XferStatus::Length %d \n", Payload->utr.BytesTransferred ));
    }
  } else if (TrbSts != 0) {
    Payload->utr.TransferStatus = UsbTransferStatusAborted;
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "XferStatus::UsbTransferStatusAborted\n"));
  }

  return EFI_SUCCESS;
}

EFI_STATUS
Ep0XferNotReady (
  IN  EFI_USBFN_IO_PROTOCOL       *This,
  IN  UINT32                      EndPointNum,
  OUT EFI_USBFN_MESSAGE           *Message,
  IN OUT UINTN                    *PayloadSize,
  OUT EFI_USBFN_MESSAGE_PAYLOAD   *Payload,
  IN  UINT32                      EpStatus
  )
{
  USB_DEV_CORE                    *UsbDeviceCorePtr;
  XDCI_CORE_HANDLE                *XdciCorePtr;
  USB_XDCI_DEV_CONTEXT            *UsbFuncIoDevPtr;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;

  *Message = EfiUsbMsgNone;

  return EFI_SUCCESS;
}


EFI_STATUS
EpEventCheck (
  IN  EFI_USBFN_IO_PROTOCOL       *This,
  IN  USBD_EVENT_BUF              *EventIndex,
  OUT UINT32                      *ProcessSize,
  OUT EFI_USBFN_MESSAGE           *Message,
  IN OUT UINTN                    *PayloadSize,
  OUT EFI_USBFN_MESSAGE_PAYLOAD   *Payload,
  OUT BOOLEAN                     *EventFlag
  )
{
  USB_DEV_CORE                    *UsbDeviceCorePtr;
  XDCI_CORE_HANDLE                *XdciCorePtr;
  USB_XDCI_DEV_CONTEXT            *UsbFuncIoDevPtr;
  UINT32                          EventReg;
  UINT32                          EpEvent;
  UINT32                          EndPointNumber;
  UINT32                          EventStatus;
  USB_EP_STATE                    Ep_State;
  UINTN                           TmpBufferSize;

  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "FUEV::EndPoint Event....\n"));
  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  EventReg = EventIndex->Event;
  *ProcessSize = DWC_XDCI_DEV_EVENT_DEFAULT_SIZE_IN_BYTES;
  *EventFlag = TRUE;
  TmpBufferSize = 0;

  //
  // Get EP num
  //
  EndPointNumber = (EventReg & DWC_XDCI_EVENT_BUFF_EP_NUM_MASK) >> DWC_XDCI_EVENT_BUFF_EP_NUM_BIT_POS;

  EventStatus = EventReg & DWC_XDCI_EVENT_BUFF_EP_EVENT_STATUS_MASK;

  //
  // Interpret event and handle transfer completion here
  //
  EpEvent = (EventReg & DWC_XDCI_EVENT_BUFF_EP_EVENT_MASK) >> DWC_XDCI_EVENT_BUFF_EP_EVENT_BIT_POS;

  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "USBFU_EP EventReg 0x%08x\n", EventReg));

  switch (EpEvent) {
  case DWC_XDCI_EVENT_BUFF_EP_XFER_CMPLT:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_EP DWC_XDCI_EVENT_BUFF_EP_XFER_CMPLT\n"));
    if (EndPointNumber > 1) {
      DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_EP None_Control transfer\n"));
      NoneEp0XferDone (This, EndPointNumber, Message, PayloadSize, Payload);
    } else {
      //
      // Control transfer
      //
      DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_EP Control transfer\n"));
      Ep0XferDone (This, EndPointNumber, Message, PayloadSize, Payload);
    }
    break;

  case DWC_XDCI_EVENT_BUFF_EP_XFER_NOT_READY:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "DWC_XDCI_EVENT_BUFF_EP_XFER_NOT_READY\n"));
    *Message = EfiUsbMsgNone;
    if (EndPointNumber < (sizeof (UsbFuncIoDevPtr->EndPointXferRec) / sizeof (UsbFuncIoDevPtr->EndPointXferRec[0]))) {
      if ((UsbFuncIoDevPtr->EndPointXferRec[EndPointNumber].ZlpFlag == TRUE) && \
          (UsbFuncIoDevPtr->EndPointXferRec[EndPointNumber].Complete == TRUE)) {
        DEBUG ((USB_FUIO_DEBUG_EVENT_D, "Request send ZLP\n"));
        if ((EndPointNumber & 0x01) != 0) {
          Transfer (This,
                    UsbFuncIoDevPtr->IndexPtrInEp.EndpointDesc->EndpointAddress,
                    EfiUsbEndpointDirectionDeviceTx,
                    &TmpBufferSize,
                    NULL
                    );
          UsbFuncIoDevPtr->EndPointXferRec[EndPointNumber].ZlpFlag = FALSE;
        }

      }
    } else {
      //
      // Is it data stage or status stage
      //
      // Data Statge
      //
      Ep_State = USB_EP_STATE_DATA;
      //
      // Control transfer
      //
      DEBUG ((USB_FUIO_DEBUG_EVENT_D, "USBFU_EP Control transfer not ready\n"));
      Ep0XferNotReady (This, EndPointNumber, Message, PayloadSize, Payload, EventStatus);
      *EventFlag = FALSE;
    }
    break;

  case DWC_XDCI_EVENT_BUFF_EP_XFER_IN_PROGRESS:
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "DWC_XDCI_EVENT_BUFF_EP_XFER_IN_PROGRESS\n"));
    break;

  default:
    DEBUG ((USB_FUIO_DEBUG_EVENT_I, "USBFUDwcXdciProcessEpEvent: UNKNOWN EP event\n"));
    break;
  }

  DEBUG ((USB_FUIO_DEBUG_EVENT_D, "FUEV::EndPoint Event....exit\n"));
  return EFI_SUCCESS;
}


EFI_STATUS
ProcessIntLineEvents (
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT32                       EventCount,
  IN UINT32                       *ProceSsEvent,
  OUT EFI_USBFN_MESSAGE           *Message,
  IN OUT UINTN                    *PayloadSize,
  OUT EFI_USBFN_MESSAGE_PAYLOAD   *Payload,
  OUT BOOLEAN                     *EventFlag
  )
{
  USB_DEV_CORE                    *UsbDeviceCorePtr;
  XDCI_CORE_HANDLE                *XdciCorePtr;
  USB_XDCI_DEV_CONTEXT            *UsbFuncIoDevPtr;
  UINT32                          CurrentEventAddr;
  UINT32                          ProceSsEventSize;
  BOOLEAN                         EventReport;
  BOOLEAN                         EpEventReport;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  CurrentEventAddr = (UINT32) (UINTN) (XdciCorePtr->CurrentEventBuffer);
  EventReport = FALSE;
  EpEventReport = FALSE;
  ProceSsEventSize = 0;
  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "FUEV:: ProcessIntLineEvents Entry\n"));

  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "FUEV:: XdciCorePtr->CurrentEventBuffer 0x%08x\n", XdciCorePtr->CurrentEventBuffer));
  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "FUEV::EventCount0x%08x\n", EventCount));
  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "FUEV::CurrentEventAddr 0x%08x\n", CurrentEventAddr));

  while ((EventCount != 0) && (!EventReport)) {
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "FUEV::event0x%08x\n", XdciCorePtr->CurrentEventBuffer->Event));
    if ((XdciCorePtr->CurrentEventBuffer->Event & DWC_XDCI_EVENT_DEV_MASK) != 0) {
      //
      // Device event
      //
      DeviceEventCheck (
        This,
        (USBD_EVENT_BUF *) (UINTN)CurrentEventAddr,
        &ProceSsEventSize,
        Message,
        &EventReport
        );
      if (EventReport) {
        *EventFlag = TRUE;
      }

    } else {
      //
      // EndPoint Event
      //
      EpEventCheck (
        This,
        (USBD_EVENT_BUF *) (UINTN)CurrentEventAddr,
        &ProceSsEventSize,
        Message,
        PayloadSize,
        Payload,
        &EpEventReport
        );
    }

    if ((*Message != EfiUsbMsgNone) || (EpEventReport)) {
      EventReport = TRUE;
      *EventFlag = TRUE;
    }

    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "FUEV:: CurrentEventAddr 0x%08x :: ProceSsEventSize 0x%08x\n", (UINTN)CurrentEventAddr,
            ProceSsEventSize));

    EventCount -= ProceSsEventSize;
    *ProceSsEvent += ProceSsEventSize;
    if ((CurrentEventAddr + ProceSsEventSize) >= \
        ((UINT32) (UINTN) (XdciCorePtr->AlignedEventBuffers) +
         (sizeof (DWC_XDCI_EVENT_BUFFER) * DWC_XDCI_MAX_EVENTS_PER_BUFFER))) {
      CurrentEventAddr = (UINT32) (UINTN) (XdciCorePtr->AlignedEventBuffers);
    } else {
      CurrentEventAddr += ProceSsEventSize;
    }
    DEBUG ((USB_FUIO_DEBUG_EVENT_D, "FUEV:: CurrentEventAddr Update 0x%08x :: ProceSsEventSize 0x%08x\n", CurrentEventAddr,
            ProceSsEventSize));

    XdciCorePtr->CurrentEventBuffer = (DWC_XDCI_EVENT_BUFFER *) (UINTN)CurrentEventAddr;
  }

  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "FUEV:: ProcessIntLineEvents Exit\n\n"));
  return EFI_SUCCESS;
}


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
  IN EFI_USBFN_IO_PROTOCOL        *This,
  OUT EFI_USBFN_MESSAGE           *Message,
  IN OUT UINTN                    *PayloadSize,
  OUT EFI_USBFN_MESSAGE_PAYLOAD   *Payload
  )
{
  UINT32                          EventCount;
  UINT32                          PeventCount;
  USB_XDCI_DEV_CONTEXT            *UsbFuncIoDevPtr;
  UINT32                          MaxIntNum;
  UINT32                          IntIndex;
  USB_DEV_CORE                    *UsbDeviceCorePtr;
  XDCI_CORE_HANDLE                *XdciCorePtr;
  BOOLEAN                         EventFlag;
  EFI_TPL                         OriginalTpl;


  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "USBFU_ EventHandler Entry\n"));
  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  if (UsbFuncIoDevPtr->StartUpController == FALSE) {
    UsbFnInitDevice (This);
  }
  OriginalTpl = BS_RAISE_TPL (TPL_HIGH_LEVEL);
  *Message = EfiUsbMsgNone;
  MaxIntNum = (UsbRegRead ((UINT32)UsbFuncIoDevPtr->XdciMmioBarAddr, DWC_XDCI_GHWPARAMS1_REG) &
               DWC_XDCI_GHWPARAMS1_NUM_INT_MASK) >>
              DWC_XDCI_GHWPARAMS1_NUM_INT_BIT_POS;

  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  EventFlag = TRUE;

  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "XdciCorePtr->MaxDevIntLines 0x%08x\n", XdciCorePtr->MaxDevIntLines));
  EventCount = UsbRegRead ((UINT32)UsbFuncIoDevPtr->XdciMmioBarAddr, DWC_XDCI_EVNTCOUNT_REG (0));

  for (IntIndex = 0; IntIndex < XdciCorePtr->MaxDevIntLines ; IntIndex++) {
    //
    // Get the number of events HW has written for this
    //  interrupt line
    //
    EventCount = UsbRegRead ((UINT32)UsbFuncIoDevPtr->XdciMmioBarAddr, DWC_XDCI_EVNTCOUNT_REG (IntIndex));
    EventCount &= DWC_XDCI_EVNTCOUNT_MASK;
    PeventCount = 0;

    //
    // Process interrupt line Buffer only if count is non-zero
    //
    if (EventCount != 0) {
      //
      // Process events in this Buffer
      //
      ProcessIntLineEvents (
        This,
        EventCount,
        &PeventCount,
        Message,
        PayloadSize,
        Payload,
        &EventFlag
        );

      //
      // Write back the Processed number of events so HW decrements it from current
      // event count
      //
      UsbRegWrite ((UINT32)UsbFuncIoDevPtr->XdciMmioBarAddr, DWC_XDCI_EVNTCOUNT_REG (IntIndex), PeventCount);

      //
      // for debug
      //
      if (*Message != EfiUsbMsgNone) {
        break;
      }

      if (EventFlag) {
        break;
      }
    }
  }

  BS_RESTORE_TPL (OriginalTpl);

  //EVENT_EXIT:
  DEBUG ((USB_FUIO_DEBUG_EVENT_I, "USBFU_ EventHandler Exit\n"));
  return EFI_SUCCESS;
}



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
  )
{
  EFI_USB_ENDPOINT_DESCRIPTOR              *EpDesc;
  EFI_USB_ENDPOINT_COMPANION_DESCRIPTOR    *EpCompDesc;

  EpDesc = NULL;
  EpCompDesc = NULL;

  //
  // start by clearing all data in the destination
  //
  SetMem (EpDest, sizeof (USB_EP_INFO), 0);
  EpDesc = EpSrc->EndpointDesc;
  EpCompDesc = EpSrc->EndpointCompDesc;

  if (EpDesc != NULL) {
    EpDest->EpNum = EpDesc->EndpointAddress & 0x0F; // Bits 0-3 are ep num
    EpDest->EpDir = ((EpDesc->EndpointAddress & USB_ENDPOINT_DIR_IN) > 0) ? UsbEpDirIn : UsbEpDirOut;
    DEBUG ((DEBUG_INFO, "EpDest->EpNum 0x%02x\n", EpDest->EpNum));
    DEBUG ((DEBUG_INFO, "EpDest->EpDir 0x%02x\n", EpDest->EpDir));
    EpDest->EpType = EpDesc->Attributes & USB_ENDPOINT_TYPE_MASK;
    EpDest->MaxPktSize = EpDesc->MaxPacketSize;
    EpDest->Interval = EpDesc->Interval;
  }
  if (EpCompDesc != NULL) {
    EpDest->MaxStreams = EpCompDesc->Attributes & USB_EP_BULK_BM_ATTR_MASK;
    EpDest->BurstSize = EpCompDesc->MaxBurst;
    EpDest->Mult = EpCompDesc->BytesPerInterval;
  }

  return;
}


EFI_STATUS
SetFnIoReqInfo (
  IN     EFI_USBFN_IO_PROTOCOL         *This,
  IN     UINT8                         EndpointIndex,
  IN     EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN OUT UINTN                         *BufferSize,
  IN OUT VOID                          *Buffer,
  IN OUT USB_XFER_REQUEST              *XfIoreq
  )
{
  USB_XDCI_DEV_CONTEXT     *UsbFuncIoDevPtr;
  EFI_STATUS               Status;
  UINTN                    ReqPacket;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  Status = EFI_SUCCESS;
  ReqPacket = 0;

  switch (EndpointIndex) {
  case 0: // Control endpoint
    XfIoreq->EpInfo.EpNum = 0;
    XfIoreq->EpInfo.EpDir = (Direction != 0 ) ? UsbEpDirIn : UsbEpDirOut;
    break;


  default:
    if (Direction == EfiUsbEndpointDirectionDeviceTx) {
      UsbFnSetEpInfo (&XfIoreq->EpInfo, &UsbFuncIoDevPtr->IndexPtrInEp);
    } else {
      UsbFnSetEpInfo (&XfIoreq->EpInfo, &UsbFuncIoDevPtr->IndexPtrOutEp);
      //
      // reference from "UsbDeviceMode.c", function UsbdEpRxData
      //

      //
      // Transfer length should be multiple of USB packet size.
      //
      ReqPacket = *BufferSize / XfIoreq->EpInfo.MaxPktSize;
      ReqPacket = ((XfIoreq->XferLen % XfIoreq->EpInfo.MaxPktSize) == 0) ? ReqPacket : ReqPacket + 1;
      XfIoreq->XferLen = (UINT32)ReqPacket * XfIoreq->EpInfo.MaxPktSize;

    }
    break;
  }

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  XfIoreq->XferBuffer = Buffer;
  XfIoreq->XferLen = (UINT32) (*BufferSize);
  XfIoreq->XferDone = NULL;

  return EFI_SUCCESS;
}


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
  )
{
  USB_XDCI_DEV_CONTEXT              *UsbFuncIoDevPtr;
  USB_DEV_CORE                      *UsbDeviceCorePtr;
  XDCI_CORE_HANDLE                  *XdciCorePtr;
  EFI_STATUS                        Status;
  USB_XFER_REQUEST                  XferReq;
  UINT32                            EndPoint;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "\n FU:Transfer - Entry\n"));
  DEBUG ((USB_FUIO_DEBUG_LOAD, "\n FU:EndpointIndex 0x%02x\n", EndpointIndex));
  DEBUG ((USB_FUIO_DEBUG_LOAD, "\n FU:Direction 0x%02x\n", Direction));

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  UsbDeviceCorePtr = UsbFuncIoDevPtr->DrvCore;
  XdciCorePtr = UsbDeviceCorePtr->ControllerHandle;
  EndPoint = UsbGetPhysicalEpNum (EndpointIndex, (Direction != 0 ) ? UsbEpDirIn : UsbEpDirOut);

  Status = SetFnIoReqInfo (
             This,
             EndpointIndex,
             Direction,
             BufferSize,
             Buffer,
             &XferReq
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((USB_FUIO_DEBUG_LOAD, "Set SetFnIoReqInfo - Error Stop!!!\n"));
    while (1);
    Status = EFI_DEVICE_ERROR;
    goto FUN_EXIT;
  }

  UsbFuncIoDevPtr->EndPointXferRec[EndPoint].EpNum = EndPoint;
  UsbFuncIoDevPtr->EndPointXferRec[EndPoint].Direction = Direction;
  UsbFuncIoDevPtr->EndPointXferRec[EndPoint].XferAddress = (UINTN)Buffer;
  UsbFuncIoDevPtr->EndPointXferRec[EndPoint].XferLength = (UINT32) (*BufferSize);
  UsbFuncIoDevPtr->EndPointXferRec[EndPoint].LogEpNum = EndpointIndex;
  UsbFuncIoDevPtr->EndPointXferRec[EndPoint].Complete = FALSE;
  UsbFuncIoDevPtr->EndPointXferRec[EndPoint].ZlpFlag = FALSE;

  Status = EFI_DEVICE_ERROR;
  switch (EndpointIndex) {
  case 0: // Control endpoint
    if (*BufferSize == 0) {
      if (Direction == EfiUsbEndpointDirectionDeviceTx) {
        Status = UsbDeviceEp0TxStatus (UsbDeviceCorePtr);
      } else {
        Status = UsbDeviceEp0RxStatus (UsbDeviceCorePtr);
      }
    } else if (Direction == EfiUsbEndpointDirectionDeviceTx) {
      Status = UsbXdciDeviceEpTxData (UsbDeviceCorePtr, &XferReq);
    } else if (Direction == EfiUsbEndpointDirectionDeviceRx) {
      DEBUG ((USB_FUIO_DEBUG_LOAD, "\n Set Setup Package - ??? Stop!!!\n"));
    }
    break;

  default:
    Status = EFI_SUCCESS;
    if (Direction == EfiUsbEndpointDirectionDeviceTx) {
      DEBUG ((USB_FUIO_DEBUG_LOAD, "\n EfiUsbEndpointDirectionDeviceTx Size = %d\n", (*BufferSize) ));
      XferReq.Zlp = TRUE;
      if ((((*BufferSize) % 512) == 0) && ((*BufferSize) != 0)) {
        UsbFuncIoDevPtr->EndPointXferRec[EndPoint].ZlpFlag = TRUE;
        DEBUG ((USB_FUIO_DEBUG_LOAD, "\n Set Zlp flag\n"));
      }
      Status = UsbXdciDeviceEpTxData (UsbDeviceCorePtr, &XferReq);
    } else {
      DEBUG ((USB_FUIO_DEBUG_LOAD, "\n EfiUsbEndpointDirectionDeviceRx Size = %d\n", (*BufferSize) ));
      Status = UsbXdciDeviceEpRxData (UsbDeviceCorePtr, &XferReq);
    }
    break;
  }

  if (EFI_ERROR (Status)) {
    goto FUN_EXIT;
  }

  if (Status != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
  }

FUN_EXIT:

  DEBUG ((USB_FUIO_DEBUG_LOAD, "FU:Transfer - Exit %r\n", Status));
  return Status;
}


/**
  This function supplies power to the USB controller if needed, initialize
  hardware and internal data structures, and then return.
  The port must not be activated by this function.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
**/
EFI_STATUS
EFIAPI
StartXdciController (
  IN EFI_USBFN_IO_PROTOCOL    *This
  )
{
  USB_XDCI_DEV_CONTEXT         *UsbFuncIoDevPtr;
  USB_DEV_CONFIG_PARAMS        ConfigParams;
  EFI_STATUS                   Status;

  Status = EFI_SUCCESS;
  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  if (UsbFuncIoDevPtr->StartUpController == TRUE) {
    goto EXIT_START_CONTROLLER;
  }

  ConfigParams.ControllerId = USB_ID_DWC_XDCI;
  ConfigParams.BaseAddress = (UINT32)UsbFuncIoDevPtr->XdciMmioBarAddr;
  ConfigParams.Role = USB_ROLE_DEVICE;
  ConfigParams.Speed = USB_SPEED_HIGH;

  //
  //*Vid = 0x8086
  //*Pid = 0x0A65
  //
  UsbFuncIoDevPtr->VendorId = USBFU_VID;
  UsbFuncIoDevPtr->DeviceId = USBFU_PID;
  UsbFuncIoDevPtr->StartUpController = TRUE;

  Status = UsbDeviceInit (&ConfigParams, (VOID **)&UsbFuncIoDevPtr->DrvCore);
  if (Status != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
    goto EXIT_START_CONTROLLER;
  }

  UsbFuncIoDevPtr->XdciDrvIfHandle = UsbFuncIoDevPtr->DrvCore->ControllerHandle;

EXIT_START_CONTROLLER:

  DEBUG ((USB_FUIO_DEBUG_LOAD, "StartXdciController - Exit :: %r\n", Status));
  return Status;
}


/**
  This function disables the hardware device by resetting the run/stop bit
  and power off the USB controller if needed.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.

  @retval EFI_SUCCESS           The function returned successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_DEVICE_ERROR      The physical device reported an error.
**/
EFI_STATUS
EFIAPI
StopXdciController (
  IN EFI_USBFN_IO_PROTOCOL    *This
  )
{
  USB_XDCI_DEV_CONTEXT  *UsbFuncIoDevPtr;
  EFI_STATUS            DevStatus;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  DEBUG ((USB_FUIO_DEBUG_LOAD, "StopController - Entry\n"));

  if (UsbFuncIoDevPtr->StartUpController == FALSE) {
    DEBUG ((USB_FUIO_DEBUG_LOAD, "The Controller not yet start up skip deinit\n"));
    return EFI_SUCCESS;
  }

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DevStatus = UsbDeviceDeinit (UsbFuncIoDevPtr->DrvCore, TRUE);

  UsbFuncIoDevPtr->DrvCore = NULL;
  UsbFuncIoDevPtr->XdciDrvIfHandle = NULL;
  UsbFuncIoDevPtr->StartUpController = FALSE;

  if (DevStatus != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((USB_FUIO_DEBUG_LOAD, "StopController - Exit\n"));
  return EFI_SUCCESS;
}


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
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
  IN EFI_USBFN_POLICY_TYPE        PolicyType,
  IN UINTN                        BufferSize,
  IN VOID                         *Buffer
  )
{
  USB_XDCI_DEV_CONTEXT  *UsbFuncIoDevPtr;
  EFI_STATUS            Status;
  UINT32                EndPoint;
  UINT8                 *FlagPtr;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  FlagPtr = NULL;

  switch (PolicyType) {
  case EfiUsbPolicyUndefined:
  case EfiUsbPolicyMaxTransactionSize:
  case EfiUsbPolicyZeroLengthTerminationSupport:

    Status = EFI_UNSUPPORTED;
    break;

  default:
    FlagPtr = Buffer;
    Status = EFI_SUCCESS;
    break;
  }

  if (BufferSize < 1) {
    Status = EFI_INVALID_PARAMETER;
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((USB_FUIO_DEBUG_LOAD, "SetEndpointPolicy - ERROR %r\n", Status));
    return Status;
  }

  EndPoint = UsbGetPhysicalEpNum (EndpointIndex, (Direction != 0 ) ? UsbEpDirIn : UsbEpDirOut);

  UsbFuncIoDevPtr->EndPointXferRec[EndPoint].ZlpFlag = *FlagPtr;

  return Status;
}


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
  IN EFI_USBFN_IO_PROTOCOL        *This,
  IN UINT8                        EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
  IN EFI_USBFN_POLICY_TYPE        PolicyType,
  IN OUT UINTN                    *BufferSize,
  IN OUT VOID                     *Buffer
  )
{
  USB_XDCI_DEV_CONTEXT  *UsbFuncIoDevPtr;
  EFI_STATUS            Status;
  UINT32                EndPoint;
  UINT32                MaxPacketSize;
  BOOLEAN               SetFlag;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  MaxPacketSize = 0;
  SetFlag = FALSE;

  switch (PolicyType) {
  case EfiUsbPolicyUndefined:

    Status = EFI_UNSUPPORTED;
    break;

  case EfiUsbPolicyMaxTransactionSize:
  case EfiUsbPolicyZeroLengthTerminationSupport:
  default:
    if (Buffer == NULL) {
      Status = EFI_INVALID_PARAMETER;
    } else {
      Status = EFI_SUCCESS;
    }
    break;
  }

  EndPoint = UsbGetPhysicalEpNum (EndpointIndex, (Direction != 0 ) ? UsbEpDirIn : UsbEpDirOut);

  if (EFI_ERROR (Status)) {
    DEBUG ((USB_FUIO_DEBUG_LOAD, "GetEndpointPolicy - ERROR %r\n", Status));
    return Status;
  }

  if (PolicyType == EfiUsbPolicyMaxTransactionSize) {

    if (*BufferSize < sizeof (UINT32)) {
      Status = EFI_INVALID_PARAMETER;
    } else {
      MaxPacketSize = MAX_TRANSFER_PACKET;
      CopyMem (Buffer, &MaxPacketSize, sizeof (UINT32));
    }

  } else if (PolicyType == EfiUsbPolicyZeroLengthTerminationSupport) {
    if (*BufferSize < sizeof (BOOLEAN)) {
      Status = EFI_INVALID_PARAMETER;
    } else {
      SetFlag = TRUE;
      CopyMem (Buffer, &SetFlag, sizeof (BOOLEAN));
    }

  } else if (PolicyType == EfiUsbPolicyZeroLengthTermination) {
    if (*BufferSize < sizeof (BOOLEAN)) {
      Status = EFI_INVALID_PARAMETER;
    } else {
      SetFlag =  UsbFuncIoDevPtr->EndPointXferRec[EndPoint].ZlpFlag;
      CopyMem (Buffer, &SetFlag, sizeof (BOOLEAN));
    }
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  return Status;
}

EFI_STATUS
UsbFnInitDevice (
  IN EFI_USBFN_IO_PROTOCOL        *This
  )
{
  EFI_STATUS                   Status;
  USB_XDCI_DEV_CONTEXT         *UsbFuncIoDevPtr;

  Status = EFI_SUCCESS;
  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  PlatformSpecificInit ();

  UsbFuncIoDevPtr->StartUpController = FALSE;
  Status = StartXdciController (&UsbFuncIoDevPtr->UsbFunIoProtocol);
  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
    goto DEV_INIT_EXIT;
  }

  Status = UsbXdciDeviceConnect (UsbFuncIoDevPtr->DrvCore);
  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbXdciDeviceConnect Status %x\n", Status));
  if (Status != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
    goto DEV_INIT_EXIT;
  }


DEV_INIT_EXIT:

  return Status;
}

EFI_STATUS
EFIAPI
StartController (
  IN EFI_USBFN_IO_PROTOCOL        *This
  )
{
  return EFI_SUCCESS;
}


EFI_STATUS
UsbFnDeInitDevice (
  IN EFI_USBFN_IO_PROTOCOL        *This
  )
{
  EFI_STATUS                   Status;
  USB_XDCI_DEV_CONTEXT         *UsbFuncIoDevPtr;

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  if (UsbFuncIoDevPtr->StartUpController == FALSE) {
    DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbFn:StopController:The Controller not yet start up force return EFI_SUCCESS\n"));
    return EFI_SUCCESS;
  }

  //
  // disconnect
  //
  Status = UsbDeviceDisconnect (UsbFuncIoDevPtr->DrvCore);
  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbDeviceDisconnect Status %x\n", Status));
  if (Status != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
    goto DEV_DEINIT_EXIT;
  }

  //
  // StopController
  //
  Status = StopXdciController (&UsbFuncIoDevPtr->UsbFunIoProtocol);
  UsbFuncIoDevPtr->StartUpController = FALSE;

DEV_DEINIT_EXIT:
  return Status;
}

EFI_STATUS
EFIAPI
StopController (
  IN EFI_USBFN_IO_PROTOCOL        *This
  )
{
  return UsbFnDeInitDevice (This);
}

