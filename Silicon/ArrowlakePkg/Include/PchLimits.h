/** @file
  Build time limits of PCH resources.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
// SATA limits
//
#define MAX_SATA_CONTROLLER                 1
#define MAX_SATA_PORTS_PCH_S                8
#define MAX_SATA_PORTS_MTL_SOC_M            2
#define SATA_1_CONTROLLER_INDEX             0


//
// SerialIo limits
//
#define PCH_MAX_SERIALIO_I2C_CONTROLLERS      6
#define PCH_MAX_SERIALIO_SPI_CONTROLLERS      3
#define PCH_MAX_SERIALIO_UART_CONTROLLERS     3


// VMD
#define VMD_DUMMY_DEVICE_ID                 (0x09AB) // Dummy Function DID

//
// Number of UFS controllers
//
#define PCH_MAX_UFS_DEV_NUM                  2

#endif // _PCH_LIMITS_H_
