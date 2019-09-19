/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
#include <Library/FspSupportLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/ContainerLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/SerialPortInfoGuid.h>
#include <Guid/SmmInformationGuid.h>
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
  Find the actual VBT image from the container.

  In case of multiple VBT tables are packed into a single FFS, the PcdGraphicsVbtAddress could
  point to the container address instead. This function checks this condition and locates the
  actual VBT table address within the container.

  @param[in] ImageId    Image ID for VBT binary to locate in the container

  @retval               Actual VBT address found in the container. 0 if not found.

**/
UINT32
LocateVbtByImageId (
  IN  UINT32     ImageId
)
{
  VBT_MB_HDR     *VbtMbHdr;
  VBT_ENTRY_HDR  *VbtEntry;
  UINT32          VbtAddr;
  UINTN           Idx;

  VbtMbHdr = (VBT_MB_HDR* )PcdGet32 (PcdGraphicsVbtAddress);
  if ((VbtMbHdr == NULL) || (VbtMbHdr->Signature != MVBT_SIGNATURE)) {
    return 0;
  }

  VbtAddr  = 0;
  VbtEntry = (VBT_ENTRY_HDR *)&VbtMbHdr[1];
  for (Idx = 0; Idx < VbtMbHdr->EntryNum; Idx++) {
    if (VbtEntry->ImageId == ImageId) {
      VbtAddr = (UINT32)VbtEntry->Data;
      break;
    }
    VbtEntry = (VBT_ENTRY_HDR *)((UINT8 *)VbtEntry + VbtEntry->Length);
  }

  DEBUG ((DEBUG_INFO, "%a VBT ImageId 0x%08X\n",
                      (VbtAddr == 0) ? "Cannot find" : "Select", ImageId));

  return VbtAddr;
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
  GpioTable  = (UINT8 *)AllocateTemporaryMemory (0);  //allocate new buffer
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
  PLATFORM_CFG_DATA   *PlatformCfgData;
  GEN_CFG_DATA        *GenericCfgData;
  LOADER_GLOBAL_DATA  *LdrGlobal;
  UINT32               TsegBase;
  UINT64               TsegSize;
  UINT32               VbtAddress;
  UINT8                BootDev;
  VOID                *Buffer;
  UINT32               Length;

  switch (InitPhase) {
  case PreSiliconInit:
    GpioInit ();
    SpiConstructor ();
    EnableLegacyRegions ();
    VariableConstructor (PcdGet32 (PcdVariableRegionBase), PcdGet32 (PcdVariableRegionSize));
    if (!FeaturePcdGet (PcdStage1BXip)) {
      Status = TestVariableService ();
      ASSERT_EFI_ERROR (Status);
    }
    // Get TSEG info from FSP HOB
    // It will be consumed in MpInit if SMM rebase is enabled
    LdrGlobal  = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ();
    TsegBase = (UINT32)GetFspReservedMemoryFromGuid (
                       LdrGlobal->FspHobList,
                       &TsegSize,
                       &gReservedMemoryResourceHobTsegGuid
                       );
    if (TsegBase != 0) {
      Status = PcdSet32S (PcdSmramTsegBase, TsegBase);
      Status = PcdSet32S (PcdSmramTsegSize, (UINT32)TsegSize);
    }
    // Locate VBT binary from VBT container
    PlatformCfgData = (PLATFORM_CFG_DATA *)FindConfigDataByTag (CDATA_PLATFORM_TAG);
    if (PlatformCfgData != NULL) {
      VbtAddress = LocateVbtByImageId (PlatformCfgData->VbtImageId);
      if (VbtAddress != 0) {
        Status = PcdSet32S (PcdGraphicsVbtAddress,  VbtAddress);
      }
    }
    // Load IP firmware from container
    Buffer = NULL;
    Length = 0;
    Status = LoadComponent (SIGNATURE_32('I', 'P', 'F', 'W'), SIGNATURE_32('T', 'S', 'T', '3'), &Buffer,  &Length);
    DEBUG ((EFI_D_INFO, "Load IP firmware @ %p:0x%X - %r\n", Buffer, Length, Status));
    break;

  case PostPciEnumeration:
    GenericCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
    if (GenericCfgData != NULL) {
      if (GenericCfgData->PayloadId == AUTO_PAYLOAD_ID_SIGNATURE) {
        // Use QEMU 3rd bootorder to select the payload
        // Add QEMU command line parameter:
        //   default           : enable OsLoader
        //   '-boot order=??a' : enable EFI payload
        //   '-boot order=??c' : enable LINUX payload
        // Here ?? is the normal boot devices.
        IoWrite8 (0x70, 0x38);
        BootDev = IoRead8 (0x71) >> 4;
        if (BootDev == 1) {
          // Boot UEFI payload
          SetPayloadId (UEFI_PAYLOAD_ID_SIGNATURE);
        } else if (BootDev == 2) {
          // Boot LINUX payload
          SetPayloadId (LINX_PAYLOAD_ID_SIGNATURE);
        } else {
          // Boot OsLoader
          SetPayloadId (0);
        }
      } else {
        SetPayloadId (GenericCfgData->PayloadId);
      }
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
  UINT8         BootOrder;

  FillBootOptionListFromCfgData (OsBootOptionList);

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

 @param[out] GfxInfo    pointer to global HOB data structure.
 */
VOID
UpdateFrameBufferInfo (
  OUT  EFI_PEI_GRAPHICS_INFO_HOB   *GfxInfo
)
{
  GRAPHICS_DATA   *GfxPtr;

  GfxPtr = (GRAPHICS_DATA *)(UINTN)PcdGet32(PcdGraphicsVbtAddress);
  GfxInfo->FrameBufferBase = (UINT64)PciRead32 (PCI_LIB_ADDRESS(0, 1, 0, 0x10)) & 0xFFFFFFF0;
  if (GfxPtr->Signature == GRAPHICS_DATA_SIG) {
    GfxInfo->GraphicsMode.HorizontalResolution = GfxPtr->ResX;
    GfxInfo->GraphicsMode.VerticalResolution   = GfxPtr->ResY;
  }
}

/**
 Update the frame buffer device info.

 @param[out] GfxDeviceInfo   pointer to EFI_PEI_GRAPHICS_DEVICE_INFO_HOB structure.
 **/
VOID
UpdateFrameBufferDeviceInfo (
  OUT  EFI_PEI_GRAPHICS_DEVICE_INFO_HOB   *GfxDeviceInfo
  )
{
  GfxDeviceInfo->BarIndex = 0;
  GfxDeviceInfo->VendorId = PciRead16 (PCI_LIB_ADDRESS (0, 1, 0, 0));
  GfxDeviceInfo->DeviceId = PciRead16 (PCI_LIB_ADDRESS (0, 1, 0, 2));
}

/**
 Update loader SMM info.

 @param[out] SmmInfoHob     pointer to SMM information HOB

**/
VOID
UpdateSmmInfo (
  OUT  LDR_SMM_INFO           *SmmInfoHob
)
{
  UINT32  TsegSize;

  TsegSize = PcdGet32 (PcdSmramTsegSize);
  if (TsegSize > 0) {
    SmmInfoHob->SmmBase = PcdGet32 (PcdSmramTsegBase);
    SmmInfoHob->SmmSize = TsegSize;
    SmmInfoHob->Flags   = 0;
    DEBUG ((EFI_D_INFO, "SmmRamBase = 0x%x, SmmRamSize = 0x%x\n", SmmInfoHob->SmmBase, SmmInfoHob->SmmSize));
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
  if (Guid == &gEfiGraphicsInfoHobGuid) {
    UpdateFrameBufferInfo (HobInfo);
  } else if (Guid == &gEfiGraphicsDeviceInfoHobGuid) {
    UpdateFrameBufferDeviceInfo (HobInfo);
  } else if (Guid == &gLoaderSerialPortInfoGuid) {
    UpdateSerialPortInfo (HobInfo);
  } else if (Guid == &gOsBootOptionGuid) {
    UpdateOsBootMediumInfo (HobInfo);
  } else if (Guid == &gSmmInformationGuid) {
    UpdateSmmInfo (HobInfo);
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
