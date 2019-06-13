/** @file

  Provides some data structure definitions used by the SD/MMC host controller driver.

Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SD_MMC_TUNING_LIB_PRIVATE_H_
#define _SD_MMC_TUNING_LIB_PRIVATE_H_

#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Emmc.h>
#include <IndustryStandard/Sd.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#define RX_STROBE_DLL1_TAP_MAX_RANGE              39
#define RX_STROBE_DLL1_TAP_MIN_RANGE              0
#define RX_STROBE_DLL1_TAP_MIN_MEPT               5
#define RX_STROBE_DLL1_TAP_MAX_MEPT               16
#define TX_DATA_DLL_TAP_MAX_RANGE                 79
#define TX_DATA_DLL_TAP_MIN_RANGE                 0
#define TX_DATA_DLL_TAP_MIN_MEPT                  4
#define TX_DATA_DLL_TAP_MAX_MEPT                  22
#define EMMC_HS400_TUNING_PATTERN_BLOCKS_NUMBER   5


/**
  This structure decribes the required Emmc info for HS400 tuning
**/
typedef struct {
  EFI_HANDLE                PartitionHandle;    ///< eMMC partition handle for block read/write
  EFI_LBA                   Lba;                ///< Logical Block Address for HS400 Tuning block read/write
  UINT32                    RelativeDevAddress; ///< Device system address, dynamically assigned by the host during initialization.
  UINT8                     HS200BusWidth;      ///< The value to be programmed for BUS_WIDTH[183] byte
} EMMC_INFO;

typedef enum {
  RxDll1 = 0,
  RxDll2
} RX_STROBE_DLL_REG;

typedef enum {
  NotAvailable = 0,
  Passed,
  Failed
} BLOCK_READ_WRITE_STATUS;

#endif
