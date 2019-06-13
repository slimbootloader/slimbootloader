/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _XDCI_COMMON_H_
#define _XDCI_COMMON_H_

#define USB_SETUP_DATA_PHASE_DIRECTION_MASK  (0x80)

//
// EP direction
//
typedef enum {
  UsbEpDirOut = 0,
  UsbEpDirIn  = 1
} USB_EP_DIR;

//
// USB Speeds
//
typedef enum {
  USB_SPEED_HIGH  = 0,
  USB_SPEED_FULL,
  USB_SPEED_LOW,
  USB_SPEED_SUPER = 4
} USB_SPEED;

typedef enum {
  USB_ID_DWC_XDCI = 0,
  USB_CORE_ID_MAX
} USB_CONTROLLER_ID;

typedef enum {
  USB_ROLE_HOST = 1,
  USB_ROLE_DEVICE,
  USB_ROLE_OTG
} USB_ROLE;

typedef enum {
  USB_XFER_QUEUED    = 0,
  USB_XFER_SUCCESSFUL,
  USB_XFER_STALL
} USB_XFER_STATUS;

typedef enum {
  USB_DEVICE_DISCONNECT_EVENT       = 0,
  USB_DEVICE_RESET_EVENT,
  USB_DEVICE_CONNECTION_DONE,
  USB_DEVICE_STATE_CHANGE_EVENT,
  USB_DEVICE_WAKEUP_EVENT,
  USB_DEVICE_HIBERNATION_REQ_EVENT,
  USB_DEVICE_SOF_EVENT              = 7,
  USB_DEVICE_ERRATIC_ERR_EVENT      = 9,
  USB_DEVICE_CMD_CMPLT_EVENT,
  USB_DEVICE_BUFF_OVERFLOW_EVENT,
  USB_DEVICE_TEST_LMP_RX_EVENT,
  USB_DEVICE_SETUP_PKT_RECEIVED,
  USB_DEVICE_XFER_NRDY,
  USB_DEVICE_XFER_DONE
} USB_DEVICE_EVENT_ID;

typedef enum {
  U0 = 0,
  U1,
  U2,
  U3,
  SS_DIS,
  RX_DET,
  SS_INACT,
  POLL,
  RECOV,
  HRESET,
  CMPLY,
  LPBK,
  RESUME_RESET = 15
} USB_DEVICE_SS_LINK_STATE;

typedef enum {
  CTRL_SETUP_PHASE,
  CTRL_DATA_PHASE,
  CTRL_STATUS_PHASE
} USB_CONTROL_XFER_PHASE;

typedef enum  {
  USB_EP_STATE_DISABLED = 0,
  USB_EP_STATE_ENABLED,
  USB_EP_STATE_STALLED,
  USB_EP_STATE_SETUP,
  USB_EP_STATE_IN_DATA,
  USB_EP_STATE_OUT_DATA,
  USB_EP_STATE_DATA,
  USB_EP_STATE_STATUS
} USB_EP_STATE;

typedef struct  {
  VOID                      *ParentHandle;
  UINT32                    Hird;
  UINT32                    EpNum;
  USB_SPEED                 Speed;
  USB_EP_STATE              EpState;
  USB_EP_DIR                EpDir;
  UINT8                     EpType;
  USB_DEVICE_SS_LINK_STATE  LinkState;
  UINT8                     *Buffer;
  BOOLEAN                   SsEvent;
} USB_DEVICE_CALLBACK_PARAM;

//
// USB endpoint
//
typedef struct {
  UINT32       EpNum;
  USB_EP_DIR   EpDir;
  UINT8        EpType;
  UINT32       MaxPktSize;
  UINT32       MaxStreams;
  UINT32       BurstSize;
  UINT32       Interval;
  UINT32       Mult;
} USB_EP_INFO;

//
// USB transfer request
//
typedef struct _USB_XFER_REQUEST USB_XFER_REQUEST;

typedef
VOID
(
  EFIAPI *USB_XFER_DONE_CALLBACK
  ) (
    IN VOID                    *XdciHndl,
    IN USB_XFER_REQUEST        *XferReq
    );

struct _USB_XFER_REQUEST {
  VOID                      *XferBuffer;     // Buffer address. bus-width aligned
  UINT32                    XferLen;         // Requested transfer length
  UINT32                    ActualXferLen;  // Actual transfer length at completion callback stage
  UINT32                    StreamId;        // Stream ID. Only relevant for bulk streaming
  UINT32                    FrameNum;        // Only relevant for periodic transfer
  USB_XFER_STATUS           XferStatus;      // Transfer status
  USB_EP_INFO               EpInfo;          // EP info
  USB_XFER_DONE_CALLBACK    XferDone;        // Transfer completion callback
  BOOLEAN                   Zlp;              // Do zero-length transfer
};

#endif

