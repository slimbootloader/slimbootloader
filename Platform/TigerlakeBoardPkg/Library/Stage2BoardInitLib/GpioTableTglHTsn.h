/** @file
  Tigerlake U RVP GPIO definition table for TSN Initialization

  Copyright (c) 2019 - 2021 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TIGERLAKE_H_TSN_GPIO_TABLE_H_
#define _TIGERLAKE_H_TSN_GPIO_TABLE_H_

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mTglHTsnDeviceGpioTable[] =
{
  // TSN Device
  //
  {GPIO_VER2_H_GPP_G3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDC0
  {GPIO_VER2_H_GPP_G4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDIO0
  {GPIO_VER2_H_GPP_E11, {GpioPadModeNative2, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDC1
  {GPIO_VER2_H_GPP_E12, {GpioPadModeNative2, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDIO1
  {GPIO_VER2_H_GPP_S0,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_SNDW1_CLK_RGMII_AUXTS
  {GPIO_VER2_H_GPP_S1,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirIn,     GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_SNDW1_DATA_RGMII_INT
  {GPIO_VER2_H_GPP_S2,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_SNDW2_CLK_RGMII_RESET
  {GPIO_VER2_H_GPP_S3,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSNSNDW1_DATA_DMIC_CLK_B_1_RGMII_PPS
  {GPIO_VER2_H_GPP_S4,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII_AUXTS
  {GPIO_VER2_H_GPP_S5,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirIn,     GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII_INT
  {GPIO_VER2_H_GPP_S6,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII_RESET
  {GPIO_VER2_H_GPP_S7,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII_PPS
};

#endif // _TIGERLAKE_U_TSN_GPIO_TABLE_H_
