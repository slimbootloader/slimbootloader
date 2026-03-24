/** @file
  Register definition for IOM IP

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IOM_REGS_H_
#define _IOM_REGS_H_

#define V_IOM_BIOS_GEM_SB_TRAN_CMD      0x2

#define V_IOM_BIOS_MBX_GET_HPD_COUNT    0x3
#define V_IOM_BIOS_MBX_DEC_HPD_COUNT    0x6
#define R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1  0x00000040U       ///< IOM_TYPEC_SW_CONFIGURATION 1

#define R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_2  0x00000044U       ///< IOM_TYPEC_SW_CONFIGURATION 2

#define R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_3  0x00000048U       ///< IOM_TYPEC_SW_CONFIGURATION 3

#define R_IOM_PCR_IOM_BIOS_MAILBOX_CMD          0x00000070U       ///< BIOS_MAILBOX_CMD_REG Register

#endif // _IOM_REGS_H_
