/** @file
  Implementation file for Heci ME Extended Measured boot functionality

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/HeciRegs.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HeciMeExtLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/MemoryAllocationLib.h>
#include <MeChipset.h>
#include <Library/HeciLib.h>
#include <PcieRegs.h>
#include <MkhiMsgsAdl.h>
#include <CoreBiosMsg.h>
#include <BupMsgs.h>

/**
  Send Get Current CSME Measured Boot State

  @param[out] GetMeasuredBootState    Pointer to get CSME Measured boot status.

  @retval EFI_UNSUPPORTED         Current CSME mode/version doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
EFIAPI
HeciGetMeasuredBootState (
  OUT UINT8  *GetMeasuredBootState
  )
{
  GET_MEASURED_BOOT_BUFFER   GetMeasuredBoot;
  UINT32                     Length;
  UINT32                     RecvLength;
  EFI_STATUS                 Status;
  UINT32                     MeMode;

  if (GetMeasuredBootState == NULL) {
    return EFI_INVALID_PARAMETER;
  }

   Status = HeciGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  GetMeasuredBoot.Request.MkhiHeader.Data = 0;
  GetMeasuredBoot.Request.MkhiHeader.Fields.GroupId = MKHI_GEN_GROUP_ID;
  GetMeasuredBoot.Request.MkhiHeader.Fields.Command = GEN_GET_MEASURED_BOOT_STATE_CMD;
  Length = sizeof (GET_MEASURED_BOOT);
  RecvLength = sizeof (GET_MEASURED_BOOT_ACK);
  ///
  /// Send Get Measure Boot State Request to ME
  ///

  Status = HeciSendwAck (
                   HECI1_DEVICE,
                   (UINT32 *)&GetMeasuredBoot,
                   Length,
                   &RecvLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_MKHI_MESSAGE_ADDR
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HeciGetMeasuredBootState: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }


  if ((GetMeasuredBoot.Response.MkhiHeader.Fields.Command == GEN_GET_MEASURED_BOOT_STATE_CMD) &&
      (GetMeasuredBoot.Response.MkhiHeader.Fields.IsResponse == 1) &&
      (GetMeasuredBoot.Response.MkhiHeader.Fields.Result == MkhiStatusSuccess)) {
     *GetMeasuredBootState = GetMeasuredBoot.Response.Data;
  } else {
    DEBUG ((DEBUG_ERROR, "GetMeasuredBoot.Response.Data Error\n"));
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

/**
  Send Set Measured Boot State to Enable or Disable CSME Measurement

  @param[in]  SetMeasuredBootState  Set Measured boot state to Enable/Disable CSME Extended Measured boot

  @retval EFI_UNSUPPORTED         Current CSME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
EFIAPI
HeciSetMeasuredBootState (
  IN UINT8 SetMeasuredBootState
  )
{
  SET_MEASURED_BOOT_BUFFER   SetMeasureBoot;
  UINT32                     Length;
  UINT32                     RecvLength;
  EFI_STATUS                 Status;
  UINT32                     MeMode;

  Status = HeciGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  SetMeasureBoot.Request.MkhiHeader.Data = 0;
  SetMeasureBoot.Request.MkhiHeader.Fields.GroupId = MKHI_GEN_GROUP_ID;
  SetMeasureBoot.Request.MkhiHeader.Fields.Command = GEN_SET_MEASURED_BOOT_STATE_CMD;
  SetMeasureBoot.Request.Data = SetMeasuredBootState & 0x01;
  Length = sizeof (SET_MEASURED_BOOT);
  RecvLength = sizeof (SET_MEASURED_BOOT_ACK);
  ///
  /// Send Set Measure Boot Enable/Disable Request to ME
  ///
  Status = HeciSendwAck (
                   HECI1_DEVICE,
                   (UINT32 *)&SetMeasureBoot,
                   Length,
                   &RecvLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_MKHI_MESSAGE_ADDR
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HeciSetMeasuredBootState: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if (SetMeasureBoot.Request.MkhiHeader.Fields.Result != MkhiStatusSuccess) {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

/**
  This function sends a command to CSME to get Log information of Measurements provided in
  CSME Extend Registers for BIOS to extend to TPM-PCR registers.

  @param[out]     EventLogBuffer    Pointer to received Event log buffer
  @param[in]      RemainingBufSize  Size of available buffer to collect event log
  @param[out]     ReceivedLogSize   Size of received event log
  @param[in, out] LogEntryIndex     On input - Index of first / starting log entry in the buffer. On output - Index of last log entry returned
  @param[out]     RemainingEntries  Pointer to Remaining Entries (0 - no more entries in a log; 1 - additional iteration(s) is(are) required)

  @retval EFI_SUCCESS             Log received successfully
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout or transition failed
  within allowed time limit
**/
EFI_STATUS
EFIAPI
HeciGetErLog (
  OUT UINT8        *EventLogBuffer,
  IN UINT32        RemainingBufSize,
  IN OUT UINT32    *ReceivedLogSize,
  IN OUT UINT8     *LogEntryIndex,
  OUT UINT8        *RemainingEntries
  )
{
  EFI_STATUS          Status;
  UINT32              Length;
  UINT32              RecvLength;
  GET_ER_LOG_BUFFER   *GetErLog;
  UINT32              MeMode;

  GetErLog = NULL;

  Status = HeciGetMeMode (&MeMode);
  if (EFI_ERROR(Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Length = sizeof (GET_ER_LOG_CMD_REQ);
  RecvLength = sizeof (GET_ER_LOG_CMD_RESP) + RemainingBufSize;     // Size of Response structure + Available buffer size

  GetErLog = AllocateZeroPool (RecvLength);
  if (GetErLog == NULL) {
    DEBUG ((DEBUG_ERROR, "HeciGetErLog Error: Could not allocate Memory\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  GetErLog->Request.FirstLogEntryIndex = *LogEntryIndex;
  GetErLog->Request.MkhiHeader.Data = 0;
  GetErLog->Request.MkhiHeader.Fields.GroupId = BUP_COMMON_GROUP_ID;
  GetErLog->Request.MkhiHeader.Fields.Command = GET_ER_LOG_CMD;
  GetErLog->Request.Flags = 0;

  Status = HeciSendwAck (
                   HECI1_DEVICE,
                   (UINT32 *)GetErLog,
                   Length,
                   &RecvLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_MKHI_MESSAGE_ADDR
                   );

  if (!EFI_ERROR (Status)) {
    if ((GetErLog->Response.MkhiHeader.Fields.Command == GET_ER_LOG_CMD) &&
        (GetErLog->Response.MkhiHeader.Fields.IsResponse == 1) &&
        (GetErLog->Response.MkhiHeader.Fields.Result == MkhiStatusSuccess)) {
      if (GetErLog->Response.DataLength > RemainingBufSize) {
        Status = EFI_BUFFER_TOO_SMALL;
      } else {
        CopyMem (EventLogBuffer, GetErLog->Response.LogData, GetErLog->Response.DataLength);
        *ReceivedLogSize = GetErLog->Response.DataLength;
        *RemainingEntries = GetErLog->Response.RemainingEntries;
        *LogEntryIndex = GetErLog->Response.LastLogEntryIndex;
      }
    } else {
      Status = EFI_DEVICE_ERROR;
    }
  }
  FreePool (GetErLog);

  return Status;
}

/**
  This function gets sha256/sha384 CSME Measured data from HECI1 MMIO space (HECI1_HER1 - HECI1_HER12).

  @param[in]      Her             - Pointer to receive HER Data buffer of 12 DWORDS
  @param[in]      HerOffset       - Offset of first HER register in MMIO Space
  @param[in]      HerMaxRegCount  - Max Register count to read HER registers

  @retval EFI_SUCCESS             Event data read successfully
  @retval EFI_INVALID_PARAMETER   Input parameters are not valid
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_NOT_READY           Device is not ready
**/
EFI_STATUS
EFIAPI
HeciGetHerHashData (
  IN UINT32    *Her,
  IN UINT32    HerOffset,
  IN UINT8     HerMaxRegCount
  )
{
  UINT64              MemBar;
  UINTN               HeciMemBar;
  UINT8               Index;

  if (Her == NULL) {
    DEBUG ((DEBUG_ERROR, "HeciGetHerDataSha384: Invalid parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  MemBar =  HeciPciRead32 (HECI_FUNCTION_NUMBER, PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0;
  if ((HeciPciRead32 (HECI_FUNCTION_NUMBER, PCI_BASE_ADDRESSREG_OFFSET) & B_PCI_BAR_MEMORY_TYPE_MASK) == B_PCI_BAR_MEMORY_TYPE_64) {
    MemBar += (UINT64) HeciPciRead32 (HECI_FUNCTION_NUMBER, (PCI_BASE_ADDRESSREG_OFFSET + 4)) << 32;
  }

  if (MemBar == 0) {
    DEBUG ((DEBUG_WARN, "MMIO Bar for HECI device isn't programmed\n"));
    return EFI_DEVICE_ERROR;
  }
  HeciMemBar = (UINTN) MemBar;

  ///
  /// Check whether HECI device is in D0 State
  ///
  if ((MmioRead32 (HeciMemBar + D0I3C) & BIT2) == BIT2) {
    DEBUG ((DEBUG_ERROR, "HeciGetHerDataFromMmio Error: HECI Device is in D0I3 state, not recommended to read HER data\n"));
    return EFI_NOT_READY;
  }

  ///
  /// Read HER data from MMIO space
  ///
  for (Index = 0; Index < HerMaxRegCount; Index++) {
    *Her = MmioRead32 (HeciMemBar + (HerOffset + (Index * sizeof (UINT32))));
    Her++;
  }

#if 0
  DEBUG ((DEBUG_INFO, "CSME Extended Measured Boot: Hash from HER register\n"));
  DumpHex(2, 0, HerMaxRegCount * sizeof(UINT32), Her);
#endif

  return EFI_SUCCESS;
}

/**
  The command is used to get the information about OEM keys, such as
  key hash, revocation status, validity.
  @param[in, out] OemKeyStatus    Status of OEM keys
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
**/
EFI_STATUS
HeciGetOemKeyStatus(
  IN OUT  OEM_KEY_STATUS *OemKeyStatus
  )
{
  EFI_STATUS               Status;
  UINT32                   Length;
  UINT32                   RecvLength;
  OEM_KEY_STATUS_BUFFER    OemKeyStatusBuff;
  UINT32                   MeMode;

  Status = HeciGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&OemKeyStatusBuff, sizeof (OemKeyStatusBuff));
  OemKeyStatusBuff.Request.MkhiHeader.Data           = 0;
  OemKeyStatusBuff.Request.MkhiHeader.Fields.GroupId = MKHI_MCA_GROUP_ID;
  OemKeyStatusBuff.Request.MkhiHeader.Fields.Command = MCA_GET_OEM_KEY_STATUS_CMD;
  Length                                         = sizeof (OEM_KEY_STATUS_REQ);
  RecvLength                                     = sizeof (OEM_KEY_STATUS_ACK);

  Status = HeciSendwAck (
                   HECI1_DEVICE,
                   (UINT32 *) &OemKeyStatusBuff,
                   Length,
                   &RecvLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_MCHI_MESSAGE_ADDR
                   );

  if (!EFI_ERROR (Status)) {
      if ((OemKeyStatusBuff.Response.MkhiHeader.Fields.Command == MCA_GET_OEM_KEY_STATUS_CMD) &&
            (OemKeyStatusBuff.Response.MkhiHeader.Fields.IsResponse == 1) &&
            (OemKeyStatusBuff.Response.MkhiHeader.Fields.Result == 0)) {
        CopyMem (OemKeyStatus, &OemKeyStatusBuff.Response.OemKeyStatus, sizeof(OEM_KEY_STATUS));
    } else {
      Status = EFI_DEVICE_ERROR;
    }
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HeciGetOemKeyStatus Result: %08x \n", OemKeyStatusBuff.Response.MkhiHeader.Fields.Result));
  }

  return Status;
}


/**
  This command is used to indicate to the FW to revoke a OEM key.
  It should be executed when MBP data indicates that a key is pending for
  revocation.
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
**/
EFI_STATUS
HeciRevokeOemKey (
  VOID
  )
{
  EFI_STATUS               Status;
  UINT32                   Length;
  UINT32                   RecvLength;
  OEM_KEY_REVOKE_BUFFER    OemKeyRevoke;
  UINT32                   MeMode;

  Status = HeciGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&OemKeyRevoke, sizeof (OEM_KEY_REVOKE_BUFFER));
  OemKeyRevoke.Request.MkhiHeader.Data           = 0;
  OemKeyRevoke.Request.MkhiHeader.Fields.GroupId = MKHI_MCA_GROUP_ID;
  OemKeyRevoke.Request.MkhiHeader.Fields.Command = MCA_REVOKE_OEM_KEY_HASH_CMD;
  Length                                         = sizeof (OEM_KEY_REVOKE);
  RecvLength                                     = sizeof (OEM_KEY_REVOKE_ACK);

  Status = HeciSendwAck (
                   HECI1_DEVICE,
                   (UINT32 *) &OemKeyRevoke,
                   Length,
                   &RecvLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_MCHI_MESSAGE_ADDR
                   );

  if (!EFI_ERROR (Status) && OemKeyRevoke.Response.MkhiHeader.Fields.Result != MkhiStatusSuccess) {
    Status = EFI_DEVICE_ERROR;
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HeciRevokeOemKey Result: %08x \n", OemKeyRevoke.Response.MkhiHeader.Fields.Result));
  }

  return Status;
}
