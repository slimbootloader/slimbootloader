/** @file

  Provides some macro definitions used by the RPMB Library.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _RPMB_LIB_PRIVATE_H_
#define _RPMB_LIB_PRIVATE_H_

#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Emmc.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#define RPMB_DATA_FRAME_SIZE            512
#define RPMB_DATA_MAC                   32
#define RPMB_MAC_SIZE                   32
#define RPMB_NONCE_SIZE                 16

// Length of part of the frame used for HMAC computation
#define HMAC_DATA_LEN \
  (sizeof(RPMB_DATA_FRAME) - OFFSET_OF(RPMB_DATA_FRAME, Data))

#endif
