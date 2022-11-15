/** @file
  Header file for TCO timer library implementation.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef TCO_TIMER_LIB_H_
#define TCO_TIMER_LIB_H_

/**
  Program/enable TCO base address
**/
VOID
EFIAPI
InitTcoBaseAddress (
  VOID
  );

/**
  Stop the TCO timer
**/
VOID
EFIAPI
StopTcoTimer (
  VOID
  );

/**
  Initialize TCO base address and stop the TCO timer

  @param[in] Timeout    Number of 0.6s ticks to wait
**/
VOID
EFIAPI
InitTcoTimer (
  VOID
  );

/**
  Start a countdown from a value on the TCO timer

  @param[in] Timeout    Number of 0.6s ticks to wait
**/
VOID
EFIAPI
StartTcoTimer (
  IN UINT16 Timeout
  );

/**
  Check if current boot was caused by TCO timeout

  @return TRUE if current boot was caused by TCO timeout
  @return FALSE if current boot was not caused by TCO timeout
**/
BOOLEAN
EFIAPI
WasBootCausedByTcoTimeout (
  VOID
  );

/**
  Clear TCO status
**/
VOID
EFIAPI
ClearTcoStatus (
  VOID
  );

#endif
