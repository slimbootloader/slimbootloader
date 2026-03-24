/** @file
  PCH preserved MMIO resource definitions.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_PRESERVED_RESOURCES_H_
#define _PCH_PRESERVED_RESOURCES_H_

/**
  Detailed recommended static allocation
  +----------------------------------------------------------------------------------------------------------+
  | PCH preserved MMIO range, 16 MB, from 0xFD800000 to 0xFE7FFFFF                                           |
  +----------------------------------------------------------------------------------------------------------+
  | Size        | Start        | End          | Usage                                                        |
  | 8 MB        | 0xFD800000   | 0xFDFFFFFF   | TraceHub SW BAR (SOC)                                        |
  | 64 KB       | 0xFE000000   | 0xFE00FFFF   | PMC MBAR                                                     |
  | 4 KB        | 0xFE010000   | 0xFE010FFF   | SPI BAR0                                                     |
  | 176 KB      | 0xFE020000   | 0xFE04BFFF   | SerialIo BAR in ACPI mode                                    |
  | 400 KB      | 0xFE04C000   | 0xFE0AFFFF   | Unused                                                       |
  | 64 KB       | 0xFE0B0000   | 0xFE0BFFFF   | eSPI LGMR BAR                                                |
  | 64 KB       | 0xFE0C0000   | 0xFE0CFFFF   | eSPI2 SEGMR BAR                                              |
  | 192 KB      | 0xFE0D0000   | 0xFE0FFFFF   | Unused                                                       |
  | 1 MB        | 0xFE100000   | 0xFE1FFFFF   | TraceHub MTB BAR (SOC)                                       |
  | 2 MB        | 0xFE200000   | 0xFE3FFFFF   | TraceHub FW BAR (SOC)                                        |
  | 64 KB       | 0xFE400000   | 0xFE40FFFF   | IOE PMC BAR                                                  |
  | 64 KB       | 0xFE410000   | 0xFE41FFFF   | Tracehub RTIT BAR (SOC)                                      |
  | 896 KB      | 0xFE420000   | 0xFE4FFFFF   | Unused                                                       |
  | 1 MB        | 0xFE500000   | 0xFE5FFFFF   | Unused                                                       |
  | 2 MB        | 0xFE600000   | 0xFE7FFFFF   | Temp address                                                 |
  | 4 KB        | 0xFEDA2000   | 0xFEDA2FFF   | HECI1 BAR0                                                   |
  | 4 KB        | 0xFEDA3000   | 0xFEDA3FFF   | HECI2 BAR0                                                   |
  | 4 KB        | 0xFEDA4000   | 0xFEDA4FFF   | HECI3 BAR0                                                   |
  +----------------------------------------------------------------------------------------------------------+
  | SBREG MMIO range                                                                                         |
  +----------------------------------------------------------------------------------------------------------+
  | 16 MB       | 0xE0000000   | 0xE0FFFFFF   | SBREG                                                        |
  | 16 MB       | 0xE2000000   | 0xE2FFFFFF   | 2nd SBREG                                                    |
  +----------------------------------------------------------------------------------------------------------+
**/
#define PCH_PRESERVED_BASE_ADDRESS          0xFD800000     ///< Pch preserved MMIO base address
#define PCH_PRESERVED_MMIO_SIZE             0x01000000     ///< 16MB
#define PCH_TRACE_HUB_SW_BASE_ADDRESS       0xFD800000     ///< TraceHub SW MMIO base address
#define PCH_TRACE_HUB_SW_MMIO_SIZE          0x00800000     ///< 8MB

#define PCH_PWRM_BASE_ADDRESS           0xFE000000     ///< PMC MBAR MMIO base address
#define PCH_PWRM_MMIO_SIZE              0x00010000     ///< 64KB
#define PCH_SPI_BASE_ADDRESS            0xFE010000     ///< SPI BAR0 MMIO base address
#define PCH_SPI_MMIO_SIZE               0x00001000     ///< 4KB
#define PCH_SERIAL_IO_BASE_ADDRESS      0xFE020000     ///< SerialIo MMIO base address
#define PCH_SERIAL_IO_MMIO_SIZE         0x0002C000     ///< 176KB
#define PCH_TRACE_HUB_MTB_BASE_ADDRESS  0xFE100000     ///< TraceHub MTB MMIO base address
#define PCH_TRACE_HUB_MTB_MMIO_SIZE     0x00100000     ///< 1MB
#define PCH_TRACE_HUB_FW_BASE_ADDRESS   0xFE200000     ///< TraceHub FW MMIO base address

#define PCH_HECI1_BASE_ADDRESS              0xFEDA2000     ///< HECI1 BAR0 MMIO base address
#define PCH_HECI1_MMIO_SIZE                 0x00001000     ///< 4KB



#define PCH_PCR_BASE_ADDRESS                0x4000000000     ///< SBREG MMIO base address
#define PCH_PCR_SECOND_BASE_ADDRESS         0x4010000000     ///< Second SBREG MMIO base address
#define PCH_PCR_MMIO_SIZE                   0x10000000       ///< 256MB
#endif
