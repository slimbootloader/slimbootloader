/** @file
  Helper functions for IppCryptoLib

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include "IppCryptoSupport.h"

void *
memset (
  void *dest,
  int ch,
  size_t count
  )
{
  return SetMem(dest, (UINTN)(count),(UINT8)(ch));
}


void *
memcpy (
  void *dest_str,
  const void * src_str,
  size_t n
  )
{
  return CopyMem(dest_str,src_str, (UINTN)(n));
}
