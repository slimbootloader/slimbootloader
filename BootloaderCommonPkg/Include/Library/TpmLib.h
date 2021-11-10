/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TPM_LIB_H_
#define _TPM_LIB_H_

#include <IndustryStandard/Tpm20.h>
#include <IndustryStandard/UefiTcgPlatform.h>
#include <IndustryStandard/Tpm2Acpi.h>
#include <Library/CryptoLib.h>
#include <Library/ContainerLib.h>

#define ACPI_SSDT_TPM2_DEVICE_OEM_TABLE_ID  SIGNATURE_64('T', 'p', 'm', '2', 'T', 'a', 'b', 'l')

/**
  Initialize the TPM. Initiate TPM_Startup if not yet done by BootGuard component.

  @param BypassTpmInit    If TRUE, skip TPM_Startup as it is done by ACM.
  @param BootMode         Indicates platform boot mode i.e. if resuming from S3 or S2/S4/S5 boot mode.

  @retval RETURN_SUCCESS             TPM initialization completed successfully.
  @retval RETURN_DEVICE_ERROR        Unexpected device behavior.
  @retval Others                     The request could not be executed successfully.

**/
RETURN_STATUS
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
DisableTpm (
  VOID
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
TpmIndicateReadyToBoot (
  IN UINT8   FwDebugEnabled
  );


/**
  Get Crypto Lib Hash alg required by bootloader.

  @param  TcgAlgMask    TCG Alg Mask

  @retval CryptoHashAlg Crypo Hash Alg.
**/
UINT8
GetCryptoHashAlg (
  UINT32 TcgAlgMask
  );


/**
  Get Tpm Hash Algo type requested.

  @retval PcdHashType    TPM Algorithm Id.
**/
UINT32
GetTpmHashAlg (
   UINT32 TcgAlgHash
  );


/**
  Function to extend Stage component hash

  @param[in]  CbInfo    Component Call Back Info

**/
VOID
ExtendStageHash (
  IN  COMPONENT_CALLBACK_INFO   *CbInfo
  );
#endif  // _TPM_LIB_H
