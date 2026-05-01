/** @file
  Container library implementation.

  Copyright (c) 2019 - 2023, Intel Corporation. All rights reserved.<BR>
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

STATIC
BOOLEAN
GetNextComponentEntryBounded (
  IN  CONTAINER_HDR     *ContainerHdr,
  IN  COMPONENT_ENTRY   *CurrEntry,
  OUT COMPONENT_ENTRY   **NextEntry
  );

STATIC
BOOLEAN
IsComponentEntryReadable (
  IN  CONTAINER_HDR     *ContainerHdr,
  IN  COMPONENT_ENTRY   *CurrEntry
  );

STATIC
BOOLEAN
GetComponentDataPointer (
  IN  CONTAINER_ENTRY   *ContainerEntry,
  IN  CONTAINER_HDR     *ContainerHdr,
  IN  COMPONENT_ENTRY   *CompEntry,
  OUT UINT8             **CompData
  );

STATIC
BOOLEAN
ValidateContainerBounds (
  IN  CONTAINER_HDR     *ContainerHdr,
  IN  UINT32             ContainerSize
  );

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
  INTN                  Offset;
  UINT32                Index;
  COMPONENT_ENTRY      *CompEntry;

  Offset = 0;
  if (ContainerHdr != NULL) {
    CompEntry   = (COMPONENT_ENTRY *)&ContainerHdr[1];
    if(ContainerHdr->Count > MAX_CONTAINER_SUB_IMAGE) {
      return 0;
    }
    for (Index = 0; Index < ContainerHdr->Count; Index++) {
      if (!GetNextComponentEntryBounded (ContainerHdr, CompEntry, &CompEntry)) {
        Offset = 0;
        break;
      }
      Offset = (UINT8 *)CompEntry - (UINT8 *)ContainerHdr;
      if ((Offset < 0) || (Offset > ContainerHdr->DataOffset)) {
        Offset = 0;
        break;
      }
    }
  }

  return (UINT32)Offset;
}

/**
  Get the next component entry while enforcing container-header bounds.

  @param[in]  ContainerHdr   Container header pointer.
  @param[in]  CurrEntry      Current component entry pointer.
  @param[out] NextEntry      Next component entry pointer.

  @retval TRUE               Next entry is valid and within DataOffset.
  @retval FALSE              Next entry would exceed header bounds.
**/
STATIC
BOOLEAN
GetNextComponentEntryBounded (
  IN  CONTAINER_HDR     *ContainerHdr,
  IN  COMPONENT_ENTRY   *CurrEntry,
  OUT COMPONENT_ENTRY   **NextEntry
  )
{
  UINT64  HeaderLimit;
  UINT64  CurrOffset;
  UINT64  NextOffset;

  if ((ContainerHdr == NULL) || (CurrEntry == NULL) || (NextEntry == NULL)) {
    return FALSE;
  }

  // Validate current entry fixed header before reading HashSize.
  if (!IsComponentEntryReadable (ContainerHdr, CurrEntry)) {
    return FALSE;
  }

  HeaderLimit = ContainerHdr->DataOffset;
  CurrOffset  = (UINT64)((UINT8 *)CurrEntry - (UINT8 *)ContainerHdr);

  NextOffset = CurrOffset + sizeof (COMPONENT_ENTRY) + CurrEntry->HashSize;
  if (NextOffset > HeaderLimit) {
    return FALSE;
  }

  *NextEntry = (COMPONENT_ENTRY *)((UINT8 *)ContainerHdr + (UINTN)NextOffset);
  return TRUE;
}

/**
  Check that the fixed-size portion of a component entry is readable within
  the container header cache.

  @param[in]  ContainerHdr   Container header pointer.
  @param[in]  CurrEntry      Current component entry pointer.

  @retval TRUE               COMPONENT_ENTRY fixed header is readable.
  @retval FALSE              Entry starts outside DataOffset bounds.
**/
STATIC
BOOLEAN
IsComponentEntryReadable (
  IN  CONTAINER_HDR     *ContainerHdr,
  IN  COMPONENT_ENTRY   *CurrEntry
  )
{
  UINT64  HeaderLimit;
  UINT64  CurrOffset;

  if ((ContainerHdr == NULL) || (CurrEntry == NULL)) {
    return FALSE;
  }

  HeaderLimit = ContainerHdr->DataOffset;
  CurrOffset  = (UINT64)((UINT8 *)CurrEntry - (UINT8 *)ContainerHdr);
  if (CurrOffset > HeaderLimit) {
    return FALSE;
  }

  if ((HeaderLimit - CurrOffset) < sizeof (COMPONENT_ENTRY)) {
    return FALSE;
  }

  return TRUE;
}

/**
  Resolve component data pointer with overflow and range validation.

  @param[in]  ContainerEntry  Container entry metadata.
  @param[in]  ContainerHdr    Container header pointer.
  @param[in]  CompEntry       Component entry pointer.
  @param[out] CompData        Resolved component data pointer.

  @retval TRUE                Component data address is valid.
  @retval FALSE               Component metadata/address is invalid.
**/
STATIC
BOOLEAN
GetComponentDataPointer (
  IN  CONTAINER_ENTRY   *ContainerEntry,
  IN  CONTAINER_HDR     *ContainerHdr,
  IN  COMPONENT_ENTRY   *CompEntry,
  OUT UINT8             **CompData
  )
{
  UINT64  CompAddr;

  if ((ContainerEntry == NULL) || (ContainerHdr == NULL) || (CompEntry == NULL) || (CompData == NULL)) {
    return FALSE;
  }

  if ((CompEntry->Offset > ContainerHdr->DataSize) ||
      (CompEntry->Size > (ContainerHdr->DataSize - CompEntry->Offset))) {
    return FALSE;
  }

  CompAddr = (UINT64)ContainerEntry->Base + (UINT64)ContainerHdr->DataOffset + (UINT64)CompEntry->Offset;
  if (CompAddr > MAX_UINT32) {
    return FALSE;
  }

  *CompData = (UINT8 *)(UINTN)(UINT32)CompAddr;
  return TRUE;
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
  IN UINT32    ContainerBase,
  IN UINT32    ContainerSize
  )
{
  CONTAINER_LIST       *ContainerList;
  CONTAINER_HDR        *ContainerHdr;
  CONTAINER_ENTRY      *ContainerEntry;
  UINT32                Index;
  VOID                 *Buffer;
  UINT32                MaxHdrSize;

  if (ContainerSize == 0) {
    DEBUG ((DEBUG_ERROR, "Unknown container size not allowed\n"));
    return EFI_SECURITY_VIOLATION;
  }

  ContainerList = (CONTAINER_LIST *)GetContainerListPtr ();
  if (ContainerList == NULL) {
    return EFI_NOT_READY;
  }

  ContainerHdr   = (CONTAINER_HDR *)(UINTN)ContainerBase;
  if (!ValidateContainerBounds (ContainerHdr, ContainerSize)) {
    return EFI_SECURITY_VIOLATION;
  }

  ContainerEntry = GetContainerBySignature (ContainerHdr->Signature);
  if (ContainerEntry != NULL) {
    return EFI_UNSUPPORTED;
  }

  Index = ContainerList->Count;
  if (Index >= PcdGet32 (PcdContainerMaxNumber)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  MaxHdrSize = GetContainerHeaderSize (ContainerHdr) + SIGNATURE_AND_KEY_SIZE_MAX;
  if (MaxHdrSize > ContainerHdr->DataOffset) {
    MaxHdrSize = ContainerHdr->DataOffset;
  }

  if (MaxHdrSize > ContainerSize) {
    DEBUG ((DEBUG_ERROR, "Invalid container header cache size (0x%x) exceeds container region (0x%x)\n",
            MaxHdrSize, ContainerSize));
    return EFI_SECURITY_VIOLATION;
  }

  Buffer  = AllocatePool (MaxHdrSize);
  if (Buffer == NULL) {
    return  EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((DEBUG_INFO, "Registering container %4a\n", &ContainerHdr->Signature));
  ContainerList->Entry[Index].Signature   = ContainerHdr->Signature;
  ContainerList->Entry[Index].HeaderCache = (UINT32)(UINTN)Buffer;
  ContainerList->Entry[Index].HeaderSize  = MaxHdrSize ;
  ContainerList->Entry[Index].Base        = ContainerBase;
  CopyMem (Buffer, (VOID *)(UINTN)ContainerBase, MaxHdrSize);
  ContainerList->Count++;

  return EFI_SUCCESS;
}

/**
  Validate container header ranges against known container region size.

  @param[in]  ContainerHdr    Container header pointer.
  @param[in]  ContainerSize   Container region size in bytes.

  @retval TRUE                Header ranges are within container region.
  @retval FALSE               Header ranges are invalid.
**/
STATIC
BOOLEAN
ValidateContainerBounds (
  IN  CONTAINER_HDR     *ContainerHdr,
  IN  UINT32             ContainerSize
  )
{
  if ((ContainerHdr == NULL) || (ContainerSize < sizeof (CONTAINER_HDR))) {
    return FALSE;
  }

  if (ContainerHdr->DataOffset < sizeof (CONTAINER_HDR)) {
    return FALSE;
  }

  if (ContainerHdr->DataOffset > ContainerSize) {
    return FALSE;
  }

  if (ContainerHdr->DataSize > (ContainerSize - ContainerHdr->DataOffset)) {
    return FALSE;
  }

  return TRUE;
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
    FreePool ((VOID *)(UINTN)ContainerList->Entry[Index].HeaderCache);
    ContainerList->Entry[Index] = ContainerList->Entry[LastIndex];
    ContainerList->Count--;
    Status = EFI_SUCCESS;
  }

  return Status;
}

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
  )
{
  UINT32                Index;
  COMPONENT_ENTRY      *CurrEntry;

  if ((ContainerHdr == NULL) || (ContainerHdr->Count == 0) || (ContainerHdr->Count > MAX_CONTAINER_SUB_IMAGE)) {
    return NULL;
  }

  CurrEntry = (COMPONENT_ENTRY *)&ContainerHdr[1];
  for (Index = 0; Index < ContainerHdr->Count; Index++) {
    if (!IsComponentEntryReadable (ContainerHdr, CurrEntry)) {
      DEBUG ((DEBUG_ERROR, "Malformed container header while traversing component list\n"));
      return NULL;
    }

    if (CurrEntry->Name == ComponentName) {
      return CurrEntry;
    }

    if (Index == (((UINT32)ContainerHdr->Count) - 1U)) {
      break;
    }

    if (!GetNextComponentEntryBounded (ContainerHdr, CurrEntry, &CurrEntry)) {
      DEBUG ((DEBUG_ERROR, "Malformed container header while traversing component list\n"));
      return NULL;
    }
  }

  return NULL;
}

/**
  This function returns the hash alg type from auth type.

  @param[in] AuthType    Authorization Type.

  @retval         Hash Algorithm Type.

**/
HASH_ALG_TYPE
GetHashAlg(
  AUTH_TYPE AuthType
  )
{
  HASH_ALG_TYPE HashAlg;

  HashAlg = HASH_TYPE_NONE;

  if((AuthType == AUTH_TYPE_SIG_RSA2048_PKCSI1_SHA256)
                  || (AuthType == AUTH_TYPE_SIG_RSA2048_PSS_SHA256) || (AuthType == AUTH_TYPE_SHA2_256)) {
    HashAlg = HASH_TYPE_SHA256;
  } else if ((AuthType == AUTH_TYPE_SIG_RSA3072_PKCSI1_SHA384)
            || (AuthType == AUTH_TYPE_SIG_RSA3072_PSS_SHA384) || (AuthType == AUTH_TYPE_SHA2_384)) {
    HashAlg = HASH_TYPE_SHA384;
  }

  return HashAlg;
}

/**
  Authenticate a container header or component.

  @param[in] Data         Data buffer to be authenticated.
  @param[in] Length       Data length to be authenticated.
  @param[in] AuthType     Authentication type.
  @param[in] AuthData     Authentication data buffer.
  @param[in] AuthDataLen  Authentication data length.
  @param[in] HashData     Hash data buffer.
  @param[in] Usage        Hash usage.

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
  IN  UINT32    AuthDataLen,
  IN  UINT8    *HashData,
  IN  UINT32    Usage
  )
{
  EFI_STATUS      Status;
  UINT8          *KeyPtr;
  SIGNATURE_HDR  *SignHdr;
  PUB_KEY_HDR    *PubKeyHdr;
  UINT16          ExpectedSigSize;
  UINT16          ExpectedKeySize;
  UINT64          SigBlockSize;
  UINT64          AuthBlockSize;

  if (!FeaturePcdGet (PcdVerifiedBootEnabled)) {
    Status = EFI_SUCCESS;
  } else {
    if (AuthType == AUTH_TYPE_SHA2_256) {
      Status = DoHashVerify (Data, Length, Usage, HASH_TYPE_SHA256, HashData);
    } else if (AuthType == AUTH_TYPE_SHA2_384) {
      Status = DoHashVerify (Data, Length, Usage, HASH_TYPE_SHA384, HashData);
    } else if ((AuthType == AUTH_TYPE_SIG_RSA2048_PKCSI1_SHA256) || ( AuthType == AUTH_TYPE_SIG_RSA3072_PKCSI1_SHA384)
           || (AuthType == AUTH_TYPE_SIG_RSA2048_PSS_SHA256) || ( AuthType == AUTH_TYPE_SIG_RSA3072_PSS_SHA384)) {
      // Derive expected fixed signature/key sizes from AuthType.
      if ((AuthType == AUTH_TYPE_SIG_RSA2048_PKCSI1_SHA256) || (AuthType == AUTH_TYPE_SIG_RSA2048_PSS_SHA256)) {
        ExpectedSigSize = RSA2048_NUMBYTES;
        ExpectedKeySize = RSA2048_MOD_SIZE + RSA_E_SIZE;
      } else {
        ExpectedSigSize = RSA3072_NUMBYTES;
        ExpectedKeySize = RSA3072_MOD_SIZE + RSA_E_SIZE;
      }

      // AuthData comes from untrusted container content. Validate boundaries
      // before reading SignHdr/PubKeyHdr fields or advancing pointers.
      if ((AuthData == NULL) || (AuthDataLen < (sizeof (SIGNATURE_HDR) + sizeof (PUB_KEY_HDR)))) {
        DEBUG ((DEBUG_ERROR, "Invalid auth data: too small for signature/public-key headers\n"));
        return EFI_SECURITY_VIOLATION;
      }

      SignHdr = (SIGNATURE_HDR *)AuthData;
      if (SignHdr->SigSize != ExpectedSigSize) {
        DEBUG ((DEBUG_ERROR, "Invalid signature size: got 0x%x expected 0x%x\n", SignHdr->SigSize, ExpectedSigSize));
        return EFI_SECURITY_VIOLATION;
      }

      SigBlockSize = (UINT64)sizeof (SIGNATURE_HDR) + (UINT64)SignHdr->SigSize;
      if (SigBlockSize > AuthDataLen) {
        DEBUG ((DEBUG_ERROR, "Invalid auth data: signature block exceeds auth-data length\n"));
        return EFI_SECURITY_VIOLATION;
      }

      if ((AuthDataLen - SigBlockSize) < sizeof (PUB_KEY_HDR)) {
        DEBUG ((DEBUG_ERROR, "Invalid auth data: insufficient space for public-key header\n"));
        return EFI_SECURITY_VIOLATION;
      }

      KeyPtr    = AuthData + (UINTN)SigBlockSize;
      PubKeyHdr = (PUB_KEY_HDR *)KeyPtr;
      if (PubKeyHdr->KeySize != ExpectedKeySize) {
        DEBUG ((DEBUG_ERROR, "Invalid public key size: got 0x%x expected 0x%x\n", PubKeyHdr->KeySize, ExpectedKeySize));
        return EFI_SECURITY_VIOLATION;
      }

      AuthBlockSize = SigBlockSize + sizeof (PUB_KEY_HDR) + (UINT64)PubKeyHdr->KeySize;
      if (AuthBlockSize > AuthDataLen) {
        DEBUG ((DEBUG_ERROR, "Invalid auth data: public-key block exceeds auth-data length\n"));
        return EFI_SECURITY_VIOLATION;
      }

      Status   = DoRsaVerify (Data, Length, Usage, SignHdr,
                             PubKeyHdr, GetHashAlg(AuthType), HashData, NULL);
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
UINT32
GetContainerKeyUsageBySig (
  IN  UINT32    ContainerSig
  )
{
  UINT8  Idx;

  if (ContainerSig == CONTAINER_KEY_HASH_STORE_SIGNATURE) {
    return HASH_USAGE_PUBKEY_MASTER;
  } else if (ContainerSig == CONTAINER_BOOT_SIGNATURE) {
    return HASH_USAGE_PUBKEY_OS;
  } else if ((ContainerSig & 0x00FFFFFF) == CONTAINER_OEM_BASE_SIGNATURE) {
    Idx = (ContainerSig >> 24) - '0';
    if (Idx < 8) {
      return HASH_USAGE_PUBKEY_OEM (Idx);
    }
  }

  return HASH_USAGE_PUBKEY_CONTAINER_DEF;
}

/**
  This function authenticates a container

  @param[in]  ContainerHeader    Container base address to register.
  @param[in]  ContainerCallback  Callback regsiterd to notify container buf info

  @retval EFI_SUCCESS            The container has been authenticated successfully.
  @retval EFI_UNSUPPORTED        If container header is invalid or autheication fails

**/
EFI_STATUS
AuthenticateContainerInternal (
  IN  CONTAINER_HDR            *ContainerHeader,
  IN  LOAD_COMPONENT_CALLBACK  ContainerCallback
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
  UINT64                    SignedDataLen64;
  UINT32                    AuthDataOffset;
  UINT32                    AuthDataLen;
  UINT32                    Index;
  LOADER_COMPRESSED_HEADER *CompressHdr;
  EFI_STATUS                Status;
  COMPONENT_CALLBACK_INFO   CbInfo;
  UINT64                    SignatureBuf;

  // Find authentication data offset and authenticate the container header
  Status = EFI_UNSUPPORTED;
  ContainerEntry   = GetContainerBySignature (ContainerHeader->Signature);
  if (ContainerEntry != NULL) {
    ContainerHdr     = (CONTAINER_HDR *)(UINTN)ContainerEntry->HeaderCache;
    ContainerHdrSize = GetContainerHeaderSize (ContainerHdr);
    if (ContainerHdrSize > 0) {
      AuthType = ContainerHdr->AuthType;
      AuthDataOffset = ALIGN_UP (ContainerHdrSize, AUTH_DATA_ALIGN);
      Status = EFI_SUCCESS;
      if ((AuthType == AUTH_TYPE_NONE) && FeaturePcdGet (PcdVerifiedBootEnabled)) {
        // Enforce header authentication if verified boot is enabled.
        Status = EFI_SECURITY_VIOLATION;
      }

      if (!EFI_ERROR (Status) && (AuthDataOffset > ContainerEntry->HeaderSize)) {
        DEBUG ((DEBUG_ERROR, "Invalid container header auth-data offset (0x%x) > header cache size (0x%x)\n",
                AuthDataOffset, ContainerEntry->HeaderSize));
        Status = EFI_SECURITY_VIOLATION;
      }

      if (!EFI_ERROR (Status)) {
        AuthData    = (UINT8 *)ContainerHdr + AuthDataOffset;
        AuthDataLen = ContainerEntry->HeaderSize - AuthDataOffset;
        Status = AuthenticateComponent ((UINT8 *)ContainerHdr, ContainerHdrSize,
                                        AuthType, AuthData, AuthDataLen, NULL,
                                        GetContainerKeyUsageBySig (ContainerHeader->Signature));
        if ((!EFI_ERROR (Status)) && (ContainerCallback != NULL)) {
          // Update component Call back info after container header authenticaton is done
          // This info will used by firmware stage to extend to TPM
          CbInfo.ComponentType    = ContainerHeader->Signature;
          CbInfo.CompBuf          = (UINT8 *)ContainerHdr;
          CbInfo.CompLen          = ContainerHdrSize;
          CbInfo.HashAlg          = GetHashAlg(AuthType);
          CbInfo.HashData         = NULL;
          ContainerCallback (PROGESS_ID_AUTHENTICATE, &CbInfo);
        }
      }
    }
  }

  if (!EFI_ERROR (Status)) {
    if ((ContainerHdr->Flags & CONTAINER_HDR_FLAG_MONO_SIGNING) != 0) {
      // Additional verification if the container is signed monolithically.
      // It is required for the container to be loaded in memory before registeration.
      Status = EFI_UNSUPPORTED;
      if ((ContainerHdr->Count > 1) && !IS_FLASH_ADDRESS (ContainerHeader)) {
        // Use the last entry to verify all other combined components
        CompEntry = (COMPONENT_ENTRY *)&ContainerHdr[1];
        for (Index = 0; Index < (UINT32)(ContainerHdr->Count - 1); Index++) {
          if (!GetNextComponentEntryBounded (ContainerHdr, CompEntry, &CompEntry)) {
            DEBUG ((DEBUG_ERROR, "Malformed monolithic container component list\n"));
            Status = EFI_SECURITY_VIOLATION;
            break;
          }
        }

        if (!EFI_ERROR (Status)) {
          if (!GetComponentDataPointer (ContainerEntry, ContainerHdr, CompEntry, &CompData)) {
            DEBUG ((DEBUG_ERROR, "Invalid monolithic component data bounds/address\n"));
            Status = EFI_SECURITY_VIOLATION;
          }
        }

        if (!EFI_ERROR (Status)) {
          if (CompEntry->Size < sizeof (LOADER_COMPRESSED_HEADER)) {
            DEBUG ((DEBUG_ERROR, "Invalid monolithic component: too small for compressed header (0x%x < 0x%x)\n",
                    CompEntry->Size, sizeof (LOADER_COMPRESSED_HEADER)));
            Status = EFI_SECURITY_VIOLATION;
          }
        }

        if (!EFI_ERROR (Status)) {
          CompressHdr = (LOADER_COMPRESSED_HEADER *)CompData;
          if (CompressHdr->Signature == LZDM_SIGNATURE) {
            if ((CompEntry->AuthType == AUTH_TYPE_NONE) && FeaturePcdGet (PcdVerifiedBootEnabled)) {
              // Enforce component authentication if verified boot is enabled.
              Status = EFI_SECURITY_VIOLATION;
            }

            if (!EFI_ERROR (Status)) {
              SignedDataLen64 = (UINT64)sizeof (LOADER_COMPRESSED_HEADER) + (UINT64)CompressHdr->CompressedSize;
              if (SignedDataLen64 > MAX_UINT32) {
                DEBUG ((DEBUG_ERROR, "Invalid monolithic component signed length overflow\n"));
                Status = EFI_SECURITY_VIOLATION;
              }
            }

            if (!EFI_ERROR (Status)) {
              SignedDataLen = (UINT32)SignedDataLen64;
              AuthDataOffset = ALIGN_UP (SignedDataLen, AUTH_DATA_ALIGN);
              if ((SignedDataLen > CompEntry->Size) || (AuthDataOffset > CompEntry->Size)) {
                DEBUG ((DEBUG_ERROR, "Invalid monolithic component auth-data bounds (CompSize=0x%x Signed=0x%x Offset=0x%x)\n",
                        CompEntry->Size, SignedDataLen, AuthDataOffset));
                Status = EFI_SECURITY_VIOLATION;
              }
            }

            if (!EFI_ERROR (Status)) {
              AuthData    = CompData + AuthDataOffset;
              AuthDataLen = CompEntry->Size - AuthDataOffset;
              if (((UINT64)ContainerEntry->Base + (UINT64)ContainerHdr->DataOffset) > MAX_UINT32) {
                DEBUG ((DEBUG_ERROR, "Invalid monolithic container data base address overflow\n"));
                Status = EFI_SECURITY_VIOLATION;
              }
            }

            if (!EFI_ERROR (Status)) {
              DataBuf = (UINT8 *)(UINTN)((UINT32)((UINT64)ContainerEntry->Base + (UINT64)ContainerHdr->DataOffset));
              // Validate that monolithic component offset is at least past the header.
              if (CompEntry->Offset < ContainerHdrSize) {
                DEBUG ((DEBUG_ERROR, "Invalid monolithic component offset (0x%x < header size 0x%x)\n",
                        CompEntry->Offset, ContainerHdrSize));
                Status = EFI_SECURITY_VIOLATION;
              } else {
                DataLen = CompEntry->Offset;
                Status  = AuthenticateComponent (DataBuf, DataLen, CompEntry->AuthType,
                                                AuthData, AuthDataLen, CompEntry->HashData, 0);
              }
            }

            if ((!EFI_ERROR (Status)) && (ContainerCallback != NULL)) {
              // Update component Call back info after authenticaton is done.
              CbInfo.ComponentType = ContainerHeader->Signature;
              CbInfo.CompBuf       = DataBuf;
              CbInfo.CompLen       = DataLen;
              CbInfo.HashAlg       = GetHashAlg (CompEntry->AuthType);
              CbInfo.HashData      = CompEntry->HashData;
              ContainerCallback (PROGESS_ID_AUTHENTICATE, &CbInfo);
            }
          }
        }
      }
    } else if (FeaturePcdGet (PcdVerifiedBootEnabled)) {
      // For non-Mono signing all components must have auth data when verified boot is enabled
      DEBUG((DEBUG_INFO, "Verify Container %4a AuthTypes\n", (CHAR8 *)&ContainerHdr->Signature));
      for (Index = 0 , CompEntry = (COMPONENT_ENTRY *)(ContainerHdr + 1); Index < ContainerHdr->Count; Index++) {
        if (!IsComponentEntryReadable (ContainerHdr, CompEntry)) {
          DEBUG ((DEBUG_ERROR, "Malformed container header while validating component auth types\n"));
          Status = EFI_SECURITY_VIOLATION;
          break;
        }

        SignatureBuf = CompEntry->Name;
        DEBUG((DEBUG_INFO, "Component %4a AuthType %X\n", (CHAR8 *)&SignatureBuf, CompEntry->AuthType));
        if (CompEntry->AuthType == AUTH_TYPE_NONE) {
          DEBUG((DEBUG_INFO, "All container content must be authenticated in verified boot flow.\n"));
          Status =  EFI_SECURITY_VIOLATION;
          break;
        }

        if (Index == (((UINT32)ContainerHdr->Count) - 1U)) {
          break;
        }

        if (!GetNextComponentEntryBounded (ContainerHdr, CompEntry, &CompEntry)) {
          DEBUG ((DEBUG_ERROR, "Malformed container header while validating component auth types\n"));
          Status = EFI_SECURITY_VIOLATION;
          break;
        }
      }
    }
  }

  return Status;
}

/**
  This function registers a container.

  @param[in]  ContainerBase      Container base address to register.
  @param[in]  ContainerSize      Container size in bytes, 0 if unknown.
  @param[in]  ContainerCallback  Callback registered to notify container buf info

  @retval EFI_NOT_READY          Not ready for register yet.
  @retval EFI_BUFFER_TOO_SMALL   Insufficant max container entry number.
  @retval EFI_OUT_OF_RESOURCES   No space to add new container.
  @retval EFI_SUCCESS            The container has been registered successfully.

**/
EFI_STATUS
RegisterContainer (
  IN  UINT32                    ContainerBase,
  IN  UINT32                    ContainerSize,
  IN  LOAD_COMPONENT_CALLBACK   ContainerCallback
  )
{
  EFI_STATUS                Status;
  EFI_STATUS                CompStatus;
  CONTAINER_HDR            *ContainerHdr;
  UINT32                    FlashCompBase;
  UINT32                    FlashCompSize;
  UINT32                    EffectiveContainerSize;
  UINT64                    SignatureBuffer;

  ContainerHdr     = (CONTAINER_HDR *)(UINTN)ContainerBase;
  SignatureBuffer  = ContainerHdr->Signature;
  DEBUG ((DEBUG_INFO, "Registering container %4a\n", (CHAR8 *)&SignatureBuffer));

  // Use caller-provided size for RAM-backed containers. If unknown (0),
  // best-effort derive it from flash map only when base addresses match.
  EffectiveContainerSize = ContainerSize;
  if (EffectiveContainerSize == 0) {
    CompStatus = GetComponentInfo (ContainerHdr->Signature, &FlashCompBase, &FlashCompSize);
    if (!EFI_ERROR (CompStatus) && (FlashCompBase == ContainerBase)) {
      EffectiveContainerSize = FlashCompSize;
    }
  }

  // Reject containers with unknown or unresolvable size to prevent pre-auth buffer overread.
  if (EffectiveContainerSize == 0) {
    DEBUG ((DEBUG_ERROR, "Unknown container size not allowed for security validation\n"));
    return EFI_SECURITY_VIOLATION;
  }

  // Register container
  Status = RegisterContainerInternal (ContainerBase, EffectiveContainerSize);
  if (!EFI_ERROR (Status)) {
    Status = AuthenticateContainerInternal (ContainerHdr, ContainerCallback);
    if (EFI_ERROR (Status)) {
      // Unregister the container since authentication failed
      UnregisterContainer (ContainerHdr->Signature);
    }
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Registering container failed due to %r\n", Status));
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

  CompEntry = NULL;

  // Search container header from cache
  ContainerEntry = GetContainerBySignature (ContainerSig);
  if (ContainerEntry == NULL) {
    // Location container from Flash Map
    Status = GetComponentInfo (ContainerSig, &ContainerBase, &ContainerSize);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    // Register container temporarily
    Status = RegisterContainerInternal (ContainerBase, ContainerSize);
    if (!EFI_ERROR (Status)) {
      Status = AuthenticateContainerInternal ((CONTAINER_HDR *)(UINTN)ContainerBase, NULL);
      if (EFI_ERROR (Status)) {
        UnregisterContainer (((CONTAINER_HDR *)(UINTN)ContainerBase)->Signature);
      }
    }
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
  ContainerHdr = (CONTAINER_HDR *)(UINTN)ContainerEntry->HeaderCache;
  if (ComponentName != 0) {
    CompEntry = LocateComponentEntryFromContainer (ContainerHdr, ComponentName);
    if (CompEntry == NULL) {
      return EFI_NOT_FOUND;
    }
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
  BOOLEAN            FoundCurrent;
  CONTAINER_HDR     *ContainerHdr;
  CONTAINER_ENTRY   *ContainerEntry;
  COMPONENT_ENTRY   *CurrEntry;
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

  ContainerHdr = (CONTAINER_HDR *)(UINTN)ContainerEntry->HeaderCache;
  if ((ContainerHdr->Count == 0) || (ContainerHdr->Count > MAX_CONTAINER_SUB_IMAGE)) {
    return Status;
  }

  FoundCurrent = FALSE;
  CurrEntry = (COMPONENT_ENTRY *)&ContainerHdr[1];
  for (Index = 0; Index < ContainerHdr->Count; Index++) {
    if (!IsComponentEntryReadable (ContainerHdr, CurrEntry)) {
      DEBUG ((DEBUG_ERROR, "Malformed container header while enumerating components\n"));
      return EFI_NOT_FOUND;
    }

    if ((CurrEntry->Attribute & COMPONENT_ENTRY_ATTR_RESERVED) == 0) {
      if (*ComponentName == 0) {
        *ComponentName = CurrEntry->Name;
        return EFI_SUCCESS;
      }

      if (FoundCurrent) {
        *ComponentName = CurrEntry->Name;
        return EFI_SUCCESS;
      }

      if (*ComponentName == CurrEntry->Name) {
        FoundCurrent = TRUE;
      }
    }

    if (Index == (((UINT32)ContainerHdr->Count) - 1U)) {
      break;
    }

    if (!GetNextComponentEntryBounded (ContainerHdr, CurrEntry, &CurrEntry)) {
      DEBUG ((DEBUG_ERROR, "Malformed container header while enumerating components\n"));
      return EFI_NOT_FOUND;
    }
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
  UINT8                    *CompData;
  CONTAINER_HDR            *ContainerHdr;
  CONTAINER_ENTRY          *ContainerEntry;
  COMPONENT_ENTRY          *CompEntry;

  if (ContainerSig < COMP_TYPE_INVALID) {
    // It is a component type, so get the info from flash map
    Status = GetComponentInfo (ComponentName, (UINT32 *)Buffer, Length);
    return Status;
  }

  Status = LocateComponentEntry (ContainerSig, ComponentName, &ContainerEntry, &CompEntry);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((ContainerEntry != NULL) && (CompEntry != NULL)) {
    ContainerHdr = (CONTAINER_HDR *)(UINTN)ContainerEntry->HeaderCache;
    if (Buffer != NULL) {
      if (!GetComponentDataPointer (ContainerEntry, ContainerHdr, CompEntry, &CompData)) {
        DEBUG ((DEBUG_ERROR, "Invalid component data bounds/address\n"));
        return EFI_SECURITY_VIOLATION;
      }
      *Buffer = (VOID *)CompData;
    }
    if (Length != NULL) {
      *Length = CompEntry->Size;
    }
  } else {
    Status = EFI_NOT_FOUND;
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
  UINT32                    Usage;
  UINT8                     AuthType;
  UINT32                    DecompressedLen;
  UINT32                    CompLen;
  UINT32                    CompLoc;
  UINT32                    AllocLen;
  UINT64                    AllocLen64;
  UINT32                    SignedDataLen;
  UINT64                    SignedDataLen64;
  UINT32                    AuthDataOffset;
  UINT32                    AuthDataLen;
  UINT32                    DstLen;
  UINT32                    ScrLen;
  BOOLEAN                   IsInFlash;
  COMPONENT_CALLBACK_INFO   CbInfo;
  UINT32                    ComponentId;
  UINT64                    ContainerIdBuf;
  UINT64                    ComponentIdBuf;

  ComponentId = ContainerSig;
  CompLoc = 0;

  ComponentIdBuf = ComponentName;
  if (ContainerSig < COMP_TYPE_INVALID) {
    ContainerIdBuf = FLASH_MAP_SIG_HEADER;
  } else {
    ContainerIdBuf = ContainerSig;
  }
  DEBUG ((DEBUG_INFO, "Loading Component %4a:%4a\n", (CHAR8 *)&ContainerIdBuf, (CHAR8 *)&ComponentIdBuf));

  if (ContainerSig < COMP_TYPE_INVALID) {
    // Check if it is component type
    Usage        =  1 << ContainerSig;
    ContainerSig = 0;
    Status = GetComponentInfo (ComponentName, &CompLoc, &CompLen);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
    CompData = (VOID *)(UINTN)CompLoc;
    if (FeaturePcdGet (PcdVerifiedBootEnabled)) {
      if(FixedPcdGet8(PcdCompSignHashAlg) == HASH_TYPE_SHA256) {
        AuthType = AUTH_TYPE_SHA2_256;
      } else if (FixedPcdGet8(PcdCompSignHashAlg) == HASH_TYPE_SHA384) {
        AuthType = AUTH_TYPE_SHA2_384;
      } else {
        return EFI_UNSUPPORTED;
      }
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

    if ((ContainerEntry == NULL) || (CompEntry == NULL)) {
      return EFI_NOT_FOUND;
    }

    if ((CompEntry->Attribute & COMPONENT_ENTRY_ATTR_RESERVED) != 0) {
      return EFI_UNSUPPORTED;
    }

    // Collect component info
    ContainerHdr = (CONTAINER_HDR *)(UINTN)ContainerEntry->HeaderCache;
    AuthType  = CompEntry->AuthType;
    HashData  = CompEntry->HashData;
    Usage     = 0;
    if (!GetComponentDataPointer (ContainerEntry, ContainerHdr, CompEntry, &CompData)) {
      DEBUG ((DEBUG_ERROR, "Invalid component data bounds/address\n"));
      return EFI_SECURITY_VIOLATION;
    }
    CompLen   = CompEntry->Size;
  }

  if (LoadComponentCallback != NULL) {
    LoadComponentCallback (PROGESS_ID_LOCATE, NULL);
  }

  // Component must have LOADER_COMPRESSED_HEADER
  if (CompLen < sizeof (LOADER_COMPRESSED_HEADER)) {
    DEBUG ((DEBUG_ERROR, "Invalid component: too small for compressed header (0x%x < 0x%x)\n",
            CompLen, sizeof (LOADER_COMPRESSED_HEADER)));
    return EFI_SECURITY_VIOLATION;
  }

  CompressHdr  = (LOADER_COMPRESSED_HEADER *)CompData;
  if (!IS_COMPRESSED (CompressHdr)) {
    return EFI_UNSUPPORTED;
  }

  // Validate signed data length (LOADER_COMPRESSED_HEADER + compressed payload)
  SignedDataLen64 = (UINT64)sizeof (LOADER_COMPRESSED_HEADER) + (UINT64)CompressHdr->CompressedSize;
  if (SignedDataLen64 > MAX_UINT32) {
    DEBUG ((DEBUG_ERROR, "Invalid signed component length overflow\n"));
    return EFI_SECURITY_VIOLATION;
  }

  SignedDataLen = (UINT32)SignedDataLen64;
  if (SignedDataLen > CompLen) {
    DEBUG ((DEBUG_ERROR, "Invalid signed component length (0x%x > 0x%x)\n", SignedDataLen, CompLen));
    return EFI_UNSUPPORTED;
  }

  // Handle zero-size components: no payload to decompress, skip DecompressGetInfo
  // entirely so ScrLen stays 0 and no scratch buffer is allocated.
  if (CompressHdr->Size == 0) {
    DstLen = 0;
    ScrLen = 0;
    Status = EFI_SUCCESS;
  } else {
    Status = DecompressGetInfo (CompressHdr->Signature, CompressHdr->Data,
                                CompressHdr->CompressedSize, &DstLen, &ScrLen);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    // Ensure the decompressor-reported output length matches the signed header.
    // This keeps destination sizing deterministic before decompression.
    if (DstLen != CompressHdr->Size) {
      DEBUG ((DEBUG_ERROR, "Invalid decompression length mismatch (Hdr=0x%x, Dst=0x%x)\n",
              CompressHdr->Size, DstLen));
      return EFI_SECURITY_VIOLATION;
    }
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
  AllocLen64 = (UINT64)ScrLen + ((UINT64)TEMP_BUF_ALIGN * 2);
  if (IsInFlash) {
    AllocLen64 += (UINT64)SignedDataLen;
  }
  if (AllocLen64 > MAX_UINT32) {
    DEBUG ((DEBUG_ERROR, "Invalid temporary allocation length overflow (Scr=0x%x Signed=0x%x)\n",
            ScrLen, SignedDataLen));
    return EFI_SECURITY_VIOLATION;
  }
  AllocLen = (UINT32)AllocLen64;

  AllocBuf = AllocateTemporaryMemory (AllocLen);
  if (AllocBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (IsInFlash) {
    // Authenticate component and decompress it if required
    if (SignedDataLen > AllocLen) {
      DEBUG ((DEBUG_ERROR, "Invalid copy length (Signed=0x%x Alloc=0x%x)\n", SignedDataLen, AllocLen));
      FreeTemporaryMemory (AllocBuf);
      return EFI_SECURITY_VIOLATION;
    }
    CompBuf = AllocBuf;
    ScrBuf  = (UINT8 *)AllocBuf + ALIGN_UP (SignedDataLen, TEMP_BUF_ALIGN);
    CopyMem (CompBuf, CompData, SignedDataLen);
    if (LoadComponentCallback != NULL) {
      LoadComponentCallback (PROGESS_ID_COPY, NULL);
    }
  } else {
    CompBuf = CompData;
    ScrBuf  = AllocBuf;
  }

  AuthDataOffset = ALIGN_UP (SignedDataLen, AUTH_DATA_ALIGN);
  if (AuthDataOffset > CompLen) {
    DEBUG ((DEBUG_ERROR, "Invalid component auth-data offset (0x%x) > component size (0x%x)\n",
            AuthDataOffset, CompLen));
    FreeTemporaryMemory (AllocBuf);
    return EFI_SECURITY_VIOLATION;
  }
  AuthDataLen = CompLen - AuthDataOffset;

  // Verify the component
  Status = AuthenticateComponent (CompBuf, SignedDataLen, AuthType,
             CompData + AuthDataOffset, AuthDataLen, HashData, Usage);
  if (LoadComponentCallback != NULL) {
    if(Status == EFI_SUCCESS){
      // Update component Call back info after authenticaton is done
      // This info will used by firmware stage to extend to TPM
      CbInfo.ComponentType    = ComponentId;
      CbInfo.CompBuf          = CompData;
      CbInfo.CompLen          = SignedDataLen;
      CbInfo.HashAlg          = GetHashAlg(AuthType);
      CbInfo.HashData         = HashData;
      LoadComponentCallback (PROGESS_ID_AUTHENTICATE, &CbInfo);
    } else {
      LoadComponentCallback (PROGESS_ID_AUTHENTICATE, NULL);
    }
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
        LoadComponentCallback (PROGESS_ID_DECOMPRESS, NULL);
      }
      if (EFI_ERROR (Status)) {
        if (ReqCompBase == NULL) {
          FreePages (CompBase, EFI_SIZE_TO_PAGES ((UINTN) DecompressedLen));
        }
      }
    } else {
      if (CompressHdr->Size == 0) {
        Status = EFI_BAD_BUFFER_SIZE;
      } else {
        Status = EFI_OUT_OF_RESOURCES;
      }
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
