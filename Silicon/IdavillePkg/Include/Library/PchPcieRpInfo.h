/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_PCIERP_CDF_INFO_H_
#define _PCH_PCIERP_CDF_INFO_H_

//
// Number of PCIe ports per PCIe controller
//
#define PCH_PCIE_CONTROLLER_PORTS                     4u


typedef enum RpIdxPerController {
  Rp0PerController = 0,
  Rp1PerController,
  Rp2PerController,
  Rp3PerController
} PCIE_RP_PER_CONTROLLER;
//
// PCH PCI Express Root Ports for single function devices in CDF
//
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1           9
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2           10
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3           11
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_4           12
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_5           16
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_6           17
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_7           18
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_8           19
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_9           20
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_10          21
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_11          22
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_12          23


#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1      0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2      0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3      0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4      0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5      0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6      0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7      0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8      0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9      0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_10     0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_11     0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_12     0

#endif
