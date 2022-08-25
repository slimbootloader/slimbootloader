/** @file
  Library for watch dog timer.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/WatchDogTimerLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#define R_ACPI_IO_OC_WDT_CTL                              0x54
#define B_ACPI_IO_OC_WDT_CTL_RLD                          BIT31
#define B_ACPI_IO_OC_WDT_CTL_ICCSURV_STS                  BIT25
#define B_ACPI_IO_OC_WDT_CTL_NO_ICCSURV_STS               BIT24
#define B_ACPI_IO_OC_WDT_CTL_TOP_SWAP_TRIGGER             BIT21
#define B_ACPI_IO_OC_WDT_CTL_RECOVERY_TRIGGER             BIT20
#define B_ACPI_IO_OC_WDT_CTL_UPDATE_TRIGGER               BIT16
#define B_ACPI_IO_OC_WDT_CTL_FORCE_ALL                    BIT15
#define B_ACPI_IO_OC_WDT_CTL_EN                           BIT14
#define B_ACPI_IO_OC_WDT_CTL_ICCSURV                      BIT13
#define B_ACPI_IO_OC_WDT_CTL_LCK                          BIT12
#define B_ACPI_IO_OC_WDT_CTL_TOV_MASK                     0x3FF
#define B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_MASK              0xFF0000
#define B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_BOOT_CNT_MASK     0xC0000
#define B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_BOOT_CNT_SHIFT    0x12


/**
  Get Watchdog Timer address

  @retval UINT32                  Watchdog's address
**/
UINT32
WdtGetAddress (
  VOID
  )
{
  return (PcdGet16 (PcdAcpiPmTimerBase) & 0xFF00) + R_ACPI_IO_OC_WDT_CTL;
}

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
  )
{
  UINT32  Readback;

  DEBUG ((DEBUG_INFO, "\n(Wdt) ReloadAndStartTimer(%d)\n", TimeoutValue));

  if ((TimeoutValue > B_ACPI_IO_OC_WDT_CTL_TOV_MASK) || (TimeoutValue == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Readback = IoRead32 (WdtGetAddress ());
  Readback |= (B_ACPI_IO_OC_WDT_CTL_EN | B_ACPI_IO_OC_WDT_CTL_ICCSURV |
               (Flags & B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_MASK));

  Readback &= ~(B_ACPI_IO_OC_WDT_CTL_TOV_MASK);
  Readback |= ((TimeoutValue - 1) & B_ACPI_IO_OC_WDT_CTL_TOV_MASK);
  IoWrite32 (WdtGetAddress (), Readback);
  Readback |= B_ACPI_IO_OC_WDT_CTL_RLD;
  IoWrite32 (WdtGetAddress (), Readback);

  return EFI_SUCCESS;
}

/**
  Disables WDT timer and cleared the specified WDT timer flag.

  @param[in] Flags                The timer flags will be cleared when disabling the WDT

**/
VOID
EFIAPI
WdtDisable (
  IN  UINT32  Flags
  )
{
  UINT32  Readback;

  Readback  = IoRead32 (WdtGetAddress ());
  Readback &= ~(B_ACPI_IO_OC_WDT_CTL_EN | (Flags & B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_MASK));

  // Clear the status bits
  Readback |= (B_ACPI_IO_OC_WDT_CTL_ICCSURV_STS | B_ACPI_IO_OC_WDT_CTL_NO_ICCSURV_STS);

  IoWrite32 (WdtGetAddress (), Readback);
}


/**
  Clear WDT flags in scratchpad

  @param[in] Flags             The scratchpad flags that would be cleared.

**/
VOID
EFIAPI
WdtClearScratchpad (
  IN  UINT32  Flags
  )
{
  UINT32  Readback;

  Readback  = IoRead32 (WdtGetAddress ());
  /* only clear flags in scratchpad */
  Readback &= ~(Flags & B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_MASK);
  /* exclude status fields */
  Readback &= ~(B_ACPI_IO_OC_WDT_CTL_ICCSURV_STS | B_ACPI_IO_OC_WDT_CTL_NO_ICCSURV_STS);
  IoWrite32 (WdtGetAddress (), Readback);
}

/**
  Set WDT flags in scratchpad

  @param[in] Flags             The scratchpad flags that would be set.

**/
VOID
EFIAPI
WdtSetScratchpad (
  IN  UINT32  Flags
  )
{
  UINT32  Readback;

  Readback  = IoRead32 (WdtGetAddress ());
  /* only set flags in scratchpad */
  Readback |= (Flags & B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_MASK);
  /* exclude status fields */
  Readback &= ~(B_ACPI_IO_OC_WDT_CTL_ICCSURV_STS | B_ACPI_IO_OC_WDT_CTL_NO_ICCSURV_STS);
  IoWrite32 (WdtGetAddress (), Readback);
}

/**
  Get WDT flags in scratchpad

  @param[in] Flags             The scratchpad flags to get.
  @retval UINT32               The flags specified.
**/
UINT32
EFIAPI
WdtGetScratchpad (
  IN  UINT32  Flags
  )
{
  UINT32  Readback;

  Readback  = IoRead32 (WdtGetAddress ());
  /* only get flagged bits from scratchpad */
  Readback &= Flags & B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_MASK;
  return Readback;
}

/**
  Returns if the previous reset is triggered by timer expiration.

  @retval TRUE                    Timer time out
  @retval FALSE                   Timer no time out
**/
UINT8
EFIAPI
IsWdtTimeout (
  VOID
  )
{
  UINT32  Readback;

  Readback = IoRead32 (WdtGetAddress ());

  if ((Readback & (B_ACPI_IO_OC_WDT_CTL_ICCSURV_STS | B_ACPI_IO_OC_WDT_CTL_NO_ICCSURV_STS)) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}



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
  )
{
  UINT32  Readback;

  Readback = IoRead32 (WdtGetAddress ());

  if ((Readback & Flags) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Returns WDT enabled/disabled status.

  @retval TRUE                    if WDT is enabled
  @retval FALSE                   if WDT is disabled
**/
UINT8
EFIAPI
IsWdtEnabled (
  VOID
  )
{
  UINT32  Readback;

  Readback = IoRead32 (WdtGetAddress ());
  if ((Readback & B_ACPI_IO_OC_WDT_CTL_EN) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Returns WDT locked status.

  @retval TRUE                    if WDT is locked
  @retval FALSE                   if WDT is unlocked
**/
UINT8
EFIAPI
IsWdtLocked (
  VOID
  )
{
  UINT32  Readback;

  Readback = IoRead32 (WdtGetAddress ());
  if ((Readback & B_ACPI_IO_OC_WDT_CTL_LCK) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Get the number of failed boots.

  @retval UINT32              the number of boots

**/
UINT32
EFIAPI
GetFailedBootCount (
  VOID
  )
{
  return WdtGetScratchpad (B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_BOOT_CNT_MASK) >>
                            B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_BOOT_CNT_SHIFT;
}

/**
  Increment the number of failed boots.
**/
VOID
EFIAPI
IncrementFailedBootCount (
  VOID
  )
{
  UINT32 FailedBootCnt;

  // Get current failed boot count
  FailedBootCnt = GetFailedBootCount ();

  // Clear current failed boot count
  ClearFailedBootCount ();

  // Calculate new failed boot count
  FailedBootCnt += 1;

  // Set bits in WDT scratchpad per new count
  WdtSetScratchpad ((FailedBootCnt << B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_BOOT_CNT_SHIFT)
                      & B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_BOOT_CNT_MASK);
}

/**
  Set the number of failed boots to 0.
**/
VOID
EFIAPI
ClearFailedBootCount (
  VOID
  )
{
  WdtClearScratchpad (B_ACPI_IO_OC_WDT_CTL_SCRATCHPAD_BOOT_CNT_MASK);
}

/**
  Check if FW update triggered.
**/
BOOLEAN
EFIAPI
IsUpdateTriggered (
  VOID
  )
{
  return WdtGetScratchpad (B_ACPI_IO_OC_WDT_CTL_UPDATE_TRIGGER) != 0;
}

/**
  Clear FW update trigger.
**/
VOID
EFIAPI
ClearUpdateTrigger (
  VOID
  )
{
  WdtClearScratchpad (B_ACPI_IO_OC_WDT_CTL_UPDATE_TRIGGER);
}

/**
  Check if top swap triggered.
**/
BOOLEAN
EFIAPI
IsTopSwapTriggered (
  VOID
  )
{
  return WdtGetScratchpad (B_ACPI_IO_OC_WDT_CTL_TOP_SWAP_TRIGGER) != 0;
}

/**
  Clear top swap trigger.
**/
VOID
EFIAPI
ClearTopSwapTrigger (
  VOID
  )
{
  WdtClearScratchpad (B_ACPI_IO_OC_WDT_CTL_TOP_SWAP_TRIGGER);
}

/**
  Set top swap trigger.
**/
VOID
EFIAPI
SetTopSwapTrigger (
  VOID
  )
{
  WdtSetScratchpad (B_ACPI_IO_OC_WDT_CTL_TOP_SWAP_TRIGGER);
}

/**
  Check if FW recovery triggered.
**/
BOOLEAN
EFIAPI
IsRecoveryTriggered (
  VOID
  )
{
  return WdtGetScratchpad (B_ACPI_IO_OC_WDT_CTL_RECOVERY_TRIGGER) != 0;
}

/**
  Clear FW recovery trigger.
**/
VOID
EFIAPI
ClearRecoveryTrigger (
  VOID
  )
{
  WdtClearScratchpad (B_ACPI_IO_OC_WDT_CTL_RECOVERY_TRIGGER);
}

/**
  Set FW recovery trigger.
**/
VOID
EFIAPI
SetRecoveryTrigger (
  VOID
  )
{
  WdtSetScratchpad (B_ACPI_IO_OC_WDT_CTL_RECOVERY_TRIGGER);
}
