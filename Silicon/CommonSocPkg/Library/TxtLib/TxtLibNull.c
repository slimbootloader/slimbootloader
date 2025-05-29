/** @file
  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

/**
  Determines whether or not the platform requires initialization for TXT use.

  @param[in] TxtLibCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the the platoform should be configured for TXT.
  @retval FALSE         - If TXT is not to be used.
**/
BOOLEAN
EFIAPI
IsTxtEnabled (
  VOID
  )
{
  return FALSE;
}

/**
  Initializes Intel TXT

  @retval EFI_SUCCESS   - TXT initialization completed successfully
**/
EFI_STATUS
EFIAPI
InitTxt(
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Restores Intel TXT device memory registers (HEAP and SINIT) for S3 resume.
  This function restores the TXT register state without touching actual memory
  content, which must be preserved across S3 for TBOOT/MLE operation.

  @retval EFI_SUCCESS     - TXT registers restored successfully
  @retval EFI_UNSUPPORTED - Required TXT information not available
  @retval Other           - Error during initialization
**/
EFI_STATUS
EFIAPI
TxtS3Restore(
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Initializes Intel TXT for S3 resume by launching BIOS ACM.
  This function finds the BIOS ACM and launches it with SCHECK function
  to complete TXT initialization after S3 resume.

  @retval EFI_SUCCESS   - BIOS ACM launched successfully for S3 resume
**/
EFI_STATUS
EFIAPI
TxtS3Resume(
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Disable CR4.SMXE on all APs when TXT is enabled at ReadyToBoot.
  This ensures SMX is disabled on all processors before OS boot.

  Null implementation for platforms without TXT support.

  @retval EFI_SUCCESS     CR4.SMXE disabled on all APs successfully.
**/
EFI_STATUS
EFIAPI
DisableSmxOnAllAPs (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Disable CR4.SMXE bit on current processor.
  This function clears the SMX enable bit on the BSP.

  Null implementation for platforms without TXT support.
**/
VOID
EFIAPI
DisableCR4Smx (
  VOID
  )
{
  // Null implementation - nothing to do when TXT is disabled
}
