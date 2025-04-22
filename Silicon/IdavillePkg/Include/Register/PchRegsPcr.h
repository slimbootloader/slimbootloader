/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCH_REGS_PCR_H__
#define __PCH_REGS_PCR_H__

#include <PchReservedResources.h>

#define PCH_PCR_ADDRESS(Pid, Offset)    (PCH_PCR_BASE_ADDRESS | ((UINT8)(Pid) << 16) | (UINT16)(Offset))

#define PID_ESPISPI                     0x93
#define PID_PCIE0_P0                    0xB4 //PCIE0.P0
#define PID_PCIE0_P1                    0x8D //PCIE0.P1
#define PID_PCIE0_P2                    0x8E //PCIE0.P2
#define PID_PCIE0_P3                    0x8F //PCIE0.P3
#define PID_PCIE1_P0                    0xB3 //PCIE1.P0
#define PID_PCIE1_P1                    0x8A //PCIE1.P1
#define PID_PCIE1_P2                    0x8B //PCIE1.P2
#define PID_PCIE1_P3                    0x8C //PCIE1.P3
#define PID_PCIE2_P0                    0x71 //PCIE2.P0
#define PID_PCIE2_P1                    0x72 //PCIE2.P1
#define PID_PCIE2_P2                    0x73 //PCIE2.P2
#define PID_PCIE2_P3                    0x74 //PCIE2.P3
#define PID_DMI                         0x4D //RLINK
#define PID_GPIOCOM0                    0xC5 //GPIOE
#define PID_GPIOCOM1                    0xC2 //GPIOW
#define PID_ITSS                        0xD0
#define PID_EMMC                        0xD6
#define PID_RTC_HOST                    0xD1

#define R_ESPI_PCR_SOFTSTRAPS           0xC210          ///< eSPI Sofstraps Register 0
#define B_ESPI_PCR_SOFTSTRAPS_CS1_EN    BIT12           ///< CS1# Enable
#endif // __PCH_REGS_PCR_H__
