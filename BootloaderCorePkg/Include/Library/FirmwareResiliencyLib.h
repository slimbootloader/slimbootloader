/** @file
The header file for firmware resiliency definitions.

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FIRMWARE_RESILIENCY_LIB_H_
#define _FIRMWARE_RESILIENCY_LIB_H_

/**
  Retrieve FW update state from the reserved region

  @param[in,out] StateMachine The current FW update state
**/
VOID
EFIAPI
GetStateMachine (
  IN OUT UINT8* StateMachine
  );

/**
  Check if ACM detected corruption in IBB

  @param[in] StateMachine The current FW update state
**/
VOID
EFIAPI
CheckForAcmFailures (
  IN UINT8 StateMachine
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

#endif
