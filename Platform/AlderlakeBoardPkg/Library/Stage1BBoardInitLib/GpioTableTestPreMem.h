/** @file
  RVP GPIO definition table for Pre-Memory Initialization

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _TEST_S_RVP_PREMEM_GPIO_TABLE_H_
#define _TEST_S_RVP_PREMEM_GPIO_TABLE_H_

#include <GpioPinsVer4S.h>
#include <GpioConfig.h>

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTableEarlyPreMemTestSDdr5UDimm1DRvp[] =
{
  {GPIO_VER4_S_GPP_F3,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResetDefault,   GpioTermNone }}, //PEG_SLOT_PWREN
  {GPIO_VER4_S_GPP_E2,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //CPU PEG Slot1 Reset

  {GPIO_VER4_S_GPP_H16,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //CPU M.2 SSD Slot Power enable

  {GPIO_VER4_S_GPP_E1,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCIE SLOT 1 - X4 CONNECTOR Power enable
  {GPIO_VER4_S_GPP_F11,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCIE SLOT 1 - X4 CONNECTOR Reset

  {GPIO_VER4_S_GPP_H11,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCIE SLOT 2 - X4 CONNECTOR Power enable
  {GPIO_VER4_S_GPP_F12,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCIE SLOT 2 - X4 CONNECTOR Reset

  {GPIO_VER4_S_GPP_B21,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCIE SLOT 3 - X2 CONNECTOR Power enable
  {GPIO_VER4_S_GPP_F13,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCIE SLOT 3 - X2 CONNECTOR Reset

  {GPIO_VER4_S_GPP_K11,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCH M.2 SSD Slot 1 Power enable
//{GPIO_VER4_S_GPP_C10,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCH M.2 SSD Slot 1 Reset

  {GPIO_VER4_S_GPP_B22,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCH M.2 SSD Slot 2 Power enable
//{GPIO_VER4_S_GPP_F16,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCH M.2 SSD Slot 2 Reset

  {GPIO_VER4_S_GPP_K2,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCH M.2 SSD Slot 3 Power enable
//{GPIO_VER4_S_GPP_B6,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioPlatformReset,  GpioTermNone }}, //PCH M.2 SSD Slot 3 Reset

  {GPIO_VER4_S_GPP_G1,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioHostDeepReset,  GpioTermNone }}, //Thunderbolt CIO_PWR_EN (CIO_PWR_EN needs set to high before PCIe SLOT_2 PERST set to high; at least 10ms)
  {GPIO_VER4_S_GPP_F5,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioHostDeepReset,  GpioTermNone }}, //TBT_RTD3_PWR_EN

  {GPIO_VER4_S_GPP_H0,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone }}, //VAL_SV_ADVANCE_STRAP/DISPLAY_ID_1
  {GPIO_VER4_S_GPP_J8,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn ,     GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone }}, //VAL_TEST_SETUP_MENU/DISPLAY_ID_0
};

#endif
