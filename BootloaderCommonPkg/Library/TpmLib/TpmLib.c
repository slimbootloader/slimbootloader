/** @file
  TPM library routines to provide TPM support.
  For more details, consult TCG TPM specifications.

  Copyright (c) 2017 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CryptoLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TpmLib.h>
#include <Library/PcdLib.h>
#include <Pi/PiBootMode.h>
#include <IndustryStandard/Tpm2Acpi.h>
#include <Library/SecureBootLib.h>
#include <Library/ResetSystemLib.h>
#include "Tpm2CommandLib.h"
#include "Tpm2DeviceLib.h"
#include "TpmLibInternal.h"
#include "TpmEventLog.h"

/**
  Count Number of PCR Active banks.

  @param  PcrBankActive Pcr Active Bank Mask
  @retval Count         Active banks count
**/
UINT8
CountPcrBankActive (
  UINT32 PcrBankActiveMask
  )
{
    UINT8 Count = 0;
    while (PcrBankActiveMask) {
        PcrBankActiveMask &= (PcrBankActiveMask - 1);
        Count++;
    }
    return Count;
}

/**
  Get Crypto Lib Hash alg required by bootloader.

  @param  TcgAlgHash    TCG Alg type
  @retval CryptoHashAlg Crypo Hash Alg.
**/
UINT8
GetCryptoHashAlg (
  UINT32 TcgAlgMask
  )
{
  HASH_ALG_TYPE CryptoHashAlg = 0;

  if(TcgAlgMask == HASH_ALG_SHA256){
    CryptoHashAlg = HASH_TYPE_SHA256;
  } else if (TcgAlgMask == HASH_ALG_SHA384){
    CryptoHashAlg = HASH_TYPE_SHA384;
  } else if (TcgAlgMask == HASH_ALG_SHA512){
    CryptoHashAlg = HASH_TYPE_SHA512;
  } else if (TcgAlgMask == HASH_ALG_SM3_256){
    CryptoHashAlg = HASH_TYPE_SM3;
  }

  return CryptoHashAlg;
}


/**
  Get Tpm Hash Algo type requested.

  @retval PcdHashType    TPM Algorithm Id.
**/
UINT32
GetTpmHashAlg (
   UINT32 TcgAlgHash
  )
{
  UINT32 TpmHashAlg = 0;

  if(TcgAlgHash == HASH_ALG_SHA256){
    TpmHashAlg = TPM_ALG_SHA256;
  } else if (TcgAlgHash == HASH_ALG_SHA384){
    TpmHashAlg = TPM_ALG_SHA384;
  } else if (TcgAlgHash == HASH_ALG_SHA512){
    TpmHashAlg = TPM_ALG_SHA512;
  } else if (TcgAlgHash == HASH_ALG_SM3_256){
    TpmHashAlg = TPM_ALG_SM3_256;
  }

  return TpmHashAlg;
}


//@todo What happens to TPM_LIB_PRIVATE_DATA memory during S3 resume?

/**
  Get a pointer to the TPM_LIB_PRIVATE_DATA instance.

  @retval A pointer to the TPM_LIB_PRIVATE_DATA instance.
**/
TPM_LIB_PRIVATE_DATA *
TpmLibGetPrivateData (
  VOID
  )
{
  EFI_STATUS                Status;
  TPM_LIB_PRIVATE_DATA      *PrivateData;

  PrivateData = NULL;

  Status = GetLibraryData (PcdGet8 (PcdTpmLibId), (VOID **)&PrivateData);
  if (Status == EFI_NOT_FOUND) {
    DEBUG ((DEBUG_INFO, "TPM Lib Private Data not found\n"));
    PrivateData = AllocatePool (sizeof (TPM_LIB_PRIVATE_DATA));
    ZeroMem (PrivateData, sizeof (TPM_LIB_PRIVATE_DATA));
    Status = SetLibraryData (PcdGet8 (PcdTpmLibId), PrivateData, sizeof (TPM_LIB_PRIVATE_DATA));
  }

  if (EFI_ERROR (Status)) {
    PrivateData = NULL;
  }

  return PrivateData;
}


/**
  Set Tpm Ready status in TPM_LIB_PRIVATE_DATA instance.

  @param  TpmReady       TPM status for Bootloader
  @retval EFI_SUCCESS    Operation executed successfully.
  @retval EFI_NOT_FOUND  TPM Lib data not found.
**/
RETURN_STATUS
TpmLibSetReadyStatus (
  IN UINT8 TpmReady
  )
{
  EFI_STATUS                   Status;
  TPM_LIB_PRIVATE_DATA        *PrivateData;

  PrivateData = NULL;

  PrivateData = TpmLibGetPrivateData ();
  if (PrivateData == NULL) {
    return EFI_NOT_FOUND;
  }

  PrivateData->TpmReady = TpmReady;
  Status = SetLibraryData (PcdGet8 (PcdTpmLibId), PrivateData, sizeof (TPM_LIB_PRIVATE_DATA));
  return Status;
}


/**
  Set TCG Event Log area info in TPM_LIB_PRIVATE_DATA instance.

  @param  Lasa       Log area start address
  @param  Laml       Log area minimum length

  @retval EFI_SUCCESS    Operation executed successfully.
  @retval EFI_NOT_FOUND  TPM Lib data not found.
**/
RETURN_STATUS
TpmLibSetEventLogData (
  IN UINT64 Lasa,
  IN UINT32 Laml
  )
{
  EFI_STATUS                   Status;
  TPM_LIB_PRIVATE_DATA        *PrivateData;

  PrivateData = NULL;

  PrivateData = TpmLibGetPrivateData ();
  if (PrivateData == NULL) {
    return EFI_NOT_FOUND;
  }

  PrivateData->LogAreaStartAddress = Lasa;
  PrivateData->LogAreaMinLength = Laml;
  Status = SetLibraryData (PcdGet8 (PcdTpmLibId), PrivateData, sizeof (TPM_LIB_PRIVATE_DATA));
  return Status;
}


/**
  Set ActivePCR banks info in TPM_LIB_PRIVATE_DATA instance.

  @param  ActivePcrBanks       Active PCR banks in TPM

  @retval EFI_SUCCESS          Operation executed successfully.
  @retval EFI_NOT_FOUND        TPM Lib data not found.
**/
RETURN_STATUS
TpmLibSetActivePcrBanks (
  IN UINT32 ActivePcrBanks
  )
{
  EFI_STATUS                   Status;
  TPM_LIB_PRIVATE_DATA        *PrivateData;

  PrivateData = NULL;

  PrivateData = TpmLibGetPrivateData ();
  if (PrivateData == NULL) {
    return EFI_NOT_FOUND;
  }

  PrivateData->ActivePcrBanks = ActivePcrBanks;
  Status = SetLibraryData (PcdGet8 (PcdTpmLibId), PrivateData, sizeof (TPM_LIB_PRIVATE_DATA));
  return Status;
}

/**
  Get ActivePCR banks info from TPM_LIB_PRIVATE_DATA instance.

  @param  ActivePcrBanks       Active PCR banks in TPM

  @retval EFI_SUCCESS          Operation executed successfully.
  @retval EFI_NOT_FOUND        TPM Lib data not found.
**/
RETURN_STATUS
TpmLibGetActivePcrBanks (
  IN UINT32 *ActivePcrBanks
  )
{
  EFI_STATUS                   Status;
  TPM_LIB_PRIVATE_DATA        *PrivateData;

  PrivateData = NULL;

  PrivateData = TpmLibGetPrivateData ();
  if (PrivateData == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = GetLibraryData (PcdGet8 (PcdTpmLibId), (VOID *) &PrivateData);

  *ActivePcrBanks = PrivateData->ActivePcrBanks;

  return Status;
}


/**
  Checks if bootloader supported PCR bank is enabled in TPM.

  @retval RETURN_SUCCESS             Operation completed successfully.
  @retval Others                     The request could not be executed successfully.

**/
RETURN_STATUS
TpmPcrBankCheck (
  OUT UINT32  *PcrBanks
  )
{
  EFI_STATUS            Status;
  UINT32                TpmHashAlgorithmBitmap;
  UINT32                ActivePcrBanks;

  Status = Tpm2GetCapabilitySupportedAndActivePcrs (&TpmHashAlgorithmBitmap, &ActivePcrBanks);
  if (Status == EFI_SUCCESS) {

    DEBUG ((DEBUG_INFO, "TpmHashAlgorithmBitmap 0x%08x ActivePcrBanks 0x%08x\n", TpmHashAlgorithmBitmap, ActivePcrBanks));

    // Update Active PCR banks info in TPM Library data
    TpmLibSetActivePcrBanks (ActivePcrBanks);

    // Check if TPM supports  PCR bank
    if (!(TpmHashAlgorithmBitmap & (HASH_ALG_SHA256 | HASH_ALG_SHA384 | HASH_ALG_SM3_256))){
      DEBUG ((DEBUG_ERROR, "Hash algorithm is NOT supported. This should never happen with TPM2.0 device. \n"));
      return EFI_UNSUPPORTED;
    }

    // Check if required PCR bank is enabled
    if (ActivePcrBanks == PcdGet32(PcdMeasuredBootHashMask)) {
      DEBUG ((DEBUG_INFO, "Bootloader requested PCR Bank is enabled. \n"));
    } else {
      Status = Tpm2PcrAllocateBanks (NULL, TpmHashAlgorithmBitmap, PcdGet32(PcdMeasuredBootHashMask));
      if (Status == EFI_SUCCESS) {
        DEBUG ((DEBUG_INFO, "Successfully enabled requested PCR Bank. It will be available after next platform reset. \n"));
      } else {
        DEBUG ((DEBUG_INFO, "Requested pcrbank 0x%x allocation failed\n", PcdGet32(PcdMeasuredBootHashMask)));
      }
    }
  }

  *PcrBanks = ActivePcrBanks;

  return Status;
}

/**
  Disable TPM as per TCG PC Client Platform Firmware Profile Spec (Section 6.1) requirements.

  @retval RETURN_SUCCESS             Operation completed successfully.
  @retval Others                     The request could not be executed successfully.

**/
RETURN_STATUS
DisableTpm (
  VOID
  )
{

  // When the TPM is not discoverable and is hidden from the OS:
  // a. The TPM2 ACPI Table SHALL NOT be listed in the RSDT or XSDT tables.
  // b. The TPM device object SHALL NOT be present in the ACPI namespace.
  // c. The EFI_TCG2_EFI_PROTOCOL SHALL NOT be present.
  // d. The TPM SHALL not be usable by OS-present applications.
  // e. The TPM Storage and Endorsement Hierarchies SHALL be disabled by Firmware
  //    on every boot using a TPM2_HierarchyControl command for both hierarchies.
  // f. The TPM Platform Hierarchy MAY be disabled by Firmware on every boot using a
  //    TPM2_HierarchyEnable command.
  // g. Firmware SHALL cap PCR[0-7] as described in Section 2.3.4 (PCR Usage).
  // h. Platform Firmware SHALL NOT create the TCG Event Log.
  // i. The Platform OEM-provided mechanism to make the TPM visible to the OS SHALL
  //    perform a Host Platform Reset.
  // j. The TPM SHALL remain hidden from the OS on resume from sleep or hibernate.

  EFI_STATUS              Status;
  TPMS_AUTH_COMMAND       AuthSession;

  DEBUG ((DEBUG_INFO, "Disabling  TPM\n"));
  // If Tpm has already been started, this call should return with EFI_SUCCESS
  Status = Tpm2Startup (TPM_SU_CLEAR);

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_WARN, "Unable to disable TPM\n"));
    return Status;
  }

  ZeroMem (&AuthSession, sizeof (AuthSession));
  AuthSession.sessionHandle = TPM_RS_PW;

  // Disable Endorsement hierarchy.
  Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &AuthSession, TPM_RH_ENDORSEMENT, NO);
  DEBUG ((DEBUG_INFO, "Tpm2HierarchyControl (0x%08x, %02x)- %r\n", TPM_RH_ENDORSEMENT, NO, Status));

  // Disable Owner hierarchy.
  Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &AuthSession, TPM_RH_OWNER, NO);
  DEBUG ((DEBUG_INFO, "Tpm2HierarchyControl (0x%08x, %02x)- %r\n", TPM_RH_OWNER, NO, Status));

  // Disable platform hierarchy.
  Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &AuthSession, TPM_RH_PLATFORM, NO);
  DEBUG ((DEBUG_INFO, "Tpm2HierarchyControl (0x%08x, %02x)- %r\n", TPM_RH_PLATFORM, NO, Status));

  // Cap PCR[0-7].
  MeasureSeparatorEvent(1);

  TpmLibSetReadyStatus (0);
  return Status;
}

/**
  It returns the status of TPM.

  @retval TRUE   TPM is available.
  @retval FALSE  TPM is not available.
**/
BOOLEAN
IsTpmEnabled (
  VOID
  )
{
  TPM_LIB_PRIVATE_DATA      *PrivateData;

  PrivateData = TpmLibGetPrivateData ();
  if ((PrivateData != NULL) && (PrivateData->TpmReady)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Get the TPM event log buffer info.


  @param Lasa  TPM event log buffer.
  @param Laml  TPM event log size.

  @retval RETURN_SUCCESS             Operation completed successfully.
  @retval RETURN_INVALID_PARAMETER   Invalid parameter.
  @retval RETURN_DEVICE_ERROR        Tpm Device not found or in bad state.
  @retval Others                     The request could not be executed successfully.

**/
RETURN_STATUS
GetTpmEventLog (
  OUT UINT64 *Lasa,
  OUT UINT32 *Laml
  )
{
  TPM_LIB_PRIVATE_DATA        *PrivateData;

  if ((Lasa == NULL) || (Laml == NULL)) {
    return RETURN_INVALID_PARAMETER;
  }

  if (!IsTpmEnabled()) {
    return RETURN_DEVICE_ERROR;
  }

  PrivateData = TpmLibGetPrivateData ();
  if (PrivateData != NULL) {
    *Lasa = PrivateData->LogAreaStartAddress;
    *Laml = PrivateData->LogAreaMinLength;
  } else {
    return RETURN_INVALID_PARAMETER;
  }

  return RETURN_SUCCESS;
}

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
  )
{
  EFI_STATUS                   Status;
  EFI_TPM2_ACPI_TABLE         *Tpm2AcpiTable;
  TPM_LIB_PRIVATE_DATA        *PrivateData;

  if (Table == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  if (!IsTpmEnabled()) {
    return RETURN_DEVICE_ERROR;
  }

  if (Table->Signature == EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE) {
    Tpm2AcpiTable = (EFI_TPM2_ACPI_TABLE *)Table;
    Status = UpdateAcpiInterfaceInfo (Tpm2AcpiTable);
    if (Status == EFI_SUCCESS) {
      PrivateData = TpmLibGetPrivateData ();
      if (PrivateData != NULL) {
        Tpm2AcpiTable->LogAreaStartAddress = PrivateData->LogAreaStartAddress;
        Tpm2AcpiTable->LogAreaMinimumLen = PrivateData->LogAreaMinLength;
      }
    }
    return Status;

  } else if (Table->OemTableId == ACPI_SSDT_TPM2_DEVICE_OEM_TABLE_ID) {
    // @todo  Update _HID with Manufacturer ID and Firmware version number.
    return RETURN_SUCCESS;

  } else {
    return RETURN_INVALID_PARAMETER;
  }

}

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
  )
{
  EFI_STATUS                   Status;
  UINT32                       ActivePcrBank;

  // Detect if any TPM is available.
  if (EFI_ERROR (IsSupportedTpmPresent()) || EFI_ERROR (Tpm2RequestUseTpm ())) {
    DEBUG ((DEBUG_ERROR, "Unable to talk to TPM !!\n"));
    goto TpmError;
  }

  // Initialize TPM if it has not already been initialized by BootGuard component (i.e. ACM)
  if (!BypassTpmInit) {
    if (BootMode == BOOT_ON_S3_RESUME) {
      DEBUG ((DEBUG_INFO, "Attempting TPM_Startup with TPM_SU_STATE. \n"));
      Status = Tpm2Startup (TPM_SU_STATE);
      if (EFI_ERROR (Status)) {
        //  As per PC Client spec, SRTM should perform a host platform reset.
        ResetSystem(EfiResetCold);
        CpuDeadLoop ();
      }
    } else {
      Status = Tpm2Startup (TPM_SU_CLEAR);
    }
    if (EFI_ERROR (Status)) {
      DisableTpm();
      goto TpmError;
    }
  }

  // Check for appropriate PCR Bank. Allocate PCR Bank requested, if it is not enabled.
  if (TpmPcrBankCheck(&ActivePcrBank) != EFI_SUCCESS) {
    DisableTpm();
    goto TpmError;
  }

  // @todo TPM_SelfTest (for dTPM)

  // Set TPM Ready Status
  TpmLibSetReadyStatus (1);

  // Initialize TCG Event Log
  if (BootMode == BOOT_ON_S3_RESUME) {
    DEBUG ((DEBUG_INFO, "TCG Event Log initialization skipped.\n"));
  } else {
    Status = TpmTcgLogInit();
    if (Status == EFI_SUCCESS) {
      if (!BypassTpmInit) {
        // TPM_Start was done by SBL via Locality 0
        TpmLogLocalityEvent (0, ActivePcrBank);
      }
    }
  }

  DEBUG ((DEBUG_INFO, "TPM initialization completed succesfully.\n"));
  return RETURN_SUCCESS;

TpmError:
  TpmLibSetReadyStatus (0);
  return RETURN_DEVICE_ERROR;
}


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
  )
{
  EFI_STATUS                 Status;
  TPML_DIGEST_VALUES         *Digests;
  TCG_PCR_EVENT2_HDR         PcrEventHdr;
  UINT32                     Data;
  UINT32                     PcrHandle;
  UINT32                     PcrBankActive;

  if (!IsTpmEnabled()){
    return RETURN_DEVICE_ERROR;
  }

  PcrHandle = 0;
  Data = WithError;
  Digests = &PcrEventHdr.Digests;
  Digests->count = 1;

  TpmLibGetActivePcrBanks(&PcrBankActive);

  Digests->count = 0;

  if ((PcrBankActive & HASH_ALG_SHA256) != 0) {
      Digests->digests[Digests->count].hashAlg = (TPMI_ALG_HASH) GetTpmHashAlg(HASH_ALG_SHA256);
      CalculateHash ((UINT8 *)&Data, sizeof (Data), GetCryptoHashAlg(HASH_ALG_SHA256) , (UINT8 *) (&(Digests->digests[Digests->count].digest)));
      Digests->count++;
  }

  if ((PcrBankActive & HASH_ALG_SHA384) != 0) {
      Digests->digests[Digests->count].hashAlg = (TPMI_ALG_HASH) GetTpmHashAlg(HASH_ALG_SHA384);
      CalculateHash ((UINT8 *)&Data, sizeof (Data), GetCryptoHashAlg(HASH_ALG_SHA384) , (UINT8 *) (&(Digests->digests[Digests->count].digest)));
      Digests->count++;
  }

  if ((PcrBankActive & HASH_ALG_SHA512) != 0) {
      Digests->digests[Digests->count].hashAlg = (TPMI_ALG_HASH) GetTpmHashAlg(HASH_ALG_SHA512);
      CalculateHash ((UINT8 *)&Data, sizeof (Data), GetCryptoHashAlg(HASH_ALG_SHA512) , (UINT8 *) (&(Digests->digests[Digests->count].digest)));
      Digests->count++;
  }

  if ((PcrBankActive & HASH_ALG_SM3_256) != 0) {
      Digests->digests[Digests->count].hashAlg = (TPMI_ALG_HASH) GetTpmHashAlg(HASH_ALG_SM3_256);
      CalculateHash ((UINT8 *)&Data, sizeof (Data), GetCryptoHashAlg(HASH_ALG_SM3_256) , (UINT8 *) (&(Digests->digests[Digests->count].digest)));
      Digests->count++;
  }

  for (PcrHandle = 0; PcrHandle <= 7; PcrHandle++) {
    Status = Tpm2PcrExtend (PcrHandle, Digests);
    if (Status == EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "PCR (%u) extended successfully with (%u) event type.\n",
              PcrHandle, EV_SEPARATOR));

      PcrEventHdr.PCRIndex = PcrHandle;
      PcrEventHdr.EventType = EV_SEPARATOR;
      PcrEventHdr.EventSize = (UINT32)sizeof (Data);

      TpmLogEvent ( &PcrEventHdr, (UINT8 *)&Data);
    } else {
      DEBUG ((DEBUG_ERROR, "PCR (%u) extend FAIL with error (0x%8x) .\n",
        PcrHandle, Status));
    }
  }

  DEBUG ((DEBUG_INFO, "Capped PCR[0-7] with hash value of (%u). Status : %u\n", WithError, Status));
  return Status;
}


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
)
{
  EFI_STATUS                 Status;
  TCG_PCR_EVENT2_HDR         PcrEventHdr;
  TPML_DIGEST_VALUES        *Digests;
  UINT32                     PcrBankActive;

  if (Data == NULL || Event == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  if (!IsTpmEnabled()){
    return RETURN_DEVICE_ERROR;
  }

  Digests = &PcrEventHdr.Digests;
  Digests->count = 0;

  TpmLibGetActivePcrBanks(&PcrBankActive);

  if ((PcrBankActive & HASH_ALG_SHA256) != 0) {
      Digests->digests[Digests->count].hashAlg = (TPMI_ALG_HASH) GetTpmHashAlg(HASH_ALG_SHA256);
      CalculateHash (Data, Length, GetCryptoHashAlg(HASH_ALG_SHA256) , (UINT8 *) (&(Digests->digests[Digests->count].digest)));
      Digests->count++;
  }
  if ((PcrBankActive & HASH_ALG_SHA384) != 0) {
      Digests->digests[Digests->count].hashAlg = (TPMI_ALG_HASH) GetTpmHashAlg(HASH_ALG_SHA384);
      CalculateHash (Data, Length, GetCryptoHashAlg(HASH_ALG_SHA384) , (UINT8 *) (&(Digests->digests[Digests->count].digest)));
      Digests->count++;
  }
  if ((PcrBankActive & HASH_ALG_SHA512) != 0) {
      Digests->digests[Digests->count].hashAlg = (TPMI_ALG_HASH) GetTpmHashAlg(HASH_ALG_SHA512);
      CalculateHash (Data, Length, GetCryptoHashAlg(HASH_ALG_SHA512) , (UINT8 *) (&(Digests->digests[Digests->count].digest)));
      Digests->count++;
  }
  if ((PcrBankActive & HASH_ALG_SM3_256) != 0) {
      Digests->digests[Digests->count].hashAlg = (TPMI_ALG_HASH) GetTpmHashAlg(HASH_ALG_SM3_256);
      CalculateHash (Data, Length, GetCryptoHashAlg(HASH_ALG_SM3_256) , (UINT8 *) (&(Digests->digests[Digests->count].digest)));
      Digests->count++;
  }

  Status = Tpm2PcrExtend (PcrHandle, Digests);
  if (Status == EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "PCR (%u) extended successfully with (%u) event type.\n",
            PcrHandle, EventType));

    PcrEventHdr.PCRIndex = PcrHandle;
    PcrEventHdr.EventType = EventType;
    PcrEventHdr.EventSize = EventSize;

    TpmLogEvent ( &PcrEventHdr, Event);

  } else {
    DEBUG ((DEBUG_ERROR, "PCR (%u) extend FAIL with error (0x%8x) .\n",
      PcrHandle, Status));
  }

  return Status;
}


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
  )
{
  EFI_STATUS                 Status;
  TCG_PCR_EVENT2_HDR         PcrEventHdr;
  TPML_DIGEST_VALUES        *Digests;

  if (Hash == NULL || Event == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  if (!IsTpmEnabled()){
    return RETURN_DEVICE_ERROR;
  }

  Digests = &PcrEventHdr.Digests;
  Digests->count = 1;
  Digests->digests[0].hashAlg = HashAlg;

  CopyMem (& (Digests->digests[0].digest), Hash, GetHashSizeFromAlgo (HashAlg));

  Status = Tpm2PcrExtend (PcrHandle, Digests);
  if (Status == EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "PCR (%u) extended successfully with (%u) event type.\n",
            PcrHandle, EventType));

    PcrEventHdr.PCRIndex = PcrHandle;
    PcrEventHdr.EventType = EventType;
    PcrEventHdr.EventSize = EventSize;

    TpmLogEvent ( &PcrEventHdr, Event);

  } else {
    DEBUG ((DEBUG_ERROR, "PCR (%u) extend FAIL with error (0x%8x) .\n",
      PcrHandle, Status));
  }

  return Status;
}

/**
  Measure and log launch of FirmwareDebugger, and extend the measurement result into a specific PCR.

  @param[in] FwDebugEnabled    Firmware Debug Mode.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
RETURN_STATUS
MeasureLaunchOfFirmwareDebugger (
  IN  UINT8 FwDebugEnabled
  )
{
  EFI_STATUS                        Status;

  Status = EFI_SUCCESS;

  if (FwDebugEnabled) {
    DEBUG((DEBUG_INFO, "Measure firmware Debugger \n"));
    // Extend to PCR[7]
    Status = TpmHashAndExtendPcrEventLog (7, (UINT8 *) FIRMWARE_DEBUGGER_EVENT_STRING,
                                           sizeof (FIRMWARE_DEBUGGER_EVENT_STRING)-1,
                                           EV_EFI_ACTION,
                                           sizeof (FIRMWARE_DEBUGGER_EVENT_STRING),
                                           (UINT8*) FIRMWARE_DEBUGGER_EVENT_STRING);
  }

  return Status;
}


/**
  Extend VerifiedBootPolicy in PCR [7].

  @retval RETURN_SUCCESS      Operation completed successfully.
  @retval Others              Unable to extend PCR.
**/
RETURN_STATUS
TpmExtendSecureBootPolicy (
  VOID
  )
{
  EFI_STATUS                 Status;
  UINT8                      Data;

  Data = FeaturePcdGet (PcdVerifiedBootEnabled);

  Status = TpmHashAndExtendPcrEventLog (7, &Data, sizeof (Data), EV_EFI_VARIABLE_DRIVER_CONFIG, sizeof ("SecureBootPolicy"), (UINT8*)"SecureBootPolicy");

  return Status;
}

/**
  As per Section 10 of TCG PC Client Platform Spec, update the auth value for
  Platform hierarchy so that only firmware can use it.

**/
RETURN_STATUS
TpmChangePlatformAuth (
  VOID
  )
{
  TPM2B_AUTH           NewPlatformAuth;
  RETURN_STATUS        Status;
  UINT32               PcrBankActive;

  TpmLibGetActivePcrBanks(&PcrBankActive);

  if (PcrBankActive & HASH_ALG_SHA256){
    NewPlatformAuth.size = SHA256_DIGEST_SIZE;
  } else if (PcrBankActive & HASH_ALG_SHA384){
    NewPlatformAuth.size = SHA384_DIGEST_SIZE;
  } else if (PcrBankActive & HASH_ALG_SHA512){
    NewPlatformAuth.size = SHA512_DIGEST_SIZE;
  } else if (PcrBankActive & HASH_ALG_SM3_256){
    NewPlatformAuth.size = SM3_DIGEST_SIZE;
  } else {
    return RETURN_INVALID_PARAMETER;
  }

  if ((GetRandomBytes(NewPlatformAuth.buffer, NewPlatformAuth.size) == 0) &&
      (Tpm2HierarchyChangeAuth (TPM_RH_PLATFORM, NULL, &NewPlatformAuth) == EFI_SUCCESS)) {
    Status = EFI_SUCCESS;
    DEBUG ((DEBUG_INFO, "Successfully updated Auth value for Platform Hierarchy.\n"));
  } else {
    Status = EFI_DEVICE_ERROR;
    DEBUG ((DEBUG_ERROR, "FAIL to update Auth value for Platform Hierarchy\n"));
  }

  //
  //Below debug statement should ONLY be 'manually' enabled for testing/validation.
  //DEBUG ((DEBUG_INFO, "New Platform Auth:\n"));
  //DumpHex (2, 0, NewPlatformAuth.size, (VOID *)NewPlatformAuth.buffer);
  //

  SecureZeroMem(NewPlatformAuth.buffer, 0, NewPlatformAuth.size);
  return Status;
}

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
  IN UINT8 FwDebugEnabled
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;

  if (MeasureLaunchOfFirmwareDebugger (FwDebugEnabled) != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "FAILED to measure firmware debugger.\n"));
    Status =  EFI_DEVICE_ERROR;
  }
  if (TpmExtendSecureBootPolicy () != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "FAILED to extend secureboot policy.\n"));
    Status =  EFI_DEVICE_ERROR;
  }
  if (MeasureSeparatorEvent (0) != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "FAILED to measure seperator events.\n"));
    Status =  EFI_DEVICE_ERROR;
  }
  if (TpmChangePlatformAuth () != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "FAILED to change TPM Platform Auth.\n"));
    Status =  EFI_DEVICE_ERROR;
  }

  return Status;
}



/**
  Function to extend Stage component hash

  @param[in]  CbInfo    Component Call Back Info

**/
VOID
ExtendStageHash (
  IN  COMPONENT_CALLBACK_INFO   *CbInfo
  )
{
  UINT8                       DigestHash[HASH_DIGEST_MAX];
  HASH_ALG_TYPE               MbHashType;
  TPMI_ALG_HASH               MbTmpAlgHash;
  UINT8                       *HashPtr;
  RETURN_STATUS               Status;
  EFI_PLATFORM_FIRMWARE_BLOB  Blob;

  //Convert Measured boot Hash Mask to HASH_ALG_TYPE (CryptoLib)
  MbHashType   = GetCryptoHashAlg(PcdGet32(PcdMeasuredBootHashMask));

  //Convert Measured boot Hash Mask to TPMI_ALG_HASH (TPM ALG ID)
  MbTmpAlgHash = (TPMI_ALG_HASH) GetTpmHashAlg(PcdGet32(PcdMeasuredBootHashMask));

  //Extend  hash if ComponentType matches
  if ((CbInfo != NULL ) && ((CbInfo->ComponentType == COMP_TYPE_STAGE_2)
                            || (CbInfo->ComponentType == COMP_TYPE_PAYLOAD)
                            || (CbInfo->ComponentType == FLASH_MAP_SIG_EPAYLOAD)
                            || (CbInfo->ComponentType == COMP_TYPE_PAYLOAD_FWU )
                            || (CbInfo->ComponentType == CONTAINER_BOOT_SIGNATURE ))) {
    // Check Hash data alg match to PcdMeasuredBootHashMask
    if ((CbInfo->HashAlg == MbHashType) && (CbInfo->HashData != NULL)) {
      // Extend CbInfo->HashData if hashalg is valid
      HashPtr = CbInfo->HashData;
      Status = EFI_SUCCESS;
    } else {
      // Get Hash to extend based on component type and component src addresss
      Status = GetHashToExtend ((UINT8) CbInfo->ComponentType,
                                  MbHashType, CbInfo->CompBuf, CbInfo->CompLen, DigestHash);
      HashPtr = DigestHash;
    }

    if ((Status == EFI_SUCCESS) && (HashPtr != NULL)) {

      if (CbInfo->ComponentType == CONTAINER_BOOT_SIGNATURE) {
        // TPM Extend for OS Image
        TpmExtendPcrAndLogEvent (8, MbTmpAlgHash, HashPtr,
                              EV_COMPACT_HASH, sizeof("LinuxLoaderPkg: OS Image"), (UINT8 *)"LinuxLoaderPkg: OS Image");
      } else {
        // Record base and length in event log
        Blob.BlobBase = (UINT64)(UINTN)CbInfo->CompBuf;
        Blob.BlobLength = CbInfo->CompLen;

        // TPM Extend for Stage components and payloads
        TpmExtendPcrAndLogEvent (0, MbTmpAlgHash, HashPtr,
                              EV_EFI_PLATFORM_FIRMWARE_BLOB, sizeof(Blob), (UINT8 *)&Blob);
      }
    } else {
      DEBUG((DEBUG_INFO, "Stage2 TPM PCR(0) extend failed!! \n"));
    }
  }
}
