/** @file
  Full variable service library for SBL modules.

  Implements VariableLib on a shared UEFI-compatible nonvolatile store with
  synchronous writes, bounded-memory reclaim, and FTW recovery.
  Full RuntimeDxe security policy parity and exhaustive cross-domain
  stress/conformance execution remain follow-up validation work.

  UVAR is split into equal working and spare halves. The working half contains
  the variable FV followed by an 8 KiB FTW workspace. Reclaim builds the complete
  replacement in spare before committing SpareComplete, then copies only the
  variable FV back, leaving the active transaction log available for recovery.
  The first 8 KiB of spare also serves as a temporary workspace backup when
  the transaction log is full; it is not used for both purposes concurrently.

  SBL owns the store only before payload takeover. Writes are synchronous; there
  is no early-write journal to flush at handoff. SPI failures latch until reboot,
  and reads are blocked if an interrupted target copy leaves the FV inconsistent.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Guid/FlashMapInfoGuid.h>
#include <Library/BaseLib.h>
#include <Library/ExtraBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLib.h>
#include <Library/VariableLib.h>
#include <Library/UefiVariableLib.h>
#include <Service/SpiFlashService.h>

#ifndef EFI_VARIABLE_ENHANCED_AUTHENTICATED_ACCESS
#define EFI_VARIABLE_ENHANCED_AUTHENTICATED_ACCESS  BIT7
#endif

// Library data keeps mutable state relocatable across CAR and loader-stage handoff.
typedef struct {
  BOOLEAN     Initialized;
  BOOLEAN     InitAttempted;
  BOOLEAN     WriteFailed;
  BOOLEAN     TargetInconsistent;
  EFI_STATUS  InitStatus;
  UINT32      Base;
  UINT32      Size;
  UINT32      StoreBase;
  UINT32      StoreSize;
  EFI_GUID    ZeroGuid;
} FULL_VARIABLE_INSTANCE;

STATIC
FULL_VARIABLE_INSTANCE *
GetFullVariableInstance (
  VOID
  )
{
  EFI_STATUS              Status;
  FULL_VARIABLE_INSTANCE  *Instance;

  Instance = NULL;
  Status = GetLibraryData (PcdGet8 (PcdVariableLibId), (VOID **)&Instance);
  if (Status == EFI_NOT_FOUND) {
    Instance = AllocateZeroPool (sizeof (*Instance));
    if (Instance == NULL) {
      return NULL;
    }
    Status = SetLibraryData (PcdGet8 (PcdVariableLibId), Instance, sizeof (*Instance));
    if (EFI_ERROR (Status)) {
      FreePool (Instance);
    }
  }
  return EFI_ERROR (Status) ? NULL : Instance;
}

#define FULL_VARIABLE_FTW_WORKING_SIZE   0x2000
#define FULL_VARIABLE_FTW_VALID_STATE                  0
#define FULL_VARIABLE_FTW_INVALID_STATE                1
#define FULL_VARIABLE_FTW_WORKSPACE_VALID_STATE        0xFE
#define FULL_VARIABLE_FTW_WORKSPACE_INVALID_MASK       0xFD
#define FULL_VARIABLE_FTW_WORKSPACE_RETIRED_STATE      0xFC
#define FULL_VARIABLE_FTW_HEADER_ALLOCATED_STATE       0xFE
#define FULL_VARIABLE_FTW_WRITES_ALLOCATED_STATE       0xFC
#define FULL_VARIABLE_FTW_HEADER_COMPLETE_STATE        0xF8
#define FULL_VARIABLE_FTW_HEADER_COMPLETE_MASK         0xFB
#define FULL_VARIABLE_FTW_SPARE_COMPLETE_STATE         0xFD
#define FULL_VARIABLE_FTW_DESTINATION_COMPLETE_STATE   0xF9
#define FULL_VARIABLE_FTW_DESTINATION_COMPLETE_MASK    0xFB
#define FULL_VARIABLE_MAX_VARIABLE_SIZE  SIZE_64KB
#define FULL_VARIABLE_MAX_HW_ERR_SIZE    SIZE_32KB
#define FULL_VARIABLE_FV_HEADER_SIZE     (sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY))

typedef struct {
  EFI_GUID  Signature;
  UINT32    Crc;
  UINT8     WorkingBlockValid : 1;
  UINT8     WorkingBlockInvalid : 1;
  UINT8     Reserved : 6;
  UINT8     Reserved3[3];
  UINT64    WriteQueueSize;
} FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER;

// Match the selected EDK2 writer's MdeModulePkg/Include/Guid/SystemNvDataGuid.h
// ABI: natural alignment makes both write headers and records 40 bytes.
// Do not pack these to match UefiVariableLib/FtwLastWrite.h; that separate
// reader's declarations are not the on-flash format emitted by this writer.
typedef struct {
  UINT8     HeaderAllocated : 1;
  UINT8     WritesAllocated : 1;
  UINT8     Complete : 1;
  UINT8     Reserved : 5;
  EFI_GUID  CallerId;
  UINT64    NumberOfWrites;
  UINT64    PrivateDataSize;
} FULL_VARIABLE_FTW_WRITE_HEADER;

typedef struct {
  UINT8   BootBlockUpdate : 1;
  UINT8   SpareComplete : 1;
  UINT8   DestinationComplete : 1;
  UINT8   Reserved : 5;
  EFI_LBA Lba;
  UINT64  Offset;
  UINT64  Length;
  INT64   RelativeOffset;
} FULL_VARIABLE_FTW_WRITE_RECORD;

STATIC_ASSERT (sizeof (FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER) == 32, "EDK2 FTW workspace ABI");
STATIC_ASSERT (sizeof (FULL_VARIABLE_FTW_WRITE_HEADER) == 40, "EDK2 FTW write header ABI");
STATIC_ASSERT (sizeof (FULL_VARIABLE_FTW_WRITE_RECORD) == 40, "EDK2 FTW write record ABI");
STATIC_ASSERT (OFFSET_OF (FULL_VARIABLE_FTW_WRITE_HEADER, CallerId) == 4, "EDK2 FTW caller offset");
STATIC_ASSERT (OFFSET_OF (FULL_VARIABLE_FTW_WRITE_HEADER, NumberOfWrites) == 24, "EDK2 FTW count offset");
STATIC_ASSERT (OFFSET_OF (FULL_VARIABLE_FTW_WRITE_HEADER, PrivateDataSize) == 32, "EDK2 FTW private size offset");
STATIC_ASSERT (OFFSET_OF (FULL_VARIABLE_FTW_WRITE_RECORD, Lba) == 8, "EDK2 FTW LBA offset");
STATIC_ASSERT (OFFSET_OF (FULL_VARIABLE_FTW_WRITE_RECORD, Offset) == 16, "EDK2 FTW target offset");
STATIC_ASSERT (OFFSET_OF (FULL_VARIABLE_FTW_WRITE_RECORD, Length) == 24, "EDK2 FTW length offset");
STATIC_ASSERT (OFFSET_OF (FULL_VARIABLE_FTW_WRITE_RECORD, RelativeOffset) == 32, "EDK2 FTW spare offset");

extern EFI_GUID gEdkiiWorkingBlockSignatureGuid;

STATIC
BOOLEAN
IsErased (
  IN CONST VOID  *Buffer,
  IN UINTN       Length
  );

STATIC
SPI_FLASH_SERVICE *
GetSpiService (
  VOID
  )
{
  return (SPI_FLASH_SERVICE *)GetServiceBySignature (SPI_FLASH_SERVICE_SIGNATURE);
}

STATIC
EFI_STATUS
FlashOffset (
  IN UINT32  Address,
  IN UINT32  Length,
  OUT UINT32 *Offset
  )
{
  EFI_STATUS              Status;
  UINT32                  RegionSize;
  SPI_FLASH_SERVICE       *SpiService;
  FULL_VARIABLE_INSTANCE  *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SpiService = GetSpiService ();
  if (SpiService == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  if ((Address < Instance->Base) || (Length > Instance->Size) ||
      (Address - Instance->Base > Instance->Size - Length)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpiService->SpiGetRegion (FlashRegionBios, NULL, &RegionSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Deliberate UINT32 wrap translates top-of-4-GiB mapping to a BIOS-region offset.
  *Offset = Address + RegionSize;
  if ((*Offset > RegionSize) || (Length > RegionSize - *Offset)) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
VerifyFlash (
  IN UINT32  Address,
  IN UINT32  Offset,
  IN UINT32  Length,
  IN UINT8   *Buffer OPTIONAL
  )
{
  EFI_STATUS         Status;
  SPI_FLASH_SERVICE  *SpiService;
  UINT32             Index;
  UINT32             Verified;
  UINT32             Count;
  UINT8              Readback[256];
  UINT8              Expected;

  SpiService = GetSpiService ();
  if (SpiService == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  Status = EFI_SUCCESS;
  for (Verified = 0; !EFI_ERROR (Status) && (Verified < Length); Verified += Count) {
    Count = MIN (Length - Verified, sizeof (Readback));
    Status = SpiService->SpiRead (FlashRegionBios, Offset + Verified, Count, Readback);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "UVAR SPI readback failed: address=0x%08X offset=0x%X status=%r\n", Address, Offset + Verified, Status));
      break;
    }
    for (Index = 0; Index < Count; Index++) {
      Expected = (Buffer == NULL) ? MAX_UINT8 : Buffer[Verified + Index];
      if (Readback[Index] != Expected) {
        DEBUG ((DEBUG_ERROR, "UVAR SPI readback mismatch: address=0x%08X index=0x%X expected=0x%02X actual=0x%02X\n",
                Address, Verified + Index, Expected, Readback[Index]));
        Status = EFI_DEVICE_ERROR;
        break;
      }
    }
  }
  AsmFlushCacheRange ((VOID *)(UINTN)Address, Length);
  MemoryFence ();
  return Status;
}

STATIC
EFI_STATUS
WriteFlash (
  IN UINT32  Address,
  IN UINT32  Length,
  IN UINT8   *Buffer
  )
{
  EFI_STATUS              Status;
  UINT32                  Offset;
  SPI_FLASH_SERVICE       *SpiService;
  FULL_VARIABLE_INSTANCE  *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SpiService = GetSpiService ();
  if (SpiService == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  Status = FlashOffset (Address, Length, &Offset);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UVAR write address translation failed: address=0x%08X length=0x%X status=%r\n", Address, Length, Status));
    return Status;
  }
  Status = SpiService->SpiWrite (FlashRegionBios, Offset, Length, Buffer);
  if (EFI_ERROR (Status)) {
    AsmFlushCacheRange ((VOID *)(UINTN)Address, Length);
    MemoryFence ();
    DEBUG ((DEBUG_ERROR, "UVAR SPI write failed: address=0x%08X offset=0x%X length=0x%X status=%r\n", Address, Offset, Length, Status));
  } else {
    Status = VerifyFlash (Address, Offset, Length, Buffer);
  }
  if (EFI_ERROR (Status)) {
    Instance->WriteFailed = TRUE;
  }
  return Status;
}

STATIC
EFI_STATUS
EraseFlash (
  IN UINT32  Address,
  IN UINT32  Length
  )
{
  EFI_STATUS              Status;
  UINT32                  Offset;
  SPI_FLASH_SERVICE       *SpiService;
  FULL_VARIABLE_INSTANCE  *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SpiService = GetSpiService ();
  if (SpiService == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  Status = FlashOffset (Address, Length, &Offset);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SpiService->SpiErase (FlashRegionBios, Offset, Length);
  if (EFI_ERROR (Status)) {
    AsmFlushCacheRange ((VOID *)(UINTN)Address, Length);
    MemoryFence ();
    DEBUG ((DEBUG_ERROR, "UVAR SPI erase failed: address=0x%08X offset=0x%X length=0x%X status=%r\n", Address, Offset, Length, Status));
  } else {
    Status = VerifyFlash (Address, Offset, Length, NULL);
  }
  if (EFI_ERROR (Status)) {
    Instance->WriteFailed = TRUE;
  }
  return Status;
}

STATIC
BOOLEAN
IsErased (
  IN CONST VOID  *Buffer,
  IN UINTN       Length
  )
{
  CONST UINT8  *Bytes;
  UINTN        Index;

  Bytes = Buffer;
  for (Index = 0; Index < Length; Index++) {
    if (Bytes[Index] != MAX_UINT8) {
      return FALSE;
    }
  }

  return TRUE;
}

STATIC
BOOLEAN
ValidWorkspace (
  IN FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  *Header
  )
{
  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  Copy;
  UINT32                                  Crc;

  if (!CompareGuid (&Header->Signature, &gEdkiiWorkingBlockSignatureGuid) ||
      (Header->WorkingBlockValid != FULL_VARIABLE_FTW_VALID_STATE) ||
      (Header->WorkingBlockInvalid == FULL_VARIABLE_FTW_VALID_STATE) ||
      (Header->WriteQueueSize != FULL_VARIABLE_FTW_WORKING_SIZE - sizeof (*Header))) {
    return FALSE;
  }

  Copy = *Header;
  Copy.Crc = MAX_UINT32;
  Copy.WorkingBlockValid = FULL_VARIABLE_FTW_INVALID_STATE;
  Copy.WorkingBlockInvalid = FULL_VARIABLE_FTW_INVALID_STATE;
  Crc = CalculateCrc32 (&Copy, sizeof (Copy));
  if (Crc != Header->Crc) {
    DEBUG ((DEBUG_ERROR, "UVAR FTW workspace CRC mismatch: address=0x%08X stored=0x%08X calculated=0x%08X\n",
            (UINT32)(UINTN)Header, Header->Crc, Crc));
  }
  return (BOOLEAN)(Crc == Header->Crc);
}

STATIC
VOID
BuildWorkspace (
  OUT FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  *Header
  )
{
  SetMem (Header, sizeof (*Header), MAX_UINT8);
  CopyGuid (&Header->Signature, &gEdkiiWorkingBlockSignatureGuid);
  Header->WriteQueueSize = FULL_VARIABLE_FTW_WORKING_SIZE - sizeof (*Header);
  Header->Crc = CalculateCrc32 (Header, sizeof (*Header));
}

STATIC
EFI_STATUS
WriteStream (
  IN UINT32  Address,
  IN CONST VOID  *Source,
  IN UINT32  Length
  )
{
  EFI_STATUS  Status;
  UINT32      Offset;
  UINT32      Count;
  UINT8       Buffer[256];

  for (Offset = 0; Offset < Length; Offset += Count) {
    Count = MIN (Length - Offset, sizeof (Buffer));
    CopyMem (Buffer, (CONST UINT8 *)Source + Offset, Count);
    if (IsErased (Buffer, Count)) {
      continue;
    }
    Status = WriteFlash (Address + Offset, Count, Buffer);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
FlushSpareToTarget (
  IN UINT32  TargetBlockOffset,
  IN UINT32  WriteLength
  );

STATIC
EFI_STATUS
ReclaimWorkspace (
  VOID
  );

STATIC
EFI_STATUS
FindFtwQueueSlot (
  IN  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  *Workspace,
  OUT UINTN                                   *QueueOffset
  );

STATIC
UEFI_VARIABLE_HEADER *
GetStartPointer (
  IN UEFI_VARIABLE_STORE_HEADER  *VarStoreHeader
  )
{
  return (UEFI_VARIABLE_HEADER *)HEADER_ALIGN (VarStoreHeader + 1);
}

STATIC
UEFI_VARIABLE_HEADER *
GetEndPointer (
  IN UEFI_VARIABLE_STORE_HEADER  *VarStoreHeader
  )
{
  return (UEFI_VARIABLE_HEADER *)HEADER_ALIGN ((UINTN)VarStoreHeader + VarStoreHeader->Size);
}

STATIC
BOOLEAN
IsValidVariableHeader (
  IN UEFI_VARIABLE_HEADER  *Variable
  )
{
  if ((Variable == NULL) || (Variable->StartId != UEFI_VARIABLE_DATA)) {
    return FALSE;
  }

  return TRUE;
}

STATIC
UINTN
GetVariableHeaderSize (
  IN BOOLEAN  AuthFlag
  )
{
  return AuthFlag ? sizeof (UEFI_AUTHENTICATED_VARIABLE_HEADER) : sizeof (UEFI_VARIABLE_HEADER);
}

STATIC
UINTN
NameSizeOfVariable (
  IN UEFI_VARIABLE_HEADER  *Variable,
  IN BOOLEAN               AuthFlag
  )
{
  UEFI_AUTHENTICATED_VARIABLE_HEADER  *AuthVariable;

  AuthVariable = (UEFI_AUTHENTICATED_VARIABLE_HEADER *)Variable;
  if (AuthFlag) {
    if ((AuthVariable->State == MAX_UINT8) ||
      (AuthVariable->DataSize == MAX_UINT32) ||
      (AuthVariable->NameSize == MAX_UINT32) ||
      (AuthVariable->Attributes == MAX_UINT32)) {
      return 0;
    }

    return (UINTN)AuthVariable->NameSize;
  }

    if ((Variable->State == MAX_UINT8) ||
      (Variable->DataSize == MAX_UINT32) ||
      (Variable->NameSize == MAX_UINT32) ||
      (Variable->Attributes == MAX_UINT32)) {
    return 0;
  }

  return (UINTN)Variable->NameSize;
}

STATIC
UINTN
DataSizeOfVariable (
  IN UEFI_VARIABLE_HEADER  *Variable,
  IN BOOLEAN               AuthFlag
  )
{
  UEFI_AUTHENTICATED_VARIABLE_HEADER  *AuthVariable;

  AuthVariable = (UEFI_AUTHENTICATED_VARIABLE_HEADER *)Variable;
  if (AuthFlag) {
    if ((AuthVariable->State == MAX_UINT8) ||
      (AuthVariable->DataSize == MAX_UINT32) ||
      (AuthVariable->NameSize == MAX_UINT32) ||
      (AuthVariable->Attributes == MAX_UINT32)) {
      return 0;
    }

    return (UINTN)AuthVariable->DataSize;
  }

    if ((Variable->State == MAX_UINT8) ||
      (Variable->DataSize == MAX_UINT32) ||
      (Variable->NameSize == MAX_UINT32) ||
      (Variable->Attributes == MAX_UINT32)) {
    return 0;
  }

  return (UINTN)Variable->DataSize;
}

STATIC
CHAR16 *
GetVariableNamePtr (
  IN UEFI_VARIABLE_HEADER  *Variable,
  IN BOOLEAN               AuthFlag
  )
{
  return (CHAR16 *)((UINTN)Variable + GetVariableHeaderSize (AuthFlag));
}

STATIC
EFI_GUID *
GetVendorGuidPtr (
  IN UEFI_VARIABLE_HEADER  *Variable,
  IN BOOLEAN               AuthFlag
  )
{
  UEFI_AUTHENTICATED_VARIABLE_HEADER  *AuthVariable;

  AuthVariable = (UEFI_AUTHENTICATED_VARIABLE_HEADER *)Variable;
  return AuthFlag ? &AuthVariable->VendorGuid : &Variable->VendorGuid;
}

STATIC
UINT8 *
GetVariableDataPtr (
  IN UEFI_VARIABLE_HEADER  *Variable,
  IN UEFI_VARIABLE_HEADER  *VariableHeader,
  IN BOOLEAN               AuthFlag
  )
{
  UINTN  Value;

  Value  = (UINTN)GetVariableNamePtr (Variable, AuthFlag);
  Value += NameSizeOfVariable (VariableHeader, AuthFlag);
  Value += GET_PAD_SIZE (NameSizeOfVariable (VariableHeader, AuthFlag));

  return (UINT8 *)Value;
}

STATIC
UEFI_VARIABLE_HEADER *
GetNextVariablePtr (
  IN UEFI_VARIABLE_STORE_INFO  *StoreInfo,
  IN UEFI_VARIABLE_HEADER      *Variable,
  IN UEFI_VARIABLE_HEADER      *VariableHeader
  )
{
  UINTN  Value;

  Value  = (UINTN)GetVariableDataPtr (Variable, VariableHeader, StoreInfo->AuthFlag);
  Value += DataSizeOfVariable (VariableHeader, StoreInfo->AuthFlag);
  Value += GET_PAD_SIZE (DataSizeOfVariable (VariableHeader, StoreInfo->AuthFlag));

  return (UEFI_VARIABLE_HEADER *)HEADER_ALIGN (Value);
}

STATIC
UEFI_VARIABLE_STORE_STATUS
GetVariableStoreStatus (
  IN UEFI_VARIABLE_STORE_HEADER  *VarStoreHeader
  )
{
  if ((VarStoreHeader == NULL) || (VarStoreHeader->Size == 0)) {
    return EfiInvalid;
  }

  if ((CompareGuid (&VarStoreHeader->Signature, &gEfiAuthenticatedVariableGuid) ||
       CompareGuid (&VarStoreHeader->Signature, &gEfiVariableGuid)) &&
      (VarStoreHeader->Format == UEFI_VARIABLE_STORE_FORMATTED) &&
      (VarStoreHeader->State == UEFI_VARIABLE_STORE_HEALTHY)) {
    return EfiValid;
  }

    if ((((UINT32 *)&VarStoreHeader->Signature)[0] == MAX_UINT32) &&
      (((UINT32 *)&VarStoreHeader->Signature)[1] == MAX_UINT32) &&
      (((UINT32 *)&VarStoreHeader->Signature)[2] == MAX_UINT32) &&
      (((UINT32 *)&VarStoreHeader->Signature)[3] == MAX_UINT32) &&
      (VarStoreHeader->Size == MAX_UINT32) &&
      (VarStoreHeader->Format == MAX_UINT8) &&
      (VarStoreHeader->State == MAX_UINT8)) {
    return EfiRaw;
  }

  return EfiInvalid;
}

STATIC
EFI_STATUS
CompareWithValidVariable (
  IN  UEFI_VARIABLE_STORE_INFO    *StoreInfo,
  IN  UEFI_VARIABLE_HEADER        *Variable,
  IN  UEFI_VARIABLE_HEADER        *VariableHeader,
  IN  CONST CHAR16                *VariableName,
  IN  CONST EFI_GUID              *VendorGuid,
  OUT UEFI_VARIABLE_POINTER_TRACK *PtrTrack
  )
{
  VOID      *Point;
  EFI_GUID  *TempVendorGuid;

  if (VariableName[0] == L'\0') {
    return EFI_NOT_FOUND;
  }

  TempVendorGuid = GetVendorGuidPtr (VariableHeader, StoreInfo->AuthFlag);

  if ((StrSize (VariableName) == NameSizeOfVariable (VariableHeader, StoreInfo->AuthFlag)) &&
      (CompareMem (VendorGuid, TempVendorGuid, sizeof (EFI_GUID)) == 0)) {
    Point = (VOID *)GetVariableNamePtr (Variable, StoreInfo->AuthFlag);
    if (CompareMem (VariableName, Point, NameSizeOfVariable (VariableHeader, StoreInfo->AuthFlag)) == 0) {
      PtrTrack->CurrPtr = Variable;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

STATIC
EFI_STATUS
EnsureVariableInitialized (
  IN FULL_VARIABLE_INSTANCE  *Instance
  )
{
  if (Instance->Initialized) {
    return EFI_SUCCESS;
  }

  if (Instance->WriteFailed || Instance->TargetInconsistent) {
    return EFI_ERROR (Instance->InitStatus) ? Instance->InitStatus : EFI_DEVICE_ERROR;
  }

  if (Instance->InitAttempted && (Instance->InitStatus != EFI_NOT_AVAILABLE_YET)) {
    return EFI_ERROR (Instance->InitStatus) ? Instance->InitStatus : EFI_DEVICE_ERROR;
  }

  return VariableConstructor (0, 0);
}

STATIC
UEFI_VARIABLE_STORE_HEADER *
GetVariableStore (
  IN  UEFI_VARIABLE_STORE_TYPE  Type,
  OUT UEFI_VARIABLE_STORE_INFO  *StoreInfo
  )
{
  UINT32                  NvStorageBase;
  UINT32                  NvStorageSize;
  EFI_STATUS              Status;
  FULL_VARIABLE_INSTANCE  *Instance;

  StoreInfo->VariableStoreHeader = NULL;
  StoreInfo->IndexTable          = NULL;
  StoreInfo->FtwLastWriteData    = NULL;
  StoreInfo->AuthFlag            = FALSE;

  if (Type != VariableStoreTypeNv) {
    return NULL;
  }

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return NULL;
  }

  if (!Instance->Initialized) {
    Status = EnsureVariableInitialized (Instance);
    if (EFI_ERROR (Status)) {
      return NULL;
    }
  }

  if (Instance->TargetInconsistent) {
    return NULL;
  }

  NvStorageBase = Instance->StoreBase;
  NvStorageSize = Instance->StoreSize;
  if ((NvStorageBase == 0) || (NvStorageSize < sizeof (UEFI_VARIABLE_STORE_HEADER))) {
    return NULL;
  }

  StoreInfo->VariableStoreHeader = (UEFI_VARIABLE_STORE_HEADER *)(UINTN)NvStorageBase;
  if (GetVariableStoreStatus (StoreInfo->VariableStoreHeader) != EfiValid) {
    return NULL;
  }

  StoreInfo->AuthFlag = (BOOLEAN)CompareGuid (&StoreInfo->VariableStoreHeader->Signature, &gEfiAuthenticatedVariableGuid);

  return StoreInfo->VariableStoreHeader;
}

STATIC
BOOLEAN
IsValidVariableFvAndStore (
  IN UINT32  FvBase,
  IN UINT32  FvLength,
  IN UINT32  StoreSize
  )
{
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;
  UEFI_VARIABLE_STORE_HEADER  *StoreHeader;

  FvHeader    = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)FvBase;
  StoreHeader = (UEFI_VARIABLE_STORE_HEADER *)((UINT8 *)FvHeader + FULL_VARIABLE_FV_HEADER_SIZE);
  return (BOOLEAN)((FvHeader->Signature == EFI_FVH_SIGNATURE) &&
                   CompareGuid (&FvHeader->FileSystemGuid, &gEfiSystemNvDataFvGuid) &&
                   (FvHeader->HeaderLength == FULL_VARIABLE_FV_HEADER_SIZE) &&
                   (FvHeader->FvLength == FvLength) &&
                   (CalculateSum16 ((UINT16 *)FvHeader, FULL_VARIABLE_FV_HEADER_SIZE) == 0) &&
                   (GetVariableStoreStatus (StoreHeader) == EfiValid) &&
                   (StoreHeader->Size == StoreSize));
}

STATIC
EFI_STATUS
InitializeNvVariableStore (
  IN UINT32  Base,
  IN UINT32  Size
  )
{
  EFI_FIRMWARE_VOLUME_HEADER              *FvHeader;
  UEFI_VARIABLE_STORE_HEADER              *StoreHeader;
  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  *Workspace;
  UINT8                                   HeaderBuffer[FULL_VARIABLE_FV_HEADER_SIZE];
  UEFI_VARIABLE_STORE_HEADER              NewStore;
  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  NewWorkspace;
  UINT8                                   State;
  EFI_STATUS                              Status;
  FULL_VARIABLE_INSTANCE                  *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StoreHeader = (UEFI_VARIABLE_STORE_HEADER *)(UINTN)Base;
  Workspace = (VOID *)(UINTN)(Instance->Base + Instance->Size / 2 - FULL_VARIABLE_FTW_WORKING_SIZE);
  if (IsValidVariableFvAndStore (Instance->Base, Instance->Size, Size)) {
    if (IsErased (Workspace, FULL_VARIABLE_FTW_WORKING_SIZE)) {
      BuildWorkspace (&NewWorkspace);
      Status = WriteFlash ((UINT32)(UINTN)Workspace, sizeof (NewWorkspace), (UINT8 *)&NewWorkspace);
      State = FULL_VARIABLE_FTW_WORKSPACE_VALID_STATE;
      if (!EFI_ERROR (Status)) {
        Status = WriteFlash ((UINT32)(UINTN)Workspace + sizeof (EFI_GUID) + sizeof (UINT32), 1, &State);
      }
      return Status;
    }
    if (ValidWorkspace (Workspace)) {
      return EFI_SUCCESS;
    }
    BuildWorkspace (&NewWorkspace);
    if ((CompareMem (Workspace, &NewWorkspace, sizeof (NewWorkspace)) == 0) &&
        IsErased (Workspace + 1, FULL_VARIABLE_FTW_WORKING_SIZE - sizeof (*Workspace))) {
      // Only the exact empty header can resume an interrupted validity commit.
      State = FULL_VARIABLE_FTW_WORKSPACE_VALID_STATE;
      return WriteFlash ((UINT32)(UINTN)Workspace + sizeof (EFI_GUID) + sizeof (UINT32), 1, &State);
    }
    return EFI_VOLUME_CORRUPTED;
  }

  ZeroMem (HeaderBuffer, sizeof (HeaderBuffer));
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)HeaderBuffer;
  CopyGuid (&FvHeader->FileSystemGuid, &gEfiSystemNvDataFvGuid);
  FvHeader->FvLength = Instance->Size;
  FvHeader->Signature = EFI_FVH_SIGNATURE;
  FvHeader->Attributes = 0x0004FEFF;
  FvHeader->HeaderLength = sizeof (HeaderBuffer);
  FvHeader->Revision = EFI_FVH_REVISION;
  FvHeader->BlockMap[0].NumBlocks = Instance->Size / SIZE_4KB;
  FvHeader->BlockMap[0].Length = SIZE_4KB;
  FvHeader->Checksum = CalculateCheckSum16 ((UINT16 *)FvHeader, sizeof (HeaderBuffer));
  if (!IsErased ((VOID *)(UINTN)Instance->Base, Instance->Size)) {
    if ((CompareMem ((VOID *)(UINTN)Instance->Base, HeaderBuffer, sizeof (HeaderBuffer)) != 0) ||
        !IsErased ((VOID *)(UINTN)Base, Instance->Size - sizeof (HeaderBuffer))) {
      DEBUG ((DEBUG_ERROR, "UVAR invalid non-erased store: base=0x%08X store_size=0x%X format=0x%02X state=0x%02X\n",
              Instance->Base, StoreHeader->Size, StoreHeader->Format, StoreHeader->State));
      return EFI_VOLUME_CORRUPTED;
    }
    DEBUG ((DEBUG_INFO, "UVAR completing interrupted FV-only provisioning: base=0x%08X\n", Instance->Base));
  } else {
    DEBUG ((DEBUG_INFO, "UVAR provisioning erased store: base=0x%08X size=0x%X\n", Instance->Base, Instance->Size));
  }
  ZeroMem (&NewStore, sizeof (NewStore));
  CopyGuid (&NewStore.Signature, &gEfiAuthenticatedVariableGuid);
  NewStore.Size = Size;
  NewStore.Format = UEFI_VARIABLE_STORE_FORMATTED;
  NewStore.State = UEFI_VARIABLE_STORE_HEALTHY;
  Status = WriteFlash (Instance->Base, sizeof (HeaderBuffer), HeaderBuffer);
  if (!EFI_ERROR (Status)) {
    Status = WriteFlash (Base, sizeof (NewStore), (UINT8 *)&NewStore);
  }
  if (!EFI_ERROR (Status)) {
    BuildWorkspace (&NewWorkspace);
    Status = WriteFlash ((UINT32)(UINTN)Workspace, sizeof (NewWorkspace), (UINT8 *)&NewWorkspace);
  }
  if (!EFI_ERROR (Status)) {
    State = FULL_VARIABLE_FTW_WORKSPACE_VALID_STATE;
    Status = WriteFlash ((UINT32)(UINTN)Workspace + sizeof (EFI_GUID) + sizeof (UINT32), 1, &State);
  }
  return Status;
}

STATIC
BOOLEAN
GetVariableHeader (
  IN  UEFI_VARIABLE_STORE_INFO  *StoreInfo,
  IN  UEFI_VARIABLE_HEADER      *Variable,
  OUT UEFI_VARIABLE_HEADER      **VariableHeader
  )
{
  UINTN  End;
  UINTN  Address;
  UINTN  HeaderSize;
  UINTN  NameSize;
  UINTN  DataSize;

  End = (UINTN)StoreInfo->VariableStoreHeader + StoreInfo->VariableStoreHeader->Size;
  Address = (UINTN)Variable;
  HeaderSize = GetVariableHeaderSize (StoreInfo->AuthFlag);
  if ((Variable == NULL) || (Address > End) || (HeaderSize > End - Address) ||
      !IsValidVariableHeader (Variable)) {
    return FALSE;
  }

  NameSize = NameSizeOfVariable (Variable, StoreInfo->AuthFlag);
  DataSize = DataSizeOfVariable (Variable, StoreInfo->AuthFlag);
  if ((NameSize < 2 * sizeof (CHAR16)) || ((NameSize & 1) != 0) ||
      (DataSize == 0) ||
      (NameSize > End - Address - HeaderSize) ||
      (DataSize > End - Address - HeaderSize - NameSize) ||
      (*(CHAR16 *)(Address + HeaderSize + NameSize - sizeof (CHAR16)) != L'\0') ||
      (StrSize ((CONST CHAR16 *)(Address + HeaderSize)) != NameSize)) {
    return FALSE;
  }

  *VariableHeader = Variable;
  return TRUE;
}

STATIC
EFI_STATUS
FindVariableEx (
  IN UEFI_VARIABLE_STORE_INFO      *StoreInfo,
  IN CONST CHAR16                  *VariableName,
  IN CONST EFI_GUID                *VendorGuid,
  OUT UEFI_VARIABLE_POINTER_TRACK  *PtrTrack
  )
{
  UEFI_VARIABLE_HEADER  *Variable;
  UEFI_VARIABLE_HEADER  *VariableHeader;
  UEFI_VARIABLE_HEADER  *InDeletedVariable;

  if ((StoreInfo->VariableStoreHeader == NULL) || (GetVariableStoreStatus (StoreInfo->VariableStoreHeader) != EfiValid)) {
    return EFI_NOT_FOUND;
  }

  PtrTrack->StartPtr = GetStartPointer (StoreInfo->VariableStoreHeader);
  PtrTrack->EndPtr   = GetEndPointer (StoreInfo->VariableStoreHeader);
  InDeletedVariable  = NULL;

  Variable = PtrTrack->StartPtr;
  while (GetVariableHeader (StoreInfo, Variable, &VariableHeader)) {
    if ((VariableHeader->State == UEFI_VAR_ADDED) ||
        (VariableHeader->State == (UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_ADDED))) {
      if (CompareWithValidVariable (StoreInfo, Variable, VariableHeader, VariableName, VendorGuid, PtrTrack) == EFI_SUCCESS) {
        if (VariableHeader->State == (UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_ADDED)) {
          InDeletedVariable = PtrTrack->CurrPtr;
        } else {
          return EFI_SUCCESS;
        }
      }
    }

    Variable = GetNextVariablePtr (StoreInfo, Variable, VariableHeader);
  }

  PtrTrack->CurrPtr = InDeletedVariable;
  return (PtrTrack->CurrPtr == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
}

STATIC
EFI_STATUS
FindVariable (
  IN CONST  CHAR16                 *VariableName,
  IN CONST  EFI_GUID               *VendorGuid,
  OUT UEFI_VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT UEFI_VARIABLE_STORE_INFO     *StoreInfo
  )
{
  if ((VariableName[0] != 0) && (VendorGuid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (GetVariableStore (VariableStoreTypeNv, StoreInfo) == NULL) {
    return EFI_DEVICE_ERROR;
  }

  return FindVariableEx (StoreInfo, VariableName, VendorGuid, PtrTrack);
}

STATIC
BOOLEAN
IsActiveVariableState (
  IN UINT8  State
  )
{
  return (BOOLEAN)((State == UEFI_VAR_ADDED) || (State == (UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_ADDED)));
}

STATIC
BOOLEAN
IsNullTerminatedInBytes (
  IN CHAR16  *VariableName,
  IN UINTN   VariableNameSize
  )
{
  UINTN  Index;

  if ((VariableName == NULL) || (VariableNameSize < sizeof (CHAR16))) {
    return FALSE;
  }

  for (Index = 0; Index < VariableNameSize / sizeof (CHAR16); Index++) {
    if (VariableName[Index] == L'\0') {
      return TRUE;
    }
  }

  return FALSE;
}

STATIC
BOOLEAN
HasUnsupportedVariableAttributeBits (
  IN UINT32  Attributes
  )
{
  UINT32  Supported;

  Supported = EFI_VARIABLE_NON_VOLATILE |
              EFI_VARIABLE_BOOTSERVICE_ACCESS |
              EFI_VARIABLE_RUNTIME_ACCESS |
              EFI_VARIABLE_HARDWARE_ERROR_RECORD |
              EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS |
              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS |
              EFI_VARIABLE_APPEND_WRITE |
              EFI_VARIABLE_ENHANCED_AUTHENTICATED_ACCESS;

  return (BOOLEAN)((Attributes & ~Supported) != 0);
}

STATIC
UINTN
GetFullVariableEntrySize (
  IN UEFI_VARIABLE_STORE_INFO  *StoreInfo,
  IN UEFI_VARIABLE_HEADER      *VariableHeader
  )
{
  UINTN  HeaderSize;
  UINTN  NameSize;
  UINTN  DataSize;
  UINTN  EntrySize;

  HeaderSize = GetVariableHeaderSize (StoreInfo->AuthFlag);
  NameSize   = NameSizeOfVariable (VariableHeader, StoreInfo->AuthFlag);
  DataSize   = DataSizeOfVariable (VariableHeader, StoreInfo->AuthFlag);

  EntrySize  = HeaderSize + NameSize + GET_PAD_SIZE (NameSize);
  EntrySize += DataSize + GET_PAD_SIZE (DataSize);
  return HEADER_ALIGN (EntrySize);
}

STATIC
BOOLEAN
IsSameVariableIdentity (
  IN UEFI_VARIABLE_STORE_INFO  *StoreInfo,
  IN UEFI_VARIABLE_HEADER      *Left,
  IN UEFI_VARIABLE_HEADER      *LeftCont,
  IN UEFI_VARIABLE_HEADER      *Right,
  IN UEFI_VARIABLE_HEADER      *RightCont
  )
{
  UINTN      LeftNameSize;
  UINTN      RightNameSize;
  EFI_GUID   *LeftGuid;
  EFI_GUID   *RightGuid;

  LeftNameSize  = NameSizeOfVariable (LeftCont, StoreInfo->AuthFlag);
  RightNameSize = NameSizeOfVariable (RightCont, StoreInfo->AuthFlag);
  if ((LeftNameSize == 0) || (LeftNameSize != RightNameSize)) {
    return FALSE;
  }

  LeftGuid  = GetVendorGuidPtr (LeftCont, StoreInfo->AuthFlag);
  RightGuid = GetVendorGuidPtr (RightCont, StoreInfo->AuthFlag);
  if (CompareMem (LeftGuid, RightGuid, sizeof (EFI_GUID)) != 0) {
    return FALSE;
  }

  return (BOOLEAN)(CompareMem (
                     GetVariableNamePtr (Left, StoreInfo->AuthFlag),
                     GetVariableNamePtr (Right, StoreInfo->AuthFlag),
                     LeftNameSize
                     ) == 0);
}

STATIC
BOOLEAN
IsSupersededVariable (
  IN UEFI_VARIABLE_STORE_INFO  *StoreInfo,
  IN UEFI_VARIABLE_HEADER      *Current,
  IN UEFI_VARIABLE_HEADER      *CurrentCont
  )
{
  UEFI_VARIABLE_POINTER_TRACK  Track;
  EFI_STATUS                   Status;

  Status = FindVariableEx (
             StoreInfo,
             GetVariableNamePtr (Current, StoreInfo->AuthFlag),
             GetVendorGuidPtr (CurrentCont, StoreInfo->AuthFlag),
             &Track
             );
  return (BOOLEAN)(!EFI_ERROR (Status) && (Track.CurrPtr != Current));
}

STATIC
UINTN
GetMaxVariableRecordSize (
  IN UEFI_VARIABLE_STORE_HEADER  *VarStoreHeader,
  IN UINT32                      Attributes
  )
{
  UINTN  StoreCapacity;
  UINTN  Limit;

  StoreCapacity = (UINTN)GetEndPointer (VarStoreHeader) - (UINTN)GetStartPointer (VarStoreHeader);
  Limit = ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != 0) ?
          FULL_VARIABLE_MAX_HW_ERR_SIZE : FULL_VARIABLE_MAX_VARIABLE_SIZE;
  return MIN (Limit, StoreCapacity) & ~(UINTN)(HEADER_ALIGNMENT - 1);
}

/**
  Compact selected live records and the requested replacement through spare.

  Preflight the final live set before any flash mutation, excluding all records
  of the replaced identity. Stream retained records and the complete replacement
  into spare with bounded buffers. For append, ExistingData is the prefix and
  Data is the suffix. SpareComplete is the recovery commit point: before it the
  working FV remains authoritative; afterward recovery restores the new image.
  Copy back only the variable FV, never the active workspace holding this log.
**/
STATIC
EFI_STATUS
ReclaimNvVariableStore (
  IN UINTN                 RequiredSize,
  IN UEFI_VARIABLE_HEADER  *ExistingVariable OPTIONAL,
  IN UEFI_VARIABLE_HEADER  *Replacement,
  IN CONST CHAR16          *VariableName,
  IN CONST UINT8           *ExistingData OPTIONAL,
  IN UINTN                 ExistingDataSize,
  IN CONST UINT8           *Data,
  IN UINTN                 DataSize
  )
{
  EFI_STATUS                             Status;
  UEFI_VARIABLE_STORE_INFO               StoreInfo;
  UEFI_VARIABLE_STORE_HEADER             *Store;
  UEFI_VARIABLE_HEADER                   *Variable;
  UEFI_VARIABLE_HEADER                   *Header;
  UEFI_AUTHENTICATED_VARIABLE_HEADER     HeaderCopy;
  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER *Workspace;
  FULL_VARIABLE_FTW_WRITE_HEADER         FtwHeader;
  FULL_VARIABLE_FTW_WRITE_RECORD         FtwRecord;
  FULL_VARIABLE_INSTANCE                 *Instance;
  UINT32                                 HalfSize;
  UINT32                                 FvRegionSize;
  UINT32                                 SpareBase;
  UINT32                                 WorkspaceAddr;
  UINT32                                 HeaderAddr;
  UINT32                                 RecordAddr;
  UINT32                                 Destination;
  UINTN                                  QueueOffset;
  UINTN                                  HeaderSize;
  UINTN                                  EntrySize;
  UINTN                                  UsedSize;
  UINTN                                  Pass;
  UINT8                                  State;

  Instance = GetFullVariableInstance ();
  if ((Instance == NULL) || Instance->WriteFailed) {
    return EFI_NOT_READY;
  }
  Store = GetVariableStore (VariableStoreTypeNv, &StoreInfo);
  if ((Store == NULL) || (GetVariableStoreStatus (Store) != EfiValid) ||
      (Store->Size != Instance->StoreSize) || (StoreInfo.FtwLastWriteData != NULL)) {
    return EFI_VOLUME_CORRUPTED;
  }

  HalfSize      = Instance->Size / 2;
  FvRegionSize  = HalfSize - FULL_VARIABLE_FTW_WORKING_SIZE;
  WorkspaceAddr = Instance->Base + FvRegionSize;
  Workspace     = (VOID *)(UINTN)WorkspaceAddr;
  if (!ValidWorkspace (Workspace)) {
    return EFI_VOLUME_CORRUPTED;
  }

  HeaderSize = GetVariableHeaderSize (StoreInfo.AuthFlag);
  SpareBase  = Instance->Base + HalfSize;
  HeaderAddr = 0;
  RecordAddr = 0;

  // Preflight live capacity before modifying workspace or spare; pass 2 streams to spare.
  for (Pass = 0; Pass < 2; Pass++) {
    if (Pass != 0) {
      Status = FindFtwQueueSlot (Workspace, &QueueOffset);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      if (FULL_VARIABLE_FTW_WORKING_SIZE - QueueOffset < sizeof (FtwHeader) + sizeof (FtwRecord)) {
        Status = ReclaimWorkspace ();
        if (EFI_ERROR (Status)) {
          return Status;
        }
        QueueOffset = sizeof (*Workspace);
      }

      HeaderAddr = WorkspaceAddr + (UINT32)QueueOffset;
      RecordAddr = HeaderAddr + (UINT32)sizeof (FtwHeader);

      SetMem (&FtwHeader, sizeof (FtwHeader), MAX_UINT8);
      CopyGuid (&FtwHeader.CallerId, &Store->Signature);
      FtwHeader.NumberOfWrites  = 1;
      FtwHeader.PrivateDataSize = 0;
      Status = WriteFlash (HeaderAddr, sizeof (FtwHeader), (UINT8 *)&FtwHeader);
      if (!EFI_ERROR (Status)) {
        State  = FULL_VARIABLE_FTW_HEADER_ALLOCATED_STATE;
        Status = WriteFlash (HeaderAddr, 1, &State);
      }
      if (!EFI_ERROR (Status)) {
        State  = FULL_VARIABLE_FTW_WRITES_ALLOCATED_STATE;
        Status = WriteFlash (HeaderAddr, 1, &State);
      }

      SetMem (&FtwRecord, sizeof (FtwRecord), MAX_UINT8);
      FtwRecord.Lba            = 0;
      FtwRecord.Offset         = FULL_VARIABLE_FV_HEADER_SIZE;
      FtwRecord.Length         = Instance->StoreSize;
      FtwRecord.RelativeOffset = -(INT64)HalfSize;
      if (!EFI_ERROR (Status)) {
        Status = WriteFlash (RecordAddr, sizeof (FtwRecord), (UINT8 *)&FtwRecord);
      }
      if (!EFI_ERROR (Status)) {
        Status = EraseFlash (SpareBase, HalfSize);
      }
      if (!EFI_ERROR (Status)) {
        Status = WriteStream (SpareBase, (VOID *)(UINTN)Instance->Base, FULL_VARIABLE_FV_HEADER_SIZE);
      }
      if (!EFI_ERROR (Status)) {
        Status = WriteStream (SpareBase + FULL_VARIABLE_FV_HEADER_SIZE, Store, sizeof (*Store));
      }
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    UsedSize = HEADER_ALIGN (sizeof (*Store));
    Variable = GetStartPointer (Store);
    while (GetVariableHeader (&StoreInfo, Variable, &Header)) {
      if (IsActiveVariableState (Header->State) &&
          ((ExistingVariable == NULL) ||
           !IsSameVariableIdentity (&StoreInfo, Variable, Header, ExistingVariable, ExistingVariable)) &&
          !IsSupersededVariable (&StoreInfo, Variable, Header)) {
        EntrySize = GetFullVariableEntrySize (&StoreInfo, Header);
        if ((EntrySize < HeaderSize) || (EntrySize > Store->Size - UsedSize)) {
          return EFI_VOLUME_CORRUPTED;
        }
        if (Pass != 0) {
          Destination = SpareBase + FULL_VARIABLE_FV_HEADER_SIZE + (UINT32)UsedSize;
          CopyMem (&HeaderCopy, Header, HeaderSize);
          HeaderCopy.State = UEFI_VAR_ADDED;
          Status = WriteStream (Destination, &HeaderCopy, (UINT32)HeaderSize);
          if (!EFI_ERROR (Status)) {
            Status = WriteStream (Destination + (UINT32)HeaderSize,
                                  (UINT8 *)Variable + HeaderSize, (UINT32)(EntrySize - HeaderSize));
          }
          if (EFI_ERROR (Status)) {
            return Status;
          }
        }
        UsedSize += EntrySize;
      }
      Variable = GetNextVariablePtr (&StoreInfo, Variable, Header);
    }
    if (RequiredSize > Store->Size - UsedSize) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  Destination = SpareBase + FULL_VARIABLE_FV_HEADER_SIZE + (UINT32)UsedSize;
  CopyMem (&HeaderCopy, Replacement, HeaderSize);
  HeaderCopy.State = UEFI_VAR_ADDED;
  Status = WriteStream (Destination, &HeaderCopy, (UINT32)HeaderSize);
  if (!EFI_ERROR (Status)) {
    Status = WriteStream (Destination + (UINT32)HeaderSize, VariableName,
                          (UINT32)NameSizeOfVariable (Replacement, StoreInfo.AuthFlag));
  }
  Destination = (UINT32)(UINTN)GetVariableDataPtr (
                                (UEFI_VARIABLE_HEADER *)(UINTN)Destination,
                                Replacement, StoreInfo.AuthFlag);
  if (!EFI_ERROR (Status) && (ExistingDataSize != 0)) {
    Status = WriteStream (Destination, ExistingData, (UINT32)ExistingDataSize);
  }
  if (!EFI_ERROR (Status)) {
    Status = WriteStream (Destination + (UINT32)ExistingDataSize, Data, (UINT32)DataSize);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Commit SpareComplete, copy the variable FV back without touching Workspace, then close the FTW entry.
  State  = FULL_VARIABLE_FTW_SPARE_COMPLETE_STATE;
  Status = WriteFlash (RecordAddr, 1, &State);
  if (!EFI_ERROR (Status)) {
    Status = FlushSpareToTarget (0, FvRegionSize);
  }
  if (!EFI_ERROR (Status)) {
    State  = FULL_VARIABLE_FTW_DESTINATION_COMPLETE_STATE;
    Status = WriteFlash (RecordAddr, 1, &State);
  }
  if (!EFI_ERROR (Status)) {
    State  = FULL_VARIABLE_FTW_HEADER_COMPLETE_STATE;
    Status = WriteFlash (HeaderAddr, 1, &State);
  }
  DEBUG ((EFI_ERROR (Status) ? DEBUG_ERROR : DEBUG_INFO,
          "UVAR streaming reclaim: %r, live=0x%X required=0x%X\n", Status, UsedSize, RequiredSize));
  return Status;
}

STATIC
EFI_STATUS
WriteNvVariableBytes (
  IN UINTN        Address,
  IN UINT32       ByteCount,
  IN CONST UINT8  *Buffer
  )
{
  if ((Address > MAX_UINT32) || ((ByteCount > 0) && (Buffer == NULL))) {
    return EFI_INVALID_PARAMETER;
  }

  return WriteFlash ((UINT32)Address, ByteCount, (UINT8 *)Buffer);
}

STATIC
EFI_STATUS
MarkVariableDeleted (
  IN UEFI_VARIABLE_HEADER  *Variable
  )
{
  EFI_STATUS  Status;
  UINT8       State;

  State = Variable->State;
  if ((State & ~UEFI_VAR_IN_DELETED_TRANSITION) != 0) {
    State  = (UINT8)(State & UEFI_VAR_IN_DELETED_TRANSITION);
    Status = WriteNvVariableBytes ((UINTN)&Variable->State, sizeof (State), &State);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  State  = (UINT8)(State & UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_DELETED);
  return WriteNvVariableBytes ((UINTN)&Variable->State, sizeof (State), &State);
}

STATIC
EFI_STATUS
RetireOtherActiveVariables (
  IN UEFI_VARIABLE_STORE_INFO  *StoreInfo,
  IN UEFI_VARIABLE_HEADER      *StartPtr,
  IN UEFI_VARIABLE_HEADER      *KeepVar
  )
{
  EFI_STATUS            Status;
  UEFI_VARIABLE_HEADER  *VarHdr;
  UEFI_VARIABLE_HEADER  *VarHdrCont;

  VarHdr = StartPtr;
  while (GetVariableHeader (StoreInfo, VarHdr, &VarHdrCont)) {
    if ((VarHdr != KeepVar) && IsActiveVariableState (VarHdrCont->State) &&
        IsSameVariableIdentity (StoreInfo, VarHdr, VarHdrCont, KeepVar, KeepVar)) {
      Status = MarkVariableDeleted (VarHdr);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    VarHdr = GetNextVariablePtr (StoreInfo, VarHdr, VarHdrCont);
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
FullVariableSetVariableInternal (
  IN CHAR16    *VariableName,
  IN EFI_GUID  *VariableGuid,
  IN UINT32    Attributes,
  IN UINTN     DataSize,
  IN VOID      *Data
  )
{
  EFI_STATUS                          Status;
  UEFI_VARIABLE_STORE_INFO            StoreInfo;
  UEFI_VARIABLE_STORE_HEADER          *VarStoreHdr;
  UEFI_VARIABLE_POINTER_TRACK         Track;
  UEFI_VARIABLE_HEADER                *VarHdr;
  UEFI_VARIABLE_HEADER                *VarHdrCont;
  UEFI_VARIABLE_HEADER                *ExistingVarCont;
  UEFI_VARIABLE_HEADER                *ExistingVar;
  UEFI_VARIABLE_HEADER                NewHdr;
  UEFI_AUTHENTICATED_VARIABLE_HEADER  AuthNewHdr;
  UEFI_VARIABLE_HEADER                *HeaderForDataPtr;
  UINT8                               *HeaderWritePtr;
  UINTN                               HeaderWriteSize;
  UINT8                               *ExistingData;
  UINT8                               *WriteData;
  UINTN                               ExistingDataSize;
  UINTN                               WriteDataSize;
  UINT32                              WriteAttributes;
  UINTN                               NameSize;
  UINTN                               HeaderSize;
  UINTN                               MaxRecordSize;
  UINTN                               RequiredSize;
  UINTN                               RemainingTail;
  UINTN                               EntryEnd;
  UINT16                              StartId;
  UINT8                               State;
  BOOLEAN                             AppendWrite;
  BOOLEAN                             DeleteOperation;
  FULL_VARIABLE_INSTANCE              *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VarStoreHdr = GetVariableStore (VariableStoreTypeNv, &StoreInfo);
  if ((VarStoreHdr == NULL) || (GetVariableStoreStatus (VarStoreHdr) != EfiValid)) {
    return EFI_DEVICE_ERROR;
  }

  if (Instance->WriteFailed || (StoreInfo.FtwLastWriteData != NULL)) {
    return EFI_NOT_READY;
  }

  if (VariableName[0] == L'\0') {
    return EFI_INVALID_PARAMETER;
  }

  if (HasUnsupportedVariableAttributeBits (Attributes)) {
    return EFI_INVALID_PARAMETER;
  }

  if (((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) ||
      ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) ||
      ((Attributes & EFI_VARIABLE_ENHANCED_AUTHENTICATED_ACCESS) != 0)) {
    return EFI_UNSUPPORTED;
  }

  AppendWrite     = (BOOLEAN)((Attributes & EFI_VARIABLE_APPEND_WRITE) != 0);
  DeleteOperation = (BOOLEAN)(!AppendWrite && (DataSize == 0));
  WriteAttributes = Attributes & ~EFI_VARIABLE_APPEND_WRITE;

  if ((WriteAttributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) ==
      EFI_VARIABLE_RUNTIME_ACCESS) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes != 0) &&
      ((WriteAttributes & (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)) == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  ExistingVarCont = NULL;
  Status          = FindVariable (VariableName, VariableGuid, &Track, &StoreInfo);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    return Status;
  }
  ExistingVar     = (!EFI_ERROR (Status)) ? Track.CurrPtr : NULL;
  if (ExistingVar != NULL) {
    if (!GetVariableHeader (&StoreInfo, ExistingVar, &ExistingVarCont)) {
      return EFI_DEVICE_ERROR;
    }

    if ((ExistingVarCont->Attributes & (EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS |
                                        EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS |
                                        EFI_VARIABLE_ENHANCED_AUTHENTICATED_ACCESS)) != 0) {
      return EFI_WRITE_PROTECTED;
    }

    // Preserve existing attributes when legacy SBL callers omit Attributes; otherwise enforce immutability.
    if (Attributes != 0) {
      if (WriteAttributes != ExistingVarCont->Attributes) {
        return EFI_INVALID_PARAMETER;
      }
    } else if (!DeleteOperation) {
      WriteAttributes = ExistingVarCont->Attributes;
    }
  } else if (!DeleteOperation && (Attributes == 0)) {
    WriteAttributes = EFI_VARIABLE_NON_VOLATILE |
                      EFI_VARIABLE_BOOTSERVICE_ACCESS |
                      EFI_VARIABLE_RUNTIME_ACCESS;
  }

  if (DeleteOperation) {
    if (ExistingVar == NULL) {
      return EFI_NOT_FOUND;
    }

    // Keep the selected value readable until all fallback records are retired.
    Status = RetireOtherActiveVariables (&StoreInfo, Track.StartPtr, ExistingVar);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    return MarkVariableDeleted (ExistingVar);
  }

  if ((WriteAttributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
    return EFI_UNSUPPORTED;
  }

  if (AppendWrite && (ExistingVar == NULL) && (DataSize == 0)) {
    return EFI_SUCCESS;
  }

  ExistingData     = NULL;
  ExistingDataSize = 0;
  WriteData        = (UINT8 *)Data;
  WriteDataSize    = DataSize;
  NameSize         = StrSize (VariableName);
  HeaderSize       = GetVariableHeaderSize (StoreInfo.AuthFlag);
  MaxRecordSize    = GetMaxVariableRecordSize (VarStoreHdr, WriteAttributes);

  if ((MaxRecordSize <= HeaderSize) || (NameSize > MaxRecordSize - HeaderSize)) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (AppendWrite && (ExistingVar != NULL)) {
    ExistingDataSize = DataSizeOfVariable (ExistingVarCont, StoreInfo.AuthFlag);
    ExistingData     = GetVariableDataPtr (ExistingVar, ExistingVarCont, StoreInfo.AuthFlag);

    if (WriteDataSize == 0) {
      return EFI_SUCCESS;
    }

    if ((ExistingDataSize > MaxRecordSize - HeaderSize - NameSize) ||
        (WriteDataSize > MaxRecordSize - HeaderSize - NameSize - ExistingDataSize)) {
      return EFI_OUT_OF_RESOURCES;
    }

    WriteDataSize = ExistingDataSize + WriteDataSize;
  } else {
    if (WriteDataSize > MaxRecordSize - HeaderSize - NameSize) {
      return EFI_OUT_OF_RESOURCES;
    }

    if (ExistingVar != NULL) {
      ExistingDataSize = DataSizeOfVariable (ExistingVarCont, StoreInfo.AuthFlag);
      ExistingData     = GetVariableDataPtr (ExistingVar, ExistingVarCont, StoreInfo.AuthFlag);

      if ((ExistingVarCont->Attributes == WriteAttributes) &&
          (ExistingDataSize == WriteDataSize) &&
          (CompareMem (ExistingData, WriteData, WriteDataSize) == 0)) {
        return EFI_SUCCESS;
      }
    }
  }

  RequiredSize  = HeaderSize + NameSize + GET_PAD_SIZE (NameSize);
  RequiredSize += WriteDataSize + GET_PAD_SIZE (WriteDataSize);
  RequiredSize  = HEADER_ALIGN (RequiredSize);
  if (RequiredSize > MaxRecordSize) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (StoreInfo.AuthFlag) {
    ZeroMem (&AuthNewHdr, sizeof (AuthNewHdr));
    AuthNewHdr.StartId         = UEFI_VARIABLE_DATA;
    AuthNewHdr.State           = UEFI_VAR_HEADER_VALID_ONLY;
    AuthNewHdr.Attributes      = WriteAttributes;
    AuthNewHdr.MonotonicCount  = 0;
    ZeroMem (&AuthNewHdr.TimeStamp, sizeof (EFI_TIME));
    AuthNewHdr.PubKeyIndex     = 0;
    AuthNewHdr.NameSize        = (UINT32)NameSize;
    AuthNewHdr.DataSize        = (UINT32)WriteDataSize;
    CopyMem (&AuthNewHdr.VendorGuid, VariableGuid, sizeof (EFI_GUID));

    HeaderForDataPtr = (UEFI_VARIABLE_HEADER *)&AuthNewHdr;
    HeaderWritePtr   = (UINT8 *)&AuthNewHdr;
    HeaderWriteSize  = sizeof (AuthNewHdr);
  } else {
    ZeroMem (&NewHdr, sizeof (NewHdr));
    NewHdr.StartId    = UEFI_VARIABLE_DATA;
    NewHdr.State      = UEFI_VAR_HEADER_VALID_ONLY;
    NewHdr.Attributes = WriteAttributes;
    NewHdr.NameSize   = (UINT32)NameSize;
    NewHdr.DataSize   = (UINT32)WriteDataSize;
    CopyMem (&NewHdr.VendorGuid, VariableGuid, sizeof (EFI_GUID));

    HeaderForDataPtr = &NewHdr;
    HeaderWritePtr   = (UINT8 *)&NewHdr;
    HeaderWriteSize  = sizeof (NewHdr);
  }

  VarHdr = (UEFI_VARIABLE_HEADER *)HEADER_ALIGN (VarStoreHdr + 1);
  while (GetVariableHeader (&StoreInfo, VarHdr, &VarHdrCont)) {
    VarHdr = GetNextVariablePtr (&StoreInfo, VarHdr, VarHdrCont);
  }

  RemainingTail = (UINTN)GetEndPointer (VarStoreHdr) - (UINTN)VarHdr;
  if ((RequiredSize > RemainingTail) || !IsErased (VarHdr, RemainingTail)) {
    return ReclaimNvVariableStore (
             RequiredSize, ExistingVar, HeaderForDataPtr, VariableName,
             AppendWrite ? ExistingData : NULL, AppendWrite ? ExistingDataSize : 0,
             WriteData, DataSize);
  }

  if (ExistingVar != NULL) {
    Status = RetireOtherActiveVariables (&StoreInfo, Track.StartPtr, ExistingVar);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (ExistingVar->State != (UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_ADDED)) {
      State = ExistingVar->State & UEFI_VAR_IN_DELETED_TRANSITION;
      Status = WriteNvVariableBytes ((UINTN)&ExistingVar->State, sizeof (State), &State);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  // Program header metadata and Name before StartId so torn header writes are never parsed as valid records.
  Status = WriteNvVariableBytes (
             (UINTN)VarHdr + sizeof (StartId),
             (UINT32)(HeaderWriteSize - sizeof (StartId)),
             HeaderWritePtr + sizeof (StartId)
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WriteNvVariableBytes ((UINTN)VarHdr + HeaderSize, (UINT32)NameSize, (UINT8 *)VariableName);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  StartId = UEFI_VARIABLE_DATA;
  Status  = WriteNvVariableBytes ((UINTN)VarHdr, sizeof (StartId), (CONST UINT8 *)&StartId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  EntryEnd = (UINTN)GetVariableDataPtr (VarHdr, HeaderForDataPtr, StoreInfo.AuthFlag);
  if (AppendWrite && (ExistingDataSize != 0)) {
    Status = WriteStream ((UINT32)EntryEnd, ExistingData, (UINT32)ExistingDataSize);
    if (!EFI_ERROR (Status)) {
      Status = WriteStream ((UINT32)(EntryEnd + ExistingDataSize), WriteData, (UINT32)DataSize);
    }
  } else {
    Status = WriteStream ((UINT32)EntryEnd, WriteData, (UINT32)WriteDataSize);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  State  = UEFI_VAR_ADDED;
  Status = WriteNvVariableBytes ((UINTN)&VarHdr->State, sizeof (State), &State);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return RetireOtherActiveVariables (
           &StoreInfo,
           (UEFI_VARIABLE_HEADER *)HEADER_ALIGN (VarStoreHdr + 1),
           VarHdr
           );
}

EFI_STATUS
EFIAPI
GetVariable (
  IN      CHAR16    *VariableName,
  IN      EFI_GUID  *VariableGuid,
  OUT     UINT32    *Attributes OPTIONAL,
  IN OUT  UINTN     *DataSize,
  OUT     VOID      *Data OPTIONAL
  )
{
  EFI_GUID                *VarGuid;
  FULL_VARIABLE_INSTANCE  *Instance;

  if ((VariableName == NULL) || (DataSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (VariableName[0] == L'\0') {
    return EFI_NOT_FOUND;
  }

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VarGuid = (VariableGuid != NULL) ? VariableGuid : &Instance->ZeroGuid;

  {
    EFI_STATUS             Status;

    if (!Instance->Initialized) {
      Status = EnsureVariableInitialized (Instance);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  {
    UEFI_VARIABLE_POINTER_TRACK  Variable;
    UEFI_VARIABLE_STORE_INFO     StoreInfo;
    UEFI_VARIABLE_HEADER         *VariableHeader;
    UINTN                        VarDataSize;
    EFI_STATUS                   Status;

    Status = FindVariable (VariableName, VarGuid, &Variable, &StoreInfo);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (!GetVariableHeader (&StoreInfo, Variable.CurrPtr, &VariableHeader)) {
      return EFI_DEVICE_ERROR;
    }

    VarDataSize = DataSizeOfVariable (VariableHeader, StoreInfo.AuthFlag);
    if (*DataSize < VarDataSize) {
      *DataSize = VarDataSize;
      return EFI_BUFFER_TOO_SMALL;
    }

    if (Data == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    CopyMem (
      Data,
      GetVariableDataPtr (Variable.CurrPtr, VariableHeader, StoreInfo.AuthFlag),
      VarDataSize
      );

    if (Attributes != NULL) {
      *Attributes = VariableHeader->Attributes;
    }

    *DataSize = VarDataSize;
    return EFI_SUCCESS;
  }
}

EFI_STATUS
EFIAPI
GetNextVariableName (
  IN OUT  UINTN     *VariableNameSize,
  IN OUT  CHAR16    *VariableName,
  IN OUT  EFI_GUID  *VariableGuid
  )
{
  EFI_STATUS                   Status;
  UEFI_VARIABLE_STORE_INFO     StoreInfo;
  UEFI_VARIABLE_STORE_HEADER   *VarStoreHdr;
  UEFI_VARIABLE_POINTER_TRACK  Track;
  UEFI_VARIABLE_HEADER         *VarHdr;
  UEFI_VARIABLE_HEADER         *VarHdrCont;
  BOOLEAN                      ReturnNext;
  BOOLEAN                      CurrentFound;
  UINTN                        CurrentNameSize;
  UINTN                        NameSize;
  EFI_GUID                     *VendorGuid;
  CHAR16                       *NameBuf;
  FULL_VARIABLE_INSTANCE       *Instance;

  if ((VariableNameSize == NULL) || (VariableName == NULL) || (VariableGuid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (*VariableNameSize < sizeof (CHAR16)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsNullTerminatedInBytes (VariableName, *VariableNameSize)) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VarStoreHdr = GetVariableStore (VariableStoreTypeNv, &StoreInfo);
  if ((VarStoreHdr == NULL) || (GetVariableStoreStatus (VarStoreHdr) != EfiValid)) {
    return EFI_DEVICE_ERROR;
  }

  Track.StartPtr = (UEFI_VARIABLE_HEADER *)HEADER_ALIGN (VarStoreHdr + 1);
  Track.EndPtr   = (UEFI_VARIABLE_HEADER *)HEADER_ALIGN ((UINTN)VarStoreHdr + VarStoreHdr->Size);

  ReturnNext   = (BOOLEAN)(VariableName[0] != L'\0');
  CurrentFound = (BOOLEAN)(VariableName[0] == L'\0');
  CurrentNameSize = CurrentFound ? 0 : StrSize (VariableName);

  VarHdr = Track.StartPtr;
  while (GetVariableHeader (&StoreInfo, VarHdr, &VarHdrCont)) {
    if (!IsActiveVariableState (VarHdrCont->State) ||
        IsSupersededVariable (&StoreInfo, VarHdr, VarHdrCont)) {
      VarHdr = GetNextVariablePtr (&StoreInfo, VarHdr, VarHdrCont);
      continue;
    }

    NameSize = NameSizeOfVariable (VarHdrCont, StoreInfo.AuthFlag);
    if (NameSize < sizeof (CHAR16)) {
      VarHdr = GetNextVariablePtr (&StoreInfo, VarHdr, VarHdrCont);
      continue;
    }

    NameBuf = GetVariableNamePtr (VarHdr, StoreInfo.AuthFlag);
    VendorGuid = GetVendorGuidPtr (VarHdrCont, StoreInfo.AuthFlag);

    if (!CurrentFound) {
      if ((NameSize == CurrentNameSize) &&
          (CompareMem (VendorGuid, VariableGuid, sizeof (EFI_GUID)) == 0) &&
          (CompareMem (NameBuf, VariableName, NameSize) == 0)) {
        CurrentFound = TRUE;
        ReturnNext = FALSE;
      }
      VarHdr = GetNextVariablePtr (&StoreInfo, VarHdr, VarHdrCont);
      continue;
    }

    if (!ReturnNext) {
      if (*VariableNameSize < NameSize) {
        *VariableNameSize = NameSize;
        return EFI_BUFFER_TOO_SMALL;
      }

      CopyMem (VariableName, NameBuf, NameSize);
      CopyMem (VariableGuid, VendorGuid, sizeof (EFI_GUID));
      *VariableNameSize = NameSize;
      return EFI_SUCCESS;
    }

    ReturnNext = FALSE;
    VarHdr = GetNextVariablePtr (&StoreInfo, VarHdr, VarHdrCont);
  }

  if ((VariableName[0] != L'\0') && !CurrentFound) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = EFI_NOT_FOUND;
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetVariable (
  IN CHAR16    *VariableName,
  IN EFI_GUID  *VariableGuid,
  IN UINT32    Attributes,
  IN UINTN     DataSize,
  IN VOID      *Data
  )
{
  EFI_GUID                *VarGuid;
  FULL_VARIABLE_INSTANCE  *Instance;

  if (VariableName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VarGuid = (VariableGuid != NULL) ? VariableGuid : &Instance->ZeroGuid;

  if ((DataSize > 0) && (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!Instance->Initialized) {
    EFI_STATUS  Status;
    Status = EnsureVariableInitialized (Instance);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return FullVariableSetVariableInternal (VariableName, VarGuid, Attributes, DataSize, Data);
}

EFI_STATUS
EFIAPI
QueryVariableInfo (
  IN  UINT32  Attributes,
  OUT UINT64  *MaximumVariableStorageSize,
  OUT UINT64  *RemainingVariableStorageSize,
  OUT UINT64  *MaximumVariableSize
  )
{
  UEFI_VARIABLE_STORE_INFO     StoreInfo;
  UEFI_VARIABLE_STORE_HEADER   *VarStoreHdr;
  UEFI_VARIABLE_POINTER_TRACK  Track;
  UEFI_VARIABLE_HEADER         *VarHdr;
  UEFI_VARIABLE_HEADER         *VarHdrCont;
  UINT64                       UsedSize;
  UINTN                        HeaderSize;
  UINTN                        NameSize;
  UINTN                        EntrySize;
  FULL_VARIABLE_INSTANCE       *Instance;

  if ((MaximumVariableStorageSize == NULL) || (RemainingVariableStorageSize == NULL) ||
      (MaximumVariableSize == NULL) || (Attributes == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if (HasUnsupportedVariableAttributeBits (Attributes)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_BOOTSERVICE_ACCESS) == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_APPEND_WRITE) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  // Current implementation supports the common NV variable query path.
  if ((Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
    return EFI_UNSUPPORTED;
  }

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VarStoreHdr = GetVariableStore (VariableStoreTypeNv, &StoreInfo);
  if ((VarStoreHdr == NULL) || (GetVariableStoreStatus (VarStoreHdr) != EfiValid)) {
    return EFI_DEVICE_ERROR;
  }

  if (((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) ||
      ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) ||
      ((Attributes & EFI_VARIABLE_ENHANCED_AUTHENTICATED_ACCESS) != 0)) {
    return EFI_UNSUPPORTED;
  }

  Track.StartPtr              = GetStartPointer (VarStoreHdr);
  Track.EndPtr                = GetEndPointer (VarStoreHdr);
  *MaximumVariableStorageSize = (UINT64)((UINTN)Track.EndPtr - (UINTN)Track.StartPtr);
  UsedSize                    = 0;
  VarHdr                      = Track.StartPtr;

  while (GetVariableHeader (&StoreInfo, VarHdr, &VarHdrCont)) {
    if (IsActiveVariableState (VarHdrCont->State) &&
        !IsSupersededVariable (&StoreInfo, VarHdr, VarHdrCont)) {
      EntrySize = GetFullVariableEntrySize (&StoreInfo, VarHdrCont);
      UsedSize += (UINT64)EntrySize;
    }

    VarHdr = GetNextVariablePtr (&StoreInfo, VarHdr, VarHdrCont);
  }

  if (UsedSize > *MaximumVariableStorageSize) {
    return EFI_DEVICE_ERROR;
  }

  *RemainingVariableStorageSize = *MaximumVariableStorageSize - UsedSize;

  HeaderSize = GetVariableHeaderSize (StoreInfo.AuthFlag);
  NameSize   = 2 * sizeof (CHAR16);
  EntrySize  = GetMaxVariableRecordSize (VarStoreHdr, Attributes);
  if (EntrySize > (HeaderSize + NameSize)) {
    *MaximumVariableSize = (UINT64)(EntrySize - HeaderSize - NameSize);
  } else {
    *MaximumVariableSize = 0;
  }

  return EFI_SUCCESS;
}

/**
  Restore a committed spare image to aligned working-FV blocks.

  Erase each 4 KiB destination block, then program it through 256-byte chunks.
  Keep the active workspace outside the target range. Once copying starts,
  TargetInconsistent blocks mapped variable reads until every block succeeds;
  an SPI failure leaves that guard set for the rest of the boot.
**/
STATIC
EFI_STATUS
FlushSpareToTarget (
  IN UINT32  TargetBlockOffset,
  IN UINT32  WriteLength
  )
{
  EFI_STATUS              Status;
  UINT32                  FvRegionSize;
  UINT32                  SpareBase;
  UINT32                  TargetBase;
  UINT32                  BlockOffset;
  UINT32                  FlashTargetOffset;
  FULL_VARIABLE_INSTANCE  *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  FvRegionSize = Instance->Size / 2 - FULL_VARIABLE_FTW_WORKING_SIZE;
  SpareBase    = Instance->Base + Instance->Size / 2;
  if ((WriteLength == 0) ||
      ((TargetBlockOffset & (SIZE_4KB - 1)) != 0) ||
      ((WriteLength & (SIZE_4KB - 1)) != 0) ||
      (TargetBlockOffset >= FvRegionSize) ||
      (WriteLength > FvRegionSize - TargetBlockOffset)) {
    return EFI_INVALID_PARAMETER;
  }

  if (TargetBlockOffset == 0) {
    if ((WriteLength < FULL_VARIABLE_FV_HEADER_SIZE + sizeof (UEFI_VARIABLE_STORE_HEADER)) ||
        !IsValidVariableFvAndStore (SpareBase, Instance->Size, Instance->StoreSize)) {
      return EFI_VOLUME_CORRUPTED;
    }
  }

  TargetBase = Instance->Base + TargetBlockOffset;
  Status = FlashOffset (TargetBase, WriteLength, &FlashTargetOffset);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Copy 4 KiB blocks while keeping the live FTW workspace untouched as the recovery authority.
  Instance->TargetInconsistent = TRUE;
  for (BlockOffset = 0; BlockOffset < WriteLength; BlockOffset += SIZE_4KB) {
    Status = EraseFlash (TargetBase + BlockOffset, SIZE_4KB);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    if (!IsErased ((VOID *)(UINTN)(TargetBase + BlockOffset), SIZE_4KB)) {
      Instance->WriteFailed = TRUE;
      return EFI_DEVICE_ERROR;
    }
    Status = WriteStream (
               TargetBase + BlockOffset,
               (CONST VOID *)(UINTN)(SpareBase + BlockOffset),
               SIZE_4KB
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Instance->TargetInconsistent = FALSE;
  return EFI_SUCCESS;
}

/**
  Restore the active workspace from the valid backup at the start of spare.

  This supports the selected payload's standalone 8 KiB workspace with 4 KiB
  FVB blocks. In EDK2 FtwMisc.c, InitFtwProtocol() therefore makes the working
  block cover only the workspace, placing its backup at spare offset zero.
  A workspace inside a larger working block could have a nonzero backup
  offset and require restoring preceding data; that geometry is unsupported
  here, so do not scan spare's tail and replay it as an alternative backup.

  Commit the restored workspace's validity only after copying it, then
  invalidate the backup so it cannot cause a later rollback. This backup is
  not the variable-FV reclaim image; spare's tail remains reserved for FTW.
**/
STATIC
EFI_STATUS
RestoreWorkspace (
  VOID
  )
{
  UINT32                  Source;
  UINT32                  Destination;
  UINT32                  Offset;
  UINT8                   Buffer[256];
  UINT8                   State;
  EFI_STATUS              Status;
  FULL_VARIABLE_INSTANCE  *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Source = Instance->Base + Instance->Size / 2;
  Destination = Source - FULL_VARIABLE_FTW_WORKING_SIZE;
  if (!ValidWorkspace ((VOID *)(UINTN)Source)) {
    return EFI_VOLUME_CORRUPTED;
  }
  State = *(UINT8 *)(UINTN)(Destination + 20) & FULL_VARIABLE_FTW_WORKSPACE_INVALID_MASK;
  Status = WriteFlash (Destination + 20, 1, &State);
  if (!EFI_ERROR (Status)) {
    Status = EraseFlash (Destination, FULL_VARIABLE_FTW_WORKING_SIZE);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (!IsErased ((VOID *)(UINTN)Destination, FULL_VARIABLE_FTW_WORKING_SIZE)) {
    Instance->WriteFailed = TRUE;
    return EFI_DEVICE_ERROR;
  }
  for (Offset = 0; Offset < FULL_VARIABLE_FTW_WORKING_SIZE; Offset += sizeof (Buffer)) {
    CopyMem (Buffer, (VOID *)(UINTN)(Source + Offset), sizeof (Buffer));
    if (Offset == 0) {
      Buffer[20] = MAX_UINT8;
    }
    if (!IsErased (Buffer, sizeof (Buffer))) {
      Status = WriteFlash (Destination + Offset, sizeof (Buffer), Buffer);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }
  State = FULL_VARIABLE_FTW_WORKSPACE_VALID_STATE;
  Status = WriteFlash (Destination + 20, 1, &State);
  if (!EFI_ERROR (Status)) {
    // Retire the spare workspace copy once the working workspace is durable.
    State  = FULL_VARIABLE_FTW_WORKSPACE_RETIRED_STATE;
    Status = WriteFlash (Source + 20, 1, &State);
  }
  return Status;
}

/**
  Replace a full or safely discardable FTW queue with an empty workspace.

  Persist a fresh workspace at Spare + 0 before replacing the active copy, so
  reset recovery always has a valid authority once the active copy is erased.
  Callers must establish that no committed variable transaction needs replay
  before reusing spare for this workspace backup.
**/
STATIC
EFI_STATUS
ReclaimWorkspace (
  VOID
  )
{
  EFI_STATUS                              Status;
  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  NewWorkspace;
  UINT32                                  SpareBase;
  UINT8                                   State;
  FULL_VARIABLE_INSTANCE                  *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SpareBase = Instance->Base + Instance->Size / 2;
  Status    = EraseFlash (SpareBase, FULL_VARIABLE_FTW_WORKING_SIZE);
  if (!EFI_ERROR (Status)) {
    BuildWorkspace (&NewWorkspace);
    Status = WriteFlash (SpareBase, sizeof (NewWorkspace), (UINT8 *)&NewWorkspace);
  }
  if (!EFI_ERROR (Status)) {
    State  = FULL_VARIABLE_FTW_WORKSPACE_VALID_STATE;
    Status = WriteFlash (SpareBase + 20, 1, &State);
  }
  if (!EFI_ERROR (Status)) {
    Status = RestoreWorkspace ();
  }
  return Status;
}

STATIC
EFI_STATUS
FindFtwQueueSlot (
  IN  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  *Workspace,
  OUT UINTN                                   *QueueOffset
  )
{
  FULL_VARIABLE_FTW_WRITE_HEADER  *Header;
  UINTN                           Offset;
  UINTN                           Remaining;
  UINTN                           RecordSize;

  Offset = sizeof (*Workspace);
  while (Offset < FULL_VARIABLE_FTW_WORKING_SIZE) {
    Remaining = FULL_VARIABLE_FTW_WORKING_SIZE - Offset;
    Header    = (VOID *)((UINT8 *)Workspace + Offset);
    if (IsErased (Header, Remaining)) {
      *QueueOffset = Offset;
      return EFI_SUCCESS;
    }
    if ((Remaining < sizeof (*Header)) || (Header->HeaderAllocated != FULL_VARIABLE_FTW_VALID_STATE) ||
      (Header->Complete != FULL_VARIABLE_FTW_VALID_STATE) ||
        (Header->PrivateDataSize > Remaining - sizeof (*Header))) {
      return EFI_VOLUME_CORRUPTED;
    }
    RecordSize = sizeof (FULL_VARIABLE_FTW_WRITE_RECORD) + (UINTN)Header->PrivateDataSize;
    if ((Header->NumberOfWrites == 0) ||
        (Header->NumberOfWrites > (Remaining - sizeof (*Header)) / RecordSize)) {
      return EFI_VOLUME_CORRUPTED;
    }
    Offset += sizeof (*Header) + (UINTN)Header->NumberOfWrites * RecordSize;
  }

  *QueueOffset = FULL_VARIABLE_FTW_WORKING_SIZE;
  return EFI_SUCCESS;
}

/**
  Process the persistent FTW queue before trusting the destination FV headers.

  Skip bounded completed entries and replay only supported single-write,
  zero-private-data variable-FV transactions whose spare image is committed.
  Retire transactions with no committed spare without copying their data.
  Reject malformed queues and unsupported pending transaction forms instead
  of allowing new writes to reuse their recovery state.
**/
STATIC
EFI_STATUS
RecoverPendingWrites (
  IN FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  *Workspace
  )
{
  FULL_VARIABLE_FTW_WRITE_HEADER  *Header;
  FULL_VARIABLE_FTW_WRITE_RECORD  *Record;
  EFI_STATUS                      Status;
  UINT32                          HalfSize;
  UINT32                          FvRegionSize;
  UINT32                          TargetBlockOffset;
  UINT32                          WriteLength;
  UINTN                           Offset;
  UINTN                           Remaining;
  UINTN                           RecordSize;
  UINTN                           EntrySize;
  UINT8                           State;
  FULL_VARIABLE_INSTANCE          *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  HalfSize     = Instance->Size / 2;
  FvRegionSize = HalfSize - FULL_VARIABLE_FTW_WORKING_SIZE;
  Offset       = sizeof (*Workspace);
  while (Offset < FULL_VARIABLE_FTW_WORKING_SIZE) {
    Remaining = FULL_VARIABLE_FTW_WORKING_SIZE - Offset;
    Header = (VOID *)((UINT8 *)Workspace + Offset);
    if (IsErased (Header, Remaining)) {
      return EFI_SUCCESS;
    }
    if (Remaining < sizeof (*Header)) {
      return EFI_VOLUME_CORRUPTED;
    }
    if ((Header->Complete != FULL_VARIABLE_FTW_VALID_STATE) &&
      ((Header->HeaderAllocated != FULL_VARIABLE_FTW_VALID_STATE) ||
       (Header->WritesAllocated != FULL_VARIABLE_FTW_VALID_STATE))) {
      // Torn allocation counts are untrusted; only an entirely erased tail is safe to discard.
      if (!IsErased (Header + 1, Remaining - sizeof (*Header))) {
        return EFI_VOLUME_CORRUPTED;
      }
      return ReclaimWorkspace ();
    }
    if ((Header->HeaderAllocated != FULL_VARIABLE_FTW_VALID_STATE) ||
        (Header->PrivateDataSize > Remaining - sizeof (*Header))) {
      return EFI_VOLUME_CORRUPTED;
    }
    RecordSize = sizeof (*Record) + (UINTN)Header->PrivateDataSize;
    if ((Header->NumberOfWrites == 0) ||
        (Header->NumberOfWrites > (Remaining - sizeof (*Header)) / RecordSize)) {
      return EFI_VOLUME_CORRUPTED;
    }
    EntrySize = sizeof (*Header) + (UINTN)Header->NumberOfWrites * RecordSize;
    if (Header->Complete == FULL_VARIABLE_FTW_VALID_STATE) {
      Offset += EntrySize;
      continue;
    }
    if ((Header->NumberOfWrites != 1) ||
        (Header->PrivateDataSize != 0)) {
      return EFI_UNSUPPORTED;
    }
    Record = (VOID *)(Header + 1);
    if ((Record->SpareComplete != FULL_VARIABLE_FTW_VALID_STATE) &&
      (Record->DestinationComplete == FULL_VARIABLE_FTW_VALID_STATE)) {
      return EFI_VOLUME_CORRUPTED;
    }
    if ((Record->SpareComplete == FULL_VARIABLE_FTW_VALID_STATE) &&
        (Record->DestinationComplete != FULL_VARIABLE_FTW_VALID_STATE)) {
      if ((Record->BootBlockUpdate == FULL_VARIABLE_FTW_VALID_STATE) ||
          (Record->RelativeOffset < -(INT64)HalfSize) ||
          (Record->RelativeOffset >= 0)) {
        return EFI_UNSUPPORTED;
      }
      TargetBlockOffset = HalfSize - (UINT32)(-Record->RelativeOffset);
      if (((TargetBlockOffset & (SIZE_4KB - 1)) != 0) ||
          (TargetBlockOffset >= FvRegionSize) ||
          (Record->Lba != TargetBlockOffset / SIZE_4KB) ||
          (Record->Length == 0) ||
          (Record->Offset >= FvRegionSize - TargetBlockOffset) ||
          (Record->Length > FvRegionSize - TargetBlockOffset - Record->Offset)) {
        return EFI_UNSUPPORTED;
      }
      WriteLength = ALIGN_UP ((UINT32)(Record->Offset + Record->Length), SIZE_4KB);
      Status = FlushSpareToTarget (TargetBlockOffset, WriteLength);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      State = *(UINT8 *)Record & FULL_VARIABLE_FTW_DESTINATION_COMPLETE_MASK;
      Status = WriteFlash ((UINT32)(UINTN)Record, 1, &State);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      DEBUG ((DEBUG_INFO, "UVAR FTW: replayed pending variable transaction\n"));
    }
    State = *(UINT8 *)Header & FULL_VARIABLE_FTW_HEADER_COMPLETE_MASK;
    Status = WriteFlash ((UINT32)(UINTN)Header, 1, &State);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Offset += EntrySize;
  }
  return EFI_SUCCESS;
}

/**
  Locate or reuse UVAR geometry, recover pending work, and validate the store.

  Restore a valid workspace backup first, then replay FTW before FV validation
  because replay may repair torn destination headers. Provision only recognized
  erased/interrupted-initialization media. A missing SPI service may be retried
  with the same geometry; terminal initialization or SPI mutation failures do
  not trigger implicit destructive retries on later variable API calls.

  @param[in] Base  Mapped UVAR base, or zero to reuse/discover the region.
  @param[in] Size  Complete UVAR size, or zero to reuse/discover the region.
**/
EFI_STATUS
EFIAPI
VariableConstructor (
  IN UINT32  Base,
  IN UINT32  Size
  )
{
  EFI_STATUS                              Status;
  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  *Workspace;
  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER  *SpareWorkspace;
  UINT32                                  HalfSize;
  UINT32                                  WorkspaceOffset;
  UINT8                                   State;
  FULL_VARIABLE_INSTANCE                  *Instance;

  Instance = GetFullVariableInstance ();
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (Instance->WriteFailed || Instance->TargetInconsistent) {
    Instance->Initialized   = FALSE;
    Instance->InitAttempted = TRUE;
    if (!EFI_ERROR (Instance->InitStatus)) {
      Instance->InitStatus = EFI_DEVICE_ERROR;
    }
    return Instance->InitStatus;
  }

  if (!Instance->Initialized && Instance->InitAttempted &&
      (Instance->InitStatus != EFI_NOT_AVAILABLE_YET) &&
      !((Instance->InitStatus == EFI_NOT_FOUND) && (Base != 0) && (Size != 0))) {
    return Instance->InitStatus;
  }

  Instance->Initialized   = FALSE;
  Instance->InitAttempted = TRUE;

  if ((Base == 0) || (Size == 0)) {
    if ((Instance->Base != 0) && (Instance->Size != 0)) {
      Base = Instance->Base;
      Size = Instance->Size;
    } else {
      Status = GetComponentInfo (FLASH_MAP_SIG_UEFIVARIABLE, &Base, &Size);
      if (EFI_ERROR (Status)) {
        Instance->InitStatus = Status;
        return Status;
      }
    }
  }

  if (((Base & (SIZE_4KB - 1)) != 0) || ((Size & (SIZE_8KB - 1)) != 0) ||
      (Size < SIZE_32KB) || (Size > MAX_UINT32 - Base)) {
    Instance->InitStatus = EFI_INVALID_PARAMETER;
    return EFI_INVALID_PARAMETER;
  }

  Instance->Base      = Base;
  Instance->Size      = Size;
  Instance->StoreBase = Base + FULL_VARIABLE_FV_HEADER_SIZE;
  Instance->StoreSize = Size / 2 - FULL_VARIABLE_FTW_WORKING_SIZE - FULL_VARIABLE_FV_HEADER_SIZE;

  HalfSize        = Size / 2;
  WorkspaceOffset = HalfSize - FULL_VARIABLE_FTW_WORKING_SIZE;
  Workspace       = (VOID *)(UINTN)(Base + WorkspaceOffset);
  SpareWorkspace  = (VOID *)(UINTN)(Base + HalfSize);
  if (!ValidWorkspace (Workspace) && ValidWorkspace (SpareWorkspace)) {
    Status = RestoreWorkspace ();
    if (EFI_ERROR (Status)) {
      Instance->InitStatus = Status;
      return Status;
    }
    DEBUG ((DEBUG_INFO, "UVAR FTW: restored EDK2 workspace from spare\n"));
  } else if (ValidWorkspace (Workspace) && ValidWorkspace (SpareWorkspace)) {
    State  = FULL_VARIABLE_FTW_WORKSPACE_RETIRED_STATE;
    Status = WriteFlash (Base + HalfSize + 20, 1, &State);
    if (EFI_ERROR (Status)) {
      Instance->InitStatus = Status;
      return Status;
    }
  }

  // Pending FTW may repair the FV or store header, so replay before validating it.
  if (ValidWorkspace (Workspace)) {
    Status = RecoverPendingWrites (Workspace);
    if (EFI_ERROR (Status)) {
      Instance->InitStatus = Status;
      return Status;
    }
  }

  Status = InitializeNvVariableStore (Instance->StoreBase, Instance->StoreSize);
  if (EFI_ERROR (Status)) {
    Instance->InitStatus = Status;
    return Status;
  }

  Instance->InitStatus  = EFI_SUCCESS;
  Instance->Initialized = TRUE;
  return EFI_SUCCESS;
}
