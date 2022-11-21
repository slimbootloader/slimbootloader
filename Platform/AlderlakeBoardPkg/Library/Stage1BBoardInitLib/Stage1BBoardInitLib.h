/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
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
#include <Guid/DeviceTableHobGuid.h>
#include <FspmUpd.h>
#include <PchReservedResources.h>
#include <Register/PchRegs.h>
#include <Register/PchBdfAssignment.h>
#include <Register/SpiRegs.h>
#include <PlatformBoardId.h>
#include <Include/PlatformData.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataCommonStruct.h>
#include <Library/ResetSystemLib.h>
#include <Register/PchRegsPmc.h>
#include <Register/RtcRegs.h>
#include <Register/PchRegsPcr.h>
#include <PlatformBase.h>
#include <ConfigDataDefs.h>
#include <Library/TpmLib.h>
#include <Library/BootGuardLib.h>
#include <Library/MeChipsetLib.h>
#include <Library/TimerLib.h>
#include <Include/TccConfigSubRegions.h>
#include <Library/PchInfoLib.h>
#include <Library/VariableLib.h>
#include <Library/GpioLib.h>
#include <Library/TopSwapLib.h>
#include "GpioTableAdlSPreMem.h"
#include "GpioTableAdlPPreMem.h"
#include "GpioTableAdlNPreMem.h"
#include "GpioTableAdlPsPreMem.h"
#include "GpioTableTestPreMem.h"
#include <Library/WatchDogTimerLib.h>
#include <Include/CpuRegs.h>
#include <Register/Cpuid.h>
#include <Library/SocInitLib.h>
#include <Library/SmbusLib.h>
#include <Library/CrashLogLib.h>

/**
  Initialize Variable.

**/
VOID
VariableInitialize (
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

#endif // __STAGE1B_BOARD_INIT_LIB_H__
