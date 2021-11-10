/** @file
  GPIO definition table for ElkhartLake

 Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _ELKHARTLAKE_GPIO_TABLE_H_
#define _ELKHARTLAKE_GPIO_TABLE_H_

#include <Register/GpioPinsVer3.h>
#include <GpioConfig.h>

#ifndef ARG_UNUSED
#if defined(__GNUC__)
#  define ARG_UNUSED  __attribute__ ((unused))
#else
#  define ARG_UNUSED
#endif
#endif

static GPIO_INIT_CONFIG ARG_UNUSED mGpioTableEhlSimics[] =
{
  // M.2 Key-E - WLAN/BT
  {GPIO_VER3_GPP_E11, {GpioPadModeGpio, GpioHostOwnDefault, GpioDirOut,   GpioOutHigh,   GpioIntDis,              GpioHostDeepReset,  GpioTermNone,  GpioOutputStateUnlock}},  //BT_RF_KILL_N
  {GPIO_VER3_GPP_E10, {GpioPadModeGpio, GpioHostOwnDefault, GpioDirOut,   GpioOutHigh,   GpioIntDis,              GpioHostDeepReset,  GpioTermNone,  GpioOutputStateUnlock}},  //WIFI_RF_KILL_N
  {GPIO_VER3_GPD7,    {GpioPadModeGpio, GpioHostOwnAcpi,    GpioDirOut,   GpioOutHigh,   GpioIntDis,              GpioPlatformReset,  GpioTermNone}},                          //M.2_WLAN_PERST_N
  {GPIO_VER3_GPP_F4,  {GpioPadModeGpio, GpioHostOwnAcpi,    GpioDirInInv, GpioOutDefault,GpioIntLevel|GpioIntSci, GpioHostDeepReset,  GpioTermWpu5K, GpioPadConfigUnlock  }},  //WIFI_WAKE_N
  {GPIO_VER3_GPP_F20, {GpioPadModeGpio, GpioHostOwnAcpi,    GpioDirInInv, GpioOutDefault,GpioIntLevel|GpioIntSci, GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock  }},  //UART_BT_WAKE_N

  // X4 Pcie Slot for Gen3 and Gen 4
  {GPIO_VER3_GPP_E13, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,  GpioOutLow,   GpioIntDis,GpioPlatformReset,  GpioTermNone}},  //ONBOARD_X4_PCIE_SLOT1_PWREN_N
  {GPIO_VER3_GPD11,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,  GpioOutHigh,  GpioIntDis,GpioPlatformReset,  GpioTermNone}},  //ONBOARD_X4_PCIE_SLOT1_RESET_N
  {GPIO_VER3_GPP_E2,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,  GpioOutDefault,GpioIntLevel|GpioIntSci,GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock  }},  //ONBOARD_X4_PCIE_SLOT1_WAKE_N

  // X1 PCIe M.2 Key-B WWAN/SATA SSD
  {GPIO_VER3_GPP_F21, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirOut,  GpioOutHigh,    GpioIntDis,     GpioPlatformReset, GpioTermNone}},  //M.2_WWAN_PWR_EN
  {GPIO_VER3_GPP_V13, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirOut,  GpioOutHigh,    GpioIntDis,     GpioPlatformReset, GpioTermNone}},  //M.2_WWAN_RST_N
  {GPIO_VER3_GPP_B14, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirOut,  GpioOutHigh,    GpioIntDis,     GpioPlatformReset, GpioTermNone}},  //M.2_WWAN_PE_RST_N
  {GPIO_VER3_GPP_B17, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirOut,  GpioOutHigh,    GpioIntDis,     GpioPlatformReset, GpioTermNone}},  //M.2_WWAN_PE_WAKE_N
  {GPIO_VER3_GPP_E0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,  GpioOutHigh,    GpioOutDefault, GpioHostDeepReset, GpioTermNone}},  //M.2_WWAN_FCP_OFF_N
  {GPIO_VER3_GPP_G12, {GpioPadModeNative3, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis,     GpioHostDeepReset, GpioTermNone}},  //M.2_SSD_SATAXPCIE_1
  {GPIO_VER3_GPP_E8,  {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis,     GpioHostDeepReset, GpioTermNone}},  //M.2_SSD_SATA_DEVSLP_1


  // FPS
  {GPIO_VER3_GPP_V14, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut, GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},                        //FPS_RST_N
  {GPIO_VER3_GPP_V15, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntLevel|GpioIntApic, GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock}},  //FPS_INT

  // Audio
  {GPIO_VER3_GPP_B18, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,  GpioOutHigh,     GpioIntDis,               GpioPlatformReset, GpioTermNone}},                       //SPKR_PD_N
  {GPIO_VER3_GPP_B15, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel|GpioIntApic, GpioPlatformReset, GpioTermNone, GpioPadConfigUnlock}},  //CODEC_INT_N

  // Touch Panel 1
  {GPIO_VER3_GPP_B16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirOut,  GpioOutHigh,    GpioIntDis, GpioPlatformReset, GpioTermNone}},                       //TCH_PNL_PWR_EN
  {GPIO_VER3_GPP_E6,  {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone, GpioPadConfigUnlock}},  //THC0_SPI1_RST_N
  {GPIO_VER3_GPP_E17, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone, GpioPadConfigUnlock}},  //THC0_SPI1_INT_N


  // SPI TPM
  {GPIO_VER3_GPP_G19, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn, GpioOutDefault, GpioIntLevel|GpioIntApic, GpioHostDeepReset, GpioTermNone, GpioPadConfigUnlock}},  //SPI_TPM_INT_N

  // Generic Buttons
  {GPIO_VER3_GPP_E12, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntBothEdge|GpioIntApic, GpioPlatformReset, GpioTermNone, GpioPadConfigUnlock}},   // VOLUMEUP_N

  // Touch Panel 1
  {GPIO_VER3_GPP_B16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirOut,  GpioOutHigh,    GpioIntDis, GpioPlatformReset, GpioTermNone}},                       //TCH_PNL_PWR_EN
  {GPIO_VER3_GPP_E6,  {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone, GpioPadConfigUnlock}},  //THC0_SPI1_RST_N
  {GPIO_VER3_GPP_E17, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone, GpioPadConfigUnlock}},  //THC0_SPI1_INT_N

//  {0x0}
};
//Fusa GPIO config
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTablePreMemEhlFusa[] =
{
  {GPIO_VER3_GPP_U4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_U4
  {GPIO_VER3_GPP_U5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_U5
  {GPIO_VER3_GPP_U6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_U6
  {GPIO_VER3_GPP_U8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_U8
  {GPIO_VER3_GPP_U9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_U9
  {GPIO_VER3_GPP_U10,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_U10
  {GPIO_VER3_GPP_C22,  {GpioPadModeNative2, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_C22
  {GPIO_VER3_GPP_C23,  {GpioPadModeNative2, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_C23
  {GPIO_VER3_GPP_D17,  {GpioPadModeGpio, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_D17
  {GPIO_VER3_GPP_D18,  {GpioPadModeGpio, GpioHostOwnDefault, GpioDirNone,   GpioOutLow,    GpioIntLvlEdgDis | GpioIntDis, GpioResumeReset,  GpioTermNone}},  //GPIO_VER3_GPP_D18

};

#endif // _ELKHARTLAKE_GPIO_TABLE_H_
