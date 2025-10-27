/** @file

  Copyright (c) 2017-2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"
#include <Library/PrintLib.h>
#include <VerInfo.h>
#include <Library/SmbiosInitLib.h>

#define VTD_RMRR_USB_LENGTH                   0x20000
#define R_SA_MCHBAR_VTD1_OFFSET               0x6C88  ///< DMA Remapping HW UNIT1 for IGD
#define R_SA_MCHBAR_VTD2_OFFSET               0x6C80  ///< DMA Remapping HW UNIT2 for all other - PEG, USB, SATA etc
#define  VBT_OFFSET            36

extern EFI_ACPI_DMAR_HEADER mAcpiDmarTableTemplate;
STATIC
CONST EFI_ACPI_COMMON_HEADER *mPlatformAcpiTables[] = {
  (EFI_ACPI_COMMON_HEADER *)&mAcpiDmarTableTemplate,
  NULL
};

UINT32       mOtgDualRoleCfg0 = 0;

CONST UINT8  mPcieRpWakeGpeBit[5] = {9, 3, 6, 7, 8};
/**
  Create NHLT (Non HDA-Link Table)

  @param[in, out]  NhltTable    The NHLT table to be created

  @retval     EFI_UNSUPPORTED   NHLT configuration does not support
  @retval     EFI_SUCCESS       NHLT has been created successfully

**/
STATIC
EFI_STATUS
EFIAPI
CreateNhltAcpiTable (
  VOID
  )
{
  PLATFORM_SERVICE     *PlatformService;
  HDA_CFG_DATA         *HdaCfgData;
  NHLT_ACPI_TABLE      *Table;
  UINT32                TableLength;
  HDA_NHLT_ENDPOINTS    HdaNhltEndpoints[HdaEndpointMax] = {
    {HdaDmicX2,        FALSE},
    {HdaDmicX4,        FALSE},
    {HdaBtRender,      FALSE},
    {HdaBtCapture,     FALSE},
    {HdaI2sRenderSKP,  FALSE},
    {HdaI2sCaptureSKP, FALSE},
    {HdaI2sRenderHP,   FALSE},
    {HdaI2sCaptureHP,  FALSE},
    {HdaModem1Render,  FALSE},
    {HdaModem1Capture, FALSE},
    {HdaModem2Render,  FALSE},
    {HdaModem2Capture, FALSE}
  };

  HdaCfgData = (HDA_CFG_DATA *) FindConfigDataByTag (CDATA_HDA_TAG);
  ASSERT (HdaCfgData != NULL);

  if ((HdaCfgData == NULL) || (HdaCfgData->DspEnable == FALSE)) {
    return EFI_UNSUPPORTED;
  }

  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService == NULL || PlatformService->AcpiTableUpdate == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // DMIC X2
  //
  switch (HdaCfgData->DspEndpointDmic) {
  case 1:
    HdaNhltEndpoints[HdaDmicX2].Enable        = TRUE;
    break;
  case 2:
    HdaNhltEndpoints[HdaDmicX4].Enable        = TRUE;
    break;
  default:
    HdaNhltEndpoints[HdaDmicX2].Enable        = FALSE;
    HdaNhltEndpoints[HdaDmicX4].Enable        = FALSE;
  }

  if (HdaCfgData->DspEndpointBluetooth) {
    HdaNhltEndpoints[HdaBtRender].Enable      = TRUE;
    HdaNhltEndpoints[HdaBtCapture].Enable     = TRUE;
  }

  if (HdaCfgData->DspEndpointI2sSpk) {
    HdaNhltEndpoints[HdaI2sRenderSKP].Enable  = TRUE;
    HdaNhltEndpoints[HdaI2sCaptureSKP].Enable = TRUE;
  }

  if (HdaCfgData->DspEndpointI2sHp) {
    HdaNhltEndpoints[HdaI2sRenderHP].Enable   = TRUE;
    HdaNhltEndpoints[HdaI2sCaptureHP].Enable  = TRUE;
  }

  Table = (NHLT_ACPI_TABLE *)AllocateTemporaryMemory (0);

  NhltConstructor (HdaNhltEndpoints, Table, &TableLength);
  NhltAcpiHeaderConstructor (Table, TableLength);

  return PlatformService->AcpiTableUpdate ((UINT8 *)Table, Table->Header.Length);
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
  CHAR8                *SmbiosStrTbl;
  UINT16                PlatformId;
  UINTN                 Length;
  CHAR8                *TempSmbiosStrTbl;
  BOOT_LOADER_VERSION  *VerInfoTbl;

  PlatformId    = GetPlatformId ();
  TempSmbiosStrTbl  = (CHAR8 *) AllocateTemporaryMemory (0);
  SmbiosStrTbl = TempSmbiosStrTbl;
  //
  // SMBIOS_TYPE_BIOS_INFORMATION
  //
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BIOS_INFORMATION, 1, "Intel Corporation");

  VerInfoTbl = GetVerInfoPtr ();
  if (VerInfoTbl != NULL) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf),
      "SBL:%03d.%03d.%03d.%03d.%03d.%05d.%c-%016lX%a\0",
      VerInfoTbl->ImageVersion.SecureVerNum,
      VerInfoTbl->ImageVersion.CoreMajorVersion,
      VerInfoTbl->ImageVersion.CoreMinorVersion,
      VerInfoTbl->ImageVersion.ProjMajorVersion,
      VerInfoTbl->ImageVersion.ProjMinorVersion,
      VerInfoTbl->ImageVersion.BuildNumber,
      VerInfoTbl->ImageVersion.BldDebug ? 'D' : 'R',
      VerInfoTbl->SourceVersion,
      VerInfoTbl->ImageVersion.Dirty ? "-dirty" : "");
  } else {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Unknown");
  }
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BIOS_INFORMATION, 2, TempStrBuf);
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BIOS_INFORMATION, 3, __DATE__);

  //
  // SMBIOS_TYPE_SYSTEM_INFORMATION
  //
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    1, "Intel Corporation");
  if (PlatformId == PLATFORM_ID_OXH) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Oxbow Hill CRB Client Platform");
  } else if(PlatformId == PLATFORM_ID_LFH){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Leaf Hill CRB Client Platform");
  } else if(PlatformId == PLATFORM_ID_JNH){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Juniper Hill CRB Client Platform");
  } else if(PlatformId == PLATFORM_ID_UP2){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "UP Squared Client Platform");
  } else if(PlatformId == PLATFORM_ID_GPMRB){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Gordon Peak MRB Client Platform");
  } else if(PlatformId == PLATFORM_ID_MB3){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "MinnowBoad 3 Client Platform");
  } else {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Unknown");
  }
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    2, TempStrBuf);
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    3, "0.1");
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    4, "System Serial Number");
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    5, "System SKU Number");
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    6, "ApolloLake Client System");

  //
  // SMBIOS_TYPE_BASEBOARD_INFORMATION
  //
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BASEBOARD_INFORMATION,
    1, "Intel Corporation");
  if (PlatformId == PLATFORM_ID_OXH) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Oxbow Hill CRB Board");
  } else if(PlatformId == PLATFORM_ID_LFH){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Leaf Hill CRB Board");
  } else if(PlatformId == PLATFORM_ID_JNH){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Juniper Hill CRB Board");
  } else if(PlatformId == PLATFORM_ID_UP2){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "UP Squared Board");
  } else if(PlatformId == PLATFORM_ID_GPMRB){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Gordon Peak MRB Board");
  } else if(PlatformId == PLATFORM_ID_MB3){
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "MinnowBoad 3 Board");
  } else {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Unknown");
  }
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BASEBOARD_INFORMATION,
    2, TempStrBuf);
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BASEBOARD_INFORMATION,
    3, "1");
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BASEBOARD_INFORMATION,
    4, "Board Serial Number");

  //
  // SMBIOS_TYPE_END_OF_TABLE
  //
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_END_OF_TABLE,
    0, NULL);

  Length = SmbiosStrTbl - TempSmbiosStrTbl;
  SmbiosStrTbl = AllocatePool (Length);
  if (SmbiosStrTbl == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (SmbiosStrTbl, TempSmbiosStrTbl, Length);
  (VOID) PcdSet32S (PcdSmbiosStringsPtr, (UINT32)(UINTN)SmbiosStrTbl);

  return EFI_SUCCESS;
}

/**
  Print the output of the GPIO Config table that was read from CfgData.

  @param[in] GpioPinNum           Number of GPIO entries in the table.

  @param[in] GpioConfData         GPIO Config Data that was read from the
                                  Configuration region either from internal
                                  or external source.

**/
VOID
PrintGpioConfigTable (
  IN UINT32              GpioPinNum,
  IN VOID               *GpioConfData
  )
{
  GPIO_CONFIG_SMIP  *GpioSmipConf;
  UINT32             Count;
  UINT32             Data32;
  UINT32             PadConf0, PadConf1;
  UINT16             Offset0, Offset1;
  UINT16             Index;
  UINT8              Community;

  GpioSmipConf = (GPIO_CONFIG_SMIP *)GpioConfData;
  for (Count = 0; Count < GpioPinNum; Count++) {
    Offset0   = (UINT16)GpioSmipConf->Half0.r.Offset + BXT_GPIO_PAD_CONF0_OFFSET;
    Offset1   = (UINT16)GpioSmipConf->Half0.r.Offset + BXT_GPIO_PAD_CONF1_OFFSET;
    Community = (UINT8)GpioSmipConf->Half0.r.Community;
    Index     = (Offset0 - GPIO_PADBAR) / 8;
    PadConf0  = GpioRead(Community, Offset0);
    PadConf1  = GpioRead(Community, Offset1);
    Data32    = GpioRead(Community, HOSTSW_OWNERSHIP_0 + (Index / 32) * 4);
    Data32    = Data32 & ((UINT32)(1 << (Index % 32)));
    GpioSmipConf++;

    DEBUG ((DEBUG_VERBOSE, "  Comm:0x%02X Offset:0x%04X PadConf0:0x%08X PadConf1:0x%08X %a\n", \
      Community, Offset0, PadConf0, PadConf1, (Data32 == 0) ? "ACPI" : ""));
  }
}

/**
  Initialization of the GPIO table specific to each SOC.

**/
VOID
EFIAPI
GpioInit (
  VOID
  )
{
  GPIO_CFG_HDR       *GpioCfgCurrHdr;
  GPIO_CFG_HDR       *GpioCfgBaseHdr;
  UINT32              Index;
  UINT32              GpioEntries;
  GPIO_CONFIG_SMIP   *GpioConfigSmip;
  GPIO_CONFIG_SMIP   *SmipEntry;
  VOID               *GpioCfgDataBuffer;

  GpioCfgCurrHdr = (GPIO_CFG_HDR *)FindConfigDataByTag (CDATA_GPIO_TAG);
  if (GpioCfgCurrHdr == NULL) {
    DEBUG ((DEBUG_WARN, "Platform GPIO Table Not Found!\n"));
    return;
  }

  GpioCfgDataBuffer = (VOID *)AllocateTemporaryMemory (0);  //allocate new buffer
  GpioConfigSmip    = (GPIO_CONFIG_SMIP *)GpioCfgDataBuffer;
  SmipEntry         = NULL;
  GpioEntries       = 0;

  if (GpioCfgCurrHdr->GpioBaseTableId <= 0xF) {
    GpioCfgBaseHdr = (GPIO_CFG_HDR *)FindConfigDataByPidTag (GpioCfgCurrHdr->GpioBaseTableId, CDATA_GPIO_TAG);
    if (GpioCfgBaseHdr == NULL) {
      DEBUG ((DEBUG_ERROR, "Base GPIO Table Not Found for Platform ID 0x%02X\n", GpioCfgCurrHdr->GpioBaseTableId));
      return;
    }

    if (GpioCfgCurrHdr->GpioItemSize != GpioCfgBaseHdr->GpioItemSize) {
      DEBUG ((DEBUG_ERROR, "Inconsistent GPIO item size\n"));
      return;
    }

    SmipEntry = (GPIO_CONFIG_SMIP *)GpioCfgBaseHdr->GpioTableData;
    for (Index = 0; Index < GpioCfgBaseHdr->GpioItemCount; Index++) {
      if (GpioCfgCurrHdr->GpioBaseTableBitMask[Index >> 3] & (1 << (Index & 7))) {
        CopyMem (GpioConfigSmip, SmipEntry, sizeof (GPIO_CONFIG_SMIP));
        GpioConfigSmip->Half0.r.HostSw        = (GpioConfigSmip->Half1.r.Reserved0 & BIT0);
        GpioConfigSmip->Half0.r.DirectIrqWake = ((GpioConfigSmip->Half1.r.Reserved0 & BIT1) >> 1);
        GpioConfigSmip->Half1.r.Reserved0     = 0;
        GpioConfigSmip++;
        GpioEntries++;
      }
      SmipEntry++;
    }

    DEBUG ((DEBUG_INFO, "%d GPIO entries are populated from Base platform\n", GpioEntries));
  }

  SmipEntry = (GPIO_CONFIG_SMIP *)GpioCfgCurrHdr->GpioTableData;
  for (Index = 0; Index < GpioCfgCurrHdr->GpioItemCount; Index++) {
    CopyMem (GpioConfigSmip, SmipEntry, sizeof (GPIO_CONFIG_SMIP));
    GpioConfigSmip->Half0.r.HostSw        = (GpioConfigSmip->Half1.r.Reserved0 & BIT0);
    GpioConfigSmip->Half0.r.DirectIrqWake = ((GpioConfigSmip->Half1.r.Reserved0 & BIT1) >> 1);
    GpioConfigSmip->Half1.r.Reserved0     = 0;
    SmipEntry++;
    GpioConfigSmip++;
    GpioEntries++;
  }

  DEBUG ((DEBUG_INFO, "Programming %d GPIO entries\n", GpioEntries));
  GpioSmipPadConfigTable (GpioEntries, (GPIO_CONFIG_SMIP *)GpioCfgDataBuffer);

  DEBUG_CODE_BEGIN();
  PrintGpioConfigTable (GpioEntries, GpioCfgDataBuffer);
  DEBUG_CODE_END();
}

/**
  Create OS config data support HOB.

  @retval EFI_SUCCESS           OS config data HOB built
  @retval EFI_NOT_FOUND         Loader Global data not found
  @retval EFI_OUT_OF_RESOURCES  Could not build HOB
**/
EFI_STATUS
BuildOsConfigDataHob (
  VOID
  )
{
  EFI_STATUS             Status;
  OS_CONFIG_DATA_HOB    *OsConfigData;
  GEN_CFG_DATA          *GenCfgData;
  UINT32                 MemorySize;
  UINT32                 PldRsvdMemSize;

  MemorySize   = 0;
  OsConfigData = BuildGuidHob (&gOsConfigDataGuid, sizeof (OS_CONFIG_DATA_HOB));
  if (OsConfigData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  GenCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenCfgData == NULL) {
    return EFI_NOT_FOUND;
  }

  switch (GenCfgData->OsCrashMemorySize) {
  case 0:
    MemorySize = 0;
    break;
  case 1:
    MemorySize = 0x400000;
    break;
  case 2:
    MemorySize = 0x800000;
    break;
  case 3:
    MemorySize = 0x1000000;
    break;
  case 4:
    MemorySize = 0x2000000;
    break;
  case 5:
    MemorySize = 0x4000000;
    break;
  default:
    MemorySize = 0x400000;
    break;
  }

  Status = EFI_SUCCESS;
  OsConfigData->Revision          = 1;
  OsConfigData->OsCrashMemorySize = MemorySize;
  OsConfigData->EnableCrashMode   = GenCfgData->EnableCrashMode;
  if (MemorySize > 0) {
    PldRsvdMemSize = PcdGet32 (PcdPayloadReservedMemSize);
    Status = PcdSet32S (PcdPayloadReservedMemSize, PldRsvdMemSize + MemorySize);
  }
  return Status;
}

/**
  Resets the entire platform on GP MRB.

  @param[in] ResetType          The type of reset to perform.

**/
VOID
EFIAPI
ResetSystemIocIpc (
  IN EFI_RESET_TYPE   ResetType
  )
{
  IOC_UART_CFG_DATA         *IocUartData;
  IOC_IPC_CONFIG_DATA       IocIpcConfig;
  UINT32                    PciBar;
  UINT32                    Data32;

  if (ResetType == EfiResetWarm) {
    //
    // Flush the cache in case the changes are needed in next boot.
    //
    AsmWbinvd ();
  } else if (ResetType == EfiResetCold) {
    IocUartData = (IOC_UART_CFG_DATA *)FindConfigDataByTag (CDATA_IOC_UART_TAG);
    if (IocUartData == NULL) {
      DEBUG ((DEBUG_WARN, "CDATA_IOC_UART_TAG Not Found\n"));
      goto DoReset;
    }

    // Uart num max 4, Debug serial port 2
    ASSERT (IocUartData->DeviceIndex < 4);
    ASSERT (IocUartData->DeviceIndex != 2);

    PciBar = (UINT32)GetUartBaseAddress (IocUartData->DeviceIndex);
    ASSERT (PciBar != 0xFFFFFFFF);

    MmioWrite32 (PciBar + R_LPSS_IO_MEM_RESETS, 0);
    MmioWrite32 (PciBar + R_LPSS_IO_MEM_RESETS, B_LPSS_IO_MEM_iDMA_RESET_REL | B_LPSS_IO_MEM_HC_RESET_REL);

    Data32 = ((IOC_UART_PPR_CLK_N_DIV << 16) | (IOC_UART_PPR_CLK_M_DIV << 1) | B_LPSS_IO_MEM_PCP_CLK_EN);
    MmioWrite32 (PciBar + R_LPSS_IO_MEM_PCP, Data32);
    MmioWrite32 (PciBar + R_LPSS_IO_MEM_PCP, Data32 | B_LPSS_IO_MEM_PCP_CLK_UPDATE);
    MmioRead32  (PciBar + R_LPSS_IO_MEM_PCP);

    IocIpcConfig.IpcDeviceType                      = IocIpcDeviceTypeUart;
    IocIpcConfig.IpcDevice.IpcDeviceUart.BaseAddr   = PciBar;
    IocIpcConfig.IpcDevice.IpcDeviceUart.BaudRate   = 4000000;
    IocIpcConfig.IpcDevice.IpcDeviceUart.InputHertz = 64000000;
    IocIpcConfig.IpcDevice.IpcDeviceUart.RegWidth   = 4;

    RequestResetSystemToIoc (&IocIpcConfig, ResetType);
  } else if (ResetType == EfiResetShutdown) {
    DEBUG ((DEBUG_WARN, "Shutdown is not supported!\n"));
    return;
  }

DoReset:
  ResetSystem (ResetType);
}

/**
  Init platform specific service for Payload

**/
VOID
InitPlatformService (
  VOID
  )
{
  UINT16                PlatformId;
  PLATFORM_SERVICE     *PlatformService;

  PlatformId = GetPlatformId ();
  if (PlatformId == PLATFORM_ID_GPMRB) {
    PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
    if (PlatformService != NULL) {
      PlatformService->ResetSystem   = ResetSystemIocIpc;
    }
  }
}

/**
  Assign IRQs for the add-in cards

  This program the IRQs for add-in cards.
  It is required by ACRN.

**/
VOID
AssignPciIrqs (
  VOID
  )
{
  UINTN  PciBase;
  UINT8  Bus;
  UINT8  Function;
  UINT8  IntPin;
  UINT8  Shift;
  UINT8  IntLine[]   = {20, 21, 22, 23};

  // Workaround for ACRN + AaaG
  // Need to program interrupt register for add-in cards
  Shift = 0;
  for (Function = PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_5; Function <= PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_6; Function++) {
    PciBase = MM_PCI_ADDRESS (DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_2, Function, 0);
    if (MmioRead32(PciBase) == 0xFFFFFFFF) {
      continue;
    }
    Bus = MmioRead8(PciBase + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
    PciBase = MM_PCI_ADDRESS (Bus, 0, 0, 0);
    if (MmioRead32(PciBase) == 0xFFFFFFFF) {
      continue;
    } else {
      IntPin = MmioRead8(PciBase + PCI_INT_PIN_OFFSET);
      if ((IntPin < 1) || (IntPin > 4)) {
        continue;
      }
      MmioWrite8(PciBase + PCI_INT_LINE_OFFSET, IntLine[(Shift + IntPin - 1) & 3]);
    }
    Shift += 1;
  }
}

VOID
SaveOtgRole (
  VOID
  )
{
  UINTN                             XhciPciBase;
  UINT32                            XhciBar;
  UINT8                             BootMode;

  BootMode = GetBootMode ();
  if (BootMode != BOOT_ON_S3_RESUME) {
    return;
  }

  XhciPciBase = GetDeviceAddr (OsBootDeviceUsb, 0);
  XhciPciBase = TO_MM_PCI_ADDRESS (XhciPciBase);

  XhciBar     = MmioRead32 (XhciPciBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFF0000;
  if (XhciBar == 0) {
    XhciBar   = 0xFC800000;
    MmioAnd32   (XhciPciBase + PCI_COMMAND_OFFSET, (UINT32)~ (EFI_PCI_COMMAND_MEMORY_SPACE));
    MmioWrite32 (XhciPciBase + PCI_BASE_ADDRESSREG_OFFSET, XhciBar);
    MmioOr32    (XhciPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
  }
  mOtgDualRoleCfg0 = MmioRead32 (XhciBar + R_XHCI_MEM_DUAL_ROLE_CFG0);
  DEBUG ((DEBUG_INFO, "Save mOtgDualRoleCfg0: 0x%x \n", mOtgDualRoleCfg0));
}

VOID
SetGpioPadCfgLock (
  VOID
  )
{
  UINT32 Data32;

  Data32 = 0;

  //
  // JTAG
  //
  GpioLockPadCfg (N_TCK);
  GpioLockPadCfg (N_TRST_B);
  GpioLockPadCfg (N_TMS);
  GpioLockPadCfg (N_TDI);
  GpioLockPadCfg (N_TDO);

  //
  // Power
  //
  GpioLockPadCfg (NW_PMIC_THERMTRIP_B);
  GpioLockPadCfg (NW_PROCHOT_B);

  //
  // Touch
  //
  GpioLockPadCfg (NW_GPIO_118);
  GpioLockPadCfg (NW_GPIO_119);
  GpioLockPadCfg (NW_GPIO_120);
  GpioLockPadCfg (NW_GPIO_121);
  GpioLockPadCfg (NW_GPIO_122);
  GpioLockPadCfg (NW_GPIO_123);

  //
  // SPI
  //
  GpioLockPadCfg (NW_GPIO_97);
  GpioLockPadCfg (NW_GPIO_98);
  GpioLockPadCfg (NW_GPIO_99);
  GpioLockPadCfg (NW_GPIO_100);
  GpioLockPadCfg (NW_GPIO_101);
  GpioLockPadCfg (NW_GPIO_102);
  GpioLockPadCfg (NW_GPIO_103);
  GpioLockPadCfg (NW_FST_SPI_CLK_FB);

  //
  // SMBus
  // Set SMBus GPIO PAD_CFG.PADRSTCFG to Powergood
  //
  Data32 = GpioPadRead (SW_SMB_ALERTB);
  Data32 &= ~(BIT31 | BIT30);
  GpioPadWrite (SW_SMB_ALERTB, Data32);

  Data32 = GpioPadRead (SW_SMB_CLK);
  Data32 &= ~(BIT31 | BIT30);
  GpioPadWrite (SW_SMB_CLK, Data32);

  Data32 = GpioPadRead (SW_SMB_DATA);
  Data32 &= ~(BIT31 | BIT30);
  GpioPadWrite (SW_SMB_DATA, Data32);

  GpioLockPadCfg (SW_SMB_ALERTB);
  GpioLockPadCfg (SW_SMB_CLK);
  GpioLockPadCfg (SW_SMB_DATA);
}

VOID
RestoreOtgRole (
  VOID
  )
{
  UINTN                             XhciPciBase;
  UINT32                            XhciBar;
  UINT32                            DualRoleCfg0;
  UINT8                             BootMode;

  BootMode = GetBootMode ();
  if (BootMode != BOOT_ON_S3_RESUME) {
    return;
  }
  XhciPciBase = GetDeviceAddr (OsBootDeviceUsb, 0);
  XhciPciBase = TO_MM_PCI_ADDRESS (XhciPciBase);

  XhciBar      = MmioRead32 (XhciPciBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFF0000;
  DualRoleCfg0 = MmioRead32 (XhciBar + R_XHCI_MEM_DUAL_ROLE_CFG0);
  DEBUG ((DEBUG_INFO, "Before restore DualRoleCfg0: 0x%x \n", DualRoleCfg0));

  MmioWrite32 (XhciBar + R_XHCI_MEM_DUAL_ROLE_CFG0, mOtgDualRoleCfg0);
}

/**
  Set IA Untrust mode at the end.

**/
VOID
EFIAPI
EnterIaUnTrustMode (
  VOID
  )
{
  AsmMsrOr64 (EFI_MSR_POWER_MISC, B_EFI_MSR_POWER_MISC_ENABLE_IA_UNTRUSTED_MODE);
}

/**
  Set SPI flash EISS and LE and clear FSP HOBs.
**/
VOID
ProgramSecuritySetting (
  VOID
  )
{
  UINTN            SpiBaseAddress;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  // Set the BIOS Lock Enable and EISS bits
  MmioOr8 (SpiBaseAddress + R_SPI_BCR, (UINT8) (B_SPI_BCR_BLE | B_SPI_BCR_EISS));

  // Enter untrust mode
  EnterIaUnTrustMode ();
}

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
  if (CpuIndex != 0) {
    // Set IA_Untrusted mode for APs.
    // BSP will be done at the very end. If it is set too early, it will
    // block FSP functions inside notifications.
    EnterIaUnTrustMode ();
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
  GEN_CFG_DATA       *GenericCfgData;
  PLATFORM_CFG_DATA  *PlatCfgData;
  UINT32              PayloadId;
  UINT32              GpioLevel;

  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    return;
  }

  PayloadId = 0;
  GenericCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if ((GenericCfgData != NULL) && (GenericCfgData->PayloadId != AUTO_PAYLOAD_ID_SIGNATURE)) {
    // If PayloadId is not AUTO, use it directly
    PayloadId = GenericCfgData->PayloadId;
  } else {
    // If PayloadId is AUTO and GPIO PayloadId selection is enabled,
    // adjust the PayloadId accordingly per GPIO level (0:UEFI  1:OsLoader).
    GpioLevel = 1;
    PlatCfgData = (PLATFORM_CFG_DATA *)FindConfigDataByTag (CDATA_PLATFORM_TAG);
    if ((PlatCfgData != NULL) && (PlatCfgData->PayloadSelGpio.Enable != 0)) {
      // The default GPIOSet to Pull Up 20K
      GpioGetInputValueWithTerm (PlatCfgData->PayloadSelGpio.PadInfo, 0x0C, &GpioLevel);
      if (GpioLevel != 0) {
        PayloadId = 0;
      } else {
        PayloadId = UEFI_PAYLOAD_ID_SIGNATURE;
      }
      DEBUG ((DEBUG_INFO, "Set PayloadId to 0x%08X based on GPIO config\n", PayloadId));
    }
  }

  SetPayloadId (PayloadId);
}

/**
  Build VT-d information to prepare PMR program

**/
STATIC
VOID
BuildVtdInfo (
  VOID
  )
{
  VTD_INFO     *VtdInfo;
  UINTN         McD0BaseAddress;
  UINT32        MchBar;
  UINT32        Idx;
  UINT32        VtdIdx;
  UINT32        Data;
  UINT32        RegOff[2] = {R_SA_MCHBAR_VTD1_OFFSET, R_SA_MCHBAR_VTD2_OFFSET};

  VtdInfo = &((PLATFORM_DATA *)GetPlatformDataPtr ())->VtdInfo;
  McD0BaseAddress = MM_PCI_ADDRESS (SA_MC_BUS, 0, 0, 0);
  MchBar          = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_REG) & ~BIT0;
  VtdInfo->HostAddressWidth = 38;

  VtdIdx = 0;
  for (Idx = 0; Idx < ARRAY_SIZE(RegOff); Idx++) {
    Data = MmioRead32 (MchBar + RegOff[Idx]) & ~3;
    if (Data != 0) {
      DEBUG ((DEBUG_INFO, "VT-d Engine %d @ 0x%08X\n", VtdIdx, Data));
      VtdInfo->VTdEngineAddress[VtdIdx++] = Data;
      ASSERT (VtdIdx <= ARRAY_SIZE(VtdInfo->VTdEngineAddress));
    }
  }

  VtdInfo->VTdEngineCount = VtdIdx;
}

/**
  Apply SD card power if card is present.

**/
VOID
SdcardPowerUp (
  VOID
  )
{
  UINT16  PlatformId;
  UINT32  Data;

  PlatformId = GetPlatformId ();
  if ((PlatformId == PLATFORM_ID_LFH) || (PlatformId == PLATFORM_ID_OXH) || (PlatformId == PLATFORM_ID_JNH)) {
    // Check if SD card is present using GPIO_177
    Data = GpioRead ((SW_GPIO_177) >> 16, (SW_GPIO_177) & 0xFFFF) & BIT1;
    if (Data == 0) {
      // Card present, so turn on SD card power using GPIO_183
      Data = GpioRead ((SW_GPIO_183) >> 16, (SW_GPIO_183) & 0xFFFF);
      GpioWrite ((SW_GPIO_183) >> 16, (SW_GPIO_183) & 0xFFFF, Data & ~BIT0);
      MmioOr32 (P2SB_MMIO_BASE_ADDRESS + 0xD60608, BIT5);
    }
  }
}

//Initialize Platform Igd OpRegion
VOID
EFIAPI
IgdOpRegionPlatformInit (
  VOID
  )
{
  EFI_STATUS                Status;

  Status = IgdOpRegionInit (NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "VBT not found %r\n", Status));
  }
}

/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE    InitPhase
  )
{
  EFI_STATUS          Status;
  UINT32              VarBase;
  UINT32              VarSize;
  UINT32              TcoCnt;
  VOID               *FspHobList;
  UINT32              TsegBase;
  UINT64              TsegSize;
  VTD_INFO           *VtdInfo;

  switch (InitPhase) {
  case PreSiliconInit:
    UpdatePayloadId ();
    GpioInit ();
    SetGpioPadCfgLock();
    SpiConstructor ();
    Status = GetComponentInfo (FLASH_MAP_SIG_VARIABLE, &VarBase, &VarSize);
    if (!EFI_ERROR(Status)) {
      VariableConstructor (VarBase, VarSize);
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

    SdcardPowerUp();
    SaveOtgRole();
    Status = PcdSet32S (PcdFuncCpuInitHook, (UINT32)(UINTN) PlatformCpuInit);
    break;
  case PostSiliconInit:
    // To prevent from generating MCA for CLFLUSH flash region
    AsmMsrAnd32 (IA32_MC4_CTL, (UINT32)~BIT4);
    // Enable GFX PCI command register if framebuffer init is required.
    if (PcdGetBool (PcdFramebufferInitEnabled)) {
      PciWrite8 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_COMMAND_OFFSET), \
                 EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
    }
    //
    // Initialize Smbios Info for SmbiosInit
    //
    if (FeaturePcdGet (PcdSmbiosEnabled)) {
      InitializeSmbiosInfo ();
    }
    // Enable DMA protection
    if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
      BuildVtdInfo ();
      VtdInfo = &((PLATFORM_DATA *)GetPlatformDataPtr ())->VtdInfo;
      SetDmaProtection (VtdInfo, TRUE);
    }
    BuildOsConfigDataHob ();
    break;
  case PrePciEnumeration:
    if (FeaturePcdGet (PcdVtdEnabled)) {
      Status = PcdSet32S (PcdAcpiTableTemplatePtr, (UINT32)(UINTN)mPlatformAcpiTables);
    }
    break;
  case PostPciEnumeration:
    // Enable framebuffer as WC for performance
    Status = SetFrameBufferWriteCombining (0, MAX_UINT32);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "Failed to set GFX framebuffer as WC\n"));
    }
    if (PcdGetBool (PcdSeedListEnabled)) {
      Status = GenerateSeeds ();
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Error in seeds generation: %r\n", Status));
      }
    }
    AssignPciIrqs ();
    RestoreOtgRole ();
    break;
  case PrePayloadLoading:
    Status = CreateNhltAcpiTable ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "HDA config missing or NHLT unsupported - %r\n", Status));
    }
    break;
  case EndOfStages:
    HeciRegisterHeciService ();
    InitPlatformService ();

    if (GetPayloadId() != 0) {
      ProgramSecuritySetting ();
    }

    break;
  case ReadyToBoot:
    if (GetPayloadId() == 0) {
      ProgramSecuritySetting ();
    }

    // Lock down Tco WDT just before handling off to OS
    TcoCnt = IoRead32 (ACPI_BASE_ADDRESS + R_TCO1_CNT);
    IoWrite32 (ACPI_BASE_ADDRESS + R_TCO1_CNT, TcoCnt | B_TCO1_CNT_LOCK);
    break;
  case EndOfFirmware:
    ClearFspHob ();
    if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
      // Disable DMA protection
      VtdInfo = &((PLATFORM_DATA *)GetPlatformDataPtr ())->VtdInfo;
      SetDmaProtection (VtdInfo, FALSE);
    }
    EnterIaUnTrustMode ();
    // Clear known MCA logged in BANK4 and enable this MCA again
    AsmWriteMsr64 (IA32_MC4_STATUS, 0);
    AsmMsrOr32    (IA32_MC4_CTL,    (UINT32)BIT4);
    break;
  default:
    break;
  }
}

/**
  Update FSP-S UPD config data.

  @param  FspsUpdPtr    The pointer to the FSP-S UPD to be updated.

**/
VOID
EFIAPI
UpdateFspConfig (
  VOID     *FspsUpdPtr
  )
{
  FSPS_UPD               *FspsUpd;
  FSP_S_CONFIG           *FspsConfig;
  PLATFORM_DATA          *PlatformData;
  UINT32                  PcieMaxRootPorts;
  UINT32                  Index;
  UINT32                  PciBase;
  UINT32                  Stepping;
  UINT32                  PlatformId;
  UINT64                  Value64;
  UINT8                   ClkReqNum;
  USB_CFG_DATA           *UsbCfgData;
  PCIE_RP_CFG_DATA       *PcieRpConfigData;
  DEV_EN_CFG_DATA        *DevEnCfgData;
  HDA_CFG_DATA           *HdaCfgData;

  FspsUpd    = (FSPS_UPD *)FspsUpdPtr;
  FspsConfig = &FspsUpd->FspsConfig;

  DevEnCfgData = (DEV_EN_CFG_DATA *)FindConfigDataByTag (CDATA_DEV_EN_TAG);
  if (DevEnCfgData == NULL) {
    DEBUG ((DEBUG_VERBOSE, "Device enable/disable CFG data not found!\n"));
    return;
  }

  /* PmSupport and EnableRenderStandby should be disabled for S3 support.
    it could enable after it is fixed */
  FspsConfig->PkgCStateLimit = 254;
  if (GetPlatformId() == PLATFORM_ID_MB3) {
    FspsConfig->PmSupport = 0;
  } else {
    FspsConfig->PmSupport = 1;
  }
  FspsConfig->EnableRenderStandby = 1;
  FspsConfig->C1e = 1;

  FspsConfig->LPSS_S0ixEnable = 1;
  FspsConfig->PavpEnable = 1;
  FspsConfig->PavpPr3 = 1;
  FspsConfig->EnableCx = 1;
  FspsConfig->CStateAutoDemotion = 3;
  FspsConfig->CStateUnDemotion = 3;
  FspsConfig->PkgCStateDemotion = 1;
  FspsConfig->PkgCStateUnDemotion = 1;
  // if set to 0, FSP will not halt and lock TCO, SBL or payload should do it.
  FspsConfig->TcoTimerHaltLock = 1;

  FspsConfig->Eist = 1;
  FspsConfig->BiProcHot = 1;
  FspsConfig->TurboMode = 1;
  FspsConfig->MonitorMwaitEnable = 0;
  FspsConfig->CdClock = 4;

  // Force Eiss and BiosLock off for now.
  // Enable it later in OS loader/EndofStages
  FspsConfig->BiosLock = 0x0;
  FspsConfig->SpiEiss = 0x0;

  Value64 = AsmReadMsr64 (0xE2);
  Value64 |= 0x400;
  AsmWriteMsr64 (0xE2, Value64);

  FspsConfig->P2sbUnhide  = 0;
  FspsConfig->IpuEn       = (UINT8)DevEnCfgData->DevEnControl1.IpuEnable;
  FspsConfig->IpuAcpiMode = 1;

  if (FeaturePcdGet (PcdVtdEnabled)) {
    FspsConfig->VtdEnable        = 1;
    //
    // The following code is a workaround to handle IPU and VT-D dependencies.
    // IPU is disabled for silicon older than APL B0, when VT-D is enabled.
    //
    PciBase  = (UINT32)PcdGet64(PcdPciExpressBaseAddress);
    Stepping = MmioRead8 (PciBase + 8);
    Value64  = AsmReadMsr64 (MSR_IA32_PLATFORM_ID);
    PlatformId = RShiftU64 (Value64, 50) & 7;
    if (Stepping <= 0xB && PlatformId == 0) {
      FspsConfig->IpuEn   = 0;
    }

    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if (GetPayloadId () == 0) {
      PlatformData->RmrrUsbAddress = (UINT32) (UINTN)AllocatePages (EFI_SIZE_TO_PAGES (VTD_RMRR_USB_LENGTH));
    } else {
      PlatformData->RmrrUsbAddress = 0;
    }
  } else {
    FspsConfig->VtdEnable = 0;
  }

  FspsConfig->IPC[0]                = 0xFFFFEEF8;
  FspsConfig->IPC[1]                = 0xFFFFFFFF;
  FspsConfig->IPC[2]                = 0xFFFFFFFF;
  FspsConfig->IPC[3]                = 0xFFFFFFFF;
  FspsConfig->Hpet                  = 1;
  FspsConfig->HpetBdfValid          = 1;
  FspsConfig->HpetBusNumber         = 0xFA;
  FspsConfig->HpetDeviceNumber      = 0x0F;
  FspsConfig->HpetFunctionNumber    = 0;
  FspsConfig->IoApicId              = 1;
  FspsConfig->IoApicBdfValid        = 1;
  FspsConfig->IoApicBusNumber       = 0xFA;
  FspsConfig->IoApicDeviceNumber    = 0x1F;
  FspsConfig->IoApicFunctionNumber  = 0;

  PcieMaxRootPorts = ARRAY_SIZE (FspsConfig->PcieRpHotPlug);
  for (Index = 0; Index < PcieMaxRootPorts; Index++) {
    FspsConfig->PcieRpHotPlug[Index]                = 0;
    FspsConfig->PcieRpPmSci[Index]                  = 1;
    FspsConfig->PcieRpTransmitterHalfSwing[Index]   = 0;
    FspsConfig->PcieRpLtrMaxSnoopLatency[Index]     = 0x1003;
    FspsConfig->PcieRpLtrMaxNonSnoopLatency[Index]  = 0x1003;
    FspsConfig->PcieRpAspm[Index]                   = 0;
  }

  PcieRpConfigData  = (PCIE_RP_CFG_DATA *)FindConfigDataByTag (CDATA_PCIE_RP_TAG);
  if (PcieRpConfigData != NULL) {
    for (Index = 0; Index < PcieMaxRootPorts; Index++) {
      ClkReqNum = PcieRpConfigData->PcieRpFeatures0[Index].ClkReqNum;
      FspsConfig->PcieRootPortEn[Index]         = PcieRpConfigData->PcieRpFeatures0[Index].En;
      FspsConfig->PcieRpAspm[Index]             = PcieRpConfigData->PcieRpFeatures0[Index].Aspm;
      FspsConfig->PcieRpClkReqSupported[Index]  = PcieRpConfigData->PcieRpFeatures0[Index].ClkReqSup;
      FspsConfig->PcieRpClkReqNumber[Index]     = ClkReqNum < 6 ? ClkReqNum : 0x0F;
    }
  }

  FspsConfig->SdcardEnabled               = (UINT8)DevEnCfgData->DevEnControl1.SdcardEnable;

  if (GetPlatformId () == PLATFORM_ID_GPMRB) {
    FspsConfig->ResetSelect                 = 0xE;
    FspsConfig->PcieRootPort8xhDecode       = 0;
  }

  FspsConfig->Usb30Mode                   = 1;
  FspsConfig->UsbOtg                      = (UINT8)DevEnCfgData->DevEnControl2.XdciEnable;

  if (DCI_DBG_FEATURE_ENABLED() ) {
    FspsConfig->DciEn = 1;
  } else {
    FspsConfig->DciEn = 0;
  }

  DEBUG ((DEBUG_VERBOSE, "Dci is %d\n", FspsConfig->DciEn));

  UsbCfgData = (USB_CFG_DATA *) FindConfigDataByTag (CDATA_USB_TAG);
  if (UsbCfgData != NULL) {
    //CopyMem (FspsConfig->PortUs20bOverCurrentPin, UsbCfgData->PortUs20bOverCurrentPin, sizeof(UsbCfgData->PortUs20bOverCurrentPin));
    //CopyMem (FspsConfig->PortUs30bOverCurrentPin, UsbCfgData->PortUs30bOverCurrentPin, sizeof(UsbCfgData->PortUs30bOverCurrentPin));
    FspsConfig->PortUs20bOverCurrentPin[0] = (UINT8)UsbCfgData->OverCurrentPin.PortUs20bOverCurrentPin0;
    FspsConfig->PortUs20bOverCurrentPin[1] = (UINT8)UsbCfgData->OverCurrentPin.PortUs20bOverCurrentPin1;
    FspsConfig->PortUs20bOverCurrentPin[2] = (UINT8)UsbCfgData->OverCurrentPin.PortUs20bOverCurrentPin2;
    FspsConfig->PortUs20bOverCurrentPin[3] = (UINT8)UsbCfgData->OverCurrentPin.PortUs20bOverCurrentPin3;
    FspsConfig->PortUs20bOverCurrentPin[4] = (UINT8)UsbCfgData->OverCurrentPin.PortUs20bOverCurrentPin4;
    FspsConfig->PortUs20bOverCurrentPin[5] = (UINT8)UsbCfgData->OverCurrentPin.PortUs20bOverCurrentPin5;
    FspsConfig->PortUs20bOverCurrentPin[6] = (UINT8)UsbCfgData->OverCurrentPin.PortUs20bOverCurrentPin6;
    FspsConfig->PortUs20bOverCurrentPin[7] = (UINT8)UsbCfgData->OverCurrentPin.PortUs20bOverCurrentPin7;

    FspsConfig->PortUs30bOverCurrentPin[0] = (UINT8)UsbCfgData->OverCurrentPin.PortUs30bOverCurrentPin0;
    FspsConfig->PortUs30bOverCurrentPin[1] = (UINT8)UsbCfgData->OverCurrentPin.PortUs30bOverCurrentPin1;
    FspsConfig->PortUs30bOverCurrentPin[2] = (UINT8)UsbCfgData->OverCurrentPin.PortUs30bOverCurrentPin2;
    FspsConfig->PortUs30bOverCurrentPin[3] = (UINT8)UsbCfgData->OverCurrentPin.PortUs30bOverCurrentPin3;
    FspsConfig->PortUs30bOverCurrentPin[4] = (UINT8)UsbCfgData->OverCurrentPin.PortUs30bOverCurrentPin4;
    FspsConfig->PortUs30bOverCurrentPin[5] = (UINT8)UsbCfgData->OverCurrentPin.PortUs30bOverCurrentPin5;
  }

  FspsConfig->UfsEnabled                  = (UINT8)DevEnCfgData->DevEnControl1.UfsEnable;

  HdaCfgData = (HDA_CFG_DATA *) FindConfigDataByTag (CDATA_HDA_TAG);
  if (HdaCfgData != NULL) {
    FspsConfig->HdaEnable                 = HdaCfgData->HdaEnable;
    FspsConfig->DspEnable                 = HdaCfgData->DspEnable;
    FspsConfig->HdAudioDspUaaCompliance   = HdaCfgData->HdAudioDspUaaCompliance;
    FspsConfig->Pme                       = HdaCfgData->Pme;
    FspsConfig->HdAudioIoBufferOwnership  = HdaCfgData->HdAudioIoBufferOwnership;
    FspsConfig->HdAudioVcType             = HdaCfgData->HdAudioVcType;
    FspsConfig->HdAudioLinkFrequency      = HdaCfgData->HdAudioLinkFrequency;
    FspsConfig->HdAudioIDispLinkFrequency = HdaCfgData->HdAudioIDispLinkFrequency;
    FspsConfig->HdAudioIDispLinkTmode     = HdaCfgData->HdAudioIDispLinkTmode;
    FspsConfig->DspEndpointDmic           = HdaCfgData->DspEndpointDmic;
    FspsConfig->DspEndpointBluetooth      = HdaCfgData->DspEndpointBluetooth;
    FspsConfig->DspEndpointI2sSkp         = HdaCfgData->DspEndpointI2sSpk;
    FspsConfig->DspEndpointI2sHp          = HdaCfgData->DspEndpointI2sHp;
    FspsConfig->DspFeatureMask            = HdaCfgData->DspFeatureMask;
    FspsConfig->DspPpModuleMask           = HdaCfgData->DspPpModuleMask;
    FspsConfig->HdaVerbTablePtr           = (UINT32)(UINTN)(&HdaVerbTableAlc662);
    FspsConfig->HdaVerbTableEntryNum      = 1;

    HdaEndpointBtRender.VirtualBusId      = HdaCfgData->VirtualIdBtRender;
    HdaEndpointBtCapture.VirtualBusId     = HdaCfgData->VirtualIdBtCapture;
    HdaEndpointI2sRenderSKP.VirtualBusId  = HdaCfgData->VirtualIdI2sRenderSpk;
    HdaEndpointI2sRenderHP.VirtualBusId   = HdaCfgData->VirtualIdI2sRenderHp;
    HdaEndpointI2sCapture.VirtualBusId    = HdaCfgData->VirtualIdI2sCapture;
  }

  if (PcdGetBool (PcdFramebufferInitEnabled)) {
    FspsConfig->GraphicsConfigPtr   = (UINT32)GetVbtAddress ();
  } else {
    FspsConfig->GraphicsConfigPtr = 0;
  }

  FspsConfig->InitS3Cpu                   = 1;

  //
  // Lock down configurations as needed
  //
  FspsConfig->BiosCfgLockDown = 0x1;
  FspsConfig->BiosInterface = 0x1;
  FspsConfig->LockDownGlobalSmi = 0x1;
  FspsConfig->RtcLock = 0x1;

  //
  // Flash protection ranges setup
  //
  FspsConfig->WriteProtectionEnable[0] = 0x1;
  FspsConfig->ReadProtectionEnable[0] = 0x1;
  FspsConfig->ProtectedRangeLimit[0] = 0x0FFF;
  FspsConfig->ProtectedRangeBase[0] = 0x0;

  FspsConfig->I2c0Enable                  = (UINT8)DevEnCfgData->DevEnControl0.I2C0Enable;
  FspsConfig->I2c1Enable                  = (UINT8)DevEnCfgData->DevEnControl0.I2C1Enable;
  FspsConfig->I2c2Enable                  = (UINT8)DevEnCfgData->DevEnControl0.I2C2Enable;
  FspsConfig->I2c3Enable                  = (UINT8)DevEnCfgData->DevEnControl0.I2C3Enable;
  FspsConfig->I2c4Enable                  = (UINT8)DevEnCfgData->DevEnControl0.I2C4Enable;
  FspsConfig->I2c5Enable                  = (UINT8)DevEnCfgData->DevEnControl0.I2C5Enable;
  FspsConfig->I2c6Enable                  = (UINT8)DevEnCfgData->DevEnControl0.I2C6Enable;
  FspsConfig->I2c7Enable                  = (UINT8)DevEnCfgData->DevEnControl0.I2C7Enable;

  FspsConfig->Hsuart0Enable               = (UINT8)DevEnCfgData->DevEnControl0.Hsuart0Enable;
  FspsConfig->Hsuart1Enable               = (UINT8)DevEnCfgData->DevEnControl0.Hsuart1Enable;
  FspsConfig->Hsuart2Enable               = (UINT8)DevEnCfgData->DevEnControl0.Hsuart2Enable;
  FspsConfig->Hsuart3Enable               = (UINT8)DevEnCfgData->DevEnControl0.Hsuart3Enable;

  FspsConfig->Spi0Enable                  = (UINT8)DevEnCfgData->DevEnControl0.Spi0Enable;
  FspsConfig->Spi1Enable                  = (UINT8)DevEnCfgData->DevEnControl0.Spi1Enable;
  FspsConfig->Spi2Enable                  = (UINT8)DevEnCfgData->DevEnControl0.Spi2Enable;

  FspsConfig->IshEnable                   = (UINT8)DevEnCfgData->DevEnControl1.IshEnable;
  FspsConfig->EnableSata                  = (UINT8)DevEnCfgData->DevEnControl1.SataEnable;
  FspsConfig->SataPortsEnable[0]          = (UINT8)DevEnCfgData->DevEnControl1.SataPort0Enable;
  FspsConfig->SataPortsEnable[1]          = (UINT8)DevEnCfgData->DevEnControl1.SataPort1Enable;

  FspsConfig->eMMCEnabled                 = (UINT8)DevEnCfgData->DevEnControl1.eMMCEnable;
  FspsConfig->SdioEnabled                 = (UINT8)DevEnCfgData->DevEnControl1.SdioEnable;
  FspsConfig->SmbusEnable                 = (UINT8)DevEnCfgData->DevEnControl1.SmbusEnable;

  // Set HS200 if HS400 is disabled. No DDR50 in scope.
  FspsConfig->eMMCHostMaxSpeed            = (UINT8)((FeaturePcdGet (PcdEmmcHs400SupportEnabled) != 0) ? 0 : 1);

  FspsConfig->PortUsb20Enable[0]          = (UINT8)DevEnCfgData->DevEnControl2.Usb20Port0Enable;
  FspsConfig->PortUsb20Enable[1]          = (UINT8)DevEnCfgData->DevEnControl2.Usb20Port1Enable;
  FspsConfig->PortUsb20Enable[2]          = (UINT8)DevEnCfgData->DevEnControl2.Usb20Port2Enable;
  FspsConfig->PortUsb20Enable[3]          = (UINT8)DevEnCfgData->DevEnControl2.Usb20Port3Enable;
  FspsConfig->PortUsb20Enable[4]          = (UINT8)DevEnCfgData->DevEnControl2.Usb20Port4Enable;
  FspsConfig->PortUsb20Enable[5]          = (UINT8)DevEnCfgData->DevEnControl2.Usb20Port5Enable;
  FspsConfig->PortUsb20Enable[6]          = (UINT8)DevEnCfgData->DevEnControl2.Usb20Port6Enable;
  FspsConfig->PortUsb20Enable[7]          = (UINT8)DevEnCfgData->DevEnControl2.Usb20Port7Enable;

  FspsConfig->PortUsb30Enable[0]          = (UINT8)DevEnCfgData->DevEnControl2.Usb30Port0Enable;
  FspsConfig->PortUsb30Enable[1]          = (UINT8)DevEnCfgData->DevEnControl2.Usb30Port1Enable;
  FspsConfig->PortUsb30Enable[2]          = (UINT8)DevEnCfgData->DevEnControl2.Usb30Port2Enable;
  FspsConfig->PortUsb30Enable[3]          = (UINT8)DevEnCfgData->DevEnControl2.Usb30Port3Enable;
  FspsConfig->PortUsb30Enable[4]          = (UINT8)DevEnCfgData->DevEnControl2.Usb30Port4Enable;
  FspsConfig->PortUsb30Enable[5]          = (UINT8)DevEnCfgData->DevEnControl2.Usb30Port5Enable;

}

/**
  Save MRC data onto the Flash region from NVS buffer.

  @param[in] Buffer pointer to an NVS data
  @param[in] Length Length of the NVS data(MRC)

  @retval EFI_SUCCESS
  @retval RETURN_ALREADY_STARTED
  @retval EFI_DEVICE_ERROR
  @retval EFI_INVALID_PARAMETER
 **/
EFI_STATUS
EFIAPI
SaveNvsData (
  IN  VOID    *Buffer,
  IN  UINT32   Length
  )
{
  VOID                           *MemPool;
  MRC_PARAM_HDR                  *MrcParamHdr;
  MRC_VAR_HDR                    *MrcVarHdr;
  UINT8                          *TmpPtr;
  VOID                           *VariableMrcData;
  PLATFORM_DATA                  *PlatformData;
  UINT32                          RegionSize;
  UINT32                          DataSize;
  UINT32                          VarLength;
  UINT32                          MrcDataBase;
  UINT32                          MrcDataSize;
  UINT32                          BiosOffset;
  UINT32                          MrcNvDataOffset;
  UINT32                          MrcParamsOffset;
  EFI_STATUS                      Status;
  UINT32                          ActIdx;
  UINT32                          Idx;
  UINT8                           Data8;
  UINT8                           BitMap;
  FLASH_MAP                      *FlashMapPtr;
  VOID                           *FspHobList;

  VariableMrcData = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    VariableMrcData = GetGuidHobData (FspHobList,
                                      &VarLength,
                                      &gFspVariableNvDataHobGuid);
  }
  if (VariableMrcData == NULL || (Buffer == NULL) || (Length < sizeof (MrcParamHdr->Crc))) {
    return EFI_NOT_FOUND;
  }

  Status = GetComponentInfo (FLASH_MAP_SIG_MRCDATA, &MrcDataBase, &MrcDataSize);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  FlashMapPtr = GetFlashMapPtr ();
  if (FlashMapPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  BiosOffset = FlashMapPtr->RomSize + MrcDataBase;

  DEBUG ((DEBUG_INFO, "\nSave MRC NVS Data\n"));

  PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();
  MemPool       = AllocateTemporaryMemory (0);
  TmpPtr        = (UINT8 *)Buffer + Length - sizeof (MrcParamHdr->Crc);

  // Read current MRC Param DATA
  RegionSize      = SIZE_4KB;
  MrcNvDataOffset = 0;
  MrcParamsOffset = MrcNvDataOffset + RegionSize;

  Status = EFI_SUCCESS;
  do {
    // Compare data to see if it is the same
    if (CompareMem (TmpPtr, PlatformData->MrcParamHdr.Crc, sizeof (PlatformData->MrcParamHdr.Crc)) == 0) {
      DEBUG ((DEBUG_INFO, "MRC VarData matches data in flash, skip update!\n"));
      break;
    }

    DataSize  = (UINT32)RleCompressData (Buffer, Length, (UINT8 *)MemPool + sizeof (MRC_PARAM_HDR));
    DataSize += sizeof (MRC_PARAM_HDR);

    DEBUG ((DEBUG_INFO, "Writing MRC ParamData to SPI BIOS @ 0x%X:0x%X\n", BiosOffset + MrcParamsOffset, DataSize));
    Status = SpiFlashErase (FlashRegionBios, BiosOffset + MrcParamsOffset, ALIGN_UP (DataSize, SIZE_4KB));
    if (EFI_ERROR (Status)) {
      break;
    }

    // Mark data without valid signature first in case of power failure
    DEBUG ((DEBUG_INFO, "Writing data\n"));
    MrcParamHdr             = (MRC_PARAM_HDR *)MemPool;
    MrcParamHdr->Signature  = MRC_PARAM_SIGNATURE;
    MrcParamHdr->Length     = DataSize;
    CopyMem (MrcParamHdr->Crc, TmpPtr, sizeof (MrcParamHdr->Crc));
    Status = SpiFlashWrite (FlashRegionBios, BiosOffset + MrcParamsOffset + sizeof (UINT32),
                            DataSize - sizeof (UINT32), (UINT8 *)MemPool + sizeof (UINT32));
    if (EFI_ERROR (Status)) {
      break;
    }

    // Mark record as valid by writing signature
    DEBUG ((DEBUG_INFO, "Writing signature\n"));
    MrcParamHdr->Signature = MRC_PARAM_SIGNATURE;
    Status = SpiFlashWrite (FlashRegionBios, BiosOffset + MrcParamsOffset, sizeof (UINT32), MemPool);
    if (EFI_ERROR (Status)) {
      break;
    }
  } while (EFI_ERROR (Status));
  DEBUG ((DEBUG_INFO, "Writing MRC ParamData: %r\n", Status));

  do {
    if (EFI_ERROR (Status)) {
      break;
    }

    // Compare data to see if it is the same
    if (CompareMem (VariableMrcData, PlatformData->MrcVarData, sizeof (PlatformData->MrcVarData)) == 0) {
      DEBUG ((DEBUG_INFO, "MRC VarData matches data in flash, skip update!\n"));
      break;
    }

    DEBUG ((DEBUG_INFO, "Read MRC VarData at 0x%X\n", MrcDataBase + MrcNvDataOffset));
    MrcVarHdr = (MRC_VAR_HDR *)MemPool;
    CopyMem ((UINT8 *)MrcVarHdr, (UINT8 *)(UINTN)MrcDataBase + MrcNvDataOffset, sizeof (MRC_VAR_HDR));

    ActIdx = 0xFF;
    if (MrcVarHdr->Signature == MRC_VAR_SIGNATURE) {
      for (Idx = 0; Idx < MrcVarHdr->SlotNum >> 3; Idx++) {
        Data8 = MrcVarHdr->SlotMap[Idx];
        if (Data8 != 0) {
          ActIdx = Idx * 8;
          while ((Data8 & BIT0) == 0) {
            ActIdx++;
            Data8 >>= 1;
          }
          break;
        }
      }
    }

    if (ActIdx == 0xFF) {
      // Need to erase the block
      DEBUG ((DEBUG_INFO, "Erasing MRC VarData block\n"));
      Status = SpiFlashErase (FlashRegionBios, BiosOffset + MrcNvDataOffset, RegionSize);
      if (EFI_ERROR (Status)) {
        break;
      }
      SetMem (MrcVarHdr, sizeof (MRC_VAR_HDR), 0xFF);
      MrcVarHdr->Signature  = MRC_VAR_SIGNATURE;
      MrcVarHdr->SlotNum    = ALIGN_DOWN (((SIZE_4KB - sizeof (MRC_VAR_HDR)) / MRC_VAR_SLOT_LENGTH), 8);
      MrcVarHdr->SlotLen    = MRC_VAR_SLOT_LENGTH;
      MrcVarHdr->SlotMap[0] = 0xFE;
      Status = SpiFlashWrite (FlashRegionBios, BiosOffset + MrcNvDataOffset + sizeof (UINT32),
                              sizeof (MRC_VAR_HDR) - sizeof (UINT32), (UINT8 *)MrcVarHdr + sizeof (UINT32));
      if (EFI_ERROR (Status)) {
        break;
      }
      Status = SpiFlashWrite (FlashRegionBios, BiosOffset + MrcNvDataOffset, sizeof (UINT32), (VOID *)MrcVarHdr);
      if (EFI_ERROR (Status)) {
        break;
      }
      Idx    = 0;
      ActIdx = 0;
    }

    // Write NV data into the slot
    DEBUG ((DEBUG_INFO, "Updating MRC VarData slot %d\n", ActIdx));
    Status = SpiFlashWrite (FlashRegionBios, BiosOffset + MrcNvDataOffset + sizeof (MRC_VAR_HDR) + ActIdx * MRC_VAR_SLOT_LENGTH,
                            VarLength, VariableMrcData);
    if (EFI_ERROR (Status)) {
      break;
    }

    // Update the slot map
    BitMap =  ~ ((1 << ((ActIdx & 7) + 1)) - 1);
    Status = SpiFlashWrite (FlashRegionBios, BiosOffset + MrcNvDataOffset + OFFSET_OF (MRC_VAR_HDR, SlotMap) + Idx, 1, &BitMap);

  } while (EFI_ERROR (Status));
  DEBUG ((DEBUG_INFO, "Write MRC VarData: %r\n", Status));

  return Status;
}

/**
 Update the OS boot option

 @param[out] OsBootOptionList   pointer to boot option list.
 **/
VOID
EFIAPI
UpdateOsBootMediumInfo (
  OUT  OS_BOOT_OPTION_LIST  *OsBootOptionList
  )
{
  PLATFORM_DATA          *PlatformData;
  BOOT_MODE_OPTION       *BootModeOption;
  BOOT_MODE_TARGET        Target;
  BOOT_MODE_ACTION        Action;

  Target        = BootModeTargetNormal;
  Action        = BootModeActionBoot;
  PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    BootModeOption  = &PlatformData->BootModeOption;
    if (BootModeOption != NULL) {
      DEBUG ((DEBUG_INFO, "Boot Target: %d, Action: %d\n", BootModeOption->Bits.Target, BootModeOption->Bits.Action));
      Target  = (BOOT_MODE_TARGET)BootModeOption->Bits.Target;
      Action  = (BOOT_MODE_ACTION)BootModeOption->Bits.Action;
    }
  }

  FillBootOptionListFromCfgData (OsBootOptionList);

  //
  // Update current boot and count based on target and action
  //
  switch (Target) {
  case BootModeTargetNormal:
    OsBootOptionList->CurrentBoot = 0;
    break;
  case BootModeTargetProvision:
    OsBootOptionList->CurrentBoot = OsBootOptionList->OsBootOptionCount > 0 ? OsBootOptionList->OsBootOptionCount - 1 : 0;
    break;
  default:
    DEBUG ((DEBUG_ERROR, "Not Supported Boot Target!\n"));
    break;
  }

  if (Action == BootModeActionShell) {
    DEBUG ((DEBUG_INFO, "Set boot to shell!\n"));
    OsBootOptionList->BootToShell = 1;
  }
}

/**
 Update the frame buffer info by reading the PCI address.

 @param[out] GfxInfo pointer to global HOB data structure.
 **/
VOID
UpdateFrameBufferInfo (
  OUT  EFI_PEI_GRAPHICS_INFO_HOB   *GfxInfo
  )
{
  GfxInfo->FrameBufferBase = PciRead32 (PCI_LIB_ADDRESS (0, 2, 0, 0x18)) & 0xFFFFFF00;
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
  GfxDeviceInfo->BarIndex = 1;
  GfxDeviceInfo->VendorId = PciRead16 (PCI_LIB_ADDRESS (0, 2, 0, 0));
  GfxDeviceInfo->DeviceId = PciRead16 (PCI_LIB_ADDRESS (0, 2, 0, 2));
}

/**
 Update loader platform info.

 @param[out] LoaderPlatformInfo pointer to platform info HOB

**/
VOID
UpdateLoaderPlatformInfo (
  OUT LOADER_PLATFORM_INFO        *LoaderPlatformInfo
)
{
  EFI_STATUS        Status;
  EMMC_TUNING_DATA  EmmcTuningData;
  UINTN             VariableLen;
  PLATFORM_DATA     *PlatformData;
  UINT32            HeciFwSts;

  EmmcTuningData.Hs400DataValid = 0;
  VariableLen = sizeof (EmmcTuningData);
  Status = GetVariable (L"MMCDLL", NULL, NULL, &VariableLen, &EmmcTuningData);
  if (EFI_ERROR (Status)) {
    AsciiStrCpyS (EmmcTuningData.SerialNumber, sizeof(EmmcTuningData.SerialNumber), "badbadbadbadba");
  }

  AsciiStrCpyS (LoaderPlatformInfo->SerialNumber, sizeof(LoaderPlatformInfo->SerialNumber), EmmcTuningData.SerialNumber);

  PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if(PlatformData != NULL) {
    LoaderPlatformInfo->HwState = (PlatformData->BtGuardInfo.Bpm.Vb) |
                                        ((PlatformData->BtGuardInfo.Bpm.Mb) << 1);

    // Get Manufacturing Mode from Heci
    HeciReadFwStatus (&HeciFwSts);
    LoaderPlatformInfo->HwState |= (HeciFwSts & BIT4) >> 2;
  }
}

/**
 Update loader SMM info.

 @param[out] LdrSmmInfo     pointer to SMM information HOB

**/
VOID
UpdateSmmInfo (
  OUT  LDR_SMM_INFO           *LdrSmmInfo
)
{
  if (LdrSmmInfo == NULL) {
    return;
  }
  LdrSmmInfo->SmmBase = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + TSEG) & ~0xF;
  LdrSmmInfo->SmmSize = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + BGSM) & ~0xF;
  LdrSmmInfo->SmmSize -= LdrSmmInfo->SmmBase;
  LdrSmmInfo->Flags = SMM_FLAGS_4KB_COMMUNICATION;
  DEBUG ((DEBUG_INFO, "SmmRamBase = 0x%x, SmmRamSize = 0x%x\n", LdrSmmInfo->SmmBase, LdrSmmInfo->SmmSize));

  //
  // Update smi ctrl register data
  //
  LdrSmmInfo->SmiCtrlReg.RegType    = (UINT8)REG_TYPE_IO;
  LdrSmmInfo->SmiCtrlReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiCtrlReg.SmiGblPos  = (UINT8)HighBitSet32 (B_SMI_EN_GBL_SMI);
  LdrSmmInfo->SmiCtrlReg.SmiApmPos  = (UINT8)HighBitSet32 (B_SMI_EN_APMC);
  LdrSmmInfo->SmiCtrlReg.SmiEosPos  = (UINT8)HighBitSet32 (B_SMI_EN_EOS);
  LdrSmmInfo->SmiCtrlReg.Address    = (UINT32)(ACPI_BASE_ADDRESS + R_SMI_EN);

  //
  // Update smi status register data
  //
  LdrSmmInfo->SmiStsReg.RegType    = (UINT8)REG_TYPE_IO;
  LdrSmmInfo->SmiStsReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiStsReg.SmiApmPos  = (UINT8)HighBitSet32 (B_SMI_STS_APMC);
  LdrSmmInfo->SmiStsReg.Address    = (UINT32)(ACPI_BASE_ADDRESS + R_SMI_STS);

  //
  // Update smi lock register data
  //
  LdrSmmInfo->SmiLockReg.RegType    = (UINT8)REG_TYPE_MMIO;
  LdrSmmInfo->SmiLockReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiLockReg.SmiLockPos = (UINT8)HighBitSet32 (B_PMC_GEN_PMCON_2_SMI_LOCK);
  LdrSmmInfo->SmiLockReg.Address    = (UINT32)(PMC_BASE_ADDRESS + R_PMC_GEN_PMCON_2);
}


/**
 Update Hob Info with platform specific data

 @param[in]  Guid          The GUID to tag the customized HOB.
 @param[out] HobInfo       The start address of GUID HOB data.
 **/
VOID
EFIAPI
PlatformUpdateHobInfo (
  IN CONST EFI_GUID              *Guid,
  OUT      VOID                  *HobInfo
)
{
  EFI_STATUS        Status;

  ASSERT (Guid != NULL);
  ASSERT (HobInfo != NULL);

  // Just compare Guid memory addresses which are in Stage2 data section instead of CompareGuid ()
  if (Guid == &gEfiGraphicsInfoHobGuid) {
    UpdateFrameBufferInfo (HobInfo);
  } else if (Guid == &gEfiGraphicsDeviceInfoHobGuid) {
    UpdateFrameBufferDeviceInfo (HobInfo);
  } else if (Guid == &gOsBootOptionGuid) {
    UpdateOsBootMediumInfo (HobInfo);
  } else if (Guid == &gLoaderPlatformInfoGuid) {
    UpdateLoaderPlatformInfo (HobInfo);
  } else if (Guid == &gSeedListInfoHobGuid) {
    Status = UpdateSeedListInfo (HobInfo);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Error caused while appending seeds to HOB: %r\n", Status));
    }
  } else if (Guid == &gSmmInformationGuid) {
    UpdateSmmInfo (HobInfo);
  }
}

/**
 Update ACPI CPU P-state PSS table

 @param[in]  PssTbl        Pointer to PSS package object start in AML buffer.
 @param[in]  Gnvs          Pointer to platform global NVS data

 **/
VOID
CpuPssPatch (
  IN VOID                 *PssTbl,
  IN GLOBAL_NVS_AREA      *Gnvs
)
{
  MSR_REGISTER        PlatformInfoMsr;
  MSR_REGISTER        TurboRatioLimit;
  PSS_PARAMS          PssParams;

  // Determine the bus ratio range
  PlatformInfoMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  PssParams.MaxBusRatio = PlatformInfoMsr.Bytes.SecondByte;
  PssParams.MinBusRatio = PlatformInfoMsr.Bytes.SixthByte;
  if ((Gnvs->CpuNvs.PpmFlags & PPM_TURBO) != 0) {
    TurboRatioLimit.Qword = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
    PssParams.TurboBusRatio = (UINT16) (TurboRatioLimit.Dwords.Low & B_MSR_TURBO_RATIO_LIMIT_1C);
  } else {
    PssParams.TurboBusRatio = 0;
  }
  PssParams.PackageMaxPower = FVID_MAX_POWER;
  PssParams.PackageMinPower = FVID_MIN_POWER;
  PssParams.GetRelativePower = NULL;
  PssParams.DoListAll = TRUE;

  AcpiPatchPssTable (PssTbl, &PssParams);

  return;
}

/**
  This function updates DSDT table PNVS/SPSS dynamically.

  @param[in]  Table         Pointer to ACPI table
  @param[in]  Gnvs          Pointer to platform global NVS data

  @retval    EFI_SUCCESS    DSDT has been patched successfully
  @retval    EFI_NOT_FOUND  Failed to find object to patch

**/
EFI_STATUS
UpdateAcpiDsdt (
  IN EFI_ACPI_DESCRIPTION_HEADER   *Table,
  IN GLOBAL_NVS_AREA               *Gnvs
  )
{
  BOOLEAN  PnvsFound;
  BOOLEAN  SpssFound;
  UINT8 *Ptr;
  UINT8 *End;

  PnvsFound = FALSE;
  SpssFound = FALSE;

  Ptr = (UINT8 *)Table;
  End = (UINT8 *)Table+ Table->Length;

  // Loop through the ASL looking for values that we must fix up.
  for (; Ptr < End; Ptr++) {
    if (!PnvsFound && (*(UINT32 *)Ptr == SIGNATURE_32 ('P', 'N', 'V', 'S')) &&
         (*(Ptr - 1) == AML_EXT_REGION_OP)) {
      * (UINT32 *) (Ptr + 6)  = (UINT32)(UINTN)&Gnvs->CpuNvs;
      * (UINT16 *) (Ptr + 11) = (UINT16)sizeof(CPU_NVS_AREA);
      PnvsFound = TRUE;
    }
    if (!SpssFound && (*(UINT32 *)Ptr == SIGNATURE_32 ('S', 'P', 'S', 'S')) &&
         (*(Ptr - 1) == AML_NAME_OP)) {
      CpuPssPatch (Ptr, Gnvs);
      SpssFound = TRUE;
    }
  }

  if (!PnvsFound || !SpssFound) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_SUCCESS;
  }
}

/**
  Update the DMAR table using new DmarLib

  @param[in, out] AcpiHeader         - The ACPI table header to be updated
**/
VOID
UpdateDmarAcpi (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER       *AcpiHeader
  )
{
  EFI_STATUS                                 Status;
  UINT8                                      Flags;
  UINT64                                     BaseAddress;
  EFI_ACPI_DMAR_STRUCTURE_HEADER             *DmarHdr;
  UINTN                                      McD0BaseAddress;

  // Check if VTd is enabled
  McD0BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
  if ((MmioRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A) & BIT23)) {
    DEBUG ((DEBUG_INFO, "Vtd Disabled, skip DMAR Table update\n"));
    return;
  }

  /// Set INTR_REMAP bit (BIT 0) if interrupt remapping is supported
  Flags = 0;
  if ((MmioRead32 (0xFED64000 + 0x10) & BIT3) && (MmioRead32 (DEF_VTD_BASE_ADDRESS + 0x10) & BIT3)) {
    Flags |= BIT0;
  }

  // Set X2APIC_OPT_OUT bit (BIT 1) based on APIC mode
  if (((AsmReadMsr64 (0x1B)) & (BIT11 + BIT10)) == BIT11 + BIT10) {
    Flags |= BIT1;
  } else {
    Flags &= 0xFD;
  }

  Status = AddAcpiDmarHdr (AcpiHeader, Flags);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to add DMAR header\n"));
    return;
  }

  // Add DRHD for Graphics Engine (VTD Engine 1)
  BaseAddress = ReadVtdBaseAddress(0);  // Graphics VTD Engine
  if (BaseAddress != 0) {
    DmarHdr = AddDrhdHdr (AcpiHeader, 0, SIZE_4KB, 0, BaseAddress);
    ASSERT (DmarHdr != NULL);
    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 2, 0);  // IGD
  }

  // Add DRHD for General Purpose Engine (VTD Engine 2)
  BaseAddress = ReadVtdBaseAddress(1);  // General Purpose VTD Engine
  if (BaseAddress != 0) {
    DmarHdr = AddDrhdHdr (AcpiHeader, 0, SIZE_4KB, 0, BaseAddress);
    ASSERT (DmarHdr != NULL);
    // Add IOAPIC scope
    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC, 0, 2, 0, 0x1F, 0);
    // Add HPET scope
    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET, 0, 0, 0, 0x1F, 0);
  }

  // Calculate DMAR table checksum
  AcpiHeader->Checksum = CalculateCheckSum8 ((UINT8 *)AcpiHeader, AcpiHeader->Length);
}

/**
  Update PCH NVS and SA NVS area address and size in ACPI table.

  @param[in] Current    Pointer to ACPI description header

  @retval    EFI_SUCCESS   Operation completed successfully.
  @retval    EFI_INVALID_PARAMETER Invalid paremeter.

**/
EFI_STATUS
EFIAPI
PlatformUpdateAcpiTable (
  IN UINT8                   *Current
  )
{
  EFI_STATUS                                        Status;
  EFI_ACPI_DESCRIPTION_HEADER                       *Table;
  EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER  *Hpet;
  UINT16                                            Data16;
  GLOBAL_NVS_AREA                                   *Gnvs;
  HDA_CFG_DATA                                      *HdaCfgData;

  Table  = (EFI_ACPI_DESCRIPTION_HEADER *) Current;
  Status = EFI_SUCCESS;
  Gnvs   = (GLOBAL_NVS_AREA *) (UINTN)PcdGet32 (PcdAcpiGnvsAddress);

  if (Table->Signature == EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    // Patch PNVS and SPSS
    // GNVS patching will be left to be patched by common AcpiInitLib
    Status = UpdateAcpiDsdt (Table, Gnvs);
  } else if (Table->Signature == EFI_ACPI_5_0_HIGH_PRECISION_EVENT_TIMER_TABLE_SIGNATURE) {
    // HPET
    Hpet = (EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER *) Current;

    Hpet->BaseAddressLower32Bit.Address = HPET_BASE_ADDRESS;
    Hpet->EventTimerBlockId             = EFI_ACPI_EVENT_TIMER_BLOCK_ID;
    Data16 = * (UINT16 *)(UINTN) HPET_BASE_ADDRESS;
    Data16 &= B_HPET_GCID_NT;
    if (Data16 != 0) {
      Hpet->EventTimerBlockId = Hpet->EventTimerBlockId | Data16;
    }

    DEBUG ((DEBUG_INFO, "Updated Hpet BaseAddressLower32Bit = 0x%08X, EventTimerBlockId = 0x%08X\n",
            (UINT32) Hpet->BaseAddressLower32Bit.Address, Hpet->EventTimerBlockId));
  } else if (Table->Signature == SIGNATURE_32 ('$', 'V', 'B', 'T')) {
    // Pointer to new VBT
    Status = PcdSet32S (PcdGraphicsVbtAddress, (UINT32)(UINTN)Table + VBT_OFFSET);
    IgdOpRegionPlatformInit();
  }

  if (MEASURED_BOOT_ENABLED() ) {
    if ((Table->Signature == EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE) ||
        (Table->OemTableId == ACPI_SSDT_TPM2_DEVICE_OEM_TABLE_ID)) {
      Status = UpdateTpm2AcpiTable (Table);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "UpdateTpm2AcpiTable fails! - %r\n", Status));
      }
    }
  }

  if (Table->Signature == EFI_ACPI_6_4_DMA_REMAPPING_TABLE_SIGNATURE) {
    DEBUG ((DEBUG_INFO, "Updated DMAR Table entries\n"));
    if (FeaturePcdGet (PcdVtdEnabled)) {
      UpdateDmarAcpi (Table);
    } else {
      Status = EFI_UNSUPPORTED;
    }
  }

  if (Table->Signature == NHLT_ACPI_TABLE_SIGNATURE) {
    HdaCfgData = (HDA_CFG_DATA *) FindConfigDataByTag (CDATA_HDA_TAG);
    ASSERT (HdaCfgData != NULL);

    if ((HdaCfgData == NULL) || (HdaCfgData->DspEnable == FALSE)) {
      DEBUG ((DEBUG_WARN, "HDA config is missing or disabled. But, allow to update NHLT.\n") );
    }
    Gnvs->PlatformNvs.HdaNhltAcpiAddr   = (UINT32) (UINTN)Table;
    Gnvs->PlatformNvs.HdaNhltAcpiLength = Table->Length;
    DEBUG ((DEBUG_INFO, "HdaNhltAcpi Addr 0x%08X, Length 0x%08X\n", \
           Gnvs->PlatformNvs.HdaNhltAcpiAddr, Gnvs->PlatformNvs.HdaNhltAcpiLength));

    DEBUG_CODE_BEGIN ();
    NhltAcpiTableDump ((NHLT_ACPI_TABLE *)Table);
    DEBUG_CODE_END ();
  }

  if (FeaturePcdGet (PcdPsdBiosEnabled)) {
    if (Table->Signature == EFI_ACPI_PSD_SIGNATURE) {
      UpdateAcpiPsdTable ( (VOID* )Current );
      DEBUG ( (DEBUG_INFO, "Updated Psd Table in AcpiTable Entries\n") );
    }
  }

  return Status;
}

/**
 Update APCI values in the Gnvs.

 @param[in] GnvsIn  pointer to platform Gnvs

**/
VOID
EFIAPI
PlatformUpdateAcpiGnvs (
  IN VOID  *GnvsIn
  )
{
  GLOBAL_NVS_AREA      *Gnvs;
  PLATFORM_NVS_AREA    *Pnvs;
  SYS_CPU_INFO         *SysCpuInfo;
  HDA_CFG_DATA         *HdaCfgData;
  DEV_EN_CFG_DATA      *DevEnCfgData;
  PCIE_RP_CFG_DATA     *PcieRpConfigData;
  PCIE_RP_PIN_CTRL     *PowerResetData;
  UINT8                Idx1;
  UINT8                Idx2;

  Gnvs = (GLOBAL_NVS_AREA *)GnvsIn;
  Pnvs = &Gnvs->PlatformNvs;
  SetMem (Gnvs, sizeof (GLOBAL_NVS_AREA), 0);

  Pnvs->BoardId = (UINT8)GetPlatformId ();

  DevEnCfgData = (DEV_EN_CFG_DATA *)FindConfigDataByTag (CDATA_DEV_EN_TAG);
  if (DevEnCfgData != NULL) {
    Pnvs->Ipc1Enable = (UINT8)DevEnCfgData->DevEnControl1.Ipc1Enable;
  }

  IgdOpRegionPlatformInit();

  SysCpuInfo = MpGetInfo ();
  if (SysCpuInfo != NULL) {
    Pnvs->LogicalProcessorCount = (UINT8)SysCpuInfo->CpuCount;
  } else {
    Pnvs->LogicalProcessorCount = 1;
  }

  Pnvs->Mmio32Base   = PcdGet32 (PcdPciResourceMem32Base);
  // SC_PCR_BASE_ADDRESS: 0xD000_0000 and PcdPciExpressBaseAddress: 0xE000_0000
  if (Pnvs->Mmio32Length < SC_PCR_BASE_ADDRESS) {
    Pnvs->Mmio32Length = SC_PCR_BASE_ADDRESS - Pnvs->Mmio32Base;
  } else {
    DEBUG((DEBUG_INFO, "acpi: Unable to configure M32L with M32B=0x%08X\n", Pnvs->Mmio32Base));
  }

  HdaCfgData = (HDA_CFG_DATA *) FindConfigDataByTag (CDATA_HDA_TAG);
  if ((HdaCfgData != NULL) && (HdaCfgData->DspEnable == TRUE)) {
    Pnvs->HdaDspFeatureMask     = HdaCfgData->DspFeatureMask;
    Pnvs->HdaDspModMask         = HdaCfgData->DspPpModuleMask;
  }

  PcieRpConfigData  = (PCIE_RP_CFG_DATA *)FindConfigDataByTag (CDATA_PCIE_RP_TAG);
  if (PcieRpConfigData != NULL) {
    PowerResetData = (PCIE_RP_PIN_CTRL *) &PcieRpConfigData->PcieRpPower0;
    for (Idx1 = 0; Idx1 < PCIE_MAX_ROOT_PORTS; Idx1++) {
      Idx2 = (UINT8)PowerResetData->PcieRpPower0.Wake;
      if (Idx2 >= sizeof(mPcieRpWakeGpeBit)) {
        Idx2 = 0;
      }
      Pnvs->PcieRpGpeWakeBit[Idx1] = mPcieRpWakeGpeBit[Idx2];
      PowerResetData++;
    }
  }

  SocUpdateAcpiGnvs ((VOID *)Gnvs);
}
