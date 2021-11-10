/** @file
  Chipset definition for ME Devices.

 Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _ME_CHIPSET_H_
#define _ME_CHIPSET_H_

#define ME_BUS                            0
#define ME_DEVICE_NUMBER                  15
#define HECI_FUNCTION_NUMBER              0

#define H_CSR                             0x04
#define ME_CSR_HA                         0x0C
#define R_ME_HFS                          0x40

typedef enum {
  HECI1_DEVICE        = 0,
  HECI_INVALID_DEVICE = 0xFF
} HECI_DEVICE;

#endif // _ME_CHIPSET_H_
