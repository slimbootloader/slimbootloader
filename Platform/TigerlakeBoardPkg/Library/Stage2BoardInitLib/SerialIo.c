/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/DebugLib.h>
#include <FspsUpd.h>
#include <Library/PchInfoLib.h>
#include <PlatformData.h>
#include <PchAccess.h>
#include <PchLimits.h>
#include "SerialIo.h"

/**
  Update Serial IO for FSP-S UPD

  @param  FspsConfig            The pointer to the FSP-S UPD to be updated.
**/
VOID
EFIAPI
SerialIoPostMemConfig (
  FSP_S_CONFIG  *FspsConfig
)
{
  UINT8               DebugPort;

  DebugPort = GetDebugPort ();
  if (DebugPort < PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    // Inform FSP to skip debug UART init
    FspsConfig->SerialIoDebugUartNumber = DebugPort;
    FspsConfig->SerialIoUartMode[DebugPort] = 0x4;
  } else if (S0IX_STATUS() == 1) {  // legacy UART
    FspsConfig->SerialIoUartMode[2] = 1;  // Force UART to PCI mode to enable OS to have full control
  }

  // Two instants of UART0 in TGL PCH: GPP_C8 to GPP_C11 and GPP_F0 to GPP_F3, while the second
  // instant can be used by CNVI.
  if (IsPchLp ()) {
    FspsConfig->SerialIoUartRxPinMuxPolicy[0] = GPIO_VER2_LP_MUXING_SERIALIO_UART0_RXD_GPP_C8;
    FspsConfig->SerialIoUartTxPinMuxPolicy[0] = GPIO_VER2_LP_MUXING_SERIALIO_UART0_TXD_GPP_C9;
    FspsConfig->SerialIoUartRtsPinMuxPolicy[0] = GPIO_VER2_LP_MUXING_SERIALIO_UART0_RTS_GPP_C10;
    FspsConfig->SerialIoUartCtsPinMuxPolicy[0] = GPIO_VER2_LP_MUXING_SERIALIO_UART0_CTS_GPP_C11;
  } else if (IsPchH ()) {
    FspsConfig->SerialIoUartRxPinMuxPolicy[0] = GPIO_VER2_H_MUXING_SERIALIO_UART0_RXD_GPP_C8;
    FspsConfig->SerialIoUartTxPinMuxPolicy[0] = GPIO_VER2_H_MUXING_SERIALIO_UART0_TXD_GPP_C9;
    FspsConfig->SerialIoUartRtsPinMuxPolicy[0] = GPIO_VER2_H_MUXING_SERIALIO_UART0_RTS_GPP_C10;
    FspsConfig->SerialIoUartCtsPinMuxPolicy[0] = GPIO_VER2_H_MUXING_SERIALIO_UART0_CTS_GPP_C11;
  } else {
    DEBUG ((DEBUG_ERROR, "Unsupported PCH to initialize GPIO pins for UART0\n"));
  }
}
