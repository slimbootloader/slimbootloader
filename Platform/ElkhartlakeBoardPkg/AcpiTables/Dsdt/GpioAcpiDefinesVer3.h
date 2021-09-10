/**@file
 VER3 GPIO ASL header

@copyright
  INTEL CONFIDENTIAL
  Copyright 2018 - 2021 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
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
