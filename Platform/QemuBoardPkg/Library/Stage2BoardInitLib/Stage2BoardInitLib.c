/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Stage2BoardInitLibInternal.h>

STATIC
CONST EFI_ACPI_TEST_TABLE  mAcpiTestTableTemplate = {
  {
    SIGNATURE_32('T', 'E', 'S', 'T'),
    sizeof (EFI_ACPI_TEST_TABLE),
    1,     // Revision
    0x00,  // Checksum will be updated at runtime
    EFI_ACPI_OEM_ID,                 // OEM ID is a 6 bytes long field
    EFI_ACPI_OEM_TABLE_ID,           // OEM Table ID(8 bytes long)
    EFI_ACPI_OEM_REVISION,           // OEM Revision
    EFI_ACPI_CREATOR_ID,             // Creator ID
    EFI_ACPI_CREATOR_REVISION        // Creator Revision
  },
  0
};

STATIC
CONST EFI_ACPI_COMMON_HEADER *mPlatformAcpiTblTmpl[] = {
  (EFI_ACPI_COMMON_HEADER *)&mAcpiTestTableTemplate,
  NULL
};

STATIC
CONST DISPLAY_TIMING_INFO mDefaultTiming = {
  40000, 60,
  800,   40, 128, 88, 1,
  600,   1,  4,   23, 1
};

UINT8
GetGraphicsDeviceNumber (
  VOID
);

/**
  Test variable services.

  @retval   EFI_SUCCESS    Test completed successfully.
            EFI_ABORTED    Test failed.

**/
EFI_STATUS
TestVariableService (
  VOID
  )
{
  UINT32      Data;
  UINTN       DataSize;
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "Test variable services\n"));

  DataSize   = sizeof(Data);
  Status   = GetVariable ("VARTST0", NULL, &DataSize, &Data);
  if (!EFI_ERROR(Status) && (Data == 0x55667788) && (sizeof(Data) == DataSize)) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR(Status) || (Data != 0x11223344) || (sizeof(Data) != DataSize)) {
    return EFI_ABORTED;
  }

  Data       = 0x55667788;
  Status     = SetVariable ("VARTST0", 0, sizeof(Data), &Data);

  Data       = 0;
  DataSize   = sizeof(Data);
  if (!EFI_ERROR(Status)) {
    Status   = GetVariable ("VARTST0", NULL, &DataSize, &Data);
  }

  if (!EFI_ERROR(Status) && (Data == 0x55667788) && (sizeof(Data) == DataSize)) {
    return EFI_SUCCESS;
  } else {
    return EFI_ABORTED;
  }
}

/**
  Initialization of the GPIO table specific to each SOC. First find the relevant GPIO Config Data based on the Platform ID.
  Once the GPIO table data is fetched from configuration region, program the GPIO PADs and interrupt registers.

  @param NONE

**/
VOID
GpioInit (
  VOID
  )
  {
  GPIO_CFG_HDR       *GpioCfgCurrHdr;
  GPIO_CFG_HDR       *GpioCfgBaseHdr;
  GPIO_CFG_HDR       *GpioCfgHdr;
  UINT32              GpioEntries;
  UINT32              Index;
  UINT32              Offset;
  UINT8              *GpioCfgDataBuffer;
  UINT8              *GpioTable;

  DEBUG ((DEBUG_INFO, "Board GPIO Init\n"));

  //Find the GPIO CFG HDR
  GpioCfgCurrHdr = (GPIO_CFG_HDR *)FindConfigDataByTag (CDATA_GPIO_TAG);
  if (GpioCfgCurrHdr == NULL) {
    DEBUG ((DEBUG_ERROR, "No GPIO CFGDATA found !\n"));
    return;
  }

  GpioEntries    = 0;
  GpioCfgBaseHdr = NULL;

  //Find the GPIO CFG Data based on Platform ID. GpioTableData is the start of the GPIO entries
  if (GpioCfgCurrHdr->GpioBaseTableId < 16) {
    DEBUG ((DEBUG_INFO, "Get base platform GPIO table from board ID %d\n", GpioCfgCurrHdr->GpioBaseTableId));
    GpioCfgBaseHdr = (GPIO_CFG_HDR *)FindConfigDataByPidTag (GpioCfgCurrHdr->GpioBaseTableId, CDATA_GPIO_TAG);
    if (GpioCfgBaseHdr == NULL) {
      DEBUG ((DEBUG_ERROR, "Cannot find base GPIO table for platform ID %d\n", GpioCfgCurrHdr->GpioBaseTableId));
      return;
    }
    if (GpioCfgCurrHdr->GpioItemSize != GpioCfgBaseHdr->GpioItemSize) {
      DEBUG ((DEBUG_ERROR, "Inconsistent GPIO item size\n"));
      return;
    }
    GpioCfgHdr = GpioCfgBaseHdr;
  } else {
    GpioCfgHdr = GpioCfgCurrHdr;
  }

  Offset     = 0;
  GpioTable  = (UINT8 *)AllocateTemporaryMemory (0);  //allocate new buffer
  GpioCfgDataBuffer = GpioTable;

  if (GpioCfgBaseHdr != NULL) {
    for (Index = 0; Index  < GpioCfgHdr->GpioItemCount; Index++) {
      if (GpioCfgCurrHdr->GpioBaseTableBitMask[Index >> 3] & (1 << (Index & 7))) {
        CopyMem (GpioTable, GpioCfgHdr->GpioTableData + Offset, GpioCfgHdr->GpioItemSize);
        GpioTable += GpioCfgHdr->GpioItemSize;
        GpioEntries++;
      }
      Offset += GpioCfgHdr->GpioItemSize;
    }
  }

  if (GpioCfgCurrHdr != NULL) {
    CopyMem (GpioTable, GpioCfgCurrHdr->GpioTableData, GpioCfgCurrHdr->GpioItemCount * GpioCfgCurrHdr->GpioItemSize);
    GpioEntries += GpioCfgCurrHdr->GpioItemCount;
  }

  DEBUG ((DEBUG_INFO, "Programming %d GPIO entries\n", GpioEntries));

  GpioConfigurePads (GpioEntries, (GPIO_INIT_CONFIG *) GpioCfgDataBuffer);
}

/**
  Update current boot Payload ID.

**/
VOID
UpdatePayloadId (
  VOID
  )
{
  UINT8                BootDev;
  GEN_CFG_DATA        *GenericCfgData;

  GenericCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenericCfgData != NULL) {
    if (GenericCfgData->PayloadId == AUTO_PAYLOAD_ID_SIGNATURE) {
      // Use QEMU 3rd bootorder to select the payload
      // Add QEMU command line parameter:
      //   default           : enable OsLoader
      //   '-boot order=??a' : enable EFI payload
      //   '-boot order=??c' : enable LINUX payload
      // Here ?? is the normal boot devices.
      IoWrite8 (0x70, 0x38);
      BootDev = IoRead8 (0x71) >> 4;
      if (BootDev == 1) {
        // Boot UEFI payload
        SetPayloadId (UEFI_PAYLOAD_ID_SIGNATURE);
      } else if (BootDev == 2) {
        // Boot LINUX payload
        SetPayloadId (LINX_PAYLOAD_ID_SIGNATURE);
      } else {
        // Boot OsLoader
        SetPayloadId (0);
      }
    } else {
      SetPayloadId (GenericCfgData->PayloadId);
    }
  }
}

/**
  Initialize graphics for QEMU.

  @retval   EFI_SUCCESS            Graphics was initialized successfully.
            EFI_OUT_OF_RESOURCES   No enough memory to build HOB.

**/
EFI_STATUS
EmuGraphicsInit (
  VOID
  )
{
  EFI_STATUS                          Status;
  UINT32                              DevPciBase;
  GRAPHICS_INIT_POLICY                GfxPolicy;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE   GopMode;
  EFI_PEI_GRAPHICS_INFO_HOB          *GfxInfoHob;
  GRAPHICS_DATA                      *QemuVbt;

  GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (GetFspHobListPtr(), NULL, &gEfiGraphicsInfoHobGuid);
  if (GfxInfoHob != NULL) {
    // If FSP initialized GFX, skip.
    return EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "Native Emu GFX init\n"));

  ZeroMem (&GfxPolicy, sizeof(GfxPolicy));
  GfxPolicy.PciTempResourceBase  = PcdGet32 (PcdPciResourceMem32Base);
  CopyMem (&GfxPolicy.DisaplayTimingInfo, &mDefaultTiming, sizeof(DISPLAY_TIMING_INFO));

  QemuVbt = (GRAPHICS_DATA *)(UINTN)PcdGet32(PcdGraphicsVbtAddress);
  if ((QemuVbt != NULL) && (QemuVbt->Signature == GRAPHICS_DATA_SIG)) {
    GfxPolicy.DisaplayTimingInfo.Hdisp = QemuVbt->ResX;
    GfxPolicy.DisaplayTimingInfo.Vdisp = QemuVbt->ResY;
  }

  DevPciBase = ((UINT32) GetGraphicsDeviceNumber ()) << 8;
  Status = GraphicsInit (DevPciBase, &GfxPolicy);
  if (!EFI_ERROR (Status)) {
    Status = GetGraphicOutputModeInfo (&GopMode);
    if (!EFI_ERROR (Status)) {
      GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)BuildGuidHob (
                  &gEfiGraphicsInfoHobGuid,
                  sizeof (EFI_PEI_GRAPHICS_INFO_HOB));
      if (GfxInfoHob != NULL) {
        GfxInfoHob->GraphicsMode.Version              = GopMode.Info->Version;
        GfxInfoHob->GraphicsMode.HorizontalResolution = GopMode.Info->HorizontalResolution;
        GfxInfoHob->GraphicsMode.VerticalResolution   = GopMode.Info->VerticalResolution;
        GfxInfoHob->GraphicsMode.PixelFormat          = GopMode.Info->PixelFormat;
        GfxInfoHob->GraphicsMode.PixelInformation     = GopMode.Info->PixelInformation;
        GfxInfoHob->GraphicsMode.PixelsPerScanLine    = GopMode.Info->PixelsPerScanLine;
        GfxInfoHob->FrameBufferBase                   = GopMode.FrameBufferBase;
        GfxInfoHob->FrameBufferSize                   = (UINT32)GopMode.FrameBufferSize;
      } else {
        Status = EFI_OUT_OF_RESOURCES;
      }
    }
  }

  return Status;
}

/**
  Add a Smbios type string into a buffer

**/
STATIC
EFI_STATUS
AddSmbiosTypeString (
  SMBIOS_TYPE_STRINGS  *Dest,
  UINT8                 Type,
  UINT8                 Index,
  CHAR8                *String
  )
{
  UINTN   Length;

  Dest->Type    = Type;
  Dest->Idx     = Index;
  if (String != NULL) {
    Length = AsciiStrLen (String);

    Dest->String  = (CHAR8 *)AllocateZeroPool (Length + 1);
    if (Dest->String == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (Dest->String, String, Length);
  }

  return EFI_SUCCESS;
}

/**
  Initialize necessary information for Smbios

  @retval EFI_SUCCESS             Initialized necessary information successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory for Smbios info

**/
EFI_STATUS
InitializeSmbiosInfo (
  VOID
  )
{
  CHAR8                 TempStrBuf[SMBIOS_STRING_MAX_LENGTH];
  UINT16                Index;
  UINTN                 Length;
  SMBIOS_TYPE_STRINGS  *TempSmbiosStrTbl;
  BOOT_LOADER_VERSION  *VerInfoTbl;
  VOID                 *SmbiosStringsPtr;

  Index             = 0;
  TempSmbiosStrTbl  = (SMBIOS_TYPE_STRINGS *) AllocateTemporaryMemory (0);
  VerInfoTbl    = GetVerInfoPtr ();

  //
  // SMBIOS_TYPE_BIOS_INFORMATION
  //
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
    1, "Intel Corporation");
  if (VerInfoTbl != NULL) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf),
      "SB_QEMU.%03d.%03d.%03d.%03d.%05d.%c\0",
      VerInfoTbl->ImageVersion.CoreMajorVersion,
      VerInfoTbl->ImageVersion.CoreMinorVersion,
      VerInfoTbl->ImageVersion.ProjMajorVersion,
      VerInfoTbl->ImageVersion.ProjMinorVersion,
      VerInfoTbl->ImageVersion.BuildNumber,
      VerInfoTbl->ImageVersion.BldDebug ? 'D' : 'R');
  } else {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a", "Unknown");
  }
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
    2, TempStrBuf);
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
    3, __DATE__);

  //
  // SMBIOS_TYPE_SYSTEM_INFORMATION
  //
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    1, "Intel Corporation");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    2, "QEMU Virtual Platform");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    3, "0.1");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    4, "System Serial Number");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    5, "System SKU Number");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    6, "Virtual System");

  //
  // SMBIOS_TYPE_BASEBOARD_INFORMATION
  //
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
    1, "Intel Corporation");
  AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%8a (ID:%02X)", GetPlatformName(), GetPlatformId ());
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
    2, TempStrBuf);
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
    3, "1");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
    4, "Board Serial Number");

  //
  // SMBIOS_TYPE_END_OF_TABLE
  //
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_END_OF_TABLE,
    0, NULL);

  Length = sizeof (SMBIOS_TYPE_STRINGS) * Index;
  SmbiosStringsPtr = AllocatePool (Length);
  if (SmbiosStringsPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (SmbiosStringsPtr, TempSmbiosStrTbl, Length);
  (VOID) PcdSet32S (PcdSmbiosStringsPtr, (UINT32)(UINTN)SmbiosStringsPtr);
  (VOID) PcdSet16S (PcdSmbiosStringsCnt, Index);

  return EFI_SUCCESS;
}


/**
  Board specific hook point.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE    InitPhase
)
{
  EFI_STATUS            Status;
  UINT32                TsegBase;
  UINT64                TsegSize;
  VOID                 *Buffer;
  UINT32                Length;
  UINT32                PmBase;
  VOID                 *FspHobList;

  switch (InitPhase) {
  case PreSiliconInit:
    if (GetPlatformId () == PLATFORM_ID_QSP_SIMICS) {
      // Update PCI Memory 32 base address for QSP
      Status = PcdSet32S (PcdPciResourceMem32Base, 0xF0000000);
    }
    GpioInit ();
    EnableLegacyRegions ();
    if (!FeaturePcdGet (PcdStage1BXip)) {
      if (GetPlatformId () != PLATFORM_ID_QSP_SIMICS) {
        SpiConstructor ();
        VariableConstructor (PcdGet32 (PcdVariableRegionBase), PcdGet32 (PcdVariableRegionSize));
        Status = TestVariableService ();
        ASSERT_EFI_ERROR (Status);
      }
    }
    // Get TSEG info from FSP HOB
    // It will be consumed in MpInit if SMM rebase is enabled
    TsegBase = 0;
    FspHobList = GetFspHobListPtr ();
    if (FspHobList != NULL) {
      TsegBase = (UINT32)GetFspReservedMemoryFromGuid (
                        FspHobList,
                        &TsegSize,
                        &gReservedMemoryResourceHobTsegGuid
                        );
    }
    if (TsegBase != 0) {
      Status = PcdSet32S (PcdSmramTsegBase, TsegBase);
      Status = PcdSet32S (PcdSmramTsegSize, (UINT32)TsegSize);
    }
    // Load IP firmware from container
    Buffer = NULL;
    Length = 0;
    Status = LoadComponent (SIGNATURE_32('I', 'P', 'F', 'W'), SIGNATURE_32('T', 'S', 'T', '3'), &Buffer,  &Length);
    DEBUG ((DEBUG_INFO, "Load IP firmware @ %p:0x%X - %r\n", Buffer, Length, Status));
    if (!EFI_ERROR(Status)) {
      DumpHex (2, 0, Length > 16 ? 16 : Length, Buffer);
    }
    // Prepare platform ACPI tempate
    Status = PcdSet32S (PcdAcpiTableTemplatePtr, (UINT32)(UINTN)mPlatformAcpiTblTmpl);
    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      UpdatePayloadId ();
    }
    break;

  case PostSiliconInit:
    // Initialize Smbios Info
    if (FeaturePcdGet (PcdSmbiosEnabled)) {
      InitializeSmbiosInfo ();
    }
    // Open TSEG so that MpInit can do SMM rebasing if required
    PciAnd8 (PCI_LIB_ADDRESS(0, 0, 0, MCH_ESMRAMC), (UINT8)~MCH_ESMRAMC_T_EN);
    PciAndThenOr8 (PCI_LIB_ADDRESS(0, 0, 0, MCH_SMRAM), (UINT8)~MCH_SMRAM_D_CLOSE, MCH_SMRAM_D_OPEN);
    if (PcdGetBool (PcdFramebufferInitEnabled)) {
      EmuGraphicsInit ();
    }
    break;

  case PostPciEnumeration:
    Status = SetFrameBufferWriteCombining (0, MAX_UINT32);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "Failed to set GFX framebuffer as WC\n"));
    }
    break;

  case PrePayloadLoading:
    // Hide TSEG
    PciOr8 (PCI_LIB_ADDRESS(0, 0, 0, MCH_ESMRAMC), MCH_ESMRAMC_T_EN);
    // Close TSEG
    PciAndThenOr8 (PCI_LIB_ADDRESS(0, 0, 0, MCH_SMRAM), (UINT8)~MCH_SMRAM_D_OPEN, MCH_SMRAM_D_CLOSE);
    if (PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE) {
      PciOr8 (PCI_LIB_ADDRESS(0, 0, 0, MCH_SMRAM), MCH_SMRAM_D_LCK);
      // Lock down SMI_EN register
      PmBase = PciRead32 (POWER_MGMT_REGISTER_Q35 (ICH9_PMBASE)) & ICH9_PMBASE_MASK;
      IoWrite32 (PmBase + ICH9_PMBASE_OFS_SMI_EN, 0);
      // Prevent software from undoing the above (until platform reset).
      PciOr16 (POWER_MGMT_REGISTER_Q35 (ICH9_GEN_PMCON_1),  ICH9_GEN_PMCON_1_SMI_LOCK);
    }
    break;

  default:
    break;
  }
}



/**
  This function overrides the default configurations in the FSP-S UPD data region.

  @param[in,out] FspUpdRgnPtr   A pointer to the UPD data region data strcture.

**/
VOID
EFIAPI
UpdateFspConfig (
  IN OUT VOID        *FspUpdRgnPtr
  )
{
  FSPS_UPD           *FspsUpd;
  FSP_S_CONFIG       *FspsConfig;

  FspsUpd    = (FSPS_UPD *)FspUpdRgnPtr;
  FspsConfig = &FspsUpd->FspsConfig;

  if (PcdGetBool (PcdFramebufferInitEnabled)) {
    FspsConfig->GraphicsConfigPtr = (UINT32)GetVbtAddress ();
    FspsConfig->PciTempResourceBase = PcdGet32 (PcdPciResourceMem32Base);
  } else {
    FspsConfig->GraphicsConfigPtr = 0;
  }
}

/**
  Save MRC data onto the Flash region from NVS buffer.

  @param Buffer pointer to an NVS data
  @param Length Length of the NVS data(MRC)

  @retval EFI_SUCCESS
  @retval RETURN_ALREADY_STARTED
  @retval EFI_DEVICE_ERROR
  @retval EFI_INVALID_PARAMETER
 */
EFI_STATUS
EFIAPI
SaveNvsData (
  IN  VOID    *Buffer,
  IN  UINT32   Length
)
{
  return EFI_UNSUPPORTED;
}

/**
  Update serial port information to global HOB data structure.

  @param SerialPortInfo  Pointer to global HOB data structure.
 **/
VOID
EFIAPI
UpdateSerialPortInfo (
  IN  SERIAL_PORT_INFO  *SerialPortInfo
)
{
  SerialPortInfo->Type       = 1;
  SerialPortInfo->BaseAddr64 = GetSerialPortBase ();
  SerialPortInfo->BaseAddr   = (UINT32) SerialPortInfo->BaseAddr64;
  SerialPortInfo->RegWidth = GetSerialPortStrideSize();
}

/**
 Update the OS boot option

 @param OsBootOptionList pointer to boot option list.
 **/
VOID
EFIAPI
UpdateOsBootMediumInfo (
  OUT  OS_BOOT_OPTION_LIST   *OsBootOptionList
)
{
  UINT8         BootOrder;

  FillBootOptionListFromCfgData (OsBootOptionList);

  if (OsBootOptionList->CurrentBoot == MAX_BOOT_OPTION_ENTRY - 1) {
    //
    // Read boot order, it is passed in by QEMU command
    //   QEMU boot order (a=1 c=2 d=3 n=4 at CMOS 0x3D)
    //   By default, this CMOS value at offset 0x3D is 0x03
    //   Use '-boot order=c' or default to boot from eMMC
    //   Use '-boot order=d' to boot from SATA
    //   Use '-boot order=n' to boot from NVMe
    //   Use '-boot order=a' to boot to setup
    //
    IoWrite8 (0x70, 0x3D);
    BootOrder = IoRead8  (0x71);

    if ((BootOrder & 0x0F) == 3) {
      // SATA boot first
      OsBootOptionList->CurrentBoot = 1;
    } else if ((BootOrder & 0x0F) == 4) {
      // NVMe boot first
      OsBootOptionList->CurrentBoot = 2;
    } else if ((BootOrder & 0x0F) == 2) {
      // SD boot first
      OsBootOptionList->CurrentBoot = 0;
    } else if ((BootOrder & 0x0F) == 1) {
      // Build setup boot option to run MicroPython and setup script
      if (FeaturePcdGet (PcdEnableSetup)) {
        OsBootOptionList->OsBootOptionCount = 1;
        OsBootOptionList->CurrentBoot       = 0;
        OsBootOptionList->RestrictedBoot    = 1;
        ZeroMem (OsBootOptionList->OsBootOption, sizeof(OS_BOOT_OPTION));
        OsBootOptionList->OsBootOption[0].DevType   = OsBootDeviceMemory;
        OsBootOptionList->OsBootOption[0].FsType    = EnumFileSystemTypeAuto;
        CopyMem (OsBootOptionList->OsBootOption[0].Image[0].FileName, "!SETP/MPYM:STPY", 16);
      }
    }
  }

  if (OsBootOptionList->CurrentBoot >= OsBootOptionList->OsBootOptionCount) {
    OsBootOptionList->CurrentBoot = 0;
  }
}

/**
 Find the correct graphics PCI device number

 @retval   GFX PCI device number
 */
UINT8
GetGraphicsDeviceNumber (
  VOID
)
{
  UINT8            Dev;

  if (GetPlatformId () == PLATFORM_ID_QSP_SIMICS) {
    Dev = 15;
  } else {
    Dev = 1;
  }

  if (PciRead16 (PCI_LIB_ADDRESS(0, Dev, 0, 2)) != 0x1111) {
    Dev = 0xff;
    DEBUG ((DEBUG_INFO, "Could not find GFX device !\n"));
  }

  return Dev;
}

/**
 Update the frame buffer info by reading the PCI address

 @param[out] GfxInfo    pointer to global HOB data structure.
 */
VOID
UpdateFrameBufferInfo (
  OUT  EFI_PEI_GRAPHICS_INFO_HOB   *GfxInfo
)
{
  GRAPHICS_DATA   *GfxPtr;
  UINT8            Dev;

  GfxPtr = (GRAPHICS_DATA *)(UINTN)PcdGet32(PcdGraphicsVbtAddress);
  Dev = GetGraphicsDeviceNumber ();
  if (Dev < 32) {
    GfxInfo->FrameBufferBase = (UINT64)PciRead32 (PCI_LIB_ADDRESS(0, Dev, 0, 0x10)) & 0xFFFFFFF0;
    if (GfxPtr->Signature == GRAPHICS_DATA_SIG) {
      GfxInfo->GraphicsMode.HorizontalResolution = GfxPtr->ResX;
      GfxInfo->GraphicsMode.VerticalResolution   = GfxPtr->ResY;
    }
  }
}

/**
 Update the frame buffer device info.

 @param[out] GfxDeviceInfo   pointer to EFI_PEI_GRAPHICS_DEVICE_INFO_HOB structure.
 **/
VOID
UpdateFrameBufferDeviceInfo (
  OUT  EFI_PEI_GRAPHICS_DEVICE_INFO_HOB   *GfxDeviceInfo
  )
{
  UINT8            Dev;

  Dev = GetGraphicsDeviceNumber ();
  if (Dev < 32) {
    GfxDeviceInfo->BarIndex = 0;
    GfxDeviceInfo->VendorId = PciRead16 (PCI_LIB_ADDRESS (0, Dev, 0, 0));
    GfxDeviceInfo->DeviceId = PciRead16 (PCI_LIB_ADDRESS (0, Dev, 0, 2));
  }

  SetDeviceAddr (PlatformDeviceGraphics, 0, (0 << 16) | (Dev << 8) | 0);
}

/**
 Update loader SMM info.

 @param[out] SmmInfoHob     pointer to SMM information HOB

**/
VOID
UpdateSmmInfo (
  OUT  LDR_SMM_INFO           *SmmInfoHob
)
{
  UINT32  TsegSize;

  TsegSize = PcdGet32 (PcdSmramTsegSize);
  if (TsegSize > 0) {
    SmmInfoHob->SmmBase = PcdGet32 (PcdSmramTsegBase);
    SmmInfoHob->SmmSize = TsegSize;
    SmmInfoHob->Flags   = 0;
  }
}

/**
 Update Hob Info with platform specific data

 @param  Guid          The GUID to tag the customized HOB.
 @param  HobInfo       The start address of GUID HOB data.
 **/
VOID
EFIAPI
PlatformUpdateHobInfo (
  IN CONST EFI_GUID              *Guid,
  OUT      VOID                  *HobInfo
)
{
  ASSERT (Guid != NULL);
  ASSERT (HobInfo != NULL);

  // Just compare Guid memory addresses which are in Stage2 data section instead of CompareGuid ()
  if (Guid == &gEfiGraphicsInfoHobGuid) {
    UpdateFrameBufferInfo (HobInfo);
  } else if (Guid == &gEfiGraphicsDeviceInfoHobGuid) {
    UpdateFrameBufferDeviceInfo (HobInfo);
  } else if (Guid == &gLoaderSerialPortInfoGuid) {
    UpdateSerialPortInfo (HobInfo);
  } else if (Guid == &gOsBootOptionGuid) {
    UpdateOsBootMediumInfo (HobInfo);
  } else if (Guid == &gSmmInformationGuid) {
    UpdateSmmInfo (HobInfo);
  }
}

/**
  Update PCH NVS and SA NVS area address and size in ACPI table

  @param[in] Current       Pointer to ACPI description header

  @retval    EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
PlatformUpdateAcpiTable (
  IN UINT8                   *Current
)
{
  return EFI_SUCCESS;
}

/**
  Dynamic update of Global NVS data

  @param[in] GnvsIn       Pointer to global nvs region

**/
VOID
EFIAPI
PlatformUpdateAcpiGnvs (
  IN VOID  *GnvsIn
)
{
  EFI_GLOBAL_NVS_AREA  *Gnvs;

  Gnvs = (EFI_GLOBAL_NVS_AREA *)GnvsIn;
  SetMem (Gnvs, sizeof(EFI_GLOBAL_NVS_AREA), 0);

  Gnvs->PciWindow32.Base    = PcdGet32(PcdPciResourceMem32Base);
  if (GetPlatformId () == PLATFORM_ID_QSP_SIMICS) {
    Gnvs->PciWindow32.End   = IO_APIC_BASE_ADDRESS - 1;
  } else {
    Gnvs->PciWindow32.End   = PcdGet64(PcdPciExpressBaseAddress) - 1;
  }
  Gnvs->PciWindow32.Length  = Gnvs->PciWindow32.End - Gnvs->PciWindow32.Base + 1;

  Gnvs->PciWindow64.Base    = 0x0800000000;
  Gnvs->PciWindow64.End     = 0x0fffffffff;
  Gnvs->PciWindow64.Length  = Gnvs->PciWindow64.Base - Gnvs->PciWindow64.End + 1;

  SocUpdateAcpiGnvs ((VOID *)Gnvs);
}
