/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_SCS_H_
#define _PCH_REGS_SCS_H_

//
//  SCS eMMC Controller PCI config
//
#define PCI_DEVICE_NUMBER_PCH_CDF_SCS_EMMC        28
#define PCI_FUNCTION_NUMBER_PCH_CDF_SCS_EMMC      0

//
// SCS Devices proprietary PCI Config Space Registers
//
#define R_SCS_CFG_PCS                     0x84                          ///< PME Control Status
#define R_SCS_CFG_PG_CONFIG               0xA2                          ///< PG Config

#endif
