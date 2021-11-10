/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef _XDCI_INTERFACE_H_
#define _XDCI_INTERFACE_H_

/**
  This function is used to initialize the device controller.
  @param[in] ConfigParams: Parameters from app to configure the core
  @param[in] ParentHandle: Return parameter for upper layers to use
                           for all HW-independent APIs
  @param[in] CoreHandle:   Handle to HW-independent APIs for device
                           controller
**/
typedef
EFI_STATUS
(EFIAPI *DEV_CORE_INIT) (
  IN USB_DEV_CONFIG_PARAMS     *ConfigParams,
  IN VOID                      *ParentHandle,
  IN VOID                      **CoreHandle
  );

/**
  This function is used to de-initialize the device controller.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] flags:      Flags indicating what type of de-initialization is required

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_DEINIT
  ) (
    IN VOID                      *CoreHandle,
    IN UINT32                    Flags
    );

/**
  This function is used to register callback function for
  specified event.
  @param[in] CoreHandle:        Handle to HW-independent APIs for device
                                controller
  @param[in] Event:             Event for which callback is to be registered
  @param[in] callback_fn:       Callback function to be called by the
                                controller driver for above event after critical processing

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_REG_CALLBACK
  ) (
    IN VOID                      *CoreHandle,
    IN USB_DEVICE_EVENT_ID       Event,
    IN EFI_STATUS                (*callback_fn) (IN USB_DEVICE_CALLBACK_PARAM *cb_event_params)
    );

/**
  This function is used to register callback function for
  specified event.
  @param[in] CoreHandle:  Handle to HW-independent APIs for device
                          controller
  @param[in] Event:       Event for which callback is to be unregistered

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_UNREG_CALLBACK
  ) (
    IN VOID                      *CoreHandle,
    IN USB_DEVICE_EVENT_ID       Event
    );

/**
  This function is used to service interrupt events on device
  controller. Use this API in your OS/stack-specific ISR framework
  In polled mode scenario, invoke this API in a loop to service the
  events.
  @param[in] CoreHandle:    Handle to HW-independent APIs for device
                            controller

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_ISR_ROUTINE
  ) (
    IN VOID                      *CoreHandle
    );

/**
  This function is used to enable device controller to connect
  to USB host.
  @param[in] CoreHandle:    Handle to HW-independent APIs for device
                            controller

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_CONNECT
  ) (
    IN VOID                      *CoreHandle
    );

/**
  This function is used to disconnect device controller
  from USB host.
  @param[in] CoreHandle:    Handle to HW-independent APIs for device
                              controller

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_DISCONNECT
  ) (
    IN VOID                      *CoreHandle
    );

/**
  This function is used to obtain USB bus speed after bus reset complete.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] speed: negotiated speed

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_GET_SPEED
  ) (
    IN VOID                      *CoreHandle,
    IN USB_SPEED                 *Speed
    );

/**
  This function is used to set USB device address.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] address: USB device address to set

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_SET_ADDRESS
  ) (
    IN VOID                      *CoreHandle,
    IN UINT32                    Address
    );

/**
  This function is used to do device controller-specific processing
  of set configuration device framework request.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] config_num: configuration number selected by USB host

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_SET_CONFIG
  ) (
    IN VOID                      *CoreHandle,
    IN UINT32                    ConfigNum
    );

/**
  This function is used to set desired link state in device controller.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] state: Desired link state

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_SET_LINK_STATE
  ) (
    IN VOID                      *CoreHandle,
    IN USB_DEVICE_SS_LINK_STATE  State
    );

/**
  This function is used to initialize non-EP0 endpoints.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo: Endpoint information for EP to be initialized

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_INIT_EP
  ) (
    IN VOID                      *CoreHandle,
    IN USB_EP_INFO               *EpInfo
    );

/**
  This function is used to enable an endpoint.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo: Endpoint information for EP to be enabled

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP_ENABLE
  ) (
    IN VOID                      *CoreHandle,
    IN USB_EP_INFO               *EpInfo
    );

/**
  This function is used to disable an endpoint.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo: Endpoint information for EP to be disabled

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP_DISABLE
  ) (
    IN VOID                      *CoreHandle,
    IN USB_EP_INFO               *EpInfo
    );

/**
  This function is used to STALL an endpoint.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo: Endpoint information for EP to be stalled

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP_STALL
  ) (
    IN VOID                      *CoreHandle,
    IN USB_EP_INFO               *EpInfo
    );

/**
  This function is used to clear STALL on an endpoint.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo: Endpoint information for which STALL needs to be cleared

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP_CLEAR_STALL
  ) (
    IN VOID                      *CoreHandle,
    IN USB_EP_INFO               *EpInfo
    );

/**
  This function is used to set EP not ready state.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo: Endpoint information for EP that needs to be
  set in not ready state

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP_SET_NRDY
  ) (
    IN VOID                      *CoreHandle,
    IN USB_EP_INFO               *EpInfo
    );

/**
  This function is used to queue request to receive SETUP packet.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] buffer: Buffer (bus-width aligned) where SETUP packet
  needs to be received

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP0_RX_SETUP_PKT
  ) (
    IN VOID                      *CoreHandle,
    IN UINT8                     *Buffer
    );

/**
  This function is used to queue request to receive status phase
  for control transfer on EP0.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP0_RX_STATUS_PKT
  ) (
    IN VOID                      *CoreHandle
    );

/**
  This function is used to queue request to send status phase for
  control transfer on EP0.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP0_TX_STATUS_PKT
  ) (
    IN VOID                      *CoreHandle
    );

/**
  This function is used to queue a single request to transmit data on
  an endpoint. If more than one request need to be queued before
  previous requests complete then a request queue needs to be
  implemented in upper layers. This API should be not be invoked until
  current request completes.
  Callback for transfer completion is invoked when requested transfer length
  is reached or if a short packet is received.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] xfer_req: Address to transfer request describing this transfer

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP_TX_DATA
  ) (
    IN VOID                      *CoreHandle,
    IN USB_XFER_REQUEST          *XferHandle
    );

/**
  This function is used to queue a single request to receive data on
  an endpoint. If more than one request need to be queued before
  previous requests complete then a request queue needs to be implemented
  in upper layers. This API should be not be invoked until current request
  completes.
  Callback for transfer completion is invoked when requested transfer length
  is reached or if a short packet is received.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] xfer_req: Address to transfer request describing this transfer

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP_RX_DATA
  ) (
    IN VOID                      *CoreHandle,
    IN USB_XFER_REQUEST          *XferHandle
    );

/**
  This function is used to cancel a transfer request that was
  previously queued on an endpoint.
  @param[in] CoreHandle: Handle to HW-independent APIs for device
  controller
  @param[in] EpInfo: Endpoint info where transfer needs to be cancelled

**/
typedef
EFI_STATUS
(
  EFIAPI *DEV_CORE_EP_CANCEL_TRANSFER
  ) (
    IN VOID                      *CoreHandle,
    IN USB_EP_INFO               *EpInfo
    );

struct UsbDeviceCoreDriver {
  DEV_CORE_INIT                 DevCoreInit;
  DEV_CORE_DEINIT               DevCoreDeinit;
  DEV_CORE_REG_CALLBACK         DevCoreRegisterCallback;
  DEV_CORE_UNREG_CALLBACK       DevCoreUnregisterCallback;
  DEV_CORE_ISR_ROUTINE          DevCoreIsrRoutine;
  DEV_CORE_ISR_ROUTINE          DevCoreIsrRoutineTimerBased;
  DEV_CORE_CONNECT              DevCoreConnect;
  DEV_CORE_DISCONNECT           DevCoreDisconnect;
  DEV_CORE_GET_SPEED            DevCoreGetSpeed;
  DEV_CORE_SET_ADDRESS          DevCoreSetAddress;
  DEV_CORE_SET_CONFIG           DevCoreSetConfig;
  DEV_CORE_SET_LINK_STATE       DevCoreSetLinkState;
  DEV_CORE_INIT_EP              DevCoreInitEp;
  DEV_CORE_EP_ENABLE            DevCoreEpEnable;
  DEV_CORE_EP_DISABLE           DevCoreEpDisable;
  DEV_CORE_EP_STALL             DevCoreEpStall;
  DEV_CORE_EP_CLEAR_STALL       DevCoreEpClearStall;
  DEV_CORE_EP_SET_NRDY          DevCoreEpSetNrdy;
  DEV_CORE_EP0_RX_SETUP_PKT     DevCoreEp0RxSetupPkt;
  DEV_CORE_EP0_RX_STATUS_PKT    DevCoreEp0RxStatusPkt;
  DEV_CORE_EP0_TX_STATUS_PKT    DevCoreEp0TxStatusPkt;
  DEV_CORE_EP_TX_DATA           DevCoreEpTxData;
  DEV_CORE_EP_RX_DATA           DevCoreEpRxData;
  DEV_CORE_EP_CANCEL_TRANSFER   DevCoreEpCancelTransfer;
};

/**
  This API is used to obtain the driver handle for HW-independent API.
  @param[in] id The ID of the core for which this driver is requested
**/
const
struct
  UsbDeviceCoreDriver *
UsbDeviceGetCoreDriver (
  USB_CONTROLLER_ID id
  );

#endif

