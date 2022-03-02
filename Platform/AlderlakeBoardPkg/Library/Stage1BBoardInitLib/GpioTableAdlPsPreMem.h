/** @file
  AlderLake PS RVP GPIO definition table for Pre-Memory Initialization

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _ALDER_LAKE_PS_DDR5_CRB_PREMEM_GPIO_TABLE_H_
#define _ALDER_LAKE_PS_DDR5_CRB_PREMEM_GPIO_TABLE_H_

#include <GpioPinsVer2Lp.h>
#include <GpioConfig.h>

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTablePreMemAdlPsDdr5Rvp[] =
{
  //CPU M.2 SSD1
  {GPIO_VER2_LP_GPP_D14,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,     GpioPlatformReset,     GpioTermNone} },//M.2_CPU_SSD1_PWREN
  {GPIO_VER2_LP_GPP_F7,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutLow,        GpioIntDefault,     GpioPlatformReset,     GpioTermNone} },//M.2_CPU_SSD1_RESET_N
  // CPU M.2 SSD2
  {GPIO_VER2_LP_GPP_C2,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,     GpioPlatformReset,     GpioTermNone} },//M.2_CPU_SSD2_PWREN
  {GPIO_VER2_LP_GPP_H1,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutLow,        GpioIntDefault,     GpioPlatformReset,     GpioTermNone} },//M2_CPU_SSD2_RESET_N
  // X4 Pcie Slot
  {GPIO_VER2_LP_GPP_A14,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutLow,        GpioIntDefault,     GpioPlatformReset,     GpioTermNone} },//X4_SLOT_PWREN
  {GPIO_VER2_LP_GPP_F10,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,     GpioPlatformReset,     GpioTermNone} },//X4_Slot_RESET
   // TBT Re-Timers
  {GPIO_VER2_LP_GPP_E4,   {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutLow,        GpioIntDefault,     GpioPlatformReset,     GpioTermNone} },//TC_RETIMER_FORCE_PWR
  {GPIO_VER2_LP_GPP_E5,   {GpioPadModeGpio,       GpioHostOwnGpio,       GpioDirOut,        GpioOutLow,        GpioIntDefault,     GpioPlatformReset,     GpioTermNone} },//SRCCLK_OEB6
  {GPIO_VER2_LP_GPP_A7,   {GpioPadModeGpio,       GpioHostOwnGpio,       GpioDirOut,        GpioOutLow,        GpioIntDefault,     GpioPlatformReset,     GpioTermNone} },//SRCCLK_OEB7
};

#endif // _ALDER_LAKE_PS_DDR5_CRB_PREMEM_GPIO_TABLE_H_
