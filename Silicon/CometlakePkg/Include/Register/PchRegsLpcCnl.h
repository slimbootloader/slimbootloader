/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_LPC_CNL_H_
#define _PCH_REGS_LPC_CNL_H_

#define V_LPC_CFG_DID_CNL_H                       0xA300
#define V_LPC_CFG_DID_CNL_LP                      0x9D80
#define V_LPC_CFG_DID_CML_H                       0x0680
#define V_LPC_CFG_DID_CML_LP                      0x0280

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

//
// CML PCH-LP Device IDs
//
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_SUPER_SKU   0x0280          ///< PCH LP Mobile Super SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_0           0x0281          ///< PCH LP Mobile (U) Super SKU (SSKU)
//#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_1           0x0282          ///< Reserved
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_2           0x0283          ///< PCH LP Mobile (Y) Premium SKU (RST)
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_3           0x0284          ///< PCH LP Mobile (U) Premium SKU (RST)
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_4           0x0285          ///< PCH LP Mobile (U) Base/Mainstream SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_5           0x0286          ///< PCH LP Mobile (Y) Super SKU (SSKU)
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_6           0x0287          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_7           0x0288          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_8           0x0289          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_9           0x028A          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_10          0x028B          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_11          0x028C          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_12          0x028D          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_13          0x028E          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_14          0x028F          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_15          0x0290          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_16          0x0291          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_17          0x0292          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_18          0x0293          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_19          0x0294          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_20          0x0295          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_21          0x0296          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_22          0x0297          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_23          0x0298          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_24          0x0299          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_25          0x029A          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_26          0x029B          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_27          0x029C          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_28          0x029D          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_29          0x029E          ///< PCH LP Mobile SKU
#define V_CML_PCH_LP_LPC_CFG_DEVICE_ID_MB_30          0x029F          ///< PCH LP Mobile SKU

//
// CML PCH-H LPC Device IDs
//
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0680_SKU        0x0680          ///< LPC/eSPI Controller
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0681_SKU        0x0681          ///< Client Super SKU (SSKU) unlocked
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0682_SKU        0x0682          ///< Client Super SKU (SSKU) locked
//#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0683_SKU        0x0683          ///< NA
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0684_SKU        0x0684          ///< PCH H Mobile H470
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0685_SKU        0x0685          ///< PCH H Mobile Z490
//#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0686_SKU        0x0686          ///< NA
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0687_SKU        0x0687          ///< PCH H Mobile Q490
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0688_SKU        0x0688          ///< PCH H Mobile B460
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0689_SKU        0x0689          ///< PCH H Mobile C256
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_068A_SKU        0x068A          ///< PCH H Mobile C252
//#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_068B_SKU        0x068B          ///< NA
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_068C_SKU        0x068C          ///< PCH H Mobile QM470
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_068D_SKU        0x068D          ///< PCH H Mobile HM470
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_068E_SKU        0x068E          ///< PCH H Mobile CM256
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_068F_SKU        0x068F          ///< PCH H Mobile Internal Super SKU (SSKU)
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0690_SKU        0x0690          ///< PCH H Mobile Workstation Server Super SKU (SSKU) Unlocked
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0691_SKU        0x0691          ///< PCH H Mobile Workstation Server Super SKU (SSKU) Locked
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0692_SKU        0x0692          ///< PCH H Mobile Small Form Factor (SFF) Super SKU (SSKU)
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0693_SKU        0x0693          ///< PCH H Mobile QMS480
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0694_SKU        0x0694          ///< PCH H Mobile Internal Production (PSKU)
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0695_SKU        0x0695          ///< PCH H Mobile Super SKU (SSKU) Unlocked
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0696_SKU        0x0696          ///< PCH H Mobile Super SKU (SSKU) Locked
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0697_SKU        0x0697          ///< PCH H Mobile SKU
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0698_SKU        0x0698          ///< PCH H Mobile SKU
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_0699_SKU        0x0699          ///< PCH H Mobile SKU
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_069A_SKU        0x069A          ///< PCH H Mobile SKU
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_069B_SKU        0x069B          ///< PCH H Mobile SKU
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_069C_SKU        0x069C          ///< PCH H Mobile SKU
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_069D_SKU        0x069D          ///< PCH H Mobile SKU
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_069E_SKU        0x069E          ///< PCH H Mobile SKU
#define V_CML_PCH_H_LPC_CFG_DEVICE_ID_069F_SKU        0x069F          ///< PCH H Mobile SKU

#endif
