/** @file
  Implement TPM2 Hierarchy related command.

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
  TPMI_RH_HIERARCHY_AUTH    AuthHandle;
  UINT32                    AuthorizationSize;
  TPMS_AUTH_COMMAND         AuthSession;
  TPM2B_AUTH                NewAuth;
} TPM2_HIERARCHY_CHANGE_AUTH_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER       Header;
  UINT32                     ParameterSize;
  TPMS_AUTH_RESPONSE         AuthSession;
} TPM2_HIERARCHY_CHANGE_AUTH_RESPONSE;

typedef struct {
  TPM2_COMMAND_HEADER       Header;
  TPMI_RH_HIERARCHY         AuthHandle;
  UINT32                    AuthorizationSize;
  TPMS_AUTH_COMMAND         AuthSession;
  TPMI_RH_HIERARCHY         Hierarchy;
  TPMI_YES_NO               State;
} TPM2_HIERARCHY_CONTROL_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER       Header;
  UINT32                     ParameterSize;
  TPMS_AUTH_RESPONSE         AuthSession;
} TPM2_HIERARCHY_CONTROL_RESPONSE;

#pragma pack()

/**
  This command allows the authorization secret for a hierarchy or lockout to be changed using the current
  authorization value as the command authorization.

  @param[in] AuthHandle        TPM_RH_LOCKOUT, TPM_RH_ENDORSEMENT, TPM_RH_OWNER or TPM_RH_PLATFORM+{PP}
  @param[in] AuthSession       Auth Session context
  @param[in] NewAuth           New authorization secret

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2HierarchyChangeAuth (
  IN TPMI_RH_HIERARCHY_AUTH    AuthHandle,
  IN TPMS_AUTH_COMMAND         *AuthSession,
  IN TPM2B_AUTH                *NewAuth
  )
{
  EFI_STATUS                           Status;
  TPM2_HIERARCHY_CHANGE_AUTH_COMMAND   Cmd;
  TPM2_HIERARCHY_CHANGE_AUTH_RESPONSE  Res;
  UINT32                               CmdSize;
  UINT32                               RespSize;
  UINT8                                *Buffer;
  UINT32                               SessionInfoSize;
  UINT8                                *ResultBuf;
  UINT32                               ResultBufSize;

  //
  // Construct command
  //
  Cmd.Header.tag          = SwapBytes16 (TPM_ST_SESSIONS);
  Cmd.Header.paramSize    = SwapBytes32 (sizeof (Cmd));
  Cmd.Header.commandCode  = SwapBytes32 (TPM_CC_HierarchyChangeAuth);
  Cmd.AuthHandle          = SwapBytes32 (AuthHandle);

  //
  // Add in Auth session
  //
  Buffer = (UINT8 *)&Cmd.AuthSession;

  // sessionInfoSize
  SessionInfoSize = CopyAuthSessionCommand (AuthSession, Buffer);
  Buffer += SessionInfoSize;
  Cmd.AuthorizationSize = SwapBytes32 (SessionInfoSize);

  // New Authorization size
  WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16 (NewAuth->size));
  Buffer += sizeof (UINT16);

  // New Authorizeation
  CopyMem (Buffer, NewAuth->buffer, NewAuth->size);
  Buffer += NewAuth->size;

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
    DEBUG ((DEBUG_ERROR, "HierarchyChangeAuth: Failed ExecuteCommand: Buffer Too Small\r\n"));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Validate response headers
  //
  RespSize = SwapBytes32 (Res.Header.paramSize);
  if (RespSize > sizeof (Res)) {
    DEBUG ((DEBUG_ERROR, "HierarchyChangeAuth: Response size too large! %d\r\n", RespSize));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Fail if command failed
  //
  if (SwapBytes32 (Res.Header.responseCode) != TPM_RC_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "HierarchyChangeAuth: Response Code error! 0x%08x\r\n", SwapBytes32 (Res.Header.responseCode)));
    Status = EFI_DEVICE_ERROR;
    goto Done;
  }

Done:
  //
  // Clear AuthSession Content
  //
  ZeroMem (&Cmd, sizeof (Cmd));
  ZeroMem (&Res, sizeof (Res));
  return Status;
}

/**
  This command enables and disables use of a hierarchy.

  @param[in] AuthHandle        TPM_RH_ENDORSEMENT, TPM_RH_OWNER or TPM_RH_PLATFORM+{PP}
  @param[in] AuthSession       Auth Session context
  @param[in] Hierarchy         Hierarchy of the enable being modified
  @param[in] State             YES if the enable should be SET,
                               NO if the enable should be CLEAR

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2HierarchyControl (
  IN TPMI_RH_HIERARCHY         AuthHandle,
  IN TPMS_AUTH_COMMAND         *AuthSession,
  IN TPMI_RH_HIERARCHY         Hierarchy,
  IN TPMI_YES_NO               State
  )
{
  EFI_STATUS                       Status;
  TPM2_HIERARCHY_CONTROL_COMMAND   Cmd;
  TPM2_HIERARCHY_CONTROL_RESPONSE  Res;
  UINT32                           CmdSize;
  UINT32                           RespSize;
  UINT8                            *Buffer;
  UINT32                           SessionInfoSize;
  UINT8                            *ResultBuf;
  UINT32                           ResultBufSize;

  //
  // Construct command
  //
  Cmd.Header.tag          = SwapBytes16 (TPM_ST_SESSIONS);
  Cmd.Header.paramSize    = SwapBytes32 (sizeof (Cmd));
  Cmd.Header.commandCode  = SwapBytes32 (TPM_CC_HierarchyControl);
  Cmd.AuthHandle          = SwapBytes32 (AuthHandle);

  //
  // Add in Auth session
  //
  Buffer = (UINT8 *)&Cmd.AuthSession;

  // sessionInfoSize
  SessionInfoSize = CopyAuthSessionCommand (AuthSession, Buffer);
  Buffer += SessionInfoSize;
  Cmd.AuthorizationSize = SwapBytes32 (SessionInfoSize);

  WriteUnaligned32 ((UINT32 *)Buffer, SwapBytes32 (Hierarchy));
  Buffer += sizeof (UINT32);

  * (UINT8 *)Buffer = State;
  Buffer++;

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
    DEBUG ((DEBUG_ERROR, "HierarchyControl: Failed ExecuteCommand: Buffer Too Small\r\n"));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Validate response headers
  //
  RespSize = SwapBytes32 (Res.Header.paramSize);
  if (RespSize > sizeof (Res)) {
    DEBUG ((DEBUG_ERROR, "HierarchyControl: Response size too large! %d\r\n", RespSize));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Fail if command failed
  //
  if (SwapBytes32 (Res.Header.responseCode) != TPM_RC_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "HierarchyControl: Response Code error! 0x%08x\r\n", SwapBytes32 (Res.Header.responseCode)));
    Status = EFI_DEVICE_ERROR;
    goto Done;
  }

Done:
  //
  // Clear AuthSession Content
  //
  ZeroMem (&Cmd, sizeof (Cmd));
  ZeroMem (&Res, sizeof (Res));
  return Status;
}
