/** @file
  Register names for CDF GPIO

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_GPIO_CDF_H_
#define _PCH_REGS_GPIO_CDF_H_

// Group: 2,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_A_PAD_OWN               0x0020
#define R_CDF_PCH_GPIO_PCR_GPP_A_HOSTSW_OWN            0x0120
#define R_CDF_PCH_GPIO_PCR_GPP_A_GPI_IS                0x0200
#define R_CDF_PCH_GPIO_PCR_GPP_A_GPI_IE                0x0230
#define R_CDF_PCH_GPIO_PCR_GPP_A_GPI_GPE_STS           0x0260
#define R_CDF_PCH_GPIO_PCR_GPP_A_GPI_GPE_EN            0x0290
#define R_CDF_PCH_GPIO_PCR_GPP_A_SMI_STS               0x02c0
#define R_CDF_PCH_GPIO_PCR_GPP_A_SMI_EN                0x02f0
#define R_CDF_PCH_GPIO_PCR_GPP_A_NMI_STS               0x0320
#define R_CDF_PCH_GPIO_PCR_GPP_A_NMI_EN                0x0350
#define R_CDF_PCH_GPIO_PCR_GPP_A_PADCFGLOCK            0x00c0
#define R_CDF_PCH_GPIO_PCR_GPP_A_PADCFGLOCKTX          0x00c4
#define V_PCH_GPIO_GPP_A_PAD_MAX 24
// Group: 3,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_B_PAD_OWN               0x002c
#define R_CDF_PCH_GPIO_PCR_GPP_B_HOSTSW_OWN            0x0124
#define R_CDF_PCH_GPIO_PCR_GPP_B_GPI_IS                0x0204
#define R_CDF_PCH_GPIO_PCR_GPP_B_GPI_IE                0x0234
#define R_CDF_PCH_GPIO_PCR_GPP_B_GPI_GPE_STS           0x0264
#define R_CDF_PCH_GPIO_PCR_GPP_B_GPI_GPE_EN            0x0294
#define R_CDF_PCH_GPIO_PCR_GPP_B_SMI_STS               0x02c4
#define R_CDF_PCH_GPIO_PCR_GPP_B_SMI_EN                0x02f4
#define R_CDF_PCH_GPIO_PCR_GPP_B_NMI_STS               0x0324
#define R_CDF_PCH_GPIO_PCR_GPP_B_NMI_EN                0x0354
#define R_CDF_PCH_GPIO_PCR_GPP_B_PADCFGLOCK            0x00c8
#define R_CDF_PCH_GPIO_PCR_GPP_B_PADCFGLOCKTX          0x00cc
#define V_PCH_GPIO_GPP_B_PAD_MAX 24
// Group: 1,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_C_PAD_OWN               0x0038
#define R_CDF_PCH_GPIO_PCR_GPP_C_HOSTSW_OWN            0x0128
#define R_CDF_PCH_GPIO_PCR_GPP_C_GPI_IS                0x0208
#define R_CDF_PCH_GPIO_PCR_GPP_C_GPI_IE                0x0238
#define R_CDF_PCH_GPIO_PCR_GPP_C_GPI_GPE_STS           0x0268
#define R_CDF_PCH_GPIO_PCR_GPP_C_GPI_GPE_EN            0x0298
#define R_CDF_PCH_GPIO_PCR_GPP_C_SMI_STS               0x02c8
#define R_CDF_PCH_GPIO_PCR_GPP_C_SMI_EN                0x02f8
#define R_CDF_PCH_GPIO_PCR_GPP_C_NMI_STS               0x0328
#define R_CDF_PCH_GPIO_PCR_GPP_C_NMI_EN                0x0358
#define R_CDF_PCH_GPIO_PCR_GPP_C_PADCFGLOCK            0x00d0
#define R_CDF_PCH_GPIO_PCR_GPP_C_PADCFGLOCKTX          0x00d4
#define V_PCH_GPIO_GPP_C_PAD_MAX 23
// Group: 5,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_D_PAD_OWN               0x0044
#define R_CDF_PCH_GPIO_PCR_GPP_D_HOSTSW_OWN            0x012c
#define R_CDF_PCH_GPIO_PCR_GPP_D_GPI_IS                0x020c
#define R_CDF_PCH_GPIO_PCR_GPP_D_GPI_IE                0x023c
#define R_CDF_PCH_GPIO_PCR_GPP_D_GPI_GPE_STS           0x026c
#define R_CDF_PCH_GPIO_PCR_GPP_D_GPI_GPE_EN            0x029c
#define R_CDF_PCH_GPIO_PCR_GPP_D_SMI_STS               0x02cc
#define R_CDF_PCH_GPIO_PCR_GPP_D_SMI_EN                0x02fc
#define R_CDF_PCH_GPIO_PCR_GPP_D_NMI_STS               0x032c
#define R_CDF_PCH_GPIO_PCR_GPP_D_NMI_EN                0x035c
#define R_CDF_PCH_GPIO_PCR_GPP_D_PADCFGLOCK            0x00d8
#define R_CDF_PCH_GPIO_PCR_GPP_D_PADCFGLOCKTX          0x00dc
#define V_PCH_GPIO_GPP_D_PAD_MAX 20
// Group: c,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_E_PAD_OWN               0x0050
#define R_CDF_PCH_GPIO_PCR_GPP_E_HOSTSW_OWN            0x0130
#define R_CDF_PCH_GPIO_PCR_GPP_E_GPI_IS                0x0210
#define R_CDF_PCH_GPIO_PCR_GPP_E_GPI_IE                0x0240
#define R_CDF_PCH_GPIO_PCR_GPP_E_GPI_GPE_STS           0x0270
#define R_CDF_PCH_GPIO_PCR_GPP_E_GPI_GPE_EN            0x02a0
#define R_CDF_PCH_GPIO_PCR_GPP_E_SMI_STS               0x02d0
#define R_CDF_PCH_GPIO_PCR_GPP_E_SMI_EN                0x0300
#define R_CDF_PCH_GPIO_PCR_GPP_E_NMI_STS               0x0330
#define R_CDF_PCH_GPIO_PCR_GPP_E_NMI_EN                0x0360
#define R_CDF_PCH_GPIO_PCR_GPP_E_PADCFGLOCK            0x00e0
#define R_CDF_PCH_GPIO_PCR_GPP_E_PADCFGLOCKTX          0x00e4
#define V_PCH_GPIO_GPP_E_PAD_MAX 6
// Group: x,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_F_PAD_OWN               0x0054
#define R_CDF_PCH_GPIO_PCR_GPP_F_HOSTSW_OWN            0x0134
#define R_CDF_PCH_GPIO_PCR_GPP_F_GPI_IS                0x0214
#define R_CDF_PCH_GPIO_PCR_GPP_F_GPI_IE                0x0244
#define R_CDF_PCH_GPIO_PCR_GPP_F_GPI_GPE_STS           0x0274
#define R_CDF_PCH_GPIO_PCR_GPP_F_GPI_GPE_EN            0x02a4
#define R_CDF_PCH_GPIO_PCR_GPP_F_SMI_STS               0x02d4
#define R_CDF_PCH_GPIO_PCR_GPP_F_SMI_EN                0x0304
#define R_CDF_PCH_GPIO_PCR_GPP_F_NMI_STS               0x0334
#define R_CDF_PCH_GPIO_PCR_GPP_F_NMI_EN                0x0364
#define R_CDF_PCH_GPIO_PCR_GPP_F_PADCFGLOCK            0x00e8
#define R_CDF_PCH_GPIO_PCR_GPP_F_PADCFGLOCKTX          0x00ec
#define V_PCH_GPIO_GPP_F_PAD_MAX 5
// Group: a,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_G_PAD_OWN               0x0058
#define R_CDF_PCH_GPIO_PCR_GPP_G_HOSTSW_OWN            0x0138
#define R_CDF_PCH_GPIO_PCR_GPP_G_GPI_IS                0x0218
#define R_CDF_PCH_GPIO_PCR_GPP_G_GPI_IE                0x0248
#define R_CDF_PCH_GPIO_PCR_GPP_G_GPI_GPE_STS           0x0278
#define R_CDF_PCH_GPIO_PCR_GPP_G_GPI_GPE_EN            0x02a8
#define R_CDF_PCH_GPIO_PCR_GPP_G_SMI_STS               0x02d8
#define R_CDF_PCH_GPIO_PCR_GPP_G_SMI_EN                0x0308
#define R_CDF_PCH_GPIO_PCR_GPP_G_NMI_STS               0x0338
#define R_CDF_PCH_GPIO_PCR_GPP_G_NMI_EN                0x0368
#define R_CDF_PCH_GPIO_PCR_GPP_G_PADCFGLOCK            0x00f0
#define R_CDF_PCH_GPIO_PCR_GPP_G_PADCFGLOCKTX          0x00f4
#define V_PCH_GPIO_GPP_G_PAD_MAX 10
// Group: b,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_H_PAD_OWN               0x0060
#define R_CDF_PCH_GPIO_PCR_GPP_H_HOSTSW_OWN            0x013c
#define R_CDF_PCH_GPIO_PCR_GPP_H_GPI_IS                0x021c
#define R_CDF_PCH_GPIO_PCR_GPP_H_GPI_IE                0x024c
#define R_CDF_PCH_GPIO_PCR_GPP_H_GPI_GPE_STS           0x027c
#define R_CDF_PCH_GPIO_PCR_GPP_H_GPI_GPE_EN            0x02ac
#define R_CDF_PCH_GPIO_PCR_GPP_H_SMI_STS               0x02dc
#define R_CDF_PCH_GPIO_PCR_GPP_H_SMI_EN                0x030c
#define R_CDF_PCH_GPIO_PCR_GPP_H_NMI_STS               0x033c
#define R_CDF_PCH_GPIO_PCR_GPP_H_NMI_EN                0x036c
#define R_CDF_PCH_GPIO_PCR_GPP_H_PADCFGLOCK            0x00f8
#define R_CDF_PCH_GPIO_PCR_GPP_H_PADCFGLOCKTX          0x00fc
#define V_PCH_GPIO_GPP_H_PAD_MAX 12
// Group: d,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_I_PAD_OWN               0x0068
#define R_CDF_PCH_GPIO_PCR_GPP_I_HOSTSW_OWN            0x0140
#define R_CDF_PCH_GPIO_PCR_GPP_I_GPI_IS                0x0220
#define R_CDF_PCH_GPIO_PCR_GPP_I_GPI_IE                0x0250
#define R_CDF_PCH_GPIO_PCR_GPP_I_GPI_GPE_STS           0x0280
#define R_CDF_PCH_GPIO_PCR_GPP_I_GPI_GPE_EN            0x02b0
#define R_CDF_PCH_GPIO_PCR_GPP_I_SMI_STS               0x02e0
#define R_CDF_PCH_GPIO_PCR_GPP_I_SMI_EN                0x0310
#define R_CDF_PCH_GPIO_PCR_GPP_I_NMI_STS               0x0340
#define R_CDF_PCH_GPIO_PCR_GPP_I_NMI_EN                0x0370
#define R_CDF_PCH_GPIO_PCR_GPP_I_PADCFGLOCK            0x0100
#define R_CDF_PCH_GPIO_PCR_GPP_I_PADCFGLOCKTX          0x0104
#define V_PCH_GPIO_GPP_I_PAD_MAX 20
// Group: i,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_J_PAD_OWN               0x0074
#define R_CDF_PCH_GPIO_PCR_GPP_J_HOSTSW_OWN            0x0144
#define R_CDF_PCH_GPIO_PCR_GPP_J_GPI_IS                0x0224
#define R_CDF_PCH_GPIO_PCR_GPP_J_GPI_IE                0x0254
#define R_CDF_PCH_GPIO_PCR_GPP_J_GPI_GPE_STS           0x0284
#define R_CDF_PCH_GPIO_PCR_GPP_J_GPI_GPE_EN            0x02b4
#define R_CDF_PCH_GPIO_PCR_GPP_J_SMI_STS               0x02e4
#define R_CDF_PCH_GPIO_PCR_GPP_J_SMI_EN                0x0314
#define R_CDF_PCH_GPIO_PCR_GPP_J_NMI_STS               0x0344
#define R_CDF_PCH_GPIO_PCR_GPP_J_NMI_EN                0x0374
#define R_CDF_PCH_GPIO_PCR_GPP_J_PADCFGLOCK            0x0108
#define R_CDF_PCH_GPIO_PCR_GPP_J_PADCFGLOCKTX          0x010c
#define V_PCH_GPIO_GPP_J_PAD_MAX 1
// Group: f,SB iD: c2
#define R_CDF_PCH_GPIO_PCR_GPP_K_PAD_OWN               0x0078
#define R_CDF_PCH_GPIO_PCR_GPP_K_HOSTSW_OWN            0x0148
#define R_CDF_PCH_GPIO_PCR_GPP_K_GPI_IS                0x0228
#define R_CDF_PCH_GPIO_PCR_GPP_K_GPI_IE                0x0258
#define R_CDF_PCH_GPIO_PCR_GPP_K_GPI_GPE_STS           0x0288
#define R_CDF_PCH_GPIO_PCR_GPP_K_GPI_GPE_EN            0x02b8
#define R_CDF_PCH_GPIO_PCR_GPP_K_SMI_STS               0x02e8
#define R_CDF_PCH_GPIO_PCR_GPP_K_SMI_EN                0x0318
#define R_CDF_PCH_GPIO_PCR_GPP_K_NMI_STS               0x0348
#define R_CDF_PCH_GPIO_PCR_GPP_K_NMI_EN                0x0378
#define R_CDF_PCH_GPIO_PCR_GPP_K_PADCFGLOCK            0x0110
#define R_CDF_PCH_GPIO_PCR_GPP_K_PADCFGLOCKTX          0x0114
#define V_PCH_GPIO_GPP_K_PAD_MAX 23
// Group: 2,SB iD: c5
#define R_CDF_PCH_GPIO_PCR_GPP_L_PAD_OWN               0x0020
#define R_CDF_PCH_GPIO_PCR_GPP_L_HOSTSW_OWN            0x0120
#define R_CDF_PCH_GPIO_PCR_GPP_L_GPI_IS                0x0200
#define R_CDF_PCH_GPIO_PCR_GPP_L_GPI_IE                0x0230
#define R_CDF_PCH_GPIO_PCR_GPP_L_GPI_GPE_STS           0x0260
#define R_CDF_PCH_GPIO_PCR_GPP_L_GPI_GPE_EN            0x0290
#define R_CDF_PCH_GPIO_PCR_GPP_L_SMI_STS               0x02c0
#define R_CDF_PCH_GPIO_PCR_GPP_L_SMI_EN                0x02f0
#define R_CDF_PCH_GPIO_PCR_GPP_L_NMI_STS               0x0320
#define R_CDF_PCH_GPIO_PCR_GPP_L_NMI_EN                0x0350
#define R_CDF_PCH_GPIO_PCR_GPP_L_PADCFGLOCK            0x00c0
#define R_CDF_PCH_GPIO_PCR_GPP_L_PADCFGLOCKTX          0x00c4
#define V_PCH_GPIO_GPP_L_PAD_MAX 24
// Group: 3,SB iD: c5
#define R_CDF_PCH_GPIO_PCR_GPP_M_PAD_OWN               0x002c
#define R_CDF_PCH_GPIO_PCR_GPP_M_HOSTSW_OWN            0x0124
#define R_CDF_PCH_GPIO_PCR_GPP_M_GPI_IS                0x0204
#define R_CDF_PCH_GPIO_PCR_GPP_M_GPI_IE                0x0234
#define R_CDF_PCH_GPIO_PCR_GPP_M_GPI_GPE_STS           0x0264
#define R_CDF_PCH_GPIO_PCR_GPP_M_GPI_GPE_EN            0x0294
#define R_CDF_PCH_GPIO_PCR_GPP_M_SMI_STS               0x02c4
#define R_CDF_PCH_GPIO_PCR_GPP_M_SMI_EN                0x02f4
#define R_CDF_PCH_GPIO_PCR_GPP_M_NMI_STS               0x0324
#define R_CDF_PCH_GPIO_PCR_GPP_M_NMI_EN                0x0354
#define R_CDF_PCH_GPIO_PCR_GPP_M_PADCFGLOCK            0x00c8
#define R_CDF_PCH_GPIO_PCR_GPP_M_PADCFGLOCKTX          0x00cc
#define V_PCH_GPIO_GPP_M_PAD_MAX 11
// Group: 0,SB iD: c5
#define R_CDF_PCH_GPIO_PCR_GPP_N_PAD_OWN               0x0034
#define R_CDF_PCH_GPIO_PCR_GPP_N_HOSTSW_OWN            0x0128
#define R_CDF_PCH_GPIO_PCR_GPP_N_GPI_IS                0x0208
#define R_CDF_PCH_GPIO_PCR_GPP_N_GPI_IE                0x0238
#define R_CDF_PCH_GPIO_PCR_GPP_N_GPI_GPE_STS           0x0268
#define R_CDF_PCH_GPIO_PCR_GPP_N_GPI_GPE_EN            0x0298
#define R_CDF_PCH_GPIO_PCR_GPP_N_SMI_STS               0x02c8
#define R_CDF_PCH_GPIO_PCR_GPP_N_SMI_EN                0x02f8
#define R_CDF_PCH_GPIO_PCR_GPP_N_NMI_STS               0x0328
#define R_CDF_PCH_GPIO_PCR_GPP_N_NMI_EN                0x0358
#define R_CDF_PCH_GPIO_PCR_GPP_N_PADCFGLOCK            0x00d0
#define R_CDF_PCH_GPIO_PCR_GPP_N_PADCFGLOCKTX          0x00d4
#define V_PCH_GPIO_GPP_N_PAD_MAX 23
// Group: c,SB iD: c5
#define R_CDF_PCH_GPIO_PCR_GPP_O_PAD_OWN               0x0040
#define R_CDF_PCH_GPIO_PCR_GPP_O_HOSTSW_OWN            0x012c
#define R_CDF_PCH_GPIO_PCR_GPP_O_GPI_IS                0x020c
#define R_CDF_PCH_GPIO_PCR_GPP_O_GPI_IE                0x023c
#define R_CDF_PCH_GPIO_PCR_GPP_O_GPI_GPE_STS           0x026c
#define R_CDF_PCH_GPIO_PCR_GPP_O_GPI_GPE_EN            0x029c
#define R_CDF_PCH_GPIO_PCR_GPP_O_SMI_STS               0x02cc
#define R_CDF_PCH_GPIO_PCR_GPP_O_SMI_EN                0x02fc
#define R_CDF_PCH_GPIO_PCR_GPP_O_NMI_STS               0x032c
#define R_CDF_PCH_GPIO_PCR_GPP_O_NMI_EN                0x035c
#define R_CDF_PCH_GPIO_PCR_GPP_O_PADCFGLOCK            0x00d8
#define R_CDF_PCH_GPIO_PCR_GPP_O_PADCFGLOCKTX          0x00dc
#define V_PCH_GPIO_GPP_O_PAD_MAX 11


#define S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET             0x10 //Size of the 4 32b DW configuration registers
//
//WEST C2
//
#define V_CDF_PCH_GPIO_PCR_GPP_A_PADCFG_OFFSET         0x800  //First DW configuration register
#define V_CDF_PCH_GPIO_PCR_GPP_B_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_A_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_A_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_C_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_B_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_B_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_D_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_C_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_C_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_E_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_D_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_D_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_F_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_E_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_E_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_G_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_F_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_F_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_H_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_G_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_G_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_I_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_H_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_H_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_J_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_I_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_I_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_K_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_J_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_J_PAD_MAX))
//EAST C5
#define V_CDF_PCH_GPIO_PCR_GPP_L_PADCFG_OFFSET         0x800 //First DW configuration register
#define V_CDF_PCH_GPIO_PCR_GPP_M_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_L_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_L_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_N_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_M_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_M_PAD_MAX))
#define V_CDF_PCH_GPIO_PCR_GPP_O_PADCFG_OFFSET         (V_CDF_PCH_GPIO_PCR_GPP_N_PADCFG_OFFSET + (S_CDF_PCH_GPIO_PAD_CFG_ADDR_OFFSET * V_PCH_GPIO_GPP_N_PAD_MAX))


//
// PCH-H GPIO registers
//
//
// GPIO Community Common Private Configuration Registers
//
// TODO: Update values for CDF
#define V_CNL_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_A      0x0

#endif
