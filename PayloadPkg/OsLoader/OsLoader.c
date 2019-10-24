/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"

UINT8    mCurrentBoot;
VOID    *mEntryStack;


/**
  Update fields of LoadedImage

  This function will update LoadedImage structure with the
  individual File info

  @param[in]      NumFiles      Number of files/components
  @param[in]      File          Array of individual Files
  @param[in, out] LoadedImage   Loaded Image information.
  @param[in]      ImageType     {NORMAL, CLASSIC, MULTIBOOT}

  @retval  RETURN_SUCCESS       Update LoadedImage successfully
  @retval  Others               There is error updating.
**/
EFI_STATUS
UpdateLoadedImage (
  IN      UINT32         NumFiles,
  IN      IMAGE_DATA    *File,
  IN OUT  LOADED_IMAGE  *LoadedImage,
  IN      UINT32         ImageType
  )
{
  EFI_STATUS                 Status;
  UINTN                      InitrdSize;
  VOID                       *InitrdData;
  UINT16                     Index;
  UINT16                     ModuleIndex;
  LINUX_IMAGE                *LinuxImage;
  MULTIBOOT_IMAGE            *MultiBoot;
  COMMON_IMAGE               *CommonImage;
  PLATFORM_SERVICE           *PlatformService;

  PlatformService = NULL;
  Status = EFI_SUCCESS;

  if (NumFiles == 1) {
    // This is not valid image use case, at least 2 files in image.
    // Support this only for test
    CommonImage                = &LoadedImage->Image.Common;
    CommonImage->BootFile.Addr = File[0].Addr;
    CommonImage->BootFile.Size = File[0].Size;
    if (IsMultiboot (File[0].Addr)) {
      LoadedImage->Flags |= LOADED_IMAGE_MULTIBOOT;
      DEBUG ((DEBUG_INFO, "One multiboot file in boot image file .... \n"));
    } else if (IsTePe32Image (File[0].Addr, NULL) && \
               (* (UINT32 *)File[0].Addr == EFI_IMAGE_DOS_SIGNATURE)) {
      // Add extra check to ensure it is a PE32 image generated from payload build.
      // Please note vmlinuxz is also following PE32 format, but it should
      // be handled as Linux image boot path
      LoadedImage->Flags |= LOADED_IMAGE_PE32;
      DEBUG ((DEBUG_INFO, "One PE32 file in boot image file .... \n"));
    } else if (IsValidFvHeader ((VOID *)File[0].Addr))  {
      LoadedImage->Flags |= LOADED_IMAGE_FV;
      DEBUG ((DEBUG_INFO, "One FV file in boot iamge file .... \n"));
    } else {
      LoadedImage->Flags |= LOADED_IMAGE_LINUX;
      DEBUG ((DEBUG_INFO, "One file in boot image file, take it as bzImage .... \n"));
    }
    return EFI_SUCCESS;
  }

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
          DEBUG ((DEBUG_INFO, "Loading boot image ACPI tables...\n"));
          if (PlatformService == NULL) {
            PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
            if ((PlatformService != NULL) && (PlatformService->AcpiTableUpdate != NULL)) {
              Status = PlatformService->AcpiTableUpdate (File[Index + 1].Addr, File[Index + 1].Size);
              DEBUG ((DEBUG_INFO, "Updating ACPI table with boot image %d - %r\n", Index, Status));
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

  return Status;
}

/**
  Parse CONTAINER image

  This function will parse CONTAINER image and get every file blobs info, Especially
  get the command line file and ELF/kernel file for boot.

  @param[in]      BootOption    Current boot option
  @param[in, out] LoadedImage   Loaded Image information.

  @retval  RETURN_SUCCESS       Parse IAS image successfully
  @retval  Others               There is error when parsing IAS image.
**/
EFI_STATUS
ParseContainerImage (
  IN     OS_BOOT_OPTION      *BootOption,
  IN OUT LOADED_IMAGE        *LoadedImage
  )
{
  EFI_STATUS                  Status;
  CONTAINER_HDR              *ContainerHdr;
  UINT64                      ComponentName;
  LOADER_COMPRESSED_HEADER   *LzHdr;
  IMAGE_DATA                  File[MAX_IAS_SUB_IMAGE];
  UINT8                       Index;

  ContainerHdr = (CONTAINER_HDR  *)LoadedImage->IasImage.Addr;
  if (ContainerHdr->Signature != CONTAINER_BOOT_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }
  Status = RegisterContainer ((UINT32) ContainerHdr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Image given is not a valid CONTAINER image\n"));
    return EFI_LOAD_ERROR;
  }

  AddMeasurePoint (0x4080);
  ZeroMem (File, sizeof (File));

  DEBUG ((DEBUG_INFO, "CONTAINER size = 0x%x, image type = 0x%x, # of components = %d\n", LoadedImage->IasImage.Size, ContainerHdr->ImageType, ContainerHdr->Count));

  // Enumerate all components
  Index = 0;
  ComponentName = 0;
  while (TRUE) {
    Status = GetNextAvailableComponent (ContainerHdr->Signature, (UINT32 *)&ComponentName);
    if (EFI_ERROR(Status) || ((UINT32)ComponentName == CONTAINER_MONO_SIGN_SIGNATURE) ) {
      break;
    } else {
      DEBUG ((DEBUG_INFO, "COMP:%a %r\n", &ComponentName, Status));
      // Load or locate components
      if (ContainerHdr->Flags & CONTAINER_HDR_FLAG_MONO_SIGNING) {
        // Use Locate to reuse the image buffer
        Status  = LocateComponent (ContainerHdr->Signature, (UINT32) ComponentName, (VOID **)&LzHdr, NULL);
        if (!EFI_ERROR(Status)) {
          if (LzHdr->Signature != LZDM_SIGNATURE) {
            Status = EFI_UNSUPPORTED;
          } else {
            File[Index].Addr = (UINT8 *)LzHdr + sizeof(LOADER_COMPRESSED_HEADER);
            File[Index].Size = LzHdr->Size;
          }
        }
      } else {
        // Use Load to decompress to a new buffer
        Status = LoadComponent (ContainerHdr->Signature, (UINT32) ComponentName, (VOID **)&File[Index].Addr, &File[Index].Size);
      }
    }
    if (Status == EFI_SUCCESS) {
      Index++;
    }
  }

  Status = UnregisterContainer (ContainerHdr->Signature);
  DEBUG ((DEBUG_INFO, "Unregister done - %r!\n", Status));

  // Mask upper nibble in ImageType so that UpdateLoadedImage() supports both IAS and CONTAINER Image types
  Status = UpdateLoadedImage (ContainerHdr->Count, File, LoadedImage, ContainerHdr->ImageType & 0xF);

  AddMeasurePoint (0x40A0);
  return Status;
}

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
  UINT32                     ImageType;
  EFI_STATUS                 Status;

  IasImage = IsIasImageValid (LoadedImage->IasImage.Addr, LoadedImage->IasImage.Size, LoadedImage->ImageHash);
  if (IasImage == NULL) {
    DEBUG ((DEBUG_INFO, "Image given is not a valid IAS image\n"));
    return EFI_LOAD_ERROR;
  }
  AddMeasurePoint (0x4080);

  ZeroMem (File, sizeof (File));
  NumFiles = IasGetFiles (IasImage, sizeof (File) / sizeof ((File)[0]), File);
  DEBUG ((DEBUG_INFO, "IAS size = 0x%x, file number: %d\n", LoadedImage->IasImage.Size, NumFiles));

  ImageType = IAS_IMAGE_TYPE (IasImage->ImageType);
  DEBUG ((DEBUG_INFO, "IAS Image Type = 0x%x\n", ImageType));

  Status = UpdateLoadedImage (NumFiles, File, LoadedImage, ImageType);

  AddMeasurePoint (0x40A0);
  return Status;
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
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;
  UINT32                    *EntryPoint;
  UINT8                     *NewCmdBuffer;
  MULTIBOOT_IMAGE           *MultiBoot;
  IMAGE_DATA                *CmdFile;
  IMAGE_DATA                *BootFile;
  LINUX_IMAGE               *LinuxImage;

  //
  // Allocate a cmd line buffer and init it with config file or default value
  // Later, maybe there is new command line parameters appended
  //
  NewCmdBuffer = (UINT8 *) AllocatePages (EFI_SIZE_TO_PAGES (CMDLINE_LENGTH_MAX));
  if (NewCmdBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = EFI_SUCCESS;
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
    Status = LoadElfImage (BootFile->Addr, (VOID *)&EntryPoint);
    if (!EFI_ERROR (Status)) {
      if (IsMultiboot (BootFile->Addr)) {
        DEBUG ((DEBUG_INFO, "and Image is Multiboot format\n"));
        SetupMultibootInfo (MultiBoot);
      }
      MultiBoot->BootState.EntryPoint = (UINT32)EntryPoint;
    }
  } else if (IsMultiboot (BootFile->Addr)) {
    DEBUG ((DEBUG_INFO, "Boot image is Multiboot format...\n"));
    Status = SetupMultibootImage (MultiBoot);
  } else if ((LoadedImage->Flags & LOADED_IMAGE_PE32) != 0) {
    DEBUG ((DEBUG_INFO, "Boot image is PE32 format\n"));
    Status = PeCoffRelocateImage ((UINT32)BootFile->Addr);
    if (!EFI_ERROR (Status)) {
      Status = PeCoffLoaderGetEntryPoint (BootFile->Addr, (VOID **)&EntryPoint);
    }
    if (!EFI_ERROR (Status)) {
      // Reuse MultiBoot structure to store the PE32 entry point information
      MultiBoot->BootState.EntryPoint = (UINT32)EntryPoint;
    }
  } else if ((LoadedImage->Flags & LOADED_IMAGE_FV) != 0) {
    DEBUG ((DEBUG_INFO, "Boot image is FV format\n"));
    Status = LoadFvImage ((UINT32 *)BootFile->Addr, BootFile->Size, (VOID **)&EntryPoint);
    if (!EFI_ERROR (Status)) {
      // Reuse MultiBoot structure to store the FV entry point information
      MultiBoot->BootState.EntryPoint = (UINT32)EntryPoint;
    }
  } else {
    DEBUG ((DEBUG_INFO, "Assume BzImage...\n"));
    LinuxImage = &LoadedImage->Image.Linux;
    Status = LoadBzImage (LinuxImage->BootFile.Addr,
                          LinuxImage->InitrdFile.Addr, LinuxImage->InitrdFile.Size,
                          LinuxImage->CmdFile.Addr,    LinuxImage->CmdFile.Size);
    if (!EFI_ERROR (Status)) {
      UpdateLinuxBootParams ();
      LinuxImage->BootParams = GetLinuxBootParams ();
      LoadedImage->Flags  = (LoadedImage->Flags  & ~LOADED_IMAGE_MULTIBOOT) | LOADED_IMAGE_LINUX;
    }
  }

  if (EFI_ERROR (Status)) {
    if (NewCmdBuffer != NULL) {
      FreePages (NewCmdBuffer, EFI_SIZE_TO_PAGES (CMDLINE_LENGTH_MAX));
      CmdFile->Addr = NULL;
    }
  } else {
    LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GetLoaderPlatformInfoPtr();
    if (LoaderPlatformInfo != NULL) {
      if (FeaturePcdGet (PcdMeasuredBootEnabled) && (LoaderPlatformInfo->LdrFeatures & FEATURE_MEASURED_BOOT)) {
        // Extend hash of the image into TPM.
        TpmExtendPcrAndLogEvent (8, TPM_ALG_SHA256, LoadedImage->ImageHash,
          EV_COMPACT_HASH, sizeof("LinuxLoaderPkg: OS Image"), (UINT8 *)"LinuxLoaderPkg: OS Image");
      }
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
  The function will de-initialize boot device.

**/
VOID
DeinitBootDevices (
  VOID
  )
{
  // Deinit boot media
  MediaInitialize (0, DevDeinit);

  if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInUsbKeyboard) != 0) {
    // Deinit USB devices if USB keyboard console is active
    DeinitUsbDevices ();
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
  if (FeaturePcdGet (PcdMeasuredBootEnabled) && (LoaderPlatformInfo->LdrFeatures & FEATURE_MEASURED_BOOT)) {
    TpmIndicateReadyToBoot ();
  }
  AddMeasurePoint (0x4100);

  // De-init boot devices before OS boot.
  DeinitBootDevices ();

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
  MULTIBOOT_IMAGE            *MultiBoot;
  BOOT_PARAMS                *BootParams;
  EFI_STATUS                 Status;

  DEBUG_CODE_BEGIN();
  PrintStackHeapInfo ();
  DEBUG_CODE_END();

  Status = EFI_SUCCESS;

  if ((LoadedImage->Flags & LOADED_IMAGE_LINUX) != 0) {
    BootParams = LoadedImage->Image.Linux.BootParams;

    if (FeaturePcdGet (PcdPreOsCheckerEnabled) && IsPreOsCheckerLoaded ()) {
      BeforeOSJump ("Starting Pre-OS Checker ...");
      StartPreOsChecker (BootParams);
    } else {
      BeforeOSJump ("Starting Kernel ...");
      JumpToKernel ((VOID *)BootParams->Hdr.Code32Start, (VOID *) BootParams);
    }
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

  } else if ((LoadedImage->Flags & (LOADED_IMAGE_PE32 | LOADED_IMAGE_FV)) != 0) {
    MultiBoot = &LoadedImage->Image.MultiBoot;
    BeforeOSJump ("Jumping into FV/PE32 ...");

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
  Initialize Boot Device (Media)

  Based on given boot option, this function will initialize Boot Device.

  @param[in]  OsBootOption      OS boot option to boot

  @retval  EFI_SUCCESS          A Boot Device is initialized successfully
  @retval  Others               An error during initializing a Boot Device

**/
EFI_STATUS
EFIAPI
InitBootDevice (
  IN  OS_BOOT_OPTION          *OsBootOption
  )
{
  EFI_STATUS                Status;
  UINTN                     BootMediumPciBase;
  UINT8                     DeviceType;
  UINT8                     DeviceInstance;

  AddMeasurePoint (0x4040);

  ASSERT (OsBootOption != NULL);

  DeviceType      = OsBootOption->DevType;
  DeviceInstance  = OsBootOption->DevInstance;

  //
  // Get OS boot device address
  //
  BootMediumPciBase = GetDeviceAddr (DeviceType, DeviceInstance);
  DEBUG ((DEBUG_INFO, "BootMediumPciBase(0x%x)\n", BootMediumPciBase));
  BootMediumPciBase = TO_MM_PCI_ADDRESS (BootMediumPciBase);

  //
  // Init Boot device functions
  //
  Status = MediaSetInterfaceType (DeviceType);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set media interface - %r\n", Status));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "Getting boot image from %a\n", GetBootDeviceNameString(DeviceType)));
  Status = MediaInitialize (BootMediumPciBase, DevInitAll);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to init media - %r\n", Status));
    return Status;
  }

  AddMeasurePoint (0x4050);
  MediaTuning (BootMediumPciBase);
  AddMeasurePoint (0x4055);

  return EFI_SUCCESS;
}

/**
  Find a MBR or GPT partition from the given hardware partition number

  Based on given boot option, this function will find a MBR or GPT partition

  @param[in]  OsBootOption      OS boot option to boot
  @param[out] HwPartHandle      Hardware partition handle when detected successfully

  @retval  EFI_SUCCESS          A hardware partition is initialized successfully
  @retval  Others               An error when finding a partition

**/
EFI_STATUS
EFIAPI
FindBootPartitions (
  IN  OS_BOOT_OPTION      *OsBootOption,
  OUT EFI_HANDLE          *HwPartHandle
  )
{
  EFI_STATUS                Status;
  UINT8                     HwPart;

  DEBUG ((DEBUG_INFO, "Try to find boot partition\n"));

  ASSERT (OsBootOption != NULL);

  HwPart = OsBootOption->HwPart;

  Status = FindPartitions (HwPart, HwPartHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to find partition - %r\n", Status));
    ClosePartitions (*HwPartHandle);
    return Status;
  }
  AddMeasurePoint (0x4060);

  DEBUG ((DEBUG_INFO, "Find partition success\n"));

  return EFI_SUCCESS;
}

/**
  Initialize File System

  Based on given boot option, this function will initialize File System

  @param[in]  OsBootOption      OS boot option to boot
  @param[in]  HwPartHandle      Detected hardware partition handle
  @param[out] FsHandle          File System handle when detected successfully

  @retval  EFI_SUCCESS          A File System is initialized successfully
  @retval  Others               An error when initializing a File System

**/
EFI_STATUS
EFIAPI
InitBootFileSystem (
  IN  OS_BOOT_OPTION      *OsBootOption,
  IN  EFI_HANDLE           HwPartHandle,
  OUT EFI_HANDLE          *FsHandle
  )
{
  EFI_STATUS      Status;
  UINT32          SwPart;
  UINT32          FsType;
  INT32           BootSlot;

  ASSERT (OsBootOption != NULL);

  SwPart = OsBootOption->SwPart;
  FsType = OsBootOption->FsType;

  DEBUG ((DEBUG_INFO, "Init File system\n"));
  if ((OS_FILE_SYSTEM_TYPE)FsType < EnumFileSystemMax) {
    Status = InitFileSystem (SwPart, FsType, HwPartHandle, FsHandle);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "No partitions found, Status = %r\n", Status));
      return Status;
    }
  } else {
    //
    // Assume RAW format partition
    //
    DEBUG ((DEBUG_INFO, "Requested FsType %d. Assume RAW format partition.\n", FsType));
    *FsHandle = NULL;
  }

  //
  // Get boot image A/B info. TBD: Better to be in LoadBootImages ()
  //
  BootSlot = GetBootSlot (OsBootOption, HwPartHandle);
  DEBUG ((DEBUG_INFO, "BootSlot = 0x%x\n", BootSlot));
  if (BootSlot == 1) {
    OsBootOption->BootFlags |= LOAD_IMAGE_FROM_BACKUP;
  }

  return EFI_SUCCESS;
}

/**
  Parse a Boot Image

  Based on given loaded image, this function will parse the image

  @param[in]      OsBootOption  OS boot option to boot
  @param[in, out] LoadedImage   Loaded Boot Image to be parsed

  @retval  EFI_SUCCESS          No issue when parsing the image
  @retval  Others               An error when parsing the image

**/
EFI_STATUS
EFIAPI
ParseBootImages (
  IN  OS_BOOT_OPTION    *OsBootOption,
  IN  EFI_HANDLE         LoadedImageHandle
  )
{
  LOADED_IMAGE        *LoadedImage;
  EFI_STATUS           Status;
  UINT8                Type;

  Status = EFI_SUCCESS;
  for (Type = 0; Type < LoadImageTypeMax; Type++) {
    if (Type == LoadImageTypeMisc) {
      continue;
    }

    GetLoadedImageByType (LoadedImageHandle, Type, &LoadedImage);
    if (LoadedImage == NULL) {
      continue;
    }

    DEBUG ((DEBUG_INFO, "ParseBootImage ImageType-%d\n", Type));
    if ((LoadedImage->Flags & LOADED_IMAGE_CONTAINER) != 0) {
      if (FeaturePcdGet (PcdContainerBootEnabled)) {
        Status = ParseContainerImage (OsBootOption, LoadedImage);
      }
    } else if ((LoadedImage->Flags & LOADED_IMAGE_IAS) != 0) {
      Status = ParseIasImage (OsBootOption, LoadedImage);
    }

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "ParseLoadedImage: Status = %r\n", Status));
      break;
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetupBootImages (
  IN  OS_BOOT_OPTION    *OsBootOption,
  IN  EFI_HANDLE         LoadedImageHandle
  )
{
  LOADED_IMAGE        *LoadedImage;
  LOADED_IMAGE        *LoadedTrustyImage;
  EFI_STATUS           Status;

  GetLoadedImageByType (LoadedImageHandle, LoadImageTypeNormal, &LoadedImage);
  GetLoadedImageByType (LoadedImageHandle, LoadImageTypeTrusty, &LoadedTrustyImage);

  //
  // Normal type image is mandatory
  //
  if (LoadedImage == NULL) {
    return EFI_LOAD_ERROR;
  }

  DEBUG ((DEBUG_INFO, "SetupBootImage ImageType-%d\n", LoadImageTypeNormal));
  Status = SetupBootImage (LoadedImage);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (LoadedTrustyImage != NULL) {
    DEBUG ((DEBUG_INFO, "SetupBootImage ImageType-%d\n", LoadImageTypeTrusty));
    Status = SetupBootImage (LoadedTrustyImage);
  }

  return Status;
}

/**
  Update Boot Parameter

  Based on given loaded image, this function will update Boot Parameters

  @param[in]      OsBootOption  OS boot option to boot
  @param[in, out] LoadedImage   Loaded Boot Image to be updated

  @retval  EFI_SUCCESS          No issue when updating the parameters
  @retval  Others               An error when updating the parameters

**/
EFI_STATUS
EFIAPI
UpdateBootParameters (
  IN  OS_BOOT_OPTION  *OsBootOption,
  IN  EFI_HANDLE       LoadedImageHandle
  )
{
  LOADED_IMAGE        *LoadedImage;
  LOADED_IMAGE        *LoadedTrustyImage;
  LOADED_IMAGE        *LoadedExtraImages;

  GetLoadedImageByType (LoadedImageHandle, LoadImageTypeNormal, &LoadedImage);
  GetLoadedImageByType (LoadedImageHandle, LoadImageTypeTrusty, &LoadedTrustyImage);
  GetLoadedImageByType (LoadedImageHandle, LoadImageTypeExtra0, &LoadedExtraImages);

  return UpdateOsParameters (OsBootOption, LoadedImage, LoadedTrustyImage, LoadedExtraImages);
}

EFI_STATUS
EFIAPI
StartBootImages (
  IN  EFI_HANDLE      LoadedImageHandle
  )
{
  LOADED_IMAGE        *LoadedImage;
  EFI_STATUS           Status;

  Status = GetLoadedImageByType (LoadedImageHandle, LoadImageTypeTrusty, &LoadedImage);
  if (EFI_ERROR (Status)) {
    Status = GetLoadedImageByType (LoadedImageHandle, LoadImageTypeNormal, &LoadedImage);
  }

  if (!EFI_ERROR (Status)) {
    Status = StartBooting (LoadedImage);
  }

  return Status;
}

/**
  Boot from OsBootOption

  Based on given boot option, this function will load image, setup
  boot parameters and boot image.

  @param[in]  OsBootOption      OS boot option to boot

  @retval  RETURN_SUCCESS       Image returns after boot into its entrypoint
  @retval  Others               There is error to boot from this boot option
**/
EFI_STATUS
BootOsImage (
  IN  OS_BOOT_OPTION         *OsBootOption
  )
{
  EFI_STATUS        Status;
  EFI_HANDLE        HwPartHandle;
  EFI_HANDLE        FsHandle;
  EFI_HANDLE        LoadedImageHandle;

  HwPartHandle      = NULL;
  FsHandle          = NULL;
  LoadedImageHandle = NULL;

  //
  // Initialize Boot Device
  //
  Status = InitBootDevice (OsBootOption);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Initialize Boot Device - Type %d, Instance %d\n",
      OsBootOption->DevType, OsBootOption->DevInstance));
    goto ERROR;
  }

  //
  // Find Boot Partition
  //
  Status = FindBootPartitions (OsBootOption, &HwPartHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Find Boot Partitions - HwPart %d\n", OsBootOption->HwPart));
    goto ERROR;
  }

  //
  // Init File System
  //
  Status = InitBootFileSystem (OsBootOption, HwPartHandle, &FsHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Initialize Boot File System - SwPart %d\n", OsBootOption->SwPart));
    goto ERROR;
  }

  //
  // Load Boot Image
  //
  Status = LoadBootImages (OsBootOption, HwPartHandle, FsHandle, &LoadedImageHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Load Boot Image\n"));
    goto ERROR;
  }
  AddMeasurePoint (0x4070);

  //
  // Parse Boot Image
  //
  Status = ParseBootImages (OsBootOption, LoadedImageHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Parse Boot Image\n"));
    goto ERROR;
  }

  //
  // Setup Boot Image
  //
  Status = SetupBootImages (OsBootOption, LoadedImageHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Setup Boot Image - Status = %r\n", Status));
    goto ERROR;
  }
  AddMeasurePoint (0x40B0);

  //
  // Update Boot Parameters
  //
  Status = UpdateBootParameters (OsBootOption, LoadedImageHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Update Boot Parameters - Status = %r\n", Status));
    goto ERROR;
  }

  //
  // Start Boot
  //
  StartBootImages (LoadedImageHandle);

  //
  // Never reach here
  //
  return EFI_DEVICE_ERROR;

ERROR:
  if (LoadedImageHandle != NULL) {
    UnloadBootImages (LoadedImageHandle);
  }

  if (FsHandle != NULL) {
    CloseFileSystem (FsHandle);
  }

  if (HwPartHandle != NULL) {
    ClosePartitions (HwPartHandle);
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
  UINT32                    CtrlPciBase;
  EFI_STATUS                Status;
  UINT32                    Height;
  UINT32                    Width;
  UINT32                    OffX;
  UINT32                    OffY;
  EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob;

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
    GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (NULL, NULL, &gEfiGraphicsInfoHobGuid);
    if (GfxInfoHob != NULL) {
      Width  = GfxInfoHob->GraphicsMode.HorizontalResolution;
      Height = GfxInfoHob->GraphicsMode.VerticalResolution;
      if ((PcdGet32 (PcdFrameBufferMaxConsoleWidth) > 0) && (Width > PcdGet32 (PcdFrameBufferMaxConsoleWidth))) {
        Width = PcdGet32 (PcdFrameBufferMaxConsoleWidth);
      }
      if ((PcdGet32 (PcdFrameBufferMaxConsoleHeight) > 0) && (Height > PcdGet32 (PcdFrameBufferMaxConsoleHeight))) {
        Height = PcdGet32 (PcdFrameBufferMaxConsoleHeight);
      }
      OffX = (GfxInfoHob->GraphicsMode.HorizontalResolution - Width) / 2;
      OffY = (GfxInfoHob->GraphicsMode.VerticalResolution - Height) / 2;
      Status = InitFrameBufferConsole (GfxInfoHob, Width, Height, OffX, OffY);
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
  @param  PldBase         payload base passed from SwitchStack().

**/
VOID
EFIAPI
PayloadMain (
  IN  VOID             *Param,
  IN  VOID             *PldBase
  )
{
  OS_BOOT_OPTION_LIST   *OsBootOptionList;
  BOOLEAN                BootShell;
  UINTN                  ShellTimeout;
  OS_BOOT_OPTION         OsBootOption;
  UINT8                  CurrIdx;
  UINT8                  BootIdx;

  mEntryStack = Param;

  DEBUG ((DEBUG_INFO, "\n\n====================Os Loader====================\n\n"));
  AddMeasurePoint (0x4010);

  //
  // Get Boot Image Info
  //
  ShellTimeout = 0;
  BootShell    = FALSE;
  OsBootOptionList = GetBootOptionList ();
  if (OsBootOptionList == NULL) {
    if (DebugCodeEnabled ()) {
      BootShell    = TRUE;
    }
  } else {
    if (OsBootOptionList->BootToShell != 0) {
      BootShell    = TRUE;
    } else if (DebugCodeEnabled ()) {
      ShellTimeout = (UINTN)PcdGet16 (PcdPlatformBootTimeOut);
      BootShell    = TRUE;
    }
  }

  if (BootShell) {
    RunShell (ShellTimeout);
  }
  AddMeasurePoint (0x4020);

  //
  // Load PreOsChecker
  //
  if (FeaturePcdGet (PcdPreOsCheckerEnabled)) {
    LoadPreOsChecker ();
  }

  while (OsBootOptionList != NULL) {

    // Print BOOT Option List
    DEBUG_CODE_BEGIN ();
    PrintBootOptions (OsBootOptionList);
    DEBUG_CODE_END ();

    // Load and run Image in order from OsImageList
    BootIdx = 0;
    CurrIdx = GetCurrentBootOption (OsBootOptionList, mCurrentBoot);
    while  (BootIdx < OsBootOptionList->OsBootOptionCount) {
      DEBUG ((DEBUG_INFO, "\n======== Try Booting with Boot Option %d ========\n", CurrIdx));

      // Get current boot option and try boot
      CopyMem ((VOID *)&OsBootOption, (VOID *)&OsBootOptionList->OsBootOption[CurrIdx], sizeof (OS_BOOT_OPTION));
      BootOsImage (&OsBootOption);

      // Move to next boot option
      CurrIdx = GetNextBootOption (OsBootOptionList, CurrIdx);
      if (CurrIdx >= OsBootOptionList->OsBootOptionCount) {
        CurrIdx = 0;
      }
      BootIdx++;
    }

    if (DebugCodeEnabled ()) {
      RunShell (0);
    } else {
      break;
    }

    // De-init boot devices while restarting payload.
    DeinitBootDevices ();
  }

  CpuHalt (NULL);
}
