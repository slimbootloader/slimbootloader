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
  Unified resiliency check point.

  Single Stage1B detection point that consolidates the previously scattered
  ACM and TCO failure checks. Records the recovery reason, attempt count and
  TCO failed-boot counter in the persistent L"RecoveryStatus" SBL variable,
  ensures the WDT recovery trigger (BIT20) is set when recovery is needed, and
  switches partitions / resets when an SBL partition switch is required.

  Must be called after BoardInit(PostConfigInit) (so VariableInitialize() has
  run) and before FSP-M.

  @param[in] BootFailureThreshold  Consecutive TCO timeouts before recovery.
  @param[in] MaxRecoveryAttempts   Recovery attempts before CpuHalt().
**/
VOID
EFIAPI
UnifiedResiliencyCheck (
  IN UINT8  BootFailureThreshold,
  IN UINT8  MaxRecoveryAttempts
  );

#endif
