/** @file
  Pcie Root Port info header

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PCH_PCIERP_INFO_H_
#define _PCH_PCIERP_INFO_H_

//
// Number of PCIe ports per PCIe controller
//
#define PCH_PCIE_CONTROLLER_PORTS                     4u

//
// Number of PCIe lanes per PCIe port
//
#define PCH_PCIE_LANES_PER_PORT                       1

//
// Number of PCIe lanes per PCIe controller
//
#define PCH_PCIE_LANES_PER_CONTROLLER                 (PCH_PCIE_CONTROLLER_PORTS * PCH_PCIE_LANES_PER_PORT)

#endif
