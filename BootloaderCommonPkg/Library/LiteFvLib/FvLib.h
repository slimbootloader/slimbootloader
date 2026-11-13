/** @file
These functions assist in parsing and manipulating a Firmware Volume.

Copyright (c) 2004 - 2014, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _EFI_FV_LIB_H_
#define _EFI_FV_LIB_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/BootloaderCommonLib.h>

// Read byte-wise: Size is a UINT8[3] array, a UINT32 load would read past it.
#define  THREE_BYTE_LEN(x)    ((UINT32)(x)[0] | ((UINT32)(x)[1] << 8) | ((UINT32)(x)[2] << 16))
#define  GET_FFS_LENGTH(x)    THREE_BYTE_LEN((x)->Size)
#define  GET_SEC_LENGTH(x)    THREE_BYTE_LEN((x)->Size)

#endif
