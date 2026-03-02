/** @file
The header file for firmware resiliency definitions.

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FIRMWARE_RESILIENCY_LIB_H_
#define _FIRMWARE_RESILIENCY_LIB_H_

/**
  Retrieve FW update state from the reserved region

  @retval StateMachine The current FWU state machine
**/
UINT8
EFIAPI
GetFwuStateMachine (
  VOID
  );

/**
  Check if ACM detected corruption in IBB
**/
VOID
EFIAPI
CheckForAcmFailures (
  VOID
  );

/**
  Check if TCO timer detected corruption in OBB or a dead loop/crash in IBB/OBB

    @param[in] BootFailureThreshold The number of boots to attempt before recovery
**/
VOID
EFIAPI
CheckForTcoTimerFailures (
  IN UINT8 BootFailureThreshold
  );

/**
  Check for ME firmware code corruption.

  Reads HFSTS1/HFSTS2 registers and detects ME code corruption.
  If corruption detected, stores info in LdrGlobal and sets recovery mode.

  @param[in,out] LdrGlobal   Pointer to loader global data
**/
VOID
EFIAPI
CheckForMeCodeFailures (
  IN OUT VOID  *LdrGlobal
  );

#endif
