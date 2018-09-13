/** @file
These functions assist in parsing and manipulating a Firmware Volume.

Copyright (c) 2004 - 2014, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_FV_LIB_H_
#define _EFI_FV_LIB_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#define  THREE_BYTE_LEN(x)    ((*(UINT32 *)(UINTN)(x)) & 0xFFFFFF)
#define  GET_FFS_LENGTH(x)    THREE_BYTE_LEN((x)->Size)
#define  GET_SEC_LENGTH(x)    THREE_BYTE_LEN((x)->Size)

#endif
