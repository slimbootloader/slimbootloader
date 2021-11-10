/** @file
  Platform hook support

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PLATFORM_HOOK_SUPPORT_H__
#define __PLATFORM_HOOK_SUPPORT_H__


/**
  Get boot mode value from config data

  @param[in]  Cdata             Config data address
  @param[in]  CdataLen          The length of config data
  @param[out] BootModeOption    Parsed boot mode value

  @retval RETURN_SUCCESS        Getting boot mode value succeeded.
  @retval RETURN_DEVICE_ERROR   Getting boot mode value could not be completed.

**/
RETURN_STATUS
EFIAPI
GetBootModeOption (
  IN  VOID    *Cdata,
  IN  UINT32   CdataLen,
  OUT UINT16  *BootModeOption
  );

/**
  Get UART BAR for a specified port

  @param[in]  Port              Specifies a UART port

  @retval                       Return UART base address

**/
UINTN
EFIAPI
GetUartBaseAddress (
  IN  UINT8   Port
  );

/**
  Performs UART gpio for IOC communication

  @param[in]  Port              Specifies a UART port

  @retval RETURN_SUCCESS        The UART gpio initialization succeeded.
  @retval RETURN_DEVICE_ERROR   The UART gpio initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
UartGpioInitialize (
  IN  UINT8   Port
  );

/**
  Performs UART config space initialization

  @param[in]  Port              Specifies a UART port

  @retval RETURN_SUCCESS        The UART config space initialization succeeded.
  @retval RETURN_DEVICE_ERROR   The UART config space initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
UartPortInitialize (
  IN  UINT8   Port
  );

#endif /* __PLATFORM_HOOK_SUPPORT_H__ */
