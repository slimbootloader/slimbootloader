/** @file
  Tigerlake U RVP GPIO definition table for TSN Initialization

  Copyright (c) 2019 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TIGERLAKE_U_TSN_GPIO_TABLE_H_
#define _TIGERLAKE_U_TSN_GPIO_TABLE_H_

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mTglUTsnDeviceGpioTable[] =
{
  // TSN Device, Same pins shared between THC Touch Panel 2
  {GPIO_VER2_LP_GPP_F17, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDC_THC1_SPI2_RSTB
  {GPIO_VER2_LP_GPP_F18, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDIO_THC1_SPI2_INTB
  {GPIO_VER2_LP_GPP_S0,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_SNDW1_CLK_RGMII_AUXTS
  {GPIO_VER2_LP_GPP_S1,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirIn,     GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_SNDW1_DATA_RGMII_INT
  {GPIO_VER2_LP_GPP_S2,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_SNDW2_CLK_RGMII_RESET
  {GPIO_VER2_LP_GPP_S3,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSNSNDW1_DATA_DMIC_CLK_B_1_RGMII_PPS
};

#endif // _TIGERLAKE_U_TSN_GPIO_TABLE_H_
