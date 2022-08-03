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
InitTcoTimer (
  VOID
  );

/**
  Set the TCO timeout

  @param[in] Timeout    Number of 0.6s ticks to wait
**/
VOID
EFIAPI
SetTcoTimeout (
  IN UINT16 Timeout
  );

/**
  Resume (un-halt) the TCO timer
**/
VOID
EFIAPI
ResumeTcoTimer (
  VOID
  );

/**
  Reload the TCO timer with its timeout
**/
VOID
EFIAPI
RestartTcoTimer (
  VOID
  );

/**
  Check if TCO status indicates failure on previous boot

  @return TRUE if timeout exceeded on previous boot
  @return FALSE if timeout not exceeded on previous boot
**/
BOOLEAN
EFIAPI
WasPreviousTcoTimeout (
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

/**
  Halt the TCO timer
**/
VOID
EFIAPI
HaltTcoTimer (
  VOID
  );

/**
  Initialize TCO timer, set TCO timeout, and restart TCO timer

  @param[in] Timeout    Number of 0.6s ticks to wait
**/
VOID
EFIAPI
EnableTcoTimer (
  IN UINT16 Timeout
  );

/**
  Initialize and halt TCO timer
**/
VOID
EFIAPI
DisableTcoTimer (
  VOID
  );

#endif
