/** @file
  Build time limits of PCH resources.

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_LIMITS_H_
#define _PCH_LIMITS_H_

//
// HSIO limits
//
#define PCH_MAX_HSIO_LANES                  38

//
// PCIe limits
//
#define MAX_PCIE_ROOT_PORTS                 29

#define PCH_MAX_PCIE_ROOT_PORTS             12
#define PCH_MAX_PCIE_CONTROLLERS            5

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
// SerialIo limits
//
#define PCH_MAX_SERIALIO_I2C_CONTROLLERS      6
#define PCH_MAX_SERIALIO_I3C_CONTROLLERS      3
#define PCH_MAX_SERIALIO_SPI_CONTROLLERS      4
#define PCH_MAX_SERIALIO_SPI_CHIP_SELECTS     2
#define PCH_MAX_SERIALIO_UART_CONTROLLERS     3

//
// THC limits
//
#define PCH_MAX_THC_CONTROLLERS               2
//
// ISH limits
//
#define PCH_MAX_ISH_GP_PINS                   11
#define PCH_MAX_ISH_UART_CONTROLLERS          2
#define PCH_MAX_ISH_I2C_CONTROLLERS           3
#define PCH_MAX_ISH_I3C_CONTROLLERS           2
#define PCH_MAX_ISH_SPI_CONTROLLERS           1
#define PCH_MAX_ISH_SPI_CS_PINS               1
//
// HDA limits
//
#define PCH_MAX_HDA_SDI                       2
#define PCH_MAX_HDA_SSP_LINK_NUM              3
#define PCH_MAX_HDA_DMIC_LINK_NUM             2
#define PCH_MAX_HDA_SNDW_LINK_NUM             5
#define PCH_MAX_HDA_SNDW_MULTILANE_NUM        2

//
// Number of eSPI devices
//
#define PCH_MAX_ESPI_DEVICES                 4

//
// Number of UFS controllers
//
#define PCH_MAX_UFS_DEV_NUM                 2

#define SETUP_MAX_USB2_PORTS                16
#define SETUP_MAX_USB3_PORTS                10
#define PCH_MAX_ADR_SOURCE_NUMBER           25


#endif // _PCH_LIMITS_H_
