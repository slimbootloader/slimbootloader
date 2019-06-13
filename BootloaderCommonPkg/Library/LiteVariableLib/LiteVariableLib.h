/** @file
Lite variable service library header file

Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _LITE_VARIABLE_LIB_H_
#define _LITE_VARIABLE_LIB_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/ExtraBaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Service/SpiFlashService.h>
#include "VariableFormat.h"


///
/// Private data structure definitions for the driver
///
#define VARIABLE_INSTANCE_SIGNATURE  SIGNATURE_32 ('V', 'A', 'R', 'I')

typedef struct {
  UINT32                Signature;
  UINT32                StoreSize;
  UINT32                StoreBase;
} VARIABLE_INSTANCE;

#endif
