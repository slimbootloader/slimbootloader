/** @file
  Definitions for HECI driver

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _HECI_CORE_H
#define _HECI_CORE_H

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include "MeChipset.h"

#define HeciPciRead32(func, reg)     PciRead32 (PCI_LIB_ADDRESS(ME_BUS, ME_DEVICE_NUMBER, func, reg))
#define HeciPciRead16(func, reg)     PciRead16 (PCI_LIB_ADDRESS(ME_BUS, ME_DEVICE_NUMBER, func, reg))
#define HeciPciOr16(func, reg, val)  PciOr16 (PCI_LIB_ADDRESS(ME_BUS, ME_DEVICE_NUMBER, func, reg), val)
#define HeciPciOr8(func, reg, val)   PciOr8 (PCI_LIB_ADDRESS(ME_BUS, ME_DEVICE_NUMBER, func, reg), val)
#define HeciPciAnd8(func, reg, val)  PciAnd8 (PCI_LIB_ADDRESS(ME_BUS, ME_DEVICE_NUMBER, func, reg), val)

#define NON_BLOCKING                 0
#define BLOCKING                     1


/**
  Abstract ME BIOS Boot Path definitions.
  BIOS Boot Path vs. MEI1 Host Firmware Status 1, 4, and 5 registers:
  MeNormalBiosPath -
    If (HECI_FWS_REGISTER.r.CurrentState == ME_STATE_NORMAL AND
      HECI_FWS_REGISTER.r.ErrorCode == ME_ERROR_CODE_NO_ERROR)
    - BIOS takes the normal firmware BIOS path.

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
    - The BIOS does not send any Intel MEI messages except DID and EOP message.
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
  ErrorPath,
  RecoveryPath,
  ErrorWithoutDidMsgPath,
  SecoverMeiMsgPath,
  SwTempDisablePath,
  SecoverJmprPath,
  DebugModePath,
  EnforcementWithoutDidMsgPath,
  SpsPath,
  SpsRcvPath,
  MaxPathValue
} ME_BIOS_BOOT_PATH;

/**
  Abstract devices map for CSME devices to be hidden
  Bit0 - MEI1
  Bit1 - MEI2
  Bit2 - MEI3
  Bit3 - MEI4
  Bit7 - SOL
**/
typedef UINT8 ME_DEV_EXPOSURE;

#define HIDE_MEI1                BIT0
#define HIDE_MEI2                BIT1
#define HIDE_MEI3                BIT2
#define HIDE_MEI4                BIT3
#define HIDE_SOL                 BIT7

#define HIDE_AMT_DEVICE          HIDE_SOL
#define HIDE_ALL_ME_DEVICE       (HIDE_MEI1|HIDE_MEI2|HIDE_MEI3|HIDE_MEI4|HIDE_AMT_DEVICE)
#define HIDE_SPS_DEVICE          (HIDE_MEI3|HIDE_MEI4|HIDE_AMT_DEVICE)

/**
  Abstract MEI message allowance in non MeNormalBiosPath definition
  Bit0 - IMR and DID messages are allowed to be sent with this BIOS path
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
typedef UINT16 MESSAGE_ALLOWANCE;

#define IMR_AND_DID_MSG_ALLOWANCE     BIT0
#define HMRFPO_DISABLE_MSG_ALLOWANCE  BIT2
#define SET_ME_ENABLE_MSG_ALLOWANCE   BIT3
#define GLOBAL_RST_MSG_ALLOWANCE      BIT4
#define HMRFPO_GRP_MSG_ALLOWANCE      BIT5
#define GEN_GRP_MSG_ALLOWANCE         BIT6
#define MEI_BUS_MSG_ALLOWANCE         BIT7
#define DYN_CLIENT_MSG_ALLOWANCE      BIT8
#define ALL_MSG_ALLOWANCE             BIT15


//
// Timeout values based on HPET
//
#define HECI_WAIT_DELAY             1000      ///< 1ms timeout for IO delay
#define HECI_INIT_TIMEOUT           15000000  ///< 15sec timeout in microseconds
#define HECI_READ_TIMEOUT           5000000   ///< 5sec timeout in microseconds
#define HECI_SEND_TIMEOUT           5000000   ///< 5sec timeout in microseconds

#define HECI_CB_OVERFLOW            0xFFFFFFFF ///< Circular buffer overflow

///
/// Header for all HECI message message placed in an Intel MEI circular buffer
///
typedef union {
  UINT32  Data;
  struct {
    /**
      This is the logical address of the Intel ME client of the message. This address is assigned
      during ME firmware initialization.
    **/
    UINT32  MeAddress       : 8;
    /**
      This is the logical address of the Host client of the message. This address is assigned
      when the host client registers itself with the Host MEI driver.
    **/
    UINT32  HostAddress     : 8;
    /**
      This is the message length in bytes, not including the MEI_MESSAGE_HEADER. A value of 0
      indicates no additional bytes are part of the message.
    **/
    UINT32  Length          : 9;
    UINT32  Reserved        : 6;
    UINT32  MessageComplete : 1; ///< Indicates the last message of the list
  } Fields;
} HECI_MESSAGE_HEADER;

/**
  Reads a HECI message from ISH.

  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in, out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in, out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             Message read and placed in buffer
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
  @retval EFI_UNSUPPORTED         Response was a multipacket message, not supported in BIOS yet
  @retval any other status        Error caused by ISH firmware
**/
EFI_STATUS
IshHeciReceive (
  IN      UINT32                  Blocking,
  IN OUT  UINT32                  *MessageBody,
  IN OUT  UINT32                  *Length
  );

/**
  Sends a HECI message to ISH.

  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the subsystem the message is being sent to.

  @retval EFI_SUCCESS             Message sent
  @retval EFI_UNSUPPORTED         Message is too big to send as single packet; multipacket messages not supported in BIOS yet
  @retval any other status        Error caused by ISH firmware
**/
EFI_STATUS
IshHeciSend (
  IN UINT32                       *Message,
  IN UINT32                       Length,
  IN UINT8                        HostAddress,
  IN UINT8                        MeAddress
  );

/**
  Function sends one message through the ISH HECI and waits
  for the corresponding response message

  @param[in, out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in, out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Message sent and response received
  @retval EFI_UNSUPPORTED         Message is too big to send / receive as single packet; multipacket messages not supported in BIOS yet
  @retval any other status        Error caused by ISH firmware
**/
EFI_STATUS
IshHeciSendAndReceive (
  IN OUT  UINT32                  *Message,
  IN      UINT32                  Length,
  IN OUT  UINT32                  *RecLength,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MeAddress
  );

#endif // _HECI_CORE_H
