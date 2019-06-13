/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef _XDCI_DEVICE_H_
#define _XDCI_DEVICE_H_

//
// USB_DEV_CONFIG_PARAMS: Struct to be filled in with configuration
// parameters and passed to the init routine for device controller
//
typedef struct {
  USB_CONTROLLER_ID  ControllerId; // Controller ID of the core
  UINT32             BaseAddress;  // Base address of the controller registers and on-chip memory
  UINT32             Flags;        // Initialization flags
  USB_SPEED          Speed;        // Desired USB bus Speed
  USB_ROLE           Role;         // Default USB role
} USB_DEV_CONFIG_PARAMS;

//
// USB_DEV_CORE: Struct used as a handle for all
// hardware-independent APIs
//
typedef struct {
  const struct UsbDeviceCoreDriver *CoreDriver;
  VOID                                *ControllerHandle;
} USB_DEV_CORE;

typedef
EFI_STATUS
(
  EFIAPI *USB_DEVICE_CALLBACK_FUNC
  ) (
    IN USB_DEVICE_CALLBACK_PARAM  *Param
    );

/**
  This function is used to initialize the device controller.
  @param[in]      ConfigParams    Parameters from app to configure the core
  @param[in, out] DevCoreHandle   Return parameter for upper layers to use
  for all HW-independent APIs

**/
EFI_STATUS
UsbDeviceInit (
  IN USB_DEV_CONFIG_PARAMS    *ConfigParams,
  IN OUT VOID                 **DevCoreHandle
  );

/**
  This function is used to de-initialize the device controller.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] flags           Flags indicating what type of de-initialization is required

**/
EFI_STATUS
UsbDeviceDeinit (
  IN VOID                      *DevCoreHandle,
  IN UINT32                    Flags
  );

/**
  This function is used to register callback function for
  specified event.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] EventId         Event for which callback is to be registered
  @param[in] CallbackFunc    Callback function to be called by the
  controller driver for above event after critical processing

**/
EFI_STATUS
UsbDeviceRegisterCallback (
  IN VOID                      *DevCoreHandle,
  IN USB_DEVICE_EVENT_ID       EventId,
  IN USB_DEVICE_CALLBACK_FUNC  CallbackFunc
  );

/**
  This function is used to register callback function for
  specified event.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] EventId         Event for which callback is to be unregistered

**/
EFI_STATUS
UsbDeviceUnregisterCallback (
  IN VOID                      *DevCoreHandle,
  IN USB_DEVICE_EVENT_ID       EventId
  );

/**
  This function is used to service interrupt events on device
  controller. Use this API in your OS/stack-specific ISR framework
  In polled mode scenario, invoke this API in a loop to service the
  events.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbDeviceIsrRoutine (
  IN VOID                      *DevCoreHandle
  );

/**
  This function is used to service interrupt events on device
  controller. Use this API in your OS/stack-specific ISR framework
  In polled mode scenario, invoke this API in a loop to service the
  events.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbDeviceIsrRoutineTimerBased (
  IN VOID                      *DevCoreHandle
  );

/**
  This function is used to enable device controller to connect
  to USB host.
  @param[in] DevCoreHandle  Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbXdciDeviceConnect (
  IN VOID                      *DevCoreHandle
  );

/**
  This function is used to disconnect device controller
  from USB host.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbDeviceDisconnect (
  IN VOID                      *DevCoreHandle
  );

/**
  This function is used to obtain USB bus Speed after bus reset complete.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] Speed           negotiated Speed

**/
EFI_STATUS
UsbDeviceGetSpeed (
  IN VOID                      *DevCoreHandle,
  IN USB_SPEED                 *Speed
  );

/**
  This function is used to set desired link state in device controller.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] State           Desired link state

**/
EFI_STATUS
UsbDeviceSetLinkState (
  IN VOID                      *DevCoreHandle,
  IN USB_DEVICE_SS_LINK_STATE  State
  );

/**
  This function is used to set USB device address.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] Address         USB device address to set

**/
EFI_STATUS
UsbDeviceSetAddress (
  IN VOID                      *DevCoreHandle,
  IN UINT32                    Address
  );

/**
  This function is used to do device controller-specific processing
  of set configuration device framework request.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] ConfigNum       configuration number selected by USB host

**/
EFI_STATUS
UsbDeviceSetConfiguration (
  IN VOID                      *DevCoreHandle,
  IN UINT32                    ConfigNum
  );

/**
  This function is used to initialize non-EP0 endpoints.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo          Endpoint information for EP to be initialized

**/
EFI_STATUS
UsbDeviceInitEp (
  IN VOID                      *DevCoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

/**
  This function is used to enable an endpoint.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo          Endpoint information for EP to be enabled

**/
EFI_STATUS
UsbDeviceEpEnable (
  IN VOID                      *DevCoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

/**
  This function is used to disable an endpoint.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo          Endpoint information for EP to be disabled

**/
EFI_STATUS
UsbDeviceEpDisable (
  IN VOID                      *DevCoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

/**
  This function is used to STALL an endpoint.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo          Endpoint information for EP to be stalled

**/
EFI_STATUS
UsbDeviceEpStall (
  IN VOID                      *DevCoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

/**
  This function is used to clear STALL on an endpoint.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo          Endpoint information for which STALL needs to be cleared

**/
EFI_STATUS
UsbDeviceEpClearStall (
  IN VOID                      *DevCoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

/**
  This function is used to set EP not ready state.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo          Endpoint information for EP that needs to be
  set in not ready state

**/
EFI_STATUS
UsbDeviceEpSetNrdy (
  IN VOID                      *DevCoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

/**
  This function is used to queue request to receive SETUP packet.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] Buffer          Buffer (bus-width aligned) where SETUP packet
  needs to be received

**/
EFI_STATUS
UsbDeviceEp0RxSetup (
  IN VOID                      *DevCoreHandle,
  IN UINT8                     *Buffer
  );

/**
  This function is used to queue request to receive status phase
  for control transfer on EP0.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbDeviceEp0RxStatus (
  IN VOID                      *DevCoreHandle
  );

/**
  This function is used to queue request to send status phase for
  control transfer on EP0.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbDeviceEp0TxStatus (
  IN VOID                      *DevCoreHandle
  );

/**
  This function is used to queue a single request to transmit data on
  an endpoint. If more than one request need to be queued before
  previous requests complete then a request queue needs to be
  implemented in upper layers. This API should be not be invoked until
  current request completes.
  Callback for transfer completion is invoked when requested transfer length
  is reached or if a short packet is received.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] XferReq         Address to transfer request describing this transfer

**/
EFI_STATUS
UsbXdciDeviceEpTxData (
  IN VOID                      *DevCoreHandle,
  IN USB_XFER_REQUEST          *XferReq
  );

/**
  This function is used to queue a single request to receive data on
  an endpoint. If more than one request need to be queued before
  previous requests complete then a request queue needs to be implemented
  in upper layers. This API should be not be invoked until current request
  completes.
  Callback for transfer completion is invoked when requested transfer length
  is reached or if a short packet is received.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] XferReq         Address to transfer request describing this transfer

**/
EFI_STATUS
UsbXdciDeviceEpRxData (
  IN VOID                      *DevCoreHandle,
  IN USB_XFER_REQUEST          *XferReq
  );

/**
  This function is used to cancel a transfer request that was
  previously queued on an endpoint.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo          Endpoint info where transfer needs to be cancelled

**/
EFI_STATUS
UsbDeviceEpCancelTransfer (
  IN VOID                      *DevCoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

#endif

