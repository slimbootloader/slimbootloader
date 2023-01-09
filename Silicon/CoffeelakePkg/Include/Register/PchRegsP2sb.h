/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_P2SB_H_
#define _PCH_REGS_P2SB_H_

#define PCI_DEVICE_NUMBER_PCH_P2SB                 31
#define PCI_FUNCTION_NUMBER_PCH_P2SB               1

#define R_P2SB_CFG_SBREG_BAR                       0x10

#define R_P2SB_CFG_SBIADDR                         0xD0

#define R_P2SB_CFG_SBIDATA                         0xD4

#define R_P2SB_CFG_SBISTAT                         0xD8
#define B_P2SB_CFG_SBISTAT_OPCODE                  0xFF00
#define B_P2SB_CFG_SBISTAT_POSTED                  BIT7
#define B_P2SB_CFG_SBISTAT_INITRDY                 BIT0
#define B_P2SB_CFG_SBISTAT_RESPONSE                0x0006
#define N_P2SB_CFG_SBISTAT_RESPONSE                1

#define R_P2SB_CFG_SBIRID                          0xDA

#define R_P2SB_CFG_SBIEXTADDR                      0xDC

#define R_P2SB_CFG_E0                              0xE0

#define V_P2SB_CFG_IBDF_BUS                        0
#define V_P2SB_CFG_IBDF_DEV                        30
#define V_P2SB_CFG_IBDF_FUNC                       7
#define V_P2SB_CFG_HBDF_BUS                        0
#define V_P2SB_CFG_HBDF_DEV                        30
#define V_P2SB_CFG_HBDF_FUNC                       6

#define R_IO_APIC_INDEX_OFFSET                     0x00
#define R_IO_APIC_DATA_OFFSET                      0x10


#endif
