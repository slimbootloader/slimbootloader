/** @file
  Register names for PCIe SIP specific registers

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

#ifndef _PCIE_SIP_RP_REGS_H_
#define _PCIE_SIP_RP_REGS_H_

#include <PcieRegs.h>


#define R_PCIE_PCR_PCD                        0x00003e00U      ///< Port Configuration And Disable
#define B_PCIE_PCR_PCD_RP0FN                  ( BIT2 | BIT1 | BIT0 ) ///< Port 1 Function Number
#define N_PCIE_PCR_PCD_RP0FN                  0
#define B_PCIE_PCR_PCD_RP0CH                  BIT3             ///< Port 1 Config Hide
#define N_PCIE_PCR_PCD_RP0CH                  3
#define B_PCIE_PCR_PCD_RP1FN                  ( BIT6 | BIT5 | BIT4 ) ///< Port 2 Function Number
#define N_PCIE_PCR_PCD_RP1FN                  4
#define B_PCIE_PCR_PCD_RP1CH                  BIT7             ///< Port 2 Config Hide
#define N_PCIE_PCR_PCD_RP1CH                  7
#define B_PCIE_PCR_PCD_RP2FN                  ( BIT10 | BIT9 | BIT8 ) ///< Port 3 Function Number
#define N_PCIE_PCR_PCD_RP2FN                  8
#define B_PCIE_PCR_PCD_RP2CH                  BIT11            ///< Port 3 Config Hide
#define N_PCIE_PCR_PCD_RP2CH                  11
#define B_PCIE_PCR_PCD_RP3FN                  ( BIT14 | BIT13 | BIT12 ) ///< Port 4 Function Number
#define N_PCIE_PCR_PCD_RP3FN                  12
#define B_PCIE_PCR_PCD_RP3CH                  BIT15            ///< Port 4 Config Hide
#define N_PCIE_PCR_PCD_RP3CH                  15
#define B_PCIE_PCR_PCD_P0D                    BIT16            ///< Port 1 Disable
#define N_PCIE_PCR_PCD_P0D                    16
#define B_PCIE_PCR_PCD_P1D                    BIT17            ///< Port 2 Disable
#define N_PCIE_PCR_PCD_P1D                    17
#define B_PCIE_PCR_PCD_P2D                    BIT18            ///< Port 3 Disable
#define N_PCIE_PCR_PCD_P2D                    18
#define B_PCIE_PCR_PCD_P3D                    BIT19            ///< Port 4 Disable
#define N_PCIE_PCR_PCD_P3D                    19
#define B_PCIE_PCR_PCD_P4D                    BIT20            ///< Port 5 Disable
#define N_PCIE_PCR_PCD_P4D                    20
#define B_PCIE_PCR_PCD_P5D                    BIT21            ///< Port 6 Disable
#define N_PCIE_PCR_PCD_P5D                    21
#define B_PCIE_PCR_PCD_P6D                    BIT22            ///< Port 7 Disable
#define N_PCIE_PCR_PCD_P6D                    22
#define B_PCIE_PCR_PCD_P7D                    BIT23            ///< Port 8 Disable
#define N_PCIE_PCR_PCD_P7D                    23
#define B_PCIE_PCR_PCD_P8D                    BIT24            ///< Port 9 Disable
#define N_PCIE_PCR_PCD_P8D                    24
#define B_PCIE_PCR_PCD_P9D                    BIT25            ///< Port 10 Disable
#define N_PCIE_PCR_PCD_P9D                    25
#define B_PCIE_PCR_PCD_P10D                   BIT26            ///< Port 11 Disable
#define N_PCIE_PCR_PCD_P10D                   26
#define B_PCIE_PCR_PCD_P11D                   BIT27            ///< Port 12 Disable
#define N_PCIE_PCR_PCD_P11D                   27
#define B_PCIE_PCR_PCD_P12D                   BIT28            ///< Port 13 Disable
#define N_PCIE_PCR_PCD_P12D                   28
#define B_PCIE_PCR_PCD_P13D                   BIT29            ///< Port 14 Disable
#define N_PCIE_PCR_PCD_P13D                   29
#define B_PCIE_PCR_PCD_P14D                   BIT30            ///< Port 15 Disable
#define N_PCIE_PCR_PCD_P14D                   30
#define B_PCIE_PCR_PCD_P15D                   BIT31            ///< Port 16 Disable
#define N_PCIE_PCR_PCD_P15D                   31

#define R_PCIE_PCR_IMRAMBL                    0x00003e10U      ///< IMR Access Memory Base And Limit
#define B_PCIE_PCR_IMRAMBL_RS3BN              ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< RS3 Bus Number
#define N_PCIE_PCR_IMRAMBL_RS3BN              0
#define B_PCIE_PCR_IMRAMBL_IAMB               0x000fff00U      ///< IMR Access Memory Base
#define N_PCIE_PCR_IMRAMBL_IAMB               8
#define B_PCIE_PCR_IMRAMBL_IAML               0xfff00000U      ///< IMR Access Memory Limit
#define N_PCIE_PCR_IMRAMBL_IAML               20

#define R_PCIE_PCR_IMRAMBU32                  0x00003e14U      ///< IMR Access Memory Base Upper 32

#define R_PCIE_PCR_IMRAMLU32                  0x00003e18U      ///< IMR Access Memory Limit Upper 32

#define R_PCIE_PCR_IMRLE                      0x00003e1cU      ///< IMR Lock And Enable
#define B_PCIE_PCR_IMRLE_IAE1                 BIT0             ///< IMR Access Enable 1
#define N_PCIE_PCR_IMRLE_IAE1                 0
#define B_PCIE_PCR_IMRLE_IAE2                 BIT1             ///< IMR Access Enable 2
#define N_PCIE_PCR_IMRLE_IAE2                 1
#define B_PCIE_PCR_IMRLE_IAE3                 BIT2             ///< IMR Access Enable 3
#define N_PCIE_PCR_IMRLE_IAE3                 2
#define B_PCIE_PCR_IMRLE_IAE4                 BIT3             ///< IMR Access Enable 4
#define N_PCIE_PCR_IMRLE_IAE4                 3
#define B_PCIE_PCR_IMRLE_IAE5                 BIT4             ///< IMR Access Enable 5
#define N_PCIE_PCR_IMRLE_IAE5                 4
#define B_PCIE_PCR_IMRLE_IAE6                 BIT5             ///< IMR Access Enable 6
#define N_PCIE_PCR_IMRLE_IAE6                 5
#define B_PCIE_PCR_IMRLE_IAE7                 BIT6             ///< IMR Access Enable 7
#define N_PCIE_PCR_IMRLE_IAE7                 6
#define B_PCIE_PCR_IMRLE_IAE8                 BIT7             ///< IMR Access Enable 8
#define N_PCIE_PCR_IMRLE_IAE8                 7
#define B_PCIE_PCR_IMRLE_IAE9                 BIT8             ///< IMR Access Enable 9
#define N_PCIE_PCR_IMRLE_IAE9                 8
#define B_PCIE_PCR_IMRLE_IAE10                BIT9             ///< IMR Access Enable 10
#define N_PCIE_PCR_IMRLE_IAE10                9
#define B_PCIE_PCR_IMRLE_IAE11                BIT10            ///< IMR Access Enable 11
#define N_PCIE_PCR_IMRLE_IAE11                10
#define B_PCIE_PCR_IMRLE_IAE12                BIT11            ///< IMR Access Enable 12
#define N_PCIE_PCR_IMRLE_IAE12                11
#define B_PCIE_PCR_IMRLE_IAE13                BIT12            ///< IMR Access Enable 13
#define N_PCIE_PCR_IMRLE_IAE13                12
#define B_PCIE_PCR_IMRLE_IAE14                BIT13            ///< IMR Access Enable 14
#define N_PCIE_PCR_IMRLE_IAE14                13
#define B_PCIE_PCR_IMRLE_IAE15                BIT14            ///< IMR Access Enable 15
#define N_PCIE_PCR_IMRLE_IAE15                14
#define B_PCIE_PCR_IMRLE_IAE16                BIT15            ///< IMR Access Enable 16
#define N_PCIE_PCR_IMRLE_IAE16                15
#define B_PCIE_PCR_IMRLE_SRL                  BIT31            ///< Secured Register Lock
#define N_PCIE_PCR_IMRLE_SRL                  31

#define R_PCIE_PCR_SRL                        0x00003e24U      ///< Secured Register Lock
#define B_PCIE_PCR_SRL_SRL                    BIT0             ///< Secured Register Lock
#define N_PCIE_PCR_SRL_SRL                    0

#define R_PCIE_PCR_BAR0P0                     0x00004e00U      ///< Base Address Register 0 Port 0
#define B_PCIE_PCR_BAR0P0_MSI                 BIT0             ///< Memory Space Indicator
#define N_PCIE_PCR_BAR0P0_MSI                 0
#define B_PCIE_PCR_BAR0P0_MMT                 ( BIT2 | BIT1 )  ///< Memory Mapping Type
#define N_PCIE_PCR_BAR0P0_MMT                 1
#define B_PCIE_PCR_BAR0P0_P                   BIT3             ///< Prefetchable
#define N_PCIE_PCR_BAR0P0_P                   3
#define B_PCIE_PCR_BAR0P0_LBA                 0xffff0000U      ///< Lower Base Address
#define N_PCIE_PCR_BAR0P0_LBA                 16

#define R_PCIE_PCR_BAR1P0                     0x00004e04U      ///< Base Address Register 1 Port 0

#define R_PCIE_MEM_IPSCE                      0x000010e0U      ///< IP Straps Config Extention
#define B_PCIE_MEM_IPSCE_RXROM                ( BIT19 | BIT18 ) ///< Receive Completion Relaxed Ordering Mode
#define N_PCIE_MEM_IPSCE_RXROM                18
#define V_PCIE_MEM_IPSCE_RXROM                0x2
#define B_PCIE_MEM_IPSCE_TXROM                ( BIT17 | BIT16 )

#define R_PCIE_MEM_FCTL                       0x00001300U      ///< Feature Control
#define B_PCIE_MEM_FCTL_CRSPSEL               BIT12            ///< Completion Timer Timeout Policy
#define N_PCIE_MEM_FCTL_CRSPSEL               12

#define R_PCIE_MEM_FCUCTL                     0x00001304U      ///< FC Update Control
#define B_PCIE_MEM_FCUCTL_FC_CP_FCM           BIT20            ///< Flow Control Completion Finite Credit Mode
#define N_PCIE_MEM_FCUCTL_FC_CP_FCM           20
#define B_PCIE_MEM_FCUCTL_FC_CP_FCM_VC1       BIT27            ///< Flow Control Completion Finite Credit Mode VC1
#define N_PCIE_MEM_FCUCTL_FC_CP_FCM_VC1       27
#define B_PCIE_MEM_FCUCTL_FC_CP_FCM_VCM       BIT28            ///< Flow Control Completion Finite Credit Mode VCM
#define N_PCIE_MEM_FCUCTL_FC_CP_FCM_VCM       28

#define R_PCIE_MEM_TXCRSTOCTL                 0x00001320U      ///< Transmit Timeout and Configuration Retry Timeout
#define B_PCIE_MEM_TXCRSTOCTL_TXNPCTODIS      BIT3             ///< Transmit Non-Posted Completion Timeout Disable
#define N_PCIE_MEM_TXCRSTOCTL_TXNPCTODIS      3
#define B_PCIE_MEM_TXCRSTOCTL_CRS_TO_DIS      BIT15
#define N_PCIE_MEM_TXCRSTOCTL_CRS_TO_DIS      15

#define R_PCIE_MEM_LLPC                       0x0000132cU      ///< Link Layer Policy Control
#define B_PCIE_MEM_LLPC_AL1NTP                BIT6             ///< ASPM L1 Nak latency Timer Policies
#define N_PCIE_MEM_LLPC_AL1NTP                6

#define R_PCIE_MEM_FCTL2                      0x00001330U      ///< Feature Control 2
#define B_PCIE_MEM_FCTL2_RPTOT                ( BIT6 | BIT5 | BIT4 ) ///< Reset Prep Timeout Timer
#define N_PCIE_MEM_FCTL2_RPTOT                4
#define B_PCIE_MEM_FCTL2_HRTCTL               ( BIT9 | BIT8 | BIT7 ) ///< Hot Reset Timer Control
#define N_PCIE_MEM_FCTL2_HRTCTL               7
#define B_PCIE_MEM_FCTL2_HPICTL               BIT10            ///< Hot Plug Interrupt Control
#define N_PCIE_MEM_FCTL2_HPICTL               10
#define B_PCIE_MEM_FCTL2_BLKNAT               BIT11
#define N_PCIE_MEM_FCTL2_BLKNAT               11
#define B_PCIE_MEM_FCTL2_RXCPPREALLOCEN       BIT27            ///< Receive Completion Preallocation Enable
#define N_PCIE_MEM_FCTL2_RXCPPREALLOCEN       27

#define R_PCIE_MEM_RPR                        0x00001334U      ///< Reset Policy
#define B_PCIE_MEM_RPR_WRM                    ( BIT3 | BIT2 )  ///< Warm Reset Mode
#define N_PCIE_MEM_RPR_WRM                    2
#define B_PCIE_MEM_RPR_CRM                    ( BIT5 | BIT4 )  ///< Cold Reset Mode
#define N_PCIE_MEM_RPR_CRM                    4
#define B_PCIE_MEM_RPR_S3SM                   ( BIT7 | BIT6 )  ///< S3 Sleep Mode
#define N_PCIE_MEM_RPR_S3SM                   6
#define B_PCIE_MEM_RPR_S4SM                   ( BIT9 | BIT8 )  ///< S4 Sleep Mode
#define N_PCIE_MEM_RPR_S4SM                   8
#define B_PCIE_MEM_RPR_S5SM                   ( BIT11 | BIT10 ) ///< S5 Sleep Mode
#define N_PCIE_MEM_RPR_S5SM                   10
#define B_PCIE_MEM_RPR_DMTO                   ( BIT13 | BIT12 ) ///< Default Mode Timeout
#define N_PCIE_MEM_RPR_DMTO                   12
#define B_PCIE_MEM_RPR_WRMTO                  ( BIT15 | BIT14 ) ///< Warm Reset Mode Timeout
#define N_PCIE_MEM_RPR_WRMTO                  14
#define B_PCIE_MEM_RPR_CRMTO                  ( BIT17 | BIT16 ) ///< Cold Reset Mode Timeout
#define N_PCIE_MEM_RPR_CRMTO                  16
#define B_PCIE_MEM_RPR_S3SMTO                 ( BIT19 | BIT18 ) ///< S3 Sleep Mode Timeout
#define N_PCIE_MEM_RPR_S3SMTO                 18
#define B_PCIE_MEM_RPR_S4SMTO                 ( BIT21 | BIT20 ) ///< S4 Sleep Mode Timeout
#define N_PCIE_MEM_RPR_S4SMTO                 20
#define B_PCIE_MEM_RPR_S5SMTO                 ( BIT23 | BIT22 ) ///< S5 Sleep Mode Timeout
#define N_PCIE_MEM_RPR_S5SMTO                 22

#define R_PCIE_MEM_DCGEN1                     0x00001350U      ///< DCG Enable 1
#define B_PCIE_MEM_DCGEN1_PXKGULDCGEN         BIT0             ///< PXKG Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXKGULDCGEN         0
#define B_PCIE_MEM_DCGEN1_PXCULDCGEN          BIT1             ///< PXC Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXCULDCGEN          1
#define B_PCIE_MEM_DCGEN1_PXLRULDCGEN         BIT8             ///< PXLR Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXLRULDCGEN         8
#define B_PCIE_MEM_DCGEN1_PXLTULDCGEN         BIT9             ///< PXLT Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXLTULDCGEN         9
#define B_PCIE_MEM_DCGEN1_PXLIULDCGEN         BIT10            ///< PXLI Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXLIULDCGEN         10
#define B_PCIE_MEM_DCGEN1_PXLSULDCGEN         BIT11            ///< PXLS Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXLSULDCGEN         11
#define B_PCIE_MEM_DCGEN1_PXCP2ULDCGEN        BIT12            ///< PXCP2 Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXCP2ULDCGEN        12
#define B_PCIE_MEM_DCGEN1_PXTRULDCGEN         BIT16            ///< PXTR Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXTRULDCGEN         16
#define B_PCIE_MEM_DCGEN1_PXTRSULDCGEN        BIT17            ///< PXTRS Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXTRSULDCGEN        17
#define B_PCIE_MEM_DCGEN1_PXTRSSULDCGEN       BIT18            ///< PXTRSS Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXTRSSULDCGEN       18
#define B_PCIE_MEM_DCGEN1_PXCP3ULDCGEN        BIT19            ///< PXCP3 Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXCP3ULDCGEN        19
#define B_PCIE_MEM_DCGEN1_PXTTULDCGEN         BIT24            ///< PXTT Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXTTULDCGEN         24
#define B_PCIE_MEM_DCGEN1_PXTTSULDCGEN        BIT25            ///< PXTTS Unit Link Clock PXTTSS DCG Enable
#define N_PCIE_MEM_DCGEN1_PXTTSULDCGEN        25
#define B_PCIE_MEM_DCGEN1_PXTTSSULDCGEN       BIT26            ///< PXTTS Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXTTSSULDCGEN       26
#define B_PCIE_MEM_DCGEN1_PXCP4ULDCGEN        BIT27            ///< PXCP4 Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN1_PXCP4ULDCGEN        27

#define R_PCIE_MEM_DCGEN2                     0x00001354U      ///< DCG Enable 2
#define B_PCIE_MEM_DCGEN2_PXPIULDCGEN         BIT0             ///< PXPI Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN2_PXPIULDCGEN         0
#define B_PCIE_MEM_DCGEN2_PXPSULDCGEN         BIT1             ///< PXPS Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN2_PXPSULDCGEN         1
#define B_PCIE_MEM_DCGEN2_PXPBULDCGEN         BIT2             ///< PXPB Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN2_PXPBULDCGEN         2
#define B_PCIE_MEM_DCGEN2_PXFIULDCGEN         BIT3             ///< PXFI Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN2_PXFIULDCGEN         3
#define B_PCIE_MEM_DCGEN2_PXFTULDCGEN         BIT4             ///< PXFT Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN2_PXFTULDCGEN         4
#define B_PCIE_MEM_DCGEN2_PXFRULDCGEN         BIT5             ///< PXFR Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN2_PXFRULDCGEN         5
#define B_PCIE_MEM_DCGEN2_PXCP5ULDCGEN        BIT6             ///< PXCP5 Unit Link Clock DCG Enable
#define N_PCIE_MEM_DCGEN2_PXCP5ULDCGEN        6

#define R_PCIE_MEM_DCGM1                      0x00001358U      ///< DCG Mode 1
#define B_PCIE_MEM_DCGM1_PXKGULDCGM           BIT0             ///< PXKG Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXKGULDCGM           0
#define B_PCIE_MEM_DCGM1_PXCULDCGM            BIT1             ///< PXC Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXCULDCGM            1
#define B_PCIE_MEM_DCGM1_PXLRULDCGM           BIT8             ///< PXLR Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXLRULDCGM           8
#define B_PCIE_MEM_DCGM1_PXLTULDCGM           BIT9             ///< PXLT Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXLTULDCGM           9
#define B_PCIE_MEM_DCGM1_PXLIULDCGM           BIT10            ///< PXLI Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXLIULDCGM           10
#define B_PCIE_MEM_DCGM1_PXLSULDCGM           BIT11            ///< PXLS Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXLSULDCGM           11
#define B_PCIE_MEM_DCGM1_PXCP2ULDCGM          BIT12            ///< PXCP2 Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXCP2ULDCGM          12
#define B_PCIE_MEM_DCGM1_PXTRULDCGM           BIT16            ///< PXTR Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXTRULDCGM           16
#define B_PCIE_MEM_DCGM1_PXTRSULDCGM          BIT17            ///< PXTRS Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXTRSULDCGM          17
#define B_PCIE_MEM_DCGM1_PXTRSSULDCGM         BIT18            ///< PXTRSS Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXTRSSULDCGM         18
#define B_PCIE_MEM_DCGM1_PXCP3ULDCGM          BIT19            ///< PXCP3 Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXCP3ULDCGM          19
#define B_PCIE_MEM_DCGM1_PXTTULDCGM           BIT24            ///< PXTT Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXTTULDCGM           24
#define B_PCIE_MEM_DCGM1_PXTTSULDCGM          BIT25            ///< PXTTS Unit Link Clock PXTTSS DCG Mode
#define N_PCIE_MEM_DCGM1_PXTTSULDCGM          25
#define B_PCIE_MEM_DCGM1_PXTTSSULDCGM         BIT26            ///< PXTTS Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXTTSSULDCGM         26
#define B_PCIE_MEM_DCGM1_PXCP4ULDCGM          BIT27            ///< PXCP4 Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM1_PXCP4ULDCGM          27

#define R_PCIE_MEM_DCGM2                      0x0000135cU      ///< DCG Mode 2
#define B_PCIE_MEM_DCGM2_PXPIULDCGM           BIT0             ///< PXPI Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM2_PXPIULDCGM           0
#define B_PCIE_MEM_DCGM2_PXPSULDCGM           BIT1             ///< PXPS Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM2_PXPSULDCGM           1
#define B_PCIE_MEM_DCGM2_PXPBULDCGM           BIT2             ///< PXPB Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM2_PXPBULDCGM           2
#define B_PCIE_MEM_DCGM2_PXFIULDCGM           BIT3             ///< PXFI Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM2_PXFIULDCGM           3
#define B_PCIE_MEM_DCGM2_PXFTULDCGM           BIT4             ///< PXFT Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM2_PXFTULDCGM           4
#define B_PCIE_MEM_DCGM2_PXFRULDCGM           BIT5             ///< PXFR Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM2_PXFRULDCGM           5
#define B_PCIE_MEM_DCGM2_PXCP5ULDCGM          BIT6             ///< PXCP5 Unit Link Clock DCG Mode
#define N_PCIE_MEM_DCGM2_PXCP5ULDCGM          6

#define R_PCIE_MEM_DCGEN3                     0x00001360U      ///< DCG Enable 3
#define B_PCIE_MEM_DCGEN3_PXEUPDCGEN          BIT0             ///< PXE Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXEUPDCGEN          0
#define B_PCIE_MEM_DCGEN3_PXBUPDCGEN          BIT1             ///< PXB Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXBUPDCGEN          1
#define B_PCIE_MEM_DCGEN3_PXCUPDCGEN          BIT2             ///< PXC Unit Prim Port Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXCUPDCGEN          2
#define B_PCIE_MEM_DCGEN3_PXCUPSRCDCGEN       BIT4             ///< PXC Unit Prim Shared Resource Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXCUPSRCDCGEN       4
#define B_PCIE_MEM_DCGEN3_PXCUPSNRDCGEN       BIT5             ///< PXC Unit Prim SnR DCG Enable
#define N_PCIE_MEM_DCGEN3_PXCUPSNRDCGEN       5
#define B_PCIE_MEM_DCGEN3_PXSRUSSNRDCGEN      BIT6             ///< PXSR Unit Side SnR DCG Enable
#define N_PCIE_MEM_DCGEN3_PXSRUSSNRDCGEN      6
#define B_PCIE_MEM_DCGEN3_PXLRUPDCGEN         BIT8             ///< PXLR Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXLRUPDCGEN         8
#define B_PCIE_MEM_DCGEN3_PXLTUPDCGEN         BIT9             ///< PXLT Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXLTUPDCGEN         9
#define B_PCIE_MEM_DCGEN3_PXLIUPDCGEN         BIT10            ///< PXLI Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXLIUPDCGEN         10
#define B_PCIE_MEM_DCGEN3_PXCP2UPDCGEN        BIT12            ///< PXCP2 Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXCP2UPDCGEN        12
#define B_PCIE_MEM_DCGEN3_PXTRUPDCGEN         BIT16            ///< PXTR Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXTRUPDCGEN         16
#define B_PCIE_MEM_DCGEN3_PXTRSUPDCGEN        BIT17            ///< PXTRS Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXTRSUPDCGEN        17
#define B_PCIE_MEM_DCGEN3_PXTRSSUPDCGEN       BIT18            ///< PXTRSS Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXTRSSUPDCGEN       18
#define B_PCIE_MEM_DCGEN3_PXTOUPDCGEN         BIT19            ///< PXTO Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXTOUPDCGEN         19
#define B_PCIE_MEM_DCGEN3_PXCP3UPDCGEN        BIT20            ///< PXCP3 Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXCP3UPDCGEN        20
#define B_PCIE_MEM_DCGEN3_PXTTUPDCGEN         BIT24            ///< PXTT Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXTTUPDCGEN         24
#define B_PCIE_MEM_DCGEN3_PXTTSUPDCGEN        BIT25            ///< PXTTS Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXTTSUPDCGEN        25
#define B_PCIE_MEM_DCGEN3_PXTTSSUPDCGEN       BIT26            ///< PXTTSS Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXTTSSUPDCGEN       26
#define B_PCIE_MEM_DCGEN3_PXCP4UPDCGEN        BIT27            ///< PXCP4 Unit Prim Clock DCG Enable
#define N_PCIE_MEM_DCGEN3_PXCP4UPDCGEN        27

#define R_PCIE_MEM_DCGM3                      0x00001368U      ///< DCG Mode 3
#define B_PCIE_MEM_DCGM3_PXEUPDCGM            BIT0             ///< PXE Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXEUPDCGM            0
#define B_PCIE_MEM_DCGM3_PXBUPDCGM            BIT1             ///< PXB Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXBUPDCGM            1
#define B_PCIE_MEM_DCGM3_PXCUPDCGM            BIT2             ///< PXC Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXCUPDCGM            2
#define B_PCIE_MEM_DCGM3_PXCUPSRCDCGM         BIT4             ///< PXC Unit Prim Shared Resource Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXCUPSRCDCGM         4
#define B_PCIE_MEM_DCGM3_PXCUPSNRDCGM         BIT5             ///< PXC Unit Prim SnR DCG Mode
#define N_PCIE_MEM_DCGM3_PXCUPSNRDCGM         5
#define B_PCIE_MEM_DCGM3_PXSRUSSNRDCGM        BIT6             ///< PXSR Unit Side SnR DCG Mode
#define N_PCIE_MEM_DCGM3_PXSRUSSNRDCGM        6
#define B_PCIE_MEM_DCGM3_PXLRUPDCGM           BIT8             ///< PXLR Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXLRUPDCGM           8
#define B_PCIE_MEM_DCGM3_PXLTUPDCGM           BIT9             ///< PXLT Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXLTUPDCGM           9
#define B_PCIE_MEM_DCGM3_PXLIUPDCGM           BIT10            ///< PXLI Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXLIUPDCGM           10
#define B_PCIE_MEM_DCGM3_PXCP2UPDCGM          BIT12            ///< PXCP2 Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXCP2UPDCGM          12
#define B_PCIE_MEM_DCGM3_PXTRUPDCGM           BIT16            ///< PXTR Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXTRUPDCGM           16
#define B_PCIE_MEM_DCGM3_PXTRSUPDCGM          BIT17            ///< PXTRS Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXTRSUPDCGM          17
#define B_PCIE_MEM_DCGM3_PXTRSSUPDCGM         BIT18            ///< PXTRSS Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXTRSSUPDCGM         18
#define B_PCIE_MEM_DCGM3_PXTOUPDCGM           BIT19            ///< PXTO Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXTOUPDCGM           19
#define B_PCIE_MEM_DCGM3_PXCP3UPDCGM          BIT20            ///< PXCP3 Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXCP3UPDCGM          20
#define B_PCIE_MEM_DCGM3_PXTTUPDCGM           BIT24            ///< PXTT Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXTTUPDCGM           24
#define B_PCIE_MEM_DCGM3_PXTTSUPDCGM          BIT25            ///< PXTTS Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXTTSUPDCGM          25
#define B_PCIE_MEM_DCGM3_PXTTSSUPDCGM         BIT26            ///< PXTTSS Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXTTSSUPDCGM         26
#define B_PCIE_MEM_DCGM3_PXCP4UPDCGM          BIT27            ///< PXCP4 Unit Prim Clock DCG Mode
#define N_PCIE_MEM_DCGM3_PXCP4UPDCGM          27

#define R_PCIE_MEM_IPCLKCTR                   0x00001370U      ///< IP Clock Control
#define B_PCIE_MEM_IPCLKCTR_PXDCGE            BIT2             ///< PXD Clock Gate Enable
#define N_PCIE_MEM_IPCLKCTR_PXDCGE            2
#define B_PCIE_MEM_IPCLKCTR_MDPCCEN           BIT3             ///< Mild DCG Prim Clock Coupling Enable
#define N_PCIE_MEM_IPCLKCTR_MDPCCEN           3

#define R_PCIE_MEM_LOCKREGCTL                 0x00001400U      ///< Lock Register Control
#define B_PCIE_MEM_LOCKREGCTL_G5EQCTL         BIT0             ///< Secured Gen 5 Equalization Register Lock
#define N_PCIE_MEM_LOCKREGCTL_G5EQCTL         0

#define R_PCIE_MEM_LPHYCP1                    0x00001410U      ///< Log Phy Control Policy 1
#define B_PCIE_MEM_LPHYCP1_RXADPSVH           BIT2             ///< Receiver Adaptation Status Violation Handling
#define N_PCIE_MEM_LPHYCP1_RXADPSVH           2
#define B_PCIE_MEM_LPHYCP1_RXADPHM            BIT3             ///< Receiver Adaptation Handshake Mapping
#define N_PCIE_MEM_LPHYCP1_RXADPHM            3
#define B_PCIE_MEM_LPHYCP1_RXEQFNEVC          BIT4             ///< Receiver Equalization Final Evaluation Control
#define N_PCIE_MEM_LPHYCP1_RXEQFNEVC          4
#define B_PCIE_MEM_LPHYCP1_PIPEMBIP           BIT6             ///< PIPE Message Bus Initialization Policy
#define N_PCIE_MEM_LPHYCP1_PIPEMBIP           6
#define B_PCIE_MEM_LPHYCP1_RXADPRECE          BIT9             ///< Receiver Adaptation Recovery Enable
#define N_PCIE_MEM_LPHYCP1_RXADPRECE          9
#define B_PCIE_MEM_LPHYCP1_BPRXSTNDBYHSRECAL  BIT12            ///< Bypass RxStandby Handshake during Recal
#define N_PCIE_MEM_LPHYCP1_BPRXSTNDBYHSRECAL  12

#define R_PCIE_MEM_UPDLPHYCP                  0x00001414U      ///< Upstream Port DMI Log Phy Control Policy
#define B_PCIE_MEM_UPDLPHYCP_UPDLLSMFLD       BIT0             ///< Upstream Port DMI LPIF+ L1 State Map For L1.Deep
#define N_PCIE_MEM_UPDLPHYCP_UPDLLSMFLD       0
#define B_PCIE_MEM_UPDLPHYCP_UPDLTCDC         BIT1             ///< Upstream Port DMI LPIF+ L1.6 TX Cmm Disable Control
#define N_PCIE_MEM_UPDLPHYCP_UPDLTCDC         1
#define B_PCIE_MEM_UPDLPHYCP_TXCMMDISLNCTL    BIT2             ///< Tx Cmm Disable Lanes Control
#define N_PCIE_MEM_UPDLPHYCP_TXCMMDISLNCTL    2

#define R_PCIE_MEM_RXL0SESQCP4                0x0000142cU      ///< RXL0s Exit Squelch Control Policy 4
#define B_PCIE_MEM_RXL0SESQCP4_G1LBWSST       ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Gen 1 Low Bandwidth Squelch Settling Timer
#define N_PCIE_MEM_RXL0SESQCP4_G1LBWSST       0
#define B_PCIE_MEM_RXL0SESQCP4_G2LBWSST       ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Gen 2 Low Bandwidth Squelch Settling Timer
#define N_PCIE_MEM_RXL0SESQCP4_G2LBWSST       8
#define B_PCIE_MEM_RXL0SESQCP4_G3LBWSST       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< Gen 3 Low Bandwidth Squelch Settling Timer
#define N_PCIE_MEM_RXL0SESQCP4_G3LBWSST       16
#define B_PCIE_MEM_RXL0SESQCP4_G4LBWSST       ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Gen 4 Low Bandwidth Squelch Settling Timer
#define N_PCIE_MEM_RXL0SESQCP4_G4LBWSST       24

#define R_PCIE_MEM_RXL0SESQCP5                0x00001430U
#define B_PCIE_MEM_RXL0SESQCP5_G5LBWSST       ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 )
#define N_PCIE_MEM_RXL0SESQCP5_G5LBWSST       0

#define R_PCIE_MEM_LPHYCP4                    0x00001434U      ///< Log Phy Control Policy 4
#define B_PCIE_MEM_LPHYCP4_LGCLKSQEXITDBTIMERS ( BIT2 | BIT1 | BIT0 ) ///< Link Clock Domain Squelch Exit Debounce Timers
#define N_PCIE_MEM_LPHYCP4_LGCLKSQEXITDBTIMERS 0
#define B_PCIE_MEM_LPHYCP4_OSCCLKSQEXITDBTIMERS ( BIT5 | BIT4 | BIT3 ) ///< OSC Clock Domain Squelch Exit Debounce Timers
#define N_PCIE_MEM_LPHYCP4_OSCCLKSQEXITDBTIMERS 3
#define B_PCIE_MEM_LPHYCP4_LGUSSP             ( BIT7 | BIT6 )  ///< LG Clock Un-Squelch Sampling Period
#define N_PCIE_MEM_LPHYCP4_LGUSSP             6
#define B_PCIE_MEM_LPHYCP4_OSUSSP             ( BIT9 | BIT8 )  ///< OSC Clock Un-Squelch Sampling Period
#define N_PCIE_MEM_LPHYCP4_OSUSSP             8
#define B_PCIE_MEM_LPHYCP4_PLLBUSDRC          BIT10            ///< PLL Bring Up Sequence During Rate Change
#define N_PCIE_MEM_LPHYCP4_PLLBUSDRC          10
#define B_PCIE_MEM_LPHYCP4_REEFRXL0SED        BIT11            ///< RxElecidle Exit Evaluation For RxL0s Entry Disable
#define N_PCIE_MEM_LPHYCP4_REEFRXL0SED        11
#define B_PCIE_MEM_LPHYCP4_DPMDNDBFE          BIT12            ///< Data Parity Mask During Non Data Block For 128b130b Encoding
#define N_PCIE_MEM_LPHYCP4_DPMDNDBFE          12
#define B_PCIE_MEM_LPHYCP4_TLSPDRS            BIT13            ///< Target Link Speed Policy During Recovery Substates
#define B_PCIE_MEM_LPHYCP4_G1AREDRL0S         BIT16            ///< Gen 1 Async RxElecidle Detector For RXL0s
#define N_PCIE_MEM_LPHYCP4_G1AREDRL0S         16
#define B_PCIE_MEM_LPHYCP4_G2AREDRL0S         BIT17            ///< Gen 2 Async RxElecidle Detector For RXL0s
#define N_PCIE_MEM_LPHYCP4_G2AREDRL0S         17
#define B_PCIE_MEM_LPHYCP4_G3AREDRL0S         BIT18            ///< Gen 3 Async RxElecidle Detector For RXL0s
#define N_PCIE_MEM_LPHYCP4_G3AREDRL0S         18
#define B_PCIE_MEM_LPHYCP4_G4AREDRL0S         BIT19            ///< Gen 4 Async RxElecidle Detector For RXL0s
#define N_PCIE_MEM_LPHYCP4_G4AREDRL0S         19
#define B_PCIE_MEM_LPHYCP4_G5AREDRL0S         BIT20            ///< Gen 5 Async RxElecidle Detector For RXL0s
#define N_PCIE_MEM_LPHYCP4_G5AREDRL0S         20
#define B_PCIE_MEM_LPHYCP4_RXL0SEG12FTSE      ( BIT25 | BIT24 ) ///< RxL0s Exit Gen 1/2 FTS Timeout Extension
#define N_PCIE_MEM_LPHYCP4_RXL0SEG12FTSE      24
#define B_PCIE_MEM_LPHYCP4_RXVLDRXL0SSP       ( BIT28 | BIT27 | BIT26 ) ///< RX Valid Trigger RXL0s Exit Suppression Period
#define N_PCIE_MEM_LPHYCP4_RXVLDRXL0SSP       26

#define R_PCIE_MEM_IORCCP1                    0x0000144cU      ///< IO Recal Control Policy 1
#define B_PCIE_MEM_IORCCP1_DISORCL12REC       BIT0             ///< Disable Offset Re-Calibration Request from L1 to Recovery
#define N_PCIE_MEM_IORCCP1_DISORCL12REC       0
#define B_PCIE_MEM_IORCCP1_DRCORRP            BIT1             ///< Disable Recovery / Configurations Offset Re-calibration Request Policy
#define N_PCIE_MEM_IORCCP1_DRCORRP            1
#define B_PCIE_MEM_IORCCP1_DISORCRODI         BIT2             ///< Disable Offset Re-Calibration Request On Downconfigure / Inactive Lanes
#define N_PCIE_MEM_IORCCP1_DISORCRODI         2
#define B_PCIE_MEM_IORCCP1_ORTS               BIT3             ///< Offset Recalibrations Trigger Status
#define N_PCIE_MEM_IORCCP1_ORTS               3
#define B_PCIE_MEM_IORCCP1_ORRPGM             ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Offset Re-Calibration Request Per Gen Mask
#define N_PCIE_MEM_IORCCP1_ORRPGM             8
#define B_PCIE_MEM_IORCCP1_G1ORRRXECC         ( BIT18 | BIT17 | BIT16 ) ///< Gen 1 Offset Re-Calibrations RX Error Collapsing Count
#define N_PCIE_MEM_IORCCP1_G1ORRRXECC         16
#define B_PCIE_MEM_IORCCP1_G2ORRRXECC         ( BIT21 | BIT20 | BIT19 ) ///< Gen 2 Offset Re-Calibrations RX Error Collapsing Count
#define N_PCIE_MEM_IORCCP1_G2ORRRXECC         19
#define B_PCIE_MEM_IORCCP1_G3ORRRXECC         ( BIT24 | BIT23 | BIT22 ) ///< Gen 3 Offset Re-Calibrations RX Error Collapsing Count
#define N_PCIE_MEM_IORCCP1_G3ORRRXECC         22
#define B_PCIE_MEM_IORCCP1_G4ORRRXECC         ( BIT27 | BIT26 | BIT25 ) ///< Gen 4 Offset Re-Calibrations RX Error Collapsing Count
#define N_PCIE_MEM_IORCCP1_G4ORRRXECC         25
#define B_PCIE_MEM_IORCCP1_G5ORRRXECC         ( BIT30 | BIT29 | BIT28 ) ///< Gen 5 Offset Re-Calibrations RX Error Collapsing Count
#define N_PCIE_MEM_IORCCP1_G5ORRRXECC         28

#define R_PCIE_MEM_PHYPG                      0x00001590U      ///< PHY Power Gating
#define B_PCIE_MEM_PHYPG_DETPHYPGE            BIT0             ///< Detect PHY Power Gating Enable
#define N_PCIE_MEM_PHYPG_DETPHYPGE            0
#define B_PCIE_MEM_PHYPG_DISPHYPGE            BIT1             ///< Disabled PHY Power Gating Enable
#define N_PCIE_MEM_PHYPG_DISPHYPGE            1
#define B_PCIE_MEM_PHYPG_L23PHYPGE            BIT2             ///< L23 PHY Power Gating Enable
#define N_PCIE_MEM_PHYPG_L23PHYPGE            2
#define B_PCIE_MEM_PHYPG_DUCFGPHYPGE          BIT3             ///< Downconfigure PHY Power Gating Enable
#define N_PCIE_MEM_PHYPG_DUCFGPHYPGE          3
#define B_PCIE_MEM_PHYPG_DLPPGP               BIT4             ///< Downconfigure Lanes Phy Power Gating Policy
#define N_PCIE_MEM_PHYPG_DLPPGP               4
#define B_PCIE_MEM_PHYPG_ULPPGP               BIT5             ///< Unconfigure Lanes Phy Power Gating Policy
#define N_PCIE_MEM_PHYPG_ULPPGP               5
#define B_PCIE_MEM_PHYPG_UNCFGPHYPGE          BIT6             ///< Unconfigure PHY Power Gating Enable
#define N_PCIE_MEM_PHYPG_UNCFGPHYPGE          6

#define R_PCIE_MEM_PIPEPDCTL                  0x00001594U      ///< PIPE Power Down Control
#define B_PCIE_MEM_PIPEPDCTL_DETNOPGPDCTL     ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Detect without Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL_DETNOPGPDCTL     0
#define B_PCIE_MEM_PIPEPDCTL_DETPGPDCTL       ( BIT7 | BIT6 | BIT5 | BIT4 ) ///< Detect with Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL_DETPGPDCTL       4
#define B_PCIE_MEM_PIPEPDCTL_L23NOPGPDCTL     ( BIT11 | BIT10 | BIT9 | BIT8 ) ///< L23 without Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL_L23NOPGPDCTL     8
#define B_PCIE_MEM_PIPEPDCTL_L23PGPDCTL       ( BIT15 | BIT14 | BIT13 | BIT12 ) ///< L23 with Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL_L23PGPDCTL       12
#define B_PCIE_MEM_PIPEPDCTL_DISNOPGPDCTL     ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Disabled without Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL_DISNOPGPDCTL     16
#define B_PCIE_MEM_PIPEPDCTL_DISPGPDCTL       ( BIT23 | BIT22 | BIT21 | BIT20 ) ///< Disabled with Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL_DISPGPDCTL       20
#define B_PCIE_MEM_PIPEPDCTL_L1PGNOPGPDCTL    ( BIT27 | BIT26 | BIT25 | BIT24 ) ///< L1 Powergateable without Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL_L1PGNOPGPDCTL    24
#define B_PCIE_MEM_PIPEPDCTL_L1PGUPGPDCTL     ( BIT31 | BIT30 | BIT29 | BIT28 ) ///< L1 Powergateable with Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL_L1PGUPGPDCTL     28

#define R_PCIE_MEM_PIPEPDCTL2                 0x00001598U      ///< PIPE Power Down Control 2
#define B_PCIE_MEM_PIPEPDCTL2_L1UPNOPGPDCTL   ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< L1 Unpowergateable without Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL2_L1UPNOPGPDCTL   0
#define B_PCIE_MEM_PIPEPDCTL2_L1D1UPUPGPDCTL  ( BIT7 | BIT6 | BIT5 | BIT4 ) ///< L1.1 Unpowergateable with Un Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL2_L1D1UPUPGPDCTL  4
#define B_PCIE_MEM_PIPEPDCTL2_L1D1PGNOPGPDCTL ( BIT11 | BIT10 | BIT9 | BIT8 ) ///< L1.1 Powergateable without Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL2_L1D1PGNOPGPDCTL 8
#define B_PCIE_MEM_PIPEPDCTL2_L1D1PGPGPDCTL   ( BIT15 | BIT14 | BIT13 | BIT12 ) ///< L1.1 Powergateable with Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL2_L1D1PGPGPDCTL   12
#define B_PCIE_MEM_PIPEPDCTL2_L1D2NOPGPDCTL   ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< L1.2 without Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL2_L1D2NOPGPDCTL   16
#define B_PCIE_MEM_PIPEPDCTL2_L1D2PGPDCTL     ( BIT23 | BIT22 | BIT21 | BIT20 ) ///< L1.2 with Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL2_L1D2PGPDCTL     20
#define B_PCIE_MEM_PIPEPDCTL2_DUCFGNOPGPDCTL  ( BIT27 | BIT26 | BIT25 | BIT24 ) ///< Down and Unconfigure without Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL2_DUCFGNOPGPDCTL  24
#define B_PCIE_MEM_PIPEPDCTL2_DUCFGPGPDCTL    ( BIT31 | BIT30 | BIT29 | BIT28 ) ///< Down and Unconfigure with Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL2_DUCFGPGPDCTL    28

#define R_PCIE_MEM_PIPEPDCTL3                 0x0000159cU      ///< PIPE Power Down Control 3
#define B_PCIE_MEM_PIPEPDCTL3_L1DLOWNOPGPDCTL ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< L1.Low without Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL3_L1DLOWNOPGPDCTL 0
#define B_PCIE_MEM_PIPEPDCTL3_L1DLOWPGPDCTL   ( BIT7 | BIT6 | BIT5 | BIT4 ) ///< L1.Low with Power Gating Power Down Control
#define N_PCIE_MEM_PIPEPDCTL3_L1DLOWPGPDCTL   4

#define R_PCIE_MEM_PIPEPDCTLEXT               0x000015a0U      ///< PIPE Power Down Control Extension
#define B_PCIE_MEM_PIPEPDCTLEXT_P2TP2TCD      BIT1             ///< P2 to P2 Transition Clock Domain
#define N_PCIE_MEM_PIPEPDCTLEXT_P2TP2TCD      1
#define B_PCIE_MEM_PIPEPDCTLEXT_P2TP2TP       BIT2             ///< P2 to P2 Transition Policy
#define N_PCIE_MEM_PIPEPDCTLEXT_P2TP2TP       2
#define B_PCIE_MEM_PIPEPDCTLEXT_P2UGTPGSM     BIT3             ///< P2 Unpower Gating To Power Gating Synchronize Mode
#define N_PCIE_MEM_PIPEPDCTLEXT_P2UGTPGSM     3
#define B_PCIE_MEM_PIPEPDCTLEXT_LSDPMRFM      ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 ) ///< Link State Deeppm Request Forwarding Mask
#define N_PCIE_MEM_PIPEPDCTLEXT_LSDPMRFM      4
#define B_PCIE_MEM_PIPEPDCTLEXT_PDDPMRFM      0xffff0000U      ///< Powerdown Deeppm Request Forwarding Mask
#define N_PCIE_MEM_PIPEPDCTLEXT_PDDPMRFM      16

#define R_PCIE_MEM_MPHYCAPCFG                 0x000015a8U      ///< MAC PHY Capability Configurations
#define B_PCIE_MEM_MPHYCAPCFG_MLSOSGSSVCC     ( BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< MACPHY Lower SKP OS Generation Supported Speeds Vector Capability Configurations
#define N_PCIE_MEM_MPHYCAPCFG_MLSOSGSSVCC     0
#define B_PCIE_MEM_MPHYCAPCFG_MLSOSRSSVCC     ( BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 ) ///< MACPHY Lower SKP OS Reception Supported Speeds Vector Capability Configurations
#define N_PCIE_MEM_MPHYCAPCFG_MLSOSRSSVCC     7

#define R_PCIE_MEM_RPDEC1                     0x00001780U      ///< Reset Prep Decode 1
#define B_PCIE_MEM_RPDEC1_RPWRERT             ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Reset Prep Warm Reset Encoding Reset Type
#define N_PCIE_MEM_RPDEC1_RPWRERT             0
#define B_PCIE_MEM_RPDEC1_RPWREPT             ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Reset Prep Warm Reset Encoding Prep Type
#define N_PCIE_MEM_RPDEC1_RPWREPT             8
#define B_PCIE_MEM_RPDEC1_RPCRERT             ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< Reset Prep Cold Reset Encoding Reset Type
#define N_PCIE_MEM_RPDEC1_RPCRERT             16
#define B_PCIE_MEM_RPDEC1_RPCREPT             ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Reset Prep Cold Reset Encoding Prep Type
#define N_PCIE_MEM_RPDEC1_RPCREPT             24

#define R_PCIE_MEM_RPDEC2                     0x00001784U      ///< Reset Prep Decode 2
#define B_PCIE_MEM_RPDEC2_RPS3ERT             ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Reset Prep S3 Encoding Reset Type
#define N_PCIE_MEM_RPDEC2_RPS3ERT             0
#define B_PCIE_MEM_RPDEC2_RPS3EPT             ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Reset Prep S3 Encoding Prep Type
#define N_PCIE_MEM_RPDEC2_RPS3EPT             8
#define B_PCIE_MEM_RPDEC2_RPS4ERT             ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< Reset Prep S4 Encoding Reset Type
#define N_PCIE_MEM_RPDEC2_RPS4ERT             16
#define B_PCIE_MEM_RPDEC2_RPS4EPT             ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Reset Prep S4 Encoding Prep Type
#define N_PCIE_MEM_RPDEC2_RPS4EPT             24

#define R_PCIE_MEM_RPDEC3                     0x00001788U      ///< Reset Prep Decode 3
#define B_PCIE_MEM_RPDEC3_RPS5ERT             ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Reset Prep S5 Encoding Reset Type
#define N_PCIE_MEM_RPDEC3_RPS5ERT             0
#define B_PCIE_MEM_RPDEC3_RPS5EPT             ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Reset Prep S5 Encoding Prep Type
#define N_PCIE_MEM_RPDEC3_RPS5EPT             8
#define B_PCIE_MEM_RPDEC3_RPDH                ( BIT18 | BIT17 | BIT16 ) ///< Reset Prep Default Handling
#define N_PCIE_MEM_RPDEC3_RPDH                16

#define R_PCIE_MEM_DECCTL                     0x00001904U      ///< Decoder Control
#define B_PCIE_MEM_DECCTL_ATSCE               BIT0             ///< ATS Check Enable
#define N_PCIE_MEM_DECCTL_ATSCE               0
#define B_PCIE_MEM_DECCTL_BUSNUMZCHK          BIT1             ///< Bus Number Zero Check
#define N_PCIE_MEM_DECCTL_BUSNUMZCHK          1
#define B_PCIE_MEM_DECCTL_MTRXLTC             BIT2             ///< Malform Receive LT Cycle
#define N_PCIE_MEM_DECCTL_MTRXLTC             2
#define B_PCIE_MEM_DECCTL_OUTBEXPCPLCHKEN     BIT3             ///< Outbound Expected Completion Check Enable
#define N_PCIE_MEM_DECCTL_OUTBEXPCPLCHKEN     3
#define B_PCIE_MEM_DECCTL_DROPCPLATNZCE       BIT4             ///< Drop Completion for AT Not Zero Check Enable
#define N_PCIE_MEM_DECCTL_DROPCPLATNZCE       4
#define B_PCIE_MEM_DECCTL_LCRXINT             BIT6             ///< Local Consume Receive Interrupt
#define N_PCIE_MEM_DECCTL_LCRXINT             6
#define B_PCIE_MEM_DECCTL_VDMATAC             BIT7             ///< VDM Address Translation Attribute Check
#define N_PCIE_MEM_DECCTL_VDMATAC             7
#define B_PCIE_MEM_DECCTL_URVDME16DW          BIT8             ///< Unsupported Request VDM exceeding 16DW
#define N_PCIE_MEM_DECCTL_URVDME16DW          8
#define B_PCIE_MEM_DECCTL_URRXUVDMINTELVID    BIT9             ///< UR Received Unknown VDM from Intel Vendor ID
#define N_PCIE_MEM_DECCTL_URRXUVDMINTELVID    9
#define B_PCIE_MEM_DECCTL_URRXUVDMUVID        BIT10            ///< UR Received Unknown VDM from Unknown Vendor ID
#define N_PCIE_MEM_DECCTL_URRXUVDMUVID        10
#define B_PCIE_MEM_DECCTL_URRXURTRCVDM        BIT11            ///< UR Received Unknown Route To Root Complex VDM
#define N_PCIE_MEM_DECCTL_URRXURTRCVDM        11
#define B_PCIE_MEM_DECCTL_URRXULTVDM          BIT12            ///< UR Received Unknown Locally Terminated VDM
#define N_PCIE_MEM_DECCTL_URRXULTVDM          12
#define B_PCIE_MEM_DECCTL_URRXURAVDM          BIT13            ///< UR Received Unknown Routed by Address VDM
#define N_PCIE_MEM_DECCTL_URRXURAVDM          13
#define B_PCIE_MEM_DECCTL_URRXURIDVDM         BIT14            ///< UR Received Unknown Routed by ID VDM
#define N_PCIE_MEM_DECCTL_URRXURIDVDM         14
#define B_PCIE_MEM_DECCTL_URRXUORVDM          BIT15            ///< UR Received Unknown Other Routing VDM
#define N_PCIE_MEM_DECCTL_URRXUORVDM          15
#define B_PCIE_MEM_DECCTL_URRXVMCTPBFRC       BIT16            ///< UR Received Valid MCTP which is Broadcast From Root Complex
#define N_PCIE_MEM_DECCTL_URRXVMCTPBFRC       16
#define B_PCIE_MEM_DECCTL_ICHKINVREQRMSGRBID  BIT17            ///< Ignore checking Invalidate Request Message to be Routed by ID
#define N_PCIE_MEM_DECCTL_ICHKINVREQRMSGRBID  17
#define B_PCIE_MEM_DECCTL_RXMCTPDECEN         BIT18            ///< Receive MCTP Decode Enable
#define N_PCIE_MEM_DECCTL_RXMCTPDECEN         18
#define B_PCIE_MEM_DECCTL_RXVDMDECE           BIT19            ///< Receive VDMs Decode Enable
#define N_PCIE_MEM_DECCTL_RXVDMDECE           19
#define B_PCIE_MEM_DECCTL_RXGPEDECEN          BIT20            ///< Receive GPE Decode Enable
#define N_PCIE_MEM_DECCTL_RXGPEDECEN          20
#define B_PCIE_MEM_DECCTL_RXSBEMCAPDECEN      BIT21            ///< Receive SBEM_CAP Decode Enable
#define N_PCIE_MEM_DECCTL_RXSBEMCAPDECEN      21
#define B_PCIE_MEM_DECCTL_RXADLEDDECEN        BIT22            ///< Receive ASRT_LED/ DSRT_LED Decode Enable
#define N_PCIE_MEM_DECCTL_RXADLEDDECEN        22
#define B_PCIE_MEM_DECCTL_RXLTRMDECH          BIT23            ///< Receive LTR Message Decode Enable Handling
#define N_PCIE_MEM_DECCTL_RXLTRMDECH          23
#define B_PCIE_MEM_DECCTL_LCRXERRMSG          BIT24            ///< Local Consume Receive Error Message
#define N_PCIE_MEM_DECCTL_LCRXERRMSG          24
#define B_PCIE_MEM_DECCTL_LCRXPTMREQ          BIT26            ///< Local Consume Receive PTM_REQ
#define N_PCIE_MEM_DECCTL_LCRXPTMREQ          26
#define B_PCIE_MEM_DECCTL_URRXUVDMRBFRC       BIT27            ///< UR Received Unknown VDM Routed by Broadcast from Root Complex
#define N_PCIE_MEM_DECCTL_URRXUVDMRBFRC       27
#define B_PCIE_MEM_DECCTL_URRXUVDMRGRTRC      BIT28            ///< UR Received Unknown VDM Routed by Gather and Route to Root Complex
#define N_PCIE_MEM_DECCTL_URRXUVDMRGRTRC      28
#define B_PCIE_MEM_DECCTL_RXMCTPBRCDECEN      BIT29            ///< Receive MCTP Broadcast From Root Complex Decode Enable
#define N_PCIE_MEM_DECCTL_RXMCTPBRCDECEN      29
#define B_PCIE_MEM_DECCTL_URRXMCTPNTCO        BIT30            ///< UR Receive MCTP Not TC0
#define N_PCIE_MEM_DECCTL_URRXMCTPNTCO        30
#define B_PCIE_MEM_DECCTL_RXIMDECEN           BIT31            ///< Receive Invalidate Message Decode Enable
#define N_PCIE_MEM_DECCTL_RXIMDECEN           31

#define R_PCIE_MEM_PIDECCTL                   0x0000190cU      ///< Primary Interface Decoder Control
#define B_PCIE_MEM_PIDECCTL_CPLBNCHK          BIT0             ///< Completion Bus Number Check
#define N_PCIE_MEM_PIDECCTL_CPLBNCHK          0

#define R_PCIE_MEM_PTMPSDC1                   0x00001920U      ///< PTM Pipe Stage Delay Configuration 1

#define R_PCIE_MEM_PTMPSDC2                   0x00001924U      ///< PTM Pipe Stage Delay Configuration 2

#define R_PCIE_MEM_PTMPSDC3                   0x00001928U      ///< PTM Pipe Stage Delay Configuration 3

#define R_PCIE_MEM_PTMPSDC4                   0x0000192cU      ///< PTM Pipe Stage Delay Configuration 4

#define R_PCIE_MEM_PTMPSDC5                   0x00001930U      ///< PTM Pipe Stage Delay Configuration 5

#define R_PCIE_MEM_PTMPSDC6                   0x00001934U      ///< PTM Pipe Stage Delay Configuration 6

#define R_PCIE_MEM_PTMPSDC9                   0x00001940U      ///< PTM Pipe Stage Delay Configuration 9

#define R_PCIE_MEM_G5L0SCTL                   0x00001e00U      ///< GEN5 L0s Control
#define B_PCIE_MEM_G5L0SCTL_G5CCNFTS          ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Gen5 Common Clock N_FTS
#define N_PCIE_MEM_G5L0SCTL_G5CCNFTS          0
#define B_PCIE_MEM_G5L0SCTL_G5UCNFTS          ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Gen5 Unique Clock N_FTS
#define N_PCIE_MEM_G5L0SCTL_G5UCNFTS          8
#define B_PCIE_MEM_G5L0SCTL_G5L0SIC           ( BIT23 | BIT22 ) ///< Gen5 L0s Entry Idle Control
#define N_PCIE_MEM_G5L0SCTL_G5L0SIC           22
#define B_PCIE_MEM_G5L0SCTL_G5ASL0SPL         ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Gen5Active State L0s Preparation Latency
#define N_PCIE_MEM_G5L0SCTL_G5ASL0SPL         24

#define R_PCIE_MEM_PX32EQCFG1                 0x00001e04U      ///< 32GT/s Equalization Configuration 1
#define B_PCIE_MEM_PX32EQCFG1_PX32GMFLNTL     BIT0             ///< 32.0 GT/s Multi-Fragment Linear and Nine-Tile List Enable
#define N_PCIE_MEM_PX32EQCFG1_PX32GMFLNTL     0
#define B_PCIE_MEM_PX32EQCFG1_PX32GMEQSMMFLNTL BIT1             ///< 32.0 GT/s Multi-EQ Search Mode, Multi fragment linear and nine tile list Enable
#define N_PCIE_MEM_PX32EQCFG1_PX32GMEQSMMFLNTL 1
#define B_PCIE_MEM_PX32EQCFG1_PX32GHAPCCPIE   BIT2             ///< 32.0 GT/s Hardware Autonomous Preset/Coefficient Count Per-Iteration Extention
#define N_PCIE_MEM_PX32EQCFG1_PX32GHAPCCPIE   2
#define B_PCIE_MEM_PX32EQCFG1_PX32GHAPCCPI    ( BIT6 | BIT5 | BIT4 | BIT3 ) ///< 32.0 GT/s Hardware Autonomous Preset/Coefficient Count Per-Iteration
#define N_PCIE_MEM_PX32EQCFG1_PX32GHAPCCPI    3
#define B_PCIE_MEM_PX32EQCFG1_PX32GEQTS2IRRC  BIT7             ///< 32.0 GT/s EQ TS2 in Recovery.ReceiverConfig Enable
#define N_PCIE_MEM_PX32EQCFG1_PX32GEQTS2IRRC  7
#define B_PCIE_MEM_PX32EQCFG1_PX32GRWTNEVE    ( BIT11 | BIT10 | BIT9 | BIT8 ) ///< 32.0 GT/s  Receiver Wait Time For New Equalization Value Evaluation
#define N_PCIE_MEM_PX32EQCFG1_PX32GRWTNEVE    8
#define B_PCIE_MEM_PX32EQCFG1_PX32GHAED       BIT12            ///< 32.0 GT/s Hardware Autonomous Equalization Done
#define N_PCIE_MEM_PX32EQCFG1_PX32GHAED       12
#define B_PCIE_MEM_PX32EQCFG1_PX32GRTPCOE     BIT15            ///< 32.0 GT/s Remote Transmitter Preset Coefficient Override Enable
#define N_PCIE_MEM_PX32EQCFG1_PX32GRTPCOE     15
#define B_PCIE_MEM_PX32EQCFG1_PX32GRTLEPCEB   BIT16            ///< 32.0 GT/s Remote Transmit Link Equalization Preset/Coefficient Evaluation Bypass
#define N_PCIE_MEM_PX32EQCFG1_PX32GRTLEPCEB   16
#define B_PCIE_MEM_PX32EQCFG1_PX32GLEP3B      BIT17            ///< 32.0 GT/s Link Equalization 3 Bypass
#define N_PCIE_MEM_PX32EQCFG1_PX32GLEP3B      17
#define B_PCIE_MEM_PX32EQCFG1_PX32GLEP23B     BIT18            ///< 32.0 GT/s Link Equalization Phase 2 and 3 Bypass
#define N_PCIE_MEM_PX32EQCFG1_PX32GLEP23B     18
#define B_PCIE_MEM_PX32EQCFG1_PX32GREIC       BIT20            ///< 32.0GT/s Reset EIEOS Interval Count
#define N_PCIE_MEM_PX32EQCFG1_PX32GREIC       20
#define B_PCIE_MEM_PX32EQCFG1_PX32GTSWLPCE    ( BIT29 | BIT28 | BIT27 ) ///< 32.0GT/s Training Sequence Wait Latency For Presets / Coefficients Evaluation
#define N_PCIE_MEM_PX32EQCFG1_PX32GTSWLPCE    27

#define R_PCIE_MEM_PX32GRTPCL1                0x00001e08U      ///< 32 GT/s Remote Transmitter Preset Coefficient List 1
#define B_PCIE_MEM_PX32GRTPCL1_RTPRECL0PL0    ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Remote Transmitter Pre-Cursor Coefficient List 0/Preset List 0
#define N_PCIE_MEM_PX32GRTPCL1_RTPRECL0PL0    0
#define B_PCIE_MEM_PX32GRTPCL1_RTPOSTCL0PL1   ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Remote Transmitter Post-Cursor Coefficient List 0/Preset List 1
#define N_PCIE_MEM_PX32GRTPCL1_RTPOSTCL0PL1   6
#define B_PCIE_MEM_PX32GRTPCL1_RTPRECL1PL2    ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Remote Transmitter Pre-Cursor Coefficient List 1/Preset List 2
#define N_PCIE_MEM_PX32GRTPCL1_RTPRECL1PL2    12
#define B_PCIE_MEM_PX32GRTPCL1_RTPOSTCL1PL3   ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Remote Transmitter Post-Cursor Coefficient List 1/Preset List 3
#define N_PCIE_MEM_PX32GRTPCL1_RTPOSTCL1PL3   18
#define B_PCIE_MEM_PX32GRTPCL1_RTPRECL2PL4    ( BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Remote Transmitter Pre-Cursor Coefficient List 2/Preset List 4
#define N_PCIE_MEM_PX32GRTPCL1_RTPRECL2PL4    24
#define B_PCIE_MEM_PX32GRTPCL1_PCM            BIT31            ///< Preset/Coefficient Mode
#define N_PCIE_MEM_PX32GRTPCL1_PCM            31

#define R_PCIE_MEM_PX32EQCFG2                 0x00001e24U      ///< 32GT/s Equalization Configuration 2
#define B_PCIE_MEM_PX32EQCFG2_REWMET          ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Receiver Eye Margin Error Threshold
#define N_PCIE_MEM_PX32EQCFG2_REWMET          0
#define B_PCIE_MEM_PX32EQCFG2_REWMETM         ( BIT9 | BIT8 )  ///< Receiver Eye Margin Error Threshold Multiplier
#define N_PCIE_MEM_PX32EQCFG2_REWMETM         8
#define B_PCIE_MEM_PX32EQCFG2_MPEME           BIT10            ///< Mid-Point Equalization Mechanism Enable
#define N_PCIE_MEM_PX32EQCFG2_MPEME           10
#define B_PCIE_MEM_PX32EQCFG2_NTEME           BIT11            ///< Nine-Tiles Equalization Mechanism Enable
#define N_PCIE_MEM_PX32EQCFG2_NTEME           11
#define B_PCIE_MEM_PX32EQCFG2_HAPCSB          ( BIT15 | BIT14 | BIT13 | BIT12 ) ///< Hardware Autonomous Preset/Coefficient Search Bound
#define N_PCIE_MEM_PX32EQCFG2_HAPCSB          12
#define B_PCIE_MEM_PX32EQCFG2_PCET            ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Preset/Coefficient Evaluation Timeout
#define N_PCIE_MEM_PX32EQCFG2_PCET            16
#define B_PCIE_MEM_PX32EQCFG2_NTSS            ( BIT22 | BIT21 | BIT20 ) ///< Nine-Tiles Step Size
#define N_PCIE_MEM_PX32EQCFG2_NTSS            20
#define B_PCIE_MEM_PX32EQCFG2_EMD             BIT23            ///< Equalization Margining Disable
#define N_PCIE_MEM_PX32EQCFG2_EMD             23
#define B_PCIE_MEM_PX32EQCFG2_NTIC            ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Nine-Tiles Iteration Count
#define N_PCIE_MEM_PX32EQCFG2_NTIC            24

#define R_PCIE_MEM_PX32GP0P1PCM               0x00001e28U      ///< 32.0 GT/s P0 And P1 Preset-Coefficient Mapping

#define R_PCIE_MEM_PX32GP1P2P3PCM             0x00001e2cU      ///< 32.0 GT/s P1, P2 And P3 Preset-Coefficient Mapping

#define R_PCIE_MEM_PX32GP3P4PCM               0x00001e30U      ///< 32.0 GT/s P3 And P4 Preset-Coefficient Mapping

#define R_PCIE_MEM_PX32GP5P6PCM               0x00001e34U      ///< 32.0 GT/s P5 And P6 Preset-Coefficient Mapping

#define R_PCIE_MEM_PX32GP6P7P8PCM             0x00001e38U      ///< 32.0 GT/s P6, P7 And P8 Preset-Coefficient Mapping

#define R_PCIE_MEM_PX32GP8P9PCM               0x00001e3cU      ///< 32.0 GT/s P8 And P9 Preset-Coefficient Mapping

#define R_PCIE_MEM_PX32GP10PCM                0x00001e40U      ///< 32.0 GT/s P10 Preset-Coefficient Mapping

#define R_PCIE_MEM_PX32GLFFS                  0x00001e44U      ///< 32.0 GT/s LF And FS

#define R_PCIE_MEM_PX32GLTCO1                 0x00001e48U      ///< 32 GT/s Local Transmitter Coefficient Override 1
#define B_PCIE_MEM_PX32GLTCO1_L0TPRECO        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 0 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO1_L0TPRECO        0
#define B_PCIE_MEM_PX32GLTCO1_L0TPOSTCO       ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 0 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO1_L0TPOSTCO       6
#define B_PCIE_MEM_PX32GLTCO1_L1TPRECO        ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 1 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO1_L1TPRECO        12
#define B_PCIE_MEM_PX32GLTCO1_L1TPOSTCO       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 1 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO1_L1TPOSTCO       18
#define B_PCIE_MEM_PX32GLTCO1_L0TCOE          BIT24            ///< Lane 0 Transmitter Coefficient Override Enable
#define N_PCIE_MEM_PX32GLTCO1_L0TCOE          24
#define B_PCIE_MEM_PX32GLTCO1_L1TCOE          BIT25            ///< Lane 1 Transmitter Coefficient Override Enable
#define N_PCIE_MEM_PX32GLTCO1_L1TCOE          25

#define R_PCIE_MEM_PX32GLTCO2                 0x00001e4CU      ///< 32 GT/s Local Transmitter Coefficient Override 2
#define B_PCIE_MEM_PX32GLTCO2_L2TPRECO        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 2 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO2_L2TPRECO        0
#define B_PCIE_MEM_PX32GLTCO2_L2TPOSTCO       ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 2 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO2_L2TPOSTCO       6
#define B_PCIE_MEM_PX32GLTCO2_L3TPRECO        ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 3 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO2_L3TPRECO        12
#define B_PCIE_MEM_PX32GLTCO2_L3TPOSTCO       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 3 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO2_L3TPOSTCO       18
#define B_PCIE_MEM_PX32GLTCO2_L2TCOE          BIT24            ///< Lane 2 Transmitter Coefficient Override Enable
#define N_PCIE_MEM_PX32GLTCO2_L2TCOE          24
#define B_PCIE_MEM_PX32GLTCO2_L3TCOE          BIT25            ///< Lane 3 Transmitter Coefficient Override Enable
#define N_PCIE_MEM_PX32GLTCO2_L3TCOE          25

#define R_PCIE_MEM_PX32GLTCO3                 0x00001e50U      ///< 32 GT/s Local Transmitter Coefficient Override 3
#define B_PCIE_MEM_PX32GLTCO3_L4TPRECO        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 4 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO3_L4TPRECO        0
#define B_PCIE_MEM_PX32GLTCO3_L4TPOSTCO       ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 4 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO3_L4TPOSTCO       6
#define B_PCIE_MEM_PX32GLTCO3_L5TPRECO        ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 5 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO3_L5TPRECO        12
#define B_PCIE_MEM_PX32GLTCO3_L5TPOSTCO       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 5 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO3_L5TPOSTCO       18
#define B_PCIE_MEM_PX32GLTCO3_L4TCOE          BIT24            ///< Lane 4 Transmitter Coefficient Override Enable
#define N_PCIE_MEM_PX32GLTCO3_L4TCOE          24
#define B_PCIE_MEM_PX32GLTCO3_L5TCOE          BIT25            ///< Lane 5 Transmitter Coefficient Override Enable
#define N_PCIE_MEM_PX32GLTCO3_L5TCOE          25

#define R_PCIE_MEM_PX32GLTCO4                 0x00001e54U      ///< 32 GT/s Local Transmitter Coefficient Override 3
#define B_PCIE_MEM_PX32GLTCO4_L6TPRECO        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 6 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO4_L6TPRECO        0
#define B_PCIE_MEM_PX32GLTCO4_L6TPOSTCO       ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 6 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO4_L6TPOSTCO       6
#define B_PCIE_MEM_PX32GLTCO4_L7TPRECO        ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 7 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO4_L7TPRECO        12
#define B_PCIE_MEM_PX32GLTCO4_L7TPOSTCO       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 7 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_MEM_PX32GLTCO4_L7TPOSTCO       18
#define B_PCIE_MEM_PX32GLTCO4_L6TCOE          BIT24            ///< Lane 6 Transmitter Coefficient Override Enable
#define N_PCIE_MEM_PX32GLTCO4_L6TCOE          24
#define B_PCIE_MEM_PX32GLTCO4_L7TCOE          BIT25            ///< Lane 7 Transmitter Coefficient Override Enable
#define N_PCIE_MEM_PX32GLTCO4_L7TCOE          25

#define R_PCIE_CFG_CMD                        0x00000004U      ///< Device Command
#define B_PCIE_CFG_CMD_SEE                    BIT8             ///< SERR Enable
#define N_PCIE_CFG_CMD_SEE                    8

#define R_PCIE_CFG_BAR0                       0x00000010U      ///< Base Address Register 0
#define B_PCIE_CFG_BAR0_MMT                   ( BIT2 | BIT1 )  ///< Memory Mapping Type
#define N_PCIE_CFG_BAR0_MMT                   1
#define V_PCIE_CFG_BAR0_MMT                   0x2
#define B_PCIE_CFG_BAR0_P                     BIT3             ///< Prefetchable Memory
#define N_PCIE_CFG_BAR0_P                     3

#define R_PCIE_CFG_CAPP                       0x00000034U      ///< Capabilities List Pointer
#define B_PCIE_CFG_CAPP_PTR                   ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Capabilities Pointer
#define N_PCIE_CFG_CAPP_PTR                   0

#define R_PCIE_CFG_BCTRL                      0x0000003eU      ///< Bridge Control
#define B_PCIE_CFG_BCTRL_SE                   BIT1             ///< SERR Enable
#define N_PCIE_CFG_BCTRL_SE                   1

#define R_PCIE_CFG_CLIST                      0x00000040U      ///< Capabilities List
#define B_PCIE_CFG_CLIST_CID                  ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Capability ID
#define N_PCIE_CFG_CLIST_CID                  0
#define B_PCIE_CFG_CLIST_NEXT                 ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Next Capability
#define N_PCIE_CFG_CLIST_NEXT                 8

#define R_PCIE_CFG_XCAP                       0x00000042U      ///< PCI Express Capabilities
#define B_PCIE_CFG_XCAP_CV                    ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Capability Version
#define N_PCIE_CFG_XCAP_CV                    0
#define B_PCIE_CFG_XCAP_DT                    ( BIT7 | BIT6 | BIT5 | BIT4 ) ///< Device / Port Type
#define N_PCIE_CFG_XCAP_DT                    4
#define B_PCIE_CFG_XCAP_SI                    BIT8             ///< Slot Implemented
#define N_PCIE_CFG_XCAP_SI                    8
#define B_PCIE_CFG_XCAP_IMN                   ( BIT13 | BIT12 | BIT11 | BIT10 | BIT9 ) ///< Interrupt Message Number
#define N_PCIE_CFG_XCAP_IMN                   9

#define R_PCIE_CFG_DCAP                       0x00000044U      ///< Device Capabilities
#define B_PCIE_CFG_DCAP_MPS                   ( BIT2 | BIT1 | BIT0 ) ///< Max Payload Size Supported
#define N_PCIE_CFG_DCAP_MPS                   0

#define R_PCIE_CFG_DCTL                       0x00000048U      ///< Device Control
#define B_PCIE_CFG_DCTL_CEE                   BIT0             ///< Correctable Error Reporting Enable
#define N_PCIE_CFG_DCTL_CEE                   0
#define B_PCIE_CFG_DCTL_NFE                   BIT1             ///< Non-Fatal Error Reporting Enable
#define N_PCIE_CFG_DCTL_NFE                   1
#define B_PCIE_CFG_DCTL_FEE                   BIT2             ///< Fatal Error Reporting Enable
#define N_PCIE_CFG_DCTL_FEE                   2
#define B_PCIE_CFG_DCTL_URE                   BIT3             ///< Unsupported Request Reporting Enable
#define N_PCIE_CFG_DCTL_URE                   3
#define B_PCIE_CFG_DCTL_ERO                   BIT4             ///< Enable Relaxed Ordering
#define N_PCIE_CFG_DCTL_ERO                   4
#define B_PCIE_CFG_DCTL_MPS                   ( BIT7 | BIT6 | BIT5 ) ///< Max Payload Size
#define N_PCIE_CFG_DCTL_MPS                   5
#define B_PCIE_CFG_DCTL_MRRS                  ( BIT14 | BIT13 | BIT12 ) ///< Max Read Request Size
#define N_PCIE_CFG_DCTL_MRRS                  12

#define R_PCIE_CFG_DSTS                       0x0000004aU      ///< Device Status

#define R_PCIE_CFG_LCAP                       0x0000004cU      ///< Link Capabilities
#define B_PCIE_CFG_LCAP_MLS                   ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Max Link Speed
#define N_PCIE_CFG_LCAP_MLS                   0
#define B_PCIE_CFG_LCAP_MLW                   ( BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 ) ///< Maximum Link Width
#define N_PCIE_CFG_LCAP_MLW                   4
#define B_PCIE_CFG_LCAP_APMS                  ( BIT11 | BIT10 ) ///< Active State Link PM Support
#define N_PCIE_CFG_LCAP_APMS                  10
#define B_PCIE_CFG_LCAP_EL0                   ( BIT14 | BIT13 | BIT12 ) ///< L0s Exit Latency
#define N_PCIE_CFG_LCAP_EL0                   12
#define B_PCIE_CFG_LCAP_EL1                   ( BIT17 | BIT16 | BIT15 ) ///< L1 Exit Latency
#define N_PCIE_CFG_LCAP_EL1                   15
#define B_PCIE_CFG_LCAP_ASPMOC                BIT22            ///< ASPM Optionality Compliance
#define N_PCIE_CFG_LCAP_ASPMOC                22
#define B_PCIE_CFG_LCAP_PN                    ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Port Number
#define N_PCIE_CFG_LCAP_PN                    24

#define R_PCIE_CFG_LCTL                       0x00000050U      ///< Link Control
#define B_PCIE_CFG_LCTL_ASPM                  ( BIT1 | BIT0 )  ///< Active State Link PM Control
#define N_PCIE_CFG_LCTL_ASPM                  0
#define B_PCIE_CFG_LCTL_LD                    BIT4             ///< Link Disable
#define N_PCIE_CFG_LCTL_LD                    4
#define B_PCIE_CFG_LCTL_RL                    BIT5             ///< Retrain Link
#define N_PCIE_CFG_LCTL_RL                    5
#define B_PCIE_CFG_LCTL_CCC                   BIT6             ///< Common Clock Configuration
#define N_PCIE_CFG_LCTL_CCC                   6

#define R_PCIE_CFG_LSTS                       0x00000052U      ///< Link Status
#define B_PCIE_CFG_LSTS_CLS                   ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Current Link Speed
#define N_PCIE_CFG_LSTS_CLS                   0
#define B_PCIE_CFG_LSTS_NLW                   ( BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 ) ///< Negotiated Link Width
#define N_PCIE_CFG_LSTS_NLW                   4
#define B_PCIE_CFG_LSTS_LT                    BIT11            ///< Link Training
#define N_PCIE_CFG_LSTS_LT                    11
#define B_PCIE_CFG_LSTS_SCC                   BIT12            ///< Slot Clock Configuration
#define N_PCIE_CFG_LSTS_SCC                   12
#define B_PCIE_CFG_LSTS_LA                    BIT13            ///< Link Active
#define N_PCIE_CFG_LSTS_LA                    13

#define R_PCIE_CFG_SLCAP                      0x00000054U      ///< Slot Capabilities

#define R_PCIE_CFG_SLCTL                      0x00000058U      ///< Slot Control

#define R_PCIE_CFG_SLSTS                      0x0000005aU      ///< Slot Status

#define R_PCIE_CFG_RCTL                       0x0000005cU      ///< Root Control
#define B_PCIE_CFG_RCTL_CRSSVE                BIT4             ///< CRS Software Visibility Enable
#define N_PCIE_CFG_RCTL_CRSSVE                4

#define R_PCIE_CFG_RSTS                       0x00000060U      ///< Root Status

#define R_PCIE_CFG_DCAP2                      0x00000064U      ///< Device Capabilities 2
#define B_PCIE_CFG_DCAP2_ARS                  BIT6             ///< Atomic Routing Supported
#define N_PCIE_CFG_DCAP2_ARS                  6
#define B_PCIE_CFG_DCAP2_AC32BS               BIT7             ///< AtomicOp Completer 32-bit Supported
#define N_PCIE_CFG_DCAP2_AC32BS               7
#define B_PCIE_CFG_DCAP2_AC64BS               BIT8             ///< AtomicOp Completer 64-bit Supported
#define N_PCIE_CFG_DCAP2_AC64BS               8
#define B_PCIE_CFG_DCAP2_AC128BS              BIT9             ///< CAS Completer 128-bit Supported
#define N_PCIE_CFG_DCAP2_AC128BS              9
#define B_PCIE_CFG_DCAP2_LTRMS                BIT11            ///< LTR Mechanism Supported
#define N_PCIE_CFG_DCAP2_LTRMS                11
#define B_PCIE_CFG_DCAP2_PX10BTCS             BIT16            ///< 10-Bit Tag Completer Supported
#define N_PCIE_CFG_DCAP2_PX10BTCS             16
#define B_PCIE_CFG_DCAP2_PX10BTRS             BIT17            ///< 10-Bit Tag Requester Supported
#define N_PCIE_CFG_DCAP2_PX10BTRS             17
#define B_PCIE_CFG_DCAP2_OBFFS                ( BIT19 | BIT18 ) ///< Optimized Buffer Flush/Fill Supported
#define N_PCIE_CFG_DCAP2_OBFFS                18
#define B_PCIE_CFG_DCAP2_EFFS                 BIT20
#define N_PCIE_CFG_DCAP2_EFFS                 20

#define R_PCIE_CFG_DCTL2                      0x00000068U      ///< Device Control 2
#define B_PCIE_CFG_DCTL2_CTD                  BIT4             ///< Completion Timeout Disable
#define N_PCIE_CFG_DCTL2_CTD                  4
#define B_PCIE_CFG_DCTL2_AFE                  BIT5             ///< ARI Forwarding Enable
#define N_PCIE_CFG_DCTL2_AFE                  5
#define B_PCIE_CFG_DCTL2_ARE                  BIT6             ///< AtomicOp Requester Enable
#define N_PCIE_CFG_DCTL2_ARE                  6
#define B_PCIE_CFG_DCTL2_AEB                  BIT7             ///< AtomicOp Egress Blocking
#define N_PCIE_CFG_DCTL2_AEB                  7
#define B_PCIE_CFG_DCTL2_LTREN                BIT10
#define B_PCIE_CFG_DCTL2_PX10BTRE             BIT12            ///< 10-Bit Tag Requester Enable
#define N_PCIE_CFG_DCTL2_PX10BTRE             12

#define R_PCIE_CFG_LCAP2                      0x0000006cU      ///< Link Capabilities 2
#define B_PCIE_CFG_LCAP2_SLSV                 ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 ) ///< Supported Link Speeds Vector

#define R_PCIE_CFG_LCTL2                      0x00000070U      ///< Link Control 2
#define B_PCIE_CFG_LCTL2_TLS                  ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Target Link Speed
#define N_PCIE_CFG_LCTL2_TLS                  0

#define R_PCIE_CFG_LSTS2                      0x00000072U      ///< Link Status 2
#define B_PCIE_CFG_LSTS2_EQP3S                BIT4             ///< Equalization Phase 3 Successful
#define N_PCIE_CFG_LSTS2_EQP3S                4

#define R_PCIE_CFG_MID                        0x00000080U      ///< Message Signaled Interrupt Identifiers
#define B_PCIE_CFG_MID_CID                    ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Capability ID
#define N_PCIE_CFG_MID_CID                    0
#define B_PCIE_CFG_MID_NEXT                   ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Next Pointer
#define N_PCIE_CFG_MID_NEXT                   8

#define R_PCIE_CFG_MC                         0x00000082U      ///< Message Signaled Interrupt Message

#define R_PCIE_CFG_MA                         0x00000084U      ///< Message Signaled Interrupt Message Address

#define R_PCIE_CFG_MD                         0x0000008cU      ///< Message Signaled Interrupt Message Data

#define R_PCIE_CFG_SVCAP                      0x00000098U      ///< Subsystem Vendor Capability
#define B_PCIE_CFG_SVCAP_CID                  ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Capability Identifier
#define N_PCIE_CFG_SVCAP_CID                  0
#define B_PCIE_CFG_SVCAP_NEXT                 ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Next Capability
#define N_PCIE_CFG_SVCAP_NEXT                 8

#define R_PCIE_CFG_SVID                       0x0000009cU      ///< Subsystem Vendor IDs

#define R_PCIE_CFG_PMCAP                      0x000000a0U      ///< Power Management Capability
#define B_PCIE_CFG_PMCAP_CID                  ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Capability Identifier
#define N_PCIE_CFG_PMCAP_CID                  0
#define B_PCIE_CFG_PMCAP_NEXT                 ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Next Capability
#define N_PCIE_CFG_PMCAP_NEXT                 8

#define R_PCIE_CFG_PMCS                       0x000000a4U      ///< PCI Power Management Control

#define R_PCIE_CFG_CCFG                       0x000000d0U      ///< Channel Configuration
#define B_PCIE_CFG_CCFG_UPRS                  ( BIT2 | BIT1 | BIT0 ) ///< Upstream Posted Request Size
#define N_PCIE_CFG_CCFG_UPRS                  0
#define B_PCIE_CFG_CCFG_UNRS                  ( BIT6 | BIT5 | BIT4 ) ///< Upstream Non-Posted Request Size
#define N_PCIE_CFG_CCFG_UNRS                  4
#define B_PCIE_CFG_CCFG_UNRD                  ( BIT13 | BIT12 ) ///< Upstream Non-Posted Request Delay
#define N_PCIE_CFG_CCFG_UNRD                  12
#define B_PCIE_CFG_CCFG_DCGEISMA              BIT15            ///< Dynamic Clock Gating Enable on ISM Active
#define N_PCIE_CFG_CCFG_DCGEISMA              15
#define B_PCIE_CFG_CCFG_NPAP                  BIT16            ///< Non-Posted Pre-Allocation Policy
#define N_PCIE_CFG_CCFG_NPAP                  16
#define B_PCIE_CFG_CCFG_UNSD                  BIT23            ///< Upstream Non-Posted Split Disable
#define N_PCIE_CFG_CCFG_UNSD                  23
#define B_PCIE_CFG_CCFG_UPSD                  BIT24            ///< Upstream Posted Split Disable
#define N_PCIE_CFG_CCFG_UPSD                  24
#define B_PCIE_CFG_CCFG_UPMWPD                BIT25            ///< Upstream Posted Memory Write Peer Disable
#define N_PCIE_CFG_CCFG_UPMWPD                25
#define B_PCIE_CFG_CCFG_UMRPD                 BIT26            ///< Upstream Memory Read Peer Disable
#define N_PCIE_CFG_CCFG_UMRPD                 26
#define B_PCIE_CFG_CCFG_IORE                  BIT30            ///< I/O Receive Enabled
#define N_PCIE_CFG_CCFG_IORE                  30
#define B_PCIE_CFG_CCFG_CRE                   BIT31            ///< Config Receive Enabled
#define N_PCIE_CFG_CCFG_CRE                   31

#define R_PCIE_CFG_MPC2                       0x000000d4U      ///< Miscellaneous Port Configuration 2
#define B_PCIE_CFG_MPC2_EOIFD                 BIT1             ///< EOI Forwarding Disable
#define N_PCIE_CFG_MPC2_EOIFD                 1
#define B_PCIE_CFG_MPC2_ASPMCO                ( BIT3 | BIT2 )  ///< ASPM Control Override
#define N_PCIE_CFG_MPC2_ASPMCO                2
#define B_PCIE_CFG_MPC2_ASPMCOEN              BIT4             ///< ASPM Control Override Enable
#define N_PCIE_CFG_MPC2_ASPMCOEN              4
#define B_PCIE_CFG_MPC2_IEIME                 BIT5             ///< Infer Electrical Idle Mechanism Enable
#define N_PCIE_CFG_MPC2_IEIME                 5
#define B_PCIE_CFG_MPC2_LSTP                  BIT6             ///< Link Speed Training Policy
#define N_PCIE_CFG_MPC2_LSTP                  6
#define B_PCIE_CFG_MPC2_CAM                   BIT8             ///< Credit Allocated Update Mode
#define N_PCIE_CFG_MPC2_CAM                   8
#define B_PCIE_CFG_MPC2_TLPF                  BIT9             ///< Transaction Layer Packet Fast Transmit Mode
#define N_PCIE_CFG_MPC2_TLPF                  9
#define B_PCIE_CFG_MPC2_PTNFAE                BIT12            ///< Poisoned TLP Non-Fatal Advisory Error Enable
#define N_PCIE_CFG_MPC2_PTNFAE                12
#define B_PCIE_CFG_MPC2_ORCE                  ( BIT15 | BIT14 ) ///< Offset Re-Calibration Enable
#define N_PCIE_CFG_MPC2_ORCE                  14
#define B_PCIE_CFG_MPC2_DISPLLEWL1SE          BIT16            ///< Disable PLL Early Wake on L1 Substate Exit
#define N_PCIE_CFG_MPC2_DISPLLEWL1SE          16
#define B_PCIE_CFG_MPC2_GEN3PLLC              BIT20            ///< Reserved
#define N_PCIE_CFG_MPC2_GEN3PLLC              20
#define B_PCIE_CFG_MPC2_GEN2PLLC              BIT21            ///< Reserved
#define N_PCIE_CFG_MPC2_GEN2PLLC              21
#define B_PCIE_CFG_MPC2_RUD                   BIT23            ///< Reserved
#define N_PCIE_CFG_MPC2_RUD                   23
#define B_PCIE_CFG_MPC2_PLLWAIT               ( BIT26 | BIT25 | BIT24 ) ///< PLL Wait
#define N_PCIE_CFG_MPC2_PLLWAIT               24
#define B_PCIE_CFG_MPC2_L1SSESE               BIT30            ///< L1 Substate Exit SCI Enable
#define N_PCIE_CFG_MPC2_L1SSESE               30

#define R_PCIE_CFG_MPC                        0x000000d8U      ///< Miscellaneous Port Configuration
#define B_PCIE_CFG_MPC_PMME                   BIT0             ///< Power Management SMI Enable
#define N_PCIE_CFG_MPC_PMME                   0
#define B_PCIE_CFG_MPC_HPME                   BIT1             ///< Hot Plug SMI Enable
#define N_PCIE_CFG_MPC_HPME                   1
#define B_PCIE_CFG_MPC_BT                     BIT2             ///< Bridge Type
#define N_PCIE_CFG_MPC_BT                     2
#define B_PCIE_CFG_MPC_MCTPSE                 BIT3             ///< MCTP Support Enable
#define N_PCIE_CFG_MPC_MCTPSE                 3
#define B_PCIE_CFG_MPC_FCP                    ( BIT6 | BIT5 | BIT4 ) ///< Flow Control Update Policy
#define N_PCIE_CFG_MPC_FCP                    4
#define B_PCIE_CFG_MPC_PCIESD                 ( BIT14 | BIT13 | BIT12 ) ///< PCIe Speed Disable
#define N_PCIE_CFG_MPC_PCIESD                 12
#define B_PCIE_CFG_MPC_CCEL                   ( BIT17 | BIT16 | BIT15 ) ///< Common Clock Exit Latency
#define N_PCIE_CFG_MPC_CCEL                   15
#define B_PCIE_CFG_MPC_UCEL                   ( BIT20 | BIT19 | BIT18 ) ///< Unique Clock Exit Latency
#define N_PCIE_CFG_MPC_UCEL                   18
#define B_PCIE_CFG_MPC_FCDL1E                 BIT21            ///< Flow Control During L1 Entry
#define N_PCIE_CFG_MPC_FCDL1E                 21
#define B_PCIE_CFG_MPC_BMERCE                 BIT24            ///< BME Receive Check Enable
#define N_PCIE_CFG_MPC_BMERCE                 24
#define B_PCIE_CFG_MPC_IRRCE                  BIT25            ///< Invalid Receive Range Check Enable
#define N_PCIE_CFG_MPC_IRRCE                  25
#define B_PCIE_CFG_MPC_P8XDE                  BIT26            ///< Port8xh Decode Enable
#define N_PCIE_CFG_MPC_P8XDE                  26
#define B_PCIE_CFG_MPC_MMBNCE                 BIT27            ///< MCTP Message Bus Number Check Enable
#define N_PCIE_CFG_MPC_MMBNCE                 27
#define B_PCIE_CFG_MPC_HPCE                   BIT30            ///< Hot Plug SCI Enable
#define N_PCIE_CFG_MPC_HPCE                   30
#define B_PCIE_CFG_MPC_PMCE                   BIT31            ///< Power Management SCI Enable
#define N_PCIE_CFG_MPC_PMCE                   31

#define R_PCIE_CFG_SMSCS                      0x000000dcU      ///< SMI And SCI Status
#define B_PCIE_CFG_SMSCS_PMMS                 BIT0             ///< Power Management SMI Status
#define N_PCIE_CFG_SMSCS_PMMS                 0
#define B_PCIE_CFG_SMSCS_HPPDM                BIT1             ///< Hot Plug Presence Detect SMI Status
#define N_PCIE_CFG_SMSCS_HPPDM                1
#define B_PCIE_CFG_SMSCS_HPLAS                BIT4             ///< Hot Plug Link Active State Changed SMI Status
#define N_PCIE_CFG_SMSCS_HPLAS                4
#define B_PCIE_CFG_SMSCS_LERSMIS              BIT5             ///< Link Equalization Request SMI Status
#define N_PCIE_CFG_SMSCS_LERSMIS              5
#define B_PCIE_CFG_SMSCS_PMCS                 BIT31            ///< Power Management SCI Status
#define N_PCIE_CFG_SMSCS_PMCS                 31

#define R_PCIE_CFG_SPR                        0x000000e0U      ///< Scratch Pad Register

#define R_PCIE_CFG_RPDCGEN                    0x000000e1U      ///< Root Port Dynamic Clock Gate Enable
#define B_PCIE_CFG_RPDCGEN_RPDBCGEN           BIT0             ///< Root Port Dynamic Backbone Clock Gate Enable
#define N_PCIE_CFG_RPDCGEN_RPDBCGEN           0
#define B_PCIE_CFG_RPDCGEN_RPDLCGEN           BIT1             ///< Root Port Dynamic Link Clock Gate Enable
#define N_PCIE_CFG_RPDCGEN_RPDLCGEN           1
#define B_PCIE_CFG_RPDCGEN_SRDBCGEN           BIT2             ///< Shared Resource Dynamic Backbone Clock Gate Enable
#define N_PCIE_CFG_RPDCGEN_SRDBCGEN           2
#define B_PCIE_CFG_RPDCGEN_BBCLKREQEN         BIT4             ///< Backbone CLKREQ Enable
#define N_PCIE_CFG_RPDCGEN_BBCLKREQEN         4
#define B_PCIE_CFG_RPDCGEN_LCLKREQEN          BIT5             ///< Link CLKREQ Enable
#define N_PCIE_CFG_RPDCGEN_LCLKREQEN          5
#define B_PCIE_CFG_RPDCGEN_PTOCGE             BIT6             ///< Partition/Trunk Oscillator Clock Gate Enable
#define N_PCIE_CFG_RPDCGEN_PTOCGE             6

#define R_PCIE_CFG_RPPGEN                     0x000000e2U      ///< Root Port Power Gating Enable
#define B_PCIE_CFG_RPPGEN_MDLSWPR             BIT0             ///< mod-PHY Data Lane Suspend Well Power Request
#define N_PCIE_CFG_RPPGEN_MDLSWPR             0
#define B_PCIE_CFG_RPPGEN_L23R2DT             BIT3             ///< L23_Rdy to Detect Transition
#define N_PCIE_CFG_RPPGEN_L23R2DT             3
#define B_PCIE_CFG_RPPGEN_SEOSCGE             BIT4             ///< Sideband Endpoint Oscillator/Side Clock Gating Enable
#define N_PCIE_CFG_RPPGEN_SEOSCGE             4

#define R_PCIE_CFG_PWRCTL                     0x000000e8U      ///< Power Control
#define B_PCIE_CFG_PWRCTL_RPDTSQPOL           BIT0             ///< Root Port Detect Squelch Polling
#define N_PCIE_CFG_PWRCTL_RPDTSQPOL           0
#define B_PCIE_CFG_PWRCTL_RPL1SQPOL           BIT1             ///< Root Port L1 Squelch Polling
#define N_PCIE_CFG_PWRCTL_RPL1SQPOL           1
#define B_PCIE_CFG_PWRCTL_RPSEWL              ( BIT3 | BIT2 )  ///< Root Port Squelch Exit Wait Latency
#define N_PCIE_CFG_PWRCTL_RPSEWL              2
#define B_PCIE_CFG_PWRCTL_TXSWING             BIT13            ///< Analog PHY Transmitter Voltage Swing
#define N_PCIE_CFG_PWRCTL_TXSWING             13
#define B_PCIE_CFG_PWRCTL_DBUPI               BIT15            ///< De-skew Buffer Unload Pointer Increment
#define N_PCIE_CFG_PWRCTL_DBUPI               15
#define B_PCIE_CFG_PWRCTL_DLP                 BIT16            ///< Down-configured Lanes Policy
#define N_PCIE_CFG_PWRCTL_DLP                 16
#define B_PCIE_CFG_PWRCTL_WPDMPGEP            BIT17            ///< Wake PLL On Detect mod-PHY Power Gating Exit Policy
#define N_PCIE_CFG_PWRCTL_WPDMPGEP            17
#define B_PCIE_CFG_PWRCTL_LTSSMRTC            BIT20            ///< LTSSM Received TSx Count
#define N_PCIE_CFG_PWRCTL_LTSSMRTC            20
#define B_PCIE_CFG_PWRCTL_LIFECF              BIT23            ///< Logical Idle Framing Error Check Filter
#define N_PCIE_CFG_PWRCTL_LIFECF              23
#define B_PCIE_CFG_PWRCTL_DARECE              BIT28            ///< Delayed Ack for Recovery Entry
#define N_PCIE_CFG_PWRCTL_DARECE              28

#define R_PCIE_CFG_DC                         0x000000ecU      ///< Decode Control
#define B_PCIE_CFG_DC_PCIBEM                  BIT2             ///< PCI Bus Emulation Mode
#define N_PCIE_CFG_DC_PCIBEM                  2
#define B_PCIE_CFG_DC_RCRBNRCE                BIT4             ///< RCRB MMIO Range Claim Enable
#define N_PCIE_CFG_DC_RCRBNRCE                4
#define B_PCIE_CFG_DC_COM                     BIT13            ///< Completion Ordering Mode
#define N_PCIE_CFG_DC_COM                     13
#define B_PCIE_CFG_DC_DCT0C                   BIT14            ///< Downstream Config Type 0 Conversion
#define N_PCIE_CFG_DC_DCT0C                   14
#define B_PCIE_CFG_DC_DCT1C                   BIT15            ///< Downstream Config Type 1 Conversion
#define N_PCIE_CFG_DC_DCT1C                   15

#define R_PCIE_CFG_IPCS                       0x000000f0U      ///< IOSF Primary Control
#define B_PCIE_CFG_IPCS_IMRS                  ( BIT6 | BIT5 | BIT4 ) ///< IOSF Max Read Request Size
#define N_PCIE_CFG_IPCS_IMRS                  4
#define B_PCIE_CFG_IPCS_IMPS                  ( BIT10 | BIT9 | BIT8 ) ///< IOSF_Max_Payload_Size
#define N_PCIE_CFG_IPCS_IMPS                  8

#define R_PCIE_CFG_PHYCTL2                    0x000000f5U      ///< Physical Layer And AFE Control 2
#define B_PCIE_CFG_PHYCTL2_PXPG2PLLOFFEN      BIT0             ///< PCI Express GEN2 PLL Off Enable
#define N_PCIE_CFG_PHYCTL2_PXPG2PLLOFFEN      0
#define B_PCIE_CFG_PHYCTL2_PXPG3PLLOFFEN      BIT1             ///< PCI Express GEN3 PLL Off Enable
#define N_PCIE_CFG_PHYCTL2_PXPG3PLLOFFEN      1
#define B_PCIE_CFG_PHYCTL2_TXCFGCHGWAIT       ( BIT5 | BIT4 )  ///< Transmit Configuration Change Wait Time
#define N_PCIE_CFG_PHYCTL2_TXCFGCHGWAIT       4
#define B_PCIE_CFG_PHYCTL2_TDFT               ( BIT7 | BIT6 )  ///< Transmit Datapath Flush Timer
#define N_PCIE_CFG_PHYCTL2_TDFT               6

#define R_PCIE_CFG_PHYCTL3                    0x000000f6U      ///< Physical Layer And AFE Control 3
#define B_PCIE_CFG_PHYCTL3_SQDIRCTRL          BIT1             ///< Squelch Direction
#define N_PCIE_CFG_PHYCTL3_SQDIRCTRL          1
#define B_PCIE_CFG_PHYCTL3_SQDIROVREN         BIT2             ///< Squelch Direction Override Enable
#define N_PCIE_CFG_PHYCTL3_SQDIROVREN         2

#define R_PCIE_CFG_IOSFSBCS                   0x000000f7U      ///< IOSF Sideband Control And Status
#define B_PCIE_CFG_IOSFSBCS_SIID              ( BIT3 | BIT2 )  ///< IOSF Sideband Interface Idle Counter
#define N_PCIE_CFG_IOSFSBCS_SIID              2
#define B_PCIE_CFG_IOSFSBCS_SCPTCGE           BIT6             ///< Side Clock Partition/Trunk Clock Gating Enable
#define N_PCIE_CFG_IOSFSBCS_SCPTCGE           6

#define R_PCIE_CFG_STRPFUSECFG                0x000000fcU      ///< Strap And Fuse Configuration
#define B_PCIE_CFG_STRPFUSECFG_DESKTOPMOB     BIT2             ///< Desktop or Mobile Fuse
#define N_PCIE_CFG_STRPFUSECFG_DESKTOPMOB     2
#define B_PCIE_CFG_STRPFUSECFG_CDCGDIS        BIT3             ///< Core Dynamic Clock Gating Disable Fuse
#define N_PCIE_CFG_STRPFUSECFG_CDCGDIS        3
#define B_PCIE_CFG_STRPFUSECFG_LTCGDIS        BIT4             ///< Link Trunk Clock Gating Disable Fuse
#define N_PCIE_CFG_STRPFUSECFG_LTCGDIS        4
#define B_PCIE_CFG_STRPFUSECFG_LDCGDIS        BIT5             ///< Link Dynamic Clock Gating Disable Fuse
#define N_PCIE_CFG_STRPFUSECFG_LDCGDIS        5
#define B_PCIE_CFG_STRPFUSECFG_ASPMDIS        BIT6             ///< ASPM Disable Fuse
#define N_PCIE_CFG_STRPFUSECFG_ASPMDIS        6
#define B_PCIE_CFG_STRPFUSECFG_PLLSHTDWNDIS   BIT8             ///< PLL Shut Down Disable Fuse
#define N_PCIE_CFG_STRPFUSECFG_PLLSHTDWNDIS   8
#define B_PCIE_CFG_STRPFUSECFG_MPHYIOPMDIS    BIT9             ///< mPHY I/O PM Disable Fuse
#define N_PCIE_CFG_STRPFUSECFG_MPHYIOPMDIS    9
#define B_PCIE_CFG_STRPFUSECFG_LR             BIT13            ///< Lane Reversal
#define N_PCIE_CFG_STRPFUSECFG_LR             13
#define B_PCIE_CFG_STRPFUSECFG_RPC            ( BIT16 | BIT15 | BIT14 ) ///< Root Port Configuration  Strap
#define N_PCIE_CFG_STRPFUSECFG_RPC            14
#define B_PCIE_CFG_STRPFUSECFG_PXIP           ( BIT27 | BIT26 | BIT25 | BIT24 ) ///< PCI Express Interrupt Pin
#define N_PCIE_CFG_STRPFUSECFG_PXIP           24
#define B_PCIE_CFG_STRPFUSECFG_SERM           BIT29            ///< Server Error Reporting Mode
#define N_PCIE_CFG_STRPFUSECFG_SERM           29

#define R_PCIE_CFG_AECH                       0x00000100U      ///< Advanced Error Extended
#define B_PCIE_CFG_AECH_CID                   0x0000ffffU      ///< Capability ID/
#define N_PCIE_CFG_AECH_CID                   0
#define B_PCIE_CFG_AECH_CV                    ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_AECH_CV                    16
#define B_PCIE_CFG_AECH_NCO                   0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_AECH_NCO                   20

#define R_PCIE_CFG_UEM                        0x00000108U      ///< Uncorrectable Error Mask
#define B_PCIE_CFG_UEM_PT                     BIT12            ///< Poisoned TLP Mask
#define N_PCIE_CFG_UEM_PT                     12
#define B_PCIE_CFG_UEM_CM                     BIT15            ///< Completer Abort Mask
#define N_PCIE_CFG_UEM_CM                     15
#define B_PCIE_CFG_UEM_URE                    BIT20            ///< Unsupported Request Error Mask
#define N_PCIE_CFG_UEM_URE                    20
#define B_PCIE_CFG_UEM_PTLPEBM                BIT26            ///< Poisoned TLP Egress Blocked Mask
#define N_PCIE_CFG_UEM_PTLPEBM                26

#define R_PCIE_CFG_PTMECH                     0x00000150U      ///< PTM Extended Capability Header
#define B_PCIE_CFG_PTMECH_CID                 0x0000ffffU      ///< Capability ID
#define N_PCIE_CFG_PTMECH_CID                 0
#define B_PCIE_CFG_PTMECH_CV                  ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_PTMECH_CV                  16
#define B_PCIE_CFG_PTMECH_NCO                 0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_PTMECH_NCO                 20

#define R_PCIE_CFG_PTMCAPR                    0x00000154U      ///< PTM Capability Register
#define B_PCIE_CFG_PTMCAPR_PTMREQC            BIT0             ///< PTM Requester Capable
#define N_PCIE_CFG_PTMCAPR_PTMREQC            0
#define B_PCIE_CFG_PTMCAPR_PTMRSPC            BIT1             ///< PTM Responder Capable
#define N_PCIE_CFG_PTMCAPR_PTMRSPC            1
#define B_PCIE_CFG_PTMCAPR_PTMRC              BIT2             ///< PTM Root Capable
#define N_PCIE_CFG_PTMCAPR_PTMRC              2
#define B_PCIE_CFG_PTMCAPR_PTMPDAC            BIT4             ///< PTM Propagation Delay Adaptation Capable
#define N_PCIE_CFG_PTMCAPR_PTMPDAC            4

#define R_PCIE_CFG_PTMCTLR                    0x00000158U      ///< PTM Control Register
#define B_PCIE_CFG_PTMCTLR_PTME               BIT0             ///< PTM Enable
#define N_PCIE_CFG_PTMCTLR_PTME               0
#define B_PCIE_CFG_PTMCTLR_RS                 BIT1             ///< Root Select
#define N_PCIE_CFG_PTMCTLR_RS                 1

#define R_PCIE_CFG_L1SECH                     0x00000200U      ///< L1 Sub-States Extended Capability Header
#define B_PCIE_CFG_L1SECH_PCIEEC              0x0000ffffU      ///< PCI Express Extended Capability ID
#define N_PCIE_CFG_L1SECH_PCIEEC              0
#define B_PCIE_CFG_L1SECH_CV                  ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_L1SECH_CV                  16
#define B_PCIE_CFG_L1SECH_NCO                 0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_L1SECH_NCO                 20

#define R_PCIE_CFG_L1SCAP                     0x00000204U      ///< L1 Sub-States Capabilities
#define B_PCIE_CFG_L1SCAP_PPL12S              BIT0             ///< PCI-PM L1.2 Supported
#define N_PCIE_CFG_L1SCAP_PPL12S              0
#define B_PCIE_CFG_L1SCAP_PPL11S              BIT1             ///< PCI-PM L1.1 Supported
#define N_PCIE_CFG_L1SCAP_PPL11S              1
#define B_PCIE_CFG_L1SCAP_AL12S               BIT2             ///< ASPM L1.2 Supported
#define N_PCIE_CFG_L1SCAP_AL12S               2
#define B_PCIE_CFG_L1SCAP_AL11S               BIT3             ///< ASPM L1.1 Supported
#define N_PCIE_CFG_L1SCAP_AL11S               3
#define B_PCIE_CFG_L1SCAP_L1PSS               BIT4             ///< L1 PM Substates Supported
#define N_PCIE_CFG_L1SCAP_L1PSS               4
#define B_PCIE_CFG_L1SCAP_PCMRT               ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Port Common Mode Restore Time
#define N_PCIE_CFG_L1SCAP_PCMRT               8
#define B_PCIE_CFG_L1SCAP_PTPOS               ( BIT17 | BIT16 ) ///< Port Tpower_on Scale
#define N_PCIE_CFG_L1SCAP_PTPOS               16
#define B_PCIE_CFG_L1SCAP_PTV                 ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 ) ///< Port Tpower_on Value
#define N_PCIE_CFG_L1SCAP_PTV                 19

#define R_PCIE_CFG_L1SCTL1                    0x00000208U      ///< L1 Sub-States Control 1
#define B_PCIE_CFG_L1SCTL1_L12LTRTLSV         ( BIT31 | BIT30 | BIT29 )
#define N_PCIE_CFG_L1SCTL1_L12LTRTLSV         29
#define V_PCIE_CFG_L1SCTL1_L12LTRTLSV         0x2
#define B_PCIE_CFG_L1SCTL1_L12LTRTLV_MASK     0x3FF0000
#define N_PCIE_CFG_L1SCTL1_L12LTRTLV          16
#define V_PCIE_CFG_L1SCTL1_L12LTRTLV          0xE6
#define B_PCIE_CFG_L1SCTL1_L1SSEIE            BIT4             ///< CLKREQ Acceleration Interrupt Enable
#define N_PCIE_CFG_L1SCTL1_L1SSEIE            4
#define B_PCIE_CFG_L1SCTL1_CMRT               ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Common Mode Restore Time
#define N_PCIE_CFG_L1SCTL1_CMRT               8
#define B_PCIE_CFG_L1SCTL1_AL11E              BIT3
#define B_PCIE_CFG_L1SCTL1_AL12E              BIT2
#define B_PCIE_CFG_L1SCTL1_PPL11E             BIT1
#define B_PCIE_CFG_L1SCTL1_PPL12E             BIT0

#define R_PCIE_CFG_L1SCTL2                    0x0000020cU      ///< L1 Sub-States Control 2
#define B_PCIE_CFG_L1SCTL2_TPOS               ( BIT1 | BIT0 )  ///< Tpower_on Scale
#define N_PCIE_CFG_L1SCTL2_TPOS               0
#define B_PCIE_CFG_L1SCTL2_POWT               ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 ) ///< Power On Wait Time
#define N_PCIE_CFG_L1SCTL2_POWT               3

#define R_PCIE_CFG_ACSECH                     0x00000220U      ///< ACS Extended Capability Header
#define B_PCIE_CFG_ACSECH_CID                 0x0000ffffU      ///< Capability ID
#define N_PCIE_CFG_ACSECH_CID                 0
#define B_PCIE_CFG_ACSECH_CV                  ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_ACSECH_CV                  16
#define B_PCIE_CFG_ACSECH_NCO                 0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_ACSECH_NCO                 20

#define R_PCIE_CFG_ACSCAPR                    0x00000224U      ///< ACS Capability Register

#define R_PCIE_CFG_VCCH                       0x00000280U      ///< Virtual Channel Capability Header
#define B_PCIE_CFG_VCCH_CID                   0x0000ffffU      ///< PCI Express Extended Capability ID
#define N_PCIE_CFG_VCCH_CID                   0
#define B_PCIE_CFG_VCCH_CV                    ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_VCCH_CV                    16
#define B_PCIE_CFG_VCCH_NCO                   0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_VCCH_NCO                   20

#define R_PCIE_CFG_PVCCR1                     0x00000284U      ///< Port VC Capability Register 1
#define B_PCIE_CFG_PVCCR1_EVCC                ( BIT2 | BIT1 | BIT0 ) ///< Extended VC Count
#define N_PCIE_CFG_PVCCR1_EVCC                0

#define R_PCIE_CFG_V0VCRC                     0x00000290U      ///< Virtual Channel 0 Resource Capability
#define B_PCIE_CFG_V0VCRC_MTS                 ( BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< Maximum Time Slots
#define N_PCIE_CFG_V0VCRC_MTS                 16

#define R_PCIE_CFG_V0CTL                      0x00000294U      ///< Virtual Channel 0 Resource Control

#define R_PCIE_CFG_V0STS                      0x0000029aU      ///< Virtual Channel 0 Resource Status
#define B_PCIE_CFG_V0STS_NP                   BIT1             ///< VC Negotiation Pending
#define N_PCIE_CFG_V0STS_NP                   1

#define R_PCIE_CFG_V1VCRC                     0x0000029cU      ///< Virtual Channel 1  Resource Capability
#define B_PCIE_CFG_V1VCRC_MTS                 ( BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< Maximum Time Slots
#define N_PCIE_CFG_V1VCRC_MTS                 16

#define R_PCIE_CFG_V1CTL                      0x000002a0U      ///< Virtual Channel 1 Resource Control
#define B_PCIE_CFG_V1CTL_ID                   ( BIT27 | BIT26 | BIT25 | BIT24 ) ///< Virtual Channel Identifier
#define N_PCIE_CFG_V1CTL_ID                   24

#define R_PCIE_CFG_V1STS                      0x000002a6U      ///< Virtual Channel 1 Resource Status
#define B_PCIE_CFG_V1STS_NP                   BIT1             ///< VC Negotiation Pending
#define N_PCIE_CFG_V1STS_NP                   1

#define R_PCIE_CFG_PCIERTP1                   0x00000300U      ///< PCI Express Replay Timer Policy 1
#define B_PCIE_CFG_PCIERTP1_G1X4              ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Gen 1 x4
#define N_PCIE_CFG_PCIERTP1_G1X4              0

#define R_PCIE_CFG_G4L0SCTL                   0x00000310U      ///< GEN4 L0s Control
#define B_PCIE_CFG_G4L0SCTL_G4CCNFTS          ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Gen4 Common Clock N_FTS
#define N_PCIE_CFG_G4L0SCTL_G4CCNFTS          0
#define B_PCIE_CFG_G4L0SCTL_G4UCNFTS          ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Gen4 Unique Clock N_FTS
#define N_PCIE_CFG_G4L0SCTL_G4UCNFTS          8
#define B_PCIE_CFG_G4L0SCTL_G4L0SIC           ( BIT23 | BIT22 ) ///< Gen4 L0s Entry Idle Control
#define N_PCIE_CFG_G4L0SCTL_G4L0SIC           22
#define B_PCIE_CFG_G4L0SCTL_G4ASL0SPL         ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Gen4Active State L0s Preparation Latency
#define N_PCIE_CFG_G4L0SCTL_G4ASL0SPL         24

#define R_PCIE_CFG_PCIENFTS                   0x00000314U      ///< PCI Express NFTS
#define B_PCIE_CFG_PCIENFTS_G1CCNFTS          ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Gen 1 Common Clock N_FTS
#define N_PCIE_CFG_PCIENFTS_G1CCNFTS          0
#define B_PCIE_CFG_PCIENFTS_G1UCNFTS          ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Gen1 Unique Clock N_FTS
#define N_PCIE_CFG_PCIENFTS_G1UCNFTS          8
#define B_PCIE_CFG_PCIENFTS_G2CCNFTS          ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< Gen2 Common Clock N_FTS
#define N_PCIE_CFG_PCIENFTS_G2CCNFTS          16
#define B_PCIE_CFG_PCIENFTS_G2UCNFTS          ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Gen2 Unique Clock N_FTS
#define N_PCIE_CFG_PCIENFTS_G2UCNFTS          24

#define R_PCIE_CFG_PCIEL0SC                   0x00000318U      ///< PCI Express L0s Control
#define B_PCIE_CFG_PCIEL0SC_G1ASL0SPL         ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< Gen1 Active State L0s Preparation Latency
#define N_PCIE_CFG_PCIEL0SC_G1ASL0SPL         16
#define B_PCIE_CFG_PCIEL0SC_G2ASL0SPL         ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Gen2 Active State L0s Preparation Latency
#define N_PCIE_CFG_PCIEL0SC_G2ASL0SPL         24

#define R_PCIE_CFG_PCIECFG2                   0x00000320U      ///< PCI Express Configuration 2
#define B_PCIE_CFG_PCIECFG2_PMET              ( BIT21 | BIT20 ) ///< PME Timeout
#define N_PCIE_CFG_PCIECFG2_PMET              20
#define B_PCIE_CFG_PCIECFG2_CRSREN            BIT22            ///< Completion Retry Status Replay Enable
#define N_PCIE_CFG_PCIECFG2_CRSREN            22
#define B_PCIE_CFG_PCIECFG2_CROAOE            BIT23            ///< Completion Relaxed Ordering Attribute Override Enable
#define N_PCIE_CFG_PCIECFG2_CROAOE            23
#define B_PCIE_CFG_PCIECFG2_CROAOV            BIT24            ///< Completion Relaxed Ordering Attribute Override Value
#define N_PCIE_CFG_PCIECFG2_CROAOV            24
#define B_PCIE_CFG_PCIECFG2_RRCP              BIT29            ///< RXL0s Receiver Control Policy
#define N_PCIE_CFG_PCIECFG2_RRCP              29

#define R_PCIE_CFG_PCIEDBG                    0x00000324U      ///< PCI Express Debug And Configuration
#define B_PCIE_CFG_PCIEDBG_DTCA               BIT4             ///< Disable TC Aliasing
#define N_PCIE_CFG_PCIEDBG_DTCA               4
#define B_PCIE_CFG_PCIEDBG_SPCE               BIT5             ///< Squelch Propagation Control Enable
#define N_PCIE_CFG_PCIEDBG_SPCE               5
#define B_PCIE_CFG_PCIEDBG_SQOL0              BIT7             ///< Squelch Off in L0
#define N_PCIE_CFG_PCIEDBG_SQOL0              7
#define B_PCIE_CFG_PCIEDBG_LDSWQRP            BIT13            ///< Link Down SWQ Reset Policy
#define N_PCIE_CFG_PCIEDBG_LDSWQRP            13
#define B_PCIE_CFG_PCIEDBG_CTONFAE            BIT14            ///< Completion Time-Out Non-Fatal Advisory Error Enable
#define N_PCIE_CFG_PCIEDBG_CTONFAE            14

#define R_PCIE_CFG_PCIESTS1                   0x00000328U      ///< PCI Express Status 1
#define B_PCIE_CFG_PCIESTS1_LNKSTAT           ( BIT22 | BIT21 | BIT20 | BIT19 ) ///< Link Status
#define N_PCIE_CFG_PCIESTS1_LNKSTAT           19
#define B_PCIE_CFG_PCIESTS1_LTSMSTATE         ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< LTSM State
#define N_PCIE_CFG_PCIESTS1_LTSMSTATE         24

#define R_PCIE_CFG_PCIESTS2                   0x0000032cU      ///< PCI Express Status 2
#define B_PCIE_CFG_PCIESTS2_P1PNCCWSSCMES     BIT28            ///< PCIe Port 1 Non-Common Clock With SSC Mode Enable Strap
#define N_PCIE_CFG_PCIESTS2_P1PNCCWSSCMES     28
#define B_PCIE_CFG_PCIESTS2_P2PNCCWSSCMES     BIT29            ///< PCIe Port 2 Non-Common Clock With SSC Mode Enable Strap
#define N_PCIE_CFG_PCIESTS2_P2PNCCWSSCMES     29
#define B_PCIE_CFG_PCIESTS2_P3PNCCWSSCMES     BIT30            ///< PCIe Port 3 Non-Common Clock With SSC Mode Enable Strap
#define N_PCIE_CFG_PCIESTS2_P3PNCCWSSCMES     30
#define B_PCIE_CFG_PCIESTS2_P4PNCCWSSCMES     BIT31            ///< PCIe Port 4 Non-Common Clock With SSC Mode Enable Strap
#define N_PCIE_CFG_PCIESTS2_P4PNCCWSSCMES     31

#define R_PCIE_CFG_PCIEALC                    0x00000338U      ///< PCI Express Additional Link Control
#define B_PCIE_CFG_PCIEALC_ONPRASPML1P        BIT11            ///< Outstanding Non-Posted Request ASPM L1 Prevention
#define N_PCIE_CFG_PCIEALC_ONPRASPML1P        11
#define B_PCIE_CFG_PCIEALC_PDSP               BIT20            ///< Present Detect State Policy
#define N_PCIE_CFG_PCIEALC_PDSP               20
#define B_PCIE_CFG_PCIEALC_RTD3PDSP           BIT21            ///< RTD3 Present Detect State Policy
#define N_PCIE_CFG_PCIEALC_RTD3PDSP           21
#define B_PCIE_CFG_PCIEALC_SSRRS              BIT23            ///< Survivability Sync Reset in Recovery state
#define N_PCIE_CFG_PCIEALC_SSRRS              23
#define B_PCIE_CFG_PCIEALC_SSRLD              BIT24            ///< Survivability Sync Reset during Link Down
#define N_PCIE_CFG_PCIEALC_SSRLD              24
#define B_PCIE_CFG_PCIEALC_BLKDQDASD          BIT25            ///< Block Detect.Quiet -> Detect.Active Strap Default
#define N_PCIE_CFG_PCIEALC_BLKDQDASD          25
#define B_PCIE_CFG_PCIEALC_BLKDQDA            BIT26            ///< Block Detect.Quiet -> Detect.Active
#define N_PCIE_CFG_PCIEALC_BLKDQDA            26
#define B_PCIE_CFG_PCIEALC_ILLRCLD            BIT28            ///< Initialize Link Layer Receiver Control on Link Down
#define N_PCIE_CFG_PCIEALC_ILLRCLD            28
#define B_PCIE_CFG_PCIEALC_ITLRCLD            BIT29            ///< Initialize Transaction Layer Receiver Control on Link Down
#define N_PCIE_CFG_PCIEALC_ITLRCLD            29

#define R_PCIE_CFG_PTMPD                      0x00000390U      ///< PTM Propagation Delay

#define R_PCIE_CFG_PTMLLMT                    0x00000394U      ///< PTM Lower Local Master Time

#define R_PCIE_CFG_PTMULMT                    0x00000398U      ///< PTM Upper Local Master Time

#define R_PCIE_CFG_PTMECFG                    0x000003b0U      ///< PTM Extended Config
#define B_PCIE_CFG_PTMECFG_IOSFMADP           ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< IOSF Max Allowed Delay programming
#define N_PCIE_CFG_PTMECFG_IOSFMADP           0
#define B_PCIE_CFG_PTMECFG_PTMRNOPAD          ( BIT5 | BIT4 )  ///< PTM Request Number Of Periodic ACK DLLP
#define N_PCIE_CFG_PTMECFG_PTMRNOPAD          4
#define B_PCIE_CFG_PTMECFG_PTMRPAE            BIT6             ///< PTM Request Periodic ACK Enable
#define N_PCIE_CFG_PTMECFG_PTMRPAE            6
#define B_PCIE_CFG_PTMECFG_PGTSCFE            BIT8             ///< Periodic Global Time Stamp Counter Fetch Enable
#define N_PCIE_CFG_PTMECFG_PGTSCFE            8
#define B_PCIE_CFG_PTMECFG_PLTLFF             ( BIT20 | BIT19 | BIT18 ) ///< Periodic Local TSC Link Fetch Frequency
#define N_PCIE_CFG_PTMECFG_PLTLFF             18

#define R_PCIE_CFG_LTROVR                     0x00000400U      ///< Latency Tolerance Reporting Override
#define B_PCIE_CFG_LTROVR_LTRSLOVRV           0x000003ffU      ///< LTR Snoop Latency Override Value
#define N_PCIE_CFG_LTROVR_LTRSLOVRV           0
#define B_PCIE_CFG_LTROVR_LTRSLSOVRV          ( BIT12 | BIT11 | BIT10 ) ///< LTR Snoop Latency Scale Override Value
#define N_PCIE_CFG_LTROVR_LTRSLSOVRV          10
#define B_PCIE_CFG_LTROVR_LTRSROVR            BIT15            ///< LTR Snoop Requirement Bit Override
#define N_PCIE_CFG_LTROVR_LTRSROVR            15
#define B_PCIE_CFG_LTROVR_LTRNSLOVRV          0x03ff0000U      ///< LTR Non-Snoop Latency Override Value
#define N_PCIE_CFG_LTROVR_LTRNSLOVRV          16
#define B_PCIE_CFG_LTROVR_LTRNSLSOVRV         ( BIT28 | BIT27 | BIT26 ) ///< LTR Non-Snoop Latency Scale Override Value
#define N_PCIE_CFG_LTROVR_LTRNSLSOVRV         26
#define B_PCIE_CFG_LTROVR_LTRNSROVR           BIT31            ///< LTR Non-Snoop Requirement Bit Override
#define N_PCIE_CFG_LTROVR_LTRNSROVR           31

#define R_PCIE_CFG_LTROVR2                    0x00000404U      ///< Latency Tolerance Reporting Override 2
#define B_PCIE_CFG_LTROVR2_LTRSOVREN          BIT0             ///< LTR Snoop Override Enable
#define N_PCIE_CFG_LTROVR2_LTRSOVREN          0
#define B_PCIE_CFG_LTROVR2_LTRNSOVREN         BIT1             ///< LTR Non-Snoop Override Enable
#define N_PCIE_CFG_LTROVR2_LTRNSOVREN         1
#define B_PCIE_CFG_LTROVR2_LTROVRPLCY         BIT3             ///< LTR Override Policy
#define N_PCIE_CFG_LTROVR2_LTROVRPLCY         3

#define R_PCIE_CFG_PHYCTL4                    0x00000408U      ///< Physical Layer And AFE Control 4
#define B_PCIE_CFG_PHYCTL4_SQDIS              BIT27            ///< Squelch Disable
#define N_PCIE_CFG_PHYCTL4_SQDIS              27

#define R_PCIE_CFG_TNPT                       0x00000418U      ///< Thermal And Power Throttling
#define B_PCIE_CFG_TNPT_DTXLTE                BIT0             ///< Dynamic TX Link Throttling Enable
#define N_PCIE_CFG_TNPT_DTXLTE                0
#define B_PCIE_CFG_TNPT_DRXLTE                BIT1             ///< Dynamic RX Link Throttling Enable
#define N_PCIE_CFG_TNPT_DRXLTE                1
#define B_PCIE_CFG_TNPT_TT                    ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 ) ///< Throttle Time
#define N_PCIE_CFG_TNPT_TT                    16
#define B_PCIE_CFG_TNPT_TP                    ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Throttle Period
#define N_PCIE_CFG_TNPT_TP                    24

#define R_PCIE_CFG_PCIEPMECTL                 0x00000420U      ///< PCIe PM Extension Control
#define B_PCIE_CFG_PCIEPMECTL_L1FSOE          BIT0             ///< L1 Full Squelch OFF Enable
#define N_PCIE_CFG_PCIEPMECTL_L1FSOE          0
#define B_PCIE_CFG_PCIEPMECTL_L1SNZCREWD      BIT3             ///< L1.SNOOZ/L1.LOW Clock Request Early Wake Disable
#define N_PCIE_CFG_PCIEPMECTL_L1SNZCREWD      3
#define B_PCIE_CFG_PCIEPMECTL_L1LTRTLV        0x00003ff0U      ///< L1.LOW LTR Threshold Latency Value
#define N_PCIE_CFG_PCIEPMECTL_L1LTRTLV        4
#define B_PCIE_CFG_PCIEPMECTL_L1LTRTLSV       ( BIT16 | BIT15 | BIT14 ) ///< L1.LOW LTR Threshold Latency ScaleValue
#define N_PCIE_CFG_PCIEPMECTL_L1LTRTLSV       14
#define B_PCIE_CFG_PCIEPMECTL_L1LE            BIT17            ///< L1.LOW Enable
#define N_PCIE_CFG_PCIEPMECTL_L1LE            17
#define B_PCIE_CFG_PCIEPMECTL_POFFWT          ( BIT19 | BIT18 ) ///< Power Off Wait Time
#define N_PCIE_CFG_PCIEPMECTL_POFFWT          18
#define B_PCIE_CFG_PCIEPMECTL_IPACPE          BIT20            ///< IP-Accessible Context Propagation Enable
#define N_PCIE_CFG_PCIEPMECTL_IPACPE          20
#define B_PCIE_CFG_PCIEPMECTL_IPIEP           BIT21            ///< IP-Inaccessible Entry Policy
#define N_PCIE_CFG_PCIEPMECTL_IPIEP           21
#define B_PCIE_CFG_PCIEPMECTL_L1OCREWD        BIT28            ///< L1.OFF Clock Request Early Wake Disable
#define N_PCIE_CFG_PCIEPMECTL_L1OCREWD        28
#define B_PCIE_CFG_PCIEPMECTL_DLSULDLSD       BIT29            ///< Disabled, Detect, L23_Rdy State,Un-Configured Lane and Down-Configured Lane Squelch Disable
#define N_PCIE_CFG_PCIEPMECTL_DLSULDLSD       29
#define B_PCIE_CFG_PCIEPMECTL_DLSULPPGE       BIT30            ///< Disabled, Detect and L23_Rdy State PHY Lane Power Gating Enable
#define N_PCIE_CFG_PCIEPMECTL_DLSULPPGE       30
#define B_PCIE_CFG_PCIEPMECTL_FDPPGE          BIT31            ///< Function Disable PHY Power Gating Enable
#define N_PCIE_CFG_PCIEPMECTL_FDPPGE          31

#define R_PCIE_CFG_PCIEPMECTL2                0x00000424U      ///< PCIe PM Extension Control 2
#define B_PCIE_CFG_PCIEPMECTL2_L1SCPGE        BIT4             ///< L1 State Controller Power Gating Enable
#define N_PCIE_CFG_PCIEPMECTL2_L1SCPGE        4
#define B_PCIE_CFG_PCIEPMECTL2_DISSCPGE       BIT5             ///< Disabled State Controller Power Gating Enable
#define N_PCIE_CFG_PCIEPMECTL2_DISSCPGE       5
#define B_PCIE_CFG_PCIEPMECTL2_L23RDYSCPGE    BIT6             ///< L23_Rdy State Controller Power Gating Enable
#define N_PCIE_CFG_PCIEPMECTL2_L23RDYSCPGE    6
#define B_PCIE_CFG_PCIEPMECTL2_DETSCPGE       BIT7             ///< Detect State Controller Power Gating Enable
#define N_PCIE_CFG_PCIEPMECTL2_DETSCPGE       7
#define B_PCIE_CFG_PCIEPMECTL2_FDCPGE         BIT8             ///< Function Disable Controller Power Gating Enable
#define N_PCIE_CFG_PCIEPMECTL2_FDCPGE         8
#define B_PCIE_CFG_PCIEPMECTL2_L1SPHYDLPGE    BIT9             ///< Reserved
#define N_PCIE_CFG_PCIEPMECTL2_L1SPHYDLPGE    9
#define B_PCIE_CFG_PCIEPMECTL2_PHYCLPGE       BIT11            ///< PHY Common Lane Power Gating Enable
#define N_PCIE_CFG_PCIEPMECTL2_PHYCLPGE       11
#define B_PCIE_CFG_PCIEPMECTL2_CPGENH         ( BIT13 | BIT12 ) ///< Controller Power Gating Entry Hysteresis
#define N_PCIE_CFG_PCIEPMECTL2_CPGENH         12
#define B_PCIE_CFG_PCIEPMECTL2_CPGEXH         ( BIT15 | BIT14 ) ///< Controller Power Gating Exit Hysteresis
#define N_PCIE_CFG_PCIEPMECTL2_CPGEXH         14
#define B_PCIE_CFG_PCIEPMECTL2_CPMCSRE        BIT27            ///< Chassis PMC Save and Restore Enable
#define N_PCIE_CFG_PCIEPMECTL2_CPMCSRE        27

#define R_PCIE_CFG_PCE                        0x00000428U      ///< Power Control Enable
#define B_PCIE_CFG_PCE_PMCRE                  BIT0             ///< PMC Request Enable
#define N_PCIE_CFG_PCE_PMCRE                  0
#define B_PCIE_CFG_PCE_SE                     BIT3             ///< Sleep Enable
#define N_PCIE_CFG_PCE_SE                     3
#define B_PCIE_CFG_PCE_HAE                    BIT5             ///< Hardware Autonomous Enable
#define N_PCIE_CFG_PCE_HAE                    5

#define R_PCIE_CFG_PGCBCTL1                   0x0000042cU      ///< PGCB Control 1
#define B_PCIE_CFG_PGCBCTL1_TACCRSTUP         ( BIT11 | BIT10 ) ///< cfg_taccrstup
#define N_PCIE_CFG_PGCBCTL1_TACCRSTUP         10

#define R_PCIE_CFG_PCIEPMECTL3                0x00000434U      ///< PCIe PM Extension Control 3
#define B_PCIE_CFG_PCIEPMECTL3_PMREQCPGEXH    ( BIT1 | BIT0 )  ///< PM  Request Controller Power Gating Exit Hysteresis
#define N_PCIE_CFG_PCIEPMECTL3_PMREQCPGEXH    0
#define B_PCIE_CFG_PCIEPMECTL3_OSCCGH         ( BIT3 | BIT2 )  ///< Osc Clock Gate Hysterisis
#define N_PCIE_CFG_PCIEPMECTL3_OSCCGH         2
#define B_PCIE_CFG_PCIEPMECTL3_L1PGAUTOPGEN   BIT4             ///< L1.PG Auto Power Gate Enable
#define N_PCIE_CFG_PCIEPMECTL3_L1PGAUTOPGEN   4

#define R_PCIE_CFG_EQCFG1                     0x00000450U      ///< Equalization Configuration 1
#define B_PCIE_CFG_EQCFG1_TUPP                BIT1             ///< Transmitter Use Preset Policy
#define N_PCIE_CFG_EQCFG1_TUPP                1
#define B_PCIE_CFG_EQCFG1_HAPCCPIE            BIT5             ///< Hardware Autonomous Preset/Coefficient Count Per-Iteration Extention
#define N_PCIE_CFG_EQCFG1_HAPCCPIE            5
#define B_PCIE_CFG_EQCFG1_EQTS2IRRC           BIT7             ///< EQ TS2 in Recovery.ReceiverConfig Enable
#define N_PCIE_CFG_EQCFG1_EQTS2IRRC           7
#define B_PCIE_CFG_EQCFG1_RWTNEVE             ( BIT11 | BIT10 | BIT9 | BIT8 ) ///< Receiver Wait Time For New Equalization Value Evaluation
#define N_PCIE_CFG_EQCFG1_RWTNEVE             8
#define B_PCIE_CFG_EQCFG1_HAED                BIT12            ///< Hardware Autonomous Equalization Done
#define N_PCIE_CFG_EQCFG1_HAED                12
#define B_PCIE_CFG_EQCFG1_HPCMQE              BIT13            ///< Hardware Preset to Coefficient Mapping Query Enable
#define N_PCIE_CFG_EQCFG1_HPCMQE              13
#define B_PCIE_CFG_EQCFG1_RTPCOE              BIT15            ///< Remote Transmitter Preset Coefficient Override Enable
#define N_PCIE_CFG_EQCFG1_RTPCOE              15
#define B_PCIE_CFG_EQCFG1_RTLEPCEB            BIT16            ///< Remote Transmit Link Equalization Preset/Coefficient Evaluation Bypass
#define N_PCIE_CFG_EQCFG1_RTLEPCEB            16
#define B_PCIE_CFG_EQCFG1_LEP3B               BIT17            ///< Link Equalization 3 Bypass
#define N_PCIE_CFG_EQCFG1_LEP3B               17
#define B_PCIE_CFG_EQCFG1_LEP23B              BIT18            ///< Link Equalization Phase 2 and 3 Bypass
#define N_PCIE_CFG_EQCFG1_LEP23B              18
#define B_PCIE_CFG_EQCFG1_LEB                 BIT19            ///< Link Equalization Bypass
#define N_PCIE_CFG_EQCFG1_LEB                 19
#define B_PCIE_CFG_EQCFG1_LERSMIE             BIT21            ///< Link Equalization Request SMI Enable
#define N_PCIE_CFG_EQCFG1_LERSMIE             21
#define B_PCIE_CFG_EQCFG1_REIFECE             BIT23            ///< Recovery Entry and Idle Framing Error Count Enable
#define N_PCIE_CFG_EQCFG1_REIFECE             23
#define B_PCIE_CFG_EQCFG1_REC                 ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Recovery Entry Count
#define N_PCIE_CFG_EQCFG1_REC                 24

#define R_PCIE_CFG_RTPCL1                     0x00000454U      ///< Remote Transmitter Preset Coefficient List 1
#define B_PCIE_CFG_RTPCL1_RTPRECL0PL0         ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Remote Transmitter Pre-Cursor Coefficient List 0/Preset List 0
#define N_PCIE_CFG_RTPCL1_RTPRECL0PL0         0
#define B_PCIE_CFG_RTPCL1_RTPOSTCL0PL1        ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Remote Transmitter Post-Cursor Coefficient List 0/Preset List 1
#define N_PCIE_CFG_RTPCL1_RTPOSTCL0PL1        6
#define B_PCIE_CFG_RTPCL1_RTPRECL1PL2         ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Remote Transmitter Pre-Cursor Coefficient List 1/Preset List 2
#define N_PCIE_CFG_RTPCL1_RTPRECL1PL2         12
#define B_PCIE_CFG_RTPCL1_RTPOSTCL1PL3        ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Remote Transmitter Post-Cursor Coefficient List 1/Preset List 3
#define N_PCIE_CFG_RTPCL1_RTPOSTCL1PL3        18
#define B_PCIE_CFG_RTPCL1_RTPRECL2PL4         ( BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Remote Transmitter Pre-Cursor Coefficient List 2/Preset List 4
#define N_PCIE_CFG_RTPCL1_RTPRECL2PL4         24
#define B_PCIE_CFG_RTPCL1_PCM                 BIT31            ///< Preset/Coefficient Mode
#define N_PCIE_CFG_RTPCL1_PCM                 31

#define R_PCIE_CFG_RTPCL2                     0x00000458U      ///< Remote Transmitter Preset Coefficient List 2
#define B_PCIE_CFG_RTPCL2_RTPOSTCL2PL5        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Remote Transmitter Post-Cursor Coefficient List 2/Preset List 5
#define N_PCIE_CFG_RTPCL2_RTPOSTCL2PL5        0
#define B_PCIE_CFG_RTPCL2_RTPRECL3PL6         ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Remote Transmitter Pre-Cursor Coefficient List 3/Preset List 6
#define N_PCIE_CFG_RTPCL2_RTPRECL3PL6         6
#define B_PCIE_CFG_RTPCL2_RTPOSTCL3PL7        ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Remote Transmitter Post-Cursor Coefficient List 3/Preset List 7
#define N_PCIE_CFG_RTPCL2_RTPOSTCL3PL7        12
#define B_PCIE_CFG_RTPCL2_RTPRECL4PL8         ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Remote Transmitter Pre-Cursor Coefficient List 4/Preset List 8
#define N_PCIE_CFG_RTPCL2_RTPRECL4PL8         18
#define B_PCIE_CFG_RTPCL2_RTPOSTCL4PL9        ( BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Remote Transmitter Post-Cursor Coefficient List 4/Preset List 9
#define N_PCIE_CFG_RTPCL2_RTPOSTCL4PL9        24

#define R_PCIE_CFG_RTPCL3                     0x0000045cU      ///< Remote Transmitter Preset Coefficient List 3
#define B_PCIE_CFG_RTPCL3_RTPRECL5PL10        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Remote Transmitter Pre-Cursor Coefficient List 5/Preset List 10
#define N_PCIE_CFG_RTPCL3_RTPRECL5PL10        0
#define B_PCIE_CFG_RTPCL3_RTPOSTCL5           ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Remote Transmitter Post-Cursor Coefficient List 5
#define N_PCIE_CFG_RTPCL3_RTPOSTCL5           6
#define B_PCIE_CFG_RTPCL3_RTPRECL6            ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Remote Transmitter Pre-Cursor Coefficient List 6
#define N_PCIE_CFG_RTPCL3_RTPRECL6            12
#define B_PCIE_CFG_RTPCL3_RTPOSTCL6           ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Remote Transmitter Post-Cursor Coefficient List 6
#define N_PCIE_CFG_RTPCL3_RTPOSTCL6           18
#define B_PCIE_CFG_RTPCL3_RTPRECL7            ( BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Remote Transmitter Pre-Cursor Coefficient List 7
#define N_PCIE_CFG_RTPCL3_RTPRECL7            24

#define R_PCIE_CFG_RTPCL4                     0x00000460U      ///< Remote Transmitter Preset Coefficient List 4
#define B_PCIE_CFG_RTPCL4_RTPOSTCL7           ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Remote Transmitter Post-Cursor Coefficient List 7
#define N_PCIE_CFG_RTPCL4_RTPOSTCL7           0
#define B_PCIE_CFG_RTPCL4_RTPRECL8            ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Remote Transmitter Pre-Cursor Coefficient List 8
#define N_PCIE_CFG_RTPCL4_RTPRECL8            6
#define B_PCIE_CFG_RTPCL4_RTPOSTCL8           ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Remote Transmitter Post-Cursor Coefficient List 8
#define N_PCIE_CFG_RTPCL4_RTPOSTCL8           12
#define B_PCIE_CFG_RTPCL4_RTPRECL9            ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Remote Transmitter Pre-Cursor Coefficient List 9
#define N_PCIE_CFG_RTPCL4_RTPRECL9            18
#define B_PCIE_CFG_RTPCL4_RTPOSTCL9           ( BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Remote Transmitter Post-Cursor Coefficient List 9
#define N_PCIE_CFG_RTPCL4_RTPOSTCL9           24

#define R_PCIE_CFG_FOMS                       0x00000464U      ///< Figure Of Merit Status
#define B_PCIE_CFG_FOMS_FOMSV                 0x00ffffffU      ///< Figure of Merit Scoreboard Value
#define N_PCIE_CFG_FOMS_FOMSV                 0
#define B_PCIE_CFG_FOMS_LN                    ( BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Lane Number
#define N_PCIE_CFG_FOMS_LN                    24
#define B_PCIE_CFG_FOMS_IDX                   ( BIT31 | BIT30 | BIT29 ) ///< Index
#define N_PCIE_CFG_FOMS_IDX                   29

#define R_PCIE_CFG_HAEQ                       0x00000468U      ///< Hardware Autonomous Equalization Control
#define B_PCIE_CFG_HAEQ_DL                    ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Dwelling Latency
#define N_PCIE_CFG_HAEQ_DL                    8
#define B_PCIE_CFG_HAEQ_MACFOMC               BIT19            ///< MAC FOM Control
#define N_PCIE_CFG_HAEQ_MACFOMC               19
#define B_PCIE_CFG_HAEQ_HAPCCPI               ( BIT31 | BIT30 | BIT29 | BIT28 ) ///< Hardware Autonomous Preset/Coefficient Count Per-Iteration
#define N_PCIE_CFG_HAEQ_HAPCCPI               28

#define R_PCIE_CFG_LTCO1                      0x00000470U      ///< Local Transmitter Coefficient Override 1
#define B_PCIE_CFG_LTCO1_L0TPRECO             ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 0 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_LTCO1_L0TPRECO             0
#define B_PCIE_CFG_LTCO1_L0TPOSTCO            ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 0 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_LTCO1_L0TPOSTCO            6
#define B_PCIE_CFG_LTCO1_L1TPRECO             ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 1 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_LTCO1_L1TPRECO             12
#define B_PCIE_CFG_LTCO1_L1TPOSTCO            ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 1 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_LTCO1_L1TPOSTCO            18
#define B_PCIE_CFG_LTCO1_L0TCOE               BIT24            ///< Lane 0 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_LTCO1_L0TCOE               24
#define B_PCIE_CFG_LTCO1_L1TCOE               BIT25            ///< Lane 1 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_LTCO1_L1TCOE               25

#define R_PCIE_CFG_LTCO2                      0x00000474U      ///< Local Transmitter Coefficient Override 2
#define B_PCIE_CFG_LTCO2_L2TPRECO             ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 2 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_LTCO2_L2TPRECO             0
#define B_PCIE_CFG_LTCO2_L2TPOSTCO            ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 2 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_LTCO2_L2TPOSTCO            6
#define B_PCIE_CFG_LTCO2_L3TPRECO             ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 3 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_LTCO2_L3TPRECO             12
#define B_PCIE_CFG_LTCO2_L3TPOSTCO            ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 3 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_LTCO2_L3TPOSTCO            18
#define B_PCIE_CFG_LTCO2_L2TCOE               BIT24            ///< Lane 2 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_LTCO2_L2TCOE               24
#define B_PCIE_CFG_LTCO2_L3TCOE               BIT25            ///< Lane 3 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_LTCO2_L3TCOE               25

#define R_PCIE_CFG_G3L0SCTL                   0x00000478U      ///< GEN3 L0s Control
#define B_PCIE_CFG_G3L0SCTL_G3CCNFTS          ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Gen3 Common Clock N_FTS
#define N_PCIE_CFG_G3L0SCTL_G3CCNFTS          0
#define B_PCIE_CFG_G3L0SCTL_G3UCNFTS          ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Gen3 Unique Clock N_FTS
#define N_PCIE_CFG_G3L0SCTL_G3UCNFTS          8
#define B_PCIE_CFG_G3L0SCTL_G3L0SIC           ( BIT23 | BIT22 ) ///< Gen3 L0s Entry Idle Control
#define N_PCIE_CFG_G3L0SCTL_G3L0SIC           22
#define B_PCIE_CFG_G3L0SCTL_G3ASL0SPL         ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Gen3 Active State L0s Preparation Latency
#define N_PCIE_CFG_G3L0SCTL_G3ASL0SPL         24

#define R_PCIE_CFG_EQCFG2                     0x0000047cU      ///< Equalization Configuration 2
#define B_PCIE_CFG_EQCFG2_REWMET              ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Receiver Eye Margin Error Threshold
#define N_PCIE_CFG_EQCFG2_REWMET              0
#define B_PCIE_CFG_EQCFG2_REWMETM             ( BIT9 | BIT8 )  ///< Receiver Eye Margin Error Threshold Multiplier
#define N_PCIE_CFG_EQCFG2_REWMETM             8
#define B_PCIE_CFG_EQCFG2_MPEME               BIT10            ///< Mid-Point Equalization Mechanism Enable
#define N_PCIE_CFG_EQCFG2_MPEME               10
#define B_PCIE_CFG_EQCFG2_NTEME               BIT11            ///< Nine-Tiles Equalization Mechanism Enable
#define N_PCIE_CFG_EQCFG2_NTEME               11
#define B_PCIE_CFG_EQCFG2_HAPCSB              ( BIT15 | BIT14 | BIT13 | BIT12 ) ///< Hardware Autonomous Preset/Coefficient Search Bound
#define N_PCIE_CFG_EQCFG2_HAPCSB              12
#define B_PCIE_CFG_EQCFG2_PCET                ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Preset/Coefficient Evaluation Timeout
#define N_PCIE_CFG_EQCFG2_PCET                16
#define B_PCIE_CFG_EQCFG2_NTSS                ( BIT22 | BIT21 | BIT20 ) ///< Nine-Tiles Step Size
#define N_PCIE_CFG_EQCFG2_NTSS                20

#define R_PCIE_CFG_MM                         0x00000480U      ///< Monitor Mux
#define B_PCIE_CFG_MM_MSS                     ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Monitor Signal Select
#define N_PCIE_CFG_MM_MSS                     0
#define B_PCIE_CFG_MM_MSST                    0xffffff00U      ///< Monitor Signal State
#define N_PCIE_CFG_MM_MSST                    8

#define R_PCIE_CFG_CDM                        0x00000484U      ///< Controller Debug And Monitor
#define B_PCIE_CFG_CDM_MCS                    ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Monitor Category Select
#define N_PCIE_CFG_CDM_MCS                    0
#define B_PCIE_CFG_CDM_MSS                    ( BIT7 | BIT6 | BIT5 | BIT4 ) ///< Monitor Segment Select
#define N_PCIE_CFG_CDM_MSS                    4

#define R_PCIE_CFG_EQCFG4                     0x0000048cU      ///< Equalization Configuration 4
#define B_PCIE_CFG_EQCFG4_PX16GMFLNTL         BIT0             ///< 16GT/s Multi-Fragment Linear and Nine-Tile List Enable
#define N_PCIE_CFG_EQCFG4_PX16GMFLNTL         0
#define B_PCIE_CFG_EQCFG4_PX16GHAPCCPIE       BIT2             ///< 16.0 GT/s Hardware Autonomous Preset/Coefficient Count Per-Iteration Extention
#define N_PCIE_CFG_EQCFG4_PX16GHAPCCPIE       2
#define B_PCIE_CFG_EQCFG4_PX16GHAPCCPI        ( BIT6 | BIT5 | BIT4 | BIT3 ) ///< 16GT/s Hardware Autonomous Preset/Coefficient Count Per-Iteration
#define N_PCIE_CFG_EQCFG4_PX16GHAPCCPI        3
#define B_PCIE_CFG_EQCFG4_PX16GEQTS2IRRC      BIT7             ///< 16GT/s EQ TS2 in Recovery.ReceiverConfig Enable
#define N_PCIE_CFG_EQCFG4_PX16GEQTS2IRRC      7
#define B_PCIE_CFG_EQCFG4_PX16GRWTNEVE        ( BIT11 | BIT10 | BIT9 | BIT8 ) ///< 16GT/s  Receiver Wait Time For New Equalization Value Evaluation
#define N_PCIE_CFG_EQCFG4_PX16GRWTNEVE        8
#define B_PCIE_CFG_EQCFG4_PX16GHAED           BIT12            ///< 16GT/s Hardware Autonomous Equalization Done
#define N_PCIE_CFG_EQCFG4_PX16GHAED           12
#define B_PCIE_CFG_EQCFG4_PX16GRTPCOE         BIT15            ///< 16GT/s Remote Transmitter Preset Coefficient Override Enable
#define N_PCIE_CFG_EQCFG4_PX16GRTPCOE         15
#define B_PCIE_CFG_EQCFG4_PX16GRTLEPCEB       BIT16            ///< 16GT/s Remote Transmit Link Equalization Preset/Coefficient Evaluation Bypass
#define N_PCIE_CFG_EQCFG4_PX16GRTLEPCEB       16
#define B_PCIE_CFG_EQCFG4_PX16GLEP3B          BIT17            ///< 16.0 GT/s Link Equalization 3 Bypass
#define N_PCIE_CFG_EQCFG4_PX16GLEP3B          17
#define B_PCIE_CFG_EQCFG4_PX16GLEP23B         BIT18            ///< 16.0 GT/s Link Equalization Phase 2 and 3 Bypass
#define N_PCIE_CFG_EQCFG4_PX16GLEP23B         18
#define B_PCIE_CFG_EQCFG4_FOMSCP              ( BIT23 | BIT22 | BIT21 ) ///< FOM Scoreboard Control Policy
#define N_PCIE_CFG_EQCFG4_FOMSCP              21
#define B_PCIE_CFG_EQCFG4_PX8GTSWLPCE         ( BIT26 | BIT25 | BIT24 ) ///< 8.0GT/s Training Sequence Wait Latency For Presets / Coefficients Evaluation
#define N_PCIE_CFG_EQCFG4_PX8GTSWLPCE         24
#define B_PCIE_CFG_EQCFG4_PX16GTSWLPCE        ( BIT29 | BIT28 | BIT27 ) ///< 16.0GT/s Training Sequence Wait Latency For Presets / Coefficients Evaluation
#define N_PCIE_CFG_EQCFG4_PX16GTSWLPCE        27

#define R_PCIE_CFG_CTRL1                      0x000004a0U      ///< Control 1
#define B_PCIE_CFG_CTRL1_L1RCEP               BIT8             ///< DMI L1 Root Port Exit Policy
#define N_PCIE_CFG_CTRL1_L1RCEP               8
#define B_PCIE_CFG_CTRL1_L1PL                 ( BIT11 | BIT10 | BIT9 )            ///< DMI L1 Preparation Latency
#define N_PCIE_CFG_CTRL1_L1PL                 9
#define V_PCIE_CFG_CTRL1_L1PL                 0x3
#define B_PCIE_CFG_CTRL1_L1RC                 BIT12            ///< DMI L1 Root Port Control
#define N_PCIE_CFG_CTRL1_L1RC                 12
#define B_PCIE_CFG_CTRL1_L0SPFCUF             ( BIT16 | BIT15 ) ///< L0s Periodic Flow Control Update Frequency
#define N_PCIE_CFG_CTRL1_L0SPFCUF             15

#define R_PCIE_CFG_CTRL2                      0x000004a4U      ///< Control 2
#define B_PCIE_CFG_CTRL2_PMETOFD              BIT6             ///< PMETO Timeout Fix Disable
#define N_PCIE_CFG_CTRL2_PMETOFD              6
#define B_PCIE_CFG_CTRL2_CLWSWPT              ( BIT24 | BIT23 | BIT22 )
#define N_PCIE_CFG_CTRL2_CLWSWPT              22

#define R_PCIE_CFG_IOSFC3TC                   0x000004c8U      ///< IOSF Channel 3 Transaction Credit
#define B_PCIE_CFG_IOSFC3TC_C0CPCC            ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Channel 0 Completion Command Credit
#define N_PCIE_CFG_IOSFC3TC_C0CPCC            0
#define B_PCIE_CFG_IOSFC3TC_C0CPDC            ( BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 ) ///< Channel 0 Completion Data Credit
#define N_PCIE_CFG_IOSFC3TC_C0CPDC            8

#define R_PCIE_CFG_PX16GRTPCL1                0x000004dcU      ///< 16 GT/s Remote Transmitter Preset Coefficient List 1
#define B_PCIE_CFG_PX16GRTPCL1_RTPRECL0PL0    ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Remote Transmitter Pre-Cursor Coefficient List 0/Preset List 0
#define N_PCIE_CFG_PX16GRTPCL1_RTPRECL0PL0    0
#define B_PCIE_CFG_PX16GRTPCL1_RTPOSTCL0PL1   ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Remote Transmitter Post-Cursor Coefficient List 0/Preset List 1
#define N_PCIE_CFG_PX16GRTPCL1_RTPOSTCL0PL1   6
#define B_PCIE_CFG_PX16GRTPCL1_RTPRECL1PL2    ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Remote Transmitter Pre-Cursor Coefficient List 1/Preset List 2
#define N_PCIE_CFG_PX16GRTPCL1_RTPRECL1PL2    12
#define B_PCIE_CFG_PX16GRTPCL1_RTPOSTCL1PL3   ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Remote Transmitter Post-Cursor Coefficient List 1/Preset List 3
#define N_PCIE_CFG_PX16GRTPCL1_RTPOSTCL1PL3   18
#define B_PCIE_CFG_PX16GRTPCL1_RTPRECL2PL4    ( BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Remote Transmitter Pre-Cursor Coefficient List 2/Preset List 4
#define N_PCIE_CFG_PX16GRTPCL1_RTPRECL2PL4    24
#define B_PCIE_CFG_PX16GRTPCL1_PCM            BIT31            ///< Preset/Coefficient Mode
#define N_PCIE_CFG_PX16GRTPCL1_PCM            31

#define R_PCIE_CFG_PX16GRTPCL2                0x000004e0U      ///< 16 GT/s Remote Transmitter Preset Coefficient List 2
#define B_PCIE_CFG_PX16GRTPCL2_RTPOSTCL2PL5   ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Remote Transmitter Post-Cursor Coefficient List 2/Preset List 5
#define N_PCIE_CFG_PX16GRTPCL2_RTPOSTCL2PL5   0
#define B_PCIE_CFG_PX16GRTPCL2_RTPRECL3PL6    ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Remote Transmitter Pre-Cursor Coefficient List 3/Preset List 6
#define N_PCIE_CFG_PX16GRTPCL2_RTPRECL3PL6    6
#define B_PCIE_CFG_PX16GRTPCL2_RTPOSTCL3PL7   ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Remote Transmitter Post-Cursor Coefficient List 3/Preset List 7
#define N_PCIE_CFG_PX16GRTPCL2_RTPOSTCL3PL7   12
#define B_PCIE_CFG_PX16GRTPCL2_RTPRECL4PL8    ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Remote Transmitter Pre-Cursor Coefficient List 4/Preset List 8
#define N_PCIE_CFG_PX16GRTPCL2_RTPRECL4PL8    18
#define B_PCIE_CFG_PX16GRTPCL2_RTPOSTCL4PL9   ( BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Remote Transmitter Post-Cursor Coefficient List 4/Preset List 9
#define N_PCIE_CFG_PX16GRTPCL2_RTPOSTCL4PL9   24

#define R_PCIE_CFG_PX16GRTPCL3                0x000004e4U      ///< 16 GT/s Remote Transmitter Preset Coefficient List 3
#define B_PCIE_CFG_PX16GRTPCL3_RTPRECL5PL10   ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Remote Transmitter Pre-Cursor Coefficient List 5/Preset List 10
#define N_PCIE_CFG_PX16GRTPCL3_RTPRECL5PL10   0
#define B_PCIE_CFG_PX16GRTPCL3_RTPOSTCL5      ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Remote Transmitter Post-Cursor Coefficient List 5
#define N_PCIE_CFG_PX16GRTPCL3_RTPOSTCL5      6
#define B_PCIE_CFG_PX16GRTPCL3_RTPRECL6       ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Remote Transmitter Pre-Cursor Coefficient List 6
#define N_PCIE_CFG_PX16GRTPCL3_RTPRECL6       12
#define B_PCIE_CFG_PX16GRTPCL3_RTPOSTCL6      ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Remote Transmitter Post-Cursor Coefficient List 6
#define N_PCIE_CFG_PX16GRTPCL3_RTPOSTCL6      18
#define B_PCIE_CFG_PX16GRTPCL3_RTPRECL7       ( BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Remote Transmitter Pre-Cursor Coefficient List 7
#define N_PCIE_CFG_PX16GRTPCL3_RTPRECL7       24

#define R_PCIE_CFG_PX16GRTPCL4                0x000004e8U      ///< 16 GT/s Remote Transmitter Preset Coefficient List 4
#define B_PCIE_CFG_PX16GRTPCL4_RTPOSTCL7      ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Remote Transmitter Post-Cursor Coefficient List 7
#define N_PCIE_CFG_PX16GRTPCL4_RTPOSTCL7      0
#define B_PCIE_CFG_PX16GRTPCL4_RTPRECL8       ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Remote Transmitter Pre-Cursor Coefficient List 8
#define N_PCIE_CFG_PX16GRTPCL4_RTPRECL8       6
#define B_PCIE_CFG_PX16GRTPCL4_RTPOSTCL8      ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Remote Transmitter Post-Cursor Coefficient List 8
#define N_PCIE_CFG_PX16GRTPCL4_RTPOSTCL8      12
#define B_PCIE_CFG_PX16GRTPCL4_RTPRECL9       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Remote Transmitter Pre-Cursor Coefficient List 9
#define N_PCIE_CFG_PX16GRTPCL4_RTPRECL9       18
#define B_PCIE_CFG_PX16GRTPCL4_RTPOSTCL9      ( BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< Remote Transmitter Post-Cursor Coefficient List 9
#define N_PCIE_CFG_PX16GRTPCL4_RTPOSTCL9      24

#define R_PCIE_CFG_EQCFG5                     0x000004f8U      ///< Equalization Configuration 5
#define B_PCIE_CFG_EQCFG5_REWMET              ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Receiver Eye Margin Error Threshold
#define N_PCIE_CFG_EQCFG5_REWMET              0
#define B_PCIE_CFG_EQCFG5_REWMETM             ( BIT9 | BIT8 )  ///< Receiver Eye Margin Error Threshold Multiplier
#define N_PCIE_CFG_EQCFG5_REWMETM             8
#define B_PCIE_CFG_EQCFG5_MPEME               BIT10            ///< Mid-Point Equalization Mechanism Enable
#define N_PCIE_CFG_EQCFG5_MPEME               10
#define B_PCIE_CFG_EQCFG5_NTEME               BIT11            ///< Nine-Tiles Equalization Mechanism Enable
#define N_PCIE_CFG_EQCFG5_NTEME               11
#define B_PCIE_CFG_EQCFG5_HAPCSB              ( BIT15 | BIT14 | BIT13 | BIT12 ) ///< Hardware Autonomous Preset/Coefficient Search Bound
#define N_PCIE_CFG_EQCFG5_HAPCSB              12
#define B_PCIE_CFG_EQCFG5_PCET                ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Preset/Coefficient Evaluation Timeout
#define N_PCIE_CFG_EQCFG5_PCET                16

#define R_PCIE_CFG_L0P0P1PCM                  0x00000500U      ///< Lane 0 P0 And P1 Preset-Coefficient Mapping

#define R_PCIE_CFG_L0P1P2P3PCM                0x00000504U      ///< Lane 0 P1, P2 And P3 Preset-Coefficient Mapping

#define R_PCIE_CFG_L0P3P4PCM                  0x00000508U      ///< Lane 0 P3 And P4 Preset-Coefficient Mapping

#define R_PCIE_CFG_L0P5P6PCM                  0x0000050cU      ///< Lane 0 P5 And P6 Preset-Coefficient Mapping

#define R_PCIE_CFG_L0P6P7P8PCM                0x00000510U      ///< Lane 0 P6, P7 And P8 Preset-Coefficient Mapping

#define R_PCIE_CFG_L0P8P9PCM                  0x00000514U      ///< Lane 0 P8 And P9 Preset-Coefficient Mapping

#define R_PCIE_CFG_L0P10PCM                   0x00000518U      ///< Lane 0 P10 Preset-Coefficient Mapping

#define R_PCIE_CFG_L0LFFS                     0x0000051cU      ///< Lane 0 LF And FS

#define R_PCIE_CFG_PX16GP0P1PCM               0x00000520U      ///< 16.0 GT/s P0 And P1 Preset-Coefficient Mapping

#define R_PCIE_CFG_PX16GP1P2P3PCM             0x00000524U      ///< 16.0 GT/s P1, P2 And P3 Preset-Coefficient Mapping

#define R_PCIE_CFG_PX16GP3P4PCM               0x00000528U      ///< 16.0 GT/s P3 And P4 Preset-Coefficient Mapping

#define R_PCIE_CFG_PX16GP5P6PCM               0x0000052cU      ///< 16.0 GT/s P5 And P6 Preset-Coefficient Mapping

#define R_PCIE_CFG_PX16GP6P7P8PCM             0x00000530U      ///< 16.0 GT/s P6, P7 And P8 Preset-Coefficient Mapping

#define R_PCIE_CFG_PX16GP8P9PCM               0x00000534U      ///< 16.0 GT/s P8 And P9 Preset-Coefficient Mapping

#define R_PCIE_CFG_PX16GP10PCM                0x00000538U      ///< 16.0 GT/s P10 Preset-Coefficient Mapping

#define R_PCIE_CFG_PX16GLFFS                  0x0000053cU      ///< 16.0 GT/s LF And FS

#define R_PCIE_CFG_COCTL                      0x00000594U      ///< Coalescing Control
#define B_PCIE_CFG_COCTL_PWCE                 BIT0             ///< Posted Write Chaining Enable
#define N_PCIE_CFG_COCTL_PWCE                 0
#define B_PCIE_CFG_COCTL_DDCE                 BIT1             ///< Completion Chaining Enable
#define N_PCIE_CFG_COCTL_DDCE                 1
#define B_PCIE_CFG_COCTL_CT                   ( BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 ) ///< Chain Timer
#define N_PCIE_CFG_COCTL_CT                   2
#define B_PCIE_CFG_COCTL_CTE                  BIT10            ///< Chain Timer Enable
#define N_PCIE_CFG_COCTL_CTE                  10
#define B_PCIE_CFG_COCTL_ROAOP                BIT11            ///< Relaxed Order Attribute Override Policy
#define N_PCIE_CFG_COCTL_ROAOP                11
#define B_PCIE_CFG_COCTL_MAGPARCD             BIT12            ///< Maximum Addressable GPA Range Check Disable
#define N_PCIE_CFG_COCTL_MAGPARCD             12
#define B_PCIE_CFG_COCTL_PCLM                 ( BIT14 | BIT13 ) ///< Posted Chain Limit Mode
#define N_PCIE_CFG_COCTL_PCLM                 13
#define B_PCIE_CFG_COCTL_NPCLM                ( BIT16 | BIT15 ) ///< Non-Posted Chain Limit Mode
#define N_PCIE_CFG_COCTL_NPCLM                15
#define B_PCIE_CFG_COCTL_CHAINBARBE           BIT18            ///< Chain Bit Arbitration Enable
#define N_PCIE_CFG_COCTL_CHAINBARBE           18

#define R_PCIE_CFG_LTCO3                      0x00000598U      ///< Local Transmitter Coefficient Override 3

#define R_PCIE_CFG_LTCO4                      0x0000059cU      ///< Local Transmitter Coefficient Override 4

#define R_PCIE_CFG_LTCO5                      0x000005a0U      ///< Local Transmitter Coefficient Override 5

#define R_PCIE_CFG_LTCO6                      0x000005a4U      ///< Local Transmitter Coefficient Override 6

#define R_PCIE_CFG_LTCO7                      0x000005a8U      ///< Local Transmitter Coefficient Override 7

#define R_PCIE_CFG_LTCO8                      0x000005acU      ///< Local Transmitter Coefficient Override 8

#define R_PCIE_CFG_ADVMCTRL                   0x000005bcU      ///< Advance Mode Control
#define B_PCIE_CFG_ADVMCTRL_PMREQBLKPGRSPT    ( BIT7 | BIT6 | BIT5 ) ///< PM_REQ Block and Power Gate Response Time
#define N_PCIE_CFG_ADVMCTRL_PMREQBLKPGRSPT    5
#define B_PCIE_CFG_ADVMCTRL_RLLG12R           BIT10            ///< Reset Link Layer in Gen1, 2 Recovery
#define N_PCIE_CFG_ADVMCTRL_RLLG12R           10
#define B_PCIE_CFG_ADVMCTRL_RRLLCL            BIT11            ///< Reset Receiver Link Layer Common Logic
#define N_PCIE_CFG_ADVMCTRL_RRLLCL            11
#define B_PCIE_CFG_ADVMCTRL_G3STFER           BIT13            ///< Gen3 Short TLP Framing Error Reporting
#define N_PCIE_CFG_ADVMCTRL_G3STFER           13
#define B_PCIE_CFG_ADVMCTRL_RXL0DC            BIT15            ///< RxL0 De-Assertion Control
#define N_PCIE_CFG_ADVMCTRL_RXL0DC            15
#define B_PCIE_CFG_ADVMCTRL_PMREQCWC          ( BIT18 | BIT17 | BIT16 ) ///< PM_REQ Clock Wake Control
#define N_PCIE_CFG_ADVMCTRL_PMREQCWC          16
#define B_PCIE_CFG_ADVMCTRL_EIOSMASKRX        BIT19            ///< EIOS Mask Receiver Datapath
#define N_PCIE_CFG_ADVMCTRL_EIOSMASKRX        19
#define B_PCIE_CFG_ADVMCTRL_EIOSDISDS         BIT20            ///< EIOS Disable DeSkew
#define N_PCIE_CFG_ADVMCTRL_EIOSDISDS         20
#define B_PCIE_CFG_ADVMCTRL_INRXL0CTRL        BIT22            ///< InRxL0 Control
#define N_PCIE_CFG_ADVMCTRL_INRXL0CTRL        22
#define B_PCIE_CFG_ADVMCTRL_CCBE              BIT23            ///< Completion Coalescing Break Event
#define N_PCIE_CFG_ADVMCTRL_CCBE              23
#define B_PCIE_CFG_ADVMCTRL_F10BTSE           BIT24            ///< Fabric 10-bit Tag Support Enable
#define N_PCIE_CFG_ADVMCTRL_F10BTSE           24

#define R_PCIE_CFG_PGTHRES                    0x000005c0U      ///< Power Gating Threshold
#define B_PCIE_CFG_PGTHRES_L1PGLTREN          BIT0             ///< L1 Power Gating LTR Enable
#define N_PCIE_CFG_PGTHRES_L1PGLTREN          0
#define B_PCIE_CFG_PGTHRES_L1PGLTRTLV         0x03ff0000U      ///< L1 Power Gating LTR Threshold Latency Value
#define N_PCIE_CFG_PGTHRES_L1PGLTRTLV         16
#define B_PCIE_CFG_PGTHRES_L1PGLTRTLSV        ( BIT31 | BIT30 | BIT29 ) ///< L1 Power Gating LTR Threshold Latency Scale Value
#define N_PCIE_CFG_PGTHRES_L1PGLTRTLSV        29

#define R_PCIE_CFG_HWSNR                      0x000005f0U      ///< Hardware Save And Restore
#define B_PCIE_CFG_HWSNR_BEPW                 ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Bank Enable Pulse Width
#define N_PCIE_CFG_HWSNR_BEPW                 0
#define B_PCIE_CFG_HWSNR_REPW                 ( BIT7 | BIT6 | BIT5 | BIT4 ) ///< Restore Enable Pulse Width
#define N_PCIE_CFG_HWSNR_REPW                 4
#define B_PCIE_CFG_HWSNR_EEH                  ( BIT9 | BIT8 )  ///< Entry and Exit Hysteresis
#define N_PCIE_CFG_HWSNR_EEH                  8
#define B_PCIE_CFG_HWSNR_READY4PG             BIT10            ///< Ready for Power Gating
#define N_PCIE_CFG_HWSNR_READY4PG             10

#define R_PCIE_CFG_PGCTRL                     0x000005f4U      ///< Power Gating Control
#define B_PCIE_CFG_PGCTRL_PMREQBLKRSPT        ( BIT2 | BIT1 | BIT0 ) ///< PM_REQ Block Response Time
#define N_PCIE_CFG_PGCTRL_PMREQBLKRSPT        0
#define V_PCIE_CFG_PGCTRL_PMREQBLKRSPT_10us   2
#define V_PCIE_CFG_PGCTRL_PMREQBLKRSPT_25us   5

#define R_PCIE_CFG_PX16GLTCO1                 0x00000600U      ///< 16 GT/s Local Transmitter Coefficient Override 1
#define B_PCIE_CFG_PX16GLTCO1_L0TPRECO        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 0 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO1_L0TPRECO        0
#define B_PCIE_CFG_PX16GLTCO1_L0TPOSTCO       ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 0 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO1_L0TPOSTCO       6
#define B_PCIE_CFG_PX16GLTCO1_L1TPRECO        ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 1 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO1_L1TPRECO        12
#define B_PCIE_CFG_PX16GLTCO1_L1TPOSTCO       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 1 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO1_L1TPOSTCO       18
#define B_PCIE_CFG_PX16GLTCO1_L0TCOE          BIT24            ///< Lane 0 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_PX16GLTCO1_L0TCOE          24
#define B_PCIE_CFG_PX16GLTCO1_L1TCOE          BIT25            ///< Lane 1 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_PX16GLTCO1_L1TCOE          25

#define R_PCIE_CFG_PX16GLTCO2                 0x00000604U      ///< 16 GT/s Local Transmitter Coefficient Override 2
#define B_PCIE_CFG_PX16GLTCO2_L2TPRECO        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 2 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO2_L2TPRECO        0
#define B_PCIE_CFG_PX16GLTCO2_L2TPOSTCO       ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 2 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO2_L2TPOSTCO       6
#define B_PCIE_CFG_PX16GLTCO2_L3TPRECO        ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 3 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO2_L3TPRECO        12
#define B_PCIE_CFG_PX16GLTCO2_L3TPOSTCO       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 3 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO2_L3TPOSTCO       18
#define B_PCIE_CFG_PX16GLTCO2_L2TCOE          BIT24            ///< Lane 2 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_PX16GLTCO2_L2TCOE          24
#define B_PCIE_CFG_PX16GLTCO2_L3TCOE          BIT25            ///< Lane 3 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_PX16GLTCO2_L3TCOE          25

#define R_PCIE_CFG_PX16GLTCO3                 0x00000608U      ///< 16 GT/s Local Transmitter Coefficient Override 3
#define B_PCIE_CFG_PX16GLTCO3_L4TPRECO        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 4 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO3_L4TPRECO        0
#define B_PCIE_CFG_PX16GLTCO3_L4TPOSTCO       ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 4 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO3_L4TPOSTCO       6
#define B_PCIE_CFG_PX16GLTCO3_L5TPRECO        ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 5 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO3_L5TPRECO        12
#define B_PCIE_CFG_PX16GLTCO3_L5TPOSTCO       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 5 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO3_L5TPOSTCO       18
#define B_PCIE_CFG_PX16GLTCO3_L4TCOE          BIT24            ///< Lane 4 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_PX16GLTCO3_L4TCOE          24
#define B_PCIE_CFG_PX16GLTCO3_L5TCOE          BIT25            ///< Lane 5 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_PX16GLTCO3_L5TCOE          25

#define R_PCIE_CFG_PX16GLTCO4                 0x0000060CU      ///< 16 GT/s Local Transmitter Coefficient Override 4
#define B_PCIE_CFG_PX16GLTCO4_L6TPRECO        ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Lane 6 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO4_L6TPRECO        0
#define B_PCIE_CFG_PX16GLTCO4_L6TPOSTCO       ( BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Lane 6 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO4_L6TPOSTCO       6
#define B_PCIE_CFG_PX16GLTCO4_L7TPRECO        ( BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 ) ///< Lane 7 Transmitter Pre-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO4_L7TPRECO        12
#define B_PCIE_CFG_PX16GLTCO4_L7TPOSTCO       ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 ) ///< Lane 7 Transmitter Post-Cursor Coefficient Override
#define N_PCIE_CFG_PX16GLTCO4_L7TPOSTCO       18
#define B_PCIE_CFG_PX16GLTCO4_L6TCOE          BIT24            ///< Lane 6 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_PX16GLTCO4_L6TCOE          24
#define B_PCIE_CFG_PX16GLTCO4_L7TCOE          BIT25            ///< Lane 7 Transmitter Coefficient Override Enable
#define N_PCIE_CFG_PX16GLTCO4_L7TCOE          25

#define R_PCIE_CFG_GDR                        0x00000690U      ///< General Debug Register
#define B_PCIE_CFG_GDR_GPIOALD                BIT10            ///< GPIO Assertion on Link Down
#define N_PCIE_CFG_GDR_GPIOALD                10

#define R_PCIE_CFG_ACRG3                      0x000006ccU      ///< Advance Control Register Group 3
#define B_PCIE_CFG_ACRG3_ADESKEW_DIS          BIT10            ///< Adapted Deskew Disable
#define N_PCIE_CFG_ACRG3_ADESKEW_DIS          10
#define B_PCIE_CFG_ACRG3_CBGM                 BIT21            ///< Chain Bit Generation Mode
#define N_PCIE_CFG_ACRG3_CBGM                 21
#define B_PCIE_CFG_ACRG3_CPGWAKECTRL          ( BIT23 | BIT22 ) ///< CPG Wake Control
#define N_PCIE_CFG_ACRG3_CPGWAKECTRL          22
#define B_PCIE_CFG_ACRG3_RRXDME               BIT27            ///<  Redo Receiver Detection Mechanism Enable
#define N_PCIE_CFG_ACRG3_RRXDME               27

#define R_PCIE_CFG_DPCECH                     0x00000a00U      ///< DPC Extended Capability Header
#define B_PCIE_CFG_DPCECH_CID                 0x0000ffffU      ///< Capability ID
#define N_PCIE_CFG_DPCECH_CID                 0
#define B_PCIE_CFG_DPCECH_CV                  ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_DPCECH_CV                  16
#define B_PCIE_CFG_DPCECH_NCO                 0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_DPCECH_NCO                 20

#define R_PCIE_CFG_DPCCAPR                    0x00000a04U      ///< DPC Capability Register
#define B_PCIE_CFG_DPCCAPR_RPEFDPC            BIT5
#define N_PCIE_CFG_DPCCAPR_RPEFDPC            5

#define R_PCIE_CFG_SPEECH                     0x00000a30U      ///< Secondary PCI Express Extended Capability Header
#define B_PCIE_CFG_SPEECH_PCIEECID            0x0000ffffU      ///< PCI Express Extended Capability ID
#define N_PCIE_CFG_SPEECH_PCIEECID            0
#define B_PCIE_CFG_SPEECH_CV                  ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_SPEECH_CV                  16
#define V_PCIE_CFG_SPEECH_CV                  0x1
#define B_PCIE_CFG_SPEECH_NCO                 0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_SPEECH_NCO                 20

#define R_PCIE_CFG_LCTL3                      0x00000a34U      ///< Link Control 3
#define B_PCIE_CFG_LCTL3_PE                   BIT0             ///< Perform Equalization
#define N_PCIE_CFG_LCTL3_PE                   0

#define R_PCIE_CFG_LES                        0x00000a38U      ///< Lane Error Status

#define R_PCIE_CFG_L01EC                      0x00000a3cU      ///< Lane 0 And Lane 1 Equalization Control

#define R_PCIE_CFG_L23EC                      0x00000a40U      ///< Lane 2 And Lane 3 Equalization Control

#define R_PCIE_CFG_L45EC                      0x00000a44U      ///< Lane 4 And Lane 5 Equalization Control

#define R_PCIE_CFG_L67EC                      0x00000a48U      ///< Lane 6 And Lane 7 Equalization Control

#define R_PCIE_CFG_DLFECH                     0x00000a90U      ///< Data Link Feature Extended Capability Header
#define B_PCIE_CFG_DLFECH_PCIECID             0x0000ffffU      ///< PCI Express Extended Capability ID
#define N_PCIE_CFG_DLFECH_PCIECID             0
#define B_PCIE_CFG_DLFECH_CV                  ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_DLFECH_CV                  16
#define B_PCIE_CFG_DLFECH_NCO                 0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_DLFECH_NCO                 20

#define R_PCIE_CFG_DLFCAP                     0x00000a94U      ///< Data Link Feature Capabilities Register
#define B_PCIE_CFG_DLFCAP_LSFCS               BIT0             ///< Local Scaled Flow Control Supported
#define N_PCIE_CFG_DLFCAP_LSFCS               0

#define R_PCIE_CFG_PL16GECH                   0x00000a9cU      ///< Physical Layer 16.0 GT/s Extended Capability Header
#define B_PCIE_CFG_PL16GECH_PCIECID           0x0000ffffU      ///< PCI Express Extended Capability ID
#define N_PCIE_CFG_PL16GECH_PCIECID           0
#define B_PCIE_CFG_PL16GECH_CV                ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_PL16GECH_CV                16
#define B_PCIE_CFG_PL16GECH_NCO               0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_PL16GECH_NCO               20

#define R_PCIE_CFG_PL16S                      0x00000aa8U      ///< Physical Layer 16.0 GT/s Status Register
#define B_PCIE_CFG_PL16S_EQP3SG4              BIT3             ///< Equalization 16.0 GT/s Phase 3 Successful
#define N_PCIE_CFG_PL16S_EQP3SG4              3

#define R_PCIE_CFG_PL16L01EC                  0x00000abcU      ///< Physical Layer 16.0 GT/s Lane 01 Equalization Control Register

#define R_PCIE_CFG_PL16L23EC                  0x00000abeU      ///< Physical Layer 16.0 GT/s Lane 23 Equalization Control Register

#define R_PCIE_CFG_PL16L45EC                  0x00000ac0U      ///< Physical Layer 16.0 GT/s Lane 45 Equalization Control Register

#define R_PCIE_CFG_PL16L67EC                  0x00000ac2U      ///< Physical Layer 16.0 GT/s Lane 67 Equalization Control Register

#define R_PCIE_CFG_PL16L89EC                  0x00000ac4U      ///< Physical Layer 16.0 GT/s Lane 89 Equalization Control Register

#define R_PCIE_CFG_PL16L1011EC                0x00000ac6U      ///< Physical Layer 16.0 GT/s Lane 1011 Equalization Control Register

#define R_PCIE_CFG_PL16L1213EC                0x00000ac8U      ///< Physical Layer 16.0 GT/s Lane 1213 Equalization Control Register

#define R_PCIE_CFG_PL16L1415EC                0x00000acaU      ///< Physical Layer 16.0 GT/s Lane 1415 Equalization Control Register

#define R_PCIE_CFG_G5ECH                      0x00000adcU      ///< Physical Layer 32.0 GT/s Extended Capability Header
#define B_PCIE_CFG_G5ECH_ECID                 0x0000ffffU      ///< PCI Express Extended Capability ID
#define N_PCIE_CFG_G5ECH_ECID                 0
#define B_PCIE_CFG_G5ECH_CV                   ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_G5ECH_CV                   16
#define B_PCIE_CFG_G5ECH_NCO                  0xfff00000U      ///<  Next Capability Offset
#define N_PCIE_CFG_G5ECH_NCO                  20

#define R_PCIE_CFG_G5CAP                      0x00000ae0U      /// Physical Layer  32.0 GT/s Capability Register
#define B_PCIE_CFG_G5STS_EQBYPSUP             BIT0             /// Equalization bypass to highest rate Supported

#define R_PCIE_CFG_G5CTL                      0x00000ae4U      /// Physical Layer  32.0 GT/s Control Register
#define B_PCIE_CFG_G5STS_EQBYPDIS             BIT0             /// Equalization bypass to highest rate Disable

#define R_PCIE_CFG_G5STS                      0x00000ae8U      ///< Physical Layer  32.0 GT/s Status Register
#define B_PCIE_CFG_G5STS_EQ32PH3SUCC          BIT3             ///< Equalization 32.0 GT/s Phase 3 Successfu
#define N_PCIE_CFG_G5STS_EQ32PH3SUCC          3

#define R_PCIE_CFG_G5LANEEQCTL_0              0x00000afcU      ///< 32.0 GT/s Lane 0123 Equalization Control Register

#define R_PCIE_CFG_APEC                       0x00000b0cU      ///< Alternate Protocol Extended Capability Header
#define B_PCIE_CFG_APEC_PCIECID               0x0000ffffU      ///< PCI Express Extended Capability ID
#define N_PCIE_CFG_APEC_PCIECID               0
#define B_PCIE_CFG_APEC_CV                    ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_APEC_CV                    16
#define B_PCIE_CFG_APEC_NCO                   0xfff00000U      ///<  Next Capability Offset
#define N_PCIE_CFG_APEC_NCO                   20

#define R_PCIE_CFG_ACGR3S2                    0x00000c50U      ///< Advance Control Register Group 3 Set 2
#define B_PCIE_CFG_ACGR3S2_LSTPTLS            ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< LSTP Target Link Speed
#define N_PCIE_CFG_ACGR3S2_LSTPTLS            0
#define B_PCIE_CFG_ACGR3S2_DRXTERMDQ          BIT4             ///< Disabled RxTermination in Detect.Quiet
#define N_PCIE_CFG_ACGR3S2_DRXTERMDQ          4
#define B_PCIE_CFG_ACGR3S2_SRT                BIT5             ///< Simplified Replay Timer
#define N_PCIE_CFG_ACGR3S2_SRT                5
#define B_PCIE_CFG_ACGR3S2_G1EBM              BIT6             ///< Gen 1 Elastic Buffer Mode
#define N_PCIE_CFG_ACGR3S2_G1EBM              6
#define B_PCIE_CFG_ACGR3S2_G2EBM              BIT7             ///< Gen 2 Elastic Buffer Mode
#define N_PCIE_CFG_ACGR3S2_G2EBM              7
#define B_PCIE_CFG_ACGR3S2_G3EBM              BIT8             ///< Gen 3 Elastic Buffer Mode
#define N_PCIE_CFG_ACGR3S2_G3EBM              8
#define B_PCIE_CFG_ACGR3S2_G4EBM              BIT9             ///< Gen 4 Elastic Buffer Mode
#define N_PCIE_CFG_ACGR3S2_G4EBM              9
#define B_PCIE_CFG_ACGR3S2_G5EBM              BIT20            ///< Gen 5 Elastic Buffer Mode
#define N_PCIE_CFG_ACGR3S2_G5EBM              20

#define R_PCIE_CFG_LTRSUBL1STD                0x00000c5cU      ///< LTR Subtraction for L1.Standard

#define R_PCIE_CFG_LTRSUBL11                  0x00000c60U      ///< LTR Subtraction for L1.1 Power Gate

#define R_PCIE_CFG_LTRSUBL12                  0x00000c64U      ///< LTR Subtraction for L1.2
#define B_PCIE_CFG_LTRSUBL12_LTRSLSUBV        0x000003ffU      ///< LTR Snoop Latency Subtraction Value
#define N_PCIE_CFG_LTRSUBL12_LTRSLSUBV        0
#define B_PCIE_CFG_LTRSUBL12_LTRSLSSUBV       ( BIT12 | BIT11 | BIT10 ) ///< LTR Snoop Latency Scale Subtraction Value
#define N_PCIE_CFG_LTRSUBL12_LTRSLSSUBV       10
#define B_PCIE_CFG_LTRSUBL12_LTRSLSUBEN       BIT15            ///< LTR Snoop Latency Subtraction Enable
#define N_PCIE_CFG_LTRSUBL12_LTRSLSUBEN       15
#define B_PCIE_CFG_LTRSUBL12_LTRNSLSUBV       0x03ff0000U      ///< LTR Non-Snoop Latency Subtraction Value
#define N_PCIE_CFG_LTRSUBL12_LTRNSLSUBV       16
#define B_PCIE_CFG_LTRSUBL12_LTRNSLSSUBV      ( BIT28 | BIT27 | BIT26 ) ///< LTR Non-Snoop Latency Scale Subtraction Value
#define N_PCIE_CFG_LTRSUBL12_LTRNSLSSUBV      26
#define B_PCIE_CFG_LTRSUBL12_LTRNSLSUBEN      BIT31            ///< LTR Non-Snoop Latency Subtraction Enable
#define N_PCIE_CFG_LTRSUBL12_LTRNSLSUBEN      31

#define R_PCIE_CFG_LTRSUBL11NPG               0x00000c68U      ///< LTR Subtraction for L1.1 No Power Gate

#define R_PCIE_CFG_VNNREMCTL                  0x00000c70U      ///< VNN Removal Control
#define B_PCIE_CFG_VNNREMCTL_LRSLFVNNRE       ( BIT1 | BIT0 )  ///< Link Reset Suppression Latency For VNN Removal Exit
#define N_PCIE_CFG_VNNREMCTL_LRSLFVNNRE       0
#define B_PCIE_CFG_VNNREMCTL_ISPLFVNNRE       ( BIT3 | BIT2 )  ///< Internal States Propagation Latency For VNN Removal Exit
#define N_PCIE_CFG_VNNREMCTL_ISPLFVNNRE       2

#define R_PCIE_CFG_VNNRSNRC1                  0x00000c74U      ///< VNN Removal Save And Restore Hardware Contexts 1

#define R_PCIE_CFG_RXQC                       0x00000c7cU      ///< Receive Queue Configurations
#define B_PCIE_CFG_RXQC_V0RQPCOC              ( BIT1 | BIT0 )  ///< VC0 Receive Queue Posted Credit Offset Control
#define N_PCIE_CFG_RXQC_V0RQPCOC              0
#define B_PCIE_CFG_RXQC_V0RQNPCOC             ( BIT3 | BIT2 )  ///< VC0 Receive Queue Non-Posted Credit Offset Control
#define N_PCIE_CFG_RXQC_V0RQNPCOC             2
#define B_PCIE_CFG_RXQC_V1RQPCOC              ( BIT7 | BIT6 )  ///< VC1 Receive Queue Posted Credit Offset Control
#define N_PCIE_CFG_RXQC_V1RQPCOC              6
#define B_PCIE_CFG_RXQC_V1RQNPCOC             ( BIT9 | BIT8 )  ///< VC1 Receive Queue Non-Posted Credit Offset Control
#define N_PCIE_CFG_RXQC_V1RQNPCOC             8
#define B_PCIE_CFG_RXQC_VMRQPCOC              ( BIT13 | BIT12 ) ///< VCm Receive Queue Posted Credit Offset Control
#define N_PCIE_CFG_RXQC_VMRQPCOC              12
#define B_PCIE_CFG_RXQC_VMRQNPCOC             ( BIT15 | BIT14 ) ///< VCm Receive Queue Non-Posted Credit Offset Control
#define N_PCIE_CFG_RXQC_VMRQNPCOC             14

#define R_PCIE_CFG_AECR1G3                    0x00000c80U      ///< Advance Extended Control Register 1 Group 3
#define B_PCIE_CFG_AECR1G3_DCDCCTDT           BIT0             ///< Disable Config.Dskew to Config.Complete transition delay timer
#define N_PCIE_CFG_AECR1G3_DCDCCTDT           0
#define B_PCIE_CFG_AECR1G3_L1OFFRDYHEWTEN     BIT6             ///< L1OFFRDY Host Exit Wait Time Enable
#define N_PCIE_CFG_AECR1G3_L1OFFRDYHEWTEN     6
#define B_PCIE_CFG_AECR1G3_L1OFFRDYHEWT       ( BIT9 | BIT8 | BIT7 ) ///< L1OFFRDY Host Exit Wait Time
#define N_PCIE_CFG_AECR1G3_L1OFFRDYHEWT       7
#define B_PCIE_CFG_AECR1G3_TPSE               BIT10            ///< Transmitted Preset Sticky Enable
#define N_PCIE_CFG_AECR1G3_TPSE               10
#define B_PCIE_CFG_AECR1G3_DTCGCM             BIT14            ///< Dynamic and Trunk Clock Gating Coupling Mode
#define N_PCIE_CFG_AECR1G3_DTCGCM             14
#define B_PCIE_CFG_AECR1G3_FCPTODIS           BIT23            ///< FCP Timeout Mechanism Disabled
#define N_PCIE_CFG_AECR1G3_FCPTODIS           23
#define B_PCIE_CFG_AECR1G3_REQTMBOF           BIT31            ///< RxEqTrain Message Bus Offset Mapping
#define N_PCIE_CFG_AECR1G3_REQTMBOF           31

#define R_PCIE_CFG_AECR2G3                    0x00000c84U      ///< Advance Extended Control Register 2 Group 3
#define N_PCIE_CFG_AECR2G3_RSVD_RW2           8
#define V_PCIE_CFG_AECR2G3_RSVD_RW2           0x2

#define R_PCIE_CFG_LPCRE                      0x00000c88U      ///< Lock Policy Control Register Extension
#define B_PCIE_CFG_LPCRE_IPCL                 BIT0             ///< IP Capability Lock
#define N_PCIE_CFG_LPCRE_IPCL                 0
#define B_PCIE_CFG_LPCRE_IPVCCAPL             BIT8             ///< IP VC Capability Lock
#define N_PCIE_CFG_LPCRE_IPVCCAPL             8

#define R_PCIE_CFG_LPCR                       0x00000c8cU      ///< Lock Policy Control Register
#define B_PCIE_CFG_LPCR_SRL                   BIT0             ///< Secured Register Lock
#define N_PCIE_CFG_LPCR_SRL                   0
#define B_PCIE_CFG_LPCR_SERL                  BIT8             ///< Secure Equalization Register Lock
#define N_PCIE_CFG_LPCR_SERL                  8
#define B_PCIE_CFG_LPCR_LTRCFGLOCK            BIT16            ///< LTR Configuration Lock
#define N_PCIE_CFG_LPCR_LTRCFGLOCK            16
#define B_PCIE_CFG_LPCR_DIDOVR_LOCK           BIT24            ///< Device ID Override Lock
#define N_PCIE_CFG_LPCR_DIDOVR_LOCK           24

#define R_PCIE_CFG_RXMC1                      0x00000c90U      ///< RX Margin Control 1
#define B_PCIE_CFG_RXMC1_MIESS                BIT0             ///< Margin Independent Error Sampler Support
#define N_PCIE_CFG_RXMC1_MIESS                0
#define B_PCIE_CFG_RXMC1_MIUDVMS              BIT1             ///< Margin Independent Up Down Voltage Margin Support
#define N_PCIE_CFG_RXMC1_MIUDVMS              1
#define B_PCIE_CFG_RXMC1_MILRTS               BIT2             ///< Margin Independent Left Right Timing Support
#define N_PCIE_CFG_RXMC1_MILRTS               2
#define B_PCIE_CFG_RXMC1_MVS                  BIT3             ///< Margin Voltage Support
#define N_PCIE_CFG_RXMC1_MVS                  3
#define B_PCIE_CFG_RXMC1_MMNOLS               ( BIT8 | BIT7 | BIT6 | BIT5 | BIT4 ) ///<  Margin Max Number Of Lanes Support
#define N_PCIE_CFG_RXMC1_MMNOLS               4
#define B_PCIE_CFG_RXMC1_MSRM                 BIT9             ///< Margin Sample Reporting Method
#define N_PCIE_CFG_RXMC1_MSRM                 9
#define B_PCIE_CFG_RXMC1_VMSLOP               ( BIT12 | BIT11 ) ///< Voltage Margin Steps Limit On PIPE
#define N_PCIE_CFG_RXMC1_VMSLOP               11
#define B_PCIE_CFG_RXMC1_TMSLOP               ( BIT14 | BIT13 ) ///< Time Margin Steps Limit On PIPE
#define N_PCIE_CFG_RXMC1_TMSLOP               13
#define B_PCIE_CFG_RXMC1_MSRTS                ( BIT25 | BIT24 | BIT23 | BIT22 | BIT21 | BIT20 ) ///< Margin Sampling Rate Timing Support
#define N_PCIE_CFG_RXMC1_MSRTS                20
#define B_PCIE_CFG_RXMC1_MSRVS                ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 ) ///< Margin Sampling Rate Voltage Support
#define N_PCIE_CFG_RXMC1_MSRVS                26

#define R_PCIE_CFG_RXMC2                      0x00000c94U      ///< RX Margin Control 2
#define B_PCIE_CFG_RXMC2_MMVOS                ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Margin Max Voltage Offset Support
#define N_PCIE_CFG_RXMC2_MMVOS                0
#define B_PCIE_CFG_RXMC2_MNOVSS               ( BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 ) ///< Margin Number Of Voltage Steps Support
#define N_PCIE_CFG_RXMC2_MNOVSS               6
#define B_PCIE_CFG_RXMC2_MMTOS                ( BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT13 ) ///< Margin Max Timing Offset Support
#define N_PCIE_CFG_RXMC2_MMTOS                13
#define B_PCIE_CFG_RXMC2_MNOTSS               ( BIT24 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 ) ///< Margin Number Of Timing Steps Support
#define N_PCIE_CFG_RXMC2_MNOTSS               19

#define R_PCIE_CFG_EINJCTL                    0x00000ca8U      ///< EINJ Control
#define B_PCIE_CFG_EINJCTL_EINJDIS            BIT0             ///< Error Injection Disable
#define N_PCIE_CFG_EINJCTL_EINJDIS            0

#define R_PCIE_CFG_PL16MECH                   0x00000edcU      ///< Physical Layer 16.0 GT/s Margining Extended Capability Header
#define B_PCIE_CFG_PL16MECH_PCIECID           0x0000ffffU      ///< PCI Express Extended Capability ID
#define N_PCIE_CFG_PL16MECH_PCIECID           0
#define B_PCIE_CFG_PL16MECH_CV                ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< Capability Version
#define N_PCIE_CFG_PL16MECH_CV                16
#define B_PCIE_CFG_PL16MECH_NCO               0xfff00000U      ///< Next Capability Offset
#define N_PCIE_CFG_PL16MECH_NCO               20

#define R_PCIE_CFG_PL16MPCPSB01               0x00000ee0U      ///< Physical Layer 16.0 GT/s Margining Port Capabilities and Port Status Byte 0 & 1
#define B_PCIE_CFG_PL16MPCPSB01_MARGINDRISW   BIT0             ///< Margining uses Driver Software
#define N_PCIE_CFG_PL16MPCPSB01_MARGINDRISW   0

#define R_PCIE_CFG_MEMBARCTL                  0xF00
#define R_PCIE_CFG_MEMBARCTL_MEMBAREN         BIT0

#define R_PCIE_CFG_PCIERTP2                   0x304
#define B_PCIE_CFG_PCIERTP2_G3X1              BIT8
#define B_PCIE_CFG_PCIERTP2_G3X1_OFFSET       8
#define B_PCIE_CFG_PCIERTP2_G3X1_MASK         0xF00
#define B_PCIE_CFG_PCIERTP2_G3X2              BIT4
#define B_PCIE_CFG_PCIERTP2_G3X2_OFFSET       4
#define B_PCIE_CFG_PCIERTP2_G3X2_MASK         0xF0
#define B_PCIE_CFG_PCIERTP2_G3X4              BIT0
#define B_PCIE_CFG_PCIERTP2_G3X4_OFFSET       0
#define B_PCIE_CFG_PCIERTP2_G3X4_MASK         0xF

#define R_PCIE_RCRB_TXMDEC1                   0x17A0
#define B_PCIE_RCRB_CRIDBNCD                  BIT0
#define B_PCIE_RCRB_MRBIDBNCD                 BIT1
#endif
