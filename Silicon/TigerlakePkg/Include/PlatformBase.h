/** @file

  Copyright (c) 2016 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PLATFORM_BASE_H_

#include <IndustryStandard/Pci.h>

#define CPU_TGL                 1
#define CPU_ICL                 0


#define ACPI_BASE_ADDRESS            0x1800

#define SPI_TEMP_MEM_BASE_ADDRESS    0xF0000000

#define TCO_BASE_ADDRESS             0x400
#define TCO_TMR_REGISTER             0x12
#define TCO1_CNT_REGISTER            0x08


// FSP will assign temporary resource to serial io devices, align with FSP resource here.
#define LPSS_UART_TEMP_BASE_ADDRESS(x)  (PCH_SERIAL_IO_BASE_ADDRESS + 0x12000 + (((x)<<1)<<12))

#endif
