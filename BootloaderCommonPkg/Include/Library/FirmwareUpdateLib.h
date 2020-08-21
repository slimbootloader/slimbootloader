/** @file
The header file for firmware update library.

Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FIRMWARE_UPDATE_LIB_H__
#define __FIRMWARE_UPDATE_LIB_H__

#include <PiPei.h>
#include <Guid/FlashMapInfoGuid.h>
#include <IndustryStandard/Acpi30.h>
#include <Guid/SystemResourceTable.h>

#define CMOS_ADDREG             0x70
#define CMOS_DATAREG            0x71

#define MAX_UPDATE_REGIONS      4

#define FW_UPDATE_SM_INIT             0xFF
#define FW_UPDATE_SM_CAP_PROCESSING   0x7F
#define FW_UPDATE_SM_PART_A           0x7E
#define FW_UPDATE_SM_PART_B           0x7D
#define FW_UPDATE_SM_PART_AB          0x7C
#define FW_UPDATE_SM_DONE             0x77 // Lower 3 bits are ignored

#define FW_UPDATE_IMAGE_UPDATE_NONE         0xFF
#define FW_UPDATE_IMAGE_UPDATE_PENDING      0xFE
#define FW_UPDATE_IMAGE_UPDATE_PROCESSING   0xFC
#define FW_UPDATE_IMAGE_UPDATE_DONE         0xF8

#define FW_UPDATE_PARTITION_A   0
#define FW_UPDATE_PARTITION_B   1

#define FW_UPDATE_SIG_LENGTH    256

#define MAX_FILE_LEN            16
#define MAX_FW_COMPONENTS       3

#define CAPSULE_FLAGS_CFG_DATA  BIT0

#define FW_UPDATE_COMP_BIOS_REGION SIGNATURE_32('B', 'I', 'O', 'S')
#define FW_UPDATE_COMP_CSME_REGION SIGNATURE_32('C', 'S', 'M', 'E')
#define FW_UPDATE_COMP_CSME_DRIVER SIGNATURE_32('C', 'S', 'M', 'D')
#define FW_UPDATE_COMP_CMD_REQUEST SIGNATURE_32('C', 'M', 'D', 'I')


#define FW_UPDATE_STATUS_SIGNATURE SIGNATURE_32 ('F', 'W', 'U', 'S')
#define FW_UPDATE_STATUS_VERSION   0x1

///
/// "FWST"  Firmware Update status data Table
/// This table contains pointer to the ESRT (EFI System Resource Table)structure
///
#define EFI_FIRMWARE_UPDATE_STATUS_TABLE_SIGNATURE  SIGNATURE_32('F', 'W', 'S', 'T')

#define ESRT_FIRMWARE_RESOURCE_VERSION    0x1

#define CREATOR_INTEL_OEM_ID        'I','N','T','E','L',' '
#define CREATOR_INTEL_OEM_TABLE_ID  SIGNATURE_64('F','W','U','P','D','S','T','S')
#define CREATOR_ID_INTEL            0x4C544E49              // "INTL"(Intel)
#define CREATOR_REV_INTEL           0x20090903
#define ACPI_FWST_OEM_REV           0x00001000

typedef enum {
  TopSwapSet,
  TopSwapClear
} TOP_SWAP_OPERATION;

typedef enum {
  PrimaryPartition,
  BackupPartition
} BOOT_PARTITION;

#pragma pack(push, 1)
//
// Firmware Update Status ACPI structure
// This structure has a generic address structure
// which contains the pointer to ESRT structure.
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER   Header;
  EFI_SYSTEM_RESOURCE_TABLE     EsrtTablePtr;
  EFI_SYSTEM_RESOURCE_ENTRY     EsrtTableEntry[MAX_FW_COMPONENTS];
} EFI_FWST_ACPI_DESCRIPTION_TABLE;

//
// Firmware Update status structure
// This structure maintains the firmware update status
// in the non volatile reserved region of Slim Bootloader
// ESRT ACPI table will be populated based on this structure
//
typedef struct {
  UINT32                Signature;
  UINT16                Version;
  UINT16                Length;
  UINT8                 CapsuleSig[FW_UPDATE_SIG_LENGTH];
  UINT8                 StateMachine;
  UINT8                 Reserved[7];
} FW_UPDATE_STATUS;

typedef struct {
  EFI_GUID              FirmwareId;
  UINT64                HardwareInstance;
  UINT32                LastAttemptVersion;
  UINT32                LastAttemptStatus;
  UINT8                 UpdatePending;
  UINT8                 Reserved[3];
} FW_UPDATE_COMP_STATUS;

typedef union _FIRMWARE_UPDATE_POLICY {
  UINT32 Data;
  struct {
    UINT32  StateMachine       : 8;
    UINT32  UpdatePartitionA   : 1;
    UINT32  UpdatePartitionB   : 1;
    UINT32  SwitchtoBackupPart : 1;
    UINT32  Reboot             : 1;
  } Fields;
} FIRMWARE_UPDATE_POLICY;

//
// Capsule image header for firmware update
//
typedef struct {
  EFI_GUID                    FileGuid;
  UINT32                      HeaderSize;
  UINT32                      FirmwreVersion;
  UINT32                      CapsuleFlags;
  UINT32                      PubKeyOffset;
  UINT32                      PubKeySize;
  UINT32                      ImageOffset;
  UINT32                      ImageSize;
  UINT32                      SignatureOffset;
  UINT32                      SignatureSize;
  UINT32                      Reserved[3];
} FIRMWARE_UPDATE_HEADER;

typedef struct {
  UINT32                      Version;
  UINT16                      EmbeddedDriverCount;
  UINT16                      PayloadItemCount;
// UINT64 ItemOffsetList[];
} EFI_FW_MGMT_CAP_HEADER;

typedef struct {
  UINT32                      Version;
  EFI_GUID                    UpdateImageTypeId;
  UINT8                       UpdateImageIndex;
  UINT8                       reserved_bytes[3];
  UINT32                      UpdateImageSize;
  UINT32                      UpdateVendorCodeSize;
  UINT64                      UpdateHardwareInstance;
} EFI_FW_MGMT_CAP_IMAGE_HEADER;

//
// Region information for firmware update
//
typedef struct {
  UINT64                      ToUpdateAddress;
  UINT32                      UpdateSize;
  UINT8                       *SourceAddress;
} FIRMWARE_UPDATE_REGION;

//
// Partition information for firmware update
// It may include multiple regions
//
typedef struct {
  UINT32                      RegionCount;
  FIRMWARE_UPDATE_REGION      FwRegion[1];
} FIRMWARE_UPDATE_PARTITION;

#pragma pack(pop)

#define CAPSULE_IMAGE_SIZE(h)   ((h)->HeaderSize + (h)->PubKeySize + (h)->ImageSize + (h)->SignatureSize)
#define COMP_STATUS_OFFSET(x, y)   ((x) + sizeof(FW_UPDATE_STATUS) + ((y) * sizeof(FW_UPDATE_COMP_STATUS)))

typedef  VOID   (*DRIVER_ENTRY) (VOID *Params);

/**
  Get capsule image for firmware update.

  This function read firmware update capsule image starting with firmware update
  header. It could be read from EMMC, UFS, USB, SATA, etc. block device. Often the
  Capsule image could be saved in the root directory of a FAT system.

  @param[out] FwBuffer        The firmware update capsule image.
  @param[out] FwSize          The capsule image size.

  @retval  EFI_SUCCESS        Get the capsule image successfully.
  @retval  others             Error happening when getting capsule image.
**/
EFI_STATUS
EFIAPI
GetCapsuleImage (
  OUT  VOID                      **FwBuffer,
  OUT  UINT32                    *FwSize
  );

/**
  Get offset of Stage 1A in the capsule image

  This function will get stage 1A base address from flash map.
  Computes offset in the BIOS region from the base address.
  Then it calculates base address of stage1A in the capsule image.

  @param[in]  ImageHdr        Pointer to Fw Mgmt capsule Image header
  @param[in]  IsBackupPartition TRUE for Back up copy, FALSE for primary copy
  @param[out] Base            Base address of the component
  @param[out] Size            Size of the component

  @retval  EFI_SUCCESS        Image offset returned successfully.
  @retval  EFI_NOT_FOUND      Could not get component information from flash map.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
PlatformGetStage1AOffset (
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN  BOOLEAN    IsBackupPartition,
  OUT UINT32     *Base,
  OUT UINT32     *Size
  );

/**
  Get details information on how to update a boot partition.

  Platform knows the capsule image and the boot device layout. By parsing capsule
  Image and boot media, the platform could produce a list of regions to be updated.

  Platform could set partition flag to decide if a reboot is required after a boot
  partition is updated.
  Platform could set region flag to indicate if the source image is the final image
  to write to boot media. If the flag is set, that source will be used to check if
  the source is same before doing firmware update.

  @param[in]  ImageHdr        Pointer to Fw Mgmt capsule Image header
  @param[in]  FwPolicy        Firmware update policy.
  @param[out] PartitionInfo   The detail information on the partition to update

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
GetFirmwareUpdateInfo (
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN  FIRMWARE_UPDATE_POLICY     FwPolicy,
  OUT FIRMWARE_UPDATE_PARTITION  **PartitionInfo
  );


/**
  Platform hook point before starting to write a boot partition.

  This function will do some platform specific things before starting to write
  data to boot media. e.g. Send other component message it is going to do boot
  media update.

  @param[in]  UpdatePartition        The firmware update capsule image.

**/
EFI_STATUS
EFIAPI
PrepareRegionsUpdate (
  IN FIRMWARE_UPDATE_PARTITION   *UpdatePartition
  );

/**
  Platform hook point after firmware update is done.

  This function will do some platform specific things after all new firmware
  is written to boot media. e.g. set boot mode to normal boot mode if platform
  support it.

**/
EFI_STATUS
EFIAPI
PlatformEndFirmwareUpdate (
  VOID
  );

/**
  Read the data from BootMedia.

  @param[in] Address          The boot media address to be read.
  @param[in] ByteCount        The size in the bytes to read from media.
  @param[out] Buffer          The Destination buffer to read the boot media.

  @retval  EFI_SUCCESS        Read successfully.
  @retval  others             Error happening when Reading.
**/
EFI_STATUS
EFIAPI
BootMediaRead (
  IN     UINT64                  Address,
  IN     UINT32                  ByteCount,
  OUT    UINT8                   *Buffer
  );

/**
  Write the data to BootMedia.

  @param[in] Address          The boot media address to be Write.
  @param[in] ByteCount        The size in the bytes to write onto media.
  @param[out] Buffer          The Destination buffer to read the boot media.

  @retval  EFI_SUCCESS        Read successfully.
  @retval  others             Error happening when Reading.
**/
EFI_STATUS
EFIAPI
BootMediaWrite (
  IN     UINT64                  Address,
  IN     UINT32                  ByteCount,
  OUT    UINT8                   *Buffer
  );

/**
  Erase the data on the BootMedia.

  @param[in] Address          The boot media address to be Erased.
  @param[in] ByteCount        The size in the bytes to Erase on media.

  @retval  EFI_SUCCESS        Read successfully.
  @retval  others             Error happening when Reading.
**/
EFI_STATUS
EFIAPI
BootMediaErase (
  IN     UINT64                  Address,
  IN     UINT32                  ByteCount
  );

/**
  Initializes input structure for csme update driver.

  This function will initialize input structure for csme
  update driver. Since HECI functionality is avaiable to access
  only in silicon driver, this init is done in silicon package.
**/
VOID *
InitCsmeUpdInputData (
  VOID
  );

/**
  API to Initialize BootMedia.

**/
VOID
EFIAPI
InitializeBootMedia (
  VOID
  );

/**
  Get all region sizes from flash map.

  This function will get topswap, redundant and non redundant region sizes from
  flash map.

  @param[in,out] TopSwapRegionSize          The boot media address to be update.
  @param[in,out] RedundantRegionSize        The source buffer to write to the boot media.
  @param[in,out] NonRedundantRegionSize     The length of data to write to boot media.

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
GetRegionInfo (
  IN OUT UINT32     *TopSwapRegionSize,
  IN OUT UINT32     *RedundantRegionSize,
  IN OUT UINT32     *NonRedundantRegionSize
  );

/**
  Get region size from flash map.

  This function will get topswap, redundant and non redundant region sizes from
  flash map.

  @param[in] FlashMap         The boot media address to be update.
  @param[in] RegionFlag       The source buffer to write to the boot media.

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
UINT32
GetRegionSize (
  IN FLASH_MAP     *FlashMap,
  IN UINT8          RegionFlag
  );

/**
  Gets component information from the flash map by partition.

  This function will look for the component matching the input signature
  in the flash map, if found, it will look for the component with back up
  flag based on the backup partition parmeter and will return the
  offset and size of the component.

  @param[in]  Signature         Signature of the component information required
  @param[in]  Region            Look for the component in desired region
  @param[in]  IsBackupPartition TRUE for Back up copy, FALSE for primary copy
  @param[out] Offset            Offset of the component
  @param[out] Size              Size of the component

  @retval    EFI_SUCCESS    Found the component with the matching signature.
  @retval    EFI_NOT_FOUND  Component with the matching signature not found.

**/
EFI_STATUS
EFIAPI
FirmwareUpdateGetComponentInfo (
  IN  UINT32     Signature,
  IN  UINT8      Region,
  IN  BOOLEAN    IsBackupPartition,
  OUT UINT32     *Offset,
  OUT UINT32     *Size
  );

/**
  Get state machine flag from flash.

  This function will get state machine flag from the bootloader reserved region
  First byte in the booloader reserved region is state machine flag

  @param[in, out] StateMachine  Pointer to state machine flag byte.

**/
VOID
GetStateMachineFlag (
  IN OUT UINT8    *StateMachine
  );

/**
  Switch between the boot partitions.

  This function will use platform specific method of switching
  between primary and backup partitions.

  @param[in] Partition        Partition to select

  @retval  EFI_SUCCESS        Switched to desired partition successfully.
  @retval  others             Error happening.
**/
EFI_STATUS
SetBootPartition (
  IN BOOT_PARTITION  Partition
  );

/**
  This function will be called after the firmware update is complete.
  This function will update firmware update status structure in reserved region

  @param[in] Signature          Signature of component to update.
  @param[in] LastAttemptVersion Version of last firmware update attempted.
  @param[in] LastAttemptStatus  Status of last firmware update attempted.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
UpdateStatus (
  IN UINT64     Signature,
  IN UINT16     LastAttemptVersion,
  IN EFI_STATUS LastAttemptStatus
 );

/**
  Perform csme Firmware update.

  This function based on the image type id guid from the image header will
  call the respective functions to perform capsule update.

  @param[in] CapImage           The pointer to the firmware update capsule image.
  @param[in] CapImageSize       The size of capsule image in bytes.
  @param[in] CsmeUpdInputData   pointer to input data structure for CSME update
  @param[in] ImageHdr           Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error status from the update routine
**/
EFI_STATUS
UpdateCsme (
  IN  UINT8                         *CapImage,
  IN  UINT32                        CapImageSize,
  IN  VOID                          *CsmeUpdInputData,
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  );

/**
  Platform hook point to clear firmware update trigger.

  This function is responsible for clearing firmware update trigger.

**/
VOID
EFIAPI
ClearFwUpdateTrigger (
  VOID
  );


/**
  Flash descriptor region lock

  This function will do some command buffer parsing and check
  for additional parameters

  @param[in]  CmdDataBuf    Pointer to command buffer.
  @param[in]  CmdDataSize   size of command data.

  @retval  EFI_SUCCESS      Flash descriptor lock successfully.
  @retval  others           Error happening when updating.

**/
EFI_STATUS
EFIAPI
SetFlashDescriptorLock (
   IN  CHAR8     *CmdDataBuf,
   IN  UINTN     CmdDataSize
   );

/**
  Anti Rollback Svn Commit

  @param[in]  CmdDataBuf    Pointer to command buffer.
  @param[in]  CmdDataSize   size of command data.

  @retval  EFI_SUCCESS      ARB Svn commit is successful.
  @retval  others           Error happened while doing commit.

**/
EFI_STATUS
EFIAPI
SetArbSvnCommit (
   IN  CHAR8     *CmdDataBuf,
   IN  UINTN     CmdDataSize
   );
#endif
