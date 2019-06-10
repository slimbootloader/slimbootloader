/** @file
  Register definitions for SD controllers on SCS.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PCH_REGS_SCS_SD_H_
#define _PCH_REGS_SCS_SD_H_

#define R_SCS_MEM_TX_CMD_DLL_CNTL                           0x820
#define R_SCS_MEM_TX_DATA_DLL_CNTL1                         0x824
#define R_SCS_MEM_TX_DATA_DLL_CNTL2                         0x828
#define R_SCS_MEM_RX_CMD_DATA_DLL_CNTL1                     0x82C
#define R_SCS_MEM_RX_STROBE_DLL_CNTL                        0x830
#define R_SCS_MEM_RX_CMD_DATA_DLL_CNTL2                     0x834

#define R_SCS_PCR_1C20                                      0x1C20
#define R_SCS_PCR_4820                                      0x4820

#endif
