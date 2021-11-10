/** @file
  Register names for PCU device.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_PCH_<generation_name>_" in register/bit names. e.g., "_PCH_CHV_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_PCH_" without <generation_name> inserted.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _REGS_PCU_H_
#define _REGS_PCU_H_

///
/// Processor Interface Registers
///
#define R_NMI_SC                      0x61  ///< NMI Status and Control
#define B_NMI_SC_SERR_NMI_STS         BIT7  ///< SERR# NMI Status
#define B_NMI_SC_IOCHK_NMI_STS        BIT6  ///< IOCHK NMI Status
#define B_NMI_SC_TMR2_OUT_STS         BIT5  ///< Timer Counter 2 Status
#define B_NMI_SC_REF_TOGGLE           BIT4  ///< Refresh Cycle toggle Status
#define B_NMI_SC_IOCHK_NMI_EN         BIT3  ///< IOCHK NMI Enable
#define B_NMI_SC_PCI_SERR_EN          BIT2  ///< SERR# NMI Enable
#define B_NMI_SC_SPKR_DAT_EN          BIT1  ///< Speaker Data Enable
#define B_NMI_SC_TIM_CNT2_EN          BIT0  ///< Timer Counter 2 Enable

///
/// RTC Registers
///
#define R_RTC_INDEX                   0x70  ///< NMI Enable and Real Time Clock Index, Co-function with R_NMI_EN
#define R_RTC_TARGET                  0x71  ///< Real-Time Clock Target Register
#define R_RTC_EXT_INDEX               0x72  ///< Extended RAM Index Register
#define R_RTC_EXT_TARGET              0x73  ///< Extended RAM Target Register
#define R_RTC_INDEX2                  0x74  ///< Real-Time Clock Index Register
#define R_RTC_TARGET2                 0x75  ///< Real-Time Clock Target Register
#define R_RTC_EXT_INDEX2              0x76  ///< Extended RAM Index Register
#define R_RTC_EXT_TARGET2             0x77  ///< Extended RAM Target Register

#define R_RTC_SECONDS                 0x00  ///< Seconds, Range 0..59
#define R_RTC_SECONDSALARM            0x01  ///< Seconds Alarm, Range 0..59
#define R_RTC_MINUTES                 0x02  ///< Minutes, Range 0..59
#define R_RTC_MINUTESALARM            0x03  ///< Minutes Alarm, Range 0..59
#define R_RTC_HOURS                   0x04  ///< Hours, Range 1..12 or 0..23 Bit 7 is AM/PM
#define R_RTC_HOURSALARM              0x05  ///< Hours Alarm, Range 1..12 or 0..23 Bit 7 is AM/PM
#define R_RTC_DAYOFWEEK               0x06  ///< Day of Week, Range 1..7
#define R_RTC_DAYOFMONTH              0x07  ///< Day of Month, Range 1..31
#define R_RTC_MONTH                   0x08  ///< Month, Range 1..12
#define R_RTC_YEAR                    0x09  ///< Year, Range 0..99

#define R_RTC_REGISTERA               0x0A  ///< RTC Register A
#define B_RTC_REGISTERA_UIP           BIT7  ///< Update In Progress
#define B_RTC_REGISTERA_DV            (BIT6 | BIT5 | BIT4) ///< Division Chain Select
#define V_RTC_REGISTERA_DV_NORM_OP    0x20  ///< Normal Operation
#define V_RTC_REGISTERA_DV_BYP_5      0x30  ///< Bypass 5 Stages (Test mode only)
#define V_RTC_REGISTERA_DV_BYP_10     0x40  ///< Bypass 10 Stages (Test mode only)
#define V_RTC_REGISTERA_DV_BYP_15     0x50  ///< Bypass 15 Stages (Test mode only)
#define V_RTC_REGISTERA_DV_DIV_RST1   0x60  ///< Divider Reset
#define V_RTC_REGISTERA_DV_DIV_RST2   0x70  ///< Divider Reset
#define B_RTC_REGISTERA_RS            (BIT3 | BIT2 | BIT1 | BIT0) ///< Rate Select
#define V_RTC_REGISTERA_RS_INT_NV_TGL 0x00  ///< Interrupt Never Toggles
#define V_RTC_REGISTERA_RS_3P906MS1   0x01  ///< 3.90625 ms
#define V_RTC_REGISTERA_RS_7P812MS1   0x02  ///< 7.8125 ms
#define V_RTC_REGISTERA_RS_122P0US    0x03  ///< 122.070 us
#define V_RTC_REGISTERA_RS_244P1US    0x04  ///< 244.141 us
#define V_RTC_REGISTERA_RS_488P2US    0x05  ///< 488.281 us
#define V_RTC_REGISTERA_RS_976P5US    0x06  ///< 976.5625 us
#define V_RTC_REGISTERA_RS_1P953MS    0x07  ///< 1.953125 ms
#define V_RTC_REGISTERA_RS_3P906MS    0x08  ///< 3.90625 ms
#define V_RTC_REGISTERA_RS_7P812MS    0x09  ///< 7.8125 ms
#define V_RTC_REGISTERA_RS_15P62MS    0x0A  ///< 15.625 ms
#define V_RTC_REGISTERA_RS_31P25MS    0x0B  ///< 31.25 ms
#define V_RTC_REGISTERA_RS_62P5MS     0x0C  ///< 62.5 ms
#define V_RTC_REGISTERA_RS_125MS      0x0D  ///< 125 ms
#define V_RTC_REGISTERA_RS_250MS      0x0E  ///< 250 ms
#define V_RTC_REGISTERA_RS_500MS      0x0F  ///< 500 ms

#define R_RTC_REGISTERB               0x0B  ///< RTC Register B
#define B_RTC_REGISTERB_SET           BIT7  ///< Update Cycle Inhibit 1: Stop auto update, begin set value; 0: Update cycle occurs
#define B_RTC_REGISTERB_PIE           BIT6  ///< Periodic Interrupt Enable
#define B_RTC_REGISTERB_AIE           BIT5  ///< Alarm Interrupt Enable
#define B_RTC_REGISTERB_UIE           BIT4  ///< Update-ended Interrupt Enable
#define B_RTC_REGISTERB_SQWE          BIT3  ///< Square Wave Enable (Not implemented)
#define B_RTC_REGISTERB_DM            BIT2  ///< Data Mode 1: Binary; 0:BCD
#define B_RTC_REGISTERB_HF            BIT1  ///< Hour Format 1: 24 mode; 0: 12 mode.
#define B_RTC_REGISTERB_DSE           BIT0  ///< Daylight Savings Enable (Not Implemented)

#define R_RTC_REGISTERC               0x0C  ///< RTC Register C
#define B_RTC_REGISTERC_IRQF          BIT7  ///< Interrupt Request Flag
#define B_RTC_REGISTERC_PF            BIT6  ///< Periodic Interrupt Flag
#define B_RTC_REGISTERC_AF            BIT5  ///< Alarm Flag
#define B_RTC_REGISTERC_UF            BIT4  ///< Update-ended Flag
#define B_RTC_REGISTERC_RESERVED      (BIT3 | BIT2 | BIT1 | BIT0)

#define R_RTC_REGISTERD               0x0D  ///< RTC Register D
#define B_RTC_REGISTERD_VRT           BIT7  ///< Valid RAM and Time Bit
#define B_RTC_REGISTERD_RESERVED      BIT6
#define B_RTC_REGISTERD_DA            0x3F  ///< Date Alarm

#define B_RTC_CENTURY                 0x32  ///< Century Data

///
/// APM Registers
///
#define R_APM_CNT                     0xB2  ///< Advanced Power Management Control Port
#define R_APM_STS                     0xB3  ///< Advanced Power Management Status Port

///
/// Reset Control Register
///
#define R_RST_CNT                     0xCF9 ///< Reset Control
#define B_RST_CNT_FULL_RST            BIT3
#define B_RST_CNT_RST_CPU             BIT2
#define B_RST_CNT_SYS_RST             BIT1
#define V_RST_CNT_FULLRESET           0x0E
#define V_RST_CNT_HARDRESET           0x06
#define V_RST_CNT_SOFTRESET           0x04  ///< Not supported by CHV
#define V_RST_CNT_HARDSTARTSTATE      0x02
#define V_RST_CNT_SOFTSTARTSTATE      0x00

///
/// IO APIC Registers
///
#define R_IO_APIC_INDEX               IO_APIC_INDEX_REGISTER ///< IOAPIC Index Register, 8bit
#define R_IO_APIC_WINDOW              IO_APIC_DATA_REGISTER ///< IOAPIC Window Register, 32bit
#define R_IO_APIC_EOI                 IO_APIC_EOI ///< IOAPIC EOI Register, 8bit

#define R_IO_APIC_ID                  0x00  ///< Identification
#define B_IO_APIC_ID_AID              (BIT27 | BIT26 | BIT25 | BIT24) ///< APIC Identification

#define R_IO_APIC_VS                  0x01  ///< Version
#define B_IO_APIC_VS_MRE              0xFF0000 ///< Maximum Redirection Entries
#define B_IO_APIC_VS_PRQ              BIT15 ///< Pin Assertion Register Supported
#define B_IO_APIC_VS_VS               0xFF  ///< Version

///
/// HPET Registers
///
#define R_HPET                        HPET_BASE_ADDRESS ///< HPET Base Address
#define R_HPET_GCID                   0x00  ///< HPET General Capabilities and ID, 64bit
#define B_HPET_GCID_CTP               0xFFFFFFFF00000000 ///< Counter Tick Period
#define B_HPET_GCID_VID               0xFFFF0000 ///< Vendor ID
#define B_HPET_GCID_LRC               BIT15 ///< Legacy Rout Capable
#define B_HPET_GCID_CS                BIT13 ///< Counter Size
#define B_HPET_GCID_NT                0x1F00 ///< Number of Timers
#define B_HPET_GCID_RID               0xFF  ///< Revision ID
#define N_HPET_ADDR_ASEL              12
#define R_HPET_GCFG                   0x10  ///< HPET General Configuration
#define B_HPET_GCFG_LRE               BIT1  ///< Legacy Rout Enable
#define B_HPET_GCFG_EN                BIT0  ///< Overall Enable
#define R_HPET_GIS                    0x20  ///< HPET General Interrupt Status
#define B_HPET_GIS_T2                 BIT2  ///< Timer 2 Status
#define B_HPET_GIS_T1                 BIT1  ///< Timer 1 Status
#define B_HPET_GIS_T0                 BIT0  ///< Timer 0 Status
#define R_HPET_MCV                    0xF0  ///< HPET Main Counter Value, 64bit
#define R_HPET_T0C                    0x100 ///< HPET Timer 0 Config and Capabilities
#define R_HPET_T0CV_L                 0x108 ///< HPET Timer 0 Lower Comparator Value
#define R_HPET_T0CV_H                 0x10C ///< HPET Timer 0 Upper Comparator Value
#define R_HPET_T1C                    0x120 ///< HPET Timer 1 Config and Capabilities
#define R_HPET_T1CV                   0x128 ///< HPET Timer 1 Comparator Value
#define R_HPET_T2C                    0x140 ///< HPET Timer 2 Config and Capabilities
#define R_HPET_T2CV                   0x148 ///< HPET Timer 2 Comparator Value
#define B_HPET_TXC_IRC                0xFFFFFFFF00000000 ///< Interrupt Rout Capability
#define B_HPET_TXC_FID                BIT15 ///< FSB Interrupt Delivery
#define B_HPET_TXC_FE                 BIT14 ///< FSB Enable
#define B_HPET_TXC_IR                 0x3E00 ///< Interrupt Rout
#define B_HPET_TXC_T32M               BIT8  ///< Timer 32-bit Mode
#define B_HPET_TXC_TVS                BIT6  ///< Timer Value Set
#define B_HPET_TXC_TS                 BIT5  ///< Timer Size
#define B_HPET_TXC_PIC                BIT4  ///< Periodic Interrupt Capable
#define B_HPET_TXC_TYP                BIT3  ///< Timer Type
#define B_HPET_TXC_IE                 BIT2  ///< Interrupt Enable
#define B_HPET_TXC_IT                 BIT1  ///< Timer Interrupt Type


#define R_P2SB_STSCMD                         0x04  ///< Status & Command Register
#define B_P2SB_STSCMD_BME                     BIT2  ///< Bus Master Enable
#define B_P2SB_STSCMD_MSE                     BIT1  ///< Memory Space Enable

#endif
