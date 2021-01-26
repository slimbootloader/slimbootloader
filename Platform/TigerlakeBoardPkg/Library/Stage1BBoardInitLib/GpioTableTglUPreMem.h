/** @file
  Tigerlake U RVP GPIO definition table for Pre-Memory Initialization

  Copyright (c) 2018 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TIGERLAKE_U_PREMEM_GPIO_TABLE_H_
#define _TIGERLAKE_U_PREMEM_GPIO_TABLE_H_

#include <GpioPinsVer2Lp.h>
#include <Library/GpioLib.h>

static GPIO_INIT_CONFIG mGpioTablePreMemTglUDdr4[] =
{
  { GPIO_VER2_LP_GPP_A14, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut, GpioOutLow, GpioIntDis, GpioPlatformReset, GpioTermNone } },  //ONBOARD_X4_PCIE_SLOT1_PWREN_N
  { GPIO_VER2_LP_GPP_C13, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut, GpioOutHigh, GpioIntDis, GpioPlatformReset, GpioTermNone } },  //ONBOARD_X4_PCIE_SLOT1_RESET_N
  // CPU M.2 SSD
  { GPIO_VER2_LP_GPP_D16, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut, GpioOutHigh, GpioIntDis, GpioPlatformReset, GpioTermNone } },  //CPU SSD PWREN
  { GPIO_VER2_LP_GPP_A11, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut, GpioOutHigh, GpioIntDis, GpioPlatformReset, GpioTermNone } },  //CPU SSD RESET
  // X1 Pcie Slot
  {GPIO_VER2_LP_GPP_F9,   { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut, GpioOutHigh, GpioIntDis,GpioPlatformReset,  GpioTermNone}},  //X1 Slot PWREN
  {GPIO_VER2_LP_GPP_A23,  { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut, GpioOutLow,  GpioIntDis,GpioPlatformReset,  GpioTermNone}},  //TC_RETIMER_FORCE_PWR
};

#endif // _TIGERLAKE_U_PREMEM_GPIO_TABLE_H_
