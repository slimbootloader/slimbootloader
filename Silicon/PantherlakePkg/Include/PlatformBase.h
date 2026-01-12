/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PLATFORM_BASE_H_
#define _PLATFORM_BASE_H_

#define ACPI_MMIO_BASE_ADDRESS       0xD0000000
#define ACPI_BASE_ADDRESS            0x1800
#define HPET_BASE_ADDRESS            0xFED00000
#define IO_APIC_BASE_ADDRESS         0xFEC00000
#define TCO_BASE_ADDRESS             0x400
#define R_TCO_IO_TCO1_CNT            0x08
#define LOCAL_APIC_BASE_ADDRESS      0xFEE00000

#define PCH_PCR_BASE_ADDRESS         0x4000000000     ///< SBREG MMIO base address
#define PCH_PWRM_BASE_ADDRESS        0xFE000000
// FSP will assign temporary resource to serial io devices, align with FSP resource here.
#define LPSS_UART_TEMP_BASE_ADDRESS(x)  (PCH_SERIAL_IO_BASE_ADDRESS + 0xC000 + (((x)<<1)<<12))

#endif
