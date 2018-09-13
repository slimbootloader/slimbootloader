/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/SiGpioLib.h>

VOID
GpioPadConfigTable (
  IN UINT32              GpioPinNum,
  IN VOID*               GpioConfData
)
/*++

Routine Description:

  Set GPIO PAD DW 0 and 1 registers for S/W/SW/N/NW GPIO communities

Arguments:

  Gpio_Pin_Num               - GPIO Pin Number to configure
  Gpio_Conf_Data            - GPIO_CONF_PAD_INIT data array for each GPIO communities.

--*/
{
  GPIO_INIT_CONFIG  *GpioInitConf;
  UINT32             Index;

  GpioInitConf = (GPIO_INIT_CONFIG *)GpioConfData;
  for (Index  = 0; Index < GpioPinNum; Index++) {
    DEBUG ((DEBUG_INFO, "GPIO GPP_A%02d DATA: 0x%08X 0x%08X\n", GpioInitConf->GpioPad1 & 0x0F, GpioInitConf->GpioPad0, GpioInitConf->GpioPad1));
    GpioInitConf++;
  }
}


