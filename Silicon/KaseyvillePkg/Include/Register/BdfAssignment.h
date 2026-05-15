/** @file
  Header file for GNRD PCI Bus Device Function map.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_BDF_ASSIGNMENT_H_
#define _PCH_BDF_ASSIGNMENT_H_

#define PCI_DEFAULT_DEVICE_NUMBER     0
#define DEFAULT_PCI_BUS_NUMBER_PCH    0

//
// USB (XHCI) Controller PCI config
//
#define PCI_BUS_NUMBER_XHCI_1     0XB4
#define PCI_BUS_NUMBER_XHCI_2     0XB8

//
// NVME PCI config
//
#define PCI_FUNCTION_NUMBER_NVME  0
#define PCI_BUS_NUMBER_NVME       0xB9

#endif // _PCH_BDF_ASSIGNMENT_H_
