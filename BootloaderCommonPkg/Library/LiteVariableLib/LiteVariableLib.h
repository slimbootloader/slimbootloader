/** @file
Lite variable service library header file

Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
