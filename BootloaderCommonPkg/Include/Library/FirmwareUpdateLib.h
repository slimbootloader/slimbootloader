/** @file
The header file for firmware update library.

Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __FIRMWARE_UPDATE_LIB_H__
#define __FIRMWARE_UPDATE_LIB_H__

#include <Guid/FlashMapInfoGuid.h>

#define CMOS_ADDREG     0x70
#define CMOS_DATAREG    0x71

#define MAX_UPDATE_REGIONS   4

#define FW_UPDATE_SM_INIT     0xFF
#define FW_UPDATE_SM_PART_A   0xFE
#define FW_UPDATE_SM_PART_B   0xFD
#define FW_UPDATE_SM_PART_AB  0xFC

#define RTC_PORT_ID 0xC3
#define PCH_LPC_REG_P2SB_UNHIDE 0xE1
#define PCH_LPC_REG_P2SB_BAR 0x10

#define FW_UPDATE_PARTITION_A 0
#define FW_UPDATE_PARTITION_B 1

#define MAX_FILE_LEN          16

#define CAPSULE_FLAGS_CFG_DATA  BIT0

typedef enum {
  TopSwapSet,
  TopSwapClear
} TOP_SWAP_OPERATION;

typedef enum {
  PrimaryPartition,
  BackupPartition
} BOOT_PARTITION;

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

#define CAPSULE_IMAGE_SIZE(h)   ((h)->HeaderSize + (h)->PubKeySize + (h)->ImageSize + (h)->SignatureSize)

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

  @param[in]  FwImage         The firmware update capsule image.
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
  IN  UINT8      *FwImage,
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

  @param[in]  FwImage         The firmware update capsule image.
  @param[in]  FwPolicy        Firmware update policy
  @param[out] PartitionInfo   The detail informaion on the partition to update


  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
GetFirmwareUpdateInfo (
  IN  UINT8                      *FwImage,
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
EndFirmwareUpdate (
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

  @retval  EFI_SUCCESS        State machine flag found.
  @retval  others             Error while getting state machine flag.
**/
EFI_STATUS
GetStateMachineFlag (
  IN OUT UINT8    *StateMachine
  );

/**
  Set state machine flag in flash.

  This function will set state machine flag in the bootloader reserved region
  First byte in the booloader reserved region is state machine flag.

  @param[in] StateMachine     State machine flag byte.

  @retval  EFI_SUCCESS        State machine flag set.
  @retval  others             Error while setting state machine flag.
**/
EFI_STATUS
SetStateMachineFlag (
  IN UINT8    StateMachine
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
  This function will enforce firmware update policy.

  @param[in][out] FwPolicy    Pointer to Firmware update policy.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
EnforceFwUpdatePolicy (
  IN FIRMWARE_UPDATE_POLICY   *FwPolicy
  );

/**
  This function will enforce firmware update policy after
  partition update is successful

  After update firmware update policy

  @param[in] FwPolicy         Firmware update policy.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
AfterUpdateEnforceFwUpdatePolicy (
  IN FIRMWARE_UPDATE_POLICY   FwPolicy
  );
#endif
