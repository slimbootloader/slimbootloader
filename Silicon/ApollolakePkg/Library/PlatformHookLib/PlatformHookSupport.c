/** @file
  The platform hook library.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <RegAccess.h>
#include <PlatformHookSupport.h>
#include <GpioDefines.h>
#include <Library/GpioLib.h>
#include <Library/GpioLibApl.h>
#include <Library/BootloaderCommonLib.h>

typedef struct {
  UINT16  Community   : 4;
  UINT16  PadNum      : 8;
  UINT16  Term        : 4;
} BOOT_MODE_GPIO_PIN_IN;

typedef struct {
  UINT16  Community   : 4;
  UINT16  PadNum      : 8;
  UINT16  GpioTxState : 1;
  UINT16  Rsvd        : 3;
} BOOT_MODE_GPIO_PIN_OUT;

typedef union {
  BOOT_MODE_GPIO_PIN_IN   GpioPinIn;
  BOOT_MODE_GPIO_PIN_OUT  GpioPinOut;
  UINT16                  Uint16;
} BOOT_MODE_GPIO_PIN;

typedef struct {
  UINT16              Invert;
  UINT16              Mask;
  UINT16              Alignment;
  BOOT_MODE_GPIO_PIN  Out;
  BOOT_MODE_GPIO_PIN  In[0];
} BOOT_MODE_GPIO;

CONST
UINT16 mUartRxGpioOffset[] = {
  0x0130, 0x0150, 0x0170, 0x0218
};

/**
  Get UART BAR for a specified port

  @param[in]  Port             Specifies a UART port

  @retval                      Return UART base address

**/
UINTN
EFIAPI
GetUartBaseAddress (
  IN  UINT8 Port
  )
{
  UINTN                 PciUartMmBase;

  PciUartMmBase = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART0 + Port, 0);

  if ((MmioRead8 (PciUartMmBase + R_LPSS_IO_STSCMD) & 0x02) != 0x02) {
    DEBUG ((DEBUG_ERROR, "UART%d was not configured properly!", Port));
    return 0xFFFFFFFF;
  }

  return MmioRead32 (PciUartMmBase + R_LPSS_IO_BAR) & 0xFFFFFFF0;
}

/**
  Performs UART gpio for IOC communication

  @param[in]  Port             Specifies a UART port

  @retval RETURN_SUCCESS       The UART gpio initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The UART gpio initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
UartGpioInitialize (
  IN  UINT8 Port
  )
{
  BXT_CONF_PAD0           PadConfig0;
  BXT_CONF_PAD1           PadConfig1;
  UINT16                  Offset;

  //
  // GPIO RX
  //
  Offset = GPIO_PADBAR + mUartRxGpioOffset[Port];
  PadConfig0.padCnf0 = GpioRead (NORTH, Offset + BXT_GPIO_PAD_CONF0_OFFSET);
  if (Port == 3) {
    PadConfig0.r.PMode = M2;
  } else {
    PadConfig0.r.PMode = M1;
  }
  PadConfig0.r.GPIORxState = 1;
  GpioWrite (NORTH, Offset + BXT_GPIO_PAD_CONF0_OFFSET, PadConfig0.padCnf0);
  PadConfig1.padCnf1 = GpioRead (NORTH, Offset + BXT_GPIO_PAD_CONF1_OFFSET);
  PadConfig1.r.Term = 0xC;
  GpioWrite (NORTH, Offset + BXT_GPIO_PAD_CONF1_OFFSET, PadConfig1.padCnf1);

  //
  // GPIO TX
  //
  Offset = GPIO_PADBAR + mUartRxGpioOffset[Port] + GPIO_REGS_SIZE;
  PadConfig0.padCnf0 = GpioRead (NORTH, Offset + BXT_GPIO_PAD_CONF0_OFFSET);
  if (Port == 3) {
    PadConfig0.r.PMode = M2;
  } else {
    PadConfig0.r.PMode = M1;
  }
  GpioWrite (NORTH, Offset + BXT_GPIO_PAD_CONF0_OFFSET, PadConfig0.padCnf0);
  PadConfig1.padCnf1 = GpioRead (NORTH, Offset + BXT_GPIO_PAD_CONF1_OFFSET);
  PadConfig1.r.Term = 0xC;
  GpioWrite (NORTH, Offset + BXT_GPIO_PAD_CONF1_OFFSET, PadConfig1.padCnf1);

  return RETURN_SUCCESS;
}
