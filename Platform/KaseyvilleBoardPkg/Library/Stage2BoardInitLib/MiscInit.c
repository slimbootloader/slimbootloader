/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"

static const CHAR16 mBootVarName[] = L"BootOrder";

/**
  Platform specific initialization for BSP and APs.

  NOTE: If there is code to access common resource in this function,
        need use lock to avoid resource access conflict.

  @param[in] CpuIndex       CPU index to initialize.
**/
VOID
EFIAPI
PlatformCpuInit (
  UINT32         CpuIndex
  )
{
  UINT32    Msr;

  //
  // No need check secondary thread
  // since it will skip updating MSR if already locked
  //
  Msr = AsmReadMsr32 (MSR_IA32_FEATURE_CONTROL);
  if ((Msr & BIT0) != BIT0) {
    Msr |= BIT0;
    AsmWriteMsr32 (MSR_IA32_FEATURE_CONTROL, Msr);
  }
}

/**
  Platform specific Post Silicon Init

**/
VOID
PlatformPostSiliconInit (
  VOID
  )
{

  UINTN                     Address;
  UINT32                    TsegBase;
  UINT32                    TsegSize;
  EFI_HOB_GUID_TYPE         *GuidHob;
  IIO_UDS                   *IioUdsHobData;
  VOID                      *FspHobList;

  Address = MM_PCI_ADDRESS (0, 0, 0, 0);
  TsegBase = MmioRead32 (Address + 0xA8) & 0xFFF00000;
  TsegSize = (MmioRead32 (Address + 0xAC) & 0xFFF00000) - TsegBase;

  GuidHob = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gIioUdsHobGuid, FspHobList);
  }
  if (GuidHob != NULL) {
    IioUdsHobData = GET_GUID_HOB_DATA (GuidHob);
    if ((IioUdsHobData != NULL) && (IioUdsHobData->PlatformData.MemTsegSize != 0)) {
      TsegSize = IioUdsHobData->PlatformData.MemTsegSize;
    }
  }

  (VOID) PcdSet32S (PcdSmramTsegBase, TsegBase);
  (VOID) PcdSet32S (PcdSmramTsegSize, TsegSize);

  if (GetPayloadId() == 0) {
    (VOID)PcdSet32S (PcdFuncCpuInitHook, (UINT32)(UINTN) PlatformCpuInit);
  }

}

/**
  Platform specific initialization for PCI Enum.
  Preprocess the existing second bus on the PPB.

  @param[in] SecBus       Secondary Bus Number.
**/
VOID
EFIAPI
PlatformPciEnumHookProc (
  UINT8         Bus,
  UINT8         Dev,
  UINT8         Fun,
  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE Phase
  )
{
  UINTN         Address;
  UINT8         SecBus;
  UINT8         Device;
  UINT8         Func;

  Address = 0;
  SecBus  = 0;
  Device  = 0;
  Func    = 0;

  if (Phase == EfiPciBeforeChildBusEnumeration) {
    Address = MM_PCI_ADDRESS (Bus, Dev, Fun, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
    SecBus  = (UINT8)MmioRead8 (Address);

    for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
      for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
        Address = MM_PCI_ADDRESS (SecBus, Device, Func, 0);
        if (MmioRead16 (Address) == 0xFFFF) {
          MmioWrite32 (Address, 0xFFFFFFFF);
          Func = PCI_MAX_FUNC;
        }
      }
    }
  }

}

/**
  Platform specific Pre Pci Enumeration

**/
VOID
PlatformPrePciEnumeration (
  VOID
  )
{
  PCI_RES_ALLOC_TABLE       *ResAllocTable;
  PCI_RES_ALLOC_TABLE       *TempRes;
  PCI_RES_ALLOC_RANGE       *TempRange;
  EFI_HOB_GUID_TYPE         *GuidHob;
  IIO_UDS                   *IioUdsHobData;
  UINT8                     Stack;
  UINTN                     Length;
  VOID                      *FspHobList;
  UINT32                    P2sbBase;
  UDS_PCIROOT_RES           *PciRootRes;
  UINT8                     Root;

  GuidHob = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gIioUdsHobGuid, FspHobList);
  }
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_INFO, "Iio Data Hob not found!\n"));
    return;
  }

  IioUdsHobData = GET_GUID_HOB_DATA (GuidHob);

  TempRes = (PCI_RES_ALLOC_TABLE *)AllocateTemporaryMemory (0);
  ZeroMem (TempRes, sizeof (PCI_RES_ALLOC_TABLE));
  for (Stack = 0; Stack < MAX_LOGIC_IIO_STACK; Stack++) {
    for(Root = 0; Root < IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].PciRootBridgeNum; Root++) {
      PciRootRes = &IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].PciRoot[Root];
      TempRange               = &TempRes->ResourceRange[TempRes->NumOfEntries];
      TempRange->BusBase      = PciRootRes->BusBase;
      TempRange->BusLimit     = PciRootRes->BusLimit;
      TempRange->Reserved     = 0;
      TempRange->IoBase       = PciRootRes->IoBase;
      TempRange->IoLimit      = PciRootRes->IoLimit;
      TempRange->Mmio32Base   = PciRootRes->Mmio32Base;
      TempRange->Mmio32Limit  = PciRootRes->Mmio32Limit;
      TempRange->Mmio64Base   = PciRootRes->Mmio64Base;
      TempRange->Mmio64Limit  = PciRootRes->Mmio64Limit;
      if (TempRange->IoLimit < TempRange->IoBase) {
        TempRange->IoBase = (UINT32)-1; // It was 0xFFFF from UDS, extend to 0xFFFFFFFF
      }
      DEBUG ((DEBUG_VERBOSE, "-- Resource Alloc Entry-%d Info --\n", TempRes->NumOfEntries));
      DEBUG ((DEBUG_VERBOSE, "  BusBase    : 0x%02X\n", TempRange->BusBase));
      DEBUG ((DEBUG_VERBOSE, "  BusLimit   : 0x%02X\n", TempRange->BusLimit));
      DEBUG ((DEBUG_VERBOSE, "  IoBase     : 0x%X\n", TempRange->IoBase));
      DEBUG ((DEBUG_VERBOSE, "  IoLimit    : 0x%X\n", TempRange->IoLimit));
      DEBUG ((DEBUG_VERBOSE, "  Mmio32Base : 0x%X\n", TempRange->Mmio32Base));
      DEBUG ((DEBUG_VERBOSE, "  Mmio32Limit: 0x%X\n", TempRange->Mmio32Limit));
      DEBUG ((DEBUG_VERBOSE, "  Mmio64Base : 0x%llX\n", TempRange->Mmio64Base));
      DEBUG ((DEBUG_VERBOSE, "  Mmio64Limit: 0x%llX\n\n", TempRange->Mmio64Limit));

      TempRes->NumOfEntries++;
    } // Root loop
  } // Stack loop

  ResAllocTable = NULL;
  if (TempRes->NumOfEntries > 0) {
    Length  = sizeof (PCI_RES_ALLOC_TABLE);
    Length += sizeof (PCI_RES_ALLOC_RANGE) * TempRes->NumOfEntries;

    ResAllocTable = AllocateZeroPool (Length);
    CopyMem (ResAllocTable, TempRes, Length);
    (VOID)PcdSet32S (PcdPciResAllocTableBase, (UINT32)(UINTN)ResAllocTable);
  }

  (VOID) PcdSet32S (PcdPciEnumHookProc, (UINT32)(UINTN) PlatformPciEnumHookProc);

  //
  // Hide P2SB so that PCI enumeration doesn't change SBREG_BAR.
  // Some OS apps might use the fixed/hardcoded address previously.
  //
  P2sbBase = (UINT32)MM_PCI_ADDRESS( 0, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB, 0 );
  MmioWrite8 (P2sbBase + R_P2SB_CFG_E0 + 1, BIT0);
}

/**
  Platform specific Post Pci Enumeration

**/
VOID
PlatformPostPciEnumeration (
  VOID
  )
{
  UINT32  P2sbBase;
  BOOLEAN DevicePresent;

  if ((MmioRead8 (MM_PCI_ADDRESS (0, 3, 0, 0xFC)) & BIT2) != 0) {
    MmioOr16 (MM_PCI_ADDRESS (0, 0x12, 0, 0x3E), (BIT3 | BIT4));
  }

  //
  // Unhide and restore P2SB to be in Pre-PCI state
  //
  P2sbBase = (UINT32)MM_PCI_ADDRESS( 0, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB, 0 );
  DevicePresent = (MmioRead16 (P2sbBase + PCI_VENDOR_ID_OFFSET) != 0xFFFF);
  if (!DevicePresent) {
    MmioWrite8 (P2sbBase + R_P2SB_CFG_E0 + 1, 0);
  }
}

/**
  Update current boot Payload ID.

**/
VOID
UpdatePayloadId (
  VOID
  )
{
  UINT32            PayloadId;
  GPIOV2_PAD_STATE  PayloadSelGpioData;
  UINT32            PayloadSelGpioPad;
  EFI_STATUS        Status;
  GEN_CFG_DATA     *GenCfgData;
  PLDSEL_CFG_DATA  *PldSelCfgData;

  PayloadId = GetPayloadId ();
  GenCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenCfgData != NULL) {
    if (GenCfgData->PayloadId == AUTO_PAYLOAD_ID_SIGNATURE) {
      // OsLoader by default if PayloadId config is 'AUTO'
      PayloadId = 0;

      PldSelCfgData = (PLDSEL_CFG_DATA *)FindConfigDataByTag (CDATA_PLDSEL_TAG);
      if ((PldSelCfgData != NULL) && (PldSelCfgData->PldSelGpio.Enable != 0)) {
        //Community Index of GPIOV2_PAD_ID is hardcoded as '0' since we can only use hard GPIO(GPIO grp C,D and E) for external connection.
        //For hard GPIO community index is 0.
        PayloadSelGpioPad = GPIOV2_PAD_ID (0, GPIOV2_IBL_CHIPSET_ID, 0, 0, PldSelCfgData->PldSelGpio.PadGroup, PldSelCfgData->PldSelGpio.PinNumber);
        PayloadSelGpioData = GpioV2StateDefault;
        Status = GetRx (PayloadSelGpioPad, &PayloadSelGpioData);
        if (Status == EFI_SUCCESS) {
          PayloadSelGpioData = PayloadSelGpioData >> 1;
          DEBUG ((DEBUG_INFO, "PayloadSelGpio Pad:0x%08X Val:%d Polarity:%d\n",
            PayloadSelGpioPad, PayloadSelGpioData, PldSelCfgData->PldSelGpio.PinPolarity));
          if ((UINT32)PayloadSelGpioData != PldSelCfgData->PldSelGpio.PinPolarity) {
            PayloadId = UEFI_PAYLOAD_ID_SIGNATURE;
          }
        } else {
          DEBUG ((DEBUG_INFO, "Warning..!! GPIO read failed, Status:%r\nProceeding with AUTO(Osloader) payloadID", Status));
        }
      }
    } else {
      PayloadId = GenCfgData->PayloadId;
    }
  }

  SetPayloadId (PayloadId);
}

/**
  Initialize available platform services for Payload

**/
VOID
PlatformServiceInit (
  VOID
  )
{
  PLATFORM_SERVICE     *PlatformService;

  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL) {
    PlatformService->ResetSystem = ResetSystem;
  }
}

/**
  Dump Debug Info (Print HECI info and Read Uefi Variable)

**/
VOID
DumpDebugInfo (
  VOID
  )
{
DEBUG_CODE_BEGIN ();

  EFI_STATUS                Status;
  UINTN                     VariableLen;
  CHAR8                     Buffer[VAR_LENGTH_VAR_STS + 1];
  UINT32                    Address;
  UINT32                    VarSize;
  UINTN                     Data;

  ZeroMem (Buffer, sizeof (Buffer));
  VariableLen = VAR_LENGTH_VAR_STS;
  Status = GetVariable (VAR_NAME_VAR_STS, NULL, NULL, &VariableLen, Buffer);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "GetVariable: %a %a(%d)\n", VAR_NAME_VAR_STS, Buffer, VariableLen));
  } else {
    DEBUG ((DEBUG_INFO, "No %a found!\n", VAR_NAME_VAR_STS));
  }

  Status = GetComponentInfo (FLASH_MAP_SIG_UEFIVARIABLE, &Address, &VarSize);
  DEBUG ((DEBUG_INFO, "UEfi Variable Address 0x%0X, VariSize 0x%0X \n", (VOID *)(UINTN)Address, (VOID *)(UINTN)VarSize));

  VariableLen = sizeof (mBootVarName) / 2;
  DEBUG ((DEBUG_INFO, "VariableLen of BootVariName %0d\n", VariableLen));
  Data = 1;

  Status = UefiGetVariable (mBootVarName, &gEfiGlobalVariableGuid, NULL,  &VariableLen, &Data);

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Found 'BootOrder' string in UefiVariable\n"));
  }
DEBUG_CODE_END ();
}

/**
  Build MADT table at runtime using MadtLib

  @param[in,out] Table       Pointer to MADT ACPI Table header

  @retval    EFI_SUCCESS     Table built successfully
**/
EFI_STATUS
PatchMadtTable (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  EFI_STATUS  Status;

  //
  // Initialize MADT header
  //
  Status = AddAcpiMadtHdr (Table, EFI_ACPI_LOCAL_APIC_ADDRESS, EFI_ACPI_6_3_PCAT_COMPAT);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Add Local X2APIC entries for all detected processors
  // Only when platform owns LAPIC enumeration; otherwise AcpiInitLib's UpdateMadt() handles it
  //
  if (FeaturePcdGet (PcdMadtUsePlatformLapic)) {
    Status = AddMadtAllLocalApics (Table);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Add PCH IO APIC
  //
  AddMadtIoApic (Table, PCH_IOAPIC_ID, PCH_IOAPIC_ADDRESS, PCH_INTERRUPT_BASE);

  //
  // Add Interrupt Source Overrides
  // IRQ0 -> IRQ2 (8259 PIC cascade)
  //
  AddMadtIntSrcOverride (Table, 0x00, 0x00, 0x00000002, 0x0000);

  //
  // SCI (IRQ9) - Level-triggered, Active High
  //
  AddMadtIntSrcOverride (Table, 0x00, 0x09, 0x00000009, 0x000D);

  //
  // Add Local X2APIC NMI - All processors, Level-triggered Active High, LINT1
  //
  AddLocalX2ApicNmi (Table, 0x000D, 0xFFFFFFFF, 0x01);

  //
  // Calculate checksum
  //
  Table->Checksum = 0;
  Table->Checksum = CalculateCheckSum8 ((UINT8 *)Table, Table->Length);

  return EFI_SUCCESS;
}

/**
  Updates DMA Remapping Table (DMAR)

  @param[in] Table            Pointer to DMAR ACPI Table

  @retval    EFI_SUCCESS      Returned Successfully
  @retval    EFI_UNSUPPORTED  Returned Unsupported

**/
EFI_STATUS
PatchDmarTable(
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  PLATFORM_DATA_BL    *PlatformData;
  EFI_STATUS          Status;

  PlatformData = (PLATFORM_DATA_BL *)GetPlatformDataPtr ();
  if ((PlatformData != NULL) && PlatformData->PlatformFeatures.VtdEnable) {
    Status = UpdateDmarTable (Table);
  } else {
    DEBUG ((DEBUG_INFO, "PlatformData not found or Vtd not supported, returning\n"));
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  Updates BDAT

  @param[in] Table            Pointer to BDAT ACPI Table

  @retval    EFI_SUCCESS      Returned Successfully
  @retval    EFI_UNSUPPORTED  Returned Unsupported

**/
EFI_STATUS
PatchBdatTable(
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  EFI_STATUS        Status;
  VOID             *FspHobList;

  FspHobList = GetFspHobListPtr ();
  if (FspHobList == NULL) {
    return EFI_DEVICE_ERROR;
  }

  Status = UpdateBdatAcpiTable (Table, FspHobList);

  return Status;
}

/**
  Update the MCFG table with platform-specific configuration data.

  This serves as the main entry point for building the MCFG table at runtime.
  It initializes the table header and adds configuration space structures
  based on platform configuration.

  @param[in, out] AcpiTable   Pointer to the ACPI table buffer.

  @retval         EFI_SUCCESS            Returned Successfully
  @retval         EFI_INVALID_PARAMETER  Returned Invalid parameter

**/
EFI_STATUS
PatchMcfgTable (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER   *AcpiTable
  )
{
  EFI_STATUS                      Status;

  // Initialize the table header
  Status = AddAcpiMcfgHdr (AcpiTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Add configuration space structure for single root complex.
  Status = AddMcfgConfigSpaceStruct (
                            AcpiTable,
                            PcdGet64(PcdPciExpressBaseAddress), // BaseAddress - to be updated by platform
                            0,                // PciSegmentGroupNumber
                            0,                // StartBusNumber
                            0xFF              // EndBusNumber
                            );
  return Status;
}

/**
  Initialize BMC Video for Splash screen

**/
VOID
InitializeBmcVideo (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT32                                PciDevice;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE     GopMode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  GopModeInfo;
  UINTN                                 PciBase;
  UINT16                                Bus;
  EFI_PEI_GRAPHICS_INFO_HOB             *GfxInfoHob;
  DISPLAY_TIMING_INFO                   Timings[2] = {
    { 40000, 60,  800, 40, 128,  88, 1, 600, 1, 4, 23, 1 },
    { 78800, 75, 1024, 16,  96, 176, 1, 768, 1, 3, 28, 1 }
  };

  if (FeaturePcdGet (PcdSplashEnabled)) {
    for (Bus = 1, PciDevice = 0; Bus <= PCI_MAX_BUS; Bus++) {
      PciBase = MM_PCI_ADDRESS (Bus, 0, 0, 0);
      if ((MmioRead32 (PciBase) == ((PRE_SI_GNX_CTRLR_DEVICE_ID << 16) | PRE_SI_GNX_CTRLR_VENDOR_ID)) ||
          (MmioRead32 (PciBase) == ((ASPEED2600_DEVICE_ID << 16) | ASPEED2600_VENDOR_ID)) ) {
        PciDevice = ((Bus) << 16) | ((0x00) << 8) | 0x00;
        break;
      }
    }

    if (PciDevice == 0) {
      DEBUG ((DEBUG_INFO, "No Aspeed VGA found!\n"));
      return;
    }

    Status = AspeedGraphicsInit (PciDevice, &Timings[0]);
    if (!EFI_ERROR (Status)) {
      ZeroMem (&GopMode, sizeof (EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));
      ZeroMem (&GopModeInfo, sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
      GopMode.Info = &GopModeInfo;

      Status = AspeedGetGraphicOutputProtocolModeInfo (&GopMode);
      if (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_VERBOSE, "  MaxMode:0x%x \n", GopMode.MaxMode));
        DEBUG ((DEBUG_VERBOSE, "  Mode:0x%x \n", GopMode.Mode));
        DEBUG ((DEBUG_VERBOSE, "  SizeOfInfo:0x%x \n", GopMode.SizeOfInfo));
        DEBUG ((DEBUG_VERBOSE, "  FrameBufferBase:0x%x \n", GopMode.FrameBufferBase));
        DEBUG ((DEBUG_VERBOSE, "  FrameBufferSize:0x%x \n", GopMode.FrameBufferSize));
        DEBUG ((DEBUG_VERBOSE, "  Version:0x%x \n", GopMode.Info->Version));
        DEBUG ((DEBUG_VERBOSE, "  HorizontalResolution:0x%x \n", GopMode.Info->HorizontalResolution));
        DEBUG ((DEBUG_VERBOSE, "  VerticalResolution:0x%x \n", GopMode.Info->VerticalResolution));
        DEBUG ((DEBUG_VERBOSE, "  PixelFormat:0x%x \n", GopMode.Info->PixelFormat));
        DEBUG ((DEBUG_VERBOSE, "  PixelsPerScanLine:0x%x \n", GopMode.Info->PixelsPerScanLine));

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
        }

        DEBUG ((DEBUG_INFO, "Aspeed VGA Init...Done\n"));
      }
    }
  }
}
