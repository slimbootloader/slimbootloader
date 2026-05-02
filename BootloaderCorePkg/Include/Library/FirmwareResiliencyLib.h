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

/** Checks HFSTS1/2 via HECI PCI config space in Stage1B.
    Triggers CSME capsule FWU (BIT16 + cold reset) on first corruption detection,
    or boots degraded if FWU already completed. **/
VOID
EFIAPI
CheckForMeCodeFailures (
  VOID
  );

/** Returns TRUE if HFSTS1/2 via HECI PCI config space indicate ME recovery/FtBupLdFlr/FwUpdIpu; FALSE if healthy or not present. **/
BOOLEAN
EFIAPI
IsMeCorrupt (
  VOID
  );

/** Reads HFSTS1 and HFSTS2 from HECI PCI config space.
    Both outputs are zeroed if HECI device is not present. **/
VOID
EFIAPI
GetMeHfsts (
  OUT UINT32  *Hfsts1,
  OUT UINT32  *Hfsts2
  );


#endif
