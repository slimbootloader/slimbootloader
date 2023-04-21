/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TPM_LIB_H_
#define _TPM_LIB_H_

#include <IndustryStandard/Tpm20.h>
#include <IndustryStandard/UefiTcgPlatform.h>
#include <IndustryStandard/Tpm2Acpi.h>
#include <Library/CryptoLib.h>
#include <Library/ContainerLib.h>
#include <Guid/BootLoaderVersionGuid.h>

#define ACPI_SSDT_TPM2_DEVICE_OEM_TABLE_ID  SIGNATURE_64('T', 'p', 'm', '2', 'T', 'a', 'b', 'l')

/**
  Extend SBL version in PCR [0].

  @param BlVersion            The current SBL version.

  @retval RETURN_SUCCESS      Operation completed successfully.
  @retval Others              Unable to extend PCR.
**/
RETURN_STATUS
EFIAPI
TpmLogCrtmVersionEvent(
  BOOT_LOADER_VERSION *BlVersion
  );

/**
  Initialize the TPM. Initiate TPM_Startup if not yet done by BootGuard component.

  @param BypassTpmInit    If TRUE, skip TPM_Startup as it is done by ACM.
  @param BootMode         Indicates platform boot mode i.e. if resuming from S3 or S2/S4/S5 boot mode.

  @retval RETURN_SUCCESS             TPM initialization completed successfully.
  @retval RETURN_DEVICE_ERROR        Unexpected device behavior.
  @retval Others                     The request could not be executed successfully.

**/
RETURN_STATUS
EFIAPI
TpmInit(
  IN BOOLEAN BypassTpmInit,
  IN UINT8  BootMode
  );

/**
  Disable TPM as per TCG PC Client Platform Firmware Profile Spec (Section 6.1) requirements.

  @retval RETURN_SUCCESS             Operation completed successfully.
  @retval Others                     The request could not be executed successfully.

**/
RETURN_STATUS
EFIAPI
DisableTpm (
  VOID
  );

/**
  Get the TPM event log info.


  @param Lasa  TPM event log buffer.
  @param Laml  TPM event log size.

  @retval RETURN_SUCCESS             Operation completed successfully.
  @retval RETURN_INVALID_PARAMETER   Invalid parameter.
  @retval RETURN_DEVICE_ERROR        Tpm Device not found or in bad state.
  @retval Others                     The request could not be executed successfully.

**/
RETURN_STATUS
EFIAPI
GetTpmEventLog (
  OUT UINT64 *Lasa,
  OUT UINT32 *Laml
  );

/**
  Update TPM ACPI table with interface and device information.
  @note If this method returns failure, TPM table should not be published.

  @param Table  TPM ACPI table which needs to be updated.

  @retval RETURN_SUCCESS             Operation completed successfully.
  @retval RETURN_INVALID_PARAMETER   Invalid parameter.
  @retval RETURN_DEVICE_ERROR        Tpm Device not found or in bad state.
  @retval Others                     The request could not be executed successfully.

**/
RETURN_STATUS
EFIAPI
UpdateTpm2AcpiTable (
  IN EFI_ACPI_DESCRIPTION_HEADER *Table
  );

/**
  This event is extended in PCR[0-7] in two scenarios.
  When WithError=1, it indicates that error occurred during TPM initialization or
  recording CRTM, POST BIOS etc.
  When WithError=0, it is used to delimit actions taken during pre-OS and OS environments.

  @param WithError                   Value used to extend in PCR[0-7].

  @retval RETURN_SUCCESS             Operation completed successfully.
  @retval Others                     Unable to extend PCR's successfully.

**/
RETURN_STATUS
EFIAPI
MeasureSeparatorEvent (
  IN   UINT32  WithError
  );


/**
Hash and Extend a PCR and log it into TCG event log.

@param[in] PcrHandle    PCR index to extend.
@param[in] Data         Data pointer.
@param[in] Length       Data Length.
@param[in] EventType    EventType to be logged in TCG Event log.
@param[in] EventSize    size of the event.
@param[in] Event        Event data.

@retval RETURN_SUCCESS      Operation completed successfully.
@retval Others              Unable to extend PCR.
**/
RETURN_STATUS
EFIAPI
TpmHashAndExtendPcrEventLog (
IN         TPMI_DH_PCR               PcrHandle,
IN         UINT8                     *Data,
IN         UINT32                    Length,
IN         TCG_EVENTTYPE             EventType,
IN         UINT32                    EventSize,
IN  CONST  UINT8                     *Event
);


/**
  Extend a PCR and log it into TCG event log.

  @param[in] PcrHandle    PCR index to extend.
  @param[in] HashAlg      Hash algorithm for Hash data.
  @param[in] Hash         Hash data to be extended.
  @param[in] EventType    EventType to be logged in TCG Event log.
  @param[in] EventSize    size of the event.
  @param[in] Event        Event data.

  @retval RETURN_SUCCESS      Operation completed successfully.
  @retval Others              Unable to extend PCR.
**/
RETURN_STATUS
EFIAPI
TpmExtendPcrAndLogEvent (
  IN         TPMI_DH_PCR               PcrHandle,
  IN         TPMI_ALG_HASH             HashAlg,
  IN  CONST  UINT8                     *Hash,
  IN         TCG_EVENTTYPE             EventType,
  IN         UINT32                    EventSize,
  IN  CONST  UINT8                     *Event
  );


/**
  Log a PCR event in TCG 2.0 format.

  @param[in] EventHdr      New event which needs to be logged.
  @param[in] EventData     New event data or description.

  @retval RETURN_SUCCESS   Operation completed successfully.
  @retval Others           Unable to log event in TCG Event log.
**/
RETURN_STATUS
EFIAPI
TpmLogEvent (
  IN  CONST  TCG_PCR_EVENT2_HDR      *EventHdr,
  IN  CONST  UINT8                   *EventData
  );


/**
  This method is used to indicate that firmware is transferring control to kernel.
  BIOS needs to performs operations(such as, measure Secure Boot Policy,
  randomize platform auth, add EV_SEPARATOR event etc) as indicated in
  PC Client Specific Firmware Profile specification.

  @param[in] FwDebugEnabled    Firmware Debug Mode enabled/disabled.

  @retval RETURN_SUCCESS   Operation completed successfully.
  @retval Others           Unable to finish handling ReadyToBoot events.
**/
RETURN_STATUS
EFIAPI
TpmIndicateReadyToBoot (
  IN UINT8   FwDebugEnabled
  );


/**
  Get Crypto Lib Hash alg required by bootloader.

  @param  TcgAlgMask    TCG Alg Mask

  @retval CryptoHashAlg Crypo Hash Alg.
**/
UINT8
EFIAPI
GetCryptoHashAlg (
  UINT32 TcgAlgMask
  );


/**
  Get Tpm Hash Algo type requested.

  @retval PcdHashType    TPM Algorithm Id.
**/
UINT32
EFIAPI
GetTpmHashAlg (
   UINT32 TcgAlgHash
  );


/**
  Function to extend Stage component hash

  @param[in]  CbInfo    Component Call Back Info

**/
VOID
EFIAPI
ExtendStageHash (
  IN  COMPONENT_CALLBACK_INFO   *CbInfo
  );

/**
  Create a TPM event log from bootloader

  @retval RETURN_SUCCESS     Operation completed successfully.
  @retval Others             Unable to create TCG event log.

**/
RETURN_STATUS
EFIAPI
CreateTpmEventLogHob (
  );

/**
  Get ActivePCR banks info from TPM_LIB_PRIVATE_DATA instance.

  @param  ActivePcrBanks       Active PCR banks in TPM

  @retval EFI_SUCCESS          Operation executed successfully.
  @retval EFI_NOT_FOUND        TPM Lib data not found.
**/
RETURN_STATUS
EFIAPI
TpmLibGetActivePcrBanks (
  IN UINT32 *ActivePcrBanks
  );
#endif  // _TPM_LIB_H
