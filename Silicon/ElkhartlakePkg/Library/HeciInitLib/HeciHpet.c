/** @file
  Definitions for HECI driver

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/IoLib.h>
#include <PlatformBase.h>
#include "HeciHpet.h"
#include <Register/HeciRegs.h>
#include "HeciCore.h"

/**
  Used for calculating timeouts

  @param[in] Start                Snapshot of the HPET timer
  @param[in] End                  Calculated time when timeout period will be done
  @param[in] Time                 Timeout period in microseconds

**/
VOID
StartTimer (
  OUT UINT32 *Start,
  OUT UINT32 *End,
  IN  UINT32 Time
  )
{
  UINT32 HpetTimer;

  ///
  /// Get the High Precision Event Timer base address
  ///

  HpetTimer = HPET_BASE_ADDRESS;

  ///
  /// Start the timer so it is up and running
  ///
  MmioOr32 ((UINTN)HpetTimer + HPET_GEN_CONFIG, HPET_START);

  ///
  /// Read current timer value into start time from HPET
  ///
  *Start = MmioRead32 ((UINTN)HpetTimer + HPET_MAIN_COUNTER);

  ///
  /// Convert microseconds into 41.6 ns timer ticks and compute end time
  ///
  *End = *Start + Time * HPET_TICKS_PER_MICRO;
}


/**
  Used to determine if a timeout has occured.

  @param[in] Start                Snapshot of the HPET timer when the timeout period started.
  @param[in] End                  Calculated time when timeout period will be done.
  @param[in] HpetTimer            The value of High Precision Event Timer

  @retval TRUE                    Timeout occured.
  @retval FALSE                   Timeout has not occured yet.
**/
BOOLEAN
IsHeciTimeout (
  IN UINT32 Start,
  IN UINT32 End
  )
{
  UINT32 Current;
  UINT32 HpetTimer;

  ///
  /// Get the High Precision Event Timer base address
  ///
  HpetTimer = HPET_BASE_ADDRESS;

  ///
  /// Read HPET and assign the value as the current time.
  ///
  Current = MmioRead32 ((UINTN)HpetTimer + HPET_MAIN_COUNTER);

  ///
  /// Test basic case (no overflow)
  ///
  if ((Start < End) && (End <= Current)) {
    return TRUE;
  }
  ///
  /// Test basic start/end conditions with overflowed timer
  ///
  if ((Start < End) && (Current < Start)) {
    return TRUE;
  }
  ///
  /// Test for overflowed start/end condition
  ///
  if ((Start > End) && ((Current < Start) && (Current > End))) {
    return TRUE;
  }
  ///
  /// Catch corner case of broken arguments
  ///
  if (Start == End) {
    return TRUE;
  }
  ///
  /// Else, we have not yet timed out
  ///
  return FALSE;
}

