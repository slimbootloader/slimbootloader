/** @file
  PCH preserved MMIO resource definitions.

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef __PCH_PRESERVED_RESOURCES_H__
#define __PCH_PRESERVED_RESOURCES_H__

#include <Library/PcdLib.h>

/**
  Detailed recommended static allocation
  +-------------------------------------------------------------------------------+
  | PCH preserved MMIO range, 16 MB, from 0xFD800000 to 0xFE7FFFFF                |
  +-------------------------------------------------------------------------------+
  | Size        | Start       | End         | Usage                               |
  | 8 MB        | 0xFD800000  | 0xFDFFFFFF  | TraceHub SW BAR                     |
  | 64 KB       | 0xFE000000  | 0xFE00FFFF  | PMC MBAR                            |
  | 4 KB        | 0xFE010000  | 0xFE010FFF  | SPI BAR0                            |
  | 176 KB      | 0xFE020000  | 0xFE04BFFF  | SerialIo BAR in ACPI mode           |
  | 400 KB      | 0xFE04C000  | 0xFE0AFFFF  | Unused                              |
  | 64 KB       | 0xFE0B0000  | 0xFE0BFFFF  | eSPI LGMR BAR                       |
  | 64 KB       | 0xFE0C0000  | 0xFE0CFFFF  | eSPI2 SEGMR BAR                     |
  | 192 KB      | 0xFE0D0000  | 0xFE0FFFFF  | Unused                              |
  | 1 MB        | 0xFE100000  | 0xFE1FFFFF  | TraceHub MTB BAR                    |
  | 2 MB        | 0xFE200000  | 0xFE3FFFFF  | TraceHub FW BAR                     |
  | 2 MB        | 0xFE400000  | 0xFE5FFFFF  | Unused                              |
  | 2 MB        | 0xFE600000  | 0xFE7FFFFF  | Temp address                        |
  +-------------------------------------------------------------------------------+
  | SBREG MMIO range                                                              |
  +-------------------------------------------------------------------------------+
  | 16 MB OR    | 0xE0000000  | 0xE0FFFFFF  | SBREG                               |
  | 256 MB      | 0xE0000000  | 0xEFFFFFFF  | SBREG_EX (SBREG_BAR_20BITADDRESS=1) |
  +-------------------------------------------------------------------------------+
**/
#if FixedPcdGetBool(PcdAdlLpSupport) == 1
#define PCH_PRESERVED_BASE_ADDRESS      0xFC800000     ///< Pch preserved MMIO base address
#define PCH_PRESERVED_MMIO_SIZE         0x02000000     ///< 32MB
#define PCH_TRACE_HUB_SW_BASE_ADDRESS   0xFC800000     ///< TraceHub SW MMIO base address
#else
#define PCH_PRESERVED_BASE_ADDRESS      0xFD800000     ///< Pch preserved MMIO base address
#define PCH_PRESERVED_MMIO_SIZE         0x01000000     ///< 16MB
#define PCH_TRACE_HUB_SW_BASE_ADDRESS   0xFD800000     ///< TraceHub SW MMIO base address
#endif
#define PCH_TRACE_HUB_SW_MMIO_SIZE      0x00800000     ///< 8MB
#define PCH_PWRM_BASE_ADDRESS           0xFE000000     ///< PMC MBAR MMIO base address
#define PCH_SPI_BASE_ADDRESS            0xFE010000     ///< SPI BAR0 MMIO base address
#define PCH_SERIAL_IO_BASE_ADDRESS      0xFE020000     ///< SerialIo MMIO base address
#define PCH_TRACE_HUB_MTB_BASE_ADDRESS  0xFE100000     ///< TraceHub MTB MMIO base address
#define PCH_TRACE_HUB_MTB_MMIO_SIZE     0x00100000     ///< 1MB
#define PCH_TRACE_HUB_FW_BASE_ADDRESS   0xFE200000     ///< TraceHub FW MMIO base address

#define PCH_PCR_BASE_ADDRESS            (IsPchS () ? 0xE0000000 : (IsPchLp () ? 0xFD000000 : 0))     ///< SBREG MMIO base address
#if FixedPcdGetBool(PcdAdlLpSupport) == 0
#endif

#endif // __PCH_PRESERVED_RESOURCES_H__
