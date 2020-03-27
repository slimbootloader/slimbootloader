/** @file
  CpuBreakpoint function.

  Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/**
  Microsoft Visual Studio 7.1 Function Prototypes for I/O Intrinsics.
**/

void __writemsr (unsigned long Register, unsigned __int64 Value);

#pragma intrinsic(__writemsr)

/**
  Write data to MSR.

  @param  Index                The register index of MSR.
  @param  Value                Data wants to be written.

  @return Value written to MSR.

**/
UINT64
EFIAPI
AsmWriteMsr64 (
  IN UINT32  Index,
  IN UINT64  Value
  )
{
  __writemsr (Index, Value);
  return Value;
}

