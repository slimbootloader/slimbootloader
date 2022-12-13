/** @file

  Copyright (c) 2017 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __OS_LOADER_H__
#define __OS_LOADER_H__

#include <Library/LiteFvLib.h>
#include <Library/PartitionLib.h>
#include <Library/TimerLib.h>
#include <Library/FileSystemLib.h>
#include <Library/PayloadMemoryAllocationLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/DecompressLib.h>
#include <Library/PayloadLib.h>
#include <Library/ShellLib.h>
#include <Library/MultibootLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/PayloadEntryLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/HobLib.h>
#include <Library/PrintLib.h>
#include <Library/Crc32Lib.h>
#include <Library/PcdLib.h>
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
#include <Library/UsbInitLib.h>
#include <Library/UsbKbLib.h>
#include <Library/ElfLib.h>
#include <Library/UniversalPayloadLib.h>
#include <Library/LinuxLib.h>
#include <Library/ThunkLib.h>
#include <Library/MpServiceLib.h>
#include <Library/ContainerLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Guid/SeedInfoHobGuid.h>
#include <Guid/OsConfigDataHobGuid.h>
#include <Guid/OsBootOptionGuid.h>
#include <Library/SeedListInfoLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Service/PlatformService.h>
#include <IndustryStandard/Mbr.h>
#include <Uefi/UefiGpt.h>
#include <PayloadModule.h>
#include "BlockIoTest.h"
#include <ConfigDataCommonDefs.h>
#include <Register/Intel/Msr/ArchitecturalMsr.h>
#include <Library/StringSupportLib.h>
#include <PreOsHeader.h>


#define MKHI_BOOTLOADER_SEED_LEN       64

#define DEFAULT_COMMAND_LINE     "console=ttyS0,115200\0"

#define EOF                      "<eof>"
#define GPT_PART_ENTRIES_MAX     4

// For LOADED_IMAGE Flags
#define LOADED_IMAGE_IAS         BIT0
#define LOADED_IMAGE_MULTIBOOT   BIT1
#define LOADED_IMAGE_LINUX       BIT2
#define LOADED_IMAGE_PE          BIT3
#define LOADED_IMAGE_FV          BIT4
#define LOADED_IMAGE_CONTAINER   BIT5
#define LOADED_IMAGE_COMPONENT   BIT6
#define LOADED_IMAGE_RUN_EXTRA   BIT7
#define LOADED_IMAGE_ELF         BIT8
#define LOADED_IMAGE_MULTIBOOT2  BIT9

#define MAX_EXTRA_FILE_NUMBER    16

#define MAX_BOOT_MENU_ENTRY      8
#define MAX_STR_SLICE_LEN        16

#define PLD_EXTRA_MOD_RTCM       SIGNATURE_32('R', 'T', 'C', 'M')

typedef struct {
  UINT32       Pos;
  UINT32       Len;
  CHAR8        Buf[MAX_STR_SLICE_LEN];
} STR_SLICE;

typedef struct {
  STR_SLICE    Name;
  STR_SLICE    Command;
  STR_SLICE    Kernel;
  STR_SLICE    InitRd;
} MENU_ENTRY;

typedef struct {
  UINT32       Default;
  UINT32       Timeout;
} CFG_SETTING;

typedef struct {
  UINT32       EntryNum;
  CFG_SETTING  Settings;
  MENU_ENTRY   MenuEntry[MAX_BOOT_MENU_ENTRY];
} LINUX_BOOT_CFG;

typedef struct {
  IMAGE_DATA              BootFile;
  IMAGE_DATA              CmdFile;
  IMAGE_DATA              InitrdFile;
  UINT16                  Reserved;
  UINT16                  ExtraBlobNumber;
  IMAGE_DATA              ExtraBlob[MAX_EXTRA_FILE_NUMBER];
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
  UINT16                  Flags;
  UINT8                   LoadImageType;
  UINT8                   Reserved;
  IMAGE_DATA              ImageData;
  EFI_HANDLE              HwPartHandle;
  LOADED_IMAGE_TYPE       Image;
  UINT8                   ImageHash[HASH_DIGEST_MAX];
  RESERVED_CMDLINE_DATA   ReservedCmdlineData;
} LOADED_IMAGE;

/**
OS Loader module entry point. Can also be used to get the
base address of the OS Loader's location in memory.

**/
VOID
_ModuleEntryPoint (
  VOID
  );

/**
Print out the Multiboot information block.

@param[in]  Mi  The Multiboot information block to be printed.

**/
VOID
DumpMbInfo (
  IN  CONST MULTIBOOT_INFO *Mi
  );

/**
Print out the Multiboot-2 information block.

@param[in]  Mi  The Multiboot-2 information block to be printed.

**/
VOID
DumpMb2Info (
  IN  CONST MULTIBOOT2_START_TAG *Mi
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
  Print the OS boot option list.

  @param[in]  OsBootOptionList    the OS boot option list

**/
VOID
PrintBootOptions (
  OS_BOOT_OPTION_LIST        *OsBootOptionList
  );

/**
  Get a pointer of Loaded Image which has specific LoadImageType

  This function will return the pointer address of a Loaded Image with given
  LoadImageType.

  @param[in]  LoadedImageHandle       Loaded Image handle
  @param[in]  LoadImageType           Load Image Type Index
  @param[out] LoadedImage             Loaded Image

  @retval     EFI_SUCCESS             Found a Loaded Image from the handle successfully
  @retval     EFI_INVALID_PARAMETER   If Loaded Image handle is invalid
  @retval     EFI_NOT_FOUND           Not found a Loaded Image from the handle

**/
EFI_STATUS
EFIAPI
GetLoadedImageByType (
  IN   EFI_HANDLE         LoadedImageHandle,
  IN   LOAD_IMAGE_TYPE    LoadImageType,
  OUT  LOADED_IMAGE     **LoadedImage
  );

/**
  Free all temporary resources used for Boot Image

  This function will clean up all temporary resources used to load Boot Image.

  @param[in]  LoadedImageHandle   Loaded Image handle.
  @param[in]  KeepRootNode        TRUE,  do not free memory for LOADED_IMAGES_INFO root node.
                                  FALSE, free memory for LOADED_IMAGES_INFO root node.
**/
VOID
EFIAPI
UnloadBootImages (
  IN  EFI_HANDLE       LoadedImageHandle,
  IN  BOOLEAN          KeepRootNode
  );

/**
  Free all allocated memory in a loaded image

  This function will clean up all temporary resources used to load a single image.

  @param[in]  LoadedImage     A load image pointer which has a boot image info

**/
VOID
UnloadLoadedImage (
  IN  LOADED_IMAGE  *LoadedImage
  );

/**
  Free the allocated memory in an image data

  This function free a memory allocated in IMAGE_DATA according to Allocation Type.

  @param[in]  ImageData       An image data pointer which has allocated memory address,
                              its size, and allocation type.

**/
VOID
FreeImageData (
  IN  IMAGE_DATA    *ImageData
  );

/**
  Load Image from boot media.

  This function will initialize OS boot device, and load image based on
  boot option, the loaded image info will be saved in  LoadedImage.

  @param[in]  BootOption        Current boot option
  @param[in]  HwPartHandle      Hardware partition handle
  @param[in]  FsHandle          FileSystem handle
  @param[out] LoadedImageHandle Loaded Image handle

  @retval     RETURN_SUCCESS    If image was loaded successfully
  @retval     Others            If image was not loaded.
**/
EFI_STATUS
EFIAPI
LoadBootImages (
  IN  OS_BOOT_OPTION  *OsBootOption,
  IN  EFI_HANDLE       HwPartHandle,
  IN  EFI_HANDLE       FsHandle,
  OUT EFI_HANDLE      *LoadedImageHandle
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
  @param[in,out] LoadedPreOsImage  Pre OS image
  @param[in,out] LoadedExtraImages Extra OS images

  @retval   RETURN_SUCCESS         If update OS parameter success
  @retval   Others                 Failed to update OS parameters
**/
EFI_STATUS
UpdateOsParameters (
  IN     OS_BOOT_OPTION      *BootOption,
  IN OUT LOADED_IMAGE        *LoadedImage,
  IN OUT LOADED_IMAGE        *LoadedPreOsImage,
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


/**
  Parse the Linux boot configuration file.

  @param[in]    CfgBuffer     The configuration buffer.
  @param[out]   LinuxBootCfg  The pointer to hold the parse results.

**/
VOID
ParseLinuxBootConfig (
  IN   CHAR8            *CfgBuffer,
  OUT  LINUX_BOOT_CFG   *LinuxBootCfg
  );


/**
  Print Linux boot options.

  @param[in]    CfgBuffer     The configuration buffer.
  @param[out]   LinuxBootCfg  The pointer to hold the parse results.

**/
VOID
PrintLinuxBootConfig (
  CHAR8                    *CfgBuffer,
  LINUX_BOOT_CFG           *LinuxBootCfg
  );

/**
  Get Linux boot option from user input.

  @param[in]    CfgBuffer     The configuration buffer.
  @param[out]   LinuxBootCfg  The pointer to hold the parse results.

  @retval       0 based boot option index.

**/
UINT32
GetLinuxBootOption (
  CHAR8                    *CfgBuffer,
  LINUX_BOOT_CFG           *LinuxBootCfg
  );

/**
  Initialize platform lcoal console.

  @param[in]  ForceFbConsole   Force to enable framebuffer.

  @retval  EFI_NOT_FOUND    No additional console was found.
  @retval  EFI_SUCCESS      Console has been initialized successfully.
  @retval  Others           There is error during console initialization.
**/
EFI_STATUS
LocalConsoleInit (
  IN  BOOLEAN   ForceFbConsole
);

/**
  Get a function pointer from the function name.

  @param[in]      FuncName    Function name

  @retval   Function pointer for the given funciton name.
  @retval   NULL   Failed to get the function address.

**/
VOID *
EFIAPI
GetProcAddress (
  IN CHAR8     *FuncName
  );

/**
  Initialize payload module serivce table and paramters.

  @param[in]      PldModParam   Payload module parameter pointer
  @param[in]      ModuleName    Payload module name

**/
VOID
EFIAPI
PayloadModuleInit (
  IN  PLD_MOD_PARAM       *PldModParam,
  IN  CHAR8               *ModuleName
  );

/**
  Print the stack/HOB and heap usage information.

**/
VOID
EFIAPI
PrintStackHeapInfo (
  VOID
  );

/**
  Start preOS boot image

  This function will call into preOS entry point with OS information as parameter.

  @param[in]  LoadedPreOsImage  Loaded PreOS image information.
  @param[in]  LoadedImage       Loaded OS image information.

  @retval  RETURN_SUCCESS       boot image is return after boot
  @retval  Others               There is error when checking boot image
**/
EFI_STATUS
StartPreOsBooting (
  IN LOADED_IMAGE            *LoadedPreOsImage,
  IN LOADED_IMAGE            *LoadedImage
  );


/**
  Call into an extra image entrypoint.

  Detect and call into the image entrypoint. If required, handle thunk call
  as well.

  @param[in]  ModSignature      Module signature.
  @param[in]  LoadedImage       Loaded Image information, expected to be PE32 format.

  @retval  EFI_SUCCESS          Image returns successfully
  @retval  Others               There is error during the module call.

**/
EFI_STATUS
CallExtraModule (
  IN   UINT32           ModSignature,
  IN   LOADED_IMAGE    *LoadedImage
  );

#endif
