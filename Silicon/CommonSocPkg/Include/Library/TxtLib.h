/** @file
  Copyright (c) 2025 - 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _TXT_LIB_H_
#define _TXT_LIB_H_

#define R_IOPORT_CMOS_STANDARD_INDEX            0x70
#define R_IOPORT_CMOS_STANDARD_DATA             0x71
#define TXT_CMOS_STATUS_REG                     0x2A

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
  Disable CR4.SMXE bit on current processor.
  This function clears the SMX enable bit on the BSP.
**/
VOID
EFIAPI
DisableCR4Smx (
  VOID
  );

#endif
