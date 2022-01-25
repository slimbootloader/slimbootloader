/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <RegAccess.h>
#include <Register/PchRegsPmc.h>
#include <Library/BootloaderCommonLib.h>
#include <PlatformBase.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsLpc.h>
#include <Library/PchPcrLib.h>
#include <Library/PchInfoLib.h>
#include <Register/PchRegsPcr.h>

//
// Dallas DS12C887 Real Time Clock
//
#define RTC_ADDRESS_SECONDS           0   // R/W  Range 0..59
#define RTC_ADDRESS_SECONDS_ALARM     1   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES           2   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES_ALARM     3   // R/W  Range 0..59
#define RTC_ADDRESS_HOURS             4   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_HOURS_ALARM       5   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_DAY_OF_THE_WEEK   6   // R/W  Range 1..7
#define RTC_ADDRESS_DAY_OF_THE_MONTH  7   // R/W  Range 1..31
#define RTC_ADDRESS_MONTH             8   // R/W  Range 1..12
#define RTC_ADDRESS_YEAR              9   // R/W  Range 0..99
#define RTC_ADDRESS_REGISTER_A        10  // R/W[0..6]  R0[7]
#define RTC_ADDRESS_REGISTER_B        11  // R/W
#define RTC_ADDRESS_REGISTER_C        12  // RO
#define RTC_ADDRESS_REGISTER_D        13  // RO
#define RTC_ADDRESS_CENTURY           50  // R/W  Range 19..20 Bit 8 is R/W
//
// Date and time initial values.
// They are used if the RTC values are invalid during driver initialization
//
#define RTC_INIT_SECOND               0
#define RTC_INIT_MINUTE               0
#define RTC_INIT_HOUR                 0
#define RTC_INIT_DAY                  1
#define RTC_INIT_MONTH                1
#define RTC_INIT_YEAR                 2001
//
// Register initial values
//
#define RTC_INIT_REGISTER_A           0x26
#define RTC_INIT_REGISTER_B           0x02
#define RTC_INIT_REGISTER_D           0x0

#define R_IOPORT_CMOS_STANDARD_INDEX            0x70
#define R_IOPORT_CMOS_STANDARD_DATA             0x71
#define R_IOPORT_CMOS_IDX_DIAGNOSTIC_STATUS     0x0E

#define R_ITSS_PCR_GIC                          0x31FC  ///< General Interrupt Control
#define B_ITSS_PCR_GIC_AME                      BIT17   ///< Alternate Access Mode Enable

/**

  This function reads NMI_EN register using Alternate Mode. Using AME is not allowed after PEI.

**/
UINT8
ReadNmiEn (
  VOID
  )
{
  UINT8           Nmi;
  UINT32          Data32;


  Data32 = MmioRead32 (PCH_PCR_ADDRESS (PID_ITSS, R_ITSS_PCR_GIC));
  Data32 |= (UINT32) (B_ITSS_PCR_GIC_AME);;
  MmioWrite32 (PCH_PCR_ADDRESS (PID_ITSS, R_ITSS_PCR_GIC), Data32);

  Nmi     = (IoRead8 (R_PCH_IO_NMI_EN));

  Data32 = MmioRead32 (PCH_PCR_ADDRESS (PID_ITSS, R_ITSS_PCR_GIC));
  Data32 &= (UINT32) ~(B_ITSS_PCR_GIC_AME);
  MmioWrite32 (PCH_PCR_ADDRESS (PID_ITSS, R_ITSS_PCR_GIC), Data32);

  return Nmi;
}

/**

    Check to see if CMOS is bad or cleared

    @param None

    @retval TRUE  - CMOS is bad
    @retval FALSE - CMOS is good

**/
BOOLEAN
IsCMOSBad(
  VOID
  )
{
  UINT8           Nmi;

  //
  // Preserve NMI bit setting
  //
  Nmi = (ReadNmiEn () & B_PCH_IO_NMI_EN_NMI_EN);

  IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, R_IOPORT_CMOS_IDX_DIAGNOSTIC_STATUS | Nmi);
  if (IoRead8 (R_IOPORT_CMOS_STANDARD_DATA) & (BIT6 + BIT7)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  This function checks if RTC Power Failure occurred by
  reading RTC_PWR_FLR bit

  @retval RTC Power Failure state: TRUE  - Battery is always present.
                                   FALSE - CMOS is cleared.
**/
BOOLEAN
PmcIsRtcBatteryGood (
  VOID
  )
{
    return ((MmioRead8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B) & B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS) == 0);
}

/**
  Clear BIT6/BIT7 of CMOS register 0xE
**/
VOID
ClearCmos0E()
{
  UINT8           Value;
  UINT8           Nmi;

  //
  // Preserve NMI bit setting
  //
  Nmi = (ReadNmiEn () & B_PCH_IO_NMI_EN_NMI_EN);

  IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, R_IOPORT_CMOS_IDX_DIAGNOSTIC_STATUS | Nmi);
  Value = IoRead8 (R_IOPORT_CMOS_STANDARD_DATA);
  IoWrite8 (R_IOPORT_CMOS_STANDARD_DATA, Value & 0x3F);
}

/**
  Read RTC content through its registers.

  @param  Address  Address offset of RTC.

  @return The data of UINT8 type read from RTC.
**/
UINT8
RtcRead (
  IN  UINT8 Address
  )
{
  IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, (UINT8) (Address | (UINT8) (IoRead8 (R_IOPORT_CMOS_STANDARD_INDEX) & 0x80)));
  return IoRead8 (R_IOPORT_CMOS_STANDARD_DATA);
}

/**
  Write RTC through its registers.

  @param  Address  Address offset of RTC.
  @param  Data     The content you want to write into RTC.

**/
VOID
RtcWrite (
  IN  UINT8   Address,
  IN  UINT8   Data
  )
{
  IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, (UINT8) (Address | (UINT8) (IoRead8 (R_IOPORT_CMOS_STANDARD_INDEX) & 0x80)));
  IoWrite8 (R_IOPORT_CMOS_STANDARD_DATA, Data);
}

/**
  Initialize RTC.

  @retval    None.

**/
VOID
RtcInit (
  VOID
  )
{
  UINT8           DataRegisterB;
  UINT8           Century;
  EFI_TIME        RtcTime = {0};

  if (PmcIsRtcBatteryGood () == FALSE) {
    //
    // Initialize RTC Register
    //
    RtcWrite (RTC_ADDRESS_REGISTER_A, RTC_INIT_REGISTER_A);

    //
    // Read Register B
    //
    DataRegisterB = RtcRead (RTC_ADDRESS_REGISTER_B);

    //
    // Clear RTC flag register
    //
    RtcRead (RTC_ADDRESS_REGISTER_C);

    //
    // Clear RTC register D
    //
    RtcWrite (RTC_ADDRESS_REGISTER_D, RTC_INIT_REGISTER_D);

    //
    // Get the Time/Date/Daylight Savings values.
    //
    RtcTime.Second = RtcRead (RTC_ADDRESS_SECONDS);
    RtcTime.Minute = RtcRead (RTC_ADDRESS_MINUTES);
    RtcTime.Hour   = RtcRead (RTC_ADDRESS_HOURS);
    RtcTime.Day    = RtcRead (RTC_ADDRESS_DAY_OF_THE_MONTH);
    RtcTime.Month  = RtcRead (RTC_ADDRESS_MONTH);
    RtcTime.Year   = RtcRead (RTC_ADDRESS_YEAR);
    Century        = RtcRead (RTC_ADDRESS_CENTURY);

    //
    // Set RTC configuration after get original time
    // The value of bit AIE should be reserved.
    //
    RtcWrite (RTC_ADDRESS_REGISTER_B, (UINT8)(RTC_INIT_REGISTER_B | (DataRegisterB & BIT5)));

    if (RtcTime.Year < 1998 || RtcTime.Year > 2099 || RtcTime.Month < 1 || RtcTime.Month > 12 \
      || RtcTime.Hour > 23 || RtcTime.Minute > 59 || RtcTime.Second > 59 || RtcTime.Nanosecond > 999999999) {
      RtcTime.Second      = RTC_INIT_SECOND;
      RtcTime.Minute      = RTC_INIT_MINUTE;
      RtcTime.Hour        = RTC_INIT_HOUR;
      RtcTime.Day         = RTC_INIT_DAY;
      RtcTime.Month       = RTC_INIT_MONTH;
      RtcTime.Year        = 0x01;
      RtcTime.Nanosecond  = 0;
      RtcTime.Daylight    = 0;
      Century             = 0x20;

      //
      // Read Register B, and inhibit updates of the RTC
      //
      DataRegisterB  = RtcRead (RTC_ADDRESS_REGISTER_B);
      DataRegisterB |= BIT7;
      RtcWrite (RTC_ADDRESS_REGISTER_B, DataRegisterB);

      RtcWrite (RTC_ADDRESS_SECONDS,          RtcTime.Second);
      RtcWrite (RTC_ADDRESS_MINUTES,          RtcTime.Minute);
      RtcWrite (RTC_ADDRESS_HOURS,            RtcTime.Hour);
      RtcWrite (RTC_ADDRESS_DAY_OF_THE_MONTH, RtcTime.Day);
      RtcWrite (RTC_ADDRESS_MONTH,            RtcTime.Month);
      RtcWrite (RTC_ADDRESS_YEAR,             (UINT8) RtcTime.Year);
      RtcWrite (RTC_ADDRESS_CENTURY,          Century);

      DataRegisterB &= ~BIT7;
      RtcWrite (RTC_ADDRESS_REGISTER_B, DataRegisterB);
    }

    //
    // Clear Rtc Power Failure Status,
    //
    MmioWrite8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B, MmioRead8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B) & ~(B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS) );
  }

  //
  // Clear CMOS Diag Err ststus
  //
  if (IsCMOSBad ()) {
    ClearCmos0E ();
  }
}
