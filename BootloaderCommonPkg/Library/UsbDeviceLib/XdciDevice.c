/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/UsbDeviceLib.h>
#include "UsbDeviceLibPrivate.h"
#include "XdciCommon.h"
#include "XdciDevice.h"
#include "XdciInterface.h"
#include "UsbDeviceMode.h"

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
  )
{
  USB_DEV_CORE    *DevCorePtr;
  EFI_STATUS      Status;

  Status = EFI_INVALID_PARAMETER;

  DEBUG ((DEBUG_INFO, "Call UsbDeviceInit start\n"));

  //
  // Allocate device handle
  //
  DevCorePtr = AllocateZeroPool (sizeof (USB_DEV_CORE));
  DEBUG ((DEBUG_INFO, "device handle = 0x%x\n", DevCorePtr));

  if (DevCorePtr == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceInit. ERROR: Failed to allocate memory\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((DEBUG_INFO, "call UsbDeviceGetCoreDriver, ID=%x, \n", ConfigParams->ControllerId));

  //
  // Get the driver for this USB device core
  //
  DevCorePtr->CoreDriver = UsbDeviceGetCoreDriver (ConfigParams->ControllerId);
  if (DevCorePtr->CoreDriver != NULL) {
    DEBUG ((DEBUG_INFO, "call DevCoreInit\n"));
    Status = DevCorePtr->CoreDriver->DevCoreInit (
               ConfigParams,
               (VOID *)DevCorePtr,
               &DevCorePtr->ControllerHandle);
  } else {
    DEBUG ((DEBUG_INFO, "UsbDeviceInit. ERROR: Driver not found\n"));
    return EFI_INVALID_PARAMETER;
  }

  *DevCoreHandle = (VOID *)DevCorePtr;
  return Status;
}

/**
  This function is used to de-initialize the device controller.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller
  @param[in] flags           Flags indicating what type of de-initialization is required

**/
EFI_STATUS
UsbDeviceDeinit (
  IN VOID      *DevCoreHandle,
  IN UINT32    Flags
  )
{
  USB_DEV_CORE    *Core;
  EFI_STATUS      Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (Core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceDeinit: ERROR: INVALID HANDLE\n"));
  } else {
    if (Core->CoreDriver != NULL) {
      Status = Core->CoreDriver->DevCoreDeinit (
                 Core->ControllerHandle,
                 Flags
                 );
      FreePool (DevCoreHandle);
    } else {
      DEBUG ((DEBUG_INFO, "UsbDeviceDeinit: Driver not found\n"));
      Status = EFI_INVALID_PARAMETER;
    }
  }

  return Status;
}

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
  )
{
  EFI_STATUS    Status;
  USB_DEV_CORE  *Core;

  Status = EFI_DEVICE_ERROR;
  Core = (USB_DEV_CORE *)DevCoreHandle;

  DEBUG ((DEBUG_INFO, "UsbDeviceRegisterCallback start\n"));

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceRegisterCallback: ERROR: INVALID HANDLE\n"));
  } else {
    if (core->CoreDriver != NULL) {
      DEBUG ((DEBUG_INFO, "Call DevCoreRegisterCallback\n"));
      Status = core->CoreDriver->DevCoreRegisterCallback (
                 core->ControllerHandle,
                 EventId,
                 CallbackFunc
                 );
    }
  }

  return Status;
}

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
  )
{
  EFI_STATUS    Status;
  USB_DEV_CORE  *Core;

  Status = EFI_DEVICE_ERROR;
  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceUnregisterCallback: ERROR: INVALID HANDLE\n"));
  } else {
    if (core->CoreDriver != NULL) {
      Status = core->CoreDriver->DevCoreUnregisterCallback (
                 core->ControllerHandle,
                 EventId
                 );
    }
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceIsrRoutine: ERROR: INVALID HANDLE\n"));
  } else {
    if (core->CoreDriver != NULL) {
      Status = core->CoreDriver->DevCoreIsrRoutine (core->ControllerHandle);
    }
  }

  return Status;
}


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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceIsrRoutine: ERROR: INVALID HANDLE\n"));
  } else {
    if (core->CoreDriver != NULL) {
      Status = core->CoreDriver->DevCoreIsrRoutineTimerBased (core->ControllerHandle);
    }
  }

  return Status;
}


/**
  This function is used to enable device controller to connect
  to USB host.
  @param[in] DevCoreHandle  Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbXdciDeviceConnect (
  IN VOID                      *DevCoreHandle
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbXdciDeviceConnect: ERROR: INVALID HANDLE\n"));
  } else {
    DEBUG ((DEBUG_INFO, "UsbXdciDeviceConnect\n"));
    Status = core->CoreDriver->DevCoreConnect (core->ControllerHandle);
  }

  return Status;
}

/**
  This function is used to disconnect device controller
  from USB host.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbDeviceDisconnect (
  IN VOID                      *DevCoreHandle
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceDisconnect: ERROR: INVALID HANDLE\n"));
  } else {
    DEBUG ((DEBUG_INFO, "UsbDeviceDisconnect\n"));
    Status = core->CoreDriver->DevCoreDisconnect (core->ControllerHandle);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceGetSpeed: ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreGetSpeed (core->ControllerHandle, Speed);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  DEBUG ((DEBUG_INFO, "UsbDeviceSetAddress: enter......\n"));
  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceSetAddress: ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreSetAddress (core->ControllerHandle, Address);
  }
  DEBUG ((DEBUG_INFO, "UsbDeviceSetAddress: exit......\n"));

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceSetConfiguration: ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreSetConfig (core->ControllerHandle, ConfigNum);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceSetLinkState: ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreSetLinkState (core->ControllerHandle, State);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceInitEp: ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreInitEp (core->ControllerHandle, EpInfo);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceEpEnable: ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEpEnable (core->ControllerHandle, EpInfo);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceEpDisable ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEpDisable (core->ControllerHandle, EpInfo);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;


  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceEpStall ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEpStall (core->ControllerHandle, EpInfo);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceEpClearStall ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEpClearStall (core->ControllerHandle, EpInfo);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceEpSetNrdy ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEpSetNrdy (core->ControllerHandle, EpInfo);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;

  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceEp0RxSetup ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEp0RxSetupPkt (core->ControllerHandle, Buffer);
  }

  return Status;
}

/**
  This function is used to queue request to receive status phase
  for control transfer on EP0.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbDeviceEp0RxStatus (
  IN VOID                      *DevCoreHandle
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;


  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceEp0RxStatus ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEp0RxStatusPkt (core->ControllerHandle);
  }
  return Status;
}

/**
  This function is used to queue request to send status phase for
  control transfer on EP0.
  @param[in] DevCoreHandle   Handle to HW-independent APIs for device
  controller

**/
EFI_STATUS
UsbDeviceEp0TxStatus (
  IN VOID                      *DevCoreHandle
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;


  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceEp0TxStatus ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEp0TxStatusPkt (core->ControllerHandle);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;


  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbXdciDeviceEpTxData ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEpTxData (core->ControllerHandle, XferReq);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;


  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbXdciDeviceEpRxData ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEpRxData (core->ControllerHandle, XferReq);
  }

  return Status;
}

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
  )
{
  USB_DEV_CORE  *Core;
  EFI_STATUS    Status;
  Status = EFI_DEVICE_ERROR;

  Core = (USB_DEV_CORE *)DevCoreHandle;


  if (core == NULL) {
    DEBUG ((DEBUG_INFO, "UsbDeviceEpCancelTransfer ERROR: INVALID HANDLE\n"));
  } else {
    Status = core->CoreDriver->DevCoreEpCancelTransfer (core->ControllerHandle, EpInfo);
  }

  return Status;
}

