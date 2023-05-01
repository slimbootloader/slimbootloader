/** @file

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
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
  BL_IIO_UDS                *IioUdsHobData;
  VOID                      *FspHobList;
  TCC_CFG_DATA              *TccCfgData;

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

  if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() == 0)) {
    (VOID)PcdSet32S (PcdFuncCpuInitHook, (UINT32)(UINTN) PlatformCpuInit);
  }

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);

  if ((TccCfgData != NULL) && (TccCfgData->TccEnable != 0) && (IsWdtFlagsSet (WDT_FLAG_TCC_DSO_IN_PROGRESS))) {
    WdtDisable (WDT_FLAG_TCC_DSO_IN_PROGRESS);
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
  BL_IIO_UDS                *IioUdsHobData;
  BL_STACK_RES              *StackRes;
  UINT8                     Stack;
  UINTN                     Length;
  VOID                      *FspHobList;
  UINT32                    P2sbBase;

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

  for (Stack = 0; Stack < BL_MAX_LOGIC_IIO_STACK; Stack++) {
    StackRes = &IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack];
    if ((StackRes->PciResourceIoLimit != 0) ||
        (StackRes->PciResourceMem32Limit != 0) ||
        (StackRes->PciResourceMem64Limit != 0)) {
      TempRange               = &TempRes->ResourceRange[TempRes->NumOfEntries];
      TempRange->BusBase      = StackRes->BusBase;
      TempRange->BusLimit     = StackRes->BusLimit;
      TempRange->IoBase       = StackRes->PciResourceIoBase;
      TempRange->IoLimit      = StackRes->PciResourceIoLimit;
      TempRange->Mmio32Base   = StackRes->PciResourceMem32Base;
      TempRange->Mmio32Limit  = StackRes->PciResourceMem32Limit;
      TempRange->Mmio64Base   = StackRes->PciResourceMem64Base;
      TempRange->Mmio64Limit  = StackRes->PciResourceMem64Limit;

      DEBUG ((DEBUG_VERBOSE, "-- Resource Alloc Entry-%d Info --\n", TempRes->NumOfEntries));
      DEBUG ((DEBUG_VERBOSE, "  BusBase    : 0x%02X\n", TempRange->BusBase));
      DEBUG ((DEBUG_VERBOSE, "  BusLimit   : 0x%02X\n", TempRange->BusLimit));
      DEBUG ((DEBUG_VERBOSE, "  IoBase     : 0x%X\n", TempRange->IoBase));
      DEBUG ((DEBUG_VERBOSE, "  IoLimit    : 0x%X\n", TempRange->IoLimit));
      DEBUG ((DEBUG_VERBOSE, "  Mmio32Base : 0x%X\n", TempRange->Mmio32Base));
      DEBUG ((DEBUG_VERBOSE, "  Mmio32Limit: 0x%X\n", TempRange->Mmio32Limit));
      DEBUG ((DEBUG_VERBOSE, "  Mmio64Base : 0x%llX\n", TempRange->Mmio64Base));
      DEBUG ((DEBUG_VERBOSE, "  Mmio64Limit: 0x%llX\n", TempRange->Mmio64Limit));

      TempRes->NumOfEntries++;
    }
  }

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
  UINT32            PayloadSelGpioData;
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
        PayloadSelGpioPad = ((GPIO_CDF_H_CHIPSET_ID << 24) |
                            (PldSelCfgData->PldSelGpio.PadGroup << 16) |
                            (PldSelCfgData->PldSelGpio.PinNumber));

        Status = GpioGetInputValue (PayloadSelGpioPad, &PayloadSelGpioData);
        DEBUG ((DEBUG_INFO, "PayloadSelGpio Pad:0x%08X Val:%d Polarity:%d Status:%r\n",
          PayloadSelGpioPad, PayloadSelGpioData, PldSelCfgData->PldSelGpio.PinPolarity, Status));
        if (!EFI_ERROR (Status) && (PayloadSelGpioData != PldSelCfgData->PldSelGpio.PinPolarity)) {
          PayloadId = UEFI_PAYLOAD_ID_SIGNATURE;
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
  UINT32                    MeMode;
  UINT32                    FwStatus;
  GEN_GET_FW_VER_ACK        MsgGenGetFwVersionAckData;
  UINTN                     VariableLen;
  CHAR8                     Buffer[VAR_LENGTH_VAR_STS + 1];
  UINT32                    Address;
  UINT32                    VarSize;
  UINTN                     Data;

  Status = HeciGetMeMode (&MeMode);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "ME Mode: %X\n", MeMode));
  } else {
    DEBUG ((DEBUG_INFO, "Failed to Get ME Mode!\n"));
  }

  Status = HeciReadFwStatus (&FwStatus);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "HECI1 FW STATUS: %X\n", FwStatus));
  } else {
    DEBUG ((DEBUG_INFO, "Failed to Get HECI1 Firmware Status!\n"));
  }

  Status = HeciGetFwVersionMsg( (UINT8 *)&MsgGenGetFwVersionAckData);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "FwVersion %02X:%d.%d.%d.%d\n",
      (MsgGenGetFwVersionAckData.Data.CodeMajor >> 8),
      (MsgGenGetFwVersionAckData.Data.CodeMajor & 0x00FF),
      MsgGenGetFwVersionAckData.Data.CodeMinor,
      MsgGenGetFwVersionAckData.Data.CodeHotFix,
      MsgGenGetFwVersionAckData.Data.CodeBuildNo));
  } else {
    DEBUG ((DEBUG_INFO, "Failed to Get ME Firmware Version!\n"));
  }

  ZeroMem (Buffer, sizeof (Buffer));
  VariableLen = VAR_LENGTH_VAR_STS;
  Status = GetVariable (VAR_NAME_VAR_STS, NULL, NULL, &VariableLen, Buffer);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "GetVariable: %s %a(%d)\n", VAR_NAME_VAR_STS, Buffer, VariableLen));
  } else {
    DEBUG ((DEBUG_INFO, "No %s found!\n", VAR_NAME_VAR_STS));
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
  PLATFORM_DATA      *PlatformData;
  EFI_STATUS          Status;

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
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
      if (MmioRead32 (PciBase) == 0x20001A03) {
        PciDevice = ((Bus) << 16) | ((0x00) << 8) | 0x00;
        break;
      }
    }

    if (PciDevice == 0) {
      DEBUG ((DEBUG_INFO, "No Aspeed2500 VGA found!\n"));
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

        DEBUG ((DEBUG_INFO, "Aspeed2500 VGA Init...Done\n"));
      }
    }
  }
}
