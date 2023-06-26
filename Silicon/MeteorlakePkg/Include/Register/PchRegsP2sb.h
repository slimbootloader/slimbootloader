/** @file
  Register names for PCH P2SB device

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _P2SB_REGS_H_
#define _P2SB_REGS_H_

//
// PCI to P2SB Bridge Registers
//
#define PCI_DEVICE_NUMBER_PCH_P2SB                 31
#define PCI_FUNCTION_NUMBER_PCH_P2SB               1


#define R_IO_APIC_MEM_INDEX_OFFSET                 0x00
#define R_IO_APIC_MEM_DATA_OFFSET                  0x10
#define V_P2SB_CFG_IBDF_BUS                        0
#define V_P2SB_CFG_IBDF_DEV                        30
#define V_P2SB_CFG_IBDF_FUNC                       7
#define V_P2SB_CFG_HBDF_BUS                        0
#define V_P2SB_CFG_HBDF_DEV                        30
#define V_P2SB_CFG_HBDF_FUNC                       6


#endif
