/** @file
  Chipset definition for ME Devices.

 Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _ME_CHIPSET_H_
#define _ME_CHIPSET_H_

#define ME_BUS                            0
#define ME_DEVICE_NUMBER                  22
#define HECI_FUNCTION_NUMBER              0

typedef enum {
  HECI1_DEVICE = 0,
  HECI2_DEVICE = 1,
  HECI3_DEVICE = 4,
  HECI4_DEVICE = 5,
  ISH_HECI     = 8
} HECI_DEVICE;

#define R_ME_GS_SHDW                      0x48
#define R_ME_HFS                          0x40
#define R_ME_HFS_2                        0x48
#define R_ME_HFS_3                        0x60
#define R_ME_HFS_4                        0x64
#define R_ME_HFS_5                        0x68
#define R_ME_HFS_6                        0x6C
#define B_BOOT_GUARD_ENF_MASK             0x0200
#define B_TPM_DISCONNECT                  0x1000

#endif // _ME_CHIPSET_H_
