/** @file
  Header file for container library implementation.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _CONTAINER_LIB_H_
#define _CONTAINER_LIB_H_

#include <Library/PcdLib.h>

#define CONTAINER_LIST_SIGNATURE SIGNATURE_32('C','T','N', 'L')

#define AUTH_TYPE_NONE                0
#define AUTH_TYPE_SHA2_256            1
#define AUTH_TYPE_SHA2_384            2
#define AUTH_TYPE_SIG_RSA2048_SHA256  3
#define AUTH_TYPE_SIG_RSA3072_SHA384  4

typedef struct {
  UINT32           Signature;
  UINT32           HeaderCache;
  UINT32           Base;
  UINT32           Reserved;
} CONTAINER_ENTRY;

typedef struct {
  UINT32           Signature;
  UINT32           Reserved;
  UINT32           TotalLength;
  UINT32           Count;
  CONTAINER_ENTRY  Entry[0];
} CONTAINER_LIST;

typedef struct {
  UINT32           Signature;
  UINT16           Version;
  UINT16           DataOffset;
  UINT32           DataSize;
  UINT8            AuthType;
  UINT8            ImageType;
  UINT8            Flags;
  UINT8            Count;
} CONTAINER_HDR;

typedef struct {
  UINT32           Name;
  UINT32           Offset;
  UINT32           Size;
  UINT8            Rserved;
  UINT8            Alignment;
  UINT8            AuthType;
  UINT8            HashSize;
  UINT8            HashData[0];
} COMPONENT_ENTRY;


/**
  Load a component region information from a container.

  @param[in] ContainerSig    Container signature.
  @param[in] ComponentName   component name.
  @param[in] Buffer          Pointer to receive component base.
  @param[in] Length          Pointer to receive component size.

  @retval EFI_UNSUPPORTED          Unsupported AuthType.
  @retval EFI_NOT_FOUND            Cannot locate component.
  @retval EFI_SECURITY_VIOLATION   Authentication failed.
  @retval EFI_SUCCESS              Authentication succeeded.

**/
EFI_STATUS
EFIAPI
LocateComponentRegion (
  IN     UINT32    ContainerSig,
  IN     UINT32    ComponentName,
  OUT    UINT32   *RegionBase,
  OUT    UINT32   *RegionLength
  );

/**
  Locate a component from a container.

  @param[in]     ContainerSig       Container signature.
  @param[in]     ComponentName      Component name.
  @param[in,out] ContainerEntryPtr  Pointer to receive container entry info.
  @param[in,out] ComponentEntryPtr  Pointer to receive component entry info.

  @retval EFI_UNSUPPORTED          Unsupported AuthType.
  @retval EFI_NOT_FOUND            Cannot locate component.
  @retval EFI_SECURITY_VIOLATION   Authentication failed.
  @retval EFI_SUCCESS              component region is located successfully.

**/
EFI_STATUS
LocateComponent (
  IN      UINT32                  ContainerSig,
  IN      UINT32                  ComponentName,
  IN OUT  CONTAINER_ENTRY       **ContainerEntryPtr,
  IN OUT  COMPONENT_ENTRY       **ComponentEntryPtr
  );

/**
  Load a component from a container to memory.

  @param[in]     ContainerSig    Container signature or component type.
  @param[in]     ComponentName   Component name.
  @param[in,out] Buffer          Pointer to receive component base.
  @param[in,out] Length          Pointer to receive component size.

  @retval EFI_UNSUPPORTED          Unsupported AuthType.
  @retval EFI_NOT_FOUND            Cannot locate component.
  @retval EFI_BUFFER_TOO_SMALL     Specified buffer size is too small.
  @retval EFI_SECURITY_VIOLATION   Authentication failed.
  @retval EFI_SUCCESS              Authentication succeeded.

**/
EFI_STATUS
EFIAPI
LoadComponent (
  IN     UINT32    ContainerSig,
  IN     UINT32    ComponentName,
  IN OUT VOID    **Buffer,
  IN OUT UINT32   *Length
  );

#endif
