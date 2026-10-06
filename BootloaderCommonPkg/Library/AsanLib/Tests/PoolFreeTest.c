/** @file
  Native regression tests using the real pool allocator and ASAN runtime.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>

/* Normally declared by AutoGen.h for a firmware module. */
extern CHAR8 *gEfiCallerBaseName;
VOID __asan_load1 (UINTN Address);

#include <Library/AsanLib.h>
/* Negative control: emulate the old free path without its early shadow check. */
#ifdef ASAN_TEST_BEFORE_FIX
#define AsanCheckFree(Buffer) TRUE
#endif
#include "../../FullMemoryAllocationLib/Pool.c"
#ifdef ASAN_TEST_BEFORE_FIX
#undef AsanCheckFree
#endif
#ifdef ASAN_TEST_NULL
#include "../../AsanLibNull/AsanLibNull.c"
#else
#include "../AsanLib.c"
#endif

#define CHECK(Expression) do { \
  if (!(Expression)) { \
    fprintf (stderr, "%s:%d: %s\n", __FILE__, __LINE__, #Expression); \
    exit (1); \
  } \
} while (0)

static _Alignas (4096) UINT8  Heap[128 * 1024];
static UINT8                Shadow[sizeof (Heap) / 8];
static UINTN                NextPage;
static UINTN                Reports;
static UINTN                Halts;
static UINTN                Asserts;
static BOOLEAN              ExpectAssert;
static jmp_buf              AssertJump;

/* RELEASE removes DEBUG output; CpuDeadLoop still proves a report was issued. */
static UINTN
ReportCount (VOID)
{
#ifdef MDEPKG_NDEBUG
  return Halts;
#else
  return Reports;
#endif
}

EFI_LOCK  gMemoryLock;
CHAR8     *gEfiCallerBaseName = "PoolFreeTest";
#ifndef _WIN32
UINT8     __ImageBase;
#endif

/* Host support only: keep the production headers, CR and ASSERT macros. */
BOOLEAN EFIAPI DebugAssertEnabled (VOID) { return TRUE; }
BOOLEAN EFIAPI DebugPrintEnabled (VOID) { return TRUE; }
BOOLEAN EFIAPI DebugPrintLevelEnabled (CONST UINTN Level) { return TRUE; }
BOOLEAN EFIAPI DebugClearMemoryEnabled (VOID) { return TRUE; }

VOID EFIAPI
DebugAssert (CONST CHAR8 *File, UINTN Line, CONST CHAR8 *Description)
{
  Asserts++;
  if (ExpectAssert) {
    longjmp (AssertJump, 1);
  }

  fprintf (stderr, "Unexpected ASSERT %s:%zu: %s\n", File, (size_t)Line, Description);
  exit (1);
}

VOID EFIAPI
DebugPrint (UINTN Level, CONST CHAR8 *Format, ...)
{
  va_list Args;

  if (strstr (Format, "==ASAN== ERROR:") != NULL) {
    va_start (Args, Format);
#ifndef ASAN_MEMORY_TEST
    CHECK (strcmp (va_arg (Args, CONST CHAR8 *), "double-free") == 0);
#endif
    va_end (Args);
    Reports++;
  }
}

/* Deliberately return: the allocator must still not touch invalid metadata. */
#ifndef ASAN_MEMORY_TEST
VOID EFIAPI CpuDeadLoop (VOID) { Halts++; }
VOID *EFIAPI AsanInternalSetMem (VOID *Buffer, UINTN Size, UINT8 Value) { return memset (Buffer, Value, Size); }
VOID *EFIAPI AsanInternalCopyMem (VOID *To, CONST VOID *From, UINTN Size) { return memmove (To, From, Size); }
#endif
VOID *EFIAPI ZeroMem (VOID *Buffer, UINTN Size) { return memset (Buffer, 0, Size); }
VOID *EFIAPI DebugClearMemory (VOID *Buffer, UINTN Size) { return memset (Buffer, 0xAF, Size); }
UINTN EFIAPI AsciiStrDecimalToUintn (CONST CHAR8 *Text) { return (UINTN)strtoull (Text, NULL, 10); }
UINTN EFIAPI
AsciiStrnLenS (CONST CHAR8 *Text, UINTN Max)
{
  UINTN Len;
  for (Len = 0; (Len < Max) && (Text[Len] != '\0'); Len++) {}
  return Len;
}

UINTN EFIAPI
AsciiSPrint (CHAR8 *Buffer, UINTN Size, CONST CHAR8 *Format, ...)
{
  int     Len;
  va_list Args;

  va_start (Args, Format);
  Len = vsnprintf (Buffer, Size, Format, Args);
  va_end (Args);
  CHECK ((Len >= 0) && ((UINTN)Len < Size));
  return (UINTN)Len;
}

LIST_ENTRY *EFIAPI
InitializeListHead (LIST_ENTRY *List)
{
  List->ForwardLink = List->BackLink = List;
  return List;
}

BOOLEAN EFIAPI IsListEmpty (CONST LIST_ENTRY *List) { return List->ForwardLink == List; }
LIST_ENTRY *EFIAPI
InsertHeadList (LIST_ENTRY *List, LIST_ENTRY *Entry)
{
  Entry->ForwardLink = List->ForwardLink;
  Entry->BackLink = List;
  List->ForwardLink->BackLink = Entry;
  List->ForwardLink = Entry;
  return List;
}

LIST_ENTRY *EFIAPI
RemoveEntryList (CONST LIST_ENTRY *Entry)
{
  Entry->BackLink->ForwardLink = Entry->ForwardLink;
  Entry->ForwardLink->BackLink = Entry->BackLink;
  return Entry->ForwardLink;
}

VOID CoreAcquireMemoryLock (VOID) {}
VOID CoreReleaseMemoryLock (VOID) {}
EFI_STATUS CoreAcquireLockOrFail (EFI_LOCK *Lock) { return EFI_SUCCESS; }
VOID *
CoreAllocatePoolPages (EFI_MEMORY_TYPE Type, UINTN Pages, UINTN Alignment)
{
  VOID *Result;
  CHECK (NextPage + Pages * EFI_PAGE_SIZE <= sizeof (Heap));
  Result = Heap + NextPage;
  NextPage += Pages * EFI_PAGE_SIZE;
  return Result;
}

VOID CoreFreePoolPages (EFI_PHYSICAL_ADDRESS Address, UINTN Pages) {}

static VOID
Reset (BOOLEAN Track)
{
  memset (Heap, 0, sizeof (Heap));
  memset (Shadow, 0, sizeof (Shadow));
  NextPage = Reports = Halts = Asserts = 0;
  ExpectAssert = FALSE;
  CoreInitializePool ();
#ifndef ASAN_TEST_NULL
  mAsanRuntime = NULL;
  mAsanRegionCount = mAsanRegionsPrinted = 0;
  mAsanImageCount = mAsanQuarantineNext = 0;
  mAsanReporting = FALSE;
  memset (mAsanQuarantine, 0, sizeof (mAsanQuarantine));
#endif
  if (Track) {
    AsanInitialize ((UINTN)Heap, sizeof (Heap), Shadow);
  }
}

static VOID *
Allocate (VOID)
{
  VOID *Buffer;
  Buffer = CoreAllocatePoolI (EfiBootServicesData, 32);
  CHECK (Buffer != NULL);
  return Buffer;
}

static VOID
InvalidMetadata (VOID *Buffer)
{
  EFI_STATUS Status;
#ifndef ASAN_TEST_NULL
  VOID  *Before[ASAN_QUARANTINE_ENTRIES];
  UINTN Next;
  memcpy (Before, mAsanQuarantine, sizeof (Before));
  Next = mAsanQuarantineNext;
#endif
  ExpectAssert = TRUE;
  if (setjmp (AssertJump) == 0) {
    Status = CoreFreePoolI (Buffer, NULL);
    CHECK (Status == EFI_INVALID_PARAMETER);
  }
  ExpectAssert = FALSE;
  CHECK ((ReportCount () == 0) && (Halts == 0));
#ifndef ASAN_TEST_NULL
  CHECK (Next == mAsanQuarantineNext);
  CHECK (memcmp (Before, mAsanQuarantine, sizeof (Before)) == 0);
  CHECK (Shadow[((UINTN)Buffer - (UINTN)Heap) / 8] == 0);
#endif
}

static VOID
TestMetadataValidation (VOID)
{
  VOID      *Buffer;
  POOL_HEAD *Head;
  POOL_TAIL *Tail;
  UINTN     Size;

  Reset (TRUE);
  Buffer = Allocate ();
  Head = BASE_CR (Buffer, POOL_HEAD, Data);
  Tail = HEAD_TO_TAIL (Head);
  Size = Head->Size;
  Head->Signature = POOL_FREE_SIGNATURE;
  InvalidMetadata (Buffer);
  Head->Signature = POOL_HEAD_SIGNATURE;
  Tail->Signature = 0;
  InvalidMetadata (Buffer);
  Tail->Signature = POOL_TAIL_SIGNATURE;
  Tail->Size++;
  InvalidMetadata (Buffer);
  Tail->Size = Size;
  CHECK (CoreFreePoolI (Buffer, NULL) == EFI_SUCCESS);
#ifndef ASAN_TEST_NULL
  CHECK (mAsanQuarantine[0] == Buffer);
  CHECK (mAsanQuarantineNext == 1);
  CHECK (Shadow[((UINTN)Buffer - (UINTN)Heap) / 8] == ASAN_SHADOW_HEAP_FREED);
#endif
  puts ("PASS header/tail validation precedes quarantine and poisoning");
}

#ifndef ASAN_TEST_NULL
static VOID
TestShadowOnlyScope (VOID)
{
  _Alignas (8) UINT8 Stack[64];
  UINT8             StackShadow[sizeof (Stack) / 8];
  VOID              *Buffer;

  Reset (TRUE);
  CHECK (CoreInternalFreePool (NULL, NULL) == EFI_INVALID_PARAMETER);
  CHECK (AsanCheckFree ((VOID *)(UINTN)1));
  CHECK (AsanCheckFree (Heap)); /* Heap redzone is not freed shadow. */
  AsanTrackStack ((UINTN)Stack, sizeof (Stack), StackShadow);
  memset (StackShadow, ASAN_SHADOW_HEAP_FREED, sizeof (StackShadow));
  CHECK (AsanCheckFree (Stack)); /* Only heap shadow denotes a double free. */
  Buffer = Allocate ();
  mAsanRegionCount = 0;
  CHECK (CoreFreePoolI (Buffer, NULL) == EFI_SUCCESS);
  CHECK (mAsanQuarantineNext == 0);
  CHECK ((ReportCount () == 0) && (Halts == 0));
  puts ("PASS untracked/non-heap shadow is ignored and untracked frees bypass quarantine");
}

static VOID
TestEviction (VOID)
{
  VOID      *Buffers[ASAN_QUARANTINE_ENTRIES + 1];
  POOL_HEAD *Head;
  UINTN     Index;
  UINTN     Used;

  Reset (TRUE);
  for (Index = 0; Index < ARRAY_SIZE (Buffers); Index++) {
    Buffers[Index] = Allocate ();
  }
  for (Index = 0; Index < ARRAY_SIZE (Buffers); Index++) {
    CHECK (CoreFreePoolI (Buffers[Index], NULL) == EFI_SUCCESS);
  }
  Head = BASE_CR (Buffers[0], POOL_HEAD, Data);
  CHECK (Head->Signature == POOL_FREE_SIGNATURE);
  CHECK (Shadow[((UINTN)Buffers[0] - (UINTN)Heap) / 8] == ASAN_SHADOW_HEAP_FREED);
  Used = mPoolHead[EfiBootServicesData].Used;
  /* Even an unusable size must never be read before the double-free report. */
  Head->Size = MAX_UINTN;
  CHECK (CoreFreePoolI (Buffers[0], NULL) == EFI_INVALID_PARAMETER);
  CHECK ((ReportCount () == 1) && (Halts == 1) && (Asserts == 0));
  CHECK (mPoolHead[EfiBootServicesData].Used == Used);
  CHECK (mAsanQuarantineNext == 1);
  CHECK (mAsanQuarantine[0] == Buffers[ASAN_QUARANTINE_ENTRIES]);
  puts ("PASS evicted/overwritten header reports double-free before CR or tail access");
}

static VOID
TestQuarantineDefenseAndReuse (VOID)
{
  VOID  *Buffer;
  VOID  *Buffers[ASAN_QUARANTINE_ENTRIES + 1];
  UINTN Index;

  Reset (TRUE);
  Buffer = Allocate ();
  CHECK (CoreFreePoolI (Buffer, NULL) == EFI_SUCCESS);
  CHECK (CoreFreePoolI (Buffer, NULL) == EFI_INVALID_PARAMETER);
  CHECK ((ReportCount () == 1) && (Halts == 1) && (Asserts == 0));
  CHECK (AsanQuarantine (Buffer, 128) == NULL);
  CHECK ((ReportCount () == 2) && (Halts == 2) && (mAsanQuarantineNext == 1));

  Reset (TRUE);
  for (Index = 0; Index < ARRAY_SIZE (Buffers); Index++) {
    Buffers[Index] = Allocate ();
  }
  for (Index = 0; Index < ARRAY_SIZE (Buffers); Index++) {
    CHECK (CoreFreePoolI (Buffers[Index], NULL) == EFI_SUCCESS);
  }
  Buffer = Allocate ();
  CHECK (Buffer == Buffers[0]);
  CHECK (AsanCheckFree (Buffer));
  CHECK (CoreFreePoolI (Buffer, NULL) == EFI_SUCCESS);
  CHECK ((ReportCount () == 0) && (Halts == 0));
  puts ("PASS quarantined double-free, direct quarantine defense, and reuse");
}

static UINTN ServiceChecks;
static BOOLEAN EFIAPI
ServiceCheckFree (CONST VOID *Buffer)
{
  ServiceChecks++;
  return AsanRtCheckFree (Buffer);
}

static VOID
TestService (VOID)
{
  ASAN_SERVICE Service;
  VOID         *Buffer;
  UINTN        Index;

  Reset (FALSE);
  Service = *(ASAN_SERVICE *)AsanGetService ();
  Service.Header.Version = 1;
  CHECK (!AsanUseService (&Service));
  CHECK (!AsanUseService (NULL));
  Service.Header.Version = ASAN_SERVICE_VERSION;
  Service.Header.Signature = 0;
  CHECK (!AsanUseService (&Service));
  Service.Header.Signature = ASAN_SERVICE_SIGNATURE;
  Service.CheckFree = ServiceCheckFree;
  ServiceChecks = 0;
  CHECK (AsanUseService (&Service));
  AsanInitialize ((UINTN)Heap, sizeof (Heap), Shadow);
  Buffer = Allocate ();
  CHECK (CoreFreePoolI (Buffer, NULL) == EFI_SUCCESS);
  /* Evict via the service, retaining freed shadow while replacing the header. */
  for (Index = 0; Index < ASAN_QUARANTINE_ENTRIES; Index++) {
    CHECK (CoreFreePoolI (Allocate (), NULL) == EFI_SUCCESS);
  }
  CHECK (BASE_CR (Buffer, POOL_HEAD, Data)->Signature == POOL_FREE_SIGNATURE);
  CHECK (CoreFreePoolI (Buffer, NULL) == EFI_INVALID_PARAMETER);
  CHECK (ServiceChecks == ASAN_QUARANTINE_ENTRIES + 2);
  CHECK ((ReportCount () == 1) && (Halts == 1) && (Asserts == 0));
  puts ("PASS shared service dispatch and incompatible version rejection");
}
#endif

static VOID
TestInactive (VOID)
{
  VOID *Buffer;

  Reset (FALSE);
  Buffer = Allocate ();
  CHECK (AsanCheckFree (Buffer));
  CHECK (CoreFreePoolI (Buffer, NULL) == EFI_SUCCESS);
  CHECK (BASE_CR (Buffer, POOL_HEAD, Data)->Signature != POOL_HEAD_SIGNATURE);
  CHECK ((ReportCount () == 0) && (Halts == 0));
  CHECK (AsanQuarantine (Buffer, 128) == Buffer);
  /* No runtime or null library must not dereference even an untracked pointer. */
  CHECK (AsanCheckFree ((VOID *)(UINTN)1));
#ifndef ASAN_TEST_NULL
  CHECK (mAsanRuntime == NULL);
  CHECK (mAsanQuarantineNext == 0);
  Reset (TRUE);
  Buffer = Allocate ();
  CHECK (CoreFreePoolI (Buffer, NULL) == EFI_SUCCESS);
  AsanStubConnect (NULL);
  __asan_load1 ((UINTN)Buffer);
  CHECK ((ReportCount () == 0) && (Halts == 0));
  /* Compiler stub disconnection must not disable allocator runtime checking. */
  CHECK (!AsanCheckFree (Buffer));
  CHECK ((ReportCount () == 1) && (Halts == 1));
#else
  CHECK (AsanGetService () == NULL);
  CHECK (!AsanUseService (NULL));
#endif
  puts ("PASS inactive/null runtime and disconnected compiler callbacks");
}

int
main (void)
{
  TestMetadataValidation ();
#ifndef ASAN_TEST_NULL
  TestShadowOnlyScope ();
  TestEviction ();
  TestQuarantineDefenseAndReuse ();
  TestService ();
#endif
  TestInactive ();
  return 0;
}
