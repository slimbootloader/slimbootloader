/** @file
  Definitions for HECI driver

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _HECI_INIT_LIB_H
#define _HECI_INIT_LIB_H


//
// HECI bus function version
//
#define HBM_MINOR_VERSION 0
#define HBM_MAJOR_VERSION 1

/**
  Abstract ME BIOS Boot Path definitions.
  BIOS Boot Path vs. MEI1 Host Firmware Status 1, 4, and 5 registers:
  MeNormalBiosPath -
    If (HECI_FWS_REGISTER.r.CurrentState == ME_STATE_NORMAL AND
      HECI_FWS_REGISTER.r.ErrorCode == ME_ERROR_CODE_NO_ERROR)
    - BIOS takes the normal firmware BIOS path.

  MeFwUpdateBiosPath
    If (HECI_FWS_REGISTER.r.CurrentState == ME_STATE_NORMAL AND
      HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_NORMAL AND
      HECI_FWS_REGISTER.r.FwUpdateInprogress == 0x01 )
    - BIOS starts a 90-seconds loop to wait for the firmware to clear the bit to 0

  MeErrorWithoutDidMsgBiosPath -
    If HECI_FWS_REGISTER.r.ErrorCode == ME_ERROR_CODE_IMAGE_FAILURE
    - The BIOS does not send any Intel MEI messages including DID and EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - MEI 1 device should be enabled by the BIOS so it allows the user to update the new firmware and
      take the firmware out of a recovery or error condition. Hide MEI 2, MEI3, SOL and IDER. In addition,
      Image Failure results in a platform involuntary 30-minute shut down triggered by Intel ME.
      BIOS shall post the warning message as part of the error handling flow.

  MeErrorBiosPath -
    If (HECI_FWS_REGISTER.r.FptBad == 0x01) OR
      (HECI_FWS_REGISTER.r.ErrorCode != ME_ERROR_CODE_NO_ERROR) OR
      (HECI_FWS_REGISTER.r.ErrorCode != ME_ERROR_CODE_IMAGE_FAILURE)
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done message. Moreover,
      the BIOS doesn't even send EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - MEI 1 device should be enabled by the BIOS so it allows the user to update the new firmware and
      take the firmware out of a recovery or error condition. Hide MEI 2, SOL and IDER.

  MeRecoveryBiosPath -
    If HECI_FWS_REGISTER.r.CurrentState == ME_STATE_RECOVERY
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done message. Moreover,
      the BIOS doesn't even send EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - MEI 1 device should be enabled by the BIOS so it allows the user to update the new firmware and
      take the firmware out of a recovery or error condition. Hide MEI 2, SOL and IDER.

  MeDebugModeBiosPath -
    If HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_DEBUG
    - The BIOS does not send any Intel MEI messages including DID and EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI1, MEI2, MEI3, SOL and IDER before OS boot. It means there is no MEI drivers loaded in OS environment.

  MeSwTempDisableBiosPath
    If (HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_SOFT_TEMP_DISABLE)
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done message, Set Me Enable message
      and Global Reset Message.
      Moreover, the BIOS doesn't even send EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI2, MEI3, SOL and IDER before OS boot. It means there is no MEI drivers loaded in OS environment.
      Hides MEI1 device after sending the Set Me Enable message or prior to boot.
      It means there is no MEI drivers loaded in OS environment.

  MeSecoverJmprBiosPath
    If (HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_JMPR)
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done message. Moreover,
      the BIOS doesn't even send EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI1, MEI2, MEI3, SOL and IDER before OS boot. It means there is no MEI drivers loaded in OS environment.

  MeSecoverMeiMsgBiosPath
    If HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_HECI_MSG
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done (DID) message and HMRFPO DISABLE message.
      The HMRFPO DISABLE message is to bring the firmware out of SECOVR_MEI_MSG operation mode back to normal.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI2, MEI3, SOL and IDER before OS boot.
      Hides MEI1 device after sending the HMRFPO DISABLE message.
      It means there is no MEI drivers loaded in OS environment.

  MeEnforcementWithoutDidMsgBiosPath
    If HECI_FW_STS4_REGISTER.r.FwInEnfFlow == 1
    - The BIOS does not send any Intel MEI messages including DID and EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI1, MEI2, MEI3, SOL and IDER before OS boot.
**/

/**
  The FW registers may report multiple status to reflect Me Bios boot path, BIOS will follow the
  prioritized Me Bios boot path to continue boot. If the priority will be changed, then
  BOOT_PATH enumeration shall be adjusted as well to reflect real priority.
**/
typedef enum {
  NormalPath = 0,
  FwUpdatePath,
  ErrorPath,
  RecoveryPath,
  ErrorWithoutDidMsgPath,
  SecoverMeiMsgPath,
  SwTempDisablePath,
  SecoverJmprPath,
  DebugModePath,
  EnforcementWithoutDidMsgPath,
  SpsPath,
  MaxPathValue
} BOOT_PATH;

/**
  Abstract ME devices hidden in BIOS Path definition
  Bit16 - MEI1
  Bit17 - MEI2
  Bit18 - MEI3
  Bit19 - SOL
**/
#define HIDE_MEI1                BIT16
#define HIDE_MEI2                BIT17
#define HIDE_MEI3                BIT18
#define HIDE_SOL                 BIT19
#define HIDE_AMT_DEVICE          HIDE_SOL
#define HIDE_SPS_DEVICE          (HIDE_MEI3 | HIDE_AMT_DEVICE)
#define HIDE_ALL_ME_DEVICE       (HIDE_MEI1 | HIDE_MEI2 | HIDE_MEI3 | HIDE_AMT_DEVICE)

/**
  Abstract MEI message allowance in non MeNormalBiosPath definition
  Bit0 - IMR AND DID messages are allowed to be sent with this BIOS path
  Bit1 - EOP message is allowed to be sent with this BIOS path
  Bit2 - HMRFPO DISABLE message is allowed to be sent with this BIOS path
  Bit3 - SET ME ENABLE message is allowed to be sent with this BIOS path
  Bit4 - GLOBAL RESET message is allowed to be sent with this BIOS path
  BIT5 - HMRFPO messages are allowed to be sent with this ME Bios boot path
  BIT6 - GEN messages are allowed to be sent with this ME Bios boot path
  BIT7 - MEI BUS messages are allowed to be sent with this ME Bios boot path
  BIT8 - Messages that for dynamic ME addressN are allowed to be sent with this ME Bios boot path
  BIT15- All messages are allowed to be sent with this ME Bios boot path
**/
#define IMR_AND_DID_MSG_ALLOWANCE     BIT0
#define HMRFPO_DISABLE_MSG_ALLOWANCE  BIT2
#define SET_ME_ENABLE_MSG_ALLOWANCE   BIT3
#define GLOBAL_RST_MSG_ALLOWANCE      BIT4
#define HMRFPO_GRP_MSG_ALLOWANCE      BIT5  // MKHI_HMRFPO_GROUP_ID messages
#define GEN_GRP_MSG_ALLOWANCE         BIT6  // MKHI_GEN_GROUP_ID messages
#define MEI_BUS_MSG_ALLOWANCE         BIT7  // HECI_MEI_BUS_MESSAGE_ADDR
#define DYN_CLIENT_MSG_ALLOWANCE      BIT8  // Messages for dynamic clients
#define ALL_MSG_ALLOWANCE             BIT15

/**
  Bit 15:0 - MEI message allowance definitions
  Bit 23:16 - ME device hidden control
  Bit 31:24 - BIOS Path value definitions
**/
typedef enum {
  MeNormalBiosPath = ((NormalPath << 24) + ALL_MSG_ALLOWANCE),
  MeFwUpdateBiosPath = ((FwUpdatePath << 24) + ALL_MSG_ALLOWANCE),
  MeErrorBiosPath = ((ErrorPath << 24) + HIDE_MEI2 + HIDE_AMT_DEVICE + IMR_AND_DID_MSG_ALLOWANCE),
  MeRecoveryBiosPath = ((RecoveryPath << 24) + HIDE_MEI2 + HIDE_AMT_DEVICE + IMR_AND_DID_MSG_ALLOWANCE + MEI_BUS_MSG_ALLOWANCE + DYN_CLIENT_MSG_ALLOWANCE + HMRFPO_GRP_MSG_ALLOWANCE),
  MeErrorWithoutDidMsgBiosPath = ((ErrorWithoutDidMsgPath << 24) + HIDE_MEI2 + HIDE_MEI3 + HIDE_AMT_DEVICE),
  MeSecoverMeiMsgBiosPath = ((SecoverMeiMsgPath << 24) + HIDE_MEI2 + HIDE_MEI3 + HIDE_AMT_DEVICE + IMR_AND_DID_MSG_ALLOWANCE + HMRFPO_DISABLE_MSG_ALLOWANCE),
  MeSwTempDisableBiosPath = ((SwTempDisablePath << 24) + HIDE_MEI2 + HIDE_MEI3 + HIDE_AMT_DEVICE + IMR_AND_DID_MSG_ALLOWANCE + SET_ME_ENABLE_MSG_ALLOWANCE + GLOBAL_RST_MSG_ALLOWANCE),
  MeSecoverJmprBiosPath = ((SecoverJmprPath << 24) + HIDE_ALL_ME_DEVICE + IMR_AND_DID_MSG_ALLOWANCE),
  MeDebugModeBiosPath = ((DebugModePath << 24) + HIDE_ALL_ME_DEVICE + IMR_AND_DID_MSG_ALLOWANCE),
  MeEnforcementWithoutDidMsgBiosPath = ((EnforcementWithoutDidMsgPath << 24) + HIDE_ALL_ME_DEVICE),
  MeSpsOprBiosPath = ((SpsPath << 24) + ALL_MSG_ALLOWANCE + HIDE_SPS_DEVICE),
  MeSpsRcvBiosPath = ((SpsPath << 24) + IMR_AND_DID_MSG_ALLOWANCE + HMRFPO_GRP_MSG_ALLOWANCE + GEN_GRP_MSG_ALLOWANCE + HIDE_SPS_DEVICE)
} ME_BIOS_BOOT_PATH;

//
// Prototypes
//
/**
  This function provides a standard way to verify the HECI cmd and MBAR regs
  in its PCI cfg space are setup properly and that the local mHeciContext
  variable matches this info.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval HeciMemBar              HECI Memory BAR.
                                  0 means invalid BAR value returned.
**/
UINTN
CheckAndFixHeciForAccess (
  IN HECI_DEVICE                  HeciDev
  );

/**
  Waits for the ME to report that it is ready for communication over the HECI
  interface.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             ME is ready
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
WaitForMEReady (
  IN HECI_DEVICE                  HeciDev
  );

/**
  Check ME Boot path. The function provides ME BIOS boot path required based on current
  HECI1 FW Status Register.  HECI1 must be enabled before invoke the function.
  The FW registers may report multiple status to reflect Me Bios boot path,
  BIOS will follow the prioritized Me Bios boot path to continue boot.
  If the priority will be changed, then BOOT_PATH enumerated type shall be
  adjusted as well to reflect real priority.

  @param[out] MeBiosPath          Pointer for ME BIOS boot path report

  @retval EFI_SUCCESS             MeBiosPath copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeBiosPath is invalid
**/
EFI_STATUS
CheckMeBootPath (
  OUT ME_BIOS_BOOT_PATH           *MeBiosPath
  );

/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        No HECI device
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED         HECI MSG is unsupported
**/
EFI_STATUS
EFIAPI
HeciInitialize (
  IN HECI_DEVICE                  HeciDev
  );

/**
  Heci Re-initializes it for Host

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_STATUS              Status code returned by ResetHeciInterface
**/
EFI_STATUS
EFIAPI
HeciReInitialize (
  IN HECI_DEVICE                  HeciDev
  );

/**
  Reads a message from the ME across HECI.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in][out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in][out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciReceive (
  IN      HECI_DEVICE             HeciDev,
  IN      UINT32                  Blocking,
  IN OUT  UINT32                  *MessageBody,
  IN OUT  UINT32                  *Length
  );

/**
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSend (
  IN HECI_DEVICE                  HeciDev,
  IN UINT32                       *Message,
  IN UINT32                       Length,
  IN UINT8                        HostAddress,
  IN UINT8                        MeAddress
  );

/**
  Function sends one message (of any length) through the HECI circular buffer with retry mechanism.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSendWithRetry (
  IN HECI_DEVICE                  HeciDev,
  IN UINT32                       *Message,
  IN UINT32                       Length,
  IN UINT8                        HostAddress,
  IN UINT8                        MeAddress
  );

/**
  Function sends one message through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in][out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in][out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSendwAck (
  IN      HECI_DEVICE             HeciDev,
  IN OUT  UINT32                  *Message,
  IN      UINT32                  Length,
  IN OUT  UINT32                  *RecLength,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MeAddress
  );

/**
  Function sends one message through the HECI circular buffer and waits
  for the corresponding ACK message with retry mechanism.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in][out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in][out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSendwAckWithRetry (
  IN      HECI_DEVICE             HeciDev,
  IN OUT  UINT32                  *Message,
  IN      UINT32                  Length,
  IN OUT  UINT32                  *RecLength,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MeAddress
  );

/**
  Me reset and waiting for ready

  @param[in] Delay                The biggest waiting time

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Me is ready
**/
EFI_STATUS
EFIAPI
MeResetWait (
  IN  HECI_DEVICE                 HeciDev,
  IN  UINT32                      Delay
  );

/**
  Function forces a reinit of the heci interface by following the reset heci interface via host algorithm
  in HPS 0.90 doc 4-17-06

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
EFI_STATUS
EFIAPI
ResetHeciInterface (
  IN  HECI_DEVICE                 HeciDev
  );

/**
  Get an abstract Intel ME State from Firmware Status Register.
  This is used to control BIOS flow for different Intel ME
  functions

  @param[out] MeStatus            Pointer for status report
                                  see MeState.h - Abstract ME status definitions.

  @retval EFI_SUCCESS             MeStatus copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeStatus is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeStatus (
  OUT UINT32                      *MeStatus
  );

/**
  Return ME Mode

  @param[out] MeMode              Pointer for ME Mode report

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  OUT UINT32                       *MeMode
  );


/**
  Read the ME policy to see if HECI timeouts are enabled.
  @retval TRUE             Timeout is enabled
  @retval FALSE            Timeout is disabled
**/
extern
BOOLEAN
MeHeciTimeoutsEnabled (
  VOID
  );

/**
  Read the ME policy to see if HECI retry is enabled.

  @retval TRUE             HECI retry is enabled
  @retval FALSE            HECI retry is disabled
**/
BOOLEAN
MeHeciRetryEnabled (
  VOID
  );

/**
  Read the ME policy to see if HECI message check is enabled for Bios Boot Path.

  @retval TRUE             HECI message check is enabled
  @retval FALSE            HECI message check is disabled
**/
BOOLEAN
MeHeciMessageCheckEnabled (
  VOID
  );

#endif // _HECI_INIT_LIB_H
