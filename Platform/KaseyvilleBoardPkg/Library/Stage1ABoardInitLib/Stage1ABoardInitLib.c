/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1ABoardInitLib.h"

/* Code region length aligned to bigger size like 1MB to save MTRR's */
#define INITIAL_CODE_REGION_LENGTH  ALIGN_UP ((FixedPcdGet32(PcdStage1ASize) + FixedPcdGet32(PcdStage1BSize)), SIZE_1MB)
#define INITIAL_CODE_REGION_BASE    (UINT32)(BASE_4GB - INITIAL_CODE_REGION_LENGTH)

#define UCODE_REGION_BASE   FixedPcdGet32(PcdUcodeBase)
#define UCODE_REGION_SIZE   FixedPcdGet32(PcdUcodeSize)
#define CODE_REGION_SIZE    INITIAL_CODE_REGION_LENGTH
#define CODE_REGION_BASE    INITIAL_CODE_REGION_BASE

CONST
FSPT_UPD TempRamInitParams = {
  .FspUpdHeader = {
    .Signature = FSPT_UPD_SIGNATURE, // UPD region signature
    .Revision  = 2, // Revision of data structure
    .Reserved  = {0},
  },
  .FsptCoreUpd = {
    .MicrocodeRegionBase    = UCODE_REGION_BASE,
    .MicrocodeRegionLength  = UCODE_REGION_SIZE,
    .CodeRegionBase         = CODE_REGION_BASE,
    .CodeRegionLength       = CODE_REGION_SIZE,
  },
  .UpdTerminator = 0x55AA,
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

  // Stitching process might pass some plafform specific data.
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

  UINT8                 DebugPort;
  PLATFORM_DATA_BL      *PlatformData;

  switch (InitPhase) {
  case PostTempRamInit:
    PlatformData = (PLATFORM_DATA_BL *) GetPlatformDataPtr ();
    // As PlatformData struct is statically allocated and cannot be NULL,
    // skipping the check for the otherwise condition
    if (PlatformData != NULL) {
        // Initialize with default configuration as BMC-less setup
        PlatformData->PlatformFeatures.BMCPresent = FALSE;
    }

    DisableWatchDogTimer ();
    EarlyPlatformDataCheck ();

    // Check if BMC is present. If BMC is not present, BMC UART wont be
    // available and boot logs need to be directed to S3M UART.
    // For simics enviroment and setup with BMC card, debug port selection
    // will be set as configured in BoardConfig.
    if (IsBMCPresent () == FALSE) {
      // BMC is not present. Override Debug port selection in BoardConfig to S3M UART (0x2).
      SetDebugPort (S3M_UART_PORT);
    } else {
      // BMC is present. Update platform data to remember the setting
      if (PlatformData != NULL) {
        PlatformData->PlatformFeatures.BMCPresent = TRUE;
      }
    }

    DebugPort = GetDebugPort ();

    if (DebugPort == BMC_UART_PORT) {
      PlatformSioEarlyInit ();
    }
    else if (DebugPort == S3M_UART_PORT) {
      PlatformHookSerialPortInitialize ();
    }

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
  return sizeof (PLATFORM_DATA_BL);
}
