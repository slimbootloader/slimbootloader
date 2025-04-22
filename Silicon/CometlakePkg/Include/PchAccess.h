/** @file
  Macros that simplify accessing PCH devices's PCI registers.

  Copyright (c) 1999-2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

@par Specification Reference:
**/
#ifndef _PCH_ACCESS_H_
#define _PCH_ACCESS_H_

#include "PchLimits.h"
#include "PchReservedResources.h"

#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 1
#endif
#ifndef STALL_ONE_SECOND
#define STALL_ONE_SECOND 1000000
#endif


//
// Include device register definitions
//

#include "Register/PchRegs.h"
#include "Register/PchRegsPcr.h"
#include "Register/PchRegsLpc.h"
#include "Register/PchRegsDmi.h"

#endif
