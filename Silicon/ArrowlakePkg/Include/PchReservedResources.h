/** @file
  PCH preserved MMIO resource definitions.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
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
  | 1 MB        | 0xFE500000   | 0xFE5FFFFF   | eSPI LGMR (SOC-S)                                            |
  | 2 MB        | 0xFE600000   | 0xFE7FFFFF   | Temp address                                                 |
  | 4 KB        | 0xFEDA2000   | 0xFEDA2FFF   | HECI1 BAR0                                                   |
  | 4 KB        | 0xFEDA3000   | 0xFEDA3FFF   | HECI2 BAR0                                                   |
  | 4 KB        | 0xFEDA4000   | 0xFEDA4FFF   | HECI3 BAR0                                                   |
  +----------------------------------------------------------------------------------------------------------+
  | SBREG MMIO range                                                                                         |
  +----------------------------------------------------------------------------------------------------------+
  | 16 MB OR    | 0xE0000000   | 0xE0FFFFFF   | SBREG                                                        |
  | 256 MB      | 0xE0000000   | 0xEFFFFFFF   | SBREG_EX (SBREG_BAR_20BITADDRESS=1)                          |
  +----------------------------------------------------------------------------------------------------------+
  | PCH preserved MMIO range above 4GB, 1 GB, from PhysicalAddressLimit to (PhysicalAddressLimit - 1GB)      |
  +----------------------------------------------------------------------------------------------------------+
  | 256 MB      | 0x3FFF0000000 | 0x3FFFFFFFFFF | IOE P2SB Regbar above 4G                                   |
  +----------------------------------------------------------------------------------------------------------+
  | 256 MB      | 0x3FFE0000000 | 0x3FFEFFFFFFF | PCH P2SB Regbar above 4G                                   |
  +----------------------------------------------------------------------------------------------------------+
  | 512 MB      | 0x3FFC0000000 | 0x3FFDFFFFFFF | Reserved above 4G                                          |
  +----------------------------------------------------------------------------------------------------------+
**/
#define PCH_TRACE_HUB_SW_BASE_ADDRESS   0xFC800000     ///< TraceHub SW MMIO base address
#define PCH_TRACE_HUB_SW_MMIO_SIZE      0x00800000     ///< 8MB
#define PCH_PCR_BASE_ADDRESS            0xE0000000     ///< SBREG MMIO base address
#define PCH_PCR_MMIO_SIZE               0x01000000     ///< 16MB
#define PCH_PWRM_BASE_ADDRESS           0xFE000000     ///< PMC MBAR MMIO base address
#define PCH_SPI_BASE_ADDRESS            0xFE010000     ///< SPI BAR0 MMIO base address
//#define PCH_SERIAL_IO_BASE_ADDRESS      0xFE020000     ///< SerialIo MMIO base address
#define PCH_TRACE_HUB_MTB_BASE_ADDRESS  0xFE100000     ///< TraceHub MTB MMIO base address
#define PCH_TRACE_HUB_MTB_MMIO_SIZE     0x00100000     ///< 1MB
#define PCH_TRACE_HUB_FW_BASE_ADDRESS   0xFE200000     ///< TraceHub FW MMIO base address

#define IOE_PCR_ABOVE_4G_BASE_ADDRESS       0x3FFF0000000  ///< IOE P2SB above 4G base address (PhysicalAddressBits - SIZE_256M

#endif
