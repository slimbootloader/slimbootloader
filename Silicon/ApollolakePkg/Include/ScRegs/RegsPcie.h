/** @file
  Register names for SC PCI-E root port devices

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_SC_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_SC_<generation_name>_" in register/bit names.
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_SC_" without <generation_name> inserted.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_PCIE_H_
#define _PCH_REGS_PCIE_H_

#define PCIE_MAX_ROOT_PORTS                            6



///
/// PCI Express Root Ports (PCIe 0: D20:F0~F1, PCIe1: D19:F0~F3)
///
#define PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_1       20
#define PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_1     0
#define PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_2     1
#define PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_2       19
#define PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_3     0
#define PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_4     1
#define PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_5     2
#define PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_6     3


#define V_PCH_PCIE_VENDOR_ID                      V_INTEL_VENDOR_ID

#define V_BXT_P_PCIE_DEVICE_ID_PORT1              0x5AD6  ///< PCI Express Root Port #1, BXT-P SC
#define V_BXT_P_PCIE_DEVICE_ID_PORT2              0x5AD7  ///< PCI Express Root Port #2, BXT-P SC
#define V_BXT_P_PCIE_DEVICE_ID_PORT3              0x5AD8  ///< PCI Express Root Port #3, BXT-P SC
#define V_BXT_P_PCIE_DEVICE_ID_PORT4              0x5AD9  ///< PCI Express Root Port #4, BXT-P SC
#define V_BXT_P_PCIE_DEVICE_ID_PORT5              0x5ADA  ///< PCI Express Root Port #5, BXT-P SC
#define V_BXT_P_PCIE_DEVICE_ID_PORT6              0x5ADB  ///< PCI Express Root Port #6, BXT-P SC
#define V_BXT_PCIE_DEVICE_ID_PORT1                0x1AD6  ///< PCI Express Root Port #1, BXT B0 SC
#define V_BXT_PCIE_DEVICE_ID_PORT2                0x1AD7  ///< PCI Express Root Port #2, BXT B0 SC
#define R_PCH_PCIE_CLIST                          0x40
#define R_PCH_PCIE_XCAP                           (R_PCH_PCIE_CLIST + R_PCIE_XCAP_OFFSET)
#define R_PCH_PCIE_DCAP                           (R_PCH_PCIE_CLIST + R_PCIE_DCAP_OFFSET)
#define R_PCH_PCIE_DCTL                           (R_PCH_PCIE_CLIST + R_PCIE_DCTL_OFFSET)
#define R_PCH_PCIE_LCAP                           (R_PCH_PCIE_CLIST + R_PCIE_LCAP_OFFSET)
#define R_PCH_PCIE_LCTL                           (R_PCH_PCIE_CLIST + R_PCIE_LCTL_OFFSET)
#define R_PCH_PCIE_LSTS                           (R_PCH_PCIE_CLIST + R_PCIE_LSTS_OFFSET)
#define R_PCH_PCIE_SLCAP                          (R_PCH_PCIE_CLIST + R_PCIE_SLCAP_OFFSET)
#define R_PCH_PCIE_SLCTL                          (R_PCH_PCIE_CLIST + R_PCIE_SLCTL_OFFSET)
#define R_PCH_PCIE_SLSTS                          (R_PCH_PCIE_CLIST + R_PCIE_SLSTS_OFFSET)
#define R_PCH_PCIE_RCTL                           (R_PCH_PCIE_CLIST + R_PCIE_RCTL_OFFSET)
#define R_PCH_PCIE_RSTS                           (R_PCH_PCIE_CLIST + R_PCIE_RSTS_OFFSET)
#define R_PCH_PCIE_DCAP2                          (R_PCH_PCIE_CLIST + R_PCIE_DCAP2_OFFSET)
#define R_PCH_PCIE_DCTL2                          (R_PCH_PCIE_CLIST + R_PCIE_DCTL2_OFFSET)
#define R_PCH_PCIE_LCTL2                          (R_PCH_PCIE_CLIST + R_PCIE_LCTL2_OFFSET)


#define R_PCIE_LCTL_LSTS                               0x50  ///< Link Control; Link Status
#define B_PCIE_LCTL_LSTS_LABS                          BIT31 ///< Link Autonomous Bandwidth Status
#define B_PCIE_LCTL_LSTS_LBMS                          BIT30 ///< Link Bandwidth Management Status
#define B_PCIE_LCTL_LSTS_DLLA                          BIT29 ///< Link Active
#define B_PCIE_LCTL_LSTS_SCC                           BIT28 ///< Slot Clock Configuration
#define B_PCIE_LCTL_LSTS_LT                            BIT27 ///< Link Training
#define B_PCIE_LCTL_LSTS_LTE                           BIT26 ///< Reserved, previously was Link Training Error
#define B_PCIE_LCTL_LSTS_NLW                           0x03F00000 ///< Negotiated Link Width
#define V_PCIE_LCTL_LSTS_NLW_1                         0x00100000
#define V_PCIE_LCTL_LSTS_NLW_2                         0x00200000
#define V_PCIE_LCTL_LSTS_NLW_4                         0x00400000
#define B_PCIE_LCTL_LSTS_LS                            0x000F0000 ///< Current Link Speed
#define B_PCIE_LCTL_LSTS_LABIE                         BIT11 ///< Link Autonomous Bandwidth Interrupt Enable
#define B_PCIE_LCTL_LSTS_LBMIE                         BIT10 ///< Link Bandwidth Management Interrupt Enable
#define B_PCIE_LCTL_LSTS_HAWD                          BIT9  ///< Hardware Autonomous Width Disable
#define B_PCIE_LCTL_LSTS_ES                            BIT7  ///< Extended Synch
#define B_PCIE_LCTL_LSTS_CCC                           BIT6  ///< Common Clock Configuration
#define B_PCIE_LCTL_LSTS_RL                            BIT5  ///< Retrain Link
#define B_PCIE_LCTL_LSTS_LD                            BIT4  ///< Link Disable
#define B_PCIE_LCTL_LSTS_RCBC                          BIT3  ///< Read Completion Boundary
#define B_PCIE_LCTL_LSTS_ASPM                          (BIT1 | BIT0) ///< Active State Link PM Control
#define V_PCIE_LCTL_LSTS_ASPM_L0S                      1     ///< L0s Entry Enabled
#define V_PCIE_LCTL_LSTS_ASPM_L1                       2     ///< L1 Entry Enable
#define V_PCIE_LCTL_LSTS_ASPM_L0S_L1                   3     ///< L0s and L1 Entry Enabled


#define R_PCIE_LCTL2_LSTS2                             0x70  ///< Link Control 2; Link Status 2
#define B_PCIE_LCTL2_LSTS2_CDL                         BIT16 ///< Current De-emphasis Level
#define B_PCIE_LCTL2_LSTS2_CD                          BIT12 ///< Compliance De-emphasis
#define B_PCIE_LCTL2_LSTS2_CSOS                        BIT11 ///< Compliance SOS
#define B_PCIE_LCTL2_LSTS2_EMC                         BIT10 ///< Enter Modified Compliance
#define B_PCIE_LCTL2_LSTS2_TM                          (BIT9 | BIT8 | BIT7) ///< Transmit Margin
#define B_PCIE_LCTL2_LSTS2_SD                          BIT6  ///< Selectable De-emphasis
#define B_PCIE_LCTL2_LSTS2_HASD                        BIT5  ///< Reserved. Hardware Autonomous Speed Disable
#define B_PCIE_LCTL2_LSTS2_EC                          BIT4  ///< Enter Compliance
#define B_PCIE_LCTL2_LSTS2_TLS                         (BIT3 | BIT2 | BIT1 | BIT0) ///< Target Link Speed


#define R_PCH_PCIE_MID                            0x80
#define S_PCH_PCIE_MID                            2
#define R_PCH_PCIE_MC                             0x82
#define S_PCH_PCIE_MC                             2
#define R_PCH_PCIE_MA                             0x84
#define S_PCH_PCIE_MA                             4
#define R_PCH_PCIE_MD                             0x88
#define S_PCH_PCIE_MD                             2

#define R_PCH_PCIE_SVCAP                          0x90
#define S_PCH_PCIE_SVCAP                          2
#define R_PCH_PCIE_SVID                           0x94
#define S_PCH_PCIE_SVID                           4

#define R_PCIE_SVID                                    0x94  ///< Subsystem Vendor IDs
#define S_PCIE_SVID                                    4
#define B_PCIE_SVID_SID                                0xFFFF0000 ///< Subsystem Identifier
#define B_PCIE_SVID_SVID                               0x0000FFFF ///< Subsystem Vendor Identifier

#define R_PCH_PCIE_PMCAP                          0xA0
#define R_PCH_PCIE_PMCS                           (R_PCH_PCIE_PMCAP + R_PCIE_PMCS_OFFST)

#define R_PCH_PCIE_MPC2                           0xD4
#define S_PCH_PCIE_MPC2                           4
#define B_PCH_PCIE_MPC2_PTNFAE                    BIT12
#define B_PCH_PCIE_MPC2_TLPF                      BIT9
#define B_PCH_PCIE_MPC2_LSTP                      BIT6
#define B_PCH_PCIE_MPC2_IEIME                     BIT5
#define B_PCH_PCIE_MPC2_ASPMCOEN                  BIT4
#define B_PCH_PCIE_MPC2_ASPMCO                    (BIT3 | BIT2)
#define V_PCH_PCIE_MPC2_ASPMCO_DISABLED           0
#define V_PCH_PCIE_MPC2_ASPMCO_L0S                1 << 2
#define V_PCH_PCIE_MPC2_ASPMCO_L1                 2 << 2
#define V_PCH_PCIE_MPC2_ASPMCO_L0S_L1             3 << 2
#define B_PCH_PCIE_MPC2_EOIFD                     BIT1
#define S_PCH_PCIE_RPDCGEN                        1
#define B_PCH_PCIE_RPDCGEN_RPSCGEN                BIT7
#define B_PCH_PCIE_RPDCGEN_PTOCGE                 BIT6
#define B_PCH_PCIE_RPDCGEN_LCLKREQEN              BIT5
#define B_PCH_PCIE_RPDCGEN_BBCLKREQEN             BIT4
#define B_PCH_PCIE_RPDCGEN_SRDLCGEN               BIT3
#define B_PCH_PCIE_RPDCGEN_SRDBCGEN               BIT2
#define B_PCH_PCIE_RPDCGEN_RPDLCGEN               BIT1
#define B_PCH_PCIE_RPDCGEN_RPDBCGEN               BIT0

//
//PCI Express Extended Capability Registers
//

#define R_PCH_PCIE_EXCAP_OFFSET                   0x100

#define R_PCH_PCIE_EX_AECH                        0x100 ///< Advanced Error Reporting Capability Header
#define V_PCH_PCIE_EX_AEC_CV                      0x1
#define R_PCH_PCIE_EX_UEM                         (R_PCH_PCIE_EX_AECH + R_PCIE_EX_UEM_OFFSEST)

#define R_PCH_PCIE_EX_ACSECH                      0x140 ///< ACS Extended Capability Header
#define V_PCH_PCIE_EX_ACS_CV                      0x1
#define R_PCH_PCIE_EX_ACSCAPR                     (R_PCH_PCIE_EX_ACSECH + R_PCIE_EX_ACSCAPR_OFFSET)

#define R_PCH_PCIE_EX_PTMECH                      0x150 ///< PTM Extended Capability Header
#define V_PCH_PCIE_EX_PTM_CV                      0x1
#define R_PCH_PCIE_EX_PTMCAPR                     (R_PCH_PCIE_EX_PTMECH + R_PCIE_EX_PTMCAPR_OFFSET)


#define R_PCH_PCIE_EX_SPEECH                      0x220 ///< Secondary PCI Express Extended Capability Header
#define V_PCH_PCIE_EX_SPEECH_CV                   0x1
#define R_PCH_PCIE_EX_LCTL3                       (R_PCH_PCIE_EX_SPEECH + R_PCIE_EX_LCTL3_OFFSET)
#define R_PCH_PCIE_EX_L01EC                       (R_PCH_PCIE_EX_SPEECH + R_PCIE_EX_L01EC_OFFSET)
#define B_PCH_PCIE_EX_L01EC_UPL1RPH               (BIT30 | BIT29 | BIT28)
#define V_PCH_PCIE_EX_L01EC_UPL1TP                0x01000000
#define B_PCH_PCIE_EX_L01EC_DPL1RPH               (BIT22 | BIT21 | BIT20)
#define V_PCH_PCIE_EX_L01EC_DPL1TP                0x00020000
#define B_PCH_PCIE_EX_L01EC_UPL0RPH               (BIT14 | BIT13 | BIT12)
#define V_PCH_PCIE_EX_L01EC_UPL0TP                0x00000000
#define B_PCH_PCIE_EX_L01EC_DPL0RPH               (BIT6 | BIT5 | BIT4)
#define V_PCH_PCIE_EX_L01EC_DPL0TP                0x00000001

#define R_PCH_PCIE_EX_L23EC                       (R_PCH_PCIE_EX_SPEECH + R_PCIE_EX_L23EC_OFFSET)
#define B_PCH_PCIE_EX_L23EC_UPL3RPH               (BIT30 | BIT29 | BIT28)
#define V_PCH_PCIE_EX_L23EC_UPL3TP                0x02000000
#define B_PCH_PCIE_EX_L23EC_DPL3RPH               (BIT22 | BIT21 | BIT20)
#define V_PCH_PCIE_EX_L23EC_DPL3TP                0x00010000
#define B_PCH_PCIE_EX_L23EC_UPL2RPH               (BIT14 | BIT13 | BIT12)
#define V_PCH_PCIE_EX_L23EC_UPL2TP                0x00000300
#define B_PCH_PCIE_EX_L23EC_DPL2RPH               (BIT6 | BIT5 | BIT4)
#define V_PCH_PCIE_EX_L23EC_DPL2TP                0x00000003

#define R_PCH_PCIE_PCIERTP1                       0x300
#define R_PCH_PCIE_PCIERTP2                       0x304

#define R_PCH_PCIE_PCIESTS1                          0x328
#define B_PCH_PCIE_PCIESTS1_LTSMSTATE                0xFF000000
#define N_PCH_PCIE_PCIESTS1_LTSMSTATE                24
#define V_PCH_PCIE_PCIESTS1_LTSMSTATE_DETRDY         0x01
#define V_PCH_PCIE_PCIESTS1_LTSMSTATE_DETRDYECINP1CG 0x0E
#define B_PCH_PCIE_PCIESTS1_LNKSTAT                  (BIT22 | BIT21 | BIT20 | BIT19)
#define N_PCH_PCIE_PCIESTS1_LNKSTAT                  19
#define V_PCH_PCIE_PCIESTS1_LNKSTAT_L0               0x7


#define R_PCH_PCIE_PTMPSDC2                       0x3A0
#define R_PCH_PCIE_PTMPSDC3                       0x3A4
#define R_PCH_PCIE_PTMECFG                        0x3B0

#define R_PCH_PCIE_PCE                            0x428
#define B_PCH_PCIE_PCE_HAE                        BIT5
#define B_PCH_PCIE_PCE_PMCRE                      BIT0

#define R_PCH_PCIE_EQCFG1                         0x450
#define S_PCH_PCIE_EQCFG1                         4
#define N_PCH_PCIE_EQCFG1_LERSMIE                 21
#define B_PCH_PCIE_EQCFG1_RTLEPCEB                BIT16
#define B_PCH_PCIE_EQCFG1_RTPCOE                  BIT15
#define B_PCH_PCIE_EQCFG1_HAED                    BIT12
#define B_PCH_PCIE_EQCFG1_EQTS2IRRC               BIT7
#define B_PCH_PCIE_EQCFG1_TUPP                    BIT1

#define R_PCH_PCIE_RTPCL1                         0x454
#define B_PCH_PCIE_RTPCL1_PCM                     BIT31
#define B_PCH_PCIE_RTPCL1_RTPRECL2PL4             0x3F000000
#define B_PCH_PCIE_RTPCL1_RTPOSTCL1PL3            0xFC0000
#define B_PCH_PCIE_RTPCL1_RTPRECL1PL2             0x3F000
#define B_PCH_PCIE_RTPCL1_RTPOSTCL0PL1            0xFC0
#define B_PCH_PCIE_RTPCL1_RTPRECL0PL0             0x3F

#define R_PCH_PCIE_RTPCL2                         0x458
#define B_PCH_PCIE_RTPCL2_RTPOSTCL3PL             0x3F000
#define B_PCH_PCIE_RTPCL2_RTPRECL3PL6             0xFC0
#define B_PCH_PCIE_RTPCL2_RTPOSTCL2PL5            0x3F

#define R_PCH_PCIE_RTPCL3                         0x45C
#define B_PCH_PCIE_RTPCL3_RTPRECL7                0x3F000000
#define B_PCH_PCIE_RTPCL3_RTPOSTCL6               0xFC0000
#define B_PCH_PCIE_RTPCL3_RTPRECL6                0x3F000
#define B_PCH_PCIE_RTPCL3_RTPOSTCL5               0xFC0
#define B_PCH_PCIE_RTPCL3_RTPRECL5PL10            0x3F

#define R_PCH_PCIE_RTPCL4                         0x460
#define B_PCH_PCIE_RTPCL4_RTPOSTCL9               0x3F000000
#define B_PCH_PCIE_RTPCL4_RTPRECL9                0xFC0000
#define B_PCH_PCIE_RTPCL4_RTPOSTCL8               0x3F000
#define B_PCH_PCIE_RTPCL4_RTPRECL8                0xFC0
#define B_PCH_PCIE_RTPCL4_RTPOSTCL7               0x3F

#define R_PCH_PCIE_HAEQ                           0x468
#define B_PCH_PCIE_HAEQ_HAPCCPI                   (BIT31 | BIT30 | BIT29 | BIT28)
#define V_PCH_PCIE_HAEQ_HAPCCPI_2PCPI             2
#define N_PCH_PCIE_HAEQ_HAPCCPI_2PCPI             28


#define R_PCH_PCIE_LTCO1                          0x470
#define B_PCH_PCIE_LTCO1_L1TCOE                   BIT25
#define B_PCH_PCIE_LTCO1_L0TCOE                   BIT24
#define B_PCH_PCIE_LTCO1_L1TPOSTCO                0xFC0000
#define B_PCH_PCIE_LTCO1_L1TPRECO                 0x3F000
#define B_PCH_PCIE_LTCO1_L0TPOSTCO                0xFC0
#define B_PCH_PCIE_LTCO1_L0TPRECO                 0x3F

#define R_PCH_PCIE_LTCO2                          0x474
#define B_PCH_PCIE_LTCO1_L3TCOE                   BIT25
#define B_PCH_PCIE_LTCO1_L2TCOE                   BIT24
#define B_PCH_PCIE_LTCO1_L3TPOSTCO                0xFC0000
#define B_PCH_PCIE_LTCO1_L3TPRECO                 0x3F000
#define B_PCH_PCIE_LTCO1_L2TPOSTCO                0xFC0
#define B_PCH_PCIE_LTCO1_L2TPRECO                 0x3F

#define R_PCH_PCIE_G3L0SCTL                       0x478
#define B_PCH_PCIE_G3L0SCTL_G3UCNFTS              0x0000FF00
#define B_PCH_PCIE_G3L0SCTL_G3CCNFTS              0x000000FF

#define R_PCH_PCIE_EQCFG2                         0x47C
#define B_PCH_PCIE_EQCFG2_NTIC                    0xFF000000
#define B_PCH_PCIE_EQCFG2_EMD                     BIT23
#define B_PCH_PCIE_EQCFG2_NTSS                    (BIT22 | BIT21 | BIT20)
#define B_PCH_PCIE_EQCFG2_PCET                    (BIT19 | BIT18 | BIT17 | BIT16)
#define B_PCH_PCIE_EQCFG2_HAPCSB                  (BIT15 | BIT14 | BIT13 | BIT12)
#define B_PCH_PCIE_EQCFG2_NTEME                   BIT11
#define B_PCH_PCIE_EQCFG2_MPEME                   BIT10
#define B_PCH_PCIE_EQCFG2_REWMETM                 (BIT9 | BIT8)
#define B_PCH_PCIE_EQCFG2_REWMET                  0xFF

//
// PCI Express Extended End Point Capability Registers
//
#define R_PCH_PCIE_LTRECH_OFFSET                  0x00
#define R_PCH_PCIE_LTRECH_CID                     0x0018
#define R_PCH_PCIE_LTRECH_MSLR_OFFSET             0x04
#define R_PCH_PCIE_LTRECH_MNSLR_OFFSET            0x06

#endif
