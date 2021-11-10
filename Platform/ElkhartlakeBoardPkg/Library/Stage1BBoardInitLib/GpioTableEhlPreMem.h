/** @file
  Tigerlake U RVP GPIO definition table for Pre-Memory Initialization

 Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _ELKHARTLAKE_PREMEM_GPIO_TABLE_H_
#define _ELKHARTLAKE_PREMEM_GPIO_TABLE_H_

#include <Register/GpioPinsVer3.h>
#include <GpioConfig.h>

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTablePreMemEhl[] =
{
  {GPIO_VER3_GPP_E0,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,    GpioIntDis,                   GpioPlatformReset,  GpioTermNone}},  //WWAN_FCP_OFF_N
  {GPIO_VER3_GPP_F21, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,    GpioIntDis,                   GpioPlatformReset,  GpioTermNone}},  //WWAN_PWREN
  {GPIO_VER3_GPP_B14, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,     GpioIntDis,                   GpioPlatformReset,  GpioTermNone}},  //WWAN_PERST_N
  {GPIO_VER3_GPP_B17, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci,    GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock}},  //WWAN_WAKE_N
  {GPIO_VER3_GPP_V13, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,     GpioIntDis,                   GpioPlatformReset,  GpioTermNone}},  //WWAN_RST_N
  {GPIO_VER3_GPD2,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntDis,                   GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}}, // LAN_WAKEB
  {GPIO_VER3_UFS_RESETB, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,    GpioIntDis,                   GpioPlatformReset,  GpioTermNone}},  // UFS_RESETB
};

/**
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTableEhlWwanOnEarlyPreMem[] =
{
  {GPIO_VER3_GPP_F21,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,     GpioIntDis,               GpioResumeReset,    GpioTermNone,  GpioOutputStateUnlock}},  //WWAN_PWREN
  {GPIO_VER3_GPP_E0,   { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,     GpioIntDis,               GpioResumeReset,    GpioTermNone}},                          //WWAN_FCP_OFF_N
  {GPIO_VER3_GPP_V13,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,     GpioIntDis,               GpioResumeReset,    GpioTermNone}},                          //WWAN_RST_N
  {GPIO_VER3_GPP_B14,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,     GpioIntDis,               GpioResumeReset,    GpioTermNone}},                          //WWAN_PERST_N
  {GPIO_VER3_GPP_B17,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,  GpioOutDefault,  GpioIntLevel|GpioIntSci,  GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock}},    //WWAN_WAKE_N
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTableEhlWwanOffEarlyPreMem[] =
{
  {GPIO_VER3_GPP_V13,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,    GpioOutLow,      GpioIntDis,                GpioResumeReset,    GpioTermNone}},                          //WWAN_RST_N
  {GPIO_VER3_GPP_B14,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,    GpioOutLow,      GpioIntDis,                GpioResumeReset,    GpioTermNone}},                          //WWAN_PERST_N
  {GPIO_VER3_GPP_B17,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,  GpioOutDefault,  GpioIntLevel | GpioIntSci, GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock}},    //WWAN_WAKE_N
  {GPIO_VER3_GPP_E0,   { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,    GpioOutLow,      GpioIntDis,                GpioResumeReset,    GpioTermNone}},                          //WWAN_FCP_OFF_N
};
**/
#endif // _ELKHARTLAKE_U_PREMEM_GPIO_TABLE_H_
