/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __STAGE1B_BOARD_INIT_LIB_H__
#define __STAGE1B_BOARD_INIT_LIB_H__

#include <PiPei.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BoardInitLib.h>
#include <Library/LoaderLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/TpmLib.h>
#include <Library/BootGuardLib.h>
#include <Library/ResetSystemLib.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <Register/PchRegsSpi.h>
#include <Register/RegsUsb.h>
#include <Register/RegsSpi.h>
#include <Register/CpuRegs.h>
#include <Register/PchRegsPmc.h>
#include <PlatformData.h>
#include <ConfigDataDefs.h>
#include <FspmUpd.h>
#include <PlatformBase.h>
#include <Register/PchRegsPcr.h>
#include <Library/HobLib.h>
#include <GpioV2PinsIbl.h>
#include <Include/GpioV2Config.h>
#include <Library/GpioV2Lib.h>
#include <Register/BdfAssignment.h>
#include <FspExtSystemMemoryMapHob.h>
#include <Register/UboxMiscRegs.h>
#include <Library/VariableLib.h>

/**

  Program Gpio Pins required for Pre Mem Init

  @param None
**/
VOID
PreMemGpioInit (
  VOID
  );

/**

    Check to see if CMOS is bad or cleared

    @param None

    @retval TRUE  - CMOS is bad
    @retval FALSE - CMOS is good

**/
BOOLEAN
IsCMOSBad(
  VOID
  );

/**
  This function checks if RTC Power Failure occurred by
  reading RTC_PWR_FLR bit

  @retval RTC Power Failure state: TRUE  - Battery is always present.
                                   FALSE - CMOS is cleared.
**/
BOOLEAN
PmcIsRtcBatteryGood (
  VOID
  );

/**
  Initialize RTC.

  @retval    None.

**/
VOID
RtcInit (
  VOID
  );

/**
  Initialize Variable.

**/
VOID
VariableInitialize (
  VOID
  );
#endif // __STAGE1B_BOARD_INIT_LIB_H__
