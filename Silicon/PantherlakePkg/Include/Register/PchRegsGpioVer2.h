/** @file
  Register names for Ver2 GPIO

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GPIO_REGS_VER2_H_
#define _GPIO_REGS_VER2_H_


//
// Community 0 (Number of groups: 2)
//
#define GPIOV2_PTL_PCD_GPP_V_PAD_MAX                                24
#define GPIOV2_PTL_PCD_GPP_C_PAD_MAX                                24

//
// Community 1 (Number of groups: 2)
//
#define GPIOV2_PTL_PCD_GPP_F_PAD_MAX                                26
#define GPIOV2_PTL_PCD_GPP_E_PAD_MAX                                25

//
// Community 3 (Number of groups: 4)
//
#define GPIOV2_PTL_PCD_CPUJTAG_PAD_MAX                              15
#define GPIOV2_PTL_PCD_GPP_H_PAD_MAX                                28
#define GPIOV2_PTL_PCD_GPPASPI0_PAD_MAX                             28
#define GPIOV2_PTL_PCD_VGPIO_3_PAD_MAX                              14

//
// Community 4 (Number of groups: 1)
//
#define GPIOV2_PTL_PCD_GPP_S_PAD_MAX                                8

//
// Community 5 (Number of groups: 3)
//
#define GPIOV2_PTL_PCD_GPP_B_PAD_MAX                                27
#define GPIOV2_PTL_PCD_GPP_D_PAD_MAX                                27
#define GPIOV2_PTL_PCD_VGPIO_PAD_MAX                                18


//
// Group to GPE mappings
//
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_GPP_V                            0
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_GPP_V                              0
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_GPP_C                            1
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_GPP_C                              1
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_GPP_F                            2
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_GPP_F                              2
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_GPP_E                            3
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_GPP_E                              3
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_CPUJTAG                          15
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_CPUJTAG                            15
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_GPP_H                            5
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_GPP_H                              5
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_GPPASPI0                         4
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_GPPASPI0                           4
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_vGPIO_3                          6
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_vGPIO_3                            6
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_GPP_S                            9
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_GPP_S                              9
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_GPP_B                            7
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_GPP_B                              7
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_GPP_D                            8
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_GPP_D                              8
#define V_GPIOV2_PTL_PCD_PMC_PWRM_GPIO_CFG_vGPIO                            15
#define V_GPIOV2_PTL_PCD_PCR_MISCCFG_GPE_vGPIO                              15

//
// Number of groups for PTL_PCD
//
#define GPIOV2_PTL_PCD_GROUP_MAX                                    12


//
// Register offsets for PTL_PCD
//
#define R_GPIOV2_PTL_PCD_PCR_MISCCFG                                0x010
#define R_GPIOV2_PTL_PCD_PCR_G0_RCP_DW0                             0x020


//
// Community 0
//

#define R_GPIOV2_PTL_PCD_PCR_COM0_MISCCFG                           0x010

#define R_GPIOV2_PTL_PCD_PCR_GPP_V_PAD_OWN                          0x130
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_PADCFGLOCK                       0x290
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_PADCFGLOCKTX                     0x294
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_HOSTSW_OWN                       0x2C0
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_GPI_IS                           0x300
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_GPI_IE                           0x310
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_GPI_GPE_STS                      0x330
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_GPI_GPE_EN                       0x350
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_SMI_STS                          0x370
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_SMI_EN                           0x390
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_NMI_STS                          0x3B0
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_NMI_EN                           0x3D0
#define R_GPIOV2_PTL_PCD_PCR_GPP_V_PAD_CFG_DW0                      0x800
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_PAD_OWN                          0x190
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_PADCFGLOCK                       0x298
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_PADCFGLOCKTX                     0x29C
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_HOSTSW_OWN                       0x2C4
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_GPI_IS                           0x304
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_GPI_IE                           0x314
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_GPI_GPE_STS                      0x334
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_GPI_GPE_EN                       0x354
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_SMI_STS                          0x374
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_SMI_EN                           0x394
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_NMI_STS                          0x3B4
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_NMI_EN                           0x3D4
#define R_GPIOV2_PTL_PCD_PCR_GPP_C_PAD_CFG_DW0                      0x980

//
// Community 1
//

#define R_GPIOV2_PTL_PCD_PCR_COM1_MISCCFG                           0x010

#define R_GPIOV2_PTL_PCD_PCR_GPP_F_PAD_OWN                          0x130
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_PADCFGLOCK                       0x290
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_PADCFGLOCKTX                     0x294
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_HOSTSW_OWN                       0x2C0
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_GPI_IS                           0x300
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_GPI_IE                           0x310
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_GPI_GPE_STS                      0x330
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_GPI_GPE_EN                       0x350
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_SMI_STS                          0x370
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_SMI_EN                           0x390
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_NMI_STS                          0x3B0
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_NMI_EN                           0x3D0
#define R_GPIOV2_PTL_PCD_PCR_GPP_F_PAD_CFG_DW0                      0x800
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_PAD_OWN                          0x198
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_PADCFGLOCK                       0x298
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_PADCFGLOCKTX                     0x29C
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_HOSTSW_OWN                       0x2C4
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_GPI_IS                           0x304
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_GPI_IE                           0x314
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_GPI_GPE_STS                      0x334
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_GPI_GPE_EN                       0x354
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_SMI_STS                          0x374
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_SMI_EN                           0x394
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_NMI_STS                          0x3B4
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_NMI_EN                           0x3D4
#define R_GPIOV2_PTL_PCD_PCR_GPP_E_PAD_CFG_DW0                      0x9A0

//
// Community 3
//

#define R_GPIOV2_PTL_PCD_PCR_COM3_MISCCFG                           0x010

#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PAD_OWN                        0x130
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PADCFGLOCK                     0x290
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PADCFGLOCKTX                   0x294
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_HOSTSW_OWN                     0x2C0
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_GPI_IS                         0x300
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_GPI_IE                         0x310
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_GPI_GPE_STS                    0x330
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_GPI_GPE_EN                     0x350
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_SMI_STS                        0x370
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_SMI_EN                         0x390
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_NMI_STS                        0x3B0
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_NMI_EN                         0x3D0
#define R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PAD_CFG_DW0                    0x800
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_PAD_OWN                          0x16C
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_PADCFGLOCK                       0x298
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_PADCFGLOCKTX                     0x29C
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_HOSTSW_OWN                       0x2C4
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_GPI_IS                           0x304
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_GPI_IE                           0x314
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_GPI_GPE_STS                      0x334
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_GPI_GPE_EN                       0x354
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_SMI_STS                          0x374
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_SMI_EN                           0x394
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_NMI_STS                          0x3B4
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_NMI_EN                           0x3D4
#define R_GPIOV2_PTL_PCD_PCR_GPP_H_PAD_CFG_DW0                      0x8F0
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_PAD_OWN                       0x1DC
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_PADCFGLOCK                    0x2A0
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_PADCFGLOCKTX                  0x2A4
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_HOSTSW_OWN                    0x2C8
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_GPI_IS                        0x308
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_GPI_IE                        0x318
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_GPI_GPE_STS                   0x338
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_GPI_GPE_EN                    0x358
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_SMI_STS                       0x378
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_SMI_EN                        0x398
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_NMI_STS                       0x3B8
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_NMI_EN                        0x3D8
#define R_GPIOV2_PTL_PCD_PCR_GPPASPI0_PAD_CFG_DW0                   0xAB0
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_PAD_OWN                        0x24C
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_PADCFGLOCK                     0x2A8
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_PADCFGLOCKTX                   0x2AC
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_HOSTSW_OWN                     0x2CC
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_GPI_IS                         0x30C
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_GPI_IE                         0x31C
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_GPI_GPE_STS                    0x33C
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_GPI_GPE_EN                     0x35C
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_SMI_STS                        0x37C
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_SMI_EN                         0x39C
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_NMI_STS                        0x3BC
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_NMI_EN                         0x3DC
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_3_PAD_CFG_DW0                    0xC70

//
// Community 4
//

#define R_GPIOV2_PTL_PCD_PCR_COM4_MISCCFG                           0x010

#define R_GPIOV2_PTL_PCD_PCR_GPP_S_PAD_OWN                          0x130
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_PADCFGLOCK                       0x290
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_PADCFGLOCKTX                     0x294
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_HOSTSW_OWN                       0x2C0
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_GPI_IS                           0x300
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_GPI_IE                           0x310
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_GPI_GPE_STS                      0x330
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_GPI_GPE_EN                       0x350
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_SMI_STS                          0x370
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_SMI_EN                           0x390
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_NMI_STS                          0x3B0
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_NMI_EN                           0x3D0
#define R_GPIOV2_PTL_PCD_PCR_GPP_S_PAD_CFG_DW0                      0x800

//
// Community 5
//

#define R_GPIOV2_PTL_PCD_PCR_COM5_MISCCFG                           0x010

#define R_GPIOV2_PTL_PCD_PCR_GPP_B_PAD_OWN                          0x130
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_PADCFGLOCK                       0x290
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_PADCFGLOCKTX                     0x294
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_HOSTSW_OWN                       0x2C0
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_GPI_IS                           0x300
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_GPI_IE                           0x310
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_GPI_GPE_STS                      0x330
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_GPI_GPE_EN                       0x350
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_SMI_STS                          0x370
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_SMI_EN                           0x390
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_NMI_STS                          0x3B0
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_NMI_EN                           0x3D0
#define R_GPIOV2_PTL_PCD_PCR_GPP_B_PAD_CFG_DW0                      0x800
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_PAD_OWN                          0x19C
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_PADCFGLOCK                       0x298
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_PADCFGLOCKTX                     0x29C
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_HOSTSW_OWN                       0x2C4
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_GPI_IS                           0x304
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_GPI_IE                           0x314
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_GPI_GPE_STS                      0x334
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_GPI_GPE_EN                       0x354
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_SMI_STS                          0x374
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_SMI_EN                           0x394
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_NMI_STS                          0x3B4
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_NMI_EN                           0x3D4
#define R_GPIOV2_PTL_PCD_PCR_GPP_D_PAD_CFG_DW0                      0x9B0
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_PAD_OWN                          0x208
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_PADCFGLOCK                       0x2A0
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_PADCFGLOCKTX                     0x2A4
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_HOSTSW_OWN                       0x2C8
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_GPI_IS                           0x308
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_GPI_IE                           0x318
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_GPI_GPE_STS                      0x338
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_GPI_GPE_EN                       0x358
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_SMI_STS                          0x378
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_SMI_EN                           0x398
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_NMI_STS                          0x3B8
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_NMI_EN                           0x3D8
#define R_GPIOV2_PTL_PCD_PCR_PWMC                                   0x434
#define R_GPIOV2_PTL_PCD_PCR_VGPIO_PAD_CFG_DW0                      0xB60

#endif // _GPIO_REGS_VER2_H_
