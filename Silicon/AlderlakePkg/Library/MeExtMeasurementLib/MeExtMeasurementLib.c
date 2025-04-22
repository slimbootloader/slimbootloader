/** @file
  CSME Extended Measurement driver.

  Copyright (c) 2019 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Register/HeciRegs.h>
#include <Register/MeRegs.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HeciMeExtLib.h>
#include <Library/TpmLib.h>
#include <Library/IoLib.h>
#include <Library/MeExtMeasurementLib.h>
#include <MeChipset.h>
#include <PcieRegs.h>
#include <MeBiosPayloadData.h>

/**
  Perform measurement for HER register.

  @param[in] HerValue             The value of HECI Extend Register.
  @param[in] Index                HerValue Size.
  @param[in] HashAlgorithm        Hash Algorithm.


  @retval EFI_SUCCESS             Measurement performed
**/
EFI_STATUS
MeasureHer (
  IN  UINT32                      *HerValue,
  IN  UINT8                       Index,
  IN  UINT8                       HashAlgorithm
  )
{
  EFI_STATUS              Status;
  UINT32                  HashDataLen;
  UINT8                   *TempEvent;
  UINT32                  EventLogSize;
  UINT8                   LogEntryIndex;
  UINT8                   RemainingEntries;
  UINT32                  RemainingBufSize;
  UINT32                  ReceivedLogSize;
  CSME_MEASUREMENT_EVENT  CsmeEvent;
  static CONST CHAR8      Signature[] = "IntelCSxEEvent01";

  ///
  /// Get ER Log using BUP_COMMON_GET_ER_LOG and pass that log to Tpm Measurement
  ///
  ZeroMem (&CsmeEvent, sizeof (CsmeEvent));
  CopyMem (&(CsmeEvent.Signature), Signature, AsciiStrLen (Signature));

  CsmeEvent.ERHashAlgorithm = HashAlgorithm;
  EventLogSize = 0;
  RemainingBufSize = MAX_EVENTLOG_SIZE;
  ReceivedLogSize = 0;
  LogEntryIndex = 0;
  TempEvent = CsmeEvent.Events;

  do {
    RemainingEntries = 0;
    Status = HeciGetErLog (
               TempEvent,
               RemainingBufSize,
               &ReceivedLogSize,
               &LogEntryIndex,
               &RemainingEntries);
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "Failed to get ER Log data\n"));
        return Status;
    }

#if 0
    DEBUG ((DEBUG_INFO, "CSME Extended Measured Boot: Event log from ER registers\n"));
    DumpHex (2, 0, ReceivedLogSize, TempEvent);
#endif

    EventLogSize += ReceivedLogSize;
    if (RemainingEntries) {
      if (EventLogSize >= MAX_EVENTLOG_SIZE) {
        DEBUG((DEBUG_ERROR, "Event Log is exceeding the Maximum allocated buffer\n"));
        return EFI_BUFFER_TOO_SMALL;
      }

      TempEvent += (ReceivedLogSize + 1);
      LogEntryIndex++;
      RemainingBufSize = MAX_EVENTLOG_SIZE - EventLogSize;
    }
  } while (RemainingEntries != 0);

  EventLogSize += sizeof (CsmeEvent.Signature) + sizeof (CsmeEvent.ERHashAlgorithm);
  HashDataLen = (Index * sizeof (UINT32));

  //
  // Measure to PCR[0] with event EV_NONHOST_INFO
  //
  Status = TpmHashAndExtendPcrEventLog (0, (UINT8 *)HerValue, HashDataLen, EV_NONHOST_INFO, EventLogSize, (UINT8*) &CsmeEvent);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Get Extended Measurement data from CSME Extended registers.

  @param[in] Her               Pointer to receive HER Data buffer
  @param[in] HashAlgorithm     Hash Algorithm
  @param[in] HerMaxRegCount    DWord registers count which has Hash data

  @retval EFI_SUCCESS             Successfully read Measurement data
  @retval EFI_INVALID_PARAMETER   Input parameters are not valid
  @retval EFI_UNSUPPORTED         Unsupported Hash Algorithm
**/
EFI_STATUS
HeciGetHerData (
  IN UINT32    *Her,
  IN UINT8     HashAlgorithm,
  IN UINT8     HerMaxRegCount
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINT32        HerOffset;

  if ((Her == NULL) || (HashAlgorithm == 0) || (HerMaxRegCount == 0)) {
    DEBUG ((DEBUG_ERROR, "HeciGetHerData: Invalid parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  HerOffset = R_HER_BASE + R_ME_HER1;

  if ((HashAlgorithm == V_ME_SHA_256) || (HashAlgorithm == V_ME_SHA_384)){
    ///
    /// Read HER data from MMIO space 0xCC0 - 0xCDF (HECI1_HER1 - HECI1_HER12)
    ///
    Status = HeciGetHerHashData (Her, HerOffset, HerMaxRegCount);
  } else {
    DEBUG ((DEBUG_ERROR, "HeciGetHerData: Failed to get CSME Measurement data\n"));
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  Extend CSME Measurment to TPM-PCR[0]

**/
EFI_STATUS
EFIAPI
ExtendCsmeMeasurementToTpmPcr (
  VOID
  )
{
  EFI_STATUS    Status;
  UINT32        Her [HER_REG_COUNT_SHA384];
  UINT32        Hers;
  UINT32        Enh_Hers;
  UINT8         HashAlgorithm;
  UINT8         HerMaxRegCount;
  UINT64        MemBar;

  MemBar =  HeciPciRead32 (HECI_FUNCTION_NUMBER, PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0;
  if ((HeciPciRead32 (HECI_FUNCTION_NUMBER, PCI_BASE_ADDRESSREG_OFFSET) & B_PCI_BAR_MEMORY_TYPE_MASK) == B_PCI_BAR_MEMORY_TYPE_64) {
    MemBar += (UINT64) HeciPciRead32 (HECI_FUNCTION_NUMBER, (PCI_BASE_ADDRESSREG_OFFSET + 4)) << 32;
  }

  if (MemBar == 0) {
    DEBUG ((DEBUG_WARN, "MMIO Bar for HECI device isn't programmed\n"));
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Enable HECI MSE
  ///
  HeciPciOr8 (HECI_FUNCTION_NUMBER, PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

    ///
  /// Read HER Status register value
  ///
  Hers = HeciPciRead32 (HECI_FUNCTION_NUMBER, R_ME_HERS);

  ///
  /// Read HER Status register value
  ///
  Enh_Hers = MmioRead32 ((UINTN) MemBar + R_HERS);
  if (Enh_Hers & B_ME_EXTEND_FEAT_PRESENT) {
    //
    // Check for ERV (PCI config) or ERC bits (MMIO)
    //
    if ((Hers & B_ME_EXTEND_REG_VALID) || (Enh_Hers & B_ME_EXTEND_COMPLETE)) {
      DEBUG ((DEBUG_INFO, "CSME Extend Measurement is valid\n"));
    } else {
      DEBUG ((DEBUG_INFO, "CSME Extend Measurement is not valid\n"));
      return EFI_NOT_FOUND;
    }
  } else {
    DEBUG ((DEBUG_INFO, "CSME Extend Measurement Capability is not supported.\n"));
    return EFI_NOT_FOUND;
  }

  ZeroMem (&Her, sizeof (Her));
  HashAlgorithm = 0;
  HerMaxRegCount = 0;

  if ((Enh_Hers & B_ME_EXTEND_REG_ALGORITHM) == V_ME_SHA_256) {
    HashAlgorithm = V_ME_SHA_256;
    HerMaxRegCount = (UINT8) HER_REG_COUNT_SHA256;
  } else if ((Enh_Hers & B_ME_EXTEND_REG_ALGORITHM) == V_ME_SHA_384) {
    HashAlgorithm = V_ME_SHA_384;
    HerMaxRegCount = (UINT8) HER_REG_COUNT_SHA384;
  }

  Status = HeciGetHerData (Her, HashAlgorithm, HerMaxRegCount);
  if (!EFI_ERROR (Status)) {
    Status = MeasureHer (Her, HerMaxRegCount, HashAlgorithm);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Extending ME Measurement to TPM-PCR0 failed, Status is %r \n", Status));
    }
  }

  return Status;
}

/**
  Me Measured Boot setup and init

  @retval     RETURN_SUCCESS       If getting the MBP data successfully
  @retval     EFI_NOT_FOUND        Failed to get MBP data
**/
EFI_STATUS
MeMeasuredBootInit (
  VOID
  )
{
  VOID                  *FspHobList;
  BOOLEAN               CsmeMbState;
  ME_BIOS_PAYLOAD       *MbpDataHob;
  UINT8                 *DataPtr;
  UINT32                MbpDataHobLen;
  EFI_STATUS            Status;

  Status = EFI_UNSUPPORTED;
  DataPtr = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    DataPtr = GetGuidHobData (FspHobList, &MbpDataHobLen, &gMeBiosPayloadHobGuid);
  }
  if ((DataPtr != NULL) && (MbpDataHobLen > 0)) {
    MbpDataHob = (ME_BIOS_PAYLOAD *) (DataPtr+4);
    //
    // Read MBP Table and check for CSME Measured Boot Support (MBP_ITEM_ID_MEASURED_BOOT  Bit 0)
    //
    CsmeMbState = (MbpDataHob->MeasuredBootSupport.MeasuredBootData.MeasuredBoot == 1) ? TRUE : FALSE;
    DEBUG((DEBUG_INFO, "CSME MeasuredBoot state: %x \n", CsmeMbState));

    //
    // 1. If Measured Boot is enabled on both CSME & Host side then extend CSME Measurement from ER Registers to TPM-PCR[0]
    // 2. Else if Host side Measured Boot is not in sync with CSME Side Measurement Boot, then Send MEI Command to CSME to enable/disable as per Host
    //    side Measured Boot State.
    // 3. If Measured Boot is disabled on both CSME & Host side then No action is required continue to Boot.
    //
    if (CsmeMbState) {
      if (MEASURED_BOOT_ENABLED()) {
        //
        // Extend CSME Measured data to TPM PCR[0]
        //
        Status = ExtendCsmeMeasurementToTpmPcr ();
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "ExtendCsmeMeasurementToTpmPcr status: %r\n", Status));
        }
      } else {
        //
        // Send Command to CSME to disable Extended Measured boot
        // Not required to issue Global reset as no Measured data to be updated
        //
        DEBUG ((DEBUG_INFO, "Send CSME command to Disable Measured Boot\n"));
        Status = HeciSetMeasuredBootState (0);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "CSME failed for HeciSetMeasuredBootState to disable MB\n"));
        }
      }
    }
  } else {
    DEBUG ((DEBUG_INFO, "Could not find Mbp hob.\n"));
    Status = EFI_NOT_FOUND;
  }

  return Status;
}
