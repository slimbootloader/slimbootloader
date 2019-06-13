/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "BlockIoTest.h"

#if  TEST_DEVICE_WRITE

/**
  Perform the BlockIO test for the given device type.

  @param  OsBootOption  pointer to boot optoin info.

  @retval EFI_SUCCESS   on successful read/write test to the block dev

 **/
EFI_STATUS
TestDevBlocks (
  IN  OS_BOOT_OPTION           *OsBootOption
  )
{
  RETURN_STATUS                Status ;
  UINT8                        *Buffer;
  UINT32                       Index;
  UINT8                        *TestData1;
  UINT8                        *TestData2;
  DEVICE_BLOCK_INFO            BlockInfo;
  UINT64                       TestLba;
  UINTN                        BootMediumPciBase;
  DEVICE_BLOCK_FUNC            DevBlockFunc;
  EFI_PEI_BLOCK_DEVICE_TYPE    DevType;
  //
  // Get OS boot device address
  //
  BootMediumPciBase = GetDeviceAddr (OsBootOption->DevType, OsBootOption->DevInstance);
  BootMediumPciBase = TO_MM_PCI_ADDRESS (BootMediumPciBase);
  DEBUG ((DEBUG_INFO, "BootMediumPciBase(0x%x)\n", BootMediumPciBase));

  //
  // Init Boot device functions
  //
  if (OsBootOption->DevType == OsBootDeviceEmmc) {
    DevBlockFunc.DevInit     = MmcInitialize;
    DevBlockFunc.GetInfo     = MmcGetMediaInfo;
    DevBlockFunc.ReadBlocks  = MmcReadBlocks;
    DevBlockFunc.WriteBlocks = MmcWriteBlocks;
    DevType = EMMC;
  } else if (OsBootOption->DevType == OsBootDeviceSd) {
    DevBlockFunc.DevInit     = SdInitialize;
    DevBlockFunc.GetInfo     = MmcGetMediaInfo;
    DevBlockFunc.ReadBlocks  = MmcReadBlocks;
    DevBlockFunc.WriteBlocks = MmcWriteBlocks;
    DevType = SD;
  } else if (OsBootOption->DevType == OsBootDeviceUfs) {
    DevBlockFunc.DevInit     = InitializeUfs;
    DevBlockFunc.GetInfo     = UfsGetMediaInfo;
    DevBlockFunc.ReadBlocks  = UfsReadBlocks;
    DevBlockFunc.WriteBlocks = UfsWriteBlocks;
    DevType = UfsDevice;
  } else if (OsBootOption->DevType == OsBootDeviceNvme) {
    DevBlockFunc.DevInit     = NvmeInitialize;
    DevBlockFunc.GetInfo     = NvmeGetMediaInfo;
    DevBlockFunc.ReadBlocks  = NvmeReadBlocks;
    DevBlockFunc.WriteBlocks = NvmeWriteBlocks;
    DevType = UfsDevice;
  } else {
    DEBUG ((DEBUG_ERROR, "Invalid Boot device configured!"));
    return RETURN_UNSUPPORTED;
  }

  // Prepare buffer
  Buffer    = (UINT8 *)AllocatePool (4096 * 3);
  TestData1 = Buffer    + 4096;
  TestData2 = TestData1 + 4096;
  SetMem32 (TestData1, 4096 / 4, 0x00FF5AA5);
  SetMem32 (TestData2, 4096 / 4, 0x11224488);


  //Init the device.
  Status = DevBlockFunc.DevInit (BootMediumPciBase, DevInitAll);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Mmcinitialize Error %r\n", Status));
    return Status;
  }

  // Test GetInfo, read and write
  for (Index = 0; Index < 7; Index++ ) {
    ZeroMem (&BlockInfo, sizeof (BlockInfo));
    Status = DevBlockFunc.GetInfo (Index, &BlockInfo);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "GetInfo [%d] %r\n", Index, Status));
      continue;
    }
    DEBUG ((DEBUG_INFO, "\nP[%d]: BlockNum=0x%lx BlockSize=0x%x\n", Index, BlockInfo.BlockNum, BlockInfo.BlockSize));

    if ((BlockInfo.BlockNum != 0) && (BlockInfo.BlockSize != 0)) {
      // Test last block read/write
      TestLba = BlockInfo.BlockNum - 1;

      // test read
      DEBUG ((DEBUG_INFO, "    P[%d]: Try to read block 0x%lx with BlockSize=0x%x\n", Index, TestLba, BlockInfo.BlockSize));
      Status = DevBlockFunc.ReadBlocks (Index, TestLba, BlockInfo.BlockSize, Buffer);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "    P[%d]: Read block Status = %r\n", Index, Status));
        continue;
      }
      DEBUG ((DEBUG_INFO, "    P[%d]: Dump Read Data\n", Index));
      DumpBuffer (Buffer, 0x200);

      // test write
      DEBUG ((DEBUG_INFO, "    P[%d]: try to write block 0x%lx with BlockSize=0x%x\n", Index, TestLba, BlockInfo.BlockSize));
      if (* (UINT32 *)Buffer == * (UINT32 *)TestData1) {
        Status = DevBlockFunc.WriteBlocks (Index, TestLba, BlockInfo.BlockSize, TestData2);
        DEBUG ((DEBUG_INFO, "    P[%d]: write 0x%x to block\n", Index, * (UINT32 *)TestData2));
      } else {
        Status = DevBlockFunc.WriteBlocks (Index, TestLba, BlockInfo.BlockSize, TestData1);
        DEBUG ((DEBUG_INFO, "    P[%d]: write 0x%x to block\n", * (UINT32 *)TestData1));
      }
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "        P[%d]: Write block Status = %r\n", Index, Status));
        continue;
      }
      DEBUG ((DEBUG_INFO, "    P[%d]: try to re-read block 0x%lx with BlockSize=0x%x\n", Index, TestLba,
              BlockInfo.BlockSize));
      Status = DevBlockFunc.ReadBlocks (Index, TestLba, BlockInfo.BlockSize, Buffer);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "        P[%d]: Read written data. Status = %r\n", Index, Status));
        continue;
      }
      DEBUG ((DEBUG_INFO, "         P[%d]: Dump Read Data after written.\n", Index));
      DumpBuffer (Buffer, 0x200);
    }
  }
  return EFI_SUCCESS;
}
#endif

