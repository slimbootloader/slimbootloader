/** @file
  Minimal AddressSanitizer runtime for modules built with clang -fsanitize=address.

  Tracks heaps, stacks and instrumented globals. Every check and every compiler
  shadow write is an outlined callback, and the dynamic shadow base is 0, so
  callbacks receive Address >> 3 and accesses outside the tracked regions are
  ignored. The callbacks are in AsanStubLib, which forwards them to the runtime
  this library connects.

  Stage2 publishes its runtime as ASAN_SERVICE. A payload that finds it calls
  AsanUseService () and adds its own regions there, so one runtime tracks both.
  Otherwise the payload uses its own copy of the runtime.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/AsanLib.h>
#include <Library/AsanStubLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#define ASAN_SHADOW_SCALE          3
#define ASAN_GRANULE               (1 << ASAN_SHADOW_SCALE)
#define ASAN_GRANULE_MASK          (ASAN_GRANULE - 1)
#define ASAN_MAX_REGIONS           8
#define ASAN_MAX_IMAGES            4
#define ASAN_QUARANTINE_ENTRIES    64
#define ASAN_QUARANTINE_MAX_BLOCK  SIZE_64KB
#define ASAN_MAX_BACKTRACE         16
#define ASAN_MAX_SEARCH_GRANULES   512
#define ASAN_STACK_FRAME_MAGIC     0x41B58AB3

//
// Start of the image that links AsanLib, defined by lld-link.
//
extern UINT8  __ImageBase;

//
// Shadow values the compiler writes for stack frames and AsanLib writes for globals.
//
#define ASAN_SHADOW_STACK_LEFT          0xF1
#define ASAN_SHADOW_STACK_MID           0xF2
#define ASAN_SHADOW_STACK_RIGHT         0xF3
#define ASAN_SHADOW_STACK_AFTER_RETURN  0xF5
#define ASAN_SHADOW_STACK_AFTER_SCOPE   0xF8
#define ASAN_SHADOW_GLOBAL_REDZONE      0xF9

//
// Layout of the metadata clang emits for each instrumented global.
//
typedef struct {
  UINTN  Beg;
  UINTN  Size;
  UINTN  SizeWithRedzone;
  UINTN  Name;
  UINTN  ModuleName;
  UINTN  HasDynamicInit;
  UINTN  SourceLocation;
  UINTN  OdrIndicator;
} ASAN_GLOBAL;

typedef struct {
  UINTN              Base;
  UINTN              End;
  UINT8              *Shadow;
  UINT32             Type;
  CONST ASAN_GLOBAL  *Globals;
  CONST ASAN_GLOBAL  *GlobalsEnd;
} ASAN_REGION;

typedef struct {
  CONST CHAR8  *Name;
  UINTN        Base;
} ASAN_IMAGE;

//
// On COFF clang puts global metadata in .ASAN$GL; the linker sorts it between these markers.
// The markers are non-zero so they are emitted as initialized data, like the metadata.
//
__attribute__((section (".ASAN$GA"), used, aligned (sizeof (ASAN_GLOBAL))))
STATIC ASAN_GLOBAL  mAsanGlobalsStart = { 0, 0, 0, 0, 0, 0, 0, MAX_UINTN };
__attribute__((section (".ASAN$GZ"), used, aligned (sizeof (ASAN_GLOBAL))))
STATIC ASAN_GLOBAL  mAsanGlobalsEnd = { 0, 0, 0, 0, 0, 0, 0, MAX_UINTN };

STATIC
VOID
EFIAPI
AsanCheck (
  IN UINTN    Address,
  IN UINTN    Size,
  IN BOOLEAN  IsWrite,
  IN VOID     *Pc
  );

STATIC
VOID
EFIAPI
AsanSetShadow (
  IN UINTN  ShadowAddress,
  IN UINTN  Count,
  IN UINT8  Value
  );

STATIC
VOID
EFIAPI
AsanHandleNoReturn (
  IN UINTN  StackAddress
  );

STATIC
BOOLEAN
EFIAPI
AsanRtAddRegion (
  IN UINT32      Type,
  IN UINTN       Base,
  IN UINTN       Size,
  IN VOID        *Shadow,
  IN CONST VOID  *Globals,
  IN CONST VOID  *GlobalsEnd
  );

STATIC
VOID
EFIAPI
AsanRtPoison (
  IN CONST VOID  *Address,
  IN UINTN       Size,
  IN UINT8       Value
  );

STATIC
VOID
EFIAPI
AsanRtUnpoison (
  IN CONST VOID  *Address,
  IN UINTN       Size
  );

STATIC
VOID *
EFIAPI
AsanRtQuarantine (
  IN VOID   *Buffer,
  IN UINTN  Size
  );

STATIC
VOID
EFIAPI
AsanRtAddImage (
  IN CONST CHAR8  *Name,
  IN UINTN        Base
  );

STATIC
BOOLEAN
EFIAPI
AsanRtCheckFree (
  IN CONST VOID  *Buffer
  );

//
// This module's runtime. Stage2 publishes it; payloads use it when Stage2 has none.
//
STATIC CONST ASAN_SERVICE  mAsanService = {
  { ASAN_SERVICE_SIGNATURE, ASAN_SERVICE_VERSION, 0 },
  AsanCheck,
  AsanSetShadow,
  AsanHandleNoReturn,
  AsanRtAddRegion,
  AsanRtPoison,
  AsanRtUnpoison,
  AsanRtQuarantine,
  AsanRtAddImage,
  AsanRtCheckFree
};

//
// Runtime this module uses, set on first use: the Stage2 service or mAsanService.
//
STATIC CONST ASAN_SERVICE  *mAsanRuntime;

STATIC ASAN_REGION  mAsanRegions[ASAN_MAX_REGIONS];
STATIC UINTN        mAsanRegionCount;
STATIC UINTN        mAsanRegionsPrinted;
STATIC ASAN_IMAGE   mAsanImages[ASAN_MAX_IMAGES];
STATIC UINTN        mAsanImageCount;
STATIC BOOLEAN      mAsanReporting;
STATIC VOID         *mAsanQuarantine[ASAN_QUARANTINE_ENTRIES];
STATIC UINTN        mAsanQuarantineNext;

/**
  Find the tracked region that contains an address.

  @param[in] Address    Address to look up.

  @retval    The region, or NULL if the address is not tracked.

**/
STATIC
ASAN_REGION *
AsanFindRegion (
  IN UINTN  Address
  )
{
  UINTN  Index;

  for (Index = 0; Index < mAsanRegionCount; Index++) {
    if ((Address >= mAsanRegions[Index].Base) && (Address < mAsanRegions[Index].End)) {
      return &mAsanRegions[Index];
    }
  }

  return NULL;
}

/**
  Get the shadow byte for an address inside a region.

  @param[in] Region     Region that contains Address.
  @param[in] Address    Address inside the region.

  @retval    Pointer to the shadow byte.

**/
STATIC
UINT8 *
AsanShadowOf (
  IN ASAN_REGION  *Region,
  IN UINTN        Address
  )
{
  return &Region->Shadow[(Address - Region->Base) >> ASAN_SHADOW_SCALE];
}

/**
  Start tracking a region.

  @param[in] Type         ASAN_REGION_TYPE of the region.
  @param[in] Base         Region base address, 8-byte aligned.
  @param[in] Size         Region size in bytes.
  @param[in] Shadow       Shadow buffer of AsanGetShadowSize (Size) bytes.
  @param[in] Globals      First global metadata entry, for AsanRegionGlobals.
  @param[in] GlobalsEnd   End of the global metadata entries, for AsanRegionGlobals.

  @retval TRUE            The region is tracked.

**/
STATIC
BOOLEAN
EFIAPI
AsanRtAddRegion (
  IN UINT32      Type,
  IN UINTN       Base,
  IN UINTN       Size,
  IN VOID        *Shadow,
  IN CONST VOID  *Globals,
  IN CONST VOID  *GlobalsEnd
  )
{
  STATIC CONST CHAR8  *TypeName[] = { "heap", "stack", "globals" };
  ASAN_REGION         *Region;
  CONST ASAN_GLOBAL   *Global;
  UINTN               RedzoneStart;
  UINTN               RedzoneEnd;

  if ((mAsanRegionCount >= ASAN_MAX_REGIONS) || (Shadow == NULL) || (Type > AsanRegionGlobals) ||
      ((Base & ASAN_GRANULE_MASK) != 0) || (Size < ASAN_GRANULE)) {
    return FALSE;
  }

  // Shadow storage can itself lie in a tracked, poisoned heap/pool range.
  // Never send shadow maintenance through the checked public memory APIs.
  AsanInternalSetMem (Shadow, AsanGetShadowSize (Size), (Type == AsanRegionHeap) ? ASAN_SHADOW_HEAP_REDZONE : 0);
  Region             = &mAsanRegions[mAsanRegionCount];
  Region->Base       = Base;
  Region->End        = Base + (Size & ~(UINTN)ASAN_GRANULE_MASK);
  Region->Shadow     = Shadow;
  Region->Type       = Type;
  Region->Globals    = NULL;
  Region->GlobalsEnd = NULL;
  mAsanRegionCount++;

  if (Type == AsanRegionGlobals) {
    // Uninstrumented library globals share this range, so only the metadata redzones are poisoned.
    Region->Globals    = Globals;
    Region->GlobalsEnd = GlobalsEnd;
    for (Global = Region->Globals; (Global != NULL) && (Global < Region->GlobalsEnd); Global++) {
      if ((Global->Beg == 0) || (Global->SizeWithRedzone <= Global->Size) ||
          (Global->Beg < Region->Base) || (Global->Beg + Global->SizeWithRedzone > Region->End)) {
        continue;
      }

      RedzoneStart = Global->Beg + Global->Size;
      RedzoneEnd   = Global->Beg + Global->SizeWithRedzone;
      if ((RedzoneStart & ASAN_GRANULE_MASK) != 0) {
        *AsanShadowOf (Region, RedzoneStart) = (UINT8)(RedzoneStart & ASAN_GRANULE_MASK);
        RedzoneStart = ALIGN_VALUE (RedzoneStart, ASAN_GRANULE);
      }

      if (RedzoneStart < RedzoneEnd) {
        AsanInternalSetMem (AsanShadowOf (Region, RedzoneStart), (RedzoneEnd - RedzoneStart) >> ASAN_SHADOW_SCALE, ASAN_SHADOW_GLOBAL_REDZONE);
      }
    }
  }

  // A payload adds its heap before its DEBUG works, so print once a later region is added.
  if (Type != AsanRegionHeap) {
    for ( ; mAsanRegionsPrinted < mAsanRegionCount; mAsanRegionsPrinted++) {
      Region = &mAsanRegions[mAsanRegionsPrinted];
      DEBUG ((DEBUG_INFO, "ASAN: tracking %a 0x%p - 0x%p, shadow at 0x%p\n", TypeName[Region->Type],
              (VOID *)Region->Base, (VOID *)Region->End, Region->Shadow));
    }
  }

  return TRUE;
}

/**
  Name the image that code addresses in reports belong to.

  @param[in] Name         Image name.
  @param[in] Base         Image base address.

**/
STATIC
VOID
EFIAPI
AsanRtAddImage (
  IN CONST CHAR8  *Name,
  IN UINTN        Base
  )
{
  if (mAsanImageCount < ASAN_MAX_IMAGES) {
    mAsanImages[mAsanImageCount].Name = Name;
    mAsanImages[mAsanImageCount].Base = Base;
    mAsanImageCount++;
  }
}

/**
  Select the runtime on first use and connect this module's callbacks to it.

  @retval    The runtime this module uses.

**/
STATIC
CONST ASAN_SERVICE *
AsanGetRuntime (
  VOID
  )
{
  if (mAsanRuntime == NULL) {
    mAsanRuntime = &mAsanService;
  }

  return mAsanRuntime;
}

/**
  Connect this module's callbacks and name its image in the selected runtime.

**/
STATIC
VOID
AsanConnect (
  VOID
  )
{
  STATIC BOOLEAN  Connected;

  if (!Connected) {
    Connected = TRUE;
    AsanGetRuntime ()->AddImage (gEfiCallerBaseName, (UINTN)&__ImageBase);
    AsanStubConnect (AsanGetRuntime ());
  }
}

/**
  Name the error for a poisoned shadow byte.

  @param[in] Region     Region that contains Address.
  @param[in] Address    Start of the granule that holds the bad byte.
  @param[in] Shadow     Shadow value of that granule.

  @retval    Error description.

**/
STATIC
CONST CHAR8 *
AsanErrorKind (
  IN ASAN_REGION  *Region,
  IN UINTN        Address,
  IN UINT8        Shadow
  )
{
  // A partially addressable granule is named after the poison that follows it.
  if ((Shadow < ASAN_GRANULE) && (Address + ASAN_GRANULE < Region->End)) {
    Shadow = *AsanShadowOf (Region, Address + ASAN_GRANULE);
  }

  switch (Shadow) {
    case ASAN_SHADOW_HEAP_REDZONE:
      return "heap-buffer-overflow";
    case ASAN_SHADOW_HEAP_FREED:
      return "heap-use-after-free";
    case ASAN_SHADOW_STACK_LEFT:
    case ASAN_SHADOW_STACK_MID:
    case ASAN_SHADOW_STACK_RIGHT:
      return "stack-buffer-overflow";
    case ASAN_SHADOW_STACK_AFTER_RETURN:
      return "stack-use-after-return";
    case ASAN_SHADOW_STACK_AFTER_SCOPE:
      return "stack-use-after-scope";
    case ASAN_SHADOW_GLOBAL_REDZONE:
      return "global-buffer-overflow";
    default:
      return "unknown-crash";
  }
}

/**
  Print a code address with its offset in the image that contains it.

  @param[in] Prefix     Text printed before the address.
  @param[in] Pc         Code address.

**/
STATIC
VOID
AsanPrintPc (
  IN CONST CHAR8  *Prefix,
  IN UINTN        Pc
  )
{
  ASAN_IMAGE  *Image;
  UINTN       Index;

  // Images have no recorded size, so pick the closest image base below Pc.
  Image = NULL;
  for (Index = 0; Index < mAsanImageCount; Index++) {
    if ((mAsanImages[Index].Base <= Pc) && ((Image == NULL) || (mAsanImages[Index].Base > Image->Base))) {
      Image = &mAsanImages[Index];
    }
  }

  // AsanSymbolize.py turns "Name+0x..." into file:line using Name.dll and its PDB.
  if (Image == NULL) {
    DEBUG ((DEBUG_ERROR, "==ASAN== %a0x%p\n", Prefix, (VOID *)Pc));
  } else {
    DEBUG ((DEBUG_ERROR, "==ASAN== %a0x%p (%a+0x%lx)\n", Prefix, (VOID *)Pc, Image->Name, (UINT64)(Pc - Image->Base)));
  }
}

/**
  Print the call stack by following frame pointers on the tracked stack.

  @param[in] Pc         Return address into the instrumented code that failed.

**/
STATIC
VOID
AsanPrintBacktrace (
  IN UINTN  Pc
  )
{
  ASAN_REGION  *Stack;
  UINTN        *Frame;
  UINTN        Depth;
  BOOLEAN      Found;
  CHAR8        Prefix[16];

  AsanPrintPc ("  #0 ", Pc);

  Frame = (UINTN *)__builtin_frame_address (0);
  Stack = AsanFindRegion ((UINTN)Frame);
  if ((Stack == NULL) || (Stack->Type != AsanRegionStack)) {
    return;
  }

  // Skip runtime frames until the one returning to Pc, then print its callers.
  Found = FALSE;
  Depth = 1;
  while (Depth < ASAN_MAX_BACKTRACE) {
    if (((UINTN)Frame < Stack->Base) || ((UINTN)(Frame + 2) > Stack->End) || (((UINTN)Frame & (sizeof (UINTN) - 1)) != 0)) {
      break;
    }

    // The outermost frame on the payload stack returns to 0.
    if (Frame[1] == 0) {
      break;
    }

    if (Found) {
      AsciiSPrint (Prefix, sizeof (Prefix), "  #%d ", (INT32)Depth++);
      AsanPrintPc (Prefix, Frame[1]);
    } else if (Frame[1] == Pc) {
      Found = TRUE;
    }

    if (Frame[0] <= (UINTN)Frame) {
      break;
    }

    Frame = (UINTN *)Frame[0];
  }
}

/**
  Find the addressable bytes nearest to a bad address.

  @param[in]  Region    Region that contains Address.
  @param[in]  Address   First bad byte.
  @param[out] Start     Start of the nearest addressable range.
  @param[out] End       End of the nearest addressable range.

  @retval TRUE          An addressable range was found nearby.

**/
STATIC
BOOLEAN
AsanFindNearbyChunk (
  IN  ASAN_REGION  *Region,
  IN  UINTN        Address,
  OUT UINTN        *Start,
  OUT UINTN        *End
  )
{
  UINTN  Left;
  UINTN  Right;
  UINTN  Index;
  UINT8  Shadow;

  Left  = 0;
  Right = 0;
  for (Index = 0; Index <= ASAN_MAX_SEARCH_GRANULES; Index++) {
    if ((Left == 0) && (Address >= Region->Base + Index * ASAN_GRANULE)) {
      Shadow = *AsanShadowOf (Region, (Address & ~(UINTN)ASAN_GRANULE_MASK) - Index * ASAN_GRANULE);
      if (Shadow < ASAN_GRANULE) {
        Left = (Address & ~(UINTN)ASAN_GRANULE_MASK) - Index * ASAN_GRANULE;
      }
    }

    if ((Right == 0) && (Index > 0) && ((Address & ~(UINTN)ASAN_GRANULE_MASK) + Index * ASAN_GRANULE < Region->End)) {
      if (*AsanShadowOf (Region, (Address & ~(UINTN)ASAN_GRANULE_MASK) + Index * ASAN_GRANULE) < ASAN_GRANULE) {
        Right = (Address & ~(UINTN)ASAN_GRANULE_MASK) + Index * ASAN_GRANULE;
      }
    }

    if ((Left != 0) || (Right != 0)) {
      break;
    }
  }

  if (Left != 0) {
    // Range ending at Left: extend it backwards over fully addressable granules.
    Shadow = *AsanShadowOf (Region, Left);
    *End   = Left + ((Shadow == 0) ? ASAN_GRANULE : Shadow);
    while ((Left > Region->Base) && (*AsanShadowOf (Region, Left - ASAN_GRANULE) == 0)) {
      Left -= ASAN_GRANULE;
    }

    *Start = Left;
    return TRUE;
  }

  if (Right != 0) {
    *Start = Right;
    while ((Right + ASAN_GRANULE < Region->End) && (*AsanShadowOf (Region, Right) == 0)) {
      Right += ASAN_GRANULE;
    }

    Shadow = *AsanShadowOf (Region, Right);
    *End   = Right + ((Shadow == 0) ? ASAN_GRANULE : ((Shadow >= ASAN_GRANULE) ? 0 : Shadow));
    return TRUE;
  }

  return FALSE;
}

/**
  Describe a bad address relative to the nearest addressable range.

  @param[in] Region     Region that contains Address.
  @param[in] Address    First bad byte.
  @param[in] What       Description of the range, such as "heap".

**/
STATIC
VOID
AsanDescribeChunk (
  IN ASAN_REGION  *Region,
  IN UINTN        Address,
  IN CONST CHAR8  *What
  )
{
  UINTN  Start;
  UINTN  End;

  if (!AsanFindNearbyChunk (Region, Address, &Start, &End)) {
    return;
  }

  if (Address >= End) {
    DEBUG ((DEBUG_ERROR, "==ASAN== 0x%p is %ld bytes after the %ld-byte %a range [0x%p, 0x%p)\n",
            (VOID *)Address, (UINT64)(Address - End), (UINT64)(End - Start), What, (VOID *)Start, (VOID *)End));
  } else if (Address < Start) {
    DEBUG ((DEBUG_ERROR, "==ASAN== 0x%p is %ld bytes before the %ld-byte %a range [0x%p, 0x%p)\n",
            (VOID *)Address, (UINT64)(Start - Address), (UINT64)(End - Start), What, (VOID *)Start, (VOID *)End));
  }
}

/**
  Describe a bad heap address.

  @param[in] Region     Heap region that contains Address.
  @param[in] Address    First bad byte.

**/
STATIC
VOID
AsanDescribeHeap (
  IN ASAN_REGION  *Region,
  IN UINTN        Address
  )
{
  UINTN  Start;
  UINTN  End;

  if (*AsanShadowOf (Region, Address) != ASAN_SHADOW_HEAP_FREED) {
    AsanDescribeChunk (Region, Address, "heap");
    return;
  }

  Start = Address & ~(UINTN)ASAN_GRANULE_MASK;
  while ((Start > Region->Base) && (*AsanShadowOf (Region, Start - ASAN_GRANULE) == ASAN_SHADOW_HEAP_FREED)) {
    Start -= ASAN_GRANULE;
  }

  End = Address & ~(UINTN)ASAN_GRANULE_MASK;
  while ((End < Region->End) && (*AsanShadowOf (Region, End) == ASAN_SHADOW_HEAP_FREED)) {
    End += ASAN_GRANULE;
  }

  DEBUG ((DEBUG_ERROR, "==ASAN== 0x%p is %ld bytes inside the freed heap block [0x%p, 0x%p) (block includes the pool header)\n",
          (VOID *)Address, (UINT64)(Address - Start), (VOID *)Start, (VOID *)End));
}

/**
  Describe a bad stack address using the frame description clang stores in each frame.

  @param[in] Region     Stack region that contains Address.
  @param[in] Address    First bad byte.

**/
STATIC
VOID
AsanDescribeStack (
  IN ASAN_REGION  *Region,
  IN UINTN        Address
  )
{
  UINTN        Frame;
  UINTN        Index;
  UINTN        *Header;
  CONST CHAR8  *Descr;
  UINTN        Count;
  UINTN        Offset;
  UINTN        VarOffset;
  UINTN        VarSize;
  UINTN        NameLen;
  UINTN        Var;

  VarOffset = 0;
  VarSize   = 0;
  NameLen   = 0;

  // A frame starts at the first granule of its left redzone.
  Frame = Address & ~(UINTN)ASAN_GRANULE_MASK;
  for (Index = 0; Index < ASAN_MAX_SEARCH_GRANULES; Index++) {
    if ((*AsanShadowOf (Region, Frame) == ASAN_SHADOW_STACK_LEFT) &&
        ((Frame == Region->Base) || (*AsanShadowOf (Region, Frame - ASAN_GRANULE) != ASAN_SHADOW_STACK_LEFT))) {
      break;
    }

    if (Frame == Region->Base) {
      return;
    }

    Frame -= ASAN_GRANULE;
  }

  // Frame header written by clang: magic, description string, function address.
  Header = (UINTN *)Frame;
  if ((Index == ASAN_MAX_SEARCH_GRANULES) || (Header[0] != ASAN_STACK_FRAME_MAGIC)) {
    return;
  }

  Offset = Address - Frame;
  DEBUG ((DEBUG_ERROR, "==ASAN== 0x%p is at offset %ld in the stack frame of:\n", (VOID *)Address, (UINT64)Offset));
  AsanPrintPc ("  ", Header[2]);

  // Description format: "<count> (<offset> <size> <name length> <name[:line]>)*"
  Descr = (CONST CHAR8 *)Header[1];
  Count = AsciiStrDecimalToUintn (Descr);
  DEBUG ((DEBUG_ERROR, "==ASAN== This frame has %ld object(s):\n", (UINT64)Count));
  for (Var = 0; Var < Count; Var++) {
    for (Index = 0; Index < 3; Index++) {
      while ((*Descr != '\0') && (*Descr != ' ')) {
        Descr++;
      }

      if (*Descr == '\0') {
        return;
      }

      Descr++;
      if (Index == 0) {
        VarOffset = AsciiStrDecimalToUintn (Descr);
      } else if (Index == 1) {
        VarSize = AsciiStrDecimalToUintn (Descr);
      } else {
        NameLen = AsciiStrDecimalToUintn (Descr);
      }
    }

    while ((*Descr != '\0') && (*Descr != ' ')) {
      Descr++;
    }

    if (*Descr == '\0') {
      return;
    }

    Descr++;
    DEBUG ((DEBUG_ERROR, "==ASAN==   [%ld, %ld) '%.*a'%a\n",
            (UINT64)VarOffset, (UINT64)(VarOffset + VarSize), NameLen, Descr,
            ((Offset >= VarOffset + VarSize) && (Offset < VarOffset + VarSize + ASAN_GRANULE * 4)) ? " <== access overflows this variable" :
            ((Offset < VarOffset) && (Offset + ASAN_GRANULE * 4 > VarOffset)) ? " <== access underflows this variable" :
            ((Offset >= VarOffset) && (Offset < VarOffset + VarSize)) ? " <== access is inside this variable" : ""));
    Descr += AsciiStrnLenS (Descr, NameLen);
  }
}

/**
  Describe a bad global address using the compiler's global metadata.

  @param[in] Region     Globals region that contains Address.
  @param[in] Address    First bad byte.

**/
STATIC
VOID
AsanDescribeGlobal (
  IN ASAN_REGION  *Region,
  IN UINTN        Address
  )
{
  CONST ASAN_GLOBAL  *Global;

  for (Global = Region->Globals; (Global != NULL) && (Global < Region->GlobalsEnd); Global++) {
    if ((Global->Beg == 0) || (Address < Global->Beg) || (Address >= Global->Beg + Global->SizeWithRedzone)) {
      continue;
    }

    DEBUG ((DEBUG_ERROR, "==ASAN== 0x%p is %ld bytes after global '%a' of size %ld at 0x%p, defined in %a\n",
            (VOID *)Address, (UINT64)(Address - Global->Beg - Global->Size), (CONST CHAR8 *)Global->Name,
            (UINT64)Global->Size, (VOID *)Global->Beg, (CONST CHAR8 *)Global->ModuleName));
    return;
  }
}

/**
  Print the shadow bytes around a bad address, marking the bad granule.

  @param[in] Region     Region that contains Address.
  @param[in] Address    First bad byte.

**/
STATIC
VOID
AsanPrintShadow (
  IN ASAN_REGION  *Region,
  IN UINTN        Address
  )
{
  UINTN  Row;
  UINTN  RowEnd;
  UINTN  Granule;
  UINTN  Bad;
  CHAR8  Line[96];
  UINTN  Len;

  Bad = Address & ~(UINTN)ASAN_GRANULE_MASK;
  Row = (Bad & ~(UINTN)(ASAN_GRANULE * 16 - 1)) - ASAN_GRANULE * 16 * 2;
  if (Row < Region->Base) {
    Row = Region->Base & ~(UINTN)(ASAN_GRANULE * 16 - 1);
  }

  RowEnd = MIN (Row + ASAN_GRANULE * 16 * 5, Region->End);
  DEBUG ((DEBUG_ERROR, "==ASAN== Shadow bytes around the buggy address (one byte per 8 bytes of memory):\n"));
  for ( ; Row < RowEnd; Row += ASAN_GRANULE * 16) {
    Len = AsciiSPrint (Line, sizeof (Line), "  0x%p:", (VOID *)Row);
    for (Granule = Row; Granule < Row + ASAN_GRANULE * 16; Granule += ASAN_GRANULE) {
      if ((Granule < Region->Base) || (Granule >= Region->End)) {
        Len += AsciiSPrint (Line + Len, sizeof (Line) - Len, "   ");
      } else {
        Len += AsciiSPrint (Line + Len, sizeof (Line) - Len, (Granule == Bad) ? "[%02x]" : " %02x", *AsanShadowOf (Region, Granule));
      }
    }

    DEBUG ((DEBUG_ERROR, "%a\n", Line));
  }

  DEBUG ((DEBUG_ERROR, "==ASAN== Legend: 00 addressable, 01-07 partially addressable, fa heap redzone, fd freed heap,\n"));
  DEBUG ((DEBUG_ERROR, "==ASAN==   f1/f2/f3 stack left/mid/right redzone, f5 stack after return, f8 stack after scope, f9 global redzone\n"));
}

/**
  Report an invalid access and halt.

  @param[in] Kind       Error description.
  @param[in] Region     Region that contains BadAddress, or NULL.
  @param[in] BadAddress First byte that is not addressable.
  @param[in] Address    Faulting access address.
  @param[in] Size       Faulting access size.
  @param[in] IsWrite    TRUE for a write access.
  @param[in] Pc         Return address into the instrumented code.

**/
STATIC
VOID
AsanReport (
  IN CONST CHAR8  *Kind,
  IN ASAN_REGION  *Region,
  IN UINTN        BadAddress,
  IN UINTN        Address,
  IN UINTN        Size,
  IN BOOLEAN      IsWrite,
  IN VOID         *Pc
  )
{
  UINTN  Index;

  if (mAsanReporting) {
    // An AsanReport in the middle of another AsanReport probably means
    // an AP detected an error while the BSP kept going. Bring the whole
    // system down.
    DEBUG ((DEBUG_ERROR, "==ASAN== Recursive error detected. Halting.\n"));
    CpuDeadLoop ();
    return;
  }

  mAsanReporting = TRUE;
  DEBUG ((DEBUG_ERROR, "\n=================================================================\n"));
  DEBUG ((DEBUG_ERROR, "==ASAN== ERROR: %a on address 0x%p\n", Kind, (VOID *)Address));
  DEBUG ((DEBUG_ERROR, "==ASAN== %a of size %ld at 0x%p\n", IsWrite ? "WRITE" : "READ", (UINT64)Size, (VOID *)Address));
  for (Index = 0; Index < mAsanImageCount; Index++) {
    DEBUG ((DEBUG_ERROR, "==ASAN== image %a at 0x%p\n", mAsanImages[Index].Name, (VOID *)mAsanImages[Index].Base));
  }

  AsanPrintBacktrace ((UINTN)Pc);

  if (Region != NULL) {
    switch (Region->Type) {
      case AsanRegionStack:
        AsanDescribeStack (Region, BadAddress);
        break;
      case AsanRegionGlobals:
        AsanDescribeGlobal (Region, BadAddress);
        break;
      default:
        AsanDescribeHeap (Region, BadAddress);
        break;
    }

    AsanPrintShadow (Region, BadAddress);
  }

  DEBUG ((DEBUG_ERROR, "==ASAN== Halting. Run AsanSymbolize.py on this log to get file:line for Name+0x... addresses.\n"));
  DEBUG ((DEBUG_ERROR, "=================================================================\n"));
  CpuDeadLoop ();
  mAsanReporting = FALSE;
}

/**
  Check that every byte of an access inside the tracked region is addressable.

  @param[in] Address    Access address.
  @param[in] Size       Access size.
  @param[in] IsWrite    TRUE for a write access.
  @param[in] Pc         Return address into the instrumented code.

**/
STATIC
VOID
EFIAPI
AsanCheck (
  IN UINTN    Address,
  IN UINTN    Size,
  IN BOOLEAN  IsWrite,
  IN VOID     *Pc
  )
{
  UINTN        Index;
  ASAN_REGION  *Region;
  UINTN        End;
  UINTN        Start;
  UINTN        Limit;
  UINTN        Next;
  UINT8        Shadow;

  if (Size == 0) {
    return;
  }

  End = Address + Size;
  if (End < Address) {
    End = MAX_UINTN;
  }

  for (Index = 0; Index < mAsanRegionCount; Index++) {
    Region = &mAsanRegions[Index];
    Start  = MAX (Address, Region->Base);
    Limit  = MIN (End, Region->End);
    for ( ; Start < Limit; Start = Next) {
      Next   = (Start | ASAN_GRANULE_MASK) + 1;
      Shadow = *AsanShadowOf (Region, Start);
      if (Shadow == 0) {
        continue;
      }

      // A shadow value k below 8 means only the first k bytes of the granule are addressable.
      if ((Shadow >= ASAN_GRANULE) || (((MIN (Next, Limit) - 1) & ASAN_GRANULE_MASK) >= Shadow)) {
        AsanReport (
          AsanErrorKind (Region, Start & ~(UINTN)ASAN_GRANULE_MASK, Shadow),
          Region,
          (Shadow >= ASAN_GRANULE) ? Start : MAX (Start, (Start & ~(UINTN)ASAN_GRANULE_MASK) + Shadow),
          Address,
          Size,
          IsWrite,
          Pc
          );
        return;
      }
    }
  }
}

/**
  Get the shadow memory size needed to track a region.

  @param[in] RegionSize   Size in bytes of the region to track.

  @retval    Shadow size in bytes.

**/
UINTN
EFIAPI
AsanGetShadowSize (
  IN UINTN  RegionSize
  )
{
  return (RegionSize + ASAN_GRANULE_MASK) >> ASAN_SHADOW_SCALE;
}

/**
  Start tracking the heap. The whole region is initially poisoned.

  @param[in] RegionBase   Region base address, 8-byte aligned.
  @param[in] RegionSize   Region size in bytes.
  @param[in] Shadow       Shadow buffer of AsanGetShadowSize (RegionSize) bytes.

**/
VOID
EFIAPI
AsanInitialize (
  IN UINTN  RegionBase,
  IN UINTN  RegionSize,
  IN VOID   *Shadow
  )
{
  AsanConnect ();
  AsanGetRuntime ()->AddRegion (AsanRegionHeap, RegionBase, RegionSize, Shadow, NULL, NULL);
}

/**
  Start tracking a stack so instrumented stack frames get redzones.

  @param[in] StackBase    Lowest stack address, 8-byte aligned.
  @param[in] StackSize    Stack size in bytes.
  @param[in] Shadow       Shadow buffer of AsanGetShadowSize (StackSize) bytes.

**/
VOID
EFIAPI
AsanTrackStack (
  IN UINTN  StackBase,
  IN UINTN  StackSize,
  IN VOID   *Shadow
  )
{
  AsanConnect ();
  AsanGetRuntime ()->AddRegion (AsanRegionStack, StackBase, StackSize, Shadow, NULL, NULL);
}

/**
  Get the address range spanned by the instrumented globals.

  @param[out] Base        Range base address, 8-byte aligned.

  @retval    Range size in bytes, or 0 if there are no instrumented globals.

**/
UINTN
EFIAPI
AsanGetGlobalsRange (
  OUT UINTN  *Base
  )
{
  ASAN_GLOBAL  *Global;
  UINTN        Start;
  UINTN        End;

  Start = MAX_UINTN;
  End   = 0;
  for (Global = &mAsanGlobalsStart + 1; (UINTN)Global < (UINTN)&mAsanGlobalsEnd; Global++) {
    // The linker may pad between metadata sections with zeros.
    if ((Global->Beg == 0) || (Global->SizeWithRedzone == 0)) {
      continue;
    }

    Start = MIN (Start, Global->Beg);
    End   = MAX (End, Global->Beg + Global->SizeWithRedzone);
  }

  if (Start >= End) {
    *Base = 0;
    return 0;
  }

  *Base = Start & ~(UINTN)ASAN_GRANULE_MASK;
  return ALIGN_VALUE (End, ASAN_GRANULE) - *Base;
}

/**
  Start tracking the instrumented globals and poison their redzones.

  @param[in] Base         Base returned by AsanGetGlobalsRange ().
  @param[in] Size         Size returned by AsanGetGlobalsRange ().
  @param[in] Shadow       Shadow buffer of AsanGetShadowSize (Size) bytes.

**/
VOID
EFIAPI
AsanTrackGlobals (
  IN UINTN  Base,
  IN UINTN  Size,
  IN VOID   *Shadow
  )
{
  AsanConnect ();
  AsanGetRuntime ()->AddRegion (AsanRegionGlobals, Base, Size, Shadow, &mAsanGlobalsStart + 1, &mAsanGlobalsEnd);
}

/**
  Mark the 8-byte granules fully inside a range as not addressable.

  @param[in] Address      Range start.
  @param[in] Size         Range size in bytes.
  @param[in] Value        Shadow value, ASAN_SHADOW_HEAP_REDZONE or ASAN_SHADOW_HEAP_FREED.

**/
STATIC
VOID
EFIAPI
AsanRtPoison (
  IN CONST VOID  *Address,
  IN UINTN       Size,
  IN UINT8       Value
  )
{
  ASAN_REGION  *Region;
  UINTN        Start;
  UINTN        End;

  Region = AsanFindRegion ((UINTN)Address);
  if (Region == NULL) {
    return;
  }

  Start = ALIGN_VALUE ((UINTN)Address, ASAN_GRANULE);
  End   = MIN (((UINTN)Address + Size) & ~(UINTN)ASAN_GRANULE_MASK, Region->End);
  if (Start < End) {
    AsanInternalSetMem (AsanShadowOf (Region, Start), (End - Start) >> ASAN_SHADOW_SCALE, Value);
  }
}

/**
  Mark a range as addressable.

  @param[in] Address      Range start, 8-byte aligned.
  @param[in] Size         Range size in bytes.

**/
STATIC
VOID
EFIAPI
AsanRtUnpoison (
  IN CONST VOID  *Address,
  IN UINTN       Size
  )
{
  ASAN_REGION  *Region;
  UINTN        Start;
  UINTN        End;

  Region = AsanFindRegion ((UINTN)Address);
  if (Region == NULL) {
    return;
  }

  Start = (UINTN)Address & ~(UINTN)ASAN_GRANULE_MASK;
  End   = MIN ((UINTN)Address + Size, Region->End);
  if (Start >= End) {
    return;
  }

  AsanInternalSetMem (AsanShadowOf (Region, Start), (End - Start) >> ASAN_SHADOW_SCALE, 0);
  if ((End & ASAN_GRANULE_MASK) != 0) {
    *AsanShadowOf (Region, End) = (UINT8)(End & ASAN_GRANULE_MASK);
  }
}

/**
  Check for a double free without reading allocator metadata or quarantining.

  @param[in] Buffer       Pool buffer being freed.

  @retval TRUE            Buffer is not known to be freed.
  @retval FALSE           Buffer is freed; a double-free report has been issued.

**/
STATIC
BOOLEAN
EFIAPI
AsanRtCheckFree (
  IN CONST VOID  *Buffer
  )
{
  ASAN_REGION  *Region;
  UINTN        Address;

  Address = (UINTN)Buffer;
  Region  = AsanFindRegion (Address);
  if ((Region != NULL) && (Region->Type == AsanRegionHeap) &&
      (*AsanShadowOf (Region, Address) == ASAN_SHADOW_HEAP_FREED)) {
    // The block size is not safe to read yet; check only the buffer's first byte.
    AsanReport ("double-free", Region, Address, Address, 1, TRUE, RETURN_ADDRESS (0));
    return FALSE;
  }

  return TRUE;
}

/**
  Defer the release of a freed pool buffer so later accesses are still caught.

  Reports a double free if Buffer is already in the freed state.

  @param[in] Buffer       Pool buffer being freed.
  @param[in] Size         Size of the pool block that holds Buffer.

  @retval    The buffer the caller must release now: Buffer itself, an older
             buffer evicted from quarantine, or NULL if nothing is to be released.

**/
STATIC
VOID *
EFIAPI
AsanRtQuarantine (
  IN VOID   *Buffer,
  IN UINTN  Size
  )
{
  UINTN  Address;
  VOID   *Evicted;
  ASAN_REGION  *Region;

  Address = (UINTN)Buffer;
  Region  = AsanFindRegion (Address);
  if ((Region == NULL) || (Region->Type != AsanRegionHeap)) {
    return Buffer;
  }

  if (*AsanShadowOf (Region, Address) == ASAN_SHADOW_HEAP_FREED) {
    AsanReport ("double-free", Region, Address, Address, Size, TRUE, RETURN_ADDRESS (0));
    return NULL;
  }

  if (Size > ASAN_QUARANTINE_MAX_BLOCK) {
    return Buffer;
  }

  Evicted                              = mAsanQuarantine[mAsanQuarantineNext];
  mAsanQuarantine[mAsanQuarantineNext] = Buffer;
  mAsanQuarantineNext                  = (mAsanQuarantineNext + 1) % ASAN_QUARANTINE_ENTRIES;
  return Evicted;
}

/**
  Clear the shadow of the stack that contains StackAddress before a call that never returns.

  Frames skipped by a no-return call never unpoison themselves.

  @param[in] StackAddress   An address on the current stack.

**/
STATIC
VOID
EFIAPI
AsanHandleNoReturn (
  IN UINTN  StackAddress
  )
{
  ASAN_REGION  *Region;

  Region = AsanFindRegion (StackAddress);
  if ((Region != NULL) && (Region->Type == AsanRegionStack)) {
    AsanInternalSetMem (Region->Shadow, AsanGetShadowSize (Region->End - Region->Base), 0);
  }
}

/**
  Set shadow bytes for a stack frame.

  @param[in] ShadowAddress  Address >> 3 of the first granule, as the dynamic shadow base is 0.
  @param[in] Count          Number of shadow bytes to set.
  @param[in] Value          Shadow value.

**/
STATIC
VOID
EFIAPI
AsanSetShadow (
  IN UINTN  ShadowAddress,
  IN UINTN  Count,
  IN UINT8  Value
  )
{
  ASAN_REGION  *Region;
  UINTN        Address;

  Address = ShadowAddress << ASAN_SHADOW_SCALE;
  Region  = AsanFindRegion (Address);
  if (Region == NULL) {
    return;
  }

  AsanInternalSetMem (AsanShadowOf (Region, Address), MIN (Count, (Region->End - Address) >> ASAN_SHADOW_SCALE), Value);
}

/**
  Use another module's runtime, normally the one Stage2 publishes, instead of this one.

  Call it before any other AsanLib function.

  @param[in] Service      ASAN_SERVICE found in the service list.

  @retval TRUE            The service is used.
  @retval FALSE           The service is not compatible, this module uses its own runtime.

**/
BOOLEAN
EFIAPI
AsanUseService (
  IN CONST VOID  *Service
  )
{
  CONST ASAN_SERVICE  *AsanService;

  AsanService = (CONST ASAN_SERVICE *)Service;
  if ((mAsanRuntime != NULL) || (AsanService == NULL) ||
      (AsanService->Header.Signature != ASAN_SERVICE_SIGNATURE) ||
      (AsanService->Header.Version != ASAN_SERVICE_VERSION)) {
    return FALSE;
  }

  mAsanRuntime = AsanService;
  return TRUE;
}

/**
  Get this module's runtime so it can be published with RegisterService ().

  @retval    This module's ASAN_SERVICE.

**/
VOID *
EFIAPI
AsanGetService (
  VOID
  )
{
  return (VOID *)&mAsanService;
}

/**
  Mark the 8-byte granules fully inside a range as not addressable.

  @param[in] Address      Range start.
  @param[in] Size         Range size in bytes.
  @param[in] Value        Shadow value, ASAN_SHADOW_HEAP_REDZONE or ASAN_SHADOW_HEAP_FREED.

**/
VOID
EFIAPI
AsanPoison (
  IN CONST VOID  *Address,
  IN UINTN       Size,
  IN UINT8       Value
  )
{
  if (mAsanRuntime != NULL) {
    mAsanRuntime->Poison (Address, Size, Value);
  }
}

/**
  Mark a range as addressable.

  @param[in] Address      Range start, 8-byte aligned.
  @param[in] Size         Range size in bytes.

**/
VOID
EFIAPI
AsanUnpoison (
  IN CONST VOID  *Address,
  IN UINTN       Size
  )
{
  if (mAsanRuntime != NULL) {
    mAsanRuntime->Unpoison (Address, Size);
  }
}

/**
  Check freed heap shadow before allocator metadata is read.

  @param[in] Buffer       Pool buffer being freed.

  @retval TRUE            Buffer is not known to be freed, or tracking is inactive.
  @retval FALSE           Buffer is freed; a double-free report has been issued.

**/
BOOLEAN
EFIAPI
AsanCheckFree (
  IN CONST VOID  *Buffer
  )
{
  if (mAsanRuntime == NULL) {
    return TRUE;
  }

  return mAsanRuntime->CheckFree (Buffer);
}

/**
  Defer the release of a freed pool buffer so later accesses are still caught.

  @param[in] Buffer       Pool buffer being freed.
  @param[in] Size         Size of the pool block that holds Buffer.

  @retval    The buffer the caller must release now: Buffer itself, an older
             buffer evicted from quarantine, or NULL if nothing is to be released.

**/
VOID *
EFIAPI
AsanQuarantine (
  IN VOID   *Buffer,
  IN UINTN  Size
  )
{
  if (mAsanRuntime == NULL) {
    return Buffer;
  }

  return mAsanRuntime->Quarantine (Buffer, Size);
}
