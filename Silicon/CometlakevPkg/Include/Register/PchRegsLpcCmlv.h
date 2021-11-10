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

//
// CMLV PCH-H LPC Device IDs
//
#define V_CMLV_PCH_H_LPC_DEVICE_ID_DT_SUPER_SKU     0xA3C1          ///< Super SKU client
#define V_CMLV_PCH_H_LPC_CFG_DEVICE_ID_A3C8         0xA3C8          ///< PCH V Mobile B460C
#define V_CMLV_PCH_H_LPC_CFG_DEVICE_ID_A3DA         0xA3DA          ///< PCH V Mobile H410C

//
// Device IDs that are KBL PCH-H Desktop specific
//
#define IS_PCH_H_LPC_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_CMLV_PCH_H_LPC_DEVICE_ID_DT_SUPER_SKU) || \
      (DeviceId == V_CMLV_PCH_H_LPC_CFG_DEVICE_ID_A3C8)  || \
      (DeviceId == V_CMLV_PCH_H_LPC_CFG_DEVICE_ID_A3DA)  \
    )
#endif
