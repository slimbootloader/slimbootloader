/** @file
  memcpy/memset for compiler-generated calls in builds without a C runtime.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseMemoryLib.h>

/**
  Copy a buffer.

  @param[in]  Dest    Destination buffer.
  @param[in]  Src     Source buffer.
  @param[in]  Count   Number of bytes to copy.

  @retval     Dest.

**/
VOID *
memcpy (
  VOID        *Dest,
  CONST VOID  *Src,
  UINTN       Count
  )
{
  return CopyMem (Dest, Src, Count);
}

/**
  Fill a buffer with a byte value.

  @param[in]  Dest    Buffer to fill.
  @param[in]  Value   Byte value, in the low 8 bits.
  @param[in]  Count   Number of bytes to fill.

  @retval     Dest.

**/
VOID *
memset (
  VOID   *Dest,
  INT32  Value,
  UINTN  Count
  )
{
  return SetMem (Dest, Count, (UINT8)Value);
}
