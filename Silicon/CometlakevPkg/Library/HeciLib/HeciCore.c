/** @file
  Heci driver core for PEI & DXE phases, determines the HECI device and
  initializes it.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PchInfoLib.h>
#include <IndustryStandard/Pci22.h>
#include <HeciHelper.h>
#include <HeciCore.h>
#include <HeciHpet.h>
#include <Service/HeciService.h>
#include <Library/HeciLib.h>
#include <Library/BootloaderCoreLib.h>
#include <CseMsg.h>

#undef     DEBUG_INFO
#define    DEBUG_INFO    DEBUG_VERBOSE

const HECI_SERVICE   mHeciService = {
  .Header.Signature  = HECI_SERVICE_SIGNATURE,
  .Header.Version    = HECI_SERVICE_VERSION,
  .SimpleHeciCommand = NULL,
  .HeciUserCommand   = NULL,
};

/**
  Read the ME policy to see if HECI retry is enabled.

  @retval TRUE             HECI retry is enabled
  @retval FALSE            HECI retry is disabled
**/
BOOLEAN
MeHeciRetryEnabled (
  VOID
  )
{
  return TRUE;
}

/**
  Read the ME policy to see if HECI message check is enabled for Bios Boot Path.

  @retval TRUE             HECI message check is enabled
  @retval FALSE            HECI message check is disabled
**/
BOOLEAN
MeHeciMessageCheckEnabled (
  VOID
  )
{
  return TRUE;
}


/**
  Read the ME policy to see if HECI timeouts are enabled.

  @retval TRUE             Timeout is enabled
  @retval FALSE            Timeout is disabled
**/
BOOLEAN
MeHeciTimeoutsEnabled (
  VOID
  )
{
  return FALSE;
}

/**
  Checks if HECI reset has occurred.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval TRUE                    HECI reset occurred
  @retval FALSE                   No HECI reset occurred
**/
BOOLEAN
CheckForHeciReset (
  IN HECI_DEVICE                  HeciDev
  )
{
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;
  UINTN                           HeciMBAR;

  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);

  ///
  /// Init Host & ME CSR
  ///
  HeciRegHCsr.ul    = MmioRead32 (HeciMBAR + H_CSR);
  HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);

  if ((HeciRegMeCsrHa.r.ME_RDY_HRA == 0) || (HeciRegHCsr.r.H_RDY == 0)) {
    return TRUE;
  }

  return FALSE;
}


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
  )
{
  UINT64  HeciBaseAddress;
  UINT32  Buffer[2];
  UINT8   Data8;

  ///
  /// Check if HECI_MBAR has changed
  ///
  HeciBaseAddress = PCI_SEGMENT_LIB_ADDRESS (ME_SEGMENT, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER + HeciDev, 0);

  ///
  /// Check for HECI PCI device availability
  ///
  if (PciSegmentRead16 (HeciBaseAddress + PCI_DEVICE_ID_OFFSET) == 0xFFFF) {
    return 0;
  }

  Buffer[0] = PciSegmentRead32 (HeciBaseAddress + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0;
  Buffer[1] = 0x0;
  if ((PciSegmentRead32 (HeciBaseAddress + PCI_BASE_ADDRESSREG_OFFSET) & 0x6) == 0x4) {
    Buffer[1] = PciSegmentRead32 (HeciBaseAddress + (PCI_BASE_ADDRESSREG_OFFSET + 4));
  }

  if (Buffer[0] == 0x0 && Buffer[1] == 0x0) {
    DEBUG ((
      DEBUG_WARN,
      "CheckAndFixHeciForAccess () -MMIO Bar for HECI%d isn't programmed in this phase\n",
      HECI_NAME_MAP (HeciDev)
      ));
  } else if (Buffer[0] == 0xFFFFFFFF) {
    DEBUG ((DEBUG_WARN, "HECI%d is not enabled in this phase\n", HECI_NAME_MAP (HeciDev)));
  } else {
    ///
    /// Enable HECI MSE
    ///
    if ((IsPchLp () && (PchStepping () < PCH_D0)) || (IsPchH () && (PchStepping () < PCH_B0))) {
      Data8 = PciSegmentRead8 (HeciBaseAddress + PCI_COMMAND_OFFSET);
      if ((Data8 & EFI_PCI_COMMAND_MEMORY_SPACE) == EFI_PCI_COMMAND_MEMORY_SPACE) {
        return *((UINTN *) (&Buffer[0]));
      }
    }
    PciSegmentOr8 (HeciBaseAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
    return *((UINTN *) (&Buffer[0]));
  }

  return 0;
}

/**
  Checks if ME is ready for communication over the HECI
  interface.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             ME is ready
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
IsMEReady (
  IN HECI_DEVICE                  HeciDev
  )
{
  HECI_ME_CONTROL_REGISTER  HeciRegMeCsrHa;
  UINTN                     HeciMBAR;

  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);

  ///
  /// Check for ME ready status
  ///
  HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
  if (HeciRegMeCsrHa.r.ME_RDY_HRA == 0) {
    return EFI_NOT_READY;
  } else {
    return EFI_SUCCESS;
  }
}

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
  )
{
  HECI_ME_CONTROL_REGISTER  HeciRegMeCsrHa;
  UINTN                     HeciMBAR;
  UINT32                    TimerStart;
  UINT32                    TimerEnd;
  volatile UINT32           *HpetTimer;

  ///
  ///  Wait for ME ready
  ///
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  while (1) {
    ///
    /// Read ME status and check for ME ready, but first check for reset
    ///
    HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);

    if (HeciRegMeCsrHa.r.ME_RST_HRA != 0) {
      ///
      /// ME requests HECI reset
      ///
      DEBUG ((DEBUG_INFO, "WaitForMEReady - Resetting Heci\n"));
      return ResetHeciInterface (HeciDev);
    }

    if (HeciRegMeCsrHa.r.ME_RDY_HRA != 0) {
      ///
      /// ME ready!!!
      ///
      return EFI_SUCCESS;
    }

    ///
    /// If 15 second timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR,
              "[HECI%d] ERROR: Timeout due to ME_RDY bit not set after 15 seconds waiting\n",
              HECI_NAME_MAP (HeciDev)));
      return EFI_TIMEOUT;
    }
    ///
    /// Wait before we check again
    ///
    MicroSecondDelay (HECI_WAIT_DELAY);
  }
}

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
  )
{
  HECI_FWS_REGISTER     Hfsts1;
  HECI_FW_STS4_REGISTER Hfsts4;

  if (MeBiosPath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Hfsts1.ul = PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (ME_SEGMENT, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, R_ME_HFS));
  Hfsts4.ul = PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (ME_SEGMENT, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, R_ME_HFS_4));

  DEBUG ((DEBUG_INFO, "HECI1 Hfsts1 = %X, Hfsts4 = %x \n", Hfsts1.ul, Hfsts4.ul));

  *MeBiosPath = MeNormalBiosPath;

  if (Hfsts1.r.ErrorCode == ME_ERROR_CODE_NO_ERROR) {
    if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_SPS) {
      *MeBiosPath = (Hfsts1.r.CurrentState > ME_STATE_RECOVERY) ? MeSpsOprBiosPath : MeSpsRcvBiosPath;
    } else if (Hfsts1.r.CurrentState == ME_STATE_NORMAL) {
      if (Hfsts1.r.FwUpdateInprogress == 0x01) {
        *MeBiosPath = (*MeBiosPath > MeFwUpdateBiosPath) ? *MeBiosPath : MeFwUpdateBiosPath;
      }
    }
  }
  if (Hfsts1.r.ErrorCode == ME_ERROR_CODE_IMAGE_FAILURE) {
    *MeBiosPath = (*MeBiosPath > MeErrorWithoutDidMsgBiosPath) ? *MeBiosPath : MeErrorWithoutDidMsgBiosPath;
  } else if (Hfsts1.r.ErrorCode != ME_ERROR_CODE_NO_ERROR) {
    *MeBiosPath = (*MeBiosPath > MeErrorBiosPath) ? *MeBiosPath : MeErrorBiosPath;
  }

  if (Hfsts1.r.FptBad == 0x01) {
    *MeBiosPath = (*MeBiosPath > MeErrorBiosPath) ? *MeBiosPath : MeErrorBiosPath;
  }

  if (Hfsts1.r.CurrentState == ME_STATE_RECOVERY) {
    *MeBiosPath = (*MeBiosPath > MeRecoveryBiosPath) ? *MeBiosPath : MeRecoveryBiosPath;
  }

  if (Hfsts4.r.FwInEnfFlow == 0x01) {
    *MeBiosPath = (*MeBiosPath > MeEnforcementWithoutDidMsgBiosPath) ? *MeBiosPath : MeEnforcementWithoutDidMsgBiosPath;
  }

  if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_DEBUG) {
    *MeBiosPath = (*MeBiosPath > MeDebugModeBiosPath) ? *MeBiosPath : MeDebugModeBiosPath;
  } else if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_SOFT_TEMP_DISABLE) {
    *MeBiosPath = (*MeBiosPath > MeSwTempDisableBiosPath) ? *MeBiosPath : MeSwTempDisableBiosPath;
  } else if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_JMPR) {
    *MeBiosPath = (*MeBiosPath > MeSecoverJmprBiosPath) ? *MeBiosPath : MeSecoverJmprBiosPath;
  } else if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_HECI_MSG) {
    *MeBiosPath = (*MeBiosPath > MeSecoverMeiMsgBiosPath) ? *MeBiosPath : MeSecoverMeiMsgBiosPath;
  }

  if (*MeBiosPath != MeNormalBiosPath && *MeBiosPath != MeSpsOprBiosPath) {
    DEBUG ((DEBUG_INFO, "CheckMeBootPath () - ME Bios Boot Path is 0x%x\n", *MeBiosPath));
  }
  return EFI_SUCCESS;
}

/**
  Check if this message is allowed with current ME Bios boot path.
  HECI1 must be enabled before invoke the function.
  FW error status may not be exposed at the beginning, so check the current Me Bios boot path
  always by every message service invoking to avoid unnecessary timeout and retrial.

  @param[in] HeciDev            The HECI device to be accessed.
  @param[in] HostAddr           The address of the host processor.
  @param[in] MeAddr             Address of the ME subsystem the message is being sent to.
  @param[in] MkhiHeader         Pointer to the message header.

  @retval TRUE                  This message is allowed
  @retval FALSE                 This message is blocked
**/
BOOLEAN
CheckMsgAllowance (
  IN HECI_DEVICE                HeciDev,
  IN UINT8                      HostAddr,
  IN UINT8                      MeAddr,
  IN MKHI_MESSAGE_HEADER        *MkhiHeader
  )
{
  ME_BIOS_BOOT_PATH             MeBiosPath;
  UINTN                         HeciMBAR;

  ///
  /// Block all messages if HECI1 is disabled already regardless the message is
  /// sent to which HECI device
  ///
  HeciMBAR = CheckAndFixHeciForAccess (HECI1_DEVICE);
  if (HeciMBAR == 0) {
    DEBUG((DEBUG_ERROR, "CheckMsgAllowance () fail, HeciMBAR is 0.\n"));
    return FALSE;
  }

  CheckMeBootPath (&MeBiosPath);

  if (HeciDev == HECI1_DEVICE) {
    ///
    /// End of Post needs to be send always if HECI1 remains enabled due to security reasons
    ///
    if ((MeBiosPath & ALL_MSG_ALLOWANCE) != 0 ||
        ((MkhiHeader->Fields.Command == GEN_END_OF_POST_CMD) && (MkhiHeader->Fields.GroupId == MKHI_GEN_GROUP_ID))) {
    } else if ((MeBiosPath >> 24) == SpsPath) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance () - SPS Path\n"));
    } else if ((MeBiosPath & IMR_AND_DID_MSG_ALLOWANCE) &&
               (MkhiHeader->Fields.GroupId == BUP_COMMON_GROUP_ID) &&
               ((MkhiHeader->Fields.Command == DRAM_INIT_DONE_CMD) || (MkhiHeader->Fields.Command == GET_IMR_SIZE_CMD))) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance () - IMR and DID commands are allowed with this Me Bios boot path.\n"));
    } else if ((MeBiosPath & HMRFPO_DISABLE_MSG_ALLOWANCE) &&
               (MkhiHeader->Fields.Command == HMRFPO_DISABLE_CMD) &&
               (MkhiHeader->Fields.GroupId == BUP_COMMON_GROUP_ID)) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance () - HMRFPO_DISABLE command is allowed with this Me Bios boot path.\n"));
    } else if ((MeBiosPath & SET_ME_ENABLE_MSG_ALLOWANCE) &&
               (MkhiHeader->Fields.Command == ME_ENABLE_CMD) &&
               (MkhiHeader->Fields.GroupId == BUP_COMMON_GROUP_ID)) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance () - Set Me Enabled command is allowed with this Me Bios boot path.\n"));
    } else if ((MeBiosPath & GLOBAL_RST_MSG_ALLOWANCE) &&
               (MkhiHeader->Fields.Command == CBM_RESET_CMD) &&
               (MkhiHeader->Fields.GroupId == MKHI_CBM_GROUP_ID)) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance () - Global Reset Request Message is allowed with this Me Bios boot path.\n"));
    } else if ((MeBiosPath & HMRFPO_GRP_MSG_ALLOWANCE) &&
               MkhiHeader->Fields.GroupId == MKHI_HMRFPO_GROUP_ID) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance () - HMRFPO messages are allowed with this ME Bios boot path\n"));
    } else if ((MeBiosPath & GEN_GRP_MSG_ALLOWANCE) &&
               MkhiHeader->Fields.GroupId == MKHI_GEN_GROUP_ID) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance () - GEN messages are allowed with this ME Bios boot path\n"));
    } else if ((MeBiosPath & MEI_BUS_MSG_ALLOWANCE) &&
               (HostAddr == BIOS_FIXED_HOST_ADDR) &&
               (MeAddr == HECI_HBM_MESSAGE_ADDR)) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance () - MEI messages are allowed with this ME Bios boot path\n"));
    } else if ((MeBiosPath & DYN_CLIENT_MSG_ALLOWANCE) &&
               (HostAddr == 0x01) && (MeAddr >= 0x20)) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance () - DYNCLIENT messages are allowed with this ME Bios boot path\n"));
    } else {
      DEBUG ((DEBUG_WARN, "CheckMsgAllowance () - This message is not supported on HECI1 with current boot path"));
      DEBUG ((
        DEBUG_WARN,
        ": %08x (GroudId:%x, CommandId:%x)\n",
        MkhiHeader->Data, MkhiHeader->Fields.GroupId, MkhiHeader->Fields.Command
        ));
      return FALSE;
    }
  }
  return TRUE;
}

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
  IN HECI_DEVICE                      HeciDev
  )
{
  UINT64                              HeciBaseAddress;
  HECI_HOST_CONTROL_REGISTER          HeciRegHCsr;
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  HECI_FWS_REGISTER                   MeFirmwareStatus;
  UINTN                               HeciMBAR;
  EFI_STATUS                          Status;
  UINT8                               Data8;
  DEBUG ((DEBUG_INFO, "HeciInitialize () - Start\n"));

  ///
  /// Check for the HECI PCI device availability
  /// Get MBAR
  ///
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMBAR == 0) {
    return EFI_DEVICE_ERROR;
  }

  HeciBaseAddress = PCI_SEGMENT_LIB_ADDRESS (ME_SEGMENT, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER + HeciDev, 0);

  HeciRegHCsrPtr  = (VOID *) (UINTN) (HeciMBAR + H_CSR);
  HeciRegHCsr.ul  = HeciRegHCsrPtr->ul;
  ///
  /// Read H_RDY bit to check if we're already initialized
  ///
  if (HeciRegHCsr.r.H_RDY == 1) {
    DEBUG ((DEBUG_INFO, "InitializeHeci () - H_RDY is already initialized\n"));
    return EFI_SUCCESS;
  }

  MeFirmwareStatus.ul = PciSegmentRead32 (HeciBaseAddress + R_ME_HFS);
  DEBUG ((DEBUG_INFO, "[HECI%d] HfSts1 = 0x%08X\n", HECI_NAME_MAP (HeciDev), MeFirmwareStatus.ul));
  if (MeFirmwareStatus.ul == 0 || MeFirmwareStatus.ul == 0xFFFFFFFF) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Check for ME FPT Bad
  ///
  if (MeFirmwareStatus.r.FptBad != 0) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Set HECI interrupt delivery mode.
  /// HECI-1 using legacy/MSI interrupt
  ///
  ///
  if ((IsPchLp () && (PchStepping () < PCH_D0)) || (IsPchH () && (PchStepping () < PCH_B0))) {
    Data8 = PciSegmentRead8 (HeciBaseAddress + R_ME_HIDM);
    if ((Data8 & (0x3)) != 0) {
      PciSegmentAnd8 (HeciBaseAddress + R_ME_HIDM, 0xFC);
    }
  } else {
    PciSegmentAnd8 (HeciBaseAddress + R_ME_HIDM, 0xFC);
  }

  ///
  /// Need to do following on ME init:
  ///
  ///  1) wait for ME_CSR_HA reg ME_RDY bit set
  ///
  Status = WaitForMEReady (HeciDev);
  ///
  ///  2) setup H_CSR reg as follows:
  ///     a) Make sure H_RST is clear
  ///     b) Set H_RDY
  ///     c) Set H_IG
  ///
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "InitializeHeci () -  Set H_RDY\n"));
    HeciRegHCsr.ul  = HeciRegHCsrPtr->ul;
    if (HeciRegHCsrPtr->r.H_RDY == 0) {
      HeciRegHCsr.r.H_RST = 0;
      HeciRegHCsr.r.H_RDY = 1;
      HeciRegHCsr.r.H_IG  = 1;
      HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;
    }
  }

  return Status;
}

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
  )
{
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  EFI_STATUS                      Status;
  UINTN                           HeciMBAR;

  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMBAR == 0) {
    return EFI_DEVICE_ERROR;
  }
  DEBUG ((DEBUG_INFO, "HeciReInitialize..... \n"));

  Status = EFI_SUCCESS;
  ///
  /// Need to do following on ME init:
  ///
  ///  1) wait for HOST_CSR_HA reg H_RDY bit set
  ///
  ///    if (WaitForHostReady() != EFI_SUCCESS) {
  ///
  if (MeResetWait (HeciDev, HECI_INIT_TIMEOUT) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }

  HeciRegHCsr.ul = MmioRead32 (HeciMBAR + H_CSR);
  if (HeciRegHCsr.r.H_RDY == 0) {
    Status = ResetHeciInterface (HeciDev);

  }

  return Status;
}

/**
  Function to pull one message packet off the HECI circular buffer up to its capacity.
  Corresponds to HECI HPS (part of) section 4.2.4

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[out] MessageHeader       Pointer to a buffer for the message header.
  @param[in] MessageData          Pointer to a buffer to receive the message in.
  @param[in, out] Length          On input is the size of the caller's buffer in bytes.
                                  On output is the size of the data copied to the buffer.
  @param[out] PacketSize          Size of the packet in bytes. This might be greater than buffer size.

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        The circular buffer is overflowed.
  @retval EFI_NO_RESPONSE         The circular buffer is empty
  @retval EFI_TIMEOUT             Failed to receive a full message
  @retval EFI_BUFFER_TOO_SMALL    Message packet is larger than caller's buffer
**/
EFI_STATUS
HeciPacketRead (
  IN      HECI_DEVICE             HeciDev,
  IN      UINT32                  Blocking,
  OUT     HECI_MESSAGE_HEADER     *MessageHeader,
  OUT     UINT32                  *MessageData,
  IN OUT  UINT32                  *BufferLength,
  OUT     UINT32                  *PacketSize
  )
{
  BOOLEAN                         GotMessage;
  EFI_STATUS                      Status;
//  UINT32                          TimerStart;
  //UINT32                          TimerEnd;
  //UINT32                          TimerStart1;
  //UINT32                          TimerEnd1;
  UINT32                          i;
  UINT32                          LengthInDwords;
  UINT32                          TempBuffer;
  UINT32                          ByteCount;
  HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  UINTN                           HeciMBAR;
  //volatile UINT32                 *HpetTimer;

  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);

  GotMessage  = FALSE;
  *PacketSize = 0;
  Status      = EFI_SUCCESS;
  ///
  /// clear Interrupt Status bit
  ///
  HeciRegHCsr.ul     = MmioRead32 (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_IS = 1;

  ///
  /// test for circular buffer overflow
  ///
  HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
  if (OverflowCB ( HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA, HeciRegMeCsrHa.r.ME_CBD_HRA) != EFI_SUCCESS) {
    ///
    /// if we get here, the circular buffer is overflowed
    ///
    *BufferLength = 0;
    return EFI_DEVICE_ERROR;
  }
  ///
  /// If NON_BLOCKING, exit if the circular buffer is empty
  ///
  HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
  if ((FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) == 0) && (Blocking == NON_BLOCKING)) {
    *BufferLength = 0;
    return EFI_NO_RESPONSE;
  }
  ///
  /// Start timeout counter
  ///
  //HpetTimer = StartTimer (&TimerStart, &TimerEnd, HECI_READ_TIMEOUT);

  ///
  /// loop until we get a message packet
  ///
  while (!GotMessage) {
    ///
    /// If timeout has expired, return fail as we have not yet received a full message.
    ///
    //if (MeHeciTimeoutsEnabled () && Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
    //  *BufferLength = 0;
    //  DEBUG ((DEBUG_ERROR, "HeciPacketRead: (1) EFI_TIMEOUT due to circular buffer not filled with full message after 5 seconds waiting\n"));
    //  return EFI_TIMEOUT;
    //}
    ///
    /// Read one message from HECI buffer and advance read pointer. Make sure
    /// that we do not pass the write pointer.
    ///
    HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
    if (FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) > 0) {
      ///
      /// Eat the HECI Message header
      ///
      MessageHeader->Data = MmioRead32 (HeciMBAR + ME_CB_RW);

      ///
      /// Compute required message length in DWORDS
      ///
      LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);

      ///
      /// Just return success if Length is 0
      ///
      if (MessageHeader->Fields.Length == 0) {
        ///
        /// Set Interrupt Generate bit and return
        ///
        MmioRead32 (HeciMBAR + H_CSR);
        HeciRegHCsr.r.H_IG = 1;
        MmioWrite32 (HeciMBAR + H_CSR, HeciRegHCsr.ul);
        *BufferLength = 0;
        goto GotPacket;
      }
      ///
      /// Make sure that the message does not overflow the circular buffer.
      ///
      HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
      if ((MessageHeader->Fields.Length + sizeof (HECI_MESSAGE_HEADER)) > (HeciRegMeCsrHa.r.ME_CBD_HRA * 4)) {
        *BufferLength = 0;
        return EFI_DEVICE_ERROR;
      }
      ///
      /// Start timeout counter for inner loop
      ///
      //HpetTimer = StartTimer (&TimerStart1, &TimerEnd1, HECI_READ_TIMEOUT);

      ///
      /// Wait here until entire message is present in circular buffer
      ///
      HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
      while (LengthInDwords > FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA)) {
        ///
        /// If timeout has expired, return fail as we have not yet received a full message
        ///
        //if (MeHeciTimeoutsEnabled () && Timeout (TimerStart1, TimerEnd1, HpetTimer) != EFI_SUCCESS) {
        //  *BufferLength = 0;
        //  DEBUG ((DEBUG_ERROR, "HeciPacketRead: (2) EFI_TIMEOUT due to circular buffer not filled with full message after 5 seconds waiting"));
        //  return EFI_TIMEOUT;
        //}
        ///
        /// Wait before we read the register again
        ///
        MicroSecondDelay (HECI_WAIT_DELAY);

        ///
        /// Read the register again
        ///
        HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
      }
      ///
      /// Update status to signal if buffer can hold the message
      ///
      if ((MessageHeader->Fields.Length) <= *BufferLength) {
        Status = EFI_SUCCESS;
      } else {
        Status = EFI_BUFFER_TOO_SMALL;
      }
      ///
      /// Copy as much bytes as there is space left in the message buffer.
      /// Excessive bytes will be dismissed.
      ///
      ByteCount = 0;
      for (i = 0; i < LengthInDwords; i++) {
        TempBuffer = MmioRead32 (HeciMBAR + ME_CB_RW);
        CopyMem (&MessageData[i], &TempBuffer, MIN(sizeof(TempBuffer),*BufferLength));
        ByteCount += MIN(sizeof(TempBuffer),*BufferLength);
        *BufferLength -= MIN(sizeof(TempBuffer),*BufferLength);
      }
      *BufferLength = ByteCount;
      *PacketSize = MessageHeader->Fields.Length;
      GotMessage  = TRUE;
  }
  ///
  /// Wait before we try to get a message again
  ///
  MicroSecondDelay (HECI_WAIT_DELAY);
  }
  ///
  /// Read ME_CSR_HA.  If the ME_RDY bit is 0, then an ME reset occurred during the
  /// transaction and the message should be discarded as bad data may have been retrieved
  /// from the host's circular buffer
  ///
  HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
  if (HeciRegMeCsrHa.r.ME_RDY_HRA == 0) {
    *BufferLength = 0;
    *PacketSize = 0;
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Set Interrupt Generate bit
  ///
  HeciRegHCsr.ul      = MmioRead32 (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_IG  = 1;
  MmioWrite32 (HeciMBAR + H_CSR, HeciRegHCsr.ul);

GotPacket:
  DEBUG ((DEBUG_INFO, "[HECI%d] Got msg: %08X\n", HECI_NAME_MAP (HeciDev), MessageHeader->Data));
  DEBUG_CODE (ShowBuffer ((UINT8 *) MessageData, *BufferLength););

  return Status;
}

/**
  Reads a message from the ME across HECI. This function can only be used after invoking HeciSend() first.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in, out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in, out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_NO_RESPONSE         Can't get data from ME FW.
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
  )
{
  UINT64                          HeciMBAR;
  HECI_MESSAGE_HEADER             PacketHeader;
  EFI_STATUS                      Status;
  UINT32                          TotalLength;
  UINT32                          PacketSize;
  UINT32                          PacketBuffer;
  //UINT32                          timer_start;
  //UINT32                          timer_end;
  //volatile UINT32                 *HpetTimer;

  PacketHeader.Data = 0;
  TotalLength       = 0;
  PacketBuffer      = *Length;
  Status            = EFI_SUCCESS;

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMBAR == 0) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Make sure we do not have a HECI reset
  ///
  if (CheckForHeciReset (HeciDev)) {
    ///
    /// if HECI reset than try to re-init HECI
    ///
    if (Blocking == BLOCKING) {
      Status = HeciInitialize (HeciDev);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
    } else {
      return EFI_DEVICE_ERROR;
    }
  }
  ///
  /// Make sure that HECI is ready for communication.
  ///
  if (Blocking == BLOCKING) {
    if (WaitForMEReady (HeciDev) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
  } else if (IsMEReady (HeciDev) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }

  ///
  /// Set up timer for BIOS timeout.
  ///
  //HpetTimer = StartTimer (&timer_start, &timer_end, HECI_READ_TIMEOUT);
  //
  // Read until MessageComplete bit is set
  //
  while (!PacketHeader.Fields.MessageComplete) {
    ///
    /// If timeout has expired, return fail as we have not yet received a full message
    ///
    //if (MeHeciTimeoutsEnabled () && Timeout (timer_start, timer_end, HpetTimer) != EFI_SUCCESS) {
    //  return EFI_TIMEOUT;
    //}
    Status = HeciPacketRead (
                  HeciDev,
                  Blocking,
                  &PacketHeader,
                  (UINT32 *) &MessageBody[TotalLength / 4],
                  &PacketBuffer,
                  &PacketSize
                  );
    ///
    /// Check for error condition on read
    ///
    if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
      *Length = TotalLength;
      return Status;
    }
    ///
    /// Check for zero length messages
    ///
    if (PacketBuffer == 0) {
      ///
      /// If we are not in the middle of a message, and we see Message Complete,
      /// this is a valid zero-length message.
      ///
      if ((TotalLength == 0) && (PacketHeader.Fields.MessageComplete)) {
        *Length   = 0;
        return EFI_SUCCESS;
      } else if (Status != EFI_BUFFER_TOO_SMALL) {
        ///
        /// We should not expect a zero-length message packet except as described above.
        ///
        *Length   = 0;
        return EFI_NO_RESPONSE;
      }
    }
    ///
    /// Track the length of what we have read so far
    ///
    TotalLength += PacketSize;
    ///
    /// To do message size calculations we are using 3 different variables:
    /// PacketBuffer - on input -> space left in the buffer, on output -> number of bytes in packet
    /// Length       - max buffer size
    /// TotalLength  - Total message size -> sum of all bytes in multiple packets
    ///
    if (TotalLength < *Length) {
      PacketBuffer = *Length - TotalLength;
    } else {
      PacketBuffer = 0;
    }
    ///
    /// If this was a NON-BLOCKING message and it is a multipacket message, we need to change the
    /// parameter to BLOCKING because it will take a non-zero value of time until a new packet appears
    ///
    Blocking = BLOCKING;
  }

  *Length = TotalLength;

  return Status;
}

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
  )
{
  EFI_STATUS                      Status;
  UINT32                          RetryCount;

  RetryCount = HECI_MAX_RETRY;
  //Folloiwng if condition will never ne true since MeHeciRetryEnabled return TRUE,
  //if (!MeHeciRetryEnabled ()) {
 //   RetryCount = 1;
 // }

  do {
    Status = HeciSend (HeciDev, Message, Length, HostAddress, MeAddress);
    if ((Status == EFI_SUCCESS) || (Status == EFI_DEVICE_ERROR) || (Status == EFI_UNSUPPORTED)) {
      if (Status != EFI_SUCCESS) {
        DEBUG ((DEBUG_ERROR, "[HECI%d] HeciSendWithRetry fail after HeciSend: %r, abort!\n", HECI_NAME_MAP (HeciDev), Status));
      }
      break;
    }
    RetryCount--;

    if (RetryCount != 0) {
      DEBUG ((DEBUG_ERROR, "[HECI%d] HeciSendWithRetry fail, Status: %r, RetryCount: %x\n", HECI_NAME_MAP (HeciDev), Status, RetryCount));
      Status = ResetHeciInterface (HeciDev);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "[HECI%d] HeciSendWithRetry retry fail after ResetHeciInterface: %r, abort!\n", HECI_NAME_MAP (HeciDev), Status));
        break;
      }
    }
  } while (RetryCount != 0);

  return Status;
}

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
  )
{
  UINT32                          CBLength;
  UINT32                          SendLength;
  UINT32                          CurrentLength;
  HECI_MESSAGE_HEADER             MessageHeader;
  EFI_STATUS                      Status;
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  UINT32                          MeMode;
  UINTN                           HeciMBAR;

  CurrentLength = 0;
  Status        = EFI_SUCCESS;

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMBAR == 0) {
    return EFI_DEVICE_ERROR;
  }

  if (MeHeciMessageCheckEnabled () == TRUE) {
    if (CheckMsgAllowance (HeciDev, HostAddress, MeAddress, (MKHI_MESSAGE_HEADER *) Message) == FALSE) {
      DEBUG ((DEBUG_INFO, "CheckMsgAllowance fail: [HECI%d] HostAddress: %x, MeAddress: %x, Length: %x\n",
        HECI_NAME_MAP(HeciDev), HostAddress, MeAddress, Length));
      DEBUG_CODE (
        ShowBuffer ((UINT8 *) Message, Length);
      );
      return EFI_UNSUPPORTED;
    }
  }

  HeciGetMeMode (&MeMode);
  if (MeMode == ME_MODE_SECOVER) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Make sure we do not have a HECI reset
  ///
  if (CheckForHeciReset (HeciDev)) {
    ///
    /// if HECI reset than try to re-init HECI
    ///
    Status = HeciInitialize (HeciDev);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
  }
  ///
  /// Make sure that HECI is ready for communication.
  ///
  if (WaitForMEReady (HeciDev) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  ///
  /// Set up memory mapped registers
  ///
  HeciRegHCsr.ul = MmioRead32 (HeciMBAR + H_CSR);

  ///
  /// Grab Circular Buffer length
  ///
  CBLength = HeciRegHCsr.r.H_CBD;

  ///
  /// Prepare message header
  ///
  MessageHeader.Data                = 0;
  MessageHeader.Fields.MeAddress    = MeAddress;
  MessageHeader.Fields.HostAddress  = HostAddress;

  ///
  /// Break message up into CB-sized packets and loop until completely sent
  ///
  while (Length > CurrentLength) {
    ///
    /// Set the Message Complete bit if this is our last packet in the message.
    /// Needs to be 'less than' to account for the header OR Needs to be exactly equal to CB depth.
    ///
    if (((((Length - CurrentLength) + 3) / 4) < CBLength) ||
        ((((((Length - CurrentLength) + 3) / 4) == CBLength) && ((((Length - CurrentLength) + 3) % 4) == 0)))
        ) {
      MessageHeader.Fields.MessageComplete = 1;
    }
    ///
    /// Calculate length for Message Header
    ///    header length == smaller of circular buffer or remaining message (both account for the size of the header)
    ///
    SendLength = ((CBLength <= (((Length - CurrentLength) + 3) / 4)) ? ((CBLength - 1) * 4) : (Length - CurrentLength));
    MessageHeader.Fields.Length = SendLength;

    ///
    /// send the current packet (CurrentLength can be treated as the index into the message buffer)
    ///
    Status = HeciPacketWrite (HeciDev, &MessageHeader, (UINT32 *) ((UINTN) Message + CurrentLength));
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ///
    /// Update the length information
    ///
    CurrentLength += SendLength;
  }
  return Status;
}

/**
  Function sends one message packet through the HECI circular buffer
  Corresponds to HECI HPS (part of) section 4.2.3

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] MessageHeader        Pointer to the message header.
  @param[in] MessageData          Pointer to the actual message data.

  @retval EFI_SUCCESS             One message packet sent
  @retval EFI_DEVICE_ERROR        ME is not ready
  @retval EFI_TIMEOUT             HECI is not ready for communication
**/
EFI_STATUS
HeciPacketWrite (
  IN  HECI_DEVICE                 HeciDev,
  IN  HECI_MESSAGE_HEADER         *MessageHeader,
  IN  UINT32                      *MessageData
  )
{
  //UINT32                          timer_start;
  //UINT32                          timer_end;
  UINT32                          i;
  UINT32                          LengthInDwords;
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;
  UINTN                           HeciMBAR;
  //volatile UINT32                 *HpetTimer;

  DEBUG ((DEBUG_INFO, "[HECI%d] Send msg: %08X\n", HECI_NAME_MAP (HeciDev), MessageHeader->Data));

  DEBUG_CODE (
    ShowBuffer ((UINT8 *) MessageData, MessageHeader->Fields.Length);
    );

  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);

  ///
  /// Make sure that HECI is ready for communication.
  ///
  if (WaitForMEReady (HeciDev) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  ///
  /// Start timeout counter
  ///
  //HpetTimer = StartTimer (&timer_start, &timer_end, HECI_SEND_TIMEOUT);

  ///
  /// Compute message length in DWORDS
  ///
  LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);

  ///
  /// Wait until there is sufficient room in the circular buffer
  /// Must have room for message and message header
  ///
  HeciRegHCsr.ul = MmioRead32 (HeciMBAR + H_CSR);
  while ((LengthInDwords + 1) > (HeciRegHCsr.r.H_CBD - FilledSlots (HeciRegHCsr.r.H_CBRP, HeciRegHCsr.r.H_CBWP))) {
    ///
    /// If timeout has expired, return fail as the circular buffer never emptied
    ///
    //if (MeHeciTimeoutsEnabled () && Timeout (timer_start, timer_end, HpetTimer) != EFI_SUCCESS) {
    //  DEBUG ((DEBUG_ERROR, "HeciPacketWrite: EFI_TIMEOUT due to circular buffer never emptied after 5 seconds waiting"));
   //   return EFI_TIMEOUT;
   // }
    ///
    /// Wait before we read the register again
    ///
    MicroSecondDelay (HECI_WAIT_DELAY);

    ///
    /// Read Host CSR for next iteration
    ///
    HeciRegHCsr.ul = MmioRead32 (HeciMBAR + H_CSR);
  }
  ///
  /// Write Message Header
  ///
  MmioWrite32 (HeciMBAR + H_CB_WW, MessageHeader->Data);

  ///
  /// Write Message Body
  ///
  for (i = 0; i < LengthInDwords; i++) {
    MmioWrite32 (HeciMBAR + H_CB_WW, MessageData[i]);
  }
  ///
  /// Set Interrupt Generate bit
  ///
  HeciRegHCsr.ul      = MmioRead32 (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_IG  = 1;
  MmioWrite32 (HeciMBAR + H_CSR, HeciRegHCsr.ul);

  ///
  /// Test if ME Ready bit is set to 1, if set to 0 a fatal error occured during
  /// the transmission of this message.
  ///
  HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
  if (HeciRegMeCsrHa.r.ME_RDY_HRA == 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                      Status;
  UINT32                          RetryCount;
  UINT32                          TempRecLength;

  //Folloiwng if condition will never ne true since MeHeciRetryEnabled return TRUE,
  RetryCount = HECI_MAX_RETRY;
 // if (!MeHeciRetryEnabled ()) {
 //   RetryCount = 1;
 // }

  TempRecLength = 0;

  do {
    ///
    /// Send the message
    ///
    Status = HeciSend (HeciDev, Message, Length, HostAddress, MeAddress);
    if (!EFI_ERROR (Status)) {
      ///
      /// Reload receive length as it has been modified by the read function
      ///
      TempRecLength = *RecLength;

      ///
      /// Read Message
      ///
      Status = HeciReceive (HeciDev, BLOCKING, Message, &TempRecLength);
      if (!EFI_ERROR (Status) || Status == EFI_BUFFER_TOO_SMALL) {
        break;
      }
    }

    if ((Status == EFI_DEVICE_ERROR) || (Status == EFI_UNSUPPORTED)) {
      ///
      /// Critical error occur, no retry any more.
      ///
      DEBUG ((DEBUG_ERROR, "[HECI%d] HeciSendwAckWithRetry fail, Status: %r, abort!\n", HECI_NAME_MAP (HeciDev), Status));
      break;
    }

    RetryCount--;
    if (RetryCount != 0) {
      DEBUG ((DEBUG_ERROR, "[HECI%d] HeciSendwAckWithRetry fail, Status: %r, RetryCount: %x\n", HECI_NAME_MAP (HeciDev), Status, RetryCount));
      Status = ResetHeciInterface (HeciDev);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "[HECI%d] HeciSendwAckWithRetry retry fail after ResetHeciInterface: %r, abort!\n", HECI_NAME_MAP (HeciDev), Status));
        break;
      }
    }
  } while (RetryCount != 0);
  ///
  /// Return read length and status
  ///
  *RecLength = TempRecLength;
  return Status;
}

/**
  Function sends one message through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in, out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in, out] RecLength       Length of the message response in bytes.
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
  )
{
  EFI_STATUS                      Status;
  UINT16                          RetryCount;
  UINT32                          TempRecLength;

  Status        = EFI_SUCCESS;
  TempRecLength = 0;

  for (RetryCount = 0; RetryCount < HECI_MAX_RETRY; RetryCount++) {
    ///
    /// Send the message
    ///
    Status = HeciSend (HeciDev, Message, Length, HostAddress, MeAddress);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ///
    /// Reload receive length as it has been modified by the read function
    ///
    TempRecLength = *RecLength;
    ///
    /// Read Message
    ///
    Status = HeciReceive (HeciDev, BLOCKING, Message, &TempRecLength);
    if (!EFI_ERROR (Status)) {
      break;
    }

    DEBUG ((
      DEBUG_ERROR,
      "HECI%d SendwAck: Retrying after %x failed attempt - Status = %r\n",
      HECI_NAME_MAP (HeciDev),
      RetryCount + 1,
      Status
      ));
  }
  ///
  /// Return read length and status
  ///
  *RecLength = TempRecLength;
  return Status;
}

/**
  Me reset and waiting for ready

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Delay                The biggest waiting time

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Me is ready
**/
EFI_STATUS
EFIAPI
MeResetWait (
  IN  HECI_DEVICE                 HeciDev,
  IN  UINT32                      Delay
  )
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  UINT32                      TimerStart;
  UINT32                      TimerEnd;
  UINTN                       HeciMBAR;
  volatile UINT32             *HpetTimer;

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMBAR == 0) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Wait for the HOST Ready bit to be cleared to signal a reset
  ///
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, Delay);
  HeciRegHCsr.ul = MmioRead32 (HeciMBAR + H_CSR);
  while (HeciRegHCsr.r.H_RDY == 1) {
    ///
    /// If timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }

    HeciRegHCsr.ul = MmioRead32 (HeciMBAR + H_CSR);
  }

  return EFI_SUCCESS;
}

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
  )
{
  HECI_FWS_REGISTER               MeFirmwareStatus;
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;
  UINT32                          TimerStart;
  UINT32                          TimerEnd;
  UINTN                           HeciMBAR;
  volatile UINT32                 *HpetTimer;

  DEBUG ((DEBUG_INFO, "[HECI%d] Resetting interface\n", HECI_NAME_MAP (HeciDev)));

  MeFirmwareStatus.ul = PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (ME_SEGMENT, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER + HeciDev, R_ME_HFS));
  if (MeFirmwareStatus.ul == 0 || MeFirmwareStatus.ul == 0xFFFFFFFF) {
    DEBUG ((DEBUG_ERROR, "[HECI%d] ERROR: HfSts1 = 0x%08X\n", HECI_NAME_MAP (HeciDev), MeFirmwareStatus.ul));
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMBAR == 0) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Enable Reset
  ///
  HeciRegHCsr.ul      = MmioRead32 (HeciMBAR + H_CSR);
  DEBUG ((DEBUG_INFO, "[HECI%d] - Step1 Enable Host Reset : H_CSR = 0x%08X\n", HECI_NAME_MAP (HeciDev), HeciRegHCsr.ul));

  ///
  /// ME reset may be asserted, mask H_IS bit as it is write 1 clear bit.
  ///
  HeciRegHCsr.ul &= (~ BIT1);

  if (!HeciRegHCsr.r.H_RST) {
    HeciRegHCsr.r.H_RST = 1;
    HeciRegHCsr.r.H_IG  = 1;
    MmioWrite32 (HeciMBAR + H_CSR, HeciRegHCsr.ul);
  }

  ///
  /// Make sure that the reset started
  ///
  /// HeciRegHCsr.ul = MMIOREADDWORD(HeciMBAR + H_CSR);
  ///
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);

  do {
    ///
    /// If timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Read the ME CSR
    ///
    HeciRegHCsr.ul = MmioRead32 (HeciMBAR + H_CSR);
    DEBUG ((DEBUG_INFO, "[HECI%d] - Step2 Wait for reset started: H_CSR = 0x%08X\n", HECI_NAME_MAP (HeciDev), HeciRegHCsr.ul));
  } while (HeciRegHCsr.r.H_RDY == 1);

  ///
  /// Wait for ME to perform reset
  ///
  /// HeciRegMeCsrHa.ul = MMIOREADDWORD(HeciMBAR + ME_CSR_HA);
  ///
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  do {
    ///
    /// If 5 second timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Read the ME CSR
    ///
    HeciRegMeCsrHa.ul = MmioRead32 (HeciMBAR + ME_CSR_HA);
    DEBUG ((DEBUG_INFO, "[HECI%d] - Step3  Wait for ME reset: ME_CSR_HA = 0x%08X\n", HECI_NAME_MAP (HeciDev), HeciRegMeCsrHa.ul));
  } while (HeciRegMeCsrHa.r.ME_RDY_HRA == 0);

  ///
  /// Make sure IS has been signalled on the HOST side
  ///
  /// HeciRegHCsr.ul = MMIOREADDWORD(HeciMBAR + H_CSR);
  ///
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  do {
    ///
    /// If 5 second timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Read the ME CSR
    ///
    HeciRegHCsr.ul = MmioRead32 (HeciMBAR + H_CSR);
    DEBUG ((DEBUG_INFO, "[HECI%d] - Step4 Wait for IS signalled : H_CSR = 0x%08X\n", HECI_NAME_MAP (HeciDev), HeciRegHCsr.ul));
  } while (HeciRegHCsr.r.H_IS == 0);

  ///
  /// Enable host side interface. Host SW writes the value read back to the H_CSR register.
  /// This clears the H_IS and bit in H_CSR to 0
  ///
  HeciRegHCsr.ul      = MmioRead32 (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_RST = 0;
  HeciRegHCsr.r.H_IG  = 1;
  HeciRegHCsr.r.H_RDY = 1;
  MmioWrite32 (HeciMBAR + H_CSR, HeciRegHCsr.ul);

  return EFI_SUCCESS;
}

/**
  Get an abstract Intel HECI1 State from Firmware Status Register.
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
  )
{
  HECI_FWS_REGISTER               MeFirmwareStatus;

  if (MeStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (CheckAndFixHeciForAccess (HECI1_DEVICE) == 0) {
    return EFI_DEVICE_ERROR;
  }

  MeFirmwareStatus.ul = PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (ME_SEGMENT, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, R_ME_HFS));

  if (MeFirmwareStatus.r.CurrentState == ME_STATE_NORMAL && MeFirmwareStatus.r.ErrorCode == ME_ERROR_CODE_NO_ERROR) {
    *MeStatus = ME_READY;
  } else if (MeFirmwareStatus.r.CurrentState == ME_STATE_RECOVERY) {
    *MeStatus = ME_IN_RECOVERY_MODE;
  } else if (MeFirmwareStatus.r.CurrentState == ME_STATE_INIT) {
    *MeStatus = ME_INITIALIZING;
  } else if (MeFirmwareStatus.r.CurrentState == ME_STATE_DISABLE_WAIT) {
    *MeStatus = ME_DISABLE_WAIT;
  } else if (MeFirmwareStatus.r.CurrentState == ME_STATE_TRANSITION) {
    *MeStatus = ME_TRANSITION;
  } else {
    *MeStatus = ME_NOT_READY;
  }

  if (MeFirmwareStatus.r.FwUpdateInprogress) {
    *MeStatus |= ME_FW_UPDATES_IN_PROGRESS;
  }

  if (MeFirmwareStatus.r.FwInitComplete == ME_FIRMWARE_COMPLETED) {
    *MeStatus |= ME_FW_INIT_COMPLETE;
  }

  if (MeFirmwareStatus.r.MeBootOptionsPresent == ME_BOOT_OPTIONS_PRESENT) {
    *MeStatus |= ME_FW_BOOT_OPTIONS_PRESENT;
  }

  DEBUG ((DEBUG_INFO, "[HECI1] MeStatus 0x%X (Hfsts1: %08X)\n", *MeStatus, MeFirmwareStatus.ul));

  return EFI_SUCCESS;
}

/**
  Return HECI1 Mode

  @param[out] MeMode              Pointer for HECI1 Mode report

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  OUT UINT32                       *MeMode
  )
{
  HECI_FWS_REGISTER MeFirmwareStatus;

  if (MeMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (CheckAndFixHeciForAccess (HECI1_DEVICE) == 0) {
    return EFI_DEVICE_ERROR;
  }

  MeFirmwareStatus.ul = PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (ME_SEGMENT, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, R_ME_HFS));

  switch (MeFirmwareStatus.r.MeOperationMode) {
    case ME_OPERATION_MODE_NORMAL:
      *MeMode = ME_MODE_NORMAL;
      break;

    case ME_OPERATION_MODE_DEBUG:
      *MeMode = ME_MODE_DEBUG;
      break;

    case ME_OPERATION_MODE_SOFT_TEMP_DISABLE:
      *MeMode = ME_MODE_TEMP_DISABLED;
      break;

    case ME_OPERATION_MODE_SECOVR_JMPR:
    case ME_OPERATION_MODE_SECOVR_HECI_MSG:
      *MeMode = ME_MODE_SECOVER;
      break;

    case ME_OPERATION_MODE_SPS:
      *MeMode = ME_MODE_SPS;
      break;

    default:
      *MeMode = ME_MODE_FAILED;
  }

  DEBUG ((DEBUG_INFO, "[HECI1] MeMode %d (Hfsts1: %08X)\n", *MeMode, MeFirmwareStatus.ul));

  return EFI_SUCCESS;
}

/**
  Register HECI service

  @retval EFI_SUCCESS             HECI service is registered successfully.
  @retval Others                  There is failure when register HECI service
**/
EFI_STATUS
EFIAPI
RegisterHeciService (
  VOID
  )
{
  EFI_STATUS      Status;

  Status = RegisterService ((VOID *)&mHeciService);

  return Status;
}

EFI_STATUS
EFIAPI
ReadHeciFwStatus (
  UINT32 *FwSts
  )
{

  if(FwSts != NULL) {
    if (CheckAndFixHeciForAccess (HECI1_DEVICE) == 0) {
      return EFI_DEVICE_ERROR;
    }

    *FwSts = PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (ME_SEGMENT, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, R_SEC_FW_STS0));
    return EFI_SUCCESS;

  }
  else {
    return RETURN_INVALID_PARAMETER;
  }
}

/**
  Get MBP data pointer

  @return MBP_CMD_RESP_DATA     pointer to MBP data header.
  @return NULL                  NULL in case MBP not found.

 **/
STATIC
MBP_CMD_RESP_DATA *
HeciMBP (
    VOID
  )
{
  MBP_CMD_RESP_DATA        *MBPHeader;
  VOID                     *FspHobList;

  MBPHeader = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    MBPHeader = GetGuidHobData (FspHobList, NULL, &gEfiHeciMbpDataHobGuid);
  }
  if (MBPHeader == NULL) {
    DEBUG ((DEBUG_ERROR, "HeciMBP MBPHeader not Found \n"));
  }

  return MBPHeader;
}


/**
  Send Get Firmware Version Request to Sec

  @param[out] MsgGenGetFwVersionAck   Return FW version from CSE.

  @exception EFI_UNSUPPORTED      Current Sec mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
  EFIAPI
HeciGetFwVersionMsg (
  OUT UINT8     *MsgAck
  )
{
  EFI_STATUS                Status;
  UINT32                    Length;
  GEN_GET_FW_VER            MsgGenGetFwCapsSku;
  GEN_GET_FW_VER_ACK        *MsgGenGetFwVersionAckData;

  MBP_CMD_RESP_DATA         *MBPHeader;
  MBP_ITEM_HEADER           *MBPItem;
  MBP_VERSION               *MBPVersion;

  DEBUG ((DEBUG_ERROR, "HeciGetFwVersionMsg Called \n"));
  if ( MsgAck == NULL ) {
    return EFI_INVALID_PARAMETER;
  }

  MsgGenGetFwVersionAckData = (GEN_GET_FW_VER_ACK *)MsgAck;

  MBPHeader = HeciMBP ();
  if ( MBPHeader != NULL) {
    MBPItem = (MBP_ITEM_HEADER *)(MBPHeader + 1);
    while ( (UINT32 *)MBPItem < (UINT32 *)MBPHeader + MBPHeader->Length) {
      if ( (MBPItem->Fields.AppId == MBP_APP_ID_KERNEL) && (MBPItem->Fields.ItemId == MBP_ITEM_ID_FW_VER_NAME)) {
        MBPVersion = (MBP_VERSION *)(MBPItem + 1);
        MsgGenGetFwVersionAckData->Data.CodeMinor   = MBPVersion->MinorVersion;
        MsgGenGetFwVersionAckData->Data.CodeMajor   = MBPVersion->MajorVersion;
        MsgGenGetFwVersionAckData->Data.CodeBuildNo = MBPVersion->BuildVersion;
        MsgGenGetFwVersionAckData->Data.CodeHotFix  = MBPVersion->HotfixVersion;
        return EFI_SUCCESS;
      }
      MBPItem = (MBP_ITEM_HEADER *)((UINT32 *)MBPItem + MBPItem->Fields.Length);

      //avoid infinite loop in case of corrupt data.
      if (MBPItem->Fields.Length == 0) {
        break;
      }
    }
  }
  DEBUG ((DEBUG_ERROR, "HeciGetFwVersionMsg by HECI Interfaces \n"));
  //
  // Allocate MsgGenGetFwVersion data structure
  //
  MsgGenGetFwCapsSku.MKHIHeader.Data               = 0;
  MsgGenGetFwCapsSku.MKHIHeader.Fields.GroupId     = 0xFF;
  MsgGenGetFwCapsSku.MKHIHeader.Fields.Command     = 0x02;
  MsgGenGetFwCapsSku.MKHIHeader.Fields.IsResponse  = 0;

  Length = sizeof (GEN_GET_FW_VER);

  DEBUG ((DEBUG_ERROR, "before CheckAndFixHeciForAccess Called \n"));
  if (CheckAndFixHeciForAccess (HECI1_DEVICE) == 0) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_ERROR, "after CheckAndFixHeciForAccess Called \n"));
  //
  // Send Get FW SKU Request to SEC
  //
  Length = sizeof (GEN_GET_FW_VER);
  Status = HeciSend (
      HECI1_DEVICE,
      (UINT32 *)&MsgGenGetFwCapsSku,
      Length,
      BIOS_FIXED_HOST_ADDR,
      0x7
      );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  Length = sizeof (GEN_GET_FW_VER_ACK);
  Status = HeciReceive (
      HECI1_DEVICE,
      BLOCKING,
      (UINT32 *)MsgGenGetFwVersionAckData,
      &Length
      );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Send Get Firmware SKU Request to Sec

  @param[in] MsgGenGetFwCapsSku      Send  message for Get Firmware Capability SKU
  @param[out] MsgGenGetFwCapsSkuAck  Return message for Get Firmware Capability SKU ACK

  @exception EFI_UNSUPPORTED        Current Sec mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwCapsSkuMsg (
  IN UINT8  *MsgGetFwCaps,
  OUT UINT8  *MsgGetFwCapsAck
  )
{
  EFI_STATUS                Status;
  UINT32                    Length;
  GEN_GET_FW_CAPSKU         *MsgGenGetFwCapsSku;
  GEN_GET_FW_CAPS_SKU_ACK   *MsgGenGetFwCapsSkuAck;

  MBP_CMD_RESP_DATA         *MBPHeader;
  MBP_ITEM_HEADER           *MBPItem;

  if ( (MsgGetFwCaps == NULL) || (MsgGetFwCapsAck == NULL) ) {
    return EFI_INVALID_PARAMETER;
  }
  MsgGenGetFwCapsSku = (GEN_GET_FW_CAPSKU *)MsgGetFwCaps;
  MsgGenGetFwCapsSkuAck = (GEN_GET_FW_CAPS_SKU_ACK *)MsgGetFwCapsAck;

  MBPHeader = HeciMBP ();
  if (MBPHeader != NULL) {
    MBPItem = (MBP_ITEM_HEADER *)(MBPHeader + 1);
    while ((UINT32 *)MBPItem < (UINT32 *)MBPHeader + MBPHeader->Length) {
      if ((MBPItem->Fields.AppId == MBP_APP_ID_KERNEL) && (MBPItem->Fields.ItemId == 2)) {
        MsgGenGetFwCapsSkuAck->Data.FWCap.Data = *(UINT32 *)(MBPItem + 1);
        DEBUG ((DEBUG_INFO, "MBP FWCAPS: 0x%08x\n\n", MsgGenGetFwCapsSkuAck->Data.FWCap.Data));
        return EFI_SUCCESS;
      }
      MBPItem = (MBP_ITEM_HEADER *)((UINT32 *)MBPItem + MBPItem->Fields.Length);

      //avoid infinite loop in case of corrupt data.
      if (MBPItem->Fields.Length == 0) {
        break;
      }
    }
  }

  DEBUG ((DEBUG_INFO, "FWCAPS: Though HECI\n"));
  //
  // Allocate MsgGenGetFwVersion data structure
  //
  MsgGenGetFwCapsSku->MKHIHeader.Data               = 0;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.GroupId     = 0x03;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.Command     = 0x02;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.IsResponse  = 0x0;
  MsgGenGetFwCapsSku->Data.RuleId                   = 0x0;
  Length = sizeof (GEN_GET_FW_CAPSKU);

  if (CheckAndFixHeciForAccess (HECI1_DEVICE) == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Send Get FW SKU Request to SEC
  //
  Status = HeciSend (
      HECI1_DEVICE,
      (UINT32 *)MsgGenGetFwCapsSku,
      Length,
      BIOS_FIXED_HOST_ADDR,
      0x7
      );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_FW_CAPS_SKU_ACK);
  Status = HeciReceive (
      HECI1_DEVICE,
      BLOCKING,
      (UINT32 *)MsgGenGetFwCapsSkuAck,
      &Length
      );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  return EFI_SUCCESS;
}
