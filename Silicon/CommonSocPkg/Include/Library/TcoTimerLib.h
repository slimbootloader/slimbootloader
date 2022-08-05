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
  Initialize TCO base address, set TCO timeout, and stop the TCO timer

  @param[in] Timeout    Number of 0.6s ticks to wait
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
  Reload the TCO timer with its timeout
**/
VOID
EFIAPI
StartTcoTimer (
  VOID
  );

/**
  Check if TCO status indicates failure on previous boot

  @return TRUE if twice the timeout exceeded on previous boot
  @return FALSE if twice the timeout not exceeded on previous boot
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

#endif
