/** @file
  Definitions for HECI driver

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _HECI_HPET_H
#define _HECI_HPET_H

/**
  Enable Hpet function.

  @retval UINT32                  Return the High Precision Event Timer base address

**/
volatile
UINT32 *
EnableHpet (
  VOID
  );

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
  );

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
  );

#endif // _HECI_HPET_H
