/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _XDCI_UTILITY_H_
#define _XDCI_UTILITY_H_

#include <Library/UsbDeviceLib.h>
#include "UsbDeviceLibPrivate.h"

/**
  This function prints device parameters.
  @param[in]      DevDesc    Pointer to device descriptor.

**/
VOID
PrintDeviceDescriptor (
  IN USB_DEVICE_DESCRIPTOR    *DevDesc
  );

/**
  This function prints configuration description parameters.
  @param[in]      ConfigDesc    Pointer to configuration descriptor.

**/
VOID
PrintConfigDescriptor (
  IN EFI_USB_CONFIG_DESCRIPTOR    *ConfigDesc
  );

/**
  This function prints Interface description parameters.
  @param[in]      IfDesc    Pointer to interface descriptor.

**/
VOID
PrintInterfaceDescriptor (
  IN EFI_USB_INTERFACE_DESCRIPTOR    *IfDesc
  );

/**
  This function prints Endpoint description parameters.
  @param[in]      EpDesc    Pointer to EndPoint descriptor.

**/
VOID
PrintEpDescriptor (
  IN EFI_USB_ENDPOINT_DESCRIPTOR    *EpDesc
  );

/**
  This function prints Endpoint Companion description parameters.
  @param[in]      EpDesc    Pointer to EndPoint descriptor.

**/
VOID
PrintEpCompDescriptor (
  IN EFI_USB_ENDPOINT_COMPANION_DESCRIPTOR    *EpDesc
  );

/**
  This function prints string description parameters.
  @param[in]      StrDesc    Pointer to string descriptor.

**/
VOID
PrintStringDescriptor (
  IN USB_STRING_DESCRIPTOR    *StrDesc
  );

/**
  This function prints Device Request parameters.
  @param[in]      DevReq    Pointer to device descriptor.

**/
VOID
PrintDeviceRequest (
  IN EFI_USB_DEVICE_REQUEST    *DevReq
  );

/**
  This function prints BOS parameters.
  @param[in]      BosDesc    Pointer to BOS descriptor.

**/
#ifdef SUPPORT_SUPER_SPEED
VOID
PrintBOSDescriptor (
  IN EFI_USB_BOS_DESCRIPTOR    *BosDesc
  );
#endif

#endif

