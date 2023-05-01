/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/GpioLib.h>
#include <Library/CryptoLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/VariableLib.h>
#include <Library/FspSupportLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/PciCf8Lib.h>
#include <Library/SocInitLib.h>
#include <FspmUpd.h>
#include <BlCommon.h>
#include <PlatformBase.h>
#include <ConfigDataDefs.h>
#include "GpioTbl.h"

CONST PLT_DEVICE  mPlatformDevices[]= {
  {{0x00001F02}, OsBootDeviceSata  , 0 },
  {{0x00000300}, OsBootDeviceSd    , 0 },
  {{0x00000300}, OsBootDeviceNvme  , 0 },
  {{0x00000400}, OsBootDeviceUsb   , 0 },
  {{0x01000000}, OsBootDeviceMemory, 0 },
  {{0x00000000}, PlatformDeviceGraphics, 0},
};

/**
  This function overrides the default configurations in the FSP-M UPD data region.

  @note At this point, memory is NOT ready, PeiServices are available to use.

  @param[in,out] FspUpdRgnPtr   A pointer to the UPD data region data strcture.

**/
VOID
EFIAPI
UpdateFspConfig (
  IN OUT VOID        *FspUpdRgnPtr
  )
{
  MEMORY_CFG_DATA   *MemCfgData;
  SILICON_CFG_DATA  *SilCfgData;

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "Found Config TAG: 0x%03X @ 0x%08X\n", CDATA_MEMORY_TAG, MemCfgData));
    DEBUG ((DEBUG_INFO, "  MemCfg.Test1=%08X, MemCfg.Test2=%08X\n", \
                        MemCfgData->MemTest1, MemCfgData->MemTest2));
  }

  SilCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if (SilCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "Found Config TAG: 0x%03X @ 0x%08X\n", CDATA_SILICON_TAG, SilCfgData));
    DEBUG ((DEBUG_INFO, "  SilCfg.Test1=%08X, SilCfg.Test2=%08X\n", \
                        SilCfgData->SiliconTest1, SilCfgData->SiliconTest2));
  }

}

/**
  Test variable services.

  @retval   EFI_SUCCESS    Test completed successfully.
            EFI_ABORTED    Test failed.

**/
EFI_STATUS
TestVariableService (
  VOID
  )
{
  UINT32      Data;
  UINTN       DataSize;
  EFI_STATUS  Status;

  Data       = 0;
  DataSize   = sizeof(Data);
  Status   = GetVariable (L"VARTST0", NULL, NULL, &DataSize, &Data);
  if (!EFI_ERROR(Status) && (Data == 0x55667788) && (sizeof(Data) == DataSize)) {
    return EFI_SUCCESS;
  }

  Data       = 0x11223344;
  Status     = SetVariable (L"VARTST0", NULL, 0, sizeof(Data), &Data);

  Data       = 0;
  DataSize   = sizeof(Data);
  if (!EFI_ERROR(Status)) {
    Status   = GetVariable (L"VARTST0", NULL, NULL, &DataSize, &Data);
  }

  if (!EFI_ERROR(Status) && (Data == 0x11223344) && (sizeof(Data) == DataSize)) {
    return EFI_SUCCESS;
  } else {
    return EFI_ABORTED;
  }
}

/**
  Detect and update current boot mode.

**/
VOID
UpdateBootMode (
  VOID
  )
{
  UINT8  BootMode;

  //
  // Reuse CMOS 0x38 high 4 bits as boot mode
  //   [7:4]  Boot mode  (a=1 c=2 d=3 n=4)
  //          3: Normal boot
  //          4: Firmware update
  //   Use '-boot order=??n' : Firmware update
  //
  IoWrite8 (0x70, 0x38);
  BootMode = IoRead8  (0x71) >> 4;

  if (BootMode == 4) {
    // Firmware update
    SetBootMode (BOOT_ON_FLASH_UPDATE);
  } else {
    // Normal
    SetBootMode (BOOT_WITH_FULL_CONFIGURATION);
  }
}

/**
  Detect and set board ID for QEMU

**/
VOID BoardDetection (
  VOID
)
{
  UINT8  BoardId;
  UINT16 mHostBridgeDevId;
  IoWrite32 (0xCF8, PCI_TO_CF8_ADDRESS(PCI_CF8_LIB_ADDRESS (0, 0, 0, 0)));
  mHostBridgeDevId = (IoRead32 (0xCFC) >> 16);
  DEBUG ((DEBUG_INFO, "Host Bridge Device ID:0x%X\n", mHostBridgeDevId));
  if (mHostBridgeDevId == INTEL_X58_ICH10_DEVICE_ID) {
    BoardId = 2; //Simics QSP board
    DEBUG ((DEBUG_INFO, "Board ID:0x%X - Loading Simics QSP!\n", BoardId));
  } else {
    //
    // Reuse CMOS 0x38 BIT0 as board ID
    //   [0]  Board ID (0, 1)
    //          0: BoardID 1
    //          1: BoardID 31
    //   Use '-no-fd-bootchk' to select board 31
    //   By default, it is board 1
    //   BoardID 0 is already reserved for default
    //
    IoWrite8 (0x70, 0x38);
    BoardId = IoRead8  (0x71) & BIT0;

    if (BoardId > 0) {
      BoardId = 31;
    } else {
      BoardId = 1;
    }
    DEBUG ((DEBUG_INFO, "Board ID:0x%X - Loading QEMU!\n", BoardId));
  }

  SetPlatformId (BoardId);
}


/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
)
{
  EFI_STATUS             Status;
  PLT_DEVICE_TABLE      *PltDeviceTable;
  VOID                  *FspHob;

  switch (InitPhase) {
  case PreConfigInit:
    PltDeviceTable = (PLT_DEVICE_TABLE *)AllocatePool (sizeof (PLT_DEVICE_TABLE) + sizeof (mPlatformDevices));
    PltDeviceTable->DeviceNumber = sizeof (mPlatformDevices) /sizeof (PLT_DEVICE);
    CopyMem (PltDeviceTable->Device, mPlatformDevices, sizeof (mPlatformDevices));
    SetDeviceTable (PltDeviceTable);
    BoardDetection ();
    UpdateBootMode ();
    if (!FeaturePcdGet (PcdStage1BXip)) {
      if (GetPlatformId () != PLATFORM_ID_QSP_SIMICS) {
        SpiConstructor ();
        VariableConstructor (PcdGet32 (PcdVariableRegionBase), PcdGet32 (PcdVariableRegionSize));
        Status = TestVariableService ();
        ASSERT_EFI_ERROR (Status);
      }
    }
    break;
  case PostConfigInit:
    PlatformNameInit ();
    break;

  case PostMemoryInit:
    FspHob = GetFspHobListPtr ();
    if (FspHob != NULL) {
      DumpFspResourceHob (FspHob);
    }
    UpdateMemoryInfo ();
    break;

  case PreMemoryInit:
  case PreTempRamExit:
  case PostTempRamExit:
    break;
  default:
    break;
  }
}

/**
  Search for the saved MrcParam to initialize Memory for fastboot

  @retval Found MrcParam or NULL
**/
VOID *
EFIAPI
FindNvsData (
  VOID
)
{
  UINT32    MrcData;
  EFI_STATUS    Status;

  Status = GetComponentInfo (FLASH_MAP_SIG_MRCDATA, &MrcData, NULL);
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  if (*(UINT32 *)(UINTN)MrcData == 0xFFFFFFFF) {
    return NULL;
  } else {
    return (VOID *)(UINTN)MrcData;
  }
}

/**
  Load the configuration data blob from media into destination buffer.

  @param[in]    Dst        Destination address to load configuration data blob.
  @param[in]    Src        Source address to load configuration data blob.
  @param[in]    Len        The destination address buffer size.

  @retval  EFI_SUCCESS             Configuration data blob was loaded successfully.
  @retval  EFI_NOT_FOUND           Configuration data blob cannot be found.
  @retval  EFI_OUT_OF_RESOURCES    Destination buffer is too small to hold the
                                   configuration data blob.
  @retval  Others                  Failed to load configuration data blob.

**/
EFI_STATUS
EFIAPI
LoadExternalConfigData (
  IN UINT32  Dst,
  IN UINT32  Src,
  IN UINT32  Len
  )
{

  return SpiLoadExternalConfigData (Dst, Src, Len);
}
