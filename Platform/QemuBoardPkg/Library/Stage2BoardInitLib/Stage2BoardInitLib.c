/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/GpioLib.h>
#include <Library/SiGpioLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/SerialPortLib.h>
#include <Library/VariableLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Guid/FrameBufferInfoGuid.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/SerialPortInfoGuid.h>
#include <FspsUpd.h>
#include <BlCommon.h>
#include <GlobalNvsArea.h>
#include <PlatformBase.h>
#include <ConfigDataDefs.h>
#include "GpioTbl.h"

#define GRAPHICS_DATA_SIG    SIGNATURE_32 ('Q', 'G', 'F', 'X')

typedef struct {
  UINT32     Signature;
  UINT32     ResX;
  UINT32     ResY;
} GRAPHICS_DATA;

UINT8
GetSerialPortStrideSize (
  VOID
);

UINT32
GetSerialPortBase (
  VOID
  );

VOID
EnableLegacyRegions (
  VOID
);

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

  DEBUG ((DEBUG_INFO, "Test variable services\n"));

  DataSize   = sizeof(Data);
  Status   = GetVariable ("VARTST0", NULL, &DataSize, &Data);
  if (!EFI_ERROR(Status) && (Data == 0x55667788) && (sizeof(Data) == DataSize)) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR(Status) || (Data != 0x11223344) || (sizeof(Data) != DataSize)) {
    return EFI_ABORTED;
  }

  Data       = 0x55667788;
  Status     = SetVariable ("VARTST0", 0, sizeof(Data), &Data);

  Data       = 0;
  DataSize   = sizeof(Data);
  if (!EFI_ERROR(Status)) {
    Status   = GetVariable ("VARTST0", NULL, &DataSize, &Data);
  }

  if (!EFI_ERROR(Status) && (Data == 0x55667788) && (sizeof(Data) == DataSize)) {
    return EFI_SUCCESS;
  } else {
    return EFI_ABORTED;
  }
}

/**
  Initialization of the GPIO table specific to each SOC. First find the relevant GPIO Config Data based on the Platform ID.
  Once the GPIO table data is fetched from configuration region, program the GPIO PADs and interrupt registers.

  @param NONE

**/
VOID
GpioInit (
  VOID
  )
  {
  GPIO_CFG_HDR       *GpioCfgCurrHdr;
  GPIO_CFG_HDR       *GpioCfgBaseHdr;
  GPIO_CFG_HDR       *GpioCfgHdr;
  UINT32              GpioEntries;
  UINT32              Index;
  UINT32              Offset;
  UINT8              *GpioCfgDataBuffer;
  UINT8              *GpioTable;

  DEBUG ((DEBUG_INFO, "Board GPIO Init\n"));

  //Find the GPIO CFG HDR
  GpioCfgCurrHdr = (GPIO_CFG_HDR *)FindConfigDataByTag (CDATA_GPIO_TAG);
  if (GpioCfgCurrHdr == NULL) {
    DEBUG ((DEBUG_ERROR, "No GPIO CFGDATA found !\n"));
    return;
  }

  GpioEntries    = 0;
  GpioCfgBaseHdr = NULL;

  //Find the GPIO CFG Data based on Platform ID. GpioTableData is the start of the GPIO entries
  if (GpioCfgCurrHdr->GpioBaseTableId < 16) {
    DEBUG ((DEBUG_INFO, "Get base platform GPIO table from board ID %d\n", GpioCfgCurrHdr->GpioBaseTableId));
    GpioCfgBaseHdr = (GPIO_CFG_HDR *)FindConfigDataByPidTag (GpioCfgCurrHdr->GpioBaseTableId, CDATA_GPIO_TAG);
    if (GpioCfgBaseHdr == NULL) {
      DEBUG ((DEBUG_ERROR, "Cannot find base GPIO table for platform ID %d\n", GpioCfgCurrHdr->GpioBaseTableId));
      return;
    }
    if (GpioCfgCurrHdr->GpioItemSize != GpioCfgBaseHdr->GpioItemSize) {
      DEBUG ((DEBUG_ERROR, "Inconsistent GPIO item size\n"));
      return;
    }
    GpioCfgHdr = GpioCfgBaseHdr;
  } else {
    GpioCfgHdr = GpioCfgCurrHdr;
  }

  Offset     = 0;
  GpioTable  = (UINT8 *)AllocateTemporaryPages (0);  //allocate new buffer
  GpioCfgDataBuffer = GpioTable;

  for (Index = 0; Index  < GpioCfgHdr->GpioItemCount; Index++) {
    if (GpioCfgCurrHdr->GpioBaseTableBitMask[Index >> 3] & (1 << (Index & 7))) {
      CopyMem (GpioTable, GpioCfgHdr->GpioTableData + Offset, GpioCfgHdr->GpioItemSize);
      GpioTable += GpioCfgHdr->GpioItemSize;
      GpioEntries++;
    }
    Offset += GpioCfgHdr->GpioItemSize;
  }

  if (GpioCfgBaseHdr != NULL) {
    CopyMem (GpioTable, GpioCfgCurrHdr->GpioTableData, GpioCfgCurrHdr->GpioItemCount * GpioCfgCurrHdr->GpioItemSize);
    GpioEntries += GpioCfgCurrHdr->GpioItemCount;
  }

  DEBUG ((DEBUG_INFO, "Programming %d GPIO entries\n", GpioEntries));

  GpioPadConfigTable (GpioEntries, GpioCfgDataBuffer);
}

/**
  Board specific hook point.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
BoardInit (
  IN  BOARD_INIT_PHASE    InitPhase
)
{
  EFI_STATUS           Status;
  GEN_CFG_DATA        *GenericCfgData;

  switch (InitPhase) {
  case PreSiliconInit:
    GpioInit ();
    SpiConstructor ();
    EnableLegacyRegions ();
    VariableConstructor (PcdGet32 (PcdVariableRegionBase), PcdGet32 (PcdVariableRegionSize));
    Status = TestVariableService ();
    ASSERT_EFI_ERROR (Status);
    break;
  case PostPciEnumeration:
    GenericCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
    if (GenericCfgData != NULL) {
      SetPayloadId (GenericCfgData->PayloadId);
    }
    break;
  default:
    break;
  }
}



/**
  This function overrides the default configurations in the FSP-S UPD data region.

  @param[in,out] FspUpdRgnPtr   A pointer to the UPD data region data strcture.

**/
VOID
EFIAPI
UpdateFspConfig (
  IN OUT VOID        *FspUpdRgnPtr
  )
{
  FSPS_UPD           *FspsUpd;
  FSP_S_CONFIG       *FspsConfig;
  SILICON_CFG_DATA   *SilCfgData;

  FspsUpd    = (FSPS_UPD *)FspUpdRgnPtr;
  FspsConfig = &FspsUpd->FspsConfig;

  if (PcdGetBool (PcdFramebufferInitEnabled)) {
    FspsConfig->GraphicsConfigPtr = PcdGet32 (PcdGraphicsVbtAddress);
  } else {
    FspsConfig->GraphicsConfigPtr = 0;
  }

  SilCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if (SilCfgData == NULL) {
    return;
  }
}

/**
  Save MRC data onto the Flash region from NVS buffer.

  @param Buffer pointer to an NVS data
  @param Length Length of the NVS data(MRC)

  @retval EFI_SUCCESS
  @retval RETURN_ALREADY_STARTED
  @retval EFI_DEVICE_ERROR
  @retval EFI_INVALID_PARAMETER
 */
EFI_STATUS
EFIAPI
SaveNvsData (
  IN  VOID    *Buffer,
  IN  UINT32   Length
)
{
  return EFI_UNSUPPORTED;
}

/**
 Update serial port information to global HOB data structure.

 @param SerialPortInfo  Pointer to global HOB data structure.
 **/
VOID
EFIAPI
UpdateSerialPortInfo (
  IN  SERIAL_PORT_INFO  *SerialPortInfo
)
{
  SerialPortInfo->Type     = 1;
  SerialPortInfo->BaseAddr = GetSerialPortBase();
  SerialPortInfo->RegWidth = GetSerialPortStrideSize();
}


/**
 Update the OS boot option

 @param OsBootOptionList pointer to boot option list.
 **/
VOID
EFIAPI
UpdateOsBootMediumInfo (
  OUT  OS_BOOT_OPTION_LIST   *OsBootOptionList
)
{
  OS_BOOT_OPTION             *BootOption;
  UINT8                      BootOrder;

  //The OS image information could come from config data later.
  BootOption = &OsBootOptionList->OsBootOption[0];

  // Boot file image from SD
  BootOption->DevType     = OsBootDeviceSd;
  BootOption->DevInstance = 0;
  BootOption->HwPart      = 0;
  BootOption->BootFlags   = 0;
  BootOption->FsType      = EnumFileSystemTypeAuto;
  BootOption->SwPart      = 0;
  CopyMem (BootOption->Image[0].FileName, "iasimage.bin", sizeof ("iasimage.bin"));
  OsBootOptionList->OsBootOptionCount++;
  BootOption++;

  // Boot file image from SATA
  BootOption->DevType     = OsBootDeviceSata;
  BootOption->DevInstance = 0;
  BootOption->HwPart      = 5;
  BootOption->BootFlags   = 0;
  BootOption->FsType      = EnumFileSystemTypeAuto;
  BootOption->SwPart      = 0;
  CopyMem (BootOption->Image[0].FileName, "iasimage.bin", sizeof ("iasimage.bin"));
  OsBootOptionList->OsBootOptionCount++;
  BootOption++;

  // Boot file image from NVMe
  BootOption->DevType     = OsBootDeviceNvme;
  BootOption->DevInstance = 0;
  BootOption->HwPart      = 0;
  BootOption->BootFlags   = 0;
  BootOption->FsType      = EnumFileSystemTypeAuto;
  BootOption->SwPart      = 0;
  CopyMem (BootOption->Image[0].FileName, "iasimage.bin", sizeof ("iasimage.bin"));
  OsBootOptionList->OsBootOptionCount++;
  BootOption++;

  // Boot file image from USB
  BootOption->DevType     = OsBootDeviceUsb;
  BootOption->DevInstance = 0;
  BootOption->HwPart      = 0;
  BootOption->BootFlags   = 0;
  BootOption->FsType      = EnumFileSystemTypeAuto;
  BootOption->SwPart      = 0;
  CopyMem (BootOption->Image[0].FileName, "iasimage.bin", sizeof ("iasimage.bin"));
  OsBootOptionList->OsBootOptionCount++;
  BootOption++;

  ASSERT (OsBootOptionList->OsBootOptionCount <= PcdGet32 (PcdOsBootOptionNumber));

  //
  // Read boot order, it is passed in by QEMU command
  //   QEMU boot order (a=1 c=2 d=3 n=4 at CMOS 0x3D)
  //   By default, this CMOS value at offset 0x3D is 0x03
  //   Use '-boot order=c' or default to boot from eMMC
  //   Use '-boot order=d' to boot from SATA
  //   Use '-boot order=n' to boot from NVMe
  //
  IoWrite8 (0x70, 0x3D);
  BootOrder = IoRead8  (0x71);

  if ((BootOrder & 0x0F) == 3) {
    // SATA boot first
    OsBootOptionList->CurrentBoot = 1;
  } else if ((BootOrder & 0x0F) == 4) {
    // NVMe boot first
    OsBootOptionList->CurrentBoot = 2;
  } else {
    // SD boot first
    OsBootOptionList->CurrentBoot = 0;
  }
}


/**
 Update the frame buffer info by reading the PCI address

 @param FrameBufferInfo pointer to global HOB data structure.
 */
VOID
EFIAPI
UpdateFrameBufferInfo (
  IN  FRAME_BUFFER_INFO  *FrameBufferInfo
)
{
  GRAPHICS_DATA   *GfxPtr;

  GfxPtr = (GRAPHICS_DATA *)(UINTN)PcdGet32(PcdGraphicsVbtAddress);
  FrameBufferInfo->LinearFrameBuffer = (UINT64)PciRead32 (PCI_LIB_ADDRESS(0, 1, 0, 0x10)) & 0xFFFFFFF0;
  if (GfxPtr->Signature == GRAPHICS_DATA_SIG) {
    FrameBufferInfo->HorizontalResolution = GfxPtr->ResX;
    FrameBufferInfo->VerticalResolution   = GfxPtr->ResY;
  }
}

/**
 Update Hob Info with platform specific data

 @param  Guid          The GUID to tag the customized HOB.
 @param  HobInfo       The start address of GUID HOB data.
 **/
VOID
EFIAPI
PlatformUpdateHobInfo (
  IN CONST EFI_GUID              *Guid,
  OUT      VOID                  *HobInfo
)
{
  ASSERT (Guid != NULL);
  ASSERT (HobInfo != NULL);

  // Just compare Guid memory addresses which are in Stage2 data section instead of CompareGuid ()
  if (Guid == &gLoaderFrameBufferInfoGuid) {
    UpdateFrameBufferInfo (HobInfo);
  } else if (Guid == &gLoaderSerialPortInfoGuid) {
    UpdateSerialPortInfo (HobInfo);
  } else if (Guid == &gOsBootOptionGuid) {
    UpdateOsBootMediumInfo (HobInfo);
  }
}

/**
  Update PCH NVS and SA NVS area address and size in ACPI table

  @param[in] Current       Pointer to ACPI description header

  @retval    EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
PlatformUpdateAcpiTable (
  IN UINT8                   *Current
)
{
  return EFI_SUCCESS;
}

/**
  Dynamic update of Global NVS data

  @param[in] GnvsIn       Pointer to global nvs region

**/
VOID
EFIAPI
PlatformUpdateAcpiGnvs (
  IN VOID  *GnvsIn
)
{
  EFI_GLOBAL_NVS_AREA  *Gnvs;

  Gnvs = (EFI_GLOBAL_NVS_AREA *)GnvsIn;
  SetMem (Gnvs, sizeof(EFI_GLOBAL_NVS_AREA), 0);

  Gnvs->PciWindow32.Base    = PcdGet32(PcdPciResourceMem32Base);
  Gnvs->PciWindow32.End     = PcdGet64(PcdPciExpressBaseAddress) - 1;
  Gnvs->PciWindow32.Length  = Gnvs->PciWindow32.End - Gnvs->PciWindow32.Base + 1;

  Gnvs->PciWindow64.Base    = 0;
  Gnvs->PciWindow64.End     = 0;
  Gnvs->PciWindow64.Length  = 0;

  SocUpdateAcpiGnvs ((VOID *)Gnvs);
}
