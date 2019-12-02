/** @file
  Library used for sorting routines.

  Copyright (c) 2009 - 2019, Intel Corporation. All rights reserved. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SortLib.h>

/**
  Worker function for QuickSorting.  This function is identical to PerformQuickSort,
  except that is uses the pre-allocated buffer so the in place sorting does not need to
  allocate and free buffers constantly.

  Each element must be equal sized.

  if BufferToSort is NULL, then ASSERT.
  if CompareFunction is NULL, then ASSERT.
  if Buffer is NULL, then ASSERT.

  if Count is < 2 then perform no action.
  if Size is < 1 then perform no action.

  @param[in, out] BufferToSort   on call a Buffer of (possibly sorted) elements
                                 on return a buffer of sorted elements
  @param[in] Count               the number of elements in the buffer to sort
  @param[in] ElementSize         Size of an element in bytes
  @param[in] CompareFunction     The function to call to perform the comparison
                                 of any 2 elements
  @param[in] Buffer              Buffer of size ElementSize for use in swapping
**/
VOID
EFIAPI
PerformQuickSort (
  IN OUT VOID                           *BufferToSort,
  IN CONST UINTN                        Count,
  IN CONST UINTN                        ElementSize,
  IN       SORT_COMPARE                 CompareFunction,
  IN VOID                               *Buffer
  )
{
  VOID        *Pivot;
  UINTN       LoopCount;
  UINTN       NextSwapLocation;

  ASSERT ((BufferToSort != NULL) && (CompareFunction != NULL) && (Buffer != NULL));

  if ((Count < 2)  || (ElementSize < 1)) {
    return;
  }

  NextSwapLocation = 0;

  //
  // pick a pivot (we choose last element)
  //
  Pivot = ((UINT8 *)BufferToSort + ((Count - 1) * ElementSize));

  //
  // Now get the pivot such that all on "left" are below it
  // and everything "right" are above it
  //
  for ( LoopCount = 0; LoopCount < Count - 1; LoopCount++) {
    //
    // if the element is less than the pivot
    //
    if (CompareFunction ((VOID *)((UINT8 *)BufferToSort + ((LoopCount)*ElementSize)), Pivot) <= 0) {
      //
      // swap
      //
      CopyMem (Buffer, (UINT8 *)BufferToSort + (NextSwapLocation * ElementSize), ElementSize);
      CopyMem ((UINT8 *)BufferToSort + (NextSwapLocation * ElementSize), (UINT8 *)BufferToSort + ((LoopCount)*ElementSize),
               ElementSize);
      CopyMem ((UINT8 *)BufferToSort + ((LoopCount)*ElementSize), Buffer, ElementSize);

      //
      // increment NextSwapLocation
      //
      NextSwapLocation++;
    }
  }
  //
  // swap pivot to it's final position (NextSwapLocaiton)
  //
  CopyMem (Buffer, Pivot, ElementSize);
  CopyMem (Pivot, (UINT8 *)BufferToSort + (NextSwapLocation * ElementSize), ElementSize);
  CopyMem ((UINT8 *)BufferToSort + (NextSwapLocation * ElementSize), Buffer, ElementSize);

  //
  // Now recurse on 2 paritial lists.  neither of these will have the 'pivot' element
  // IE list is sorted left half, pivot element, sorted right half...
  //
  if (NextSwapLocation >= 2) {
    PerformQuickSort (
      BufferToSort,
      NextSwapLocation,
      ElementSize,
      CompareFunction,
      Buffer);
  }

  if ((Count - NextSwapLocation - 1) >= 2) {
    PerformQuickSort (
      (UINT8 *)BufferToSort + (NextSwapLocation + 1) * ElementSize,
      Count - NextSwapLocation - 1,
      ElementSize,
      CompareFunction,
      Buffer);
  }

  return;
}
