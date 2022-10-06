/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __REG_ACCESS_H__
#define __REG_ACCESS_H__

#include "Register/IioRegs.h"
#include "Register/UBOX_MISC.h"
#include "Register/PchRegs.h"

#define R_IOPORT_CMOS_STANDARD_INDEX        0x70
#define R_IOPORT_CMOS_STANDARD_DATA         0x71

#define ONLY_REGISTER_OFFSET(x)             (x & 0xFFF)

#endif /* __REG_ACCESS_H__ */
