/** @file
  GPIO definition table for ElkhartLake

 Copyright (c) 2018 - 2023, Intel Corporation. All rights reserved.<BR>
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

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mEhlPchTsnDeviceGpioTable[] =
{
  {GPIO_VER3_GPP_C17, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII2_MDC
  {GPIO_VER3_GPP_C16, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII2_MDIO
  {GPIO_VER3_GPP_U0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII2_INT
  {GPIO_VER3_GPP_U1,  {GpioPadModeGpio,    GpioHostOwnDefault, GpioDirOut,    GpioOutHigh,      GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII2_RESETB
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mEhlPseTsn0DeviceGpioTable[] =
{
  {GPIO_VER3_GPP_C3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII0_MDC
  {GPIO_VER3_GPP_C4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII0_MDIO
  {GPIO_VER3_GPP_T4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn,     GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII0_INT
  {GPIO_VER3_GPP_T5,  {GpioPadModeGpio,    GpioHostOwnDefault, GpioDirOut,    GpioOutHigh,      GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII0_RESETB
  {GPIO_VER3_GPP_T6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII0_AUXTS
  {GPIO_VER3_GPP_T7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII0_PPS
  {GPIO_VER3_GPP_A0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A23, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}}
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mEhlPseTsn1DeviceGpioTable[] =
{
  {GPIO_VER3_GPP_C6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII1_MDC
  {GPIO_VER3_GPP_C7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII1_MDIO
  {GPIO_VER3_GPP_H0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn,     GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII1_INT
  {GPIO_VER3_GPP_H1,  {GpioPadModeGpio,    GpioHostOwnDefault, GpioDirOut,    GpioOutHigh,      GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII1_RESETB
  {GPIO_VER3_GPP_H2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII1_AUXTS
  {GPIO_VER3_GPP_H3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},   //RGMII1_PPS
  {GPIO_VER3_GPP_A11, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A12, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A14, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A15, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A16, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A17, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A18, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},
  {GPIO_VER3_GPP_A22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}}
};

#endif // _ELKHARTLAKE_GPIO_TABLE_H_
