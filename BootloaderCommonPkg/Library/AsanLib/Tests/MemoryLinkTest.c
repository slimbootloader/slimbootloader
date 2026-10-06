/** @file
  Freestanding instrumented caller for no-runtime Stage1-style COFF links.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Library/BaseMemoryLib.h>

STATIC UINT8  mSource[64];
STATIC UINT8  mDestination[64];

VOID
EFIAPI
MemoryEntry (
  VOID
  )
{
  CopyMem (mDestination, mSource, sizeof (mSource));
  SetMem (mDestination, sizeof (mDestination), 0xA5);
  __builtin_memcpy (mDestination, mSource, sizeof (mSource));
  __builtin_memset (mSource, 0x5A, sizeof (mSource));
  mDestination[0] = mSource[1];
}
