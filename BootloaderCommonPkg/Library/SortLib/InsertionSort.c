/** @file
  Library used for sorting routines.

  Copyright (c) 2019, Intel Corporation. All rights reserved. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SortLib.h>

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
  )
{
  LIST_ENTRY            *Curr;

  ASSERT ((ListHead != NULL) && (Entry != NULL) && (CompareFunction != NULL));

  Curr = ListHead->BackLink;
  while (Curr != ListHead) {
    if (CompareFunction ((VOID *)Entry, (VOID *)Curr) > 0) {
      break;
    }
    Curr = Curr->BackLink;
  }

  Curr->ForwardLink->BackLink = Entry;
  Entry->BackLink = Curr;
  Entry->ForwardLink = Curr->ForwardLink;
  Curr->ForwardLink = Entry;
}
