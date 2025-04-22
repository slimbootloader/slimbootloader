/** @file
  IOC IPC library

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __IOC_IPC_LIB_H__
#define __IOC_IPC_LIB_H__

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>

#define IOC_IPC_PACKET_LENGTH_MAX   256

typedef enum {
  IocIpcDeviceTypeUart,
  IocIpcDeviceTypeMax
} IOC_IPC_DEVICE_TYPE;

typedef struct {
  UINT32                BaseAddr;
  UINT32                BaudRate;
  UINT32                InputHertz;
  UINT32                RegWidth;
} IOC_IPC_UART_DEVICE;

typedef union {
  IOC_IPC_UART_DEVICE   IpcDeviceUart;
} IOC_IPC_DEVICE;

typedef struct {
  UINT8                 Retries;
  UINT8                 TimeoutInitial;
  UINT8                 TimeoutXmit;
} IOC_IPC_MISC;

typedef struct {
  IOC_IPC_DEVICE_TYPE   IpcDeviceType;
  IOC_IPC_DEVICE        IpcDevice;
  IOC_IPC_MISC          IpcMisc;
} IOC_IPC_CONFIG_DATA;

/**
  This function initializes IPC interface

  @param[in]  IpcDevice     Specifies Ipc Interface Device

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
typedef
EFI_STATUS
(EFIAPI *IPC_FUNC_INIT) (
  IN  IOC_IPC_DEVICE    *IpcDevice
  );

/**
  This function sends a single character via IPC interface

  @param[in]  IpcDevice     Specifies Ipc Interface Device
  @param[in]  Ch            Specifies a character to be sent

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
typedef
EFI_STATUS
(EFIAPI *IPC_FUNC_SEND) (
  IN  IOC_IPC_DEVICE    *IpcDevice,
  IN  UINT8             Ch
  );

/**
  This function receives data via IPC interface

  @param[in]  IpcDevice     Specifies Ipc Interface Device
  @param[out] Buffer        Memory address to store the received data
  @param[in]  Len           Length of characters to be received
  @param[in]  Timeout       Timeout in millisecond

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
typedef
EFI_STATUS
(EFIAPI *IPC_FUNC_RECV) (
  IN  IOC_IPC_DEVICE    *IpcDevice,
  OUT CHAR8             *Buffer,
  IN  UINT32            Len,
  IN  UINT32            Timeout
  );

/**
  This function resets IPC transmission

  @param[in]  IpcDevice     Specifies Ipc Interface Device

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
typedef
EFI_STATUS
(EFIAPI *IPC_FUNC_RESET) (
  IN  IOC_IPC_DEVICE    *IpcDevice
  );

typedef struct {
  IPC_FUNC_INIT         IpcInitialize;
  IPC_FUNC_SEND         IpcSend;
  IPC_FUNC_RECV         IpcReceive;
  IPC_FUNC_RESET        IpcReset;
} IOC_IPC_FUNC;

/**
  The function gets config data from IOC.

  Based on IpcConfig, this function will query config data from IOC.

  @param[in]  IpcConfig   IPC config information
  @param[out] Buffer      Memory address to store the received data

  @retval The number of bytes received from IOC

**/
UINT32
EFIAPI
QueryConfigDataFromIoc (
  IN  IOC_IPC_CONFIG_DATA *IpcConfig,
  OUT CHAR8               *Buffer
  );

/**
  This will trigger system reset via IOC.

  @param[in]  IpcConfig   IPC config information
  @param[in]  ResetType   Cold, Warm or Shutdown

**/
VOID
RequestResetSystemToIoc (
  IN  IOC_IPC_CONFIG_DATA  *IpcConfig,
  IN  EFI_RESET_TYPE        ResetType
  );

#endif /* __IOC_IPC_LIB_H__ */
