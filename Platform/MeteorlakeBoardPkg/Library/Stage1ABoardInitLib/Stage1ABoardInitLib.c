/** @file

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
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
#include <GpioV2PinsMtlSoc.h>
#include <Library/ConfigDataLib.h>
#include <Library/PchInfoLib.h>
#include <Library/MtlSocInfoLib.h>
#include <Register/GpioRegsVer6.h>
#include <Register/PmcRegsVer6.h>
#include <RegisterAccess.h>
#include <Library/TcoTimerLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/GpioV2Lib.h>

#define UCODE_REGION_BASE   FixedPcdGet32(PcdUcodeBase)
#define UCODE_REGION_SIZE   FixedPcdGet32(PcdUcodeSize)
#define SG1B_REDB_BASE      (UINT32) ((2 * FixedPcdGet32(PcdTopSwapRegionSize)) + FixedPcdGet32(PcdRedundantRegionSize) + FixedPcdGet32(PcdStage1BSize))
#define CODE_REGION_SIZE    ALIGN_UP (SG1B_REDB_BASE, SIZE_1MB)
#define CODE_REGION_BASE    (UINT32) (BASE_4GB - CODE_REGION_SIZE)

#define MTL_MAX_SERIALIO_UART_CONTROLLERS     3

CONST
FSPT_UPD TempRamInitParams = {
  .FspUpdHeader = {
    .Signature = FSPT_UPD_SIGNATURE,
    .Revision  = 2,
    .Reserved  = {0},
  },
  .FsptArchUpd = {
    .Revision = 1,
    .Length = 0x20,
    .Reserved  = {0},
  },
  .FsptCoreUpd = {
    .MicrocodeRegionBase    = UCODE_REGION_BASE,
    .MicrocodeRegionSize    = UCODE_REGION_SIZE,
    .CodeRegionBase         = CODE_REGION_BASE,
    .CodeRegionSize         = CODE_REGION_SIZE,
    .Reserved               = {0},
  },
  .FsptConfig = {
    .PcdSerialIoUartDebugEnable = 1,
    .PcdSerialIoUartNumber      = FixedPcdGet32 (PcdDebugPortNumber),
    .PcdSerialIoUartMode        = 4,
    .PcdSerialIoUartBaudRate    = 115200,
    .PcdPciExpressBaseAddress   = FixedPcdGet32 (PcdPciMmcfgBase),
    .PcdPciExpressRegionLength  = 0x10000000,
    .PcdSerialIoUartParity      = 1,
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



CONST GPIOV2_INIT_CONFIG mMtlmUartGpioTable[] =
{ //     PAD                        PadMode             HostOwn         Direction        OutputState          InterruptConfig   ResetConfig       TerminationConfig   LockConfig     LockTx
  {GPIOV2_MTL_SOC_M_GPP_H8,  {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_RXD
  {GPIOV2_MTL_SOC_M_GPP_H9,  {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_TXD
  {GPIOV2_MTL_SOC_M_GPP_H6, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_RXD
  {GPIOV2_MTL_SOC_M_GPP_H7, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_TXD
  {GPIOV2_MTL_SOC_M_GPP_F1, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_RXD
  {GPIOV2_MTL_SOC_M_GPP_F2, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_TXD
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
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
  )
{
  UINT8             DebugPort;

  switch (InitPhase) {
  case PostTempRamInit:
    // Initialize TCO timer in board-specific SG1A file
    // as not to interfere with other boards' disable TCO
    // timer functions which do the same thing
    InitTcoTimer ();
    EarlyPlatformDataCheck ();

    DebugPort = GetDebugPort ();
    if ((DebugPort != 0xFF) && (DebugPort < MTL_MAX_SERIALIO_UART_CONTROLLERS)) {
      ConfigureGpioV2 (CDATA_NO_TAG, (VOID *)(mMtlmUartGpioTable + DebugPort * 2), 2);
    }

    PlatformHookSerialPortInitialize ();
    SerialPortInitialize ();
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
