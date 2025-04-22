/** @file
  Register names for CNL GPIO

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

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_GPIO_CNL_H_
#define _PCH_REGS_GPIO_CNL_H_

//
// PCH-LP GPIO
//
#define CNL_PCH_LP_GPIO_GROUP_MAX             15

#define CNL_PCH_LP_GPIO_GPP_A_PAD_MAX         25
#define CNL_PCH_LP_GPIO_GPP_B_PAD_MAX         26
#define CNL_PCH_LP_GPIO_GPP_C_PAD_MAX         24
#define CNL_PCH_LP_GPIO_GPP_D_PAD_MAX         24
#define CNL_PCH_LP_GPIO_GPP_E_PAD_MAX         24
#define CNL_PCH_LP_GPIO_GPP_F_PAD_MAX         24
#define CNL_PCH_LP_GPIO_GPP_G_PAD_MAX         8
#define CNL_PCH_LP_GPIO_GPP_H_PAD_MAX         24
#define CNL_PCH_LP_GPIO_GPD_PAD_MAX           16

//
// PCH-H GPIO
//
#define CNL_PCH_H_GPIO_GROUP_MAX              10

#define CNL_PCH_H_GPIO_GPP_A_PAD_MAX          24
#define CNL_PCH_H_GPIO_GPP_B_PAD_MAX          24
#define CNL_PCH_H_GPIO_GPP_C_PAD_MAX          24
#define CNL_PCH_H_GPIO_GPP_D_PAD_MAX          24
#define CNL_PCH_H_GPIO_GPP_E_PAD_MAX          24
#define CNL_PCH_H_GPIO_GPP_F_PAD_MAX          24
#define CNL_PCH_H_GPIO_GPP_G_PAD_MAX          24
#define CNL_PCH_H_GPIO_GPP_H_PAD_MAX          24
#define CNL_PCH_H_GPIO_GPP_I_PAD_MAX          11
#define CNL_PCH_H_GPIO_GPD_PAD_MAX            12


//
// PCH-LP GPIO registers //Not used in PCH-V
//
//
// GPIO Community Common Private Configuration Registers
//
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_A     0x0
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_B     0x1
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_C     0xC
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_D     0x4
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_E     0xD
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_F     0x5
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_G     0x2
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_H     0x6
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPD       0x9
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_VGPIO     0x7
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_SPI       0x3
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_AZA       0xA
#define V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_JTAG      0xE

//
// GPIO Community 0 Private Configuration Registers
//
#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_PAD_OWN        0x20
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_PAD_OWN        0x30
#define R_CNL_PCH_LP_GPIO_PCR_GPP_G_PAD_OWN        0x40

#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_PADCFGLOCK     0x80
#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_PADCFGLOCKTX   0x84
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_PADCFGLOCK     0x88
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_PADCFGLOCKTX   0x8C
#define R_CNL_PCH_LP_GPIO_PCR_GPP_G_PADCFGLOCK     0x90
#define R_CNL_PCH_LP_GPIO_PCR_GPP_G_PADCFGLOCKTX   0x94

#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_HOSTSW_OWN     0xB0
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_HOSTSW_OWN     0xB4
#define R_CNL_PCH_LP_GPIO_PCR_GPP_G_HOSTSW_OWN     0xB8

#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_GPI_IS         0x0100
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_GPI_IS         0x0104
#define R_CNL_PCH_LP_GPIO_PCR_GPP_G_GPI_IS         0x0108

#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_GPI_IE         0x0120
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_GPI_IE         0x0124
#define R_CNL_PCH_LP_GPIO_PCR_GPP_G_GPI_IE         0x0128

#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_GPI_GPE_STS    0x0140
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_GPI_GPE_STS    0x0144
#define R_CNL_PCH_LP_GPIO_PCR_GPP_G_GPI_GPE_STS    0x0148

#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_GPI_GPE_EN     0x0160
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_GPI_GPE_EN     0x0164
#define R_CNL_PCH_LP_GPIO_PCR_GPP_G_GPI_GPE_EN     0x0168

//#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_SMI_STS        0x0180  // Not supported setting for this group
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_SMI_STS        0x0184

//#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_SMI_EN         0x01A0  // Not supported setting for this group
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_SMI_EN         0x01A4

//#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_NMI_STS        0x01C0  // Not supported setting for this group
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_NMI_STS        0x01C4

//#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_NMI_EN         0x01E0  // Not supported setting for this group
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_NMI_EN         0x01E4

#define R_CNL_PCH_LP_GPIO_PCR_GPP_A_PADCFG_OFFSET  0x600
#define R_CNL_PCH_LP_GPIO_PCR_GPP_B_PADCFG_OFFSET  0x790
#define R_CNL_PCH_LP_GPIO_PCR_GPP_G_PADCFG_OFFSET  0x930

//
// GPIO Community 1 Private Configuration Registers
//
#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_PAD_OWN        0x20
#define R_CNL_PCH_LP_GPIO_PCR_GPP_F_PAD_OWN        0x30
#define R_CNL_PCH_LP_GPIO_PCR_GPP_H_PAD_OWN        0x3C

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_PADCFGLOCK     0x80
#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_PADCFGLOCKTX   0x84
#define R_CNL_PCH_LP_GPIO_PCR_GPP_F_PADCFGLOCK     0x88
#define R_CNL_PCH_LP_GPIO_PCR_GPP_F_PADCFGLOCKTX   0x8C
#define R_CNL_PCH_LP_GPIO_PCR_GPP_H_PADCFGLOCK     0x90
#define R_CNL_PCH_LP_GPIO_PCR_GPP_H_PADCFGLOCKTX   0x94

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_HOSTSW_OWN     0xB0
#define R_CNL_PCH_LP_GPIO_PCR_GPP_F_HOSTSW_OWN     0xB4
#define R_CNL_PCH_LP_GPIO_PCR_GPP_H_HOSTSW_OWN     0xB8

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_GPI_IS         0x0100
#define R_CNL_PCH_LP_GPIO_PCR_GPP_F_GPI_IS         0x0104
#define R_CNL_PCH_LP_GPIO_PCR_GPP_H_GPI_IS         0x0108

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_GPI_IE         0x0120
#define R_CNL_PCH_LP_GPIO_PCR_GPP_F_GPI_IE         0x0124
#define R_CNL_PCH_LP_GPIO_PCR_GPP_H_GPI_IE         0x0128

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_GPI_GPE_STS    0x0140
#define R_CNL_PCH_LP_GPIO_PCR_GPP_F_GPI_GPE_STS    0x0144
#define R_CNL_PCH_LP_GPIO_PCR_GPP_H_GPI_GPE_STS    0x0148

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_GPI_GPE_EN     0x0160
#define R_CNL_PCH_LP_GPIO_PCR_GPP_F_GPI_GPE_EN     0x0164
#define R_CNL_PCH_LP_GPIO_PCR_GPP_H_GPI_GPE_EN     0x0168

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_SMI_STS        0x0180

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_SMI_EN         0x01A0

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_NMI_STS        0x01C0

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_NMI_EN         0x01E0

#define R_CNL_PCH_LP_GPIO_PCR_GPP_D_PADCFG_OFFSET  0x600
#define R_CNL_PCH_LP_GPIO_PCR_GPP_F_PADCFG_OFFSET  0x790
#define R_CNL_PCH_LP_GPIO_PCR_GPP_H_PADCFG_OFFSET  0x910

//
// GPIO Community 2 Private Configuration Registers
//
#define R_CNL_PCH_LP_GPIO_PCR_GPD_PAD_OWN          0x20

#define R_CNL_PCH_LP_GPIO_PCR_GPD_PADCFGLOCK       0x80
#define R_CNL_PCH_LP_GPIO_PCR_GPD_PADCFGLOCKTX     0x84

#define R_CNL_PCH_LP_GPIO_PCR_GPD_HOSTSW_OWN       0xB0

#define R_CNL_PCH_LP_GPIO_PCR_GPD_GPI_IS           0x0100
#define R_CNL_PCH_LP_GPIO_PCR_GPD_GPI_IE           0x0120

#define R_CNL_PCH_LP_GPIO_PCR_GPD_GPI_GPE_STS      0x0140
#define R_CNL_PCH_LP_GPIO_PCR_GPD_GPI_GPE_EN       0x0160


#define R_CNL_PCH_LP_GPIO_PCR_GPD_PADCFG_OFFSET    0x600


//
// GPIO Community 4 Private Configuration Registers
//
#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_PAD_OWN        0x20
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_PAD_OWN        0x2C

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_PADCFGLOCK     0x80
#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_PADCFGLOCKTX   0x84
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_PADCFGLOCK     0x88
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_PADCFGLOCKTX   0x8C

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_HOSTSW_OWN     0xB0
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_HOSTSW_OWN     0xB4

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_GPI_IS         0x0100
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_GPI_IS         0x0104

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_GPI_IE         0x0120
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_GPI_IE         0x0124

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_GPI_GPE_STS    0x0140
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_GPI_GPE_STS    0x0144

#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_GPI_GPE_EN     0x0164

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_SMI_STS        0x0180
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_SMI_STS        0x0184

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_SMI_EN         0x01A0
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_SMI_EN         0x01A4

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_NMI_STS        0x01C0
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_NMI_STS        0x01C4

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_NMI_EN         0x01E0
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_NMI_EN         0x01E4

#define R_CNL_PCH_LP_GPIO_PCR_GPP_C_PADCFG_OFFSET  0x600
#define R_CNL_PCH_LP_GPIO_PCR_GPP_E_PADCFG_OFFSET  0x780

//
// GPIO Community Common Private Configuration Registers
//

#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_A      0x0
#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_B      0x1
#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_C      0x2
#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_D      0x3
#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_E      0x4
#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_F      0x5
#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_G      0x6
#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_H      0x7
#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_I      0x8
#define V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPD        0x9

//
// GPIO Community 0 Private Configuration Registers
//
#define R_CNL_PCH_H_GPIO_PCR_GPP_A_PAD_OWN         0x20
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_PAD_OWN         0x30

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_PADCFGLOCK      0xA0
#define R_CNL_PCH_H_GPIO_PCR_GPP_A_PADCFGLOCKTX    0xA4
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_PADCFGLOCK      0xA8
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_PADCFGLOCKTX    0xAC

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_HOSTSW_OWN      0xD0
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_HOSTSW_OWN      0xD4

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_GPI_IS          0x0100
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_GPI_IS          0x0104

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_GPI_IE          0x0120
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_GPI_IE          0x0124

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_GPI_GPE_STS     0x0140
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_GPI_GPE_STS     0x0144

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_GPI_GPE_EN      0x0160
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_GPI_GPE_EN      0x0164

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_SMI_STS         0x0180
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_SMI_STS         0x0184

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_SMI_EN          0x01A0
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_SMI_EN          0x01A4

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_NMI_STS         0x01C0
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_NMI_STS         0x01C4

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_NMI_EN          0x01E0
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_NMI_EN          0x01E4

#define R_CNL_PCH_H_GPIO_PCR_GPP_A_PADCFG_OFFSET   0x400
#define R_CNL_PCH_H_GPIO_PCR_GPP_B_PADCFG_OFFSET   0x4C0

//
// GPIO Community 1 Private Configuration Registers
//
#define R_CNL_PCH_H_GPIO_PCR_GPP_C_PAD_OWN         0x20
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_PAD_OWN         0x2C
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_PAD_OWN         0x38
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_PAD_OWN         0x40
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_PAD_OWN         0x4C
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_PAD_OWN         0x58

#define R_CNL_PCH_H_GPIO_PCR_GPP_C_PADCFGLOCK      0x90
#define R_CNL_PCH_H_GPIO_PCR_GPP_C_PADCFGLOCKTX    0x94
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_PADCFGLOCK      0x98
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_PADCFGLOCKTX    0x9C

#define R_CNL_PCH_H_GPIO_PCR_GPP_E_PADCFGLOCK     0xA0
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_PADCFGLOCKTX   0xA4
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_PADCFGLOCK     0xA8
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_PADCFGLOCKTX   0xAC
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_PADCFGLOCK     0xB0
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_PADCFGLOCKTX   0xB4
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_PADCFGLOCK     0xB8
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_PADCFGLOCKTX   0xBC


#define R_CNL_PCH_H_GPIO_PCR_GPP_C_HOSTSW_OWN     0xD0
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_HOSTSW_OWN     0xD4
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_HOSTSW_OWN     0xD8
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_HOSTSW_OWN     0xDC
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_HOSTSW_OWN     0xE0
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_HOSTSW_OWN     0xE4


#define R_CNL_PCH_H_GPIO_PCR_GPP_C_GPI_IS          0x0100
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_GPI_IS          0x0104
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_GPI_IS           0x0108
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_GPI_IS         0x010C
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_GPI_IS         0x0110
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_GPI_IS         0x0114


#define R_CNL_PCH_H_GPIO_PCR_GPP_C_GPI_IE          0x0120
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_GPI_IE          0x0124
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_GPI_IE           0x0128
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_GPI_IE         0x012C
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_GPI_IE         0x0130
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_GPI_IE         0x0134

#define R_CNL_PCH_H_GPIO_PCR_GPP_C_GPI_GPE_STS     0x0140
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_GPI_GPE_STS     0x0144
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_GPI_GPE_STS      0x0148
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_GPI_GPE_STS    0x014C
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_GPI_GPE_STS    0x0150
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_GPI_GPE_STS    0x0154//#define R_CNL_PCH_H_GPIO_PCR_AZA_GPI_GPE_STS       0x014C  // Not supported setting for this group

#define R_CNL_PCH_H_GPIO_PCR_GPP_C_GPI_GPE_EN      0x0160
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_GPI_GPE_EN      0x0164
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_GPI_GPE_EN       0x0168
// SKL PCH-H:
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_GPI_GPE_EN     0x016C
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_GPI_GPE_EN     0x0170
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_GPI_GPE_EN     0x0174

#define R_CNL_PCH_H_GPIO_PCR_GPP_C_SMI_STS         0x0180
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_SMI_STS         0x0184
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_SMI_STS          0x0188
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_SMI_STS        0x018C
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_SMI_STS        0x0190
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_SMI_STS        0x0194

#define R_CNL_PCH_H_GPIO_PCR_GPP_C_SMI_EN          0x01A0
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_SMI_EN          0x01A4
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_SMI_EN           0x01A8
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_SMI_EN         0x01AC
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_SMI_EN         0x01B0
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_SMI_EN         0x01B4

#define R_CNL_PCH_H_GPIO_PCR_GPP_C_NMI_STS         0x01C0
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_NMI_STS         0x01C4
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_NMI_STS          0x01C8
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_NMI_STS        0x01CC
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_NMI_STS        0x01D0
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_NMI_STS        0x01D4

#define R_CNL_PCH_H_GPIO_PCR_GPP_C_NMI_EN          0x01E0
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_NMI_EN          0x01E4
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_NMI_EN           0x01E8
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_NMI_EN         0x01EC
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_NMI_EN         0x01F0
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_NMI_EN         0x01F4

#define R_CNL_PCH_H_GPIO_PCR_GPP_C_PADCFG_OFFSET   0x400
#define R_CNL_PCH_H_GPIO_PCR_GPP_D_PADCFG_OFFSET   0x4C0
#define R_CNL_PCH_H_GPIO_PCR_GPP_E_PADCFG_OFFSET    0x580
#define R_CNL_PCH_H_GPIO_PCR_GPP_F_PADCFG_OFFSET  0x5E8
#define R_CNL_PCH_H_GPIO_PCR_GPP_G_PADCFG_OFFSET  0x6A8
#define R_CNL_PCH_H_GPIO_PCR_GPP_H_PADCFG_OFFSET  0x768

//
// GPIO Community 2 Private Configuration Registers
//

#define R_CNL_PCH_H_GPIO_PCR_GPD_PAD_OWN           0x20

#define R_CNL_PCH_H_GPIO_PCR_GPD_PADCFGLOCK        0x90
#define R_CNL_PCH_H_GPIO_PCR_GPD_PADCFGLOCKTX      0x94

#define R_CNL_PCH_H_GPIO_PCR_GPD_HOSTSW_OWN        0xD0

#define R_CNL_PCH_H_GPIO_PCR_GPD_GPI_IS            0x0100

#define R_CNL_PCH_H_GPIO_PCR_GPD_GPI_IE            0x0120

#define R_CNL_PCH_H_GPIO_PCR_GPD_GPI_GPE_STS       0x0140

#define R_CNL_PCH_H_GPIO_PCR_GPD_GPI_GPE_EN        0x0160

#define R_CNL_PCH_H_GPIO_PCR_GPD_PADCFG_OFFSET     0x400

//
// GPIO Community 3 Private Configuration Registers
//

#define R_CNL_PCH_H_GPIO_PCR_GPP_I_PAD_OWN        0x20
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFGLOCK     0x90
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFGLOCKTX   0x94
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_HOSTSW_OWN     0xD0
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_IS         0x0100
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_IE         0x0120
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_GPE_STS    0x0140
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_GPE_EN     0x0160
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_SMI_STS        0x0180
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_SMI_EN         0x01A0
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_NMI_STS        0x01C0
#define R_CNL_PCH_H_GPIO_PCR_GPP_I_NMI_EN         0x01E0


#define R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFG_OFFSET   0x400


#endif
