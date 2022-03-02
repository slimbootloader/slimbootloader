/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_LPC_ADL_H_
#define _PCH_REGS_LPC_ADL_H_

#define V_LPC_CFG_DID_ADL_S                       0x7A80
#define V_LPC_CFG_DID_ADL_P                       0x5180
#define V_LPC_CFG_DID_ADL_PS                      0x5188
#define V_LPC_CFG_DID_ADL_N                       0x5480

//
// ADL PCH-P LPC Device IDs
//
#define V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_0         0x5180          ///< LPC/eSPI Controller
#define V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_1         0x5181          ///< LPC/eSPI Controller P SuperSKU
#define V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_2         0x5182          ///< LPC/eSPI Controller P Premium
#define V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_3         0x5183          ///< LPC/eSPI Controller Placeholder
#define V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_4         0x5184          ///< LPC/eSPI Controller
#define V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_5         0x5185          ///< LPC/eSPI Controller
#define V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_6         0x5186          ///< LPC/eSPI Controller M Super SKU
#define V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_7         0x5187          ///< LPC/eSPI Controller M Premium

//
// ADL PCH-M LPC Device IDs
//
#define V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_0         0x5480          ///< LPC/eSPI Controller
#define V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_1         0x5481          ///< LPC/eSPI Controller U SuperSKU
#define V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_2         0x5482          ///< LPC/eSPI Controller U Premiun
#define V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_3         0x5483          ///< LPC/eSPI Controller U Base/Mainstream
#define V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_4         0x5484          ///< LPC/eSPI Controller
#define V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_5         0x5485          ///< LPC/eSPI Controller
#define V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_6         0x5486          ///< LPC/eSPI Controller Y SuperSKU
#define V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_7         0x5487          ///< LPC/eSPI Controller Y Premium
#define V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_8         0x5488          ///< LPC/eSPI Controller IOT SKU

//
// ADL PCH-S LPC Device IDs
//
#define V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_0         0x7A80          ///< LPC/eSPI Controller
#define V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_1         0x7A81          ///< LPC/eSPI Controller Client SuperSKU (SSKU) unlocked
#define V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_2         0x7A82          ///< LPC/eSPI Controller Server SuperSKU (SSKU) unlocked
#define V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_3         0x7A83          ///< LPC/eSPI Controller Q670
#define V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_4         0x7A84          ///< LPC/eSPI Controller Z690
#define V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_5         0x7A85          ///< LPC/eSPI Controller H670
#define V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_6         0x7A86          ///< LPC/eSPI Controller B660
#define V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_7         0x7A87          ///< LPC/eSPI Controller H610
#endif
