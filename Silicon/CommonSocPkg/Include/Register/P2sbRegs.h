/** @file
  Register names for PCH P2SB device

Conventions:

  - Register definition format:
    Prefix_SubsystemName_RegisterSpace_RegisterName
  - Prefix:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "N_" are the bit position
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

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _P2SB_REGS_H_
#define _P2SB_REGS_H_

//
// PCI to P2SB Bridge Registers
//
#define R_P2SB_CFG_SBREG_BAR                  0x00000010U      ///< Sideband Register Access BAR
                                                               /* PCI header registers
                                                                */
#define B_P2SB_CFG_SBREG_BAR_RBA              ( BIT31 | BIT30 | BIT29 | BIT28 ) ///< Register Base Address
                                                               /* Lower DWORD of the base address
                                                                * for the sideband register access
                                                                * BAR.
                                                                */
#define R_P2SB_CFG_SBREG_BARH                 0x00000014U      ///< Sideband Register BAR High DWORD
                                                               /* PCI header registers
                                                                */
#define R_P2SB_CFG_HPTC                       0x00000060U      ///< High Performance Event Timer Configuration
                                                               /* HPET configuration register
                                                                */
#define B_P2SB_CFG_HPTC_AS                    ( BIT1 | BIT0 )  ///< Address Select
                                                               /* This 2-bit field selects 1 of 4
                                                                * possible memory address ranges for
                                                                * the High Performance Timer functionality.
                                                                * The encodings are:
                                                                * 00 : FED0_0000h - FED0_03FFFh
                                                                * 01 : FED0_1000h - FED0_13FFFh
                                                                * 10 : FED0_2000h - FED0_23FFFh
                                                                * 11 : FED0_3000h - FED0_33FFFh
                                                                */
#define B_P2SB_CFG_HPTC_AE                    BIT7             ///< Address Enable
                                                               /* When set, the P2SB will decode the
                                                                * High Performance Timer memory address
                                                                * range selected by bits 1:0 below.
                                                                */
#define N_HPET_ADDR_ASEL                      12
#define R_P2SB_CFG_IOAC                       0x00000064U      ///< IOxAPIC Configuration
                                                               /* IOAPIC configuration register
                                                                */
#define B_P2SB_CFG_IOAC_ASEL                  ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< APIC Range Select
                                                               /* These bits define address bits 19:12
                                                                * for the IOxAPIC range. The default
                                                                * value of 00h enables compatibility
                                                                * with prior products as an initial
                                                                * value. This value must not be changed
                                                                * unless the IOxAPIC Enable bit is
                                                                * cleared.
                                                                */
#define B_P2SB_CFG_IOAC_AE                    BIT8             ///< Address Enable
                                                               /* When set, the P2SB will decode the
                                                                * IOxAPIC memory address range selected
                                                                * by bits 7:0 below.
                                                                */
#define N_IO_APIC_ASEL                        12
#define R_IO_APIC_MEM_INDEX_OFFSET            0x00000000U
#define R_IO_APIC_MEM_DATA_OFFSET             0x00000010U
#define R_P2SB_CFG_IBDF                       0x0000006cU      ///< IOxAPIC Bus:Device:Function
                                                               /* This register specifies the bus:device:function
                                                                * ID that the IOxAPIC will use in the following
                                                                * :
                                                                * tAs the Requester ID when initiating Interrupt
                                                                * Messages to the CPU
                                                                * tAs the Completer ID when responding to
                                                                * the reads targeting the IOxAPICs Memory-Mapped
                                                                * I/O registers
                                                                * This field is default to Bus 0: Device 31:
                                                                * Function 0 after reset. SBL shall program
                                                                * this field accordingly if unique bus:device:function
                                                                * number is required for the internal IOxAPIC.
                                                                */
#define V_P2SB_CFG_IBDF_BUS                   0
#define V_P2SB_CFG_IBDF_DEV                   30
#define V_P2SB_CFG_IBDF_FUNC                  7
#define R_P2SB_CFG_HBDF                       0x00000070U      ///< HPET Bus:Device:Function
                                                               /* This register specifies the bus:device:function
                                                                * ID that the HPET device will use in the
                                                                * following :
                                                                * tAs the Requester ID when initiating Interrupt
                                                                * Messages to the CPU
                                                                * tAs the Completer ID when responding to
                                                                * the reads targeting the corresponding HPETs
                                                                * Memory-Mapped I/O registers
                                                                * This field is default to Bus 0: Device 31:
                                                                * Function 0 after reset. SBL shall program
                                                                * this field accordingly if unique bus:device:function
                                                                * number is required for the corresponding
                                                                * HPET.
                                                                */
#define V_P2SB_CFG_HBDF_BUS                   0
#define V_P2SB_CFG_HBDF_DEV                   30
#define V_P2SB_CFG_HBDF_FUNC                  6
#define R_P2SB_CFG_LFIORIEC                   0x00000074U      ///< LPC Fixed IO Route To IEC
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
#define B_P2SB_CFG_SBIADDR_OFFSET             0x0000ffffU      ///< Address Offset
                                                               /* Register address offset.  The content
                                                                * of this register field is sent in
                                                                * the IOSF Sideband Message Register
                                                                * Access address(15:0) field.
                                                                */
#define B_P2SB_CFG_SBIADDR_DESTID             ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Destination Port ID
                                                               /* The content of this register field
                                                                * is sent in the IOSF Sideband Message
                                                                * Register Access dest field.
                                                                * See the IOSF Sideband C-spec chapter
                                                                * for Destination ID assignments.
                                                                */
#define N_P2SB_CFG_SBIADDR_DESTID             24
#define R_P2SB_CFG_SBIDATA_V2                    0x000000d4U      ///< SBI Data
                                                               /* Provides mechanism to send message on IOSFSB
                                                                */
#define R_P2SB_CFG_SBISTAT_V2                    0x000000d8U      ///< SBI Status
                                                               /* Provides mechanism to send message on IOSFSB
                                                                */
#define B_P2SB_CFG_SBISTAT_INITRDY            BIT0             ///< Initiate/ Ready
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
#define B_P2SB_CFG_SBISTAT_POSTED             BIT7             ///< SBI Posted
                                                               /* When set to 1, the message will
                                                                * be send as a posted message instead
                                                                * of non-posted.  This should only
                                                                * be used if the receiver is known
                                                                * to support posted operations for
                                                                * the specified operation.
                                                                */
#define N_P2SB_CFG_SBISTAT_POSTED             7
#define B_P2SB_CFG_SBISTAT_OPCODE_V2             ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< SBI Opcode
                                                               /* This is the Opcode sent in the IOSF
                                                                * sideband message.
                                                                */
#define N_P2SB_CFG_SBISTAT_OPCODE             8
#define R_P2SB_CFG_SBIRID_V2                     0x000000daU      ///< SBI Routing Identification
                                                               /* Provides mechanism to send message on IOSFSB
                                                                */
#define B_P2SB_CFG_SBIRID_FID                 ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Function ID
                                                               /* The contents of this field are sent
                                                                * in the IOSF Sideband Register access
                                                                * FID field.
                                                                * This field should generally remain
                                                                * at zero unless specifically required
                                                                * by a particular application.
                                                                */
#define N_P2SB_CFG_SBIRID_FID                 0
#define B_P2SB_CFG_SBIRID_BAR                 ( BIT10 | BIT9 | BIT8 ) ///< Base Address Register
                                                               /* The contents of this field are sent
                                                                * in the IOSF Sideband Register Access
                                                                * BAR field.  This should be zero
                                                                * performing a Memory Mapped operation
                                                                * to a PCI compliant device.
                                                                */
#define N_P2SB_CFG_SBIRID_BAR                 8
#define B_P2SB_CFG_SBIRID_FBE                 ( BIT15 | BIT14 | BIT13 | BIT12 ) ///< First Byte Enable
                                                               /* The content of this field is sent
                                                                * in the IOSF Sideband Register Access
                                                                * FBE field.
                                                                */
#define N_P2SB_CFG_SBIRID_FBE                 12
#define R_P2SB_CFG_SBIEXTADDR_V2                 0x000000dcU      ///< SBI Extended Address
                                                               /* Provides mechanism to send message on IOSFSB
                                                                */

//
// Others
//
#define R_P2SB_CFG_P2SBC                      0x000000e0U      ///< P2SB Control
                                                               /* P2SB general configuration register
                                                                */
#define B_P2SB_CFG_P2SBC_RCSPE                BIT4             ///< RAVDM Completion Status Policy Enable
                                                               /* When this bit is SET, P2SB performs
                                                                * the following. 1) Return '000' (Succesful
                                                                * Completion) for upstream Reg_rsp
                                                                * RAVDM on IOSF-P. 2) Return '001'
                                                                * (Unsupported Requst) for upstream
                                                                * Reg_rsp. When clear P2SB always
                                                                * returbn '000' (Succesful Completion)
                                                                * for Upstream Reg_rsp RAVDM
                                                                */
#define B_P2SB_CFG_P2SBC_HIDE                 BIT8             ///< Hide Device
                                                               /* When this bit is set, the P2SB will
                                                                * return 1s on any PCI Configuration
                                                                * Read on IOSF-P.  All other transactions
                                                                * including PCI Configuration Writes
                                                                * are unaffected by this.  This does
                                                                * not affect reads performed on the
                                                                * IOSF-SB interface.
                                                                */
#define B_P2SB_CFG_P2SBC_PGCBCGE              BIT16            ///< PGCB Clock Gating Enable
                                                               /* When asserted the P2SB can de-assert
                                                                * the clock request to disable the
                                                                * PGCB clock dynamically when it reaches
                                                                * the power down idle state.
                                                                */
#define B_P2SB_CFG_P2SBC_MASKLOCK             BIT17            ///< Endpoint Mask Lock
                                                               /* Locks the value of the EPMASK[0-7]
                                                                * registers.  Once this value is written
                                                                * to a one it may only be cleared
                                                                * by a reset.
                                                                */
#define B_P2SB_CFG_P2SBC_DPPEE                BIT24            ///< Data Phase Parity Error Enable
                                                               /* IOSFP data phase parity error handling
                                                                * enabling bit
                                                                */
#define B_P2SB_CFG_P2SBC_CPEE                 BIT25            ///< Command Parity Error Enable
                                                               /* IOSFP command parity error handling
                                                                * enabling bit
                                                                */
#define B_P2SB_CFG_P2SBC_DPEE                 BIT26            ///< Data Parity Error Enable
                                                               /* IOSFP data parity error handling(EP
                                                                * bit) enabling bit
                                                                */
#define R_P2SB_CFG_PCE                        0x000000e4U      ///< Power Control Enable
                                                               /* Power Control Enable register
                                                                */
#define B_P2SB_CFG_PCE_PMCPG_EN               BIT0             ///< PMC Power Gating Enable
                                                               /* When set to 1, the P2SB will engage
                                                                * power gating if it is idle and the
                                                                * pmc_p2sb_sw_pg_req_b signal is asserted.
                                                                */
#define B_P2SB_CFG_PCE_I3E                    BIT1             ///< I3 Enable
                                                               /* No support for S0i3 power gating.
                                                                */
#define B_P2SB_CFG_PCE_D3HE                   BIT2             ///< D3-Hot Enable
                                                               /* No support for D3 Hot power gating.
                                                                */
#define B_P2SB_CFG_PCE_HAE                    BIT5             ///< Hardware Autonomous Enable
                                                               /* When set, the P2SB will automatically
                                                                * engage power gating when it has
                                                                * reached its idle condition.
                                                                */
#define R_P2SB_CFG_PDOMAIN                    0x000000e8U      ///< Primary Clock Domain Controls
                                                               /* Primary CDC configuration register
                                                                */
#define B_P2SB_CFG_PDOMAIN_CGD                BIT0             ///< Primary Clock Gating Disable
                                                               /* When set to 1, the local clock gating
                                                                * for the IOSF-P clock domain within
                                                                * the P2SB will be disabled.
                                                                */
#define B_P2SB_CFG_PDOMAIN_CKREQD             BIT1             ///< Primary CLKREQ DISABLE
                                                               /* Primary CLKREQ DISABLE Register
                                                                * bit
                                                                */
#define R_P2SB_CFG_SDOMAIN                    0x000000eaU      ///< Sideband Clock Domain Controls
                                                               /* Sideband CDC configuration register
                                                                */
#define B_P2SB_CFG_SDOMAIN_CGD                BIT0             ///< Sideband Clock Gating Disable
                                                               /* When set to 1, the local clock gating
                                                                * for the IOSF-P clock domain within
                                                                * the P2SB will be disabled.
                                                                */
#define B_P2SB_CFG_SDOMAIN_CKREQD             BIT1             ///< Sideband CLKREQ DISABLE
                                                               /* Sideband CLKREQ DISABLE
                                                                */
#define R_P2SB_CFG_UREC                       0x000000f4U      ///< Unsupported Request Error Control
                                                               /* This register is only reset by a loss of
                                                                * core power
                                                                */
#define B_P2SB_CFG_UREC_ERE                   BIT0             ///< Error Reporting Enable
                                                               /* When set, error logged in UES register
                                                                * will trigger error reporting flow
                                                                * in P2SB; Used only when ENABLE_AER
                                                                * is set
                                                                */
#define R_P2SB_CFG_SBREGPOSTED0               0x00000200U      ///< Sideband Register Posted 0
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
#define R_P2SB_CFG_SBREGPOSTED1               0x00000204U      ///< Sideband Register Posted 1
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
#define R_P2SB_CFG_SBREGPOSTED2               0x00000208U      ///< Sideband Register Posted 2
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
#define R_P2SB_CFG_SBREGPOSTED3               0x0000020cU      ///< Sideband Register Posted 3
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
#define R_P2SB_CFG_SBREGPOSTED4               0x00000210U      ///< Sideband Register Posted 4
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
#define R_P2SB_CFG_SBREGPOSTED5               0x00000214U      ///< Sideband Register Posted 5
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
#define R_P2SB_CFG_SBREGPOSTED6               0x00000218U      ///< Sideband Register Posted 6
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
#define R_P2SB_CFG_SBREGPOSTED7               0x0000021cU      ///< Sideband Register Posted 7
                                                               /* provides an mechanism to send MMIO writes
                                                                * as posted writes on the IOSFSB space
                                                                */
#define R_P2SB_CFG_EPMASK0                    0x00000220U      ///< Endpoint Mask 0
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
#define R_P2SB_CFG_EPMASK1                    0x00000224U      ///< Endpoint Mask 1
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
#define R_P2SB_CFG_EPMASK2                    0x00000228U      ///< Endpoint Mask 2
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
#define R_P2SB_CFG_EPMASK3                    0x0000022cU      ///< Endpoint Mask 3
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
#define R_P2SB_CFG_EPMASK4                    0x00000230U      ///< Endpoint Mask 4
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
#define R_P2SB_CFG_EPMASK5                    0x00000234U      ///< Endpoint Mask 5
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
#define R_P2SB_CFG_EPMASK6                    0x00000238U      ///< Endpoint Mask 6
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */
#define R_P2SB_CFG_EPMASK7                    0x0000023cU      ///< Endpoint Mask 7
                                                               /* provide a mechanism for disabling particular
                                                                * IOSF-SB endpoints from being allowed to
                                                                * targeted by transactions from the P2SB
                                                                */

#endif      // _P2SB_REGS_H_
