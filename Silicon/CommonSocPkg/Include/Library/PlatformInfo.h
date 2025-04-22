/** @file
  Header file for Platform Info. Platform can provide an implementation.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PLATFORM_INFO_H_
#define _PLATFORM_INFO_H_

#include <CpuRegs.h>

/**
  Get CPU Family ID

  @retval CPU Family ID
**/
CPU_FAMILY
EFIAPI
GetCpuFamily (
  VOID
);

/**
  Get Maximum CPU Pcie Root Port Number

  @retval Maximum CPU Pcie Root Port Number
**/
UINT8
GetMaxCpuPciePortNum (
  VOID
);
#endif // _PLATFORM_INFO_H_
