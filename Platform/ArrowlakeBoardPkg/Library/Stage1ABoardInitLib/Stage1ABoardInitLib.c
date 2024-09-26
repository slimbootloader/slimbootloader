/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
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
#include <GpioV2PinsMtlPchS.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <GpioV2Config.h>
#include <Library/GpioV2Lib.h>

#define UCODE_REGION_BASE   FixedPcdGet32(PcdUcodeBase)
#define UCODE_REGION_SIZE   FixedPcdGet32(PcdUcodeSize)

#define CODE_REGION_SIZE    FixedPcdGet32(PcdTopSwapRegionSize)
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
    .PcdSerialIoUartMode        = 3,
    .PcdSerialIoUartBaudRate    = 115200,
    .PcdPciExpressBaseAddress   = FixedPcdGet32 (PcdPciMmcfgBase),
    .PcdPciExpressRegionLength  = 0x10000000,

    .PcdLpcUartDebugEnable      = 1,
    .PcdSerialIoUartPowerGating = 1,
    .PcdDebugInterfaceFlags     = 0x12,
  },
  .UpdTerminator = 0x55AA,
};


CONST GPIOV2_INIT_CONFIG mArlsUartGpioTable[] =
{ //     PAD                        PadMode             HostOwn         Direction        OutputState          InterruptConfig   ResetConfig       TerminationConfig   LockConfig     LockTx
  {GPIOV2_MTL_PCH_S_GPP_C_8,  {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_RXD
  {GPIOV2_MTL_PCH_S_GPP_C_9,  {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_TXD
  {GPIOV2_MTL_PCH_S_GPP_C_10, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_RTS
  {GPIOV2_MTL_PCH_S_GPP_C_11, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_CTS

  {GPIOV2_MTL_PCH_S_GPP_C_12, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_RXD
  {GPIOV2_MTL_PCH_S_GPP_C_13, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_TXD
  {GPIOV2_MTL_PCH_S_GPP_C_14, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_RTS
  {GPIOV2_MTL_PCH_S_GPP_C_15, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_CTS

  {GPIOV2_MTL_PCH_S_GPP_C_20, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_RXD
  {GPIOV2_MTL_PCH_S_GPP_C_21, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_TXD
  {GPIOV2_MTL_PCH_S_GPP_C_22, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_RTS
  {GPIOV2_MTL_PCH_S_GPP_C_23, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_CTS
};

CONST GPIOV2_INIT_CONFIG mArlhUartGpioTable[] =
{ //     PAD                        PadMode             HostOwn         Direction        OutputState          InterruptConfig   ResetConfig       TerminationConfig   LockConfig     LockTx
  {GPIOV2_MTL_SOC_M_GPP_H8,  {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_RXD
  {GPIOV2_MTL_SOC_M_GPP_H9,  {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_TXD
//{GPIOV2_MTL_SOC_M_GPP_H10, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_RTS
//{GPIOV2_MTL_SOC_M_GPP_H11, {GpioV2PadModeNative1, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART0_CTS

  {GPIOV2_MTL_SOC_M_GPP_H6, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_RXD
  {GPIOV2_MTL_SOC_M_GPP_H7, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART1_TXD

  {GPIOV2_MTL_SOC_M_GPP_F1, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_RXD
  {GPIOV2_MTL_SOC_M_GPP_F2, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_TXD
// {GPIOV2_MTL_SOC_M_GPP_F0, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_RTS
// {GPIOV2_MTL_SOC_M_GPP_F3, {GpioV2PadModeNative2, GpioV2HostOwnGpio, GpioV2DirNone,  GpioV2StateDefault,  GpioV2IntDis,    GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Unlock }}, //SERIALIO_UART2_CTS
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

    if (DebugPort < MTL_MAX_SERIALIO_UART_CONTROLLERS) {
      if (MtlIsSocM ()) {
        ConfigureGpioV2 (CDATA_NO_TAG, (VOID *)(mArlhUartGpioTable + DebugPort * 2), 2);
      } else if (MtlIsSocS ()) {
        ConfigureGpioV2 (CDATA_NO_TAG, (VOID *)(mArlsUartGpioTable + DebugPort * 4), 4);
      }
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
