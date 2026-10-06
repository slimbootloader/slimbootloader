/** @file
  AddressSanitizer runtime interface. AsanStubLib forwards the compiler callbacks to it,
  and Stage2 publishes it so payloads share one runtime with Stage2.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ASAN_SERVICE_H__
#define __ASAN_SERVICE_H__

#include <Uefi/UefiBaseType.h>
#include <Guid/BootLoaderServiceGuid.h>

#define ASAN_SERVICE_SIGNATURE  SIGNATURE_32 ('S', 'A', 'S', 'N')
#define ASAN_SERVICE_VERSION    2

/**
  Check that every byte of an access is addressable, and report and halt if not.

  @param[in] Address    Access address.
  @param[in] Size       Access size in bytes.
  @param[in] IsWrite    TRUE for a write access.
  @param[in] Pc         Return address into the instrumented code.

**/
typedef
VOID
(EFIAPI *ASAN_CHECK) (
  IN UINTN    Address,
  IN UINTN    Size,
  IN BOOLEAN  IsWrite,
  IN VOID     *Pc
  );

/**
  Set shadow bytes for a stack frame.

  @param[in] ShadowAddress  Address >> 3 of the first granule.
  @param[in] Count          Number of shadow bytes to set.
  @param[in] Value          Shadow value.

**/
typedef
VOID
(EFIAPI *ASAN_SET_SHADOW) (
  IN UINTN  ShadowAddress,
  IN UINTN  Count,
  IN UINT8  Value
  );

/**
  Clear the shadow of the stack that contains StackAddress before a call that never returns.

  @param[in] StackAddress   An address on the current stack.

**/
typedef
VOID
(EFIAPI *ASAN_HANDLE_NO_RETURN) (
  IN UINTN  StackAddress
  );

typedef enum {
  AsanRegionHeap,
  AsanRegionStack,
  AsanRegionGlobals
} ASAN_REGION_TYPE;

/**
  Start tracking a region.

  A heap starts fully poisoned. A stack starts addressable. Globals start addressable
  except for the redzones described by the compiler metadata in [Globals, GlobalsEnd).

  @param[in] Type         ASAN_REGION_TYPE of the region.
  @param[in] Base         Region base address, 8-byte aligned.
  @param[in] Size         Region size in bytes.
  @param[in] Shadow       Shadow buffer of Size / 8 bytes, rounded up.
  @param[in] Globals      First compiler global metadata entry, for AsanRegionGlobals.
  @param[in] GlobalsEnd   End of the global metadata entries, for AsanRegionGlobals.

  @retval TRUE            The region is tracked.

**/
typedef
BOOLEAN
(EFIAPI *ASAN_ADD_REGION) (
  IN UINT32      Type,
  IN UINTN       Base,
  IN UINTN       Size,
  IN VOID        *Shadow,
  IN CONST VOID  *Globals,
  IN CONST VOID  *GlobalsEnd
  );

/**
  Mark the 8-byte granules fully inside a range as not addressable.

  @param[in] Address      Range start.
  @param[in] Size         Range size in bytes.
  @param[in] Value        Shadow value.

**/
typedef
VOID
(EFIAPI *ASAN_POISON) (
  IN CONST VOID  *Address,
  IN UINTN       Size,
  IN UINT8       Value
  );

/**
  Mark a range as addressable.

  @param[in] Address      Range start, 8-byte aligned.
  @param[in] Size         Range size in bytes.

**/
typedef
VOID
(EFIAPI *ASAN_UNPOISON) (
  IN CONST VOID  *Address,
  IN UINTN       Size
  );

/**
  Defer the release of a freed heap buffer, reporting a double free.

  @param[in] Buffer       Heap buffer being freed.
  @param[in] Size         Size of the heap block that holds Buffer.

  @retval    The buffer to release now, or NULL if nothing is to be released.

**/
typedef
VOID *
(EFIAPI *ASAN_QUARANTINE) (
  IN VOID   *Buffer,
  IN UINTN  Size
  );

/**
  Name the image that code addresses in reports belong to.

  @param[in] Name         Image name, used by AsanSymbolize.py to find the image file.
  @param[in] Base         Image base address.

**/
typedef
VOID
(EFIAPI *ASAN_ADD_IMAGE) (
  IN CONST CHAR8  *Name,
  IN UINTN        Base
  );

/**
  Check freed heap shadow without reading allocator metadata or quarantining.

  @param[in] Buffer       Pool buffer being freed.

  @retval TRUE            Buffer is not known to be freed.
  @retval FALSE           Buffer is freed; a double-free report has been issued.

**/
typedef
BOOLEAN
(EFIAPI *ASAN_CHECK_FREE) (
  IN CONST VOID  *Buffer
  );

typedef struct {
  SERVICE_COMMON_HEADER    Header;
  ASAN_CHECK               Check;
  ASAN_SET_SHADOW          SetShadow;
  ASAN_HANDLE_NO_RETURN    HandleNoReturn;
  ASAN_ADD_REGION          AddRegion;
  ASAN_POISON              Poison;
  ASAN_UNPOISON            Unpoison;
  ASAN_QUARANTINE          Quarantine;
  ASAN_ADD_IMAGE           AddImage;
  ASAN_CHECK_FREE          CheckFree;
} ASAN_SERVICE;

#endif
