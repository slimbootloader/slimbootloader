/** @file
  Build time limits of PCH resources.

 Copyright (c) 2013 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_LIMITS_H_
#define _PCH_LIMITS_H_

//
// PCIe limits
//
#define PCH_MAX_PCIE_ROOT_PORTS             CNL_PCH_H_PCIE_MAX_ROOT_PORTS
#define CNL_PCH_H_PCIE_MAX_ROOT_PORTS       24
#define PCH_LP_PCIE_MAX_ROOT_PORTS          16

#define PCH_PCIE_CONTROLLER_PORTS           4u


//
// RST PCIe Storage Cycle Router limits
//
#define PCH_MAX_RST_PCIE_STORAGE_CR         3

//
// SATA limits
//
#define SATA_1_CONTROLLER_INDEX             0
#define CDF_PCH_SATA_CONTROLLER_MAX         3       ///< Max number of SATA controller in CDF PCH
#define PCH_SATA_CONTROLLER_MAX             CDF_PCH_SATA_CONTROLLER_MAX ///< Max number of SATA controller

//
// SerialIo limits
//
#define PCH_MAX_SERIALIO_SPI_CONTROLLERS     3
#define PCH_MAX_SERIALIO_UART_CONTROLLERS    3


#define PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS  14      ///< Max Physical Connector XHCI, not counting virtual ports like USB-R.
#define PCH_LP_XHCI_MAX_USB2_PHYSICAL_PORTS 10      ///< Max Physical Connector XHCI, not counting virtual ports like USB-R.

#define PCH_H_XHCI_MAX_USB2_PORTS           16      ///< 14 High Speed lanes + Including two ports reserved for USBr
#define PCH_LP_XHCI_MAX_USB2_PORTS          12      ///< 10 High Speed lanes + Including two ports reserved for USBr


#define PCH_H_XHCI_MAX_USB3_PORTS           10      ///< 10 Super Speed lanes
#define PCH_LP_XHCI_MAX_USB3_PORTS          6       ///< 6 Super Speed lanes


//
// SerialIo limits
//
#define PCH_SERIALIO_MAX_CONTROLLERS         12  ///< Number of SerialIo controllers, this includes I2C, SPI and UART
#define PCH_SERIALIO_MAX_SPI_CONTROLLERS      3  ///< Number of SerialIo SPI controllers
#define PCH_SERIALIO_MAX_UART_CONTROLLERS     3  ///< Number of SerialIo UART controllers

//
// ISH limits
//
#define PCH_ISH_MAX_GP_PINS                   8
#define PCH_ISH_MAX_UART_CONTROLLERS          2
#define PCH_ISH_MAX_I2C_CONTROLLERS           3
#define PCH_ISH_MAX_SPI_CONTROLLERS           1


//
// HDAudio limits
//
#define PCH_MAX_HDA_SSP_LINK_NUM              6
#define PCH_MAX_HDA_DMIC_LINK_NUM             2
#define PCH_MAX_HDA_SNDW_LINK_NUM             4


//
// PCH Thermal limits
//
enum PCH_PMSYNC_GPIO_X_SELECTION {
  TsGpioC,
  TsGpioD,
  MaxTsGpioPin
};

#endif // _PCH_LIMITS_H_
