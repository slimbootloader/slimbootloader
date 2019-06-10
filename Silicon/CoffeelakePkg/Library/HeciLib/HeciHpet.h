/** @file
  Definitions for HECI driver

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
