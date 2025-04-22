/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <GpioConfig.h>

EFI_STATUS
EFIAPI
GpioConfigurePads (
  IN UINT32                    GpioPinNum,
  IN VOID                     *GpioConfData
  )
/*++

Routine Description:

  Set GPIO PAD DW 0 and 1 registers for S/W/SW/N/NW GPIO communities

Arguments:

  Gpio_Pin_Num               - GPIO Pin Number to configure
  Gpio_Conf_Data            - GPIO_CONF_PAD_INIT data array for each GPIO communities.

--*/
{
  UINT32   *Pad0;
  UINT32   *Pad1;
  UINT32    Index;

  Pad0 = (UINT32 *)GpioConfData;
  for (Index  = 0; Index < GpioPinNum; Index++) {
    Pad1 = Pad0 + 1;
    DEBUG ((DEBUG_INFO, "GPIO GPP_A%02d DATA: 0x%08X 0x%08X\n", (*Pad1) & 0x0F, *Pad0, *Pad1));
    Pad0 = Pad1 + 1;
  }

  return EFI_SUCCESS;
}


