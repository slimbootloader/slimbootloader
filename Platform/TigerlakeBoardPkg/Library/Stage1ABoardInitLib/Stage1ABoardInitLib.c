/** @file

  Copyright (c) 2008 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BoardInitLib.h>
#include <FsptUpd.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <PlatformData.h>
#include <CpuRegs.h>
#include <PchAccess.h>
#include <Library/GpioLib.h>
#include <GpioConfig.h>
#include <GpioPinsVer2Lp.h>
#include <Library/ConfigDataLib.h>

#define UCODE_REGION_BASE   FixedPcdGet32(PcdUcodeBase)
#define UCODE_REGION_SIZE   FixedPcdGet32(PcdUcodeSize)
#define CODE_REGION_BASE    0xFFD00000
#define CODE_REGION_SIZE    0x00300000

const
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
    .PcdSerialIoUartDebugEnable = 1,
    .PcdSerialIoUartNumber      = FixedPcdGet32 (PcdDebugPortNumber) < PCH_MAX_SERIALIO_UART_CONTROLLERS ? \
                                    FixedPcdGet32 (PcdDebugPortNumber) : 2,
    .PcdSerialIoUartMode        = 4, // SerialIoUartSkipInit, let SBL init UART
    .PcdSerialIoUartBaudRate    = 115200,
    .PcdPciExpressBaseAddress   = FixedPcdGet32 (PcdPciMmcfgBase),
    .PcdPciExpressRegionLength  = 0x10000000,
    .PcdSerialIoUartParity      = 1, // NoParity
    .PcdSerialIoUartDataBits    = 8,
    .PcdSerialIoUartStopBits    = 1,
    .PcdSerialIoUartAutoFlow    = 0,
    .PcdSerialIoUartRxPinMux    = 0,
    .PcdSerialIoUartTxPinMux    = 0,
    .PcdSerialIoUartRtsPinMux   = 0,
    .PcdSerialIoUartCtsPinMux   = 0,
    .PcdLpcUartDebugEnable      = 1,
  },
  .UpdTerminator = 0x55AA,
};

CONST GPIO_INIT_CONFIG mUartGpioTable[] = {
  {GPIO_VER2_LP_GPP_C8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_RXD
  {GPIO_VER2_LP_GPP_C9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_TXD
  {GPIO_VER2_LP_GPP_C12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_RXD
  {GPIO_VER2_LP_GPP_C13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_TXD
  {GPIO_VER2_LP_GPP_C20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RXD
  {GPIO_VER2_LP_GPP_C21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_TXD
};

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

  // Stitching process might pass some plafform specific data.
  StitchData = (STITCH_DATA *)(UINTN)0xFFFFFFF4;
  if (StitchData->Marker != 0xAA) {
    // PlatformID will be deferred to be detected
    SetDebugPort ( PcdGet8 (PcdDebugPortNumber));
  } else {
    SetDebugPort  (StitchData->DebugUart);
    SetPlatformId (StitchData->PlatformId);
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
  UINT32                    MsrIdx;
  UINT32                    ImgLen;
  UINT32                    AdjLen;
  UINT64                    MskLen;
  UINT8                     DebugPort;

  switch (InitPhase) {
  case PostTempRamInit:
    DisableWatchDogTimer ();

    EarlyPlatformDataCheck ();
    DebugPort = GetDebugPort ();
    if (DebugPort < PCH_MAX_SERIALIO_UART_CONTROLLERS) {
      ConfigureGpio (CDATA_NO_TAG, 2, (UINT8 *)(mUartGpioTable + DebugPort * 2));
    }

    PlatformHookSerialPortInitialize ();
    SerialPortInitialize ();

    // Enlarge the code cache region to cover full flash for non-BootGuard case only
    if ((AsmReadMsr64(MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == 0) {
      // WHL FSP-T does not allow to enable full flash code cache due to cache size restriction.
      // Here, MTRR is patched to enable full flash region cache to avoid performance penalty.
      // However, the SBL code flow should ensure only limited flash regions will be accessed
      // before FSP TempRamExit() is called. The combined DATA and CODE cache size should satisfy
      // the BWG requirement.
      MskLen = (AsmReadMsr64(MSR_CACHE_VARIABLE_MTRR_BASE + 1) | (SIZE_4GB - 1)) + 1;
      MsrIdx = MSR_CACHE_VARIABLE_MTRR_BASE + 1 * 2;
      ImgLen = PcdGet32(PcdFlashSize);
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

