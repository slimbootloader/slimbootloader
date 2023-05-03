/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOOTLOADER_COMMON_LIB_H_
#define _BOOTLOADER_COMMON_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <Guid/BootLoaderServiceGuid.h>
#include <Guid/FlashMapInfoGuid.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <Guid/DeviceTableHobGuid.h>
#include <Guid/KeyHashGuid.h>
#include <Library/BaseLib.h>
#include <Library/CryptoLib.h>

#define  IS_X64                       (sizeof(UINT64) == sizeof(UINTN))

#define  STACK_DEBUG_FILL_PATTERN     0x5AA55AA5
#define  UEFI_PAYLOAD_ID_SIGNATURE    SIGNATURE_32('U', 'E', 'F', 'I')
#define  LINX_PAYLOAD_ID_SIGNATURE    SIGNATURE_32('L', 'I', 'N', 'X')
#define  AUTO_PAYLOAD_ID_SIGNATURE    SIGNATURE_32('A', 'U', 'T', 'O')

#define  ALIGN_UP(address, align)     (((address) + ((align) - 1)) & ~((align)-1))
#define  ALIGN_DOWN(address, align)   ((address) & ~((align)-1))

//
// These MACRO definations should be used only after feature config data is set in stage1b.
//
#define ACPI_FEATURE_ENABLED()   (GetFeatureCfg() & FEATURE_ACPI)
#define ACPI_ENABLED()           (FeaturePcdGet (PcdAcpiEnabled) && ACPI_FEATURE_ENABLED())
#define MEASURED_BOOT_ENABLED()  (FeaturePcdGet (PcdMeasuredBootEnabled) && (GetFeatureCfg() & FEATURE_MEASURED_BOOT) && ACPI_FEATURE_ENABLED())

#define PCD_GET32_WITH_ADJUST(x)  GetAdjustedPcdBase (PcdGet32 (x))

#pragma pack(1)

// Data that are handed off between stages
#define  MAX_TS_NUM                   64
typedef struct {
  UINT32        PerfIndex;
  UINT32        FreqKhz;
  UINT64        TimeStamp[MAX_TS_NUM];
} BL_PERF_DATA;

typedef struct {
  UINT32        BufBase;
  UINT32        BufSize;
} LIBRARY_DATA;

typedef struct {
  UINT32        Signature;
  UINT32        CompressedSize;
  UINT32        Size;
  UINT16        Version;
  UINT8         Svn;
  UINT8         Attribute;
  UINT8         Data[];
} LOADER_COMPRESSED_HEADER;

#pragma pack()

//
// Enabled features that might come from configuration data.
//
#define        FEATURE_ACPI                     BIT0
#define        FEATURE_MEASURED_BOOT            BIT1
#define        FEATURE_MMC_TUNING               BIT2
#define        FEATURE_MMC_FORCE_TUNING         BIT3
#define        FEATURE_VERIFIED_BOOT            BIT4

#define MM_PCI_ADDRESS( Bus, Device, Function, Register ) \
  ( (UINTN)PcdGet64(PcdPciExpressBaseAddress) + \
    (UINTN)((Bus) << 20) + \
    (UINTN)((Device) << 15) + \
    (UINTN)((Function) << 12) + \
    (UINTN)(Register) \
  )

#define TO_MM_PCI_ADDRESS( Addr ) \
  MM_PCI_ADDRESS ( ((Addr) >> 16) & 0xFF, ((Addr) >> 8) & 0xFF, (Addr) & 0xFF, 0)

#define TO_PCI_LIB_ADDRESS( Addr ) \
  PCI_LIB_ADDRESS ( ((Addr) >> 16) & 0xFF, ((Addr) >> 8) & 0xFF, (Addr) & 0xFF, 0)

//
// Enumeration of stages of bootloader execution.
//
typedef enum {
  LOADER_STAGE_1A,
  LOADER_STAGE_1B,
  LOADER_STAGE_2,
  LOADER_STAGE_PAYLOAD
} LOADER_STAGE;

//
// Enum type for SMM rebase enabling
//
typedef enum {
  SMM_REBASE_DISABLE,
  SMM_REBASE_ENABLE,
  SMM_REBASE_AUTO
} SMM_REBASE_MODE;

//
// Enum for boot partitions
//
typedef enum {
  PrimaryPartition,
  BackupPartition
} BOOT_PARTITION;

/**
  Returns the current stage of Bootloader execution.

  @retval LOADER_STAGE  Current stage of bootloader execution.
**/
LOADER_STAGE
EFIAPI
GetLoaderStage (
  VOID
  );


/**
  Get the component hash data by the component type.

  @param[in]  ComponentType   Component type.
  @param[out] HashData        Hash data pointer corresponding Component type.
  @param[out] HashType        Hash Type for Hash store.

  @retval RETURN_SUCCESS             Get hash data succeeded.
  @retval RETURN_UNSUPPORTED         Hash component type is not supported.
  @retval RETURN_NOT_FOUND           Hash data is not found.

**/
RETURN_STATUS
GetComponentHash (
  IN        UINT8            ComponentType,
  OUT CONST UINT8            **HashData,
  OUT       UINT8            *HashType
  );

/**
  Returns the pointer to the Flash Map.

  @return The pointer to the Flash Map.

**/
VOID *
EFIAPI
GetFlashMapPtr (
  VOID
  );

/**
  Returns the pointer to the HOB list.

  If the pointer to the HOB list is NULL, then ASSERT().

  @return The pointer to the HOB list.

**/
VOID *
EFIAPI
GetHobListPtr (
  VOID
  );

/**
  This function retrieves performance data pointer.

  @retval    The performance data pointer.

**/
BL_PERF_DATA *
EFIAPI
GetPerfDataPtr (
  VOID
  );

/**
  This function gets the configuration data pointer.

  @retval    The configuration data blob pointer.

**/
VOID *
EFIAPI
GetConfigDataPtr (
  VOID
  );

/**
  This function retrieves features configuration.

  @retval    The feature configuration.

**/
UINT32
EFIAPI
GetFeatureCfg (
  VOID
  );

/**
  This function retrieves current platform id.

  @retval    The current platform id.

**/
UINT16
EFIAPI
GetPlatformId (
  VOID
  );

/**
  This function retrieves log buffer pointer.

  @retval    The log buffer pointer.

**/
VOID *
EFIAPI
GetDebugLogBufferPtr (
  VOID
  );

/**
  This function retrieves global library data pointer.

  @retval    The library data pointer.

**/
VOID *
EFIAPI
GetLibraryDataPtr (
  VOID
  );

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
  );

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
  );

/**
  This function retrieves global library data pointer.

  @retval    The library data pointer.

**/
VOID *
EFIAPI
GetServiceListPtr (
  VOID
  );

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
  );

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
  );

/**
  This function retrieves global PCD data pointer.

  @retval    The PCD data pointer.

**/
VOID *
EFIAPI
GetPcdDataPtr (
  VOID
  );

/**
  This function retrieves current boot partition.

  @retval    The current boot partition.

**/
UINT8
EFIAPI
GetCurrentBootPartition (
  VOID
  );

/**
  Halt CPU from execution.

  @param[in]  Message      Message to display before halt.

**/
VOID
CpuHalt (
  IN CHAR8  *Message
  );

/**
  Halt CPU from execution and print message and error code.

  @param[in]  Message      Message to display before halt.
  @param[in]  Status       Error status.

**/
VOID
CpuHaltWithStatus (
  IN CHAR8           *Message,
  IN RETURN_STATUS    Status
  );

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
  );

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
  );

/**
  This function retrieves platform data pointer.

  @retval    The platform data pointer.

**/
VOID *
EFIAPI
GetPlatformDataPtr (
  VOID
  );

/**
  Gets component entry from the flash map by partition.

  This function will look for the component matching the input signature
  in the flash map, if found, it will look for the component with back up
  flag based on the backup partition parmeter and will return the
  entry of the component from flash map.

  @param[in]  Signature         Signature of the component information required
  @param[in]  IsBackupPartition TRUE for Back up copy, FALSE for primary copy

  @retval    NULL    Component entry not found in flash map
  @retval    Others  Pointer to component entry

**/
FLASH_MAP_ENTRY_DESC *
EFIAPI
GetComponentEntryByPartition (
  IN  UINT32                Signature,
  IN  BOOLEAN               IsBackupPartition
  );

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
  );

/**
  Gets component base from the flash map.

  This function will look for the component based on the input signature
  in the flash map, if found, will return the base address of the component.

  @param[in]  Signature     Signature of the component information required
  @param[out] Base          Base address of the component
  @param[out] Size          Size of the component

  @retval    EFI_SUCCESS    Found the component with the matching signature.
  @retval    EFI_NOT_FOUND  Component with the matching signature not found.

**/
EFI_STATUS
EFIAPI
GetComponentInfo (
  IN  UINT32     Signature,
  OUT UINT32     *Base,
  OUT UINT32     *Size
  );

/**
  Get region size from flash map.

  This function will get topswap, redundant and non redundant region sizes from
  flash map.

  @param[in] FlashMap          The boot media address to be update.
  @param[in] RegionFlag        The source buffer to write to the boot media.
  @param[in] RegionOffset      The offset to the image.

  @retval  others             Error happening when updating.
**/
UINT32
GetRegionOffsetSize (
  IN FLASH_MAP     *FlashMap,
  IN UINT8          RegionFlag,
  IN UINT32        *RegionOffset
  );

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
  );

/**
  This function sets device table.

  @param DeviceTable   The pointer to device table.

**/
VOID
EFIAPI
SetDeviceTable (
  IN VOID                 *DeviceTable
  );

/**
  This function retrieves the platform device table.

  @retval    The platform device table.

**/
VOID *
EFIAPI
GetDeviceTable (
  VOID
  );

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
  );

/**
  Set device address in device table

  If the device is PCI device, the device address format is 0x00BBDDFF, where
  BB, DD and FF are PCI bus, device and function number.
  If the device is MMIO device, the device address format is 0xMMxxxxxx, where
  MM should be non-zero value, xxxxxx could be any value.

  @param[in]  DeviceType         The device type, refer OS_BOOT_MEDIUM_TYPE.
  @param[in]  DeviceInstance     The device instance number starting from 0.
  @param[in]  DeviceAddr         The device address.

  @retval     EFI_SUCCESS        If the given device type and instance are found and set.
              EFI_NOT_FOUND      The given device type and instance are not found.

**/
EFI_STATUS
EFIAPI
SetDeviceAddr (
  IN  UINT8          DeviceType,
  IN  UINT8          DeviceInstance,
  IN  UINT32         DeviceAddr
  );

/**
  This function retrieves container list pointer.

  @retval    The container list pointer.

**/
VOID *
EFIAPI
GetContainerListPtr (
  VOID
  );

/**
  This function retrieves hash store pointer.

  @retval    The hash store pointer.

**/
VOID *
EFIAPI
GetHashStorePtr (
  VOID
  );

/**
  Match a given hash with the ones in hash store.

  @param[in]  Usage       Hash usage.
  @param[in]  HashData    Hash data pointer.
  @param[in]  HashAlg     Hash algorithm.

  @retval RETURN_SUCCESS             Found a match in hash store.
  @retval RETURN_INVALID_PARAMETER   HashData is NULL.
  @retval RETURN_NOT_FOUND           Hash data is not found.

**/
RETURN_STATUS
MatchHashInStore (
  IN       UINT32           Usage,
  IN       UINT8            HashAlg,
  IN       UINT8           *HashData
  );

#endif
