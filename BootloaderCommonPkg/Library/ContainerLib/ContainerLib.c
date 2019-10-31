/** @file
  Container library implementation.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/ContainerLib.h>
#include <Library/CryptoLib.h>
#include <Library/SecureBootLib.h>
#include <Library/DecompressLib.h>

#define  TEMP_BUF_ALIGN    0x10
#define  AUTH_DATA_ALIGN   0x04

#define  IS_FLASH_ADDRESS(x)   (((UINT32)(UINTN)(x)) >= 0xF0000000)

/**
  Get the container pointer by the container signature

  @param[in] Signature            The signature for the container to get.

  @retval NULL                    The service is not available.
  @retval Others                  The pointer of container entry.

**/
STATIC
CONTAINER_ENTRY *
GetContainerBySignature (
  IN UINT32       Signature
  )
{
  UINT32                Index;
  CONTAINER_LIST       *ContainerList;
  CONTAINER_ENTRY      *ContainerEntry;

  ContainerList = (CONTAINER_LIST *)GetContainerListPtr ();
  if (ContainerList != NULL) {
    for (Index = 0; Index < ContainerList->Count; Index++) {
      ContainerEntry = (CONTAINER_ENTRY *)&ContainerList->Entry[Index];
      if (ContainerEntry->Signature == Signature) {
        return ContainerEntry;
      }
    }
  }

  return NULL;
}

/**
  This function registers a container.

  @param[in]  ContainerBase      Container base address to register.

  @retval EFI_NOT_READY          Not ready for register yet.
  @retval EFI_BUFFER_TOO_SMALL   Insufficant max container entry number.
  @retval EFI_OUT_OF_RESOURCES   No space to add new container.
  @retval EFI_UNSUPPORTED        Duplicated container found.
  @retval EFI_SUCCESS            The container has been registered successfully.

**/
STATIC
EFI_STATUS
RegisterContainerInternal (
  IN UINT32    ContainerBase
  )
{
  CONTAINER_LIST       *ContainerList;
  CONTAINER_HDR        *ContainerHdr;
  CONTAINER_ENTRY      *ContainerEntry;
  UINT32                Index;
  VOID                 *Buffer;

  ContainerList = (CONTAINER_LIST *)GetContainerListPtr ();
  if (ContainerList == NULL) {
    return EFI_NOT_READY;
  }

  ContainerHdr   = (CONTAINER_HDR *)ContainerBase;
  ContainerEntry = GetContainerBySignature (ContainerHdr->Signature);
  if (ContainerEntry != NULL) {
    return EFI_UNSUPPORTED;
  }

  Index = ContainerList->Count;
  if (Index >= PcdGet32 (PcdContainerMaxNumber)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Buffer = AllocatePool (ContainerHdr->DataOffset);
  if (Buffer == NULL) {
    return  EFI_OUT_OF_RESOURCES;
  }

  ContainerList->Entry[Index].Signature   = ContainerHdr->Signature;
  ContainerList->Entry[Index].HeaderCache = (UINT32)Buffer;
  ContainerList->Entry[Index].Base        = ContainerBase;
  CopyMem (Buffer, (VOID *)ContainerBase, ContainerHdr->DataOffset);
  ContainerList->Count++;

  return EFI_SUCCESS;
}


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
  )
{
  EFI_STATUS            Status;
  CONTAINER_LIST       *ContainerList;
  CONTAINER_ENTRY      *ContainerEntry;
  UINT32                Index;
  UINT32                LastIndex;

  ContainerList = (CONTAINER_LIST *)GetContainerListPtr ();
  if (ContainerList == NULL) {
    return EFI_NOT_READY;
  }

  Status = EFI_NOT_FOUND;
  if (ContainerList->Count == 0) {
    return Status;
  }

  LastIndex = ContainerList->Count - 1;
  if (Signature == 0xFFFFFFFF) {
    Index = LastIndex;
  } else {
    for (Index = 0; Index < ContainerList->Count; Index++) {
      ContainerEntry = (CONTAINER_ENTRY *)&ContainerList->Entry[Index];
      if (ContainerEntry->Signature == Signature) {
        break;
      }
    }
  }

  if (Index < ContainerList->Count) {
    FreePool ((VOID *)ContainerList->Entry[Index].HeaderCache);
    ContainerList->Entry[Index] = ContainerList->Entry[LastIndex];
    ContainerList->Count--;
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
  This function returns the container header size.

  @param[in] ContainerEntry    Container entry pointer.

  @retval         Container header size.

**/
STATIC
UINT32
GetContainerHeaderSize (
  IN  CONTAINER_HDR  *ContainerHdr
  )
{
  INT32                 Offset;
  UINT32                Index;
  COMPONENT_ENTRY      *CompEntry;

  Offset = 0;
  if (ContainerHdr != NULL) {
    CompEntry   = (COMPONENT_ENTRY *)&ContainerHdr[1];
    for (Index = 0; Index < ContainerHdr->Count; Index++) {
      CompEntry = (COMPONENT_ENTRY *)((UINT8 *)(CompEntry + 1) + CompEntry->HashSize);
      Offset = (UINT8 *)CompEntry - (UINT8 *)ContainerHdr;
      if ((Offset < 0) || (Offset >= ContainerHdr->DataOffset)) {
        Offset = 0;
        break;
      }
    }
  }

  return (UINT32)Offset;
}

/**
  This function returns the component entry info.

  @param[in] ContainerEntry    Container entry pointer.
  @param[in] ComponentName     Component name in container.

  @retval         Container header size.

**/
STATIC
COMPONENT_ENTRY  *
LocateComponentEntryFromContainer (
  IN  CONTAINER_HDR  *ContainerHdr,
  IN  UINT32          ComponentName
  )
{
  UINT32                Index;
  COMPONENT_ENTRY      *CompEntry;
  COMPONENT_ENTRY      *CurrEntry;

  CompEntry = NULL;
  CurrEntry = (COMPONENT_ENTRY *)&ContainerHdr[1];
  for (Index = 0; Index < ContainerHdr->Count; Index++) {
    if (CurrEntry->Name == ComponentName) {
      CompEntry = CurrEntry;
      break;
    }
    CurrEntry = (COMPONENT_ENTRY *)((UINT8 *)(CurrEntry + 1) + CurrEntry->HashSize);
  }

  return CompEntry;
}

/**
  Authenticate a container header or component.

  @param[in] Data         Data buffer to be authenticated.
  @param[in] Length       Data length to be authenticated.
  @param[in] AuthType     Authentication type.
  @param[in] AuthData     Authentication data buffer.
  @param[in] HashData     Hash data buffer.
  @param[in] CompType     Component type.

  @retval EFI_UNSUPPORTED          Unsupported AuthType.
  @retval EFI_SECURITY_VIOLATION   Authentication failed.
  @retval EFI_SUCCESS              Authentication succeeded.

**/
STATIC
EFI_STATUS
AuthenticateComponent (
  IN  UINT8    *Data,
  IN  UINT32    Length,
  IN  UINT8     AuthType,
  IN  UINT8    *AuthData,
  IN  UINT8    *HashData,
  IN  UINT8     CompType
  )
{
  EFI_STATUS  Status;

  if (!FeaturePcdGet (PcdVerifiedBootEnabled)) {
    Status = EFI_SUCCESS;
  } else {
    if (AuthType == AUTH_TYPE_SHA2_256) {
      Status = DoHashVerify (Data, Length, HASH_TYPE_SHA256, CompType, HashData);
    } else if (AuthType == AUTH_TYPE_SIG_RSA2048_SHA256) {
      Status = DoRsaVerify (Data, Length, CompType, AuthData,
                            AuthData + RSA2048NUMBYTES, HashData, NULL);
    } else if (AuthType == AUTH_TYPE_NONE) {
      Status = EFI_SUCCESS;
    } else {
      Status = EFI_UNSUPPORTED;
    }
  }

  return Status;
}

/**
  Return Containser Key Type based on its signature

  @param[in]    ContainerSig    Signature of the container

  @retval                       COMP_TYPE_PUBKEY_OS for CONTAINER boot image
                                COMP_TYPE_PUBKEY_CFG_DATA, otherwise
**/
UINT8
GetContainerKeyTypeBySig (
  IN  UINT32    ContainerSig
  )
{
  if (ContainerSig == CONTAINER_BOOT_SIGNATURE) {
    return COMP_TYPE_PUBKEY_OS;
  } else {
    return COMP_TYPE_PUBKEY_CFG_DATA;
  }
}

/**
  This function authenticates a container

  @param[in]  ContainerHeader    Container base address to register.

  @retval EFI_SUCCESS            The container has been authenticated successfully.
  @retval EFI_UNSUPPORTED        If container header is invalid or autheication fails

**/
EFI_STATUS
AutheticateContainerInternal (
  IN  CONTAINER_HDR      *ContainerHeader
  )
{
  CONTAINER_HDR            *ContainerHdr;
  CONTAINER_ENTRY          *ContainerEntry;
  COMPONENT_ENTRY          *CompEntry;
  UINT8                    *AuthData;
  UINT32                    ContainerHdrSize;
  UINT8                     AuthType;
  UINT8                    *DataBuf;
  UINT8                    *CompData;
  UINT32                    DataLen;
  UINT32                    SignedDataLen;
  UINT32                    Index;
  LOADER_COMPRESSED_HEADER *CompressHdr;
  EFI_STATUS                Status;

  // Find authentication data offset and authenticate the container header
  Status = EFI_UNSUPPORTED;
  ContainerEntry   = GetContainerBySignature (ContainerHeader->Signature);
  if (ContainerEntry != NULL) {
    ContainerHdr     = (CONTAINER_HDR *)ContainerEntry->HeaderCache;
    ContainerHdrSize = GetContainerHeaderSize (ContainerHdr);
    if (ContainerHdrSize > 0) {
      AuthType = ContainerHdr->AuthType;
      AuthData = (UINT8 *)ContainerHdr + ALIGN_UP(ContainerHdrSize, AUTH_DATA_ALIGN);
      if ((AuthType == AUTH_TYPE_NONE) && FeaturePcdGet (PcdVerifiedBootEnabled)) {
        Status = EFI_SECURITY_VIOLATION;
      } else {
        Status = AuthenticateComponent ((UINT8 *)ContainerHdr, ContainerHdrSize,
                                        AuthType, AuthData, NULL, GetContainerKeyTypeBySig(ContainerHeader->Signature) );
      }
    }
  }

  if (!EFI_ERROR (Status) && \
      ((ContainerHdr->Flags & CONTAINER_HDR_FLAG_MONO_SIGNING) != 0)) {
    // Additional verification if the container is signed monolithically.
    // It is required for the container to be loaded in memory before registeration.
    Status = EFI_UNSUPPORTED;
    if ((ContainerHdr->Count > 1) && !IS_FLASH_ADDRESS (ContainerHeader)) {
      // Use the last entry to verify all other combined components
      CompEntry = (COMPONENT_ENTRY *)&ContainerHdr[1];
      for (Index = 0; Index < (UINT32)(ContainerHdr->Count - 1); Index++) {
        CompEntry = (COMPONENT_ENTRY *)((UINT8 *)(CompEntry + 1) + CompEntry->HashSize);
      }
      CompData    = (UINT8 *)(ContainerEntry->Base + ContainerHdr->DataOffset + CompEntry->Offset);
      CompressHdr = (LOADER_COMPRESSED_HEADER *)CompData;
      if (CompressHdr->Signature == LZDM_SIGNATURE) {
        SignedDataLen = sizeof (LOADER_COMPRESSED_HEADER) + CompressHdr->CompressedSize;
        AuthData = CompData + ALIGN_UP(SignedDataLen, AUTH_DATA_ALIGN);
        DataBuf  = (UINT8 *)(ContainerEntry->Base + ContainerHdr->DataOffset);
        DataLen  = CompEntry->Offset;
        Status   = AuthenticateComponent (DataBuf, DataLen, CompEntry->AuthType,
                                          AuthData, CompEntry->HashData, COMP_TYPE_INVALID);
      }
    }
  }

  return Status;
}

/**
  This function registers a container.

  @param[in]  ContainerBase      Container base address to register.

  @retval EFI_NOT_READY          Not ready for register yet.
  @retval EFI_BUFFER_TOO_SMALL   Insufficant max container entry number.
  @retval EFI_OUT_OF_RESOURCES   No space to add new container.
  @retval EFI_SUCCESS            The container has been registered successfully.

**/
EFI_STATUS
RegisterContainer (
  IN  UINT32   ContainerBase
  )
{
  EFI_STATUS                Status;
  CONTAINER_HDR            *ContainerHdr;
  UINT64                    SignatureBuffer;

  ContainerHdr     = (CONTAINER_HDR *)ContainerBase;
  SignatureBuffer  = ContainerHdr->Signature;
  DEBUG ((EFI_D_INFO, "Registering container %4a\n", (CHAR8 *)&SignatureBuffer));

  // Register container
  Status = RegisterContainerInternal (ContainerBase);
  if (!EFI_ERROR (Status)) {
    Status = AutheticateContainerInternal (ContainerHdr);
    if (EFI_ERROR (Status)) {
      // Unregister the container since authentication failed
      UnregisterContainer (ContainerHdr->Signature);
    }
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Registering container failed due to %r\n", Status));
  }

  return Status;
}

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
  )
{
  EFI_STATUS                Status;
  CONTAINER_HDR            *ContainerHdr;
  CONTAINER_ENTRY          *ContainerEntry;
  COMPONENT_ENTRY          *CompEntry;
  UINT32                    ContainerBase;
  UINT32                    ContainerSize;

  // Search container header from cache
  ContainerEntry = GetContainerBySignature (ContainerSig);
  if (ContainerEntry == NULL) {
    // Location container from Flash Map
    Status = GetComponentInfo (ContainerSig, &ContainerBase, &ContainerSize);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    // Register container temporarily
    Status = RegisterContainer (ContainerBase);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    // Find authentication data offset and authenticate the container header
    ContainerEntry = GetContainerBySignature (ContainerSig);
    if (ContainerEntry == NULL) {
      return EFI_UNSUPPORTED;
    }
  }

  // Locate the component from the container header
  ContainerHdr = (CONTAINER_HDR *)ContainerEntry->HeaderCache;
  CompEntry = LocateComponentEntryFromContainer (ContainerHdr, ComponentName);
  if (CompEntry == NULL) {
    return EFI_NOT_FOUND;
  }

  if (ContainerEntryPtr != NULL) {
    *ContainerEntryPtr = ContainerEntry;
  }

  if (ComponentEntryPtr != NULL) {
    *ComponentEntryPtr   = CompEntry;
  }

  return EFI_SUCCESS;
}

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
)
{
  EFI_STATUS         Status;
  CONTAINER_HDR     *ContainerHdr;
  CONTAINER_ENTRY   *ContainerEntry;
  COMPONENT_ENTRY   *CurrEntry;
  COMPONENT_ENTRY   *NextEntry;
  UINT32             Index;

  if (ComponentName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Search container header from cache
  Status = EFI_NOT_FOUND;
  ContainerEntry = GetContainerBySignature (ContainerSig);
  if (ContainerEntry == NULL) {
    return Status;
  }

  ContainerHdr = (CONTAINER_HDR *)ContainerEntry->HeaderCache;
  if (ContainerHdr->Count == 0) {
    return Status;
  }

  CurrEntry = (COMPONENT_ENTRY *)&ContainerHdr[1];
  NextEntry = (COMPONENT_ENTRY *)((UINT8 *)(CurrEntry + 1) + CurrEntry->HashSize);
  for (Index = 0; Index < (UINT32)(ContainerHdr->Count - 1); Index++) {
    if ((CurrEntry->Attribute & COMPONENT_ENTRY_ATTR_RESERVED) == 0) {
      if (*ComponentName == 0) {
        *ComponentName = CurrEntry->Name;
        Status = EFI_SUCCESS;
        break;
      } else if (*ComponentName == CurrEntry->Name) {
        if ((NextEntry->Attribute & COMPONENT_ENTRY_ATTR_RESERVED) == 0) {
          *ComponentName = NextEntry->Name;
          Status = EFI_SUCCESS;
          break;
        } else {
          NextEntry = (COMPONENT_ENTRY *)((UINT8 *)(NextEntry + 1) + NextEntry->HashSize);
          continue;
        }
      }
    }
    CurrEntry = NextEntry;
    NextEntry = (COMPONENT_ENTRY *)((UINT8 *)(CurrEntry + 1) + CurrEntry->HashSize);
  }

  return Status;
}

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
  )
{
  EFI_STATUS                Status;
  CONTAINER_HDR            *ContainerHdr;
  CONTAINER_ENTRY          *ContainerEntry;
  COMPONENT_ENTRY          *CompEntry;

  if (ContainerSig < COMP_TYPE_INVALID) {
    // It is a component type, so get the info from flash map
    Status = GetComponentInfo (ComponentName, (UINT32 *)Buffer, Length);
    return EFI_NOT_FOUND;
  }

  Status = LocateComponentEntry (ContainerSig, ComponentName, &ContainerEntry, &CompEntry);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ContainerHdr = (CONTAINER_HDR *)ContainerEntry->HeaderCache;
  if (Buffer != NULL) {
    *Buffer = (VOID *)(ContainerEntry->Base + ContainerHdr->DataOffset + CompEntry->Offset);
  }
  if (Length != NULL) {
    *Length = CompEntry->Size;
  }

  return Status;
}

/**
  Load a component from a container or flahs map to memory and call callback
  function at predefined point.

  @param[in]     ContainerSig    Container signature or component type.
  @param[in]     ComponentName   Component name.
  @param[in,out] Buffer          Pointer to receive component base.
  @param[in,out] Length          Pointer to receive component size.
  @param[in,out] LoadComponentCallback  Callback function pointer.

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
  )
{
  EFI_STATUS                Status;
  LOADER_COMPRESSED_HEADER *CompressHdr;
  CONTAINER_HDR            *ContainerHdr;
  CONTAINER_ENTRY          *ContainerEntry;
  COMPONENT_ENTRY          *CompEntry;
  UINT8                    *CompData;
  UINT8                    *CompBuf;
  UINT8                    *HashData;
  VOID                     *CompBase;
  VOID                     *ScrBuf;
  VOID                     *AllocBuf;
  VOID                     *ReqCompBase;
  UINT8                     CompType;
  UINT8                     AuthType;
  UINT32                    DecompressedLen;
  UINT32                    CompLen;
  UINT32                    AllocLen;
  UINT32                    SignedDataLen;
  UINT32                    DstLen;
  UINT32                    ScrLen;
  BOOLEAN                   IsInFlash;

  if (ContainerSig < COMP_TYPE_INVALID) {
    // Check if it is container signature or component type
    CompType     = (UINT8)ContainerSig;
    ContainerSig = 0;

    Status = GetComponentInfo (ComponentName, (UINT32 *)&CompData,  &CompLen);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    if (FeaturePcdGet (PcdVerifiedBootEnabled)) {
      AuthType = AUTH_TYPE_SHA2_256;
    } else {
      AuthType = AUTH_TYPE_NONE;
    }
    HashData = NULL;
  } else {
    // Find the component info
    Status = LocateComponentEntry (ContainerSig, ComponentName, &ContainerEntry, &CompEntry);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if ((CompEntry->Attribute & COMPONENT_ENTRY_ATTR_RESERVED) != 0) {
      return EFI_UNSUPPORTED;
    }

    // Collect component info
    ContainerHdr = (CONTAINER_HDR *)ContainerEntry->HeaderCache;
    AuthType  = CompEntry->AuthType;
    HashData  = CompEntry->HashData;
    CompType  = COMP_TYPE_INVALID;
    CompData  = (UINT8 *)(ContainerEntry->Base + ContainerHdr->DataOffset + CompEntry->Offset);
    CompLen   = CompEntry->Size;
  }

  if (LoadComponentCallback != NULL) {
    LoadComponentCallback (PROGESS_ID_LOCATE);
  }

  // Component must have LOADER_COMPRESSED_HEADER
  Status = EFI_UNSUPPORTED;
  CompressHdr  = (LOADER_COMPRESSED_HEADER *)CompData;
  if (IS_COMPRESSED (CompressHdr)) {
    SignedDataLen = sizeof (LOADER_COMPRESSED_HEADER) + CompressHdr->CompressedSize;
    if (SignedDataLen <= CompLen) {
      Status = DecompressGetInfo (CompressHdr->Signature, CompressHdr->Data,
                                  CompressHdr->CompressedSize, &DstLen, &ScrLen);
    }
  }
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  // If it is required to use an existing buffer, verify the size
  ReqCompBase = NULL;
  DecompressedLen = CompressHdr->Size;
  if ((Buffer != NULL) && (*Buffer != NULL)) {
    ReqCompBase = *Buffer;
    if ((Length != NULL) && (*Length != 0) && (*Length < DecompressedLen)) {
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  // If it is on flash, the data needs to be copied into memory first
  // before authentication for security concern.
  IsInFlash = IS_FLASH_ADDRESS (CompData);
  AllocLen  = ScrLen + TEMP_BUF_ALIGN * 2;
  if (IsInFlash) {
    AllocLen += SignedDataLen;
  }
  AllocBuf = AllocateTemporaryMemory (AllocLen);
  if (AllocBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  if (IsInFlash) {
    // Authenticate component and decompress it if required
    CompBuf = AllocBuf;
    ScrBuf  = (UINT8 *)AllocBuf + ALIGN_UP (SignedDataLen, TEMP_BUF_ALIGN);
    CopyMem (CompBuf, CompData, SignedDataLen);
    if (LoadComponentCallback != NULL) {
      LoadComponentCallback (PROGESS_ID_COPY);
    }
  } else {
    CompBuf = CompData;
    ScrBuf  = AllocBuf;
  }

  // Verify the component
  Status = AuthenticateComponent (CompBuf, SignedDataLen, AuthType,
             CompData + ALIGN_UP(SignedDataLen, AUTH_DATA_ALIGN),  HashData, CompType);
  if (LoadComponentCallback != NULL) {
    LoadComponentCallback (PROGESS_ID_AUTHENTICATE);
  }
  if (!EFI_ERROR (Status)) {
    CompressHdr = (LOADER_COMPRESSED_HEADER *)CompBuf;
    if (ReqCompBase == NULL) {
      CompBase = AllocatePages (EFI_SIZE_TO_PAGES ((UINTN) DecompressedLen));
    } else {
      CompBase = ReqCompBase;
    }
    if (CompBase != NULL) {
      Status = Decompress (CompressHdr->Signature, CompressHdr->Data, CompressHdr->CompressedSize,
                           CompBase, ScrBuf);
      if (LoadComponentCallback != NULL) {
        LoadComponentCallback (PROGESS_ID_DECOMPRESS);
      }
      if (EFI_ERROR (Status)) {
        if (ReqCompBase == NULL) {
          FreePages (CompBase, EFI_SIZE_TO_PAGES ((UINTN) DecompressedLen));
        }
      }
    } else {
      Status = EFI_OUT_OF_RESOURCES;
    }

  } else {
    Status = EFI_SECURITY_VIOLATION;
  }
  FreeTemporaryMemory (AllocBuf);

  if (!EFI_ERROR (Status)) {
    if (Buffer != NULL) {
      *Buffer = CompBase;
    }
    if (Length != NULL) {
      *Length = DecompressedLen;
    }
  }

  return Status;
}


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
  )
{
  return LoadComponentWithCallback (ContainerSig, ComponentName, Buffer, Length, NULL);
}
