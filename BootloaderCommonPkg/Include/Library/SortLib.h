/** @file
  Library used for sorting and comparison routines.

  Copyright (c) 2009 - 2019, Intel Corporation. All rights reserved. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef __SORT_LIB_H__
#define __SORT_LIB_H__

/**
  Prototype for comparison function for any two element types.

  @param[in] Buffer1                  The pointer to first buffer.
  @param[in] Buffer2                  The pointer to second buffer.

  @retval 0                           Buffer1 equal to Buffer2.
  @return <0                          Buffer1 is less than Buffer2.
  @return >0                          Buffer1 is greater than Buffer2.
**/
typedef
INTN
(EFIAPI *SORT_COMPARE)(
  IN CONST VOID                 *Buffer1,
  IN CONST VOID                 *Buffer2
  );

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
  );

/**
  Insertion Sort for doubly-linked list

  This function is to insert a new list entry to a sorted doubly-linked list head.

  @param[in, out] ListHead          A pointer to the head node of doubly-linked list
  @param[in, out] Entry             A pointer to the new entry node to be inserted
                                    to the sorted list head
  @param[in]      CompareFunction   The function to call to perform the comparison
                                    of any 2 elements

**/
VOID
EFIAPI
PerformInsertionSortList (
  IN OUT  LIST_ENTRY                *ListHead,
  IN OUT  LIST_ENTRY                *Entry,
  IN      SORT_COMPARE               CompareFunction
  );

#endif //__SORT_LIB_H__
