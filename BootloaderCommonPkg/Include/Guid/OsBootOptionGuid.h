/** @file
  This file defines the hob structure for the OS boot configuration.

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __OS_BOOT_OPTION_GUID_H__
#define __OS_BOOT_OPTION_GUID_H__
#include  <Uefi/UefiBaseType.h>

#pragma pack(1)

#define MAX_FILE_PATH_LEN          16
#define MAX_EXTRA_IMAGE_NUM        4
#define MAX_BOOT_OPTION_ENTRY      32

#define BOOT_FLAGS_MISC            BIT0
#define BOOT_FLAGS_CRASH_OS        BIT1
#define BOOT_FLAGS_PREOS           BIT2
#define BOOT_FLAGS_EXTRA           BIT3
#define BOOT_FLAGS_MENDER          BIT4

// This bit is used dynamically.
#define LOAD_IMAGE_FROM_BACKUP     BIT7

typedef enum {
  LoadImageTypeNormal              = 0x0,
  LoadImageTypePreOs,
  LoadImageTypeMisc,
  LoadImageTypeExtra0,
  LoadImageTypeExtra1,
  LoadImageTypeExtra2,
  LoadImageTypeExtra3,
  LoadImageTypeMax
} LOAD_IMAGE_TYPE;

///
/// OS Boot Option GUID
///
extern EFI_GUID gOsBootOptionGuid;

//
// Enumeration of reset reason.
//
typedef enum {
  ResetCold     = 0,
  ResetTcoWdt   = BIT0,
  ResetWarm     = BIT1,
  ResetPowerOn  = BIT2,
  ResetGlobal   = BIT3,
  ResetWakeS3   = BIT4,
  ResetUnknown  = BIT5,
  Reserved      = BIT6 | BIT7
} RESET_CAUSE;

// Define OS boot media devices
typedef enum {
  OsBootDeviceSata,
  OsBootDeviceSd,
  OsBootDeviceEmmc,
  OsBootDeviceUfs,
  OsBootDeviceSpi,
  OsBootDeviceUsb,
  OsBootDeviceNvme,
  OsBootDeviceMemory,
  OsBootDeviceMax
} OS_BOOT_MEDIUM_TYPE;

// Define other platform devices as an extention to OS_BOOT_MEDIUM_TYPE
typedef enum {
  PlatformDeviceMin = OsBootDeviceMax,
  PlatformDeviceGraphics,
  PlatformDeviceMax
} PLATFORM_DEVICE_TYPE;

typedef enum  {
  EnumFileSystemTypeFat,
  EnumFileSystemTypeExt2,
  EnumFileSystemTypeAuto,
  EnumFileSystemMax
} OS_FILE_SYSTEM_TYPE;

typedef enum  {
  EnumImageTypeDefault,
  EnumImageTypeAdroid,
  EnumImageTypeClearLinux,
  EnumImageTypeAcrn,
  EnumImageTypeFastboot,
  EnumImageTypeMax
} BOOT_IMAGE_TYPE;

typedef enum  {
  EnumPreOsTypeDefault     = 0x0,
  EnumPreOsTypeTrustyOs,
  EnumPreOsTypeMax
} PREOS_IMAGE_TYPE;

typedef struct {
  //
  // Indicate this entry is invalid or not
  //
  UINT8                Valid;

  ///
  /// Zero-based software partition number for boot image
  ///
  UINT8                SwPart;

  UINT16               Reserved;

  ///
  /// Lba offset address relative to partition
  ///
  UINT32               LbaAddr;
} LBA_IMAGE_LOCATION;

///
/// Only used to load image from firmware boot media (e.g. SPI flash)
///
typedef struct {
  ///
  /// Should be '!'
  ///
  UINT8                Indicate;
  ///
  /// The signature of the container
  ///
  UINT32               ContainerSig;
  ///
  /// should be '/'
  ///
  UINT8                BackSlash;
  ///
  /// The component name
  ///
  UINT32               ComponentName;
  ///
  /// Optional, should set to ':' if Parameter field exists.
  ///
  UINT8                Colon;
  ///
  /// The parameter to component image
  /// Available only on Colon == ':'
  ///
  UINT32               Parameter;
} CONTAINER_IMAGE;


typedef union {
  ///
  /// Used for image from file system, Ascii file name
  /// If FileName[0] is zero, means this entry is invalid
  ///
  UINT8                FileName[MAX_FILE_PATH_LEN];
  ///
  /// Used for image from raw partition
  ///
  LBA_IMAGE_LOCATION   LbaImage;
  ///
  ///
  ///
  CONTAINER_IMAGE      ContainerImage;
} BOOT_IMAGE;

typedef struct {

  ///
  /// Image type for Image[0]. Refer BOOT_IMAGE_TYPE
  ///
  UINT8                ImageType:5;

  ///
  /// Image type for Image[1]. Refer PREOS_IMAGE_TYPE
  ///
  UINT8                PreOsImageType:3;

  ///
  /// Zero means normal boot.
  ///
  UINT8                BootFlags;
  UINT8                Reserved;

  ///
  /// Boot medium type, Refer OS_BOOT_MEDIUM_TYPE
  ///
  UINT8                DevType;

  ///
  /// If there are multiple controllers, it indicate which
  /// controller instance the boot medium belong to.
  ///
  UINT8                DevInstance;

  ///
  /// Zero-based hardware partition number
  ///
  UINT8                HwPart;

  ///
  /// Zero-based software partition number for boot image
  /// Used for file system only.
  ///
  UINT8                SwPart;

  ///
  /// For File system support only, Refer OS_FILE_SYSTEM_TYPE
  ///
  UINT8                FsType;

  // Image[0] is for normal boot OS
  // Image[1] is for Pre OS
  // Image[2] is for misc image
  // Image[3-6] is for extra Images
  BOOT_IMAGE           Image[LoadImageTypeMax];
} OS_BOOT_OPTION;


typedef struct {
  UINT8          Revision;
  UINT8          ResetReason;
  UINT8          BootOptionReset:1;
  UINT8          BootToShell:1;
  UINT8          RestrictedBoot:1;
  UINT8          CurrentBoot:5;
  UINT8          OsBootOptionCount;
  OS_BOOT_OPTION OsBootOption[0];
} OS_BOOT_OPTION_LIST;

#pragma pack()

#endif
