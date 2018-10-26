/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Stage2BoardInitLib.h"
#include <PsdLib.h>

#define  VBT_OFFSET            36

UINT32       mOtgDualRoleCfg0 = 0;

BL_HDAUDIO_VERB_TABLE VerbAlc662 = {
  /*
   *  VerbTable: (Realtek ALC662)
   *  Codec Vendor: 0x10EC0662
   */
  {
    0x10EC0662,     /* Vendor ID / Device ID */
    0xFF,           /* Revision ID */
    0xFF,           /* SDI number */
    15 * 4,         /* Number of data DWORDs */
    0               /* Front Pannel Support */
  },
  {
    /* ===== HDA Codec Subsystem ID Verb-table ===== */

    /* HDA Codec Subsystem ID  : 0x80860000 */
    0x00172000,
    0x00172100,
    0x00172286,
    0x00172380,


    /* ===== Pin Widget Verb-table ===== */

    /* Widget node 0x01 */
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    /* Pin widget 0x12 - DMIC */
    0x01271C00,
    0x01271D00,
    0x01271E13,
    0x01271F40,
    /* Pin widget 0x14 - FRONT (Port-D) */
    0x01471C10,
    0x01471D40,
    0x01471E01,
    0x01471F01,
    /* Pin widget 0x15 - SURR (Port-A) */
    0x01571C12,
    0x01571D10,
    0x01571E01,
    0x01571F01,
    /* Pin widget 0x16 - CEN/LFE (Port-G) */
    0x01671C11,
    0x01671D60,
    0x01671E01,
    0x01671F01,
    /* Pin widget 0x18 - MIC1 (Port-B) */
    0x01871C30,
    0x01871D90,
    0x01871EA1,
    0x01871F01,
    /* Pin widget 0x19 - MIC2 (Port-F) */
    0x01971C40,
    0x01971D90,
    0x01971EA1,
    0x01971F02,
    /* Pin widget 0x1A - LINE1 (Port-C) */
    0x01A71C3F,
    0x01A71D30,
    0x01A71E81,
    0x01A71F01,
    /* Pin widget 0x1B - LINE2 (Port-E) */
    0x01B71C1F,
    0x01B71D40,
    0x01B71E21,
    0x01B71F02,
    /* Pin widget 0x1C - CD-IN */
    0x01C71CF0,
    0x01C71D11,
    0x01C71E11,
    0x01C71F41,
    /* Pin widget 0x1D - BEEP-IN */
    0x01D71C01,
    0x01D71DE6,
    0x01D71E45,
    0x01D71F40,
    /* Pin widget 0x1E - S/PDIF-OUT */
    0x01E71C20,
    0x01E71D11,
    0x01E71E44,
    0x01E71F01,
    /* Widget node 0x20  */
    0x02050004,
    0x02040001,
    0x02050004,
    0x02040001,
    /* Widget node 0x20 - 1 */
    0x02050005,
    0x02040080,
    0x02050001,
    0x0204A9B8
  }
};

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

  GpioCfgDataBuffer = (VOID *)AllocateTemporaryPages (0);  //allocate new buffer
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
  OS_CONFIG_DATA_HOB    *OsConfigData;
  GEN_CFG_DATA          *GenCfgData;
  UINT32                MemorySize;
  UINT32                PldRsvdMemSize;

  MemorySize   = 0;
  OsConfigData = BuildGuidHob (&gOsConfigDataGuid, sizeof (OS_CONFIG_DATA_HOB));
  if (OsConfigData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  OsConfigData->Revision = 1;

  GenCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if ((GenCfgData != NULL) && (GenCfgData->OsCrashMemorySize != 0)) {
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
  }

  OsConfigData->OsCrashMemorySize = MemorySize;
  OsConfigData->EnableCrashMode   = GenCfgData->EnableCrashMode;
  if (MemorySize > 0) {
    PldRsvdMemSize = PcdGet32 (PcdPayloadReservedMemSize);
    PcdSet32 (PcdPayloadReservedMemSize, PldRsvdMemSize + MemorySize);
  }
  return EFI_SUCCESS;
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

  if (ResetType == EfiResetCold) {
    IocUartData = (IOC_UART_CFG_DATA *)FindConfigDataByTag (CDATA_IOC_UART_TAG);
    if (IocUartData == NULL) {
      DEBUG ((DEBUG_WARN, "CDATA_IOC_UART_TAG Not Found\n"));
      goto DoReset;
    }

    // Uart num max 4, Debug serial port 2
    ASSERT (IocUartData->DeviceIndex < 4);
    ASSERT (IocUartData->DeviceIndex != 2);

    PciBar = GetUartBaseAddress (IocUartData->DeviceIndex);
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
  UINT32 PciBase;
  UINT8  Bus;
  UINT8  Function;
  UINT8  IntPin;
  UINT8  Shift;
  UINT8  IntLine[]   = {20, 21, 22, 23};

  // Workaround for ACRN + AaaG
  // Need to program interrupt register for add-in cards
  Shift = 0;
  for (Function = PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_5; Function <= PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_6; Function++) {
    PciBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_2, Function);
    if (MmioRead32(PciBase) == 0xFFFFFFFF) {
      continue;
    }
    Bus = MmioRead8(PciBase + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
    PciBase = MmPciBase (Bus, 0, 0);
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

/**
  Init UART2 BAR

  Current ACRN is using fixed UART address for GP MRB target.
  So here init UART2 BAR to 0xFC000000
  And make sure it will be invoked in both normal and S3 path.

**/
VOID
InitUart2Bar (
  VOID
  )
{
  UINT32      PciBase;
  UINT32      Base;

  if (FeaturePcdGet (PcdVtdEnabled)) {
    Base    = 0xFC000000;
    PciBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_SC, \
                         PCI_DEVICE_NUMBER_LPSS_HSUART, \
                         (PCI_FUNCTION_NUMBER_LPSS_HSUART0 + 2));

    MmioAnd32   (PciBase + PCI_COMMAND_OFFSET, (UINT32)~ (EFI_PCI_COMMAND_MEMORY_SPACE));
    MmioWrite32 (PciBase + PCI_BASE_ADDRESSREG_OFFSET, Base);
    MmioOr32    (PciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
  }
}

VOID
SaveOtgRole (
  VOID
  )
{
  UINT32                            XhciPciBase;
  UINT32                            XhciBar;
  UINT8                             BootMode;

  BootMode = GetBootMode ();
  if (BootMode != BOOT_ON_S3_RESUME) {
    return;
  }

  XhciPciBase = MmPciBase (0, 21, 0);
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
  UINT32                            XhciPciBase;
  UINT32                            XhciBar;
  UINT32                            DualRoleCfg0;
  UINT8                             BootMode;

  BootMode = GetBootMode ();
  if (BootMode != BOOT_ON_S3_RESUME) {
    return;
  }

  XhciPciBase  = MmPciBase (0, 21, 0);
  XhciBar      = MmioRead32 (XhciPciBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFF0000;
  DualRoleCfg0 = MmioRead32 (XhciBar + R_XHCI_MEM_DUAL_ROLE_CFG0);
  DEBUG ((DEBUG_INFO, "Before restore DualRoleCfg0: 0x%x \n", DualRoleCfg0));

  MmioWrite32 (XhciBar + R_XHCI_MEM_DUAL_ROLE_CFG0, mOtgDualRoleCfg0);
}

/**
  Set framebuffer range as WC using MTRR to improve performance.

  The BSP MTRR needs to be programmed before FspSiliconInit() API so that
  all APs' MTRRs will be syned up during FspSiliconInit() call.

**/
VOID
SetFrameBufferWriteCombining (
  VOID
)
{
  UINT32             MsrIdx;
  UINT32             MsrMax;
  UINT32             Base;

  // Enable Framebuffer as WC.
  MsrMax = EFI_MSR_CACHE_VARIABLE_MTRR_BASE +
           (2 * (UINT32)(AsmReadMsr64(EFI_MSR_IA32_MTRR_CAP) & B_EFI_MSR_IA32_MTRR_CAP_VARIABLE_SUPPORT));
  for (MsrIdx = EFI_MSR_CACHE_VARIABLE_MTRR_BASE; MsrIdx < MsrMax; MsrIdx += 2) {
    // Try to find a free MTRR pair
    if ((AsmReadMsr64(MsrIdx + 1) & B_EFI_MSR_CACHE_MTRR_VALID) == 0) {
      break;
    }
  }

  if (MsrIdx < MsrMax) {
    // Framebuffer belongs to PMEM32 in PCI resource allocation.
    // The 1st 256MB from PcdPciResourceMem32Base will be consumed by MEM32 resource.
    // And framebuffer should be allocated to the next 256MB aligned address.
    Base = (PcdGet32 (PcdPciResourceMem32Base) + SIZE_256MB)  &  ~(SIZE_256MB - 1);
    AsmWriteMsr64 (MsrIdx,     Base | CACHE_WRITECOMBINING);
    AsmWriteMsr64 (MsrIdx + 1, 0xF00000000ULL + B_EFI_MSR_CACHE_MTRR_VALID + (UINT32)(~(SIZE_256MB - 1)));
  } else {
    DEBUG ((DEBUG_WARN, "Failed to find a free MTRR pair for framebuffer!\n"));
  }
}

/**
  Clear FSP HOB data

**/
VOID
ClearFspHob (
  VOID
  )
{
  LOADER_GLOBAL_DATA          *LdrGlobal;
  EFI_HOB_HANDOFF_INFO_TABLE  *HandOffHob;
  UINT32                      Length;

  LdrGlobal  = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ();
  HandOffHob = (EFI_HOB_HANDOFF_INFO_TABLE  *) LdrGlobal->FspHobList;
  Length     = (UINT8 *) (UINTN) HandOffHob->EfiEndOfHobList - (UINT8 *)HandOffHob;
  ZeroMem (HandOffHob, Length);
  LdrGlobal->FspHobList = NULL;
}

/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
BoardInit (
  IN  BOARD_INIT_PHASE    InitPhase
  )
{
  EFI_STATUS         Status;
  UINT32             VarBase;
  UINT32             VarSize;
  GEN_CFG_DATA      *GenericCfgData;
  UINT32             TcoCnt;
  UINT64             MsrData;
  UINTN              SpiBaseAddress;

  switch (InitPhase) {
  case PreSiliconInit:
    GpioInit ();
    SetGpioPadCfgLock();
    SpiConstructor ();
    Status = GetComponentInfo (FLASH_MAP_SIG_VARIABLE, &VarBase, &VarSize);
    if (!EFI_ERROR(Status)) {
      VariableConstructor (VarBase, VarSize);
    }
    SaveOtgRole();
    if (PcdGetBool (PcdFramebufferInitEnabled)) {
      // Enable framebuffer as WC for performance
      SetFrameBufferWriteCombining ();
    }
    GenericCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
    if (GenericCfgData != NULL) {
      SetPayloadId (GenericCfgData->PayloadId);
    }
    break;
  case PostSiliconInit:
    // To prevent from generating MCA for CLFLUSH flash region
    AsmMsrAnd32 (IA32_MC4_CTL, (UINT32)~BIT4);
    // Enable GFX PCI command register if framebuffer init is required.
    if (PcdGetBool (PcdFramebufferInitEnabled)) {
      PciWrite8 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_COMMAND_OFFSET), \
                 EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
    }
    BuildOsConfigDataHob ();
    break;
  case PostPciEnumeration:
    IgdOpRegionInit();
    if (PcdGetBool (PcdSeedListEnabled)) {
      Status = GenerateSeeds ();
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Error in seeds generation: %r\n", Status));
      }
    }
    InitUart2Bar ();
    AssignPciIrqs ();
    RestoreOtgRole ();
    break;
  case EndOfStages:
    RegisterHeciService();
    InitPlatformService ();

    // Lock down SPI for all other payload entry except FWUpdate and OSloader
    // as this phase is too early for them to lock it here
    SpiBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_SC,
                       PCI_DEVICE_NUMBER_SPI,
                       PCI_FUNCTION_NUMBER_SPI
                       );

    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() != 0)) {
      // Set the BIOS Lock Enable and EISS bits
      MmioOr8 (SpiBaseAddress + R_SPI_BCR, (UINT8) (B_SPI_BCR_BLE | B_SPI_BCR_EISS));

      ClearFspHob ();
    }
    break;
  case ReadyToBoot:
    // Lock down SPI for everything
    SpiBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_SC,
                       PCI_DEVICE_NUMBER_SPI,
                       PCI_FUNCTION_NUMBER_SPI
                       );
    // Set the BIOS Lock Enable and EISS bits
    MmioOr8 (SpiBaseAddress + R_SPI_BCR, (UINT8) (B_SPI_BCR_BLE | B_SPI_BCR_EISS));

    ClearFspHob ();

    // Lock down Tco WDT just before handling off to OS
    TcoCnt = IoRead32 (ACPI_BASE_ADDRESS + R_TCO1_CNT);
    IoWrite32 (ACPI_BASE_ADDRESS + R_TCO1_CNT, TcoCnt | B_TCO1_CNT_LOCK);
    break;
  case EndOfFirmware:
    // Clear known MCA logged in BANK4 and enable this MCA again
    AsmWriteMsr64 (IA32_MC4_STATUS, 0);
    AsmMsrOr32    (IA32_MC4_CTL,    (UINT32)BIT4);

    // Set IA_Untrusted mode
    MsrData = AsmReadMsr64 (EFI_MSR_POWER_MISC);
    MsrData |= B_EFI_MSR_POWER_MISC_ENABLE_IA_UNTRUSTED_MODE;
    AsmWriteMsr64 (EFI_MSR_POWER_MISC, MsrData);
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

  FspsConfig->HdAudioIoBufferOwnership = 0;

  FspsConfig->HdaVerbTableEntryNum = 1;
  FspsConfig->HdaVerbTablePtr = (UINT32) (&VerbAlc662);

  FspsConfig->Eist = 1;
  FspsConfig->BiProcHot = 1;
  FspsConfig->TurboMode = 1;
  FspsConfig->MonitorMwaitEnable = 0;
  FspsConfig->CdClock = 4;

  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    // Disable Eiss for firmware update
    FspsConfig->SpiEiss = 0x0;
    FspsConfig->BiosInterface = 0x0;
  } else {
    FspsConfig->SpiEiss = 0x1;
  }

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
    PlatformId = (Value64 >> 50) & 7;
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

  FspsConfig->HdaEnable                   = (UINT8)DevEnCfgData->DevEnControl0.HdaEnable;
  FspsConfig->DspEnable                   = (UINT8)DevEnCfgData->DevEnControl0.DspEnable;
  FspsConfig->Pme                         = 1;
  FspsConfig->HdAudioIoBufferOwnership    = 3;
  FspsConfig->HdAudioLinkFrequency        = 0;
  FspsConfig->HdAudioIDispLinkFrequency   = 0;
  FspsConfig->DspEndpointDmic             = 0;
  FspsConfig->DspEndpointI2sSkp           = 1;
  FspsConfig->DspEndpointI2sHp            = 1;
  FspsConfig->DspEndpointBluetooth        = 0;
  FspsConfig->DspFeatureMask              = 0x2A;
  FspsConfig->HDAudioPwrGate              = 1;
  FspsConfig->HDAudioClkGate              = 1;
  FspsConfig->HdAudioDspUaaCompliance     = 1;

  if (PcdGetBool (PcdFramebufferInitEnabled)) {
    FspsConfig->GraphicsConfigPtr = PcdGet32 (PcdGraphicsVbtAddress);
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

  VariableMrcData = GetGuidHobData (((LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ())->FspHobList,
                                          &VarLength, &gFspVariableNvDataHobGuid);
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
  MemPool       = AllocateTemporaryPages (0);
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

    DataSize  = RleCompressData (Buffer, Length, (UINT8 *)MemPool + sizeof (MRC_PARAM_HDR));
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
    CopyMem ((UINT8 *)MrcVarHdr, (UINT8 *)MrcDataBase + MrcNvDataOffset, sizeof (MRC_VAR_HDR));

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
      MrcVarHdr->SlotNum    = (SIZE_4KB - sizeof (MRC_VAR_HDR)) / MRC_VAR_SLOT_LENGTH;
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
 Update serial port information to global HOB data structure.

 @param SerialPortInfo  Pointer to global HOB data structure.
 **/
VOID
EFIAPI
UpdateSerialPortInfo (
  IN  SERIAL_PORT_INFO  *SerialPortInfo
  )
{
  SerialPortInfo->Type     = 2;
  SerialPortInfo->BaseAddr = GetSerialPortBase();
  SerialPortInfo->RegWidth = GetSerialPortStrideSize();

  DEBUG ((DEBUG_INFO, "SerialPortInfo Type=%d BaseAddr=0x%08X RegWidth=%d\n",
          SerialPortInfo->Type, SerialPortInfo->BaseAddr, SerialPortInfo->RegWidth));
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
  OS_CFG_BOOT            *OsBootCfgData;
  PLATFORM_DATA          *PlatformData;
  BOOT_MODE_OPTION       *BootModeOption;
  BOOT_MODE_TARGET        Target;
  BOOT_MODE_ACTION        Action;
  UINT32                  Length;
  UINT8                   Count;

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

  OsBootCfgData = (OS_CFG_BOOT *)FindConfigDataByTag (CDATA_OS_TAG);
  if (OsBootCfgData == NULL) {
    DEBUG ((DEBUG_ERROR, "ERROR: Could not find OS boot option from config data!\n"));
    return ;
  }

  Count = OsBootCfgData->OsBootOptionCount;
  if (Count > PcdGet32(PcdOsBootOptionNumber)) {
    Count = (UINT8)PcdGet32(PcdOsBootOptionNumber);
    DEBUG ((DEBUG_ERROR, "OS boot option count (0x%x) is bigger than support max (0x%x)\n",
      OsBootCfgData->OsBootOptionCount, PcdGet32(PcdOsBootOptionNumber)));
  }

  Length = sizeof (OS_BOOT_OPTION) * Count;
  CopyMem(OsBootOptionList->OsBootOption, OsBootCfgData->OsBootOptions, Length);

  //
  // Update current boot and count based on target and action
  //
  switch (Target) {
  case BootModeTargetNormal:
    OsBootOptionList->CurrentBoot = 0;
    break;
  case BootModeTargetProvision:
    OsBootOptionList->CurrentBoot = OsBootCfgData->OsBootOptionCount - 1;
    break;
  default:
    DEBUG ((DEBUG_ERROR, "Not Supported Boot Target!\n"));
    break;
  }

  if (Action == BootModeActionShell) {
    DEBUG ((DEBUG_INFO, "Set boot to shell!\n"));
    OsBootOptionList->BootToShell = 1;
  }
  OsBootOptionList->Revision          = 1;
  OsBootOptionList->OsBootOptionCount = Count;
}

/**
 Update the frame buffer info by reading the PCI address.

 @param[in] FrameBufferInfo pointer to global HOB data structure.
 **/
VOID
EFIAPI
UpdateFrameBufferInfo (
  IN  FRAME_BUFFER_INFO  *FrameBufferInfo
  )
{
  FrameBufferInfo->LinearFrameBuffer = PciRead32 (PCI_LIB_ADDRESS (0, 2, 0, 0x18)) & 0xFFFFFF00;
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
  Status = GetVariable ("MMCDLL", NULL, &VariableLen, &EmmcTuningData);
  if (EFI_ERROR (Status)) {
    AsciiStrCpy (EmmcTuningData.SerialNumber, "badbadbadbadba");
  }

  AsciiStrCpy(LoaderPlatformInfo->SerialNumber, EmmcTuningData.SerialNumber);

  PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if(PlatformData != NULL) {
    LoaderPlatformInfo->SecurityFlags = (PlatformData->BtGuardInfo.Bpm.Vb & FeaturePcdGet(PcdVerifiedBootEnabled)) |
                                        ((PlatformData->BtGuardInfo.Bpm.Mb & FeaturePcdGet(PcdMeasuredBootEnabled) & MEASURED_BOOT_ENABLED() ) << 1);

    // Get Manufacturing Mode from Heci
    ReadHeciFwStatus(&HeciFwSts);
    LoaderPlatformInfo->SecurityFlags |= (HeciFwSts & BIT4) >> 2;
  }
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
  if (Guid == &gLoaderFrameBufferInfoGuid) {
    UpdateFrameBufferInfo (HobInfo);
  } else if (Guid == &gLoaderSerialPortInfoGuid) {
    UpdateSerialPortInfo (HobInfo);
  } else if (Guid == &gOsBootOptionGuid) {
    UpdateOsBootMediumInfo (HobInfo);
  } else if (Guid == &gLoaderPlatformInfoGuid) {
    UpdateLoaderPlatformInfo (HobInfo);
  } else if (Guid == &gSeedListInfoHobGuid) {
    Status = UpdateSeedListInfo (HobInfo);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Error caused while appending seeds to HOB: %r\n", Status));
    }
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
  UINT8              *Ptr;
  PSS_PACKAGE_LAYOUT *PssPackage;
  UINT16             *PackageLength;
  UINT32              NumberOfStates;
  UINT32              MaxNumberOfStates;
  UINT32              Index;
  MSR_REGISTER        PlatformInfoMsr;
  MSR_REGISTER        TurboRatioLimit;
  UINT8               MaxBusRatio;
  UINT8               MinBusRatio;
  UINT8               TurboBusRatio;
  UINT8               BusRatio;
  UINT8               BusRatioRange;
  UINT16              PowerRange;
  UINT8               Turbo;
  UINT32              PowerStep;
  UINT16              NewPackageLength;

  Ptr = PssTbl;
  MaxNumberOfStates = Ptr[7];
  PssPackage = (PSS_PACKAGE_LAYOUT *) &Ptr[8];

  // Determine whether turbo mode is supported or not
  Turbo = ((Gnvs->CpuNvs.PpmFlags & PPM_TURBO) ? 1 : 0);

  // Determine the bus ratio range
  PlatformInfoMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  TurboRatioLimit.Qword = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
  MaxBusRatio           = PlatformInfoMsr.Bytes.SecondByte;
  MinBusRatio           = PlatformInfoMsr.Bytes.SixthByte;
  TurboBusRatio         = (UINT8) (TurboRatioLimit.Dwords.Low & B_MSR_TURBO_RATIO_LIMIT_1C);
  if (Turbo != 0) {
    MaxBusRatio = TurboBusRatio;
  }
  BusRatioRange = MaxBusRatio - MinBusRatio;
  PowerRange    = FVID_MAX_POWER - FVID_MIN_POWER;
  NumberOfStates = ((UINT32)(BusRatioRange + 1) < MaxNumberOfStates ? (BusRatioRange + 1) : MaxNumberOfStates);
  DEBUG((EFI_D_INFO, "Number of P States: %d (Ratio %d ~ %d)\n", NumberOfStates, MinBusRatio, MaxBusRatio));

  NewPackageLength = (UINT16) (NumberOfStates * sizeof (PSS_PACKAGE_LAYOUT) + 3);
  Ptr[7]           = (UINT8) NumberOfStates;
  PackageLength    = (UINT16 *) &Ptr[5];
  *PackageLength   = ((NewPackageLength & 0x0F) | 0x40) | ((NewPackageLength << 4) & 0x0FF00);

  PowerStep = PowerRange / NumberOfStates;
  for (Index = 0; Index < NumberOfStates; Index++) {
    BusRatio = (UINT8)(MaxBusRatio - Index);
    PssPackage->CoreFrequency = BusRatio * 100;
    PssPackage->Control       = ((UINT32)BusRatio) << 8;
    PssPackage->Status        = ((UINT32)BusRatio) << 8;
    PssPackage->Power         = FVID_MAX_POWER - Index * PowerStep;
    PssPackage->TransLatency  = NATIVE_PSTATE_LATENCY;
    PssPackage->BMLatency     = PSTATE_BM_LATENCY;
    PssPackage++;
  }

  // Set remaining as padding byte
  SetMem (PssPackage, (MaxNumberOfStates - Index) * sizeof (PSS_PACKAGE_LAYOUT), AML_NOOP_OP);

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
      * (UINT32 *) (Ptr + 6)  = (UINT32)&Gnvs->CpuNvs;
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
  EFI_ACPI_DMAR_TABLE                               *Dmar;
  PLATFORM_DATA                                     *PlatformData;
  GLOBAL_NVS_AREA                                   *Gnvs;

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
    Data16 = * (UINT16 *) HPET_BASE_ADDRESS;
    Data16 &= B_HPET_GCID_NT;
    if (Data16 != 0) {
      Hpet->EventTimerBlockId = Hpet->EventTimerBlockId | Data16;
    }

    DEBUG ((DEBUG_INFO, "Updated Hpet BaseAddressLower32Bit = 0x%08X, EventTimerBlockId = 0x%08X\n",
            (UINT32) Hpet->BaseAddressLower32Bit.Address, Hpet->EventTimerBlockId));
  } else if (Table->Signature == SIGNATURE_32 ('$', 'V', 'B', 'T')) {
    // Pointer to new VBT
    PcdSet32 (PcdGraphicsVbtAddress, (UINT32)(UINTN)Table + VBT_OFFSET);
    IgdOpRegionInit ();
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

  if (FeaturePcdGet (PcdVtdEnabled)) {
    if (Table->Signature == EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE) {
      PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();
      if (PlatformData->RmrrUsbAddress != 0) {
        Dmar = (EFI_ACPI_DMAR_TABLE *)Table;
        Dmar->RmrrHeci.RmrBaseAddress  = PlatformData->RmrrUsbAddress;
        Dmar->RmrrHeci.RmrLimitAddress = Dmar->RmrrHeci.RmrBaseAddress + VTD_RMRR_USB_LENGTH - 1;
      }
      UpdateDmarAcpi (Table);
    }
  }

  if (Table->Signature == NHLT_ACPI_TABLE_SIGNATURE) {
    Gnvs->PlatformNvs.HdaNhltAcpiAddr   = (UINT32) (UINTN)Table;
    Gnvs->PlatformNvs.HdaNhltAcpiLength = Table->Length;
    DEBUG ((DEBUG_INFO, "HdaNhltAcpi Addr 0x%08X, Length 0x%08X\n", \
           Gnvs->PlatformNvs.HdaNhltAcpiAddr, Gnvs->PlatformNvs.HdaNhltAcpiLength));
  }

  if (FeaturePcdGet (PcdPsdBiosEnabled)) {
    if (Table->Signature == EFI_ACPI_PSD_SIGNATURE) {
      UpdateAcpiPsdTable ( (VOID* )Current );
      DEBUG ( (DEBUG_INFO, "Updated Psd Table in AcpiTable Entries\n") );
    }
  }
  ASSERT_EFI_ERROR (Status);

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

  Gnvs = (GLOBAL_NVS_AREA *)GnvsIn;
  Pnvs = &Gnvs->PlatformNvs;
  SetMem (Gnvs, sizeof (GLOBAL_NVS_AREA), 0);

  Pnvs->BoardId = (UINT8)GetPlatformId ();

  SysCpuInfo = MpGetInfo ();
  if (SysCpuInfo != NULL) {
    Pnvs->LogicalProcessorCount = (UINT8)SysCpuInfo->CpuCount;
  } else {
    Pnvs->LogicalProcessorCount = 1;
  }

  Pnvs->Mmio32Base   = PcdGet32 (PcdPciResourceMem32Base);
  Pnvs->Mmio32Length = 0xD0000000 - Pnvs->Mmio32Base;

  SocUpdateAcpiGnvs ((VOID *)Gnvs);
}
