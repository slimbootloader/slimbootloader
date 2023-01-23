/** @file
  Header file for container library implementation.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _CONTAINER_LIB_H_
#define _CONTAINER_LIB_H_

#include <Library/PcdLib.h>
#include <Library/CryptoLib.h>


#define CONTAINER_LIST_SIGNATURE SIGNATURE_32('C','T','N', 'L')

#define PROGESS_ID_LOCATE             1
#define PROGESS_ID_COPY               2
#define PROGESS_ID_AUTHENTICATE       3
#define PROGESS_ID_DECOMPRESS         4

typedef UINT8 AUTH_TYPE;
#define AUTH_TYPE_NONE                       0
#define AUTH_TYPE_SHA2_256                   1
#define AUTH_TYPE_SHA2_384                   2
#define AUTH_TYPE_SIG_RSA2048_PKCSI1_SHA256  3
#define AUTH_TYPE_SIG_RSA3072_PKCSI1_SHA384  4
#define AUTH_TYPE_SIG_RSA2048_PSS_SHA256     5
#define AUTH_TYPE_SIG_RSA3072_PSS_SHA384     6

#define CONTAINER_OEM_BASE_SIGNATURE        SIGNATURE_32 ('O', 'E', 'M',   0)
#define CONTAINER_BOOT_SIGNATURE            SIGNATURE_32 ('B', 'O', 'O', 'T')
#define CONTAINER_SETUP_SIGNATURE           SIGNATURE_32 ('S', 'E', 'T', 'P')
#define CONTAINER_MONO_SIGN_SIGNATURE       SIGNATURE_32 ('_', 'S', 'G', '_')
#define CONTAINER_KEY_HASH_STORE_SIGNATURE  SIGNATURE_32 ('K', 'E', 'Y', 'H')

// Flags for CONTAINER_HDR
#define CONTAINER_HDR_FLAG_MONO_SIGNING     BIT0

// Attributes for COMPONENT_ENTRY
#define COMPONENT_ENTRY_ATTR_RESERVED       BIT7

// Container Image types
#define CONTAINER_TYPE_NORMAL               0x0     // Used for boot images in FV, regular ELF, PE32, etc. formats
#define CONTAINER_TYPE_CLASSIC_LINUX        0x3     // Used for booting Linux with bzImage, cmdline, initrd, etc.
#define CONTAINER_TYPE_MULTIBOOT            0x4     // Multiboot compliant ELF images

// Max images per container
#define MAX_CONTAINER_SUB_IMAGE             32

typedef struct {
  UINT32           ComponentType;
  UINT8           *CompBuf;
  UINT32           CompLen;
  HASH_ALG_TYPE    HashAlg;
  UINT8           *HashData;
} COMPONENT_CALLBACK_INFO;

typedef VOID (*LOAD_COMPONENT_CALLBACK) (UINT32 ProgressId, COMPONENT_CALLBACK_INFO *CbInfo);

typedef struct {
  UINT32           Signature;
  UINT32           HeaderCache;
  UINT32           HeaderSize;
  UINT32           Base;
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
  UINT8            Version;
  UINT8            Svn;
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
  UINT8            Attribute;
  UINT8            Alignment;
  UINT8            AuthType;
  UINT8            HashSize;
  UINT8            HashData[0];
} COMPONENT_ENTRY;


/**
  Load a component from a container or flahs map to memory and call callback
  function at predefined point.

  @param[in]     ContainerSig    Container signature or component type.
  @param[in]     ComponentName   Component name.
  @param[in,out] Buffer          Pointer to receive component base.
  @param[in,out] Length          Pointer to receive component size.
  @param[in]     CallbackFunc    Callback function pointer.

  @retval EFI_UNSUPPORTED          Unsupported AuthType.
  @retval EFI_NOT_FOUND            Cannot locate component.
  @retval EFI_BUFFER_TOO_SMALL     Specified buffer size is too small.
  @retval EFI_SECURITY_VIOLATION   Authentication failed.
  @retval EFI_SUCCESS              Authentication succeeded.

**/
EFI_STATUS
EFIAPI
LoadComponentWithCallback (
  IN     UINT32                   ContainerSig,
  IN     UINT32                   ComponentName,
  IN OUT VOID                   **Buffer,
  IN OUT UINT32                  *Length,
  IN     LOAD_COMPONENT_CALLBACK  LoadComponentCallback
  );

/**
  Locate a component region information from a container or flash map.

  @param[in]      ContainerSig    Container signature or component type.
  @param[in]      ComponentName   component name.
  @param[in, out] Buffer          Pointer to receive component base.
  @param[in, out] Length          Pointer to receive component size.

  @retval EFI_UNSUPPORTED          Unsupported AuthType.
  @retval EFI_NOT_FOUND            Cannot locate component.
  @retval EFI_SECURITY_VIOLATION   Authentication failed.
  @retval EFI_SUCCESS              Authentication succeeded.

**/
EFI_STATUS
EFIAPI
LocateComponent (
  IN     UINT32    ContainerSig,
  IN     UINT32    ComponentName,
  IN OUT VOID    **Buffer,
  IN OUT UINT32   *Length
  );

/**
  Load a component from a container or flash map to memory.

  @param[in] ContainerSig    Container signature or component type.
  @param[in] ComponentName   Component name.
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

/**
  Locate a component information from a container.

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
LocateComponentEntry (
  IN      UINT32                  ContainerSig,
  IN      UINT32                  ComponentName,
  IN OUT  CONTAINER_ENTRY       **ContainerEntryPtr,
  IN OUT  COMPONENT_ENTRY       **ComponentEntryPtr
  );


/**
  This function returns the component entry info.

  @param[in] ContainerEntry    Container entry pointer.
  @param[in] ComponentName     Component name in container.

  @retval         Container header size.

**/
COMPONENT_ENTRY  *
EFIAPI
LocateComponentEntryFromContainer (
  IN  CONTAINER_HDR  *ContainerHdr,
  IN  UINT32          ComponentName
  );

/**
  Get Next Component in a container.

  @param[in]       ContainerSig    Container signature or component type.
  @param[in, out]  ComponentName   Pointer to receive next component name.

  @retval EFI_INVALID_PARAMETER    ComponentName is NULL.
  @retval EFI_NOT_FOUND            Cannot find next component.
  @retval EFI_SUCCESS              Next component was found successfully.

**/
EFI_STATUS
EFIAPI
GetNextAvailableComponent (
  IN     UINT32     ContainerSig,
  IN     UINT32    *ComponentName
);

/**
  This function registers a container.

  @param[in]  ContainerBase      Container base address to register.
  @param[in]  ContainerCallback  Callback regsiterd to notify container buf info

  @retval EFI_NOT_READY          Not ready for register yet.
  @retval EFI_BUFFER_TOO_SMALL   Insufficant max container entry number.
  @retval EFI_OUT_OF_RESOURCES   No space to add new container.
  @retval EFI_SUCCESS            The container has been registered successfully.

**/
EFI_STATUS
RegisterContainer (
  IN  UINT32                    ContainerBase,
  IN  LOAD_COMPONENT_CALLBACK   ContainerCallback
  );

/**
  This function unregisters a container with given signature.

  @param[in]  Signature      Container signature.

  @retval EFI_NOT_READY          Not ready for unregister yet.
  @retval EFI_NOT_FOUND          Not container available for unregisteration.
  @retval EFI_SUCCESS            The container has been unregistered successfully.

**/
EFI_STATUS
UnregisterContainer (
  IN  UINT32   Signature
  );
#endif
