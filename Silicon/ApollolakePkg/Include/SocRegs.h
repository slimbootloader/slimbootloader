/** @file

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SOC_REGS_H_

#define DEFAULT_PCI_BUS_NUMBER_SC             0

#define PCI_DEVICE_NUMBER_LPSS_HSUART         24
#define PCI_FUNCTION_NUMBER_LPSS_HSUART0      0
#define R_LPSS_IO_STSCMD                      0x04
#define R_LPSS_IO_BAR                         0x10
#define R_LPSS_IO_BAR1                        0x18

#define PCI_DEVICE_NUMBER_P2SB                13
#define PCI_FUNCTION_NUMBER_P2SB              0
#define R_P2SB_BASE                           0x10

#define PCI_DEVICE_NUMBER_PMC                 13
#define PCI_FUNCTION_NUMBER_PMC               1
#define PCI_FUNCTION_NUMBER_PMC_SSRAM         3

#define V_FSP_LPSS_UART_PPR_CLK_N_DIV         32767
#define V_FSP_LPSS_UART_PPR_CLK_M_DIV         602
#define LPSS_UART_TMP_BAR_DELTA               0x10000

#include <SaRegs.h>

#include <ScRegs/ScReservedResources.h>
#include <ScRegs/RegsPmc.h>
#include <ScRegs/RegsP2sb.h>
#include <ScRegs/RegsPcu.h>
#include <ScRegs/RegsLpss.h>
#include <ScRegs/RegsPcie.h>
#include <ScRegs/RegsGpio.h>
#include <ScRegs/RegsPcr.h>
#include <ScRegs/RegsSmbus.h>

#define SC_PCIE_ROOT_PORT_BUS(RpNumber)       ((RpNumber < 2) ? PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_1 : PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_2)
#define SC_PCIE_ROOT_PORT_FUNC(RpNumber)      ((RpNumber < 2) ? RpNumber : RpNumber - 2)

// from RegsScc.h in FSP
#define R_SCC_MEM_TX_CMD_DLL_CNTL                           0x820  ///< Tx CMD Path Ctrl
#define R_SCC_MEM_TX_DATA_DLL_CNTL1                         0x824  ///< Tx Data Path Ctrl 1
#define R_SCC_MEM_TX_DATA_DLL_CNTL2                         0x828  ///< Tx Data Path Ctrl 2
#define R_SCC_MEM_RX_CMD_DATA_DLL_CNTL1                     0x82C  ///< Rx CMD&Data Path Ctrl 1
#define R_SCC_MEM_RX_STROBE_DLL_CNTL                        0x830  ///< Rx Strobe Ctrl Path
#define R_SCC_MEM_RX_CMD_DATA_DLL_CNTL2                     0x834  ///< Rx CMD&Data Path Ctrl 2

#endif
