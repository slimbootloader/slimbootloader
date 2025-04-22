/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/GpioLib.h>
#include <GpioConfig.h>
#include <GpioPinsCmlvH.h>
#include <GpioPinsCmlvLp.h>
/*
//
// GPIO_PAD Fileds
//
typedef struct {
  UINT32    PadNum:16;
  UINT32    GrpIdx:8;
  UINT32    ChipsetId:4;
  UINT32    Rsvd:4;
} GPIO_PAD_FIELD;

//
// GPIO_CFG_DATA DW1 fields
//
typedef struct {
  UINT32    Rsvd1:16;
  UINT32    PadNum:8;
  UINT32    GrpIdx:5;
  UINT32    Rsvd2:3;
} GPIO_CFG_DATA_DW1;

#define CNL_LP_CHIPSET_ID   0x4
#define CNL_H_CHIPSET_ID    0x3
*/
//
// CML S 82 UDIMM GPIO TBL
//
static GPIO_INIT_CONFIG mGpioTableCmlS82Ddr4PreMem[] =
{
  // PEG slot
  {GPIO_CNL_H_GPP_F22,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioPlatformReset, GpioTermNone }},//GPIO_PEG SLOT_RESET
  // turn on power in premem after reset asserted
  {GPIO_CNL_H_GPP_F8,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioPlatformReset, GpioTermNone }},//DGPU_PWR_EN_N
  {GPIO_CNL_H_GPP_G22,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioPlatformReset, GpioTermNone }},//PCH_PEGSLOT_PWREN
  {GPIO_CNL_H_GPP_G10,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioHostDeepReset, GpioTermNone }},//PCH_RST_GPIO
  {GPIO_CNL_H_GPP_G1,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioHostDeepReset, GpioTermWpu2K, GpioPadConfigLock }},//PLX_SX_ENTRY_G1_PCH_N
  //Asserting power pin enable in pre-mem phase to solve some NVMe devices cannot be detected.
  {GPIO_CNL_H_GPP_I10, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh,    GpioIntDefault, GpioPlatformReset, GpioTermNone }},//GPP_I_10_DDPD_CTRLDATA
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
