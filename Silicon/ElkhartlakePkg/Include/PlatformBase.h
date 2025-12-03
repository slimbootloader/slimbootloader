/** @file

  Copyright (c) 2016 - 2025 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PLATFORM_BASE_H_

#include <IndustryStandard/Pci.h>

#define TCO_BASE_ADDRESS             0x400
#define ACPI_BASE_ADDRESS            0x1800
#define HPET_BASE_ADDRESS            0xFED00000
#define SPI_TEMP_MEM_BASE_ADDRESS    0xF0000000

#define LOCAL_APIC_BASE_ADDRESS      0xFEE00000
#define IO_APIC_BASE_ADDRESS         0xFEC00000

// FSP will assign temporary resource to serial io devices, align with FSP resource here.
#define LPSS_UART_TEMP_BASE_ADDRESS(x)      (PCH_SERIAL_IO_BASE_ADDRESS + 0x1E000 + (x << 13))

#define MmPciBase(Bus, Device, Function) \
  ( (UINTN)PcdGet64(PcdPciExpressBaseAddress) + \
    (UINTN)(Bus << 20) +    \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) \
  )

#endif
