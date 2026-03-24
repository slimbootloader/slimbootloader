/** @file
  GPIO ASL header for PTL & WCL_PCD.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIOV2_ACPI_DEFINES_H_
#define _GPIOV2_ACPI_DEFINES_H_

#define GPIO_HID_PTL_PCD_H "INTC10BB" // PTL PCD-H GPIO
#define GPIO_HID_PTL_PCD_P "INTC10BC" // PTL PCD-P GPIO

#define GPIO_HID_WCL_PCD   "INTC10EC" // WCL PCD GPIO

//
// PCH GPIO Community address
//
#define PTL_PCD_GPIOV2_COM0                               0x590000 // PID: 0xF259
#define PTL_PCD_GPIOV2_COM1                               0x5A0000 // PID: 0xF25A
#define PTL_PCD_GPIOV2_COM3                               0x5B0000 // PID: 0xF25B
#define PTL_PCD_GPIOV2_COM4                               0x5C0000 // PID: 0xF25C
#define PTL_PCD_GPIOV2_COM5                               0x5D0000 // PID: 0xF25D

#define WCL_PCD_GPIOV2_COM0                               0x590000 // PID: 0xF259
#define WCL_PCD_GPIOV2_COM1                               0x5A0000 // PID: 0xF25A
#define WCL_PCD_GPIOV2_COM3                               0x5B0000 // PID: 0xF25B
#define WCL_PCD_GPIOV2_COM4                               0x5C0000 // PID: 0xF25C
#define WCL_PCD_GPIOV2_COM5                               0x5D0000 // PID: 0xF25D

#endif // _GPIOV2_ACPI_DEFINES_H_
