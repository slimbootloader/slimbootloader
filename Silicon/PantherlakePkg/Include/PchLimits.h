/** @file
  Build time limits of PCH resources.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_LIMITS_H_
#define _PCH_LIMITS_H_


//
// PCIe limits
//
#define MAX_PCIE_ROOT_PORTS                 29

#define PCH_MAX_PCIE_ROOT_PORTS             12
#define PCH_MAX_PCIE_CONTROLLERS            5

//
// DMI lanes
//
#define PCH_MAX_DMI_LANES                   8


//
// SerialIo limits
//
#define PCH_MAX_SERIALIO_I2C_CONTROLLERS      6
#define PCH_MAX_SERIALIO_I3C_CONTROLLERS      3
#define PCH_MAX_SERIALIO_SPI_CONTROLLERS      4
#define PCH_MAX_SERIALIO_SPI_CHIP_SELECTS     2
#define PCH_MAX_SERIALIO_UART_CONTROLLERS     3


//
// Number of UFS controllers
//
#define PCH_MAX_UFS_DEV_NUM                 2


#endif // _PCH_LIMITS_H_
