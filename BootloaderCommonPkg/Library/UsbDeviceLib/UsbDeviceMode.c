/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include "XdciUtility.h"
#include "UsbDeviceMode.h"
#include "UsbDeviceDxe.h"

//
// Global USBD driver object. This is the main private driver object
// that contains all data needed for this driver to operate.
//
USB_DEVICE_DRIVER_OBJ mDrvObj;

//
// Global data IO transaction request object
//
USB_DEVICE_IO_REQ
mCtrlIoReq = {
  //
  // IO information containing the Buffer and data size
  //
  {
    NULL,
    0,
  },
  //
  // Note: This object is used for Control Ep transfers only
  // therefore the endpoint info must always be NULL
  //
  {
    NULL,
    NULL,
  }
};

//
// global flag to signal device event processing loop to run/stop
//
BOOLEAN mXdciRun = FALSE;

STATIC
VOID
XhciSwitchSwid (
  BOOLEAN enable
  )
{
  UINTN                             XhciPciMmBase;
  EFI_PHYSICAL_ADDRESS              XhciMemBaseAddress;
  UINT32                            DualRoleCfg0;
  UINT32                            DualRoleCfg1;

  XhciPciMmBase = MmPciAddress (0, 0, mUsbDeviceInfo.XhciDeviceAddress.Device, mUsbDeviceInfo.XhciDeviceAddress.Func, 0);
  XhciMemBaseAddress = MmioRead32 ((UINTN) (XhciPciMmBase + R_XHCI_MEM_BASE)) & B_XHCI_MEM_BASE_BA;
  DEBUG ((DEBUG_INFO, "XhciPciMmBase=%x, XhciMemBaseAddress=%x\n", XhciPciMmBase, XhciMemBaseAddress));

  DualRoleCfg0 = MmioRead32 ((UINTN) (XhciMemBaseAddress + R_XHCI_MEM_DUAL_ROLE_CFG0));
  if (enable) {
    DualRoleCfg0 = DualRoleCfg0 | (1 << 24) | (1 << 21) | (1 << 20);
    DEBUG ((DEBUG_INFO, "DualRoleCfg0 : Set SW ID : 0x%x \n", DualRoleCfg0));
  } else {
    DualRoleCfg0 = DualRoleCfg0 & ~ (1 << 24) & ~ (1 << 21) & ~ (1 << 20);
    DEBUG ((DEBUG_INFO, "DualRoleCfg0 : Clear SW ID : 0x%x \n", DualRoleCfg0));
  }
  MmioWrite32 ((UINTN) (XhciMemBaseAddress + R_XHCI_MEM_DUAL_ROLE_CFG0), DualRoleCfg0);

  DualRoleCfg1 = MmioRead32 ((UINTN) (XhciMemBaseAddress + R_XHCI_MEM_DUAL_ROLE_CFG1));
  DEBUG ((DEBUG_INFO, "DualRoleCfg1 : 0x%x \n", DualRoleCfg1));
}

VOID
EFIAPI
UsbdMonitorEvents (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  )
{
  USB_XDCI_DEV_CONTEXT    *XdciDevContext;
  UINT32                  EventCount;
  UINT32                  PreEventCount;
  UINT32                  LoopCount;

  XdciDevContext = (USB_XDCI_DEV_CONTEXT *) Context;
  EventCount = UsbRegRead ((UINT32)XdciDevContext->XdciMmioBarAddr, DWC_XDCI_EVNTCOUNT_REG (0));
  if (EventCount == 0) {
    return;
  }

  LoopCount = 0;
  PreEventCount = EventCount;
  while (EventCount != 0) {
    if (UsbDeviceIsrRoutineTimerBased (mDrvObj.XdciDrvObj) != EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "UsbDeviceRun() - Failed to execute event ISR\n"));
    }
    EventCount = UsbRegRead ((UINT32)XdciDevContext->XdciMmioBarAddr, DWC_XDCI_EVNTCOUNT_REG (0));
    if (PreEventCount == EventCount) {
      LoopCount++;
      if (LoopCount >= 5) {
        DEBUG ((DEBUG_INFO, "USB is working on a long event...\n"));
        break;
      }
    } else {
      LoopCount = 0;
    }
  }

  return;
}

/**
  Initializes the XDCI core.

  @param MmioBar       Address of MMIO BAR
  @param XdciHndl      Double pointer to for XDCI layer to set as an
                       opaque handle to the driver to be used in subsequent
                       interactions with the XDCI layer.

  @return EFI_SUCCESS if successfully initialized XDCI, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdInit (
  IN UINT32    MmioBar,
  IN VOID      **XdciHndl
  )
{
  EFI_STATUS               Status;
  USB_DEV_CONFIG_PARAMS    ConfigParams;

  Status = EFI_DEVICE_ERROR;

  XhciSwitchSwid (TRUE);

  DEBUG ((DEBUG_INFO, "UsbdInit start\n"));
  ConfigParams.ControllerId = USB_ID_DWC_XDCI;
  ConfigParams.BaseAddress  = MmioBar;
  ConfigParams.Role = USB_ROLE_DEVICE;
  ConfigParams.Speed = USB_SPEED_SUPER;

  Status = UsbDeviceInit (&ConfigParams, XdciHndl);

  DEBUG ((DEBUG_INFO, "UsbdInit status is %x\n", Status));
  DEBUG ((DEBUG_INFO, "ConfigParams.BaseAddress 0x%x\n", ConfigParams.BaseAddress));

  return Status;
}


/**
  Copies relevant endpoint data from standard USB endpoint descriptors
  to the usbEpInfo structure used by the XDCI.

  @param EpDest   destination structure
  @param EpSrc    source structure

  @return VOID

**/
VOID
UsbdSetEpInfo (
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
    EpDest->EpNum = EpDesc->EndpointAddress & 0x0F; //Bits 0-3 are ep num
    EpDest->EpDir = ((EpDesc->EndpointAddress & USB_ENDPOINT_DIR_IN) > 0) ? UsbEpDirIn : UsbEpDirOut;
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


/**
  Initializes the given endpoint.

  @param XdciHndl  Pointer (handle) to the XDCI driver object
  @param DevEpInfo Pointer to endpoint info structure
                   for the endpoint to initialize

  @return EFI_SUCCESS if operation succeeded, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdInitEp (
  IN VOID                      *XdciHndl,
  IN USB_DEVICE_ENDPOINT_INFO  *DevEpInfo
  )
{
  EFI_STATUS   Status;
  USB_EP_INFO  EpInfo;

  Status = EFI_DEVICE_ERROR;

  UsbdSetEpInfo (&EpInfo, DevEpInfo);
  Status = UsbDeviceInitEp (XdciHndl, &EpInfo);

  return Status;
}


/**
  Callback handler used when transfer operations complete. Calls
  upper layer routine to handle the operation.

  @param XdciHndl  Pointer (handle) to the XDCI driver object
  @param XferReq   Pointer to the transfer request structure

  @return VOID

**/
VOID
EFIAPI
UsbdXferDoneHndlr (
  IN VOID                    *XdciHndl,
  IN USB_XFER_REQUEST        *XferReq
  )
{
  EFI_USB_DEVICE_XFER_INFO  XferInfo;

  DEBUG ((DEBUG_INFO, "UsbdXferDoneHndlr\n"));

  XferInfo.EndpointNum = (UINT8)XferReq->EpInfo.EpNum;
  XferInfo.EndpointDir = XferReq->EpInfo.EpDir;
  XferInfo.EndpointType = XferReq->EpInfo.EpType;
  XferInfo.Buffer = XferReq->XferBuffer;
  XferInfo.Length = XferReq->ActualXferLen;

  //
  // If this is a non-control transfer complete, notify the class driver
  //
  if (XferInfo.EndpointNum > 0) {
    if (mDrvObj.UsbdDevObj->DataCallback != NULL) {
      mDrvObj.UsbdDevObj->DataCallback (&XferInfo);
    }
  }

  return;
}


/**
  Queue a request to transmit data.

  @param XdciHndl  Pointer (handle) to the XDCI driver object
  @param IoReq     Pointer to IO structure containing details of the
                   transfer request

  @return EFI_SUCCESS if operation succeeded, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdEpTxData (
  IN VOID               *XdciHndl,
  IN USB_DEVICE_IO_REQ  *IoReq
  )
{
  EFI_STATUS        Status;
  USB_XFER_REQUEST  TxReq;

  Status = EFI_DEVICE_ERROR;

  //
  //set endpoint data
  //
  UsbdSetEpInfo (& (TxReq.EpInfo), & (IoReq->EndpointInfo)); // set endpoint data

  //
  //if this is a control endpoint, set the number and direction
  //
  if (IoReq->EndpointInfo.EndpointDesc == NULL) {
    TxReq.EpInfo.EpNum = 0;
    TxReq.EpInfo.EpDir = UsbEpDirIn;
  }

  //
  // setup the trasfer request
  //
  TxReq.XferBuffer = IoReq->IoInfo.Buffer;
  TxReq.XferLen = IoReq->IoInfo.Length;
  TxReq.XferDone = UsbdXferDoneHndlr;

  DEBUG ((DEBUG_INFO,  "TX REQUEST: EpNum: 0x%x, epDir: 0x%x, epType: 0x%x, MaxPktSize: 0x%x\n", \
          TxReq.EpInfo.EpNum, TxReq.EpInfo.EpDir, TxReq.EpInfo.EpType, TxReq.EpInfo.MaxPktSize));

  Status = UsbXdciDeviceEpTxData (XdciHndl, &TxReq);

  return Status;
}


/**
  Queue a request to receive data.

  @param XdciHndl  Pointer (handle) to the XDCI driver object
  @param IoReq     Pointer to IO structure containing details of the
                   receive request

  @return EFI_SUCCESS if operation succeeded, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdEpRxData (
  IN VOID               *XdciHndl,
  IN USB_DEVICE_IO_REQ  *IoReq
  )
{
  EFI_STATUS        Status;
  USB_XFER_REQUEST  RxReq;
  UINT32            ReqPacket;

  Status = EFI_DEVICE_ERROR;

  DEBUG ((DEBUG_INFO,  "RX REQUEST in: IoReq->IoInfo.Length: 0x%x\n", IoReq->IoInfo.Length));
  DEBUG ((DEBUG_INFO,  "RX REQUEST in: MaxPacketSize: 0x%x\n", IoReq->EndpointInfo.EndpointDesc->MaxPacketSize));

  if (IoReq->EndpointInfo.EndpointDesc->MaxPacketSize == 0) {
    return EFI_DEVICE_ERROR;
  }

  //
  // set endpoint data
  //
  UsbdSetEpInfo (& (RxReq.EpInfo), & (IoReq->EndpointInfo));

  //
  // setup the trasfer request
  //
  RxReq.XferBuffer = IoReq->IoInfo.Buffer;

  //
  // Transfer length should be multiple of USB packet size.
  //
  ReqPacket = IoReq->IoInfo.Length / IoReq->EndpointInfo.EndpointDesc->MaxPacketSize;
  ReqPacket = ((IoReq->IoInfo.Length % IoReq->EndpointInfo.EndpointDesc->MaxPacketSize) == 0) ? ReqPacket : ReqPacket + 1;
  RxReq.XferLen = ReqPacket * IoReq->EndpointInfo.EndpointDesc->MaxPacketSize;

  RxReq.XferDone = UsbdXferDoneHndlr;

  DEBUG ((DEBUG_INFO,  "RX REQUEST: EpNum: 0x%x, epDir: 0x%x, epType: 0x%x\n", \
          RxReq.EpInfo.EpNum, RxReq.EpInfo.EpDir, RxReq.EpInfo.EpType));
  DEBUG ((DEBUG_INFO,  "RX REQUEST send: XferLen: 0x%x\n", RxReq.XferLen));

  Status = UsbXdciDeviceEpRxData (XdciHndl, &RxReq);

  return Status;
}


/**
  Callback used to handle Reset events from the XDCI.

  @param Param Pointer to a generic callback parameter structure

  @return XDCI usb status

**/
EFI_STATUS
EFIAPI
UsbdResetEvtHndlr (
  IN USB_DEVICE_CALLBACK_PARAM  *Param
  )
{
  EFI_STATUS    Status;

  Status = EFI_DEVICE_ERROR;

  DEBUG ((DEBUG_INFO, "UsbdResetEvtHndlr\n"));

  //
  // reset device address to 0
  //
  Status = UsbDeviceSetAddress (mDrvObj.XdciDrvObj, 0x0);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "UsbdResetHdlr() - Failed to set address in XDCI\n"));
  }

  return Status;
}


/**
  Callback used to handle Connection done events from the XDCI.

  @param Param Pointer to a generic callback parameter structure

  @return XDCI usb status

**/
EFI_STATUS
EFIAPI
UsbdConnDoneEvtHndlr (
  IN USB_DEVICE_CALLBACK_PARAM *Param
  )
{
  EFI_STATUS    Status;

  Status = EFI_DEVICE_ERROR;

  DEBUG ((DEBUG_INFO, "UsbdConnDoneEvtHndlr\n"));

  //
  //reset device address to 0
  //
  Status = UsbDeviceSetAddress (mDrvObj.XdciDrvObj, 0x0);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "UsbdConnDoneHdlr() - Failed to set address in XDCI\n"));
  }

  //
  // set the device state to attached/connected
  //
  mDrvObj.State = UsbDevStateAttached;

  return Status;
}


/**
  Callback used to handle Control Endpoint Setup events from the XDCI.

  @param Param Pointer to a generic callback parameter structure

  @return XDCI usb status

**/
EFI_STATUS
EFIAPI
UsbdSetupEvtHndlr (
  IN USB_DEVICE_CALLBACK_PARAM *Param
  )
{
  EFI_STATUS              Status;
  EFI_USB_DEVICE_REQUEST  Req;

  Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "UsbdSetupEvtHndlr\n"));

  //
  // Fill out request object from the incomming Buffer
  //
  CopyMem (&Req, Param->Buffer, sizeof (EFI_USB_DEVICE_REQUEST));

  Status = UsbdSetupHdlr (&Req);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "UsbdSetupEvtHndlr: EFI_DEVICE_ERROR\n"));
  }

  return Status;
}


/**
   Callback used to handle XferNotReady events from the XDCI.

   @param[in] Param Pointer to a generic callback parameter structure

   @return XDCI usb status
 **/
EFI_STATUS
EFIAPI
UsbdNrdyEvtHndlr (
  IN USB_DEVICE_CALLBACK_PARAM *Param
  )
{
  DEBUG ((DEBUG_INFO, "UsbdNrdyEvtHndlr\n"));
  return EFI_SUCCESS;
}


/**
  Registers callbacks for event handlers with the XDCI layer.
  The functions will be called as the registered events are triggered.

  @param  XdciHndl to XDCI core driver
  @return EFI_SUCCESS if successful, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdRegisterCallbacks (
  IN VOID  *XdciHndl
  )
{
  if (UsbDeviceRegisterCallback (XdciHndl, USB_DEVICE_RESET_EVENT, UsbdResetEvtHndlr) != EFI_SUCCESS) {
    goto UdciRegCallbackError;
  }

  if (UsbDeviceRegisterCallback (XdciHndl, USB_DEVICE_CONNECTION_DONE, UsbdConnDoneEvtHndlr) != EFI_SUCCESS) {
    goto UdciRegCallbackError;
  }

  if (UsbDeviceRegisterCallback (XdciHndl, USB_DEVICE_SETUP_PKT_RECEIVED, UsbdSetupEvtHndlr) != EFI_SUCCESS) {
    goto UdciRegCallbackError;
  }

  if (UsbDeviceRegisterCallback (XdciHndl, USB_DEVICE_XFER_NRDY, UsbdNrdyEvtHndlr) != EFI_SUCCESS) {
    goto UdciRegCallbackError;
  }

  return EFI_SUCCESS;

UdciRegCallbackError:
  return EFI_DEVICE_ERROR;
}


/**
  Returns the configuration descriptor for this device. The data
  Buffer returned will also contain all downstream interface and
  endpoint Buffers.

  @param Buffer    Pointer to destination Buffer to copy descriptor data to
  @param DescIndex the index of the descriptor to return
  @param ReqLen    the length in bytes of the request Buffer
  @param DataLen   Pointer whos value is to be filled with the byte count of
                   data copied to the output Buffer

  @return EFI_SUCCESS if descritor successfully copied, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdGetConfigDesc (
  IN VOID      *Buffer,
  IN UINT8     DescIndex,
  IN UINT32    ReqLen,
  IN UINT32    *DataLen
  )
{
  EFI_STATUS             Status;
  UINT8                  NumConfigs;
  UINT32                 ConfigLen;
  USB_DEVICE_CONFIG_OBJ  *ConfigObj;
  VOID                   *Descriptor;
  UINT32                 Length;

  Status = EFI_DEVICE_ERROR;
  NumConfigs = 0;
  ConfigLen = 0;
  ConfigObj = NULL;
  Descriptor = 0;
  Length = 0;

  DEBUG ((DEBUG_INFO, "UsbdGetConfigDesc()\n"));

  //
  // For a CONFIGURATION request we send back all descriptors branching out
  // from this descriptor including the INTERFACE and ENDPOINT descriptors
  //
  //
  // Verify the requested configuration exists - check valid index
  //
  NumConfigs = mDrvObj.UsbdDevObj->DeviceDesc->NumConfigurations;

  if (DescIndex < NumConfigs) {
    //
    // get the configuration object using the index Offset
    //
    ConfigObj = (mDrvObj.UsbdDevObj->ConfigObjs + DescIndex);
    //
    // get the complete configuration Buffer block including Interface and Endpoint data
    //
    Descriptor = ConfigObj->ConfigAll;
    //
    // The config descriptor TotalLength has the full value for all desc Buffers
    //
    ConfigLen = ConfigObj->ConfigDesc->TotalLength;
    //
    // copy the data to the output Buffer
    //
    Length = MIN (ReqLen, ConfigLen);
    CopyMem (Buffer, Descriptor, Length);
    *DataLen = Length;
    Status = EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_INFO, "UsbdGetConfigDesc() - Invalid Config index: %i\n", DescIndex));
  }

  if (Status == EFI_SUCCESS) {
    if (ConfigObj != NULL) {
      PrintConfigDescriptor (ConfigObj->ConfigDesc);
    }
  }

  return Status;
}


/**
  Sets the active configuration to the selected configuration index if it exists.

  @param CfgValue  the configuration value to set

  @return EFI_SUCCESS if the configuration was set, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdSetConfig (
  UINT8  CfgValue
  )
{
  EFI_STATUS                 Status;
  UINT8                      NumConfigs;
  USB_DEVICE_CONFIG_OBJ      *PtrConfigObj;
  USB_DEVICE_INTERFACE_OBJ   *PtrIfObj;
  USB_DEVICE_ENDPOINT_OBJ    *PtrEpObj;
  UINT8                      CfgItr;
  UINT8                      IfItr;
  UINT8                      EpItr;
  USB_DEVICE_ENDPOINT_INFO   EpInfo;
  USB_EP_INFO                UsbEpInfo;

  Status = EFI_DEVICE_ERROR;
  NumConfigs = 0;
  CfgItr = 0;
  IfItr = 0;
  EpItr = 0;
  PtrConfigObj = NULL;
  PtrIfObj = NULL;
  PtrEpObj = NULL;

  DEBUG ((DEBUG_INFO, "UsbdSetConfig()\n"));
  //
  // Verify the requested configuration exists - check valid index
  //
  NumConfigs = mDrvObj.UsbdDevObj->DeviceDesc->NumConfigurations;

  if (CfgValue != 0) {
    //
    // Search for a matching configuration
    //
    for (CfgItr = 0; CfgItr < NumConfigs; CfgItr++) {
      PtrConfigObj = (mDrvObj.UsbdDevObj->ConfigObjs + CfgItr);
      if (PtrConfigObj->ConfigDesc->ConfigurationValue == CfgValue) {

        //
        // Set the active configuration object
        //
        mDrvObj.ActiveConfigObj = PtrConfigObj;
        //
        // Find all interface objects for this configuration
        //
        for (IfItr = 0; IfItr < PtrConfigObj->ConfigDesc->NumInterfaces; IfItr++) {
          PtrIfObj = (PtrConfigObj->InterfaceObjs + IfItr);
          //
          // Configure the Endpoints in the XDCI
          //
          for (EpItr = 0; EpItr < PtrIfObj->InterfaceDesc->NumEndpoints; EpItr++) {
            PtrEpObj = (PtrIfObj->EndpointObjs + EpItr);

            EpInfo.EndpointDesc = PtrEpObj->EndpointDesc;
            EpInfo.EndpointCompDesc = PtrEpObj->EndpointCompDesc;

            if (UsbdInitEp (mDrvObj.XdciDrvObj, &EpInfo) == EFI_SUCCESS) {
              UsbdSetEpInfo (&UsbEpInfo, &EpInfo);
              if (UsbDeviceEpEnable (mDrvObj.XdciDrvObj, &UsbEpInfo) == EFI_SUCCESS) {
                Status = EFI_SUCCESS;
              } else {
                DEBUG ((DEBUG_INFO, "UsbdSetConfig() - Failed to enable endpoint\n"));
              }
            } else {
              DEBUG ((DEBUG_INFO, "UsbdSetConfig() - Failed to initialize endpoint\n"));
            }
          }
        }
        //
        // Let the class driver know it is configured
        //
        if (Status == EFI_SUCCESS) {
          if (mDrvObj.UsbdDevObj->ConfigCallback != NULL) {
            mDrvObj.UsbdDevObj->ConfigCallback (CfgValue);
          }
        }

        mDrvObj.State = UsbDevStateConfigured; // we are now configured

        break; // break from config search loop
      }
    }
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "UsbdSetConfig() - Invalid requested configuration value: %i\n", CfgValue));
  }

  return Status;
}


/**
  Returns the currently active configuration value.

  @param Buffer    Pointer to destination Buffer to copy configuration value to
  @param ReqLen    the length in bytes of the request Buffer
  @param DataLen   Pointer whos value is to be filled with the byte count of
                   data copied to the output Buffer

  @return EFI_SUCCESS if config value is successfully copied, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdGetConfig (
  VOID      *Buffer,
  UINT32    ReqLen,
  UINT32    *DataLen
  )
{
  EFI_STATUS    Status;

  Status = EFI_DEVICE_ERROR;

  DEBUG ((DEBUG_INFO, "UsbdGetConfig()\n"));

  if (ReqLen >= 1) { // length of data expected must be 1
    if (mDrvObj.ActiveConfigObj != NULL) { // assure we have a config active
      *DataLen = 1; // one byte for ConfigurationValue
      * (UINT8 *)Buffer = mDrvObj.ActiveConfigObj->ConfigDesc->ConfigurationValue;

      Status = EFI_SUCCESS;
    } else {
      DEBUG ((DEBUG_INFO, "UsbdGetConfig() - No active configuration available\n"));
    }
  } else {
    DEBUG ((DEBUG_INFO, "UsbdGetConfig() - Invalid data length\n"));
  }

  return Status;
}


/**
  Returns the requested string descriptor if it exists.

  @param Buffer    Pointer to destination Buffer to copy descriptor data to
  @param DescIndex the index of the descriptor to return
  @param LangId    the target language ID
  @param ReqLen    the length in bytes of the request Buffer
  @param DataLen   Pointer whos value is to be filled with the byte count of
                   data copied to the output Buffer

  @return EFI_SUCCESS if descritor successfully copied, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdGetStringDesc (
  VOID      *Buffer,
  UINT8     DescIndex,
  UINT16    LangId,
  UINT32    ReqLen,
  UINT32    *DataLen
  )
{
  EFI_STATUS             Status;
  UINT32                 Length;
  USB_STRING_DESCRIPTOR  *StringDesc;
  UINT8                  Index;
  UINT8                  StrLangEntries;
  BOOLEAN                StrLangFound;

  Status = EFI_DEVICE_ERROR;
  Length = 0;
  Index = 0;
  StrLangEntries = 0;
  StrLangFound = FALSE;

  DEBUG ((DEBUG_INFO, "UsbdGetStringDesc: Index: 0x%x, LangId: 0x%x, ReqLen: 0x%x\n", DescIndex, LangId, ReqLen));

  //
  // index zero of the string table contains the supported language codes
  //
  if (DescIndex == 0) {
    StringDesc = (mDrvObj.UsbdDevObj->StringTable);
    Length = MIN (ReqLen, StringDesc->Length);
    CopyMem (Buffer, StringDesc, Length);
    *DataLen = Length;
    Status = EFI_SUCCESS;
  } else {

    //
    // Verify the requested language ID is supported. String descriptor Zero
    // (First entry in the string table) is expected to contain the language list.
    // The requested language ID is specified in the Index member of the request.
    //
    StringDesc = mDrvObj.UsbdDevObj->StringTable; // get language string descriptor
    StrLangEntries = ((StringDesc->Length - 2) >> 1);
    DEBUG ((DEBUG_INFO, "StrLangEntries=%x\n", StrLangEntries));

    DEBUG ((DEBUG_INFO, "Looking LangID: \n"));

    for (Index = 0; Index < StrLangEntries; Index++) {
      DEBUG ((DEBUG_INFO, "LangID [%x]= %x\n", Index, StringDesc->LangID [Index]));

      if (StringDesc->LangID [Index] == LangId) {
        DEBUG ((DEBUG_INFO, "Found it\n"));
        StrLangFound = TRUE;
      }
    }

    //
    // If we found a matching language, attempt to get the string index requested
    //
    if (StrLangFound) {
      DEBUG ((DEBUG_INFO, "UsbdGetStringDesc: StrLangFound=Found, DescIndex=%x, StrTblEntries=%x\n", DescIndex,
              mDrvObj.UsbdDevObj->StrTblEntries));

      if (DescIndex < mDrvObj.UsbdDevObj->StrTblEntries) {
        //
        // get the string descriptor for the requested index
        //
        StringDesc = (mDrvObj.UsbdDevObj->StringTable + DescIndex);

        Length = MIN (ReqLen, StringDesc->Length);
        DEBUG ((DEBUG_INFO, "ReqLen=%x, StringLength=%x, Length=%x\n", ReqLen, StringDesc->Length, Length));

        CopyMem (Buffer, StringDesc, Length);
        *DataLen = Length;
        Status = EFI_SUCCESS;
      } else {
        DEBUG ((DEBUG_INFO, "UsbdGetStringDesc: Invalid String index in USB_REQ_GET_DESCRIPTOR request\n"));
      }
    } else {
      DEBUG ((DEBUG_INFO, "UsbdGetStringDesc: Unsupported String Language ID for USB_REQ_GET_DESCRIPTOR request\n"));
    }
  }

  if (Status == EFI_SUCCESS) {
    PrintStringDescriptor (StringDesc);
  }
  return Status;
}


#ifdef SUPPORT_SUPER_SPEED
/**
  Returns the configuration descriptor for this device. The data
  Buffer returned will also contain all downstream interface and
  endpoint Buffers.

  @param Buffer    Pointer to destination Buffer to copy descriptor data to
  @param ReqLen    the length in bytes of the request Buffer
  @param DataLen   Pointer whos value is to be filled with the byte count of
                   data copied to the output Buffer

  @return EFI_SUCCESS if descritor successfully copied, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdGetBOSDesc (
  IN VOID      *Buffer,
  IN UINT32    ReqLen,
  IN UINT32    *DataLen
  )
{
  EFI_USB_BOS_DESCRIPTOR  *BosDesc;
  UINT32                  Length;

  BosDesc = NULL;
  Length = 0;

  DEBUG ((DEBUG_INFO, "UsbdGetBOSDesc()\n"));

  BosDesc = mDrvObj.UsbdDevObj->BosDesc;
  Length = MIN (ReqLen, mDrvObj.UsbdDevObj->BosDesc->TotalLength);

  CopyMem (Buffer, BosDesc, Length);
  *DataLen = Length;

  PrintBOSDescriptor (BosDesc);

  return EFI_SUCCESS;
}
#endif

/**
  Returns the current status for Device/Interface/Endpoint.

  @param Buffer    Pointer to destination Buffer to copy descriptor data to
  @param ReqType   The type of status to get
  @param ReqLen    the length in bytes of the request Buffer
  @param DataLen   Pointer whos value is to be filled with the byte count of
                   data copied to the output Buffer

  @return EFI_SUCCESS if status successfully copied, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdGetStatus (
  VOID      *Buffer,
  UINT8     ReqType,
  UINT32    ReqLen,
  UINT32    *DataLen
  )
{
  EFI_STATUS  Status;

  Status = EFI_DEVICE_ERROR;

  DEBUG ((DEBUG_INFO, "UsbdGetStatus()\n"));

  if (ReqLen >= 2) { // length of data must be at least 2 bytes
    switch (ReqType & USB_TARGET_MASK) {
    case USB_TARGET_DEVICE:
      *DataLen = 2; // two byte for status
      * (UINT16 *)Buffer = USB_STATUS_SELFPOWERED;
      Status = EFI_SUCCESS;
      break;

    case USB_TARGET_INTERFACE:
      //
      // No implementation needed at this time
      //
      break;

    case USB_TARGET_ENDPOINT:
      //
      // No implementation needed at this time
      // Should specify if endpoint is halted. Implement as necessary.
      //
      break;

    case USB_TARGET_OTHER:
      //
      // No implementation needed at this time
      //
      break;

    default:
      break;
    }
  } else {
    DEBUG ((DEBUG_INFO, "UsbdGetStatus() - Invalid data length\n"));
  }

  return Status;
}


/**
  Sets the address of the device.

  @param Address   the address value to set

  @return EFI_SUCCESS if address was set, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdSetAddress (
  UINT8    Address
  )
{
  EFI_STATUS  Status;

  Status = EFI_DEVICE_ERROR;

  DEBUG ((DEBUG_INFO, "UsbdSetAddress: setting address: 0x%x\n", Address));

  if (Address <= 0x7F) { // address must not be > 127
    mDrvObj.Address = Address;

    //
    // Configure Address in the XDCI
    //
    Status = UsbDeviceSetAddress (mDrvObj.XdciDrvObj, mDrvObj.Address);
    if (!EFI_ERROR (Status)) {
      mDrvObj.State = UsbDevStateAddress;
    } else {
      DEBUG ((DEBUG_INFO, "UsbdSetAddress: Failed to set address in XDCI\n"));
    }
  } else {
    DEBUG ((DEBUG_INFO, "UsbdSetAddress: Invalid address: 0x%x\n", Address));
  }

  return Status;
}


/**
  Handles Setup device requests. Standard requests are immediately
  handled here, and any Class/Vendor specific requests are forwarded
  to the class driver.

  @param CtrlRequest  Pointer to a device request

  @return EFI_SUCCESS if request successfully handled, FALSE otherwise

**/
EFI_STATUS
UsbdSetupHdlr (
  IN EFI_USB_DEVICE_REQUEST    *CtrlRequest
  )
{
  EFI_STATUS              Status;
  UINT8                   DescIndex;
  USB_DEVICE_DESCRIPTOR   *DevDesc;

  Status = EFI_DEVICE_ERROR;
  DescIndex = 0;
  DevDesc = NULL;

  //
  // Initialize the IO object
  //
  mCtrlIoReq.IoInfo.Length = 0;

  DEBUG ((DEBUG_INFO, "UsbdSetupHdlr start\n"));
  PrintDeviceRequest (CtrlRequest);

  //
  // Handle Standard Device Requests
  //
  if ((CtrlRequest->RequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD) {
    switch (CtrlRequest->Request) {
    case USB_REQ_GET_DESCRIPTOR:
      DEBUG ((DEBUG_INFO, "UsbdSetupHdlr: Host requests get descriptor\n"));
      if (CtrlRequest->RequestType == USB_RT_TX_DIR_D_TO_H) {
        DescIndex = (CtrlRequest->Value & 0xff); // low byte is the index requested
        switch (CtrlRequest->Value >> 8) { // high byte contains request type
        case USB_DESC_TYPE_DEVICE:
          DEBUG ((DEBUG_INFO, "Descriptor tyep: Device\n"));
          DevDesc = mDrvObj.UsbdDevObj->DeviceDesc;
          //
          // copy the data to the output Buffer
          //
          mCtrlIoReq.IoInfo.Length = MIN (CtrlRequest->Length, DevDesc->Length);
          CopyMem (mCtrlIoReq.IoInfo.Buffer, DevDesc, mCtrlIoReq.IoInfo.Length);
          PrintDeviceDescriptor (DevDesc);
          break;

        case USB_DESC_TYPE_CONFIG:
          DEBUG ((DEBUG_INFO, "Descriptor tyep: Configuration\n"));
          Status = UsbdGetConfigDesc (
                     mCtrlIoReq.IoInfo.Buffer,
                     DescIndex,
                     CtrlRequest->Length,
                     & (mCtrlIoReq.IoInfo.Length)
                     );
          break;

        case USB_DESC_TYPE_STRING:
          DEBUG ((DEBUG_INFO, "Descriptor tyep: String\n"));
          Status = UsbdGetStringDesc (
                     mCtrlIoReq.IoInfo.Buffer,
                     DescIndex,
                     CtrlRequest->Index,
                     CtrlRequest->Length,
                     & (mCtrlIoReq.IoInfo.Length)
                     );
          break;

#ifdef SUPPORT_SUPER_SPEED
        case USB_DESC_TYPE_BOS:
          DEBUG ((DEBUG_INFO, "Descriptor tyep: BOS\n"));
          Status = UsbdGetBOSDesc (
                     mCtrlIoReq.IoInfo.Buffer,
                     CtrlRequest->Length,
                     & (mCtrlIoReq.IoInfo.Length)
                     );
          break;

        case USB_DESC_TYPE_SS_ENDPOINT_COMPANION:
          DEBUG ((DEBUG_INFO, "Descriptor tyep: Endpoint Companion\n"));
          break;
#endif

        default:
          DEBUG ((DEBUG_INFO, "Descriptor tyep: Unsupported, USB_REQ_GET_DESCRIPTOR request: 0x%x\n", (CtrlRequest->Value >> 8)));
          break;
        }
      } else {
        DEBUG ((DEBUG_INFO, "UsbdSetupHdlr() - Invalid direction for USB_REQ_GET_DESCRIPTOR request\n"));
      }
      break;

    case USB_REQ_GET_CONFIG:
      DEBUG ((DEBUG_INFO, "USB_REQ_GET_CONFIG\n"));
      if (CtrlRequest->RequestType == USB_RT_TX_DIR_D_TO_H) {
        Status = UsbdGetConfig (mCtrlIoReq.IoInfo.Buffer, CtrlRequest->Length, & (mCtrlIoReq.IoInfo.Length));
      } else {
        DEBUG ((DEBUG_INFO, "UsbdSetupHdlr: Invalid direction for USB_REQ_GET_CONFIG request\n"));
      }
      break;

    case USB_REQ_SET_CONFIG:
      DEBUG ((DEBUG_INFO, "USB_REQ_SET_CONFIG\n"));
      if (CtrlRequest->RequestType == USB_RT_TX_DIR_H_TO_D) {
        Status = UsbdSetConfig ((UINT8)CtrlRequest->Value);
      } else {
        DEBUG ((DEBUG_INFO, "UsbdSetupHdlr: Invalid direction for USB_REQ_SET_CONFIG request\n"));
      }
      break;

    case USB_REQ_SET_ADDRESS:
      DEBUG ((DEBUG_INFO, "USB_REQ_SET_ADDRESS\n"));
      if (CtrlRequest->RequestType == USB_RT_TX_DIR_H_TO_D) {
        Status = UsbdSetAddress ((UINT8)CtrlRequest->Value);
      } else {
        DEBUG ((DEBUG_INFO, "UsbdSetupHdlr: Invalid direction for USB_REQ_SET_ADDRESS request\n"));
      }
      break;

    case USB_REQ_GET_STATUS:
      DEBUG ((DEBUG_INFO, "USB_REQ_GET_STATUS\n"));
      if (CtrlRequest->RequestType & USB_RT_TX_DIR_D_TO_H) {
        Status = UsbdGetStatus (mCtrlIoReq.IoInfo.Buffer, CtrlRequest->RequestType, CtrlRequest->Length,
                                & (mCtrlIoReq.IoInfo.Length));
      } else {
        DEBUG ((DEBUG_INFO, "UsbdSetupHdlr: Invalid direction for USB_REQ_GET_STATUS request\n"));
      }
      break;
#ifdef SUPPORT_SUPER_SPEED
    case USB_REQ_CLEAR_FEATURE:
    case USB_REQ_SET_FEATURE:
    case USB_REQ_SET_DESCRIPTOR:
    case USB_REQ_GET_INTERFACE:
    case USB_REQ_SET_INTERFACE:
    case USB_REQ_SYNCH_FRAME:
#endif
    default:
      DEBUG ((DEBUG_INFO, "UsbdSetupHdlr: Unsupported Standard Request: 0x%x\n", CtrlRequest->Request));
      break;
    }
  } else { // This is not a Standard request, it specifies Class/Vendor handling
    //
    // Forward request to class driver
    //
    DEBUG ((DEBUG_INFO, "UsbdSetupHdlr: Class/Vendor Request\n"));
    if (mDrvObj.UsbdDevObj->SetupCallback != NULL) {
      mDrvObj.UsbdDevObj->SetupCallback (CtrlRequest, & (mCtrlIoReq.IoInfo));
    }
  }

  DEBUG ((DEBUG_INFO, "dataLen=%x\n", mCtrlIoReq.IoInfo.Length));
  //
  // Transfer data according to request if necessary
  //
  if (mCtrlIoReq.IoInfo.Length > 0) {
    Status = UsbdEpTxData (mDrvObj.XdciDrvObj, &mCtrlIoReq);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "UsbdSetupHdlr: Failed to TX data\n"));
    }
  } else {
    //
    // If we are not responding with data, send control status
    //
    Status = UsbDeviceEp0TxStatus (mDrvObj.XdciDrvObj);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "UsbdSetupHdlr: Failed to Tx Ep0 Status\n"));
    }
  }

  return Status;
}


/**
  Handles Connection done events. Sets the device address to zero.

  @return EFI_SUCCESS if able to set the address, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
UsbdConnDoneHdlr (
  VOID
  )
{
  EFI_STATUS    Status;

  Status = EFI_DEVICE_ERROR;

  DEBUG ((DEBUG_INFO, "UsbdConnDoneHdlr()\n"));

  //
  // reset device address to 0
  //
  Status = UsbDeviceSetAddress (mDrvObj.XdciDrvObj, 0x0);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "UsbdConnDoneHdlr() - Failed to set address in XDCI\n"));
  }

  //
  // set the device state to attached/connected
  //
  mDrvObj.State = UsbDevStateAttached;

  return Status;
}


/**
  Handles transmit/receive completion events. Directly handles
  control endpoint events and forwards class/vendor specific events
  to the class drivers.

  @param   XferInfo   Pointer to Xfer structure

  @return

**/
VOID
UsbdXferDoneHdlr (
  IN EFI_USB_DEVICE_XFER_INFO    *XferInfo
  )
{
  //
  // If this is a non-control transfer complete, notify the class driver
  //
  if (XferInfo->EndpointNum > 0) {
    if (mDrvObj.UsbdDevObj->DataCallback != NULL) {
      mDrvObj.UsbdDevObj->DataCallback (XferInfo);
    }
  }

  return;
}


/**
  Binds a USB class driver with this USB device driver core.
  After calling this routine, the driver is ready to begin
  USB processing.

  @param[in]   This         A pointer to the EFI_USB_DEVICE_MODE_PROTOCOL instance.
  @param[in]   UsbdDevObj   Pointer to a usbd device object which contains
                       all relevant information for the class driver device

  @return TRUE if binding was successful, FALSE otherwise

**/
EFI_STATUS
EFIAPI
UsbDeviceBind (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This,
  IN USB_DEVICE_OBJ                             *UsbdDevObj
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  //
  // allocate Tx Buffer
  //
  mCtrlIoReq.IoInfo.Buffer = AllocateZeroPool (USB_EPO_MAX_PKT_SIZE_ALL);
  if (mCtrlIoReq.IoInfo.Buffer != NULL) {
    mDrvObj.UsbdDevObj = UsbdDevObj;
    mDrvObj.ActiveConfigObj = NULL;
    mDrvObj.Address = 0;
    mDrvObj.State = UsbDevStateInit;
  } else {
    DEBUG ((DEBUG_INFO, "UsbDeviceBind() - Failed to allocate IO Buffer\n"));
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}


/**
  Unbinds the USB class driver from this USB device driver core.

  @param[in]   This         A pointer to the EFI_USB_DEVICE_MODE_PROTOCOL instance.

  @return TRUE if successful, FALSE otherwise

**/
EFI_STATUS
EFIAPI
UsbDeviceUnbind (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  )
{
  mDrvObj.UsbdDevObj = NULL;
  mDrvObj.ActiveConfigObj = NULL;
  mDrvObj.Address = 0;
  mDrvObj.State = UsbDevStateOff;
  mDrvObj.XdciInitialized = FALSE;

  //
  // release allocated Buffer data
  //
  if (mCtrlIoReq.IoInfo.Buffer) {
    FreePool (mCtrlIoReq.IoInfo.Buffer);
  }

  return EFI_SUCCESS;
}


/**
  Performs continual USB device event processing until a cancel
  event occurs.

  @param[in]   This         A pointer to the EFI_USB_DEVICE_MODE_PROTOCOL instance.
  @param[in]   TimeoutMs   Connection timeout in ms. If 0, waits forever.

  @return  TRUE if run executed normally, FALSE if error ocurred

**/
EFI_STATUS
EFIAPI
UsbDeviceRun (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This,
  IN UINT32                                     TimeoutMs
  )
{
  EFI_STATUS              Status;
  USB_XDCI_DEV_CONTEXT    *XdciDevContext;

  Status = EFI_DEVICE_ERROR;

  XdciDevContext = USBUSBD_CONTEXT_FROM_PROTOCOL (This);

  //
  // can only run if XDCI is initialized
  //
  if ((mDrvObj.XdciInitialized == TRUE)) {

    if ((mDrvObj.State == UsbDevStateConfigured) && (XdciDevContext->XdciPollTimer == NULL)) {
      Status = BS_CREAT_EVENT (
                 EVT_TIMER | EVT_NOTIFY_SIGNAL,
                 TPL_NOTIFY,
                 UsbdMonitorEvents,
                 XdciDevContext,
                 &XdciDevContext->XdciPollTimer
                 );
      if (!EFI_ERROR (Status)) {
        Status = BS_SET_TIMER (XdciDevContext->XdciPollTimer, TimerPeriodic, EFI_TIMER_PERIOD_MILLISECONDS (20));
        // DEBUG ((DEBUG_ERROR, "UsbDeviceRun Create Event\n"));
      }
    }

    mXdciRun = TRUE; // set the run flag to active
    Status = EFI_SUCCESS;

    //
    // start the Event processing loop
    //
    while (TRUE) {
      if (XdciDevContext->XdciPollTimer == NULL) {
        if (UsbDeviceIsrRoutine (mDrvObj.XdciDrvObj) != EFI_SUCCESS) {
          DEBUG ((DEBUG_INFO, "UsbDeviceRun() - Failed to execute event ISR\n"));
        }
      }

      //
      // Check if a run cancel request exists, if so exit processing loop
      //
      if (!mXdciRun) {
        if (XdciDevContext->XdciPollTimer != NULL) {
          DEBUG ((DEBUG_ERROR, "UsbDeviceRun close Event\n"));
          BS_SET_TIMER (XdciDevContext->XdciPollTimer, TimerCancel, 0);
          BS_CLOSE_EVENT (XdciDevContext->XdciPollTimer);
          XdciDevContext->XdciPollTimer = NULL;
        }
        Status = EFI_SUCCESS;
        DEBUG ((DEBUG_INFO, "UsbDeviceRun() - processing was cancelled\n"));
        break;
      }

      //
      // check for timeout
      //
      if (TimeoutMs == 0) {
        return EFI_TIMEOUT;
      }

      BS_STALL (50);

      TimeoutMs--;
    }
  }

  return Status;
}


/**
  Sets a flag to stop the running device processing loop.

  @param[in]   This         A pointer to the EFI_USB_DEVICE_MODE_PROTOCOL instance.
  @return TRUE always

**/
EFI_STATUS
EFIAPI
UsbDeviceStop (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  )
{
  mXdciRun = FALSE; // set run flag to FALSE to stop processing
  return EFI_SUCCESS;
}


/**
  Initialize the device controller interface and setup the callback.

  @param[in]   This    A pointer to the EFI_USB_DEVICE_MODE_PROTOCOL instance.

  @return TRUE always

**/
EFI_STATUS
EFIAPI
UsbDeviceInitXdci (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  )
{
  EFI_STATUS            Status;
  USB_XDCI_DEV_CONTEXT  *XdciDevContext;

  Status = EFI_DEVICE_ERROR;

  XdciDevContext = USBUSBD_CONTEXT_FROM_PROTOCOL (This);

  PlatformSpecificInit ();

  if (mDrvObj.XdciInitialized == FALSE) {
    if (XdciDevContext->XdciMmioBarAddr != 0) {

      //
      // Initialize device controller driver
      //
      DEBUG ((DEBUG_INFO, "UsbDeviceInitXdci() - Initializing Controller...\n"));

      //
      // Initialize the device controller interface
      //
      if (UsbdInit ((UINT32)XdciDevContext->XdciMmioBarAddr, &mDrvObj.XdciDrvObj) == EFI_SUCCESS) {

        //
        // Setup callbacks
        //
        if (UsbdRegisterCallbacks (mDrvObj.XdciDrvObj) == EFI_SUCCESS) {

          mDrvObj.XdciInitialized = TRUE;
          Status = EFI_SUCCESS;

          DEBUG ((DEBUG_INFO, "UsbDeviceInitXdci() - Controller initialization complete\n"));
        } else {
          DEBUG ((DEBUG_INFO, "UsbDeviceInitXdci() - Failed to register UDCI callbacks\n"));
        }
      } else {
        DEBUG ((DEBUG_INFO, "UsbDeviceInitXdci() - Failed to initialize UDCI\n"));
      }
    } else {
      DEBUG ((DEBUG_INFO, "UsbDeviceInitXdci() - XDCI MMIO BAR not set\n"));
    }
  } else {
    DEBUG ((DEBUG_INFO, "UsbDeviceInitXdci() - XDCI already initialized\n"));
    Status = EFI_ALREADY_STARTED;
  }

  return Status;
}


/**
  Wrapper function to enable device controller to connect
  to USB host.

  @param[in]   This    A pointer to the EFI_USB_DEVICE_MODE_PROTOCOL instance.

**/
EFI_STATUS
EFIAPI
UsbDeviceConnect (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  )
{
  EFI_STATUS  Status;

  Status = EFI_DEVICE_ERROR;

  DEBUG ((DEBUG_INFO,  "UsbDeviceConnect \n"));
  if (UsbXdciDeviceConnect (mDrvObj.XdciDrvObj) == EFI_SUCCESS) {
    Status = EFI_SUCCESS;
  }
  return Status;
}


/**
  Wrapper function to disconnect device controller
  from USB host.

  @param[in]   This    A pointer to the EFI_USB_DEVICE_MODE_PROTOCOL instance.

**/
EFI_STATUS
EFIAPI
UsbDeviceDisConnect (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  )
{
  EFI_STATUS  Status;

  Status = EFI_DEVICE_ERROR;

  DEBUG ((DEBUG_INFO,  "UsbDeviceDisConnect \n"));
  if (UsbDeviceDisconnect (mDrvObj.XdciDrvObj) == EFI_SUCCESS) {
    mDrvObj.State = UsbDevStateInit;
    Status = EFI_SUCCESS;
  }

  XhciSwitchSwid (FALSE);
  return Status;
}


/**
  Queue a request to transmit data.

  @param This       A pointer to the EFI_USB_DEVICE_MODE_PROTOCOL instance.
  @param IoRequest  Pointer to IO structure containing details of the
                    transfer request

  @return EFI_SUCCESS if operation succeeded, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
EFIAPI
UsbDeviceEpTxData (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This,
  IN USB_DEVICE_IO_REQ                          *IoRequest
  )
{
  EFI_STATUS  Status;

  Status = UsbdEpTxData (mDrvObj.XdciDrvObj, IoRequest);
  return Status;
}


/**
  Queue a request to receive data.

  @param This       A pointer to the EFI_USB_DEVICE_MODE_PROTOCOL instance.
  @param IoRequest  Pointer to IO structure containing details of the
                    transfer request

  @return EFI_SUCCESS if operation succeeded, EFI_DEVICE_ERROR otherwise

**/
EFI_STATUS
EFIAPI
UsbDeviceEpRxData (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This,
  IN USB_DEVICE_IO_REQ                          *IoRequest
  )
{
  EFI_STATUS  Status;

  Status = UsbdEpRxData (mDrvObj.XdciDrvObj, IoRequest);
  return Status;
}


//
// The Runtime UsbDeviceMode Protocol instance produced by this driver
//
EFI_USB_DEVICE_MODE_PROTOCOL  mUsbDeviceModeProtocol = {
  UsbDeviceInitXdci,
  UsbDeviceConnect,
  UsbDeviceDisConnect,
  UsbDeviceEpTxData,
  UsbDeviceEpRxData,
  UsbDeviceBind,
  UsbDeviceUnbind,
  UsbDeviceRun,
  UsbDeviceStop
};

VOID
CheckUsbEvent (
  VOID
  )
{
  if (mUsbXdciDevContext && mUsbXdciDevContext->XdciPollTimer) {
    UsbdMonitorEvents (NULL, mUsbXdciDevContext);
  }
};
