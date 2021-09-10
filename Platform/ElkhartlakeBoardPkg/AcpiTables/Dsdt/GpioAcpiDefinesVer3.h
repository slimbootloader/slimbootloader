/**@file
 VER3 GPIO ASL header
 Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Definition for GPIO groups and pads
//
#ifndef _GPIO_ACPI_DEFINES_VER3_
#define _GPIO_ACPI_DEFINES_VER3_

#include "Register/GpioPinsVer3.h"

#define GPIO_VER3_ACPI_HID "INTC1020"

#define GPIO_VER3_PCH_GPIO_COM0_HW_BANK_PINS                      32

/* For Old GPIO FrameWork GPIO Base pin number statrs based on Max Bank pins */
#define GPIO_VER3_PCH_GPIO_COM0_GPP_B0_OGFW_PIN_BASE_INDEX         0   //(GpioAcpiPinNumber = 0 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM0_GPP_T0_OGFW_PIN_BASE_INDEX        32   //(GpioAcpiPinNumber = 1 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM0_GPP_G0_OGFW_PIN_BASE_INDEX        64   //(GpioAcpiPinNumber = 2 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM1_GPP_V0_OGFW_PIN_BASE_INDEX        96   //(GpioAcpiPinNumber = 3 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM1_GPP_H0_OGFW_PIN_BASE_INDEX       128   //(GpioAcpiPinNumber = 4 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM1_GPP_D0_OGFW_PIN_BASE_INDEX       160   //(GpioAcpiPinNumber = 5 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM1_GPP_U0_OGFW_PIN_BASE_INDEX       192   //(GpioAcpiPinNumber = 6 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM1_GPP_VGPIO0_OGFW_PIN_BASE_INDEX   224   //(GpioAcpiPinNumber = 7 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM3_GPP_S0_OGFW_PIN_BASE_INDEX       256   //(GpioAcpiPinNumber = 8 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM3_GPP_A0_OGFW_PIN_BASE_INDEX       288   //(GpioAcpiPinNumber = 9 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM3_GPP_VGPIO3_OGFW_PIN_BASE_INDEX   320   //(GpioAcpiPinNumber = 10 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM4_GPP_C0_OGFW_PIN_BASE_INDEX       352   //(GpioAcpiPinNumber = 11 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM4_GPP_F0_OGFW_PIN_BASE_INDEX       384   //(GpioAcpiPinNumber = 12 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM4_GPP_E0_OGFW_PIN_BASE_INDEX       416   //(GpioAcpiPinNumber = 13 * 32 + 0)
#define GPIO_VER3_PCH_GPIO_COM5_GPP_R0_OGFW_PIN_BASE_INDEX       448   //(GpioAcpiPinNumber = 14 * 32 + 0)

/* For New GPIO FrameWork GPIO Base pin number statrs based on no of actual Pins used per group */
// COM0
#define GPIO_VER3_PCH_GPIO_COM0_GPP_B0_NGFW_PIN_BASE_INDEX         0   // 0
#define GPIO_VER3_PCH_GPIO_COM0_GPP_T0_NGFW_PIN_BASE_INDEX        24   // B0_BASE_INDEX + GPIO_VER3_PCH_GPIO_GPP_B_PAD_MAX
#define GPIO_VER3_PCH_GPIO_COM0_GPP_G0_NGFW_PIN_BASE_INDEX        40   // T0_BASE_INDEX + GPIO_VER3_PCH_GPIO_GPP_T_PAD_MAX

// COM1
#define GPIO_VER3_PCH_GPIO_COM1_GPP_V0_NGFW_PIN_BASE_INDEX         0   // 0
#define GPIO_VER3_PCH_GPIO_COM1_GPP_H0_NGFW_PIN_BASE_INDEX        16   // V0_BASE_INDEX + GPIO_VER3_PCH_GPIO_GPP_V_PAD_MAX
#define GPIO_VER3_PCH_GPIO_COM1_GPP_D0_NGFW_PIN_BASE_INDEX        40   // H0_BASE_INDEX + GPIO_VER3_PCH_GPIO_GPP_H_PAD_MAX
#define GPIO_VER3_PCH_GPIO_COM1_GPP_GSPI2_NGFW_PIN_BASE_INDEX     60   // D0_BASE_INDEX + GPIO_VER3_PCH_GPIO_GPP_D_PAD_MAX
#define GPIO_VER3_PCH_GPIO_COM1_GPP_U0_NGFW_PIN_BASE_INDEX        61   // GSPI2_BASE_INDEX + 1
#define GPIO_VER3_PCH_GPIO_COM1_GPP_GSPI3_NGFW_PIN_BASE_INDEX     81   // U0_BASE_INDEX + GPIO_VER3_PCH_GPIO_GPP_U_PAD_MAX
#define GPIO_VER3_PCH_GPIO_COM1_GPP_VGPIO_NGFW_PIN_BASE_INDEX     85   // GSPI3_BASE_INDEX + 4

// COM3
#define GPIO_VER3_PCH_GPIO_COM3_GPP_S0_NGFW_PIN_BASE_INDEX        17
#define GPIO_VER3_PCH_GPIO_COM3_GPP_A0_NGFW_PIN_BASE_INDEX        19

// COM4
#define GPIO_VER3_PCH_GPIO_COM4_GPP_C0_NGFW_PIN_BASE_INDEX         0
#define GPIO_VER3_PCH_GPIO_COM4_GPP_F0_NGFW_PIN_BASE_INDEX        24
#define GPIO_VER3_PCH_GPIO_COM4_GPP_E0_BGFW_PIN_BASE_INDEX        54

// COM5
#define GPIO_VER3_PCH_GPIO_COM5_GPP_R0_NGFW_PIN_BASE_INDEX         0

#endif // _GPIO_ACPI_DEFINES_VER3_
