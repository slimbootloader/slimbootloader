/** @file
  Definitions for HECI driver

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _HECI_HPET_H
#define _HECI_HPET_H

//
// HPET Information
//
#define HPET_GEN_CONFIG      0x10
#define HPET_MAIN_COUNTER    0xF0

#define HPET_START           0x1
#define HPET_TICKS_PER_MICRO 24  ///< 41.6 ns tick so 24 ticks per microsecond ish

/**
  Used for calculating timeouts

  @param[in] Start                Snapshot of the HPET timer
  @param[in] End                  Calculated time when timeout period will be done
  @param[in] Time                 Timeout period in microseconds

**/
VOID
StartTimer (
  OUT UINT32                      *Start,
  OUT UINT32                      *End,
  IN  UINT32                      Time
  );

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
  IN  UINT32                      Start,
  IN  UINT32                      End
  );

#endif // _HECI_HPET_H
