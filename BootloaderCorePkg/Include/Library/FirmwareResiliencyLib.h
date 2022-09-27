/** @file
The header file for firmware resiliency definitions.

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FIRMWARE_RESILIENCY_LIB_H_
#define _FIRMWARE_RESILIENCY_LIB_H_

/**
  Retrieve the current overall FW update state, if it exists

  @param[out] State The current overall FW update state
**/
VOID
EFIAPI
GetUpdateState (
   OUT UINT8* State
   );

/**
  Retrieve the current in-flight FW update state, if it exists

  @param[out] State The current in-flight FW update state
**/
VOID
EFIAPI
GetInFlightUpdateState (
  OUT UINT8* State
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

#endif
