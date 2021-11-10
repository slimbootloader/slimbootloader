/** @file
  Global data used in memory service

Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Imem.h>


//
// MemoryLock - synchronizes access to the memory map and pool lists
//
EFI_LOCK          gMemoryLock;

//
// MemoryMap - the current memory map
//
LIST_ENTRY        gMemoryMap  = INITIALIZE_LIST_HEAD_VARIABLE (gMemoryMap);


/**
  Raising to the task priority level of the mutual exclusion
  lock, and then acquires ownership of the lock.

  @param  Lock               The lock to acquire

  @return Lock owned

**/
VOID
CoreAcquireLock (
  IN EFI_LOCK  *Lock
  )
{
  ASSERT ((Lock != NULL) && (*Lock == 0));
  *Lock = 1;
}



/**
  Releases ownership of the mutual exclusion lock, and
  restores the previous task priority level.

  @param  Lock               The lock to release

  @return Lock unowned

**/
VOID
CoreReleaseLock (
  IN EFI_LOCK  *Lock
  )
{
  ASSERT ((Lock != NULL) && (*Lock == 1));
  *Lock = 0;
}

//
// Lock Stuff
//
/**
  Initialize a basic mutual exclusion lock.   Each lock
  provides mutual exclusion access at it's task priority
  level.  Since there is no-premption (at any TPL) or
  multiprocessor support, acquiring the lock only consists
  of raising to the locks TPL.

  @param  Lock               The EFI_LOCK structure to initialize

  @retval EFI_SUCCESS        Lock Owned.
  @retval EFI_ACCESS_DENIED  Reentrant Lock Acquisition, Lock not Owned.

**/
EFI_STATUS
CoreAcquireLockOrFail (
  IN EFI_LOCK  *Lock
  )
{
  ASSERT (Lock != NULL);

  if (*Lock == 1) {
    //
    // Lock is already owned, so bail out
    //
    return EFI_ACCESS_DENIED;
  }

  *Lock = 1;

  return EFI_SUCCESS;
}
