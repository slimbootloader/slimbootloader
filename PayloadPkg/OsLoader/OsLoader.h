/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __LINUX_LOADER_H__
#define __LINUX_LOADER_H__

#include <Library/PartitionLib.h>
#include <Library/FileSystemLib.h>
#include <Library/PayloadMemoryAllocationLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/PayloadLib.h>
#include <Library/ShellLib.h>
#include <Library/IasImageLib.h>
#include <Library/MultibootLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/PayloadEntryLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/HobLib.h>
#include <Library/PrintLib.h>
#include <Library/Crc32Lib.h>
#include <Library/PcdLib.h>
#include <Library/LinuxBzimage.h>
#include <Library/BootOptionLib.h>
#include <Library/TpmLib.h>
#include <Library/VariableLib.h>
#include <Library/MmcTuningLib.h>
#include <Library/ShellExtensionLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/RpmbLib.h>
#include <Library/GraphicsLib.h>
#include <Library/SerialPortLib.h>
#include <Library/ConsoleInLib.h>
#include <Library/ConsoleOutLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/AbSupportLib.h>
#include <Library/SblParameterLib.h>
#include <Library/TrustyBootLib.h>
#include <Library/UsbKbLib.h>
#include <Library/ElfLib.h>
#include <Guid/SeedInfoHobGuid.h>
#include <Guid/OsConfigDataHobGuid.h>
#include <Guid/OsBootOptionGuid.h>
#include <Library/SeedListInfoLib.h>
#include <Guid/FrameBufferInfoGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Service/PlatformService.h>
#include <IndustryStandard/Mbr.h>
#include <Uefi/UefiGpt.h>
#include "BlockIoTest.h"
#include <ConfigDataCommonDefs.h>

#define MKHI_BOOTLOADER_SEED_LEN       64

#define DEFAULT_COMMAND_LINE     "console=ttyS0,115200\0"

#define BOOT_PARAMS_BASE         0x00090000
#define LINUX_KERNEL_BASE        0x00100000
#define IMAGE_MAX_SIZE           0x00F00000
#define CMDLINE_OFFSET           0xF000
#define CMDLINE_LENGTH_MAX       0x800
#define EOF                      "<eof>"
#define GPT_PART_ENTRIES_MAX     4

#define LOADED_IMAGE_IAS         BIT0
#define LOADED_IMAGE_MULTIBOOT   BIT1
#define LOADED_IMAGE_LINUX       BIT2
#define LOADED_IMAGE_PE32        BIT3

#define MAX_EXTRA_FILE_NUMBER    16

typedef struct {
  IMAGE_DATA              BootFile;
  IMAGE_DATA              CmdFile;
  IMAGE_DATA              InitrdFile;
  UINT16                  Reserved;
  UINT16                  ExtraBlobNumber;
  IMAGE_DATA              ExtraBlob[MAX_EXTRA_FILE_NUMBER];
  BOOT_PARAMS             *BootParams;
} LINUX_IMAGE;

typedef struct {
  IMAGE_DATA              BootFile;
  IMAGE_DATA              CmdFile;
} COMMON_IMAGE;


typedef union {
  COMMON_IMAGE            Common;
  MULTIBOOT_IMAGE         MultiBoot;
  LINUX_IMAGE             Linux;
} LOADED_IMAGE_TYPE;

typedef struct {
  UINT8                   Flags;
  UINT8                   LoadImageType;
  UINT16                  Reserved;
  IMAGE_DATA              IasImage;
  EFI_HANDLE              HwPartHandle;
  LOADED_IMAGE_TYPE       Image;
  UINT8                   ImageHash[SHA256_DIGEST_SIZE];
} LOADED_IMAGE;


/**
Print out the Multiboot information block.

@param[in]  Mi  The Multiboot information block to be printed.

**/
VOID
DumpMbInfo (
  IN  CONST MULTIBOOT_INFO *Mi
  );

/**
  Print out the Multiboot boot state.

  @param[in]  BootState  The Multiboot boot state pointer.
**/
VOID
DumpMbBootState (
  IN  CONST IA32_BOOT_STATE  *BootState
  );

/**
  Dumps kernel boot parameters.

  @param[in] Bp  Pointer to boot parameter structure.
 **/
VOID
DumpBootParameters (
  IN BOOT_PARAMS             *Bp
  );

/**
  Dumps the content of the buffer starting from Address.

  @param[in]  Address   Start address of buffer to dump
  @param[in]  Length    Size of the buffer to dump

 **/
VOID
DumpBuffer (
  IN CHAR8                   *Address,
  IN UINT32                  Length
  );

/**
  Print the OS boot option list.

  @param[in]  OsBootOptionList    the OS boot option list

**/
VOID
PrintBootOptions (
  OS_BOOT_OPTION_LIST        *OsBootOptionList
  );

/**
  Load Image from boot media.

  This function will initialize OS boot device, and load image based on
  boot option, the loaded image info will be saved in  LoadedImage.

  @param[in]  BootOption     Current boot option
  @param[out] LoadedImage    Loaded Image information.

  @retval  RETURN_SUCCESS    If image was loaded successfully
  @retval  Others            If image was not loaded.
**/
EFI_STATUS
GetImageFromMedia (
  IN  OS_BOOT_OPTION         *BootOption,
  OUT LOADED_IMAGE           *LoadedImage
  );

/**
  Get hardware partition handle from boot option info

  This function will initialize boot device, and get hardware partition
  handle based on boot option.

  @param[in]  BootOption      Current boot option
  @param[out] HwPartHandle    Hardware partition handle for boot image

  @retval  RETURN_SUCCESS     If partition was found successfully
  @retval  Others             If partition was not found
**/
EFI_STATUS
FindBootPartition (
  IN  OS_BOOT_OPTION         *BootOption,
  OUT EFI_HANDLE             *HwPartHandle
  );

/**
  Setup linux kernel and boot parameter table

  This function will check if boot image is bzImage firstly.
  If yes, will copy kernel to address 0x100000, and copy boot parameters
  to 0x90000. And also update boot parametere fields.

  @param[in]  LinuxImage        Loaded Linux image information.

  @retval  RETURN_SUCCESS       Setup bzImage kernel and boot parameter successfully
  @retval  Others               There is error when setup bzImage and boot parameters
**/
EFI_STATUS
SetupBzImage (
  IN LINUX_IMAGE             *LinuxImage
  );

/**
  Wrapper function to print LinuxLoader Measure Point information.
**/
VOID
PrintLinuxMeasurePoint (
  VOID
  );

/**
  Get command line arguments from the config file.

  This function will read command line arguments and put
  it into Buffer.

  @param[out] Buffer      Buffer to get command line arguments
  @param[in]  BufferLen   Buffer size
  @param[in]  File        Pointer to the config file
  @param[in]  FileLen     File size

  @retval Number of bytes read from the config file.
**/
UINTN
GetFromConfigFile (
  OUT UINT8                  *Buffer,
  IN  UINT32                  BufferLen,
  IN  UINT8                  *File,
  IN  UINT32                  FileLen
  );

/**
  Update OS boot parameters

  This function will append reqired command line parameters,
  and update mem map info.

  @param[in]     BootOption        Current boot option
  @param[in,out] LoadedImage       Normal OS boot image
  @param[in,out] LoadedTrustyImage Trusty OS image
  @param[in,out] LoadedExtraImages Extra OS images

  @retval   RETURN_SUCCESS         If update OS parameter success
  @retval   Others                 Failed to update OS parameters
**/
EFI_STATUS
UpdateOsParameters (
  IN     OS_BOOT_OPTION      *BootOption,
  IN OUT LOADED_IMAGE        *LoadedImage,
  IN OUT LOADED_IMAGE        *LoadedTrustyImage,
  IN OUT LOADED_IMAGE        *LoadedExtraImages
  );

/**
  Get the boot option index to start to boot with.

  @param[in]  OsBootOptionList    the OS boot option list
  @param[in]  BootOptionIndex     Previous boot option index.

  @retval     New boot option index
**/
UINT8
GetCurrentBootOption (
  IN OS_BOOT_OPTION_LIST     *OsBootOptionList,
  IN UINT8                   BootOptionIndex
  );

/**
  Get the next boot option index to boot with.

  @param[in]  OsBootOptionList    the OS boot option list
  @param[in]  BootOptionIndex     Current boot option index.

  @retval     New boot option index
**/
UINT8
GetNextBootOption (
  IN OS_BOOT_OPTION_LIST     *OsBootOptionList,
  IN UINT8                   BootOptionIndex
  );

/**
  ASM function that goes into kernel image.

  @param  KernelStart        Pointer to the start of kernel.
  @param  KernelBootParams   Pointer to the boot parameter structure.

 **/
VOID
EFIAPI
JumpToKernel (
  VOID *KernelStart,
  VOID *KernelBootParams
  );

/**
  Seed Sanity check to check seed HOB validity before passing to OS.

  This is needed to ensure that only the correct seeds are passed from Stage2 HOB buffer
  to OS based on the Image type( OS, Trusty flags) for current target boot index[0].

  1.native android (AOS loader) without Trusty OS (evmm) = only rpmb needed
  2.native android (AOS loader) with Trusty OS           = dseed + rpmb
  3.Clear linux without Trusty                           = useed + dseed
  4.Clear Linux with Trusty (no AOS loader)              = all (useed/dseed/rpmb keys)
  5.ACRN                                                 = all

  @param[in]     CurrentBootOption        Current boot option
  @param[in,out] SeedList                 Pointer to the Seed list

  @retval        EFI_SUCCESS              Successfully checked the seed validity.
  @retval        EFI_NOT_FOUND            Couldnt retrieve seeds or couldnt find bootoptionslist.

**/
EFI_STATUS
SeedSanityCheck (
  IN     OS_BOOT_OPTION      *CurrentBootOption,
  IN OUT LOADER_SEED_LIST    *SeedList
  );

/**
  Check the eMMC storage serial number validity.

  If card serial number != SPI flash serial number, then
  save the new serial number from the card to SPI flash and
  warm reset the platform.

  @retval  EFI_SUCCESS       Successfully checked and saved the serial number.
  @retval  EFI_NOT_FOUND     Couldnt retrieve serial number either from card or flash.
  @retval  EFI_DEVICE_ERROR  Couldnt save the new serial number to SPI flash.
**/
EFI_STATUS
EmmcSerialNumCheck (
  VOID
  );

/**
  Provision the RPMB key into RPMB partition.

  Currently, there is only eMMC RPMB support. So this flow works
  only if DeviceType = eMMC.

  @param[in]     CurrentBootOption        Current boot option
  @param[in]     SeedList                 Pointer to the Seed list

  @retval        EFI_SUCCESS              Successfully provisioned the RPMB key.
  @retval        EFI_NOT_FOUND            Couldn't find PlatformService->CfgData or PlatformInfo
                                          or SeedList not found.
  @retval        EFI_UNSUPPORTED          RpmbProvisioning CfgData flag not enabled or SeedList
                                          feature not enabled.
  @retval        EFI_OUT_OF_RESOURCES     Couldn't allocate memory.
  @retval        Others                   Error occurred inside RPMB library.
**/
EFI_STATUS
RpmbKeyProvisioning (
  IN     OS_BOOT_OPTION      *CurrentBootOption,
  IN     LOADER_SEED_LIST    *SeedList
  );

#endif
