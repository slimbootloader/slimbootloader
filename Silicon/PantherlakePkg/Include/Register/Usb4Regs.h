/** @file
  Register names for USB4 IP

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

#ifndef _USB4_REGS_H_
#define _USB4_REGS_H_

//
// Custom definitions
//
#define V_HOST_DMA_ITBT_SECURITY_LEVEL0           0x0
#define V_HOST_DMA_ITBT_SECURITY_LEVEL5           0x5
#define V_HOST_DMA_DMA_ACTIVE_DELAY_VALUE         0x22
//
// End if custom definitions

#define R_USB4_CFG_TBT_DMA_CFG_PM_CAP_1       0x00000084U      ///< PM Capbility 1 Control and Status
                                                               /// new name: R_USB4_CFG_TBT_PMREG_DW1

#define R_USB4_CFG_TBT_DMA_CFG_VS_CAP_2       0x000000acU      ///< VS_CAP_2
                                                               /// new name: R_USB4_CFG_TBT_VESC_REG0

#define R_USB4_CFG_TBT_DMA_CFG_VS_CAP_9       0x000000c8U      ///< VS_CAP_9
                                                               /// new name: R_USB4_CFG_TBT_VESC_REG7

#define B_USB4_CFG_TBT_DMA_CFG_VS_CAP_9_FW_READY BIT31         ///< FW READY

#define R_USB4_CFG_TBT_DMA_CFG_VS_CAP_10      0x000000ccU      ///< VS CAP 10
                                                               /// new name: R_USB4_CFG_TBT_VESC_REG8

#define R_USB4_CFG_TBT_DMA_CFG_VS_CAP_11      0x000000d0U      ///< VS CAP 11
                                                               /// new name: R_USB4_CFG_TBT_VESC_REG9

#define R_USB4_CFG_TBT_DMA_CFG_VS_CAP_16      0x000000e4U      ///< VS_CAP_16
                                                               /// new name: R_USB4_CFG_TBT_VESC_REG14

#define R_USB4_CFG_TBT_DMA_CFG_VS_CAP_17      0x000000e8U      ///< VS CAP 17
                                                               /// new name: R_USB4_CFG_TBT_VESC_REG15

#define R_USB4_CFG_TBT_DMA_CFG_VS_CAP_22      0x000000fcU      ///< VS CAP 22 YFL Vendor Configuration Bits
                                                               /// new name: R_USB4_CFG_TBT_VESC_REG20

#define B_USB4_CFG_TBT_DMA_CFG_VS_CAP_22_FORCE_POWER BIT1      ///< Force Power
#define B_USB4_CFG_TBT_DMA_CFG_VS_CAP_22_DMA_ACTIVE_DELAY ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 ) ///< dma active delay
#define N_USB4_CFG_TBT_DMA_CFG_VS_CAP_22_DMA_ACTIVE_DELAY 24

#endif // _USB4_REGS_H_
