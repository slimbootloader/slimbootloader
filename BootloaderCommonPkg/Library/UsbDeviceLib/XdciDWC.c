/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UsbDeviceMode.h"
#include "XdciInterface.h"
#include "XdciDWC.h"

#pragma optimize ("", off)

/**
  Function to read MMIO USB register.
  @param[in] Base       Base address of MMIO.
  @param[in] Offset     Offset of the register.
**/
UINT32
UsbRegRead (
  IN UINT32    Base,
  IN UINT32    Offset
  )
{
  volatile UINT32 *Addr;
  Addr = (volatile UINT32 *) (UINTN) (Base + Offset);
  return *Addr;
}

/**
  Function to write MMIO USB register.
  @param[in] Base       Base address of MMIO.
  @param[in] Offset     Offset of the register.
  @param[in] val        Value to write onto the register.
**/
VOID
UsbRegWrite (
  IN UINT32    Base,
  IN UINT32    Offset,
  IN UINT32    val
  )
{
  volatile UINT32 *Addr;
  Addr = (volatile UINT32 *) (UINTN) (Base + Offset);
  *Addr = val;
}


/**
  Internal utility function:
  This function is used to obtain physical endpoint number
  xDCI needs physical endpoint number for EP registers
  We also use it to index into our EP array
  Note: Certain data structures/commands use logical EP numbers
  as opposed to physical endpoint numbers so one should be
  careful when interpreting EP numbers.
  @param[in] EndpointNum Logical endpoint number
  @param[in] EndpointDir Direction for the endpoint

**/
STATIC
UINT32
DwcXdciGetPhysicalEpNum (
  IN UINT32        EndpointNum,
  IN USB_EP_DIR    EndpointDir
  )
{
  return (EndpointDir != 0) ? ((EndpointNum << 1) | EndpointDir) : (EndpointNum << 1);
}


/**
  Internal utility function:
  This function is used to obtain the MPS for control transfers
  Based on the Speed. If this is called before bus reset completes
  then it returns MPS Based on desired Speed. If it is after bus
  reset then MPS returned is Based on actual negotiated Speed.
  @param[in] CoreHandle     xDCI controller handle address
  @param[in] mps            address of 32-bit var able to return the MPS

**/
STATIC
EFI_STATUS
DwcXdciCoreGetCtrlMps (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              *mps
  )
{
  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreGetCtrlMps: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  if (mps == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreGetCtrlMps: INVALID parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  switch (CoreHandle->ActualSpeed) {
  case USB_SPEED_HIGH:
    *mps = DWC_XDCI_HS_CTRL_EP_MPS;
    break;
  case USB_SPEED_FULL:
    *mps = DWC_XDCI_FS_CTRL_EP_MPS;
    break;
  case USB_SPEED_LOW:
    *mps = DWC_XDCI_LS_CTRL_EP_MPS;
    break;
  case USB_SPEED_SUPER:
    *mps = DWC_XDCI_SS_CTRL_EP_MPS;
    break;
  default:
    *mps = 0;
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreGetCtrlMps: UNKNOWN Speed\n"));
    break;
  }

  return EFI_SUCCESS;
}


/**
  Internal utility function:
  This function is used to initialize the parameters required
  for executing endpoint command.
  @param[in] CoreHandle      xDCI controller handle address
  @param[in] EpInfo          EP info address
  @param[in] ConfigAction    Configuration action specific to EP command
  @param[in] EpCmd           xDCI EP command for which parameters are initialized
  @param[in] EpCmdParams     address of struct to return EP params

**/
STATIC
EFI_STATUS
DwcXdciCoreInitEpCmdParams (
  IN XDCI_CORE_HANDLE                *CoreHandle,
  IN USB_EP_INFO                     *EpInfo,
  IN UINT32                          ConfigAction,
  IN DWC_XDCI_ENDPOINT_CMD           EpCmd,
  IN DWC_XDCI_ENDPOINT_CMD_PARAMS    *EpCmdParams
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreInitEpCmdParams: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Reset params
  //
  EpCmdParams->Param0 = EpCmdParams->Param1 = EpCmdParams->Param2 = 0;

  switch (EpCmd) {
  case EPCMD_SET_EP_CONFIG:
    //
    // Issue DEPCFG command for EP
    // Issue a DEPCFG (Command 1) command for endpoint
    //
    if (EpInfo->MaxStreams) {
      EpCmdParams->Param1 = DWC_XDCI_PARAM1_SET_EP_CFG_STRM_CAP_MASK;
    }

    if (EpInfo->Interval) {
      EpCmdParams->Param1 |= ((EpInfo->Interval - 1) << DWC_XDCI_PARAM1_SET_EP_CFG_BINTM1_BIT_POS);
    }

    //
    // Set EP num
    //
    EpCmdParams->Param1 |= (EpInfo->EpNum << DWC_XDCI_PARAM1_SET_EP_CFG_EP_NUM_BIT_POS);
    //
    // Set EP direction
    //
    EpCmdParams->Param1 |= (EpInfo->EpDir << DWC_XDCI_PARAM1_SET_EP_CFG_EP_DIR_BIT_POS);
    //
    // Set EP-specific Event enable for not ready and
    // complete events
    //
    EpCmdParams->Param1 &= ~DWC_XDCI_PARAM1_SET_EP_CFG_EVT_EN_MASK;
    //
    // Setup the events we want enabled for this EP
    //
    EpCmdParams->Param1 |= (DWC_XDCI_PARAM1_SET_EP_CFG_EVT_XFER_NRDY_MASK |
                            DWC_XDCI_PARAM1_SET_EP_CFG_EVT_XFER_IN_PRG_MASK |
                            DWC_XDCI_PARAM1_SET_EP_CFG_EVT_XFER_CMPLT_MASK);

    //
    // We only have one interrupt line for this core.
    // Set interrupt number to 0
    //
    EpCmdParams->Param1 &= ~DWC_XDCI_PARAM1_SET_EP_CFG_INTR_NUM_MASK;

    //
    // Set FIFOnum = 0 for control EP0
    //
    EpCmdParams->Param0 &= ~DWC_XDCI_PARAM0_SET_EP_CFG_FIFO_NUM_MASK;

    //
    // Program FIFOnum for non-EP0 EPs
    //
    if (EpInfo->EpNum && EpInfo->EpDir) {
      EpCmdParams->Param0 |= (EpInfo->EpNum << DWC_XDCI_PARAM0_SET_EP_CFG_FIFO_NUM_BIT_POS);
    }

    //
    // Program max packet size
    //
    EpCmdParams->Param0 &= ~DWC_XDCI_PARAM0_SET_EP_CFG_MPS_MASK;
    EpCmdParams->Param0 |= (EpInfo->MaxPktSize << DWC_XDCI_PARAM0_SET_EP_CFG_MPS_BIT_POS);

    //
    // Set Burst size. 0 means burst size of 1
    //
    EpCmdParams->Param0 &= ~DWC_XDCI_PARAM0_SET_EP_CFG_BRST_SIZE_MASK;
    EpCmdParams->Param0 |= (EpInfo->BurstSize << DWC_XDCI_PARAM0_SET_EP_CFG_BRST_SIZE_BIT_POS);

    //
    // Set EP type
    //
    EpCmdParams->Param0 &= ~DWC_XDCI_PARAM0_SET_EP_CFG_EP_TYPE_MASK;
    EpCmdParams->Param0 |= (EpInfo->EpType << DWC_XDCI_PARAM0_SET_EP_CFG_EP_TYPE_BIT_POS);

    //
    // Set config action
    //
    EpCmdParams->Param0 &= ~DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_MASK;
    EpCmdParams->Param0 |= (ConfigAction << DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_BIT_POS);
    break;

  case EPCMD_SET_EP_XFER_RES_CONFIG:
    // Set Param0 to 1. Same for all EPs when resource
    // configuration is done
    //
    EpCmdParams->Param0 = 1;
    break;

  case EPCMD_END_XFER:
    //
    // Nothing to set. Already reset params for all cmds
    //
    break;

  case EPCMD_START_NEW_CONFIG:
    //
    // Nothing to set. Already reset params for all cmds
    //
    break;

  default:
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((DEBUG_INFO, "\nDwcXdciCoreInitEpCmdParams: INVALID Parameter"));
    break;
  }

  return Status;
}


/**
  Internal utility function:
  This function is used to issue the xDCI endpoint command.
  @param[in] CoreHandle: xDCI controller handle address
  @param[in] EpNum: Physical EP num
  @param[in] EpCmd: xDCI EP command
  @param[in] EpCmdParams: EP command parameters address

**/
STATIC
EFI_STATUS
DwcXdciCoreIssueEpCmd (
  IN XDCI_CORE_HANDLE                *CoreHandle,
  IN UINT32                          EpNum,
  IN UINT32                          EpCmd,
  IN DWC_XDCI_ENDPOINT_CMD_PARAMS    *EpCmdParams
  )
{
  UINT32 BaseAddr;
  UINT32 MaxDelayIter;//DWC_XDCI_MAX_DELAY_ITERATIONS;

  MaxDelayIter = 5000;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreIssueEpCmd: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = CoreHandle->BaseAddress;

  //
  // Set EP command parameter values
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EPCMD_PARAM2_REG (EpNum),
    EpCmdParams->Param2
    );

  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EPCMD_PARAM1_REG (EpNum),
    EpCmdParams->Param1
    );

  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EPCMD_PARAM0_REG (EpNum),
    EpCmdParams->Param0
    );

  //
  // Set the command code and activate it
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EPCMD_REG (EpNum),
    EpCmd | DWC_XDCI_EPCMD_CMD_ACTIVE_MASK
    );

  //
  // Wait until command completes
  //
  do {
    if ((UsbRegRead (BaseAddr, DWC_XDCI_EPCMD_REG (EpNum)) & DWC_XDCI_EPCMD_CMD_ACTIVE_MASK) == 0) {
      break;
    } else {
      gBS->Stall (DWC_XDCI_MAX_DELAY_ITERATIONS);
    }
  } while ((--MaxDelayIter) != 0);

  if (MaxDelayIter == 0) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreIssueEpCmd. ERROR: Failed to issue Command\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Internal utility function:
  This function is used to flush all FIFOs.
  @param[in] CoreHandle  xDCI controller handle address

**/
STATIC
EFI_STATUS
DwcXdciCoreFlushAllFifos (
  IN XDCI_CORE_HANDLE    *CoreHandle
  )
{
  UINT32 BaseAddr;
  UINT32 MaxDelayIter;

  MaxDelayIter = DWC_XDCI_MAX_DELAY_ITERATIONS;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreFlushAllFifos: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = CoreHandle->BaseAddress;

  //
  // Write the command to flush all FIFOs
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DGCMD_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) | DWC_XDCI_DGCMD_CMD_ALL_FIFO_FLUSH | DWC_XDCI_DGCMD_CMD_ACTIVE_MASK)
    );

  //
  // Wait until command completes
  //
  do {
    if ((UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) & DWC_XDCI_DGCMD_CMD_ACTIVE_MASK) == 0) {
      break;
    } else {
      gBS->Stall (DWC_XDCI_MAX_DELAY_ITERATIONS);
    }
  } while ((--MaxDelayIter) != 0);

  if (MaxDelayIter == 0) {
    DEBUG ((DEBUG_INFO, "Failed to issue Command\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Internal utility function:
  This function is used to flush Tx FIFO specific to an endpoint.
  @param[in] CoreHandle  xDCI controller handle address
  @param[in] EpNum       Physical EP num

**/
STATIC
EFI_STATUS
DwcXdciCoreFlushEpTxFifo (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              EpNum
  )
{
  UINT32 BaseAddr;
  UINT32 MaxDelayIter;
  UINT32 fifoNum;

  MaxDelayIter = DWC_XDCI_MAX_DELAY_ITERATIONS;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreFlushEpTxFifo: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = CoreHandle->BaseAddress;

  //
  // Translate to FIFOnum
  // NOTE: Assuming this is a Tx EP
  //
  fifoNum = (EpNum >> 1);

  //
  // TODO: Currently we are only using TxFIFO 0. Later map these
  // Write the FIFO num/dir param for the generic command.
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DGCMD_PARAM_REG,
    ((UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_PARAM_REG) & ~DWC_XDCI_DGCMD_PARAM_TX_FIFO_NUM_MASK) |
     DWC_XDCI_DGCMD_PARAM_TX_FIFO_DIR_MASK)
    );

  //
  // Write the command to flush all FIFOs
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DGCMD_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) | DWC_XDCI_DGCMD_CMD_SEL_FIFO_FLUSH | DWC_XDCI_DGCMD_CMD_ACTIVE_MASK)
    );


  //
  // Wait until command completes
  //
  do {
    if ((UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) & DWC_XDCI_DGCMD_CMD_ACTIVE_MASK) == 0) {
      break;
    } else {
      gBS->Stall (DWC_XDCI_MAX_DELAY_ITERATIONS);
    }
  } while ((--MaxDelayIter) != 0);

  if (MaxDelayIter == 0) {
    DEBUG ((DEBUG_INFO, "Failed to issue Command\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}



STATIC
EFI_STATUS
DwcXdciCorePrepareOneTrb (
  IN DWC_XDCI_TRB            *Trb,
  IN DWC_XDCI_TRB_CONTROL    TrbCtrl,
  IN UINT32                  LastBit,
  IN UINT32                  ChainBit,
  IN UINT8                   *BufferPtr,
  IN UINT32                  size
  )
{
  DEBUG ((DEBUG_INFO, "Trb is 0x%x, BufferPtr is 0x%x, size is 0x%x\n", Trb, BufferPtr, size));

  Trb->BuffPtrLow = (UINT32) (UINTN)BufferPtr;
  Trb->BuffPtrHigh = 0;
  Trb->LenXferParams = size;
  Trb->TrbCtrl = TrbCtrl << DWC_XDCI_TRB_CTRL_TYPE_BIT_POS;

  if (ChainBit != 0) {
    Trb->TrbCtrl |= ChainBit << DWC_XDCI_TRB_CTRL_CHAIN_BUFF_BIT_POS;
  }

  if (LastBit != 0) {
    Trb->TrbCtrl |= LastBit << DWC_XDCI_TRB_CTRL_LST_TRB_BIT_POS;
  }

  Trb->TrbCtrl |= DWC_XDCI_TRB_CTRL_IOSP_MISOCH_MASK | DWC_XDCI_TRB_CTRL_HWO_MASK;

  DEBUG ((DEBUG_INFO,
          "(DwcXdciCorePrepareOneTrb) Trb->BuffPtrLow = 0x%x, Trb->LenXferParams is 0x%x, Trb->TrbCtrl is 0x%x\n",
          Trb->BuffPtrLow, Trb->LenXferParams, Trb->TrbCtrl));
  return EFI_SUCCESS;
}


/**
  Internal utility function:
  This function is used to initialize transfer request block.
  @param[in] CoreHandle  xDCI controller handle address
  @param[in] Trb         Address of TRB to initialize
  @param[in] TrbCtrl     TRB control value
  @param[in] BufferPtr   Transfer Buffer address
  @param[in] size        Size of the transfer

**/
STATIC
EFI_STATUS
DwcXdciCoreInitTrb (
  IN XDCI_CORE_HANDLE        *CoreHandle,
  IN DWC_XDCI_TRB            *Trb,
  IN DWC_XDCI_TRB_CONTROL    TrbCtrl,
  IN UINT8                   *BufferPtr,
  IN UINT32                  size
  )
{
#define ONE_TRB_SIZE      (DWC_XDCI_TRB_BUFF_SIZE_MASK & 0x00F00000)
  UINT8                   *TrbBuffer;
  UINT32                  TrbCtrlLast;
  UINT32                  TrbCtrlChain;
  UINT32                  TrbIndex;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreInitTrb: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  if (Trb == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreInitTrb: INVALID handle\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Init TRB fields
  // NOTE: Assuming we are only using 32-bit addresses
  // TODO: update for 64-bit addresses
  //
  if (size <= DWC_XDCI_TRB_BUFF_SIZE_MASK) {
    //
    // Can transfer in one TRB
    //
    TrbCtrlChain = 0;
    TrbCtrlLast = 1;
    DwcXdciCorePrepareOneTrb (Trb, TrbCtrl, TrbCtrlLast, TrbCtrlChain, BufferPtr, size);
    return EFI_SUCCESS;
  }

  //
  // Can't transfer in one TRB.
  // Seperate it in every ONE_TRB_SIZE of TRB
  //
  TrbBuffer = BufferPtr;
  TrbIndex = 0;
  while (size > ONE_TRB_SIZE) {
    TrbCtrlChain = 1;
    TrbCtrlLast = 0;
    DwcXdciCorePrepareOneTrb (Trb, TrbCtrl, TrbCtrlLast, TrbCtrlChain, TrbBuffer, ONE_TRB_SIZE);
    TrbBuffer += ONE_TRB_SIZE;
    size -= ONE_TRB_SIZE;
    Trb++;
    TrbIndex++;
    if (TrbIndex >= DWC_XDCI_TRB_NUM) {
      return EFI_OUT_OF_RESOURCES;
    }
  }
  TrbCtrlChain = 0;
  TrbCtrlLast = 1;
  DwcXdciCorePrepareOneTrb (Trb, TrbCtrl, TrbCtrlLast, TrbCtrlChain, TrbBuffer, size);

  return EFI_SUCCESS;
}


/**
  Internal function:
  This function is used to start a SETUP phase on control endpoint.
  @param[in] CoreHandle  xDCI controller handle address

**/
STATIC
EFI_STATUS
DwcXdciCoreStartEp0SetupXfer (
  IN XDCI_CORE_HANDLE    *CoreHandle
  )
{
  DWC_XDCI_ENDPOINT_CMD_PARAMS    EpCmdParams;
  EFI_STATUS                      Status;
  DWC_XDCI_TRB                    *Trb;

  Status = EFI_DEVICE_ERROR;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreStartEp0SetupXfer: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  if (CoreHandle->EpHandles[0].State == USB_EP_STATE_SETUP) {
    DEBUG ((DEBUG_INFO, "EP0 was already in SETUP phase\n"));
    return EFI_SUCCESS;
  }

  CoreHandle->EpHandles[0].State = USB_EP_STATE_SETUP;
  Trb = CoreHandle->Trbs;
  DEBUG ((DEBUG_INFO, "(DwcXdciCoreStartEp0SetupXfer)\n"));

  Status = DwcXdciCoreInitTrb (
             CoreHandle,
             Trb,
             TRBCTL_SETUP,
             CoreHandle->AlignedSetupBuffer,
             8
             );

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Issue a DEPSTRTXFER for EP0
  // Reset params
  //
  EpCmdParams.Param0 = EpCmdParams.Param1 = EpCmdParams.Param2 = 0;

  //
  // Init the lower re-bits for TRB address
  //
  EpCmdParams.Param1 = (UINT32) (UINTN)Trb;

  //
  // Issue the command to start transfer on physical
  // endpoint 0
  //
  Status = DwcXdciCoreIssueEpCmd (
             CoreHandle,
             0,
             EPCMD_START_XFER,
             &EpCmdParams
             );

  //
  // Save new resource index for this transfer
  //
  CoreHandle->EpHandles[0].CurrentXferRscIdx = ((UsbRegRead (
        CoreHandle->BaseAddress,
        DWC_XDCI_EPCMD_REG (0)) & DWC_XDCI_EPCMD_RES_IDX_MASK) >> DWC_XDCI_EPCMD_RES_IDX_BIT_POS
      );

  return Status;
}


/**
  Internal function:
  This function is used to process the state change event.
  @param[in] CoreHandle  xDCI controller handle address
  @param[in] Event       device event dword

**/
STATIC
EFI_STATUS
DwcXdciProcessDeviceStateChangeEvent (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              Event
  )
{
  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessDeviceStateChangeEvent: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  CoreHandle->HirdVal = (Event & DWC_XDCI_EVENT_BUFF_DEV_HIRD_MASK) >> DWC_XDCI_EVENT_BUFF_DEV_HIRD_BIT_POS;

  CoreHandle->LinkState = ((Event & DWC_XDCI_EVENT_BUFF_DEV_LINK_STATE_MASK) >>
                           DWC_XDCI_EVENT_BUFF_DEV_LINK_STATE_BIT_POS);

  if (CoreHandle->EventCallbacks.DevLinkStateCallback) {
    CoreHandle->EventCallbacks.CbEventParams.ParentHandle = CoreHandle->ParentHandle;
    CoreHandle->EventCallbacks.CbEventParams.LinkState = CoreHandle->LinkState;
    CoreHandle->EventCallbacks.CbEventParams.Hird = CoreHandle->HirdVal;
    CoreHandle->EventCallbacks.CbEventParams.SsEvent = (Event & DWC_XDCI_EVENT_BUFF_DEV_SS_EVENT_MASK) != 0 ? 1 : 0;
    CoreHandle->EventCallbacks.DevLinkStateCallback (&CoreHandle->EventCallbacks.CbEventParams);
  }

  return EFI_SUCCESS;
}


/**
  Internal function:
  This function is used to issue a command to end transfer.
  @param[in] CoreHandle  xDCI controller handle address
  @param[in] EpNum       Physical EP num for which transfer is to be ended

**/
STATIC
EFI_STATUS
DwcXdciEndXfer (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              EpNum
  )
{
  EFI_STATUS                      Status;
  DWC_XDCI_ENDPOINT_CMD_PARAMS    EpCmdParams;
  UINT32                          CmdParams;
  DWC_XDCI_TRB                    *TrbPtr;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciEndXfer: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  CoreHandle->EpHandles[EpNum].CheckFlag = FALSE;

  //
  // Issue a DEPENDXFER for EP
  // Reset params
  //
  EpCmdParams.Param0 = EpCmdParams.Param1 = EpCmdParams.Param2 = 0;

  CmdParams = ((CoreHandle->EpHandles[EpNum].CurrentXferRscIdx << DWC_XDCI_EPCMD_RES_IDX_BIT_POS) |
               DWC_XDCI_EPCMD_FORCE_RM_MASK);

  if (CoreHandle->EpHandles[EpNum].CurrentXferRscIdx == 0) {
    return EFI_SUCCESS;
  }
  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             CoreHandle,
             EpNum,
             CmdParams | DWC_XDCI_EPCMD_END_XFER,
             &EpCmdParams
             );

  if (Status == EFI_SUCCESS) {
    CoreHandle->EpHandles[EpNum].CurrentXferRscIdx = 0;
    TrbPtr = CoreHandle->Trbs + (EpNum * DWC_XDCI_TRB_NUM);
    ZeroMem (TrbPtr, DWC_XDCI_TRB_NUM * sizeof (DWC_XDCI_TRB));
  }

  return Status;
}


/**
  Internal function:
  This function is used to process bus reset detection event.
  @param[in] CoreHandle  xDCI controller handle address

**/
STATIC
EFI_STATUS
DwcXdciProcessDeviceResetDet (
  IN XDCI_CORE_HANDLE    *CoreHandle
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  if (CoreHandle == NULL) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Flush all FIFOs
  //
  Status = DwcXdciCoreFlushAllFifos (CoreHandle);
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciProcessDeviceResetDet: Failed to flush FIFOs\n"));
  }

  //
  // Start SETUP phase on EP0
  //
  Status = DwcXdciCoreStartEp0SetupXfer (CoreHandle);

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciProcessDeviceResetDet: Failed to start SETUP phase for EP0\n"));
    return Status;
  }

  //
  // Notify upper layer if a callback is registerd for
  //  this event
  //
  if (CoreHandle->EventCallbacks.DevBusResetCallback) {
    CoreHandle->EventCallbacks.CbEventParams.ParentHandle = CoreHandle->ParentHandle;
    Status = CoreHandle->EventCallbacks.DevBusResetCallback (&CoreHandle->EventCallbacks.CbEventParams);
  }

  return Status;
}


/**
  Internal function:
  This function is used to process connection done (means reset
  complete) event.
  @param[in] CoreHandle  xDCI controller handle address

**/
STATIC
EFI_STATUS
DwcXdciProcessDeviceResetDone (
  IN XDCI_CORE_HANDLE    *CoreHandle
  )
{
  DWC_XDCI_ENDPOINT_CMD_PARAMS    EpCmdParams;
  UINT32                          BaseAddr;
  EFI_STATUS                      Status;

  Status = EFI_SUCCESS;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessDeviceResetDone: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = CoreHandle->BaseAddress;
  CoreHandle->ActualSpeed = (UsbRegRead (BaseAddr, DWC_XDCI_DSTS_REG) & DWC_XDCI_DSTS_CONN_SPEED_MASK);
  DEBUG ((DEBUG_INFO, "DwcXdciProcessDeviceResetDone CoreHandle->ActualSpeed is %x\n", CoreHandle->ActualSpeed));

  //
  // Program MPS Based on the negotiated Speed
  //
  DwcXdciCoreGetCtrlMps (CoreHandle, &CoreHandle->EpHandles[0].EpInfo.MaxPktSize);
  DwcXdciCoreGetCtrlMps (CoreHandle, &CoreHandle->EpHandles[1].EpInfo.MaxPktSize);

  //
  // Init DEPCFG cmd params for EP0
  //
  Status = DwcXdciCoreInitEpCmdParams (
             CoreHandle,
             &CoreHandle->EpHandles[0].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_MDFY_STATE,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             CoreHandle,
             0,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Init DEPCFG cmd params for EP1
  //
  Status = DwcXdciCoreInitEpCmdParams (
             CoreHandle,
             &CoreHandle->EpHandles[1].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_MDFY_STATE,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             CoreHandle,
             1,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  //
  // Put the other PHY into suspend
  //
  if (CoreHandle->ActualSpeed == USB_SPEED_SUPER) {
    //
    // Put HS PHY to suspend
    //
    UsbRegWrite (
      BaseAddr,
      DWC_XDCI_GUSB2PHYCFG_REG (0),
      (UsbRegRead (BaseAddr, DWC_XDCI_GUSB2PHYCFG_REG (0)) | DWC_XDCI_GUSB2PHYCFG_SUSPEND_PHY_MASK)
      );

    //
    // Clear SS PHY's suspend mask
    //
    UsbRegWrite (
      BaseAddr,
      DWC_XDCI_GUSB3PIPECTL_REG (0),
      (UsbRegRead (BaseAddr, DWC_XDCI_GUSB3PIPECTL_REG (0)) & ~DWC_XDCI_GUSB3PIPECTL_SUSPEND_PHY_MASK)
      );

  } else {
    //
    // Put SS PHY to suspend
    //
    UsbRegWrite (
      BaseAddr,
      DWC_XDCI_GUSB3PIPECTL_REG (0),
      (UsbRegRead (BaseAddr, DWC_XDCI_GUSB3PIPECTL_REG (0)) | DWC_XDCI_GUSB3PIPECTL_SUSPEND_PHY_MASK)
      );

    //
    // Clear HS PHY's suspend mask
    //
    UsbRegWrite (
      BaseAddr,
      DWC_XDCI_GUSB2PHYCFG_REG (0),
      (UsbRegRead (BaseAddr, DWC_XDCI_GUSB2PHYCFG_REG (0)) & ~DWC_XDCI_GUSB2PHYCFG_SUSPEND_PHY_MASK)
      );
  }

  //
  // Notify upper layer if callback is registered
  //
  if (CoreHandle->EventCallbacks.DevResetDoneCallback) {
    CoreHandle->EventCallbacks.CbEventParams.ParentHandle = CoreHandle->ParentHandle;
    CoreHandle->EventCallbacks.CbEventParams.Speed = CoreHandle->ActualSpeed;
    CoreHandle->EventCallbacks.DevResetDoneCallback (&CoreHandle->EventCallbacks.CbEventParams);
  }

  return Status;
}


/**
  Internal function:
  This function is used to process device event.
  @param[in] CoreHandle              xDCI controller handle address
  @param[in] IntLineEventBuffer      event Buffer pointing to device event
  @param[in] ProcessedEventSize      address of variable to save the size of
  the event that was Processed

**/
STATIC
EFI_STATUS
DwcXdciProcessDeviceEvent (
  IN XDCI_CORE_HANDLE         *CoreHandle,
  IN DWC_XDCI_EVENT_BUFFER    *IntLineEventBuffer,
  IN UINT32                   *ProcessedEventSize
  )
{
  UINT32 Event;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessDeviceEvent: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Extract device Event
  //
  Event = (IntLineEventBuffer->Event & DWC_XDCI_EVENT_BUFF_DEV_EVT_MASK);
  Event >>= DWC_XDCI_EVENT_BUFF_DEV_EVT_BIT_POS;

  //
  // Assume default Event size. Change it in switch case if
  //  different
  //
  *ProcessedEventSize = DWC_XDCI_DEV_EVENT_DEFAULT_SIZE_IN_BYTES;

  switch (Event) {
  case DWC_XDCI_EVENT_BUFF_DEV_DISCONN_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_DISCONN_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_USB_RESET_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_USB_RESET_EVENT\n"));
    DwcXdciProcessDeviceResetDet (CoreHandle);
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_CONN_DONE_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_CONN_DONE_EVENT\n"));
    DwcXdciProcessDeviceResetDone (CoreHandle);
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_STATE_CHANGE_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_STATE_CHANGE_EVENT\n"));
    DwcXdciProcessDeviceStateChangeEvent (CoreHandle, IntLineEventBuffer->Event);
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_WKUP_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_WKUP_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_HBRNTN_REQ_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_HBRNTN_REQ_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_SOF_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_SOF_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_ERRATIC_ERR_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_ERRATIC_ERR_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_CMD_CMPLT_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_CMD_CMPLT_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_BUFF_OVFL_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_BUFF_OVFL_EVENT\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_DEV_TST_LMP_RX_EVENT:
    DEBUG ((DEBUG_INFO, "Device DWC_XDCI_EVENT_BUFF_DEV_TST_LMP_RX_EVENT\n"));
    *ProcessedEventSize = DWC_XDCI_DEV_EVENT_TST_LMP_SIZE_IN_BYTES;
    break;

  default:
    DEBUG ((DEBUG_INFO, "DwcXdciProcessDeviceEvent: UNHANDLED device Event: %x\n", Event));
    break;
  }

  return EFI_SUCCESS;
}


/**
  Internal function:
  This function is used to process EP not ready for
  non-control endpoints.
  @param[in] CoreHandle  xDCI controller handle address
  @param[in] EpNum       Physical endpoint number

**/
STATIC
EFI_STATUS
DwcXdciProcessEpXferNotReady (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              EpNum
  )
{
  //
  // TODO: Not doing on-demand transfers
  // Revisit if required for later use
  //
  return EFI_SUCCESS;
}


/**
  Internal function:
  This function is used to process EP not ready for
  control endpoints.
  @param[in] CoreHandle      xDCI controller handle address
  @param[in] EpNum           Physical endpoint number
  @param[in] epEventStatus

**/
STATIC
EFI_STATUS
DwcXdciProcessEp0XferNotReady (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              EpNum,
  IN UINT32              epEventStatus
  )
{
  USB_EP_STATE        EpState;

  EpState = USB_EP_STATE_SETUP;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessEp0XferNotReady: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }
  //
  // Is it data stage or Status stage
  //
  if ((epEventStatus & DWC_XDCI_EVENT_BUFF_EP_CTRL_DATA_REQ_MASK) != 0) {
    EpState = USB_EP_STATE_DATA;
  } else if ((epEventStatus & DWC_XDCI_EVENT_BUFF_EP_CTRL_STATUS_REQ_MASK) != 0) {
    EpState = USB_EP_STATE_STATUS;
  }

  if ((EpNum == 0) && (EpState == USB_EP_STATE_STATUS)) {
    if ((epEventStatus & DWC_XDCI_EVENT_BUFF_EP_XFER_ACTIVE_MASK) != 0) {
      DEBUG ((DEBUG_INFO, "XFER_ACTIVE\n"));
    } else {
      DEBUG ((DEBUG_INFO, "XFER_NOT_ACTIVE\n"));
    }
    DwcXdciEp0ReceiveStatusPkt (CoreHandle);
  }

  //
  // Notify upper layer if a callback is registered for
  // this event
  //
  if (CoreHandle->EventCallbacks.DevXferNrdyCallback) {
    CoreHandle->EventCallbacks.CbEventParams.ParentHandle = CoreHandle->ParentHandle;
    CoreHandle->EventCallbacks.CbEventParams.EpState = EpState;
    CoreHandle->EventCallbacks.DevXferNrdyCallback (&CoreHandle->EventCallbacks.CbEventParams);
  }

  return EFI_SUCCESS;
}


/**
  Internal function:
  This function is used to process transfer phone done for EP0.
  @param[in] CoreHandle  xDCI controller handle address
  @param[in] EpNum       Physical endpoint number (0 for OUT and 1 for IN)

**/
STATIC
EFI_STATUS
DwcXdciProcessEp0XferPhaseDone (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              EpNum
  )
{
  DWC_XDCI_ENDPOINT    *EpHandle;
  DWC_XDCI_TRB         *Trb;
  EFI_STATUS           Status;
  UINT32               TrbSts;
  UINT32               TrbCtrl;
  UINT32               TrbBufsize;

  Status = EFI_SUCCESS;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessEp0XferPhaseDone: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  EpHandle = &CoreHandle->EpHandles[EpNum];
  Trb = CoreHandle->Trbs + (EpNum * DWC_XDCI_TRB_NUM);
  DEBUG ((DEBUG_INFO, "(DwcXdciProcessEp0XferPhaseDone)EpNum is %d\n", EpNum));

  if (Trb->TrbCtrl & DWC_XDCI_TRB_CTRL_HWO_MASK) {
    DEBUG ((DEBUG_INFO, "DwcXdciProcessEp0XferPhaseDone. HW owns TRB: %x!!!\n", (UINT32) (UINTN)Trb));
  }

  EpHandle->CurrentXferRscIdx = 0;
  EpHandle->State = USB_EP_STATE_ENABLED;
  TrbCtrl = (Trb->TrbCtrl & DWC_XDCI_TRB_CTRL_TYPE_MASK) >> DWC_XDCI_TRB_CTRL_TYPE_BIT_POS;
  TrbSts = (Trb->LenXferParams & DWC_XDCI_TRB_STATUS_MASK) >> DWC_XDCI_TRB_STATUS_BIT_POS;
  TrbBufsize = Trb->LenXferParams & DWC_XDCI_TRB_BUFF_SIZE_MASK;

  switch (TrbCtrl) {
  case DWC_XDCI_TRB_CTRL_TYPE_SETUP:
    DEBUG ((DEBUG_INFO, "SETUP\n"));
    if (CoreHandle->EventCallbacks.DevSetupPktReceivedCallback) {
      CoreHandle->EventCallbacks.CbEventParams.ParentHandle = CoreHandle->ParentHandle;
      CoreHandle->EventCallbacks.CbEventParams.Buffer = CoreHandle->AlignedSetupBuffer;
      Status = CoreHandle->EventCallbacks.DevSetupPktReceivedCallback (&CoreHandle->EventCallbacks.CbEventParams);
    }

    if (! (CoreHandle->AlignedSetupBuffer[0] & USB_SETUP_DATA_PHASE_DIRECTION_MASK)) {
      //
      // Keep a Buffer ready for setup phase
      //
      DwcXdciCoreStartEp0SetupXfer (CoreHandle);
    }

    break;

  case DWC_XDCI_TRB_CTRL_TYPE_STATUS2:
    DEBUG ((DEBUG_INFO, "STATUS2\n"));
    break;

  case DWC_XDCI_TRB_CTRL_TYPE_STATUS3:
    DEBUG ((DEBUG_INFO, "STATUS3\n"));
    //
    // Notify upper layer of control transfer completion
    // if a callback function was registerd
    //
    if (CoreHandle->EventCallbacks.DevXferDoneCallback) {
      CoreHandle->EventCallbacks.CbEventParams.ParentHandle = CoreHandle->ParentHandle;
      CoreHandle->EventCallbacks.CbEventParams.EpNum = (EpNum >> 1);
      CoreHandle->EventCallbacks.CbEventParams.EpDir = (EpNum & 1);
      CoreHandle->EventCallbacks.CbEventParams.Buffer = (UINT8 *) (UINTN) (Trb->BuffPtrLow);
      CoreHandle->EventCallbacks.DevXferDoneCallback (&CoreHandle->EventCallbacks.CbEventParams);
    }

    //
    // Status phase done. Queue next SETUP packet
    //
    Status = DwcXdciCoreStartEp0SetupXfer (CoreHandle);

    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "DwcXdciProcessEp0XferPhaseDone: FAILED to queue SETUP\n"));
    }
    break;

  case DWC_XDCI_TRB_CTRL_TYPE_DATA:
    DEBUG ((DEBUG_INFO, "DATA\n"));
    if (TrbSts == DWC_XDCI_TRB_STATUS_SETUP_PENDING || TrbBufsize != 0) {
      DEBUG ((DEBUG_INFO, "ERROR: Control transfert aborted by host: Setup pending\n"));
      DwcXdciCoreStartEp0SetupXfer (CoreHandle);
    }

    if (CoreHandle->EventCallbacks.DevXferDoneCallback) {
      CoreHandle->EventCallbacks.CbEventParams.ParentHandle = CoreHandle->ParentHandle;
      CoreHandle->EventCallbacks.CbEventParams.EpNum = (EpNum >> 1);
      CoreHandle->EventCallbacks.CbEventParams.EpDir = (EpNum & 1);
      CoreHandle->EventCallbacks.CbEventParams.Buffer = (UINT8 *) (UINTN) (Trb->BuffPtrLow);
      CoreHandle->EventCallbacks.DevXferDoneCallback (&CoreHandle->EventCallbacks.CbEventParams);
    }
    break;

  default:
    DEBUG ((DEBUG_INFO, "DwcXdciProcessEp0XferPhaseDone: UNHANDLED STATE in TRB\n"));
    break;
  }

  return Status;
}


/**
  Internal function:
  This function is used to process transfer done for
  non-control endpoints.
  @param[in] CoreHandle  xDCI controller handle address
  @param[in] EpNum       Physical endpoint number

**/
STATIC
EFI_STATUS
DwcXdciProcessEpXferDone (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              EpNum
  )
{
  DWC_XDCI_ENDPOINT    *EpHandle;
  DWC_XDCI_TRB         *Trb;
  USB_XFER_REQUEST     *XferReq;
  UINT32               RemainingLen;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessEpXferDone: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  EpHandle = &CoreHandle->EpHandles[EpNum];
  EpHandle->CurrentXferRscIdx = 0;
  Trb = EpHandle->Trb;
  XferReq = &EpHandle->XferHandle;

  //
  // if transfer done, set CheckFlag to FALSE for allow next transfer request.
  //
  EpHandle->CheckFlag = FALSE;

  if ((Trb == NULL) || (XferReq == NULL)) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessEpXferDone: INVALID parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Compute the actual transfer length
  //
  XferReq->ActualXferLen = XferReq->XferLen;
  RemainingLen = (Trb->LenXferParams & DWC_XDCI_TRB_BUFF_SIZE_MASK);

  if (RemainingLen > XferReq->XferLen) {
    //
    // Buffer overrun? This should never happen
    //
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessEpXferDone: Possible Buffer overrun\n"));
  } else {
    XferReq->ActualXferLen -= RemainingLen;
  }

  //
  // Notify upper layer of request-specific transfer completion
  // if there is a callback specifically for this request
  //
  if (XferReq->XferDone) {
    XferReq->XferDone (CoreHandle->ParentHandle, XferReq);
  }

  //
  // Notify upper layer if a callback was registered
  //
  if (CoreHandle->EventCallbacks.DevXferDoneCallback) {
    CoreHandle->EventCallbacks.CbEventParams.ParentHandle = CoreHandle->ParentHandle;
    CoreHandle->EventCallbacks.CbEventParams.EpNum = (EpNum >> 1);
    CoreHandle->EventCallbacks.CbEventParams.EpDir = (EpNum & 1);
    CoreHandle->EventCallbacks.CbEventParams.EpType = EpHandle->EpInfo.EpType;
    CoreHandle->EventCallbacks.CbEventParams.Buffer = (UINT8 *) (UINTN) (EpHandle->Trb->BuffPtrLow);
    CoreHandle->EventCallbacks.DevXferDoneCallback (&CoreHandle->EventCallbacks.CbEventParams);
  }

  return EFI_SUCCESS;
}


/**
  Internal function:
  This function is used to process endpoint events.
  @param[in] CoreHandle           xDCI controller handle address
  @param[in] IntLineEventBuffer   address of Buffer containing event
  to process
  @param[in] ProcessedEventSize   address to save the size of event
  Processed

**/
STATIC
EFI_STATUS
DwcXdciProcessEpEvent (
  IN XDCI_CORE_HANDLE         *CoreHandle,
  IN DWC_XDCI_EVENT_BUFFER    *IntLineEventBuffer,
  IN UINT32                   *ProcessedEventSize
  )
{
  UINT32          EpNum;
  UINT32          EpEvent;
  UINT32          EpEventStatus;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessEpEvent: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  EpEvent = IntLineEventBuffer->Event;

  *ProcessedEventSize = DWC_XDCI_DEV_EVENT_DEFAULT_SIZE_IN_BYTES;

  //
  // Get EP num
  //
  EpNum = ((EpEvent & DWC_XDCI_EVENT_BUFF_EP_NUM_MASK) >> DWC_XDCI_EVENT_BUFF_EP_NUM_BIT_POS);
  EpEventStatus = (EpEvent & DWC_XDCI_EVENT_BUFF_EP_EVENT_STATUS_MASK);

  //
  // Interpret event and handle transfer completion here
  //
  EpEvent = ((EpEvent & DWC_XDCI_EVENT_BUFF_EP_EVENT_MASK) >> DWC_XDCI_EVENT_BUFF_EP_EVENT_BIT_POS);

  switch (EpEvent) {
  case DWC_XDCI_EVENT_BUFF_EP_XFER_CMPLT:
    DEBUG ((DEBUG_INFO, "XFER_CMPLT ep %d\n", EpNum));
    if (EpNum > 1) {
      DwcXdciProcessEpXferDone (CoreHandle, EpNum);
    } else {
      DwcXdciProcessEp0XferPhaseDone (CoreHandle, EpNum);
    }
    break;

  case DWC_XDCI_EVENT_BUFF_EP_XFER_IN_PROGRESS:
    DEBUG ((DEBUG_INFO, "IN_PROGRESS\n"));
    break;

  case DWC_XDCI_EVENT_BUFF_EP_XFER_NOT_READY:
    DEBUG ((DEBUG_INFO, "NOT_READY ep %d\n", EpNum));
    if (EpNum > 1) {
      //
      // Endpoint transfer is not ready
      //
      DwcXdciProcessEpXferNotReady (CoreHandle, EpNum);
    } else {
      DwcXdciProcessEp0XferNotReady (CoreHandle, EpNum, EpEventStatus);
    }
    break;

  default:
    DEBUG ((DEBUG_INFO, "DwcXdciProcessEpEvent: UNKNOWN EP event\n"));
    break;
  }

  return EFI_SUCCESS;
}


/**
  Internal function:
  This function is used to process events on single interrupt line.
  @param[in] CoreHandle             xDCI controller handle address
  @param[in] eventCount             event bytes to process
  @param[in] ProcessedEventCount    address to save the size
  (in bytes) of event Processed
  Processed

**/
STATIC
EFI_STATUS
DwcXdciProcessInterruptLineEvents (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              eventCount,
  IN UINT32              *ProcessedEventCount
  )
{
  UINT32    ProcessedEventSize;
  UINT32    CurrentEventAddr;

  ProcessedEventSize = 0;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessInterruptLineEvents: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  if (CoreHandle->CurrentEventBuffer == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciProcessInterruptLineEvents: INVALID event Buffer\n"));
    return EFI_INVALID_PARAMETER;
  }

  CurrentEventAddr = (UINT32) (UINTN) (CoreHandle->CurrentEventBuffer);

  //
  // Process eventCount/eventSize number of events
  // in this run
  //
  while (eventCount != 0) {
    if (CoreHandle->CurrentEventBuffer->Event & DWC_XDCI_EVENT_DEV_MASK) {
      DwcXdciProcessDeviceEvent (
        CoreHandle,
        CoreHandle->CurrentEventBuffer,
        &ProcessedEventSize
        );
    } else {
      DwcXdciProcessEpEvent (
        CoreHandle,
        CoreHandle->CurrentEventBuffer,
        &ProcessedEventSize);
    }

    eventCount -= ProcessedEventSize;
    *ProcessedEventCount += ProcessedEventSize;
    if ((CurrentEventAddr + ProcessedEventSize) >=
        ((UINT32) (UINTN) (CoreHandle->AlignedEventBuffers) + (sizeof (DWC_XDCI_EVENT_BUFFER) * DWC_XDCI_MAX_EVENTS_PER_BUFFER))
        ) {
      CurrentEventAddr = (UINT32) (UINTN) (CoreHandle->AlignedEventBuffers);
      DEBUG ((DEBUG_INFO, "DwcXdciProcessInterruptLineEvents: Event Buffer bound reached\n"));
    } else {
      CurrentEventAddr += ProcessedEventSize;
    }

    CoreHandle->CurrentEventBuffer = (DWC_XDCI_EVENT_BUFFER *) (UINTN)CurrentEventAddr;
  }

  return EFI_SUCCESS;
}

//
// DWC XDCI APIs
//

/**
  Interface:

  This function is used to initialize the xDCI core.
  @param[in] ConfigParams    Parameters from app to configure the core
  @param[in] deviceCorePtr   HW-independent APIs handle for device core
  @param[in] CoreHandle      xDCI controller handle retured

**/
EFI_STATUS
EFIAPI
DwcXdciCoreInit (
  IN USB_DEV_CONFIG_PARAMS    *ConfigParams,
  IN VOID                     *deviceCorePtr,
  IN VOID                     **CoreHandle
  )
{
  EFI_STATUS                      Status;
  UINT32                          BaseAddr;
  XDCI_CORE_HANDLE                *LocalCoreHandle;
  DWC_XDCI_ENDPOINT_CMD_PARAMS    EpCmdParams;
  UINT32                          MaxDelayIter;
  UINT8                           Idx;

  Status = EFI_DEVICE_ERROR;
  MaxDelayIter = DWC_XDCI_MAX_DELAY_ITERATIONS;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)AllocateZeroPool (sizeof (XDCI_CORE_HANDLE));

  if (CoreHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (LocalCoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to allocate handle for xDCI\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (LocalCoreHandle, sizeof (XDCI_CORE_HANDLE));

  LocalCoreHandle->ParentHandle = deviceCorePtr;

  *CoreHandle = (VOID *)LocalCoreHandle;

  LocalCoreHandle->Id = ConfigParams->ControllerId;
  LocalCoreHandle->BaseAddress = BaseAddr = ConfigParams->BaseAddress;
  LocalCoreHandle->Flags = ConfigParams->Flags;
  LocalCoreHandle->DesiredSpeed = LocalCoreHandle->ActualSpeed = ConfigParams->Speed;
  LocalCoreHandle->Role = ConfigParams->Role;

  DEBUG ((DEBUG_INFO, "Resetting the USB core\n"));
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCTL_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) | DWC_XDCI_DCTL_CSFTRST_MASK
    );
  //
  // Wait until core soft reset completes
  //
  do {
    if ((UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) & DWC_XDCI_DCTL_CSFTRST_MASK) == 0) {
      break;
    } else {
      gBS->Stall (DWC_XDCI_MAX_DELAY_ITERATIONS);
    }
  } while ((--MaxDelayIter) != 0);

  if (MaxDelayIter == 0) {
    DEBUG ((DEBUG_INFO, "Failed to reset device controller\n"));
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((DEBUG_INFO, "USB core has been reset\n"));

  //
  // All FIFOs are flushed at this point
  //
  //
  // Ensure we have EP0 Rx/Tx handles initialized
  //
  LocalCoreHandle->EpHandles[0].EpInfo.EpNum = 0;
  LocalCoreHandle->EpHandles[0].EpInfo.EpDir = UsbEpDirOut;
  LocalCoreHandle->EpHandles[0].EpInfo.EpType = USB_ENDPOINT_CONTROL;
  LocalCoreHandle->EpHandles[0].EpInfo.MaxPktSize = DWC_XDCI_SS_CTRL_EP_MPS;
  //
  // 0 means burst size of 1
  //
  LocalCoreHandle->EpHandles[0].EpInfo.BurstSize = 0;

  LocalCoreHandle->EpHandles[1].EpInfo.EpNum = 0;
  LocalCoreHandle->EpHandles[1].EpInfo.EpDir = UsbEpDirIn;
  LocalCoreHandle->EpHandles[1].EpInfo.EpType = USB_ENDPOINT_CONTROL;
  LocalCoreHandle->EpHandles[1].EpInfo.MaxPktSize = DWC_XDCI_SS_CTRL_EP_MPS;
  //
  // 0 means burst size of 1
  //
  LocalCoreHandle->EpHandles[1].EpInfo.BurstSize = 0;

  LocalCoreHandle->DevState = UsbDevStateDefault;

  //
  // Clear KeepConnect bit so we can allow disconnect and
  // re-connect. Stay in RX_DETECT state
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCTL_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) &
    (~DWC_XDCI_DCTL_KEEP_CONNECT_MASK) &
    ((~DWC_XDCI_DCTL_STATE_CHANGE_REQ_MASK) | (DWC_XDCI_DCTL_STATE_CHANGE_REQ_RX_DETECT <<
        DWC_XDCI_DCTL_STATE_CHANGE_REQ_BIT_POS))
    );

  DEBUG ((DEBUG_INFO, "Device controller Synopsys ID: %x\n", UsbRegRead (BaseAddr, DWC_XDCI_GSNPSID_REG)));
  DEBUG ((DEBUG_INFO, "Default value of xDCI GSBUSCFG0 and GSBUSCFG1: %x, %x\n",
          UsbRegRead (BaseAddr, DWC_XDCI_GSBUSCFG0_REG),
          UsbRegRead (BaseAddr, DWC_XDCI_GSBUSCFG1_REG)));

  DEBUG ((DEBUG_INFO, "Default value of xDCI GTXTHRCFG and GRXTHRCFG: %x, %x\n",
          UsbRegRead (BaseAddr, DWC_XDCI_GTXTHRCFG_REG),
          UsbRegRead (BaseAddr, DWC_XDCI_GRXTHRCFG_REG)));

  //
  // Clear ULPI auto-resume bit
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GUSB2PHYCFG_REG (0),
    (UsbRegRead (BaseAddr, DWC_XDCI_GUSB2PHYCFG_REG (0)) & ~DWC_XDCI_GUSB2PHYCFG_ULPI_AUTO_RESUME_MASK)
    );

  DEBUG ((DEBUG_INFO, "Default value of xDCI GUSB2PHYCFG and GUSB3PIPECTL: %x, %x\n",
          UsbRegRead (BaseAddr, DWC_XDCI_GUSB2PHYCFG_REG (0)),
          UsbRegRead (BaseAddr, DWC_XDCI_GUSB3PIPECTL_REG (0))));
  //
  // Only one RxFIFO
  //
  DEBUG ((DEBUG_INFO, "Default value of DWC_XDCI_GRXFIFOSIZ: %x\n",
          UsbRegRead (BaseAddr, DWC_XDCI_GRXFIFOSIZ_REG (0))));

  for (Idx = 0; Idx < DWC_XDCI_MAX_ENDPOINTS; Idx++) {
    DEBUG ((DEBUG_INFO, "Default value of xDCI DWC_XDCI_GTXFIFOSIZ %d: %x\n",
            Idx, UsbRegRead (BaseAddr, DWC_XDCI_GTXFIFOSIZ_REG (Idx))));
  }

  //
  // TODO: Need to check if TxFIFO should start where RxFIFO ends
  // or default is correct i.e. TxFIFO starts at 0 just like RxFIFO
  //

  //
  // Allocate and Initialize Event Buffers
  //
  LocalCoreHandle->MaxDevIntLines = ((UsbRegRead (BaseAddr, DWC_XDCI_GHWPARAMS1_REG) &
                                      DWC_XDCI_GHWPARAMS1_NUM_INT_MASK) >>
                                     DWC_XDCI_GHWPARAMS1_NUM_INT_BIT_POS);

  DEBUG ((DEBUG_INFO, "Max dev int lines: %d\n", LocalCoreHandle->MaxDevIntLines));
  //
  // One event Buffer per interrupt line.
  //  Need to align it to size of event Buffer
  //  Buffer needs to be big enough. Otherwise the core
  //  won't operate
  //
  LocalCoreHandle->AlignedEventBuffers = (DWC_XDCI_EVENT_BUFFER *)
                                         ((UINT32) (UINTN) (LocalCoreHandle->EventBuffers) +
                                          ((sizeof (DWC_XDCI_EVENT_BUFFER) * DWC_XDCI_MAX_EVENTS_PER_BUFFER) -
                                              (((UINT32) (UINTN) (LocalCoreHandle->EventBuffers)) %
                                                  (sizeof (DWC_XDCI_EVENT_BUFFER) * DWC_XDCI_MAX_EVENTS_PER_BUFFER))));

  for (Idx = 0; Idx < LocalCoreHandle->MaxDevIntLines; Idx++) {
    UsbRegWrite (
      BaseAddr,
      DWC_XDCI_GEVNTADR_REG (Idx),
      (UINT32) (UINTN) (LocalCoreHandle->AlignedEventBuffers + Idx * sizeof (DWC_XDCI_EVENT_BUFFER) *
                        DWC_XDCI_MAX_EVENTS_PER_BUFFER)
      );

    //
    // Clear High 32bit address register, GEVNTADR register is 64-bit register
    // default is 0xffffffffffffffff
    //
    UsbRegWrite (BaseAddr, DWC_XDCI_GEVNTADR_REG (Idx) + 4, 0x00000000);

    LocalCoreHandle->CurrentEventBuffer = LocalCoreHandle->AlignedEventBuffers;
    //
    // Write size and clear the mask
    //
    UsbRegWrite (
      BaseAddr,
      DWC_XDCI_EVNTSIZ_REG (Idx),
      sizeof (DWC_XDCI_EVENT_BUFFER) * DWC_XDCI_MAX_EVENTS_PER_BUFFER
      );

    //
    // Write 0 to the event count register as the last step
    //
    //  for event configuration
    //
    UsbRegWrite (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (Idx), 0);

    DEBUG ((DEBUG_INFO, "Value of xDCI Event Buffer %d: %x, Size: %x, Count: %x\n",
            Idx,
            UsbRegRead (BaseAddr, DWC_XDCI_GEVNTADR_REG (Idx)),
            UsbRegRead (BaseAddr, DWC_XDCI_EVNTSIZ_REG (Idx)),
            UsbRegRead (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (Idx))));
  }

  //
  // Program Global Control Register to disable scaledown,
  // disable clock gating
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GCTL_REG,
    ((UsbRegRead (BaseAddr, DWC_XDCI_GCTL_REG) &
      ~ (DWC_XDCI_GCTL_SCALE_DOWN_MODE_MASK + DWC_XDCI_GCTL_RAMCLKSEL_MASK + DWC_XDCI_GCTL_DISABLE_SCRAMB_MASK)) |
     DWC_XDCI_GCTL_DISABLE_CLK_GATING_MASK |
     (DWC_XDCI_GCTL_PRT_CAP_DEVICE << DWC_XDCI_GCTL_PRT_CAP_DIR_BIT_POS)));

  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_GCTL_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_GCTL_REG)));

  //
  // TODO: Program desired Speed and set LPM capable
  // We will do this when SuperSpeed works. For now,
  // force into High-Speed mode to aVOID anyone trying this
  // on Super Speed port
  //
#ifdef SUPPORT_SUPER_SPEED
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCFG_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DCFG_REG) & ~DWC_XDCI_DCFG_DESIRED_DEV_SPEED_MASK) | LocalCoreHandle->DesiredSpeed
    );
#else
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCFG_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DCFG_REG) & ~DWC_XDCI_DCFG_DESIRED_DEV_SPEED_MASK) | DWC_XDCI_DCFG_DESIRED_HS_SPEED
  );
#endif

  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_DCFG_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_DCFG_REG)));
  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_DSTS_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_DSTS_REG)));

  //
  // Enable Device Interrupt Events
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DEVTEN_REG,
    DWC_XDCI_DEVTEN_DEVICE_INTS
    );
  //
  // Program the desired role
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GCTL_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_GCTL_REG) & ~DWC_XDCI_GCTL_PRT_CAP_DIR_MASK) | (LocalCoreHandle->Role <<
        DWC_XDCI_GCTL_PRT_CAP_DIR_BIT_POS)
    );
  //
  // Clear USB2 suspend for start new config command
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GUSB2PHYCFG_REG (0),
    (UsbRegRead (BaseAddr, DWC_XDCI_GUSB2PHYCFG_REG (0)) & ~DWC_XDCI_GUSB2PHYCFG_SUSPEND_PHY_MASK)
    );

  //
  // Clear USB3 suspend for start new config command
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GUSB3PIPECTL_REG (0),
    (UsbRegRead (BaseAddr, DWC_XDCI_GUSB3PIPECTL_REG (0)) & ~DWC_XDCI_GUSB3PIPECTL_SUSPEND_PHY_MASK)
    );

  //
  // Issue DEPSTARTCFG command for EP0
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[0].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_NONE,
             EPCMD_START_NEW_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to init params for START_NEW_CONFIG EP command on xDCI\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             0,
             EPCMD_START_NEW_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue START_NEW_CONFIG EP command on xDCI\n"));
    return Status;
  }

  //
  // Issue DEPCFG command for EP0
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[0].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_INIT_STATE,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to init params for SET_EP_CONFIG command on xDCI for EP0\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             0,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams);

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue SET_EP_CONFIG command on xDCI for EP0\n"));
    return Status;
  }

  //
  // Issue DEPCFG command for EP1
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[1].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_INIT_STATE,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to init params for SET_EP_CONFIG command on xDCI for EP1\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             1,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue SET_EP_CONFIG command on xDCI for EP1\n"));
    return Status;
  }

  //
  // Issue DEPXFERCFG command for EP0
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[0].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_NONE,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO,
            "DwcXdciCoreInit: Failed to init params for EPCMD_SET_EP_XFER_RES_CONFIG command on xDCI for EP0\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             0,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue EPCMD_SET_EP_XFER_RES_CONFIG command on xDCI for EP0\n"));
    return Status;
  }

  //
  // Issue DEPXFERCFG command for EP1
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[1].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_NONE,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO,
            "DwcXdciCoreInit: Failed to init params for EPCMD_SET_EP_XFER_RES_CONFIG command on xDCI for EP1\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             1,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue EPCMD_SET_EP_XFER_RES_CONFIG command on xDCI for EP1\n"));
    return Status;
  }

  //
  // Prepare a Buffer for SETUP packet
  //
  LocalCoreHandle->Trbs = (DWC_XDCI_TRB *) (UINTN) ((UINT32) (UINTN)
                          LocalCoreHandle->UnalignedTrbs +
                          (DWC_XDCI_TRB_BYTE_ALIGNMENT -
                           ((UINT32) (UINTN)LocalCoreHandle->UnalignedTrbs %
                            DWC_XDCI_TRB_BYTE_ALIGNMENT)));

  DEBUG ((DEBUG_INFO, "(DwcXdciCoreInit)@@@@@@@@@ unalignedTrbs address is 0x%x\n", LocalCoreHandle->UnalignedTrbs));
  DEBUG ((DEBUG_INFO, "(DwcXdciCoreInit)@@@@@@@@@ TRB address is 0x%x\n", LocalCoreHandle->Trbs));
  //
  // Allocate Setup Buffer that is 8-byte aligned
  //
  LocalCoreHandle->AlignedSetupBuffer = LocalCoreHandle->DefaultSetupBuffer +
                                        (DWC_XDCI_SETUP_BUFF_SIZE -
                                         ((UINT32) (UINTN) (LocalCoreHandle->DefaultSetupBuffer) % DWC_XDCI_SETUP_BUFF_SIZE));

  //
  // Aligned Buffer for Status phase
  //
  LocalCoreHandle->AlignedMiscBuffer = LocalCoreHandle->MiscBuffer +
                                       (DWC_XDCI_SETUP_BUFF_SIZE -
                                        ((UINT32) (UINTN) (LocalCoreHandle->AlignedMiscBuffer) % DWC_XDCI_SETUP_BUFF_SIZE));


  //
  // Enable Physical Endpoints 0
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EP_DALEPENA_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_EP_DALEPENA_REG) | (1 << 0)
    );
  //
  // Enable Physical Endpoints 1
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EP_DALEPENA_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_EP_DALEPENA_REG) | (1 << 1)
    );

  DEBUG ((DEBUG_INFO, "Default value of xDCI DWC_XDCI_DEVTEN_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_DEVTEN_REG)));
  return Status;
}


/**
  Interface:
  This function is used to de-initialize the xDCI core.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] flags       Special flags for de-initializing the core in
  particular way

**/
EFI_STATUS
EFIAPI
DwcXdciCoreDeinit (
  IN VOID      *CoreHandle,
  IN UINT32    flags
  )
{
  FreePool (CoreHandle);
  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to register event callback function.
  @param[in] CoreHandle     xDCI controller handle
  @param[in] Event          Event for which callback is to be registered
  @param[in] CallbackFunc   Callback function to invoke after event occurs

**/
EFI_STATUS
EFIAPI
DwcXdciCoreRegisterCallback (
  IN VOID                      *CoreHandle,
  IN USB_DEVICE_EVENT_ID       Event,
  IN USB_DEVICE_CALLBACK_FUNC  CallbackFunc
  )
{
  XDCI_CORE_HANDLE  *LocalCoreHandle;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (LocalCoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreRegisterCallback: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((DEBUG_INFO, "DwcXdciCoreRegisterCallback: event is %d\n", Event));
  switch (Event) {
  case USB_DEVICE_DISCONNECT_EVENT:
    LocalCoreHandle->EventCallbacks.DevDisconnectCallback = CallbackFunc;
    break;

  case USB_DEVICE_RESET_EVENT:
    LocalCoreHandle->EventCallbacks.DevBusResetCallback = CallbackFunc;
    break;

  case USB_DEVICE_CONNECTION_DONE:
    LocalCoreHandle->EventCallbacks.DevResetDoneCallback = CallbackFunc;
    break;

  case USB_DEVICE_STATE_CHANGE_EVENT:
    LocalCoreHandle->EventCallbacks.DevLinkStateCallback = CallbackFunc;
    break;

  case USB_DEVICE_WAKEUP_EVENT:
    LocalCoreHandle->EventCallbacks.DevWakeupCallback = CallbackFunc;
    break;

  case USB_DEVICE_HIBERNATION_REQ_EVENT:
    LocalCoreHandle->EventCallbacks.DevHibernationCallback = CallbackFunc;
    break;

  case USB_DEVICE_SOF_EVENT:
    LocalCoreHandle->EventCallbacks.DevSofCallback = CallbackFunc;
    break;

  case USB_DEVICE_ERRATIC_ERR_EVENT:
    LocalCoreHandle->EventCallbacks.DevErraticErrCallback = CallbackFunc;
    break;

  case USB_DEVICE_CMD_CMPLT_EVENT:
    LocalCoreHandle->EventCallbacks.DevCmdCmpltCallback = CallbackFunc;
    break;

  case USB_DEVICE_BUFF_OVERFLOW_EVENT:
    LocalCoreHandle->EventCallbacks.DevBuffOvflwCallback = CallbackFunc;
    break;

  case USB_DEVICE_TEST_LMP_RX_EVENT:
    LocalCoreHandle->EventCallbacks.DevTestLmpRxCallback = CallbackFunc;
    break;

  case USB_DEVICE_SETUP_PKT_RECEIVED:
    LocalCoreHandle->EventCallbacks.DevSetupPktReceivedCallback = CallbackFunc;
    break;

  case USB_DEVICE_XFER_NRDY:
    LocalCoreHandle->EventCallbacks.DevXferNrdyCallback = CallbackFunc;
    break;

  case USB_DEVICE_XFER_DONE:
    LocalCoreHandle->EventCallbacks.DevXferDoneCallback = CallbackFunc;
    break;

  default:
    break;
  }

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to unregister event callback function.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] event       Event for which callback function is to be unregistered

**/
EFI_STATUS
EFIAPI
DwcXdciCoreUnregisterCallback (
  IN VOID                   *CoreHandle,
  IN USB_DEVICE_EVENT_ID    event
  )
{
  XDCI_CORE_HANDLE  *LocalCoreHandle;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (LocalCoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreUnregisterCallback: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  switch (event) {
  case USB_DEVICE_DISCONNECT_EVENT:
    LocalCoreHandle->EventCallbacks.DevDisconnectCallback = NULL;
    break;

  case USB_DEVICE_RESET_EVENT:
    LocalCoreHandle->EventCallbacks.DevBusResetCallback = NULL;
    break;

  case USB_DEVICE_CONNECTION_DONE:
    LocalCoreHandle->EventCallbacks.DevResetDoneCallback = NULL;
    break;

  case USB_DEVICE_STATE_CHANGE_EVENT:
    LocalCoreHandle->EventCallbacks.DevLinkStateCallback = NULL;
    break;

  case USB_DEVICE_WAKEUP_EVENT:
    LocalCoreHandle->EventCallbacks.DevWakeupCallback = NULL;
    break;

  case USB_DEVICE_HIBERNATION_REQ_EVENT:
    LocalCoreHandle->EventCallbacks.DevHibernationCallback = NULL;
    break;

  case USB_DEVICE_SOF_EVENT:
    LocalCoreHandle->EventCallbacks.DevSofCallback = NULL;
    break;

  case USB_DEVICE_ERRATIC_ERR_EVENT:
    LocalCoreHandle->EventCallbacks.DevErraticErrCallback = NULL;
    break;

  case USB_DEVICE_CMD_CMPLT_EVENT:
    LocalCoreHandle->EventCallbacks.DevCmdCmpltCallback = NULL;
    break;

  case USB_DEVICE_BUFF_OVERFLOW_EVENT:
    LocalCoreHandle->EventCallbacks.DevBuffOvflwCallback = NULL;
    break;

  case USB_DEVICE_TEST_LMP_RX_EVENT:
    LocalCoreHandle->EventCallbacks.DevTestLmpRxCallback = NULL;
    break;

  case USB_DEVICE_SETUP_PKT_RECEIVED:
    LocalCoreHandle->EventCallbacks.DevSetupPktReceivedCallback = NULL;
    break;

  case USB_DEVICE_XFER_NRDY:
    LocalCoreHandle->EventCallbacks.DevXferNrdyCallback = NULL;
    break;

  case USB_DEVICE_XFER_DONE:
    LocalCoreHandle->EventCallbacks.DevXferDoneCallback = NULL;
    break;

  default:
    break;
  }

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used as an interrupt service routine.
  @param[in] CoreHandle  xDCI controller handle

**/
EFI_STATUS
EFIAPI
DwcXdciCoreIsrRoutine (
  IN VOID     *CoreHandle
  )
{
  XDCI_CORE_HANDLE    *LocalCoreHandle;
  UINT32              BaseAddr;
  UINT32              EventCount;
  UINT32              ProcessedEventCount;
  UINT32              Idx;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreIsrRoutine: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  if (LocalCoreHandle->InterrupProcessing == TRUE) {
    DEBUG ((DEBUG_INFO, "interrupProcessing.........\n"));
    return EFI_SUCCESS;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;
  //
  // Event Buffer corresponding to each interrupt line needs
  // to be Processed
  //
  LocalCoreHandle->InterrupProcessing = TRUE;
  for (Idx = 0; Idx < LocalCoreHandle->MaxDevIntLines; Idx++) {
    //
    // Get the number of events HW has written for this
    //  interrupt line
    //
    EventCount = UsbRegRead (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (Idx));
    EventCount &= DWC_XDCI_EVNTCOUNT_MASK;
    ProcessedEventCount = 0;

    //
    // Process interrupt line Buffer only if count is non-zero
    //
    if (EventCount != 0) {
      //
      // Process events in this Buffer
      //
      DwcXdciProcessInterruptLineEvents (LocalCoreHandle, EventCount, &ProcessedEventCount);
      //
      // Write back the Processed number of events so HW decrements it from current
      // event count
      //
      UsbRegWrite (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (Idx), ProcessedEventCount);
    }
  }
  LocalCoreHandle->InterrupProcessing = FALSE;
  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used as an interrupt service routine and it processes only one event at a time.
  @param[in] CoreHandle  xDCI controller handle

**/
EFI_STATUS
EFIAPI
DwcXdciCoreIsrRoutineTimerBased (
  IN VOID     *CoreHandle
  )
{
  XDCI_CORE_HANDLE    *LocalCoreHandle;
  UINT32              BaseAddr;
  UINT32              EventCount;
  UINT32              ProcessedEventCount;
  UINT32              CurrentEventAddr;
  UINT32              ProcessedEventSize;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;
  ProcessedEventSize = 0;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreIsrRoutineTimerBased: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  if (LocalCoreHandle->CurrentEventBuffer == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreIsrRoutineTimerBased: INVALID event Buffer\n"));
    return EFI_INVALID_PARAMETER;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  EventCount = UsbRegRead (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (0)) & DWC_XDCI_EVNTCOUNT_MASK;

  if (LocalCoreHandle->InterrupProcessing == TRUE) {
    DEBUG ((DEBUG_INFO, "interrupProcessing.........\n"));
    return EFI_SUCCESS;
  }

  LocalCoreHandle->InterrupProcessing = TRUE;

  ProcessedEventCount = 0;
  CurrentEventAddr = (UINT32) (UINTN) (LocalCoreHandle->CurrentEventBuffer);

  if (LocalCoreHandle->CurrentEventBuffer->Event & DWC_XDCI_EVENT_DEV_MASK) {
    DwcXdciProcessDeviceEvent (
      LocalCoreHandle,
      LocalCoreHandle->CurrentEventBuffer,
      &ProcessedEventSize
      );
  } else {
    DwcXdciProcessEpEvent (
      LocalCoreHandle,
      LocalCoreHandle->CurrentEventBuffer,
      &ProcessedEventSize);
  }

  EventCount -= ProcessedEventSize;
  ProcessedEventCount += ProcessedEventSize;
  if ((CurrentEventAddr + ProcessedEventSize) >=
      ((UINT32) (UINTN) (LocalCoreHandle->AlignedEventBuffers) + (sizeof (DWC_XDCI_EVENT_BUFFER) *
          DWC_XDCI_MAX_EVENTS_PER_BUFFER))
      ) {
    CurrentEventAddr = (UINT32) (UINTN) (LocalCoreHandle->AlignedEventBuffers);
    DEBUG ((DEBUG_INFO, "DwcXdciProcessInterruptLineEvents: Event Buffer bound reached\n"));
  } else {
    CurrentEventAddr += ProcessedEventSize;
  }

  LocalCoreHandle->CurrentEventBuffer = (DWC_XDCI_EVENT_BUFFER *) (UINTN)CurrentEventAddr;
  UsbRegWrite (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (0), ProcessedEventCount);
  LocalCoreHandle->InterrupProcessing = FALSE;

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to enable xDCI to connect to the host.
  @param[in] CoreHandle  xDCI controller handle

**/
EFI_STATUS
EFIAPI
DwcXdciCoreConnect (
  IN VOID     *CoreHandle
  )
{
  XDCI_CORE_HANDLE    *LocalCoreHandle;
  UINT32              MaxDelayIter;
  UINT32              BaseAddr;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;
  MaxDelayIter = DWC_XDCI_MAX_DELAY_ITERATIONS;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreConnect: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  //
  // Clear KeepConnect bit so we can allow disconnect and re-connect
  // Also issue No action on state change to aVOID any link change
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCTL_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) & ~DWC_XDCI_DCTL_KEEP_CONNECT_MASK) & ~DWC_XDCI_DCTL_STATE_CHANGE_REQ_MASK
    );

  //
  // Set Run bit to connect to the host
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCTL_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) | DWC_XDCI_DCTL_RUN_STOP_MASK
    );

  //
  // Wait until core starts running
  //
  do {
    if ((UsbRegRead (BaseAddr, DWC_XDCI_DSTS_REG) & DWC_XDCI_DSTS_DEV_CTRL_HALTED_MASK) == 0) {
      break;
    } else {
      gBS->Stall (DWC_XDCI_MAX_DELAY_ITERATIONS);
    }
  } while (--while ((--MaxDelayIter) != 0));

  if (MaxDelayIter == 0) {
    DEBUG ((DEBUG_INFO, "Failed to run the device controller\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to disconnect xDCI from the host.
  @param[in] CoreHandle  xDCI controller handle

**/
EFI_STATUS
EFIAPI
DwcXdciCoreDisconnect (
  IN VOID    *CoreHandle
  )
{
  XDCI_CORE_HANDLE  *LocalCoreHandle;
  UINT32            MaxDelayIter;
  UINT32            BaseAddr;
  UINT32            EventCount;
  UINT32            Dsts;
  UINT32            Idx;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;
  MaxDelayIter = DWC_XDCI_MAX_DELAY_ITERATIONS;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreDisconnect: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  EventCount = UsbRegRead (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (0));
  EventCount &= DWC_XDCI_EVNTCOUNT_MASK;

  DEBUG ((DEBUG_INFO, "DwcXdciCoreDisconnect: EventCount=%d\n", EventCount));
  while (EventCount != 0) {
    DwcXdciCoreIsrRoutine (LocalCoreHandle);
    EventCount = UsbRegRead (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (0));
    EventCount &= DWC_XDCI_EVNTCOUNT_MASK;
    DEBUG ((DEBUG_INFO, "DwcXdciCoreDisconnect: EventCount=%d\n", EventCount));
  }

  //
  // Issue DEPENDXFER for active transfers
  //
  for (Idx = 0; Idx < DWC_XDCI_MAX_ENDPOINTS; Idx++) {
    if (LocalCoreHandle->EpHandles[Idx].CurrentXferRscIdx) {
      DwcXdciEndXfer (LocalCoreHandle, Idx);
    }
  }
  //
  // Clear Run bit to disconnect from host
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCTL_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) & ~DWC_XDCI_DCTL_RUN_STOP_MASK);

  //
  // Wait until core is halted
  //
  do {
    Dsts = UsbRegRead (BaseAddr, DWC_XDCI_DSTS_REG);
    DEBUG ((DEBUG_INFO, "DwcXdciCoreDisconnect: waiting halt: DSTS=0x%x\n", Dsts));
    if ((Dsts & DWC_XDCI_DSTS_DEV_CTRL_HALTED_MASK) != 0) {
      break;
    } else {
      gBS->Stall (DWC_XDCI_MAX_DELAY_ITERATIONS);
    }
  } while ((--MaxDelayIter) != 0);

  if (MaxDelayIter == 0) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreDisconnect: Failed to halt the device controller\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to obtain current USB bus Speed.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] Speed       Address of variable to save the Speed

**/
EFI_STATUS
EFIAPI
DwcXdciCoreGetSpeed (
  IN VOID         *CoreHandle,
  IN USB_SPEED    *Speed
  )
{
  XDCI_CORE_HANDLE *LocalCoreHandle;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreGetSpeed: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  if (Speed == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreGetSpeed: INVALID parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  *Speed = UsbRegRead (LocalCoreHandle->BaseAddress, DWC_XDCI_DSTS_REG) & DWC_XDCI_DSTS_CONN_SPEED_MASK;

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to obtain current USB bus Speed.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] address     USB address to set (assigned by USB host)

**/
EFI_STATUS
EFIAPI
DwcXdciCoreSetAddress (
  IN VOID      *CoreHandle,
  IN UINT32    address
  )
{
  XDCI_CORE_HANDLE  *LocalCoreHandle;
  UINT32            BaseAddr;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreSetAddress: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  DEBUG ((DEBUG_INFO, "DwcXdciCoreSetAddress is 0x%x \n", address));
  //
  // Program USB device address
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCFG_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DCFG_REG) & ~DWC_XDCI_DCFG_DEV_ADDRESS_MASK) | (address <<
        DWC_XDCI_DCFG_DEV_ADDRESS_BIT_POS)
    );

  LocalCoreHandle->DevState = UsbDevStateAddress;
  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_GCTL_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_GCTL_REG)));
  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_DEVTEN_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_DEVTEN_REG)));
  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_DCFG_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_DCFG_REG)));
  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_DSTS_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_DSTS_REG)));

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to set configuration.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] ConfigNum   config num to set (assigned by USB host)

**/
EFI_STATUS
EFIAPI
DwcXdciCoreSetConfig (
  IN VOID      *CoreHandle,
  IN UINT32    ConfigNum
  )
{
  XDCI_CORE_HANDLE              *LocalCoreHandle;
  DWC_XDCI_ENDPOINT_CMD_PARAMS  EpCmdParams;
  EFI_STATUS                    Status;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreSetConfig: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Issue DEPSTARTCFG command on EP0 (new config for
  // non-control EPs)
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[0].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_NONE,
             EPCMD_START_NEW_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreSetConfig: Failed to init params for EPCMD_START_NEW_CONFIG command\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             0,
             (EPCMD_START_NEW_CONFIG | (2 << DWC_XDCI_EPCMD_RES_IDX_BIT_POS)),
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreSetConfig: Failed to issue EPCMD_START_NEW_CONFIG command\n"));
    return Status;
  }

  return Status;
}


/**
  Interface:
  This function is used to set link state.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] state       Desired link state

**/
EFI_STATUS
EFIAPI
DwcXdciSetLinkState (
  IN VOID                        *CoreHandle,
  IN USB_DEVICE_SS_LINK_STATE    state
  )
{
  XDCI_CORE_HANDLE  *LocalCoreHandle;
  UINT32            BaseAddr;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciSetLinkState: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  //
  // Clear old mask
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCTL_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) & ~DWC_XDCI_DCTL_STATE_CHANGE_REQ_MASK
    );

  //
  // Request new state
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCTL_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) | (state << DWC_XDCI_DCTL_STATE_CHANGE_REQ_BIT_POS)
    );

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to initialize endpoint.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] EpInfo      Address of structure describing properties of EP
  to be initialized

**/
EFI_STATUS
EFIAPI
DwcXdciInitEp (
  IN VOID           *CoreHandle,
  IN USB_EP_INFO    *EpInfo
  )
{
  XDCI_CORE_HANDLE              *LocalCoreHandle;
  DWC_XDCI_ENDPOINT_CMD_PARAMS  EpCmdParams;
  EFI_STATUS                    Status;
  UINT32                        EpNum;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciInitEp: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Convert to physical endpoint
  //
  EpNum = DwcXdciGetPhysicalEpNum (EpInfo->EpNum, EpInfo->EpDir);

  //
  // Save EP properties
  //
  CopyMem (& (LocalCoreHandle->EpHandles[EpNum].EpInfo), EpInfo, sizeof (USB_EP_INFO));

  //
  // Init CheckFlag
  //
  LocalCoreHandle->EpHandles[EpNum].CheckFlag = FALSE;

  //
  // Init DEPCFG cmd params for EP
  //
  Status = DwcXdciCoreInitEpCmdParams (
             CoreHandle,
             &LocalCoreHandle->EpHandles[EpNum].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_INIT_STATE,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciInitEp: Failed to init params for  EPCMD_SET_EP_CONFIG command\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             CoreHandle,
             EpNum,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciInitEp: Failed to issue  EPCMD_SET_EP_CONFIG command\n"));
    return Status;
  }

  //
  // Issue a DEPXFERCFG command for endpoint
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[EpNum].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_NONE,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciInitEp: Failed to init params for  EPCMD_SET_EP_XFER_RES_CONFIG command\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             EpNum,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciInitEp: Failed to issue EPCMD_SET_EP_XFER_RES_CONFIG command\n"));
  }

  return Status;
}


/**
  Interface:
  This function is used to enable non-Ep0 endpoint.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] EpInfo      Address of structure describing properties of EP
  to be enabled

**/
EFI_STATUS
EFIAPI
DwcXdciEpEnable (
  IN VOID           *CoreHandle,
  IN USB_EP_INFO    *EpInfo
  )
{
  XDCI_CORE_HANDLE  *LocalCoreHandle;
  UINT32            EpNum;
  UINT32            BaseAddr;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpEnable: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  //
  // Convert to physical endpoint
  //
  EpNum = DwcXdciGetPhysicalEpNum (EpInfo->EpNum, EpInfo->EpDir);

  //
  // Enable Physical Endpoint EpNum
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EP_DALEPENA_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_EP_DALEPENA_REG) | (1 << EpNum)
    );

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to disable non-Ep0 endpoint.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] EpInfo      Address of structure describing properties of EP
  to be enabled

**/
EFI_STATUS
EFIAPI
DwcXdciEpDisable (
  IN VOID           *CoreHandle,
  IN USB_EP_INFO    *EpInfo
  )
{
  XDCI_CORE_HANDLE  *LocalCoreHandle;
  UINT32            EpNum;
  UINT32            BaseAddr;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpDisable: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  //
  // Convert to physical endpoint
  //
  EpNum = DwcXdciGetPhysicalEpNum (EpInfo->EpNum, EpInfo->EpDir);

  //
  // Disable Physical Endpoint EpNum
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EP_DALEPENA_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_EP_DALEPENA_REG) & ~ (1 << EpNum)
    );

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to STALL and endpoint.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] EpInfo      Address of structure describing properties of EP
  to be enabled

**/
EFI_STATUS
EFIAPI
DwcXdciEpStall (
  IN VOID           *CoreHandle,
  IN USB_EP_INFO    *EpInfo
  )
{
  XDCI_CORE_HANDLE              *LocalCoreHandle;
  DWC_XDCI_ENDPOINT_CMD_PARAMS  EpCmdParams;
  EFI_STATUS                    Status;
  UINT32                        EpNum;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpStall: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Convert to physical endpoint
  //
  EpNum = DwcXdciGetPhysicalEpNum (EpInfo->EpNum, EpInfo->EpDir);

  //
  // Set Ep State Info
  //
  if (LocalCoreHandle->EpHandles[EpNum].State != USB_EP_STATE_STALLED) {
    LocalCoreHandle->EpHandles[EpNum].OrgState = LocalCoreHandle->EpHandles[EpNum].State;
    LocalCoreHandle->EpHandles[EpNum].State = USB_EP_STATE_STALLED;
  }
  //
  // Issue a DWC_XDCI_EPCMD_SET_STALL for EP
  // Reset params
  //
  EpCmdParams.Param0 = EpCmdParams.Param1 = EpCmdParams.Param2 = 0;

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             EpNum,
             DWC_XDCI_EPCMD_SET_STALL,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpStall: Failed to issue EP stall command\n"));
  }

  return Status;
}


/**
  Interface:
  This function is used to clear endpoint STALL.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] EpInfo      Address of structure describing properties of EP
  to be enabled

**/
EFI_STATUS
EFIAPI
DwcXdciEpClearStall (
  IN VOID           *CoreHandle,
  IN USB_EP_INFO    *EpInfo
  )
{
  XDCI_CORE_HANDLE              *LocalCoreHandle;
  DWC_XDCI_ENDPOINT_CMD_PARAMS  EpCmdParams;
  EFI_STATUS                    Status;
  UINT32                        EpNum;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpClearStall: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Convert to physical endpoint
  //
  EpNum = DwcXdciGetPhysicalEpNum (EpInfo->EpNum, EpInfo->EpDir);

  //
  // Set Ep State Info
  //
  LocalCoreHandle->EpHandles[EpNum].State = LocalCoreHandle->EpHandles[EpNum].OrgState;

  //
  // Issue a DWC_XDCI_EPCMD_CLEAR_STALL for EP
  // Reset params
  //
  EpCmdParams.Param0 = EpCmdParams.Param1 = EpCmdParams.Param2 = 0;

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             EpNum,
             DWC_XDCI_EPCMD_CLEAR_STALL,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpStall: Failed to issue EP clea stall command\n"));
  }

  return Status;
}


/**
  Interface:
  This function is used to set endpoint in NOT READY state.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] EpInfo      Address of structure describing properties of EP
  to be enabled

**/
EFI_STATUS
EFIAPI
DwcXdciEpSetNrdy (
  IN VOID           *CoreHandle,
  IN USB_EP_INFO    *EpInfo
  )
{
  XDCI_CORE_HANDLE  *LocalCoreHandle;
  UINT32            EpNum;
  UINT32            BaseAddr;
  UINT32            MaxDelayIter;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;
  MaxDelayIter = DWC_XDCI_MAX_DELAY_ITERATIONS;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpSetNrdy: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  //
  // Convert to physical endpoint
  //
  EpNum = DwcXdciGetPhysicalEpNum (EpInfo->EpNum, EpInfo->EpDir);

  //
  // Program the EP number in command's param reg
  //
  UsbRegWrite (BaseAddr, DWC_XDCI_DGCMD_PARAM_REG, EpNum);

  //
  // Issue EP not ready generic device command
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DGCMD_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) | DWC_XDCI_DGCMD_CMD_SET_EP_NRDY)
    );

  //
  // Activate the command
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DGCMD_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) | DWC_XDCI_DGCMD_CMD_ACTIVE_MASK)
    );

  //
  // Wait until command completes
  //
  do {
    if ((UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) & DWC_XDCI_DGCMD_CMD_ACTIVE_MASK) == 0) {
      break;
    } else {
      gBS->Stall (DWC_XDCI_MAX_DELAY_ITERATIONS);
    }
  } while ((--MaxDelayIter) != 0);

  if (MaxDelayIter == 0) {
    DEBUG ((DEBUG_INFO, "Failed to issue Command\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Interface:
  This function is used to queue receive SETUP packet request.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] Buffer      Address of Buffer to receive SETUP packet

**/
EFI_STATUS
EFIAPI
DwcXdciEp0ReceiveSetupPkt (
  IN VOID     *CoreHandle,
  IN UINT8    *Buffer
  )
{
  XDCI_CORE_HANDLE                *LocalCoreHandle;
  DWC_XDCI_ENDPOINT_CMD_PARAMS    EpCmdParams;
  EFI_STATUS                      Status;
  DWC_XDCI_TRB                    *Trb;

  Status = EFI_DEVICE_ERROR;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEp0ReceiveSetupPkt: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  LocalCoreHandle->EpHandles[0].EpInfo.EpNum = 0;
  LocalCoreHandle->EpHandles[0].EpInfo.EpDir = 0;
  LocalCoreHandle->EpHandles[0].State = USB_EP_STATE_SETUP;
  Trb = LocalCoreHandle->Trbs;
  DEBUG ((DEBUG_INFO, "(DwcXdciEp0ReceiveSetupPkt)\n"));

  Status = DwcXdciCoreInitTrb (
             LocalCoreHandle,
             Trb,
             TRBCTL_SETUP,
             Buffer,
             8
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciEp0ReceiveSetupPkt: Init TRB Failed \n"));
    return Status;
  }

  //
  // Issue a DEPSTRTXFER for EP0
  // Reset params
  //
  EpCmdParams.Param0 = EpCmdParams.Param1 = EpCmdParams.Param2 = 0;

  //
  // Init the lower re-bits for TRB address
  //
  EpCmdParams.Param1 = (UINT32) (UINTN)Trb;

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             0,
             EPCMD_START_XFER,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "\nDwcXdciEp0ReceiveSetupPkt: Failed to issue Start Transfer command"));
  }

  //
  // Save new resource index for this transfer
  //
  LocalCoreHandle->EpHandles[0].CurrentXferRscIdx = ((UsbRegRead (LocalCoreHandle->BaseAddress, DWC_XDCI_EPCMD_REG (0)) &
      DWC_XDCI_EPCMD_RES_IDX_MASK) >> DWC_XDCI_EPCMD_RES_IDX_BIT_POS
      );

  return Status;
}


/**
  Interface:
  This function is used to queue receive Status packet on EP0.
  @param[in] CoreHandle  xDCI controller handle

**/
EFI_STATUS
EFIAPI
DwcXdciEp0ReceiveStatusPkt (
  IN VOID    *CoreHandle
  )
{
  XDCI_CORE_HANDLE                *LocalCoreHandle;
  DWC_XDCI_TRB                    *Trb;
  DWC_XDCI_TRB_CONTROL            TrbCtrl;
  DWC_XDCI_ENDPOINT_CMD_PARAMS    EpCmdParams;
  EFI_STATUS                      Status;
  UINT32                          BaseAddr;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEp0ReceiveStatusPkt: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  //
  // We are receiving on EP0 so physical EP is 0
  //
  Trb = LocalCoreHandle->Trbs;
  DEBUG ((DEBUG_INFO, "(DwcXdciEp0ReceiveStatusPkt)\n"));
  if (Trb->TrbCtrl & DWC_XDCI_TRB_CTRL_HWO_MASK) {
    DEBUG ((DEBUG_INFO, "statusPkt still not transferred.\n"));
    return EFI_SUCCESS;
  }

  LocalCoreHandle->EpHandles[0].EpInfo.EpNum = 0;
  LocalCoreHandle->EpHandles[0].EpInfo.EpDir = 0;

  //
  // OUT data phase for 3-phased control transfer
  //
  TrbCtrl = TRBCTL_3_PHASE;

  //
  // Init TRB for the transfer
  //
  Status = DwcXdciCoreInitTrb (
             LocalCoreHandle,
             Trb,
             TrbCtrl,
             LocalCoreHandle->AlignedSetupBuffer,
             0
             );

  if (Status == EFI_SUCCESS) {
    //
    // Issue a DEPSTRTXFER for EP0
    // Reset params
    //
    EpCmdParams.Param0 = EpCmdParams.Param1 = EpCmdParams.Param2 = 0;

    //
    // Init the lower bits for TRB address
    //
    EpCmdParams.Param1 = (UINT32) (UINTN)Trb;

    //
    // Issue the command
    //
    Status = DwcXdciCoreIssueEpCmd (
               LocalCoreHandle,
               0,
               EPCMD_START_XFER,
               &EpCmdParams
               );

    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "DwcXdciEp0ReceiveStatusPkt: Failed to issue Start Transfer command for EP0\n"));
    }
    //
    // Save new resource index for this transfer
    //
    LocalCoreHandle->EpHandles[0].CurrentXferRscIdx = ((UsbRegRead (BaseAddr,
        DWC_XDCI_EPCMD_REG (0)) & DWC_XDCI_EPCMD_RES_IDX_MASK) >> DWC_XDCI_EPCMD_RES_IDX_BIT_POS);

    //
    // TODO: We are not using the EP state for control transfers
    // right now simply because we're only supporting IN
    // data phase. For the current use case, we don't
    // need OUT data phase. We can add that later and we will
    // add some of the state and SETUP packet awareness code
    //
    LocalCoreHandle->EpHandles[0].State = USB_EP_STATE_STATUS;
  }

  return Status;
}


/**
  Interface:
  This function is used to send Status packet on EP0.
  @param[in] CoreHandle  xDCI controller handle

**/
EFI_STATUS
EFIAPI
DwcXdciEp0SendStatusPkt (
  IN VOID    *CoreHandle
  )
{
  XDCI_CORE_HANDLE                *LocalCoreHandle;
  DWC_XDCI_TRB                    *Trb;
  DWC_XDCI_ENDPOINT_CMD_PARAMS    EpCmdParams;
  EFI_STATUS                      Status;
  UINT32                          BaseAddr;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEp0SendStatusPkt: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  //
  // We are sending on EP0 so physical EP is 1
  //
  Trb = (LocalCoreHandle->Trbs + (1 * DWC_XDCI_TRB_NUM));
  DEBUG ((DEBUG_INFO, "(DwcXdciEp0SendStatusPkt)\n"));

  LocalCoreHandle->EpHandles[0].State = USB_EP_STATE_STATUS;
  Status = DwcXdciCoreInitTrb (
             LocalCoreHandle,
             Trb,
             TRBCTL_2_PHASE,
             LocalCoreHandle->AlignedMiscBuffer,
             0
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciEp0SendStatusPkt: TRB failed during Status phase\n"));
    return Status;
  }

  //
  // Issue a DEPSTRTXFER for EP1
  // Reset params
  //
  EpCmdParams.Param0 = EpCmdParams.Param1 = EpCmdParams.Param2 = 0;

  //
  // Init the lower re-bits for TRB address
  //
  EpCmdParams.Param1 = (UINT32) (UINTN)Trb;

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             1,
             EPCMD_START_XFER,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciEp0SendStatusPkt: Failed to issue Start Transfer on EP0\n"));
  }

  //
  // Save new resource index for this transfer
  //
  LocalCoreHandle->EpHandles[1].CurrentXferRscIdx = ((UsbRegRead (BaseAddr,
      DWC_XDCI_EPCMD_REG (1)) & DWC_XDCI_EPCMD_RES_IDX_MASK) >> DWC_XDCI_EPCMD_RES_IDX_BIT_POS);
  LocalCoreHandle->EpHandles[0].State = USB_EP_STATE_STATUS;

  return Status;
}


/**
  Interface:
  This function is used to send data on non-EP0 endpoint.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] XferReq Address to transfer request describing this transfer

**/
EFI_STATUS
EFIAPI
DwcXdciEpTxData (
  IN VOID                *CoreHandle,
  IN USB_XFER_REQUEST    *XferReq
  )
{
  XDCI_CORE_HANDLE              *LocalCoreHandle;
  DWC_XDCI_ENDPOINT_CMD_PARAMS  EpCmdParams;
  DWC_XDCI_TRB                  *Trb;
  DWC_XDCI_TRB_CONTROL          TrbCtrl;
  EFI_STATUS                    Status;
  UINT32                        EpNum;
  UINT32                        BaseAddr;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpTxData: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  if (XferReq == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpTxData: INVALID transfer request\n"));
    return EFI_INVALID_PARAMETER;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  //
  // Convert to physical endpoint
  //
  EpNum = DwcXdciGetPhysicalEpNum (
            XferReq->EpInfo.EpNum,
            XferReq->EpInfo.EpDir
            );

  Trb = (LocalCoreHandle->Trbs + (EpNum * DWC_XDCI_TRB_NUM));
  DEBUG ((DEBUG_INFO, "(DwcXdciEpTxData)EpNum is %d\n", EpNum));


  if (EpNum > 1) {
    TrbCtrl = TRBCTL_NORMAL;
  } else {
    TrbCtrl = TRBCTL_CTRL_DATA_PHASE;
  }

  if (Trb->TrbCtrl & DWC_XDCI_TRB_CTRL_HWO_MASK) {
    Status = DwcXdciEndXfer (LocalCoreHandle, EpNum);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "DwcXdciEpTxData: Failed to end previous transfer\n"));
    }

    Status = DwcXdciCoreFlushEpTxFifo (LocalCoreHandle, EpNum);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "DwcXdciEpTxData: Failed to end previous transfer\n"));
    }
  }

  //
  // Data phase
  //
  CopyMem (& (LocalCoreHandle->EpHandles[EpNum].XferHandle), XferReq, sizeof (USB_XFER_REQUEST));
  LocalCoreHandle->EpHandles[EpNum].State = USB_EP_STATE_DATA;

  LocalCoreHandle->EpHandles[EpNum].Trb = Trb;

  Status = DwcXdciCoreInitTrb (
             LocalCoreHandle,
             Trb,
             TrbCtrl,
             XferReq->XferBuffer,
             XferReq->XferLen
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpTxData: TRB failed\n"));
    return Status;
  }

  //
  // Issue a DEPSTRTXFER for EP
  // Reset params
  //
  EpCmdParams.Param0 = EpCmdParams.Param1 = EpCmdParams.Param2 = 0;

  //
  // Init the lower re-bits for TRB address
  //
  EpCmdParams.Param1 = (UINT32) (UINTN)Trb;

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             EpNum,
             EPCMD_START_XFER,
             &EpCmdParams
             );

  //
  // Save new resource index for this transfer
  //
  LocalCoreHandle->EpHandles[EpNum].CurrentXferRscIdx = ((UsbRegRead (BaseAddr,
      DWC_XDCI_EPCMD_REG (EpNum)) & DWC_XDCI_EPCMD_RES_IDX_MASK) >> DWC_XDCI_EPCMD_RES_IDX_BIT_POS);

  return Status;
}


/**
  Interface:
  This function is used to receive data on non-EP0 endpoint.
  @param[in] CoreHandle xDCI controller handle
  @param[in] XferReq Address to transfer request describing this transfer
**/
EFI_STATUS
EFIAPI
DwcXdciEpRxData (
  IN VOID                *CoreHandle,
  IN USB_XFER_REQUEST    *XferReq
  )
{
  XDCI_CORE_HANDLE              *LocalCoreHandle;
  DWC_XDCI_ENDPOINT_CMD_PARAMS  EpCmdParams;
  DWC_XDCI_TRB                  *Trb;
  DWC_XDCI_TRB_CONTROL          TrbCtrl;
  EFI_STATUS                    Status;
  UINT32                        EpNum;
  UINT32                        BaseAddr;

  LocalCoreHandle = (XDCI_CORE_HANDLE *)CoreHandle;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpRxData: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  if (XferReq == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpRxData: INVALID transfer request\n"));
    return EFI_INVALID_PARAMETER;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  //
  // Convert to physical endpoint
  //
  EpNum = DwcXdciGetPhysicalEpNum (XferReq->EpInfo.EpNum, XferReq->EpInfo.EpDir);

  Trb = (LocalCoreHandle->Trbs + (EpNum * DWC_XDCI_TRB_NUM));
  DEBUG ((DEBUG_INFO, "(DwcXdciEpRxData)EpNum is %d\n", EpNum));

  if (EpNum > 1) {
    TrbCtrl = TRBCTL_NORMAL;
  } else {
    TrbCtrl = TRBCTL_CTRL_DATA_PHASE;
  }

  //
  // If CheckFlag didn't set to FALSE, means the previous transfer request didn't complete,
  // need to wait the previous request done.
  //
  if (LocalCoreHandle->EpHandles[EpNum].CheckFlag == TRUE) {
    return EFI_NOT_READY;
  }

  LocalCoreHandle->EpHandles[EpNum].CheckFlag = TRUE;

  //
  // Data phase
  //
  CopyMem (& (LocalCoreHandle->EpHandles[EpNum].XferHandle), XferReq, sizeof (USB_XFER_REQUEST));

  LocalCoreHandle->EpHandles[EpNum].State = USB_EP_STATE_DATA;

  LocalCoreHandle->EpHandles[EpNum].Trb = Trb;

  DEBUG ((DEBUG_INFO, "(DwcXdciEpRxData)XferReq->XferLen is 0x%x\n", XferReq->XferLen));

  Status = DwcXdciCoreInitTrb (
             LocalCoreHandle,
             Trb,
             TrbCtrl,
             XferReq->XferBuffer,
             XferReq->XferLen
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpRxData: TRB failed\n"));
    return Status;
  }
  //
  // Issue a DEPSTRTXFER for EP
  // Reset params
  //
  EpCmdParams.Param0 = EpCmdParams.Param1 = EpCmdParams.Param2 = 0;

  //
  // Init the lower re-bits for TRB address
  //
  EpCmdParams.Param1 = (UINT32) (UINTN)Trb;

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             EpNum,
             EPCMD_START_XFER,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpRxData: Failed to start transfer\n"));
  }

  //
  // Save new resource index for this transfer
  //
  LocalCoreHandle->EpHandles[EpNum].CurrentXferRscIdx = ((UsbRegRead (BaseAddr,
      DWC_XDCI_EPCMD_REG (EpNum)) & DWC_XDCI_EPCMD_RES_IDX_MASK) >> DWC_XDCI_EPCMD_RES_IDX_BIT_POS);

  return Status;
}


/**
  Interface:
  This function is used to flush all the FIFO of the given endpoint.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] EpNum       Physical EP num

**/
STATIC
EFI_STATUS
DwcXdciCoreFlushEpFifo (
  IN XDCI_CORE_HANDLE    *CoreHandle,
  IN UINT32              EpNum
  )
{
  UINT32 BaseAddr;
  UINT32 MaxDelayIter;
  UINT32 FifoNum;
  UINT32 Param;

  MaxDelayIter = DWC_XDCI_MAX_DELAY_ITERATIONS;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR: DwcXdciCoreFlushEpTxFifo: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  BaseAddr = CoreHandle->BaseAddress;

  //
  // Translate to FIFOnum
  // NOTE: Assuming this is a Tx EP
  //
  FifoNum = (EpNum >> 1);

  //
  // TODO: Currently we are only using TxFIFO 0. Later map these
  // Write the FIFO num/dir param for the generic command.
  //

  Param = UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_PARAM_REG);
  Param &= ~ (DWC_XDCI_DGCMD_PARAM_TX_FIFO_NUM_MASK | DWC_XDCI_DGCMD_PARAM_TX_FIFO_DIR_MASK);

  if ((EpNum & 0x01) != 0) {
    Param |= (FifoNum | DWC_XDCI_DGCMD_PARAM_TX_FIFO_DIR_MASK);
  } else {
    Param |= FifoNum;
  }

  DEBUG ((DEBUG_INFO, "USB FU Flash: CMD 0x%08x :: Param 0x%08x\n",
          (UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) | DWC_XDCI_DGCMD_CMD_SEL_FIFO_FLUSH | DWC_XDCI_DGCMD_CMD_ACTIVE_MASK),
          Param));

  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DGCMD_PARAM_REG,
    Param
    );

  //
  // Write the command to flush all FIFOs
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DGCMD_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) | DWC_XDCI_DGCMD_CMD_SEL_FIFO_FLUSH | DWC_XDCI_DGCMD_CMD_ACTIVE_MASK)
    );


  //
  // Wait until command completes
  //
  do {
    if ((UsbRegRead (BaseAddr, DWC_XDCI_DGCMD_REG) & DWC_XDCI_DGCMD_CMD_ACTIVE_MASK) == 0) {
      break;
    } else {
      gBS->Stall (DWC_XDCI_MAX_DELAY_ITERATIONS);
    }
  } while ((--MaxDelayIter) != 0);

  if (MaxDelayIter == 0) {
    DEBUG ((DEBUG_INFO, "Failed to issue Command\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Interface:
  This function is used to cancel a transfer on non-EP0 endpoint.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] EpInfo      Address of structure describing properties of EP

**/
EFI_STATUS
EFIAPI
DwcXdciEpCancelTransfer (
  IN VOID           *CoreHandle,
  IN USB_EP_INFO    *EpInfo
  )
{
  EFI_STATUS  Status;
  UINT32      EpNum;

  Status = EFI_DEVICE_ERROR;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpCancelTransfer: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Get physical EP num
  //
  EpNum = DwcXdciGetPhysicalEpNum (EpInfo->EpNum, EpInfo->EpDir);
  Status = DwcXdciEndXfer (CoreHandle, EpNum);
  DwcXdciCoreFlushEpFifo (CoreHandle, EpNum);

  return Status;
}

/**
   Wrapper function to process bus reset detection event.
   @param[in] CoreHandle  xDCI controller handle address
 **/
EFI_STATUS
UsbProcessDeviceResetDet (
  IN XDCI_CORE_HANDLE    *CoreHandle
  )
{
  return DwcXdciProcessDeviceResetDet (CoreHandle);
}

/**
   Wrapper function to process bus reset detection event.
   complete) event.
   @param[in] CoreHandle  xDCI controller handle address
 **/
EFI_STATUS
UsbProcessDeviceResetDone (
  IN XDCI_CORE_HANDLE    *CoreHandle
  )
{
  return DwcXdciProcessDeviceResetDone (CoreHandle);
}

/**
  Wrapper function to get the physical end point number.
  @param[in] EndpointNum Logical endpoint number
  @param[in] EndpointDir Direction for the endpoint
**/
UINT32
UsbGetPhysicalEpNum (
  IN UINT32        EndpointNum,
  IN USB_EP_DIR    EndpointDir
  )
{
  return DwcXdciGetPhysicalEpNum (
           EndpointNum,
           EndpointDir
           );
}

/**
   This function re-initializes the USB device controller.
   @param[in] CoreHandle  xDCI controller handle address
 **/
EFI_STATUS
EFIAPI
UsbXdciCoreReinit (
  IN VOID                     *CoreHandle
  )
{
  EFI_STATUS                      Status;
  UINT32                          BaseAddr;
  XDCI_CORE_HANDLE                *LocalCoreHandle;
  DWC_XDCI_ENDPOINT_CMD_PARAMS    EpCmdParams;
  UINT32                          MaxDelayIter;
  UINT8                           Idx;

  Status = EFI_DEVICE_ERROR;
  MaxDelayIter = DWC_XDCI_MAX_DELAY_ITERATIONS;

  LocalCoreHandle = CoreHandle;

  if (CoreHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (LocalCoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to allocate handle for xDCI\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  BaseAddr = LocalCoreHandle->BaseAddress;

  DEBUG ((DEBUG_INFO, "Resetting the USB core\n"));
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCTL_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) | DWC_XDCI_DCTL_CSFTRST_MASK
    );

  //
  // Wait until core soft reset completes
  //
  do {
    if ((UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) & DWC_XDCI_DCTL_CSFTRST_MASK) == 0) {
      break;
    } else {
      gBS->Stall (DWC_XDCI_MAX_DELAY_ITERATIONS);
    }
  } while ((--MaxDelayIter) != 0);

  if (MaxDelayIter == 0) {
    DEBUG ((DEBUG_INFO, "Failed to reset device controller\n"));
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((DEBUG_INFO, "USB core has been reset\n"));

  LocalCoreHandle->DevState = UsbDevStateDefault;

  //
  // Clear KeepConnect bit so we can allow disconnect and
  // re-connect. Stay in RX_DETECT state
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCTL_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_DCTL_REG) &
    (~DWC_XDCI_DCTL_KEEP_CONNECT_MASK) &
    ((~DWC_XDCI_DCTL_STATE_CHANGE_REQ_MASK) |
     (DWC_XDCI_DCTL_STATE_CHANGE_REQ_RX_DETECT << DWC_XDCI_DCTL_STATE_CHANGE_REQ_BIT_POS))
    );

  DEBUG ((DEBUG_INFO, "Device controller Synopsys ID: %x\n", UsbRegRead (BaseAddr, DWC_XDCI_GSNPSID_REG)));
  DEBUG ((DEBUG_INFO, "Default value of xDCI GSBUSCFG0 and GSBUSCFG1: %x, %x\n",
          UsbRegRead (BaseAddr, DWC_XDCI_GSBUSCFG0_REG),
          UsbRegRead (BaseAddr, DWC_XDCI_GSBUSCFG1_REG)));

  DEBUG ((DEBUG_INFO, "Default value of xDCI GTXTHRCFG and GRXTHRCFG: %x, %x\n",
          UsbRegRead (BaseAddr, DWC_XDCI_GTXTHRCFG_REG),
          UsbRegRead (BaseAddr, DWC_XDCI_GRXTHRCFG_REG)));

  //
  // Clear ULPI auto-resume bit
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GUSB2PHYCFG_REG (0),
    (UsbRegRead (BaseAddr, DWC_XDCI_GUSB2PHYCFG_REG (0)) & ~DWC_XDCI_GUSB2PHYCFG_ULPI_AUTO_RESUME_MASK)
    );

  DEBUG ((DEBUG_INFO, "Default value of xDCI GUSB2PHYCFG and GUSB3PIPECTL: %x, %x\n",
          UsbRegRead (BaseAddr, DWC_XDCI_GUSB2PHYCFG_REG (0)),
          UsbRegRead (BaseAddr, DWC_XDCI_GUSB3PIPECTL_REG (0))));

  //
  // Only one RxFIFO
  //
  DEBUG ((DEBUG_INFO, "Default value of DWC_XDCI_GRXFIFOSIZ: %x\n",
          UsbRegRead (BaseAddr, DWC_XDCI_GRXFIFOSIZ_REG (0))));

  for (Idx = 0; Idx < DWC_XDCI_MAX_ENDPOINTS; Idx++) {
    DEBUG ((DEBUG_INFO, "Default value of xDCI DWC_XDCI_GTXFIFOSIZ %d: %x\n",
            Idx, UsbRegRead (BaseAddr, DWC_XDCI_GTXFIFOSIZ_REG (Idx))));
  }

  //
  // TODO: Need to check if TxFIFO should start where RxFIFO ends
  // or default is correct i.e. TxFIFO starts at 0 just like RxFIFO
  //

  //
  // Allocate and Initialize Event Buffers
  //
  LocalCoreHandle->MaxDevIntLines = ((UsbRegRead (BaseAddr, DWC_XDCI_GHWPARAMS1_REG) &
                                      DWC_XDCI_GHWPARAMS1_NUM_INT_MASK) >>
                                     DWC_XDCI_GHWPARAMS1_NUM_INT_BIT_POS);

  DEBUG ((DEBUG_INFO, "Max dev int lines: %d\n", LocalCoreHandle->MaxDevIntLines));
  //
  // One event Buffer per interrupt line.
  //  Need to align it to size of event Buffer
  //  Buffer needs to be big enough. Otherwise the core
  //  won't operate
  //
  LocalCoreHandle->AlignedEventBuffers = (DWC_XDCI_EVENT_BUFFER *)
                                         ((UINT32) (UINTN) (LocalCoreHandle->EventBuffers) +
                                          ((sizeof (DWC_XDCI_EVENT_BUFFER) * DWC_XDCI_MAX_EVENTS_PER_BUFFER) -
                                              (((UINT32) (UINTN) (LocalCoreHandle->EventBuffers)) %
                                                  (sizeof (DWC_XDCI_EVENT_BUFFER) * DWC_XDCI_MAX_EVENTS_PER_BUFFER))));

  for (Idx = 0; Idx < LocalCoreHandle->MaxDevIntLines; Idx++) {
    UsbRegWrite (
      BaseAddr,
      DWC_XDCI_GEVNTADR_REG (Idx),
      (UINT32) (UINTN) (LocalCoreHandle->AlignedEventBuffers + Idx * sizeof (DWC_XDCI_EVENT_BUFFER) *
                        DWC_XDCI_MAX_EVENTS_PER_BUFFER)
      );

    //
    // Clear High 32bit address register, GEVNTADR register is 64-bit register
    // default is 0xffffffffffffffff
    //
    UsbRegWrite (BaseAddr, DWC_XDCI_GEVNTADR_REG (Idx) + 4, 0x00000000);

    LocalCoreHandle->CurrentEventBuffer = LocalCoreHandle->AlignedEventBuffers;
    //
    // Write size and clear the mask
    //
    UsbRegWrite (
      BaseAddr,
      DWC_XDCI_EVNTSIZ_REG (Idx),
      sizeof (DWC_XDCI_EVENT_BUFFER) * DWC_XDCI_MAX_EVENTS_PER_BUFFER
      );

    //
    // Write 0 to the event count register as the last step
    // for event configuration
    //
    UsbRegWrite (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (Idx), 0);

    DEBUG ((DEBUG_INFO, "Value of xDCI Event Buffer %d: %x, Size: %x, Count: %x\n",
            Idx,
            UsbRegRead (BaseAddr, DWC_XDCI_GEVNTADR_REG (Idx)),
            UsbRegRead (BaseAddr, DWC_XDCI_EVNTSIZ_REG (Idx)),
            UsbRegRead (BaseAddr, DWC_XDCI_EVNTCOUNT_REG (Idx))));
  }

  //
  // Program Global Control Register to disable scaledown,
  // disable clock gating
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GCTL_REG,
    ((UsbRegRead (BaseAddr, DWC_XDCI_GCTL_REG) &
      ~ (DWC_XDCI_GCTL_SCALE_DOWN_MODE_MASK + DWC_XDCI_GCTL_RAMCLKSEL_MASK + DWC_XDCI_GCTL_DISABLE_SCRAMB_MASK)) |
     DWC_XDCI_GCTL_DISABLE_CLK_GATING_MASK |
     (DWC_XDCI_GCTL_PRT_CAP_DEVICE << DWC_XDCI_GCTL_PRT_CAP_DIR_BIT_POS)));

  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_GCTL_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_GCTL_REG)));


  //
  // TODO: Program desired Speed and set LPM capable
  // We will do this when SuperSpeed works. For now,
  // force into High-Speed mode to aVOID anyone trying this
  // on Super Speed port
  //
#ifdef SUPPORT_SUPER_SPEED
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCFG_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DCFG_REG) & ~DWC_XDCI_DCFG_DESIRED_DEV_SPEED_MASK) | LocalCoreHandle->DesiredSpeed
    );
#else
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DCFG_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_DCFG_REG) & ~DWC_XDCI_DCFG_DESIRED_DEV_SPEED_MASK) | DWC_XDCI_DCFG_DESIRED_HS_SPEED
  );
#endif

  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_DCFG_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_DCFG_REG)));
  DEBUG ((DEBUG_INFO, "Setup value of xDCI DWC_XDCI_DSTS_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_DSTS_REG)));

  //
  // Enable Device Interrupt Events
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_DEVTEN_REG,
    DWC_XDCI_DEVTEN_DEVICE_INTS
    );

  //
  // Program the desired role
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GCTL_REG,
    (UsbRegRead (BaseAddr, DWC_XDCI_GCTL_REG) & ~DWC_XDCI_GCTL_PRT_CAP_DIR_MASK) | (LocalCoreHandle->Role <<
        DWC_XDCI_GCTL_PRT_CAP_DIR_BIT_POS)
    );

  //
  // Clear USB2 suspend for start new config command
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GUSB2PHYCFG_REG (0),
    (UsbRegRead (BaseAddr, DWC_XDCI_GUSB2PHYCFG_REG (0)) & ~DWC_XDCI_GUSB2PHYCFG_SUSPEND_PHY_MASK)
    );
  //
  // Clear USB3 suspend for start new config command
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_GUSB3PIPECTL_REG (0),
    (UsbRegRead (BaseAddr, DWC_XDCI_GUSB3PIPECTL_REG (0)) & ~DWC_XDCI_GUSB3PIPECTL_SUSPEND_PHY_MASK)
    );
  //
  // Issue DEPSTARTCFG command for EP0
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[0].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_NONE,
             EPCMD_START_NEW_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to init params for START_NEW_CONFIG EP command on xDCI\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             0,
             EPCMD_START_NEW_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue START_NEW_CONFIG EP command on xDCI\n"));
    return Status;
  }

  //
  // Issue DEPCFG command for EP0
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[0].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_INIT_STATE,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to init params for SET_EP_CONFIG command on xDCI for EP0\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             0,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams);

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue SET_EP_CONFIG command on xDCI for EP0\n"));
    return Status;
  }

  //
  // Issue DEPCFG command for EP1
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[1].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_INIT_STATE,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to init params for SET_EP_CONFIG command on xDCI for EP1\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             1,
             EPCMD_SET_EP_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue SET_EP_CONFIG command on xDCI for EP1\n"));
    return Status;
  }

  //
  // Issue DEPXFERCFG command for EP0
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[0].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_NONE,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO,
            "DwcXdciCoreInit: Failed to init params for EPCMD_SET_EP_XFER_RES_CONFIG command on xDCI for EP0\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             0,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue EPCMD_SET_EP_XFER_RES_CONFIG command on xDCI for EP0\n"));
    return Status;
  }

  //
  // Issue DEPXFERCFG command for EP1
  //
  Status = DwcXdciCoreInitEpCmdParams (
             LocalCoreHandle,
             &LocalCoreHandle->EpHandles[1].EpInfo,
             DWC_XDCI_PARAM0_SET_EP_CFG_ACTN_NONE,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO,
            "DwcXdciCoreInit: Failed to init params for EPCMD_SET_EP_XFER_RES_CONFIG command on xDCI for EP1\n"));
    return Status;
  }

  //
  // Issue the command
  //
  Status = DwcXdciCoreIssueEpCmd (
             LocalCoreHandle,
             1,
             EPCMD_SET_EP_XFER_RES_CONFIG,
             &EpCmdParams
             );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "DwcXdciCoreInit: Failed to issue EPCMD_SET_EP_XFER_RES_CONFIG command on xDCI for EP1\n"));
    return Status;
  }

  //
  // Prepare a Buffer for SETUP packet
  //
  LocalCoreHandle->Trbs = (DWC_XDCI_TRB *) (UINTN) ((UINT32) (UINTN)
                          LocalCoreHandle->UnalignedTrbs +
                          (DWC_XDCI_TRB_BYTE_ALIGNMENT -
                           ((UINT32) (UINTN)LocalCoreHandle->UnalignedTrbs %
                            DWC_XDCI_TRB_BYTE_ALIGNMENT)));

  DEBUG ((DEBUG_INFO, "(DwcXdciCoreInit)@@@@@@@@@ unalignedTrbs address is 0x%x\n", LocalCoreHandle->UnalignedTrbs));
  DEBUG ((DEBUG_INFO, "(DwcXdciCoreInit)@@@@@@@@@ TRB address is 0x%x\n", LocalCoreHandle->Trbs));

  //
  // Allocate Setup Buffer that is 8-byte aligned
  //
  LocalCoreHandle->AlignedSetupBuffer = LocalCoreHandle->DefaultSetupBuffer +
                                        (DWC_XDCI_SETUP_BUFF_SIZE -
                                         ((UINT32) (UINTN) (LocalCoreHandle->DefaultSetupBuffer) % DWC_XDCI_SETUP_BUFF_SIZE));

  //
  // Aligned Buffer for Status phase
  //
  LocalCoreHandle->AlignedMiscBuffer = LocalCoreHandle->MiscBuffer +
                                       (DWC_XDCI_SETUP_BUFF_SIZE -
                                        ((UINT32) (UINTN) (LocalCoreHandle->AlignedMiscBuffer) % DWC_XDCI_SETUP_BUFF_SIZE));

  //
  // We will queue SETUP request when we see bus reset
  //

  //
  // Enable Physical Endpoints 0
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EP_DALEPENA_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_EP_DALEPENA_REG) | (1 << 0)
    );

  //
  // Enable Physical Endpoints 1
  //
  UsbRegWrite (
    BaseAddr,
    DWC_XDCI_EP_DALEPENA_REG,
    UsbRegRead (BaseAddr, DWC_XDCI_EP_DALEPENA_REG) | (1 << 1)
    );

  DEBUG ((DEBUG_INFO, "Default value of xDCI DWC_XDCI_DEVTEN_REG: 0x%x\n", UsbRegRead (BaseAddr, DWC_XDCI_DEVTEN_REG)));
  return Status;


}

/**
  Interface:
  This function is used to flush all the FIFO.
  @param[in] CoreHandle  xDCI controller handle
  @param[in] EpInfo      EP info address

**/
EFI_STATUS
UsbXdciCoreFlushEpFifo (
  IN VOID           *CoreHandle,
  IN USB_EP_INFO    *EpInfo
  )
{
  EFI_STATUS  Status;
  UINT32      EpNum;

  Status = EFI_DEVICE_ERROR;

  if (CoreHandle == NULL) {
    DEBUG ((DEBUG_INFO, "DwcXdciEpCancelTransfer: INVALID handle\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Get physical EP num
  //
  EpNum = DwcXdciGetPhysicalEpNum (EpInfo->EpNum, EpInfo->EpDir);
  DwcXdciCoreFlushEpFifo (CoreHandle, EpNum);

  return Status;
}
