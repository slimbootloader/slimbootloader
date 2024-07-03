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

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_PCIE_H_
#define _PCH_REGS_PCIE_H_


#define V_PCH_PCIE_CFG_VENDOR_ID                  V_PCH_INTEL_VENDOR_ID

#define R_PCH_PCIE_CFG_MID                            0x80
#define S_PCH_PCIE_CFG_MID                            2
#define R_PCH_PCIE_CFG_MC                             0x82
#define S_PCH_PCIE_CFG_MC                             2
#define R_PCH_PCIE_CFG_MA                             0x84
#define S_PCH_PCIE_CFG_MA                             4
#define R_PCH_PCIE_CFG_MD                             0x88
#define S_PCH_PCIE_CFG_MD                             2

#define R_PCH_PCIE_CFG_SVCAP                          0x90
#define R_PCH_PCIE_CFG_SIP17_SVCAP                    0x98
#define S_PCH_PCIE_CFG_SVCAP                          2
#define R_PCH_PCIE_CFG_SVID                           0x94
#define S_PCH_PCIE_CFG_SVID                           4

#define R_PCH_PCIE_CFG_PMCAP                          0xA0
#define R_PCH_PCIE_CFG_PMCS                           (R_PCH_PCIE_CFG_PMCAP + R_PCIE_PMCS_OFFST)
#define B_PCH_PCIE_CFG_PMCS_PS_MASK                   0x3
#define V_PCH_PCIE_CFG_PMCS_PS_D3H                    0x3

#define R_PCH_PCIE_CFG_MPC2                           0xD4
#define B_PCH_PCIE_CFG_MPC2_L1SSESE                   BIT30
#define B_PCH_PCIE_CFG_MPC2_PLLWAIT                   (BIT26 | BIT25 | BIT24)
#define B_PCH_PCIE_CFG_MPC2_DISPLLEWL1SE              BIT16
#define S_PCH_PCIE_CFG_MPC2                           4
#define B_PCH_PCIE_CFG_MPC2_PTNFAE                    BIT12
#define B_PCH_PCIE_CFG_MPC2_LSTP                      BIT6
#define B_PCH_PCIE_CFG_MPC2_IEIME                     BIT5
#define B_PCH_PCIE_CFG_MPC2_ASPMCOEN                  BIT4
#define B_PCH_PCIE_CFG_MPC2_ASPMCO                    (BIT3 | BIT2)
#define V_PCH_PCIE_CFG_MPC2_ASPMCO_DISABLED           0
#define V_PCH_PCIE_CFG_MPC2_ASPMCO_L0S                (1 << 2)
#define V_PCH_PCIE_CFG_MPC2_ASPMCO_L1                 (2 << 2)
#define V_PCH_PCIE_CFG_MPC2_ASPMCO_L0S_L1             (3 << 2)
#define B_PCH_PCIE_CFG_MPC2_EOIFD                     BIT1

#define R_PCH_PCIE_CFG_MPC                            0xD8
#define S_PCH_PCIE_CFG_MPC                            4
#define B_PCH_PCIE_CFG_MPC_PMCE                       BIT31
#define B_PCH_PCIE_CFG_MPC_HPCE                       BIT30
#define B_PCH_PCIE_CFG_MPC_MMBNCE                     BIT27
#define B_PCH_PCIE_CFG_MPC_P8XDE                      BIT26
#define B_PCH_PCIE_CFG_MPC_IRRCE                      BIT25
#define B_PCH_PCIE_CFG_MPC_SRL                        BIT23
#define B_PCH_PCIE_CFG_MPC_UCEL                       (BIT20 | BIT19 | BIT18)
#define N_PCH_PCIE_CFG_MPC_UCEL                       18
#define B_PCH_PCIE_CFG_MPC_CCEL                       (BIT17 | BIT16 | BIT15)
#define N_PCH_PCIE_CFG_MPC_CCEL                       15
#define B_PCH_PCIE_CFG_MPC_PCIESD                     (BIT14 | BIT13)
#define N_PCH_PCIE_CFG_MPC_PCIESD                     13
#define V_PCH_PCIE_CFG_MPC_PCIESD_GEN1                1
#define V_PCH_PCIE_CFG_MPC_PCIESD_GEN2                2
#define V_PCH_PCIE_CFG_MPC_PCIESD_GEN3                3
#define V_PCH_PCIE_CFG_MPC_PCIESD_GEN4                4
#define B_PCH_PCIE_CFG_MPC_MCTPSE                     BIT3
#define B_PCH_PCIE_CFG_MPC_HPME                       BIT1
#define N_PCH_PCIE_CFG_MPC_HPME                       1
#define B_PCH_PCIE_CFG_MPC_PMME                       BIT0
#define N_PCH_PCIE_CFG_MPC_PMME                       0

#define R_PCH_PCIE_CFG_SMSCS                          0xDC
#define S_PCH_PCIE_CFG_SMSCS                          4
#define B_PCH_PCIE_CFG_SMSCS_PMCS                     BIT31
#define N_PCH_PCIE_CFG_SMSCS_LERSMIS                  5
#define N_PCH_PCIE_CFG_SMSCS_HPLAS                    4
#define N_PCH_PCIE_CFG_SMSCS_HPPDM                    1
#define B_PCH_PCIE_CFG_SMSCS_PMMS                     BIT0
#define N_PCH_PCIE_CFG_SMSCS_PMMS                     0

#define R_PCH_PCIE_CFG_SPR                            0xE0

#define R_PCH_PCIE_CFG_RPDCGEN                        0xE1
#define S_PCH_PCIE_CFG_RPDCGEN                        1
#define B_PCH_PCIE_CFG_RPDCGEN_RPSCGEN                BIT7
#define B_PCH_PCIE_CFG_RPDCGEN_PTOCGE                 BIT6
#define B_PCH_PCIE_CFG_RPDCGEN_LCLKREQEN              BIT5
#define B_PCH_PCIE_CFG_RPDCGEN_BBCLKREQEN             BIT4
#define B_PCH_PCIE_CFG_RPDCGEN_SRDBCGEN               BIT2
#define B_PCH_PCIE_CFG_RPDCGEN_RPDLCGEN               BIT1
#define B_PCH_PCIE_CFG_RPDCGEN_RPDBCGEN               BIT0

#define R_PCH_PCIE_CFG_RPPGEN                         0xE2
#define B_PCH_PCIE_CFG_RPPGEN_PTOTOP                  BIT6
#define B_PCH_PCIE_CFG_RPPGEN_SEOSCGE                 BIT4

#define R_PCH_PCIE_CFG_PWRCTL                         0xE8
#define B_PCH_PCIE_CFG_PWRCTL_LTSSMRTC                BIT20
#define B_PCH_PCIE_CFG_PWRCTL_WPDMPGEP                BIT17
#define B_PCH_PCIE_CFG_PWRCTL_DBUPI                   BIT15
#define B_PCH_PCIE_CFG_PWRCTL_TXSWING                 BIT13
#define B_PCH_PCIE_CFG_PWRCTL_RPL1SQPOL               BIT1
#define B_PCH_PCIE_CFG_PWRCTL_RPDTSQPOL               BIT0

#define R_PCH_PCIE_CFG_DC                             0xEC
#define B_PCH_PCIE_CFG_DC_PCIBEM                      BIT2
#define B_PCH_PCIE_CFG_DC_COM                         BIT13

#define R_PCH_PCIE_CFG_IPCS                           0xF0
#define B_PCH_PCIE_CFG_IPCS_IMPS                      (BIT10 | BIT9 | BIT8)
#define N_PCH_PCIE_CFG_IPCS_IMPS                      8
#define V_PCH_PCIE_CFG_IPCS_IMPS_64B                  7
#define V_PCH_PCIE_CFG_IPCS_IMPS_256B                 1

#define R_PCH_PCIE_CFG_PHYCTL2                        0xF5
#define B_PCH_PCIE_CFG_PHYCTL2_TDFT                   (BIT7 | BIT6)
#define B_PCH_PCIE_CFG_PHYCTL2_TXCFGCHGWAIT           (BIT5 | BIT4)
#define N_PCH_PCIE_CFG_PHYCTL2_TXCFGCHGWAIT           4
#define B_PCH_PCIE_CFG_PHYCTL2_PXPG3PLLOFFEN          BIT1
#define B_PCH_PCIE_CFG_PHYCTL2_PXPG2PLLOFFEN          BIT0

#define R_PCH_PCIE_CFG_IOSFSBCS                       0xF7
#define B_PCH_PCIE_CFG_IOSFSBCS_SCPTCGE               BIT6
#define B_PCH_PCIE_CFG_IOSFSBCS_SIID                  (BIT3 | BIT2)

#define R_PCH_PCIE_CFG_STRPFUSECFG                    0xFC
#define B_PCH_PCIE_CFG_STRPFUSECFG_PXIP               (BIT27 | BIT26 | BIT25 | BIT24)
#define N_PCH_PCIE_CFG_STRPFUSECFG_PXIP               24
#define B_PCH_PCIE_CFG_STRPFUSECFG_RPC_SIP17          (BIT16 | BIT15 | BIT14)
#define B_PCH_PCIE_CFG_STRPFUSECFG_RPC                (BIT15 | BIT14)
#define V_PCH_PCIE_CFG_STRPFUSECFG_RPC_1_1_1_1        0
#define V_PCH_PCIE_CFG_STRPFUSECFG_RPC_2_1_1          1
#define V_PCH_PCIE_CFG_STRPFUSECFG_RPC_2_2            2
#define V_PCH_PCIE_CFG_STRPFUSECFG_RPC_4              3
#define V_PCH_PCIE_CFG_STRPFUSECFG_RPC_4_SIP17        4
#define V_PCH_PCIE_CFG_STRPFUSECFG_RPC_16             7
#define N_PCH_PCIE_CFG_STRPFUSECFG_RPC                14
#define B_PCH_PCIE_CFG_STRPFUSECFG_MODPHYIOPMDIS      BIT9
#define B_PCH_PCIE_CFG_STRPFUSECFG_PLLSHTDWNDIS       BIT8
#define B_PCH_PCIE_CFG_STRPFUSECFG_STPGATEDIS         BIT7
#define B_PCH_PCIE_CFG_STRPFUSECFG_ASPMDIS            BIT6
#define B_PCH_PCIE_CFG_STRPFUSECFG_LDCGDIS            BIT5
#define B_PCH_PCIE_CFG_STRPFUSECFG_LTCGDIS            BIT4
#define B_PCH_PCIE_CFG_STRPFUSECFG_CDCGDIS            BIT3
#define B_PCH_PCIE_CFG_STRPFUSECFG_DESKTOPMOB         BIT2

//
//PCI Express Extended Capability Registers
//

#define R_PCH_PCIE_CFG_EXCAP_OFFSET                   0x100

#define R_PCH_PCIE_CFG_EX_AECH                        0x100 ///< Advanced Error Reporting Capability Header
#define V_PCH_PCIE_CFG_EX_AEC_CV                      0x1
#define R_PCH_PCIE_CFG_EX_UEM                         (R_PCH_PCIE_CFG_EX_AECH + R_PCIE_EX_UEM_OFFSET) // Uncorrectable Error Mask

#define R_PCH_PCIE_CFG_EX_CES                         0x110 ///< Correctable Error Status
#define B_PCH_PCIE_CFG_EX_CES_BD                      BIT7  ///< Bad DLLP Status
#define B_PCH_PCIE_CFG_EX_CES_BT                      BIT6  ///< Bad TLP Status
#define B_PCH_PCIE_CFG_EX_CES_RE                      BIT0  ///< Receiver Error Status

#define  R_PCH_PCIE_CFG_L1SCTL2                       0x20C ///< L1 Sub-States Control 2
#define  B_PCH_PCIE_CFG_L1SCTL2_POWT                  BIT3
#define  B_PCH_PCIE_CFG_L1SCTL2_POWT_OFFSET           3
#define  B_PCH_PCIE_CFG_L1SCTL2_POWT_MASK             0x000000F8
#define  V_PCH_PCIE_CFG_L1SCTL2_POWT                  0x5
#define  B_PCH_PCIE_CFG_L1SCTL2_TPOS                  BIT0
#define  B_PCH_PCIE_CFG_L1SCTL2_TPOS_OFFSET           0
#define  B_PCH_PCIE_CFG_L1SCTL2_TPOS_MASK             0x00000003
#define  V_PCH_PCIE_CFG_L1SCTL2_TPOS                  0x2

//CES.RE, CES.BT, CES.BD

#define R_PCH_PCIE_CFG_EX_ACSECH                      0x220 ///< ACS Extended Capability Header
#define V_PCH_PCIE_CFG_EX_ACS_CV                      0x1
#define R_PCH_PCIE_CFG_EX_ACSCAPR                     (R_PCH_PCIE_CFG_EX_ACSECH + R_PCIE_EX_ACSCAPR_OFFSET)

#define R_PCH_PCIE_CFG_EX_PTMECH                      0x150 ///< PTM Extended Capability Header
#define V_PCH_PCIE_CFG_EX_PTM_CV                      0x1
#define R_PCH_PCIE_CFG_EX_PTMCAPR                     (R_PCH_PCIE_CFG_EX_PTMECH + R_PCIE_EX_PTMCAP_OFFSET)

#define R_PCH_PCIE_CFG_PVCCR1                         0x284
#define B_PCH_PCIE_CFG_PVCCR1_EVCC_MASK               0x7
#define V_PCH_PCIE_CFG_PVCCR1_EVCC_2_VC               0x1

#define R_PCH_PCIE_CFG_V0VCRC                         0x290
#define B_PCH_PCIE_CFG_V0VCRC_MTS_MASK                0x7F0000

#define R_PCH_PCIE_CFG_VC0CTL                         0x294 ///< Virtual Channel 0 Resource Control
#define B_PCH_PCIE_CFG_VC0CTL_TVM_MASK                0xFE
#define B_PCH_PCIE_CFG_VC0CTL_TVM_OFFSET              1
#define B_PCH_PCIE_CFG_VC0CTL_TVM_NO_VC               0x7F
#define B_PCH_PCIE_CFG_VC0CTL_TVM_EN_VC               0x1F
#define B_PCH_PCIE_CFG_VC0CTL_ETVM_MASK               0x3F
#define B_PCH_PCIE_CFG_VC0CTL_ETVM_OFFSET             10

#define R_PCH_PCIE_CFG_V1VCRC                         0x29C
#define B_PCH_PCIE_CFG_V1VCRC_MTS_MASK                0x7F0000

#define R_PCH_PCIE_CFG_VC1CTL                         0x2A0
#define B_PCH_PCIE_CFG_VC1CTL_TVM_MASK                0xFE
#define B_PCH_PCIE_CFG_VC1CTL_TVM                     BIT1
#define B_PCH_PCIE_CFG_VC1CTL_TVM_OFFSET              1
#define V_PCH_PCIE_CFG_VC1CTL_TVM                     0x7F
#define B_PCH_PCIE_CFG_VC1CTL_ETVM_MASK               0x1F
#define B_PCH_PCIE_CFG_VC1CTL_ETVM_OFFSET             10
#define B_PCH_PCIE_CFG_VC1CTL_ID_MASK                 0xF000000
#define B_PCH_PCIE_CFG_VC1CTL_ID_OFFSET               24
#define V_PCH_PCIE_CFG_VC1CTL_ID_ONE                  1
#define B_PCH_PCIE_CFG_VC1CTL_EN_OFFSET               0x80000000
#define B_PCH_PCIE_CFG_VC1CTL_EN                      BIT31

#define R_PCH_PCIE_CFG_VC1STS                         0x2A6
#define B_PCH_PCIE_CFG_VC1STS_NP                      BIT1

#define R_PCH_PCIE_CFG_VC0STS                         0x2A9
#define B_PCH_PCIE_CFG_VC0STS_NP                      BIT1

#define R_PCH_PCIE_CFG_V1VCRC                         0x29C
#define R_PCH_PCIE_CFG_V1VCRC_MTS_MASK                0x7F
#define R_PCH_PCIE_CFG_V1VCRC_MTS_OFFSET              16

#define R_PCH_PCIE_CFG_EX_L1SECH                      0x200 ///< L1 Sub-States Extended Capability Header
#define V_PCH_PCIE_CFG_EX_L1S_CV                      0x1
#define R_PCH_PCIE_CFG_EX_L1SCAP                      (R_PCH_PCIE_CFG_EX_L1SECH + R_PCIE_EX_L1SCAP_OFFSET)
#define R_PCH_PCIE_CFG_EX_L1SCTL1                     (R_PCH_PCIE_CFG_EX_L1SECH + R_PCIE_EX_L1SCTL1_OFFSET)
#define R_PCH_PCIE_CFG_EX_L1SCTL2                     (R_PCH_PCIE_CFG_EX_L1SECH + R_PCIE_EX_L1SCTL2_OFFSET)

#define R_PCH_PCIE_CFG_EX_DPCECH                      0xA00 ///< Downstream Port Containment
#define V_PCH_PCIE_CFG_EX_DPCECH_CV                   0x1

#define R_PCH_PCIE_CFG_PCIERTP1                       0x300
#define R_PCH_PCIE_CFG_PCIERTP2                       0x304
#define R_PCH_PCIE_CFG_G4L0SCTL                       0x310
#define R_PCH_PCIE_CFG_PCIENFTS                       0x314
#define R_PCH_PCIE_CFG_PCIEL0SC                       0x318
#define R_PCH_PCIE_CFG_G4L0SCTL                       0x310
#define R_PCH_PCIE_CFG_G5L0SCTL                      0x1E00

#define R_PCH_PCIE_CFG_PCIECFG2                       0x320
#define B_PCH_PCIE_CFG_PCIECFG2_LBWSSTE               BIT30
#define B_PCH_PCIE_CFG_PCIECFG2_RLLG3R                BIT27
#define B_PCH_PCIE_CFG_PCIECFG2_CROAOV                BIT24
#define B_PCH_PCIE_CFG_PCIECFG2_CROAOE                BIT23
#define B_PCH_PCIE_CFG_PCIECFG2_CRSREN                BIT22
#define B_PCH_PCIE_CFG_PCIECFG2_PMET                  (BIT21 | BIT20)
#define V_PCH_PCIE_CFG_PCIECFG2_PMET                  1
#define N_PCH_PCIE_CFG_PCIECFG2_PMET                  20

#define R_PCH_PCIE_CFG_PCIEDBG                        0x324
#define B_PCH_PCIE_CFG_PCIEDBG_LBWSSTE                BIT30
#define B_PCH_PCIE_CFG_PCIEDBG_USSP                   (BIT27 | BIT26)
#define B_PCH_PCIE_CFG_PCIEDBG_LGCLKSQEXITDBTIMERS    (BIT25 | BIT24)
#define B_PCH_PCIE_CFG_PCIEDBG_CTONFAE                BIT14
#define B_PCH_PCIE_CFG_PCIEDBG_LDSWQRP                BIT13
#define B_PCH_PCIE_CFG_PCIEDBG_SQOL0                  BIT7
#define B_PCH_PCIE_CFG_PCIEDBG_SPCE                   BIT5
#define B_PCH_PCIE_CFG_PCIEDBG_LR                     BIT4

#define R_PCH_PCIE_CFG_PCIESTS1                              0x328
#define B_PCH_PCIE_CFG_PCIESTS1_LTSMSTATE                    0xFF000000
#define N_PCH_PCIE_CFG_PCIESTS1_LTSMSTATE                    24
#define V_PCH_PCIE_CFG_PCIESTS1_LTSMSTATE_DETRDY             0x01
#define V_PCH_PCIE_CFG_PCIESTS1_LTSMSTATE_DETRDYECINP1CG     0x0E
#define V_PCH_PCIE_CFG_PCIESTS1_LTSMSTATE_L0                 0x33
#define V_PCH_PCIE_CFG_PCIESTS1_LTSMSTATE_DISWAIT            0x5E
#define V_PCH_PCIE_CFG_PCIESTS1_LTSMSTATE_DISWAITPG          0x60
#define V_PCH_PCIE_CFG_PCIESTS1_LTSMSTATE_RECOVERYSPEEDREADY 0x6C
#define V_PCH_PCIE_CFG_PCIESTS1_LTSMSTATE_RECOVERYLNK2DETECT 0x6F


#define B_PCH_PCIE_CFG_PCIESTS1_LNKSTAT               (BIT22 | BIT21 | BIT20 | BIT19)
#define N_PCH_PCIE_CFG_PCIESTS1_LNKSTAT               19
#define V_PCH_PCIE_CFG_PCIESTS1_LNKSTAT_L0            0x7

#define R_PCH_PCIE_CFG_PCIESTS2                       0x32C
#define B_PCH_PCIE_CFG_PCIESTS2_P4PNCCWSSCMES         BIT31
#define B_PCH_PCIE_CFG_PCIESTS2_P3PNCCWSSCMES         BIT30
#define B_PCH_PCIE_CFG_PCIESTS2_P2PNCCWSSCMES         BIT29
#define B_PCH_PCIE_CFG_PCIESTS2_P1PNCCWSSCMES         BIT28
#define B_PCH_PCIE_CFG_PCIESTS2_CLRE                  0x0000F000
#define N_PCH_PCIE_CFG_PCIESTS2_CLRE                  12

#define R_PCH_PCIE_CFG_PCIEALC                        0x338
#define B_PCH_PCIE_CFG_PCIEALC_ITLRCLD                BIT29
#define B_PCH_PCIE_CFG_PCIEALC_ILLRCLD                BIT28
#define B_PCH_PCIE_CFG_PCIEALC_BLKDQDA                BIT26
#define B_PCH_PCIE_CFG_PCIEALC_BLKDQDASD              BIT25
#define B_PCH_PCIE_CFG_PCIEALC_RTD3PDSP_SIP17         BIT21
#define B_PCH_PCIE_CFG_PCIEALC_RTD3PDSP               BIT20
#define B_PCH_PCIE_CFG_SIP17_PCIEALC_RTD3PDSP         BIT21 /// From SIP17 register layout is changed
#define B_PCH_PCIE_CFG_SIP17_PCIEALC_PDSP             BIT20 /// From SIP17 register layout is changed

#define R_PCH_PCIE_CFG_PTMPD                          0x390 ///< PTM Propagation Delay
#define R_PCH_PCIE_CFG_PTMLLMT                        0x394 ///< PTM Lower Local Host Time
#define R_PCH_PCIE_CFG_PTMULMT                        0x398 ///< PTM Upper Local Host Time
#define R_PCH_PCIE_CFG_PTMPSDC1                       0x39C ///< PTM Pipe Stage Delay Configuration 1
#define R_PCH_PCIE_CFG_PTMPSDC2                       0x3A0 ///< PTM Pipe Stage Delay Configuration 2
#define R_PCH_PCIE_CFG_PTMPSDC3                       0x3A4 ///< PTM Pipe Stage Delay Configuration 3
#define R_PCH_PCIE_CFG_PTMPSDC4                       0x3A8 ///< PTM Pipe Stage Delay Configuration 4
#define R_PCH_PCIE_CFG_PTMPSDC5                       0x3AC ///< PTM Pipe Stage Delay Configuration 5
#define R_PCH_PCIE_CFG_PTMECFG                        0x3B0 ///< PTM Extended Configuration
#define B_PCH_PCIE_CFG_PTMECFG_IOSFMADP               0xF   ///< IOSF Max Allowed Delay programming. bit0~bit3

#define R_PCH_PCIE_CFG_LTROVR                         0x400
#define B_PCH_PCIE_CFG_LTROVR_LTRNSROVR               BIT31 ///< LTR Non-Snoop Requirement Bit Override
#define B_PCH_PCIE_CFG_LTROVR_LTRSROVR                BIT15 ///< LTR Snoop Requirement Bit Override

#define R_PCH_PCIE_CFG_LTROVR2                        0x404
#define B_PCH_PCIE_CFG_LTROVR2_FORCE_OVERRIDE         BIT3 ///< LTR Force Override Enable
#define B_PCH_PCIE_CFG_LTROVR2_LOCK                   BIT2 ///< LTR Override Lock
#define B_PCH_PCIE_CFG_LTROVR2_LTRNSOVREN             BIT1 ///< LTR Non-Snoop Override Enable
#define B_PCH_PCIE_CFG_LTROVR2_LTRSOVREN              BIT0 ///< LTR Snoop Override Enable

#define R_PCH_PCIE_CFG_PHYCTL4                        0x408
#define B_PCH_PCIE_CFG_PHYCTL4_SQDIS                  BIT27

#define R_PCH_PCIE_CFG_TNPT                           0x418 ///< Thermal and Power Throttling
#define B_PCH_PCIE_CFG_TNPT_DRXLTE                    BIT1
#define B_PCH_PCIE_CFG_TNPT_DTXLTE                    BIT0
#define N_PCH_PCIE_CFG_TNPT_TP                        24
#define V_PCH_PCIE_CFG_TNPT_TP_3_MS                   0x2
#define R_PCH_PCIE_CFG_PCIEPMECTL                     0x420
#define B_PCH_PCIE_CFG_PCIEPMECTL_FDPPGE              BIT31
#define B_PCH_PCIE_CFG_PCIEPMECTL_DLSULPPGE           BIT30
#define B_PCH_PCIE_CFG_PCIEPMECTL_DLSULDLSD           BIT29
#define B_PCH_PCIE_CFG_PCIEPMECTL_L1LE                BIT17
#define B_PCH_PCIE_CFG_PCIEPMECTL_L1LTRTLV            (BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4)
#define N_PCH_PCIE_CFG_PCIEPMECTL_L1LTRTLV            4
#define V_PCH_PCIE_CFG_PCIEPMECTL_L1LTRTLV            0x32
#define B_PCH_PCIE_CFG_PCIEPMECTL_L1FSOE              BIT0

#define R_PCH_PCIE_CFG_PCIEPMECTL2                    0x424
#define B_PCH_PCIE_CFG_PCIEPMECTL2_CPMCSRE            BIT27
#define B_PCH_PCIE_CFG_PCIEPMECTL2_CPGEXH             BIT14
#define N_PCH_PCIE_CFG_PCIEPMECTL2_CPGEXH_OFFSET      14
#define B_PCH_PCIE_CFG_PCIEPMECTL2_CPGEXH_MASK        (BIT14 | BIT15)
#define V_PCH_PCIE_CFG_PCIEPMECTL2_CPGEXH_5US         0x1
#define B_PCH_PCIE_CFG_PCIEPMECTL2_CPGENH             BIT12
#define N_PCH_PCIE_CFG_PCIEPMECTL2_CPGENH_OFFSET      12
#define B_PCH_PCIE_CFG_PCIEPMECTL2_CPGENH_MASK        (BIT12 | BIT13)
#define B_PCH_PCIE_CFG_PCIEPMECTL2_PHYCLPGE           BIT11
#define B_PCH_PCIE_CFG_PCIEPMECTL2_FDCPGE             BIT8
#define B_PCH_PCIE_CFG_PCIEPMECTL2_DETSCPGE           BIT7
#define B_PCH_PCIE_CFG_PCIEPMECTL2_L23RDYSCPGE        BIT6
#define B_PCH_PCIE_CFG_PCIEPMECTL2_DISSCPGE           BIT5
#define B_PCH_PCIE_CFG_PCIEPMECTL2_L1SCPGE            BIT4

#define R_PCH_PCIE_CFG_PCE                            0x428
#define B_PCH_PCIE_CFG_PCE_HAE                        BIT5
#define B_PCH_PCIE_CFG_PCE_SE                         BIT3
#define B_PCH_PCIE_CFG_PCE_PMCRE                      BIT0

#define R_PCH_PCIE_CFG_PGCBCTL1                       0x42C

#define R_PCH_PCIE_CFG_PCIEPMECTL3                    0x434
#define B_PCH_PCIE_CFG_PCIEPMECTL3_L1PGAUTOPGEN       BIT4
#define B_PCH_PCIE_CFG_PCIEPMECTL3_OSCCGH             BIT2
#define B_PCH_PCIE_CFG_PCIEPMECTL3_OSCCGH_OFFSET      2
#define B_PCH_PCIE_CFG_PCIEPMECTL3_OSCCGH_MASK        (BIT3 | BIT2)
#define V_PCH_PCIE_CFG_PCIEPMECTL3_OSCCGH_1US         1
#define B_PCH_PCIE_CFG_PCIEPMECTL3_PMREQCPGEXH        BIT0
#define B_PCH_PCIE_CFG_PCIEPMECTL3_PMREQCPGEXH_MASK   0x3
#define B_PCH_PCIE_CFG_PCIEPMECTL3_PMREQCPGEXH_OFFSET 0
#define V_PCH_PCIE_CFG_PCIEPMECTL3_PMREQCPGEXH        0x1

#define R_PCH_PCIE_CFG_RTPCL2                         0x458
#define B_PCH_PCIE_CFG_RTPCL2_RTPOSTCL3PL             0x3F000
#define B_PCH_PCIE_CFG_RTPCL2_RTPRECL3PL6             0xFC0
#define B_PCH_PCIE_CFG_RTPCL2_RTPOSTCL2PL5            0x3F

#define R_PCH_PCIE_CFG_RTPCL3                         0x45C
#define B_PCH_PCIE_CFG_RTPCL3_RTPRECL7                0x3F000000
#define B_PCH_PCIE_CFG_RTPCL3_RTPOSTCL6               0xFC0000
#define B_PCH_PCIE_CFG_RTPCL3_RTPRECL6                0x3F000
#define B_PCH_PCIE_CFG_RTPCL3_RTPOSTCL5               0xFC0
#define B_PCH_PCIE_CFG_RTPCL3_RTPRECL5PL10            0x3F

#define R_PCH_PCIE_CFG_RTPCL4                         0x460
#define B_PCH_PCIE_CFG_RTPCL4_RTPOSTCL9               0x3F000000
#define B_PCH_PCIE_CFG_RTPCL4_RTPRECL9                0xFC0000
#define B_PCH_PCIE_CFG_RTPCL4_RTPOSTCL8               0x3F000
#define B_PCH_PCIE_CFG_RTPCL4_RTPRECL8                0xFC0
#define B_PCH_PCIE_CFG_RTPCL4_RTPOSTCL7               0x3F

#define R_PCH_PCIE_CFG_LTCO2                          0x474
#define B_PCH_PCIE_CFG_LTCO2_L3TCOE                   BIT25
#define B_PCH_PCIE_CFG_LTCO2_L2TCOE                   BIT24
#define B_PCH_PCIE_CFG_LTCO2_L3TPOSTCO                0xFC0000
#define B_PCH_PCIE_CFG_LTCO2_L3TPRECO                 0x3F000
#define B_PCH_PCIE_CFG_LTCO2_L2TPOSTCO                0xFC0
#define B_PCH_PCIE_CFG_LTCO2_L2TPRECO                 0x3F

#define R_PCH_PCIE_CFG_G3L0SCTL                       0x478
#define B_PCH_PCIE_CFG_G3L0SCTL_G3UCNFTS              0x0000FF00
#define B_PCH_PCIE_CFG_G3L0SCTL_G3CCNFTS              0x000000FF

#define R_PCH_PCIE_CFG_GDR                            0x690
#define B_PCH_PCIE_CFG_GDR_GPIOALD                    BIT10

#define R_PCH_PCIE_ACGR3S2                              0xC50
#define B_PCH_PCIE_ACGR3S2_G5EBM                        BIT20
#define B_PCH_PCIE_ACGR3S2_G4EBM                        BIT9
#define B_PCH_PCIE_ACGR3S2_G3EBM                        BIT8
#define B_PCH_PCIE_ACGR3S2_G2EBM                        BIT7
#define B_PCH_PCIE_ACGR3S2_G1EBM                        BIT6
#define B_PCH_PCIE_ACGR3S2_SRT                          BIT5

#define R_PCH_PCIE_CFG_VNNREMCTL                        0xC70
#define B_PCH_PCIE_CFG_VNNREMCTL_LRSLFVNNRE             (BIT1 | BIT0)
#define V_PCH_PCIE_CFG_VNNREMCTL_LRSLFVNNRE_4_OSC_CLK   0x0
#define V_PCH_PCIE_CFG_VNNREMCTL_LRSLFVNNRE_8_OSC_CLK   0x1
#define V_PCH_PCIE_CFG_VNNREMCTL_LRSLFVNNRE_16_OSC_CLK  0x2
#define V_PCH_PCIE_CFG_VNNREMCTL_LRSLFVNNRE_32_OSC_CLK  0x3
#define B_PCH_PCIE_CFG_VNNREMCTL_ISPLFVNNRE             (BIT3 | BIT2)
#define N_PCH_PCIE_CFG_VNNREMCTL_ISPLFVNNRE             2
#define V_PCH_PCIE_CFG_VNNREMCTL_ISPLFVNNRE_8_OSC_CLK   0x0
#define V_PCH_PCIE_CFG_VNNREMCTL_ISPLFVNNRE_16_OSC_CLK  0x1
#define V_PCH_PCIE_CFG_VNNREMCTL_ISPLFVNNRE_32_OSC_CLK  0x2
#define V_PCH_PCIE_CFG_VNNREMCTL_ISPLFVNNRE_64_OSC_CLK  0x3
#define B_PCH_PCIE_CFG_VNNREMCTL_LDVNNRE                BIT4
#define B_PCH_PCIE_CFG_VNNREMCTL_RTD3VNNRE              BIT5
#define B_PCH_PCIE_CFG_VNNREMCTL_DNPVNNRE               BIT6
#define B_PCH_PCIE_CFG_VNNREMCTL_HPVNNRE                BIT7
#define B_PCH_PCIE_CFG_VNNREMCTL_FDVNNRE                BIT8

#define R_PCH_PCIE_CFG_VNNRSNRC1                        0xC74

#define R_PCH_PCIE_AECR1G3                              0xC80
#define B_PCH_PCIE_AECR1G3_DTCGCM                       BIT14
#define B_PCH_PCIE_AECR1G3_TPSE                         BIT10
#define B_PCH_PCIE_AECR1G3_L1OFFRDYHEWT                 BIT7
#define B_PCH_PCIE_AECR1G3_L1OFFRDYHEWTEN               BIT6

#define R_PCH_PCIE_CFG_EINJCTL                          0xCA8
#define B_PCH_PCIE_CFG_EINJCTL_EINJDIS                  BIT0

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
#define B_SPX_PCR_PCD_RP1CH                   BIT3                    ///< Port 1 config hide
#define B_SPX_PCR_PCD_RP2FN                   (BIT6 | BIT5 | BIT4)    ///< Port 2 Function Number
#define B_SPX_PCR_PCD_RP2CH                   BIT7                    ///< Port 2 config hide
#define B_SPX_PCR_PCD_RP3FN                   (BIT10 | BIT9 | BIT8)   ///< Port 3 Function Number
#define B_SPX_PCR_PCD_RP3CH                   BIT11                   ///< Port 3 config hide
#define B_SPX_PCR_PCD_RP4FN                   (BIT14 | BIT13 | BIT12) ///< Port 4 Function Number
#define B_SPX_PCR_PCD_RP4CH                   BIT15                   ///< Port 4 config hide
#define S_SPX_PCR_PCD_RP_FIELD                4                       ///< 4 bits for each RP FN
#define B_SPX_PCR_PCD_P1D                     BIT16                   ///< Port 1 disable
#define B_SPX_PCR_PCD_P2D                     BIT17                   ///< Port 2 disable
#define B_SPX_PCR_PCD_P3D                     BIT18                   ///< Port 3 disable
#define B_SPX_PCR_PCD_P4D                     BIT19                   ///< Port 4 disable
#define B_SPX_PCR_PCD_SRL                     BIT31                   ///< Secured Register Lock

#define R_SPX_PCR_PCIEHBP                     0x0004                  ///< PCI Express high-speed bypass
#define B_SPX_PCR_PCIEHBP_PCIEHBPME           BIT0                    ///< PCIe HBP mode enable
#define B_SPX_PCR_PCIEHBP_PCIEGMO             (BIT2 | BIT1)           ///< PCIe gen mode override
#define B_SPX_PCR_PCIEHBP_PCIETIL0O           BIT3                    ///< PCIe transmitter-in-L0 override
#define B_SPX_PCR_PCIEHBP_PCIERIL0O           BIT4                    ///< PCIe receiver-in-L0 override
#define B_SPX_PCR_PCIEHBP_PCIELRO             BIT5                    ///< PCIe link recovery override
#define B_SPX_PCR_PCIEHBP_PCIELDO             BIT6                    ///< PCIe link down override
#define B_SPX_PCR_PCIEHBP_PCIESSM             BIT7                    ///< PCIe SKP suppression mode
#define B_SPX_PCR_PCIEHBP_PCIESST             BIT8                    ///< PCIe suppress SKP transmission
#define B_SPX_PCR_PCIEHBP_PCIEHBPPS           (BIT13 | BIT12)         ///< PCIe HBP port select
#define B_SPX_PCR_PCIEHBP_CRCSEL              (BIT15 | BIT14)         ///< CRC select
#define B_SPX_PCR_PCIEHBP_PCIEHBPCRC          0xFFFF0000              ///< PCIe HBP CRC

#define R_SPX_PCR_IMRAMBL                     0x10                    ///< IMR access memory base and limit
#define B_SPX_PCR_IMRAMBL_RS3BN               0x000000FF              ///< Bus Number for RS3
#define N_SPX_PCR_IMRAMBL_RS3BN               0
#define B_SPX_PCR_IMRAMBL_IAMB                0x000FFF00              ///< IMR access memory base, lower bits
#define N_SPX_PCR_IMRAMBL_IAMB                8
#define B_SPX_PCR_IMRAMBL_IAML                0xFFF00000              ///< IMR access memory limit, lower bits
#define N_SPX_PCR_IMRAMBL_IAML                20

#define R_SPX_PCR_IMRAMBU32                   0x14                    ///< IMR access memory base, upper bits

#define R_SPX_PCR_IMRAMLU32                   0x18                    ///< IMR access memory limit, upper bits

#define R_SPX_PCR_IMRAMLE                     0x1C                    ///< IMR access memory lock & enable
#define B_SPX_PCR_IMRAMLE_IAE1                BIT0                    ///< IMR access enable for port 1 of given controller
#define B_SPX_PCR_IMRAMLE_IAE2                BIT1                    ///< IMR access enable for port 1 of given controller
#define B_SPX_PCR_IMRAMLE_IAE3                BIT2                    ///< IMR access enable for port 1 of given controller
#define B_SPX_PCR_IMRAMLE_IAE4                BIT3                    ///< IMR access enable for port 1 of given controller
#define B_SPX_PCR_IMRAMLE_SRL                 BIT31                   ///< IMR register lock

#define R_SPX_SIP16_PCR_PCD                   0x3E00                  ///< Port configuration and disable

//
// ICC PCR (PID: ICC)
//
#define R_ICC_PCR_TMCSRCCLK                   0x1000                  ///< Timing Control SRC Clock Register
#define R_ICC_PCR_TMCSRCCLK2                  0x1004                  ///< Timing Control SRC Clock Register 2
#define R_ICC_PCR_MSKCKRQ                     0x100C                  ///< Mask Control CLKREQ
#endif
