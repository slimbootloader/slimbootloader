/** @file
  Register names for PCH PCI-E root port devices

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

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_PCIE_H_
#define _PCH_REGS_PCIE_H_


#define R_PCH_PCIE_CFG_MPC                            0xD8

#define R_PCH_PCIE_CFG_SMSCS                          0xDC

#define R_PCH_PCIE_CFG_SPR                            0xE0


#define R_PCH_PCIE_CFG_RPPGEN                         0xE2


#define  R_PCH_PCIE_CFG_L1SCTL2                       0x20C ///< L1 Sub-States Control 2
#define  B_PCH_PCIE_CFG_L1SCTL2_POWT                  BIT3
#define  B_PCH_PCIE_CFG_L1SCTL2_POWT_OFFSET           3
#define  B_PCH_PCIE_CFG_L1SCTL2_POWT_MASK             0x000000F8
#define  V_PCH_PCIE_CFG_L1SCTL2_POWT                  0x5
#define  B_PCH_PCIE_CFG_L1SCTL2_TPOS                  BIT0
#define  B_PCH_PCIE_CFG_L1SCTL2_TPOS_OFFSET           0
#define  B_PCH_PCIE_CFG_L1SCTL2_TPOS_MASK             0x00000003
#define  V_PCH_PCIE_CFG_L1SCTL2_TPOS                  0x2


#define R_PCH_PCIE_CFG_V1VCRC                         0x29C


#define R_PCH_PCIE_CFG_V1VCRC                         0x29C


#define R_PCH_PCIE_CFG_G4L0SCTL                       0x310
#define R_PCH_PCIE_CFG_G4L0SCTL                       0x310


#define R_PCH_PCIE_CFG_LTROVR                         0x400
#define B_PCH_PCIE_CFG_LTROVR_LTRNSROVR               BIT31 ///< LTR Non-Snoop Requirement Bit Override
#define B_PCH_PCIE_CFG_LTROVR_LTRSROVR                BIT15 ///< LTR Snoop Requirement Bit Override
#define R_PCH_PCIE_CFG_LTROVR2                        0x404
#define B_PCH_PCIE_CFG_LTROVR2_FORCE_OVERRIDE         BIT3 ///< LTR Force Override Enable
#define B_PCH_PCIE_CFG_LTROVR2_LOCK                   BIT2 ///< LTR Override Lock
#define B_PCH_PCIE_CFG_LTROVR2_LTRNSOVREN             BIT1 ///< LTR Non-Snoop Override Enable
#define B_PCH_PCIE_CFG_LTROVR2_LTRSOVREN              BIT0 ///< LTR Snoop Override Enable
#define R_PCH_PCIE_CFG_TNPT                           0x418 ///< Thermal and Power Throttling
#define B_PCH_PCIE_CFG_TNPT_DRXLTE                    BIT1
#define B_PCH_PCIE_CFG_TNPT_DTXLTE                    BIT0
#define R_PCH_PCIE_CFG_PCIEPMECTL                     0x420
#define B_PCH_PCIE_CFG_PCIEPMECTL_DLSULPPGE           BIT30
#define B_PCH_PCIE_CFG_PCIEPMECTL_L1LE                BIT17
#define B_PCH_PCIE_CFG_PCIEPMECTL_L1FSOE              BIT0
#define B_PCH_PCIE_CFG_PCIEPMECTL3_PMREQCPGEXH        BIT0
#define B_PCH_PCIE_CFG_PCIEPMECTL3_PMREQCPGEXH_OFFSET 0


#define  R_PCH_PCIE_RXMC1                               0xC90
#define  B_PCH_PCIE_RXMC1_MSRVS                         BIT26
#define  B_PCH_PCIE_RXMC1_MSRVS_OFFSET                  26
#define  B_PCH_PCIE_RXMC1_MSRVS_MASK                    0xFC000000
#define  V_PCH_PCIE_RXMC1_MSRVS                         0x13
#define  B_PCH_PCIE_RXMC1_MSRTS                         BIT20
#define  B_PCH_PCIE_RXMC1_MSRTS_OFFSET                  20
#define  B_PCH_PCIE_RXMC1_MSRTS_MASK                    0x3F00000
#define  V_PCH_PCIE_RXMC1_MSRTS                         0x3F
#define  B_PCH_PCIE_RXMC1_TMSLOP                        BIT13
#define  B_PCH_PCIE_RXMC1_TMSLOP_OFFSET                 13
#define  B_PCH_PCIE_RXMC1_TMSLOP_MASK                   0x6000
#define  V_PCH_PCIE_RXMC1_TMSLOP                        0x1
#define  B_PCH_PCIE_RXMC1_VMSLOP                        BIT11
#define  B_PCH_PCIE_RXMC1_VMSLOP_OFFSET                 11
#define  B_PCH_PCIE_RXMC1_VMSLOP_MASK                   0x1800
#define  V_PCH_PCIE_RXMC1_VMSLOP                        0x1
#define  B_PCH_PCIE_RXMC1_MSRM                          BIT9
#define  B_PCH_PCIE_RXMC1_MMNOLS                        BIT4
#define  B_PCH_PCIE_RXMC1_MMNOLS_OFFSET                 4
#define  B_PCH_PCIE_RXMC1_MMNOLS_MASK                   0x1F0
#define  V_PCH_PCIE_RXMC1_MMNOLS                        0x1
#define  B_PCH_PCIE_RXMC1_MVS                           BIT3
#define  B_PCH_PCIE_RXMC1_MILRTS                        BIT2
#define  B_PCH_PCIE_RXMC1_MIUDVMS                       BIT1
#define  B_PCH_PCIE_RXMC1_MIESS                         BIT0

#define  R_PCH_PCIE_RXMC2                               0xC94
#define  B_PCH_PCIE_RXMC2_MNOTSS                        BIT19
#define  B_PCH_PCIE_RXMC2_MNOTSS_OFFSET                 19
#define  B_PCH_PCIE_RXMC2_MNOTSS_MASK                   0x1F80000
#define  V_PCH_PCIE_RXMC2_MNOTSS                        0x20
#define  B_PCH_PCIE_RXMC2_MMTOS                         BIT13
#define  B_PCH_PCIE_RXMC2_MMTOS_OFFSET                  13
#define  B_PCH_PCIE_RXMC2_MMTOS_MASK                    0x7E000
#define  V_PCH_PCIE_RXMC2_MMTOS                         0x1C
#define  B_PCH_PCIE_RXMC2_MNOVSS                        BIT6
#define  B_PCH_PCIE_RXMC2_MNOVSS_OFFSET                 6
#define  B_PCH_PCIE_RXMC2_MNOVSS_MASK                   0x1FC0
#define  V_PCH_PCIE_RXMC2_MNOVSS                        0x1C
#define  B_PCH_PCIE_RXMC2_MMVOS                         BIT0
#define  B_PCH_PCIE_RXMC2_MMVOS_OFFSET                  0
#define  B_PCH_PCIE_RXMC2_MMVOS_MASK                    0x3F
#define  V_PCH_PCIE_RXMC2_MMVOS                         0x0

#define  R_PCH_PCIE_PL16MPCPS                           0xEE0
#define  B_PCH_PCIE_PL16MPCPS_MARGINDRISW               BIT0
//
// PCIE PCRs (PID:SPA SPB SPC SPD SPE SPF)
//
#define R_SPX_PCR_PCD                         0                       ///< Port configuration and disable
#define B_SPX_PCR_PCD_RP1FN                   (BIT2 | BIT1 | BIT0)    ///< Port 1 Function Number
#define S_SPX_PCR_PCD_RP_FIELD                4                       ///< 4 bits for each RP FN


#define R_SPX_SIP16_PCR_PCD                   0x3E00                  ///< Port configuration and disable
//
// ICC PCR (PID: ICC)
//
#endif
