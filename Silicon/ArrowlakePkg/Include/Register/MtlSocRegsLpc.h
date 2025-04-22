/** @file
  Register names for MTL SOC LPC/eSPI device

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_LPC_MTL_SOC_H_
#define _PCH_REGS_LPC_MTL_SOC_H_

#define V_LPC_CFG_DID_MTL_SOC_M                    0x7E00
#define V_LPC_CFG_DID_MTL_SOC_S                    0xAE00
#define V_LPC_CFG_DID_ARL_SOC_M                    0x7700

//
// MTL SOC-M LPC Device IDs
//
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_0         0x7E00          ///< LPC/eSPI Controller
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_1         0x7E01          ///< LPC/eSPI Controller P SuperSKU
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_2         0x7E02          ///< LPC/eSPI Controller P Premiun
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_3         0x7E03          ///< LPC/eSPI Controller P Base/Mainstream
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_4         0x7E04          ///< LPC/eSPI Controller
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_5         0x7E05          ///< LPC/eSPI Controller
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_6         0x7E06          ///< LPC/eSPI Controller M SuperSKU
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_7         0x7E07          ///< LPC/eSPI Controller M Premium
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_8         0x7E08          ///< LPC/eSPI Controller
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_14        0x7E0E          ///< LPC/eSPI Controller
#define V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_15        0x7E0F          ///< LPC/eSPI Controller

#define V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_0         0xAE00          ///< LPC/eSPI Controller Unfused
#define V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_1         0xAE01          ///< LPC/eSPI Controller
#define V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_6         0xAE06          ///< LPC/eSPI Controller
#define V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_10        0xAE0A          ///< LPC/eSPI Controller
#define V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_11        0xAE0B          ///< LPC/eSPI Controller
#define V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_16        0xAE10          ///< LPC/eSPI Controller
#define V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_17        0xAE11          ///< LPC/eSPI Controller
#define V_ARL_SOC_M_LPC_CFG_DEVICE_ID_MB_0         0x7700          ///< LPC/eSPI Controller Unfused
#define V_ARL_SOC_M_LPC_CFG_DEVICE_ID_MB_8         0x7708          ///< LPC/eSPI Controller
#define V_ARL_SOC_M_LPC_CFG_DEVICE_ID_MB_9         0x7709          ///< LPC/eSPI Controller

#endif
