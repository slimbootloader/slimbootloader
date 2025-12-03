/** @file
  Memory page management functions.

Copyright (c) 2007 - 2025, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Imem.h"

#define EFI_DEFAULT_PAGE_ALLOCATION_ALIGNMENT  (EFI_PAGE_SIZE)

//
// Entry for tracking the memory regions for each memory type to coalesce similar memory types
//
typedef struct {
  EFI_PHYSICAL_ADDRESS  BaseAddress;
  EFI_PHYSICAL_ADDRESS  MaximumAddress;
  UINT64                CurrentNumberOfPages;
  UINT64                NumberOfPages;
  UINTN                 InformationIndex;
  BOOLEAN               Special;
  BOOLEAN               Runtime;
} EFI_MEMORY_TYPE_STATISTICS;

//
// MemoryMap - The current memory map
//
UINTN         mMemoryMapKey = 0;

#define MAX_MAP_DEPTH 6

///
/// mMapDepth - depth of new descriptor stack
///
UINTN         mMapDepth = 0;
///
/// mMapStack - space to use as temp storage to build new map descriptors
///
MEMORY_MAP    mMapStack[MAX_MAP_DEPTH];
UINTN         mFreeMapStack = 0;
///
/// This list maintain the free memory map list
///
LIST_ENTRY    mFreeMemoryMapEntryList = INITIALIZE_LIST_HEAD_VARIABLE (mFreeMemoryMapEntryList);

CONST EFI_MEMORY_TYPE_STATISTICS mMemoryTypeStatisticsInit[EfiMaxMemoryType + 1] = {
  { 0, 0, 0, 0, EfiMaxMemoryType, TRUE,  FALSE },  // EfiReservedMemoryType
  { 0, 0, 0, 0, EfiMaxMemoryType, FALSE, FALSE },  // EfiLoaderCode
  { 0, 0, 0, 0, EfiMaxMemoryType, FALSE, FALSE },  // EfiLoaderData
  { 0, 0, 0, 0, EfiMaxMemoryType, FALSE, FALSE },  // EfiBootServicesCode
  { 0, MAX_ADDRESS, 0, 0, EfiMaxMemoryType, FALSE, FALSE },  // EfiBootServicesData
  { 0, 0, 0, 0, EfiMaxMemoryType, TRUE,  TRUE  },  // EfiRuntimeServicesCode
  { 0, MAX_ADDRESS, 0, 0, EfiMaxMemoryType, TRUE,  TRUE  },  // EfiRuntimeServicesData
  { 0, MAX_ADDRESS, 0, 0, EfiMaxMemoryType, FALSE, FALSE },  // EfiConventionalMemory
  { 0, 0, 0, 0, EfiMaxMemoryType, FALSE, FALSE },  // EfiUnusableMemory
  { 0, 0, 0, 0, EfiMaxMemoryType, TRUE,  FALSE },  // EfiACPIReclaimMemory
  { 0, 0, 0, 0, EfiMaxMemoryType, TRUE,  FALSE },  // EfiACPIMemoryNVS
  { 0, 0, 0, 0, EfiMaxMemoryType, FALSE, FALSE },  // EfiMemoryMappedIO
  { 0, 0, 0, 0, EfiMaxMemoryType, FALSE, FALSE },  // EfiMemoryMappedIOPortSpace
  { 0, 0, 0, 0, EfiMaxMemoryType, TRUE,  TRUE  },  // EfiPalCode
  { 0, 0, 0, 0, EfiMaxMemoryType, FALSE, FALSE },  // EfiPersistentMemory
  { 0, 0, 0, 0, EfiMaxMemoryType, FALSE, FALSE }   // EfiMaxMemoryType
};

EFI_MEMORY_TYPE_STATISTICS mMemoryTypeStatistics[EfiMaxMemoryType + 1];

/**
  Called to initialize the Pages.

**/
VOID
CoreInitializePages (
  VOID
  )
{
  mMemoryMapKey = 0;
  mMapDepth     = 0;
  mFreeMapStack = 0;

  InitializeListHead (&mFreeMemoryMapEntryList);
  InitializeListHead (&gMemoryMap);

  CopyMem (mMemoryTypeStatistics, mMemoryTypeStatisticsInit, sizeof (mMemoryTypeStatistics));
}

/**
  Enter critical section by gaining lock on gMemoryLock.

**/
VOID
CoreAcquireMemoryLock (
  VOID
  )
{
  CoreAcquireLock (&gMemoryLock);
}



/**
  Exit critical section by releasing lock on gMemoryLock.

**/
VOID
CoreReleaseMemoryLock (
  VOID
  )
{
  CoreReleaseLock (&gMemoryLock);
}




/**
  Internal function.  Removes a descriptor entry.

  @param  Entry                  The entry to remove

**/
VOID
RemoveMemoryMapEntry (
  IN OUT MEMORY_MAP      *Entry
  )
{
  RemoveEntryList (&Entry->Link);
  Entry->Link.ForwardLink = NULL;

  if (Entry->FromPages) {
    //
    // Insert the free memory map descriptor to the end of mFreeMemoryMapEntryList
    //
    InsertTailList (&mFreeMemoryMapEntryList, &Entry->Link);
  }
}

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
  )
{
  LIST_ENTRY        *Link;
  MEMORY_MAP        *Entry;

  ASSERT ((Start & EFI_PAGE_MASK) == 0);
  ASSERT (End > Start) ;

  ASSERT_LOCKED (&gMemoryLock);

  DEBUG ((DEBUG_PAGE, "AddRange: %lx-%lx to %d\n", Start, End, Type));

  //
  // If memory of type EfiConventionalMemory is being added that includes the page
  // starting at address 0, then zero the page starting at address 0.  This has
  // two benifits.  It helps find NULL pointer bugs and it also maximizes
  // compatibility with operating systems that may evaluate memory in this page
  // for legacy data structures.  If memory of any other type is added starting
  // at address 0, then do not zero the page at address 0 because the page is being
  // used for other purposes.
  //
  if (Type == EfiConventionalMemory && Start == 0 && (End >= EFI_PAGE_SIZE - 1)) {
    SetMem ((VOID *) (UINTN)Start, EFI_PAGE_SIZE, 0);
  }

  //
  // Memory map being altered so updated key
  //
  mMemoryMapKey += 1;

  //
  // Look for adjoining memory descriptor
  //

  // Two memory descriptors can only be merged if they have the same Type
  // and the same Attribute
  //

  Link = gMemoryMap.ForwardLink;
  while (Link != &gMemoryMap) {
    Entry = CR (Link, MEMORY_MAP, Link, MEMORY_MAP_SIGNATURE);
    Link  = Link->ForwardLink;

    if (Entry->Type != Type) {
      continue;
    }

    if (Entry->Attribute != Attribute) {
      continue;
    }

    if (Entry->End + 1 == Start) {

      Start = Entry->Start;
      RemoveMemoryMapEntry (Entry);

    } else if (Entry->Start == End + 1) {

      End = Entry->End;
      RemoveMemoryMapEntry (Entry);
    }
  }

  //
  // Add descriptor
  //

  mMapStack[mMapDepth].Signature     = MEMORY_MAP_SIGNATURE;
  mMapStack[mMapDepth].FromPages      = FALSE;
  mMapStack[mMapDepth].Type          = Type;
  mMapStack[mMapDepth].Start         = Start;
  mMapStack[mMapDepth].End           = End;
  mMapStack[mMapDepth].VirtualStart  = 0;
  mMapStack[mMapDepth].Attribute     = Attribute;
  InsertTailList (&gMemoryMap, &mMapStack[mMapDepth].Link);

  mMapDepth += 1;
  ASSERT (mMapDepth < MAX_MAP_DEPTH);

  return ;
}

/**
  Internal function.  Deque a descriptor entry from the mFreeMemoryMapEntryList.
  If the list is emtry, then allocate a new page to refuel the list.
  Please Note this algorithm to allocate the memory map descriptor has a property
  that the memory allocated for memory entries always grows, and will never really be freed
  For example, if the current boot uses 2000 memory map entries at the maximum point, but
  ends up with only 50 at the time the OS is booted, then the memory associated with the 1950
  memory map entries is still allocated from EfiBootServicesMemory.


  @return The Memory map descriptor dequed from the mFreeMemoryMapEntryList

**/
MEMORY_MAP *
AllocateMemoryMapEntry (
  VOID
  )
{
  MEMORY_MAP            *FreeDescriptorEntries;
  MEMORY_MAP            *Entry;
  UINTN                  Index;

  if (IsListEmpty (&mFreeMemoryMapEntryList)) {
    //
    // The list is empty, to allocate one page to refuel the list
    //
    FreeDescriptorEntries = CoreAllocatePoolPages (EfiBootServicesData, EFI_SIZE_TO_PAGES (DEFAULT_PAGE_ALLOCATION),
                            DEFAULT_PAGE_ALLOCATION);
    if (FreeDescriptorEntries != NULL) {
      //
      // Enque the free memmory map entries into the list
      //
      for (Index = 0; Index < DEFAULT_PAGE_ALLOCATION / sizeof (MEMORY_MAP); Index++) {
        FreeDescriptorEntries[Index].Signature = MEMORY_MAP_SIGNATURE;
        InsertTailList (&mFreeMemoryMapEntryList, &FreeDescriptorEntries[Index].Link);
      }
    } else {
      return NULL;
    }
  }
  //
  // dequeue the first descriptor from the list
  //
  Entry = CR (mFreeMemoryMapEntryList.ForwardLink, MEMORY_MAP, Link, MEMORY_MAP_SIGNATURE);
  RemoveEntryList (&Entry->Link);

  return Entry;
}


/**
  Internal function.  Moves any memory descriptors that are on the
  temporary descriptor stack to heap.

**/
VOID
CoreFreeMemoryMapStack (
  VOID
  )
{
  MEMORY_MAP      *Entry;
  MEMORY_MAP      *Entry2;
  LIST_ENTRY      *Link2;

  ASSERT_LOCKED (&gMemoryLock);

  //
  // If already freeing the map stack, then return
  //
  if (mFreeMapStack != 0) {
    return ;
  }

  //
  // Move the temporary memory descriptor stack into pool
  //
  mFreeMapStack += 1;

  while (mMapDepth != 0) {
    //
    // Deque an memory map entry from mFreeMemoryMapEntryList
    //
    Entry = AllocateMemoryMapEntry ();

    ASSERT (Entry);

    //
    // Update to proper entry
    //
    mMapDepth -= 1;

    if (mMapStack[mMapDepth].Link.ForwardLink != NULL) {

      //
      // Move this entry to general memory
      //
      RemoveEntryList (&mMapStack[mMapDepth].Link);
      mMapStack[mMapDepth].Link.ForwardLink = NULL;

      CopyMem (Entry, &mMapStack[mMapDepth], sizeof (MEMORY_MAP));
      Entry->FromPages = TRUE;

      //
      // Find insertion location
      //
      for (Link2 = gMemoryMap.ForwardLink; Link2 != &gMemoryMap; Link2 = Link2->ForwardLink) {
        Entry2 = CR (Link2, MEMORY_MAP, Link, MEMORY_MAP_SIGNATURE);
        if (Entry2->FromPages && Entry2->Start > Entry->Start) {
          break;
        }
      }

      InsertTailList (Link2, &Entry->Link);

    } else {
      //
      // This item of mMapStack[mMapDepth] has already been dequeued from gMemoryMap list,
      // so here no need to move it to memory.
      //
      InsertTailList (&mFreeMemoryMapEntryList, &Entry->Link);
    }
  }

  mFreeMapStack -= 1;
}

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
  )
{
  EFI_PHYSICAL_ADDRESS        End;

  if ((Start & EFI_PAGE_MASK) != 0) {
    return;
  }

  if (Type >= EfiMaxMemoryType && Type < MEMORY_TYPE_OEM_RESERVED_MIN) {
    return;
  }

  CoreAcquireMemoryLock ();
  End = Start + LShiftU64 (NumberOfPages, EFI_PAGE_SHIFT) - 1;
  CoreAddRange (EfiConventionalMemory, Start, End, Attribute);
  mMemoryTypeStatistics[Type].BaseAddress    = Start;
  mMemoryTypeStatistics[Type].MaximumAddress = End;
  mMemoryTypeStatistics[Type].NumberOfPages  = NumberOfPages;
  CoreFreeMemoryMapStack ();
  CoreReleaseMemoryLock ();
}

/**
  Internal function.  Converts a memory range to the specified type or attributes.
  The range must exist in the memory map.  Either ChangingType or
  ChangingAttributes must be set, but not both.

  @param  Start                  The first address of the range Must be page
                                 aligned
  @param  NumberOfPages          The number of pages to convert
  @param  ChangingType           Boolean indicating that type value should be changed
  @param  NewType                The new type for the memory range
  @param  ChangingAttributes     Boolean indicating that attributes value should be changed
  @param  NewAttributes          The new attributes for the memory range

  @retval EFI_INVALID_PARAMETER  Invalid parameter
  @retval EFI_NOT_FOUND          Could not find a descriptor cover the specified
                                 range  or convertion not allowed.
  @retval EFI_SUCCESS            Successfully converts the memory range to the
                                 specified type.

**/
EFI_STATUS
CoreConvertPagesEx (
  IN UINT64           Start,
  IN UINT64           NumberOfPages,
  IN BOOLEAN          ChangingType,
  IN EFI_MEMORY_TYPE  NewType,
  IN BOOLEAN          ChangingAttributes,
  IN UINT64           NewAttributes
  )
{

  UINT64          NumberOfBytes;
  UINT64          End;
  UINT64          RangeEnd;
  UINT64          Attribute;
  EFI_MEMORY_TYPE MemType;
  LIST_ENTRY      *Link;
  MEMORY_MAP      *Entry;

  Entry = NULL;
  NumberOfBytes = LShiftU64 (NumberOfPages, EFI_PAGE_SHIFT);
  End = Start + NumberOfBytes - 1;

  ASSERT (NumberOfPages);
  ASSERT ((Start & EFI_PAGE_MASK) == 0);
  ASSERT (End > Start) ;
  ASSERT_LOCKED (&gMemoryLock);
  ASSERT ( (ChangingType == FALSE) || (ChangingAttributes == FALSE) );

  if (NumberOfPages == 0 || ((Start & EFI_PAGE_MASK) != 0) || (Start >= End)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Convert the entire range
  //

  while (Start < End) {

    //
    // Find the entry that the covers the range
    //
    for (Link = gMemoryMap.ForwardLink; Link != &gMemoryMap; Link = Link->ForwardLink) {
      Entry = CR (Link, MEMORY_MAP, Link, MEMORY_MAP_SIGNATURE);

      if (Entry->Start <= Start && Entry->End > Start) {
        break;
      }
    }

    if (Link == &gMemoryMap) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "ConvertPages: failed to find range %lx - %lx\n", Start, End));
      return EFI_NOT_FOUND;
    }

    //
    // Convert range to the end, or to the end of the descriptor
    // if that's all we've got
    //
    RangeEnd = End;

    ASSERT (Entry != NULL);
    if (Entry->End < End) {
      RangeEnd = Entry->End;
    }

    if (ChangingType) {
      DEBUG ((DEBUG_PAGE, "ConvertRange: %lx-%lx to type %d\n", Start, RangeEnd, NewType));
    }
    if (ChangingAttributes) {
      DEBUG ((DEBUG_PAGE, "ConvertRange: %lx-%lx to attr %lx\n", Start, RangeEnd, NewAttributes));
    }

    if (ChangingType) {
      //
      // Debug code - verify conversion is allowed
      //
      if (! (NewType == EfiConventionalMemory ? 1 : 0) ^ (Entry->Type == EfiConventionalMemory ? 1 : 0)) {
        DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "ConvertPages: Incompatible memory types\n"));
        return EFI_NOT_FOUND;
      }

      //
      // Update counters for the number of pages allocated to each memory type
      //
      if ((UINT32)Entry->Type < EfiMaxMemoryType) {
        if (Start >= mMemoryTypeStatistics[Entry->Type].BaseAddress
            && Start <= mMemoryTypeStatistics[Entry->Type].MaximumAddress) {
          if (NumberOfPages > mMemoryTypeStatistics[Entry->Type].CurrentNumberOfPages) {
            mMemoryTypeStatistics[Entry->Type].CurrentNumberOfPages = 0;
          } else {
            mMemoryTypeStatistics[Entry->Type].CurrentNumberOfPages -= NumberOfPages;
          }
        }
      }

      if ((UINT32)NewType < EfiMaxMemoryType) {
        if (Start >= mMemoryTypeStatistics[NewType].BaseAddress && Start <= mMemoryTypeStatistics[NewType].MaximumAddress) {
          mMemoryTypeStatistics[NewType].CurrentNumberOfPages += NumberOfPages;
        }
      }
    }

    //
    // Pull range out of descriptor
    //
    if (Entry->Start == Start) {

      //
      // Clip start
      //
      Entry->Start = RangeEnd + 1;

    } else if (Entry->End == RangeEnd) {

      //
      // Clip end
      //
      Entry->End = Start - 1;

    } else {

      //
      // Pull it out of the center, clip current
      //

      //
      // Add a new one
      //
      mMapStack[mMapDepth].Signature = MEMORY_MAP_SIGNATURE;
      mMapStack[mMapDepth].FromPages  = FALSE;
      mMapStack[mMapDepth].Type      = Entry->Type;
      mMapStack[mMapDepth].Start     = RangeEnd + 1;
      mMapStack[mMapDepth].End       = Entry->End;

      //
      // Inherit Attribute from the Memory Descriptor that is being clipped
      //
      mMapStack[mMapDepth].Attribute = Entry->Attribute;

      Entry->End = Start - 1;
      ASSERT (Entry->Start < Entry->End);

      Entry = &mMapStack[mMapDepth];
      InsertTailList (&gMemoryMap, &Entry->Link);

      mMapDepth += 1;
      ASSERT (mMapDepth < MAX_MAP_DEPTH);
    }

    //
    // The new range inherits the same Attribute as the Entry
    // it is being cut out of unless attributes are being changed
    //
    if (ChangingType) {
      Attribute = Entry->Attribute;
      MemType = NewType;
    } else {
      Attribute = NewAttributes;
      MemType = Entry->Type;
    }

    //
    // If the descriptor is empty, then remove it from the map
    //
    if (Entry->Start == Entry->End + 1) {
      RemoveMemoryMapEntry (Entry);
      Entry = NULL;
    }

    //
    // Add our new range in
    //
    CoreAddRange (MemType, Start, RangeEnd, Attribute);
    if (ChangingType && (MemType == EfiConventionalMemory)) {
      //
      // Avoid calling DEBUG_CLEAR_MEMORY() for an address of 0 because this
      // macro will ASSERT() if address is 0.  Instead, CoreAddRange() guarantees
      // that the page starting at address 0 is always filled with zeros.
      //
      if (Start == 0) {
        if (RangeEnd > EFI_PAGE_SIZE) {
          DEBUG_CLEAR_MEMORY ((VOID *) (UINTN) EFI_PAGE_SIZE, (UINTN) (RangeEnd - EFI_PAGE_SIZE + 1));
        }
      } else {
        DEBUG_CLEAR_MEMORY ((VOID *) (UINTN) Start, (UINTN) (RangeEnd - Start + 1));
      }
    }

    //
    // Move any map descriptor stack to general pool
    //
    CoreFreeMemoryMapStack ();

    //
    // Bump the starting address, and convert the next range
    //
    Start = RangeEnd + 1;
  }

  //
  // Converted the whole range, done
  //

  return EFI_SUCCESS;
}


/**
  Internal function.  Converts a memory range to the specified type.
  The range must exist in the memory map.

  @param  Start                  The first address of the range Must be page
                                 aligned
  @param  NumberOfPages          The number of pages to convert
  @param  NewType                The new type for the memory range

  @retval EFI_INVALID_PARAMETER  Invalid parameter
  @retval EFI_NOT_FOUND          Could not find a descriptor cover the specified
                                 range  or convertion not allowed.
  @retval EFI_SUCCESS            Successfully converts the memory range to the
                                 specified type.

**/
EFI_STATUS
CoreConvertPages (
  IN UINT64           Start,
  IN UINT64           NumberOfPages,
  IN EFI_MEMORY_TYPE  NewType
  )
{
  return CoreConvertPagesEx (Start, NumberOfPages, TRUE, NewType, FALSE, 0);
}

/**
  Internal function. Finds a consecutive free page range below
  the requested address.

  @param  MaxAddress             The address that the range must be below
  @param  MinAddress             The address that the range must be above
  @param  NumberOfPages          Number of pages needed
  @param  NewType                The type of memory the range is going to be
                                 turned into
  @param  Alignment              Bits to align with

  @return The base address of the range, or 0 if the range was not found

**/
UINT64
CoreFindFreePagesI (
  IN UINT64           MaxAddress,
  IN UINT64           MinAddress,
  IN UINT64           NumberOfPages,
  IN EFI_MEMORY_TYPE  NewType,
  IN UINTN            Alignment
  )
{
  UINT64          NumberOfBytes;
  UINT64          Target;
  UINT64          DescStart;
  UINT64          DescEnd;
  UINT64          DescNumberOfBytes;
  LIST_ENTRY      *Link;
  MEMORY_MAP      *Entry;

  if ((MaxAddress < EFI_PAGE_MASK) || (NumberOfPages == 0)) {
    return 0;
  }

  if ((MaxAddress & EFI_PAGE_MASK) != EFI_PAGE_MASK) {

    //
    // If MaxAddress is not aligned to the end of a page
    //

    //
    // Change MaxAddress to be 1 page lower
    //
    MaxAddress -= (EFI_PAGE_MASK + 1);

    //
    // Set MaxAddress to a page boundary
    //
    MaxAddress &= ~ (UINT64)EFI_PAGE_MASK;

    //
    // Set MaxAddress to end of the page
    //
    MaxAddress |= EFI_PAGE_MASK;
  }

  NumberOfBytes = LShiftU64 (NumberOfPages, EFI_PAGE_SHIFT);
  Target = 0;

  for (Link = gMemoryMap.ForwardLink; Link != &gMemoryMap; Link = Link->ForwardLink) {
    Entry = CR (Link, MEMORY_MAP, Link, MEMORY_MAP_SIGNATURE);

    //
    // If it's not a free entry, don't bother with it
    //
    if (Entry->Type != EfiConventionalMemory) {
      continue;
    }

    DescStart = Entry->Start;
    DescEnd = Entry->End;

    //
    // If desc is past max allowed address or below min allowed address, skip it
    //
    if ((DescStart >= MaxAddress) || (DescEnd < MinAddress)) {
      continue;
    }

    //
    // If desc ends past max allowed address, clip the end
    //
    if (DescEnd >= MaxAddress) {
      DescEnd = MaxAddress;
    }

    DescEnd = ((DescEnd + 1) & (~((UINT64)Alignment - 1))) - 1;

    // Skip if DescEnd is less than DescStart after alignment clipping
    if (DescEnd < DescStart) {
      continue;
    }

    //
    // Compute the number of bytes we can used from this
    // descriptor, and see it's enough to satisfy the request
    //
    DescNumberOfBytes = DescEnd - DescStart + 1;

    if (DescNumberOfBytes >= NumberOfBytes) {
      //
      // If the start of the allocated range is below the min address allowed, skip it
      //
      if ((DescEnd - NumberOfBytes + 1) < MinAddress) {
        continue;
      }

      //
      // If this is the best match so far remember it
      //
      if (DescEnd > Target) {
        Target = DescEnd;
      }
    }
  }

  //
  // If this is a grow down, adjust target to be the allocation base
  //
  Target -= NumberOfBytes - 1;

  //
  // If we didn't find a match, return 0
  //
  if ((Target & EFI_PAGE_MASK) != 0) {
    return 0;
  }

  return Target;
}


/**
  Internal function.  Finds a consecutive free page range below
  the requested address

  @param  MaxAddress             The address that the range must be below
  @param  NoPages                Number of pages needed
  @param  NewType                The type of memory the range is going to be
                                 turned into
  @param  Alignment              Bits to align with

  @return The base address of the range, or 0 if the range was not found.

**/
UINT64
FindFreePages (
  IN UINT64           MaxAddress,
  IN UINT64           NoPages,
  IN EFI_MEMORY_TYPE  NewType,
  IN UINTN            Alignment
  )
{
  UINT64   Start;

  //
  // Attempt to find free pages in the preferred bin based on the requested memory type
  //
  if ((UINT32)NewType < EfiMaxMemoryType && MaxAddress >= mMemoryTypeStatistics[NewType].MaximumAddress) {
    Start = CoreFindFreePagesI (
              mMemoryTypeStatistics[NewType].MaximumAddress,
              mMemoryTypeStatistics[NewType].BaseAddress,
              NoPages,
              NewType,
              Alignment
              );
    if (Start != 0) {
      return Start;
    }
  }

  //
  // No enough resource
  //
  return 0;
}


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
CoreInternalAllocatePages (
  IN EFI_ALLOCATE_TYPE      Type,
  IN EFI_MEMORY_TYPE        MemoryType,
  IN UINTN                  NumberOfPages,
  IN OUT EFI_PHYSICAL_ADDRESS  *Memory
  )
{
  EFI_STATUS      Status;
  UINT64          Start;
  UINT64          NumberOfBytes;
  UINT64          End;
  UINT64          MaxAddress;
  UINTN           Alignment;

  if ((UINT32)Type >= MaxAllocateType) {
    return EFI_INVALID_PARAMETER;
  }

  if ((MemoryType >= EfiMaxMemoryType && MemoryType < MEMORY_TYPE_OEM_RESERVED_MIN) ||
      (MemoryType == EfiConventionalMemory) || (MemoryType == EfiPersistentMemory)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Memory == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Alignment = EFI_DEFAULT_PAGE_ALLOCATION_ALIGNMENT;

  if  (MemoryType == EfiACPIReclaimMemory   ||
       MemoryType == EfiACPIMemoryNVS       ||
       MemoryType == EfiRuntimeServicesCode ||
       MemoryType == EfiRuntimeServicesData) {

    Alignment = EFI_ACPI_RUNTIME_PAGE_ALLOCATION_ALIGNMENT;
  }

  if (Type == AllocateAddress) {
    if ((*Memory & (Alignment - 1)) != 0) {
      return EFI_NOT_FOUND;
    }
  }

  NumberOfPages += EFI_SIZE_TO_PAGES (Alignment) - 1;
  NumberOfPages &= ~ (EFI_SIZE_TO_PAGES (Alignment) - 1);

  //
  // If this is for below a particular address, then
  //
  Start = *Memory;

  //
  // The max address is the max natively addressable address for the processor
  //
  MaxAddress = MAX_ADDRESS;

  //
  // Check for Type AllocateAddress,
  // if NumberOfPages is 0 or
  // if (NumberOfPages << EFI_PAGE_SHIFT) is above MAX_ADDRESS or
  // if (Start + NumberOfBytes) rolls over 0 or
  // if Start is above MAX_ADDRESS or
  // if End is above MAX_ADDRESS,
  // return EFI_NOT_FOUND.
  //
  if (Type == AllocateAddress) {
    if ((NumberOfPages == 0) ||
        (NumberOfPages > RShiftU64 (MaxAddress, EFI_PAGE_SHIFT))) {
      return EFI_NOT_FOUND;
    }
    NumberOfBytes = LShiftU64 (NumberOfPages, EFI_PAGE_SHIFT);
    End = Start + NumberOfBytes - 1;

    if ((Start >= End) ||
        (Start > MaxAddress) ||
        (End > MaxAddress)) {
      return EFI_NOT_FOUND;
    }
  }

  if (Type == AllocateMaxAddress) {
    MaxAddress = Start;
  }

  CoreAcquireMemoryLock ();

  //
  // If not a specific address, then find an address to allocate
  //
  if (Type != AllocateAddress) {
    Start = FindFreePages (MaxAddress, NumberOfPages, MemoryType, Alignment);
    if (Start == 0) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
  }

  //
  // Convert pages from FreeMemory to the requested type
  //
  Status = CoreConvertPages (Start, NumberOfPages, MemoryType);

Done:
  CoreReleaseMemoryLock ();

  if (!EFI_ERROR (Status)) {
    *Memory = Start;
  }

  return Status;
}

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
  IN  EFI_ALLOCATE_TYPE     Type,
  IN  EFI_MEMORY_TYPE       MemoryType,
  IN  UINTN                 NumberOfPages,
  OUT EFI_PHYSICAL_ADDRESS  *Memory
  )
{
  EFI_STATUS  Status;

  Status = CoreInternalAllocatePages (Type, MemoryType, NumberOfPages, Memory);
  return Status;
}

/**
  Frees previous allocated pages.

  @param  Memory                 Base address of memory being freed
  @param  NumberOfPages          The number of pages to free
  @param  MemoryType             Pointer to memory type

  @retval EFI_NOT_FOUND          Could not find the entry that covers the range
  @retval EFI_INVALID_PARAMETER  Address not aligned
  @return EFI_SUCCESS         -Pages successfully freed.

**/
EFI_STATUS
EFIAPI
CoreInternalFreePages (
  IN EFI_PHYSICAL_ADDRESS   Memory,
  IN UINTN                  NumberOfPages,
  OUT EFI_MEMORY_TYPE       *MemoryType OPTIONAL
  )
{
  EFI_STATUS      Status;
  LIST_ENTRY      *Link;
  MEMORY_MAP      *Entry;
  UINTN           Alignment;

  //
  // Free the range
  //
  CoreAcquireMemoryLock ();

  //
  // Find the entry that the covers the range
  //
  Entry = NULL;
  for (Link = gMemoryMap.ForwardLink; Link != &gMemoryMap; Link = Link->ForwardLink) {
    Entry = CR (Link, MEMORY_MAP, Link, MEMORY_MAP_SIGNATURE);
    if (Entry->Start <= Memory && Entry->End > Memory) {
      break;
    }
  }
  if (Link == &gMemoryMap) {
    Status = EFI_NOT_FOUND;
    goto Done;
  }

  Alignment = EFI_DEFAULT_PAGE_ALLOCATION_ALIGNMENT;

  ASSERT (Entry != NULL);
  if  (Entry->Type == EfiACPIReclaimMemory   ||
       Entry->Type == EfiACPIMemoryNVS       ||
       Entry->Type == EfiRuntimeServicesCode ||
       Entry->Type == EfiRuntimeServicesData) {

    Alignment = EFI_ACPI_RUNTIME_PAGE_ALLOCATION_ALIGNMENT;

  }

  if ((Memory & (Alignment - 1)) != 0) {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  NumberOfPages += EFI_SIZE_TO_PAGES (Alignment) - 1;
  NumberOfPages &= ~ (EFI_SIZE_TO_PAGES (Alignment) - 1);

  if (MemoryType != NULL) {
    *MemoryType = Entry->Type;
  }

  Status = CoreConvertPages (Memory, NumberOfPages, EfiConventionalMemory);

  if (EFI_ERROR (Status)) {
    goto Done;
  }

Done:
  CoreReleaseMemoryLock ();
  return Status;
}

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
  IN EFI_PHYSICAL_ADDRESS  Memory,
  IN UINTN                 NumberOfPages
  )
{
  EFI_STATUS        Status;
  EFI_MEMORY_TYPE   MemoryType;

  Status = CoreInternalFreePages (Memory, NumberOfPages, &MemoryType);
  return Status;
}

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
  )
{
  UINT64            Start;

  //
  // Find the pages to convert
  //
  Start = FindFreePages (MAX_ADDRESS, NumberOfPages, PoolType, Alignment);

  //
  // Convert it to boot services data
  //
  if (Start == 0) {
    DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "AllocatePoolPages: failed to allocate %d pages\n", (UINT32)NumberOfPages));
  } else {
    CoreConvertPages (Start, NumberOfPages, PoolType);
  }

  return (VOID *) (UINTN) Start;
}


/**
  Internal function.  Frees pool pages allocated via AllocatePoolPages ()

  @param  Memory                 The base address to free
  @param  NumberOfPages          The number of pages to free

**/
VOID
CoreFreePoolPages (
  IN EFI_PHYSICAL_ADDRESS   Memory,
  IN UINTN                  NumberOfPages
  )
{
  CoreConvertPages (Memory, NumberOfPages, EfiConventionalMemory);
}

/**
  Finds all used pages for given type and count for size.

  @param  MemType     The type of memory to count.

  @retval     The total used memory size.

**/
UINT64
CoreGetUsedMemorySize (
  IN EFI_MEMORY_TYPE  MemType
  )
{
  LIST_ENTRY      *Link;
  MEMORY_MAP      *Entry;
  UINT64           Length;

  Length = 0;
  Entry = NULL;
  for (Link = gMemoryMap.ForwardLink; Link != &gMemoryMap; Link = Link->ForwardLink) {
    Entry = CR (Link, MEMORY_MAP, Link, MEMORY_MAP_SIGNATURE);

    //
    // If it's not a free entry, don't bother with it
    //
    if (Entry->Type == MemType) {
    //
    // If desc is past max allowed address or below min allowed address, skip it
    //
      Length += (Entry->End - Entry->Start + 1);
    }
  }

  return Length;
}

/**
  Retrieve the memory resource inforamtion for the specified type.

  @param[in]   Type          The type of memory.
  @param[out]  StartAddr     The pointer to receive the start address of the memory type.
  @param[out]  FreeAddr      The pointer to receive the last free address of the memory type.
  @param[out]  EndAddr       The pointer to receive the end address of the memory type.

  @retval     EFI_INVALID_PARAMETER  Invalid parameter for Type.
              EFI_SUCCESS            Memory resoruce information is returned successfully.
**/
EFI_STATUS
EFIAPI
GetMemoryResourceInfo (
  IN   EFI_MEMORY_TYPE   Type,
  OUT  UINT64           *StartAddr  OPTIONAL,
  OUT  UINT64           *FreeAddr   OPTIONAL,
  OUT  UINT64           *EndAddr    OPTIONAL
  )
{
  UINT64          Start;
  UINT64          End;

  if (Type >= EfiMaxMemoryType) {
    return EFI_INVALID_PARAMETER;
  }

  Start = mMemoryTypeStatistics[Type].BaseAddress;
  End   = Start + LShiftU64 (mMemoryTypeStatistics[Type].NumberOfPages, EFI_PAGE_SHIFT) - 1;

  if (StartAddr != NULL) {
    *StartAddr = Start;
  }

  if (EndAddr != NULL) {
    *EndAddr = End;
  }

  if (FreeAddr != NULL) {
    // Memory might be allocated in fragmented parts.
    // Here use a calculated virtual free memory address instead.
    // UsedMemory = EndAddr - FreeAddr
    *FreeAddr = End - CoreGetUsedMemorySize (Type);
  }

  return EFI_SUCCESS;
}
