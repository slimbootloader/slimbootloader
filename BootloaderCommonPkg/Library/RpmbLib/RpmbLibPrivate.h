/** @file

  Provides some macro definitions used by the RPMB Library.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
