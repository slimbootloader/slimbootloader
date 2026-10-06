/** @file
  Direct memory API tests with the real runtime, wrappers and NASM backends.
  Reuse the pool harness's host debug/print support and runtime reset helpers.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define ASAN_MEMORY_TEST
#define main PoolFreeMain
#include "PoolFreeTest.c"
#undef main
#include <Library/AsanStubLib.h>

/* Negative control: reproduce the old direct-call gap without editing production. */
#ifdef ASAN_TEST_BYPASS_DIRECT
#define CopyMem AsanInternalCopyMem
#define SetMem AsanInternalSetMem
#endif

static jmp_buf  ReportJump;
static BOOLEAN ExpectReport;
static UINTN   Reads;
static UINTN   Writes;

VOID EFIAPI
CpuDeadLoop (VOID)
{
  Halts++;
  CHECK (ExpectReport);
  longjmp (ReportJump, 1);
}

#ifndef ASAN_TEST_NO_ASAN
VOID *__asan_memcpy (VOID *To, CONST VOID *From, UINTN Size);
VOID *__asan_memmove (VOID *To, CONST VOID *From, UINTN Size);
VOID *__asan_memset (VOID *Buffer, INT32 Value, UINTN Size);

static VOID EFIAPI
CountCheck (UINTN Address, UINTN Size, BOOLEAN IsWrite, VOID *Pc)
{
  CHECK (Size != 0);
  CHECK (Pc != NULL);
  if (IsWrite) {
    Writes++;
  } else {
    Reads++;
  }
#ifndef ASAN_TEST_NULL
  AsanCheck (Address, Size, IsWrite, Pc);
#endif
}
#endif

static VOID
TestLegal (VOID)
{
  UINT8 Source[64];
  UINT8 Buffer[64];
  UINT8 Expected[64];
  UINTN Index;

  for (Index = 0; Index < sizeof (Source); Index++) {
    Source[Index] = (UINT8)Index;
  }
  CHECK (CopyMem (Buffer, Source, sizeof (Source)) == Buffer);
  CHECK (memcmp (Buffer, Source, sizeof (Source)) == 0);
  memcpy (Expected, Buffer, sizeof (Buffer));
  memmove (Expected + 3, Expected, 40);
  CHECK (CopyMem (Buffer + 3, Buffer, 40) == Buffer + 3);
  CHECK (memcmp (Buffer, Expected, sizeof (Buffer)) == 0);
  memmove (Expected, Expected + 3, 40);
  CHECK (CopyMem (Buffer, Buffer + 3, 40) == Buffer);
  CHECK (memcmp (Buffer, Expected, sizeof (Buffer)) == 0);
  CHECK (CopyMem (Buffer, Buffer, sizeof (Buffer)) == Buffer);
  CHECK (SetMem (Buffer, sizeof (Buffer), 0xA5) == Buffer);
  for (Index = 0; Index < sizeof (Buffer); Index++) {
    CHECK (Buffer[Index] == 0xA5);
  }
  /* Neither the service nor the raw primitives may touch these addresses. */
  CHECK (CopyMem (NULL, (VOID *)(UINTN)1, 0) == NULL);
  CHECK (SetMem ((VOID *)(UINTN)1, 0, 0) == (VOID *)(UINTN)1);
  puts ("PASS copy/fill, both overlap directions, self-copy and zero length");
}

#ifndef ASAN_TEST_NULL
static VOID
TestViolation (UINTN Operation, CONST ASAN_SERVICE *Service)
{
  UINT8 *Source;
  UINT8 *Destination;
  UINTN Before;

  Reset (FALSE);
  if (Service != NULL) {
    CHECK (AsanUseService (Service));
  }
  AsanInitialize ((UINTN)Heap, sizeof (Heap), Shadow);
  /* Reset simulates a fresh module, but cannot reset AsanConnect's local static. */
  AsanStubConnect ((Service != NULL) ? Service : AsanGetService ());
  Reads = Writes = 0;
  Source = Heap + 256;
  Destination = Heap + 512;
  AsanUnpoison (Source, 13);
  AsanUnpoison (Destination, 13);
  memset (Source, 0x31, 32);
  memset (Destination, 0x52, 32);
  Before = Halts;
  ExpectReport = TRUE;
  if (setjmp (ReportJump) == 0) {
    switch (Operation) {
      case 0:
        AsanUnpoison (Destination, 32);
        CopyMem (Destination, Source, 14); /* Invalid source read. */
        break;
      case 1:
        AsanUnpoison (Source, 32);
        CopyMem (Destination, Source, 14); /* Invalid destination write. */
        break;
      case 2:
        SetMem (Destination, 14, 0xFF);
        break;
      case 3:
        CopyMem (Source, Source, 14); /* Self-copy must still check. */
        break;
      case 4:
        __asan_memcpy (Destination, Source, 14);
        break;
      case 5:
        __asan_memmove (Destination, Source, 14);
        break;
      default:
        __asan_memset (Destination, 0xFF, 14);
        break;
    }
    CHECK (FALSE);
  }
  ExpectReport = FALSE;
  CHECK (Halts == Before + 1);
  CHECK (ReportCount () == 1);
  CHECK (Source[0] == 0x31);
  CHECK (Destination[0] == 0x52); /* Report before any destination modification. */
  CHECK (Destination[13] == 0x52);
  if (Service != NULL) {
    CHECK (Reads == (((Operation == 2) || (Operation == 6)) ? 0 : 1));
    CHECK (Writes == (((Operation == 0) || (Operation == 3) ||
                      (Operation == 4) || (Operation == 5)) ? 0 : 1));
  }
}

static VOID
TestTrackedAndShadow (VOID)
{
  UINT8 *Buffer;
  UINTN Index;
  _Alignas (8) UINT8 Stack[256];
  _Alignas (8) UINT8 GlobalBuffer[256];
  ASAN_GLOBAL       Global = { 0 };

  Reset (TRUE);
  AsanStubConnect (AsanGetService ());
  Buffer = Heap + 256;
  AsanUnpoison (Buffer, 13);
  CHECK (SetMem (Buffer, 13, 0xA5) == Buffer);
  CHECK (CopyMem (Buffer + 1, Buffer, 12) == Buffer + 1);
  CHECK (CopyMem (Buffer, Buffer + 1, 12) == Buffer);
  CHECK (CopyMem (Buffer + 13, Buffer + 13, 0) == Buffer + 13);
  CHECK (SetMem (Buffer + 13, 0, 0) == Buffer + 13);
  CHECK (Halts == 0);
  /*
   * Shadow of a second region lives in poisoned heap memory. Registration,
   * poisoning, unpoisoning, compiler shadow writes and no-return cleanup must
   * bypass public checks even after the first runtime has connected.
   */
  AsanTrackStack ((UINTN)Stack, sizeof (Stack), Heap + 8192);
  AsanPoison (Stack, sizeof (Stack), ASAN_SHADOW_STACK_LEFT);
  CHECK (Heap[8192] == ASAN_SHADOW_STACK_LEFT);
  AsanUnpoison (Stack, 13);
  CHECK (Heap[8193] == 5);
  AsanSetShadow ((UINTN)Stack >> 3, 32, ASAN_SHADOW_STACK_RIGHT);
  CHECK (Heap[8192] == ASAN_SHADOW_STACK_RIGHT);
  AsanHandleNoReturn ((UINTN)Stack);
  CHECK (Heap[8192] == 0);
  Global.Beg = (UINTN)GlobalBuffer;
  Global.Size = 8;
  Global.SizeWithRedzone = sizeof (GlobalBuffer);
  CHECK (AsanRtAddRegion (AsanRegionGlobals, (UINTN)GlobalBuffer,
                         sizeof (GlobalBuffer), Heap + 16384, &Global, &Global + 1));
  CHECK (Heap[16384] == 0);
  CHECK (Heap[16385] == ASAN_SHADOW_GLOBAL_REDZONE);
  CHECK (Halts == 0);
  for (Index = 0; Index < 7; Index++) {
    TestViolation (Index, NULL);
  }
  puts ("PASS tracked legal accesses, raw shadow maintenance and seven invalid API/intrinsic cases");
}

static VOID
TestPayloadService (VOID)
{
  ASAN_SERVICE Service;
  UINTN        Index;

  Reset (FALSE);
  Service = *(CONST ASAN_SERVICE *)AsanGetService ();
  Service.Check = CountCheck;
  CHECK (AsanUseService (&Service));
  AsanInitialize ((UINTN)Heap, sizeof (Heap), Shadow);
  AsanStubConnect (&Service);
  AsanUnpoison (Heap + 256, 13);
  Reads = Writes = 0;
  CopyMem (Heap + 256, Heap + 256, 13);
  SetMem (Heap + 256, 13, 0);
  CopyMem (NULL, NULL, 0);
  SetMem (NULL, 0, 0);
  CHECK ((Reads == 1) && (Writes == 2));
  for (Index = 0; Index < 7; Index++) {
    TestViolation (Index, &Service);
  }
  AsanStubConnect (NULL);
  SetMem (Heap + 512, 14, 0);
  CopyMem (Heap + 512, Heap + 256, 14);
  CHECK (Heap[512 + 13] == 0x31);
  puts ("PASS service-connected payload checks and disconnected poisoned accesses");
}
#endif

int
main (void)
{
#ifndef ASAN_TEST_NO_ASAN
  AsanStubConnect (NULL);
#endif
  TestLegal ();
#ifndef ASAN_TEST_NULL
  TestTrackedAndShadow ();
  TestPayloadService ();
#elif !defined (ASAN_TEST_NO_ASAN)
  {
    ASAN_SERVICE Service = { 0 };
    Service.Check = CountCheck;
    AsanStubConnect (&Service);
    Reads = Writes = 0;
    TestLegal ();
    CHECK ((Reads == 4) && (Writes == 5));
    Reads = Writes = 0;
    __asan_memcpy (Heap + 64, Heap, 16);
    __asan_memmove (Heap + 1, Heap, 16);
    __asan_memset (Heap, 0x1234, 16);
    CHECK ((Reads == 2) && (Writes == 3));
    CHECK (Heap[0] == 0x34);
    AsanStubConnect (NULL);
    puts ("PASS runtime-free module linkage and exactly-once intrinsic dispatch");
  }
#endif
  return 0;
}
