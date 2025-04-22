/** @file
  Header file for LowPowerSupport.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _LOW_POWER_SUPPORT_H_
#define _LOW_POWER_SUPPORT_H_

/**
 Update Pep constraints.

 @param  Void
 @retval Updated Pep constraints Value
**/

UINT64
GetLowPowerS0IdleConstraint (
  VOID
  );

#endif //LOW_POWER_SUPPORT_H_
