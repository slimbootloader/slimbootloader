/**@file
 VER4 GPIO ASL header

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Definition for GPIO groups and pads
//
#ifndef _GPIO_ACPI_DEFINES_ADL_
#define _GPIO_ACPI_DEFINES_ADL_

#include "GpioPinsVer4S.h"

#define GPIO_VER4_S_ACPI_HID   "INTC1056" //ADL-S GPIO Controller

//
// ADL-PCH-S
//
#define GPIO_VER4_S_DRIVER_GPP_I0                 0

#define GPIO_VER4_S_DRIVER_GPP_R0                 32


#define GPIO_VER4_S_DRIVER_VGPIO0                 96
#define GPIO_VER4_S_DRIVER_VGPIO30                115

#define GPIO_VER4_S_DRIVER_VGPIO_0_0              128

#define GPIO_VER4_S_DRIVER_GPP_B0                 160

#define GPIO_VER4_S_DRIVER_GPP_G0                 192

#define GPIO_VER4_S_DRIVER_GPP_H0                 224

#define GPIO_VER4_S_DRIVER_GPP_A0                 256

#define GPIO_VER4_S_DRIVER_GPP_C0                 288

#define GPIO_VER4_S_DRIVER_GPP_S0                 320

#define GPIO_VER4_S_DRIVER_GPP_E0                 352

#define GPIO_VER4_S_DRIVER_GPP_K0                 384

#define GPIO_VER4_S_DRIVER_GPP_F0                 416

#define GPIO_VER4_S_DRIVER_GPP_D0                 448

#endif // _GPIO_ACPI_DEFINES_ADL_
