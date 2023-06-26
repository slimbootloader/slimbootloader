/** @file
  Register names for PCH P2SB device

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

  Copyright (c) 2013 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _P2SB_REGS_H_
#define _P2SB_REGS_H_

//
// PCI to P2SB Bridge Registers
//
                                                               /* PCI header registers
                                                                */
                                                               /* Lower DWORD of the base address
                                                                * for the sideband register access
                                                                * BAR.
                                                                */
                                                               /* HPET configuration register
                                                                */
                                                               /* When set, the P2SB will decode the
                                                                * High Performance Timer memory address
                                                                * range selected by bits 1:0 below.
                                                                */
                                                               /* This 2-bit field selects 1 of 4
                                                                * possible memory address ranges for
                                                                * the High Performance Timer functionality.
                                                                * The encodings are:
                                                                * 00 : FED0_0000h - FED0_03FFFh
                                                                * 01 : FED0_1000h - FED0_13FFFh
                                                                * 10 : FED0_2000h - FED0_23FFFh
                                                                * 11 : FED0_3000h - FED0_33FFFh
                                                                */
                                                               /* IOAPIC configuration register
                                                                */
                                                               /* When set, the P2SB will decode the
                                                                * IOxAPIC memory address range selected
                                                                * by bits 7:0 below.
                                                                */
                                                               /* These bits define address bits 19:12
                                                                * for the IOxAPIC range. The default
                                                                * value of 00h enables compatibility
                                                                * with prior products as an initial
                                                                * value. This value must not be changed
                                                                * unless the IOxAPIC Enable bit is
                                                                * cleared.
                                                                */
#define R_IO_APIC_MEM_INDEX_OFFSET            0x00000000U
#define R_IO_APIC_MEM_DATA_OFFSET             0x00000010U
                                                               /* This register specifies the bus:device:function
                                                                * ID that the IOxAPIC will use in the following
                                                                * :
                                                                * tAs the Requester ID when initiating Interrupt
                                                                * Messages to the CPU
                                                                * tAs the Completer ID when responding to
                                                                * the reads targeting the IOxAPICs Memory-Mapped
                                                                * I/O registers
                                                                * This field is default to Bus 0: Device 31:
                                                                * Function 0 after reset. BIOS shall program
                                                                * this field accordingly if unique bus:device:function
                                                                * number is required for the internal IOxAPIC.
                                                                */
#define V_P2SB_CFG_IBDF_BUS                   0
#define V_P2SB_CFG_IBDF_DEV                   30
#define V_P2SB_CFG_IBDF_FUNC                  7
                                                               /* This register specifies the bus:device:function
                                                                * ID that the HPET device will use in the
                                                                * following :
                                                                * tAs the Requester ID when initiating Interrupt
                                                                * Messages to the CPU
                                                                * tAs the Completer ID when responding to
                                                                * the reads targeting the corresponding HPETs
                                                                * Memory-Mapped I/O registers
                                                                * This field is default to Bus 0: Device 31:
                                                                * Function 0 after reset. BIOS shall program
                                                                * this field accordingly if unique bus:device:function
                                                                * number is required for the corresponding
                                                                * HPET.
                                                                */
#define V_P2SB_CFG_HBDF_BUS                   0
#define V_P2SB_CFG_HBDF_DEV                   30
#define V_P2SB_CFG_HBDF_FUNC                  6
                                                               /* Select IEC (integrated EC) instead of LPC/eSPI
                                                                * if register bit LFIORIEC[x]=1. If register
                                                                * bit LFIORIEC[x]=0, LPC or eSPI is selected
                                                                * based on hardstrap eSpiEnable
                                                                */

//
// Definition for SBI
//
#define R_P2SB_CFG_SBIADDR_V2                    0x000000d0U      ///< SBI Address
                                                               /* Provides mechanism to send message on IOSFSB;The
                                                                * SAI check is only required on RS field but
                                                                * due to tool limitation, the SAI check is
                                                                * applied in RDL on whole register
                                                                */
#define R_P2SB_CFG_SBIDATA_V2                    0x000000d4U      ///< SBI Data
                                                               /* Provides mechanism to send message on IOSFSB
                                                                */
#define R_P2SB_CFG_SBISTAT_V2                    0x000000d8U      ///< SBI Status
                                                               /* Provides mechanism to send message on IOSFSB
                                                                */
#define B_P2SB_CFG_SBISTAT_OPCODE_V2             ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Opcode
                                                               /* This is the Opcode sent in the IOSF
                                                                * sideband message.
                                                                */
#define B_P2SB_CFG_SBISTAT_POSTED             BIT7             ///< Posted
                                                               /* When set to 1, the message will
                                                                * be send as a posted message instead
                                                                * of non-posted.  This should only
                                                                * be used if the receiver is known
                                                                * to support posted operations for
                                                                * the specified operation.
                                                                */
#define B_P2SB_CFG_SBISTAT_RESPONSE_V2           ( BIT2 | BIT1 )  ///< Response Status
                                                               /* 00 - Successful
                                                                * 01 - Unsuccessful / Not Supported
                                                                * 10 - Powered Down
                                                                * 11 - Multi-cast Mixed
                                                                * This register reflects the response
                                                                * status for the previously completed
                                                                * transaction.  The value of this
                                                                * register is only meaningful if SBISTAT.INITRDY
                                                                * is zero.
                                                                */
#define N_P2SB_CFG_SBISTAT_RESPONSE           1
#define B_P2SB_CFG_SBISTAT_INITRDY            BIT0             ///< Initiate/ Ready#
                                                               /* 0:  The IOSF sideband interface
                                                                * is ready for a new transaction
                                                                * 1:  The IOSF sideband interface
                                                                * is busy with the previous transaction.
                                                                * A write to set this register bit
                                                                * to 1 will trigger an IOSF sideband
                                                                * message on the private IOSF sideband
                                                                * interface. The message will be formed
                                                                * based on the values programmed in
                                                                * the Sideband Message Interface Register
                                                                * Access registers.
                                                                * Software needs to ensure that the
                                                                * interface is not busy (SBISTAT.INITRDY
                                                                * is clear) before writing to this
                                                                * register.
                                                                */
#define R_P2SB_CFG_SBIRID_V2                     0x000000daU      ///< SBI Routing Identification
                                                               /* Provides mechanism to send message on IOSFSB
                                                                */
#define R_P2SB_CFG_SBIEXTADDR_V2                 0x000000dcU      ///< SBI Extended Address
                                                               /* Provides mechanism to send message on IOSFSB
                                                                */

//
// Others
//
                                                               /* P2SB general configuration register
                                                                */
                                                               /* IOSFP data parity error handling(EP
                                                                * bit) enabling bit
                                                                */
                                                               /* IOSFP command parity error handling
                                                                * enabling bit
                                                                */
                                                               /* IOSFP data phase parity error handling
                                                                * enabling bit
                                                                */
                                                               /* Power Control Enable register
                                                                */
                                                               /* Primary CDC configuration register
                                                                */
                                                               /* Sideband CDC configuration register
                                                                */
                                                               /* This register is only reset by a loss of
                                                                * core power
                                                                */
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */

#endif      // _P2SB_REGS_H_
