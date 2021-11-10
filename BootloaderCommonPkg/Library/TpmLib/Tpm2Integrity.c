/** @file
  Implement TPM2 Integrity related command.

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
  TPMI_DH_PCR               PcrHandle;
  UINT32                    AuthorizationSize;
  TPMS_AUTH_COMMAND         AuthSessionPcr;
  TPML_DIGEST_VALUES        DigestValues;
} TPM2_PCR_EXTEND_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER       Header;
  UINT32                     ParameterSize;
  TPMS_AUTH_RESPONSE         AuthSessionPcr;
} TPM2_PCR_EXTEND_RESPONSE;

typedef struct {
  TPM2_COMMAND_HEADER       Header;
  TPMI_RH_PLATFORM          AuthHandle;
  UINT32                    AuthSessionSize;
  TPMS_AUTH_COMMAND         AuthSession;
  TPML_PCR_SELECTION        PcrAllocation;
} TPM2_PCR_ALLOCATE_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER       Header;
  UINT32                     AuthSessionSize;
  TPMI_YES_NO                AllocationSuccess;
  UINT32                     MaxPCR;
  UINT32                     SizeNeeded;
  UINT32                     SizeAvailable;
  TPMS_AUTH_RESPONSE         AuthSession;
} TPM2_PCR_ALLOCATE_RESPONSE;

#pragma pack()

/**
  This command is used to cause an update to the indicated PCR.
  The digests parameter contains one or more tagged digest value identified by an algorithm ID.
  For each digest, the PCR associated with pcrHandle is Extended into the bank identified by the tag (hashAlg).

  @param[in] PcrHandle   Handle of the PCR
  @param[in] Digests     List of tagged digest values to be extended

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2PcrExtend (
  IN      TPMI_DH_PCR               PcrHandle,
  IN      TPML_DIGEST_VALUES        *Digests
  )
{
  EFI_STATUS                        Status;
  TPM2_PCR_EXTEND_COMMAND           Cmd;
  TPM2_PCR_EXTEND_RESPONSE          Res;
  UINT32                            CmdSize;
  UINT32                            RespSize;
  UINT32                            ResultBufSize;
  UINT8                             *Buffer;
  UINTN                             Index;
  UINT32                            SessionInfoSize;
  UINT16                            DigestSize;

  Cmd.Header.tag         = SwapBytes16 (TPM_ST_SESSIONS);
  Cmd.Header.commandCode = SwapBytes32 (TPM_CC_PCR_Extend);
  Cmd.PcrHandle          = SwapBytes32 (PcrHandle);


  //
  // Add in Auth session
  //
  Buffer = (UINT8 *)&Cmd.AuthSessionPcr;

  // sessionInfoSize
  SessionInfoSize = CopyAuthSessionCommand (NULL, Buffer);
  Buffer += SessionInfoSize;
  Cmd.AuthorizationSize = SwapBytes32 (SessionInfoSize);

  //Digest Count
  WriteUnaligned32 ((UINT32 *)Buffer, SwapBytes32 (Digests->count));
  Buffer += sizeof (UINT32);

  //Digest
  for (Index = 0; Index < Digests->count; Index++) {
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16 (Digests->digests[Index].hashAlg));
    Buffer += sizeof (UINT16);
    DigestSize = GetHashSizeFromAlgo (Digests->digests[Index].hashAlg);
    if (DigestSize == 0) {
      DEBUG ((DEBUG_ERROR, "Unknown hash algorithm %d\r\n", Digests->digests[Index].hashAlg));
      return EFI_DEVICE_ERROR;
    }
    CopyMem (
      Buffer,
      &Digests->digests[Index].digest,
      DigestSize
      );
    Buffer += DigestSize;
  }

  CmdSize              = (UINT32) ((UINTN)Buffer - (UINTN)&Cmd);
  Cmd.Header.paramSize = SwapBytes32 (CmdSize);

  ResultBufSize = sizeof (Res);
  Status = Tpm2SubmitCommand (CmdSize, (UINT8 *)&Cmd, &ResultBufSize, (UINT8 *)&Res);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (ResultBufSize > sizeof (Res)) {
    DEBUG ((DEBUG_ERROR, "Tpm2PcrExtend: Failed ExecuteCommand: Buffer Too Small\r\n"));
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Validate response headers
  //
  RespSize = SwapBytes32 (Res.Header.paramSize);
  if (RespSize > sizeof (Res)) {
    DEBUG ((DEBUG_ERROR, "Tpm2PcrExtend: Response size too large! %d\r\n", RespSize));
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Fail if command failed
  //
  if (SwapBytes32 (Res.Header.responseCode) != TPM_RC_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Tpm2PcrExtend: Response Code error! 0x%08x\r\n", SwapBytes32 (Res.Header.responseCode)));
    return EFI_DEVICE_ERROR;
  }

  //
  // Unmarshal the response
  //

  // None

  return EFI_SUCCESS;
}


/**
  This command is used to set the desired PCR allocation of PCR and algorithms.

  @param[in]  AuthHandle         TPM_RH_PLATFORM+{PP}
  @param[in]  AuthSession        Auth Session context
  @param[in]  PcrAllocation      The requested allocation
  @param[out] AllocationSuccess  YES if the allocation succeeded
  @param[out] MaxPCR             maximum number of PCR that may be in a bank
  @param[out] SizeNeeded         number of octets required to satisfy the request
  @param[out] SizeAvailable      Number of octets available. Computed before the allocation

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
**/
EFI_STATUS
EFIAPI
Tpm2PcrAllocate (
  IN  TPMI_RH_PLATFORM          AuthHandle,
  IN  TPMS_AUTH_COMMAND         *AuthSession,
  IN  TPML_PCR_SELECTION        *PcrAllocation,
  OUT TPMI_YES_NO               *AllocationSuccess,
  OUT UINT32                    *MaxPCR,
  OUT UINT32                    *SizeNeeded,
  OUT UINT32                    *SizeAvailable
  )
{
  EFI_STATUS                  Status;
  TPM2_PCR_ALLOCATE_COMMAND   Cmd;
  TPM2_PCR_ALLOCATE_RESPONSE  Res;
  UINT32                      CmdSize;
  UINT32                      RespSize;
  UINT8                       *Buffer;
  UINT32                      SessionInfoSize;
  UINT8                       *ResultBuf;
  UINT32                      ResultBufSize;
  UINTN                       Index;

  //
  // Construct command
  //
  Cmd.Header.tag          = SwapBytes16 (TPM_ST_SESSIONS);
  Cmd.Header.paramSize    = SwapBytes32 (sizeof (Cmd));
  Cmd.Header.commandCode  = SwapBytes32 (TPM_CC_PCR_Allocate);
  Cmd.AuthHandle          = SwapBytes32 (AuthHandle);

  //
  // Add in Auth session
  //
  Buffer = (UINT8 *)&Cmd.AuthSession;

  // sessionInfoSize
  SessionInfoSize = CopyAuthSessionCommand (AuthSession, Buffer);
  Buffer += SessionInfoSize;
  Cmd.AuthSessionSize = SwapBytes32 (SessionInfoSize);

  // Count
  WriteUnaligned32 ((UINT32 *)Buffer, SwapBytes32 (PcrAllocation->count));
  Buffer += sizeof (UINT32);
  for (Index = 0; Index < PcrAllocation->count; Index++) {
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16 (PcrAllocation->pcrSelections[Index].hash));
    Buffer += sizeof (UINT16);
    * (UINT8 *)Buffer = PcrAllocation->pcrSelections[Index].sizeofSelect;
    Buffer++;
    CopyMem (Buffer, PcrAllocation->pcrSelections[Index].pcrSelect, PcrAllocation->pcrSelections[Index].sizeofSelect);
    Buffer += PcrAllocation->pcrSelections[Index].sizeofSelect;
  }

  CmdSize = (UINT32) (Buffer - (UINT8 *)&Cmd);
  Cmd.Header.paramSize = SwapBytes32 (CmdSize);

  ResultBuf     = (UINT8 *) &Res;
  ResultBufSize = sizeof (Res);

  //
  // Call the TPM
  //
  Status = Tpm2SubmitCommand (
             CmdSize,
             (UINT8 *)&Cmd,
             &ResultBufSize,
             ResultBuf
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  if (ResultBufSize > sizeof (Res)) {
    DEBUG ((DEBUG_ERROR, "Tpm2PcrAllocate: Failed ExecuteCommand: Buffer Too Small\r\n"));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Validate response headers
  //
  RespSize = SwapBytes32 (Res.Header.paramSize);
  if (RespSize > sizeof (Res)) {
    DEBUG ((DEBUG_ERROR, "Tpm2PcrAllocate: Response size too large! %d\r\n", RespSize));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Fail if command failed
  //
  if (SwapBytes32 (Res.Header.responseCode) != TPM_RC_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Tpm2PcrAllocate: Response Code error! 0x%08x\r\n", SwapBytes32 (Res.Header.responseCode)));
    Status = EFI_DEVICE_ERROR;
    goto Done;
  }

  //
  // Return the response
  //
  *AllocationSuccess = Res.AllocationSuccess;
  *MaxPCR = SwapBytes32 (Res.MaxPCR);
  *SizeNeeded = SwapBytes32 (Res.SizeNeeded);
  *SizeAvailable = SwapBytes32 (Res.SizeAvailable);

Done:
  //
  // Clear AuthSession Content
  //
  ZeroMem (&Cmd, sizeof (Cmd));
  ZeroMem (&Res, sizeof (Res));
  return Status;
}

/**
  Alloc PCR data.

  @param[in]  PlatformAuth      platform auth value. NULL means no platform auth change.
  @param[in]  SupportedPCRBanks Supported PCR banks
  @param[in]  PCRBanks          PCR banks

  @retval EFI_SUCCESS Operation completed successfully.
**/
EFI_STATUS
EFIAPI
Tpm2PcrAllocateBanks (
  IN TPM2B_AUTH                *PlatformAuth,  OPTIONAL
  IN UINT32                    SupportedPCRBanks,
  IN UINT32                    PCRBanks
  )
{
  EFI_STATUS                Status;
  TPMS_AUTH_COMMAND         *AuthSession;
  TPMS_AUTH_COMMAND         LocalAuthSession;
  TPML_PCR_SELECTION        PcrAllocation;
  TPMI_YES_NO               AllocationSuccess;
  UINT32                    MaxPCR;
  UINT32                    SizeNeeded;
  UINT32                    SizeAvailable;

  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    AuthSession = &LocalAuthSession;
    ZeroMem (&LocalAuthSession, sizeof (LocalAuthSession));
    LocalAuthSession.sessionHandle = TPM_RS_PW;
    LocalAuthSession.hmac.size = PlatformAuth->size;
    CopyMem (LocalAuthSession.hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
  }

  //
  // Fill input
  //
  ZeroMem (&PcrAllocation, sizeof (PcrAllocation));
  if ((HASH_ALG_SHA1 & SupportedPCRBanks) != 0) {
    PcrAllocation.pcrSelections[PcrAllocation.count].hash = TPM_ALG_SHA1;
    PcrAllocation.pcrSelections[PcrAllocation.count].sizeofSelect = PCR_SELECT_MAX;
    if ((HASH_ALG_SHA1 & PCRBanks) != 0) {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0xFF;
    } else {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0x00;
    }
    PcrAllocation.count++;
  }
  if ((HASH_ALG_SHA256 & SupportedPCRBanks) != 0) {
    PcrAllocation.pcrSelections[PcrAllocation.count].hash = TPM_ALG_SHA256;
    PcrAllocation.pcrSelections[PcrAllocation.count].sizeofSelect = PCR_SELECT_MAX;
    if ((HASH_ALG_SHA256 & PCRBanks) != 0) {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0xFF;
    } else {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0x00;
    }
    PcrAllocation.count++;
  }
  if ((HASH_ALG_SHA384 & SupportedPCRBanks) != 0) {
    PcrAllocation.pcrSelections[PcrAllocation.count].hash = TPM_ALG_SHA384;
    PcrAllocation.pcrSelections[PcrAllocation.count].sizeofSelect = PCR_SELECT_MAX;
    if ((HASH_ALG_SHA384 & PCRBanks) != 0) {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0xFF;
    } else {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0x00;
    }
    PcrAllocation.count++;
  }
  if ((HASH_ALG_SHA512 & SupportedPCRBanks) != 0) {
    PcrAllocation.pcrSelections[PcrAllocation.count].hash = TPM_ALG_SHA512;
    PcrAllocation.pcrSelections[PcrAllocation.count].sizeofSelect = PCR_SELECT_MAX;
    if ((HASH_ALG_SHA512 & PCRBanks) != 0) {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0xFF;
    } else {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0x00;
    }
    PcrAllocation.count++;
  }
  if ((HASH_ALG_SM3_256 & SupportedPCRBanks) != 0) {
    PcrAllocation.pcrSelections[PcrAllocation.count].hash = TPM_ALG_SM3_256;
    PcrAllocation.pcrSelections[PcrAllocation.count].sizeofSelect = PCR_SELECT_MAX;
    if ((HASH_ALG_SM3_256 & PCRBanks) != 0) {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0xFF;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0xFF;
    } else {
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[0] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[1] = 0x00;
      PcrAllocation.pcrSelections[PcrAllocation.count].pcrSelect[2] = 0x00;
    }
    PcrAllocation.count++;
  }
  Status = Tpm2PcrAllocate (
             TPM_RH_PLATFORM,
             AuthSession,
             &PcrAllocation,
             &AllocationSuccess,
             &MaxPCR,
             &SizeNeeded,
             &SizeAvailable
             );
  DEBUG ((DEBUG_INFO, "Tpm2PcrAllocateBanks call Tpm2PcrAllocate - %r\n", Status));
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  DEBUG ((DEBUG_INFO, "AllocationSuccess - %02x\n", AllocationSuccess));
  DEBUG ((DEBUG_INFO, "MaxPCR            - %08x\n", MaxPCR));
  DEBUG ((DEBUG_INFO, "SizeNeeded        - %08x\n", SizeNeeded));
  DEBUG ((DEBUG_INFO, "SizeAvailable     - %08x\n", SizeAvailable));

Done:
  ZeroMem (&LocalAuthSession.hmac, sizeof (LocalAuthSession.hmac));
  return Status;
}
