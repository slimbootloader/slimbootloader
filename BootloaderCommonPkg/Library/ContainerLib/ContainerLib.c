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

/**
  This function registers a container.

  @param[in]  ContainerBase      Container base address to register.

  @retval EFI_NOT_READY          Not ready for register yet.
  @retval EFI_BUFFER_TOO_SMALL   Insufficant max container entry number.
  @retval EFI_OUT_OF_RESOURCES   No space to add new container.
  @retval EFI_SUCCESS            The container has been registered successfully.

**/
STATIC
EFI_STATUS
RegisterContainer (
  IN UINT32    ContainerBase
  )
{
  CONTAINER_LIST       *ContainerList;
  CONTAINER_HDR        *ContainerHdr;
  UINT32                Index;
  VOID                 *Buffer;

  ContainerList = (CONTAINER_LIST *)GetContainerListPtr ();
  if (ContainerList == NULL) {
    return EFI_NOT_READY;
  }

  Index = ContainerList->Count;
  if (Index >= PcdGet32 (PcdContainerMaxNumber)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  ContainerHdr = (CONTAINER_HDR *)ContainerBase;
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
  This function unregisters a container in the last entry.

  @retval EFI_NOT_READY          Not ready for unregister yet.
  @retval EFI_NOT_FOUND          Not container available for unregisteration.
  @retval EFI_SUCCESS            The container has been unregistered successfully.

**/
STATIC
EFI_STATUS
UnregisterLastContainer (
  VOID
  )
{
  EFI_STATUS            Status;
  CONTAINER_LIST       *ContainerList;
  UINT32                Index;

  ContainerList = (CONTAINER_LIST *)GetContainerListPtr ();
  if (ContainerList == NULL) {
    return EFI_NOT_READY;
  }

  Index = ContainerList->Count;
  if (Index > 0) {
    Index--;
    FreePool ((VOID *)ContainerList->Entry[Index].HeaderCache);
    ContainerList->Count = Index;
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_NOT_FOUND;
  }

  return Status;
}

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
  This function returns the container header size.

  @param[in] ContainerEntry    Container entry pointer.
  @param[in] ComponentName     Component name in container.

  @retval         Container header size.

**/
STATIC
COMPONENT_ENTRY  *
LocateComponentEntry (
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
  return Status;
}

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
  )
{
  EFI_STATUS                Status;
  CONTAINER_HDR            *ContainerHdr;
  CONTAINER_ENTRY          *ContainerEntry;
  COMPONENT_ENTRY          *CompEntry;
  UINT8                    *AuthData;
  UINT32                    ContainerBase;
  UINT32                    ContainerSize;
  UINT32                    ContainerHdrSize;
  UINT8                     AuthType;

  // Search container header from cache
  ContainerEntry = GetContainerBySignature (ContainerSig);
  if (ContainerEntry == NULL) {
    // Location container from Flash Map
    Status = GetComponentInfo (ContainerSig, &ContainerBase, &ContainerSize);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    // Register container temporarily
    DEBUG ((EFI_D_INFO, "Register container 0x%08X\n", ContainerSig));
    Status = RegisterContainer (ContainerBase);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    // Find authentication data offset and authenticate the container header
    ContainerEntry   = GetContainerBySignature (ContainerSig);
    ContainerHdr     = (CONTAINER_HDR *)ContainerEntry->HeaderCache;
    ContainerHdrSize = GetContainerHeaderSize (ContainerHdr);
    if (ContainerHdrSize > 0) {
      AuthType = ContainerHdr->AuthType;
      AuthData = (UINT8 *)ContainerHdr + ALIGN_UP(ContainerHdrSize, AUTH_DATA_ALIGN);
      if ((AuthType == AUTH_TYPE_NONE) && FeaturePcdGet (PcdVerifiedBootEnabled)) {
        Status = EFI_SECURITY_VIOLATION;
      } else {
        Status = AuthenticateComponent ((UINT8 *)ContainerHdr, ContainerHdrSize,
                                             AuthType, AuthData, NULL, COMP_TYPE_PUBKEY_FWU);
      }
    } else {
      Status   = EFI_UNSUPPORTED;
    }

    if (EFI_ERROR (Status)) {
      // Unregister the container since authentication failed
      UnregisterLastContainer ();
      return EFI_SECURITY_VIOLATION;
    }
  }

  // Locate the component from the container header
  ContainerHdr = (CONTAINER_HDR *)ContainerEntry->HeaderCache;
  CompEntry = LocateComponentEntry (ContainerHdr, ComponentName);
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
  )
{
  EFI_STATUS                Status;
  CONTAINER_HDR            *ContainerHdr;
  CONTAINER_ENTRY          *ContainerEntry;
  COMPONENT_ENTRY          *CompEntry;

  Status = LocateComponent (ContainerSig, ComponentName, &ContainerEntry, &CompEntry);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ContainerHdr = (CONTAINER_HDR *)ContainerEntry->HeaderCache;
  if (RegionBase != NULL) {
    *RegionBase = ContainerEntry->Base + ContainerHdr->DataOffset + CompEntry->Offset;
  }
  if (RegionLength != NULL) {
    *RegionLength = CompEntry->Size;
  }

  return Status;
}

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
  VOID                     *ReqCompBase;
  UINT8                     CompType;
  UINT8                     AuthType;
  UINT32                    DecompressedLen;
  UINT32                    CompLen;
  UINT32                    SignedDataLen;
  UINT32                    DstLen;
  UINT32                    ScrLen;

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
    Status = LocateComponent (ContainerSig, ComponentName, &ContainerEntry, &CompEntry);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    // Collect component info
    ContainerHdr = (CONTAINER_HDR *)ContainerEntry->HeaderCache;
    CompData  = (UINT8 *)(ContainerEntry->Base + ContainerHdr->DataOffset + CompEntry->Offset);
    CompLen   = CompEntry->Size;
    AuthType  = CompEntry->AuthType;
    HashData  = CompEntry->HashData;
    CompType  = COMP_TYPE_INVALID;
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

  // Allocate temporary buffer since decompression is required.
  CompBuf = AllocateTemporaryMemory (SignedDataLen + ScrLen + TEMP_BUF_ALIGN * 2);
  if (CompBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Authenticate component and decompress it if required
  CopyMem (CompBuf, CompData, SignedDataLen);
  Status = AuthenticateComponent (CompBuf, SignedDataLen, AuthType,
             CompData + ALIGN_UP(SignedDataLen, AUTH_DATA_ALIGN),  HashData, CompType);
  if (!EFI_ERROR (Status)) {
    CompressHdr = (LOADER_COMPRESSED_HEADER *)CompBuf;
    if (ReqCompBase == NULL) {
      CompBase = AllocatePool (DecompressedLen);
    } else {
      CompBase = ReqCompBase;
    }
    if (CompBase != NULL) {
      Status = Decompress (CompressHdr->Signature, CompressHdr->Data, CompressHdr->CompressedSize,
                           CompBase, (VOID *)(CompBuf + ALIGN_UP (SignedDataLen, TEMP_BUF_ALIGN)));
      if (EFI_ERROR (Status)) {
        if (ReqCompBase == NULL) {
          FreePool (CompBase);
        }
      }
    } else {
      Status = EFI_OUT_OF_RESOURCES;
    }

  } else {
    Status = EFI_SECURITY_VIOLATION;
  }
  FreeTemporaryMemory (CompBuf);

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
