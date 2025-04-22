/** @file
  Header file for LowPowerSupport.

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _CPU_PCIE_INFO_H_
#define _CPU_PCIE_INFO_H_


/**
PCIe controller configuration.
**/


//
// Device 1 Memory Mapped IO Register Offset Equates
//
#define SA_PEG_BUS_NUM     0x00
#define SA_PEG_DEV_NUM     0x01
#define SA_PEG0_DEV_NUM    SA_PEG_DEV_NUM
#define SA_PEG0_FUN_NUM    0x00
#define SA_PEG1_DEV_NUM    SA_PEG_DEV_NUM
#define SA_PEG3_DEV_NUM    0x06


#endif
