/** @file
  Tigerlake H RVP GPIO definition table for Pre-Memory Initialization

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TIGERLAKE_H_PREMEM_GPIO_TABLE_H_
#define _TIGERLAKE_H_PREMEM_GPIO_TABLE_H_

#include <GpioPinsVer2Lp.h>
#include <Library/GpioLib.h>

static GPIO_INIT_CONFIG mGpioTablePreMemTglHDdr4[] =
{
  {GPIO_VER2_H_GPP_I9,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PERST_N_M2_SLOTB_N
  {GPIO_VER2_H_GPP_F3,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //TC_RETIMER_FORCE_PWR
  //
  // Dp-In GPIO settings
  //
  {GPIO_VER2_H_GPP_H6,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDefault, GpioPlatformReset, GpioTermNone } },//DP_IN Detect Port 1 for BoardIdTglHDdr4SODimm
  {GPIO_VER2_H_GPP_H7,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDefault, GpioPlatformReset, GpioTermNone } },//DP_IN Detect Port 1 for BoardIdTglHDdr4SODimm
  {GPIO_VER2_H_GPP_H8,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDefault, GpioPlatformReset, GpioTermNone } },//DP_IN Detect Port 1 for BoardIdTglHDdr4SODimm
  {GPIO_VER2_H_GPP_H9,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDefault, GpioPlatformReset, GpioTermNone } },//DP_IN Detect Port 1 for BoardIdTglHDdr4SODimm
};
#endif // _TIGERLAKE_U_PREMEM_GPIO_TABLE_H_
