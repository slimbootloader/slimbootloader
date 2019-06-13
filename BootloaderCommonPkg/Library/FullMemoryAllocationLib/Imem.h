/** @file
  Data structure and functions to allocate and free memory space.

Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IMEM_H_
#define _IMEM_H_

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#define  EFI_LOCK    UINTN

//
// attributes for reserved memory before it is promoted to system memory
//
#define EFI_MEMORY_PRESENT       0x0100000000000000ULL
#define EFI_MEMORY_INITIALIZED   0x0200000000000000ULL
#define EFI_MEMORY_TESTED        0x0400000000000000ULL
#define MEMORY_ATTRIBUTE_DEFAULT (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED)

#define  ASSERT_LOCKED(x)

#define  EFI_ACPI_RUNTIME_PAGE_ALLOCATION_ALIGNMENT  (EFI_PAGE_SIZE)
#define  DEFAULT_PAGE_ALLOCATION                     (EFI_PAGE_SIZE)

typedef struct {
  UINT32  Type;             ///< EFI memory type defined in UEFI specification.
  UINT32  NumberOfPages;    ///< The pages of this type memory.
} EFI_MEMORY_TYPE_INFORMATION;

///
/// Enumeration of EFI memory allocation types.
///
typedef enum {
  ///
  /// Allocate any available range of pages that satisfies the request.
  ///
  AllocateAnyPages,
  ///
  /// Allocate any available range of pages whose uppermost address is less than
  /// or equal to a specified maximum address.
  ///
  AllocateMaxAddress,
  ///
  /// Allocate pages at a specified address.
  ///
  AllocateAddress,
  ///
  /// Maximum enumeration value that may be used for bounds checking.
  ///
  MaxAllocateType
} EFI_ALLOCATE_TYPE;

//
// +---------------------------------------------------+
// | 0..(EfiMaxMemoryType - 1)    - Normal memory type |
// +---------------------------------------------------+
// | EfiMaxMemoryType..0x6FFFFFFF - Invalid            |
// +---------------------------------------------------+
// | 0x70000000..0x7FFFFFFF       - OEM reserved       |
// +---------------------------------------------------+
// | 0x80000000..0xFFFFFFFF       - OS reserved        |
// +---------------------------------------------------+
//
#define MEMORY_TYPE_OS_RESERVED_MIN                 0x80000000
#define MEMORY_TYPE_OS_RESERVED_MAX                 0xFFFFFFFF
#define MEMORY_TYPE_OEM_RESERVED_MIN                0x70000000
#define MEMORY_TYPE_OEM_RESERVED_MAX                0x7FFFFFFF

//
// MEMORY_MAP_ENTRY
//

#define MEMORY_MAP_SIGNATURE   SIGNATURE_32('m','m','a','p')

typedef struct {
  UINTN           Signature;
  LIST_ENTRY      Link;
  BOOLEAN         FromPages;

  EFI_MEMORY_TYPE Type;
  UINT64          Start;
  UINT64          End;

  UINT64          VirtualStart;
  UINT64          Attribute;
} MEMORY_MAP;

//
// Internal prototypes
//


/**
  Internal function.  Used by the pool functions to allocate pages
  to back pool allocation requests.

  @param  PoolType               The type of memory for the new pool pages
  @param  NumberOfPages          No of pages to allocate
  @param  Alignment              Bits to align.

  @return The allocated memory, or NULL

**/
VOID *
CoreAllocatePoolPages (
  IN EFI_MEMORY_TYPE    PoolType,
  IN UINTN              NumberOfPages,
  IN UINTN              Alignment
  );



/**
  Internal function.  Frees pool pages allocated via AllocatePoolPages ()

  @param  Memory                 The base address to free
  @param  NumberOfPages          The number of pages to free

**/
VOID
CoreFreePoolPages (
  IN EFI_PHYSICAL_ADDRESS   Memory,
  IN UINTN                  NumberOfPages
  );



/**
  Internal function to allocate pool of a particular type.
  Caller must have the memory lock held

  @param  PoolType               Type of pool to allocate
  @param  Size                   The amount of pool to allocate

  @return The allocate pool, or NULL

**/
VOID *
CoreAllocatePoolI (
  IN EFI_MEMORY_TYPE  PoolType,
  IN UINTN            Size
  );



/**
  Internal function to free a pool entry.
  Caller must have the memory lock held

  @param  Buffer                 The allocated pool entry to free
  @param  PoolType               Pointer to pool type

  @retval EFI_INVALID_PARAMETER  Buffer not valid
  @retval EFI_SUCCESS            Buffer successfully freed.

**/
EFI_STATUS
CoreFreePoolI (
  IN VOID               *Buffer,
  OUT EFI_MEMORY_TYPE   *PoolType OPTIONAL
  );



/**
  Enter critical section by gaining lock on gMemoryLock.

**/
VOID
CoreAcquireMemoryLock (
  VOID
  );


/**
  Exit critical section by releasing lock on gMemoryLock.

**/
VOID
CoreReleaseMemoryLock (
  VOID
  );

/**
  Raising to the task priority level of the mutual exclusion
  lock, and then acquires ownership of the lock.

  @param  Lock               The lock to acquire

  @return Lock owned

**/
VOID
CoreAcquireLock (
  IN EFI_LOCK  *Lock
  );


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
  );



/**
  Releases ownership of the mutual exclusion lock, and
  restores the previous task priority level.

  @param  Lock               The lock to release

  @return Lock unowned

**/
VOID
CoreReleaseLock (
  IN EFI_LOCK  *Lock
  );

/**
  Allocates pages from the memory map.

  @param  Type                   The type of allocation to perform
  @param  MemoryType             The type of memory to turn the allocated pages
                                 into
  @param  NumberOfPages          The number of pages to allocate
  @param  Memory                 A pointer to receive the base allocated memory
                                 address

  @return Status. On success, Memory is filled in with the base address allocated
  @retval EFI_INVALID_PARAMETER  Parameters violate checking rules defined in
                                 spec.
  @retval EFI_NOT_FOUND          Could not allocate pages match the requirement.
  @retval EFI_OUT_OF_RESOURCES   No enough pages to allocate.
  @retval EFI_SUCCESS            Pages successfully allocated.

**/
EFI_STATUS
EFIAPI
CoreAllocatePages (
  IN EFI_ALLOCATE_TYPE      Type,
  IN EFI_MEMORY_TYPE        MemoryType,
  IN UINTN                  NumberOfPages,
  IN OUT EFI_PHYSICAL_ADDRESS  *Memory
  );

/**
  Frees previous allocated pages.

  @param  Memory                 Base address of memory being freed
  @param  NumberOfPages          The number of pages to free

  @retval EFI_NOT_FOUND          Could not find the entry that covers the range
  @retval EFI_INVALID_PARAMETER  Address not aligned
  @return EFI_SUCCESS         -Pages successfully freed.

**/
EFI_STATUS
EFIAPI
CoreFreePages (
  IN EFI_PHYSICAL_ADDRESS   Memory,
  IN UINTN                  NumberOfPages
  );

/**
  Frees pool.

  @param  Buffer                 The allocated pool entry to free

  @retval EFI_INVALID_PARAMETER  Buffer is not a valid value.
  @retval EFI_SUCCESS            Pool successfully freed.

**/
EFI_STATUS
EFIAPI
CoreFreePool (
  IN VOID  *Buffer
  );

/**
  Allocate pool of a particular type.

  @param  PoolType               Type of pool to allocate
  @param  Size                   The amount of pool to allocate
  @param  Buffer                 The address to return a pointer to the allocated
                                 pool

  @retval EFI_INVALID_PARAMETER  Buffer is NULL.
                                 PoolType is in the range EfiMaxMemoryType..0x6FFFFFFF.
                                 PoolType is EfiPersistentMemory.
  @retval EFI_OUT_OF_RESOURCES   Size exceeds max pool size or allocation failed.
  @retval EFI_SUCCESS            Pool successfully allocated.

**/
EFI_STATUS
EFIAPI
CoreAllocatePool (
  IN EFI_MEMORY_TYPE  PoolType,
  IN UINTN            Size,
  OUT VOID            **Buffer
  );

/**
  Internal function.  Adds a ranges to the memory map.
  The range must not already exist in the map.

  @param  Type                   The type of memory range to add
  @param  Start                  The starting address in the memory range Must be
                                 paged aligned
  @param  End                    The last address in the range Must be the last
                                 byte of a page
  @param  Attribute              The attributes of the memory range to add

**/
VOID
CoreAddRange (
  IN EFI_MEMORY_TYPE          Type,
  IN EFI_PHYSICAL_ADDRESS     Start,
  IN EFI_PHYSICAL_ADDRESS     End,
  IN UINT64                   Attribute
  );

/**
  Called to initialize the pool.

**/
VOID
CoreInitializePool (
  VOID
  );

/**
  Called to initialize the Pages.

**/
VOID
CoreInitializePages (
  VOID
  );

/**
Called to initialize the memory map and add descriptors to
the current descriptor list.
The first descriptor that is added must be general usable
memory as the addition allocates heap.

@param  Type                   The type of memory to add
@param  Start                  The starting address in the memory range Must be
                               page aligned
@param  NumberOfPages          The number of pages in the range
@param  Attribute              Attributes of the memory to add

@return None.  The range is added to the memory map

**/
VOID
CoreAddMemoryDescriptor (
  IN EFI_MEMORY_TYPE       Type,
  IN EFI_PHYSICAL_ADDRESS  Start,
  IN UINT64                NumberOfPages,
  IN UINT64                Attribute
  );

//
// Internal Global data
//
extern EFI_LOCK           gMemoryLock;
extern LIST_ENTRY         gMemoryMap;
extern LIST_ENTRY         mGcdMemorySpaceMap;
extern BOOLEAN            mMemoryTypeInformationInitialized;
extern EFI_MEMORY_TYPE_INFORMATION   gMemoryTypeInformation[EfiMaxMemoryType + 1];
#endif
