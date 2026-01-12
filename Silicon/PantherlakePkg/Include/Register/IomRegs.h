/** @file
  Register definition for IOM IP

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

#ifndef _IOM_REGS_H_
#define _IOM_REGS_H_

#include <IpWrapperBitDef.h>

#define V_IOM_BIOS_UPDATE_DEVEN_CMD     0x1
#define V_IOM_BIOS_GEM_SB_TRAN_CMD      0x2
#define V_IOM_BIOS_TCSTATE_LIMIT_CMD    0x7

#define V_IOM_BIOS_MBX_GET_HPD_COUNT    0x3
#define V_IOM_BIOS_MBX_DEC_HPD_COUNT    0x6

#define R_IOM_PCR_IOM_CSME_IMR_MG_STATUS        0x00000010U       ///< IOM CSME IMR MG STATUS REG

#define R_IOM_PCR_IOM_CSME_IMR_TBT_STATUS       0x00000014U       ///< Thunderbolt Firmware Status in IMR

#define R_IOM_PCR_RECEPTACLE_SBU1_BIAS_CTL_0    0x00001070U       ///< RECEPTACLE SBU1 BIAS CTL

#define R_IOM_PCR_RECEPTACLE_SBU2_BIAS_CTL_0    0x00001088U       ///< RECEPTACLE SBU2 BIAS CTL

#define R_IOM_PCR_IOM_FW_INFO                   0x00001900U       ///< IOM FW INFO

#define R_IOM_PCR_IOM_FW_INFO_ENGR              0x00001904U       ///< IOM FW INFO ENGR

#define R_IOM_PCR_AUX_ISO_CTRL_0                0x00001ac0U       ///< IOM AUX ORI BIAS CTRL

#define R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1  0x00000040U       ///< IOM_TYPEC_SW_CONFIGURATION 1
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1_D3_COLD_EN               BIT9 ///< D3 COLD EN
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1_EC_REPLAY_CONNECTION_S3  BIT13 ///< EC REPLAY CONNECTION S3
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1_EC_REPLAY_CONNECTION_S4  BIT14 ///< EC REPLAY CONNECTION S4S5
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1_D3_COLD_REQ              BIT15 ///< D3 COLD REQ
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1_CRID_EN                  BIT18 ///< CRID EN
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1_LOCK                     BIT31 ///< LOCK FIELD
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1_USB3_INIT_DONE           BIT24 ///< USB3 INIT DONE
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1_USB4_HR_INIT_DONE        BIT25 ///< USB4 HR INIT DONE
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1_PCIE_INIT_DONE           BIT26 ///< PCIE INIT DONE

#define R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_2  0x00000044U       ///< IOM_TYPEC_SW_CONFIGURATION 2
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_2_DATA 0x7fffffffU   ///< DATA FIELD
                                                                  ///  Product specific feature settings
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_2_LOCK BIT31         ///< LOCK Field
                                                                  ///  Locks this register from furtherchanges

#define R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_3  0x00000048U       ///< IOM_TYPEC_SW_CONFIGURATION 3

#define V_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_3_WAKE_IGFX_FROM_D3_EN BIT12 ///< Wake iGfx from the D3
                                                               /* IOM issues HPD_ASSERT or HPD_IRQ associated with
                                                                * DP_ALT and DP tunneling connection.
                                                                */

#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_3_LOCK BIT31         ///< LOCK FIELD

#define R_IOM_PCR_IOM_TYPEC_STATUS_1            0x00000050U       ///< IOM TYPEC STATUS 1 REG
#define B_IOM_PCR_IOM_TYPEC_STATUS_1_IOM_READY BIT30              ///< IOM READY

#define R_IOM_PCR_IOM_BIOS_MAILBOX_CMD          0x00000070U       ///< BIOS_MAILBOX_CMD_REG Register
#define B_IOM_PCR_IOM_BIOS_MAILBOX_CMD_RUN_BUSY BIT31             ///< RUN_BUSY

#define R_IOM_PCR_IOM_BIOS_MAILBOX_DATA         0x00000074U       ///< BIOS_MAILBOX_DATA_REG Register

#define R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_4  0x00000098U       ///< IOM_TYPEC_SW_CONFIGURATION 4
#define B_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_4_LOCK BIT31         ///< LOCK FIELD

#define R_IOM_PCR_PRODUCT_INFO                  0x000000b8U       ///< PRODUCT INFO REGISTER
#define B_IOM_PCR_PRODUCT_INFO_DASH_STEPPING_ID ( BIT3 | BIT2 | BIT1 | BIT0 ) ///< Dash Stepping Id
#define N_IOM_PCR_PRODUCT_INFO_DASH_STEPPING_ID 0
#define B_IOM_PCR_PRODUCT_INFO_BASE_STEPPING_ID ( BIT11 | BIT10 | BIT9 | BIT8 ) ///< Base Stepping Id
#define N_IOM_PCR_PRODUCT_INFO_BASE_STEPPING_ID 8
#define B_IOM_PCR_PRODUCT_INFO_SKU_ID         ( BIT19 | BIT18 | BIT17 | BIT16 ) ///< SKU Id
#define N_IOM_PCR_PRODUCT_INFO_SKU_ID         16

///
///
///
#define R_IOM_PCR_IOM_TYPEC_CONFIGURATION_1     0x0000003cU        ///< IOM TYPEC CONFIG 1 REGISTER
#define B_IOM_PCR_IOM_TYPEC_CONFIGURATION_1_FIXED_CONNECTION_PORT1 ( BIT3 | BIT2 | BIT1 | BIT0 )      ///< FIXED CONNECTION PORT1
#define N_IOM_PCR_IOM_TYPEC_CONFIGURATION_1_FIXED_CONNECTION_PORT2 4
#define B_IOM_PCR_IOM_TYPEC_CONFIGURATION_1_FIXED_CONNECTION_PORT2 ( BIT7 | BIT6 | BIT5 | BIT4 )      ///< FIXED CONNECTION PORT2
#define B_IOM_PCR_IOM_TYPEC_CONFIGURATION_1_FIXED_CONNECTION_PORT3 ( BIT11 | BIT10 | BIT9 | BIT8 )    ///< FIXED CONNECTION PORT3
#define B_IOM_PCR_IOM_TYPEC_CONFIGURATION_1_FIXED_CONNECTION_PORT4 ( BIT15 | BIT14 | BIT13 | BIT12 )  ///< FIXED CONNECTION PORT4
#define B_IOM_PCR_IOM_TYPEC_CONFIGURATION_1_FIXED_CONNECTION_PORT5 ( BIT19 | BIT18 | BIT17 | BIT16 )  ///< FIXED CONNECTION PORT5
#define B_IOM_PCR_IOM_TYPEC_CONFIGURATION_1_FIXED_CONNECTION_PORT6 ( BIT23 | BIT22 | BIT21 | BIT20 )  ///< FIXED CONNECTION PORT6
///

#endif // _IOM_REGS_H_
