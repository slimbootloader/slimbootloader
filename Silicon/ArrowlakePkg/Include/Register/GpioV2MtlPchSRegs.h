/** @file
  Register definitions for MTL_PCH_S PCH.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIOV2_REGS_MTL_PCH_S_H_
#define _GPIOV2_REGS_MTL_PCH_S_H_

//
// Community 0 (Number of groups: 4)
//
#define GPIOV2_MTL_PCH_S_GPP_D_PAD_MAX          25
#define GPIOV2_MTL_PCH_S_GPP_R_PAD_MAX          14
#define GPIOV2_MTL_PCH_S_GPP_J_PAD_MAX          18
#define GPIOV2_MTL_PCH_S_VGPIO_PAD_MAX          31

//
// Community 1 (Number of groups: 3)
//
#define GPIOV2_MTL_PCH_S_GPP_A_PAD_MAX          15
#define GPIOV2_MTL_PCH_S_DIR_ESPI_PAD_MAX       12
#define GPIOV2_MTL_PCH_S_GPP_B_PAD_MAX          22

//
// Community 2 (Number of groups: 1)
//
#define GPIOV2_MTL_PCH_S_DSW_PAD_MAX            17

//
// Community 3 (Number of groups: 6)
//
#define GPIOV2_MTL_PCH_S_SPI0_PAD_MAX           9
#define GPIOV2_MTL_PCH_S_GPP_C_PAD_MAX          24
#define GPIOV2_MTL_PCH_S_GPP_H_PAD_MAX          20
#define GPIOV2_MTL_PCH_S_VGPIO_3_PAD_MAX        4
#define GPIOV2_MTL_PCH_S_VGPIO_0_PAD_MAX        8
#define GPIOV2_MTL_PCH_S_VGPIO_4_PAD_MAX        31

//
// Community 4 (Number of groups: 4)
//
#define GPIOV2_MTL_PCH_S_GPP_S_PAD_MAX          8
#define GPIOV2_MTL_PCH_S_GPP_E_PAD_MAX          23
#define GPIOV2_MTL_PCH_S_GPP_K_PAD_MAX          14
#define GPIOV2_MTL_PCH_S_GPP_F_PAD_MAX          24

//
// Community 5 (Number of groups: 2)
//
#define GPIOV2_MTL_PCH_S_GPP_I_PAD_MAX          21
#define GPIOV2_MTL_PCH_S_JTAG_CPU_PAD_MAX       16

//
// Number of groups for MTL_PCH_S
//
#define GPIOV2_MTL_PCH_S_GROUP_MAX              20


//
// Register offsets for MTL_PCH_S
//
#define R_GPIOV2_MTL_PCH_S_PCR_MISCCFG                             0x0010
#define R_GPIOV2_MTL_PCH_S_PCR_G0_RCP_DW0                          0x0020

//
// GPE to Gpio Group mapping
//
// PMC values
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_D                             0x00
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_R                             0x01
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_J                             0x02
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_VGPIO                             0x03
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_A                             0x0C
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_B                             0x0D
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_DSW                               0x04
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_C                             0x0A
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_H                             0x0B
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_S                             0x06
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_E                             0x07
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_K                             0x08
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_F                             0x09
#define V_GPIOV2_MTL_PCH_S_PMC_PWRM_GPIO_CFG_GPP_I                             0x05

// GPIO MISCCFG values
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_D                              0x00
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_R                              0x01
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_J                              0x02
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_VGPIO                              0x03
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_A                              0x0C
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_DIR_ESPI                           0x0F
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_B                              0x0E
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_DSW                                0x04
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_SPI0                               0xFF
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_C                              0x0B
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_H                              0x0A
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_VGPIO_3                            0x0F
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_VGPIO_0                            0x0F
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_VGPIO_4                            0x0F
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_S                              0x06
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_E                              0x07
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_K                              0x08
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_F                              0x09
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_GPP_I                              0x05
#define V_GPIOV2_MTL_PCH_S_PCR_MISCCFG_GPE0_JTAG_CPU                           0x0F




//
// Community 0
//

#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_PAD_OWN                        0x0B0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_PADCFGLOCK                     0x110
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_PADCFGLOCKTX                   0x114
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_HOSTSW_OWN                     0x150
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_GPI_IS                         0x200
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_GPI_IE                         0x220
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_GPI_GPE_STS                    0x240
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_GPI_GPE_EN                     0x260
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_SMI_STS                        0x280
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_SMI_EN                         0x2A0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_NMI_STS                        0x2C0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_NMI_EN                         0x2E0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_D_PAD_CFG_DW0                    0x600
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_PAD_OWN                        0x0C0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_PADCFGLOCK                     0x118
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_PADCFGLOCKTX                   0x11C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_HOSTSW_OWN                     0x154
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_GPI_IS                         0x204
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_GPI_IE                         0x224
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_GPI_GPE_STS                    0x244
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_GPI_GPE_EN                     0x264
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_SMI_STS                        0x284
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_SMI_EN                         0x2A4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_NMI_STS                        0x2C4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_NMI_EN                         0x2E4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_R_PAD_CFG_DW0                    0x790
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_PAD_OWN                        0x0C8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_PADCFGLOCK                     0x120
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_PADCFGLOCKTX                   0x124
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_HOSTSW_OWN                     0x158
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_GPI_IS                         0x208
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_GPI_IE                         0x228
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_GPI_GPE_STS                    0x248
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_GPI_GPE_EN                     0x268
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_SMI_STS                        0x288
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_SMI_EN                         0x2A8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_NMI_STS                        0x2C8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_NMI_EN                         0x2E8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_J_PAD_CFG_DW0                    0x870
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_PAD_OWN                        0x0D4
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_PADCFGLOCK                     0x128
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_PADCFGLOCKTX                   0x12C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_HOSTSW_OWN                     0x15C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_GPI_IS                         0x20C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_GPI_IE                         0x22C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_GPI_GPE_STS                    0x24C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_GPI_GPE_EN                     0x26C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_SMI_STS                        0x28C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_SMI_EN                         0x2AC
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_NMI_STS                        0x2CC
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_NMI_EN                         0x2EC
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_PAD_CFG_DW0                    0x990

//
// Community 1
//

#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_PAD_OWN                        0x0B0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_PADCFGLOCK                     0x110
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_PADCFGLOCKTX                   0x114
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_HOSTSW_OWN                     0x150
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_GPI_IS                         0x200
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_GPI_IE                         0x220
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_GPI_GPE_STS                    0x240
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_GPI_GPE_EN                     0x260
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_SMI_STS                        0x280
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_SMI_EN                         0x2A0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_NMI_STS                        0x2C0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_NMI_EN                         0x2E0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_A_PAD_CFG_DW0                    0x600
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_PAD_OWN                     0x0B8
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_PADCFGLOCK                  0x118
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_PADCFGLOCKTX                0x11C
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_HOSTSW_OWN                  0x154
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_GPI_IS                      0x204
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_GPI_IE                      0x224
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_GPI_GPE_STS                 0x244
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_GPI_GPE_EN                  0x264
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_SMI_STS                     0x284
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_SMI_EN                      0x2A4
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_NMI_STS                     0x2C4
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_NMI_EN                      0x2E4
#define R_GPIOV2_MTL_PCH_S_PCR_DIR_ESPI_PAD_CFG_DW0                 0x720
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_PAD_OWN                        0x0C0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_PADCFGLOCK                     0x120
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_PADCFGLOCKTX                   0x124
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_HOSTSW_OWN                     0x158
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_GPI_IS                         0x208
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_GPI_IE                         0x228
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_GPI_GPE_STS                    0x248
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_GPI_GPE_EN                     0x268
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_SMI_STS                        0x288
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_SMI_EN                         0x2A8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_NMI_STS                        0x2C8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_NMI_EN                         0x2E8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_B_PAD_CFG_DW0                    0x7B0

//
// Community 2
//

#define R_GPIOV2_MTL_PCH_S_PCR_DSW_PAD_OWN                          0x0B0
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_PADCFGLOCK                       0x110
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_PADCFGLOCKTX                     0x114
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_HOSTSW_OWN                       0x150
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_GPI_IS                           0x200
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_GPI_IE                           0x220
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_GPI_GPE_STS                      0x240
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_GPI_GPE_EN                       0x260
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_SMI_STS                          0x280
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_SMI_EN                           0x2A0
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_NMI_STS                          0x2C0
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_NMI_EN                           0x2E0
#define R_GPIOV2_MTL_PCH_S_PCR_DSW_PAD_CFG_DW0                      0x600

//
// Community 3
//

#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_PAD_OWN                         0x0B0
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_PADCFGLOCK                      0x110
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_PADCFGLOCKTX                    0x114
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_HOSTSW_OWN                      0x150
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_GPI_IS                          0x200
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_GPI_IE                          0x220
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_GPI_GPE_STS                     0x240
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_GPI_GPE_EN                      0x260
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_SMI_STS                         0x280
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_SMI_EN                          0x2A0
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_NMI_STS                         0x2C0
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_NMI_EN                          0x2E0
#define R_GPIOV2_MTL_PCH_S_PCR_SPI0_PAD_CFG_DW0                     0x600
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_PAD_OWN                        0x0B8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_PADCFGLOCK                     0x118
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_PADCFGLOCKTX                   0x11C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_HOSTSW_OWN                     0x154
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_GPI_IS                         0x204
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_GPI_IE                         0x224
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_GPI_GPE_STS                    0x244
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_GPI_GPE_EN                     0x264
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_SMI_STS                        0x284
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_SMI_EN                         0x2A4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_NMI_STS                        0x2C4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_NMI_EN                         0x2E4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_C_PAD_CFG_DW0                    0x690
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_PAD_OWN                        0x0C4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_PADCFGLOCK                     0x120
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_PADCFGLOCKTX                   0x124
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_HOSTSW_OWN                     0x158
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_GPI_IS                         0x208
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_GPI_IE                         0x228
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_GPI_GPE_STS                    0x248
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_GPI_GPE_EN                     0x268
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_SMI_STS                        0x288
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_SMI_EN                         0x2A8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_NMI_STS                        0x2C8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_NMI_EN                         0x2E8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_H_PAD_CFG_DW0                    0x810
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_PAD_OWN                      0x0D0
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_PADCFGLOCK                   0x128
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_PADCFGLOCKTX                 0x12C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_HOSTSW_OWN                   0x15C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_GPI_IS                       0x20C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_GPI_IE                       0x22C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_GPI_GPE_STS                  0x24C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_GPI_GPE_EN                   0x26C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_SMI_STS                      0x28C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_SMI_EN                       0x2AC
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_NMI_STS                      0x2CC
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_NMI_EN                       0x2EC
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_3_PAD_CFG_DW0                  0x950
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_PAD_OWN                      0x0D4
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_PADCFGLOCK                   0x130
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_PADCFGLOCKTX                 0x134
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_HOSTSW_OWN                   0x160
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_GPI_IS                       0x210
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_GPI_IE                       0x230
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_GPI_GPE_STS                  0x250
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_GPI_GPE_EN                   0x270
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_SMI_STS                      0x290
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_SMI_EN                       0x2B0
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_NMI_STS                      0x2D0
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_NMI_EN                       0x2F0
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_0_PAD_CFG_DW0                  0x990
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_PAD_OWN                      0x0D8
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_PADCFGLOCK                   0x138
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_PADCFGLOCKTX                 0x13C
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_HOSTSW_OWN                   0x164
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_GPI_IS                       0x214
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_GPI_IE                       0x234
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_GPI_GPE_STS                  0x254
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_GPI_GPE_EN                   0x274
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_SMI_STS                      0x294
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_SMI_EN                       0x2B4
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_NMI_STS                      0x2D4
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_NMI_EN                       0x2F4
#define R_GPIOV2_MTL_PCH_S_PCR_VGPIO_4_PAD_CFG_DW0                  0xA10

//
// Community 4
//

#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_PAD_OWN                        0x0B0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_PADCFGLOCK                     0x110
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_PADCFGLOCKTX                   0x114
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_HOSTSW_OWN                     0x150
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_GPI_IS                         0x200
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_GPI_IE                         0x220
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_GPI_GPE_STS                    0x240
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_GPI_GPE_EN                     0x260
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_SMI_STS                        0x280
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_SMI_EN                         0x2A0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_NMI_STS                        0x2C0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_NMI_EN                         0x2E0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_S_PAD_CFG_DW0                    0x600
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_PAD_OWN                        0x0B4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_PADCFGLOCK                     0x118
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_PADCFGLOCKTX                   0x11C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_HOSTSW_OWN                     0x154
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_GPI_IS                         0x204
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_GPI_IE                         0x224
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_GPI_GPE_STS                    0x244
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_GPI_GPE_EN                     0x264
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_SMI_STS                        0x284
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_SMI_EN                         0x2A4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_NMI_STS                        0x2C4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_NMI_EN                         0x2E4
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_E_PAD_CFG_DW0                    0x680
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_PAD_OWN                        0x0C0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_PADCFGLOCK                     0x120
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_PADCFGLOCKTX                   0x124
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_HOSTSW_OWN                     0x158
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_GPI_IS                         0x208
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_GPI_IE                         0x228
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_GPI_GPE_STS                    0x248
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_GPI_GPE_EN                     0x268
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_SMI_STS                        0x288
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_SMI_EN                         0x2A8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_NMI_STS                        0x2C8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_NMI_EN                         0x2E8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_K_PAD_CFG_DW0                    0x7F0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_PAD_OWN                        0x0C8
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_PADCFGLOCK                     0x128
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_PADCFGLOCKTX                   0x12C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_HOSTSW_OWN                     0x15C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_GPI_IS                         0x20C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_GPI_IE                         0x22C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_GPI_GPE_STS                    0x24C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_GPI_GPE_EN                     0x26C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_SMI_STS                        0x28C
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_SMI_EN                         0x2AC
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_NMI_STS                        0x2CC
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_NMI_EN                         0x2EC
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_F_PAD_CFG_DW0                    0x8D0

//
// Community 5
//

#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_PAD_OWN                        0x0B0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_PADCFGLOCK                     0x110
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_PADCFGLOCKTX                   0x114
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_HOSTSW_OWN                     0x150
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_GPI_IS                         0x200
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_GPI_IE                         0x220
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_GPI_GPE_STS                    0x240
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_GPI_GPE_EN                     0x260
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_SMI_STS                        0x280
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_SMI_EN                         0x2A0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_NMI_STS                        0x2C0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_NMI_EN                         0x2E0
#define R_GPIOV2_MTL_PCH_S_PCR_GPP_I_PAD_CFG_DW0                    0x600
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_PAD_OWN                     0x0BC
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_PADCFGLOCK                  0x118
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_PADCFGLOCKTX                0x11C
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_HOSTSW_OWN                  0x154
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_GPI_IS                      0x204
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_GPI_IE                      0x224
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_GPI_GPE_STS                 0x244
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_GPI_GPE_EN                  0x264
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_SMI_STS                     0x284
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_SMI_EN                      0x2A4
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_NMI_STS                     0x2C4
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_NMI_EN                      0x2E4
#define R_GPIOV2_MTL_PCH_S_PCR_JTAG_CPU_PAD_CFG_DW0                 0x750

#endif // _GPIOV2_REGS_MTL_PCH_S_H_