/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1ABoardInitLib.h"

#define UCODE_REGION_BASE   FixedPcdGet32(PcdUcodeBase)
#define UCODE_REGION_SIZE   FixedPcdGet32(PcdUcodeSize)
#define CODE_REGION_SIZE    INITIAL_CODE_REGION_LENGTH
#define CODE_REGION_BASE    INTTIAL_CODE_REGION_BASE

CONST
FSPT_UPD TempRamInitParams = {
  .FspUpdHeader = {
    .Signature = FSPT_UPD_SIGNATURE,
    .Revision  = 1,
    .Reserved  = {0},
  },
  .FsptCoreUpd = {
    .MicrocodeRegionBase    = UCODE_REGION_BASE,
    .MicrocodeRegionLength  = UCODE_REGION_SIZE,
    .CodeRegionBase         = CODE_REGION_BASE,
    .CodeRegionLength       = CODE_REGION_SIZE,
    .Reserved1              = {0},
  },
  .UpdTerminator = 0x55AA,
};

CONST GPIO_INIT_CONFIG mUartGpioTable[] = {
  {GPIO_CDF_GPP_B20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},//UART0_RXD
  {GPIO_CDF_GPP_B21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},//UART0_TXD
  {GPIO_CDF_GPP_B22, {GpioPadModeNative2, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},//UART1_RXD
  {GPIO_CDF_GPP_B23, {GpioPadModeNative2, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},//UART1_TXD
  {GPIO_CDF_GPP_D0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},//IE_UART_RXD
  {GPIO_CDF_GPP_D1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirDefault, GpioOutDefault, GpioIntDefault, GpioResetDefault, GpioTermDefault}},//IE_UART_TXD
};

/**
  Stitching process might pass some specific plafform data to be
  consumed pretty early. This will be used to guide the platform initialization
  even before CFGDATA is available.

**/
STATIC
VOID
EarlyPlatformDataCheck (
  VOID
)
{
  STITCH_DATA          *StitchData;
  UINT8                 DebugPort;

  //
  // Stitching process might pass some plafform specific data.
  //
  StitchData = (STITCH_DATA *)(UINTN)0xFFFFFFF4;
  if (StitchData->Marker == 0xAA) {
    SetDebugPort (StitchData->DebugUart);
    if ((StitchData->PlatformId > 0) && (StitchData->PlatformId < 32)) {
      SetPlatformId (StitchData->PlatformId);
    }
  } else {
    // Use the default port defined in BoardConfig.py
    SetDebugPort ( PcdGet8 (PcdDebugPortNumber));
  }

  DebugPort = GetDebugPort ();
  if (DebugPort < (ARRAY_SIZE (mUartGpioTable) / 2)) {
    ConfigureGpio (CDATA_NO_TAG, 2, (UINT8 *)(mUartGpioTable + (DebugPort << 1)));
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
  switch (InitPhase) {
  case PostTempRamInit:
    DisableWatchDogTimer ();
    EarlyPlatformDataCheck ();
    PlatformHookSerialPortInitialize ();
    SerialPortInitialize ();
    PlatformSioEarlyInit ();
    if ((AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == 0) {
      EnlargeCodeRegionCache ();
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
