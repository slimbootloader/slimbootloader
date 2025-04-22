/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_LPC_CNL_H_
#define _PCH_REGS_LPC_CNL_H_

#define V_LPC_CFG_DID_CNL_H                       0xA300
#define V_LPC_CFG_DID_CNL_LP                      0x9D80

#define CNL_PCH_H_MIN_SUPPORTED_STEPPING          PCH_A0
#define CNL_PCH_LP_MIN_SUPPORTED_STEPPING         PCH_A0

//
// CNL PCH-LP Device IDs
//
#define V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_SUPER_SKU   0x9D80          ///< PCH LP Mobile Super SKU
#define V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_0           0x9D81          ///< PCH LP Mobile (U) Super SKU
#define V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_1           0x9D82          ///< PCH LP Mobile Super SKU Locked
#define V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_2           0x9D83          ///< PCH LP Mobile (Y) Premium SKU
#define V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_3           0x9D84          ///< PCH LP Mobile (U) Premium SKU
#define V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_4           0x9D85          ///< PCH LP Mobile (U) Base/Mainstream SKU
#define V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_5           0x9D86          ///< PCH LP Mobile (Y) Super SKU

//
// CNL PCH-H Desktop LPC Device IDs
//
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A300_SKU        0xA300          ///< LPC/eSPI Controller
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A301_SKU        0xA301          ///< Client Super SKU (SSKU) unlocked
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A302_SKU        0xA302          ///< Client Super SKU (SSKU) locked
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A303_SKU        0xA303          ///< PCH H Mobile H310
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A304_SKU        0xA304          ///< PCH H Mobile H370
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A305_SKU        0xA305          ///< PCH H Mobile Z390
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A306_SKU        0xA306          ///< PCH H Mobile Q370
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A307_SKU        0xA307          ///< PCH H Mobile Q350
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A308_SKU        0xA308          ///< PCH H Mobile B360
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A309_SKU        0xA309          ///< PCH H Mobile C246
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30A_SKU        0xA30A          ///< PCH H Mobile C242
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30B_SKU        0xA30B          ///< PCH H Mobile X399
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30C_SKU        0xA30C          ///< PCH H Mobile QM370
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30D_SKU        0xA30D          ///< PCH H Mobile HM370
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30E_SKU        0xA30E          ///< PCH H Mobile CM246
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30F_SKU        0xA30F          ///< PCH H Mobile Internal Super SKU (SSKU)
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A310_SKU        0xA310          ///< PCH H Mobile Server Super SKU (SSKU) Unlocked
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A311_SKU        0xA311          ///< PCH H Mobile Server Super SKU (SSKU) Locked
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A312_SKU        0xA312          ///< PCH H Mobile Small Form Factor (SFF) Super SKU (SSKU)
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A313_SKU        0xA313          ///< PCH H Mobile Small Form Factor (SFF)
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A314_SKU        0xA314          ///< PCH H Mobile Internal Production
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A315_SKU        0xA315
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A316_SKU        0xA316
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A317_SKU        0xA317
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A318_SKU        0xA318
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A319_SKU        0xA319
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A31A_SKU        0xA31A
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A31B_SKU        0xA31B
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A31C_SKU        0xA31C
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A31D_SKU        0xA31D
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A31E_SKU        0xA31E
#define V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A31F_SKU        0xA31F

#endif
