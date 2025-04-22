/** @file
  Private definitions for PCH GSPI controller lib.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef __PCH_GSPI_LIB_PRIVATE_H__
#define __PCH_GSPI_LIB_PRIVATE_H__

#include <Uefi.h>
#include <Base.h>
#include <PchAccess.h>
#include <IndustryStandard/Pci.h>
#include <Library/IoLib.h>
#include <Library/PchGspiLib.h>
#include <Library/PchPciBdfLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchInfoLib.h>

#define PCH_GSPI_SSCR0      0x00
#define   PCH_GSPI_SSCR0_SSE  BIT7
#define PCH_GSPI_SSCR1      0x04
#define PCH_GSPI_SSSR       0x08
#define   PCH_GSPI_SSSR_TNF   BIT2
#define   PCH_GSPI_SSSR_RNE   BIT3
#define   PCH_GSPI_SSSR_BSY   BIT4
#define PCH_GSPI_SSDR       0x10
#define PCH_GSPI_SSTO       0x28
#define PCH_GSPI_SITF       0x44
#define   PCH_GSPI_SIRF_SITFL_SHIFT   16
#define   PCH_GSPI_SIRF_SITFL_MASK    (0x3F << PCH_GSPI_SIRF_SITFL_SHIFT)
#define   PCH_GSPI_SIRF_LWMTF_SHIFT   8
#define   PCH_GSPI_SIRF_LWMTF_MASK    (0x3F << PCH_GSPI_SIRF_LWMTF_SHIFT)
#define   PCH_GSPI_SIRF_HWMTF_MASK    0x3F
#define PCH_GSPI_SIRF       0x48
#define   PCH_GSPI_SIRF_WMRF_MASK    0x3F

#define PCH_GSPI_CLOCKS     0x200
#define PCH_GSPI_RESETS     0x204
#define PCH_GSPI_CLOCKS     0x200
#define PCH_GSPI_CS_CTL     0x224

#define PCH_GSPI_BLOCK_SIZE 0x30

#endif // __PCH_GSPI_LIB_PRIVATE_H__
