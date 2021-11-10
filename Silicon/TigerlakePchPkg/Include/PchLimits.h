/** @file
  Build time limits of PCH resources.

  Copyright (c) 2013 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

@par Glossary:
 - CDF - Cedar Fork
**/
#ifndef _PCH_LIMITS_H_
#define _PCH_LIMITS_H_

//
// PCIe limits
//
#define PCH_MAX_PCIE_ROOT_PORTS             CNL_PCH_H_PCIE_MAX_ROOT_PORTS
#define CNL_PCH_H_PCIE_MAX_ROOT_PORTS       24

#define PCH_PCIE_CONTROLLER_PORTS           4u

//
// SATA limits
//
#define SATA_1_CONTROLLER_INDEX             0
#define CDF_PCH_SATA_CONTROLLER_MAX         3       ///< Max number of SATA controller in CDF PCH
#define PCH_SATA_CONTROLLER_MAX             CDF_PCH_SATA_CONTROLLER_MAX ///< Max number of SATA controller
#define PCH_H_AHCI_MAX_PORTS                8       ///< Max number of sata ports in CNL PCH H
#define PCH_LP_AHCI_MAX_PORTS               3       ///< Max number of sata ports in CNL PCH LP

//
// SerialIo limits
//
#define PCH_MAX_SERIALIO_I2C_CONTROLLERS      8
#define PCH_MAX_SERIALIO_SPI_CONTROLLERS      7
#define PCH_MAX_SERIALIO_SPI_CHIP_SELECTS     2
#define PCH_MAX_SERIALIO_UART_CONTROLLERS     3

//
// ISH limits
//
#ifdef PCH_ICL
#define PCH_ISH_MAX_I2C_CONTROLLERS           3
#else
#define PCH_ISH_MAX_I2C_CONTROLLERS           6
#endif

//
// HDAudio limits
//
#ifdef PCH_ICL
#define PCH_HDA_SSP_MAX_INTERFACES         6  ///< Number of HDAudio I2S/SSP interfaces
#else
#define PCH_HDA_SSP_MAX_INTERFACES         3  ///< Number of HDAudio I2S/SSP interfaces
#endif

#endif // _PCH_LIMITS_H_
