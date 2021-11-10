/** @file
  Register Definitions for Me States

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _ME_STATE_H
#define _ME_STATE_H
///
/// Ignore ME_FW_INIT_COMPLETE status Macro
///
#define ME_STATUS_ME_STATE_ONLY(a)  ((a) & (~(ME_FW_INIT_COMPLETE | ME_FW_BOOT_OPTIONS_PRESENT)))

///
/// Macro to check if ME FW INIT is completed
///
#define ME_STATUS_IS_ME_FW_INIT_COMPLETE(a) (((a) & ME_FW_INIT_COMPLETE) == ME_FW_INIT_COMPLETE)

///
/// Macro to combine the complete bit to status
///
#define ME_STATUS_WITH_ME_INIT_COMPLETE(a)  ((a) | ME_FW_INIT_COMPLETE)

///
/// Macro to check ME Boot Option Present
///
#define ME_STATUS_IS_ME_FW_BOOT_OPTIONS_PRESENT(a)  (((a) & ME_FW_BOOT_OPTIONS_PRESENT) == ME_FW_BOOT_OPTIONS_PRESENT)

//
// Abstract ME Mode Definitions
//
#define ME_MODE_NORMAL        0x00
#define ME_MODE_DEBUG         0x02
#define ME_MODE_TEMP_DISABLED 0x03
#define ME_MODE_SECOVER       0x04
#define ME_MODE_SPS           0x05  // Server SPS firmware is running in ME
#define ME_MODE_FAILED        0x06

//
// Abstract ME Status definitions
//
#define ME_READY                    0x00
#define ME_INITIALIZING             0x01
#define ME_IN_RECOVERY_MODE         0x02
#define ME_DISABLE_WAIT             0x06
#define ME_TRANSITION               0x07
#define ME_NOT_READY                0x0F
#define ME_FW_INIT_COMPLETE         0x80
#define ME_FW_BOOT_OPTIONS_PRESENT  0x100
#define ME_FW_UPDATES_IN_PROGRESS   0x200


#endif // ME_STATE_H
