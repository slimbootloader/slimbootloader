/** @file
  Register names for TypeC PCIe Root Ports

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

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TC_PCIE_RP_REGS_H_
#define _TC_PCIE_RP_REGS_H_

#include <IpWrapperBitDef.h>

//
// Custom definitions
//
#define V_PCIE_FPBCAP_CID                        0x0015
#define V_FPBCAPR_FPBMEMLVECSIZES                0x1
#define V_FPBCAPR_FPBMEMHVECSIZES                0x0

#define V_TCSS_EXCAP_NCO_LISTEND                 0
#define V_TCSS_SVCAP_CID                         0x0D
#define V_TCSS_MID_CID                           0x05
#define V_TCSS_XCAP_CV                           0x02
#define V_TCSS_CLIST_CID                         0x10
#define V_TCSS_PMCAP_CID                         0x01

#define V_TCSS_DLFECH_CV                         0x1
#define V_TCSS_DLFECH_CID                        0x25
#define V_TC_PCIE_RP_CFG_SPEECH_CV               0x1
#define V_TC_PCIE_RP_CFG_SPEECH_CID              0x19
#define V_TC_PCIE_RP_CFG_PTMECH_CV               0x1
#define V_TC_PCIE_RP_CFG_PTMECH_CID              0x1F
#define V_TC_PCIE_RP_CFG_ACSECH_CV               0x1
#define V_TC_PCIE_RP_CFG_ACSECH_CID              0x0D
#define V_TC_PCIE_RP_CFG_AECH_CV                 0x1
#define V_TC_PCIE_RP_CFG_AECH_CID                0x01
//
// End of custom definitions
//

#define R_TC_PCIE_RP_PCR_PCD                  0x00003e00U      ///< Port Configuration And Disable

#define R_TC_PCIE_RP_PCR_IMRLE                0x00003e1cU      ///< IMR Lock And Enable
#define B_TC_PCIE_RP_PCR_IMRLE_SRL            BIT31            ///< Secured Register Lock

#define R_TC_PCIE_RP_PCR_SRL                  0x00003e24U      ///< Secured Register Lock
#define B_TC_PCIE_RP_PCR_SRL_SRL              BIT0             ///< Secured Register Lock

#define R_TC_PCIE_RP_CFG_CMD                  0x00000004U      ///< Device Command
#define B_TC_PCIE_RP_CFG_CMD_SEE              BIT8             ///< SERR# Enable

#define R_TC_PCIE_RP_CFG_LCTL                 0x00000050U      ///< Link Control
#define B_TC_PCIE_RP_CFG_LCTL_CCC             BIT6             ///< Common Clock Configuration

#define R_TC_PCIE_RP_CFG_LSTS                 0x00000052U      ///< Link Status

#define R_TC_PCIE_RP_CFG_SLSTS                0x0000005aU      ///< Slot Status

#define R_TC_PCIE_RP_CFG_RSTS                 0x00000060U      ///< Root Status

#define R_TC_PCIE_RP_CFG_ID                   0x00000000U      ///< Identifiers

#define R_TC_PCIE_RP_CFG_CAPP                 0x00000034U      ///< Capabilities List Pointer

#define R_TC_PCIE_RP_CFG_BCTRL                0x0000003EU
#define B_TC_PCIE_RP_CFG_BCTRL_SE             BIT1             ///< SERR# Enable
#define B_TC_PCIE_RP_CFG_BCTRL_VE             BIT3
#define B_TC_PCIE_RP_CFG_BCTRL_V16            BIT4

#define R_TC_PCIE_RP_CFG_BCTRL                0x0000003EU
#define B_TC_PCIE_RP_CFG_BCTRL_VE             BIT3
#define B_TC_PCIE_RP_CFG_BCTRL_V16            BIT4

#define R_TC_PCIE_RP_CFG_CLIST                0x00000040U      ///< Capabilities List
#define N_TC_PCIE_RP_CFG_CLIST_NEXT           8

#define R_TC_PCIE_RP_CFG_XCAP                 0x00000042U      ///< PCI Express Capabilities
#define B_TC_PCIE_RP_CFG_XCAP_CV              ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Capability Version
#define B_TC_PCIE_RP_CFG_XCAP_SI              BIT8             ///< Slot Implemented

#define R_TC_PCIE_RP_CFG_DCAP                 0x00000044U      ///< Device Capabilities
#define B_TC_PCIE_RP_CFG_DCAP_MPS             ( BIT2 | BIT1 | BIT0 ) ///< Max Payload Size Supported

#define R_TC_PCIE_RP_CFG_DCTL                 0x00000048U      ///< Device Control
#define B_TC_PCIE_RP_CFG_DCTL_CEE             BIT0             ///< Correctable Error Reporting Enable
#define B_TC_PCIE_RP_CFG_DCTL_NFE             BIT1             ///< Non-Fatal Error Reporting Enable
#define B_TC_PCIE_RP_CFG_DCTL_FEE             BIT2             ///< Fatal Error Reporting Enable
#define B_TC_PCIE_RP_CFG_DCTL_URE             BIT3             ///< Unsupported Request Reporting Enable

#define R_TC_PCIE_RP_CFG_LCAP                 0x0000004cU      ///< Link Capabilities
#define B_TC_PCIE_RP_CFG_LCAP_APMS            ( BIT11 | BIT10 ) ///< Active State Link PM Support
#define N_TC_PCIE_RP_CFG_LCAP_APMS            10
#define B_TC_PCIE_RP_CFG_LCAP_EL1             ( BIT17 | BIT16 | BIT15 ) ///< L1 Exit Latency
#define N_TC_PCIE_RP_CFG_LCAP_EL1             15

#define R_TC_PCIE_RP_CFG_SLCAP                0x00000054U      ///< Slot Capabilities
#define B_TC_PCIE_RP_CFG_SLCAP_HPS            BIT5             ///< Hot Plug Surprise
#define B_TC_PCIE_RP_CFG_SLCAP_HPC            BIT6             ///< Hot Plug Capable

#define R_TC_PCIE_RP_CFG_DCAP2                0x00000064U      ///< Device Capabilities 2
#define B_TC_PCIE_RP_CFG_DCAP2_LTRMS          BIT11            ///< LTR Mechanism Supported
#define B_TC_PCIE_RP_CFG_DCAP2_PX10BTCS       BIT16            ///< 10-Bit Tag Completer Supported
#define B_TC_PCIE_RP_CFG_DCAP2_OBFFS          ( BIT19 | BIT18 ) ///< Optimized Buffer Flush/Fill Supported
#define N_TC_PCIE_RP_CFG_DCAP2_OBFFS          18

#define R_TC_PCIE_RP_CFG_DCTL2                0x00000068U      ///< Device Control 2

#define R_TC_PCIE_RP_CFG_MID                  0x00000080U      ///< Message Signaled Interrupt Identifiers
#define N_TC_PCIE_RP_CFG_MID_NEXT             8

#define R_TC_PCIE_RP_CFG_SVCAP                0x00000090U      ///< Subsystem Vendor Capability
#define N_TC_PCIE_RP_CFG_SVCAP_NEXT           8

#define R_TC_PCIE_RP_CFG_PMCAP                0x000000a0U      ///< Power Management Capability
#define N_TC_PCIE_RP_CFG_PMCAP_NEXT           8

#define R_TC_PCIE_RP_CFG_PMC                  0x000000a2U      ///< PCI Power Management Capabilities

#define R_TC_PCIE_RP_CFG_PMCS                 0x000000a4U      ///< PCI Power Management Control
#define B_TC_PCIE_RP_CFG_PMCS_NSR             BIT3             ///< No Soft Reset

#define R_TC_PCIE_RP_CFG_FPBCAP               0x000000d0U      ///< FPB Capability Header
#define N_TC_PCIE_RP_CFG_FPBCAP_NCO           8

#define R_TC_PCIE_RP_CFG_FPBCAPR              0x000000d4U      ///< FPB Capabilities Register
#define B_TC_PCIE_RP_CFG_FPBCAPR_FPBRIDS      BIT0             ///< FPB RID Decode Mechanism Supported
#define B_TC_PCIE_RP_CFG_FPBCAPR_FPBMEMLS     BIT1             ///< FPB MEM Low Decode Mechanism Supported
#define B_TC_PCIE_RP_CFG_FPBCAPR_FPBMEMHS     BIT2             ///< FPB MEM High Decode Mechanism Supported
#define B_TC_PCIE_RP_CFG_FPBCAPR_FPBRIDVECSIZES ( BIT10 | BIT9 | BIT8 ) ///< FPB RID Vector Size Supported
#define B_TC_PCIE_RP_CFG_FPBCAPR_FPBMEMLVECSIZES ( BIT18 | BIT17 | BIT16 ) ///< FPB MEM Low Vector Size Supported
#define N_TC_PCIE_RP_CFG_FPBCAPR_FPBMEMLVECSIZES 16
#define B_TC_PCIE_RP_CFG_FPBCAPR_FPBMEMHVECSIZES ( BIT26 | BIT25 | BIT24 ) ///< FPB MEM High Vector Size Supported
#define N_TC_PCIE_RP_CFG_FPBCAPR_FPBMEMHVECSIZES 24

#define R_TC_PCIE_RP_CFG_AECH                 0x00000100U      ///< Advanced Error Extended
#define N_TC_PCIE_RP_CFG_AECH_CV              16
#define N_TC_PCIE_RP_CFG_AECH_NCO             20

#define R_TC_PCIE_RP_CFG_PTMECH               0x00000150U      ///< PTM Extended Capability Header
#define N_TC_PCIE_RP_CFG_PTMECH_CV            16
#define N_TC_PCIE_RP_CFG_PTMECH_NCO           20

#define R_TC_PCIE_RP_CFG_PTMCAPR              0x00000154U      ///< PTM Capability Register
#define B_TC_PCIE_RP_CFG_PTMCAPR_PTMRSPC      BIT1             ///< PTM Responder Capable
#define B_TC_PCIE_RP_CFG_PTMCAPR_PTMRC        BIT2             ///< PTM Root Capable

#define R_TC_PCIE_RP_CFG_PTMCTLR              0x00000158U      ///< PTM Control Register
#define B_TC_PCIE_RP_CFG_PTMCTLR_PTME         BIT0             ///< PTM Enable
#define B_TC_PCIE_RP_CFG_PTMCTLR_RS           BIT1             ///< Root Select

#define R_TC_PCIE_RP_CFG_ACSECH               0x00000220U      ///< ACS Extended Capability Header
#define N_TC_PCIE_RP_CFG_ACSECH_CV            16
#define N_TC_PCIE_RP_CFG_ACSECH_NCO           20

#define R_TC_PCIE_RP_CFG_ACSCAPR              0x00000224U      ///< ACS Capability Register
#define B_TC_PCIE_RP_CFG_ACSCAPR_U            BIT4             ///< ACS Upstream Forwarding

#define R_TC_PCIE_RP_CFG_V0CTL                0x00000294U      ///< Virtual Channel 0 Resource Control
#define B_TC_PCIE_RP_CFG_V0CTL_TVM            ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 ) ///< Transaction Class / Virtual Channel Map

#define R_TC_PCIE_RP_CFG_PCIERTP1             0x00000300U      ///< PCI Express Replay Timer Policy 1
#define B_TC_PCIE_RP_CFG_PCIERTP1_G1X4        ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Gen 1 x4
#define B_TC_PCIE_RP_CFG_PCIERTP1_G1X2        ( BIT7 | BIT6 | BIT5 | BIT4 ) ///< Gen 1 x2
#define N_TC_PCIE_RP_CFG_PCIERTP1_G1X2        4
#define B_TC_PCIE_RP_CFG_PCIERTP1_G1X1        ( BIT11 | BIT10 | BIT9 | BIT8 ) ///< Gen 1 x1
#define N_TC_PCIE_RP_CFG_PCIERTP1_G1X1        8
#define B_TC_PCIE_RP_CFG_PCIERTP1_G2X4        ( BIT15 | BIT14 | BIT13 | BIT12 ) ///< Gen 2 x4
#define N_TC_PCIE_RP_CFG_PCIERTP1_G2X4        12
#define B_TC_PCIE_RP_CFG_PCIERTP1_G2X2        ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Gen 2 x2
#define N_TC_PCIE_RP_CFG_PCIERTP1_G2X2        16
#define B_TC_PCIE_RP_CFG_PCIERTP1_G2X1        ( BIT23 | BIT22 | BIT21 | BIT20 ) ///< Gen 2 x1
#define N_TC_PCIE_RP_CFG_PCIERTP1_G2X1        20

#define R_TC_PCIE_RP_CFG_PCIERTP2             0x00000304U      ///< PCI Express Replay Timer Policy 2
#define B_TC_PCIE_RP_CFG_PCIERTP2_G3X4        ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Gen 3 x4
#define N_TC_PCIE_RP_CFG_PCIERTP2_G3X4        0
#define B_TC_PCIE_RP_CFG_PCIERTP2_G3X2        ( BIT7 | BIT6 | BIT5 | BIT4 ) ///< Gen 3 x2
#define N_TC_PCIE_RP_CFG_PCIERTP2_G3X2        4
#define B_TC_PCIE_RP_CFG_PCIERTP2_G3X1        ( BIT11 | BIT10 | BIT9 | BIT8 ) ///< Gen 3 x1
#define N_TC_PCIE_RP_CFG_PCIERTP2_G3X1        8

#define R_TC_PCIE_RP_CFG_PCIEL0SC             0x00000318U      ///< PCI Express L0s Control
#define B_TC_PCIE_RP_CFG_PCIEL0SC_G1ASL0SPL   ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< Gen1 Active State L0s Preparation Latency
#define N_TC_PCIE_RP_CFG_PCIEL0SC_G1ASL0SPL   16
#define B_TC_PCIE_RP_CFG_PCIEL0SC_G2ASL0SPL   ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Gen2 Active State L0s Preparation Latency
#define N_TC_PCIE_RP_CFG_PCIEL0SC_G2ASL0SPL   24

#define R_TC_PCIE_RP_CFG_PCIECFG2             0x00000320U      ///< PCI Express Configuration 2
#define B_TC_PCIE_RP_CFG_PCIECFG2_PMET        ( BIT21 | BIT20 ) ///< PME Timeout
#define N_TC_PCIE_RP_CFG_PCIECFG2_PMET        20
#define B_TC_PCIE_RP_CFG_PCIECFG2_CRSREN      BIT22            ///< Completion Retry Status Replay Enable
#define B_TC_PCIE_RP_CFG_PCIECFG2_CROAOE      BIT23            ///< Completion Relaxed Ordering Attribute Override Enable
#define B_TC_PCIE_RP_CFG_PCIECFG2_CROAOV      BIT24            ///< Completion Relaxed Ordering Attribute Override Value
#define B_TC_PCIE_RP_CFG_PCIECFG2_RLLG3R      BIT27            ///< Reset Link Layer In GEN3 Recovery
#define B_TC_PCIE_RP_CFG_PCIECFG2_LBWSSTE     BIT30            ///< Low Bandwidth Squelch Settling Timer Enable

#define R_TC_PCIE_RP_CFG_PCIEDBG              0x00000324U      ///< PCI Express Debug And Configuration
#define B_TC_PCIE_RP_CFG_PCIEDBG_SPCE         BIT5             ///< Squelch Propagation Control Enable
#define B_TC_PCIE_RP_CFG_PCIEDBG_LDSWQRP      BIT13            ///< Link Down SWQ Reset Policy
#define B_TC_PCIE_RP_CFG_PCIEDBG_CTONFAE      BIT14            ///< Completion Time-Out Non-Fatal Advisory Error Enable
#define B_TC_PCIE_RP_CFG_PCIEDBG_LGCLKSQEXITDBTIMERS ( BIT25 | BIT24 ) ///< Link Clock Domain Squelch Exit Debounce Timers

#define R_TC_PCIE_RP_CFG_PCIEALC              0x00000338U      ///< PCI Express Additional Link Control
#define B_TC_PCIE_RP_CFG_PCIEALC_PDSP         BIT20            ///< Present Detect State Policy
#define B_TC_PCIE_RP_CFG_PCIEALC_RTD3PDSP     BIT21            ///< RTD3 Present Detect State Policy
#define B_TC_PCIE_RP_CFG_PCIEALC_BLKDQDASD    BIT25            ///< Block Detect.Quiet -> Detect.Active Strap Default
#define B_TC_PCIE_RP_CFG_PCIEALC_BLKDQDA      BIT26            ///< Block Detect.Quiet -> Detect.Active

#define R_TC_PCIE_RP_CFG_PTMECFG              0x000003b0U      ///< PTM Extended Config
#define B_TC_PCIE_RP_CFG_PTMECFG_IOSFMADP     ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< IOSF Max Allowed Delay programming
#define N_TC_PCIE_RP_CFG_PTMECFG_IOSFMADP     0
#define B_TC_PCIE_RP_CFG_PTMECFG_PTMRNOPAD    ( BIT5 | BIT4 )  ///< PTM Request Number Of Periodic ACK DLLP
#define N_TC_PCIE_RP_CFG_PTMECFG_PTMRNOPAD    4
#define B_TC_PCIE_RP_CFG_PTMECFG_PTMRPAE      BIT6             ///< PTM Request Periodic ACK Enable
#define B_TC_PCIE_RP_CFG_PTMECFG_PLTLFF       ( BIT20 | BIT19 | BIT18 ) ///< Periodic Local TSC Link Fetch Frequency
#define N_TC_PCIE_RP_CFG_PTMECFG_PLTLFF       18

#define R_TC_PCIE_RP_CFG_LTROVR               0x00000400U      ///< Latency Tolerance Reporting Override
#define B_TC_PCIE_RP_CFG_LTROVR_LTRSLOVRV     0x000003ffU      ///< LTR Snoop Latency Override Value
#define B_TC_PCIE_RP_CFG_LTROVR_LTRSLSOVRV    ( BIT12 | BIT11 | BIT10 ) ///< LTR Snoop Latency Scale Override Value
#define N_TC_PCIE_RP_CFG_LTROVR_LTRSLSOVRV    10
#define B_TC_PCIE_RP_CFG_LTROVR_LTRSROVR      BIT15            ///< LTR Snoop Requirement Bit Override
#define B_TC_PCIE_RP_CFG_LTROVR_LTRNSLOVRV    0x03ff0000U      ///< LTR Non-Snoop Latency Override Value
#define N_TC_PCIE_RP_CFG_LTROVR_LTRNSLOVRV    16
#define B_TC_PCIE_RP_CFG_LTROVR_LTRNSLSOVRV   ( BIT28 | BIT27 | BIT26 ) ///< LTR Non-Snoop Latency Scale Override Value
#define N_TC_PCIE_RP_CFG_LTROVR_LTRNSLSOVRV   26
#define B_TC_PCIE_RP_CFG_LTROVR_LTRNSROVR     BIT31            ///< LTR Non-Snoop Requirement Bit Override

#define R_TC_PCIE_RP_CFG_LTROVR2              0x00000404U      ///< Latency Tolerance Reporting Override 2
#define B_TC_PCIE_RP_CFG_LTROVR2_LTRSOVREN    BIT0             ///< LTR Snoop Override Enable
#define B_TC_PCIE_RP_CFG_LTROVR2_LTRNSOVREN   BIT1             ///< LTR Non-Snoop Override Enable
#define B_TC_PCIE_RP_CFG_LTROVR2_LTROVRPLCY   BIT3             ///< LTR Override Policy

#define R_TC_PCIE_RP_CFG_PCIEPMECTL           0x00000420U      ///< PCIe PM Extension Control
#define B_TC_PCIE_RP_CFG_PCIEPMECTL_L1FSOE    BIT0             ///< L1 Full Squelch OFF Enable
#define B_TC_PCIE_RP_CFG_PCIEPMECTL_DLSULDLSD BIT29            ///< Disabled, Detect, L23_Rdy State,Un-Configured Lane and Down-Configured Lane Squelch Disable
#define B_TC_PCIE_RP_CFG_PCIEPMECTL_DLSULPPGE BIT30            ///< Disabled, Detect and L23_Rdy State PHY Lane Power Gating Enable
#define B_TC_PCIE_RP_CFG_PCIEPMECTL_FDPPGE    BIT31            ///< Function Disable PHY Power Gating Enable

#define R_TC_PCIE_RP_CFG_PCIEPMECTL2          0x00000424U      ///< PCIe PM Extension Control 2
#define B_TC_PCIE_RP_CFG_PCIEPMECTL2_L1SCPGE  BIT4             ///< L1 State Controller Power Gating Enable
#define B_TC_PCIE_RP_CFG_PCIEPMECTL2_DISSCPGE BIT5             ///< Disabled State Controller Power Gating Enable
#define B_TC_PCIE_RP_CFG_PCIEPMECTL2_L23RDYSCPGE BIT6             ///< L23_Rdy State Controller Power Gating Enable
#define B_TC_PCIE_RP_CFG_PCIEPMECTL2_DETSCPGE BIT7             ///< Detect State Controller Power Gating Enable
#define B_TC_PCIE_RP_CFG_PCIEPMECTL2_FDCPGE   BIT8             ///< Function Disable Controller Power Gating Enable
#define B_TC_PCIE_RP_CFG_PCIEPMECTL2_PHYCLPGE BIT11            ///< PHY Common Lane Power Gating Enable
#define B_TC_PCIE_RP_CFG_PCIEPMECTL2_CPMCSRE  BIT27            ///< Chassis PMC Save and Restore Enable

#define R_TC_PCIE_RP_CFG_PCE                  0x00000428U      ///< Power Control Enable
#define B_TC_PCIE_RP_CFG_PCE_PMCRE            BIT0             ///< PMC Request Enable
#define B_TC_PCIE_RP_CFG_PCE_SE               BIT3             ///< Sleep Enable
#define B_TC_PCIE_RP_CFG_PCE_HAE              BIT5             ///< Hardware Autonomous Enable

#define R_TC_PCIE_RP_CFG_COCTL                0x00000594U      ///< Coalescing Control
#define B_TC_PCIE_RP_CFG_COCTL_PWCE           BIT0             ///< Posted Write Chaining Enable
#define B_TC_PCIE_RP_CFG_COCTL_DDCE           BIT1             ///< Completion Chaining Enable
#define B_TC_PCIE_RP_CFG_COCTL_CT             ( BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 ) ///< Chain Timer
#define N_TC_PCIE_RP_CFG_COCTL_CT             2
#define B_TC_PCIE_RP_CFG_COCTL_CTE            BIT10            ///< Chain Timer Enable
#define B_TC_PCIE_RP_CFG_COCTL_ROAOP          BIT11            ///< Relaxed Order Attribute Override Policy
#define B_TC_PCIE_RP_CFG_COCTL_PCLM           ( BIT14 | BIT13 ) ///< Posted Chain Limit Mode
#define N_TC_PCIE_RP_CFG_COCTL_PCLM           13
#define B_TC_PCIE_RP_CFG_COCTL_NPCLM          ( BIT16 | BIT15 ) ///< Non-Posted Chain Limit Mode
#define N_TC_PCIE_RP_CFG_COCTL_NPCLM          15

#define R_TC_PCIE_RP_CFG_ADVMCTRL             0x000005bcU      ///< Advance Mode Control
#define B_TC_PCIE_RP_CFG_ADVMCTRL_PMREQBLKPGRSPT ( BIT7 | BIT6 | BIT5 ) ///< PM_REQ Block and Power Gate Response Time
#define N_TC_PCIE_RP_CFG_ADVMCTRL_PMREQBLKPGRSPT 5
#define B_TC_PCIE_RP_CFG_ADVMCTRL_G3STFER     BIT13            ///< Gen3 Short TLP Framing Error Reporting
#define B_TC_PCIE_RP_CFG_ADVMCTRL_PMREQCWC    ( BIT18 | BIT17 | BIT16 ) ///< PM_REQ Clock Wake Control
#define N_TC_PCIE_RP_CFG_ADVMCTRL_PMREQCWC    16
#define B_TC_PCIE_RP_CFG_ADVMCTRL_EIOSMASKRX  BIT19            ///< EIOS Mask Receiver Datapath
#define B_TC_PCIE_RP_CFG_ADVMCTRL_EIOSDISDS   BIT20            ///< EIOS Disable DeSkew
#define B_TC_PCIE_RP_CFG_ADVMCTRL_INRXL0CTRL  BIT22            ///< InRxL0 Control

#define R_TC_PCIE_RP_CFG_PGTHRES              0x000005c0U      ///< Power Gating Threshold
#define B_TC_PCIE_RP_CFG_PGTHRES_L1PGLTREN    BIT0             ///< L1 Power Gating LTR Enable
#define B_TC_PCIE_RP_CFG_PGTHRES_L1PGLTRTLV   0x03ff0000U      ///< L1 Power Gating LTR Threshold Latency Value
#define N_TC_PCIE_RP_CFG_PGTHRES_L1PGLTRTLV   16
#define B_TC_PCIE_RP_CFG_PGTHRES_L1PGLTRTLSV  ( BIT31 | BIT30 | BIT29 ) ///< L1 Power Gating LTR Threshold Latency Scale Value
#define N_TC_PCIE_RP_CFG_PGTHRES_L1PGLTRTLSV  29

#define R_TC_PCIE_RP_CFG_HWSNR                0x000005f0U      ///< Hardware Save And Restore
#define B_TC_PCIE_RP_CFG_HWSNR_BEPW           ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Bank Enable Pulse Width
#define B_TC_PCIE_RP_CFG_HWSNR_REPW           ( BIT7 | BIT6 | BIT5 | BIT4 ) ///< Restore Enable Pulse Width
#define N_TC_PCIE_RP_CFG_HWSNR_REPW           4
#define B_TC_PCIE_RP_CFG_HWSNR_EEH            ( BIT9 | BIT8 )  ///< Entry and Exit Hysteresis
#define N_TC_PCIE_RP_CFG_HWSNR_EEH            8

#define R_TC_PCIE_RP_CFG_PGCTRL               0x000005f4U      ///< Power Gating Control
#define B_TC_PCIE_RP_CFG_PGCTRL_PMREQBLKRSPT  ( BIT2 | BIT1 | BIT0 ) ///< PM_REQ Block Response Time
#define R_TC_PCIE_RP_CFG_ACRG3                0x000006ccU      ///< Advance Control Register Group 3
#define B_TC_PCIE_RP_CFG_ACRG3_PPTMTMUCF      ( BIT17 | BIT16 | BIT15 | BIT14 ) ///< Periodic PTM To TMU Coefficient Fetch Frequency
#define N_TC_PCIE_RP_CFG_ACRG3_PPTMTMUCF      14
#define B_TC_PCIE_RP_CFG_ACRG3_PPTMTMUCFE     BIT18            ///< Periodic PTM To TMU Coefficient Fetch Enable
#define B_TC_PCIE_RP_CFG_ACRG3_CBGM           BIT21            ///< Chain Bit Generation Mode

#define R_TC_PCIE_RP_CFG_TBTPTMBCTRL          0x000006d0U      ///< TBT PTM Block Control
#define B_TC_PCIE_RP_CFG_TBTPTMBCTRL_PTMBE    BIT0             ///< PTM Block Enable
#define B_TC_PCIE_RP_CFG_TBTPTMBCTRL_PME      BIT1             ///< Periodic Mode Enable
#define B_TC_PCIE_RP_CFG_TBTPTMBCTRL_FIE      BIT2             ///< Filter Increment Enable
#define B_TC_PCIE_RP_CFG_TBTPTMBCTRL_FS       ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 ) ///< Filter Strength
#define N_TC_PCIE_RP_CFG_TBTPTMBCTRL_FS       3
#define B_TC_PCIE_RP_CFG_TBTPTMBCTRL_CP       ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Calculation Period
#define B_TC_PCIE_RP_CFG_TBTPTMBCTRL_TMUSC    ( BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< TMU Sampling Correction
#define N_TC_PCIE_RP_CFG_TBTPTMBCTRL_TMUSC    16
#define B_TC_PCIE_RP_CFG_TBTPTMBCTRL_TL       ( BIT25 | BIT24 | BIT23 | BIT22 | BIT21 ) ///< Threshold Limit
#define N_TC_PCIE_RP_CFG_TBTPTMBCTRL_TL       21

#define R_TC_PCIE_RP_CFG_DPCCAPR              0x00000a04U      ///< DPC Capability Register
#define B_TC_PCIE_RP_CFG_DPCCAPR_RPEFDPC      BIT5             ///< RP Extensions For DPC

#define R_TC_PCIE_RP_CFG_SPEECH               0x00000a30U      ///< Secondary PCI Express Extended Capability Header
#define N_TC_PCIE_RP_CFG_SPEECH_CV            16
#define N_TC_PCIE_RP_CFG_SPEECH_NCO           20

#define R_TC_PCIE_RP_CFG_DLFECH               0x00000a90U      ///< Data Link Feature Extended Capability Header
#define N_TC_PCIE_RP_CFG_DLFECH_CV            16
#define N_TC_PCIE_RP_CFG_DLFECH_NCO           20

#define R_TC_PCIE_RP_CFG_CCFG                 0x00000ba0U      ///< Channel Configuration
#define B_TC_PCIE_RP_CFG_CCFG_UPRS            ( BIT2 | BIT1 | BIT0 ) ///< Upstream Posted Request Size
#define B_TC_PCIE_RP_CFG_CCFG_UNRS            ( BIT6 | BIT5 | BIT4 ) ///< Upstream Non-Posted Request Size
#define N_TC_PCIE_RP_CFG_CCFG_UNRS            4
#define B_TC_PCIE_RP_CFG_CCFG_UNRD            ( BIT13 | BIT12 ) ///< Upstream Non-Posted Request Delay
#define N_TC_PCIE_RP_CFG_CCFG_UNRD            12
#define B_TC_PCIE_RP_CFG_CCFG_DCGEISMA        BIT15            ///< Dynamic Clock Gating Enable on ISM Active
#define B_TC_PCIE_RP_CFG_CCFG_UPSD            BIT24            ///< Upstream Posted Split Disable

#define R_TC_PCIE_RP_CFG_MPC2                 0x00000ba4U      ///< Miscellaneous Port Configuration 2
#define B_TC_PCIE_RP_CFG_MPC2_EOIFD           BIT1             ///< EOI Forwarding Disable
#define B_TC_PCIE_RP_CFG_MPC2_LSTP            BIT6             ///< Link Speed Training Policy
#define B_TC_PCIE_RP_CFG_MPC2_PTNFAE          BIT12            ///< Poisoned TLP Non-Fatal Advisory Error Enable

#define R_TC_PCIE_RP_CFG_MPC                  0x00000ba8U      ///< Miscellaneous Port Configuration
#define B_TC_PCIE_RP_CFG_MPC_MCTPSE           BIT3             ///< MCTP Support Enable
#define B_TC_PCIE_RP_CFG_MPC_FCP              ( BIT6 | BIT5 | BIT4 ) ///< Flow Control Update Policy
#define N_TC_PCIE_RP_CFG_MPC_FCP              4
#define B_TC_PCIE_RP_CFG_MPC_CCEL             ( BIT17 | BIT16 | BIT15 ) ///< Common Clock Exit Latency
#define N_TC_PCIE_RP_CFG_MPC_CCEL             15
#define B_TC_PCIE_RP_CFG_MPC_FCDL1E           BIT21            ///< Flow Control During L1 Entry
#define B_TC_PCIE_RP_CFG_MPC_IRRCE            BIT25            ///< Invalid Receive Range Check Enable
#define B_TC_PCIE_RP_CFG_MPC_MMBNCE           BIT27            ///< MCTP Message Bus Number Check Enable

#define R_TC_PCIE_RP_CFG_SMSCS          0x00000bacU      ///< SMI And SCI Status

#define R_TC_PCIE_RP_CFG_RPDCGEN              0x00000bb1U      ///< Root Port Dynamic Clock Gate Enable
#define B_TC_PCIE_RP_CFG_RPDCGEN_RPDBCGEN     BIT0             ///< Root Port Dynamic Backbone Clock Gate Enable
#define B_TC_PCIE_RP_CFG_RPDCGEN_RPDLCGEN     BIT1             ///< Root Port Dynamic Link Clock Gate Enable
#define B_TC_PCIE_RP_CFG_RPDCGEN_SRDBCGEN     BIT2             ///< Shared Resource Dynamic Backbone Clock Gate Enable
#define B_TC_PCIE_RP_CFG_RPDCGEN_BBCLKREQEN   BIT4             ///< Backbone CLKREQ Enable
#define B_TC_PCIE_RP_CFG_RPDCGEN_LCLKREQEN    BIT5             ///< Link CLKREQ Enable
#define B_TC_PCIE_RP_CFG_RPDCGEN_PTOCGE       BIT6             ///< Partition/Trunk Oscillator Clock Gate Enable

#define R_TC_PCIE_RP_CFG_RPPGEN               0x00000bb2U      ///< Root Port Power Gating Enable
#define B_TC_PCIE_RP_CFG_RPPGEN_MDLSWPR       BIT0             ///< mod-PHY Data Lane Suspend Well Power Request
#define B_TC_PCIE_RP_CFG_RPPGEN_SEOSCGE       BIT4             ///< Sideband Endpoint Oscillator/Side Clock Gating Enable
#define B_TC_PCIE_RP_CFG_RPPGEN_PTOTOP        BIT6             ///< PME_TO Time-Out Policy

#define R_TC_PCIE_RP_CFG_PWRCTL               0x00000bb8U      ///< Power Control
#define B_TC_PCIE_RP_CFG_PWRCTL_WPDMPGEP      BIT17            ///< Wake PLL On Detect mod-PHY Power Gating Exit Policy

#define R_TC_PCIE_RP_CFG_DC                   0x00000bbcU      ///< Decode Control
#define B_TC_PCIE_RP_CFG_DC_COM               BIT13            ///< Completion Ordering Mode

#define R_TC_PCIE_RP_CFG_IPCS                 0x00000bc0U      ///< IOSF Primary Control
#define B_TC_PCIE_RP_CFG_IPCS_IMPS            ( BIT10 | BIT9 | BIT8 ) ///< IOSF_Max_Payload_Size
#define N_TC_PCIE_RP_CFG_IPCS_IMPS            8

#define R_TC_PCIE_RP_CFG_PHYCTL2              0x00000bc5U      ///< Physical Layer And AFE Control 2
#define B_TC_PCIE_RP_CFG_PHYCTL2_PXPG2PLLOFFEN BIT0             ///< PCI Express GEN2 PLL Off Enable
#define B_TC_PCIE_RP_CFG_PHYCTL2_PXPG3PLLOFFEN BIT1             ///< PCI Express GEN3 PLL Off Enable
#define B_TC_PCIE_RP_CFG_PHYCTL2_TXCFGCHGWAIT ( BIT5 | BIT4 )  ///< Transmit Configuration Change Wait Time
#define B_TC_PCIE_RP_CFG_PHYCTL2_TDFT         ( BIT7 | BIT6 )  ///< Transmit Datapath Flush Timer

#define R_TC_PCIE_RP_CFG_IOSFSBCS             0x00000bc7U      ///< IOSF Sideband Control And Status
#define B_TC_PCIE_RP_CFG_IOSFSBCS_SIID        ( BIT3 | BIT2 )  ///< IOSF Sideband Interface Idle Counter
#define B_TC_PCIE_RP_CFG_IOSFSBCS_SCPTCGE     BIT6             ///< Side Clock Partition/Trunk Clock Gating Enable

#define R_TC_PCIE_RP_CFG_STRPFUSECFG          0x00000bccU      ///< Strap And Fuse Configuration
#define B_TC_PCIE_RP_CFG_STRPFUSECFG_PXIP     ( BIT27 | BIT26 | BIT25 | BIT24 ) ///< PCI Express Interrupt Pin
#define N_TC_PCIE_RP_CFG_STRPFUSECFG_PXIP     24

#define R_TC_PCIE_RP_CFG_ACGR3S2              0x00000c50U      ///< Advance Control Register Group 3 Set 2
#define B_TC_PCIE_RP_CFG_ACGR3S2_LSTPTLS      ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< LSTP Target Link Speed
#define N_TC_PCIE_RP_CFG_ACGR3S2_LSTPTLS      0

#define R_TC_PCIE_RP_CFG_AECR1G3              0x00000c80U      ///< Advance Extended Control Register 1 Group 3
#define B_TC_PCIE_RP_CFG_AECR1G3_DTCGCM       BIT14            ///< Dynamic and Trunk Clock Gating Coupling Mode

#define R_TC_PCIE_RP_CFG_CLTRSUB              0x00000c54U      ///< Consolidated LTR Subtraction
#define B_TC_PCIE_RP_CFG_CLTRSUB_LTRSLSUBV    0x000003ffU      ///< LTR Snoop Latency Subtraction Value
#define B_TC_PCIE_RP_CFG_CLTRSUB_LTRSLSSUBV   ( BIT12 | BIT11 | BIT10 ) ///< LTR Snoop Latency Scale Subtraction Value
#define N_TC_PCIE_RP_CFG_CLTRSUB_LTRSLSSUBV   10
#define B_TC_PCIE_RP_CFG_CLTRSUB_LTRSLSUBEN   BIT15            ///< LTR Snoop Latency Subtraction Enable
#define B_TC_PCIE_RP_CFG_CLTRSUB_LTRNSLSUBV   0x03ff0000U      ///< LTR Non-Snoop Latency Subtraction Value
#define N_TC_PCIE_RP_CFG_CLTRSUB_LTRNSLSUBV   16
#define B_TC_PCIE_RP_CFG_CLTRSUB_LTRNSLSSUBV  ( BIT28 | BIT27 | BIT26 ) ///< LTR Non-Snoop Latency Scale Subtraction Value
#define N_TC_PCIE_RP_CFG_CLTRSUB_LTRNSLSSUBV  26
#define B_TC_PCIE_RP_CFG_CLTRSUB_LTRNSLSUBEN  BIT31            ///< LTR Non-Snoop Latency Subtraction Enable

#define R_TC_PCIE_RP_CFG_LPCR                 0x00000c8cU      ///< Lock Policy Control Register

#endif // _TC_PCIE_RP_REGS_H_
