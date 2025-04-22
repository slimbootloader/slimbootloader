/** @file
  TEST S RVP GPIO definition table for Post-Memory Initialization

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _TESTS_DDR5_CRB_POSTMEM_GPIO_TABLE_H_
#define _TESTS_DDR5_CRB_POSTMEM_GPIO_TABLE_H_

#include <GpioPinsVer2Lp.h>
#include <GpioConfig.h>

// PLATFORM_ID_TEST_S_DDR5_UDIMM_RVP
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTablePostMemTestsDdr5UDimmRvp[] =
{
  // Codec
  {GPIO_VER4_S_GPP_F23,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioPlatformReset,  GpioTermNone, GpioPadConfigUnlock}}, // Codec Interrupt
  {GPIO_VER4_S_GPP_R10,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                      // Codec Power enable

  // EC
  {GPIO_VER4_S_GPP_B4,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntEdge,             GpioPlatformReset,  GpioTermNone}}, // EC SMI
  {GPIO_VER4_S_GPP_B14,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // EC Speaker (Rework), Strap

  // GPIO Automation (Rework)
  {GPIO_VER4_S_GPP_C5,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // GPIO Automation (Rework), Strap

  // LAN Power enable
  {GPIO_VER4_S_GPP_R20,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // LAN Power enable

  // M.2 Key-E Slot for WiFi/BT
  {GPIO_VER4_S_GPP_B2,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}}, // M.2 Key-E slot WiFi Wake -> UART_BT_WAKE_N
  {GPIO_VER4_S_GPP_B3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                      // M.2 Key-E slot BT RF Kill
  {GPIO_VER4_S_GPP_B19,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                      // M.2 Key-E slot WiFi RF Kill
  {GPIO_VER4_S_GPP_G6,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioHostDeepReset,  GpioTermNone}},                      // M.2 Key-E slot for WiFi -> CNVI_EN_N
  {GPIO_VER4_S_GPP_H2,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioResumeReset,    GpioTermNone, GpioPadConfigUnlock}}, // M.2 Key-E slot for WiFi Wake
  {GPIO_VER4_S_GPP_H17,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                      // M.2 Key-E slot for WiFi Reset

  // HBR SSD
  {GPIO_VER4_S_GPP_F18,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // HBR SSD Reset
  {GPIO_VER4_S_GPP_H13,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // VAL_CPU_SSD_FULL_PWR_OFF_N

  // M2_SSD_1
  {GPIO_VER4_S_GPP_C10,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // SSD_1 RTD3 Reset LPSS UART-0 header (Rework)

  // M.2 SSD_2
  {GPIO_VER4_S_GPP_F16,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // M.2 SSD_2 RTD3 Reset MIPI60 (Rework)

  // M.2 SSD_3
  {GPIO_VER4_S_GPP_B6,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // SSD_3 RTD3 Reset

  // PCIe SLOT_1
  {GPIO_VER4_S_GPP_F0,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}}, // PCIe SLOT_1 Wake MIPI60 (Rework)

  // PCIe SLOT_2
  {GPIO_VER4_S_GPP_F1,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}}, // PCIe SLOT_2 Wake MIPI60 (Rework)

  // PCIe SLOT_3
  {GPIO_VER4_S_GPP_F2,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}}, // PCIe SLOT_3 Wake MIPI60 (Rework)

  // PEG SLOT_1
  {GPIO_VER4_S_GPP_E6,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}}, // PEG SLOT_1 Wake
  {GPIO_VER4_S_GPP_F7,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                      // PEG_SLOT_DGPU_SEL_N, MIPI60 (Rework)
  {GPIO_VER4_S_GPP_F9,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                      // PEG_SLOT_DGPU_PWR_OK, MIPI60 (Rework)

  // SATA
  {GPIO_VER4_S_GPP_R11,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // SATA Direct power enable

  // Thunderbolt
  {GPIO_VER4_S_GPP_G0,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                      // Thunderbolt FRC_PWR
//{GPIO_VER4_S_GPP_G1,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioHostDeepReset,  GpioTermNone}},                      // Thunderbolt CIO_PWR_EN, moved to EarlyPreMem
  {GPIO_VER4_S_GPP_G7,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                      // Thunderbolt
  {GPIO_VER4_S_GPP_E3,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioResetDefault,   GpioTermNone, GpioPadConfigUnlock}}, // TBT_PLUG_EVENT_N
//{GPIO_VER4_S_GPP_E7,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                      // GPIO_TBT_RST_N, moved to PreMem
//{GPIO_VER4_S_GPP_F5,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioHostDeepReset,  GpioTermNone}},                      // TBT_RTD3_PWR_EN, moved to EarlyPreMem
  {GPIO_VER4_S_GPP_F8,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}}, // TBT_GPIO_WAKE_N
  {GPIO_VER4_S_GPP_G5,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioResetDefault,   GpioTermNone}},                      // TBT_USB_FRC_PWR

  // DP Retimer
  {GPIO_VER4_S_GPP_D13,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // DP_RP_EN (rework)

  // Type C
  {GPIO_VER4_S_GPP_B20,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResetDefault,   GpioTermNone}},                      // BR_SW_SEL
  {GPIO_VER4_S_GPP_F22,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntEdge,             GpioPlatformReset,  GpioTermNone}},                      // USBC_PSON_OVERRIDE_N
  {GPIO_VER4_S_GPP_H1,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone, GpioPadConfigUnlock}}, // Type-C Alert
  {GPIO_VER4_S_GPP_K4,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioHostDeepReset,  GpioTermNone}},                      // PLX_SX_ENTRY_G1_PCH
  {GPIO_VER4_S_GPP_K3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntEdge,             GpioHostDeepReset,  GpioTermNone}},                      // PD_SX_ACK_G2_PCH
  {GPIO_VER4_S_GPP_E21,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResetDefault,   GpioTermNone}},                      // DG_BSSB_SBU_MUX_OE

  // TPM module
  {GPIO_VER4_S_GPP_F17,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // TPM module MIPI60 (Rework)

  // Touch panel
  {GPIO_VER4_S_GPP_D22,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone, GpioPadUnlock}}, // GPP_D_22_UART3_RTSB_THC1_SPI2_RST_N
  {GPIO_VER4_S_GPP_D23,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntApic, GpioResumeReset,    GpioTermNone, GpioPadUnlock}}, // GPP_D_23_UART3_CTSB_THC1_SPI2_INT_N
  {GPIO_VER4_S_GPP_R21,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},                // Touch panel power enable

  // PCIe Aux Power Enable
  {GPIO_VER4_S_GPP_I0,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResumeReset,    GpioTermNone}}, // PEG_PCIE_SLOT_AUXPWR_EN

  // PLN
  {GPIO_VER4_S_GPP_H15,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // PCH SSD PLN Circuit Strap
  {GPIO_VER4_S_GPP_H23,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // PCH SSD PLN Circuit

  // ZPODD
  {GPIO_VER4_S_GPP_B1,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntLevel|GpioIntSci, GpioPlatformReset,  GpioTermNone, GpioPadConfigUnlock}}, // SATA_ODD_DA_N
  {GPIO_VER4_S_GPP_E12,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResetDefault,   GpioTermNone}},                      // SATA_ODD_PWREN
  {GPIO_VER4_S_GPP_F4,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutLow,      GpioIntDis,              GpioResetDefault,   GpioTermNone}},                      // SATA_ODD_DP_N

  // Others
  {GPIO_VER4_S_GPP_K5,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // DNX_IN_PROG_AUX_NBIAS
  {GPIO_VER4_S_GPP_K1,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // PM_SLP_S0_CS_N
  {GPIO_VER4_S_GPP_H10,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntEdge,             GpioResetDefault,   GpioTermNone}}, // 2X4_POWER_DETECT, Power supply detect
  {GPIO_VER4_S_GPP_K0,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // Thunderbolt P1 P2 Alert
//{GPIO_VER4_S_GPP_H0,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // VAL_SV_ADVANCE_STRAP/DISPLAY_ID_1, moved to EarlyPreMem
//{GPIO_VER4_S_GPP_J8,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn ,     GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // VAL_TEST_SETUP_MENU/DISPLAY_ID_0, moved to EarlyPreMem
  {GPIO_VER4_S_GPP_H14,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntEdge,             GpioPlatformReset,  GpioTermNone}}, // VOL_UP

  // Not used
  {GPIO_VER4_S_GPD0,      {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ
  {GPIO_VER4_S_GPD1,      {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ
  {GPIO_VER4_S_GPD7,      {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ, strap
  {GPIO_VER4_S_GPD12,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ
  {GPIO_VER4_S_GPP_A14,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ
  {GPIO_VER4_S_GPP_B23,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ, strap
  {GPIO_VER4_S_GPP_C2,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ, strap
  {GPIO_VER4_S_GPP_D3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // GFX_DETECT_STRAP
  {GPIO_VER4_S_GPP_D14,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ
  {GPIO_VER4_S_GPP_F10,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ, MIPI60 (Rework)
  {GPIO_VER4_S_GPP_H12,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ, strap
  {GPIO_VER4_S_GPP_H18,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ, strap
  {GPIO_VER4_S_GPP_J9,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}}, // HiZ

};

// PLATFORM_ID_TEST_S_DDR5_SODIMM_RVP
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTablePostMemTestsDdr5SODimmRvp[] =
{
  // Codec
  {GPIO_VER4_S_GPP_F23,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioPlatformReset,  GpioTermNone, GpioPadConfigUnlock}},//Codec Interrupt
  {GPIO_VER4_S_GPP_R10,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//Codec Power enable

  // EC
  {GPIO_VER4_S_GPP_B4,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntEdge,             GpioPlatformReset,  GpioTermNone}},// EC SMI
  {GPIO_VER4_S_GPP_B14,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},// EC Speaker (Rework)Strap

  // GPIO Automation (Rework)
  {GPIO_VER4_S_GPP_C5,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},// GPIO Automation (Rework) Strap

  //Foxville LAN
  {GPIO_VER4_S_GPD1,      {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResumeReset,    GpioTermNone}}, // FOX_LAN_DISABLE_N
  {GPIO_VER4_S_GPP_G1,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}}, // PLTRST_FOX_N
  {GPIO_VER4_S_GPP_R20,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioResumeReset,    GpioTermNone}}, // LAN_FOX_ON. Set Output high based on the setup option.
  {GPIO_VER4_S_GPD2,      {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge,             GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}}, // FOX_WAKE_N. Set to SCI based on the setup option.

  // M.2 Key-E Slot for Wifi
  {GPIO_VER4_S_GPP_B2,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}},// M.2 Key-E Slot  Wifi Wake-> UART_BT_WAKE_N
  {GPIO_VER4_S_GPP_B3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},// M.2 Key-E Slot  BT RF Kill
  {GPIO_VER4_S_GPP_B19,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},// M.2 Key-E Slot  RF Kill
  {GPIO_VER4_S_GPP_G6,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioHostDeepReset,  GpioTermNone}},// M.2 Key-E slot for Wifi -> CNVI_EN_N
  {GPIO_VER4_S_GPP_H2,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioResumeReset,    GpioTermNone, GpioPadConfigUnlock}},//M.2 Key-E slot for Wifi Wake
  {GPIO_VER4_S_GPP_H17,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//M.2 Key-E slot for Wifi Reset

  // HBR SSD
  {GPIO_VER4_S_GPP_F18,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//HBR SSD Reset

  // M2_SSD_1
  {GPIO_VER4_S_GPP_C10,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},// SSD_1 RTD3 Reset LPSS UART-0 header (Rework

  // M.2 SSD_3
  {GPIO_VER4_S_GPP_B6,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},// SSD_3 RTD3 Reset

  // PCIe SLOT_1
  {GPIO_VER4_S_GPP_F0,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}},//PCIe SLOT_1 Wake MIPI60 (Rework)

  // PCIe SLOT_2
  {GPIO_VER4_S_GPP_F1,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioResetDefault,   GpioTermNone, GpioPadConfigUnlock}},//PCIe SLOT_2 Wake MIPI60 (Rework)

  // PCIe SLOT_3
  {GPIO_VER4_S_GPP_F2,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}},//PCIe SLOT_3 Wake MIPI60 (Rework)

  // PEG SLOT_1
  {GPIO_VER4_S_GPP_E6,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}},//PEG SLOT_1 Wake
  {GPIO_VER4_S_GPP_F6,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//PEG SLOT_1  MIPI60 (Rework)
  {GPIO_VER4_S_GPP_F7,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//PEG SLOT_1  MIPI60 (Rework)
  {GPIO_VER4_S_GPP_F9,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//PEG SLOT_1  MIPI60 (Rework)

  // SATA
  {GPIO_VER4_S_GPP_R11,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//SATA Direct power enable

  // Thunderbolt
  {GPIO_VER4_S_GPP_F3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//POR Thunderbolt USB Force Power, MIPI60 (Rework)
  //{GPIO_VER4_S_GPP_F5,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//POR Thunderbolt RTD3 enable and CIO_PWR_EN
  {GPIO_VER4_S_GPP_F8,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//POR Thunderbolt Wake, MIPI60 (Rework)
  {GPIO_VER4_S_GPP_F10,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//POR PD AIC detect, MIPI60 (Rework)
  {GPIO_VER4_S_GPP_F16,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//POR Thunderbolt Reset,MIPI60 (Rework)
  {GPIO_VER4_S_GPP_G0,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioResumeReset,    GpioTermNone}},//POR Thunderbolt FRC_PWR
  {GPIO_VER4_S_GPP_G7,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//Thunderbolt Plug event
  {GPIO_VER4_S_GPP_K0,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//Thunderbolt P1 P2 Alert

  // Type C
  {GPIO_VER4_S_GPP_B20,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntEdge,             GpioResetDefault,   GpioTermNone}},//Type-C PD Detection
  {GPIO_VER4_S_GPP_F22,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntEdge,             GpioPlatformReset,  GpioTermNone}},//USBC_PSON_OVERRIDE_N
  {GPIO_VER4_S_GPP_H1,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone, GpioPadConfigUnlock}},//Type-C Alert
  {GPIO_VER4_S_GPP_K4,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutLow,      GpioIntDefault,          GpioHostDeepReset,  GpioTermNone}},//PLX_SX_ENTRY_G1_PCH_N
  {GPIO_VER4_S_GPP_K3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntEdge,             GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}},//PD_SX_ACK_G2_PCH

  // TPM module
  {GPIO_VER4_S_GPP_F17,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//TPM module MIPI60 (Rework)

  // Touch panel
  {GPIO_VER4_S_GPP_D22,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone, GpioPadUnlock}},//GPP_D_22_UART3_RTSB_THC1_SPI2_RST_N
  {GPIO_VER4_S_GPP_D23,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntEdge|GpioIntApic, GpioResumeReset,    GpioTermNone, GpioPadUnlock}},//GPP_D_23_UART3_CTSB_THC1_SPI2_INT_N
  {GPIO_VER4_S_GPP_R21,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//Touch panel power enable

  //PCIe Aux Power Enable
  {GPIO_VER4_S_GPP_I0,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioResumeReset,    GpioTermNone}},//GPP_I_0_EXT_PWR_GATE_N

  //PLN
  {GPIO_VER4_S_GPP_H15,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,     GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//PCH SSD PLN Circuit Strap
  {GPIO_VER4_S_GPP_H23,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//PCH SSD PLN Circuit

  //Others
  {GPIO_VER4_S_GPP_K5,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,     GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//DNX_IN_PROG_AUX_NBIAS
  {GPIO_VER4_S_GPP_K1,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//Platform
  {GPIO_VER4_S_GPP_H10,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,      GpioOutDefault,  GpioIntEdge,             GpioResetDefault,   GpioTermNone}},//Power supply detect
  {GPIO_VER4_S_GPP_H0,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirDefault, GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//VAL_SV_ADVANCE_STRAP
  {GPIO_VER4_S_GPP_H14,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,   GpioOutDefault,  GpioIntEdge,             GpioPlatformReset,  GpioTermNone}},//VOL_UP

  //Not used
  {GPIO_VER4_S_GPD0,      {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPD7,      {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPD12,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_A14,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_B23,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_C2,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_D3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//GFX_DETECT_STRAP
  {GPIO_VER4_S_GPP_D11,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_D13,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_D14,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_E3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_E7,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_H12,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ, strap
  {GPIO_VER4_S_GPP_H13,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ
  {GPIO_VER4_S_GPP_H18,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioResetDefault,   GpioTermNone}},//HiZ, strap
  {GPIO_VER4_S_GPP_J8 ,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//HiZ, strap
  {GPIO_VER4_S_GPP_J9 ,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,    GpioOutDefault,  GpioIntDefault,          GpioPlatformReset,  GpioTermNone}},//HiZ, strap

};


#endif
