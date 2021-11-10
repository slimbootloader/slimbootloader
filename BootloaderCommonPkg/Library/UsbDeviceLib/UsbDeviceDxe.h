/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __USB_DEVICE_DXE_H__
#define __USB_DEVICE_DXE_H__

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Protocol/EfiUsbFnIo.h>
#include <Protocol/UsbDeviceModeProtocol.h>
#include "UsbFuncIo.h"
#include "UsbDeviceMode.h"


#define PCI_IF_USBDEV                      0xFE

#define EFI_USB_DEV_SIGNATURE              0x55534244 //"USBD"
#define USBFUIO_CONTEXT_FROM_PROTOCOL(a)   CR (a, USB_XDCI_DEV_CONTEXT, UsbFunIoProtocol, EFI_USB_DEV_SIGNATURE)
#define USBUSBD_CONTEXT_FROM_PROTOCOL(a)   CR (a, USB_XDCI_DEV_CONTEXT, UsbDevModeProtocol, EFI_USB_DEV_SIGNATURE)


typedef struct _USB_FUIO_EVENT_NODE   USB_FUIO_EVENT_NODE;

#pragma pack(1)
struct _USB_FUIO_EVENT_NODE {
  EFI_USBFN_MESSAGE            Message;
  UINTN                        PayloadSize;
  EFI_USBFN_MESSAGE_PAYLOAD    Payload;
  USB_FUIO_EVENT_NODE          *Nextptr;
};

typedef struct {
  UINTN                         Signature;
  UINTN                         XdciMmioBarAddr;
  EFI_HANDLE                    XdciHandle;
  //
  // Timer to handle EndPoint event periodically.
  //
  EFI_EVENT                     XdciPollTimer;
  EFI_USB_DEVICE_MODE_PROTOCOL  UsbDevModeProtocol;
  EFI_USBFN_IO_PROTOCOL         UsbFunIoProtocol;

  //
  // Structure members used by UsbFunIoProtocol.
  //
  USB_MEM_NODE                  *FirstNodePtr;
  EFI_USB_DEVICE_INFO           *DevInfoPtr;
  EFI_USB_CONFIG_INFO           IndexPtrConfig;
  EFI_USB_INTERFACE_INFO        IndexPtrInteface;
  USB_DEVICE_ENDPOINT_INFO      IndexPtrInEp;
  USB_DEVICE_ENDPOINT_INFO      IndexPtrOutEp;
  XDCI_CORE_HANDLE              *XdciDrvIfHandle;
  USB_DEV_CORE                  *DrvCore;
  UINT16                        VendorId;
  UINT16                        DeviceId;
  USBD_EP_XFER_REC              EndPointXferRec[DWC_XDCI_MAX_ENDPOINTS];
  BOOLEAN                       StartUpController;
  BOOLEAN                       DevReConnect;
  BOOLEAN                       DevResetFlag;
  EFI_EVENT                     TimerEvent;
  USB_FUIO_EVENT_NODE           *EventNodePtr;
  //
  // Following structure members are used by UsbDevModeProtocol.
  //

} USB_XDCI_DEV_CONTEXT;
#pragma pack()

EFI_STATUS
EFIAPI
GetUsbPortType (
  EFI_USBFN_PORT_TYPE    *PortType
  );

VOID
EFIAPI
PlatformSpecificInit (
  VOID
  );

#if 0
extern EFI_COMPONENT_NAME_PROTOCOL    mUsbDeviceDxeComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL   mUsbDeviceDxeComponentName2;

#endif

extern USB_XDCI_DEV_CONTEXT     *mUsbXdciDevContext;
extern USB_DEVICE_PLATFORM_INFO     mUsbDeviceInfo;

#define MmPciAddress( Segment, Bus, Device, Function, Register ) \
  ( (UINTN)PcdGet64(PcdPciExpressBaseAddress) + \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )

///
/// xHCI MMIO registers
///
#define R_XHCI_MEM_DUAL_ROLE_CFG0       0x80D8
#define B_XHCI_EN_PIPE4_SYNC            BIT30 ///< enable/disable PIPE 4.1 synchronous phystatus
#define R_XHCI_MEM_DUAL_ROLE_CFG1       0x80DC
#define R_XHCI_MEM_BASE                 0x10  ///< Memory Base Address
#define B_XHCI_MEM_BASE_BA              0xFFFFFFFFFFFF0000 ///< Base Address

#define R_OTG_BAR0                      0x10  ///< BAR 0
#define B_OTG_BAR0_BA                   0xFFE00000 ///< Base Address

#endif
