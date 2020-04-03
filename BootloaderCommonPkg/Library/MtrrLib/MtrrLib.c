/** @file
  MTRR related functions.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

/**
  Convert the MTRR memory type to a readable str

  @param[in]  type     MTRR type to convert to a string.

  @retval  String return based on the MTRR type

**/
CONST CHAR8*
MtrrTypeToStr (
  IN UINTN   Type
  )
{
  switch (Type) {
  case 0x00:
    return "Uncacheable";
  case 0x01:
    return "Write Combining";
  case 0x04:
    return "Write-Through";
  case 0x05:
    return "Write-Protected";
  case 0x06:
    return "Write-Back";
  default:
    return "Unknown";
  }
}

/**
  Print MTRR settings

  @param[in]  Str     A string will be print before MTRR data if it is not NULL.

**/
VOID
EFIAPI
PrintMtrr (
  IN CHAR8    *Str
  )
{
  UINT32      Index;
  UINT32      BaseMsr;
  UINT32      MaskMsr;
  UINT32      Type;
  BOOLEAN     MtrrEnabled;
  BOOLEAN     FixedMtrrEnabled;
  BOOLEAN     Valid;
  UINT64      DefType;
  UINT64      BaseVal;
  UINT64      Base;
  UINT64      MaskVal;
  UINT64      Mask;
  UINT64      Range;
  UINT64      Limit;

  if (Str != NULL) {
    DEBUG ((DEBUG_INFO, "%a\n", Str));
  }

  // Dump IA32_MTRR_DEF_TYPE
  DefType          = AsmReadMsr64 (0x2ff);
  MtrrEnabled      = ((DefType & (1 << 11)) != 0);
  FixedMtrrEnabled = ((DefType & (1 << 10)) != 0);
  DEBUG ((DEBUG_INFO, "MTRRs..............: %a\n", MtrrEnabled      ? "Enabled" : "Disabled"));
  DEBUG ((DEBUG_INFO, "Fixed MTRRs........: %a\n", FixedMtrrEnabled ? "Enabled" : "Disabled"));
  DEBUG ((DEBUG_INFO, "Default............: %a\n\n", MtrrTypeToStr (DefType & 0xff)));

  // Dump Variable MTRR registers
  DEBUG ((DEBUG_INFO, "Variable MTRRs\n"));
  for (Index = 0; Index < 10; Index++) {
    BaseMsr = 0x200 + (Index * 2);
    MaskMsr = BaseMsr + 1;

    BaseVal = AsmReadMsr64 (BaseMsr);
    MaskVal = AsmReadMsr64 (MaskMsr);

    Valid = ((MaskVal & (1 << 11)) != 0);
    if (!Valid) {
      continue;
    }

    Type  = BaseVal & 0xff;
    Base  = BaseVal & 0xffffff000ull;
    Mask  = MaskVal & 0xffffff000ull;
    Range = (~Mask & 0xfffffffffull);
    Limit = Base + Range;

    DEBUG ((DEBUG_INFO, "[0x%09llx-0x%09llx] %a\n", Base, Limit, MtrrTypeToStr (Type)));
    DEBUG ((DEBUG_INFO, "   IA32_MTRR_PHYSBASE_%d (0x%x): 0x%016llx\n", Index, BaseMsr, BaseVal));
    DEBUG ((DEBUG_INFO, "   IA32_MTRR_PHYSMASK_%d (0x%x): 0x%016llx\n", Index, MaskMsr, MaskVal));
  }
}

