/** @file
  PCH preserved MMIO resource definitions.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef __PCH_PRESERVED_RESOURCES_H__
#define __PCH_PRESERVED_RESOURCES_H__

/**
  Detailed recommended static allocation
  +-------------------------------------------------------------------------+
  | PCH preserved MMIO range, 24 MB, from 0xF6800000 to 0xF8000000          |
  +-------------------------------------------------------------------------+
  | Size        | Start       | End         | Usage                         |
  | 64 KB       | 0xF6800000  | 0xF680FFFF  | PMC                           |
  | 64 KB       | 0xF6810000  | 0xF681FFFF  | eSPI LGMR BAR                 |
  | 64 KB       | 0xF6820000  | 0xF682FFFF  | eSPI2 SEGMR BAR               |
  | 4 KB        | 0xF6830000  | 0xF6830FFF  | SPI BAR0                      |
  | 828 KB      | 0xF6831000  | 0xF68FFFFF  | Unused space                  |
  | 4 MB        | 0xF6900000  | 0xF6CFFFFF  | Temp address                  |
  | 3 MB        | 0xF6D00000  | 0xF6FFFFFF  | Unused space                  |
  | 16 MB       | 0xF7000000  | 0xF7FFFFFF  | SBREG                         |
  +-------------------------------------------------------------------------+
**/
#define PCH_PRESERVED_BASE_ADDRESS      0xF6800000                                              ///< Pch preserved MMIO base address

#define PCH_PRESERVED_MMIO_SIZE         0x01800000                                              ///< 24 MB

#define PCH_PWRM_OFFSET                 0x00000000     ///< PMC offset
#define PCH_PWRM_BASE_ADDRESS          (PCH_PRESERVED_BASE_ADDRESS + PCH_PWRM_OFFSET)          ///< PMC base address
#define PCH_PWRM_MMIO_SIZE              0x00010000     ///< 64 KB
#define PCH_ESPI_LGMR_OFFSET            0x00010000     ///< eSPI LGMR offset
#define PCH_ESPI_LGMR_BASE_ADDRESS      (PCH_PRESERVED_BASE_ADDRESS + PCH_ESPI_LGMR_OFFSET)     ///< eSPI LGMR BAR base address
#define PCH_ESPI_LGMR_MMIO_SIZE         0x00010000     ///< 64 KB
#define PCH_ESPI_SEGMR_OFFSET           0x00020000     ///< eSPI2 SEGMR offset
#define PCH_ESPI_SEGMR_BASE_ADDRESS     (PCH_PRESERVED_BASE_ADDRESS + PCH_ESPI_SEGMR_OFFSET)    ///< eSPI2 SEGMR BAR base address
#define PCH_ESPI_SEGMR_MMIO_SIZE        0x00010000     ///< 64 KB
#define PCH_SPI_OFFSET                  0x00030000     ///< SPI BAR0 offset
#define PCH_SPI_BASE_ADDRESS            (PCH_PRESERVED_BASE_ADDRESS + PCH_SPI_OFFSET)           ///< SPI BAR0 base address
#define PCH_SPI_MMIO_SIZE               0x00001000     ///< 4 KB
#define PCH_TEMP_OFFSET                 0x00100000     ///< Temp offset
#define PCH_TEMP_BASE_ADDRESS           (PCH_PRESERVED_BASE_ADDRESS + PCH_TEMP_OFFSET)          ///< Temp address base address
#define PCH_TEMP_MMIO_SIZE              0x00400000     ///< 4 MB
#define PCH_PCR_OFFSET                  0x00800000     ///< SBREG offset, note: offset + base must be aligned to 16MB
#define PCH_PCR_BASE_ADDRESS            (PCH_PRESERVED_BASE_ADDRESS + PCH_PCR_OFFSET)           ///< SBREG base address
#define PCH_PCR_MMIO_SIZE               0x01000000     ///< 16 MB
#endif // __PCH_PRESERVED_RESOURCES_H__
