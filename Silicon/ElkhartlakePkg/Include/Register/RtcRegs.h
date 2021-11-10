/** @file
  Register names for RTC device

Conventions:

  - Register definition format:
    Prefix_[GenerationName]_[ComponentName]_SubsystemName_RegisterSpace_RegisterName
  - Prefix:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register size
    Definitions beginning with "N_" are the bit position
  - [GenerationName]:
    Three letter acronym of the generation is used (e.g. SKL,KBL,CNL etc.).
    Register name without GenerationName applies to all generations.
  - [ComponentName]:
    This field indicates the component name that the register belongs to (e.g. PCH, SA etc.)
    Register name without ComponentName applies to all components.
    Register that is specific to -H denoted by "_PCH_H_" in component name.
    Register that is specific to -LP denoted by "_PCH_LP_" in component name.
  - SubsystemName:
    This field indicates the subsystem name of the component that the register belongs to
    (e.g. PCIE, USB, SATA, GPIO, PMC etc.).
  - RegisterSpace:
    MEM - MMIO space register of subsystem.
    IO  - IO space register of subsystem.
    PCR - Private configuration register of subsystem.
    CFG - PCI configuration space register of subsystem.
  - RegisterName:
    Full register name.

  Copyright (c) 2018 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _REGS_RTC_H_
#define _REGS_RTC_H_

#define R_RTC_IO_INDEX                           0x70
#define R_RTC_IO_TARGET                          0x71
#define R_RTC_IO_REGA                            0x0A
#define R_RTC_IO_REGB                            0x0B
#define R_RTC_IO_REGD                            0x0D

//
// Private Configuration Register
// RTC PCRs (PID:RTC)
//
#define R_RTC_PCR_BUC                         0x3414               ///< Backed Up Control

#define RTC_SECONDS               (0x00)
#define RTC_MINUTES               (0x02)
#define RTC_HOURS                 (0x04)
#define RTC_DAY_OF_MONTH          (0x07)
#define RTC_MONTH                 (0x08)
#define RTC_YEAR                  (0x09)

#define B_RTC_CENTURY                 0x32  ///< Century Data
#define R_RTC_SECONDSALARM            0x01  ///< Seconds Alarm, Range 0..59
#define R_RTC_MINUTESALARM            0x03  ///< Minutes Alarm, Range 0..59
#define R_RTC_HOURSALARM              0x05  ///< Hours Alarm, Range 1..12 or 0..23 Bit 7 is AM/PM

#endif
