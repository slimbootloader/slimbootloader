/** @file
  Build time limits of PCH resources.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PCH_LIMITS_H_
#define _PCH_LIMITS_H_

//
// PCIe limits
//
#define PCH_MAX_PCIE_ROOT_PORTS             24
#define PCH_MAX_PCIE_CONTROLLERS            6

//
// PCIe clocks limits
//
#define PCH_MAX_PCIE_CLOCKS                 16

//
// DMI lanes
//
#define PCH_MAX_DMI_LANES                   8

//
// PCH PSF segments
//
#define PCH_MAX_PSF_SEGMENTS                8

//
// RST PCIe Storage Cycle Router limits
//
#define PCH_MAX_RST_PCIE_STORAGE_CR         3

//
// SATA limits
//
#define PCH_MAX_SATA_CONTROLLERS            3
#define PCH_MAX_SATA_PORTS                  8

//
// USB limits
//
#define PCH_MAX_USB2_PORTS                  16
#define PCH_MAX_USB3_PORTS                  10

//
// SerialIo limits
//
#define PCH_MAX_SERIALIO_CONTROLLERS         12
#define PCH_MAX_SERIALIO_I2C_CONTROLLERS      6
#define PCH_MAX_SERIALIO_SPI_CONTROLLERS      3
#define PCH_MAX_SERIALIO_UART_CONTROLLERS     3

//
// Number of eSPI slaves
//
#define PCH_MAX_ESPI_SLAVES                  2

#endif // _PCH_LIMITS_H_
