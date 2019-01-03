/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "OsLoader.h"

UINT8    mCurrentBoot;
VOID    *mEntryStack;

/**
  Parse IAS image

  This function will parse IAS image and get every file blobs info, Especially
  get the command line file and ELF/kernel file for boot.

  @param[in]      BootOption    Current boot option
  @param[in, out] LoadedImage   Loaded Image information.

  @retval  RETURN_SUCCESS       Parse IAS image successfully
  @retval  Others               There is error when parsing IAS image.
**/
EFI_STATUS
ParseIasImage (
  IN     OS_BOOT_OPTION      *BootOption,
  IN OUT LOADED_IMAGE        *LoadedImage
  )
{
  IAS_HEADER                 *IasImage;
  UINT32                     NumFiles;
  IMAGE_DATA                 File[MAX_IAS_SUB_IMAGE];
  UINTN                      InitrdSize;
  VOID                       *InitrdData;
  UINT32                     ImageType;
  UINT16                     Index;
  UINT16                     ModuleIndex;
  LINUX_IMAGE                *LinuxImage;
  MULTIBOOT_IMAGE            *MultiBoot;
  COMMON_IMAGE               *CommonImage;
  PLATFORM_SERVICE           *PlatformService;
  EFI_STATUS                 Status;

  PlatformService = NULL;

  IasImage = IsIasImageValid (LoadedImage->IasImage.Addr, LoadedImage->IasImage.Size, LoadedImage->ImageHash);
  if (IasImage == NULL) {
    DEBUG ((DEBUG_INFO, "Image given is not a valid IAS image\n"));
    return EFI_LOAD_ERROR;
  }
  AddMeasurePoint (0x4080);

  ZeroMem (File, sizeof (File));
  NumFiles = IasGetFiles (IasImage, sizeof (File) / sizeof ((File)[0]), File);
  DEBUG ((DEBUG_INFO, "IAS size = 0x%x, file number: %d\n", LoadedImage->IasImage.Size, NumFiles));

  if (NumFiles == 1) {
    // This is not valid IAS use case, at least 2 files in IAS image.
    // Support this only for test
    CommonImage                = &LoadedImage->Image.Common;
    CommonImage->BootFile.Addr = File[0].Addr;
    CommonImage->BootFile.Size = File[0].Size;
    if (IsMultiboot (File[0].Addr)) {
      LoadedImage->Flags |= LOADED_IMAGE_MULTIBOOT;
      DEBUG ((DEBUG_INFO, "One multiboot file in IAS file .... \n"));
    } else if (IsTePe32Image (File[0].Addr, NULL) && \
               (* (UINT32 *)File[0].Addr == EFI_IMAGE_DOS_SIGNATURE)) {
      // Add extra check to ensure it is a PE32 image generated from payload build.
      // Please note vmlinuxz is also following PE32 format, but it should
      // be handled as Linux image boot path
      LoadedImage->Flags |= LOADED_IMAGE_PE32;
      DEBUG ((DEBUG_INFO, "One PE32 file in IAS file .... \n"));
    } else {
      LoadedImage->Flags |= LOADED_IMAGE_LINUX;
      DEBUG ((DEBUG_INFO, "One file in IAS file, take it as bzImage .... \n"));
    }
    return EFI_SUCCESS;
  }

  ImageType = IAS_IMAGE_TYPE (IasImage->ImageType);
  DEBUG ((DEBUG_INFO, "IAS Image Type = 0x%x\n", ImageType));
  if (ImageType == IAS_TYPE_CLASSIC) {
    // Files: cmdline, bzImage, initrd, acpi, firmware1, firmware2, ...
    LinuxImage                = &LoadedImage->Image.Linux;
    LoadedImage->Flags       |= LOADED_IMAGE_LINUX;
    LinuxImage->CmdFile.Addr  = File[0].Addr;
    LinuxImage->CmdFile.Size  = File[0].Size;
    LinuxImage->BootFile.Addr = File[1].Addr;
    LinuxImage->BootFile.Size = File[1].Size;

    // Make sure Initrd is page aligned.
    if (NumFiles > 2) {
      InitrdData = File[2].Addr;
      InitrdSize = File[2].Size;
      if ((((UINT32) InitrdData) & EFI_PAGE_MASK) != 0 && InitrdSize > 0) {
        InitrdData = AllocatePages (EFI_SIZE_TO_PAGES (InitrdSize));
        if (InitrdData == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem (InitrdData, File[2].Addr, InitrdSize);
      }
      LinuxImage->InitrdFile.Addr = InitrdData;
      LinuxImage->InitrdFile.Size = InitrdSize;
    }

    // Save other binary blobs
    Index = 3;
    while ((Index < MAX_MULTIBOOT_MODULE_NUMBER) && (Index < NumFiles)) {
      LinuxImage->ExtraBlob[Index - 3].Addr = File[Index].Addr;
      LinuxImage->ExtraBlob[Index - 3].Size = File[Index].Size;
      Index++;
    }
    LinuxImage->ExtraBlobNumber = Index;
  } else if (ImageType == IAS_TYPE_MULTIBOOT) {
    // Files: cmdline1, elf1, cmdline2, elf2, ...
    // Assume the first elf file is the one to boot
    MultiBoot                = &LoadedImage->Image.MultiBoot;
    LoadedImage->Flags      |= LOADED_IMAGE_MULTIBOOT;
    MultiBoot->CmdFile.Addr  = File[0].Addr;
    MultiBoot->CmdFile.Size  = File[0].Size;
    MultiBoot->BootFile.Addr = File[1].Addr;
    MultiBoot->BootFile.Size = File[1].Size;

    // Save other ELF pairs.
    ModuleIndex = 0;
    for (Index = 2; Index < NumFiles; Index += 2) {
      if (Index < MAX_MULTIBOOT_MODULE_NUMBER) {
        if (* (UINT32 *) File[Index].Addr == MULTIBOOT_SPECIAL_MODULE_MAGIC) {
          DEBUG ((DEBUG_INFO, "Loading IAS ACPI tables...\n"));
          if (PlatformService == NULL) {
            PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
            if ((PlatformService != NULL) && (PlatformService->AcpiTableUpdate != NULL)) {
              Status = PlatformService->AcpiTableUpdate (File[Index + 1].Addr, File[Index + 1].Size);
              DEBUG ((DEBUG_INFO, "Updating ACPI table with IAS image %d - %r\n", Index, Status));
            }
          }
          continue;
        }

        MultiBoot->MbModule[ModuleIndex].String = (UINT8 *) File[Index].Addr;
        MultiBoot->MbModule[ModuleIndex].Start  = (UINT32) File[Index + 1].Addr;
        MultiBoot->MbModule[ModuleIndex].End    = (UINT32) File[Index + 1].Addr + File[Index + 1].Size;
        ModuleIndex++;
      }
    }
    MultiBoot->MbModuleNumber = ModuleIndex;
  } else {
    DEBUG ((DEBUG_ERROR, "Error: unsupported image type #0x%X\n", ImageType));
    return EFI_UNSUPPORTED;
  }

  AddMeasurePoint (0x40A0);
  return EFI_SUCCESS;
}

/**
  Setup boot image

  This function will check boot image type, then setup boot parameters
  based it. Currently support ELF, multiboot, PE32 and bzImage format.

  @param[in, out]  LoadedImage  Loaded boot image information.

  @retval  RETURN_SUCCESS       Setup boot image successfully
  @retval  Others               There is error when setup boot image
**/
EFI_STATUS
SetupBootImage (
  IN OUT LOADED_IMAGE        *LoadedImage
  )
{
  EFI_STATUS                 Status;
  UINT32                     *EntryPoint;
  UINT8                      *NewCmdBuffer;
  MULTIBOOT_IMAGE            *MultiBoot;
  IMAGE_DATA                 *CmdFile;
  IMAGE_DATA                 *BootFile;

  //
  // Allocate a cmd line buffer and init it with config file or default value
  // Later, maybe there is new command line parameters appended
  //
  NewCmdBuffer = (UINT8 *) AllocatePages (EFI_SIZE_TO_PAGES (CMDLINE_LENGTH_MAX));
  if (NewCmdBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CmdFile = &LoadedImage->Image.Common.CmdFile;
  if (CmdFile->Size > 0) {
    CmdFile->Size = GetFromConfigFile (NewCmdBuffer, CMDLINE_LENGTH_MAX, (UINT8 *)CmdFile->Addr, CmdFile->Size);
  }
  if (CmdFile->Size == 0) {
    CopyMem (NewCmdBuffer, DEFAULT_COMMAND_LINE, AsciiStrSize (DEFAULT_COMMAND_LINE));
    CmdFile->Size = AsciiStrSize (DEFAULT_COMMAND_LINE);
  }
  CmdFile->Addr = NewCmdBuffer;

  // Setup Images
  BootFile = &LoadedImage->Image.Common.BootFile;

  MultiBoot = &LoadedImage->Image.MultiBoot;
  if (IsElfImage (BootFile->Addr)) {
    DEBUG ((DEBUG_INFO, "Boot image is ELF format...\n"));
    EntryPoint = LoadElfImage (BootFile->Addr);
    if (EntryPoint != NULL) {
      if (IsMultiboot (BootFile->Addr)) {
        DEBUG ((DEBUG_INFO, "and Image is Multiboot format\n"));
        SetupMultibootInfo (MultiBoot);
      }
      MultiBoot->BootState.EntryPoint = (UINT32)EntryPoint;
    }
  } else if (IsMultiboot (BootFile->Addr)) {
    DEBUG ((DEBUG_INFO, "Boot image is Multiboot format...\n"));
    Status = SetupMultibootImage (MultiBoot);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else if ((LoadedImage->Flags & LOADED_IMAGE_PE32) != 0) {
    DEBUG ((DEBUG_INFO, "Boot image is PE32 format\n"));
    Status = PeCoffRelocateImage ((UINT32)BootFile->Addr);
    if (!EFI_ERROR (Status)) {
      Status = PeCoffLoaderGetEntryPoint (BootFile->Addr, (VOID **)&EntryPoint);
    }
    if (EFI_ERROR (Status)) {
      return Status;
    }
    // Reuse MultiBoot structure to store the PE32 entry point information
    MultiBoot->BootState.EntryPoint = (UINT32)EntryPoint;
  } else {
    DEBUG ((DEBUG_INFO, "Assume BzImage...\n"));
    Status = SetupBzImage (&LoadedImage->Image.Linux);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Setup BzImage error, %r\n", Status));
      return Status;
    }
    LoadedImage->Flags  = (LoadedImage->Flags  & ~LOADED_IMAGE_MULTIBOOT) | LOADED_IMAGE_LINUX;
  }

  return EFI_SUCCESS;
}

/**
  Load Image from OS boot device.

  This function will initialize OS boot device if required, and load image
  based the information from BootOption, the loaded image info will be saved
  in LoadedImage.

  @param[in]  BootOption        Image information where to load image.
  @param[in, out] LoadedImage   Loaded Image information.

  @retval  RETURN_SUCCESS       Image is loaded successfully
  @retval  Others               Image is not loaded
**/
EFI_STATUS
LoadAndSetupImage (
  IN     OS_BOOT_OPTION      *BootOption,
  IN OUT LOADED_IMAGE        *LoadedImage
  )
{
  EFI_STATUS                 Status;
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;

  Status = GetImageFromMedia (BootOption, LoadedImage);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "GetImageFromMedia: Status = %r\n", Status));
    return Status;
  }
  AddMeasurePoint (0x4070);

  if ((LoadedImage->Flags & LOADED_IMAGE_IAS) != 0) {
    Status = ParseIasImage (BootOption, LoadedImage);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "ParseIasImage: Status = %r\n", Status));
      return Status;
    }
  }

  if ((LoadedImage->LoadImageType == LOAD_IMAGE_TRUSTY) || (LoadedImage->LoadImageType == LOAD_IMAGE_NORMAL)) {
    Status = SetupBootImage (LoadedImage);
  }

  DEBUG ((DEBUG_INFO, "SetupBootImage: Status = %r\n", Status));
  AddMeasurePoint (0x40B0);

  if (Status == EFI_SUCCESS) {
    LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GetLoaderPlatformInfoPtr();
    if (LoaderPlatformInfo == NULL) {
      return EFI_NOT_FOUND;
    }
    if (FeaturePcdGet (PcdMeasuredBootEnabled) && (LoaderPlatformInfo->SecurityFlags && BIT1)) {
      // Extend hash of the image into TPM.
      TpmExtendPcrAndLogEvent ( 8, TPM_ALG_SHA256, LoadedImage->ImageHash,
        EV_COMPACT_HASH, sizeof("LinuxLoaderPkg: OS Image"), (UINT8 *)"LinuxLoaderPkg: OS Image");
    }
  }
  return Status;
}

/**
  Print the stack/HOB and heap usage information.

**/
VOID
EFIAPI
PrintStackHeapInfo (
  VOID
  )
{
  EFI_STATUS       Status;
  UINT64           StartAddr;
  UINT64           FreeAddr;
  UINT64           EndAddr;
  UINT32           StackBot;
  UINT32           StackTop;
  UINT8            Idx;
  EFI_MEMORY_TYPE  MemoryType;

  StackTop = 0;
  for  (Idx = 0; Idx < 2; Idx++) {
    MemoryType = (Idx > 0) ? EfiReservedMemoryType : EfiBootServicesData;
    Status = GetMemoryResourceInfo (
               MemoryType,
               &StartAddr,
               &FreeAddr,
               &EndAddr
               );
    if (!EFI_ERROR (Status)) {
      if (Idx == 0) {
        StackTop = (UINT32)StartAddr;
      }
      DEBUG ((
               DEBUG_INFO,
               "Payload %a heap: 0x%X (0x%X used)\n",
               (Idx > 0) ? "reserved" : "normal",
               (UINT32) (EndAddr - StartAddr + 1),
               (UINT32) (EndAddr - FreeAddr)
               ));
    }
  }

  if (StackTop > 0) {
    StackBot = DetectUsedStackBottom (StackTop, PcdGet32 (PcdPayloadStackSize));
    DEBUG ((
             DEBUG_INFO,
             "Payload stack: 0x%X (0x%X used)\n\n",
             PcdGet32 (PcdPayloadStackSize),
             StackTop - StackBot
             ));
  }
}


/**
  This function will send FSP notification, indicate ReadyToBoot event to TPM and print
  performance data.

  @param[in]  Message        Last message before transfering to OS.

**/
VOID
BeforeOSJump (
  CHAR8 *Message
  )
{
  PLATFORM_SERVICE          *PlatformService;
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;
  DEBUG_LOG_BUFFER_HEADER   *LogBufHdr;

  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if ((PlatformService != NULL) && (PlatformService->NotifyPhase != NULL)) {
    PlatformService->NotifyPhase (ReadyToBoot);
    PlatformService->NotifyPhase (EndOfFirmware);
  }
  AddMeasurePoint (0x40F0);

  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GetLoaderPlatformInfoPtr();
  if (LoaderPlatformInfo == NULL) {
    return ;
  }
  if (FeaturePcdGet (PcdMeasuredBootEnabled) && (LoaderPlatformInfo->SecurityFlags && BIT1)) {
    TpmIndicateReadyToBoot ();
  }
  AddMeasurePoint (0x4100);

  // Print performance data
  PrintLinuxMeasurePoint ();

  DEBUG ((DEBUG_INIT, "\n%a\n\n", Message));

  // Print debug log buffer if serial port is not an active debug output device
  if ((PcdGet32 (PcdDebugOutputDeviceMask) & DEBUG_OUTPUT_DEVICE_SERIAL_PORT) == 0) {
    LogBufHdr = (DEBUG_LOG_BUFFER_HEADER *) GetDebugLogBufferPtr ();
    SerialPortWrite ((UINT8 *)"\nLOGBUF:", 8);
    SerialPortWrite ((UINT8 *)LogBufHdr->Buffer, LogBufHdr->UsedLength - LogBufHdr->HeaderLength);
  }

}

/**
  Start boot image

  This function will jump to kernel entry point.

  @param[in]  LoadedImage       Loaded boot image information.

  @retval  RETURN_SUCCESS       boot image is return after boot
  @retval  Others               There is error when checking boot image
**/
EFI_STATUS
StartBooting (
  IN LOADED_IMAGE            *LoadedImage
  )
{
  EFI_STATUS                 Status;
  MULTIBOOT_IMAGE            *MultiBoot;
  BOOT_PARAMS                *BootParams;

  DEBUG_CODE_BEGIN();
  PrintStackHeapInfo ();
  DEBUG_CODE_END();

  Status = EFI_SUCCESS;

  if ((LoadedImage->Flags & LOADED_IMAGE_LINUX) != 0) {
    BootParams = LoadedImage->Image.Linux.BootParams;
    BeforeOSJump ("Starting Kernel ...");
    JumpToKernel ((VOID *)BootParams->Hdr.Code32Start, (VOID *) BootParams);
    Status = EFI_DEVICE_ERROR;

  } else if ((LoadedImage->Flags & LOADED_IMAGE_MULTIBOOT) != 0) {
    DEBUG ((DEBUG_INIT, "Jumping into ELF or Multiboot image entry point...\n"));
    MultiBoot = &LoadedImage->Image.MultiBoot;
    if (MultiBoot->BootState.EntryPoint == 0) {
      DEBUG ((DEBUG_ERROR, "EntryPoint is not found\n"));
      return RETURN_INVALID_PARAMETER;
    }
    BeforeOSJump ("Starting MB Kernel ...");
    JumpToMultibootOs((IA32_BOOT_STATE*)&MultiBoot->BootState);
    Status = EFI_DEVICE_ERROR;

  } else if ((LoadedImage->Flags & LOADED_IMAGE_PE32) != 0) {
    MultiBoot = &LoadedImage->Image.MultiBoot;
    BeforeOSJump ("Jumping into PE32 ...");

    //
    // Use switch stack to ensure stack will be rolled back to original point.
    //
    SwitchStack (
      (SWITCH_STACK_ENTRY_POINT)MultiBoot->BootState.EntryPoint,
      (VOID *)PcdGet32 (PcdPayloadHobList),
      NULL,
      (VOID *)((UINT8 *)mEntryStack + 8)
      );
    Status = EFI_DEVICE_ERROR;
  } else {
    DEBUG ((DEBUG_ERROR, "Image is not supported, flags:0x%x\n", LoadedImage->Flags));
    return RETURN_UNSUPPORTED;
  }

  if (Status == EFI_DEVICE_ERROR) {
    CpuHalt ("Boot image returned");
  }

  return EFI_SUCCESS;
}

/**
  Boot from OsBootOption

  Based on given boot option, this function will load image, setup
  boot parameters and boot image.

  @param[in]  OsBootOption      OS boot optoin to boot

  @retval  RETURN_SUCCESS       Image returns after boot into its entrypoint
  @retval  Others               There is error to boot from this boot option
**/
EFI_STATUS
BootOsImage (
  IN  OS_BOOT_OPTION         *OsBootOption
  )
{
  EFI_STATUS                 Status;
  EFI_HANDLE                 HwPartHandle;
  LOADED_IMAGE               LoadedImage;
  LOADED_IMAGE               LoadedTrustyImage;
  LOADED_IMAGE               LoadedExtraImage[MAX_EXTRA_IMAGE_NUM];
  OS_BOOT_OPTION             BootOption;
  INT32                      BootSlot;
  UINT8                      Index;

  HwPartHandle = NULL;
  CopyMem (&BootOption, OsBootOption, sizeof (OS_BOOT_OPTION));
  ZeroMem (&LoadedImage, sizeof (LOADED_IMAGE));
  Status = FindBootPartition (&BootOption, &HwPartHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Get boot image A/B info
  BootSlot = GetBootSlot (&BootOption, HwPartHandle);
  DEBUG ((DEBUG_INFO, "BootSlot = 0x%x\n", BootSlot));
  if (BootSlot == 1) {
    BootOption.BootFlags |= LOAD_IMAGE_FROM_BACKUP;
  }

  // Load normal OS boot image
  LoadedImage.HwPartHandle = HwPartHandle;
  Status = LoadAndSetupImage (&BootOption, &LoadedImage);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "LoadImage: Status = %r\n", Status));
    return Status;
  }

  // Load Trusy OS image if required
  if ((BootOption.BootFlags & BOOT_FLAGS_TRUSTY) != 0) {
    ZeroMem (&LoadedTrustyImage, sizeof (LOADED_IMAGE));
    LoadedTrustyImage.HwPartHandle  = HwPartHandle;
    LoadedTrustyImage.LoadImageType = LOAD_IMAGE_TRUSTY;
    Status = LoadAndSetupImage (&BootOption, &LoadedTrustyImage);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Load extra Image: Status = %r\n", Status));
      return Status;
    }
  }

  // Load Extra OS image if required
  if ((BootOption.BootFlags & BOOT_FLAGS_EXTRA) != 0) {
    for (Index = 0; Index < MAX_EXTRA_IMAGE_NUM; Index++) {
      if (BootOption.Image[LOAD_IMAGE_EXTRA0 + Index].LbaImage.Valid == 0) {
        break;
      }
      ZeroMem (&LoadedExtraImage[Index], sizeof (LOADED_IMAGE));
      LoadedExtraImage[Index].HwPartHandle  = HwPartHandle;
      LoadedExtraImage[Index].LoadImageType = LOAD_IMAGE_EXTRA0 + Index;
      Status = LoadAndSetupImage (&BootOption, &LoadedExtraImage[Index]);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "Load extra Image[%d]: Status = %r\n", Index, Status));
      }
    }
  }

  Status = UpdateOsParameters (&BootOption, &LoadedImage, &LoadedTrustyImage, &LoadedExtraImage[0]);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "UpdateOsParameters: Status = %r\n", Status));
    return Status;
  }
  AddMeasurePoint (0x40E0);

  if ((BootOption.BootFlags & BOOT_FLAGS_TRUSTY) != 0) {
    Status = StartBooting (&LoadedTrustyImage);
  } else {
    Status = StartBooting (&LoadedImage);
  }
  return Status;
}

/**
  Initialize platform console.

  @retval  EFI_NOT_FOUND    No additional console was found.
  @retval  EFI_SUCCESS      Console has been initialized successfully.
  @retval  Others           There is error during console initialization.
**/
EFI_STATUS
InitConsole (
  VOID
)
{
  UINT32                   CtrlPciBase;
  EFI_STATUS               Status;
  UINT32                   Height;
  UINT32                   Width;
  UINT32                   OffX;
  UINT32                   OffY;
  FRAME_BUFFER_INFO       *FrameBuffer;

  Status = EFI_NOT_FOUND;

  // Initialize input console
  if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInUsbKeyboard) != 0) {
    CtrlPciBase = GetDeviceAddr (OsBootDeviceUsb, 0);
    if (CtrlPciBase > 0) {
      CtrlPciBase = TO_MM_PCI_ADDRESS (CtrlPciBase);
      Status = InitUsbKeyBoard (CtrlPciBase);
    }
  }

  if (PcdGet32 (PcdConsoleOutDeviceMask) & ConsoleOutFrameBuffer) {
    FrameBuffer = (FRAME_BUFFER_INFO *)GetGuidHobData (NULL, NULL, &gLoaderFrameBufferInfoGuid);
    if (FrameBuffer != NULL) {
      Width  = FrameBuffer->HorizontalResolution;
      Height = FrameBuffer->VerticalResolution;
      if ((PcdGet32 (PcdFrameBufferMaxConsoleWidth) > 0) && (Width > PcdGet32 (PcdFrameBufferMaxConsoleWidth))) {
        Width = PcdGet32 (PcdFrameBufferMaxConsoleWidth);
      }
      if ((PcdGet32 (PcdFrameBufferMaxConsoleHeight) > 0) && (Height > PcdGet32 (PcdFrameBufferMaxConsoleHeight))) {
        Height = PcdGet32 (PcdFrameBufferMaxConsoleHeight);
      }
      OffX = (FrameBuffer->HorizontalResolution-Width)/2;
      OffY = (FrameBuffer->VerticalResolution-Height)/2;
      Status = InitFrameBufferConsole (FrameBuffer, Width, Height, OffX, OffY);
    }
  }

  return Status;
}

/**
  Run Shell.

  This function will enter Shell environment.

  @param  Timeout           Waiting time out in seconds before entrying Shell.
                            0 will enter Shell without waiting.
**/
VOID
RunShell (
  IN  UINTN    Timeout
  )
{
  CONST SHELL_COMMAND    **ShellExtensionCmd;

  InitConsole ();

  ShellExtensionCmd = GetShellExtensionCmds ();
  Shell (ShellExtensionCmd, Timeout);
}

/**
  Payload main entry.

  This function will continue Payload execution with a new memory based stack.

  @param  Param           parameter passed from SwitchStack().

**/
VOID
EFIAPI
PayloadMain (
  IN  VOID             *Param
  )
{
  OS_BOOT_OPTION_LIST    *OsBootOptionList;
  UINTN                  ShellTimeout;

  mEntryStack = Param;

  DEBUG ((DEBUG_INFO, "\n\n====================Os Loader====================\n\n"));
  AddMeasurePoint (0x4010);

  //
  // Get Boot Image Info
  //
  OsBootOptionList = GetBootOptionList ();
  if ((OsBootOptionList == NULL) || (OsBootOptionList->OsBootOptionCount == 0)) {
    ShellTimeout = 0;
    DEBUG ((DEBUG_ERROR, "Get OS boot option fail. \n"));
    goto GOTO_SHELL;
  }

  // Shell will be invoked even in release build if BootToShell is set
  ShellTimeout = (UINTN)PcdGet16 (PcdPlatformBootTimeOut);
  if (OsBootOptionList->BootToShell != 0) {
    ShellTimeout = 0;
  }
  if ((DebugCodeEnabled () == TRUE) || (OsBootOptionList->BootToShell != 0)) {
    RunShell (ShellTimeout);
  }
  AddMeasurePoint (0x4020);

  DEBUG_CODE_BEGIN();
  PrintBootOptions (OsBootOptionList);
  DEBUG_CODE_END ();

#if  TEST_DEVICE_WRITE
  UINT32                Index;
  for (Index = 0; Index < OsBootOptionList->OsBootOptionCount; Index++) {
    TestDevBlocks (&OsBootOptionList->OsBootOption[Index]);
  }
#endif

  //
  // Load and run Image in order from OsImageList
  //
  mCurrentBoot = GetCurrentBootOption (OsBootOptionList, mCurrentBoot);
  while  (mCurrentBoot < OsBootOptionList->OsBootOptionCount) {
    BootOsImage (&OsBootOptionList->OsBootOption[mCurrentBoot]);
    mCurrentBoot = GetNextBootOption (OsBootOptionList, mCurrentBoot);
    if (mCurrentBoot == OsBootOptionList->OsBootOptionCount) {
      break;
    }

    //
    // Use switch stack to ensure stack will be rolled back to original point.
    // The stack will be adjusted by +8 to match the exact stack top at the
    // time that SecStartup() was called by Stage2.
    //
    DEBUG ((DEBUG_INIT, "Try next boot option\n"));
    SwitchStack (
      (SWITCH_STACK_ENTRY_POINT)SecStartup,
      (VOID *)PcdGet32 (PcdPayloadHobList),
      NULL,
      (VOID *) ((UINT8 *)Param + 8)
      );
  }

  DEBUG ((DEBUG_INFO, "Error: while trying to booting...\n"));

GOTO_SHELL:
  if (DebugCodeEnabled () == TRUE) {
    RunShell (0);
  }

  CpuHalt (NULL);
}
