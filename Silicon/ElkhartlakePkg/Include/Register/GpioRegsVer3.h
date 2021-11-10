/** @file
  Register names for VER3 GPIO

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

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _GPIO_REGS_VER3_H_
#define _GPIO_REGS_VER3_H_

//
// PCH GPIO
//
#define GPIO_VER3_PCH_GPIO_GROUP_MAX        20

#define GPIO_VER3_PCH_GPIO_GPP_B_PAD_MAX    26
#define GPIO_VER3_PCH_GPIO_GPP_T_PAD_MAX    16
#define GPIO_VER3_PCH_GPIO_GPP_G_PAD_MAX    25
#define GPIO_VER3_PCH_GPIO_GPP_V_PAD_MAX    16
#define GPIO_VER3_PCH_GPIO_GPP_H_PAD_MAX    24
#define GPIO_VER3_PCH_GPIO_GPP_D_PAD_MAX    21
#define GPIO_VER3_PCH_GPIO_GPP_U_PAD_MAX    24
#define GPIO_VER3_PCH_GPIO_VGPIO_PAD_MAX    28
#define GPIO_VER3_PCH_GPIO_GPD_PAD_MAX      17
#define GPIO_VER3_PCH_GPIO_CPU_PAD_MAX      17
#define GPIO_VER3_PCH_GPIO_GPP_S_PAD_MAX    2
#define GPIO_VER3_PCH_GPIO_GPP_A_PAD_MAX    24
#define GPIO_VER3_PCH_GPIO_VGPIO_3_PAD_MAX  4
#define GPIO_VER3_PCH_GPIO_GPP_C_PAD_MAX    24
#define GPIO_VER3_PCH_GPIO_GPP_F_PAD_MAX    25
#define GPIO_VER3_PCH_GPIO_HVCMOS_PAD_MAX   6
#define GPIO_VER3_PCH_GPIO_GPP_E_PAD_MAX    25
#define GPIO_VER3_PCH_GPIO_JTAG_PAD_MAX     9
#define GPIO_VER3_PCH_GPIO_GPP_R_PAD_MAX    8
#define GPIO_VER3_PCH_GPIO_SPI_PAD_MAX      9

//
// PCH GPIO registers
//
//
// GPIO Community Common Private Configuration Registers
//
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_B     0x0
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_T     0x1
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_A     0x2
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_R     0x3
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPD       0x4 // DSW
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_V     0x5
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_H     0x6
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_D     0x7
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_U     0x8
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_VGPIO     0x9
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_F     0xA
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_C     0xB
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_E     0xC
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_G     0xD
#define V_GPIO_VER3_PCH_GPIO_PCR_MISCCFG_GPE0_GPP_S     0xE
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_B     0x0
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_T     0x1
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_D     0x2
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_A     0x3 // Will redirect to GPP_V
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_R     0x4 // Will redirect to GPP_V
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_V     0x5
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPD       0x6 // DSW
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_H     0x7 // Will redirect to HVCMOS
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_U     0x8 // Will redirect to GPP_E
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_VGPIO     0x9
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_F     0xA
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_C     0xB
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_E     0xC
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_G     0xD
#define V_GPIO_VER3_PCH_A0_GPIO_PCR_MISCCFG_GPE0_GPP_S     0xE

//
// GPIO Community 0 Private Configuration Registers
//
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_PAD_OWN          0x20
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_PAD_OWN          0x30
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_PAD_OWN          0x38

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_PADCFGLOCK       0x80
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_PADCFGLOCKTX     0x84
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_PADCFGLOCK       0x88
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_PADCFGLOCKTX     0x8C
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_PADCFGLOCK       0x90
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_PADCFGLOCKTX     0x94

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_HOSTSW_OWN       0xB0
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_HOSTSW_OWN       0xB4
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_HOSTSW_OWN       0xB8

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_GPI_IS           0x0100
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_GPI_IS           0x0104
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_GPI_IS           0x0108

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_GPI_IE           0x0120
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_GPI_IE           0x0124
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_GPI_IE           0x0128

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_GPI_GPE_STS      0x0140
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_GPI_GPE_STS      0x0144
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_GPI_GPE_STS      0x0148

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_GPI_GPE_EN       0x0160
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_GPI_GPE_EN       0x0164
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_GPI_GPE_EN       0x0168

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_SMI_STS          0x0180

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_SMI_EN           0x01A0

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_NMI_STS          0x01C0

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_NMI_EN           0x01E0

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_PADCFG_OFFSET    0x700
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_PADCFG_OFFSET    0x8A0
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_PADCFG_OFFSET    0x9A0

//
// GPIO Community 1 Private Configuration Registers
//
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_PAD_OWN          0x20
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_PAD_OWN          0x28
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_PAD_OWN          0x34
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_PAD_OWN          0x40
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_PAD_OWN          0x4C

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_PADCFGLOCK       0x80
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_PADCFGLOCKTX     0x84
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_PADCFGLOCK       0x88
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_PADCFGLOCKTX     0x8C
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_PADCFGLOCK       0x90
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_PADCFGLOCKTX     0x94
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_PADCFGLOCK       0x98
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_PADCFGLOCKTX     0x9C
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_PADCFGLOCK       0xA0
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_PADCFGLOCKTX     0xA4

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_HOSTSW_OWN       0xB0
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_HOSTSW_OWN       0xB4
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_HOSTSW_OWN       0xB8
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_HOSTSW_OWN       0xBC
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_HOSTSW_OWN       0xC0

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_GPI_IS           0x0100
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_GPI_IS           0x0104
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_GPI_IS           0x0108
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_GPI_IS           0x010C
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_GPI_IS           0x0110

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_GPI_IE           0x0120
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_GPI_IE           0x0124
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_GPI_IE           0x0128
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_GPI_IE           0x012C
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_GPI_IE           0x0130

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_GPI_GPE_STS      0x0140
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_GPI_GPE_STS      0x0144
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_GPI_GPE_STS      0x0148
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_GPI_GPE_STS      0x014C
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_GPI_GPE_STS      0x0150

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_GPI_GPE_EN       0x0160
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_GPI_GPE_EN       0x0164
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_GPI_GPE_EN       0x0168
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_GPI_GPE_EN       0x016C
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_GPI_GPE_EN       0x0170

//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_SMI_STS        0x0180  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_SMI_STS        0x0184  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_SMI_STS          0x0188

//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_SMI_EN         0x01A0  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_SMI_EN         0x01A4  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_SMI_EN           0x01A8

//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_NMI_STS        0x01C0  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_NMI_STS        0x01C4  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_NMI_STS          0x01C8

//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_NMI_EN         0x01E0  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_NMI_EN         0x01E4  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_NMI_EN           0x01E8

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_PADCFG_OFFSET    0x700
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_PADCFG_OFFSET    0x800
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_PADCFG_OFFSET    0x980
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_PADCFG_OFFSET    0xAD0
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_PADCFG_OFFSET    0xC50

//
// GPIO Community 2 Private Configuration Registers
//
#define R_GPIO_VER3_PCH_GPIO_PCR_GPD_PAD_OWN            0x20

#define R_GPIO_VER3_PCH_GPIO_PCR_GPD_PADCFGLOCK         0x80
#define R_GPIO_VER3_PCH_GPIO_PCR_GPD_PADCFGLOCKTX       0x84

#define R_GPIO_VER3_PCH_GPIO_PCR_GPD_HOSTSW_OWN         0xB0

#define R_GPIO_VER3_PCH_GPIO_PCR_GPD_GPI_IS             0x0100
#define R_GPIO_VER3_PCH_GPIO_PCR_GPD_GPI_IE             0x0120

#define R_GPIO_VER3_PCH_GPIO_PCR_GPD_GPI_GPE_STS        0x0140
#define R_GPIO_VER3_PCH_GPIO_PCR_GPD_GPI_GPE_EN         0x0160


#define R_GPIO_VER3_PCH_GPIO_PCR_GPD_PADCFG_OFFSET      0x700

//
// GPIO Community 3 Private Configuration Registers
//
#define R_GPIO_VER3_PCH_GPIO_PCR_CPU_PAD_OWN            0x20
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_PAD_OWN          0x2C
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_PAD_OWN          0x30
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_3_PAD_OWN        0x3C

#define R_GPIO_VER3_PCH_GPIO_PCR_CPU_PADCFGLOCK         0x80
#define R_GPIO_VER3_PCH_GPIO_PCR_CPU_PADCFGLOCKTX       0x84
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_PADCFGLOCK       0x88
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_PADCFGLOCKTX     0x8C
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_PADCFGLOCK       0x90
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_PADCFGLOCKTX     0x94
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_3_PADCFGLOCK     0x98
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_3_PADCFGLOCKTX   0x9C

#define R_GPIO_VER3_PCH_GPIO_PCR_CPU_HOSTSW_OWN         0xB0
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_HOSTSW_OWN       0xB4
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_HOSTSW_OWN       0xB8
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_3_HOSTSW_OWN     0xBC

//#define R_GPIO_VER3_PCH_GPIO_PCR_CPU_GPI_IS           0x0100  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_GPI_IS         0x0104  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_GPI_IS           0x0108

//#define R_GPIO_VER3_PCH_GPIO_PCR_CPU_GPI_IE           0x0120  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_GPI_IE         0x0124  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_GPI_IE           0x0128

//#define R_GPIO_VER3_PCH_GPIO_PCR_CPU_GPI_GPE_STS        0x0140
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_GPI_GPE_STS    0x0144  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_GPI_GPE_STS      0x0148

//#define R_GPIO_VER3_PCH_GPIO_PCR_CPU_GPI_GPE_EN         0x0160
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_GPI_GPE_EN     0x0164  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_GPI_GPE_EN       0x0168


#define R_GPIO_VER3_PCH_GPIO_PCR_CPU_PADCFG_OFFSET      0x700
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_PADCFG_OFFSET    0x810
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_PADCFG_OFFSET    0x830
#define R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_3_PADCFG_OFFSET  0x9B0

//
// GPIO Community 4 Private Configuration Registers
//
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_PAD_OWN          0x20
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_PAD_OWN          0x2C
#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_PAD_OWN         0x3C
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_PAD_OWN          0x40
#define R_GPIO_VER3_PCH_GPIO_PCR_JTAG_PAD_OWN           0x50

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_PADCFGLOCK       0x80
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_PADCFGLOCKTX     0x84
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_PADCFGLOCK       0x88
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_PADCFGLOCKTX     0x8C
#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_PADCFGLOCK      0x90
#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_PADCFGLOCKTX    0x94
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_PADCFGLOCK       0x98
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_PADCFGLOCKTX     0x9C
#define R_GPIO_VER3_PCH_GPIO_PCR_JTAG_PADCFGLOCK        0xA0
#define R_GPIO_VER3_PCH_GPIO_PCR_JTAG_PADCFGLOCKTX      0xA4

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_HOSTSW_OWN       0xB0
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_HOSTSW_OWN       0xB4
#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_HOSTSW_OWN      0xB8
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_HOSTSW_OWN       0xBC
#define R_GPIO_VER3_PCH_GPIO_PCR_JTAG_HOSTSW_OWN        0xC0

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_GPI_IS           0x0100
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_GPI_IS           0x0104
//#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_GPI_IS        0x0108  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_GPI_IS           0x010C

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_GPI_IE           0x0120
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_GPI_IE           0x0124
//#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_GPI_IE        0x0128  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_GPI_IE           0x012C

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_GPI_GPE_STS      0x0140
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_GPI_GPE_STS      0x0144
//#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_GPI_GPE_STS   0x0148  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_GPI_GPE_STS      0x014C

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_GPI_GPE_EN       0x0160
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_GPI_GPE_EN       0x0164
//#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_GPI_GPE_EN    0x0168  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_GPI_GPE_EN       0x016C

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_SMI_STS          0x0180
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_SMI_STS        0x0184  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_SMI_STS       0x0188  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_SMI_STS          0x018C

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_SMI_EN           0x01A0
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_SMI_EN         0x01A4  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_SMI_EN        0x01A8  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_SMI_EN           0x01AC

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_NMI_STS          0x01C0
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_NMI_STS        0x01C4  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_NMI_STS       0x01C8  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_NMI_STS          0x01CC

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_NMI_EN           0x01E0
//#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_NMI_EN         0x01E4  // Not supported setting for this group
//#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_NMI_EN        0x01E8  // Not supported setting for this group
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_NMI_EN           0x01EC

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_PADCFG_OFFSET    0x700
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_PADCFG_OFFSET    0x880
#define R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_PADCFG_OFFSET   0xA10
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_PADCFG_OFFSET    0xA70
#define R_GPIO_VER3_PCH_GPIO_PCR_JTAG_PADCFG_OFFSET     0xC00

//
// GPIO Community 5 Private Configuration Registers
//
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_PAD_OWN          0x20
#define R_GPIO_VER3_PCH_GPIO_PCR_SPI_PAD_OWN            0x24

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_PADCFGLOCK       0x80
#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_PADCFGLOCKTX     0x84
#define R_GPIO_VER3_PCH_GPIO_PCR_SPI_PADCFGLOCK         0x88
#define R_GPIO_VER3_PCH_GPIO_PCR_SPI_PADCFGLOCKTX       0x8C

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_HOSTSW_OWN       0xB0
#define R_GPIO_VER3_PCH_GPIO_PCR_SPI_HOSTSW_OWN         0xB4

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_GPI_IS           0x0100

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_GPI_IE           0x0120

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_GPI_GPE_STS      0x0140

#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_GPI_GPE_EN       0x0160


#define R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_PADCFG_OFFSET    0x700
#define R_GPIO_VER3_PCH_GPIO_PCR_SPI_PADCFG_OFFSET      0x780

#endif
