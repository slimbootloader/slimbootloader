/** @file
  Register names for IGD block

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IGD_REGS_H_
#define _IGD_REGS_H_

///
/// Device 2 Register Equates
///
//
// The following equates must be reviewed and revised when the specification is ready.
//
#define IGD_BUS_NUM          0x00
#define IGD_DEV_NUM          0x02
#define IGD_FUN_NUM          0x00

#define GTT_MM_ADDR                          0xCF000000
#define LMEM_BAR                             0xD0000000

#endif // _IGD_REGS_H_
