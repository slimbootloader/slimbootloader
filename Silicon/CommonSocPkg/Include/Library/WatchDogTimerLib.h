/** @file
  Header file for watch dog timer library implementation.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef WATCH_DOG_TIMBER_LIB_H_
#define WATCH_DOG_TIMBER_LIB_H_

#define  WDT_TIMEOUT_TCC_DSO           200    // 200 seconds
#define  WDT_FLAG_TCC_DSO_IN_PROGRESS  BIT17

/**
  Reloads WDT with new timeout value and starts it.

  @param[in] TimeoutValue         Time in seconds before WDT times out. Supported range = 1 - 1024.
  @param[in] Flags                WDT timer flag that indicate the purpose to start the WDT timer.

  @retval EFI_SUCCESS             if everything's OK
  @retval EFI_INVALID_PARAMETER   if TimeoutValue parameter is wrong
**/
EFI_STATUS
EFIAPI
WdtReloadAndStart (
  IN  UINT32  TimeoutValue,
  IN  UINT32  Flags
  );


/**
  Disables WDT timer and cleared the specified WDT timer flag.

  @param[in] Flags                The timer flags will be cleared when disabling the WDT

**/
VOID
EFIAPI
WdtDisable (
  IN  UINT32  Flags
  );


/**
  Clear WDT flags in scratchpad

  @param[in] Flags             The scratchpad flags that would be cleared.

**/
VOID
EFIAPI
WdtClearScratchpad (
  IN  UINT32  Flags
  );

/**
  Set WDT flags in scratchpad

  @param[in] Flags             The scratchpad flags that would be set.

**/
VOID
EFIAPI
WdtSetScratchpad (
  IN  UINT32  Flags
  );

/**
  Get WDT flags in scratchpad

  @param[in] Flags             The scratchpad flags to get.
  @retval UINT32               The flags specified.
**/
UINT32
EFIAPI
WdtGetScratchpad (
  IN  UINT32  Flags
  );

/**
  Returns if the previous reset is triggered by timer expiration.

  @retval TRUE                    Timer time out
  @retval FALSE                   Timer no time out
**/
UINT8
EFIAPI
IsWdtTimeout (
  VOID
  );

/**
  Returns if specified timer flag is set.

  @param[in] Flags                The timer flags that would be checked.

  @retval TRUE                    if specified timer flags is set
  @retval FALSE                   if specified timer flags is not set
**/
UINT8
EFIAPI
IsWdtFlagsSet (
  IN  UINT32  Flags
  );


/**
  Returns WDT enabled/disabled status.

  @retval TRUE                    if WDT is enabled
  @retval FALSE                   if WDT is disabled
**/
UINT8
EFIAPI
IsWdtEnabled (
  VOID
  );


/**
  Returns WDT locked status.

  @retval TRUE                    if WDT is locked
  @retval FALSE                   if WDT is unlocked
**/
UINT8
EFIAPI
IsWdtLocked (
  VOID
  );

/**
  Get the number of failed boots.

  @retval UINT32              the number of boots

**/
UINT32
EFIAPI
GetFailedBootCount (
  VOID
  );

/**
  Increment the number of failed boots.
**/
VOID
EFIAPI
IncrementFailedBootCount (
  VOID
  );

/**
  Set the number of failed boots to 0.
**/
VOID
EFIAPI
ClearFailedBootCount (
  VOID
  );

/**
  Check if FW update triggered.
**/
BOOLEAN
EFIAPI
IsUpdateTriggered (
  VOID
  );

/**
  Clear FW update trigger.
**/
VOID
EFIAPI
ClearUpdateTrigger (
  VOID
  );

/**
  Check if top swap triggered.
**/
BOOLEAN
EFIAPI
IsTopSwapTriggered (
  VOID
  );

/**
  Clear top swap trigger.
**/
VOID
EFIAPI
ClearTopSwapTrigger (
  VOID
  );

/**
  Set top swap trigger.
**/
VOID
EFIAPI
SetTopSwapTrigger (
  VOID
  );

/**
  Check if FW recovery triggered.
**/
BOOLEAN
EFIAPI
IsRecoveryTriggered (
  VOID
  );

/**
  Clear FW recovery trigger.
**/
VOID
EFIAPI
ClearRecoveryTrigger (
  VOID
  );

/**
  Set FW recovery trigger.
**/
VOID
EFIAPI
SetRecoveryTrigger (
  VOID
  );

#endif
