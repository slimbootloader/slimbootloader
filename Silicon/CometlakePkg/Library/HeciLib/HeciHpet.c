/** @file
  Definitions for HECI driver

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiPei.h>
#include <Library/IoLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <PlatformBase.h>
#include "HeciHpet.h"
#include "HeciCore.h"

GLOBAL_REMOVE_IF_UNREFERENCED UINT32    mHpetBaseAddr = 0;

/**
  Get HPET base address.

  @param[out] HpetBase                  Buffer of HPET base address

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchHpetBaseGet (
  OUT UINT32                            *HpetBase
  )
{
  if (HpetBase == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *HpetBase = HPET_BASE_ADDRESS;
  return EFI_SUCCESS;
}

/**
  Enable Hpet function.

  @retval UINT32                  Return the High Precision Event Timer base address

**/
volatile
UINT32 *
EnableHpet (
  VOID
  )
{
  UINT32            HpetTimer;
  UINT32            *HpetTimerPtr;

  HpetTimer = 0;

  ///
  /// Get the High Precision Event Timer base address and enable the memory range
  ///
  if (mHpetBaseAddr == 0) {
    PchHpetBaseGet (&HpetTimer);
    mHpetBaseAddr = HpetTimer;
  } else {
    HpetTimer = mHpetBaseAddr;
  }

  HpetTimerPtr = (UINT32*) (UINTN) HpetTimer;

  ///
  /// Start the timer so it is up and running
  ///
  HpetTimerPtr[HPET_GEN_CONFIG_LOW] = HPET_START;

  return HpetTimerPtr;
}

/**
  Used for calculating timeouts

  @param[in] Start                Snapshot of the HPET timer
  @param[in] End                  Calculated time when timeout period will be done
  @param[in] Time                 Timeout period in microseconds

**/
volatile
UINT32 *
StartTimer (
  OUT UINT32                      *Start,
  OUT UINT32                      *End,
  IN  UINT32                      Time
  )
{
  UINT32          Ticks;
  volatile UINT32 *HpetTimer;
  ///
  /// Make sure that HPET is enabled and running
  ///
  HpetTimer = EnableHpet ();

  ///
  /// Read current timer value into start time from HPET
  ///
  *Start = HpetTimer[HPET_MAIN_COUNTER_LOW];

  ///
  /// Convert microseconds into 41.6 ns timer ticks
  ///
  Ticks = Time * HPET_TICKS_PER_MICRO;

  ///
  /// Compute end time
  ///
  *End = *Start + Ticks;

  return HpetTimer;
}

/**
  Used to determine if a timeout has occured.

  @param[in] Start                Snapshot of the HPET timer when the timeout period started.
  @param[in] End                  Calculated time when timeout period will be done.
  @param[in] HpetTimer            The value of High Precision Event Timer

  @retval EFI_TIMEOUT             Timeout occured.
  @retval EFI_SUCCESS             Not yet timed out
**/
EFI_STATUS
Timeout (
  IN  UINT32                      Start,
  IN  UINT32                      End,
  IN  volatile UINT32             *HpetTimer
  )
{
  UINT32  Current;

  ///
  /// Read HPET and assign the value as the current time.
  ///
  Current = HpetTimer[HPET_MAIN_COUNTER_LOW];

  ///
  /// Test basic case (no overflow)
  ///
  if ((Start < End) && (End <= Current)) {
    return EFI_TIMEOUT;
  }
  ///
  /// Test basic start/end conditions with overflowed timer
  ///
  if ((Start < End) && (Current < Start)) {
    return EFI_TIMEOUT;
  }
  ///
  /// Test for overflowed start/end condition
  ///
  if ((Start > End) && ((Current < Start) && (Current > End))) {
    return EFI_TIMEOUT;
  }
  ///
  /// Catch corner case of broken arguments
  ///
  if (Start == End) {
    return EFI_TIMEOUT;
  }
  ///
  /// Else, we have not yet timed out
  ///
  return EFI_SUCCESS;
}

