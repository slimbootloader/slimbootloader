/** @file
  Header file for Boot Guard Lib implementation

  Copyright (c) 2012-2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


@par Specification Reference:

**/

#ifndef _BOOT_GUARD_LIB_H_
#define _BOOT_GUARD_LIB_H_

///
/// The TPM category, TPM 1.2, TPM 2.0 and PTT are defined.
///
typedef enum {
  TpmNone = 0, ///< 0: No TPM device present on system
  dTpm12,      ///< 1: TPM 1.2 device present on system
  dTpm20,      ///< 2: TPM 2.0 device present on system
  Ptt,         ///< 3: PTT present on system
  TpmTypeMax   ///< 4: Unknown device
} TPM_TYPE;

///
/// Information related to Boot Guard Configuration.
///
typedef struct {
  BOOLEAN  VerifiedBoot;        ///< Report VerifiedBoot Boot setting in Boot Guard profile. 0: Disable; 1: Enable.
  BOOLEAN  MeasuredBoot;        ///< Report Measured Boot setting in Boot Guard profile. 0: Disable; 1: Enable.
  /**
  ByPassTpmInit is set to 1 if Boot Guard ACM does TPM initialization successfully.
  - 0: No TPM initialization happen in Boot Guard ACM.
  - 1: TPM initialization is done by Boot Guard ACM.
  **/
  BOOLEAN  BypassTpmInit;
  TPM_TYPE TpmType;             ///< Report what TPM device is available on system.
  BOOLEAN  BootGuardCapability; ///< Value is set to 1 if chipset is Boot Guard capable.
  /**
  Value is set to 1 if microcode failed to load Boot Guard ACM or ENF Shutdown path is taken by ME FW.
  - 0: BIOS TPM code continue with TPM initization based on MeasuredBoot.
  - 1: BIOS TPM code is not to do any futher TPM initization and extends.
  **/
  BOOLEAN  DisconnectAllTpms;
  /**
  It is indicated BIOS TPM code is not to create DetailPCR or AuthorityPCR event log if Sx resume type is S3,
  Deep-S3, or iFFS Resume.
  - 0: Create TPM event log if not Sx Resume Type.
  - 1: Bypass TPM Event Log if Sx Resume Type is identified.
  **/
  BOOLEAN  ByPassTpmEventLog;
  /**
  This field indicates that the ACM's Tpm2Startup (State) command failed during S3 resume.
  - 0: Successful Tpm2Startup (State)
  - 1: Failure during Tpm2Startup (State). BIOS will need to perform a cold reset to handle the error.
  **/
  BOOLEAN  TpmStartupFailureOnS3;
} BOOT_GUARD_INFO;

/**
  Determine if Boot Guard is supported

  @retval TRUE  - Processor is Boot Guard capable.
  @retval FALSE - Processor is not Boot Guard capable.

**/
BOOLEAN
EFIAPI
IsBootGuardSupported (
  VOID
  );

/**
  Report platform specific  Boot Guard information.

  @param[out] *BootGuardInfo - Pointer to BootGuardInfo.
**/
VOID
EFIAPI
GetBootGuardInfo (
  IN  UINTN            HeciBaseAddress,
  OUT BOOT_GUARD_INFO *BootGuardInfo
  );


/**
  Create Boot Guard TPM event log

  @param[in] TpmType - Which type of TPM is available on system.
**/
VOID
CreateTpmEventLog (
  IN TPM_TYPE TpmType
  );
#endif
