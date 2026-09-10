/** @fileP2SbSocLib

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/BootloaderCoreLib.h>
#include <PchAccess.h>
#include <CpuRegs.h>
#include <FsptUpd.h>
#include <PlatformData.h>
#include <GpioConfig.h>

#if FixedPcdGetBool (PcdWclSupport) == 1
#include <GpioV2PinsWclPcd.h>
#include <Register/PchRegsGpioVer2Wcl.h>
#else
#include <GpioV2PinsPtlPcd.h>
#include <Register/PchRegsGpioVer2.h>
#endif

#include <Library/ConfigDataLib.h>
#include <Library/PchInfoLib.h>
#include <Register/GpioV2ChipsetId.h>
#include <Library/TcoTimerLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <GpioV2Config.h>
#include <Library/GpioV2Lib.h>

#define UCODE_REGION_BASE   FixedPcdGet32(PcdUcodeBase)
#define UCODE_REGION_SIZE   FixedPcdGet32(PcdUcodeSize)
#define SG1B_REDB_BASE      (UINT32) ((2 * FixedPcdGet32(PcdTopSwapRegionSize)) + FixedPcdGet32(PcdRedundantRegionSize))
#define CODE_REGION_SIZE    ALIGN_UP (SG1B_REDB_BASE, SIZE_1MB)
#define CODE_REGION_BASE    (UINT32) (BASE_4GB - CODE_REGION_SIZE)

#define PTL_MAX_SERIALIO_UART_CONTROLLERS     3

CONST
FSPT_UPD TempRamInitParams = {
  .FspUpdHeader = {
    .Signature = FSPT_UPD_SIGNATURE,
    .Revision  = 2,
    .Reserved  = {0},
  },
  .FsptArchUpd = {
    .Revision = 2,
    .Length = 0x20,
    .Reserved  = {0},
  },
  .FsptCoreUpd = {
    .MicrocodeRegionBase    = UCODE_REGION_BASE,
    .MicrocodeRegionSize    = UCODE_REGION_SIZE,
    .CodeRegionBase         = CODE_REGION_BASE,
    .CodeRegionSize         = CODE_REGION_SIZE
  },
  .FsptConfig = {
    .PcdLpssUartDebugEnable = 1,
    .PcdDebugInterfaceFlags = BIT4 | BIT1,
    .PcdLpssUartNumber      = FixedPcdGet32 (PcdDebugPortNumber),
    .PcdLpssUartMode        = 3,
    .PcdLpssUartPowerGating = 2,
    .PcdLpssUartBaudRate    = 115200,
    .PcdPciExpressBaseAddress   = FixedPcdGet64 (PcdPciMmcfgBase),
    .PcdPciExpressRegionLength  = 0x10000000,
    .PcdLpssUartParity        = 1,
    .PcdLpssUartDataBits    = 8,
    .PcdLpssUartStopBits    = 1,
    .PcdLpssUartAutoFlow    = 0,
    .PcdLpssUartRxPinMux    = 0,
    .PcdLpssUartTxPinMux    = 0,
    .PcdLpssUartRtsPinMux   = 0,
    .PcdLpssUartCtsPinMux   = 0,
    .PcdLpssUartDebugMmioBase = 0xFE02C000,
    .PcdLpcUartDebugEnable      = 1,
    .PcdSerialIoI2cNumber       = 0,
    .PcdSerialIoI2cMmioBase     = 0xFE028000,
    .PcdSerialIoI2cSdaPin       = 0x1A2893,
    .PcdSerialIoI2cSclPin       = 0x1A2894,
    .PcdSerialIoI2cPadsTerm     = GpioV2TermWpu20K,
  },
  .UpdTerminator = 0x55AA,
};

#if FixedPcdGetBool (PcdWclSupport) == 1
CONST GPIOV2_INIT_CONFIG mPtlUartGpioTable[] =
{ //     PAD                        PadMode             HostOwn         Direction        OutputState          InterruptConfig   ResetConfig       TerminationConfig   LockConfig     LockTx
  {GPIOV2_WCL_PCD_XXGPP_H_8, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_RXD
  {GPIOV2_WCL_PCD_XXGPP_H_9, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_TXD
  {GPIOV2_WCL_PCD_XXGPP_H_6, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_RXD
  {GPIOV2_WCL_PCD_XXGPP_H_7, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_TXD
  {GPIOV2_WCL_PCD_XXGPP_F_1, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_RXD
  {GPIOV2_WCL_PCD_XXGPP_F_2, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_TXD
};
#else
CONST GPIOV2_INIT_CONFIG mPtlUartGpioTable[] =
{ //     PAD                        PadMode             HostOwn         Direction        OutputState          InterruptConfig   ResetConfig       TerminationConfig   LockConfig     LockTx
  {GPIOV2_PTL_PCD_XXGPP_H_8, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_RXD
  {GPIOV2_PTL_PCD_XXGPP_H_9, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_TXD
  {GPIOV2_PTL_PCD_XXGPP_H_6, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_RXD
  {GPIOV2_PTL_PCD_XXGPP_H_7, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_TXD
  {GPIOV2_PTL_PCD_XXGPP_F_1, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_RXD
  {GPIOV2_PTL_PCD_XXGPP_F_2, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_TXD
};
#endif

/**
  Stitching process might pass some specific platform data to be
  consumed pretty early. This will be used to guide the platform initialization
  even before CFGDATA is available.

**/
VOID
EarlyPlatformDataCheck (
  VOID
)
{
  STITCH_DATA          *StitchData;

  // Stitching process might pass some platform specific data.
  StitchData = (STITCH_DATA *)(UINTN)(0xFFFFFFF4);

  if (StitchData->Marker != 0xAA) {
    // set default  as Debug UART
    // PlatformID will be deferred to be detected
    SetDebugPort (PcdGet8 (PcdDebugPortNumber));
  } else {
    SetDebugPort  (StitchData->DebugUart);
    if ((StitchData->PlatformId > 0) && (StitchData->PlatformId < 32)) {
      SetPlatformId (StitchData->PlatformId);
    }
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
  IN  BOARD_INIT_PHASE  InitPhase
  )
{
  UINT8             DebugPort;
  UINT32            MsrIdx;
  UINT32            ImgLen;
  UINT32            AdjLen;
  UINT64            MskLen;

  switch (InitPhase) {
  case PostTempRamInit:
    // Initialize TCO timer in board-specific SG1A file
    // as not to interfere with other boards' disable TCO
    // timer functions which do the same thing
    InitTcoTimer ();
    EarlyPlatformDataCheck ();
    DebugPort = GetDebugPort ();

    if (DebugPort < PTL_MAX_SERIALIO_UART_CONTROLLERS) {
      ConfigureGpioV2 (CDATA_NO_TAG, (VOID *)(mPtlUartGpioTable + DebugPort * 2), 2);
    }

    PlatformHookSerialPortInitialize ();
    SerialPortInitialize ();
    // Set DebugPrintErrorLevel to default PCD.
    SetDebugPrintErrorLevel (PcdGet32 (PcdDebugPrintErrorLevel));

    // Enlarge the code cache region to cover full flash for non-BootGuard case or fast boot case
    if ((AsmReadMsr64(MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == 0
        || PcdGetBool (PcdFastBootEnabled)) {
      // FSP-T does not allow to enable full flash code cache due to cache size restriction.
      // Here, MTRR is patched to enable full flash region cache to avoid performance penalty.
      // However, the SBL code flow should ensure only limited flash regions will be accessed
      // before FSP TempRamExit() is called. The combined DATA and CODE cache size should satisfy
      // the BWG requirement.
      MskLen = (AsmReadMsr64(MSR_CACHE_VARIABLE_MTRR_BASE + 1) | (SIZE_4GB - 1)) + 1;
      MsrIdx = MSR_CACHE_VARIABLE_MTRR_BASE + 1 * 2;
      ImgLen = PcdGet32(PcdFlashSize);
      // PCH only decodes max 16MB of SPI flash from the top down to MMIO.
      if (ImgLen > SIZE_16MB) {
        ImgLen = SIZE_16MB;
      }
      AdjLen = GetPowerOfTwo32(ImgLen);
      if (ImgLen > AdjLen) {
        AdjLen <<= 1;
      }
      AsmWriteMsr64(MsrIdx, (SIZE_4GB - AdjLen) | CACHE_WRITEPROTECTED);
      AsmWriteMsr64(MsrIdx + 1, (MskLen - AdjLen) | B_CACHE_MTRR_VALID);
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
