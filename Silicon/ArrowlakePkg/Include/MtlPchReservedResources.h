/** @file
  MTL PCH reserved MMIO resource definitions.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _MTL_PCH_PRESERVED_RESOURCES_H_
#define _MTL_PCH_PRESERVED_RESOURCES_H_

#include <Base.h>

/**
  Detailed allocation
  +----------------------------------------------------------------------------------------------+
  | PCH preserved MMIO range, 32 MB, from ReservedMmioLimit - 32MB to ReservedMmioLimit          |
  +----------------------------------------------------------------------------------------------+
  | Size        | Start Offset| End Offset  | Usage                                              |
  | 8 MB        | 0x00000000  | 0x007FFFFF  | TraceHub SW BAR                                    |
  | 2 MB        | 0x00800000  | 0x009FFFFF  | TraceHub FW BAR                                    |
  | 2 MB        | 0x00A00000  | 0x00BFFFFF  | Temp address                                       |
  | 1 MB        | 0x00C00000  | 0x00CFFFFF  | TraceHub MTB BAR                                   |
  | 64 KB       | 0x00D00000  | 0x00D0FFFF  | PMC MBAR                                           |
  | 176 KB      | 0x00D10000  | 0x00D3BFFF  | SerialIo BAR in ACPI mode                          |
  | 64 KB       | 0x00D30000  | 0x00D3FFFF  | eSPI LGMR BAR                                      |
  | 64 KB       | 0x00D40000  | 0x00D4FFFF  | eSPI2 SEGMR BAR                                    |
  | 4 KB        | 0x00D50000  | 0x00D50FFF  | HECI1 MBAR                                         |
  | 4 KB        | 0x00D51000  | 0x00D51FFF  | HECI2 MBAR                                         |
  | 4 KB        | 0x00D52000  | 0x00D52FFF  | HECI3 MBAR                                         |
  +----------------------------------------------------------------------------------------------+
**/

//
// BASE_ADDRESS defines need to be added to the MTL PCH reserved MMIO base
//
#define MTL_PCH_RESERVED_MMIO_SIZE          0x02000000     ///< 32MB
#define MTL_PCH_TRACE_HUB_SW_OFFSET         0x00000000     ///< TraceHub SW MMIO base address
#define MTL_PCH_TRACE_HUB_SW_MMIO_SIZE      0x00800000     ///< 8MB
#define MTL_PCH_TRACE_HUB_FW_OFFSET         0x00800000     ///< TraceHub FW MMIO base address
#define MTL_PCH_TRACE_HUB_FW_MMIO_SIZE      0x00200000     ///< 2MB
#define MTL_PCH_TEMP_OFFSET                 0x00A00000     ///< preserved temp address for misc usage
#define MTL_PCH_TEMP_MMIO_SIZE              0x00200000     ///< 2MB
#define MTL_PCH_TRACE_HUB_MTB_OFFSET        0x00C00000     ///< TraceHub MTB MMIO base address
#define MTL_PCH_TRACE_HUB_MTB_MMIO_SIZE     0x00100000     ///< 1MB
#define MTL_PCH_PWRM_OFFSET                 0x00D00000     ///< PMC MBAR MMIO base address
#define MTL_PCH_PWRM_MMIO_SIZE              0x00010000     ///< 64KB
#define MTL_PCH_SERIAL_IO_OFFSET            0x00D10000     ///< SerialIo MMIO base address
#define MTL_PCH_SERIAL_IO_MMIO_SIZE         0x0002C000     ///< 176KB
#define MTL_PCH_SSRAM_OFFSET                0x00D3C000     ///< MTL IOE SSRAM  base address
#define MTL_PCH_SSRAM_MMIO_SIZE             0x00004000     ///< 16KB
#define MTL_PCH_ESPI_LGMR_OFFSET            0x00D40000     ///< eSPI LGMR MMIO base address
#define MTL_PCH_ESPI_LGMR_MMIO_SIZE         0x00010000     ///< 64KB
#define MTL_PCH_ESPI_SEGMR_OFFSET           0x00D50000     ///< Second eSPI GMR MMIO base address
#define MTL_PCH_ESPI_SEGMR_MMIO_SIZE        0x00010000     ///< 64KB
#define MTL_PCH_HECI1_OFFSET                0x00D60000     ///< HECI1 MMIO base address
#define MTL_PCH_HECI1_MMIO_SIZE             0x00001000     ///< 4KB
#define MTL_PCH_HECI2_OFFSET                0x00D61000     ///< HECI2 MMIO base address
#define MTL_PCH_HECI2_MMIO_SIZE             0x00001000     ///< 4KB
#define MTL_PCH_HECI3_OFFSET                0x00D62000     ///< HECI3 MMIO base address
#define MTL_PCH_HECI3_MMIO_SIZE             0x00001000     ///< 4KB

//
// MMIOs above 4G
//
#define MTL_PCH_P2SB_MMIO64_OFFSET          0x00000000     ///< P2SB MMIO base address
#define MTL_PCH_P2SB_MMIO64_SIZE            0x10000000     ///< 256MB

//
// IO
//
#define MTL_PCH_IO_RESERVED_SIZE            SIZE_1KB
#define MTL_PCH_PMC_ACPI_IO_OFFSET          0x00000000
#define MTL_PCH_PMC_ACPI_IO_SIZE            0x00000100
#define MTL_PCH_SMBUS_IO_OFFSET             0x00000100
#define MTL_PCH_SMBUS_IO_SIZE               0x00000100
#define MTL_PCH_TCO_IO_OFFSET               0x00000200
#define MTL_PCH_TCO_IO_SIZE                 0x00000100
#endif