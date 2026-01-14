/** @file
  Register names for MTL SOC LPC/eSPI device

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_LPC_PTL_SOC_P_
#define _PCH_REGS_LPC_PTL_SOC_P_

#define V_ESPI_CFG_DID_PTL_PCD_H                        0xE300
#define V_ESPI_CFG_DID_PTL_PCD_P                        0xE400
#define V_ESPI_CFG_DID_WCL_PCD                          0x4D00

//
// PTL PCD P LPC Device IDs
//
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_0             0xE400///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_1             0xE401///< LPC/eSPI Controller P SuperSKU
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_2             0xE402///< LPC/eSPI Controller P Premiun
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_3             0xE403///< LPC/eSPI Controller P Base/Mainstream
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_4             0xE404///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_5             0xE405///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_6             0xE406///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_7             0xE407///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_8             0xE408///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_9             0xE409///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_10            0xE40A///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_11            0xE40B///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_12            0xE40C///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_13            0xE40D///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_14            0xE40E///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_15            0xE40F///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_16            0xE410///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_17            0xE411///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_18            0xE412///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_19            0xE413///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_20            0xE414///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_21            0xE415///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_22            0xE416///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_23            0xE417///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_24            0xE418///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_25            0xE419///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_26            0xE41A///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_27            0xE41B///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_28            0xE41C///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_29            0xE41D///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_30            0xE41E///< LPC/eSPI Controller
#define V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_31            0xE41F///< LPC/eSPI Controller


//
// PTL PCD H LPC Device IDs
//
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_0             0xE300///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_1             0xE301///< LPC/eSPI Controller P SuperSKU
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_2             0xE302///< LPC/eSPI Controller P Premiun
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_3             0xE303///< LPC/eSPI Controller P Base/Mainstream
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_4             0xE304///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_5             0xE305///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_6             0xE306///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_7             0xE307///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_8             0xE308///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_9             0xE309///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_10            0xE30A///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_11            0xE30B///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_12            0xE30C///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_13            0xE30D///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_14            0xE30E///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_15            0xE30F///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_16            0xE310///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_17            0xE311///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_18            0xE312///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_19            0xE313///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_20            0xE314///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_21            0xE315///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_22            0xE316///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_23            0xE317///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_24            0xE318///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_25            0xE319///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_26            0xE31A///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_27            0xE31B///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_28            0xE31C///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_29            0xE31D///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_30            0xE31E///< LPC/eSPI Controller
#define V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_31            0xE31F///< LPC/eSPI Controller

//
// WCL PCD LPC Device IDs
//
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_0           0x4D00          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_1           0x4D01          ///< LPC/eSPI Controller P SuperSKU
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_2           0x4D02          ///< LPC/eSPI Controller P Premiun
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_3           0x4D03          ///< LPC/eSPI Controller P Base/Mainstream
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_4           0x4D04          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_5           0x4D05          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_6           0x4D06          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_7           0x4D07          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_8           0x4D08          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_9           0x4D09          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_10          0x4D0A          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_11          0x4D0B          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_12          0x4D0C          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_13          0x4D0D          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_14          0x4D0E          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_15          0x4D0F          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_16          0x4D10          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_17          0x4D11          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_18          0x4D12          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_19          0x4D13          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_20          0x4D14          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_21          0x4D15          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_22          0x4D16          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_23          0x4D17          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_24          0x4D18          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_25          0x4D19          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_26          0x4D1A          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_27          0x4D1B          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_28          0x4D1C          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_29          0x4D1D          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_30          0x4D1E          ///< LPC/eSPI Controller
#define V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_31          0x4D1F          ///< LPC/eSPI Controller

#endif
