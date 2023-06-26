/**@file
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

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _REGS_RTC_H_
#define _REGS_RTC_H_


#define R_RTC_IO_INDEX                 0x00000070U      ///< RTC Index Register
                                                        /* This 8-bit register selects which indirect
                                                         * register appears in the target register
                                                         * to be manipulated by software. Software
                                                         * will program this register to select the
                                                         * desired RTC indexed register.
                                                        */

#define R_RTC_IO_TARGET                0x00000071U      ///< RTC Target Register
                                                        /* This 8-bit register specifies the data
                                                         * to be read or written to the register pointed
                                                         * to by the INDEX register.
                                                        */

#define R_RTC_IO_INDEX_ALT             0x00000074U      ///< RTC Alternative Index Register

#define R_RTC_IO_TARGET_ALT            0x00000075U      ///< RTC Alternative Target Register

#define R_RTC_IO_EXT_INDEX_ALT         0x00000076U      ///< RTC External Alternative Index Register

#define R_RTC_IO_EXT_TARGET_ALT        0x00000077U      ///< RTC External Alternative Target Register


                                                        /* When set, the upper 128 byte bank
                                                         * of RTC RAM can be accessed.
                                                        */

                                                        /* When set, bytes 38h-3Fh in the lower
                                                         * 128 byte bank of RTC RAM are locked
                                                         * and cannot be accessed. Writes will
                                                         * be dropped and reads will not return
                                                         * any guaranteed data. Bit reset on
                                                         * system reset.
                                                        */

                                                        /* When set, bytes 38h-3Fh in the upper
                                                         * 128 byte bank of RTC RAM are locked
                                                         * and cannot be accessed. Writes will
                                                         * be dropped and reads will not return
                                                         * any guaranteed data. Bit reset on
                                                         * system reset.
                                                        */

                                                        /* When set to 1 the internal VRM that
                                                         * generates the rtc well supply voltage
                                                         * in SUS mode is disabled when SLP_S0#
                                                         * is asserted to '0'. When 0, HW control
                                                         * of the RTC internal VRM is disabled.
                                                        */

                                                        /* When set, prevents RTC version of
                                                         * TS (BUC.TS) from being changed.
                                                        */

#define R_RTC_PCR_BUC                 0x00003414U       ///< Backed Up Control
                                                        /* All bits in this register are in the RTC
                                                         * well and only cleared by RTEST.
                                                        */

#define B_RTC_PCR_BUC_TS                     BIT0       ///< Top Swap
                                                        /* This should be set by BIOS when
                                                         * the corresponding TS bit in the
                                                         * LPC or eSPI controller is set in
                                                         * order to properly restore the state
                                                         * of that field after reset since
                                                         * they are not preserved in an RTC
                                                         * well bit in those devices.
                                                        */

#define B_RTC_PCR_BUC_DSO                    BIT4       ///< Daylight Savings Override
                                                        /* When this bit is a '1', the DSE
                                                         * bit in the RTC Register B bit(0)
                                                         * is a RW bit but has no effect where
                                                         * daylight savings is hard-disabled
                                                         * internally. When this bit is a '0',
                                                         * the DSE bit in the RTC register
                                                         * B bit(0) is a RW bit that is configurable
                                                         * by software to enable the daylight
                                                         * savings.
                                                        */

#define R_RTC_PCR_RTCDCG              0x00003418U       ///< RTC Dynamic Clock Gating Control
                                                        /* All bits in this register are in the Primary
                                                         * Well and cleared by host_side_rst_b.
                                                        */

                                                        /* 0 - Disable dynamic clock gate on
                                                         * rosc_side_clk (Default) 1 - Enable
                                                         * dynamic clock gate on rosc_side_clk
                                                        */

                                                        /* 0 - Disable dynamic clock gate on
                                                         * ipci_clk (Default) 1 - Enable dynamic
                                                         * clock gate on ipci_clk
                                                        */

                                                        /* 0 - Disable dynamic clock gate on
                                                         * pgcb_clk (Default) 1 - Enable dynamic
                                                         * clock gate on pgcb_clk
                                                        */

                                                        /* 0 - RTC port 70h returns value 0
                                                         * 1 - The regular Io Read to POrt 70h always returns
                                                          the shadowed NMI_EN# in RTC
                                                        */

                                                        /* This register exists in the RTC well. The
                                                         * description of this register should not
                                                         * be put in EDS
                                                        */

#endif
