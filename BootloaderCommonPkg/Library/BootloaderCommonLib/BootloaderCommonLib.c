/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SerialPortLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>

CONST CHAR8  mHex[]   = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
CONST CHAR8 *mStage[] = { "1A", "1B", "2", "PAYLOAD"};

/**
  Halt CPU from execution.

  @param[in]  Message      Message to display before halt.

**/
VOID
CpuHalt (
  IN CHAR8  *Message
  )
{
  DEBUG_LOG_BUFFER_HEADER *LogBufHdr;

  if (Message != NULL) {
    DEBUG ((DEBUG_ERROR, "\n%a", Message));
  }
  DEBUG ((DEBUG_ERROR, "\nSTAGE_%a: System halted!\n", mStage[GetLoaderStage()]));

  // Flush all console buffer if serial console is not active
  if ((PcdGet32 (PcdDebugOutputDeviceMask) & DEBUG_OUTPUT_DEVICE_SERIAL_PORT) == 0) {
    LogBufHdr = (DEBUG_LOG_BUFFER_HEADER *) GetDebugLogBufferPtr ();
    SerialPortWrite ((UINT8 *)LogBufHdr->Buffer, LogBufHdr->UsedLength - LogBufHdr->HeaderLength);
  }

  CpuDeadLoop ();
}

/**
  Halt CPU from execution and print message and error code.

  @param[in]  Message      Message to display before halt.
  @param[in]  Status       Error status.

**/
VOID
CpuHaltWithStatus (
  IN CHAR8           *Message,
  IN RETURN_STATUS    Status
  )
{
  DEBUG ((DEBUG_ERROR, "Error: %r\n", Status));
  CpuHalt (Message);
}

/**
  Dump a binary block using HEX byte format (16 bytes per line).

  @param[in]  Indent      Indent space for each line (16 bytes).
  @param[in]  Offset      Offset from the data buffer pointer.
  @param[in]  DataSize    Data buffer size.
  @param[in]  UserData    Pointer to the data buffer.

**/
VOID
DumpHex (
  IN UINTN        Indent,
  IN UINTN        Offset,
  IN UINTN        DataSize,
  IN VOID         *UserData
  )
{
  DEBUG_CODE_BEGIN();

  UINT8 *Data;

  CHAR8 Val[50];

  CHAR8 Str[20];

  UINT8 TempByte;
  UINTN Size;
  UINTN Index;

  Data = UserData;
  while (DataSize != 0) {
    Size = 16;
    if (Size > DataSize) {
      Size = DataSize;
    }

    for (Index = 0; Index < Size; Index += 1) {
      TempByte            = Data[Index];
      Val[Index * 3 + 0]  = mHex[TempByte >> 4];
      Val[Index * 3 + 1]  = mHex[TempByte & 0xF];
      Val[Index * 3 + 2]  = (CHAR8) ((Index == 7) ? '-' : ' ');
      Str[Index]          = (CHAR8) ((TempByte < ' ' || TempByte > 'z') ? '.' : TempByte);
    }

    Val[Index * 3]  = 0;
    Str[Index]      = 0;
    DEBUG ((DEBUG_INFO, "%*a%08X: %-48a *%a*\n", Indent, "", Offset, Val, Str));

    Data += Size;
    Offset += Size;
    DataSize -= Size;
  }

  DEBUG_CODE_END();
}

/**
  Returns the data buffer for a specific library ID.

  @param[in]  LibId   Library ID.
                      It needs to be less than PcdMaxLibraryDataEntry.
  @param[in, out] BufPtr  Pointer to receive the buffer address.

  @retval EFI_SUCCESS             Library data was returned successfully.
  @retval EFI_NOT_FOUND           Cannot find the data for a given library ID.
  @retval EFI_INVALID_PARAMETER   LibId is invalid.

**/
EFI_STATUS
EFIAPI
GetLibraryData (
  IN      UINT32    LibId,
  IN OUT  VOID    **BufPtr
  )
{
  LIBRARY_DATA         *LibDataPtr;
  EFI_STATUS            Status;

  if (LibId >= PcdGet32 (PcdMaxLibraryDataEntry)) {
    return EFI_INVALID_PARAMETER;
  }

  Status     = EFI_NOT_FOUND;
  LibDataPtr = (LIBRARY_DATA *) GetLibraryDataPtr ();
  if (LibDataPtr != NULL) {
    if (LibDataPtr[LibId].BufBase != 0) {
      *BufPtr = (VOID *) (UINTN)LibDataPtr[LibId].BufBase;
      Status = EFI_SUCCESS;
    }
  }

  return Status;
}

/**
  Set the data buffer for a specific library ID.

  @param[in]  LibId       Library ID.
                          It needs to be less than PcdMaxLibraryDataEntry.
  @param[in]  BufPtr      Data buffer address.
  @param[in]  BufSize     Data buffer size.

  @retval EFI_SUCCESS     Data buffer was set successfully.
  @retval EFI_ABORTED     Failed to set the data buffer.
  @retval EFI_INVALID_PARAMETER   LibId is invalid.

**/
EFI_STATUS
EFIAPI
SetLibraryData (
  IN  UINT32    LibId,
  IN  VOID     *BufPtr,
  IN  UINT32    BufSize
  )
{
  LIBRARY_DATA         *LibDataPtr;
  EFI_STATUS            Status;

  if (LibId >= PcdGet32 (PcdMaxLibraryDataEntry)) {
    return EFI_INVALID_PARAMETER;
  }

  Status     = EFI_ABORTED;
  LibDataPtr = (LIBRARY_DATA *) GetLibraryDataPtr ();
  if (LibDataPtr != NULL) {
    LibDataPtr[LibId].BufBase = (UINT32)BufPtr;
    LibDataPtr[LibId].BufSize = BufSize;
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
  This function registers the service list.

  @param  Service       Serivce header pointer to register

  @retval EFI_OUT_OF_RESOURCES   No space to add new service
  @retval EFI_SUCCESS            The service has been registered.

**/
EFI_STATUS
EFIAPI
RegisterService (
  IN VOID      *Service
  )
{
  SERVICES_LIST             *ServiceList;
  UINT32                    Index;

  ServiceList = (SERVICES_LIST *)GetServiceListPtr ();
  for (Index = 0; Index < ServiceList->Count; Index++) {
    if (ServiceList->Header[Index] == NULL) {
      break;
    }
    if ((ServiceList->Header[Index]->Signature == 0) ||
        (ServiceList->Header[Index]->Signature == ((SERVICE_COMMON_HEADER *)Service)->Signature)) {
      break;
    }
  }

  if (Index == ServiceList->Count) {
    return  EFI_OUT_OF_RESOURCES;
  } else {
    ServiceList->Header[Index] = (SERVICE_COMMON_HEADER *)Service;
  }

  return EFI_SUCCESS;
}

/**
  Get the service pointer by the service signature

  @param[in] Signature            The Signature for the service to get

  @retval NULL                    The service is not available
  @retval Others                  The pointer of service.

**/
VOID *
EFIAPI
GetServiceBySignature (
  IN UINT32                 Signature
  )
{
  SERVICES_LIST             *ServiceList;
  SERVICE_COMMON_HEADER     *ServiceHeader;
  UINT32                    Index;

  ServiceList = (SERVICES_LIST *)GetServiceListPtr ();
  for (Index = 0; Index < ServiceList->Count; Index++) {
    ServiceHeader = ServiceList->Header[Index];
    if (ServiceHeader->Signature == Signature) {
      return ServiceHeader;
    }
  }

  return NULL;
}

/**
  Detect the actual used stack bottom.

  It keeps searching for a known pattern from the stack bottom to top.
  It breaks out when a first unmatched pattern is detected.

  @param[in] StackTop     The allocated stock top address.
  @param[in] StackSize    The allocated stock size.

  @retval    The actual used stack bottom address.

**/
UINT32
DetectUsedStackBottom (
  IN  UINT32    StackTop,
  IN  UINT32    StackSize
  )
{
  UINT32  *StackBot;

  StackBot = (UINT32 *) ((StackTop - StackSize) & ~ (sizeof (UINTN) - 1));
  ASSERT (*StackBot == STACK_DEBUG_FILL_PATTERN);

  while ((UINT32)StackBot < StackTop) {
    if (*StackBot != STACK_DEBUG_FILL_PATTERN) {
      break;
    }
    StackBot++;
  }
  return (UINT32)StackBot;
}

/**
  Gets component information from the flash map by partition.

  This function will look for the component matching the input signature
  in the flash map, if found, it will look for the component with back up
  flag based on the backup partition parmeter and will return the
  base address and size of the component.

  @param[in]  Signature         Signature of the component information required
  @param[in]  IsBackupPartition TRUE for Back up copy, FALSE for primary copy
  @param[out] Base              Base address of the component
  @param[out] Size              Size of the component

  @retval    EFI_SUCCESS    Found the component with the matching signature.
  @retval    EFI_NOT_FOUND  Component with the matching signature not found.

**/
EFI_STATUS
EFIAPI
GetComponentInfoByPartition (
  IN  UINT32     Signature,
  IN  BOOLEAN    IsBackupPartition,
  OUT UINT32     *Base,
  OUT UINT32     *Size
  )
{
  UINTN                 Index;
  UINT32                MaxEntries;
  UINT32                PcdBase;
  UINT32                PcdSize;
  UINT32                RomBase;
  FLASH_MAP             *FlashMapPtr;
  EFI_STATUS            Status;
  FLASH_MAP_ENTRY_DESC  EntryDesc;

  PcdBase = 0;
  PcdSize = 0;
  Status = EFI_NOT_FOUND;

  FlashMapPtr = GetFlashMapPtr ();
  if (FlashMapPtr == NULL) {
    return EFI_UNSUPPORTED;
  }

  RomBase = (UINT32) (0x100000000ULL - FlashMapPtr->RomSize);
  MaxEntries = ((FlashMapPtr->Length - FLASH_MAP_HEADER_SIZE) / sizeof (FLASH_MAP_ENTRY_DESC));

  for (Index = 0; Index < MaxEntries; Index++) {
    EntryDesc = FlashMapPtr->EntryDesc[Index];
    //
    // Look for the component with desired signature
    //
    if (EntryDesc.Signature == 0xFFFFFFFF) {
      Status = EFI_NOT_FOUND;
      break;
    }
    if (EntryDesc.Signature == Signature) {
      //
      // Check if need to get back up copy
      // Back up copies can be identified with back up flag
      //
      if ( ((EntryDesc.Flags & FLASH_MAP_FLAGS_NON_REDUNDANT_REGION) != 0) ||
           (((IsBackupPartition ? FLASH_MAP_FLAGS_BACKUP : 0) ^ (EntryDesc.Flags & FLASH_MAP_FLAGS_BACKUP)) == 0) ) {
        PcdBase = (UINT32) (RomBase + EntryDesc.Offset);
        PcdSize = EntryDesc.Size;
        Status = EFI_SUCCESS;
        break;
      }
    }
  }

  //
  // If base and pcdbase are not 0, fill and return the value
  //
  if ((Base != NULL) && (PcdBase != 0)) {
    *Base = PcdBase;
  }
  if ((Size != NULL) && (PcdSize != 0)) {
    *Size = PcdSize;
  }

  return Status;
}


/**
  Get region size from flash map.

  This function will get topswap, redundant and non redundant region sizes from
  flash map.

  @param[in] FlashMap          The boot media address to be update.
  @param[in] RegionFlag        The source buffer to write to the boot media.
  @param[in] RegionOffset      The offset to the image.

  @retval
**/
UINT32
GetRegionOffsetSize (
  IN FLASH_MAP     *FlashMap,
  IN UINT8          RegionFlag,
  IN UINT32        *RegionOffset
  )
{
  UINTN                 Index;
  UINT32                RegionSize;
  UINT32                MaxEntries;
  UINT32                Offset;
  FLASH_MAP_ENTRY_DESC  EntryDesc;

  RegionSize = 0;
  Offset = 0xFFFFFFFF;

  if (FlashMap == NULL) {
    FlashMap = GetFlashMapPtr ();
  }

  //
  // Calculate maximum entries in the flash map
  //
  MaxEntries = ((FlashMap->Length - FLASH_MAP_HEADER_SIZE) / sizeof (FLASH_MAP_ENTRY_DESC));

  if (MaxEntries == 0) {
    return 0;
  }

  for (Index = 0; Index < MaxEntries; Index++) {
    EntryDesc = FlashMap->EntryDesc[Index];
    //
    // Break at the invalid entry
    //
    if (EntryDesc.Signature == 0xFFFFFFFF) {
      break;
    }

    //
    // If it is the region requested
    //
    if (EntryDesc.Flags & RegionFlag) {
      //
      // we do not want to count back up region blocks
      // Primary and back up blocks have equal reqion size
      //
      if (Offset == 0xFFFFFFFF) {
        Offset = EntryDesc.Offset;
      }
      if ((EntryDesc.Flags & FLASH_MAP_FLAGS_BACKUP) == 0 ) {
        RegionSize += EntryDesc.Size;
      }
    }
  }

  if (RegionOffset != NULL) {
    *RegionOffset = Offset;
  }

  return RegionSize;
}

/**
  This function retrieves a GUIDed HOB data and size.

  This function will search the HobListPtr to find the first GUIDed HOB that 
  its GUID matches Guid, and return the GUID size in Length if Lengh is no NULL.
  If HobListPtr is NULL, it will use the loader HOB list.

  @param[in]  HobListPtr  A HOB list pointer. If it is NULL using loader HOB list.
  @param[out] Length      GUID HOB length will be return if it is not NULL.
  @param[in]  Guid        A pointer to HOB GUID to search.

  @retval NULL            Failed to find the GUID HOB.
  @retval others          GUIDed HOB data pointer.

**/
VOID *
EFIAPI
GetGuidHobData (
  IN CONST VOID          *HobListPtr, OPTIONAL
  OUT      UINT32        *Length, OPTIONAL
  IN       EFI_GUID      *Guid
  )
{
  UINT8                  *GuidHob;
  CONST VOID             *HobList;

  HobList = HobListPtr;
  if (HobList == NULL) {
    HobList = (CONST VOID *) GetHobList ();
  }

  GuidHob = GetNextGuidHob (Guid, HobList);
  if (GuidHob != NULL) {
    if (Length != NULL) {
      *Length = GET_GUID_HOB_DATA_SIZE (GuidHob);
    }
    return GET_GUID_HOB_DATA (GuidHob);
  }

  return NULL;
}


/**
  Get device address

  If the device is PCI device, the device address format is 0x00BBDDFF, where
  BB, DD and FF are PCI bus, device and function number.
  If the device is MMIO device, the device address format is 0xMMxxxxxx, where
  MM should be non-zero value, xxxxxx could be any value.

  @param[in]  DeviceType         The device type, refer OS_BOOT_MEDIUM_TYPE.
  @param[in]  DeviceInstance     The device instance number starting from 0.

  @retval     Device address for a given device instance, return 0 if the device
              could not be found from device table.
**/
UINT32
EFIAPI
GetDeviceAddr (
  IN  UINT8          DeviceType,
  IN  UINT8          DeviceInstance
  )
{
  PLT_DEVICE_TABLE   *DeviceTable;
  PLT_DEVICE         *Device;
  UINT32             DeviceBase;
  UINT32             Index;

  DeviceBase  = 0;
  Device = NULL;
  DeviceTable = (PLT_DEVICE_TABLE *)GetDeviceTable();
  for (Index = 0; Index < DeviceTable->DeviceNumber; Index++) {
    Device = &DeviceTable->Device[Index];
    if ((Device->Type == DeviceType) && (Device->Instance == DeviceInstance)){
      break;
    }
  }

  if (DeviceTable->DeviceNumber != Index) {
    DeviceBase = Device->Dev.DevAddr;
  }
  return DeviceBase;
}

