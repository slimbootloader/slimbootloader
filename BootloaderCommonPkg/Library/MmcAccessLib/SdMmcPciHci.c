/** @file
  This driver is used to manage SD/MMC PCI host controllers which are compliance
  with SD Host Controller Simplified Specification version 3.00.

  It would expose EFI_SD_MMC_PASS_THRU_PROTOCOL for upper layer use.

  Copyright (c) 2015 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/IoMmuLib.h>
#include "SdMmcPciHcDxe.h"

/**
  Dump the content of SD/MMC host controller's Capability Register.

  @param[in]  Capability      The buffer to store the capability data.

**/
VOID
DumpCapabilityReg (
  IN SD_MMC_HC_SLOT_CAP   *Capability
  )
{
  //
  // Dump Capability Data
  //
  DEBUG ((DEBUG_VERBOSE, " == Capability is 0x%x ==\n", Capability));
  DEBUG ((DEBUG_VERBOSE, "   Timeout Clk Freq  %d%a\n", Capability->TimeoutFreq,
          (Capability->TimeoutUnit) ? "MHz" : "KHz"));
  DEBUG ((DEBUG_VERBOSE, "   Base Clk Freq     %dMHz\n", Capability->BaseClkFreq));
  DEBUG ((DEBUG_VERBOSE, "   Max Blk Len       %dbytes\n", 512 * (1 << Capability->MaxBlkLen)));
  DEBUG ((DEBUG_VERBOSE, "   8-bit Support     %a\n", Capability->BusWidth8 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   ADMA2 Support     %a\n", Capability->Adma2 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   HighSpeed Support %a\n", Capability->HighSpeed ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   SDMA Support      %a\n", Capability->Sdma ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Suspend/Resume    %a\n", Capability->SuspRes ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Voltage 3.3       %a\n", Capability->Voltage33 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Voltage 3.0       %a\n", Capability->Voltage30 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Voltage 1.8       %a\n", Capability->Voltage18 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   64-bit Sys Bus    %a\n", Capability->SysBus64 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Async Interrupt   %a\n", Capability->AsyncInt ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   SlotType          "));
  if (Capability->SlotType == 0x00) {
    DEBUG ((DEBUG_VERBOSE, "%a\n", "Removable Slot"));
  } else if (Capability->SlotType == 0x01) {
    DEBUG ((DEBUG_VERBOSE, "%a\n", "Embedded Slot"));
  } else if (Capability->SlotType == 0x02) {
    DEBUG ((DEBUG_VERBOSE, "%a\n", "Shared Bus Slot"));
  } else {
    DEBUG ((DEBUG_VERBOSE, "%a\n", "Reserved"));
  }
  DEBUG ((DEBUG_VERBOSE, "   SDR50  Support    %a\n", Capability->Sdr50 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   SDR104 Support    %a\n", Capability->Sdr104 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   DDR50  Support    %a\n", Capability->Ddr50 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Driver Type A     %a\n", Capability->DriverTypeA ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Driver Type C     %a\n", Capability->DriverTypeC ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Driver Type D     %a\n", Capability->DriverTypeD ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Driver Type 4     %a\n", Capability->DriverType4 ? "TRUE" : "FALSE"));
  if (Capability->TimerCount == 0) {
    DEBUG ((DEBUG_VERBOSE, "   Retuning TimerCnt Disabled\n", 2 * (Capability->TimerCount - 1)));
  } else {
    DEBUG ((DEBUG_VERBOSE, "   Retuning TimerCnt %dseconds\n", 2 * (Capability->TimerCount - 1)));
  }
  DEBUG ((DEBUG_VERBOSE, "   SDR50 Tuning      %a\n", Capability->TuningSDR50 ? "TRUE" : "FALSE"));
  DEBUG ((DEBUG_VERBOSE, "   Retuning Mode     Mode %d\n", Capability->RetuningMod + 1));
  DEBUG ((DEBUG_VERBOSE, "   Clock Multiplier  M = %d\n", Capability->ClkMultiplier + 1));
  DEBUG ((DEBUG_VERBOSE, "   HS 400            %a\n", Capability->Hs400 ? "TRUE" : "FALSE"));
  return;
}

/**
  Read/Write specified SD/MMC host controller mmio register.

  @param[in]      Address      Host controller mmio base address.
  @param[in]      Offset       The offset within the selected BAR to start the
                               memory operation.
  @param[in]      Read         A boolean to indicate it's read or write operation.
  @param[in]      Count        The width of the mmio register in bytes.
                               Must be 1, 2 , 4 or 8 bytes.
  @param[in, out] Data         For read operations, the destination buffer to store
                               the results. For write operations, the source buffer
                               to write data from. The caller is responsible for
                               having ownership of the data buffer and ensuring its
                               size not less than Count bytes.

  @retval EFI_INVALID_PARAMETER The PciIo or Data is NULL or the Count is not valid.
  @retval EFI_SUCCESS           The read/write operation succeeds.
  @retval Others                The read/write operation fails.

**/
EFI_STATUS
EFIAPI
SdMmcHcRwMmio (
  IN     UINT32                Address,
  IN     UINT32                Offset,
  IN     BOOLEAN               Read,
  IN     UINT8                 Count,
  IN OUT VOID                  *Data
  )
{
  if ((Address == 0x0) || (Data == NULL))  {
    return EFI_INVALID_PARAMETER;
  }

  if ((Count != 1) && (Count != 2) && (Count != 4) && (Count != 8)) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Count) {
  case 1:
    if (Read) {
      * (UINT8 *)Data = MmioRead8 (Address + Offset);
    } else {
      MmioWrite8 (Address + Offset, * (UINT8 *)Data);
    }
    break;
  case 2:
    if (Read) {
      * (UINT16 *)Data = MmioRead16 (Address + Offset);
    } else {
      MmioWrite16 (Address + Offset, * (UINT16 *)Data);
    }
    break;
  case 4:
    if (Read) {
      * (UINT32 *)Data = MmioRead32 (Address + Offset);
    } else {
      MmioWrite32 (Address + Offset, * (UINT32 *)Data);
    }
    break;
  case 8:
    if (Read) {
      * (UINT64 *)Data = MmioRead64 (Address + Offset);
    } else {
      MmioWrite64 (Address + Offset, * (UINT64 *)Data);
    }
    break;
  default:
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Do OR operation with the value of the specified SD/MMC host controller mmio register.

  @param[in] Address           Host controller mmio base address.
  @param[in] Offset            The offset within the selected BAR to start the
                               memory operation.
  @param[in] Count             The width of the mmio register in bytes.
                               Must be 1, 2 , 4 or 8 bytes.
  @param[in] OrData            The pointer to the data used to do OR operation.
                               The caller is responsible for having ownership of
                               the data buffer and ensuring its size not less than
                               Count bytes.

  @retval EFI_INVALID_PARAMETER The PciIo or OrData is NULL or the Count is not valid.
  @retval EFI_SUCCESS           The OR operation succeeds.
  @retval Others                The OR operation fails.

**/
EFI_STATUS
EFIAPI
SdMmcHcOrMmio (
  IN  UINT32                   Address,
  IN  UINT32                   Offset,
  IN  UINT8                    Count,
  IN  VOID                     *OrData
  )
{
  EFI_STATUS                   Status;
  UINT64                       Data;
  UINT64                       Or;

  Status = SdMmcHcRwMmio (Address, Offset, TRUE, Count, &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Count == 1) {
    Or = * (UINT8 *) OrData;
  } else if (Count == 2) {
    Or = * (UINT16 *) OrData;
  } else if (Count == 4) {
    Or = * (UINT32 *) OrData;
  } else if (Count == 8) {
    Or = * (UINT64 *) OrData;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  Data  |= Or;
  Status = SdMmcHcRwMmio (Address, Offset, FALSE, Count, &Data);

  return Status;
}

/**
  Do AND operation with the value of the specified SD/MMC host controller mmio register.

  @param[in] Address           Host controller mmio base address.
  @param[in] Offset            The offset within the selected BAR to start the
                               memory operation.
  @param[in] Count             The width of the mmio register in bytes.
                               Must be 1, 2 , 4 or 8 bytes.
  @param[in] AndData           The pointer to the data used to do AND operation.
                               The caller is responsible for having ownership of
                               the data buffer and ensuring its size not less than
                               Count bytes.

  @retval EFI_INVALID_PARAMETER The PciIo or AndData is NULL or the Count is not valid.
  @retval EFI_SUCCESS           The AND operation succeeds.
  @retval Others                The AND operation fails.

**/
EFI_STATUS
EFIAPI
SdMmcHcAndMmio (
  IN  UINT32                   Address,
  IN  UINT32                   Offset,
  IN  UINT8                    Count,
  IN  VOID                     *AndData
  )
{
  EFI_STATUS                   Status;
  UINT64                       Data;
  UINT64                       And;

  Status = SdMmcHcRwMmio (Address, Offset, TRUE, Count, &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Count == 1) {
    And = * (UINT8 *) AndData;
  } else if (Count == 2) {
    And = * (UINT16 *) AndData;
  } else if (Count == 4) {
    And = * (UINT32 *) AndData;
  } else if (Count == 8) {
    And = * (UINT64 *) AndData;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  Data  &= And;
  Status = SdMmcHcRwMmio (Address, Offset, FALSE, Count, &Data);

  return Status;
}

/**
  Wait for the value of the specified MMIO register set to the test value.

  @param[in]  Address       Host controller mmio base address.
  @param[in]  Offset        The offset within the selected BAR to start the
                            memory operation.
  @param[in]  Count         The width of the mmio register in bytes.
                            Must be 1, 2, 4 or 8 bytes.
  @param[in]  MaskValue     The mask value of memory.
  @param[in]  TestValue     The test value of memory.

  @retval EFI_NOT_READY     The MMIO register hasn't set to the expected value.
  @retval EFI_SUCCESS       The MMIO register has expected value.
  @retval Others            The MMIO operation fails.

**/
EFI_STATUS
EFIAPI
SdMmcHcCheckMmioSet (
  IN  UINT32                    Address,
  IN  UINT32                    Offset,
  IN  UINT8                     Count,
  IN  UINT64                    MaskValue,
  IN  UINT64                    TestValue
  )
{
  EFI_STATUS            Status;
  UINT64                Value;

  //
  // Access PCI MMIO space to see if the value is the tested one.
  //
  Value  = 0;
  Status = SdMmcHcRwMmio (Address, Offset, TRUE, Count, &Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Value &= MaskValue;

  if (Value == TestValue) {
    return EFI_SUCCESS;
  }

  return EFI_NOT_READY;
}

/**
  Wait for the value of the specified MMIO register set to the test value.

  @param[in]  Address       Host controller mmio base address.
  @param[in]  Offset        The offset within the selected BAR to start the
                            memory operation.
  @param[in]  Count         The width of the mmio register in bytes.
                            Must be 1, 2, 4 or 8 bytes.
  @param[in]  MaskValue     The mask value of memory.
  @param[in]  TestValue     The test value of memory.
  @param[in]  Timeout       The time out value for wait memory set, uses 1
                            microsecond as a unit.

  @retval EFI_TIMEOUT       The MMIO register hasn't expected value in timeout
                            range.
  @retval EFI_SUCCESS       The MMIO register has expected value.
  @retval Others            The MMIO operation fails.

**/
EFI_STATUS
EFIAPI
SdMmcHcWaitMmioSet (
  IN  UINT32                    Address,
  IN  UINT32                    Offset,
  IN  UINT8                     Count,
  IN  UINT64                    MaskValue,
  IN  UINT64                    TestValue,
  IN  UINT64                    Timeout
  )
{
  EFI_STATUS            Status;
  BOOLEAN               InfiniteWait;

  if (Timeout == 0) {
    InfiniteWait = TRUE;
  } else {
    InfiniteWait = FALSE;
  }

  while (InfiniteWait || (Timeout > 0)) {
    Status = SdMmcHcCheckMmioSet (
               Address,
               Offset,
               Count,
               MaskValue,
               TestValue
               );
    if (Status != EFI_NOT_READY) {
      return Status;
    }

    //
    // Stall for 1 microsecond.
    //
    MicroSecondDelay (1);

    Timeout--;
  }

  return EFI_TIMEOUT;
}

/**
  Software reset the specified SD/MMC host controller and enable all interrupts.

  @param[in]  Address       Host controller mmio base address.

  @retval EFI_SUCCESS       The software reset executes successfully.
  @retval Others            The software reset fails.

**/
EFI_STATUS
SdMmcHcReset (
  IN UINT32                 Address
  )
{
  EFI_STATUS                Status;
  UINT8                     SwReset;

  SwReset = 0xFF;
  Status  = SdMmcHcRwMmio (Address, SD_MMC_HC_SW_RST, FALSE, sizeof (SwReset), &SwReset);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SdMmcHcReset: write full 1 fails: %r\n", Status));
    return Status;
  }

  Status = SdMmcHcWaitMmioSet (
             Address,
             SD_MMC_HC_SW_RST,
             sizeof (SwReset),
             0xFF,
             0x00,
             SD_MMC_HC_GENERIC_TIMEOUT
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "SdMmcHcReset: reset done with %r\n", Status));
    return Status;
  }

  //
  // Enable all interrupt after reset all.
  //
  Status = SdMmcHcEnableInterrupt (Address);

  return Status;
}

/**
  Set all interrupt status bits in Normal and Error Interrupt Status Enable
  register.

  @param[in]  Address       Host controller mmio base address.

  @retval EFI_SUCCESS       The operation executes successfully.
  @retval Others            The operation fails.

**/
EFI_STATUS
SdMmcHcEnableInterrupt (
  IN UINT32                 Address
  )
{
  EFI_STATUS                Status;
  UINT16                    IntStatus;

  //
  // Enable all bits in Error Interrupt Status Enable Register
  //
  IntStatus = 0xFFFF;
  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_ERR_INT_STS_EN, FALSE, sizeof (IntStatus), &IntStatus);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Enable all bits in Normal Interrupt Status Enable Register
  //
  IntStatus = 0xFFFF;
  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_NOR_INT_STS_EN, FALSE, sizeof (IntStatus), &IntStatus);

  return Status;
}

/**
  Get the capability data from the specified slot.

  @param[in]  Address         Host controller mmio base address.
  @param[out] Capability      The buffer to store the capability data.

  @retval EFI_SUCCESS         The operation executes successfully.
  @retval Others              The operation fails.

**/
EFI_STATUS
SdMmcHcGetCapability (
  IN     UINT32                Address,
  OUT    SD_MMC_HC_SLOT_CAP   *Capability
  )
{
  EFI_STATUS                Status;
  UINT64                    Cap;

  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_CAP, TRUE, sizeof (Cap), &Cap);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Capability, &Cap, sizeof (Cap));

  return EFI_SUCCESS;
}

/**
  Get the maximum current capability data from the specified slot.

  @param[in]  Address         Host controller mmio base address.
  @param[out] MaxCurrent      The buffer to store the maximum current capability data.

  @retval EFI_SUCCESS         The operation executes successfully.
  @retval Others              The operation fails.

**/
EFI_STATUS
SdMmcHcGetMaxCurrent (
  IN  UINT32                Address,
  OUT UINT64               *MaxCurrent
  )
{
  EFI_STATUS          Status;

  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_MAX_CURRENT_CAP, TRUE, sizeof (UINT64), MaxCurrent);

  return Status;
}

/**
  Detect whether there is a SD/MMC card attached at the specified SD/MMC host controller
  slot.

  Refer to SD Host Controller Simplified spec 3.0 Section 3.1 for details.

  @param[in]  Address       Host controller mmio base address.
  @param[out] MediaPresent  The pointer to the media present boolean value.

  @retval EFI_SUCCESS       There is no media change happened.
  @retval EFI_MEDIA_CHANGED There is media change happened.
  @retval Others            The detection fails.

**/
EFI_STATUS
SdMmcHcCardDetect (
  IN  UINT32                Address,
  OUT BOOLEAN              *MediaPresent
  )
{
  EFI_STATUS                Status;
  UINT16                    Data;
  UINT32                    PresentState;

  //
  // Check Present State Register to see if there is a card presented.
  //
  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_PRESENT_STATE, TRUE, sizeof (PresentState), &PresentState);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((PresentState & BIT16) != 0) {
    *MediaPresent = TRUE;
  } else {
    *MediaPresent = FALSE;
  }

  //
  // Check Normal Interrupt Status Register
  //
  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_NOR_INT_STS, TRUE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((Data & (BIT6 | BIT7)) != 0) {
    //
    // Clear BIT6 and BIT7 by writing 1 to these two bits if set.
    //
    Data  &= BIT6 | BIT7;
    Status = SdMmcHcRwMmio (Address, SD_MMC_HC_NOR_INT_STS, FALSE, sizeof (Data), &Data);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    return EFI_MEDIA_CHANGED;
  }

  return EFI_SUCCESS;
}

/**
  Stop SD/MMC card clock.

  Refer to SD Host Controller Simplified spec 3.0 Section 3.2.2 for details.

  @param[in]  Address       Host controller mmio base address.

  @retval EFI_SUCCESS       Succeed to stop SD/MMC clock.
  @retval Others            Fail to stop SD/MMC clock.

**/
EFI_STATUS
SdMmcHcStopClock (
  IN UINT32                 Address
  )
{
  EFI_STATUS                Status;
  UINT32                    PresentState;
  UINT16                    ClockCtrl;

  //
  // Ensure no SD transactions are occurring on the SD Bus by
  // waiting for Command Inhibit (DAT) and Command Inhibit (CMD)
  // in the Present State register to be 0.
  //
  Status = SdMmcHcWaitMmioSet (
             Address,
             SD_MMC_HC_PRESENT_STATE,
             sizeof (PresentState),
             BIT0 | BIT1,
             0,
             SD_MMC_HC_GENERIC_TIMEOUT
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set SD Clock Enable in the Clock Control register to 0
  //
  ClockCtrl = (UINT16)~BIT2;
  Status = SdMmcHcAndMmio (Address, SD_MMC_HC_CLOCK_CTRL, sizeof (ClockCtrl), &ClockCtrl);

  return Status;
}

/**
  SD/MMC card clock supply.

  Refer to SD Host Controller Simplified spec 3.0 Section 3.2.1 for details.

  @param[in] Address        Host controller mmio base address.
  @param[in] ClockFreq      The max clock frequency to be set. The unit is KHz.
  @param[in] Capability     The capability of the slot.

  @retval EFI_SUCCESS       The clock is supplied successfully.
  @retval Others            The clock isn't supplied successfully.

**/
EFI_STATUS
SdMmcHcClockSupply (
  IN UINT32                 Address,
  IN UINT64                 ClockFreq,
  IN SD_MMC_HC_SLOT_CAP     Capability
  )
{
  EFI_STATUS                Status;
  UINT32                    BaseClkFreq;
  UINT32                    SettingFreq;
  UINT32                    Divisor;
  UINT32                    Remainder;
  UINT16                    ControllerVer;
  UINT16                    ClockCtrl;

  //
  // Calculate a divisor for SD clock frequency
  //
  ASSERT (Capability.BaseClkFreq != 0);

  BaseClkFreq = Capability.BaseClkFreq;
  if (ClockFreq == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (ClockFreq > (BaseClkFreq * 1000)) {
    ClockFreq = BaseClkFreq * 1000;
  }

  //
  // Calculate the divisor of base frequency.
  //
  Divisor     = 0;
  SettingFreq = BaseClkFreq * 1000;
  while (ClockFreq < SettingFreq) {
    Divisor++;

    SettingFreq = (BaseClkFreq * 1000) / (2 * Divisor);
    Remainder   = (BaseClkFreq * 1000) % (2 * Divisor);
    if ((ClockFreq == SettingFreq) && (Remainder == 0)) {
      break;
    }
    if ((ClockFreq == SettingFreq) && (Remainder != 0)) {
      SettingFreq ++;
    }
  }

  //DEBUG ((DEBUG_INFO, "BaseClkFreq %dMHz Divisor %d ClockFreq %dKhz\n", BaseClkFreq, Divisor, ClockFreq));

  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_CTRL_VER, TRUE, sizeof (ControllerVer), &ControllerVer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Set SDCLK Frequency Select and Internal Clock Enable fields in Clock Control register.
  //
  if ((ControllerVer & 0xFF) == 2) {
    ASSERT (Divisor <= 0x3FF);
    ClockCtrl = ((Divisor & 0xFF) << 8) | ((Divisor & 0x300) >> 2);
  } else if (((ControllerVer & 0xFF) == 0) || ((ControllerVer & 0xFF) == 1)) {
    //
    // Only the most significant bit can be used as divisor.
    //
    if (((Divisor - 1) & Divisor) != 0) {
      Divisor = 1 << (HighBitSet32 (Divisor) + 1);
    }
    ASSERT (Divisor <= 0x80);
    ClockCtrl = (Divisor & 0xFF) << 8;
  } else {
    DEBUG ((DEBUG_ERROR, "Unknown SD Host Controller Spec version [0x%x]!!!\n", ControllerVer));
    return EFI_UNSUPPORTED;
  }

  //
  // Stop bus clock at first
  //
  Status = SdMmcHcStopClock (Address);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Supply clock frequency with specified divisor
  //
  ClockCtrl |= BIT0;
  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_CLOCK_CTRL, FALSE, sizeof (ClockCtrl), &ClockCtrl);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Set SDCLK Frequency Select and Internal Clock Enable fields fails\n"));
    return Status;
  }

  //
  // Wait Internal Clock Stable in the Clock Control register to be 1
  //
  Status = SdMmcHcWaitMmioSet (
             Address,
             SD_MMC_HC_CLOCK_CTRL,
             sizeof (ClockCtrl),
             BIT1,
             BIT1,
             SD_MMC_HC_GENERIC_TIMEOUT
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set SD Clock Enable in the Clock Control register to 1
  //
  ClockCtrl = BIT2;
  Status = SdMmcHcOrMmio (Address, SD_MMC_HC_CLOCK_CTRL, sizeof (ClockCtrl), &ClockCtrl);

  return Status;
}

/**
  SD/MMC bus power control.

  Refer to SD Host Controller Simplified spec 3.0 Section 3.3 for details.

  @param[in] Address        Host controller mmio base address.
  @param[in] PowerCtrl      The value setting to the power control register.

  @retval TRUE              There is a SD/MMC card attached.
  @retval FALSE             There is no a SD/MMC card attached.

**/
EFI_STATUS
SdMmcHcPowerControl (
  IN UINT32                 Address,
  IN UINT8                  PowerCtrl
  )
{
  EFI_STATUS                Status;

  //
  // Clr SD Bus Power
  //
  PowerCtrl &= (UINT8)~BIT0;
  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_POWER_CTRL, FALSE, sizeof (PowerCtrl), &PowerCtrl);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set SD Bus Voltage Select and SD Bus Power fields in Power Control Register
  //
  PowerCtrl |= BIT0;
  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_POWER_CTRL, FALSE, sizeof (PowerCtrl), &PowerCtrl);

  return Status;
}

/**
  Set the SD/MMC bus width.

  Refer to SD Host Controller Simplified spec 3.0 Section 3.4 for details.

  @param[in] Address        Host controller mmio base address.
  @param[in] BusWidth       The bus width used by the SD/MMC device, it must be 1, 4 or 8.

  @retval EFI_SUCCESS       The bus width is set successfully.
  @retval Others            The bus width isn't set successfully.

**/
EFI_STATUS
SdMmcHcSetBusWidth (
  IN UINT32                 Address,
  IN UINT16                 BusWidth
  )
{
  EFI_STATUS                Status;
  UINT8                     HostCtrl1;

  if (BusWidth == 1) {
    HostCtrl1 = (UINT8)~ (BIT5 | BIT1);
    Status = SdMmcHcAndMmio (Address, SD_MMC_HC_HOST_CTRL1, sizeof (HostCtrl1), &HostCtrl1);
  } else if (BusWidth == 4) {
    Status = SdMmcHcRwMmio (Address, SD_MMC_HC_HOST_CTRL1, TRUE, sizeof (HostCtrl1), &HostCtrl1);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    HostCtrl1 |= BIT1;
    HostCtrl1 &= (UINT8)~BIT5;
    Status = SdMmcHcRwMmio (Address, SD_MMC_HC_HOST_CTRL1, FALSE, sizeof (HostCtrl1), &HostCtrl1);
  } else if (BusWidth == 8) {
    Status = SdMmcHcRwMmio (Address, SD_MMC_HC_HOST_CTRL1, TRUE, sizeof (HostCtrl1), &HostCtrl1);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    HostCtrl1 &= (UINT8)~BIT1;
    HostCtrl1 |= BIT5;
    Status = SdMmcHcRwMmio (Address, SD_MMC_HC_HOST_CTRL1, FALSE, sizeof (HostCtrl1), &HostCtrl1);
  } else {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  return Status;
}

/**
  Supply SD/MMC card with lowest clock frequency at initialization.

  @param[in] Address        Host controller mmio base address.
  @param[in] Capability     The capability of the slot.

  @retval EFI_SUCCESS       The clock is supplied successfully.
  @retval Others            The clock isn't supplied successfully.

**/
EFI_STATUS
SdMmcHcInitClockFreq (
  IN UINT32                 Address,
  IN SD_MMC_HC_SLOT_CAP     Capability
  )
{
  EFI_STATUS                Status;
  UINT32                    InitFreq;

  //
  // Calculate a divisor for SD clock frequency
  //
  if (Capability.BaseClkFreq == 0) {
    //
    // Don't support get Base Clock Frequency information via another method
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Supply 400KHz clock frequency at initialization phase.
  //
  InitFreq = 400;
  Status = SdMmcHcClockSupply (Address, InitFreq, Capability);
  return Status;
}

/**
  Supply SD/MMC card with maximum voltage at initialization.

  Refer to SD Host Controller Simplified spec 3.0 Section 3.3 for details.

  @param[in] Address        Host controller mmio base address.
  @param[in] Capability     The capability of the slot.

  @retval EFI_SUCCESS       The voltage is supplied successfully.
  @retval Others            The voltage isn't supplied successfully.

**/
EFI_STATUS
SdMmcHcInitPowerVoltage (
  IN UINT32                 Address,
  IN SD_MMC_HC_SLOT_CAP     Capability
  )
{
  EFI_STATUS                Status;
  UINT8                     MaxVoltage;
  UINT8                     HostCtrl2;

  //
  // Calculate supported maximum voltage according to SD Bus Voltage Select
  //
  if (Capability.Voltage33 != 0) {
    //
    // Support 3.3V
    //
    MaxVoltage = 0x0E;
  } else if (Capability.Voltage30 != 0) {
    //
    // Support 3.0V
    //
    MaxVoltage = 0x0C;
  } else if (Capability.Voltage18 != 0) {
    //
    // Support 1.8V
    //
    MaxVoltage = 0x0A;
    HostCtrl2  = BIT3;
    Status = SdMmcHcOrMmio (Address, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);
    MicroSecondDelay (5000);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    ASSERT (FALSE);
    return EFI_DEVICE_ERROR;
  }

  //
  // Set SD Bus Voltage Select and SD Bus Power fields in Power Control Register
  //
  Status = SdMmcHcPowerControl (Address, MaxVoltage);

  return Status;
}

/**
  Initialize the Timeout Control register with most conservative value at initialization.

  Refer to SD Host Controller Simplified spec 3.0 Section 2.2.15 for details.

  @param[in] Address        Host controller mmio base address.

  @retval EFI_SUCCESS       The timeout control register is configured successfully.
  @retval Others            The timeout control register isn't configured successfully.

**/
EFI_STATUS
SdMmcHcInitTimeoutCtrl (
  IN UINT32                 Address
  )
{
  EFI_STATUS                Status;
  UINT8                     Timeout;

  Timeout = 0x0E;
  Status  = SdMmcHcRwMmio (Address, SD_MMC_HC_TIMEOUT_CTRL, FALSE, sizeof (Timeout), &Timeout);

  return Status;
}

/**
  Initial SD/MMC host controller with lowest clock frequency, max power and max timeout value
  at initialization.

  @param[in] Address        Host controller mmio base address.
  @param[in] Capability     The capability of the slot.

  @retval EFI_SUCCESS       The host controller is initialized successfully.
  @retval Others            The host controller isn't initialized successfully.

**/
EFI_STATUS
SdMmcHcInitHost (
  IN UINT32                 Address,
  IN SD_MMC_HC_SLOT_CAP     Capability
  )
{
  EFI_STATUS       Status;

  Status = SdMmcHcInitClockFreq (Address, Capability);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SdMmcHcInitPowerVoltage (Address, Capability);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SdMmcHcInitTimeoutCtrl (Address);
  return Status;
}

/**
  Turn on/off LED.

  @param[in] Address        Host controller mmio base address.
  @param[in] On             The boolean to turn on/off LED.

  @retval EFI_SUCCESS       The LED is turned on/off successfully.
  @retval Others            The LED isn't turned on/off successfully.

**/
EFI_STATUS
SdMmcHcLedOnOff (
  IN UINT32                 Address,
  IN BOOLEAN                On
  )
{
  EFI_STATUS                Status;
  UINT8                     HostCtrl1;

  if (On) {
    HostCtrl1 = BIT0;
    Status    = SdMmcHcOrMmio (Address, SD_MMC_HC_HOST_CTRL1, sizeof (HostCtrl1), &HostCtrl1);
  } else {
    HostCtrl1 = (UINT8)~BIT0;
    Status    = SdMmcHcAndMmio (Address, SD_MMC_HC_HOST_CTRL1, sizeof (HostCtrl1), &HostCtrl1);
  }

  return Status;
}

/**
  Build ADMA descriptor table for transfer.

  Refer to SD Host Controller Simplified spec 3.0 Section 1.13 for details.

  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The ADMA descriptor table is created successfully.
  @retval Others            The ADMA descriptor table isn't created successfully.

**/
EFI_STATUS
BuildAdmaDescTable (
  IN SD_MMC_HC_TRB          *Trb
  )
{
  EFI_PHYSICAL_ADDRESS      Data;
  UINT64                    DataLen;
  UINT64                    Entries;
  UINT32                    Index;
  UINT64                    Remaining;
  UINT32                    Address;
  UINTN                     TableSize;
  EFI_STATUS                Status;

  Data    = (EFI_PHYSICAL_ADDRESS) (UINTN)Trb->DataPhy;
  DataLen = Trb->DataLen;

  DEBUG ((DEBUG_INFO, "BuildAdmaDescTable Data=0x%08X DataLen=0x%08X\n", (UINT32) (UINTN)Data, (UINT32)DataLen));
  //
  // Only support 32bit ADMA Descriptor Table
  //
  if ((Data >= 0x100000000ul) || ((Data + DataLen) > 0x100000000ul)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Address field shall be set on 32-bit boundary (Lower 2-bit is always set to 0)
  // for 32-bit address descriptor table.
  //
  if ((Data & (BIT0 | BIT1)) != 0) {
    DEBUG ((DEBUG_INFO, "The buffer [0x%x] to construct ADMA desc is not aligned to 4 bytes boundary!\n", Data));
  }

  Entries   = DivU64x32 ((DataLen + ADMA_MAX_DATA_PER_LINE - 1), ADMA_MAX_DATA_PER_LINE);
  TableSize = (UINTN)MultU64x32 (Entries, sizeof (SD_MMC_HC_ADMA_DESC_LINE));
  Trb->AdmaPages = (UINT32)EFI_SIZE_TO_PAGES (TableSize);

  Status = IoMmuAllocateBuffer (
                                EFI_SIZE_TO_PAGES (TableSize),
                                (VOID **)&Trb->AdmaDesc,
                                &Trb->AdmaDescPhy,
                                &Trb->AdmaMap
                               );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem ((VOID *) (UINTN) Trb->AdmaDesc, TableSize);

  Remaining = DataLen;
  Address   = (UINT32)Data;
  for (Index = 0; Index < Entries; Index++) {
    if (Remaining <= ADMA_MAX_DATA_PER_LINE) {
      Trb->AdmaDesc[Index].Valid = 1;
      Trb->AdmaDesc[Index].Act   = 2;
      Trb->AdmaDesc[Index].Length  = (UINT16)Remaining;
      Trb->AdmaDesc[Index].Address = Address;
      break;
    } else {
      Trb->AdmaDesc[Index].Valid = 1;
      Trb->AdmaDesc[Index].Act   = 2;
      Trb->AdmaDesc[Index].Length  = 0;
      Trb->AdmaDesc[Index].Address = Address;
    }

    Remaining -= ADMA_MAX_DATA_PER_LINE;
    Address   += ADMA_MAX_DATA_PER_LINE;
  }

  //
  // Set the last descriptor line as end of descriptor table
  //
  Trb->AdmaDesc[Index].End = 1;
  return EFI_SUCCESS;
}

/**
  Sets up host memory to allow DMA transfer.

  @param[in] Trb       A pointer to the SD command TRB structure.

  @retval EFI_SUCCESS  Memory has been mapped for DMA transfer.
  @retval Others       Memory has not been mapped.
**/
EFI_STATUS
SdMmcSetupMemoryForDmaTransfer (
  IN SD_MMC_HC_TRB           *Trb
  )
{
  UINTN                         MapLength;
  EFI_STATUS                    Status;

  if (Trb->Data != NULL && Trb->DataLen != 0) {
    MapLength = Trb->DataLen;
    Status = IoMmuMap (
                      Trb->Read ? EdkiiIoMmuOperationBusMasterWrite : EdkiiIoMmuOperationBusMasterRead,
                      Trb->Data,
                      &MapLength,
                      &Trb->DataPhy,
                      &Trb->DataMap
                      );
    if (EFI_ERROR (Status) || (Trb->DataLen != MapLength)) {
      return EFI_BAD_BUFFER_SIZE;
    }
  }

  return EFI_SUCCESS;
}


/**
  Create a new TRB for the SD/MMC cmd request.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Packet         A pointer to the SD command data structure.

  @return Created Trb or NULL.

**/
SD_MMC_HC_TRB *
SdMmcCreateTrb (
  IN SD_MMC_HC_PRIVATE_DATA              *Private,
  IN EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet
  )
{
  SD_MMC_HC_TRB                 *Trb;
  EFI_STATUS                    Status;

  Trb = AllocatePool (sizeof (SD_MMC_HC_TRB));
  if (Trb == NULL) {
    return NULL;
  }
  SetMem ((VOID *)Trb, sizeof (SD_MMC_HC_TRB), 0);

  Trb->Signature = SD_MMC_HC_TRB_SIG;
  Trb->BlockSize = 0x200;
  Trb->Packet    = Packet;
  Trb->Started   = FALSE;
  Trb->Timeout   = Packet->Timeout;
  Trb->Private   = Private;

  if ((Packet->InTransferLength != 0) && (Packet->InDataBuffer != NULL)) {
    Trb->Data    = Packet->InDataBuffer;
    Trb->DataLen = Packet->InTransferLength;
    Trb->Read    = TRUE;
  } else if ((Packet->OutTransferLength != 0) && (Packet->OutDataBuffer != NULL)) {
    Trb->Data    = Packet->OutDataBuffer;
    Trb->DataLen = Packet->OutTransferLength;
    Trb->Read    = FALSE;
  } else if ((Packet->InTransferLength == 0) && (Packet->OutTransferLength == 0)) {
    Trb->Data    = NULL;
    Trb->DataLen = 0;
  } else {
    goto Error;
  }

  if ((Trb->DataLen != 0) && (Trb->DataLen < Trb->BlockSize)) {
    Trb->BlockSize = (UINT16)Trb->DataLen;
  }

  if (((Private->Slot.CardType == EmmcCardType) &&
       (Packet->SdMmcCmdBlk->CommandIndex == EMMC_SEND_TUNING_BLOCK)) ||
      ((Private->Slot.CardType == SdCardType) &&
       (Packet->SdMmcCmdBlk->CommandIndex == SD_SEND_TUNING_BLOCK))) {
    Trb->Mode = SdMmcPioMode;
  } else {
    if (Trb->DataLen == 0) {
      Trb->Mode = SdMmcNoData;
    } else if (Private->Capability.Adma2 != 0) {
      Trb->Mode = SdMmcAdmaMode;
      Status = SdMmcSetupMemoryForDmaTransfer (Trb);
      if (EFI_ERROR (Status)) {
        goto Error;
      }
      Status = BuildAdmaDescTable (Trb);
      if (EFI_ERROR (Status)) {
        goto Error;
      }
    } else if (Private->Capability.Sdma != 0) {
      Trb->Mode = SdMmcSdmaMode;
      Status = SdMmcSetupMemoryForDmaTransfer (Trb);
      if (EFI_ERROR (Status)) {
        goto Error;
      }
    } else {
      Trb->Mode = SdMmcPioMode;
    }
  }

  return Trb;

Error:
  SdMmcFreeTrb (Trb);
  return NULL;
}

/**
  Free the resource used by the TRB.

  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

**/
VOID
SdMmcFreeTrb (
  IN SD_MMC_HC_TRB           *Trb
  )
{
  if (Trb != NULL) {
    if (Trb->AdmaDesc != NULL) {
      IoMmuFreeBuffer (Trb->AdmaPages, Trb->AdmaDesc, Trb->AdmaMap);
    }
    if (Trb->DataMap != NULL) {
      IoMmuUnmap (Trb->DataMap);
    }
    FreePool (Trb);
  }
  return;
}

/**
  Check if the env is ready for execute specified TRB.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The env is ready for TRB execution.
  @retval EFI_NOT_READY     The env is not ready for TRB execution.
  @retval Others            Some erros happen.

**/
EFI_STATUS
SdMmcCheckTrbEnv (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  )
{
  EFI_STATUS                          Status;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet;
  UINT32                              PresentState;

  Packet = Trb->Packet;

  if ((Packet->SdMmcCmdBlk->CommandType == SdMmcCommandTypeAdtc) ||
      (Packet->SdMmcCmdBlk->ResponseType == SdMmcResponseTypeR1b) ||
      (Packet->SdMmcCmdBlk->ResponseType == SdMmcResponseTypeR5b)) {
    //
    // Wait Command Inhibit (CMD) and Command Inhibit (DAT) in
    // the Present State register to be 0
    //
    PresentState = BIT0 | BIT1;
  } else {
    //
    // Wait Command Inhibit (CMD) in the Present State register
    // to be 0
    //
    PresentState = BIT0;
  }

  Status = SdMmcHcCheckMmioSet (
             Private->SdMmcHcBase,
             SD_MMC_HC_PRESENT_STATE,
             sizeof (PresentState),
             PresentState,
             0
             );

  return Status;
}

/**
  Wait for the env to be ready for execute specified TRB.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The env is ready for TRB execution.
  @retval EFI_TIMEOUT       The env is not ready for TRB execution in time.
  @retval Others            Some erros happen.

**/
EFI_STATUS
SdMmcWaitTrbEnv (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  )
{
  EFI_STATUS                          Status;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet;
  UINT64                              Timeout;
  BOOLEAN                             InfiniteWait;

  //
  // Wait Command Complete Interrupt Status bit in Normal Interrupt Status Register
  //
  Packet  = Trb->Packet;
  Timeout = Packet->Timeout;
  if (Timeout == 0) {
    InfiniteWait = TRUE;
  } else {
    InfiniteWait = FALSE;
  }

  while (InfiniteWait || (Timeout > 0)) {
    //
    // Check Trb execution result by reading Normal Interrupt Status register.
    //
    Status = SdMmcCheckTrbEnv (Private, Trb);
    if (Status != EFI_NOT_READY) {
      return Status;
    }
    //
    // Stall for 1 microsecond.
    //
    MicroSecondDelay (1);

    Timeout--;
  }

  return EFI_TIMEOUT;
}

/**
  Execute the specified TRB.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The TRB is sent to host controller successfully.
  @retval Others            Some erros happen when sending this request to the host controller.

**/
EFI_STATUS
SdMmcExecTrb (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  )
{
  EFI_STATUS                          Status;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet;
  UINT32                              Address;
  UINT16                              Cmd;
  UINT16                              IntStatus;
  UINT32                              Argument;
  UINT16                              BlkCount;
  UINT16                              BlkSize;
  UINT16                              TransMode;
  UINT8                               HostCtrl1;
  UINT32                              SdmaAddr;
  UINT64                              AdmaAddr;

  Packet  = Trb->Packet;
  Address = Trb->Private->SdMmcHcBase;
  //
  // Clear all bits in Error Interrupt Status Register
  //
  IntStatus = 0xFFFF;
  Status    = SdMmcHcRwMmio (Address, SD_MMC_HC_ERR_INT_STS, FALSE, sizeof (IntStatus), (VOID *) (UINTN)&IntStatus);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Clear all bits in Normal Interrupt Status Register excepts for Card Removal & Card Insertion bits.
  //
  IntStatus = 0xFF3F;
  Status    = SdMmcHcRwMmio (Address, SD_MMC_HC_NOR_INT_STS, FALSE, sizeof (IntStatus), (VOID *) (UINTN)&IntStatus);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Set Host Control 1 register DMA Select field
  //
  if (Trb->Mode == SdMmcAdmaMode) {
    HostCtrl1 = BIT4;
    Status = SdMmcHcOrMmio (Address,  SD_MMC_HC_HOST_CTRL1, sizeof (HostCtrl1), (VOID *) (UINTN)&HostCtrl1);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  SdMmcHcLedOnOff (Address, TRUE);

  if (Trb->Mode == SdMmcSdmaMode) {
    if ((UINT64) (UINTN)Trb->DataPhy >= 0x100000000ul) {
      return EFI_INVALID_PARAMETER;
    }

    SdmaAddr = (UINT32) (UINTN)Trb->DataPhy;
    Status   = SdMmcHcRwMmio (Address, SD_MMC_HC_SDMA_ADDR, FALSE, sizeof (SdmaAddr), (VOID *) (UINTN)&SdmaAddr);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else if (Trb->Mode == SdMmcAdmaMode) {
    AdmaAddr = (UINT64) (UINTN)Trb->AdmaDescPhy;
    Status   = SdMmcHcRwMmio (Address, SD_MMC_HC_ADMA_SYS_ADDR, FALSE, sizeof (AdmaAddr), (VOID *) (UINTN)&AdmaAddr);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  BlkSize = Trb->BlockSize;
  if (Trb->Mode == SdMmcSdmaMode) {
    //
    // Set SDMA boundary to be 512K bytes.
    //
    BlkSize |= 0x7000;
  }

  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_BLK_SIZE, FALSE, sizeof (BlkSize), (VOID *) (UINTN)&BlkSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BlkCount = 0;
  if (Trb->Mode != SdMmcNoData) {
    //
    // Calcuate Block Count.
    //
    BlkCount = (UINT16) (Trb->DataLen / Trb->BlockSize);
  }
  Status   = SdMmcHcRwMmio (Address, SD_MMC_HC_BLK_COUNT, FALSE, sizeof (BlkCount), (VOID *) (UINTN)&BlkCount);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Argument = Packet->SdMmcCmdBlk->CommandArgument;
  Status   = SdMmcHcRwMmio (Address, SD_MMC_HC_ARG1, FALSE, sizeof (Argument), (VOID *) (UINTN)&Argument);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TransMode = 0;
  if (Trb->Mode != SdMmcNoData) {
    if (Trb->Mode != SdMmcPioMode) {
      TransMode |= BIT0;
    }
    if (Trb->Read) {
      TransMode |= BIT4;
    }
    if (BlkCount > 1) {
      TransMode |= BIT5 | BIT1;
    }
    //
    // Only SD memory card needs to use AUTO CMD12 feature.
    //
    if (Private->Slot.CardType == SdCardType) {
      if (BlkCount > 1) {
        TransMode |= BIT2;
      }
    }
  }

  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_TRANS_MOD, FALSE, sizeof (TransMode), (VOID *) (UINTN)&TransMode);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Cmd = (UINT16)LShiftU64 (Packet->SdMmcCmdBlk->CommandIndex, 8);
  if (Packet->SdMmcCmdBlk->CommandType == SdMmcCommandTypeAdtc) {
    Cmd |= BIT5;
  }
  //
  // Convert ResponseType to value
  //
  if (Packet->SdMmcCmdBlk->CommandType != SdMmcCommandTypeBc) {
    switch (Packet->SdMmcCmdBlk->ResponseType) {
    case SdMmcResponseTypeR1:
    case SdMmcResponseTypeR5:
    case SdMmcResponseTypeR6:
    case SdMmcResponseTypeR7:
      Cmd |= (BIT1 | BIT3 | BIT4);
      break;
    case SdMmcResponseTypeR2:
      Cmd |= (BIT0 | BIT3);
      break;
    case SdMmcResponseTypeR3:
    case SdMmcResponseTypeR4:
      Cmd |= BIT1;
      break;
    case SdMmcResponseTypeR1b:
    case SdMmcResponseTypeR5b:
      Cmd |= (BIT0 | BIT1 | BIT3 | BIT4);
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  //
  // Execute cmd
  //
  Status = SdMmcHcRwMmio (Address, SD_MMC_HC_COMMAND, FALSE, sizeof (Cmd), (VOID *) (UINTN)&Cmd);
  return Status;
}

/**
  Check the TRB execution result.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The TRB is executed successfully.
  @retval EFI_NOT_READY     The TRB is not completed for execution.
  @retval Others            Some erros happen when executing this request.

**/
EFI_STATUS
SdMmcCheckTrbResult (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  )
{
  EFI_STATUS                          Status;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet;
  UINT16                              IntStatus;
  UINT32                              Response[4];
  UINT32                              SdmaAddr;
  UINT8                               Index;
  UINT8                               SwReset;
  UINT32                              PioLength;

  SwReset = 0;
  Packet  = Trb->Packet;
  //
  // Check Trb execution result by reading Normal Interrupt Status register.
  //
  Status = SdMmcHcRwMmio (
             Private->SdMmcHcBase,
             SD_MMC_HC_NOR_INT_STS,
             TRUE,
             sizeof (IntStatus),
             &IntStatus
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  //
  // Check Transfer Complete bit is set or not.
  //
  if ((IntStatus & BIT1) == BIT1) {
    if ((IntStatus & BIT15) == BIT15) {
      //
      // Read Error Interrupt Status register to check if the error is
      // Data Timeout Error.
      // If yes, treat it as success as Transfer Complete has higher
      // priority than Data Timeout Error.
      //
      Status = SdMmcHcRwMmio (
                 Private->SdMmcHcBase,
                 SD_MMC_HC_ERR_INT_STS,
                 TRUE,
                 sizeof (IntStatus),
                 &IntStatus
                 );
      if (!EFI_ERROR (Status)) {
        if ((IntStatus & BIT4) == BIT4) {
          Status = EFI_SUCCESS;
        } else if ((IntStatus & BIT5) == BIT5) {
          Status = EFI_CRC_ERROR;
        } else {
          Status = EFI_DEVICE_ERROR;
        }
      }
    }

    goto Done;
  }
  //
  // Check if there is a error happened during cmd execution.
  // If yes, then do error recovery procedure to follow SD Host Controller
  // Simplified Spec 3.0 section 3.10.1.
  //
  if ((IntStatus & BIT15) == BIT15) {
    Status = SdMmcHcRwMmio (
               Private->SdMmcHcBase,
               SD_MMC_HC_ERR_INT_STS,
               TRUE,
               sizeof (IntStatus),
               &IntStatus
               );
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    if ((IntStatus & 0x0F) != 0) {
      SwReset |= BIT1;
    }
    if ((IntStatus & 0xF0) != 0) {
      SwReset |= BIT2;
    }

    Status  = SdMmcHcRwMmio (
                Private->SdMmcHcBase,
                SD_MMC_HC_SW_RST,
                FALSE,
                sizeof (SwReset),
                &SwReset
                );
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    Status = SdMmcHcWaitMmioSet (
               Private->SdMmcHcBase,
               SD_MMC_HC_SW_RST,
               sizeof (SwReset),
               0xFF,
               0,
               SD_MMC_HC_GENERIC_TIMEOUT
               );
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if ((IntStatus & BIT5) == BIT5) {
      Status = EFI_CRC_ERROR;
    } else {
      Status = EFI_DEVICE_ERROR;
    }
    goto Done;
  }
  //
  // Check if DMA interrupt is signalled for the SDMA transfer.
  //
  if ((Trb->Mode == SdMmcSdmaMode) && ((IntStatus & BIT3) == BIT3)) {
    //
    // Clear DMA interrupt bit.
    //
    IntStatus = BIT3;
    Status    = SdMmcHcRwMmio (
                  Private->SdMmcHcBase,
                  SD_MMC_HC_NOR_INT_STS,
                  FALSE,
                  sizeof (IntStatus),
                  &IntStatus
                  );
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    //
    // Update SDMA Address register.
    //
    SdmaAddr = SD_MMC_SDMA_ROUND_UP ((UINT32) (UINTN)Trb->DataPhy, SD_MMC_SDMA_BOUNDARY);
    Status   = SdMmcHcRwMmio (
                 Private->SdMmcHcBase,
                 SD_MMC_HC_SDMA_ADDR,
                 FALSE,
                 sizeof (UINT32),
                 &SdmaAddr
                 );
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    Trb->DataPhy = (EFI_PHYSICAL_ADDRESS) (UINTN)SdmaAddr;
  }

  if ((Packet->SdMmcCmdBlk->CommandType != SdMmcCommandTypeAdtc) &&
      (Packet->SdMmcCmdBlk->ResponseType != SdMmcResponseTypeR1b) &&
      (Packet->SdMmcCmdBlk->ResponseType != SdMmcResponseTypeR5b)) {
    if ((IntStatus & BIT0) == BIT0) {
      Status = EFI_SUCCESS;
      goto Done;
    }
  }

  if (((Private->Slot.CardType == EmmcCardType) &&
       (Packet->SdMmcCmdBlk->CommandIndex == EMMC_SEND_TUNING_BLOCK)) ||
      ((Private->Slot.CardType == SdCardType) &&
       (Packet->SdMmcCmdBlk->CommandIndex == SD_SEND_TUNING_BLOCK))) {
    //
    // When performing tuning procedure (Execute Tuning is set to 1) through PIO mode,
    // wait Buffer Read Ready bit of Normal Interrupt Status Register to be 1.
    // Refer to SD Host Controller Simplified Specification 3.0 figure 2-29 for details.
    //
    if ((IntStatus & BIT5) == BIT5) {
      //
      // Clear Buffer Read Ready interrupt at first.
      //
      IntStatus = BIT5;
      SdMmcHcRwMmio (Private->SdMmcHcBase, SD_MMC_HC_NOR_INT_STS, FALSE, sizeof (IntStatus), &IntStatus);
      //
      // Read data out from Buffer Port register
      //
      for (PioLength = 0; PioLength < Trb->DataLen; PioLength += 4) {
        SdMmcHcRwMmio (Private->SdMmcHcBase, SD_MMC_HC_BUF_DAT_PORT, TRUE, 4, (UINT8 *)Trb->Data + PioLength);
      }
      Status = EFI_SUCCESS;
      goto Done;
    }
  }

  Status = EFI_NOT_READY;
Done:
  //
  // Get response data when the cmd is executed successfully.
  //
  if (!EFI_ERROR (Status)) {
    if (Packet->SdMmcCmdBlk->CommandType != SdMmcCommandTypeBc) {
      for (Index = 0; Index < 4; Index++) {
        Status = SdMmcHcRwMmio (
                   Private->SdMmcHcBase,
                   SD_MMC_HC_RESPONSE + Index * 4,
                   TRUE,
                   sizeof (UINT32),
                   &Response[Index]
                   );
        if (EFI_ERROR (Status)) {
          SdMmcHcLedOnOff (Private->SdMmcHcBase, FALSE);
          return Status;
        }
      }
      CopyMem (Packet->SdMmcStatusBlk, Response, sizeof (Response));
    }
  }

  if (Status != EFI_NOT_READY) {
    SdMmcHcLedOnOff (Private->SdMmcHcBase, FALSE);
  }

  return Status;
}

/**
  Wait for the TRB execution result.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The TRB is executed successfully.
  @retval Others            Some erros happen when executing this request.

**/
EFI_STATUS
SdMmcWaitTrbResult (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  )
{
  EFI_STATUS                          Status;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet;
  UINT64                              Timeout;
  BOOLEAN                             InfiniteWait;

  Packet = Trb->Packet;
  //
  // Wait Command Complete Interrupt Status bit in Normal Interrupt Status Register
  //
  Timeout = Packet->Timeout;
  if (Timeout == 0) {
    InfiniteWait = TRUE;
  } else {
    InfiniteWait = FALSE;
  }

  while (InfiniteWait || (Timeout > 0)) {
    //
    // Check Trb execution result by reading Normal Interrupt Status register.
    //
    Status = SdMmcCheckTrbResult (Private, Trb);
    if (Status != EFI_NOT_READY) {
      return Status;
    }
    //
    // Stall for 1 microsecond.
    //
    MicroSecondDelay (1);

    Timeout--;
  }

  return EFI_TIMEOUT;
}

/**
  Sends SD command to an SD card that is attached to the SD controller.

  The PassThru() function sends the SD command specified by Packet to the SD card
  specified by Slot.

  If Packet is successfully sent to the SD card, then EFI_SUCCESS is returned.

  If a device error occurs while sending the Packet, then EFI_DEVICE_ERROR is returned.

  If Slot is not in a valid range for the SD controller, then EFI_INVALID_PARAMETER
  is returned.

  If Packet defines a data command but both InDataBuffer and OutDataBuffer are NULL,
  EFI_INVALID_PARAMETER is returned.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in,out] Packet         A pointer to the SD command data structure.

  @retval EFI_SUCCESS           The SD Command Packet was sent by the host.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to send the SD
                                command Packet.
  @retval EFI_INVALID_PARAMETER Packet, Slot, or the contents of the Packet is invalid.
  @retval EFI_INVALID_PARAMETER Packet defines a data command but both InDataBuffer and
                                OutDataBuffer are NULL.
  @retval EFI_NO_MEDIA          SD Device not present in the Slot.
  @retval EFI_UNSUPPORTED       The command described by the SD Command Packet is not
                                supported by the host controller.
  @retval EFI_BAD_BUFFER_SIZE   The InTransferLength or OutTransferLength exceeds the
                                limit supported by SD card ( i.e. if the number of bytes
                                exceed the Last LBA).

**/
EFI_STATUS
EFIAPI
SdMmcSendCommand (
  IN     SD_MMC_HC_PRIVATE_DATA                *Private,
  IN OUT EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   *Packet
  )
{
  EFI_STATUS                      Status;
  SD_MMC_HC_TRB                   *Trb;

  if ((Private == 0x0) || (Packet == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Packet->SdMmcCmdBlk == NULL) || (Packet->SdMmcStatusBlk == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Packet->OutDataBuffer == NULL) && (Packet->OutTransferLength != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Packet->InDataBuffer == NULL) && (Packet->InTransferLength != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!Private->Slot.MediaPresent) {
    return EFI_NO_MEDIA;
  }

  Trb = SdMmcCreateTrb (Private, Packet);
  if (Trb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = SdMmcWaitTrbEnv (Private, Trb);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = SdMmcExecTrb (Private, Trb);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = SdMmcWaitTrbResult (Private, Trb);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

Done:
  SdMmcFreeTrb (Trb);

  return Status;
}
