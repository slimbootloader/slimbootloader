/** @file
  Implement TPM2 Capability related command.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <IndustryStandard/UefiTcgPlatform.h>
#include <Tpm2CommandLib.h>
#include <Tpm2DeviceLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#pragma pack(1)

typedef struct {
  TPM2_COMMAND_HEADER       Header;
  TPM_CAP                   Capability;
  UINT32                    Property;
  UINT32                    PropertyCount;
} TPM2_GET_CAPABILITY_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER      Header;
  TPMI_YES_NO               MoreData;
  TPMS_CAPABILITY_DATA      CapabilityData;
} TPM2_GET_CAPABILITY_RESPONSE;

#pragma pack()

/**
  This command returns various information regarding the TPM and its current state.

  The capability parameter determines the category of data returned. The property parameter
  selects the first value of the selected category to be returned. If there is no property
  that corresponds to the value of property, the next higher value is returned, if it exists.
  The moreData parameter will have a value of YES if there are more values of the requested
  type that were not returned.
  If no next capability exists, the TPM will return a zero-length list and moreData will have
  a value of NO.

  NOTE:
  To simplify this function, leave returned CapabilityData for caller to unpack since there are
  many capability categories and only few categories will be used in firmware. It means the caller
  need swap the byte order for the fields in CapabilityData.

  @param[in]  Capability         Group selection; determines the format of the response.
  @param[in]  Property           Further definition of information.
  @param[in]  PropertyCount      Number of properties of the indicated type to return.
  @param[out] MoreData           Flag to indicate if there are more values of this type.
  @param[out] CapabilityData     The capability data.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
**/
EFI_STATUS
EFIAPI
Tpm2GetCapability (
  IN      TPM_CAP                   Capability,
  IN      UINT32                    Property,
  IN      UINT32                    PropertyCount,
  OUT     TPMI_YES_NO               *MoreData,
  OUT     TPMS_CAPABILITY_DATA      *CapabilityData
  )
{
  EFI_STATUS                        Status;
  TPM2_GET_CAPABILITY_COMMAND       SendBuffer;
  TPM2_GET_CAPABILITY_RESPONSE      RecvBuffer;
  UINT32                            SendBufferSize;
  UINT32                            RecvBufferSize;

  //
  // Construct command
  //
  SendBuffer.Header.tag = SwapBytes16 (TPM_ST_NO_SESSIONS);
  SendBuffer.Header.commandCode = SwapBytes32 (TPM_CC_GetCapability);

  SendBuffer.Capability = SwapBytes32 (Capability);
  SendBuffer.Property = SwapBytes32 (Property);
  SendBuffer.PropertyCount = SwapBytes32 (PropertyCount);

  SendBufferSize = (UINT32) sizeof (SendBuffer);
  SendBuffer.Header.paramSize = SwapBytes32 (SendBufferSize);

  //
  // send Tpm command
  //
  RecvBufferSize = sizeof (RecvBuffer);
  Status = Tpm2SubmitCommand (SendBufferSize, (UINT8 *)&SendBuffer, &RecvBufferSize, (UINT8 *)&RecvBuffer );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (RecvBufferSize <= sizeof (TPM2_RESPONSE_HEADER) + sizeof (UINT8)) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Fail if command failed
  //
  if (SwapBytes32(RecvBuffer.Header.responseCode) != TPM_RC_SUCCESS) {
    DEBUG ((DEBUG_VERBOSE, "Tpm2GetCapability: Response Code error! 0x%08x\r\n", SwapBytes32(RecvBuffer.Header.responseCode)));
    return EFI_DEVICE_ERROR;
  }

  //
  // Return the response
  //
  *MoreData = RecvBuffer.MoreData;
  //
  // Does not unpack all possible property here, the caller should unpack it and note the byte order.
  //
  CopyMem (CapabilityData, &RecvBuffer.CapabilityData, RecvBufferSize - sizeof (TPM2_RESPONSE_HEADER) - sizeof (UINT8));

  return EFI_SUCCESS;
}

/**
  This command returns the information of TPM PCRs.

  This function parse the value got from TPM2_GetCapability and return the PcrSelection.

  @param[out] Pcrs    The Pcr Selection

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
**/
EFI_STATUS
EFIAPI
Tpm2GetCapabilityPcrs (
  OUT TPML_PCR_SELECTION      *Pcrs
  )
{
  TPMS_CAPABILITY_DATA    TpmCap;
  TPMI_YES_NO             MoreData;
  EFI_STATUS              Status;
  UINTN                   Index;

  Status = Tpm2GetCapability (
             TPM_CAP_PCRS,
             0,
             1,
             &MoreData,
             &TpmCap
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Pcrs->count = SwapBytes32 (TpmCap.data.assignedPCR.count);
  if (Pcrs->count > HASH_COUNT) {
    DEBUG ((DEBUG_VERBOSE, "Tpm2GetCapabilityPcrs - Pcrs->count error %x\n", Pcrs->count));
    return EFI_DEVICE_ERROR;
  }

  for (Index = 0; Index < Pcrs->count; Index++) {
    Pcrs->pcrSelections[Index].hash = SwapBytes16 (TpmCap.data.assignedPCR.pcrSelections[Index].hash);
    Pcrs->pcrSelections[Index].sizeofSelect = TpmCap.data.assignedPCR.pcrSelections[Index].sizeofSelect;
    if (Pcrs->pcrSelections[Index].sizeofSelect > PCR_SELECT_MAX) {
      DEBUG ((DEBUG_VERBOSE, "Tpm2GetCapabilityPcrs - sizeofSelect error %x\n", Pcrs->pcrSelections[Index].sizeofSelect));
      return EFI_DEVICE_ERROR;
    }
    CopyMem (Pcrs->pcrSelections[Index].pcrSelect, TpmCap.data.assignedPCR.pcrSelections[Index].pcrSelect,
             Pcrs->pcrSelections[Index].sizeofSelect);
  }

  return EFI_SUCCESS;
}

/**
  This function will query the TPM to determine which hashing algorithms
  are supported and which PCR banks are currently active.

  @param[out]  TpmHashAlgorithmBitmap A bitmask containing the algorithms supported by the TPM.
  @param[out]  ActivePcrBanks         A bitmask containing the PCRs currently allocated.

  @retval     EFI_SUCCESS   TPM was successfully queried and return values can be trusted.
  @retval     Others        An error occurred, likely in communication with the TPM.

**/
EFI_STATUS
EFIAPI
Tpm2GetCapabilitySupportedAndActivePcrs (
  OUT UINT32                            *TpmHashAlgorithmBitmap,
  OUT UINT32                            *ActivePcrBanks
  )
{
  EFI_STATUS            Status;
  TPML_PCR_SELECTION    Pcrs;
  UINTN                 Index;

  //
  // Get supported PCR and current Active PCRs.
  //
  Status = Tpm2GetCapabilityPcrs (&Pcrs);

  //
  // If error, assume that we have at least SHA-1 (and return the error.)
  //
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetSupportedAndActivePcrs - Tpm2GetCapabilityPcrs fail!\n"));
    *TpmHashAlgorithmBitmap = HASH_ALG_SHA1;
    *ActivePcrBanks         = HASH_ALG_SHA1;
  }
  //
  // Otherwise, process the return data to determine what algorithms are supported
  // and currently allocated.
  //
  else {
    DEBUG ((DEBUG_INFO, "GetSupportedAndActivePcrs - Count = %08x\n", Pcrs.count));
    *TpmHashAlgorithmBitmap = 0;
    *ActivePcrBanks         = 0;
    for (Index = 0; Index < Pcrs.count; Index++) {
      switch (Pcrs.pcrSelections[Index].hash) {
      case TPM_ALG_SHA1:
        DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SHA1 present.\n"));
        *TpmHashAlgorithmBitmap |= HASH_ALG_SHA1;
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SHA1 active.\n"));
          *ActivePcrBanks |= HASH_ALG_SHA1;
        }
        break;
      case TPM_ALG_SHA256:
        DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SHA256 present.\n"));
        *TpmHashAlgorithmBitmap |= HASH_ALG_SHA256;
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SHA256 active.\n"));
          *ActivePcrBanks |= HASH_ALG_SHA256;
        }
        break;
      case TPM_ALG_SHA384:
        DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SHA384 present.\n"));
        *TpmHashAlgorithmBitmap |= HASH_ALG_SHA384;
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SHA384 active.\n"));
          *ActivePcrBanks |= HASH_ALG_SHA384;
        }
        break;
      case TPM_ALG_SHA512:
        DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SHA512 present.\n"));
        *TpmHashAlgorithmBitmap |= HASH_ALG_SHA512;
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SHA512 active.\n"));
          *ActivePcrBanks |= HASH_ALG_SHA512;
        }
        break;
      case TPM_ALG_SM3_256:
        DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SM3_256 present.\n"));
        *TpmHashAlgorithmBitmap |= HASH_ALG_SM3_256;
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - HASH_ALG_SM3_256 active.\n"));
          *ActivePcrBanks |= HASH_ALG_SM3_256;
        }
        break;

      default:
        DEBUG ((DEBUG_VERBOSE, "GetSupportedAndActivePcrs - Unsupported bank 0x%04x.\n", Pcrs.pcrSelections[Index].hash));
        break;
      }
    }
  }

  return Status;
}
