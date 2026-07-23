/** @file
  Definitions for the unified SBL resiliency recovery status.

  The RECOVERY_STATUS structure is stored as an SBL Variable (via
  LiteVariableLib in the FLASH_MAP_SIG_VARIABLE region) and is the single
  source of truth for the resiliency recovery flow. It records the recovery
  reason (a bitmask of failure sources), the number of recovery attempts and
  the result of the last attempt. It also holds the consecutive TCO timeout
  counter that previously lived in the WDT scratchpad (BIT19:18).

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _RECOVERY_STATUS_H_
#define _RECOVERY_STATUS_H_

//
// SBL Variable name used to persist the recovery status.
//
#define RECOVERY_STATUS_VARIABLE_NAME   L"RecoveryStatus"

//
// Structure revision.
//
#define RECOVERY_STATUS_REVISION        1

//
// Recovery reason bitmask. Each bit represents an independent failure source.
// Multiple bits can be set simultaneously for compound failures.
//
#define RECOVERY_REASON_NONE            0x00
#define RECOVERY_REASON_SBL             0x01    // TCO timeout detected SBL failure
#define RECOVERY_REASON_CSME_WDT        0x02    // CSME triggered Top Swap (WDT expiry) due to failure outside SBL
#define RECOVERY_REASON_CSME            0x04    // CSME firmware failure (HFSTS) - to be implemented
#define RECOVERY_REASON_IOE             0x08    // IOE CSME firmware failure - to be implemented
// BIT4..BIT7 reserved for future failure sources

//
// LastResult sentinel values.
// EFI_SUCCESS == 0, so a distinct "not yet attempted" value is required so the
// post-recovery cleanup logic does not falsely delete the variable before a
// recovery attempt has actually run.
//
#define RECOVERY_RESULT_PENDING         0xFF    // Recovery not yet attempted
#define RECOVERY_RESULT_SUCCESS         0x00    // Recovery completed (== EFI_SUCCESS)

#pragma pack(push, 1)
typedef struct {
  UINT8   Revision;         // RECOVERY_STATUS_REVISION
  UINT8   Reason;           // Bitmask of RECOVERY_REASON_*
  UINT8   AttemptCount;     // Number of recovery attempts for current reason(s)
  UINT8   LastResult;       // RECOVERY_RESULT_PENDING initially; payload writes EFI_STATUS low byte
  UINT8   FailedBootCount;  // Consecutive TCO timeout count (replaces WDT BIT19:18)
  UINT8   Reserved[3];      // Alignment pad: keeps variable DataSize 4-byte aligned for LiteVariableLib
} RECOVERY_STATUS;
#pragma pack(pop)

extern EFI_GUID gRecoveryStatusVariableGuid;

#endif // _RECOVERY_STATUS_H_