/** @file
  Header file for ITBT PCIE IP specific defines.

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _SA_REGS_ITBT_PCIE_H_
#define _SA_REGS_ITBT_PCIE_H_

#define MAX_ITBT_PCIE_PORT                   0x4
#define SA_ITBT_PCIE_BUS_NUM                 0x0 // Bus      #0
#define SA_ITBT_PCIE_DEV_NUM                 0x7 // Device   #7
#define SA_ITBT_PCIE_0_DEFAULT_FUN_NUM       0x0 // Function #0
#define SA_ITBT_PCIE_1_DEFAULT_FUN_NUM       0x1 // Function #1
#define SA_ITBT_PCIE_2_DEFAULT_FUN_NUM       0x2 // Function #2
#define SA_ITBT_PCIE_3_DEFAULT_FUN_NUM       0x3 // Function #3


typedef struct {
  UINT8   Segment;
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  BOOLEAN Enable;
} ITBT_PCIE_RP_INFO;

#endif
