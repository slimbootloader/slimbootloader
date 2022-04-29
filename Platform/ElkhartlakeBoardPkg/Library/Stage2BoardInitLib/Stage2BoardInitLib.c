/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/VariableLib.h>
#include <Library/ContainerLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/SerialPortInfoGuid.h>
#include <FspsUpd.h>
#include <GlobalNvsAreaDef.h>
#include <ConfigDataDefs.h>
#include <Library/BootloaderCoreLib.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <Library/PchInfoLib.h>
#include <BlCommon.h>
#include <Guid/BootLoaderServiceGuid.h>
#include <Library/SpiBlockIoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Service/SpiFlashService.h>
#include <Library/PrintLib.h>
#include <Guid/OsConfigDataHobGuid.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/DmaRemappingTable.h>
#include <Library/VTdLib.h>
#include <Library/MpInitLib.h>
#include <Library/SmbiosInitLib.h>
#include <Library/BoardSupportLib.h>
#include <IndustryStandard/SmBios.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <MemInfoHob.h>
#include <Library/DebugDataLib.h>
#include <Library/IgdOpRegionLib.h>
#include <Register/PchRegsSerialIoEhl.h>
#include <ConfigDataCommonStruct.h>
#include "GpioTableEhl.h"
#include <Library/PcieRpConfig.h>
#include <Guid/GraphicsInfoHob.h>
#include <Library/HobLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <RegAccess.h>
#include <CpuDataStruct.h>
#include <PowerMgmtNvsStruct.h>
#include <CpuPowerMgmt.h>
#include <Register/CpuRegs.h>
#include "HdaVerbTable.h"
#include <Library/TpmLib.h>
#include <PlatformData.h>
#include <Guid/SmmInformationGuid.h>
#include <Library/PchSciLib.h>
#include <Library/BdatLib.h>
#include <Library/S3SaveRestoreLib.h>
#include <PseConfig.h>
#include <PlatformBoardId.h>
#include <PchReservedResources.h>
#include "BoardSaConfigPostMem.h"
#include <Library/HeciInitLib.h>
#include <TccConfigSubRegions.h>
#include <Library/TccLib.h>
#include <Library/MeExtMeasurementLib.h>
#include <GpioConfig.h>
#include <Register/RegsSpi.h>
#include <Library/GpioLib.h>
#include <Library/PlatformHookLib.h>

BOOLEAN mTccDsoTuning      = FALSE;
UINT8   mTccRtd3Support    = 0;

//
// GPIO_PAD Fileds
//
typedef struct {
  UINT32    PadNum:16;
  UINT32    GrpIdx:8;
  UINT32    ChipsetId:4;
  UINT32    Rsvd:4;
} GPIO_PAD_FIELD;

//
// GPIO_CFG_DATA DW1 fields
//
typedef struct {
  UINT32    Rsvd1:16;
  UINT32    PadNum:8;
  UINT32    GrpIdx:5;
  UINT32    Rsvd2:3;
} GPIO_CFG_DATA_DW1;

SMMBASE_INFO mSmmBaseInfo = {
  { BL_PLD_COMM_SIG, SMMBASE_INFO_COMM_ID, 0, 0 }
};

S3_SAVE_REG mS3SaveReg = {
  { BL_PLD_COMM_SIG, S3_SAVE_REG_COMM_ID, 1, 0 },
  { { REG_TYPE_IO, WIDE32, { 0, 0}, (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), 0x00000000 } }
};

typedef enum {
  PchPIRQA = 16,
  PchPIRQB, //17
  PchPIRQC, //18
  PchPIRQD, //19
  PchPIRQE, //20
  PchPIRQF, //21
  PchPIRQG, //22
  PchPIRQH  //23
} PCH_PIRQ_ROUTE;

SI_PCH_DEVICE_INTERRUPT_CONFIG mPchDevIntConfig[] = {
//  {31, 0, SiPchNoInt, 0}, // LPC/eSPI Interface, doesn't use interrupts
//  {31, 1, SiPchNoInt, 0}, // P2SB, doesn't use interrupts
//  {31, 2, SiPchNoInt, 0}, // PMC , doesn't use interrupts
  {31, 3, SiPchIntA, PchPIRQA}, // cAVS(Audio, Voice, Speach), INTA is default, programmed in PciCfgSpace 3Dh
  {31, 4, SiPchIntA, PchPIRQA}, // SMBus Controller, no default value, programmed in PciCfgSpace 3Dh
//  {31, 5, SiPchNoInt, 0}, // SPI , doesn't use interrupts
  {31, 7, SiPchIntA, PchPIRQA}, // NorthPeak TraceHub, INTA is default, RO register
  {30, 0, SiPchIntA, PchPIRQA}, // SerialIo: UART #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[7]
  {30, 1, SiPchIntB, PchPIRQB}, // SerialIo: UART #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[8]
  {30, 2, SiPchIntC, PchPIRQC}, // SerialIo: SPI #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[10]
  {30, 3, SiPchIntD, PchPIRQD}, // SerialIo: SPI #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[11]
  {30, 4, SiPchIntA, PchPIRQA}, // TSN Gbe Controller, INTx
  {29, 0, SiPchIntA, PchPIRQA}, // PSE LH2PSE IPC
  {29, 1, SiPchIntA, PchPIRQA}, // PSE TSN GBE-0
  {29, 2, SiPchIntA, PchPIRQA}, // PSE TSN GBE-1
  {29, 3, SiPchIntB, 34}, // PSE DMA-0
  {29, 4, SiPchIntC, 35}, // PSE DMA-1
  {29, 5, SiPchIntD, 36}, // PSE DMA-2
  {29, 6, SiPchIntA, PchPIRQA}, // PSE PWM
  {29, 7, SiPchIntA, PchPIRQA}, // PSE ADC
  {28, 0, SiPchIntA, PchPIRQA}, // PCI Express Port 1, INT is default, programmed in PciCfgSpace + FCh
  {28, 1, SiPchIntB, PchPIRQB}, // PCI Express Port 2, INT is default, programmed in PciCfgSpace + FCh
  {28, 2, SiPchIntC, PchPIRQC}, // PCI Express Port 3, INT is default, programmed in PciCfgSpace + FCh
  {28, 3, SiPchIntD, PchPIRQD}, // PCI Express Port 4, INT is default, programmed in PciCfgSpace + FCh
  {28, 4, SiPchIntA, PchPIRQA}, // PCI Express Port 5, INT is default, programmed in PciCfgSpace + FCh
  {28, 5, SiPchIntB, PchPIRQB}, // PCI Express Port 6, INT is default, programmed in PciCfgSpace + FCh
  {28, 6, SiPchIntC, PchPIRQC}, // PCI Express Port 7, INT is default, programmed in PciCfgSpace + FCh
//  {28, 7, SiPchIntD, PchPIRQD}, // PCI Express Port 8, INT is default, programmed in PciCfgSpace + FCh //TODO: From spec there is not PCIe Port 8, need to reconfirm
  {27, 0, SiPchIntA, PchPIRQA}, // PSE I2C-0
  {27, 1, SiPchIntB, PchPIRQB}, // PSE I2C-1
  {27, 2, SiPchIntC, PchPIRQC}, // PSE I2C-2
  {27, 3, SiPchIntD, PchPIRQD}, // PSE I2C-3
  {27, 4, SiPchIntA, PchPIRQA}, // PSE I2C-4
  {27, 5, SiPchIntB, PchPIRQB}, // PSE I2C-5
  {27, 6, SiPchIntC, PchPIRQC}, // PSE I2C-6
  {26, 0, SiPchIntA, PchPIRQA}, // SCS: eMMC
  {26, 1, SiPchIntB, PchPIRQB}, // SCS: SD
  {26, 2, SiPchIntC, PchPIRQC}, // SCS: SD1
  {26, 3, SiPchIntD, PchPIRQD}, // ISI
  {25, 0, SiPchIntA, 31}, // SerialIo I2C Controller #4, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[5]
  {25, 1, SiPchIntB, 32}, // SerialIo I2C Controller #5, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[6]
  {25, 2, SiPchIntC, 33}, // SerialIo UART Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[9]
  {24, 0, SiPchIntA, PchPIRQA}, // PSE I2C-7
  {24, 1, SiPchIntB, PchPIRQB}, // PSE CAN-0
  {24, 2, SiPchIntC, PchPIRQC}, // PSE CAN-1
  {24, 3, SiPchIntD, PchPIRQD}, // PSE QEP-1
  {24, 4, SiPchIntA, PchPIRQA}, // PSE QEP-2
  {24, 5, SiPchIntB, PchPIRQB}, // PSE QEP-3
  {24, 6, SiPchIntC, PchPIRQC}, // PSE QEP-4
  {23, 0, SiPchIntA, PchPIRQA}, // SATA Controller, INTA is default, programmed in PciCfgSpace + 3Dh
  {22, 0, SiPchIntA, PchPIRQA}, // CSME: HECI #1
  {22, 1, SiPchIntB, PchPIRQB}, // CSME: HECI #2
  {22, 4, SiPchIntC, PchPIRQC}, // CSME: HECI #3
  {22, 5, SiPchIntD, PchPIRQD}, // CSME: HECI #4
//  {22, 7, SiPchIntA, PchPIRQA}, // CSME: WLAN
  {21, 0, SiPchIntA, 27}, // SerialIo I2C Controller #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[1]
  {21, 1, SiPchIntB, 28}, // SerialIo I2C Controller #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[2]
  {21, 2, SiPchIntC, 29}, // SerialIo I2C Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[3]
  {21, 3, SiPchIntD, 30}, // SerialIo I2C Controller #3, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[4]
//  {21, 7, SiPchNoInt, 0}, // NorthPeak Phantom ACPI Function
  {20, 0, SiPchIntA, PchPIRQA}, // USB 3.0 xHCI Controller, no default value, programmed in PciCfgSpace 3Dh
  {20, 1, SiPchIntB, PchPIRQB}, // USB Device Controller (OTG)
//  {20, 2, SiPchNoInt, 0}, // Shared SRAM, no interrupts
  {20, 3, SiPchIntA, PchPIRQA}, // CNVi WiFi
//  {20, 4, SiPchNoInt, 0}, // vTIO Phantom ACPI Function
  {19, 0, SiPchIntA, PchPIRQA}, // PSE SPI-0
  {19, 1, SiPchIntB, PchPIRQB}, // PSE SPI-1
  {19, 2, SiPchIntC, PchPIRQC}, // PSE SPI-2
  {19, 3, SiPchIntD, PchPIRQD}, // PSE SPI-3
  {19, 4, SiPchIntA, PchPIRQA}, // PSE GPIO-0
  {19, 5, SiPchIntB, PchPIRQB}, // PSE GPIO-1
  {18, 0, SiPchIntA, 24}, // SerialIo: SPI #2 INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[12]
//  {18, 4, SiPchNoInt, PchPIRQC}, // CSME: fTPM DMA Phantom (ACPI) Function
  {18, 5, SiPchIntB, 25}, // SCS: UFS #0
  {18, 7, SiPchIntC, 26}, // SCS: UFS #1
  {17, 0, SiPchIntA, PchPIRQA}, // PSE UART-0
  {17, 1, SiPchIntB, PchPIRQB}, // PSE UART-1
  {17, 2, SiPchIntC, PchPIRQC}, // PSE UART-2
  {17, 3, SiPchIntD, PchPIRQD}, // PSE UART-3
  {17, 4, SiPchIntA, PchPIRQA}, // PSE UART-4
  {17, 5, SiPchIntB, PchPIRQB}, // PSE UART-5
  {17, 6, SiPchIntC, PchPIRQC}, // PSE IS2-0
  {17, 7, SiPchIntD, PchPIRQD}, // PSE IS2-1
  {16, 0, SiPchIntA, PchPIRQA}, // SerialIo: I2C #6
  {16, 1, SiPchIntB, PchPIRQB}, // SerialIo: I2C #7
  {16, 2, SiPchIntC, PchPIRQC}, // BT CNVi
//  {16, 5, SiPchIntB, PchPIRQB}, // IEH
  {16, 6, SiPchIntA, PchPIRQA}, // THC #0
  {16, 7, SiPchIntB, PchPIRQB}  // THC #1
};

STATIC UINT8 mPchSciSupported = 0xFF;

CONST EFI_ACPI_DESCRIPTION_HEADER  mAcpiTccRtctTableTemplate = {
  EFI_ACPI_RTCT_SIGNATURE,
  sizeof (EFI_ACPI_DESCRIPTION_HEADER)
  // Other fields will be updated in runtime
};

STATIC
CONST EFI_ACPI_COMMON_HEADER *mPlatformAcpiTables[] = {
  (EFI_ACPI_COMMON_HEADER *)&mAcpiTccRtctTableTemplate,
  NULL
};

VOID
EnableLegacyRegions (
  VOID
);

VOID
ClearSmbusStatus (
  VOID
);

/**
  Print the output of the GPIO Config table that was read from CfgData.

  @param[in] GpioPinNum     Number of GPIO entries in the table.
  @param[in] GpioConfData   GPIO Config Data that was read from the Configuration region
                            either from internal or external source.
**/
VOID
PrintGpioConfigTable (
  IN UINT32              GpioPinNum,
  IN VOID                *GpioConfData
)
{
  GPIO_INIT_CONFIG  *GpioInitConf;
  UINT32            *PadDataPtr;
  UINT32             Index;

  GpioInitConf = (GPIO_INIT_CONFIG *)GpioConfData;
  for (Index  = 0; Index < GpioPinNum; Index++) {
    PadDataPtr = (UINT32 *)&GpioInitConf->GpioConfig;
    DEBUG ((DEBUG_VERBOSE, "GPIO PAD: 0x%08X   DATA: 0x%08X 0x%08X\n", GpioInitConf->GpioPad, PadDataPtr[0], PadDataPtr[1]));
    GpioInitConf++;
  }
}

/**
  Retreive PadInfo embedded inside DW1 of GPIO CFG DATA.
  Prepare a PadInfo DWORD first, add into the GpioTable,
  followed by DW0 and DW1 directly from GPIO CFG DATA.
  This format of GpioTable is what the Gpio library expects.

  @param    GpioTable   Pointer to the GpioTable to be updated
  @param    GpioCfg     Pointer to the cfg data
  @param    Offset      Index of a particulr pin's DW0, DW1 in GpioCfg

  @retval   GpioTable   Pointer to fill the next gpio item
**/
UINT8 *
FillGpioTable (
  IN  UINT8         *GpioTable,
  IN  GPIO_CFG_HDR  *GpioCfg,
  IN  UINT32        Offset,
  IN  UINT8         ChipsetId
)
{
  GPIO_CFG_DATA_DW1 *Dw1;
  GPIO_PAD_FIELD    GpioPad;
  UINT32            *GpioItem;

  //
  // Get the DW1 and extract PadInfo
  //
  GpioItem = (UINT32 *) (GpioCfg->GpioTableData + Offset);
  Dw1 = (GPIO_CFG_DATA_DW1 *) (&GpioItem[1]);
  GpioPad.PadNum    = (UINT16) Dw1->PadNum;
  GpioPad.GrpIdx    = (UINT8)  Dw1->GrpIdx;
  GpioPad.ChipsetId = ChipsetId;

  //
  // Remove PadInfo data from DW1
  //
  Dw1->PadNum = 0;
  Dw1->GrpIdx = 0;

  //
  // Copy PadInfo(PinOffset), DW0, DW1
  //
  CopyMem (GpioTable, (VOID *)&GpioPad, sizeof(GPIO_PAD_FIELD));
  GpioTable += sizeof(GPIO_PAD_FIELD);
  CopyMem (GpioTable, GpioItem, GpioCfg->GpioItemSize);
  GpioTable += GpioCfg->GpioItemSize;

  return GpioTable;
}

/**
  Initialize the GPIO Config table that was read from CfgData into GPIO PAD registers.
  Create OS config data support HOB.

  @param VOID

  @retval EFI_SUCCESS           OS config data HOB built
  @retval EFI_NOT_FOUND         Loader Global data not found
  @retval EFI_OUT_OF_RESOURCES  Could not build HOB
**/
EFI_STATUS
BuildOsConfigDataHob (
  VOID
  )
{
  OS_CONFIG_DATA_HOB    *OsConfigData;
  GEN_CFG_DATA          *GenCfgData;
  UINT32                MemorySize;
  UINT32                PldRsvdMemSize;
  EFI_STATUS            Status;

  MemorySize   = 0;
  OsConfigData = BuildGuidHob (&gOsConfigDataGuid, sizeof (OS_CONFIG_DATA_HOB));
  if (OsConfigData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  OsConfigData->Revision = 1;

  GenCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenCfgData != NULL) {
    if (GenCfgData->OsCrashMemorySize != 0) {
      switch (GenCfgData->OsCrashMemorySize) {
        case 0:
          MemorySize = 0;
          break;
        case 1:
          MemorySize = 0x400000;
          break;
        case 2:
          MemorySize = 0x800000;
          break;
        case 3:
          MemorySize = 0x1000000;
          break;
        case 4:
          MemorySize = 0x2000000;
          break;
        case 5:
          MemorySize = 0x4000000;
          break;
        default:
          MemorySize = 0x400000;
          break;
      }
    }
    OsConfigData->EnableCrashMode   = GenCfgData->EnableCrashMode;
  }

  OsConfigData->OsCrashMemorySize = MemorySize;
  if (MemorySize > 0) {
    PldRsvdMemSize = PcdGet32 (PcdPayloadReservedMemSize);
    Status = PcdSet32S (PcdPayloadReservedMemSize, PldRsvdMemSize + MemorySize);
    ASSERT_EFI_ERROR (Status);
  }
  return EFI_SUCCESS;
}

/**
  Set SPI flash EISS and LE
**/
VOID
ProgramSecuritySetting (
  VOID
  )
{
  UINTN            SpiBaseAddress;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  // Set the BIOS Lock Enable and EISS bits
  MmioOr8 (SpiBaseAddress + R_SPI_BCR, (UINT8) (B_SPI_BCR_BLE | B_SPI_BCR_EISS));
}

/**
  Add a Smbios type string into a buffer

**/
STATIC
EFI_STATUS
AddSmbiosTypeString (
  SMBIOS_TYPE_STRINGS  *Dest,
  UINT8                 Type,
  UINT8                 Index,
  CHAR8                *String
  )
{
  UINTN   Length;

  Dest->Type    = Type;
  Dest->Idx     = Index;
  if (String != NULL) {
    Length = AsciiStrLen (String);

    Dest->String  = (CHAR8 *)AllocateZeroPool (Length + 1);
    if (Dest->String == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (Dest->String, String, Length);
  }

  return EFI_SUCCESS;
}

/**
  Initialize necessary information for Smbios

  @retval EFI_SUCCESS             Initialized necessary information successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory for Smbios info

**/
EFI_STATUS
InitializeSmbiosInfo (
  VOID
  )
{
  CHAR8                 TempStrBuf[SMBIOS_STRING_MAX_LENGTH];
  UINT16                Index;
  UINTN                 Length;
  SMBIOS_TYPE_STRINGS  *TempSmbiosStrTbl;
  BOOT_LOADER_VERSION  *VerInfoTbl;
  VOID                 *SmbiosStringsPtr;

  Index         = 0;
  TempSmbiosStrTbl  = (SMBIOS_TYPE_STRINGS *) AllocateTemporaryMemory (0);
  VerInfoTbl    = GetVerInfoPtr ();

  //
  // SMBIOS_TYPE_BIOS_INFORMATION
  //
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
    1, "Intel Corporation");
  if (VerInfoTbl != NULL) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf),
      "SB_EHL.%03d.%03d.%03d.%03d.%03d.%05d.%c-%016lX%a\0",
      VerInfoTbl->ImageVersion.SecureVerNum,
      VerInfoTbl->ImageVersion.CoreMajorVersion,
      VerInfoTbl->ImageVersion.CoreMinorVersion,
      VerInfoTbl->ImageVersion.ProjMajorVersion,
      VerInfoTbl->ImageVersion.ProjMinorVersion,
      VerInfoTbl->ImageVersion.BuildNumber,
      VerInfoTbl->ImageVersion.BldDebug ? 'D' : 'R',
      VerInfoTbl->SourceVersion,
      VerInfoTbl->ImageVersion.Dirty ? "-dirty" : "");
  } else {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Unknown");
  }
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
    2, TempStrBuf);
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
    3, __DATE__);

  //
  // SMBIOS_TYPE_SYSTEM_INFORMATION
  //
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    1, "Intel Corporation");

  AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "ElkhartLake Embedded Platform");

  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    2, TempStrBuf);
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    3, "0.1");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    4, "System Serial Number");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    5, "System SKU Number");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
    6, "ElkhartLake Embedded System");

  //
  // SMBIOS_TYPE_BASEBOARD_INFORMATION
  //
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
    1, "Intel Corporation");
  AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "ElkhartLake CRB");

  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
    2, TempStrBuf);
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
    3, "1");
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
    4, "Board Serial Number");

  //
  // SMBIOS_TYPE_PROCESSOR_INFORMATION : TBD
  //

  //
  // SMBIOS_TYPE_END_OF_TABLE
  //
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_END_OF_TABLE,
    0, NULL);

  Length = sizeof (SMBIOS_TYPE_STRINGS) * Index;
  SmbiosStringsPtr = AllocatePool (Length);
  if (SmbiosStringsPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (SmbiosStringsPtr, TempSmbiosStrTbl, Length);
  (VOID) PcdSet32S (PcdSmbiosStringsPtr, (UINT32)(UINTN)SmbiosStringsPtr);
  (VOID) PcdSet16S (PcdSmbiosStringsCnt, Index);

  return EFI_SUCCESS;
}

/**
  Clear SMI sources

**/
VOID
ClearSmi (
  VOID
)
{
  UINT32                SmiEn;
  UINT32                SmiSts;
  UINT16                Pm1Sts;

  SmiEn = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
  if (((SmiEn & B_ACPI_IO_SMI_EN_GBL_SMI) !=0) && ((SmiEn & B_ACPI_IO_SMI_EN_EOS) !=0)) {
    return;
  }

  //
  // Clear the status before setting smi enable
  //
  SmiSts = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4));
  Pm1Sts = IoRead16 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS));

  SmiSts |=
    (
      B_ACPI_IO_SMI_STS_SMBUS |
      B_ACPI_IO_SMI_STS_PERIODIC |
      B_ACPI_IO_SMI_STS_TCO |
      B_ACPI_IO_SMI_STS_MCSMI |
      B_ACPI_IO_SMI_STS_SWSMI_TMR |
      B_ACPI_IO_SMI_STS_APM |
      B_ACPI_IO_SMI_STS_ON_SLP_EN |
      B_ACPI_IO_SMI_STS_BIOS
    );

  Pm1Sts |=
    (
      B_ACPI_IO_PM1_STS_WAK |
      B_ACPI_IO_PM1_STS_PRBTNOR |
      B_ACPI_IO_PM1_STS_RTC |
      B_ACPI_IO_PM1_STS_PWRBTN |
      B_ACPI_IO_PM1_STS_GBL |
      B_ACPI_IO_PM1_STS_TMROF
      );

  IoWrite32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4), SmiSts);
  IoWrite16 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS), (UINT16) Pm1Sts);
}

/**
  Update current boot Payload ID.

**/
VOID
UpdatePayloadId (
  VOID
  )
{
  EFI_STATUS        Status;
  UINT32            PayloadSelGpioData;
  UINT32            PayloadSelGpioPad;
  GEN_CFG_DATA      *GenCfgData;
  PLATFORM_CFG_DATA *PlatCfgData;

  GenCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenCfgData == NULL) {
    ASSERT (FALSE);
    return;
  }
  SetPayloadId (GenCfgData->PayloadId);

  if (GetPayloadId () != AUTO_PAYLOAD_ID_SIGNATURE) {
    return;
  }

  // Switch payloads based on configured GPIO pin
  PlatCfgData = (PLATFORM_CFG_DATA *)FindConfigDataByTag (CDATA_PLATFORM_TAG);
  if ((PlatCfgData != NULL) && (PlatCfgData->PayloadSelGpio.Enable != 0)) {
    // Calculate the GPIO pad
    PayloadSelGpioPad = GPIO_VER3_GPP_B0 + (PlatCfgData->PayloadSelGpio.PinGroup << 16) + PlatCfgData->PayloadSelGpio.PinNumber;
    // Switch payloads based on configured GPIO pin
    Status = GpioGetInputValue (PayloadSelGpioPad, &PayloadSelGpioData);
    if (!EFI_ERROR (Status)) {
      if (PayloadSelGpioData == 1) {
        SetPayloadId (0);
        DEBUG ((DEBUG_INFO, "Update PayloadId to OS Loader\n"));
      } else {
        SetPayloadId (UEFI_PAYLOAD_ID_SIGNATURE);
        DEBUG ((DEBUG_INFO, "Update PayloadId to UEFI\n"));
      }
    } else {
      DEBUG ((DEBUG_ERROR, "Invalid GPIO pin for Payload Select\n"));
    }
  }
}

//Initialize Platform Igd OpRegion
VOID
EFIAPI
IgdOpRegionPlatformInit (
  VOID
  )
{
  GLOBAL_NVS_AREA           *Gnvs;
  EFI_STATUS                Status;

  Gnvs = (GLOBAL_NVS_AREA *)(UINTN)PcdGet32 (PcdAcpiGnvsAddress);

  Status = IgdOpRegionInit (NULL);
  Gnvs->SaNvs.IgdOpRegionAddress = (UINT32)(UINTN)PcdGet32 (PcdIgdOpRegionAddress);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "VBT not found %r\n", Status));
  }
}

/**
  Do board specific init based on phase indication

  @param[in]  InitPhase     The phase indication to do board init.

**/
VOID
BoardInit (
  IN  BOARD_INIT_PHASE    InitPhase
)
{
  EFI_STATUS                  Status;
  UINT32                      Length;
  EFI_PEI_GRAPHICS_INFO_HOB   *FspGfxHob;
  VOID                        *FspHobList;
  UINT32                      TsegBase;
  UINT32                      TsegSize;

  if (mPchSciSupported == 0xFF){
    mPchSciSupported = PchIsSciSupported();
  }

  switch (InitPhase) {
  case PreSiliconInit:
    EnableLegacyRegions ();
    ConfigureGpio (CDATA_GPIO_TAG, 0, NULL);
    if (mPchSciSupported) {
      DEBUG ((DEBUG_INFO, "ConfigureGpio for Fusa\n"));
      ConfigureGpio (CDATA_NO_TAG, ARRAY_SIZE(mGpioTablePreMemEhlFusa), (UINT8*)mGpioTablePreMemEhlFusa);
    }
    SpiConstructor ();
    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      UpdatePayloadId ();
    }
    GpioLockPads();
    // Prepare platform ACPI tables
    Status = PcdSet32S (PcdAcpiTableTemplatePtr, (UINT32)(UINTN)mPlatformAcpiTables);
    break;
  case PostSiliconInit:
    // Set TSEG base/size PCD
    TsegBase = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + TSEG) & ~0xF;
    TsegSize = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + BGSM) & ~0xF;
    TsegSize -= TsegBase;
    (VOID) PcdSet32S (PcdSmramTsegBase, TsegBase);
    (VOID) PcdSet32S (PcdSmramTsegSize, (UINT32)TsegSize);

    if (PcdGetBool (PcdFramebufferInitEnabled)) {
      FspGfxHob = NULL;
      FspHobList = GetFspHobListPtr ();
      if (FspHobList != NULL) {
        FspGfxHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (FspHobList, &Length, &gEfiGraphicsInfoHobGuid);
      }
      if (FspGfxHob != NULL) {
        PciWrite8 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_COMMAND_OFFSET), \
                   EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
        PciWrite32 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0x18), \
                   (UINT32)FspGfxHob->FrameBufferBase);
      }
    }
    BuildOsConfigDataHob ();
    //
    // Initialize Smbios Info for SmbiosInit
    //
    if (FeaturePcdGet (PcdSmbiosEnabled)) {
      InitializeSmbiosInfo ();
    }
    break;
  case PrePciEnumeration:
    break;
  case PostPciEnumeration:
    // Set pre-OS checker features flag
    if (mPchSciSupported) {
      if (!SciBootSuccess ()) {
        DEBUG ((DEBUG_WARN, "SCI device has boot issue\n"));
      }
    }
    ClearSmbusStatus ();

    break;
  case PrePayloadLoading:
    ///
    /// Initialize the IGD OpRegion
    ///
    IgdOpRegionPlatformInit ();

    ///
    /// Initialize the HECI device
    ///
    Status = HeciInitialize (HECI1_DEVICE);

    DEBUG ((DEBUG_INFO, "HeciInitLib init Status: %r\n", Status));

    //Initialize and setup MeMeasureboot
    MeMeasuredBootInit();
    break;
  case EndOfStages:
    ClearSmi ();
    if (GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE) {
      if (GetBootMode() != BOOT_ON_S3_RESUME) {
        ClearS3SaveRegion ();
        mSmmBaseInfo.SmmBaseHdr.Count     = (UINT8) MpGetInfo()->CpuCount;
        mSmmBaseInfo.SmmBaseHdr.TotalSize = sizeof(BL_PLD_COMM_HDR) + mSmmBaseInfo.SmmBaseHdr.Count * sizeof(CPU_SMMBASE);
        Status = AppendS3Info ((VOID *)&mSmmBaseInfo, TRUE);
        mS3SaveReg.S3SaveHdr.TotalSize = sizeof(BL_PLD_COMM_HDR) + mS3SaveReg.S3SaveHdr.Count * sizeof(REG_INFO);
        AppendS3Info ((VOID *)&mS3SaveReg, FALSE);
      }
    }
    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() != 0)) {
      ProgramSecuritySetting ();
    }
    break;
  case ReadyToBoot:
    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() == 0)) {
      ProgramSecuritySetting ();
    }
    break;
  case EndOfFirmware:
    break;
  default:
    break;
  }
}

/**
  Update FSP-S UPD config data for TCC mode and tuning

  @param  FspsUpd            The pointer to the FSP-S UPD to be updated.

  @retval EFI_NOT_FOUND                 Platform Features or Tcc mode not found
          EFI_ERROR                     Error trying to load sub-region
          EFI_SUCCESS                   Successfully loaded subregions
**/
EFI_STATUS
TccModePostMemConfig (
  FSPS_UPD  *FspsUpd
)
{
  UINT32                                    *TccCacheconfigBase;
  UINT32                                     TccCacheconfigSize;
  UINT32                                    *TccCrlBase;
  UINT32                                     TccCrlSize;
  UINT32                                    *TccStreamBase;
  UINT32                                     TccStreamSize;
  EFI_STATUS                                 Status;
  UINT8                                      Index;
  UINT8                                      MaxPchPcieRootPorts;
  BIOS_SETTINGS                             *PolicyConfig;
  TCC_STREAM_CONFIGURATION                  *StreamConfig;
  TCC_CFG_DATA                              *TccCfgData;

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag (CDATA_TCC_TAG);
  if ((TccCfgData == NULL) || ((TccCfgData->TccEnable == 0) && (TccCfgData->TccTuning == 0))) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "Set TCC silicon:\n"));

  // Set default values for TCC related Silicon settings
  FspsUpd->FspsConfig.TccModeEnable             = 1;
  FspsUpd->FspsConfig.Eist                      = 0;        // Intel Speed Shift
  FspsUpd->FspsConfig.Hwp                       = 0;        // Intel Hardware P-states support
  FspsUpd->FspsConfig.Cx                        = 0;        // Intel C-states support
  FspsUpd->FspsConfig.PsfTccEnable              = 1;        // Enable will decrease psf transaction latency by disabling some psf power management features
  FspsUpd->FspsConfig.PchDmiAspmCtrl            = 0;        // ASPM configuration on the PCH side of the DMI/OPI Link
  FspsUpd->FspsConfig.PchLegacyIoLowLatency     = 1;
  FspsUpd->FspsConfig.RenderStandby             = 0;        // IGFX RenderStandby
  FspsUpd->FspsConfig.EnableItbm                = 0;
  FspsUpd->FspsConfig.GtClosEnable              = 1;
  FspsUpd->FspsConfig.AcSplitLock               = 0;
  FspsUpd->FspsConfig.PchPmPwrBtnOverridePeriod = 0;
  FspsUpd->FspsConfig.PchTsnGbeMultiVcEnable    = 1;
  FspsUpd->FspsConfig.PseTsnGbeMultiVcEnable[0] = 1;
  FspsUpd->FspsConfig.PseTsnGbeMultiVcEnable[1] = 1;

  MaxPchPcieRootPorts = GetPchMaxPciePortNum ();
  for (Index = 0; Index < MaxPchPcieRootPorts; Index++) {
    FspsUpd->FspsConfig.PcieRpAspm[Index]           = 0;   // The Pcie ASPM configuration of the root port
    FspsUpd->FspsConfig.PcieRpL1Substates[Index]    = 0;    // The Pcie L1 Substates configuration of the root port
    FspsUpd->FspsConfig.PciePtm[Index]              = 1;
    if (IsRpMultiVC (Index) == 1) {
      FspsUpd->FspsConfig.PcieRpMultiVcEnabled[Index] = 1;
      FspsUpd->FspsConfig.PcieRpVc1TcMap[Index]       = 0x7F; // Enable Tc1 to Tc7 bits
    }
  }

  FspsUpd->FspsConfig.SoftwareSramEn  = TccCfgData->TccSoftSram;
  FspsUpd->FspsConfig.DsoTuningEn     = TccCfgData->TccTuning;
  FspsUpd->FspsConfig.TccErrorLogEn   = TccCfgData->TccErrorLog;
  FspsUpd->FspsConfig.IfuEnable       = 0;

// Load TCC stream config from container
  TccStreamBase = NULL;
  TccStreamSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'T'),
                          (VOID **)&TccStreamBase, &TccStreamSize);
  if (EFI_ERROR (Status) || (TccStreamSize < sizeof (TCC_STREAM_CONFIGURATION))) {
    DEBUG ((DEBUG_INFO, "Load TCC Stream %r, size = 0x%x\n", Status, TccStreamSize));
  } else {
    FspsUpd->FspsConfig.TccStreamCfgBase = (UINT32)(UINTN)TccStreamBase;
    FspsUpd->FspsConfig.TccStreamCfgSize = TccStreamSize;
    DEBUG ((DEBUG_INFO, "Load tcc stream @0x%p, size = 0x%x\n", TccStreamBase, TccStreamSize));

    // Update UPD from stream
    if (TccCfgData->TccTuning != 0) {
      StreamConfig   = (TCC_STREAM_CONFIGURATION *) TccStreamBase;
      PolicyConfig = (BIOS_SETTINGS *) &StreamConfig->BiosSettings;
      FspsUpd->FspsConfig.Eist                       = PolicyConfig->Pstates;
      FspsUpd->FspsConfig.Hwp                        = PolicyConfig->HwpEn;
      FspsUpd->FspsConfig.Cx                         = PolicyConfig->Cstates;
      FspsUpd->FspsConfig.TurboMode                  = PolicyConfig->Turbo;
      FspsUpd->FspsConfig.PsfTccEnable               = PolicyConfig->FabricPm;
      FspsUpd->FspsConfig.PchDmiAspmCtrl             = PolicyConfig->DmiAspm;
      FspsUpd->FspsConfig.PchLegacyIoLowLatency      = PolicyConfig->PchPwrClkGate;
      FspsUpd->FspsConfig.RenderStandby              = PolicyConfig->GtRstRc6;

      for (Index = 0; Index < MaxPchPcieRootPorts; Index++) {
        FspsUpd->FspsConfig.PcieRpAspm[Index]        = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcieRpL1Substates[Index] = PolicyConfig->PchPcieRpL1;
      }
      mTccDsoTuning      = TRUE;
      if (mPchSciSupported != 0) {
        mTccRtd3Support                = PolicyConfig->Dstates;
      }
    }
  }

  // Print configured TCC stream settings
  DEBUG ((DEBUG_INFO, "TCC Stage 2 parameters configuration details:\n"));
  DEBUG ((DEBUG_INFO, "Eist                  = %x\n", FspsUpd->FspsConfig.Eist                   ));
  DEBUG ((DEBUG_INFO, "Hwp                   = %x\n", FspsUpd->FspsConfig.Hwp                    ));
  DEBUG ((DEBUG_INFO, "Cx                    = %x\n", FspsUpd->FspsConfig.Cx                     ));
  DEBUG ((DEBUG_INFO, "TurboMode             = %x\n", FspsUpd->FspsConfig.TurboMode              ));
  DEBUG ((DEBUG_INFO, "PsfTccEnable          = %x\n", FspsUpd->FspsConfig.PsfTccEnable           ));
  DEBUG ((DEBUG_INFO, "PchDmiAspmCtrl        = %x\n", FspsUpd->FspsConfig.PchDmiAspmCtrl         ));
  DEBUG ((DEBUG_INFO, "PchLegacyIoLowLatency = %x\n", FspsUpd->FspsConfig.PchLegacyIoLowLatency  ));
  DEBUG ((DEBUG_INFO, "RenderStandby         = %x\n", FspsUpd->FspsConfig.RenderStandby          ));
  DEBUG ((DEBUG_INFO, "SoftwareSramEn        = %x\n", FspsUpd->FspsConfig.SoftwareSramEn         ));
  DEBUG ((DEBUG_INFO, "DsoTuningEn           = %x\n", FspsUpd->FspsConfig.DsoTuningEn            ));
  DEBUG ((DEBUG_INFO, "TccErrorLogEn         = %x\n", FspsUpd->FspsConfig.TccErrorLogEn          ));
  DEBUG ((DEBUG_INFO, "PcieRpAspm            = %x\n", FspsUpd->FspsConfig.PcieRpAspm[0]          ));
  DEBUG ((DEBUG_INFO, "PcieRpL1Substates     = %x\n", FspsUpd->FspsConfig.PcieRpL1Substates[0]   ));
  DEBUG ((DEBUG_INFO, "Rtd3Support           = %x\n", mTccRtd3Support                            ));

  // Load TCC cache config binary from container
  TccCacheconfigBase = NULL;
  TccCacheconfigSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'C'),
                                (VOID **)&TccCacheconfigBase, &TccCacheconfigSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TCC Cache config not found! %r\n", Status));
  } else {
    FspsUpd->FspsConfig.TccCacheCfgBase = (UINT32)(UINTN)TccCacheconfigBase;
    FspsUpd->FspsConfig.TccCacheCfgSize = TccCacheconfigSize;
    DEBUG ((DEBUG_INFO, "Load tcc cache @0x%p, size = 0x%x\n", TccCacheconfigBase, TccCacheconfigSize));
  }

  // Load TCC CRL binary from container
  TccCrlBase = NULL;
  TccCrlSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'M'),
                                (VOID **)&TccCrlBase, &TccCrlSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TCC CRL not found! %r\n", Status));
  } else {
    FspsUpd->FspsConfig.TccCrlBinBase = (UINT32)(UINTN)TccCrlBase;
    FspsUpd->FspsConfig.TccCrlBinSize = TccCrlSize;
    DEBUG ((DEBUG_INFO, "Load tcc crl @0x%p, size = 0x%x\n", TccCrlBase, TccCrlSize));
  }

  return Status;
}

/**
  Update PSE policies.

  @param[in] FspsUpd   Pointer to FSPS_UPD
  @param[in] SiCfgData Pointer to Silicon config data
**/
STATIC
VOID
FspUpdatePsePolicy (
  IN FSPS_UPD           *FspsUpd,
  IN SILICON_CFG_DATA   *SiCfgData
  )
{
  UINT32        Index;
  UINT32        *SiipRegionBase;
  UINT32        SiipRegionSize;
  UINTN         PinIndex;
  UINTN         PinIndexBase;
  FSP_S_CONFIG  *Fspscfg;
  EFI_STATUS  Status;
  Fspscfg     = &FspsUpd->FspsConfig;

  // Load Pse IP firmware from container and
  // update the base and size into FspSi config
  SiipRegionBase = NULL;
  SiipRegionSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('P', 'S', 'E', 'F'),
                          (VOID **)&SiipRegionBase, &SiipRegionSize);
  if (!EFI_ERROR (Status)) {
    if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
      TpmHashAndExtendPcrEventLog (0, (UINT8 *)SiipRegionBase, SiipRegionSize, EV_EFI_PLATFORM_FIRMWARE_BLOB, sizeof("PSEF Firmware"), (UINT8 *)"PSEF Firmware");
    }
  } else {
    DEBUG ((DEBUG_ERROR, "PSE is enabled but missing PSE FW !! %r\n", Status));
    return;
  }

  DEBUG ((DEBUG_INFO, "Load PSE firmware @ %p:0x%X - %r\n", SiipRegionBase, SiipRegionSize, Status));
  Fspscfg->SiipRegionBase         = (UINT32)(UINTN)SiipRegionBase;
  Fspscfg->SiipRegionSize         = SiipRegionSize;
  Fspscfg->PchPseLogOutputChannel = SiCfgData->PchPseLogOutputChannel;
  Fspscfg->PchPseLogOutputSize    = SiCfgData->PchPseLogOutputSize;
  Fspscfg->PchPseLogOutputOffset  = SiCfgData->PchPseLogOutputOffset;
  Fspscfg->PchPseEcliteEnabled    = 1;
  Fspscfg->PchPseOobEnabled       = 0;
  Fspscfg->PchPseWoLEnabled       = 1;
  Fspscfg->PchPseAicEnabled       = (UINT8)SiCfgData->PchPseAicEnabled;
  Fspscfg->CpuTempSensorReadEnable= 1;
  //Fspscfg->PseJtagEnabled       = 0;
  //Fspscfg->PseJtagPinMux        = 0;

  for (Index = 0; Index < GetPchMaxPseI2sControllersNum (); Index++) {
    Fspscfg->PchPseI2sEnable[Index]              = SiCfgData->PchPseI2sEnable[Index];
    Fspscfg->PchPseI2sSbInterruptEnable[Index]   = SiCfgData->PchPseI2sSbInterruptEnable[Index];
  }
  for (Index = 0; Index < GetPchMaxPsePWMNum (); Index++){
    Fspscfg->PchPsePwmPinEnable[Index] = SiCfgData->PchPsePwmPinEnable[Index];
  }
  Fspscfg->PchPseI2sTxPinMux[0]            = GPIO_VER3_MUXING_PSE_I2S0_TXD_GPP_E16;
  Fspscfg->PchPseI2sRxPinMux[0]            = GPIO_VER3_MUXING_PSE_I2S0_RXD_GPP_E15;
  Fspscfg->PchPseI2sSfrmPinMux[0]          = GPIO_VER3_MUXING_PSE_I2S0_SFRM_GPP_E21;
  Fspscfg->PchPseI2sSclkPinMux[0]          = GPIO_VER3_MUXING_PSE_I2S0_SCLK_GPP_E20;

  Fspscfg->PchPsePwmEnable            = SiCfgData->PchPsePwmEnable;
  Fspscfg->PchPsePwmSbInterruptEnable = SiCfgData->PchPsePwmSbInterruptEnable;

  Fspscfg->PchPsePwmPinMux[8]         = GPIO_VER3_MUXING_PSE_PWM_08_GPP_E4;
  Fspscfg->PchPsePwmPinMux[9]         = GPIO_VER3_MUXING_PSE_PWM_09_GPP_E5;
  Fspscfg->PchPsePwmPinMux[10]        = GPIO_VER3_MUXING_PSE_PWM_10_GPP_E6;
  Fspscfg->PchPsePwmPinMux[11]        = GPIO_VER3_MUXING_PSE_PWM_11_GPP_E17;
  Fspscfg->PchPsePwmPinMux[12]        = GPIO_VER3_MUXING_PSE_PWM_12_GPP_E18;
  Fspscfg->PchPsePwmPinMux[13]        = GPIO_VER3_MUXING_PSE_PWM_13_GPP_E19;
  Fspscfg->PchPsePwmPinMux[14]        = GPIO_VER3_MUXING_PSE_PWM_14_GPP_H10;
  Fspscfg->PchPsePwmPinMux[15]        = GPIO_VER3_MUXING_PSE_PWM_15_GPP_H11;

  for (Index = 0; Index < GetPchMaxPseUartControllersNum (); Index++) {
    Fspscfg->PchPseUartEnable[Index]            = SiCfgData->PchPseUartEnable[Index];
    Fspscfg->PchPseUartSbInterruptEnable[Index] = SiCfgData->PchPseUartSbInterruptEnable[Index];
  }
  for (Index = 0; Index < GetPchMaxPseHsuartControllersNum (); Index++) {
    Fspscfg->PchPseHsuartEnable[Index]            = SiCfgData->PchPseHsuartEnable[Index];
  }
  for (Index = 0; Index < GetPchMaxPseQepControllersNum (); Index++) {
    Fspscfg->PchPseQepEnable[Index]            = SiCfgData->PchPseQepEnable[Index];
    Fspscfg->PchPseQepSbInterruptEnable[Index] = SiCfgData->PchPseQepSbInterruptEnable[Index];
  }
  for (Index = 0; Index < GetPchMaxPseI2cControllersNum (); Index++) {
    Fspscfg->PchPseI2cEnable[Index]            = SiCfgData->PchPseI2cEnable[Index];
    Fspscfg->PchPseI2cSbInterruptEnable[Index] = SiCfgData->PchPseI2cSbInterruptEnable[Index];
  }
  for (Index = 0; Index < GetPchMaxPseSpiControllersNum (); Index++) {
    Fspscfg->PchPseSpiEnable[Index]            = SiCfgData->PchPseSpiEnable[Index];
    Fspscfg->PchPseSpiSbInterruptEnable[Index] = SiCfgData->PchPseSpiSbInterruptEnable[Index];
    Fspscfg->PchPseSpiCs0Enable[Index]         = SiCfgData->PchPseSpiCs0Enable[Index];
  }
  Fspscfg->PchPseSpiMosiPinMux[1]              = GPIO_VER3_MUXING_PSE_SPI1_MOSI_GPP_D3;
  Fspscfg->PchPseSpiMisoPinMux[1]              = GPIO_VER3_MUXING_PSE_SPI1_MISO_GPP_D2;
  Fspscfg->PchPseSpiClkPinMux[1]               = GPIO_VER3_MUXING_PSE_SPI1_SCLK_GPP_D1;
  Fspscfg->PchPseSpiCs0PinMux[1]               = GPIO_VER3_MUXING_PSE_SPI1_CSB0_GPP_D0;

  Fspscfg->PchPseAdcEnable            = SiCfgData->PchPseAdcEnable;
  Fspscfg->PchPseAdcSbInterruptEnable = SiCfgData->PchPseAdcSbInterruptEnable;
  for (Index = 0; Index < GetPchMaxPseCanControllersNum (); Index++) {
    Fspscfg->PchPseCanEnable[Index]            = SiCfgData->PchPseCanEnable[Index];
    Fspscfg->PchPseCanSbInterruptEnable[Index] = SiCfgData->PchPseCanSbInterruptEnable[Index];
  }
  for (Index = 0; Index < GetPchMaxPseGbeControllersNum (); Index++) {
    Fspscfg->PchPseGbeEnable[Index]            = SiCfgData->PchPseGbeEnable[Index];
  }
  for (Index = 0; Index < GetPchMaxPseDmaControllersNum (); Index++) {
    Fspscfg->PchPseDmaEnable[Index]            = SiCfgData->PchPseDmaEnable[Index];
    Fspscfg->PchPseDmaSbInterruptEnable[Index] = SiCfgData->PchPseDmaSbInterruptEnable[Index];
  }
  Fspscfg->PchPseLh2PseSbInterruptEnable  = SiCfgData->PchPseLh2PseSbInterruptEnable;
  for (Index = 0; Index < GetPchMaxPseTimedGpioControllersNum (); Index++) {
    Fspscfg->PchPseTimedGpioEnable[Index] = SiCfgData->PchPseTimedGpioEnable[Index];
    if (Index == 0) {
      PinIndexBase = 0;
    } else if (Index == 1) {
      PinIndexBase = 30;
    }
    Fspscfg->PchPseTimedGpioPinAllocation[Index] = SiCfgData->PchPseTimedGpioPinAllocation[Index];
      for (PinIndex = PinIndexBase; PinIndex < PinIndexBase + 30; PinIndex++) {
          Fspscfg->PchPseTimedGpioPinEnable[PinIndex] = SiCfgData->PchPseTimedGpioPinEnable[PinIndex];
      }
  }
  Fspscfg->PchPseTgpio6PinMux             = GPIO_VER3_MUXING_PSE_TGPIO6_GPP_T3;
  Fspscfg->PchPseTgpio7PinMux             = GPIO_VER3_MUXING_PSE_TGPIO7_GPP_G11;
  Fspscfg->PchPseTgpio8PinMux             = GPIO_VER3_MUXING_PSE_TGPIO8_GPP_B7;
  Fspscfg->PchPseTgpio9PinMux             = GPIO_VER3_MUXING_PSE_TGPIO9_GPP_B8;
  Fspscfg->PchPseTgpio10PinMux            = GPIO_VER3_MUXING_PSE_TGPIO10_GPP_U7;
  Fspscfg->PchPseTgpio11PinMux            = GPIO_VER3_MUXING_PSE_TGPIO11_GPP_U11;
  Fspscfg->PchPseTgpio12PinMux            = GPIO_VER3_MUXING_PSE_TGPIO12_GPP_U19;
  Fspscfg->PchPseTgpio13PinMux            = GPIO_VER3_MUXING_PSE_TGPIO13_GPP_D12;
  Fspscfg->PchPseTgpio14PinMux            = GPIO_VER3_MUXING_PSE_TGPIO14_GPP_E20;
  Fspscfg->PchPseTgpio15PinMux            = GPIO_VER3_MUXING_PSE_TGPIO15_GPP_E3;
  Fspscfg->PchPseTgpio16PinMux            = GPIO_VER3_MUXING_PSE_TGPIO16_GPP_E7;
  Fspscfg->PchPseTgpio17PinMux            = GPIO_VER3_MUXING_PSE_TGPIO17_GPP_E15;
  Fspscfg->PchPseTgpio18PinMux            = GPIO_VER3_MUXING_PSE_TGPIO18_GPP_E6;
  Fspscfg->PchPseTgpio19PinMux            = GPIO_VER3_MUXING_PSE_TGPIO19_GPP_C1;
}

/**
  Update FSP-S UPD config data

  @param  FspsUpdPtr            The pointer to the FSP-S UPD to be updated.
**/
VOID
UpdateFspConfig (
  VOID     *FspsUpdPtr
)
{
  UINT8       MaxPcieRootPorts;
  UINT8       MaxSataPorts;
  UINT8       MaxUsb2Ports;
  UINT8       MaxUsb3Ports;
  UINT8       DebugPort;
  UINT32      Index;
  UINT32      BaseAddress;
  UINT32      TotalSize;
  UINT32      SpiIasBase;
  UINT32      *TsnMacAddrBase;
  UINT32      TsnMacAddrSize;
  UINT32      *PseTsnIpConfigBase;
  UINT32      PseTsnIpConfigSize;
  UINT32      *TsnConfigBase;
  UINT32      TsnConfigSize;
  BOOLEAN     BiosProtected;
  BOOLEAN     IasProtected;
  EFI_STATUS  Status;
  FLASH_REGION_TYPE  RegionType;
  FSPS_UPD           *FspsUpd;
  FSP_S_CONFIG       *Fspscfg;
  SILICON_CFG_DATA   *SiCfgData;
  POWER_CFG_DATA     *PowerCfgData;
  MEMORY_CFG_DATA    *MemCfgData;
  UINT8              SaDisplayConfigTable[17] = { 0 };

  FspsUpd    = (FSPS_UPD *)FspsUpdPtr;
  Fspscfg     = &FspsUpd->FspsConfig;
  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);

  if (SiCfgData == NULL) {
    DEBUG ((DEBUG_INFO, "Failed to find Silicon CFG!\n"));
  }

  // eMMC, SdCard
  Fspscfg->ScsEmmcEnabled      = 1;
  Fspscfg->ScsEmmcHs400Enabled = 1;
  Fspscfg->ScsSdCardEnabled    = 1;
  Fspscfg->ScsEmmcDdr50Enabled = 1;

  // Ufs
  Fspscfg->UfsEnable[0]        = 0;
  Fspscfg->UfsEnable[1]        = 0;

  Fspscfg->DevIntConfigPtr     = (UINT32)(UINTN)mPchDevIntConfig;
  Fspscfg->NumOfDevIntConfig   = sizeof (mPchDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);

  Fspscfg->PxRcConfig[0]       = 11;
  Fspscfg->PxRcConfig[1]       = 10;
  Fspscfg->PxRcConfig[2]       = 11;
  Fspscfg->PxRcConfig[3]       = 11;
  Fspscfg->PxRcConfig[4]       = 11;
  Fspscfg->PxRcConfig[5]       = 11;
  Fspscfg->PxRcConfig[6]       = 11;
  Fspscfg->PxRcConfig[7]       = 11;

  /* TBD: dummy values to avoid a panic */
  for (Index = 0; Index < (sizeof (Fspscfg->CpuUsb3OverCurrentPin) / sizeof (Fspscfg->CpuUsb3OverCurrentPin[0])); Index++) {
    Fspscfg->CpuUsb3OverCurrentPin[Index] = 0xFF; //UsbOverCurrentPinSkip = 0xFF
  }

  if (GetBootMode () != BOOT_ON_FLASH_UPDATE) {
    BiosProtected = FALSE;
    IasProtected  = FALSE;
    Status = SpiGetRegionAddress (FlashRegionBios, &BaseAddress, &TotalSize);
    if (!EFI_ERROR (Status)) {
      BiosProtected = TRUE;
      //
      // TODO:
      // Disble flash write firstly, so that UEFI payload variable could work
      // Later need refine the range to exclue UEFI variable range.
      //
      Fspscfg->PchWriteProtectionEnable[0] = FALSE;
      Fspscfg->PchReadProtectionEnable[0]  = FALSE;
      Fspscfg->PchProtectedRangeBase[0]    = (UINT16) (BaseAddress >> 12);
      Fspscfg->PchProtectedRangeLimit[0]   = (UINT16) ((BaseAddress + TotalSize - 1) >> 12);

      RegionType = (FLASH_REGION_TYPE) PcdGet32 (PcdSpiIasImageRegionType);
      if (RegionType < FlashRegionMax) {
        if (RegionType != FlashRegionBios) {
          Status = GetComponentInfo (FLASH_MAP_SIG_SPI_IAS1, &SpiIasBase, NULL);
          if (EFI_ERROR (Status)) {
            return;
          }
          SpiIasBase &= 0xFFFF;
          Status = SpiGetRegionAddress (RegionType, &BaseAddress, &TotalSize);
          if (!EFI_ERROR (Status)) {
            if ((PcdGet32 (PcdSpiIasImage1RegionSize) + PcdGet32 (PcdSpiIasImage2RegionSize)) <= TotalSize) {
              IasProtected = TRUE;
              Fspscfg->PchWriteProtectionEnable[1] = TRUE;
              Fspscfg->PchReadProtectionEnable[1]  = FALSE;
              Fspscfg->PchProtectedRangeBase[1]    = (UINT16) ((BaseAddress + SpiIasBase) >> 12);
              Fspscfg->PchProtectedRangeLimit[1]   = (UINT16) ((BaseAddress + SpiIasBase +
                                                          PcdGet32 (PcdSpiIasImage1RegionSize) +
                                                          PcdGet32 (PcdSpiIasImage2RegionSize) - 1) >> 12);
            }
          }
        } else {
          IasProtected = TRUE;
        }
      }
    }
    DEBUG (((BiosProtected) ? DEBUG_INFO : DEBUG_WARN, "BIOS SPI region will %a protected\n", (BiosProtected) ? "be" : "NOT BE"));
    DEBUG (((IasProtected)  ? DEBUG_INFO : DEBUG_WARN, "IAS  SPI region will %a protected\n", (IasProtected)  ? "be" : "NOT BE"));
  }

  Fspscfg->EnableTcoTimer    = 0;

  Fspscfg->SkipMpInit        = 0;

  Fspscfg->PchSerialIoI2cSdaPinMux[4] = GPIO_VER3_MUXING_SERIALIO_I2C4_SDA_GPP_H8;
  Fspscfg->PchSerialIoI2cSclPinMux[4] = GPIO_VER3_MUXING_SERIALIO_I2C4_SCL_GPP_H9;

  if (SiCfgData != NULL) {
    // Xdci
    Fspscfg->XdciEnable = SiCfgData->XdciEnable;

    //CPU Config Data
    Fspscfg->AesEnable                   = SiCfgData->AesEnable;
    Fspscfg->TxtEnable                   = SiCfgData->TxtEnable;
    Fspscfg->PpinSupport                 = SiCfgData->PpinSupport;
    Fspscfg->AcSplitLock                 = SiCfgData->AcSplitLock;

    //CPU SGX Data (Removed due to fsp 2233)
    //Fspscfg->SgxSinitNvsData             = SiCfgData->SgxSinitNvsData;

    //CPU ITBT Config
    Fspscfg->ITbtForcePowerOnTimeoutInMs    = SiCfgData->ITbtForcePowerOnTimeoutInMs;
    Fspscfg->ITbtConnectTopologyTimeoutInMs = SiCfgData->ITbtConnectTopologyTimeoutInMs;
    CopyMem (Fspscfg->ITbtPcieRootPortEn, SiCfgData->ITbtPcieRootPortEn, sizeof(SiCfgData->ITbtPcieRootPortEn));

    // SA_MISC_PEI_CONFIG
    Fspscfg->Device4Enable              = SiCfgData->Device4Enable;

    // GRAPHICS_PEI_CONFIG
    Fspscfg->PavpEnable                 = SiCfgData->PavpEnable;
    Fspscfg->CdClock                    = SiCfgData->CdClock;
    Fspscfg->PeiGraphicsPeimInit        = SiCfgData->PeiGraphicsPeimInit;
    Fspscfg->GraphicsConfigPtr          = PcdGet32(PcdGraphicsVbtAddress);

    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mEhlCrbRowDisplayDdiConfig, sizeof(mEhlCrbRowDisplayDdiConfig));
    Fspscfg->DdiPortAConfig             = SaDisplayConfigTable[0];
    Fspscfg->DdiPortBConfig             = SaDisplayConfigTable[1];
    Fspscfg->DdiPortCConfig             = SaDisplayConfigTable[2];
    Fspscfg->DdiPortAHpd                = SaDisplayConfigTable[3];
    Fspscfg->DdiPortBHpd                = SaDisplayConfigTable[4];
    Fspscfg->DdiPortCHpd                = SaDisplayConfigTable[5];
    Fspscfg->DdiPort1Hpd                = SaDisplayConfigTable[6];
    Fspscfg->DdiPort2Hpd                = SaDisplayConfigTable[7];
    Fspscfg->DdiPort3Hpd                = SaDisplayConfigTable[8];
    Fspscfg->DdiPort4Hpd                = SaDisplayConfigTable[9];
    Fspscfg->DdiPortADdc                = SaDisplayConfigTable[10];
    Fspscfg->DdiPortBDdc                = SaDisplayConfigTable[11];
    Fspscfg->DdiPortCDdc                = SaDisplayConfigTable[12];
    Fspscfg->DdiPort1Ddc                = SaDisplayConfigTable[13];
    Fspscfg->DdiPort2Ddc                = SaDisplayConfigTable[14];
    Fspscfg->DdiPort3Ddc                = SaDisplayConfigTable[15];
    Fspscfg->DdiPort4Ddc                = SaDisplayConfigTable[16];

    // VMD_PEI_CONFIG
    Fspscfg->VmdEnable                  = SiCfgData->VmdEnable;
    Fspscfg->VmdPortA                   = SiCfgData->VmdPortA;
    Fspscfg->VmdPortB                   = SiCfgData->VmdPortB;
    Fspscfg->VmdPortC                   = SiCfgData->VmdPortC;
    Fspscfg->VmdPortD                   = SiCfgData->VmdPortD;
    Fspscfg->VmdCfgBarSz                = 25;//SiCfgData->VmdCfgBarSz;
    Fspscfg->VmdCfgBarAttr              = 2;//SiCfgData->VmdCfgBarAttr;
    Fspscfg->VmdMemBarSz1               = 25;//SiCfgData->VmdMemBarSz1;
    Fspscfg->VmdMemBar1Attr             = SiCfgData->VmdMemBar1Attr;
    Fspscfg->VmdMemBarSz2               = 20;//SiCfgData->VmdMemBarSz2;
    Fspscfg->VmdMemBar2Attr             = SiCfgData->VmdMemBar2Attr;

    // TCSS_PEI_CONFIG
    CopyMem (Fspscfg->IomTypeCPortPadCfg,           SiCfgData->IomTypeCPortPadCfg,        sizeof(SiCfgData->IomTypeCPortPadCfg));
    CopyMem (Fspscfg->CpuUsb3OverCurrentPin,        SiCfgData->CpuUsb3OverCurrentPin,     sizeof(SiCfgData->CpuUsb3OverCurrentPin));
    Fspscfg->UsbTcPortEn                = SiCfgData->UsbTcPortEn;
    Fspscfg->UsbOverride                = SiCfgData->UsbOverride;
    Fspscfg->TcssAuxOri                 = SiCfgData->TcssAuxOri;
    Fspscfg->TcssHslOri                 = SiCfgData->TcssHslOri;
    Fspscfg->VccSt                      = SiCfgData->VccSt;
    Fspscfg->PmcPdEnable                = 1;//SiCfgData->PmcPdEnable;
    Fspscfg->D3HotEnable                = SiCfgData->D3HotEnable;
    Fspscfg->D3ColdEnable               = SiCfgData->D3ColdEnable;

    // PCH_PCIE_CONFIG
    Fspscfg->PchLegacyIoLowLatency                                           = SiCfgData->PchLegacyIoLowLatency;
    Fspscfg->PchPostMasterClockGating                                            = SiCfgData->PchMasterClockGating2;
    Fspscfg->PchPostMasterPowerGating                                            = SiCfgData->PchMasterPowerGating2;
    Fspscfg->TestPcieClockGating                                             = SiCfgData->TestPcieClockGating;

    MaxPcieRootPorts = GetPchMaxPciePortNum ();
    for (Index = 0; Index < MaxPcieRootPorts; Index++) {
      Fspscfg->PcieRpHotPlug[Index]                                          = SiCfgData->PcieRpHotPlug[Index];
      Fspscfg->PcieRpPmSci[Index]                                            = SiCfgData->PcieRpPmSci[Index];
      Fspscfg->PcieRpTransmitterHalfSwing[Index]                             = SiCfgData->PcieRpTransmitterHalfSwing[Index];
      Fspscfg->PcieRpClkReqDetect[Index]                                     = SiCfgData->PcieRpClkReqDetect[Index];
      Fspscfg->PcieRpAdvancedErrorReporting[Index]                           = SiCfgData->PcieRpAdvancedErrorReporting[Index];
      Fspscfg->PcieRpUnsupportedRequestReport[Index]                         = SiCfgData->PcieRpUnsupportedRequestReport[Index];
      Fspscfg->PcieRpFatalErrorReport[Index]                                 = SiCfgData->PcieRpFatalErrorReport[Index];
      Fspscfg->PcieRpNoFatalErrorReport[Index]                               = SiCfgData->PcieRpNoFatalErrorReport[Index];
      Fspscfg->PcieRpCorrectableErrorReport[Index]                           = SiCfgData->PcieRpCorrectableErrorReport[Index];
      Fspscfg->PcieRpSystemErrorOnFatalError[Index]                          = SiCfgData->PcieRpSystemErrorOnFatalError[Index];
      Fspscfg->PcieRpSystemErrorOnNonFatalError[Index]                       = SiCfgData->PcieRpSystemErrorOnNonFatalError[Index];
      Fspscfg->PcieRpSystemErrorOnCorrectableError[Index]                    = SiCfgData->PcieRpSystemErrorOnCorrectableError[Index];
      Fspscfg->PcieRpMaxPayload[Index]                                       = SiCfgData->PcieRpMaxPayload[Index];
      Fspscfg->PcieRpPcieSpeed[Index]                                        = SiCfgData->PcieRpPcieSpeed[Index];
      Fspscfg->PcieRpPhysicalSlotNumber[Index]                               = SiCfgData->PcieRpPhysicalSlotNumber[Index];
      Fspscfg->PcieRpSlotImplemented[Index]                                  = SiCfgData->PcieRpSlotImplemented[Index];
      Fspscfg->PcieRpCompletionTimeout[Index]                                = SiCfgData->PcieRpCompletionTimeout[Index];
      Fspscfg->PcieRpAspm[Index]                                             = SiCfgData->PcieRpAspm[Index];
      Fspscfg->PcieRpL1Substates[Index]                                      = SiCfgData->PcieRpL1Substates[Index];
      Fspscfg->PcieRpLtrEnable[Index]                                        = SiCfgData->PcieRpLtrEnable[Index];
      Fspscfg->PcieRpLtrConfigLock[Index]                                    = SiCfgData->PcieRpLtrConfigLock[Index];
      Fspscfg->PcieRpAcsEnabled[Index]                                       = SiCfgData->PcieRpAcsEnabled[Index];
      Fspscfg->PcieRpEnableCpm[Index]                                        = SiCfgData->PcieRpEnableCpm[Index];
      Fspscfg->PcieRpDetectTimeoutMs[Index]                                  = SiCfgData->PcieRpDetectTimeoutMs[Index];
      Fspscfg->PciePtm[Index]                                                = SiCfgData->PciePtm[Index];
      //Fspscfg->PcieRpLtrMaxSnoopLatency[Index]                               = 0x1003;
      //Fspscfg->PcieRpLtrMaxNoSnoopLatency[Index]                             = 0x1003;
      Fspscfg->PcieRpSnoopLatencyOverrideMode[Index]                         = SiCfgData->PcieRpSnoopLatencyOverrideMode[Index];
      Fspscfg->PcieRpSnoopLatencyOverrideMultiplier[Index]                   = SiCfgData->PcieRpSnoopLatencyOverrideMultiplier[Index];
      Fspscfg->PcieRpSnoopLatencyOverrideValue[Index]                        = 0x003C;
      Fspscfg->PcieRpNonSnoopLatencyOverrideMode[Index]                      = SiCfgData->PcieRpNonSnoopLatencyOverrideMode[Index];
      Fspscfg->PcieRpNonSnoopLatencyOverrideMultiplier[Index]                = SiCfgData->PcieRpNonSnoopLatencyOverrideMultiplier[Index];
      Fspscfg->PcieRpNonSnoopLatencyOverrideValue[Index]                     = 0x003C;
      Fspscfg->PcieRpSlotPowerLimitScale[Index]                              = SiCfgData->PcieRpSlotPowerLimitScale[Index];
      Fspscfg->PcieRpSlotPowerLimitValue[Index]                              = SiCfgData->PcieRpSlotPowerLimitValue[Index];
    }

    // PCH_SATA_CONFIG
    MaxSataPorts = GetPchMaxSataPortNum (SATA_1_CONTROLLER_INDEX);
    for (Index = 0; Index < MaxSataPorts; Index++) {
      Fspscfg->SataPortsEnable[Index]             = SiCfgData->SataPortsEnable[Index];
      Fspscfg->SataPortsHotPlug[Index]            = SiCfgData->SataPortsHotPlug[Index];
      Fspscfg->SataPortsSpinUp[Index]             = SiCfgData->SataPortsSpinUp[Index];
      Fspscfg->SataPortsExternal[Index]           = SiCfgData->SataPortsExternal[Index];
      Fspscfg->SataPortsDevSlp[Index]             = SiCfgData->SataPortsDevSlp[Index];
      Fspscfg->SataPortsEnableDitoConfig[Index]   = SiCfgData->SataPortsEnableDitoConfig[Index];
      Fspscfg->SataPortsDmVal[Index]              = SiCfgData->SataPortsDmVal[Index];
      Fspscfg->SataPortsDitoVal[Index]            = 0x271;//SiCfgData->SataPortsDitoVal
      Fspscfg->SataPortsSolidStateDrive[Index]    = SiCfgData->SataPortsSolidStateDrive[Index];
    }
    Fspscfg->SataEnable                           = SiCfgData->SataEnable;
    Fspscfg->SataMode                             = SiCfgData->SataMode;
    Fspscfg->SataPwrOptEnable                     = SiCfgData->SataPwrOptEnable;
    for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
      Fspscfg->SataRstPcieEnable[Index]           = SiCfgData->SataRstPcieEnable[Index];
      Fspscfg->SataRstPcieStoragePort[Index]      = SiCfgData->SataRstPcieStoragePort[Index];
      Fspscfg->SataRstPcieDeviceResetDelay[Index] = SiCfgData->SataRstPcieDeviceResetDelay[Index];
    }
    Fspscfg->SataSalpSupport                      = 1;

    Fspscfg->PchLanEnable                         = 0;

    // PCH_USB_CONFIG
    MaxUsb2Ports = GetPchXhciMaxUsb2PortNum ();
    for (Index = 0; Index < MaxUsb2Ports; Index++) {
      Fspscfg->PortUsb20Enable[Index]    = SiCfgData->PortUsb20Enable[Index];
      Fspscfg->Usb2OverCurrentPin[Index] = SiCfgData->Usb2OverCurrentPin[Index];
      Fspscfg->USB20Otg[Index]           = SiCfgData->USB20Otg[Index];
      Fspscfg->Usb2PhyPetxiset[Index]    = SiCfgData->Usb2PhyPetxiset[Index];
      Fspscfg->Usb2PhyTxiset[Index]      = SiCfgData->Usb2PhyTxiset[Index];
      Fspscfg->Usb2PhyPredeemp[Index]    = SiCfgData->Usb2PhyPredeemp[Index];
      Fspscfg->Usb2PhyPehalfbit[Index]   = SiCfgData->Usb2PhyPehalfbit[Index];
    }
    MaxUsb3Ports = GetPchXhciMaxUsb3PortNum ();
    for (Index = 0; Index < MaxUsb3Ports; Index++) {
      Fspscfg->PortUsb30Enable[Index]               = SiCfgData->PortUsb30Enable[Index];
      Fspscfg->Usb3OverCurrentPin[Index]            = SiCfgData->Usb3OverCurrentPin[Index];
      Fspscfg->USB30Otg[Index]                      = SiCfgData->USB30Otg[Index];
    }
    Fspscfg->UsbClockGatingEnable                   = SiCfgData->PchMasterClockGating2;
    Fspscfg->UsbPowerGatingEnable                   = SiCfgData->PchMasterPowerGating2;

    // HSIO USB3 signal compliance settings
    CopyMem (Fspscfg->Usb3HsioTxDeEmphEnable,        SiCfgData->Usb3HsioTxDeEmphEnable,        sizeof(SiCfgData->Usb3HsioTxDeEmphEnable));
    CopyMem (Fspscfg->Usb3HsioTxDeEmph,              SiCfgData->Usb3HsioTxDeEmph,              sizeof(SiCfgData->Usb3HsioTxDeEmph));
    CopyMem (Fspscfg->Usb3HsioTxDownscaleAmpEnable,  SiCfgData->Usb3HsioTxDownscaleAmpEnable,  sizeof(SiCfgData->Usb3HsioTxDownscaleAmpEnable));
    CopyMem (Fspscfg->Usb3HsioTxDownscaleAmp,        SiCfgData->Usb3HsioTxDownscaleAmp,        sizeof(SiCfgData->Usb3HsioTxDownscaleAmp));

    // PCH_LOCK_DOWN_CONFIG
    Fspscfg->PchLockDownBiosLock         = SiCfgData->PchLockDownBiosLock;


    // CNVi Config
    Fspscfg->CnviMode                    = 1;
    Fspscfg->CnviBtCore                  = 1;
    Fspscfg->CnviBtAudioOffload          = 0;
    Fspscfg->CnviRfResetPinMux           = GPIO_VER3_MUXING_CNVI_RF_RESET_GPP_G8;
    Fspscfg->CnviClkreqPinMux            = GPIO_VER3_MUXING_CNVI_CRF_XTAL_CLKREQ_GPP_G9;

    // PCH_THERMAL_CONFIG
    Fspscfg->DmiTS0TW                    = SiCfgData->DmiTS0TW;
    Fspscfg->DmiTS1TW                    = SiCfgData->DmiTS1TW;
    Fspscfg->DmiTS2TW                    = SiCfgData->DmiTS2TW;

    for (Index = 0; Index < MaxTsGpioPin; Index++) {
      Fspscfg->PchMemoryPmsyncEnable[Index]     = 1;
      Fspscfg->PchMemoryC0TransmitEnable[Index] = 1;
      Fspscfg->PchMemoryPinSelection[Index]     = 0;
    }

    // PCH SERIAL SPI_CONFIG
    for (Index = 0; Index < PCH_MAX_SERIALIO_SPI_CONTROLLERS; Index++) {
      Fspscfg->SerialIoSpiMode[Index] = SiCfgData->SerialIoSpiMode[Index];
    }

    // PCH_DMI_CONFIG
    Fspscfg->PchPwrOptEnable            = SiCfgData->PchPwrOptEnable;

    // PCH_eSPI_CONFIG
    Fspscfg->PchEspiLgmrEnable          = SiCfgData->PchEspiLgmrEnable;

    // PCH SPI_CONFIG
    Fspscfg->SerialIoSpiMode[1]         = 0;

    Fspscfg->SerialIoSpiCsPolarity[0]   = 1;
    Fspscfg->SerialIoSpiCsPolarity[1]   = 1;
    Fspscfg->SerialIoSpiCsPolarity[2]   = 0;

    // PCH SERIAL_UART_CONFIG
    for (Index = 0; Index < GetPchMaxSerialIoUartControllersNum (); Index++) {
      Fspscfg->SerialIoUartMode[Index]            = SiCfgData->SerialIoUartMode[Index];
//    Fspscfg->SerialIoUartBaudRate[Index]        = SiCfgData->SerialIoUartBaudRate[Index];
      Fspscfg->SerialIoUartParity[Index]          = SiCfgData->SerialIoUartParity[Index];
      Fspscfg->SerialIoUartDataBits[Index]        = SiCfgData->SerialIoUartDataBits[Index];
      Fspscfg->SerialIoUartStopBits[Index]        = SiCfgData->SerialIoUartStopBits[Index];
      Fspscfg->SerialIoUartAutoFlow[Index]        = SiCfgData->SerialIoUartAutoFlow[Index];
      Fspscfg->SerialIoUartPowerGating[Index]     = SiCfgData->SerialIoUartPowerGating[Index];
      Fspscfg->SerialIoUartDmaEnable[Index]       = SiCfgData->SerialIoUartDmaEnable[Index];
      Fspscfg->SerialIoUartDbg2[Index]            = SiCfgData->SerialIoUartDbg2[Index];
    }
    DebugPort = GetDebugPort();
    if (DebugPort < PCH_MAX_SERIALIO_UART_CONTROLLERS) {
      Fspscfg->SerialIoDebugUartNumber = DebugPort;
      // Skip FSP-S to reinitialize current UART port
      Fspscfg->SerialIoUartMode[DebugPort] = 0x4;
    }

    // PCH I2C_CONFIG
    for (Index = 0; Index < GetPchMaxSerialIoI2cControllersNum (); Index++) {
      Fspscfg->SerialIoI2cMode[Index] = SiCfgData->SerialIoI2cMode[Index];
      Fspscfg->PchSerialIoI2cPadsTermination[Index]  = SiCfgData->PchSerialIoI2cPadsTermination[Index];
//    SerialIoConfig->I2cDeviceConfig[Index].PinMux.Sda     = Fspscfg->PchSerialIoI2cSdaPinMux[Index];
//    SerialIoConfig->I2cDeviceConfig[Index].PinMux.Scl     = Fspscfg->PchSerialIoI2cSclPinMux[Index];
    }

    // PCH_THC_CONFIG
    Fspscfg->ThcPort0Assignment         = SiCfgData->ThcPort0Assignment;
    Fspscfg->ThcPort1Assignment         = SiCfgData->ThcPort1Assignment;

    // PCH_FIVR_CONFIG
    Fspscfg->PchFivrExtV1p05RailVoltage                  = SiCfgData->PchFivrExtV1p05RailVoltage;
    Fspscfg->PchFivrExtV1p05RailIccMax                   = SiCfgData->PchFivrExtV1p05RailIccMax;
    Fspscfg->PchFivrExtVnnRailVoltage                    = SiCfgData->PchFivrExtVnnRailVoltage;
    Fspscfg->PchFivrExtVnnRailIccMax                     = SiCfgData->PchFivrExtVnnRailIccMax;
    Fspscfg->PchFivrExtVnnRailSxVoltage                  = SiCfgData->PchFivrExtVnnRailSxVoltage;
    Fspscfg->PchFivrExtVnnRailSxIccMax                   = SiCfgData->PchFivrExtVnnRailSxIccMax;
    Fspscfg->SdCardGpioCmdPadTermination                 = 0x1F;
    Fspscfg->PchFivrVccinAuxLowToHighCurModeVolTranTime  = SiCfgData->PchFivrVccinAuxLowToHighCurModeVolTranTime;//0x0C;
    Fspscfg->PchFivrVccinAuxRetToHighCurModeVolTranTime  = SiCfgData->PchFivrVccinAuxRetToHighCurModeVolTranTime;//0x36;
    Fspscfg->PchFivrVccinAuxRetToLowCurModeVolTranTime   = SiCfgData->PchFivrVccinAuxRetToLowCurModeVolTranTime;//0x2B;
    Fspscfg->PchFivrVccinAuxOffToHighCurModeVolTranTime  = SiCfgData->PchFivrVccinAuxOffToHighCurModeVolTranTime;//0x0096;
    Fspscfg->PchFivrDynPm                                = SiCfgData->PchFivrDynPm;//0x01;
    Fspscfg->PchFivrExtVnnRailIccMaximum                 = SiCfgData->PchFivrExtVnnRailIccMaximum;//0xC8;
    Fspscfg->PchFivrExtV1p05RailIccMaximum               = SiCfgData->PchFivrExtV1p05RailIccMaximum;//0x64;
    Fspscfg->PchFivrExtVnnRailSxIccMaximum               = SiCfgData->PchFivrExtVnnRailSxIccMaximum;//0xC8;
    Fspscfg->PchFivrExtV1p05RailCtrlRampTmr              = SiCfgData->PchFivrExtV1p05RailCtrlRampTmr;//0x1;
    Fspscfg->PchFivrExtV1p05RailEnabledStates            = SiCfgData->PchFivrExtV1p05RailEnabledStates;//0x1E;
    Fspscfg->PchFivrExtV1p05RailSupportedVoltageStates   = SiCfgData->PchFivrExtV1p05RailSupportedVoltageStates;//0x02;
    Fspscfg->PchFivrExtVnnRailEnabledStates              = SiCfgData->PchFivrExtVnnRailEnabledStates;//0x1E;
    Fspscfg->PchFivrExtVnnRailSupportedVoltageStates     = SiCfgData->PchFivrExtVnnRailSupportedVoltageStates;//0x0E;
    Fspscfg->PchFivrExtVnnRailSxEnabledStates            = SiCfgData->PchFivrExtVnnRailSxEnabledStates;//0x1C;

    DEBUG ((DEBUG_INFO, "------------------------------------------PCH_FIVR_CONFIG----------------------------------------\n"));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrVccinAuxLowToHighCurModeVolTranTime: 0x%x\n",SiCfgData->PchFivrVccinAuxLowToHighCurModeVolTranTime));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrVccinAuxRetToHighCurModeVolTranTime: 0x%x\n",SiCfgData->PchFivrVccinAuxRetToHighCurModeVolTranTime));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrVccinAuxRetToLowCurModeVolTranTime: 0x%x\n",SiCfgData->PchFivrVccinAuxRetToLowCurModeVolTranTime));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrVccinAuxOffToHighCurModeVolTranTime: 0x%x\n",SiCfgData->PchFivrVccinAuxOffToHighCurModeVolTranTime));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrDynPm: 0x%x\n",SiCfgData->PchFivrDynPm));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrExtVnnRailIccMaximum: 0x%x\n",SiCfgData->PchFivrExtVnnRailIccMaximum));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrExtV1p05RailIccMaximum: 0x%x\n",SiCfgData->PchFivrExtV1p05RailIccMaximum));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrExtVnnRailSxIccMaximum: 0x%x\n",SiCfgData->PchFivrExtVnnRailSxIccMaximum));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrExtV1p05RailCtrlRampTmr: 0x%x\n",SiCfgData->PchFivrExtV1p05RailCtrlRampTmr));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrExtV1p05RailEnabledStates: 0x%x\n",SiCfgData->PchFivrExtV1p05RailEnabledStates));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrExtV1p05RailSupportedVoltageStates: 0x%x\n",SiCfgData->PchFivrExtV1p05RailSupportedVoltageStates));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrExtVnnRailEnabledStates: 0x%x\n",SiCfgData->PchFivrExtVnnRailEnabledStates));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrExtVnnRailSupportedVoltageStates: 0x%x\n",SiCfgData->PchFivrExtVnnRailSupportedVoltageStates));
    DEBUG ((DEBUG_INFO, "SiCfgData->PchFivrExtVnnRailSxEnabledStates: 0x%x\n",SiCfgData->PchFivrExtVnnRailSxEnabledStates));
    DEBUG ((DEBUG_INFO, "------------------------------------------------END-----------------------------------------------------\n"));

    // PCH_PM_CONFIG
    Fspscfg->PchPmSlpS3MinAssert                         = SiCfgData->PchPmSlpS3MinAssert;
    Fspscfg->PchPmSlpS4MinAssert                         = SiCfgData->PchPmSlpS4MinAssert;
    Fspscfg->PchPmSlpSusMinAssert                        = SiCfgData->PchPmSlpSusMinAssert;
    Fspscfg->PchPmSlpAMinAssert                          = SiCfgData->PchPmSlpAMinAssert;
    Fspscfg->PchPmPwrBtnOverridePeriod                   = SiCfgData->PchPmPwrBtnOverridePeriod;
    Fspscfg->PchS0ixAutoDemotion                         = SiCfgData->PchS0ixAutoDemotion;
    Fspscfg->PmcLpmS0ixSubStateEnableMask                = SiCfgData->PmcLpmS0ixSubStateEnableMask;
    Fspscfg->PmcV1p05PhyExtFetControlEn                  = 1;
    Fspscfg->PmcV1p05IsExtFetControlEn                   = 1;
    Fspscfg->PmcModPhySusPgEnable = 0x1;
    Fspscfg->PchPwrOptEnable = 0x1;
    Fspscfg->PmcUsb2PhySusPgEnable      = 0x1;
    Fspscfg->PmcCpuC10GatePinEnable     = 0x1;
    Fspscfg->PchPmMeWakeSts             = 0x1;
    Fspscfg->PchPmWolOvrWkSts           = 0x1;

    Fspscfg->SerialIoUartRxPinMuxPolicy[0]               = GPIO_VER3_MUXING_SERIALIO_UART0_RXD_GPP_F1;
    Fspscfg->SerialIoUartTxPinMuxPolicy[0]               = GPIO_VER3_MUXING_SERIALIO_UART0_TXD_GPP_F2;
    Fspscfg->SerialIoUartRtsPinMuxPolicy[0]              = GPIO_VER3_MUXING_SERIALIO_UART0_RTS_GPP_F0;
    Fspscfg->SerialIoUartCtsPinMuxPolicy[0]              = GPIO_VER3_MUXING_SERIALIO_UART0_CTS_GPP_F3;

    // Check for TSN related sub-regions
    TsnMacAddrBase      = NULL;
    PseTsnIpConfigBase  = NULL;
    TsnConfigBase       = NULL;
    TsnMacAddrSize      = 0;
    PseTsnIpConfigSize  = 0;
    TsnConfigSize       = 0;

    Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'M', 'A', 'C'),
                            (VOID **)&TsnMacAddrBase, &TsnMacAddrSize);
    if (!EFI_ERROR (Status)) {
      if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
        TpmHashAndExtendPcrEventLog (1, (UINT8 *)TsnMacAddrBase, TsnMacAddrSize, EV_EFI_VARIABLE_DRIVER_CONFIG, sizeof("TMAC Config"), (UINT8 *)"TMAC Config");
      }
    }
    Fspscfg->TsnMacAddrBase      = (UINT32)(UINTN)TsnMacAddrBase;
    Fspscfg->TsnMacAddrSize      = TsnMacAddrSize;

    Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'S', 'I', 'P'),
                            (VOID **)&PseTsnIpConfigBase, &PseTsnIpConfigSize);
    if (!EFI_ERROR (Status)) {
      if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
        TpmHashAndExtendPcrEventLog (1, (UINT8 *)PseTsnIpConfigBase, PseTsnIpConfigSize, EV_EFI_VARIABLE_DRIVER_CONFIG, sizeof("TSIP Config"), (UINT8 *)"TSIP Config");
      }
    }
    Fspscfg->PseTsnIpConfigBase  = (UINT32)(UINTN)PseTsnIpConfigBase;
    Fspscfg->PseTsnIpConfigSize  = PseTsnIpConfigSize;

    Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'S', 'N', 'C'),
                            (VOID **)&TsnConfigBase, &TsnConfigSize);
    if (!EFI_ERROR (Status)) {
      if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
        TpmHashAndExtendPcrEventLog (1, (UINT8 *)TsnConfigBase, TsnConfigSize, EV_EFI_VARIABLE_DRIVER_CONFIG, sizeof("TSNC Config"), (UINT8 *)"TSNC Config");
      }
    }
    Fspscfg->TsnConfigBase       = (UINT32)(UINTN)TsnConfigBase;
    Fspscfg->TsnConfigSize       = TsnConfigSize;

    Fspscfg->EnableTimedGpio0   = (UINT8)SiCfgData->EnableTimedGpio0;
    Fspscfg->EnableTimedGpio1   = (UINT8)SiCfgData->EnableTimedGpio1;

    // PCH_TSN_CONFIG
    Fspscfg->PchTsnEnable         = SiCfgData->PchTsnEnable;
    Fspscfg->PchTsnGbeLinkSpeed   = SiCfgData->TsnLinkSpeed;
    Fspscfg->PchTsnGbeSgmiiEnable = (UINT8)SiCfgData->PchTsnGbeSgmiiEnable;

    // PSE_TSN_CONFIG
    Fspscfg->PseTsnGbeSgmiiEnable[0]         = (UINT8)SiCfgData->PseTsnGbe0SgmiiEnable;
    Fspscfg->PseTsnGbeSgmiiEnable[1]         = (UINT8)SiCfgData->PseTsnGbe1SgmiiEnable;
    Fspscfg->PseTsnGbePhyInterfaceType[0]    = (UINT8)SiCfgData->PseTsnGbe0PhyInterfaceType;
    Fspscfg->PseTsnGbePhyInterfaceType[1]    = (UINT8)SiCfgData->PseTsnGbe1PhyInterfaceType;
    DEBUG ((DEBUG_VERBOSE, "------------------------------------------PCH and PSE TSN CONFIG----------------------------------------\n"));
    DEBUG ((DEBUG_VERBOSE, "Fspscfg->TsnConfigBase: 0x%x\n",Fspscfg->TsnConfigBase));
    DEBUG ((DEBUG_VERBOSE, "Fspscfg->TsnConfigSize: 0x%x\n",Fspscfg->TsnConfigSize));
    DEBUG ((DEBUG_VERBOSE, "Fspscfg->PchTsnEnable: 0x%x\n",Fspscfg->PchTsnEnable));
    DEBUG ((DEBUG_VERBOSE, "Fspscfg->PchTsnGbeLinkSpeed: 0x%x\n",Fspscfg->PchTsnGbeLinkSpeed));
    DEBUG ((DEBUG_VERBOSE, "Fspscfg->PchTsnGbeSgmiiEnable: 0x%x\n",Fspscfg->PchTsnGbeSgmiiEnable));
    DEBUG ((DEBUG_VERBOSE, "Fspscfg->PseTsnGbeSgmiiEnable[0]: 0x%x\n",Fspscfg->PseTsnGbeSgmiiEnable[0]));
    DEBUG ((DEBUG_VERBOSE, "Fspscfg->PseTsnGbeSgmiiEnable[1]: 0x%x\n",Fspscfg->PseTsnGbeSgmiiEnable[1]));
    DEBUG ((DEBUG_VERBOSE, "Fspscfg->PseTsnGbePhyInterfaceType[0]: 0x%x\n",Fspscfg->PseTsnGbePhyInterfaceType[0]));
    DEBUG ((DEBUG_VERBOSE, "Fspscfg->PseTsnGbePhyInterfaceType[1]: 0x%x\n",Fspscfg->PseTsnGbePhyInterfaceType[1]));
    DEBUG ((DEBUG_VERBOSE, "------------------------------------------------END-----------------------------------------------------\n"));

    // AMT_ME_CONFIG
    Fspscfg->AmtEnabled           = SiCfgData->AmtEnabled;
    Fspscfg->FwProgress           = SiCfgData->FwProgress;
    Fspscfg->MeUnconfigOnRtcClear = 0x01;

    // PCH_INT_CONFIG
    Fspscfg->TcoIrqEnable      = SiCfgData->TcoIrqEnable;
    Fspscfg->GpioIrqRoute      = SiCfgData->GpioIrqRoute;
    Fspscfg->SciIrqSelect      = SiCfgData->SciIrqSelect;
    Fspscfg->TcoIrqSelect      = SiCfgData->TcoIrqSelect;

    // PCH_8254_CONFIG
    Fspscfg->Enable8254ClockGating      = SiCfgData->Enable8254ClockGating;
    Fspscfg->Enable8254ClockGatingOnS3  = SiCfgData->Enable8254ClockGatingOnS3;
    if (MemCfgData != NULL && MemCfgData->PchPseEnable) {
      FspUpdatePsePolicy(FspsUpd, SiCfgData);
    }
    // SA Post CONFIG
    Fspscfg->RenderStandby        = SiCfgData->RenderStandby;
    Fspscfg->PmSupport            = SiCfgData->PmSupport;
    Fspscfg->PavpEnable           = SiCfgData->PavpEnable;
    Fspscfg->CdClock              = SiCfgData->CdClock;
    Fspscfg->CdynmaxClampEnable   = SiCfgData->CdynmaxClampEnable;
    Fspscfg->GtFreqMax            = SiCfgData->GtFreqMax;
    Fspscfg->DisableTurboGt       = SiCfgData->DisableTurboGt;

    // Fusa Config
    Fspscfg->DisplayFusaConfigEnable       = 0x00;
    Fspscfg->GraphicFusaConfigEnable       = 0x00;
    Fspscfg->OpioFusaConfigEnable          = 0x00;
    Fspscfg->PsfFusaConfigEnable           = 0x00;

    //Pcie Rp Config
    for (Index = 0; Index < 7; Index++) {
       Fspscfg->PcieRpVc1TcMap[Index]  = 0x60;
     }
    for (Index = 0; Index < 7; Index++) {
       Fspscfg->PcieRpLtrMaxSnoopLatency[Index]  = 0x1003;
       Fspscfg->PcieRpLtrMaxNoSnoopLatency[Index]  = 0x1003;
     }

    for (Index = 0; Index < 4; Index++) {
       Fspscfg->SdCardGpioDataPadTermination[Index]  = 0x1F;
     }
    Fspscfg->SerialIoSpiCsEnable[1]=0x1;
    Fspscfg->SerialIoSpiCsEnable[3]=0x1;
    Fspscfg->SerialIoSpiCsEnable[5]=0x1;
    Fspscfg->FastPkgCRampDisable[0]=0x1;
    Fspscfg->SlowSlewRate[0]=0x1;

  } else {
    DEBUG ((DEBUG_ERROR, "SiCfgData not found!\n"));
    ASSERT (SiCfgData != NULL);
  } //end of SiCfgData

  if (mPchSciSupported) {
    Fspscfg->IsFusaSupported = 0x1;
    if (MemCfgData != NULL) {
        if (MemCfgData->IbeccErrorInj != 0x1){
            Fspscfg->IehMode = 0x1;
        } else {
            Fspscfg->IehMode = 0x0;
        }
    }
    //
    // PchPse*Enable UPDs should be set to to 0x2 for
    // host ownership; set to 1 for PSE ownership.
    //
    Fspscfg->PchUnlockGpioPads   = 0x0;
  }

  PowerCfgData = (POWER_CFG_DATA *) FindConfigDataByTag (CDATA_POWER_TAG);
  if (PowerCfgData == NULL) {
    DEBUG ((DEBUG_ERROR, "Missing power Cfg Data!\n"));
  } else {

  //CPU Power Management Basic Config
    Fspscfg->OneCoreRatioLimit             = PowerCfgData->OneCoreRatioLimit;
    Fspscfg->TwoCoreRatioLimit             = PowerCfgData->TwoCoreRatioLimit;
    Fspscfg->ThreeCoreRatioLimit           = PowerCfgData->ThreeCoreRatioLimit;
    Fspscfg->FourCoreRatioLimit            = PowerCfgData->FourCoreRatioLimit;
    Fspscfg->FiveCoreRatioLimit            = PowerCfgData->FiveCoreRatioLimit;
    Fspscfg->SixCoreRatioLimit             = PowerCfgData->SixCoreRatioLimit;
    Fspscfg->SevenCoreRatioLimit           = PowerCfgData->SevenCoreRatioLimit;
    Fspscfg->EightCoreRatioLimit           = PowerCfgData->EightCoreRatioLimit;
    Fspscfg->Hwp                           = PowerCfgData->Hwp;
    Fspscfg->HdcControl                    = PowerCfgData->HdcControl;
    Fspscfg->EnableItbm                    = PowerCfgData->EnableItbm;
    Fspscfg->EnableItbmDriver              = PowerCfgData->EnableItbmDriver;
    Fspscfg->PowerLimit2                   = PowerCfgData->PowerLimit2;
    Fspscfg->TurboPowerLimitLock           = PowerCfgData->TurboPowerLimitLock;
    Fspscfg->PowerLimit3DutyCycle          = PowerCfgData->PowerLimit3DutyCycle;
    Fspscfg->PowerLimit3Lock               = PowerCfgData->PowerLimit3Lock;
    Fspscfg->PowerLimit4Lock               = PowerCfgData->PowerLimit4Lock;
    Fspscfg->TccOffsetClamp                = PowerCfgData->TccOffsetClamp;
    Fspscfg->TccOffsetLock                 = PowerCfgData->TccOffsetLock;
    Fspscfg->TurboMode                     = PowerCfgData->TurboMode;
    Fspscfg->TccActivationOffset           = PowerCfgData->TccActivationOffset;
    Fspscfg->PowerLimit1                   = PowerCfgData->PowerLimit1;
    Fspscfg->PowerLimit2Power              = PowerCfgData->PowerLimit2Power;
    Fspscfg->PowerLimit3                   = PowerCfgData->PowerLimit3;
    Fspscfg->PowerLimit4                   = PowerCfgData->PowerLimit4;
    Fspscfg->PowerLimit1Time               = PowerCfgData->PowerLimit1Time;
    Fspscfg->PowerLimit3Time               = PowerCfgData->PowerLimit3Time;
    Fspscfg->TccOffsetTimeWindowForRatl    = PowerCfgData->TccOffsetTimeWindowForRatl;
    Fspscfg->HwpInterruptControl           = PowerCfgData->HwpInterruptControl;
    Fspscfg->EnablePerCorePState           = PowerCfgData->EnablePerCorePState;
    Fspscfg->EnableHwpAutoPerCorePstate    = PowerCfgData->EnableHwpAutoPerCorePstate;
    Fspscfg->EnableHwpAutoEppGrouping      = PowerCfgData->EnableHwpAutoEppGrouping;
    Fspscfg->EnableEpbPeciOverride         = PowerCfgData->EnableEpbPeciOverride;
    Fspscfg->EnableFastMsrHwpReq           = PowerCfgData->EnableFastMsrHwpReq;
    Fspscfg->MinRingRatioLimit             = PowerCfgData->MinRingRatioLimit;
    Fspscfg->MaxRingRatioLimit             = PowerCfgData->MaxRingRatioLimit;

    //CPU VR Power Management Custom Config
    CopyMem (Fspscfg->VrConfigEnable,      PowerCfgData->VrConfigEnable,        sizeof(PowerCfgData->VrConfigEnable));
    CopyMem (Fspscfg->TdcCurrentLimit,     PowerCfgData->TdcCurrentLimit,       sizeof(PowerCfgData->TdcCurrentLimit));
    CopyMem (Fspscfg->DcLoadline,          PowerCfgData->DcLoadline,            sizeof(PowerCfgData->DcLoadline));
    CopyMem (Fspscfg->AcLoadline,          PowerCfgData->AcLoadline,            sizeof(PowerCfgData->AcLoadline));
    CopyMem (Fspscfg->Psi1Threshold,       PowerCfgData->Psi1Threshold,         sizeof(PowerCfgData->Psi1Threshold));
    CopyMem (Fspscfg->Psi2Threshold,       PowerCfgData->Psi2Threshold,         sizeof(PowerCfgData->Psi2Threshold));
    CopyMem (Fspscfg->Psi3Threshold,       PowerCfgData->Psi3Threshold,         sizeof(PowerCfgData->Psi3Threshold));
    CopyMem (Fspscfg->Psi3Enable,          PowerCfgData->Psi3Enable,            sizeof(PowerCfgData->Psi3Enable));
    CopyMem (Fspscfg->Psi4Enable,          PowerCfgData->Psi4Enable,            sizeof(PowerCfgData->Psi4Enable));
    CopyMem (Fspscfg->ImonSlope,           PowerCfgData->ImonSlope,             sizeof(PowerCfgData->ImonSlope));
    CopyMem (Fspscfg->ImonOffset,          PowerCfgData->ImoniOffset,           sizeof(PowerCfgData->ImoniOffset));
    CopyMem (Fspscfg->IccMax,              PowerCfgData->IccMax,                sizeof(PowerCfgData->IccMax));
    //Removed due to fsp 2233
    //CopyMem (Fspscfg->VrVoltageLimit,      PowerCfgData->VrVoltageLimit,        sizeof(PowerCfgData->VrVoltageLimit));
    CopyMem (Fspscfg->TdcEnable,           PowerCfgData->TdcEnable,             sizeof(PowerCfgData->TdcEnable));
    CopyMem (Fspscfg->TdcTimeWindow,       PowerCfgData->TdcTimeWindow,         sizeof(PowerCfgData->TdcTimeWindow));

    Fspscfg->PsysPmax                      = PowerCfgData->PsysPmax;
    Fspscfg->AcousticNoiseMitigation       = 0x1;
    Fspscfg->FivrSpreadSpectrum            = 0xF;

    //CPU Power Management Custom Config
    Fspscfg->MaxRatio                      = PowerCfgData->MaxRatio;
    Fspscfg->Custom1PowerLimit1            = PowerCfgData->Custom1PowerLimit1;
    Fspscfg->Custom1PowerLimit2            = PowerCfgData->Custom1PowerLimit2;
    Fspscfg->Custom1PowerLimit1Time        = PowerCfgData->Custom1PowerLimit1Time;
    Fspscfg->Custom1TurboActivationRatio   = PowerCfgData->Custom1TurboActivationRatio;
    Fspscfg->Custom2PowerLimit1            = PowerCfgData->Custom2PowerLimit1;
    Fspscfg->Custom2PowerLimit2            = PowerCfgData->Custom2PowerLimit2;
    Fspscfg->Custom2PowerLimit1Time        = PowerCfgData->Custom2PowerLimit1Time;
    Fspscfg->Custom2TurboActivationRatio   = PowerCfgData->Custom2TurboActivationRatio;
    Fspscfg->Custom3PowerLimit1            = PowerCfgData->Custom3PowerLimit1;
    Fspscfg->Custom3PowerLimit2            = PowerCfgData->Custom3PowerLimit2;
    Fspscfg->Custom3PowerLimit1Time        = PowerCfgData->Custom3PowerLimit1Time;
    Fspscfg->Custom3TurboActivationRatio   = PowerCfgData->Custom3TurboActivationRatio;

    //CPU Power Management Psys Config
    Fspscfg->PsysPowerLimit1Power          = PowerCfgData->PsysPowerLimit1Power;
    Fspscfg->PsysPowerLimit2Power          = PowerCfgData->PsysPowerLimit2Power;
    Fspscfg->PsysPmax                      = PowerCfgData->PsysPmax;
    Fspscfg->PsysPowerLimit1               = PowerCfgData->PsysPowerLimit1;
    Fspscfg->PsysPowerLimit1Time           = PowerCfgData->PsysPowerLimit1Time;
    Fspscfg->PsysPowerLimit2               = PowerCfgData->PsysPowerLimit2;

    //CPU Power Management Test Config
    Fspscfg->Eist                          = PowerCfgData->Eist;
    Fspscfg->EnergyEfficientPState         = PowerCfgData->EnergyEfficientPState;
    Fspscfg->EnergyEfficientTurbo          = PowerCfgData->EnergyEfficientTurbo;
    Fspscfg->TStates                       = PowerCfgData->TStates;
    Fspscfg->BiProcHot                     = PowerCfgData->BiProcHot;
    Fspscfg->DisableProcHotOut             = PowerCfgData->DisableProcHotOut;
    Fspscfg->ProcHotResponse               = PowerCfgData->ProcHotResponse;
    Fspscfg->DisableVrThermalAlert         = PowerCfgData->DisableVrThermalAlert;
    Fspscfg->EnableAllThermalFunctions     = PowerCfgData->EnableAllThermalFunctions;
    Fspscfg->ThermalMonitor                = PowerCfgData->ThermalMonitor;
    Fspscfg->Cx                            = PowerCfgData->Cx;
    Fspscfg->PmgCstCfgCtrlLock             = PowerCfgData->PmgCstCfgCtrlLock;
    Fspscfg->C1e                           = PowerCfgData->C1e;
    Fspscfg->C1StateAutoDemotion           = PowerCfgData->C1StateAutoDemotion;
    Fspscfg->C1StateUnDemotion             = PowerCfgData->C1StateUnDemotion;
    Fspscfg->CStatePreWake                 = PowerCfgData->CStatePreWake;
    Fspscfg->TimedMwait                    = PowerCfgData->TimedMwait;
    Fspscfg->CstCfgCtrIoMwaitRedirection   = PowerCfgData->CstCfgCtrIoMwaitRedirection;
    Fspscfg->ProcHotLock                   = PowerCfgData->ProcHotLock;
    Fspscfg->ConfigTdpLevel                = PowerCfgData->ConfigTdpLevel;
    Fspscfg->RaceToHalt                    = PowerCfgData->RaceToHalt;
    Fspscfg->CstateLatencyControl1TimeUnit = PowerCfgData->CstateLatencyControl1TimeUnit;
    Fspscfg->CstateLatencyControl2TimeUnit = PowerCfgData->CstateLatencyControl2TimeUnit;
    Fspscfg->CstateLatencyControl3TimeUnit = PowerCfgData->CstateLatencyControl3TimeUnit;
    Fspscfg->CstateLatencyControl4TimeUnit = PowerCfgData->CstateLatencyControl4TimeUnit;
    Fspscfg->CstateLatencyControl5TimeUnit = PowerCfgData->CstateLatencyControl5TimeUnit;
    Fspscfg->PkgCStateLimit                = PowerCfgData->PkgCStateLimit;
    Fspscfg->CstateLatencyControl1Irtl     = PowerCfgData->CstateLatencyControl1Irtl;
    Fspscfg->CstateLatencyControl2Irtl     = PowerCfgData->CstateLatencyControl2Irtl;
    Fspscfg->CstateLatencyControl3Irtl     = PowerCfgData->CstateLatencyControl3Irtl;
    Fspscfg->CstateLatencyControl4Irtl     = PowerCfgData->CstateLatencyControl4Irtl;
    Fspscfg->CstateLatencyControl5Irtl     = PowerCfgData->CstateLatencyControl5Irtl;
  }

  if (mPchSciSupported) {
    DEBUG ((DEBUG_INFO, "Applying Fusa FSP UPD settings.........\n"));
    Fspscfg->Eist                          = 0;            // Intel Speed Step->EnableGv
    Fspscfg->Hwp                           = 0;            // Intel Speed Shift
    Fspscfg->Cx                            = 0;            // Intel C-states
    Fspscfg->EnableItbm                    = 0;            // Intel Turbo Boost Max Technology 3.0
    Fspscfg->AcSplitLock                   = 0;            // AC check on split locks
    Fspscfg->RenderStandby                 = 0;            // IGFX RenderStandby
    Fspscfg->DisplayFusaConfigEnable       = 1;            // Fusa Display Configuration
    Fspscfg->GraphicFusaConfigEnable       = 1;            // Fusa Graphics Configuration
    Fspscfg->OpioFusaConfigEnable          = 1;            // Fusa Opio Configuration
    Fspscfg->PsfFusaConfigEnable           = 1;            // Fusa Psf Configuration
    Fspscfg->DisableProcHotOut             = 0;            // Fusa Prochot output enable
    Fspscfg->BiProcHot                     = 0;            // Fusa Bi-direction Prochot diable

    for (Index = 0; Index < MaxPcieRootPorts; Index++) {
      Fspscfg->PcieRpAspm[Index]           = 0;
      Fspscfg->PcieRpL1Substates[Index]    = 0;
    }
    DEBUG ((DEBUG_INFO, "Fusa FSP UPD settings updated.........Done\n"));
  }

  if (FeaturePcdGet (PcdTccEnabled)) {
    TccModePostMemConfig (FspsUpd);
  }

  if (GetBootMode () == BOOT_ON_FLASH_UPDATE) {
    Fspscfg->PchLockDownBiosInterface = FALSE;
    Fspscfg->RtcBiosInterfaceLock     = FALSE;
    Fspscfg->PchSbAccessUnlock        = TRUE;
    Fspscfg->SkipMpInit               = TRUE;
    Fspscfg->EnableTcoTimer           = TRUE;
    DEBUG ((DEBUG_INFO, "Firmware update mode, unlock Bios setting\n"));
  }
  if (S0IX_STATUS() == 1) {
    // configure s0ix related FSP-S config
    Fspscfg->XdciEnable = 0;
  }
  // PCH_GPIO_PADS
  Fspscfg->PchUnlockGpioPads = (UINT8)SiCfgData->PchUnlockGpioPads;
}


/**
  This function will set the DISB - DRAM Initialization Scratch-pad Bit.
**/
STATIC
VOID
SetDramInitScratchpad (
  IN  VOID
  )
{
  MmioAndThenOr8 (
    PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_A + 2,
    (UINT8) ~((B_PMC_PWRM_GEN_PMCON_A_MS4V | B_PMC_PWRM_GEN_PMCON_A_SUS_PWR_FLR) >> 16),
    B_PMC_PWRM_GEN_PMCON_A_DISB >> 16
    );
}

/**
  Save MRC data onto the Flash region from NVS buffer.

  @param Buffer pointer to an NVS data
  @param Length Length of the NVS data(MRC)

  @retval EFI_SUCCESS
  @retval RETURN_ALREADY_STARTED
  @retval EFI_DEVICE_ERROR
  @retval EFI_INVALID_PARAMETER
**/
EFI_STATUS
EFIAPI
SaveNvsData (
  IN  VOID    *Buffer,
  IN  UINT32   Length
)
{
  EFI_STATUS  Status;
  UINT32      Address;
  UINT32      BaseAddress;
  UINT32      RegionSize;
  UINT32      MrcDataRegSize;

  Status = GetComponentInfo (FLASH_MAP_SIG_MRCDATA, &Address, &MrcDataRegSize);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  if (Length > MrcDataRegSize) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Compare input data against the stored MRC training data
  // if they match, no need to update again.
  //
  if (CompareMem ((VOID *)(UINTN)Address, Buffer, Length) == 0) {
    SetDramInitScratchpad ();
    return EFI_ALREADY_STARTED;
  }

  Status = SpiGetRegionAddress (FlashRegionBios, &BaseAddress, &RegionSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BaseAddress = ((UINT32)(~RegionSize) + 1);
  if (Address < BaseAddress) {
    return EFI_ACCESS_DENIED;
  }

  Address -= BaseAddress;
  if ((Address + ROUNDED_UP(Length, KB_(4))) > RegionSize) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (!EFI_ERROR(Status)) {
    Status = SpiFlashErase (FlashRegionBios, Address, ROUNDED_UP(Length, KB_(4)));
    if (!EFI_ERROR(Status)) {
      Status = SpiFlashWrite (FlashRegionBios, Address, Length, Buffer);
      if (!EFI_ERROR(Status)) {
        DEBUG ((DEBUG_INFO, "MRC data successfully cached to 0x%X\n", Address));
        SetDramInitScratchpad ();
      }
    }
  }

  return Status;
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
  SerialPortInfo->BaseAddr64 = GetSerialPortBase ();
  SerialPortInfo->BaseAddr   = (UINT32) SerialPortInfo->BaseAddr64;
  SerialPortInfo->RegWidth = GetSerialPortStrideSize ();
  if (GetDebugPort () >= PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    // IO Type
    SerialPortInfo->Type = 1;
  } else {
    // MMIO Type
    SerialPortInfo->Type = 2;
  }
}

/**
 Update the OS boot option

 @param OsBootOptionList pointer to boot option list.
 **/
VOID
EFIAPI
UpdateOsBootMediumInfo (
  OUT  OS_BOOT_OPTION_LIST  *OsBootOptionList
)
{
  OS_BOOT_OPTION             *BootOption;
  UINT32                      Idx;

  FillBootOptionListFromCfgData (OsBootOptionList);
  // Disable PreOS checker since the SKU doesn't support it
  if (!mPchSciSupported) {
    for (Idx = 0; Idx < OsBootOptionList->OsBootOptionCount; Idx++) {
      BootOption = &(OsBootOptionList->OsBootOption[Idx]);
      if ((BootOption->BootFlags & BOOT_FLAGS_PREOS) != 0) {
        BootOption->BootFlags &= ~BOOT_FLAGS_PREOS;
      }
    }
  }
}

/**
  Update loader platform info.

  @param[out] LoaderPlatformInfo pointer to platform info HOB

**/
VOID
UpdateLoaderPlatformInfo (
  OUT LOADER_PLATFORM_INFO   *LoaderPlatformInfo
)
{
  PLATFORM_DATA               *PlatformData;

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();

  if(PlatformData != NULL){
    LoaderPlatformInfo->HwState = PlatformData->BtGuardInfo.VerifiedBoot | (PlatformData->BtGuardInfo.MeasuredBoot << 1);
    LoaderPlatformInfo->Flags   = FLAGS_SPI_DISABLE_SMM_WRITE_PROTECT;

    if (PlatformData->BtGuardInfo.TpmType == dTpm20)
      LoaderPlatformInfo->TpmType = TPM_TYPE_DTPM20;
    else if (PlatformData->BtGuardInfo.TpmType == Ptt)
      LoaderPlatformInfo->TpmType = TPM_TYPE_PTT;
    else if (PlatformData->BtGuardInfo.TpmType == TpmNone)
      LoaderPlatformInfo->TpmType = TPM_TYPE_NONE;

    DEBUG ((DEBUG_INFO, "Stage2: HwState 0x%x TpmType 0x%x\n", LoaderPlatformInfo->HwState, LoaderPlatformInfo->TpmType));
  }
}

/**
 Update the frame buffer info by reading the PCI address.

 @param[out] GfxInfo pointer to global HOB data structure.
 **/
VOID
UpdateFrameBufferInfo (
  OUT  EFI_PEI_GRAPHICS_INFO_HOB   *GfxInfo
  )
{
  if (PcdGetBool (PcdIntelGfxEnabled)) {
    GfxInfo->FrameBufferBase = PciRead32 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0x18)) & 0xFFFFFF00;
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
  if (PcdGetBool (PcdIntelGfxEnabled)) {
    GfxDeviceInfo->BarIndex = 1;
    GfxDeviceInfo->VendorId = PciRead16 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0));
    GfxDeviceInfo->DeviceId = PciRead16 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 2));
  }
}


/**
 Update loader SMM info.

 @param[out] LdrSmmInfo     pointer to SMM information HOB

**/
VOID
UpdateSmmInfo (
  OUT  LDR_SMM_INFO           *LdrSmmInfo
)
{
  if (LdrSmmInfo == NULL) {
    return;
  }
  LdrSmmInfo->SmmBase = PcdGet32 (PcdSmramTsegBase);
  LdrSmmInfo->SmmSize = PcdGet32 (PcdSmramTsegSize);
  LdrSmmInfo->Flags = SMM_FLAGS_4KB_COMMUNICATION;
  DEBUG ((DEBUG_INFO, "SmmRamBase = 0x%x, SmmRamSize = 0x%x\n", LdrSmmInfo->SmmBase, LdrSmmInfo->SmmSize));

  //
  // Update smi ctrl register data
  //
  LdrSmmInfo->SmiCtrlReg.RegType    = (UINT8)REG_TYPE_IO;
  LdrSmmInfo->SmiCtrlReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiCtrlReg.SmiGblPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_GBL_SMI);
  LdrSmmInfo->SmiCtrlReg.SmiApmPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_APMC);
  LdrSmmInfo->SmiCtrlReg.SmiEosPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_EOS);
  LdrSmmInfo->SmiCtrlReg.Address    = (UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN);

  //
  // Update smi status register data
  //
  LdrSmmInfo->SmiStsReg.RegType    = (UINT8)REG_TYPE_IO;
  LdrSmmInfo->SmiStsReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiStsReg.SmiApmPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_STS_APM);
  LdrSmmInfo->SmiStsReg.Address    = (UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_STS );

  //
  // Update smi lock register data
  //
  LdrSmmInfo->SmiLockReg.RegType    = (UINT8)REG_TYPE_MMIO;
  LdrSmmInfo->SmiLockReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiLockReg.SmiLockPos = (UINT8)HighBitSet32 (B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK);
  LdrSmmInfo->SmiLockReg.Address    = (UINT32)(PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B);

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

  if (Guid == &gEfiGraphicsInfoHobGuid) {
    UpdateFrameBufferInfo (HobInfo);
  } else if (Guid == &gEfiGraphicsDeviceInfoHobGuid) {
    UpdateFrameBufferDeviceInfo (HobInfo);
  } else if (Guid == &gOsBootOptionGuid) {
    UpdateOsBootMediumInfo (HobInfo);
  } else if (Guid == &gLoaderPlatformInfoGuid) {
    UpdateLoaderPlatformInfo (HobInfo);
  } else if (Guid == &gSmmInformationGuid) {
    UpdateSmmInfo (HobInfo);
  } else if (Guid == &gLoaderSerialPortInfoGuid) {
    UpdateSerialPortInfo (HobInfo);
  }
}
