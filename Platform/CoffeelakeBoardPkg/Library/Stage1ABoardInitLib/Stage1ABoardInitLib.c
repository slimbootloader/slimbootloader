/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BoardInitLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/SerialPortLib.h>
#include <Library/GpioLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/DebugLib.h>
#include <PlatformData.h>
#include <GpioPinsCnlLp.h>
#include <RegAccess.h>
#include <Register/CpuRegs.h>
#include <FsptUpd.h>
#include <PlatformData.h>
#include <ConfigDataDefs.h>

#define SIO_IDX             0x2E
#define SIO_DAT             0x2F
#define SIO_ENTRY_KEY       0x87
#define SIO_EXIT_KEY        0xAA

#define UCODE_REGION_BASE   FixedPcdGet32(PcdUcodeBase)
#define UCODE_REGION_SIZE   FixedPcdGet32(PcdUcodeSize)
#define CODE_REGION_BASE    FixedPcdGet32(PcdStage1ABase)
#define CODE_REGION_SIZE    FixedPcdGet32(PcdStage1ASize)

CONST
FSPT_UPD TempRamInitParams = {
  .FspUpdHeader = {
    .Signature = FSPT_UPD_SIGNATURE,
    .Revision  = 1,
    .Reserved  = {0},
  },
  .FsptCoreUpd = {
    .MicrocodeRegionBase   = UCODE_REGION_BASE,
    .MicrocodeRegionSize   = UCODE_REGION_SIZE,
    .CodeRegionBase        = CODE_REGION_BASE,
    .CodeRegionSize        = CODE_REGION_SIZE,
    .Reserved              = {0},
  },
  .FsptConfig = {
    .PcdSerialIoUartDebugEnable = 0,
    .PcdSerialIoUartNumber      = 0,
    .PcdSerialIoUart0PinMuxing  = 0,
    .UnusedUpdSpace0            = 0,
    .PcdSerialIoUartInputClock  = 0,
    .PcdPciExpressBaseAddress   = FixedPcdGet32 (PcdPciMmcfgBase),
    .PcdPciExpressRegionLength  = 0x20000000,
    .ReservedFsptUpd1           = {0},
  },
  .UpdTerminator = 0x55AA,
};

CONST UINT8  mUpxSioInitTable[] = {
  0x26, 0x80,  // Set to 48Mhz clock
  0x07, 0x01,  // Select UART1
  0x60, 0x03,  // UART1 Base MSB
  0x61, 0xF8,  // UART1 Base LSB
  0x70, 0x04,  // UART1 IRQ
  0x30, 0x01,  // UART1 Enable
  0x07, 0x02,  // Select UART2
  0x60, 0x02,  // UART2 Base MSB
  0x61, 0xF8,  // UART2 Base LSB
  0x70, 0x03,  // UART2 IRQ
  0xF1, 0x04,  // UART2 SIR mode
  0x30, 0x01,  // UART2 Enable
};

// F81438 mode
// M1  M2
//  0  0:  RS-422 Full Duplex
//  0  1:  RS-232
//  1  0:  RS-485 Driver Half Duplex
//  1  1:  RS-485 Receiver Full Duplex
CONST GPIO_INIT_CONFIG mUpxSioGpioTable[] = {
  {GPIO_CNL_LP_GPP_A20, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,  GpioHostDeepReset, GpioTermNone}},//COM1_MODE1
  {GPIO_CNL_LP_GPP_A21, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,  GpioHostDeepReset, GpioTermNone}},//COM1_MODE2
  {GPIO_CNL_LP_GPP_A22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,  GpioHostDeepReset, GpioTermNone}},//COM2_MODE1
  {GPIO_CNL_LP_GPP_A23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,  GpioHostDeepReset, GpioTermNone}},//COM2_MODE2
};

CONST GPIO_INIT_CONFIG mUartGpioTable[] = {
  {GPIO_CNL_LP_GPP_C8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_RXD
  {GPIO_CNL_LP_GPP_C9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_TXD
  {GPIO_CNL_LP_GPP_C12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_RXD
  {GPIO_CNL_LP_GPP_C13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_TXD
  {GPIO_CNL_LP_GPP_C20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RXD
  {GPIO_CNL_LP_GPP_C21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_TXD
};

CONST GPIO_INIT_CONFIG mGpioDebugPortPinTable[] = {
  {FixedPcdGet32(PcdGpioDebugPortPinPad), {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutHigh, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},
};

typedef enum {
  BootPartition1,
  BootPartition2,
  BootPartitionMax
} BOOT_PARTITION_SELECT;

/**
  Enable UART in SIO chip.

**/
VOID
EarlySioInit (
  VOID
)
{
  UINT8                 Idx;
  UINT32                LpcBaseAddr;

  if (GetPlatformId() == PLATFORM_ID_UPXTREME) {
    // Set SIO Mode GPIO pins
    GpioConfigurePads (ARRAY_SIZE(mUpxSioGpioTable), (GPIO_INIT_CONFIG *)mUpxSioGpioTable);

    // Enable SIO decoding
    LpcBaseAddr = PCI_LIB_ADDRESS (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_LPC,
                       PCI_FUNCTION_NUMBER_PCH_LPC,
                       0
                       );
    PciOr16 (LpcBaseAddr + R_LPC_CFG_IOE, B_LPC_CFG_IOE_SIO);

    // Unlock SIO (F71889/F81801)
    IoWrite8 (SIO_IDX, SIO_ENTRY_KEY);
    IoWrite8 (SIO_IDX, SIO_ENTRY_KEY);
    // Init logic devices
    for  (Idx = 0; Idx < sizeof(mUpxSioInitTable); Idx += 2) {
      IoWrite8 (SIO_IDX, mUpxSioInitTable[Idx]);
      IoWrite8 (SIO_DAT, mUpxSioInitTable[Idx + 1]);
    }
    // Lock SIO
    IoWrite8 (SIO_IDX, SIO_EXIT_KEY);
  }
}

/**
  Stitching process might pass some specific plafform data to be
  consumed pretty early. This will be used to guide the platform initialization
  even before CFGDATA is available.

**/
VOID
EarlyPlatformDataCheck (
  VOID
)
{
  STITCH_DATA          *StitchData;

  // Stitching process might pass some plafform specific data.
  StitchData = (STITCH_DATA *)(UINTN)0xFFFFFFF4;
  if (StitchData->Marker != 0xAA) {
    // PlatformID will be deferred to be detected
    SetDebugPort ( PcdGet8 (PcdDebugPortNumber));
  } else {
    SetDebugPort  (StitchData->DebugUart);
    if ((StitchData->PlatformId > 0) && (StitchData->PlatformId < 32)) {
      SetPlatformId (StitchData->PlatformId);
    }
  }
}

/**
  Determines the boot partition that the platform firmware is booting from

  @param[out] BootPartition   The Boot partition the platform is booting from

  @retval     EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBootPartition (
  OUT BOOT_PARTITION_SELECT      *BootPartition
  )
{
  UINTN     P2sbBase;
  UINT32    P2sbBar;
  //UINT16    RegOffset;
  //UINT8     RtcPortId;
  UINT32    Data32;
  BOOLEAN   P2sbIsHidden;

  //
  //
  // get Top swap register Bit0 in PCH Private Configuration Space.
  //
  //RegOffset  = 0x3414;               // RTC Backed Up Control (BUC) offset
  //RtcPortId  = 0xC3;                 // RTC port ID
  P2sbBase   = MmPciBase (0, PCI_DEVICE_NUMBER_PCH_LPC, 1); // P2SB device base

  P2sbIsHidden = FALSE;
  if (MmioRead16 (P2sbBase) == 0xFFFF) {
    MmioWrite8 (P2sbBase + R_P2SB_CFG_E0 + 1, 0); // unhide H2SB
    P2sbIsHidden = TRUE;
    DEBUG ((DEBUG_VERBOSE, "P2sb is hidden, unhide it\n"));
  }

  P2sbBar   = MmioRead32 (P2sbBase + R_P2SB_CFG_SBREG_BAR);
  P2sbBar  &= 0xFFFFFFF0;
  ASSERT (P2sbBar != 0xFFFFFFF0);

  Data32    = MmioRead32 (P2sbBar | ((PID_RTC_HOST) << 16) | (UINT16)(R_RTC_PCR_BUC));
  DEBUG ((DEBUG_VERBOSE, "--P2sbBar=0x%x, Data32=0x%x\n", P2sbBar, Data32));

  if (P2sbIsHidden) {
    MmioWrite8 (P2sbBase + R_P2SB_CFG_E0 + 1, BIT0); //Hide P2SB
    DEBUG ((DEBUG_VERBOSE, "Hide p2sb again.\n"));
  }

  if ((Data32 & BIT0) == 0) {
    DEBUG ((DEBUG_VERBOSE, "Boot from partition 1\n"));
    *BootPartition = BootPartition1;
  } else {
    DEBUG ((DEBUG_VERBOSE, "Boot from partition 2\n"));
    *BootPartition = BootPartition2;
  }

  return EFI_SUCCESS;
}


/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
)
{
  BOOT_PARTITION_SELECT     BootPartition;
  EFI_STATUS                Status;
  UINT8                     DebugPort;
  UINT32                    MsrIdx;
  UINT32                    ImgLen;
  UINT32                    AdjLen;
  UINT64                    MskLen;

  // DebugPort:
  //   0xFF: External 0x3F8 based I/O UART
  //   0,1,2: Internal SOC MMIO UART

  switch (InitPhase) {
  case PostTempRamInit:
    DisableWatchDogTimer ();
    EarlyPlatformDataCheck ();
    EarlySioInit ();
    DebugPort = GetDebugPort ();
    if (DebugPort < PCH_MAX_SERIALIO_UART_CONTROLLERS) {
      GpioConfigurePads (2, (GPIO_INIT_CONFIG *)mUartGpioTable + (DebugPort << 1));
    }

    if ((PcdGet32 (PcdDebugOutputDeviceMask) & DEBUG_OUTPUT_DEVICE_DEBUG_PORT) != 0) {
      if (PcdGet32 (PcdGpioDebugPortMmioBase)  != 0) {
        GpioConfigurePads (1, (GPIO_INIT_CONFIG *)mGpioDebugPortPinTable);
      }
    }

    PlatformHookSerialPortInitialize ();
    SerialPortInitialize ();
    Status = GetBootPartition (&BootPartition);
    if (!EFI_ERROR(Status)) {
      SetCurrentBootPartition (BootPartition == BootPartition2 ? 1 : 0);
    }

    // Enlarge the code cache region to cover full flash for non-BootGuard case only
    if ((AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == 0) {
       // WHL FSP-T does not allow to enable full flash code cache due to cache size restriction.
       // Here, MTRR is patched to enable full flash region cache to avoid performance penalty.
       // However, the SBL code flow should ensure only limited flash regions will be accessed
       // before FSP TempRamExit() is called. The combined DATA and CODE cache size should satisfy
       // the BWG requirement.
       MskLen = (AsmReadMsr64 (MSR_CACHE_VARIABLE_MTRR_BASE + 1) | (SIZE_4GB - 1)) + 1;
       MsrIdx = MSR_CACHE_VARIABLE_MTRR_BASE + 1 * 2;
       ImgLen = PcdGet32 (PcdFlashSize);
       AdjLen = GetPowerOfTwo32 (ImgLen);
       if (ImgLen > AdjLen) {
         AdjLen <<= 1;
       }
       AsmWriteMsr64 (MsrIdx,     (SIZE_4GB - AdjLen) | EFI_CACHE_WRITEPROTECTED);
       AsmWriteMsr64 (MsrIdx + 1, (MskLen - AdjLen) | B_CACHE_MTRR_VALID);
    }
    break;
  default:
    break;
  }
}

/**
  Get size of Platform Specific Data

  @param[in] none

  @retval    UINT32     Size of Platform Specific Data

**/
UINT32
EFIAPI
GetPlatformDataSize (
  IN  VOID
  )
{
  return sizeof (PLATFORM_DATA);
}


