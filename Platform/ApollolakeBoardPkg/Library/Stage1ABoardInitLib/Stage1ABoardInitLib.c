/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BoardInitLib.h>
#include <Library/GpioLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/BootGuardLib.h>
#include <Library/BootloaderCoreLib.h>
#include <FsptUpd.h>
#include <GpioDefines.h>
#include <PlatformData.h>

const
FSPT_UPD TempRamInitParams = {
  .FspUpdHeader = {
    .Signature = FSPT_UPD_SIGNATURE,
    .Revision  = 1,
    .Reserved  = {0},
  },
  .FsptCommonUpd = {
    .Revision              = 1,
    .MicrocodeRegionBase   = 0,
    .MicrocodeRegionLength = 0,
    .CodeRegionBase        = 0xFFFE0000,
    .CodeRegionLength      = 0x00020000,
  },
  .UpdTerminator = 0x55AA,
};


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
  UINT32                Data;
  STITCH_DATA          *StitchData;

  // Stitching process might pass some plafform specific data.
  Data       = *(UINT32 *)0xFFFFFFF4;
  StitchData = (STITCH_DATA *)&Data;
  if (StitchData->Marker != 0xAA) {
    // No data, set default debug port to 2
    // PlatformID will be deferred to be detected
    SetDebugPort (2);
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
  PLATFORM_DATA   *PlatformData;

  switch (InitPhase) {
  case PostTempRamInit:
    DisableWatchDogTimer ();
    EarlyPlatformDataCheck ();
    PlatformHookSerialPortInitialize ();
    SerialPortInitialize ();
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if (PlatformData != NULL) {
      FetchPreRBPData (& (PlatformData->BtGuardInfo));
    }
    break;
  default:
    break;
  }
}

/**
  Get size of Platform Specific Data.

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
