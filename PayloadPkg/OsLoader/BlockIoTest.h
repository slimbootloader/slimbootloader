/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __BLOCK_IO_TEST_H__
#define __BLOCK_IO_TEST_H__

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PayloadLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/MmcAccessLib.h>
#include <Library/SpiBlockIoLib.h>
#include <Library/UfsBlockIoLib.h>
#include <Library/UsbBlockIoLib.h>
#include <Guid/OsBootOptionGuid.h>

#define TEST_DEVICE_WRITE     0

/**
  Perform the BlockIO test for the given device type.

  @param  OsBootOption   pointer to boot optoin info.

  @retval EFI_SUCCESS   on successful read/write test to the block dev

 **/
EFI_STATUS
TestDevBlocks (
  IN  OS_BOOT_OPTION           *OsBootOption
  );

#endif
