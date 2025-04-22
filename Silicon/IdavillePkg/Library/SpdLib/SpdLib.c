/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <RegAccess.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/TimerLib.h>
#include <Library/SpdLib.h>


/**

  Finds the time delta between two Time Stamp Counter values in the given time units.

  @param[in] StartCount - First Time Stamp Counter value
  @param[in] EndCount   - Second Time Stamp Counter value
  @param[in] Unit       - For Time in Seconds (TDIFF_UNIT_S) = 1 \n
                          For Time in milliseconds (TDIFF_UNIT_MS) = 1000 \n
                          For Time in microseconds (TDIFF_UNIT_US) = 1000000 \n
                          For Time in nanoseconds (TDIFF_UNIT_NS) = 1000000000


  @retval Time delta in provided units

**/
UINT32
TimeDiff (
  IN UINT64    StartCount,
  IN UINT64    EndCount,
  IN UINT32    Unit
  )
{
  if (GetPerformanceCounterProperties (NULL, NULL) == 0) {
    return 0;
  }
  return (UINT32) DivU64x64Remainder (MultU64x32 (EndCount - StartCount, Unit), GetPerformanceCounterProperties (NULL, NULL), NULL);
}

/**

  Determines the delay since GetPerformanceCounter was called

  @param[in] StartTsc - 64-bit counter value from GetPerformanceCounter()

  @retval delay time in 1 us units

**/
UINT32
GetDelayTsc (
  IN UINT64  StartTsc
  )
{
  UINT64    EndTsc;

  EndTsc = GetPerformanceCounter ();
  return (TimeDiff (StartTsc, EndTsc, TDIFF_UNIT_US));
}

/**
  Read PCU SPD_CHASSIS_SMBUS device registers

  @param[in]  RegOffset  Register offset to read the SPD_CHASSIS_SMBUS device

  @retval     UINT32     Value at RegOffset
**/
UINT32
SpdRead (
  IN  UINT16    RegOffset
)
{
  UINTN         Address;

  Address = MM_PCI_ADDRESS (PCI_BUS_NUMBER_SPD, PCI_DEVICE_NUMBER_SPD, PCI_FUNC_NUMBER_SPD, 0);

  return MmioRead32 (Address + RegOffset);
}

/**
  Write to PCU SPD_CHASSIS_SMBUS device registers

  @param[in]  ByteOffset  Register offset to write to the SPD_CHASSIS_SMBUS device
  @param[in]  Value       Value to be written

  @retval UINT32          Value written
**/
UINT32
SpdWrite (
  IN  UINT16    RegOffset,
  IN  UINT32    Value
)
{
  UINTN         Address;

  Address = MM_PCI_ADDRESS (PCI_BUS_NUMBER_SPD, PCI_DEVICE_NUMBER_SPD, PCI_FUNC_NUMBER_SPD, 0);

  return MmioWrite32 (Address + RegOffset, Value);
}

/**
  Wait for smbus host not busy. The function waits based on the build
  target.

  @retval EFI_SUCCESS     Completed successfully.
  @retval !EFI_SUCCESS    Failure.

**/
EFI_STATUS
WaitForHostNotBusyTarget (
  VOID
  )
{
  UINT64            StartCount;
  SMB_STATUS_CFG    SmbStat;
  EFI_STATUS        Status = EFI_SUCCESS;

  //
  // Wait for Host not busy
  //
  StartCount = GetPerformanceCounter ();
  do {
    SmbStat.Data = SpdRead (SMB_STATUS_CFG_OFFSET);
    if (!SmbStat.Bits.smb_busy) {
      break;
    }
  // Wait for timeout
  } while (GetDelayTsc (StartCount) < SMB_TIMEOUT);

  return Status;
}

/**
  Wait for smbus write to complete based on target build.

  @retval EFI_SUCCESS     Completed successfully.
  @retval !EFI_SUCCESS    Failure.
**/
EFI_STATUS
WaitForWriteToCompleteTarget (
  )
{
  SMB_STATUS_CFG    SmbStat;
  EFI_STATUS        Status;

  //
  // Wait for the write to complete
  //
  do {
    SmbStat.Data = SpdRead (SMB_STATUS_CFG_OFFSET);
  } while (!SmbStat.Bits.smb_wod && !SmbStat.Bits.smb_sbe);

  // If SMB Status indicates write completed with no error, return success
  if ((SmbStat.Bits.smb_wod == 1) && (SmbStat.Bits.smb_sbe == 0)) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

/**
  Disable TSOD to do SMBus transactions

  @retval SmbCmdData  Original CmdData before disabling TSOD

**/
UINT32
SpdDisableTsod (
  VOID
)
{
  SMB_CMD_CFG     SmbCmd;
  UINT32          SmbCmdData;

  SmbCmdData  = SpdRead (SMB_CMD_CFG_OFFSET);
  SmbCmd.Data = SmbCmdData;
  if (SmbCmd.Bits.smb_tsod_poll_en == 1) {
    SmbCmd.Bits.smb_tsod_poll_en = 0;
    SpdWrite (SMB_CMD_CFG_OFFSET, SmbCmd.Data);
  }

  return SmbCmdData;
}

/**
  Restore TSOD based on the saved value

  @param[in] SaveSmbCmdData  Saved TSOD setting

**/
VOID
SpdRestoreTsod (
  IN  UINT32    SaveSmbCmdData
)
{
  SMB_CMD_CFG     SmbCmd;

  SmbCmd.Data = SaveSmbCmdData;
  if (SmbCmd.Bits.smb_tsod_poll_en == 1) {
    SpdWrite (SMB_CMD_CFG_OFFSET, SmbCmd.Data);
  }
}

/**
  Get SMBus slave address for [MC][CH][DI]

  @param[in]  Channel       Channel Index
  @param[in]  Dimm          Dimm Index
  @param[out] StrapAddress  Slave address to read the SPD byte

  @retval EFI_SUCCESS       Strap address returned successfully
**/
EFI_STATUS
SpdGetSmbStrap (
  IN  UINT8     Channel,
  IN  UINT8     Dimm,
  OUT UINT16   *StrapAddress
)
{

  *StrapAddress = Dimm + ((Channel % (MAX_CHANNEL * MAX_IMC / 2)) * MAX_DIMM);

  return EFI_SUCCESS;
}

/**
  Send command to select EEPROM Page for specific ByteOffset.
  And return relative offset in a page.

  @param[in]  ByteOffset  ByteOffset to get the Data from SPD EEPROM
  @param[out] SmbOffset   Relative Byte Offset in EEPROM page

  @retval EFI_SUCCESS       Data read successfully
**/
EFI_STATUS
SpdSelectPageAndOffset (
  IN     UINT16   ByteOffset,
  IN OUT UINT8   *SmbOffset
)
{
  SMB_CMD_CFG     SmbCmd;
  SMB_DATA_CFG    SmbData;
  UINT8           Data;
  INT16           PageOffset;
  UINT32          SaveSmbCmdData;
  EFI_STATUS      Status;

  Data        = 0;
  PageOffset  = 0;

  //
  // Since we are reading bytes sequentially,
  // select page only once for a DDR4_page_size bytes.
  //
  if (ByteOffset % SPD_EE_PAGE_SIZE == 0) {
    //
    // Disable Tsod
    //
    SaveSmbCmdData = SpdDisableTsod();

    //
    // Prepare Smb command to select Page
    //
    SmbCmd.Data                 = 0;
    SmbCmd.Bits.smb_ckovrd      = 1;
    SmbCmd.Bits.smb_dis_wrt     = 0;
    SmbCmd.Bits.smb_dti         = DTI_WP_EEPROM;
    SmbCmd.Bits.smb_ba          = (UINT32) 0;
    SmbCmd.Bits.smb_wrt         = 1;
    SmbCmd.Bits.smb_word_access = 0;
    SmbCmd.Bits.smb_pntr_sel    = 0;
    SmbCmd.Bits.smb_cmd_trigger = 1;
    // Select appropriate page based on SPD ByteOffset
    if (ByteOffset > 255) {
      SmbCmd.Bits.smb_sa        = SPD_EE_PAGE_SELECT_1;
    } else {
      SmbCmd.Bits.smb_sa        = SPD_EE_PAGE_SELECT_0;
    }

    //
    // Prepare data to write to select the page
    //
    SmbData.Data                = 0;
    SmbData.Bits.smb_wdata      = (UINT32) Data;

    //
    // Send Write data & command
    //
    WaitForHostNotBusyTarget ();
    SpdWrite (SMB_DATA_CFG_OFFSET, SmbData.Data);
    SpdWrite (SMB_CMD_CFG_OFFSET,  SmbCmd.Data );

    Status = WaitForWriteToCompleteTarget ();
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // Restore Tsod
    //
    SpdRestoreTsod (SaveSmbCmdData);
  }

  //
  // Adjust SmbOffset
  //
  if (ByteOffset > 255) {
    PageOffset -= 256;
  }
  *SmbOffset = (UINT8) (ByteOffset + PageOffset);

  return EFI_SUCCESS;
}

/**
  Read actual HW to get SPD

  @param[in]  StrapAddress  SMBus Slave Address to read correct SPD byteoffset
  @param[in]  SmbOffset     Relative Byte Offset in EEPROM page
  @param[out] Data          Value of the data at SmbOffset

  @retval EFI_SUCCESS       Data read successfully
  @retval EFI_DEVICE_ERROR  If error in data reading
**/
EFI_STATUS
SpdReadData (
  IN  UINT16      StrapAddress,
  IN  UINT8       SmbOffset,
  OUT UINT8      *Data
)
{
  SMB_CMD_CFG     SmbCmd;
  SMB_DATA_CFG    SmbData;
  SMB_STATUS_CFG  SmbStat;
  UINT32          SaveSmbCmdData;

  //
  // Disable Tsod
  //
  SaveSmbCmdData = SpdDisableTsod();

  //
  // Prepare Smb command
  //
  SmbCmd.Data                 = 0;
  SmbCmd.Bits.smb_ckovrd      = 1;
  SmbCmd.Bits.smb_ba          = (UINT32) SmbOffset;
  SmbCmd.Bits.smb_dti         = DTI_EEPROM;
  SmbCmd.Bits.smb_sa          = (UINT32) StrapAddress;
  SmbCmd.Bits.smb_wrt         = 0;
  SmbCmd.Bits.smb_pntr_sel    = 0;
  SmbCmd.Bits.smb_word_access = 0;
  SmbCmd.Bits.smb_cmd_trigger = 1;

  //
  // Send Read Command
  //
  WaitForHostNotBusyTarget ();
  SpdWrite (SMB_CMD_CFG_OFFSET, SmbCmd.Data);
  WaitForHostNotBusyTarget ();

  //
  // Wait for the data
  //
  SmbStat.Data = SpdRead (SMB_STATUS_CFG_OFFSET);
  while (!SmbStat.Bits.smb_rdo && !SmbStat.Bits.smb_sbe) {
    SmbStat.Data = SpdRead (SMB_STATUS_CFG_OFFSET);
  }

  //
  // Read Data
  //
  if (SmbStat.Bits.smb_rdo && !SmbStat.Bits.smb_sbe) {
    SmbData.Data = SpdRead (SMB_DATA_CFG_OFFSET);
  } else {
    return EFI_DEVICE_ERROR;
  }

  //
  // Restore Tsod
  //
  SpdRestoreTsod (SaveSmbCmdData);

  //
  // return Data
  //
  *Data = (UINT8) SmbData.Bits.smb_rdata;

  return EFI_SUCCESS;
}

/**
  Driver to read actual HW to get SPD

  @param[in]  Channel     Channel Index
  @param[in]  Dimm        Dimm Index
  @param[in]  ByteOffset  ByteOffset to get the Data from SPD EEPROM
  @param[out] Data        Value of SPD byte @ ByteOffset

  @retval EFI_SUCCESS     Data read successfully
  @retval Status          If error in data reading
**/
EFI_STATUS
SpdGetFromHw (
  IN  UINT8       Channel,
  IN  UINT8       Dimm,
  IN  UINT16      ByteOffset,
  OUT UINT8      *Data
)
{
  EFI_STATUS      Status;
  UINT8           SmbOffset;
  UINT16          StrapAddress;

  //
  // Prepare Smb data
  //
  Status = SpdGetSmbStrap(Channel, Dimm, &StrapAddress);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Select Page and Offset
  //
  Status = SpdSelectPageAndOffset (ByteOffset, &SmbOffset);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Read SPD byte offset
  //
  Status = SpdReadData (StrapAddress, SmbOffset, Data);

  return Status;
}

/**
  Adjust the SMBUS frequency to the specified value.

  @param[in] ClkFrequency - Clock Frequency to be programmed

  @retval EFI_SUCCESS           - Clock Period programmed successfully
**/
EFI_STATUS
AdjustProcSmbFreq (
  IN SMB_CLOCK_FREQUENCY  ClkFrequency
  )
{
  SMB_PERIOD_CFG      SmbPeriod;

  SmbPeriod.Data = SpdRead (SMB_PERIOD_CFG_OFFSET);

  switch (ClkFrequency) {
  case SmbClk100K:
    if ((SmbPeriod.Bits.smb_clk_prd == SMB_100K_CLK_PERIOD) && (SmbPeriod.Bits.smb_clk_offset == SMB_100K_CLK_OFFSET)) {
      return EFI_SUCCESS;
    }

    SmbPeriod.Bits.smb_clk_prd = SMB_100K_CLK_PERIOD;
    SmbPeriod.Bits.smb_clk_offset = SMB_100K_CLK_OFFSET;
    break;

  case SmbClk400K:
    if ((SmbPeriod.Bits.smb_clk_prd == SMB_400K_CLK_PERIOD) && (SmbPeriod.Bits.smb_clk_offset == SMB_400K_CLK_OFFSET)) {
      return EFI_SUCCESS;
    }

    SmbPeriod.Bits.smb_clk_prd = SMB_400K_CLK_PERIOD;
    SmbPeriod.Bits.smb_clk_offset = SMB_400K_CLK_OFFSET;
    break;

  case SmbClk700K:
    if ((SmbPeriod.Bits.smb_clk_prd == SMB_700K_CLK_PERIOD) && (SmbPeriod.Bits.smb_clk_offset == SMB_700K_CLK_OFFSET)) {
      return EFI_SUCCESS;
    }

    SmbPeriod.Bits.smb_clk_prd = SMB_700K_CLK_PERIOD;
    SmbPeriod.Bits.smb_clk_offset = SMB_700K_CLK_OFFSET;
    break;

  case SmbClk1M:
    if ((SmbPeriod.Bits.smb_clk_prd == SMB_1M_CLK_PERIOD) && (SmbPeriod.Bits.smb_clk_offset == SMB_1M_CLK_OFFSET)) {
      return EFI_SUCCESS;
    }

    SmbPeriod.Bits.smb_clk_prd = SMB_1M_CLK_PERIOD;
    SmbPeriod.Bits.smb_clk_offset = SMB_1M_CLK_OFFSET;
    break;

  default:
    DEBUG ((DEBUG_INFO, "Invalid SMBUS ClkFrequency Parameter: %x\n", ClkFrequency));
    return EFI_INVALID_PARAMETER;
    break;
  }

  SpdWrite (SMB_PERIOD_CFG_OFFSET, SmbPeriod.Data);

  return EFI_SUCCESS;
}

/**
  Initialize SMBUS controller

  @retval EFI_SUCCESS      Smbus initalized successfully
**/
EFI_STATUS
InitProcSmb (
  VOID
  )
{
  SMB_STATUS_CFG  SmbStat;
  SMB_CMD_CFG     SmbCmd;

  // Initialize SMBus speed to 400Khz for DIMM detection.
  AdjustProcSmbFreq (SmbClk400K);

  //
  // Check if the controller is busy
  //
  SmbStat.Data = SpdRead (SMB_STATUS_CFG_OFFSET);
  if (SmbStat.Bits.smb_busy) {
    SmbCmd.Data = SpdRead (SMB_CMD_CFG_OFFSET);

    //
    // Override the clock
    //
    SmbCmd.Bits.smb_ckovrd = 0;
    SmbCmd.Bits.smb_soft_rst = 1;
    SpdWrite (SMB_CMD_CFG_OFFSET, SmbCmd.Data);
    //
    // Wait 35ms
    //
    MicroSecondDelay (35000);
    SmbCmd.Bits.smb_ckovrd   = 1;
    SmbCmd.Bits.smb_soft_rst = 0;
    SpdWrite (SMB_CMD_CFG_OFFSET, SmbCmd.Data);
  }

  //
  // Make Sure TSOD Polling is disabled
  //
  SmbCmd.Data = SpdRead (SMB_CMD_CFG_OFFSET);
  SmbCmd.Bits.smb_tsod_poll_en = 0;
  SpdWrite (SMB_CMD_CFG_OFFSET, SmbCmd.Data);

  return EFI_SUCCESS;
}

/**
  Main driver function to get SPD from SMBus

  @param[in]  Channel   Channel Index
  @param[in]  Dimm      Dimm Index
  @param[in]  Count     Number of SPD bytes (DDR4 = 512)
  @param[out] SpdBytes  Pointer to SpdBytes array

  @retval EFI_SUCCESS           Smbus SPD get successfully
  @retval EFI_INVALID_PARAMETER NULL input pointer
  @retval EFI_UNSUPPORTED       If no DIMM present or invalid SPD data
  @retval EFI_ERROR             otherwise
**/
EFI_STATUS
EFIAPI
SpdGetFromSmb (
  IN  UINT8     Channel,
  IN  UINT8     Dimm,
  IN  UINT16    Count,
  OUT UINT8    *SpdBytes
)
{
  EFI_STATUS    Status;
  UINT16        ByteOffset;
  UINT8         Data;

  if (SpdBytes == NULL || Count == 0) {
    DEBUG ((DEBUG_INFO, "Invalid SpdBytes struct\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Init Processor Smb
  //
  Status = InitProcSmb ();

  //
  // Read bytes from DDR4 SPD
  //
  for (ByteOffset = 0; ByteOffset < Count; ByteOffset++) {
    Status = SpdGetFromHw (Channel, Dimm, ByteOffset, &Data);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    // If first SPD byte is 0x0, means there is no DIMM
    if (ByteOffset == 0x0 && Data == 0x0) {
      return EFI_UNSUPPORTED;
    }
    SpdBytes[ByteOffset] = Data;
  }

  return Status;
}
