/** @file
  This file defines the hob structure for the OS boot configuration.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __OS_BOOT_OPTION_GUID_H__
#define __OS_BOOT_OPTION_GUID_H__
#include  <Uefi/UefiBaseType.h>

#pragma pack(1)

#define MAX_FILE_PATH_LEN          16
#define MAX_EXTRA_IMAGE_NUM        4

#define BOOT_FLAGS_MISC            BIT0
#define BOOT_FLAGS_CRASH_OS        BIT1
#define BOOT_FLAGS_TRUSTY          BIT2
#define BOOT_FLAGS_EXTRA           BIT3

// This bit is used dynamically.
#define LOAD_IMAGE_FROM_BACKUP     BIT7

#define LOAD_IMAGE_NORMAL          0
#define LOAD_IMAGE_TRUSTY          1
#define LOAD_IMAGE_MISC            2
#define LOAD_IMAGE_EXTRA0          3
#define LOAD_IMAGE_EXTRA1          4
#define LOAD_IMAGE_EXTRA2          5
#define LOAD_IMAGE_EXTRA3          6

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


typedef struct {
  ///
  /// Used for image from file system, Ascii file name
  /// If FileName[0] is zero, means this entry is invalid
  ///
  UINT8                FileName[MAX_FILE_PATH_LEN];
  ///
  /// Used for image from raw partition
  ///
  LBA_IMAGE_LOCATION   LbaImage;
} BOOT_IMAGE;

typedef struct {

  ///
  /// Image type for Image[0]. Refer BOOT_IMAGE_TYPE
  ///
  UINT8                ImageType;

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
  // Image[1] is for trusty OS
  // Image[2] is for misc image
  // Image[3-6] is for extra Images
  BOOT_IMAGE           Image[7];
} OS_BOOT_OPTION;


typedef struct {
  UINT8          Revision;
  UINT8          ResetReason;
  UINT8          BootOptionReset:1;
  UINT8          BootToShell:1;
  UINT8          CurrentBoot:6;
  UINT8          OsBootOptionCount;
  OS_BOOT_OPTION OsBootOption[0];
} OS_BOOT_OPTION_LIST;

#pragma pack()

#endif
