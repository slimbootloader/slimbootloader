/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"


UINT8    mCurrentBoot;
VOID    *mEntryStack;

/**
  Callback function to add performance measure point during component loading.

  @param[in]  ProgressId    Component loading progress ID code.
  @param[in]  CbInfo    Component Call Back Info

**/
VOID
LoadComponentCallback (
  IN  UINT32                     ProgressId,
  IN  COMPONENT_CALLBACK_INFO   *CbInfo
  )
{
  if (ProgressId == PROGESS_ID_AUTHENTICATE) {
    AddMeasurePoint (0x4080);
  }

  if (FeaturePcdGet (PcdMeasuredBootEnabled) && (GetFeatureCfg() & FEATURE_MEASURED_BOOT)) {
    // Extend the OS component hash
    ExtendStageHash (CbInfo);
  }
}

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
  UINT16                     Index;
  UINT16                     ModuleIndex;
  LINUX_IMAGE                *LinuxImage;
  MULTIBOOT_IMAGE            *MultiBoot;
  COMMON_IMAGE               *CommonImage;
  PLATFORM_SERVICE           *PlatformService;
  CHAR8                      *TypeStr;
  CHAR8                      BlobName[5];               // 4 character component name + null termination
  CHAR8                      BlobAddr[17];              // 64-bit address in ASCII hex + null termination
  CHAR8                      *BlobPos;                  // Pointer to a character in the kernel cmdline string
  CHAR8                      BlobSearchStr[28];         // ASCII string in the expected format: SBL.XXXX=0x0000000000000000
  VOID                       *BlobReservedBuf;          // Pointer to allocated reserved memory

  PlatformService = NULL;
  Status = EFI_SUCCESS;

  if (ImageType == CONTAINER_TYPE_NORMAL) {
    // Image can be of type: PE, FV, bzImage, or ELF
    // Container can contain additional ACPI binary blobs
    // Assuming that the first image in the container is used for booting
    CommonImage                = &LoadedImage->Image.Common;
    CopyMem (&CommonImage->BootFile, &File[0], sizeof (IMAGE_DATA));
    if (IsTePe32Image (File[0].Addr, NULL) && \
               (* (UINT32 *)File[0].Addr == EFI_IMAGE_DOS_SIGNATURE)) {
      // Add extra check to ensure it is a PE32 image generated from payload build.
      // Please note vmlinuxz is also following PE32 format, but it should
      // be handled as Linux image boot path
      LoadedImage->Flags |= LOADED_IMAGE_PE;
      TypeStr = "PE";
    } else if (IsValidFvHeader ((VOID *)File[0].Addr))  {
      LoadedImage->Flags |= LOADED_IMAGE_FV;
      TypeStr = "FV";
    } else if (IsBzImage((VOID *)File[0].Addr)) {
      LoadedImage->Flags |= LOADED_IMAGE_LINUX;
      TypeStr = "Kernel";
    } else if (IsElfFormat ((VOID *)File[0].Addr))  {
      LoadedImage->Flags |= LOADED_IMAGE_ELF;
      TypeStr = "ELF";
    } else {
      DEBUG ((DEBUG_INFO, "Unknown file type !\n"));
      return EFI_UNSUPPORTED;
    }

    DEBUG ((DEBUG_INFO, "One %a file in boot image file .... \n", TypeStr));

    // If there are more files, check for ACPI blobs and update ACPI tables accordingly
    if (NumFiles > 1) {
      Index = 1;
      while (Index < NumFiles) {
      // Update ACPI tables if we encounter an ACPI blob
      if (File[Index].Name == SIGNATURE_32('A', 'C', 'P', 'I')) {
          DEBUG ((DEBUG_INFO, "Loading boot image ACPI tables...\n"));
          PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
          if ((PlatformService != NULL) && (PlatformService->AcpiTableUpdate != NULL)) {
            Status = PlatformService->AcpiTableUpdate (File[Index].Addr, File[Index].Size);
            DEBUG ((DEBUG_INFO, "Updating ACPI table with boot image %d - %r\n", Index, Status));
          }
          FreeImageData (&File[Index]);
          continue;
        }
      Index++;
      }
    }

    return EFI_SUCCESS;
  } else if (ImageType == CONTAINER_TYPE_CLASSIC_LINUX) {
    // Files: cmdline, bzImage, initrd, other optional files (acpi, firmware1, firmware2, ...)
    // The file order for the first three files mentioned above is fixed. The rest are optional and can be in any order.
    // Container can contain additional ACPI binary blobs

    // Make sure that the boot file (File[1]) is present
    if (NumFiles < 2) {
      DEBUG ((DEBUG_ERROR, "Containers with only one file need to packaged as a \"Normal\" container.\n"));
      ASSERT (NumFiles >= 2);
    }

    LinuxImage                = &LoadedImage->Image.Linux;
    LoadedImage->Flags       |= LOADED_IMAGE_LINUX;
    CopyMem (&LinuxImage->CmdFile, &File[0], sizeof (IMAGE_DATA));
    CopyMem (&LinuxImage->BootFile, &File[1], sizeof (IMAGE_DATA));

    if (NumFiles > 2) {
      CopyMem (&LinuxImage->InitrdFile, &File[2], sizeof (IMAGE_DATA));

      //
      // Make sure Initrd is page aligned
      //
      if (((((UINTN)File[2].Addr) & EFI_PAGE_MASK) != 0) && (File[2].Size > 0)) {
        LinuxImage->InitrdFile.Addr = AllocatePages (EFI_SIZE_TO_PAGES (File[2].Size));
        if (LinuxImage->InitrdFile.Addr == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem (LinuxImage->InitrdFile.Addr, File[2].Addr, File[2].Size);
        LinuxImage->InitrdFile.AllocType = ImageAllocateTypePage;
        FreeImageData (&File[2]);
      }
    }

    // Save other binary blobs
    Index = 3;
    while ((Index < MAX_EXTRA_FILE_NUMBER) && (Index < NumFiles)) {
      // Update ACPI tables if we encounter an ACPI blob
      if (File[Index].Name == SIGNATURE_32('A', 'C', 'P', 'I')) {
          DEBUG ((DEBUG_INFO, "Loading boot image ACPI tables...\n"));
          PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
          if ((PlatformService != NULL) && (PlatformService->AcpiTableUpdate != NULL)) {
            Status = PlatformService->AcpiTableUpdate (File[Index].Addr, File[Index].Size);
            DEBUG ((DEBUG_INFO, "Updating ACPI table with boot image %d - %r\n", Index, Status));
          }
          FreeImageData (&File[Index]);
          Index++;
          continue;
        }
      CopyMem (&LinuxImage->ExtraBlob[Index - 3], &File[Index], sizeof (IMAGE_DATA));

      //
      // Update the blob's address in the kernel command line so that the OS knows where it resides
      // We also copy the blob into a reserved memory address so that the OS does not overwrite it
      //

      // Get the blob name
      CopyMem(BlobName, &File[Index].Name, 4);
      BlobName[4] = '\0';

      // Copy the extra blob into reserved memory
      BlobReservedBuf = AllocateReservedPages(EFI_SIZE_TO_PAGES(File[Index].Size));
      if (BlobReservedBuf == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem(BlobReservedBuf, File[Index].Addr, File[Index].Size);
      DEBUG ((DEBUG_INFO, "Copied %a to reserved memory @ 0x%016X\n", BlobName, BlobReservedBuf));

      // Generate the search string: SBL.XXXX=0x0000000000000000
      AsciiSPrint(BlobSearchStr, 28, "SBL.%a=0x0000000000000000", BlobName);
      DEBUG ((DEBUG_INFO, "Searching for \"%a\" blob placeholder string  in cmdline: %a... ", BlobName, BlobSearchStr));

      // Find the location of the placeholder string
      BlobPos = AsciiStrStr(LinuxImage->CmdFile.Addr, BlobSearchStr);

      if (BlobPos != NULL) {
        // Move the pointer to where we get to the actual adress (part after 0x)
        // e.g. SBL.ABCD=0x0000000000000000
        // AsciiStrStr will get us a pointer to 'S'. Adding 11 will get us to the address
        BlobPos += 11;

        // Get the blob's address into a string
        // AsciiSPrint(BlobAddr, 17, "%016X", File[Index].Addr);
        AsciiSPrint(BlobAddr, 17, "%016X", BlobReservedBuf);

        // Copy the actual address at the placeholder location
        AsciiStrCpyS(BlobPos, 17, BlobAddr);

        // Replace the copied string's last character with a space for all files except the last one
        // We don't do this for the last one since the kernel expects a null-terminated cmdline
        if (Index != NumFiles) {
          BlobPos += 16;
          *BlobPos = ' ';
        }

        DEBUG ((DEBUG_INFO, "Found and patched address!\n"));
      } else {
        DEBUG ((DEBUG_INFO, "Could not find cmdline placeholder\n"));
      }

      // Move to the next file
      Index++;
    }
    LinuxImage->ExtraBlobNumber = Index;
  } else if (ImageType == CONTAINER_TYPE_MULTIBOOT) {
    // Files: cmdline1, elf1, cmdline2, elf2, ...
    // Container can contain additional ACPI binary blobs
    // Assume the first elf file is the one to boot
    if (IsMultiboot (File[1].Addr)) {
      LoadedImage->Flags |= LOADED_IMAGE_MULTIBOOT;
      TypeStr = "Multiboot";
    } else if (IsMultiboot2 (File[1].Addr)) {
      LoadedImage->Flags |= LOADED_IMAGE_MULTIBOOT2;
      TypeStr = "Multiboot-2";
    } else {
      DEBUG ((DEBUG_ERROR, "\"Multiboot\" container type used for a non-multiboot image!"));
      return EFI_UNSUPPORTED;
    }

    DEBUG ((DEBUG_INFO, "%a file in boot image file .... \n", TypeStr));

    MultiBoot                = &LoadedImage->Image.MultiBoot;
    LoadedImage->Flags      |= LOADED_IMAGE_MULTIBOOT;
    CopyMem (&MultiBoot->CmdFile, &File[0], sizeof (IMAGE_DATA));
    CopyMem (&MultiBoot->BootFile, &File[1], sizeof (IMAGE_DATA));

    // Save other ELF pairs.
    ModuleIndex = 0;
    for (Index = 2; Index < NumFiles; Index += 2) {
      if (Index < MAX_MULTIBOOT_MODULE_NUMBER) {
        // Multiboot modules are in a cmdline-ELF pair according to the spec.
        // So to accomodate for that, ACPI binary blobs should be preceded by
        // a corresponding dummy cmdline file that contains the MULTIBOOT_SPECIAL_MODULE_MAGIC
        // string to indicate that the paired file is the ACPI binary blob
        if (* (UINT32 *) File[Index].Addr == MULTIBOOT_SPECIAL_MODULE_MAGIC) {
          DEBUG ((DEBUG_INFO, "Loading boot image ACPI tables...\n"));
          PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
          if ((PlatformService != NULL) && (PlatformService->AcpiTableUpdate != NULL)) {
            Status = PlatformService->AcpiTableUpdate (File[Index + 1].Addr, File[Index + 1].Size);
            DEBUG ((DEBUG_INFO, "Updating ACPI table with boot image %d - %r\n", Index, Status));
          }
          FreeImageData (&File[Index + 1]);
          continue;
        }

        //
        // IMAGE_DATA memory will be freed later if fails
        //
        CopyMem (&MultiBoot->MbModuleData[ModuleIndex].CmdFile, &File[Index], sizeof (IMAGE_DATA));
        CopyMem (&MultiBoot->MbModuleData[ModuleIndex].ImgFile, &File[Index + 1], sizeof (IMAGE_DATA));

        MultiBoot->MbModule[ModuleIndex].String = (UINT8 *) MultiBoot->MbModuleData[ModuleIndex].CmdFile.Addr;
        MultiBoot->MbModule[ModuleIndex].Start  = (UINT32)(UINTN)MultiBoot->MbModuleData[ModuleIndex].ImgFile.Addr;
        MultiBoot->MbModule[ModuleIndex].End    = (UINT32)(UINTN)MultiBoot->MbModuleData[ModuleIndex].ImgFile.Addr
          + MultiBoot->MbModuleData[ModuleIndex].ImgFile.Size;

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

  @retval  RETURN_SUCCESS       Parse container image successfully
  @retval  Others               There is error when parsing CONTAINER image.
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
  IMAGE_DATA                  File[MAX_CONTAINER_SUB_IMAGE];
  UINT8                       Index;

  ContainerHdr = (CONTAINER_HDR  *)LoadedImage->ImageData.Addr;
  if (ContainerHdr->Signature != CONTAINER_BOOT_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }

  Status = RegisterContainer ((UINT32)(UINTN)ContainerHdr, LoadComponentCallback);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Image given is not a valid CONTAINER image\n"));
    return EFI_LOAD_ERROR;
  }

  ZeroMem (File, sizeof (File));

  DEBUG ((DEBUG_INFO, "CONTAINER size = 0x%x, image type = 0x%x, # of components = %d\n", LoadedImage->ImageData.Size, ContainerHdr->ImageType, ContainerHdr->Count));

  // Enumerate all components
  Index = 0;
  ComponentName = 0;
  do {
    Status = GetNextAvailableComponent (ContainerHdr->Signature, (UINT32 *)&ComponentName);
    if (EFI_ERROR (Status) || ((UINT32)ComponentName == CONTAINER_MONO_SIGN_SIGNATURE)) {
      break;
    }
    DEBUG ((DEBUG_INFO, "COMP:%a %r\n", &ComponentName, Status));
    // Load or locate components
    if (ContainerHdr->Flags & CONTAINER_HDR_FLAG_MONO_SIGNING) {
      // Use Locate to reuse the image buffer
      Status = LocateComponent (ContainerHdr->Signature, (UINT32) ComponentName, (VOID **)&LzHdr, NULL);
      if (EFI_ERROR (Status)) {
        break;
      }
      if (LzHdr->Signature != LZDM_SIGNATURE) {
        Status = EFI_UNSUPPORTED;
        break;
      }

      File[Index].Addr = (UINT8 *)LzHdr + sizeof(LOADER_COMPRESSED_HEADER);
      File[Index].Size = LzHdr->Size;
      File[Index].AllocType = ImageAllocateTypePointer;
    } else {
      //
      // Use Load to decompress to a new aligned page
      //
      Status = LoadComponent (ContainerHdr->Signature, (UINT32) ComponentName, (VOID **)&File[Index].Addr, &File[Index].Size);
      if (!EFI_ERROR (Status)) {
        File[Index].AllocType = ImageAllocateTypePage;
      }
    }

    // Save the name of the component
    File[Index].Name = (UINT32) ComponentName;

    Index++;
  } while ((Status == EFI_SUCCESS) && (Index < ARRAY_SIZE (File)));

  Status = UnregisterContainer (ContainerHdr->Signature);
  DEBUG ((DEBUG_INFO, "Unregister done - %r!\n", Status));

  // Mask upper nibble in ImageType so that UpdateLoadedImage() supports both IAS and CONTAINER Image types
  Status = UpdateLoadedImage (Index, File, LoadedImage, ContainerHdr->ImageType & 0xF);

  if (EFI_ERROR (Status)) {
    UnloadLoadedImage (LoadedImage);
  }

  AddMeasurePoint (0x40A0);
  return Status;
}

/**
  Parse a single component image

  This function will parse a single image.
  This image could be TE/PE/FV or multi-boot format.

  @param[in]      BootOption    Current boot option
  @param[in, out] LoadedImage   Loaded Image information.

  @retval  RETURN_SUCCESS       Parse component image successfully
  @retval  Others               There is error when parsing component image.
**/
EFI_STATUS
ParseComponentImage (
  IN     OS_BOOT_OPTION      *BootOption,
  IN OUT LOADED_IMAGE        *LoadedImage
  )
{
  IMAGE_DATA                 File;
  EFI_STATUS                 Status;

  File.Addr = LoadedImage->ImageData.Addr;
  File.Size = LoadedImage->ImageData.Size;
  File.AllocType = ImageAllocateTypePointer;
  Status = UpdateLoadedImage (1, &File, LoadedImage, 0);
  if (EFI_ERROR (Status)) {
    UnloadLoadedImage (LoadedImage);
  }
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
  UINTN                      EntryPoint;
  UINT8                     *NewCmdBuffer;
  MULTIBOOT_IMAGE           *MultiBoot;
  IMAGE_DATA                *CmdFile;
  IMAGE_DATA                *BootFile;
  LINUX_IMAGE               *LinuxImage;
  LOADED_PAYLOAD_INFO        PayloadInfo;
  UINT32                     Size;
  UINT16                     Machine;

  //
  // Allocate a cmd line buffer and init it with config file or default value
  // Later, maybe there is new command line parameters appended
  //
  NewCmdBuffer = (UINT8 *) AllocatePages (EFI_SIZE_TO_PAGES (CMDLINE_LENGTH_MAX));
  if (NewCmdBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CmdFile = &LoadedImage->Image.Common.CmdFile;
  Size = 0;
  if (CmdFile->Addr != NULL && CmdFile->Size != 0) {
    Size = (UINT32)GetFromConfigFile (NewCmdBuffer, CMDLINE_LENGTH_MAX, (UINT8 *)CmdFile->Addr, CmdFile->Size);
  }
  if (Size == 0) {
    Size = (UINT32)AsciiStrSize (DEFAULT_COMMAND_LINE);
    CopyMem (NewCmdBuffer, DEFAULT_COMMAND_LINE, Size);
  }
  // Free Allocated Memory earlier first
  FreeImageData (CmdFile);
  // Assign new one
  CmdFile->Addr = NewCmdBuffer;
  CmdFile->Size = Size;
  CmdFile->AllocType = ImageAllocateTypePage;

  // Setup Images
  BootFile = &LoadedImage->Image.Common.BootFile;

  MultiBoot = &LoadedImage->Image.MultiBoot;
  if (IsElfFormat ((CONST UINT8 *)BootFile->Addr)) {
    DEBUG ((DEBUG_INFO, "Boot image is ELF format...\n"));
    EntryPoint = 0;
    ZeroMem (&PayloadInfo, sizeof(PayloadInfo));
    Status = LoadElfPayload (BootFile->Addr, &PayloadInfo);
    if (!EFI_ERROR (Status)) {
      EntryPoint = PayloadInfo.EntryPoint;
      if (IsMultiboot (BootFile->Addr)) {
        DEBUG ((DEBUG_INFO, "and Image is Multiboot format\n"));
        SetupMultibootInfo (MultiBoot);
      } else if (IsMultiboot2 (BootFile->Addr)) {
        DEBUG ((DEBUG_INFO, "and Image is Multiboot-2 format\n"));
        SetupMultiboot2Info (MultiBoot);
      }
      MultiBoot->BootState.EntryPoint = (UINT32)(UINTN)EntryPoint;
    }
  } else if (IsMultiboot (BootFile->Addr)) {
    DEBUG ((DEBUG_INFO, "Boot image is Multiboot format...\n"));
    Status = SetupMultibootImage (MultiBoot);
  } else if (IsMultiboot2 (BootFile->Addr)) {
    DEBUG ((DEBUG_INFO, "Boot image is Multiboot-2 format...\n"));
    Status = SetupMultiboot2Image (MultiBoot);
  } else if ((LoadedImage->Flags & LOADED_IMAGE_PE) != 0) {
    DEBUG ((DEBUG_INFO, "Boot image is PE32 format\n"));
    Status = PeCoffRelocateImage ((UINT32)(UINTN)BootFile->Addr);
    if (!EFI_ERROR (Status)) {
      Status = PeCoffLoaderGetEntryPoint (BootFile->Addr, (VOID **)&EntryPoint);
    }
    if (!EFI_ERROR (Status)) {
      // Reuse MultiBoot structure to store the PE32 entry point information
      MultiBoot->BootState.EntryPoint = (UINT32)(UINTN)EntryPoint;
    }
  } else if ((LoadedImage->Flags & LOADED_IMAGE_FV) != 0) {
    DEBUG ((DEBUG_INFO, "Boot image is FV format\n"));
    Status = LoadFvImage ((UINT32 *)BootFile->Addr, BootFile->Size, (VOID **)&EntryPoint, &Machine);
    if (!EFI_ERROR (Status)) {
      // Reuse MultiBoot structure to store the FV entry point information
      MultiBoot->BootState.EntryPoint = (UINT32)(UINTN)EntryPoint;
    }
  } else {
    DEBUG ((DEBUG_INFO, "Assume BzImage...\n"));
    LinuxImage = &LoadedImage->Image.Linux;
    Status = LoadBzImage (LinuxImage->BootFile.Addr,
                          LinuxImage->InitrdFile.Addr, LinuxImage->InitrdFile.Size,
                          LinuxImage->CmdFile.Addr,    LinuxImage->CmdFile.Size);
    if (!EFI_ERROR (Status)) {
      LoadedImage->Flags  = (LoadedImage->Flags  & ~LOADED_IMAGE_MULTIBOOT) | LOADED_IMAGE_LINUX;
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
             "Payload stack: 0x%X (0x%X used)\n",
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
  Checks whether the platform is a pre-production part; or in manufacturing mode or debug mode.

  @retval  TRUE    Platform is a pre-production part; or in manufacturing mode or debug mode.
  @retval  FALSE   Platfomr is not a pre-production part; or in manufacturing mode or debug mode.
**/
BOOLEAN
PlatformDebugStateEnabled (
  IN UINT16 HwState
  )
{
  MSR_IA32_FEATURE_CONTROL_REGISTER Ia32FeatureControlMsr;
  MSR_IA32_DEBUG_INTERFACE_REGISTER DebugInterfaceMsr;

  //
  // Check for Platform in Pre-production or in Manufaturing mode or secure debug mode
  //
  if((HwState & HWSTATE_IN_MANU_SECURE_DEBUG_MODE) || (HwState & HWSTATE_PLATFORM_PRE_PRODUCTION)){
    return TRUE;
  }

  //
  // Check for Sample part
  //
  Ia32FeatureControlMsr.Uint64 = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);
  if (Ia32FeatureControlMsr.Bits.Lock == 0) {
    return TRUE;
  }

  //
  // Check for Debug mode
  //
  DebugInterfaceMsr.Uint64 = AsmReadMsr64 (MSR_IA32_DEBUG_INTERFACE);
  if (DebugInterfaceMsr.Bits.Enable) {
    return TRUE;
  }

  return FALSE;
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
  UINT8                      PlatformDebugEnabled;

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
    PlatformDebugEnabled = PlatformDebugStateEnabled (LoaderPlatformInfo->HwState);
    if(TpmIndicateReadyToBoot (PlatformDebugEnabled) != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "FAILED to complete TPM ReadyToBoot actions. \n"));
    }
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

  This function will jump to image entry point.

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
  EFI_STATUS                 Status;
  PLD_MOD_PARAM              PldModParam;
  OS_BOOT_OPTION_LIST       *OsBootOptionList;
  UINT32                     CurrIdx;
  CHAR8                     *PathPtr;
  CHAR8                      ModCmdLineBuf[16];
  PRE_OS_ENTRYPOINT          EntryPoint;

  DEBUG_CODE_BEGIN();
  PrintStackHeapInfo ();
  DEBUG_CODE_END();

  Status = EFI_SUCCESS;

  if ((LoadedImage->Flags & LOADED_IMAGE_RUN_EXTRA) != 0) {
    DEBUG ((DEBUG_INIT, "Call PRE-OS image entry point...\n"));
    MultiBoot  = &LoadedImage->Image.MultiBoot;
    EntryPoint = (PRE_OS_ENTRYPOINT)(UINTN)MultiBoot->BootState.EntryPoint;
    EntryPoint((VOID *)(UINTN)PcdGet32(PcdPayloadHobList));
  } else if ((LoadedImage->Flags & LOADED_IMAGE_LINUX) != 0) {
    BeforeOSJump ("Starting Kernel ...");
    LinuxBoot ((VOID *)(UINTN)PcdGet32 (PcdPayloadHobList), NULL);
    Status = EFI_DEVICE_ERROR;
  } else if ((LoadedImage->Flags & LOADED_IMAGE_ELF) != 0) {
    BeforeOSJump ("Starting ELF ...");
    MultiBoot  = &LoadedImage->Image.MultiBoot;
    EntryPoint = (PRE_OS_ENTRYPOINT)(UINTN)MultiBoot->BootState.EntryPoint;
    EntryPoint((VOID *)(UINTN)PcdGet32(PcdPayloadHobList));
  } else if ((LoadedImage->Flags & LOADED_IMAGE_MULTIBOOT) != 0) {
    DEBUG ((DEBUG_INIT, "Jumping Multiboot image entry point...\n"));
    MultiBoot = &LoadedImage->Image.MultiBoot;
    if (MultiBoot->BootState.EntryPoint == 0) {
      DEBUG ((DEBUG_ERROR, "EntryPoint is not found\n"));
      return RETURN_INVALID_PARAMETER;
    }

    BeforeOSJump ("Starting MB Kernel ...");
    JumpToMultibootOs((IA32_BOOT_STATE*)&MultiBoot->BootState);
    Status = EFI_DEVICE_ERROR;

  } else if ((LoadedImage->Flags & LOADED_IMAGE_MULTIBOOT2) != 0) {
    DEBUG ((DEBUG_INIT, "Jumping Multiboot-2 image entry point...\n"));
    MultiBoot = &LoadedImage->Image.MultiBoot;
    if (MultiBoot->BootState.EntryPoint == 0) {
      DEBUG ((DEBUG_ERROR, "EntryPoint is not found\n"));
      return RETURN_INVALID_PARAMETER;
    }

    BeforeOSJump ("Starting MB-2 Kernel ...");
    JumpToMultibootOs((IA32_BOOT_STATE*)&MultiBoot->BootState);
    Status = EFI_DEVICE_ERROR;

  } else if ((LoadedImage->Flags & (LOADED_IMAGE_PE | LOADED_IMAGE_FV)) != 0) {
    MultiBoot = &LoadedImage->Image.MultiBoot;
    BeforeOSJump ("Jumping into FV/PE32 ...");

    if (FeaturePcdGet (PcdPayloadModuleEnabled)) {
      OsBootOptionList = GetBootOptionList ();
      if (OsBootOptionList == NULL) {
        return EFI_NOT_FOUND;
      }
      CurrIdx = mCurrentBoot;
      PathPtr = (CHAR8 *)OsBootOptionList->OsBootOption[CurrIdx].Image[0].FileName;
      ModCmdLineBuf[0] = 0;
      ZeroMem (&PldModParam, sizeof(PLD_MOD_PARAM));
      PldModParam.GetProcAddress = GetProcAddress;
      PldModParam.CmdLineBuf     = ModCmdLineBuf;
      PldModParam.CmdLineLen     = sizeof(ModCmdLineBuf);
      PayloadModuleInit (&PldModParam, PathPtr);
    }

    //
    // Use switch stack to ensure stack will be rolled back to original point.
    //
    SwitchStack (
      (SWITCH_STACK_ENTRY_POINT)(UINTN)MultiBoot->BootState.EntryPoint,
      (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList),
      FeaturePcdGet (PcdPayloadModuleEnabled) ? &PldModParam : NULL,
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

  ASSERT (OsBootOption != NULL);

  HwPart = OsBootOption->HwPart;
  DEBUG ((DEBUG_INFO, "Try to find boot partition for HW part %d\n", HwPart));

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
  UINT32          DevType;
  INT32           BootSlot;
  OS_FILE_SYSTEM_TYPE  FsType;

  ASSERT (OsBootOption != NULL);

  SwPart = OsBootOption->SwPart;
  FsType = OsBootOption->FsType;
  DevType = OsBootOption->DevType;
  DEBUG ((DEBUG_INFO, "Init File system\n"));
  if ((DevType != OsBootDeviceSpi) && (DevType != OsBootDeviceMemory) && (FsType < EnumFileSystemMax)) {
    Status = InitFileSystem (SwPart, FsType, HwPartHandle, FsHandle);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "No file system found, Status = %r\n", Status));
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

  Status = EFI_UNSUPPORTED;
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
      Status = ParseContainerImage (OsBootOption, LoadedImage);
    } else if ((LoadedImage->Flags & LOADED_IMAGE_COMPONENT) != 0) {
      Status = ParseComponentImage (OsBootOption, LoadedImage);
    } else if ((LoadedImage->Flags & LOADED_IMAGE_LINUX) != 0) {
      Status = EFI_SUCCESS;
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
  LOADED_IMAGE        *LoadedPreOsImage;
  LOADED_IMAGE        *LoadedExtraImage;
  EFI_STATUS           Status;
  UINT8                Type;

  //
  // Check if extra images exist and need setup
  //
  for (Type = LoadImageTypeExtra0; Type < LoadImageTypeMax; Type++) {
    Status = GetLoadedImageByType (LoadedImageHandle, Type, &LoadedExtraImage);
    if (EFI_ERROR (Status)) {
      break;
    }
    if ((LoadedExtraImage != NULL) && ((LoadedExtraImage->Flags & LOADED_IMAGE_RUN_EXTRA) != 0)) {
      Status = SetupBootImage (LoadedExtraImage);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  GetLoadedImageByType (LoadedImageHandle, LoadImageTypeNormal, &LoadedImage);
  GetLoadedImageByType (LoadedImageHandle, LoadImageTypePreOs, &LoadedPreOsImage);

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
  if (LoadedPreOsImage != NULL) {
    DEBUG ((DEBUG_INFO, "SetupBootImage ImageType-%d\n", LoadImageTypePreOs));
    Status = SetupBootImage (LoadedPreOsImage);
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
  LOADED_IMAGE        *LoadedPreOsImage;
  LOADED_IMAGE        *LoadedExtraImages;
  EFI_STATUS           Status;


  Status = GetLoadedImageByType (LoadedImageHandle, LoadImageTypeNormal, &LoadedImage);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = GetLoadedImageByType (LoadedImageHandle, LoadImageTypePreOs, &LoadedPreOsImage);
  Status = GetLoadedImageByType (LoadedImageHandle, LoadImageTypeExtra0, &LoadedExtraImages);
  return UpdateOsParameters (OsBootOption, LoadedImage, LoadedPreOsImage, LoadedExtraImages);
}

EFI_STATUS
EFIAPI
StartBootImages (
  IN  EFI_HANDLE      LoadedImageHandle
  )
{
  LOADED_IMAGE        *LoadedImage;
  LOADED_IMAGE        *LoadedPreOsImage;
  LOADED_IMAGE        *LoadedExtraImage;
  EFI_STATUS           Status;
  UINT8                Type;

  //
  // Check if extra images exist and need start before OS image.
  //
  for (Type = LoadImageTypeExtra0; Type < LoadImageTypeMax; Type++) {
    Status = GetLoadedImageByType (LoadedImageHandle, Type, &LoadedExtraImage);
    if (EFI_ERROR (Status)) {
      break;
    }
    if ((LoadedExtraImage != NULL) && ((LoadedExtraImage->Flags & LOADED_IMAGE_RUN_EXTRA) != 0)) {
      // For now, only RTCM will be supported.
      Status = CallExtraModule (PLD_EXTRA_MOD_RTCM, LoadedExtraImage);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  Status = GetLoadedImageByType (LoadedImageHandle, LoadImageTypeNormal, &LoadedImage);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetLoadedImageByType (LoadedImageHandle, LoadImageTypePreOs, &LoadedPreOsImage);
  if (EFI_ERROR (Status)) {
    Status = StartBooting (LoadedImage);
  } else {
    // PreOs found, need start PreOS
    Status = StartPreOsBooting (LoadedPreOsImage, LoadedImage);
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
  EFI_STATUS           Status;
  EFI_HANDLE           HwPartHandle;
  EFI_HANDLE           FsHandle;
  EFI_HANDLE           LoadedImageHandle;
  DEVICE_BLOCK_INFO    DevBlkInfo;
  UINT8                OldHwPart;
  UINT8                HwPart;
  UINT8                StartPart;
  UINT8                EndPart;
  OS_BOOT_MEDIUM_TYPE  MediaType;

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
    goto Exit;
  }


  //
  // For USB devices, try to boot from each of them until a success or
  // reaching the end of the list. This is because it is hard to have fixed
  // order on the USB devices if multiple devices exist in system.
  //
  MediaType = MediaGetInterfaceType ();
  OldHwPart = OsBootOption->HwPart;
  if ((MediaType == OsBootDeviceUsb) && (OldHwPart == 0xFF)) {
    StartPart = 0;
    EndPart   = 0x10;
  } else {
    StartPart = OldHwPart;
    EndPart   = OldHwPart;
  }

  for (HwPart = StartPart; HwPart <= EndPart; HwPart++) {

    OsBootOption->HwPart = HwPart;

    //
    // Check if it is a valid HW part using MediaGetMediaInfo
    //
    Status = MediaGetMediaInfo (HwPart, &DevBlkInfo);
    if (EFI_ERROR (Status)) {
      break;
    }

    DEBUG ((DEBUG_INFO, "Try HwPart %d\n", HwPart));

    //
    // Find Boot Partition
    //
    Status = FindBootPartitions (OsBootOption, &HwPartHandle);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Failed to Find Boot Partitions - HwPart %d\n", OsBootOption->HwPart));
    }

    //
    // Init File System
    //
    if (!EFI_ERROR (Status)) {
      Status = InitBootFileSystem (OsBootOption, HwPartHandle, &FsHandle);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "Failed to Initialize Boot File System - SwPart %d\n", OsBootOption->SwPart));
      }
    }

    //
    // Load Boot Image
    //
    if (!EFI_ERROR (Status)) {
      Status = LoadBootImages (OsBootOption, HwPartHandle, FsHandle, &LoadedImageHandle);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "Failed to Load Boot Image\n"));
      }
    }

    //
    // Error handling
    //
    if (EFI_ERROR (Status)) {
      if (LoadedImageHandle != NULL) {
        UnloadBootImages (LoadedImageHandle, FALSE);
        LoadedImageHandle = NULL;
      }

      if (FsHandle != NULL) {
        CloseFileSystem (FsHandle);
        FsHandle = NULL;
      }

      if (HwPartHandle != NULL) {
        ClosePartitions (HwPartHandle);
        HwPartHandle = NULL;
      }
    } else {
      break;
    }

  }

  AddMeasurePoint (0x4070);
  OsBootOption->HwPart = OldHwPart;

  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Parse Boot Image
  //
  Status = ParseBootImages (OsBootOption, LoadedImageHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Parse Boot Image\n"));
    goto Exit;
  }

  //
  // Setup Boot Image
  //
  Status = SetupBootImages (OsBootOption, LoadedImageHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Setup Boot Image - Status = %r\n", Status));
    goto Exit;
  }
  AddMeasurePoint (0x40B0);

  //
  // Update Boot Parameters
  //
  Status = UpdateBootParameters (OsBootOption, LoadedImageHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to Update Boot Parameters - Status = %r\n", Status));
    goto Exit;
  }

  //
  // Start Boot
  //
  Status = StartBootImages (LoadedImageHandle);

Exit:
  if (LoadedImageHandle != NULL) {
    UnloadBootImages (LoadedImageHandle, FALSE);
  }

  if (FsHandle != NULL) {
    CloseFileSystem (FsHandle);
  }

  if (HwPartHandle != NULL) {
    ClosePartitions (HwPartHandle);
  }

DEBUG_CODE_BEGIN();
  PrintStackHeapInfo ();
DEBUG_CODE_END();

  return Status;
}

/**
  Initialize platform console.

  @param[in]  ForceFbConsole   Always initialize frame buffer

  @retval  EFI_NOT_FOUND    No additional console was found.
  @retval  EFI_SUCCESS      Console has been initialized successfully.
  @retval  Others           There is error during console initialization.
**/
EFI_STATUS
LocalConsoleInit (
  IN  BOOLEAN   ForceFbConsole
)
{
  UINTN                     CtrlPciBase;
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

  if (((PcdGet32 (PcdConsoleOutDeviceMask) & ConsoleOutFrameBuffer) != 0) || ForceFbConsole) {
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
  LocalConsoleInit (FALSE);

  Shell (Timeout);
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
  OS_BOOT_OPTION         OsBootOption;
  BOOLEAN                BootShell;
  UINTN                  ShellTimeout;
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
    if (OsBootOptionList->RestrictedBoot != 0) {
      BootShell    = FALSE;
    } else {
      if (OsBootOptionList->BootToShell != 0) {
        ShellTimeout = (UINTN)PcdGet16 (PcdPlatformBootTimeOut);
        BootShell    = TRUE;
      } else if (DebugCodeEnabled ()) {
        ShellTimeout = (UINTN)PcdGet16 (PcdPlatformBootTimeOut);
        BootShell    = TRUE;
      }
    }
  }

  if (BootShell) {
    RunShell (ShellTimeout);
  }
  AddMeasurePoint (0x4020);

  while (OsBootOptionList != NULL) {

    // Print BOOT Option List
    DEBUG_CODE_BEGIN ();
    PrintBootOptions (OsBootOptionList);
    DEBUG_CODE_END ();

    // Load and run Image in order from OsImageList
    BootIdx = 0;
    CurrIdx = GetCurrentBootOption (OsBootOptionList, 0);
    while  (BootIdx < OsBootOptionList->OsBootOptionCount) {
      mCurrentBoot = CurrIdx;
      DEBUG ((DEBUG_INFO, "\n======== Try Booting with Boot Option %d ========\n", CurrIdx));

      // Get current boot option and try boot
      CopyMem ((VOID *)&OsBootOption, (VOID *)&OsBootOptionList->OsBootOption[CurrIdx], sizeof (OS_BOOT_OPTION));
      BootOsImage (&OsBootOption);

      // De-init the current boot devices
      // If USB keyboard console is used, don't DeInit USB yet at this moment.
      // It will be handled just before transfering to OS.
      if (!((OsBootOption.DevType == OsBootDeviceUsb) &&
          ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInUsbKeyboard) != 0))) {
        MediaInitialize (0, DevDeinit);
      }

      if (OsBootOptionList->RestrictedBoot != 0) {
        // Restricted boot should not try other boot option
        break;
      } else {
        // Move to next boot option
        CurrIdx = GetNextBootOption (OsBootOptionList, CurrIdx);
        if (CurrIdx >= OsBootOptionList->OsBootOptionCount) {
          CurrIdx = 0;
        }
        BootIdx++;
      }
    }

    if ((DebugCodeEnabled () || (OsBootOptionList->BootToShell != 0)) && (OsBootOptionList->RestrictedBoot == 0)) {
      // Restricted boot should not fall back to shell
      RunShell (0);
    } else {
      break;
    }
  }

  CpuHalt (NULL);
}
