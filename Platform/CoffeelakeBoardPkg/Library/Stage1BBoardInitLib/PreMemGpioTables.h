/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/GpioLib.h>
#include <GpioConfig.h>
#include <GpioPinsCnlH.h>
#include <GpioPinsCnlLp.h>


//
// CFL S 82 UDIMM GPIO TBL
//
static GPIO_INIT_CONFIG mGpioTableCflS82Ddr4PreMem[] =
{
  // PEG slot
  {GPIO_CNL_H_GPP_F22,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioPlatformReset, GpioTermNone }},//GPIO_PEG SLOT_RESET
  // turn on power in premem after reset asserted
  {GPIO_CNL_H_GPP_F8,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioPlatformReset, GpioTermNone }},//DGPU_PWR_EN_N
  {GPIO_CNL_H_GPP_K22,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioPlatformReset, GpioTermNone }},//PCH_PEGSLOT1_PWREN
  {GPIO_CNL_H_GPP_K10,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioHostDeepReset, GpioTermNone }},//PCH_RST_GPIO for PEG aux power
  // Titan Ridge
  {GPIO_CNL_H_GPP_K1,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioHostDeepReset, GpioTermWpu2K, GpioPadConfigLock }},//PLX_SX_ENTRY_G1_PCH_N
  //Asserting power pin enable in pre-mem phase to solve some NVMe devices cannot be detected.
  {GPIO_CNL_H_GPP_I10, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh,    GpioIntDefault, GpioPlatformReset, GpioTermNone }},//GPP_I_10_DDPD_CTRLDATA
};

//
// WHL ERB GPIO TBL
//
static GPIO_INIT_CONFIG mGpioTableCnlUDdr4PreMem[] =
{
  {GPIO_CNL_LP_GPP_C15,  { GpioPadModeGpio,  GpioHostOwnAcpi,  GpioDirOut,  GpioOutLow,  GpioIntDis,  GpioPlatformReset,  GpioTermNone}},   //SLOT1_RST_N
  {GPIO_CNL_LP_GPP_C14,  { GpioPadModeGpio,  GpioHostOwnAcpi,  GpioDirOut,  GpioOutLow,  GpioIntDis,  GpioPlatformReset,  GpioTermNone}},   //SLOT1_PWREN_N
  {GPIO_CNL_LP_GPP_C12,  { GpioPadModeGpio,  GpioHostOwnAcpi,  GpioDirOut,  GpioOutLow,  GpioIntDis,  GpioPlatformReset,  GpioTermNone}},   //PCIE_NAND_RST_N
  {GPIO_CNL_LP_GPP_C13,  { GpioPadModeGpio,  GpioHostOwnAcpi,  GpioDirOut,  GpioOutHigh, GpioIntDis,  GpioPlatformReset,  GpioTermNone}},   //PCIE_NAND_PWREN_N
};

static GPIO_INIT_CONFIG mGpioTableWhlUDdr4WwanPreMem[] =
{
  // Turn on WWAN power and assert reset pins by default
  {GPIO_CNL_LP_GPP_A11,  { GpioPadModeGpio,  GpioHostOwnGpio,  GpioDirInInv, GpioOutDefault, GpioIntLevel|GpioIntSci, GpioHostDeepReset, GpioTermWpu20K, GpioPadConfigUnlock}},   //WWAN_WAKE_N
  {GPIO_CNL_LP_GPP_D13,  { GpioPadModeGpio,  GpioHostOwnGpio,  GpioDirOut,   GpioOutHigh,    GpioIntDis,              GpioResumeReset,   GpioTermNone,   GpioOutputStateUnlock}}, //WWAN_FCP_OFF
  {GPIO_CNL_LP_GPP_D16,  { GpioPadModeGpio,  GpioHostOwnGpio,  GpioDirOut,   GpioOutHigh,    GpioIntDis,              GpioResumeReset,   GpioTermNone,   GpioOutputStateUnlock}}, //EN_V3.3A_WWAN_LS
  {GPIO_CNL_LP_GPP_E15,  { GpioPadModeGpio,  GpioHostOwnGpio,  GpioDirOut,   GpioOutHigh,    GpioIntDis,              GpioResumeReset,   GpioTermNone,   GpioOutputStateUnlock}}, //WWAN_PERST
  {GPIO_CNL_LP_GPP_F1,   { GpioPadModeGpio,  GpioHostOwnGpio,  GpioDirOut,   GpioOutLow,     GpioIntDis,              GpioResumeReset,   GpioTermNone,   GpioOutputStateUnlock}}, //WWAN_RST_N
  {GPIO_CNL_LP_GPP_H16,  { GpioPadModeGpio,  GpioHostOwnGpio,  GpioDirOut,   GpioOutHigh,    GpioIntDis,              GpioResumeReset,   GpioTermNone,   GpioOutputStateUnlock}}, //WWAN_WAKE_CTRL
  {GPIO_CNL_LP_GPP_H17,  { GpioPadModeGpio,  GpioHostOwnGpio,  GpioDirOut,   GpioOutHigh,    GpioIntDis,              GpioResumeReset,   GpioTermNone,   GpioOutputStateUnlock}}, //WWAN_DISABLE_N
};


//
// CFL H DDR4 GPIO TBL
//
static GPIO_INIT_CONFIG mGpioTableCoffeelakeHDdr4PreMem[] =
{
  // M.2 SSD1
  // pull low first in premem and pull high in postmem
  {GPIO_CNL_H_GPP_C14,  {GpioPadModeGpio , GpioHostOwnGpio , GpioDirOut , GpioOutLow , GpioIntDis , GpioPlatformReset , GpioTermNone }}, // SSD1_RESET
  // turn on power in premem after reset asserted
  {GPIO_CNL_H_GPP_K8,   {GpioPadModeGpio , GpioHostOwnGpio , GpioDirOut , GpioOutHigh , GpioIntDis , GpioPlatformReset , GpioTermNone }}, // SSD1_PWREN

  // M.2 SSD2
  // pull low first in premem and pull high in postmem
  {GPIO_CNL_H_GPP_C15,  {GpioPadModeGpio , GpioHostOwnGpio , GpioDirOut , GpioOutLow , GpioIntDis , GpioPlatformReset , GpioTermNone }}, // SSD2_RESET
  // turn on power in premem after reset asserted
  {GPIO_CNL_H_GPP_K9,   {GpioPadModeGpio , GpioHostOwnGpio , GpioDirOut , GpioOutHigh , GpioIntDis , GpioPlatformReset , GpioTermNone }}, // SSD2_PWREN

  // x4 SLOT2
  // do not reset SLOT2 due to TR AIC card cannot be reset in S3 resume.
  // {GPIO_CNL_H_GPP_F4,   {GpioPadModeGpio , GpioHostOwnGpio , GpioDirOut , GpioOutLow , GpioIntDefault , GpioPlatformReset , GpioTermNone }}, // SLOT2_RST_N
  // turn on power
  {GPIO_CNL_H_GPP_H18,  {GpioPadModeGpio , GpioHostOwnGpio , GpioDirOut , GpioOutLow , GpioIntDis , GpioPlatformReset , GpioTermNone }}, // SLOT2_PWREN

  // PEG slot
  // PEG couldn't assert reset in premem and de-assert in postmem because SA RC detect PEG card in premem phase. Leave PEG reset pin by platform RESET
  // {GPIO_CNL_H_GPP_F9,   {GpioPadModeGpio , GpioHostOwnGpio , GpioDirOut , GpioOutLow , GpioIntDis , GpioPlatformReset , GpioTermNone }}, // PEG_SLOT_RST
  // turn on power
  {GPIO_CNL_H_GPP_F8,   {GpioPadModeGpio , GpioHostOwnGpio , GpioDirOut , GpioOutHigh , GpioIntDis , GpioPlatformReset , GpioTermNone }}, // PEG_SLOT_PWREN

};

GPIO_INIT_CONFIG mGpioTableEMMC[] =
{
  {GPIO_CNL_LP_GPP_F11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_CMD
  {GPIO_CNL_LP_GPP_F12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA0
  {GPIO_CNL_LP_GPP_F13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA1
  {GPIO_CNL_LP_GPP_F14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA2
  {GPIO_CNL_LP_GPP_F15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA3
  {GPIO_CNL_LP_GPP_F16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA4
  {GPIO_CNL_LP_GPP_F17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA5
  {GPIO_CNL_LP_GPP_F18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA6
  {GPIO_CNL_LP_GPP_F19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA7
  {GPIO_CNL_LP_GPP_F20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_RCLK
  {GPIO_CNL_LP_GPP_F21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_CLK
  {GPIO_CNL_LP_GPP_F22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_RESETB
};
