/** @file
  Copyright (c) 2025 - 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _TXT_LIB_H_
#define _TXT_LIB_H_

#define R_IOPORT_CMOS_STANDARD_INDEX            0x70
#define R_IOPORT_CMOS_STANDARD_DATA             0x71
#define TXT_CMOS_STATUS_REG                     0x2A

typedef struct _TXT_LIB_CONTEXT_ TXT_LIB_CONTEXT;

/**
  Determines whether or not the platform requires initialization for TXT use.

  @retval TRUE          - If the the platoform should be configured for TXT.
  @retval FALSE         - If TXT is not to be used.
**/
BOOLEAN
EFIAPI
IsTxtEnabled ();

/*
  Initialize Intel TXT
*/
EFI_STATUS
EFIAPI
InitTxt();

/**
  Initializes Intel TXT for S3 resume by launching BIOS ACM.
  This function finds the BIOS ACM and launches it with SCHECK function
  to complete TXT initialization after S3 resume.

  @retval EFI_SUCCESS   - BIOS ACM launched successfully for S3 resume
**/
EFI_STATUS
EFIAPI
TxtS3Resume();

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
TxtS3Restore();

/**
  Disable or restore possible SMI sources before or after ACM launch.

  @param[in] TxtLibCtx   A pointer to initialized TXT context data.
  @param[in] Operation   TRUE to save/disable sources, FALSE to restore.

  @retval EFI_SUCCESS    Always returns EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
DisableSmiSources (
  IN TXT_LIB_CONTEXT *TxtLibCtx,
  IN BOOLEAN         Operation
  );

/**
  Disable CR4.SMXE bit on current processor.
  This function clears the SMX enable bit on the current CPU.

**/
VOID
EFIAPI
DisableCR4Smx (
  VOID
  );

/**
  Disable CR4.SMXE on BSP when TXT is enabled before payload loading.

  @retval EFI_SUCCESS     CR4.SMXE disabled on BSP successfully.
**/
EFI_STATUS
EFIAPI
DisableSmxOnBSP (
  VOID
  );

#endif
