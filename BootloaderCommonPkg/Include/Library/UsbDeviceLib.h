/** @file

Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _USB_DEVICE_LIB_H_
#define _USB_DEVICE_LIB_H_

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Usb.h>
#include <Library/MemoryAllocationLib.h>

//
// USB standard descriptors and reqeust
//
typedef USB_DEVICE_REQUEST        EFI_USB_DEVICE_REQUEST;
typedef USB_DEVICE_DESCRIPTOR     EFI_USB_DEVICE_DESCRIPTOR;
typedef USB_CONFIG_DESCRIPTOR     EFI_USB_CONFIG_DESCRIPTOR;
typedef USB_INTERFACE_DESCRIPTOR  EFI_USB_INTERFACE_DESCRIPTOR;
typedef USB_ENDPOINT_DESCRIPTOR   EFI_USB_ENDPOINT_DESCRIPTOR;

#define MAX_DESCRIPTOR_SIZE         64
#define STRING_ARR_SIZE             (MAX_DESCRIPTOR_SIZE - 2)

#ifdef SUPPORT_SUPER_SPEED
//
// USB device capability Type Codes
// USB3 Table 9-13
//
typedef enum {
  WirelessUSB = 0x01,
  USB2Extension,
  SuperSpeedUSB,
  ContainerID,
  SuperSpeedPlusUSB = 0x0A
} USB_DEVICE_CAP_TYPE_CODE;
#endif


//
// USB device states from USB spec sec 9.1
//
typedef enum {
  UsbDevStateOff = 0,
  UsbDevStateInit,
  UsbDevStateAttached,
  UsbDevStatePowered,
  UsbDevStateDefault,
  UsbDevStateAddress,
  UsbDevStateConfigured,
  UsbDevStateSuspended,
  UsbDevStateError
} USB_DEVICE_STATE;


//
// The following set of structs are used during USB data transaction
// operatitions, including requests and completion events.
//
#pragma pack(1)

typedef struct {
  UINT32     EndpointNum;
  UINT8      EndpointDir;
  UINT8      EndpointType;
  UINT32     Length;
  VOID       *Buffer;
} EFI_USB_DEVICE_XFER_INFO;

//
// SuperSpeed Endpoint companion descriptor
// USB3 table 9-22
//
typedef struct {
  UINT8      Length;
  UINT8      DescriptorType;
  UINT8      MaxBurst;
  UINT8      Attributes;
  UINT16     BytesPerInterval;
} EFI_USB_ENDPOINT_COMPANION_DESCRIPTOR;

typedef struct {
  EFI_USB_ENDPOINT_DESCRIPTOR              *EndpointDesc;
  EFI_USB_ENDPOINT_COMPANION_DESCRIPTOR    *EndpointCompDesc;
} USB_DEVICE_ENDPOINT_INFO, USB_DEVICE_ENDPOINT_OBJ;

typedef struct {
  VOID        *Buffer;
  UINT32      Length;
} USB_DEVICE_IO_INFO;

typedef struct {
  USB_DEVICE_IO_INFO           IoInfo;
  USB_DEVICE_ENDPOINT_INFO     EndpointInfo;
} USB_DEVICE_IO_REQ;

//
// Optional string descriptor
//
typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  UINT16          LangID[STRING_ARR_SIZE];
} USB_STRING_DESCRIPTOR;


//
// The following structures abstract the device descriptors a class
// driver needs to provide to the USBD core.
// These structures are filled & owned by the class/function layer.
//
typedef struct {
  EFI_USB_INTERFACE_DESCRIPTOR         *InterfaceDesc;
  USB_DEVICE_ENDPOINT_OBJ              *EndpointObjs;
} USB_DEVICE_INTERFACE_OBJ;

typedef struct {
  EFI_USB_CONFIG_DESCRIPTOR     *ConfigDesc;
  VOID                          *ConfigAll;
  USB_DEVICE_INTERFACE_OBJ      *InterfaceObjs;
} USB_DEVICE_CONFIG_OBJ;

#ifdef SUPPORT_SUPER_SPEED
//
// SuperSpeed Binary Device Object Store(BOS) descriptor
// USB3 9.6.2
//
typedef struct {
  UINT8      Length;
  UINT8      DescriptorType;
  UINT16     TotalLength;
  UINT8      NumDeviceCaps;
} EFI_USB_BOS_DESCRIPTOR;

//
// Generic Header of Device Capability descriptor
// USB3 9.6.2.2
//
typedef struct {
  UINT8      Length;
  UINT8      DescriptorType;
  UINT8      DevCapabilityType;
  UINT8      CapDependent;
} EFI_USB_SS_DEVICE_CAP_DESCRIPTOR;

//
// USB2.0 Extension descriptor
// USB3 Table 9-14
//
typedef struct {
  UINT8      Length;
  UINT8      DescriptorType;
  UINT8      DeviceCapabilityType;
  UINT32     Attributes;
} EFI_USB_USB2_EXT_CAP_DESCRIPTOR;


//
// SuperSpeed USB Device Capability descriptor
// USB3 Table 9-15
//
typedef struct {
  UINT8      Length;
  UINT8      DescriptorType;
  UINT8      DeviceCapabilityType;
  UINT8      Attributes;
  UINT16     SpeedSupported;
  UINT8      FunctionalitySupport;
  UINT8      U1DevExitLat;
  UINT16     U2DevExitLat;
} EFI_USB_SS_USB_DEV_CAP_DESCRIPTOR;

//
// Container ID descriptor
// USB3 Table 9-16
//
typedef struct {
  UINT8      Length;
  UINT8      DescriptorType;
  UINT8      DeviceCapabilityType;
  UINT8      Reserved;
  UINT8      UUID[16];
} EFI_USB_CONTAINER_ID_DESCRIPTOR;

//
// Container ID descriptor
// USB3 Table 9-16
//
typedef struct {
  UINT8      Length;
  UINT8      DescriptorType;
  UINT8      DeviceCapabilityType;
  UINT8      ReservedByte;
  UINT32     Attributes;
  UINT16     FunctionalitySupport;
  UINT16     ReservedWord;
  UINT32     SublinkSpeedAttr[2];
} EFI_USB_SS_PLUS_USB_DEV_CAP_DESCRIPTOR;

#endif

typedef
EFI_STATUS
(EFIAPI *EFI_USB_CONFIG_CALLBACK) (
  IN UINT8                      CfgVal
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_SETUP_CALLBACK) (
  IN EFI_USB_DEVICE_REQUEST     *CtrlRequest,
  IN USB_DEVICE_IO_INFO         *IoInfo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DATA_CALLBACK) (
  IN EFI_USB_DEVICE_XFER_INFO   *XferInfo
  );

typedef struct {
  USB_DEVICE_DESCRIPTOR       *DeviceDesc;
  USB_DEVICE_CONFIG_OBJ       *ConfigObjs;
  USB_STRING_DESCRIPTOR       *StringTable;
#ifdef SUPPORT_SUPER_SPEED
  EFI_USB_BOS_DESCRIPTOR      *BosDesc;
#endif
  UINT8                       StrTblEntries;
  EFI_USB_CONFIG_CALLBACK     ConfigCallback;
  EFI_USB_SETUP_CALLBACK      SetupCallback;
  EFI_USB_DATA_CALLBACK       DataCallback;
} USB_DEVICE_OBJ;


//
// Main USBD driver object structure containing all data necessary
// for USB device mode processing at this layer
//
typedef struct {
  USB_DEVICE_OBJ              *UsbdDevObj;      /* pointer to a Device Object */
  VOID                        *XdciDrvObj;      /* Opaque handle to XDCI driver */
  BOOLEAN                     XdciInitialized;  /* flag to specify if the XDCI driver is initialized */
  USB_DEVICE_CONFIG_OBJ       *ActiveConfigObj; /* pointer to currently active configuraiton */
  USB_DEVICE_STATE            State;            /* current state of the USB Device state machine */
  UINT8                       Address;          /* configured device address */
} USB_DEVICE_DRIVER_OBJ;

typedef struct {
  UINT8       Bus;
  UINT8       Device;
  UINT8       Func;
  UINT8       Reserved;
} PCI_ADDRESS;

typedef struct {
  PCI_ADDRESS        XhciDeviceAddress;
  PCI_ADDRESS        XdciDeviceAddress;
} USB_DEVICE_PLATFORM_INFO;
#pragma pack()


/**
  This function performs device initialization.

  @param[in,out] Prot                   USB device mode protocol

  @retval EFI_SUCCESS                   Successfully polled the value.
  @retval EFI_TIMEOUT                   Timeout while polling the value.
**/
EFI_STATUS
EFIAPI
UsbDeviceInitialization (
  IN OUT       VOID          **Prot
  );

#endif
