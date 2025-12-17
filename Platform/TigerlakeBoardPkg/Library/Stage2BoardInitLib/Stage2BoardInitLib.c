/** @file

  Copyright (c) 2008 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/GpioSiLib.h>
#include <GpioConfig.h>
#include <Library/SpiFlashLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/VariableLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/SmmInformationGuid.h>
#include <FspsUpd.h>
#include <GlobalNvsAreaDef.h>
#include <CpuNvsAreaDef.h>
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
#include <Library/DmarLib.h>
#include <Library/VTdLib.h>
#include <Library/MpInitLib.h>
#include <Library/HeciInitLib.h>
#include <Library/BoardSupportLib.h>
#include <MemInfoHob.h>
#include <Library/DebugDataLib.h>
#include <Library/IgdOpRegionLib.h>
#include <ConfigDataCommonStruct.h>
#include <Library/PciExpressLib.h>
#include "HdaVerbTable.h"
#include <Library/TpmLib.h>
#include <PlatformData.h>
#include <CpuDataStruct.h>
#include <CpuRegs.h>
#include <CpuPowerMgmt.h>
#include <CpuInitDataHob.h>
#include <PowerMgmtNvsStruct.h>
#include <Library/BdatLib.h>
#include <Library/SmbiosInitLib.h>
#include <IndustryStandard/SmBios.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <PchAccess.h>
#include <CpuRegsAccess.h>
#include <GpioTableTglHTsn.h>
#include <GpioTableTglUTsn.h>
#include <Library/ContainerLib.h>
#include <Library/S3SaveRestoreLib.h>
#include <TsnLib.h>
#include "CpuInit.h"
#include <CpuRegs.h>
#include <Library/MeExtMeasurementLib.h>
#include "PsdLib.h"
#include "FusaLib.h"
#include <PlatformBoardId.h>
#include <Lpit.h>
#include <LowPowerSupport.h>
#include <TccConfigSubRegions.h>
#include <Library/LocalApicLib.h>
#include <Library/TccLib.h>
#include <Library/WatchDogTimerLib.h>
#include "Dts.h"
#include "SerialIo.h"
#include <Library/PciePm.h>
#include <Library/PlatformInfo.h>
#include <Library/PlatformHookLib.h>
#include <Library/TxtLib.h>

//
// The EC implements an embedded controller interface at ports 0x60/0x64 and a ACPI compliant
// system management controller at ports 0x62/0x66. Port 0x66 is the command and status port,
// port 0x62 is the data port.
//
#define EC_C_PORT             0x66
//
// EC commands that are issued to the EC through the command port (0x66).
// New commands and command parameters should only be written by the host when IBF=0.
// Data read from the EC data port is valid only when OBF=1.
//
#define EC_C_ACPI_ENABLE      0xAA    // Enable ACPI mode

extern EFI_ACPI_DMAR_HEADER mAcpiDmarTableTemplate;
STATIC
CONST EFI_ACPI_COMMON_HEADER *mPlatformAcpiTables[] = {
  (EFI_ACPI_COMMON_HEADER *)&mAcpiDmarTableTemplate,
  NULL
};

/**
  Update the DMAR table

  @param[in, out] AcpiHeader         - The DMAR table header to update
**/
VOID
DmarTableUpdate (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER *AcpiHeader
  )
{
  EFI_STATUS                         Status;
  MEMORY_CFG_DATA                    *MemCfgData;
  SILICON_CFG_DATA                   *SiCfgData;
  UINT8                              Flags;
  UINT64                             BaseAddress;
  EFI_ACPI_DMAR_STRUCTURE_HEADER     *DmarHdr;
  UINT16                             IgdMode;
  UINT16                             GttMode;
  UINT32                             IgdMemSize;
  UINT32                             GttMemSize;
  UINT64                             RmrrLimit;

  Flags = 0;
  IgdMemSize = 0;
  GttMemSize = 0;

  // Set DMAR Flags based on config data
  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if (SiCfgData && SiCfgData->InterruptRemappingSupport) {
    Flags |= BIT0;
  }

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData) {
    if (MemCfgData->X2ApicOptOut) {
      Flags |= BIT1;
    }
    if (MemCfgData->DmaControlGuarantee) {
      Flags |= BIT2;
    }
  }

  // Initialize DMAR table header
  Status = AddAcpiDmarHdr (AcpiHeader, Flags);
  if (EFI_ERROR (Status)) {
    return;
  }

  // Add DRHD for VTD Engine 1
  BaseAddress = ReadVtdBaseAddress(0);
  if (BaseAddress != 0) {
    DmarHdr = AddDrhdHdr (AcpiHeader, 0, SIZE_4KB, 0, BaseAddress);
    if (DmarHdr != NULL) {
      AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 2, 0);
    }
  }

  // Add DRHD for VTD Engine 2
  BaseAddress = ReadVtdBaseAddress(1);
  if (BaseAddress != 0) {
    DmarHdr = AddDrhdHdr (AcpiHeader, 0, SIZE_4KB, 0, BaseAddress);
    if (DmarHdr != NULL) {
      AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 5, 0);
    }
  }

  // Add DRHD for VTd Engine 4 (iTBT PCIE 0)
  BaseAddress = ReadVtdBaseAddress(3);
  if (BaseAddress != 0) {
    DmarHdr = AddDrhdHdr (AcpiHeader, 0, SIZE_4KB, 0, BaseAddress);
    if (DmarHdr != NULL) {
      AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE, 0, 0, 0, 7, 0);
    }
  }

  // Add DRHD for VTD Engine 5 (iTBT PCIE 1)
  BaseAddress = ReadVtdBaseAddress(4);
  if (BaseAddress != 0) {
    DmarHdr = AddDrhdHdr (AcpiHeader, 0, SIZE_4KB, 0, BaseAddress);
    if (DmarHdr != NULL) {
      AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE, 0, 0, 0, 7, 1);
    }
  }

  // Add DRHD for VTD Engine 6 (iTBT PCIE 2)
  BaseAddress = ReadVtdBaseAddress(5);
  if (BaseAddress != 0) {
    DmarHdr = AddDrhdHdr (AcpiHeader, 0, SIZE_4KB, 0, BaseAddress);
    if (DmarHdr != NULL) {
      AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE, 0, 0, 0, 7, 2);
    }
  }

  // Add DRHD for VTD Engine 7 (iTBT PCIE 3)
  BaseAddress = ReadVtdBaseAddress(6);
  if (BaseAddress != 0) {
    DmarHdr = AddDrhdHdr (AcpiHeader, 0, SIZE_4KB, 0, BaseAddress);
    if (DmarHdr != NULL) {
      AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE, 0, 0, 0, 7, 3);
    }
  }

  // Add DRHD for VTd Engine 3
  BaseAddress = ReadVtdBaseAddress(2);
  if (BaseAddress != 0) {
  DmarHdr = AddDrhdHdr (AcpiHeader, 0, SIZE_4KB, 0, BaseAddress);
    if (DmarHdr != NULL) {
      // Add IOAPIC scope
      AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC, 0, 2, 0, 0x1E, 7);
      // Add HPET scope
      AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET, 0, 0, 0, 0x1E, 6);
    }
  }

  // Add RMRR for IGD
  BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
  IgdMode = ((PciRead16 ((UINTN)BaseAddress + R_SA_GGC) & B_SA_GGC_GMS_MASK) >> N_SA_GGC_GMS_OFFSET) & 0xFF;
  if (IgdMode < 0xF0) {
    IgdMemSize = IgdMode * 32 * 1024 * 1024;
  } else {
    IgdMemSize = 4 * (IgdMode - 0xF0 + 1) * 1024 * 1024;
  }
  GttMode = (PciRead16 ((UINTN)BaseAddress + R_SA_GGC) & B_SA_GGC_GGMS_MASK) >> N_SA_GGC_GGMS_OFFSET;
  if (GttMode <= V_SA_GGC_GGMS_8MB) {
    GttMemSize = (1 << GttMode) * 1024 * 1024;
  }
  BaseAddress = (PciRead32 ((UINTN)BaseAddress + R_SA_BGSM) & ~(0x01));
  RmrrLimit   = BaseAddress + IgdMemSize + GttMemSize - 1;
  DmarHdr     = AddRmrrHdr (AcpiHeader, 0, BaseAddress, RmrrLimit);
  if (DmarHdr != NULL) {
    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 2, 0);
  }

  // Calculate DMAR table checksum
  AcpiHeader->Checksum = CalculateCheckSum8 ((UINT8 *)AcpiHeader, AcpiHeader->Length);
}

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

CHAR8 *mBoardIdIndex[] = {
//"0123456789ABCDEF0123456789ABCDEF"       // Do not exceed 32 characters
  "TigerLake",
  "TigerLake U DDR4 SODIMM RVP",           // 0x01
  "TigerLake U LPDDR4/4x T4 RVP",          // 0x03
  "TigerLake H DDR4 SODIMM RVP",           // 0x21 or 0xF
  "Up Xtreme i11 DDR4 SODIMM",             // 0x04
};

//
// This table contains data on INTx and IRQ for PCH-LP
//
SI_PCH_DEVICE_INTERRUPT_CONFIG mPchLpDevIntConfig[] = {
//  {31, 0, SiPchNoInt, 0}, // LPC/eSPI Interface, doesn't use interrupts
//  {31, 1, SiPchNoInt, 0}, // P2SB, doesn't use interrupts
//  {31, 2, SiPchNoInt, 0}, // PMC , doesn't use interrupts
  {31, 3, SiPchIntA, 16}, // cAVS(Audio, Voice, Speach), INTA is default, programmed in PciCfgSpace 3Dh
  {31, 4, SiPchIntA, 16}, // SMBus Controller, no default value, programmed in PciCfgSpace 3Dh
//  {31, 5, SiPchNoInt, 0}, // SPI , doesn't use interrupts
  {31, 7, SiPchIntA, 16}, // TraceHub, INTA is default, RO register
  {30, 0, SiPchIntA, 16}, // SerialIo: UART #0 and TSN #0
  {30, 1, SiPchIntB, 17}, // SerialIo: UART #1 and TSN #1
  {30, 2, SiPchIntC, 36}, // SerialIo: SPI #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[10]
  {30, 3, SiPchIntD, 37}, // SerialIo: SPI #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[11]
//  {30, 6, SiPchIntA, 17}, // Reserved for HPET
//  {30, 7, SiPchIntB, 18}, // Reserved for IOAPIC
  {28, 0, SiPchIntA, 16}, // PCI Express Port 1, INT is default, programmed in PciCfgSpace + FCh
  {28, 1, SiPchIntB, 17}, // PCI Express Port 2, INT is default, programmed in PciCfgSpace + FCh
  {28, 2, SiPchIntC, 18}, // PCI Express Port 3, INT is default, programmed in PciCfgSpace + FCh
  {28, 3, SiPchIntD, 19}, // PCI Express Port 4, INT is default, programmed in PciCfgSpace + FCh
  {25, 2, SiPchIntC, 33}, // SerialIo UART Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[9]
//  {24, 0, 0, 0}, // Reserved (used by RST PCIe Storage Cycle Router)
  {23, 0, SiPchIntA, 16}, // SATA Controller, INTA is default, programmed in PciCfgSpace + 3Dh
  {22, 0, SiPchIntA, 16}, // CSME: HECI #1
  {22, 1, SiPchIntB, 17}, // CSME: HECI #2
  {22, 4, SiPchIntA, 16}, // CSME: HECI #3
  {22, 5, SiPchIntD, 19}, // CSME: HECI #4
//  {22, 7, SiPchIntA, 16}, // CSME: WLAN
  {21, 0, SiPchIntA, 27}, // SerialIo I2C Controller #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[1]
  {21, 1, SiPchIntB, 40}, // SerialIo I2C Controller #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[2]
                        //   Note: To avoid interrupt conflict with TPM reserved interrupt (28), changing SerialIo I2C #1 interrupt value to 40.
  {21, 2, SiPchIntC, 29}, // SerialIo I2C Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[3]
  {21, 3, SiPchIntD, 30}, // SerialIo I2C Controller #3, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[4]
//  {21, 7, SiPchIntA, 16}, // Northpeak Phantom (ACPI) Dunction - not exposed to OS
  {20, 0, SiPchIntA, 16}, // USB 3.0 xHCI Controller, no default value, programmed in PciCfgSpace 3Dh
  {20, 1, SiPchIntB, 17}, // USB Device Controller (OTG)
  {20, 3, SiPchIntA, 16}, // CNVi WiFi
  //{20, 2, SiPchNoInt, 0}, // Shared SRAM, no interrupts
//  {20, 4, 0, 0}, // TraceHub Phantom (ACPI) Function
//  {18, 2, SiPchNoInt, 0}, // CSME: PMT,  doesn't use interrupts
//  {18, 4, 0, 0}  // CSME: fTPM DMA
  {19, 0, SiPchIntA, 43}, // SerialIo: SPI #3
//  {19, 1, SiPchIntB, 24}, // SerialIo: SPI #4
  {18, 0, SiPchIntA, 16}, // Integrated Sensor Hub
  {18, 6, SiPchIntB, 34}, // SerialIo: SPI #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[12]
  {17, 0, SiPchIntA, 35}, // SerialIo: UART #3
  {17, 1, SiPchIntB, 20}, // SerialIo: UART #4
  {17, 2, SiPchIntC, 21}, // SerialIo: UART #5
  {17, 3, SiPchIntD, 42}, // SerialIo: UART #6
  {16, 0, SiPchIntC, 18}, // SerialIo: I2C #6
  {16, 1, SiPchIntD, 19}, // SerialIo: I2C #7
//  {16, 5, SiPchIntC, 18}, // IEH
  {16, 6, SiPchIntA, 23}, // THC #0
  {16, 7, SiPchIntB, 22}, // THC #1
  {31, 6, SiPchIntA, 16}, // GbE Controller, INTA is default, programmed in PciCfgSpace 3Dh
  {30, 4, SiPchIntA, 16}, // TSN Controller, INTA is default, programmed in PCR[TSN] + PCICFGCTRL[29]
  {29, 0, SiPchIntA, 16}, // PCI Express Port 9, INT is default, programmed in PciCfgSpace + FCh
  {29, 1, SiPchIntB, 17}, // PCI Express Port 10, INT is default, programmed in PciCfgSpace + FCh
  {29, 2, SiPchIntC, 18}, // PCI Express Port 11, INT is default, programmed in PciCfgSpace + FCh
  {29, 3, SiPchIntD, 19}, // PCI Express Port 12, INT is default, programmed in PciCfgSpace + FCh
  {28, 4, SiPchIntA, 16}, // PCI Express Port 5, INT is default, programmed in PciCfgSpace + FCh
  {28, 5, SiPchIntB, 17}, // PCI Express Port 6, INT is default, programmed in PciCfgSpace + FCh
  {28, 6, SiPchIntC, 18}, // PCI Express Port 7, INT is default, programmed in PciCfgSpace + FCh
  {28, 7, SiPchIntD, 19}, // PCI Express Port 8, INT is default, programmed in PciCfgSpace + FCh
  {25, 0, SiPchIntA, 31}, // SerialIo I2C Controller #4, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[5]
  {25, 1, SiPchIntB, 32}, // SerialIo I2C Controller #5, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[6]
  {22, 2, SiPchIntC, 18}, // CSME: IDE-Redirection (IDE-R)
  {22, 3, SiPchIntD, 19}, // CSME: Keyboard and Text (KT) Redirection
  {19, 1, SiPchIntB, 24}, // SerialIo: SPI #4
  {19, 2, SiPchIntC, 25}, // SerialIo: SPI #5 (LP)
  {19, 3, SiPchIntD, 38}, // SerialIo: SPI #6 (LP)
  //{16, 2, SiPchIntC, 18}, // BT CNVi - Not present
  //{16, 3, SiPchIntD, 19}  // ISH: FPAK - Not present
};


//
// This table contains data on INTx and IRQ for PCH-H
//
SI_PCH_DEVICE_INTERRUPT_CONFIG mPchHDevIntConfig[] = {
//  {31, 0, SiPchNoInt, 0}, // LPC/eSPI Interface, doesn't use interrupts
//  {31, 1, SiPchNoInt, 0}, // P2SB, doesn't use interrupts
//  {31, 2, SiPchNoInt, 0}, // PMC , doesn't use interrupts
  {31, 3, SiPchIntA, 16}, // cAVS(Audio, Voice, Speach), INTA is default, programmed in PciCfgSpace 3Dh
  {31, 4, SiPchIntA, 16}, // SMBus Controller, no default value, programmed in PciCfgSpace 3Dh
//  {31, 5, SiPchNoInt, 0}, // SPI , doesn't use interrupts
  {31, 7, SiPchIntA, 16}, // TraceHub, INTA is default, RO register
  {30, 0, SiPchIntA, 16}, // SerialIo: UART #0 and TSN #0
  {30, 1, SiPchIntB, 17}, // SerialIo: UART #1 and TSN #1
  {30, 2, SiPchIntC, 36}, // SerialIo: SPI #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[10]
  {30, 3, SiPchIntD, 37}, // SerialIo: SPI #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[11]
//  {30, 6, SiPchIntA, 17}, // Reserved for HPET
//  {30, 7, SiPchIntB, 18}, // Reserved for IOAPIC
  {28, 0, SiPchIntA, 16}, // PCI Express Port 1, INT is default, programmed in PciCfgSpace + FCh
  {28, 1, SiPchIntB, 17}, // PCI Express Port 2, INT is default, programmed in PciCfgSpace + FCh
  {28, 2, SiPchIntC, 18}, // PCI Express Port 3, INT is default, programmed in PciCfgSpace + FCh
  {28, 3, SiPchIntD, 19}, // PCI Express Port 4, INT is default, programmed in PciCfgSpace + FCh
  {25, 2, SiPchIntC, 33}, // SerialIo UART Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[9]
//  {24, 0, 0, 0}, // Reserved (used by RST PCIe Storage Cycle Router)
  {23, 0, SiPchIntA, 16}, // SATA Controller, INTA is default, programmed in PciCfgSpace + 3Dh
  {22, 0, SiPchIntA, 16}, // CSME: HECI #1
  {22, 1, SiPchIntB, 17}, // CSME: HECI #2
  {22, 4, SiPchIntA, 16}, // CSME: HECI #3
  {22, 5, SiPchIntD, 19}, // CSME: HECI #4
//  {22, 7, SiPchIntA, 16}, // CSME: WLAN
  {21, 0, SiPchIntA, 27}, // SerialIo I2C Controller #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[1]
  {21, 1, SiPchIntB, 40}, // SerialIo I2C Controller #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[2]
                        //   Note: To avoid interrupt conflict with TPM reserved interrupt (28), changing SerialIo I2C #1 interrupt value to 40.
  {21, 2, SiPchIntC, 29}, // SerialIo I2C Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[3]
  {21, 3, SiPchIntD, 30}, // SerialIo I2C Controller #3, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[4]
//  {21, 7, SiPchIntA, 16}, // Northpeak Phantom (ACPI) Dunction - not exposed to OS
  {20, 0, SiPchIntA, 16}, // USB 3.0 xHCI Controller, no default value, programmed in PciCfgSpace 3Dh
  {20, 1, SiPchIntB, 17}, // USB Device Controller (OTG)
  {20, 3, SiPchIntA, 16}, // CNVi WiFi
  //{20, 2, SiPchNoInt, 0}, // Shared SRAM, no interrupts
//  {20, 4, 0, 0}, // TraceHub Phantom (ACPI) Function
//  {18, 2, SiPchNoInt, 0}, // CSME: PMT,  doesn't use interrupts
//  {18, 4, 0, 0}  // CSME: fTPM DMA
  {19, 0, SiPchIntA, 43}, // SerialIo: SPI #3
//  {19, 1, SiPchIntB, 24}, // SerialIo: SPI #4
  {18, 0, SiPchIntA, 16}, // Integrated Sensor Hub
  {18, 6, SiPchIntB, 34}, // SerialIo: SPI #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[12]
  {17, 0, SiPchIntA, 35}, // SerialIo: UART #3
  {17, 1, SiPchIntB, 20}, // SerialIo: UART #4
  {17, 2, SiPchIntC, 21}, // SerialIo: UART #5
  {17, 3, SiPchIntD, 42}, // SerialIo: UART #6
  {16, 0, SiPchIntC, 18}, // SerialIo: I2C #6
  {16, 1, SiPchIntD, 19}, // SerialIo: I2C #7
//  {16, 5, SiPchIntC, 18}, // IEH
  {16, 6, SiPchIntA, 23}, // THC #0
  {16, 7, SiPchIntB, 22}, // THC #1
  {31, 6, SiPchIntA, 16}, // GbE Controller, INTA is default, programmed in PciCfgSpace 3Dh
  {30, 4, SiPchIntA, 16}, // TSN Controller, INTA is default, programmed in PCR[TSN] + PCICFGCTRL[29]
  {30, 5, SiPchIntB, 17}, // TSN Controller 2, INTA is default, programmed in PCR[TSN] + PCICFGCTRL[30]
  {29, 0, SiPchIntA, 16}, // PCI Express Port 9, INT is default, programmed in PciCfgSpace + FCh
  {29, 1, SiPchIntB, 17}, // PCI Express Port 10, INT is default, programmed in PciCfgSpace + FCh
  {29, 2, SiPchIntC, 18}, // PCI Express Port 11, INT is default, programmed in PciCfgSpace + FCh
  {29, 3, SiPchIntD, 19}, // PCI Express Port 12, INT is default, programmed in PciCfgSpace + FCh
  {29, 4, SiPchIntA, 16}, // PCI Express Port 13, INT is default, programmed in PciCfgSpace + FCh
  {29, 5, SiPchIntB, 17}, // PCI Express Port 14, INT is default, programmed in PciCfgSpace + FCh
  {29, 6, SiPchIntC, 18}, // PCI Express Port 15, INT is default, programmed in PciCfgSpace + FCh
  {29, 7, SiPchIntD, 19}, // PCI Express Port 16, INT is default, programmed in PciCfgSpace + FCh
  {28, 4, SiPchIntA, 16}, // PCI Express Port 5, INT is default, programmed in PciCfgSpace + FCh
  {28, 5, SiPchIntB, 17}, // PCI Express Port 6, INT is default, programmed in PciCfgSpace + FCh
  {28, 6, SiPchIntC, 18}, // PCI Express Port 7, INT is default, programmed in PciCfgSpace + FCh
  {28, 7, SiPchIntD, 19}, // PCI Express Port 8, INT is default, programmed in PciCfgSpace + FCh
  {27, 0, SiPchIntA, 16}, // PCI Express Port 17, INT is default, programmed in PciCfgSpace + FCh
  {27, 1, SiPchIntB, 17}, // PCI Express Port 18, INT is default, programmed in PciCfgSpace + FCh
  {27, 2, SiPchIntC, 18}, // PCI Express Port 19, INT is default, programmed in PciCfgSpace + FCh
  {27, 3, SiPchIntD, 19}, // PCI Express Port 20, INT is default, programmed in PciCfgSpace + FCh
  {27, 4, SiPchIntA, 16}, // PCI Express Port 21
  {27, 5, SiPchIntB, 17}, // PCI Express Port 22
  {27, 6, SiPchIntC, 18}, // PCI Express Port 23
  {27, 7, SiPchIntD, 19}, // PCI Express Port 24
  {25, 0, SiPchIntA, 31}, // SerialIo I2C Controller #4, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[5]
  {25, 1, SiPchIntB, 32}, // SerialIo I2C Controller #5, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[6]
  {22, 2, SiPchIntC, 18}, // CSME: IDE-Redirection (IDE-R)
  {22, 3, SiPchIntD, 19}, // CSME: Keyboard and Text (KT) Redirection
  {17, 4, SiPchIntA, 24}, // SerialIo: SPI #5
  {17, 5, SiPchIntB, 25}, // SerialIo: SPI #6
};

STATIC SMMBASE_INFO mSmmBaseInfo = {
  { BL_PLD_COMM_SIG, SMMBASE_INFO_COMM_ID, 0, 0 }
};

STATIC S3_SAVE_REG mS3SaveReg = {
  { BL_PLD_COMM_SIG, S3_SAVE_REG_COMM_ID, 1, 0 },
  { { REG_TYPE_IO, WIDE32, { 0, 0}, (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), 0x00000000 } }
};

VOID
EnableLegacyRegions (
  VOID
  );

/**
Return Cpu stepping type

@retval UINT8                   Cpu stepping type
**/
CPU_STEPPING
EFIAPI
GetCpuStepping(
  VOID
  )
{
  EFI_CPUID_REGISTER Cpuid;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid(CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  return ((CPU_STEPPING)(Cpuid.RegEax & CPUID_FULL_STEPPING));
}

/**
Return CPU Sku

@retval UINT8              CPU Sku
**/
UINT8
EFIAPI
GetCpuSku(
  VOID
  )
{
  UINT8              CpuType;
  UINT16             CpuDid;
  UINT32             CpuFamilyModel;
  EFI_CPUID_REGISTER Cpuid;
  BOOLEAN            SkuFound;

  SkuFound = TRUE;
  CpuType = EnumCpuUnknown;

  ///
  /// Read the CPUID & DID information
  ///
  AsmCpuid(CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  CpuFamilyModel = Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL;
  CpuDid = PciRead16((UINTN)(PCI_LIB_ADDRESS(SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0) + 0x02));

  switch (CpuFamilyModel) {

    case CPUID_FULL_FAMILY_MODEL_TIGERLAKE_ULT_ULX:
    switch (CpuDid) {
      case V_SA_DEVICE_ID_MB_ULT_1:    // ULT (4+2)
      case V_SA_DEVICE_ID_MB_ULT_2:    // ULT (2+2)
        CpuType = EnumCpuUlt;
        break;
      case V_SA_DEVICE_ID_MB_ULX_1:    // ULX (4+2)
      case V_SA_DEVICE_ID_MB_ULX_2:    // ULX (2+2)
        CpuType = EnumCpuUlx;
        break;
      case V_SA_DEVICE_ID_HALO_1:      // Halo (6+1)
      case V_SA_DEVICE_ID_HALO_2:      // Halo (8+2)
        CpuType = EnumCpuHalo;
        break;
      case V_SA_DEVICE_ID_DT_1:        // Desktop (6+1)
      case V_SA_DEVICE_ID_DT_2:        // Desktop (4+1)
        CpuType = EnumCpuTrad;
        break;
      default:
        SkuFound = FALSE;
        break;
    }
    break;
  }

  if (!SkuFound) {
    DEBUG((DEBUG_ERROR, "Unsupported CPU SKU, Device ID: 0x%02X, CPUID: 0x%08X!\n", CpuDid, CpuFamilyModel));
    ASSERT(FALSE);
  }

  return CpuType;
}

/**
  Create OS config data support HOB.

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
  MmioOr8 (SpiBaseAddress + R_SPI_CFG_BC, (UINT8) (B_SPI_CFG_BC_LE | B_SPI_CFG_BC_EISS));

  ClearFspHob ();
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
  UINT32                Pm1Sts;
  UINT16                Pm1Cnt;

  SmiEn = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
  if (((SmiEn & B_ACPI_IO_SMI_EN_GBL_SMI) !=0) && ((SmiEn & B_ACPI_IO_SMI_EN_EOS) !=0)) {
    return;
  }

  //
  // Clear the status before setting smi enable
  //
  SmiSts = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_STS));
  Pm1Sts = IoRead32 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS));
  Pm1Cnt = IoRead16 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT));

  // Clear RTC alarm and corresponding Pm1Sts only if wake-up source is RTC SMI#
  if (((Pm1Sts & B_ACPI_IO_PM1_STS_RTC_EN) != 0) &&
      ((Pm1Sts & B_ACPI_IO_PM1_STS_RTC) != 0) &&
      ((Pm1Cnt & B_ACPI_IO_PM1_CNT_SCI_EN) == 0)) {
    IoWrite8 (R_RTC_IO_INDEX, R_RTC_IO_REGC);
    (void)IoRead8 (R_RTC_IO_TARGET);
    Pm1Sts |= B_ACPI_IO_PM1_STS_RTC;
  }

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
      B_ACPI_IO_PM1_STS_PWRBTN |
      B_ACPI_IO_PM1_STS_GBL |
      B_ACPI_IO_PM1_STS_TMROF
      );

  IoWrite32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_STS), SmiSts);
  IoWrite16 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS), (UINT16) Pm1Sts);

  // Clear GPE0 STS in case some bits are set
  IoOr32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_STS_127_96), 0);
}

/**
  Update current boot Payload ID.

**/
VOID
UpdatePayloadId (
  VOID
  )
{
  EFI_STATUS         Status;
  UINT32             PayloadSelGpioData;
  UINT32             PayloadSelGpioPad;
  GEN_CFG_DATA      *GenCfgData;
  PLDSEL_CFG_DATA   *PldSelCfgData;

  GenCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenCfgData == NULL) {
    return;
  }
  SetPayloadId (GenCfgData->PayloadId);
  if (GetPayloadId () != AUTO_PAYLOAD_ID_SIGNATURE) {
    return;
  }

  PldSelCfgData = (PLDSEL_CFG_DATA *)FindConfigDataByTag (CDATA_PLDSEL_TAG);
  if ((PldSelCfgData != NULL) && (PldSelCfgData->PldSelGpio.Enable != 0)) {
    PayloadSelGpioPad = GpioGroupPinToPad (PldSelCfgData->PldSelGpio.PadGroup, PldSelCfgData->PldSelGpio.PinNumber);
    if (PayloadSelGpioPad == 0) {
      Status = EFI_ABORTED;
    } else {
      // Switch payloads based on configured GPIO pin
      Status = GpioGetInputValue (PayloadSelGpioPad, &PayloadSelGpioData);
    }

    if (!EFI_ERROR (Status)) {
      if (PayloadSelGpioData != PldSelCfgData->PldSelGpio.PinPolarity) {
        SetPayloadId (UEFI_PAYLOAD_ID_SIGNATURE);
        DEBUG ((DEBUG_INFO, "Update PayloadId to UEFI\n"));
      } else {
        SetPayloadId (0);
        DEBUG ((DEBUG_INFO, "Update PayloadId to OS Loader\n"));
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
  IGD_OP_PLATFORM_INFO      IgdPlatformInfo;
  EFI_STATUS                Status;

  Gnvs = (GLOBAL_NVS_AREA *)(UINTN)PcdGet32 (PcdAcpiGnvsAddress);

  IgdPlatformInfo.TurboIMON = Gnvs->SaNvs.GfxTurboIMON;

  Status = IgdOpRegionInit (&IgdPlatformInfo);
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
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE    InitPhase
)
{
  EFI_STATUS                Status;
  UINT32                    RgnBase;
  UINT32                    RgnSize;
  UINT32                    Length;
  UINT32                    TsegBase;
  UINT32                    TsegSize;
  EFI_PEI_GRAPHICS_INFO_HOB *FspGfxHob;
  VOID                      *FspHobList;
  SILICON_CFG_DATA          *SiCfgData;
  FEATURES_CFG_DATA         *FeaturesCfgData;
  UINTN                     LpcBase;
  BL_SW_SMI_INFO            *BlSwSmiInfo;

  switch (InitPhase) {
  case PreSiliconInit:
    DEBUG ((DEBUG_INFO, "PCH gen (0x%x), series (0x%x), stepping (0x%x)\n", PchGeneration(), PchSeries(), PchStepping()));
    DEBUG ((DEBUG_INFO, "LPC DeviceId (0x%x)\n", PchGetLpcDid()));

    if (FeaturePcdGet (PcdSmbiosEnabled) && FeaturePcdGet (PcdEnableDts)) {
      ReadPchDts ();
    }

    EnableLegacyRegions ();
    ConfigureGpio (CDATA_GPIO_TAG, 0, NULL);
    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      UpdatePayloadId ();
    }

    SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
    if (SiCfgData != NULL) {
      // Configure TSN GPIO table if TSN is enabled.
      if (SiCfgData->PchTsnEnable[0] || SiCfgData->PchTsnEnable[1]) {
        switch (GetPlatformId ()) {
          case BoardIdTglHDdr4SODimm:
          case 0x0F:
            ConfigureGpio (CDATA_NO_TAG, sizeof (mTglHTsnDeviceGpioTable) / sizeof (mTglHTsnDeviceGpioTable[0]), (UINT8*)mTglHTsnDeviceGpioTable);
            break;
          default:
          case BoardIdTglUDdr4:
          case BoardIdTglULp4Type4:
            ConfigureGpio (CDATA_NO_TAG, sizeof (mTglUTsnDeviceGpioTable) / sizeof (mTglUTsnDeviceGpioTable[0]), (UINT8*)mTglUTsnDeviceGpioTable);
            break;
        }
      }
    }

    SpiConstructor ();
    Status = GetComponentInfo (FLASH_MAP_SIG_VARIABLE, &RgnBase, &RgnSize);
    if (!EFI_ERROR(Status)) {
      VariableConstructor (RgnBase, RgnSize);
    }

    // Prepare platform ACPI tables
    Status = PcdSet32S (PcdAcpiTableTemplatePtr, (UINT32)(UINTN)mPlatformAcpiTables);
    break;
  case PostSiliconInit:
    if (IsWdtFlagsSet(WDT_FLAG_TCC_DSO_IN_PROGRESS)) {
      WdtDisable (WDT_FLAG_TCC_DSO_IN_PROGRESS);
    }
    // Set TSEG base/size PCD
    TsegBase = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + R_SA_TSEGMB) & ~0xF;
    TsegSize = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + R_SA_BGSM) & ~0xF;
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
        DEBUG ((DEBUG_INFO, "FspGfxHob->FrameBufferBase = 0x%lx\n", FspGfxHob->FrameBufferBase));
        PciWrite32 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0x1C), \
                   (UINT32)RShiftU64 (FspGfxHob->FrameBufferBase, 32));
        PciWrite32 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0x18), \
                   (UINT32)FspGfxHob->FrameBufferBase);
        PciWrite8 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_COMMAND_OFFSET), \
                   EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
      } else {
        DEBUG ((DEBUG_ERROR, "FspGfxHob is not available\n"));
      }
    }
    BuildOsConfigDataHob ();
    if (FeaturePcdGet (PcdFusaEnabled)) {
      FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
      if ((FeaturesCfgData != NULL) && (FeaturesCfgData->Features.FusaEnable == 1)) {
          FusaKeyOffProcessing();
      }
    }
    //
    // Override the Smbios default Info using SMBIOS binary blob
    //
    if (FeaturePcdGet (PcdSmbiosEnabled)) {
      LoadSmbiosStringsFromComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('S', 'M', 'B', 'S'));
      if (FeaturePcdGet (PcdEnableDts)) {
        ReadCpuDts ();
      }
    }
    break;
  case PrePciEnumeration:
    if (FeaturePcdGet (PcdVtdEnabled)) {
      Status = PcdSet32S (PcdAcpiTableTemplatePtr, (UINT32)(UINTN)mPlatformAcpiTables);
    }
    break;
  case PostPciEnumeration:
    if (FeaturePcdGet (PcdEnablePciePm)) {
      PciePmConfig ();
    }
    Status = SetFrameBufferWriteCombining (0, MAX_UINT32);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "Failed to set GFX framebuffer as WC\n"));
    }
    if (GetBootMode() == BOOT_ON_S3_RESUME) {
      ClearSmi ();
      RestoreS3RegInfo (FindS3Info (S3_SAVE_REG_COMM_ID));

      //
      // If payload registered a software SMI handler for bootloader to restore
      // SMRR base and mask in S3 resume path, trigger sw smi
      //
      BlSwSmiInfo = FindS3Info (BL_SW_SMI_COMM_ID);
      if (BlSwSmiInfo != NULL) {
        TriggerPayloadSwSmi (BlSwSmiInfo->BlSwSmiHandlerInput);
      }
    }

    ///
    /// Initialize TXT Before ACPI initialization.
    /// This will update the correct TXT enabled state in ACPI table.
    ///
    if (FeaturePcdGet (PcdTxtEnabled)) {
      FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
      if (FeaturesCfgData->Features.TxtEnabled == 1) {
          InitTxt();
      }
    }
    break;
  case PrePayloadLoading:
    if (FeaturePcdGet (PcdSmbiosEnabled) && FeaturePcdGet (PcdEnableDts)) {
      AppendSmbiosBootDts ();
    }
    IgdOpRegionPlatformInit ();

    ///
    /// Initialize the HECI device (for test HeciInitLib only)
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
        //
        // Set SMMBASE_INFO dummy strucutre in TSEG before others
        //
        mSmmBaseInfo.SmmBaseHdr.Count     = (UINT8) MpGetInfo()->CpuCount;
        mSmmBaseInfo.SmmBaseHdr.TotalSize = sizeof(BL_PLD_COMM_HDR) + mSmmBaseInfo.SmmBaseHdr.Count * sizeof(CPU_SMMBASE);
        Status = AppendS3Info ((VOID *)&mSmmBaseInfo, TRUE);
        //
        // Set REG_INFO struct in TSEG region except 'Val' for regs
        //
        mS3SaveReg.S3SaveHdr.TotalSize = sizeof(BL_PLD_COMM_HDR) + mS3SaveReg.S3SaveHdr.Count * sizeof(REG_INFO);
        AppendS3Info ((VOID *)&mS3SaveReg, FALSE);
      }
    }
    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() != 0)) {
      ProgramSecuritySetting ();
    }

    //
    // Enable decoding of I/O locations 62h and 66h to LPC
    //
    SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
    if ( (SiCfgData != NULL) && (SiCfgData->EcEnable == 1)){
      LpcBase = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, 0);
      MmioOr16 (LpcBase + R_LPC_CFG_IOE, B_LPC_CFG_IOE_ME1);

      //
      // Enable EC's ACPI mode to control power to motherboard during Sleep (S3)
      //
      IoWrite16 (EC_C_PORT, EC_C_ACPI_ENABLE);
    }

    break;

  case ReadyToBoot:
    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() == 0)) {
      ProgramSecuritySetting ();

      //Set SMI Lock (SMI_LOCK)
      DEBUG ((DEBUG_INFO, "Set SMI Lock\n"));
      MmioOr8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B, (UINT8)B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK);
    }

    break;
  default:
    break;
  }
}

CPU_FAMILY
EFIAPI
GetCpuFamily (
  VOID
  )
{
  EFI_CPUID_REGISTER Cpuid;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (0x1, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  return ((CPU_FAMILY) (Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL));
}

/**
  Get Maximum CPU Pcie Root Port Number

  @retval Maximum CPU Pcie Root Port Number
**/
UINT8
GetMaxCpuPciePortNum (
  VOID
  )
{
  switch (GetCpuFamily()) {
    case EnumCpuTglUltUlx:
      return TIGERLAKE_ULT_ULX_MAX_ROOT_PORT;
    case EnumCpuTglDtHalo:
      return TIGERLAKE_DT_HALO_MAX_ROOT_PORT;
    default:
      return TIGERLAKE_ULT_ULX_MAX_ROOT_PORT;
  }
}

/**
  Update PcieRp policies.

  @param[in] FspsUpd   Pointer to FSPS_UPD
**/
STATIC
VOID
FspUpdatePcieRpPolicy (
  IN FSPS_UPD                 *FspsUpd
  )
{
  UINT8             Index;
  UINT8             MaxPcieRootPorts;
  MaxPcieRootPorts = GetPchMaxPciePortNum ();
  for (Index = 0; Index < MaxPcieRootPorts; Index++) {
    FspsUpd->FspsConfig.PcieRpHotPlug[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpPmSci[Index]   = 0x1;
    FspsUpd->FspsConfig.PcieRpTransmitterHalfSwing[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpClkReqDetect[Index] = 0x1;
    FspsUpd->FspsConfig.PcieRpAdvancedErrorReporting[Index] = 0x1;
    FspsUpd->FspsConfig.PcieRpUnsupportedRequestReport[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpFatalErrorReport[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpNoFatalErrorReport[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpCorrectableErrorReport[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpSystemErrorOnFatalError[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpSystemErrorOnNonFatalError[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpSystemErrorOnCorrectableError[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpMaxPayload[Index] = 0x1;
    FspsUpd->FspsConfig.PcieRpPcieSpeed[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpPhysicalSlotNumber[Index] = Index;
    FspsUpd->FspsConfig.PcieRpSlotImplemented[Index] = 0x1;
    FspsUpd->FspsConfig.PcieRpCompletionTimeout[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpAcsEnabled[Index] = 0x1;
    FspsUpd->FspsConfig.PcieRpAspm[Index] = 0x4;
    FspsUpd->FspsConfig.PcieRpL1Substates[Index] = 0x2;
    FspsUpd->FspsConfig.PcieRpLtrEnable[Index] = 0x1;
    FspsUpd->FspsConfig.PcieRpLtrConfigLock[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpEnableCpm[Index] = 0x1;
    FspsUpd->FspsConfig.PcieRpDetectTimeoutMs[Index] = 0x0;
    FspsUpd->FspsConfig.PcieDpc[Index] = 0x1;
    FspsUpd->FspsConfig.PcieEdpc[Index] = 0x1;

    FspsUpd->FspsConfig.PcieRpLtrMaxSnoopLatency[Index] = 0x1003;
    FspsUpd->FspsConfig.PcieRpLtrMaxNoSnoopLatency[Index] = 0x1003;
    FspsUpd->FspsConfig.PcieRpSnoopLatencyOverrideMode[Index] = 0x2;
    FspsUpd->FspsConfig.PcieRpSnoopLatencyOverrideMultiplier[Index] = 0x2;
    FspsUpd->FspsConfig.PcieRpSnoopLatencyOverrideValue[Index] = 0x003C;
    FspsUpd->FspsConfig.PcieRpNonSnoopLatencyOverrideMode[Index] = 0x2;
    FspsUpd->FspsConfig.PcieRpNonSnoopLatencyOverrideMultiplier[Index] = 0x2;
    FspsUpd->FspsConfig.PcieRpNonSnoopLatencyOverrideValue[Index] = 0x003C;
    FspsUpd->FspsConfig.PcieRpSlotPowerLimitScale[Index] = 0x0;
    FspsUpd->FspsConfig.PcieRpSlotPowerLimitValue[Index] = 0x0;
  }
  FspsUpd->FspsConfig.PcieRpFunctionSwap = 0x1;
}

#if FixedPcdGetBool(PcdTccEnabled)
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
  UINT8  Index;
  UINT8  MaxPchPcieRootPorts;
  UINT8  MaxCpuPciePorts;

  DEBUG ((DEBUG_INFO, "Set TCC silicon:\n"));

  // TCC related Silicon settings
  FspsUpd->FspsConfig.Eist                      = 0;        // Intel Speed Step->EnableGv
  FspsUpd->FspsConfig.Hwp                       = 0;        // Intel Speed Shift
  FspsUpd->FspsConfig.Cx                        = 0;        // Intel C-states
  FspsUpd->FspsConfig.PsfTccEnable              = 1;        // Enable will decrease psf transaction latency by disabling some psf power management features
  FspsUpd->FspsConfig.PchDmiAspmCtrl            = 0;        // ASPM configuration on the PCH side of the DMI/OPI Link
  FspsUpd->FspsConfig.PchLegacyIoLowLatency     = 1;
  FspsUpd->FspsConfig.RenderStandby             = 0;        // IGFX RenderStandby
  FspsUpd->FspsConfig.CpuPcieClockGating        = 0;        // Describes whether PCI Express Clock Gating for each root port is enabled by platform modules
  FspsUpd->FspsConfig.PchTsnMultiVcEnable       = 1;
  FspsUpd->FspsConfig.OpioRecenter              = 0;

  MaxPchPcieRootPorts = GetPchMaxPciePortNum ();
  for (Index = 0; Index < MaxPchPcieRootPorts; Index++) {
    FspsUpd->FspsConfig.PcieRpAspm[Index]           = 0 ;   // The Pcie ASPM configuration of the root port
    FspsUpd->FspsConfig.PcieRpL1Substates[Index]    = 0;    // The Pcie L1 Substates configuration of the root port
    FspsUpd->FspsConfig.PciePtm[Index]              = 1;
  }

  MaxCpuPciePorts = GetMaxCpuPciePortNum();
  for (Index = 0; Index < MaxCpuPciePorts; Index++) {
    FspsUpd->FspsConfig.CpuPcieRpAspm[Index]           = 0; // Cpu Aspm
    FspsUpd->FspsConfig.CpuPcieRpL1Substates[Index]    = 0; // Cpu L1 substates
    FspsUpd->FspsConfig.CpuPcieRpPtmEnabled[Index]     = 1;
    FspsUpd->FspsConfig.CpuPcieRpVcEnabled[Index]      = 1;
    FspsUpd->FspsConfig.CpuPcieRpMultiVcEnabled[Index] = 1;
  }

  FspsUpd->FspsConfig.IfuEnable       = 0;

  return EFI_SUCCESS;
}
#endif

/**
  Update FSP-S UPD config data

  @param  FspsUpdPtr            The pointer to the FSP-S UPD to be updated.
**/
VOID
EFIAPI
UpdateFspConfig (
  VOID                *FspsUpdPtr
)
{
  UINT32              BaseAddress;
  UINT32              TotalSize;
  UINT32              SpiContainerBase;
  BOOLEAN             BiosProtected;
  BOOLEAN             ContainerProtected;
  EFI_STATUS          Status;
  FLASH_REGION_TYPE   RegionType;
  FSPS_UPD            *FspsUpd;
  SILICON_CFG_DATA    *SiCfgData;
  FSP_S_CONFIG        *FspsConfig;
  UINT8               Index;
  UINT8               PrIndex;
  UINT32              SmiEn;
  UINT8               CsIndex;
  UINT32              *TsnMacAddrBase;
  UINT32              TsnMacAddrSize;
  UINT32              Address;
  UINT32              VarSize;
  TSN_MAC_ADDR_SUB_REGION *TsnSubRegion;
  UINT8                MaxPcieRootPorts;
  UINT32              *HdaVerbTablePtr;
  UINT8                HdaVerbTableNum;
  FSP_INFO_HEADER      *FspHeader;
  UINT32               FspsBase;
  FEATURES_CFG_DATA    *FeaturesCfgData;


  FspsBase = PcdGet32 (PcdFSPSBase);
  FspHeader = (FSP_INFO_HEADER *)(UINTN)(FspsBase + FSP_INFO_HEADER_OFF);
  // TGL FSP doesn't correctly support MultiPhase Si Init.
  FspHeader->FspMultiPhaseSiInitEntryOffset = 0;

  FspsUpd    = (FSPS_UPD *)FspsUpdPtr;
  FspsConfig = &FspsUpd->FspsConfig;

  // SGX is not supported on this platform. Do not change these two lines.
  FspsConfig->SgxEpoch0           = 0x553DFD8D5FA48F27;
  FspsConfig->SgxEpoch1           = 0xD76767B9BE4BFDC1;

  // Update serial io
  SerialIoPostMemConfig (FspsConfig);

  //
  // Update device interrupt table
  //
  if (IsPchLp ()) {
    DEBUG ((DEBUG_INFO, "PchLp is detected\n"));
    FspsConfig->DevIntConfigPtr   = (UINT32)(UINTN)mPchLpDevIntConfig;
    FspsConfig->NumOfDevIntConfig = sizeof (mPchLpDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);
  } else if (IsPchH ()) {
    DEBUG ((DEBUG_INFO, "PchH is detected\n"));
    FspsConfig->DevIntConfigPtr   = (UINT32)(UINTN)mPchHDevIntConfig;
    FspsConfig->NumOfDevIntConfig = sizeof (mPchHDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);
  } else {
    DEBUG ((DEBUG_ERROR, "Unsupported PCH.\n"));
  }

  if (GetBootMode () != BOOT_ON_FLASH_UPDATE) {
    BiosProtected = FALSE;
    ContainerProtected  = FALSE;
    PrIndex = 0;
    Status = SpiGetRegionAddress (FlashRegionBios, &BaseAddress, &TotalSize);
    if (!EFI_ERROR (Status)) {
      BiosProtected = TRUE;
      Status = GetComponentInfo (FLASH_MAP_SIG_UEFIVARIABLE, &Address, &VarSize);
      if (!EFI_ERROR (Status)) {
        //
        // Protect the BIOS region except for the UEFI variable region
        //
        Address -= ((UINT32)(~TotalSize) + 1);

        FspsConfig->PchWriteProtectionEnable[PrIndex] = TRUE;
        FspsConfig->PchReadProtectionEnable[PrIndex]  = FALSE;
        FspsConfig->PchProtectedRangeBase[PrIndex]    = (UINT16) (BaseAddress >> 12);
        FspsConfig->PchProtectedRangeLimit[PrIndex]   = (UINT16) ((BaseAddress + Address - 1) >> 12);
        PrIndex++;

        FspsConfig->PchWriteProtectionEnable[PrIndex] = TRUE;
        FspsConfig->PchReadProtectionEnable[PrIndex]  = FALSE;
        FspsConfig->PchProtectedRangeBase[PrIndex]    = (UINT16) ((BaseAddress + Address + VarSize) >> 12);
        FspsConfig->PchProtectedRangeLimit[PrIndex]   = (UINT16) ((BaseAddress + TotalSize - 1) >> 12);
        PrIndex++;
      } else {
        //
        // Protect the whole BIOS region
        //
        FspsConfig->PchWriteProtectionEnable[PrIndex] = TRUE;
        FspsConfig->PchReadProtectionEnable[PrIndex]  = FALSE;
        FspsConfig->PchProtectedRangeBase[PrIndex]    = (UINT16) (BaseAddress >> 12);
        FspsConfig->PchProtectedRangeLimit[PrIndex]   = (UINT16) ((BaseAddress + TotalSize - 1) >> 12);
        PrIndex++;
      }

      RegionType = (FLASH_REGION_TYPE) PcdGet32 (PcdSpiContainerImageRegionType);
      if (RegionType < FlashRegionMax) {
        if (RegionType != FlashRegionBios) {
          Status = GetComponentInfo (FLASH_MAP_SIG_SPI_CONTAINER1, &SpiContainerBase, NULL);
          if (!EFI_ERROR (Status)) {
            SpiContainerBase &= 0xFFFF;
            Status = SpiGetRegionAddress (RegionType, &BaseAddress, &TotalSize);
            if (!EFI_ERROR (Status)) {
              if ((PcdGet32 (PcdSpiContainerImage1RegionSize) + PcdGet32 (PcdSpiContainerImage2RegionSize)) <= TotalSize) {
                ContainerProtected = TRUE;
                FspsConfig->PchWriteProtectionEnable[PrIndex] = TRUE;
                FspsConfig->PchReadProtectionEnable[PrIndex]  = FALSE;
                FspsConfig->PchProtectedRangeBase[PrIndex]    = (UINT16) ((BaseAddress + SpiContainerBase) >> 12);
                FspsConfig->PchProtectedRangeLimit[PrIndex]   = (UINT16) ((BaseAddress + SpiContainerBase +
                                                            PcdGet32 (PcdSpiContainerImage1RegionSize) +
                                                            PcdGet32 (PcdSpiContainerImage2RegionSize) - 1) >> 12);
                PrIndex++;
              }
            }
          }
        } else {
          ContainerProtected = TRUE;
        }
      }
    }
    DEBUG (((BiosProtected) ? DEBUG_INFO : DEBUG_WARN, "BIOS SPI region will %a protected\n", (BiosProtected) ? "be" : "NOT BE"));
    DEBUG (((ContainerProtected)  ? DEBUG_INFO : DEBUG_WARN, "CONTAINER  SPI region will %a protected\n", (ContainerProtected)  ? "be" : "NOT BE"));
  }
  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if ((SiCfgData != NULL) && SiCfgData->PchHdaEnable == 1) {
    HdaVerbTablePtr = (UINT32 *) AllocateZeroPool (4 * sizeof (UINT32));
    if (HdaVerbTablePtr != NULL) {
      HdaVerbTableNum = 0;
      HdaVerbTablePtr[HdaVerbTableNum++]   = (UINT32)(UINTN) &HdaVerbTableDisplayAudio;
      HdaVerbTablePtr[HdaVerbTableNum++]   = (UINT32)(UINTN) &TglHdaVerbTableAlc711;
      HdaVerbTablePtr[HdaVerbTableNum++]   = (UINT32)(UINTN) &TglHdaVerbTableAlc701;
      HdaVerbTablePtr[HdaVerbTableNum++]   = (UINT32)(UINTN) &TglHdaVerbTableAlc274;
      FspsUpd->FspsConfig.PchHdaVerbTablePtr      = (UINT32)(UINTN) HdaVerbTablePtr;
      FspsUpd->FspsConfig.PchHdaVerbTableEntryNum = HdaVerbTableNum;
    } else {
      DEBUG ((DEBUG_ERROR, "UpdateFspConfig Error: Could not allocate Memory for HdaVerbTable\n"));
    }
  }
  if (PcdGetBool (PcdFramebufferInitEnabled) && (GetBootMode() != BOOT_ON_S3_RESUME)) {
    FspsConfig->GraphicsConfigPtr = (UINT32) GetVbtAddress();
    FspsConfig->PeiGraphicsPeimInit = 1;
  } else {
    FspsConfig->GraphicsConfigPtr = 0;
    FspsConfig->PeiGraphicsPeimInit = 0;
  }

  // Force Eiss and BiosLock off for now.
  // Enable it later in OS loader/EndofStages
  FspsConfig->PchLockDownBiosLock = 0x0;

  // Enable IEH
  FspsConfig->IehMode = 0x1;

  if (FeaturePcdGet (PcdTxtEnabled)) {
    FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
    if (FeaturesCfgData->Features.TxtEnabled == 1) {
      DEBUG((DEBUG_INFO, "Enabling TXT in FSP-S UPD's\n"));
      FspsConfig->TxtEnable = 0x1;
    }
 }

  FspsConfig->SerialIoSpiMode[1] = 0x1;
  for (Index = 0; Index < GetPchMaxSerialIoSpiControllersNum (); Index++) {
    for (CsIndex = 0; CsIndex < PCH_MAX_SERIALIO_SPI_CHIP_SELECTS; CsIndex++) {
      FspsConfig->SerialIoSpiCsPolarity[Index * PCH_MAX_SERIALIO_SPI_CHIP_SELECTS + CsIndex] = 0x1;
    }
  }

  FspUpdatePcieRpPolicy(FspsUpd);

  // Required for RVP board
  FspsConfig->Usb2OverCurrentPin[2] = 0xff;

  // Required for sata ports to work
  FspsConfig->SataPortsEnable[0] = 0x1;
  FspsConfig->SataPortsEnable[1] = 0x1;

  if (IsPchH ()) {
    FspsConfig->SataPortsEnable[4] = 0x1;
  }

  if( GetPayloadId() == 0) {
    // Disable SMI sources
    SmiEn = IoRead32((UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
    IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), SmiEn & (UINT32) (~B_ACPI_IO_SMI_EN_GBL_SMI));

    // Lock down SMI
    FspsConfig->PchLockDownGlobalSmi = 0x1;
  }

  // disable VMD since it caused UEFI payload hang.
  // hang details:
  //LocatePciExpressCapabilityRegBlock()
  //  read IsPciExp (offset 0x260 in PCI_IO_DEVICE) 1
  // but read all FFs after 0x100 in its PCI config space.
  // which causing deadloop when trying to read CapId.
  FspsConfig->VmdEnable = 0;

  //  Following UPDs are related to TCC . But all of these are also generic features that can be changed regardless of TCC feature.
  if (SiCfgData != NULL) {
    FspsUpd->FspsConfig.Eist                      = SiCfgData->Eist;          // Intel Speed Step
    FspsUpd->FspsConfig.Hwp                       = SiCfgData->Hwp;           // Intel Speed Shift
    FspsUpd->FspsConfig.Cx                        = SiCfgData->Cx;            // Intel C-states
    FspsUpd->FspsConfig.EnableItbm                = SiCfgData->EnableItbm;    // Intel Turbo Boost Max Technology 3.0
    FspsUpd->FspsConfig.AcSplitLock               = SiCfgData->AcSplitLock;   // AC check on split locks
    FspsUpd->FspsConfig.PsfTccEnable              = SiCfgData->PsfTccEnable;  // Enable will decrease psf transaction latency by disabling some psf power management features
    FspsUpd->FspsConfig.PchDmiAspmCtrl            = SiCfgData->PchDmiAspmCtrl;// ASPM configuration on the PCH side of the DMI/OPI Link
    FspsUpd->FspsConfig.PchLegacyIoLowLatency     = SiCfgData->PchLegacyIoLowLatency;

    MaxPcieRootPorts = GetPchMaxPciePortNum ();
    for (Index = 0; Index < MaxPcieRootPorts; Index++) {
      FspsUpd->FspsConfig.PcieRpAspm[Index]         = SiCfgData->PcieRpAspm[Index] ;       // The Pcie ASPM configuration of the root port
      FspsUpd->FspsConfig.PcieRpL1Substates[Index]  = SiCfgData->PcieRpL1Substates[Index]; // The Pcie L1 Substates configuration of the root port
      FspsUpd->FspsConfig.PciePtm[Index]            = SiCfgData->PciePtm[Index];
    }

    FspsUpd->FspsConfig.RenderStandby             = SiCfgData->RenderStandby; // IGFX RenderStandby
    FspsUpd->FspsConfig.PmSupport                 = SiCfgData->PmSupport;     // IGFX PmSupport
    FspsUpd->FspsConfig.CpuPcieClockGating = SiCfgData->CpuPcieClockGating;   // Describes whether PCI Express Clock Gating for each root port is enabled by platform modules
    for (Index = 0; Index < GetMaxCpuPciePortNum(); Index++) {
      FspsUpd->FspsConfig.CpuPcieRpAspm[Index]           = SiCfgData->CpuPcieRpAspm[Index];        // Cpu Aspm
      FspsUpd->FspsConfig.CpuPcieRpL1Substates[Index]    = SiCfgData->CpuPcieRpL1Substates[Index]; // Cpu L1 substates
      FspsUpd->FspsConfig.CpuPcieRpPtmEnabled[Index]     = SiCfgData->CpuPcieRpPtmEnabled[Index] ;
      FspsUpd->FspsConfig.CpuPcieRpVcEnabled[Index]      = SiCfgData->CpuPcieRpVcEnabled[Index] ;
      FspsUpd->FspsConfig.CpuPcieRpMultiVcEnabled[Index] = SiCfgData->CpuPcieRpMultiVcEnabled[Index];
      FspsUpd->FspsConfig.CpuPcieRpLtrEnable[Index]      = 1;
      FspsUpd->FspsConfig.CpuPcieRpLtrMaxSnoopLatency[Index]      = 0x1003;
      FspsUpd->FspsConfig.CpuPcieRpLtrMaxNoSnoopLatency[Index]    = 0x1003;
      FspsUpd->FspsConfig.CpuPcieRpPmSci[Index]          = 1;
      FspsUpd->FspsConfig.CpuPcieRpMaxPayload[Index]     = 0x1;
    }
    FspsUpd->FspsConfig.CpuPcieRpFunctionSwap            = 0x1;
    FspsUpd->FspsConfig.CpuPcieSetSecuredRegisterLock    = 0x1;

    // TSN feature support
    TsnMacAddrBase      = NULL;
    TsnMacAddrSize      = 0;

    CopyMem (FspsConfig->PchTsnEnable, SiCfgData->PchTsnEnable, sizeof(SiCfgData->PchTsnEnable));
    FspsConfig->PchTsnLinkSpeed = SiCfgData->PchTsnLinkSpeed;

    if (SiCfgData->PchTsnEnable[0] || SiCfgData->PchTsnEnable[1]) {
      FspsConfig->PchTsnMultiVcEnable = SiCfgData->PchTsnMultiVcEnable;
      Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'M', 'A', 'C'),
                              (VOID **)&TsnMacAddrBase, &TsnMacAddrSize);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to load TSN MAC subregion %r\n", Status));
      } else {
        TsnSubRegion = (TSN_MAC_ADDR_SUB_REGION*) TsnMacAddrBase;

        FspsConfig->PchTsn0MacAddressHigh = TsnSubRegion->Config.Port[0].MacAddr.U32MacAddr[1];
        FspsConfig->PchTsn0MacAddressLow  = TsnSubRegion->Config.Port[0].MacAddr.U32MacAddr[0];
        FspsConfig->PchTsn1MacAddressHigh = TsnSubRegion->Config.Port[1].MacAddr.U32MacAddr[1];
        FspsConfig->PchTsn1MacAddressLow  = TsnSubRegion->Config.Port[1].MacAddr.U32MacAddr[0];

        DEBUG ((DEBUG_INFO, "Load TSN MAC subregion @0x%p, size = 0x%x\n", TsnMacAddrBase, TsnMacAddrSize));
      }
    }
    for (Index = 0; Index < (sizeof (FspsConfig->PortUsb20Enable) / sizeof (FspsConfig->PortUsb20Enable[0])); Index++) {
      FspsConfig->PortUsb20Enable[Index] = ((*((UINT32*) (&SiCfgData->PortUsb20Enable))) >> Index) & 0x1;
    }
  }

  FspsConfig->PortUsb30Enable[0] = 0x1;
  FspsConfig->PortUsb30Enable[1] = 0x1;
  FspsConfig->PortUsb30Enable[2] = 0x1;
  FspsConfig->PortUsb30Enable[3] = 0x1;

  if (IsPchH ()) {
    FspsConfig->PortUsb30Enable[4] = 0x1;
    FspsConfig->PortUsb30Enable[5] = 0x1;
    FspsConfig->PortUsb30Enable[6] = 0x1;
    FspsConfig->PortUsb30Enable[7] = 0x1;
    FspsConfig->PortUsb30Enable[8] = 0x1;
    FspsConfig->PortUsb30Enable[9] = 0x1;
    FspsConfig->CpuUsb3OverCurrentPin[0] = 0x0;
    FspsConfig->CpuUsb3OverCurrentPin[1] = 0x1;
    FspsConfig->CpuUsb3OverCurrentPin[2] = 0x2;
    FspsConfig->CpuUsb3OverCurrentPin[3] = 0x3;
    if ((SiCfgData != NULL) && (SiCfgData->PchTsnEnable[0] || SiCfgData->PchTsnEnable[1])) {
      FspsConfig->Usb2OverCurrentPin[1] = 0xff;
      FspsConfig->Usb2OverCurrentPin[4] = 0xff;
      FspsConfig->Usb3OverCurrentPin[1] = 0xff;
      FspsConfig->Usb3OverCurrentPin[4] = 0xff;
    } else {
      FspsConfig->Usb2OverCurrentPin[1] = 0x3;
      FspsConfig->Usb2OverCurrentPin[4] = 0x2;
      FspsConfig->Usb3OverCurrentPin[1] = 0x3;
      FspsConfig->Usb3OverCurrentPin[4] = 0x2;
    }
    FspsConfig->Usb2OverCurrentPin[0] = 0x0;
    FspsConfig->Usb2OverCurrentPin[2] = 0x0;
    FspsConfig->Usb2OverCurrentPin[3] = 0x1;
    FspsConfig->Usb2OverCurrentPin[5] = 0xff;
    FspsConfig->Usb2OverCurrentPin[6] = 0xff;
    FspsConfig->Usb2OverCurrentPin[7] = 0x8;
    FspsConfig->Usb2OverCurrentPin[8] = 0x9;
    FspsConfig->Usb2OverCurrentPin[9] = 0xa;
    FspsConfig->Usb2OverCurrentPin[10] = 0xb;
    FspsConfig->Usb2OverCurrentPin[11] = 0x4;
    FspsConfig->Usb2OverCurrentPin[12] = 0x4;
    FspsConfig->Usb2OverCurrentPin[13] = 0xff;
    FspsConfig->Usb3OverCurrentPin[2] = 0x0;
    FspsConfig->Usb3OverCurrentPin[3] = 0x1;
    FspsConfig->Usb3OverCurrentPin[5] = 0xff;
    FspsConfig->Usb3OverCurrentPin[6] = 0xff;
    FspsConfig->Usb3OverCurrentPin[7] = 0xff;
    FspsConfig->Usb3OverCurrentPin[8] = 0xff;
    FspsConfig->Usb3OverCurrentPin[9] = 0xff;
  } else {
    FspsConfig->CpuUsb3OverCurrentPin[0] = 0x0;
    FspsConfig->CpuUsb3OverCurrentPin[1] = 0x0;
    FspsConfig->CpuUsb3OverCurrentPin[2] = 0x3;
    FspsConfig->CpuUsb3OverCurrentPin[3] = 0x3;
    FspsConfig->Usb2OverCurrentPin[1] = 0xff;
    FspsConfig->Usb2OverCurrentPin[3] = 0x0;
    FspsConfig->Usb2OverCurrentPin[4] = 0x0;
    FspsConfig->Usb2OverCurrentPin[5] = 0x3;
    FspsConfig->Usb2OverCurrentPin[7] = 0x0;
    FspsConfig->Usb2OverCurrentPin[8] = 0x3;
    FspsConfig->Usb2OverCurrentPin[9] = 0x3;
    FspsConfig->Usb3OverCurrentPin[2] = 0xff;
    FspsConfig->Usb3OverCurrentPin[3] = 0xff;
  }

  FspsConfig->Usb2PhyTxiset[8] = 0x0;

  for (Index = 0; Index < 14; Index++) {
    FspsConfig->Usb2PhyPetxiset[Index] = 0x6;
    FspsConfig->Usb2PhyPredeemp[Index] = 0x3;
  }
  FspsConfig->Usb2PhyPehalfbit[8] = 0x0;
  FspsConfig->IomTypeCPortPadCfg[2] = 0x0;
  FspsConfig->IomTypeCPortPadCfg[3] = 0x0;

  FspsConfig->SkipMpInit = 0;

  FspsConfig->UsbTcPortEn = 0xf;

  if (SiCfgData != NULL) {
    FspsConfig->EnableTcoTimer   = SiCfgData->EnableTcoTimer;
    FspsConfig->EnableTimedGpio0 = SiCfgData->EnableTimedGpio0;
    FspsConfig->EnableTimedGpio1 = SiCfgData->EnableTimedGpio1;
    FspsConfig->XdciEnable       = SiCfgData->XdciEnable;
    CopyMem (FspsConfig->ITbtPcieRootPortEn, SiCfgData->ITbtPcieRootPortEn, sizeof(SiCfgData->ITbtPcieRootPortEn));
    CopyMem (FspsConfig->PtmEnabled, SiCfgData->TcssPcieRootPortPtmEn, sizeof(SiCfgData->TcssPcieRootPortPtmEn));
  }

#if FixedPcdGetBool(PcdTccEnabled)
    Status = TccModePostMemConfig (FspsUpd);
#endif

  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    FspsUpd->FspsConfig.SiSkipBiosDoneWhenFwUpdate = TRUE;
    FspsUpd->FspsConfig.PchLockDownBiosInterface = FALSE;
    FspsUpd->FspsConfig.PchSbAccessUnlock        = TRUE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock = FALSE;
    DEBUG ((DEBUG_INFO, "Firmware update mode, unlock Bios setting\n"));
  } else {
    FspsUpd->FspsConfig.PchLockDownBiosInterface = TRUE;
    FspsUpd->FspsConfig.PchSbAccessUnlock        = FALSE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock = TRUE;
  }

  if (S0IX_STATUS() == 1) {
    FspsConfig->C1e = 1;
    FspsConfig->Cx = 1;
    FspsConfig->PkgCStateLimit = 8;
    FspsConfig->CstateLatencyControl1Irtl = 0x0;
    FspsConfig->CstateLatencyControl2Irtl = 0x0;
    FspsConfig->CstateLatencyControl3Irtl = 0x0;
    FspsConfig->CstateLatencyControl4Irtl = 0x0;
    FspsConfig->CstateLatencyControl5Irtl = 0x0;
    FspsConfig->AcousticNoiseMitigation = 0;
    FspsConfig->PmSupport = 1;
    FspsConfig->RenderStandby = 1;
    FspsConfig->SataPwrOptEnable = 1;
    FspsConfig->SataPortsDevSlp[0] = 1;
    FspsConfig->SataPortsDevSlp[1] = 1;
    FspsConfig->SataPortsHotPlug[0] = 0;
    FspsConfig->SataPortsHotPlug[1] = 0;
    FspsConfig->SataPortsExternal[0] = 0;
    FspsConfig->SataPortsExternal[1] = 0;
    FspsConfig->Enable8254ClockGating = 1;
    FspsConfig->PchFivrDynPm = 1;
    FspsConfig->D3HotEnable = 0;
    FspsConfig->D3ColdEnable = 1;
    FspsConfig->PchLanEnable = 0;
    FspsConfig->PchTsnEnable[0] = 0;
    FspsConfig->PchTsnEnable[1] = 0;
    FspsConfig->XdciEnable = 0;

    // PCH SERIAL_UART_CONFIG
    for (Index = 0; Index < GetPchMaxSerialIoUartControllersNum (); Index++) {
      FspsConfig->SerialIoUartPowerGating[Index]     = 2;
      FspsConfig->SerialIoUartDmaEnable[Index]       = 1;
      FspsConfig->SerialIoUartDbg2[Index]            = 0;
    }
  }

  if (FeaturePcdGet (PcdEnablePciePm)) {
    StoreRpConfig (FspsConfig);
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
  @retval EFI_OUT_OF_RESOURCES
 */
EFI_STATUS
EFIAPI
SaveNvsData (
  IN  VOID    *Buffer,
  IN  UINT32   Length
)
{
  EFI_STATUS  Status;
  UINT32      Address;
  UINT32      NvsSize;
  UINT32      BaseAddress;
  UINT32      RegionSize;

  DEBUG ((DEBUG_INFO, "SaveNvsData  Length=0x%X\n", Length));

  Status = GetComponentInfo (FLASH_MAP_SIG_MRCDATA, &Address, &NvsSize);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  if (NvsSize < Length) {
    DEBUG ((DEBUG_INFO, "MRC region size (0x%x) is less than NVS data\n", NvsSize));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Compare input data against the stored MRC training data
  // if they match, no need to update again.
  //
  if (CompareMem ((VOID *)(UINTN)Address, Buffer, Length) == 0) {
    DEBUG ((DEBUG_INFO, "Matched with saved NVS data\n"));
    return EFI_ALREADY_STARTED;
  }

  Status = SpiGetRegionAddress (FlashRegionBios, &BaseAddress, &RegionSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  DEBUG ((DEBUG_INFO, "bios range (0x%x : 0x%x)\n", BaseAddress, RegionSize));

  BaseAddress = ((UINT32)(~RegionSize) + 1);
  if (Address < BaseAddress) {
    return EFI_ACCESS_DENIED;
  }

  Address -= BaseAddress;
  if ((Address + ROUNDED_UP(Length, KB_(4))) > RegionSize) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = SpiFlashErase (FlashRegionBios, Address, ROUNDED_UP(Length, KB_(4)));
  if (!EFI_ERROR(Status)) {
    Status = SpiFlashWrite (FlashRegionBios, Address, Length, Buffer);
    if (!EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "MRC data successfully saved to 0x%X\n", Address));
    }
  }

  return Status;
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
  UINT16                                  Bus;
  volatile PCI_DEVICE_INDEPENDENT_REGION *PciDev;
  UINT8                                  Instance;
  OS_BOOT_OPTION                         *BootOption;
  UINT32                                 Idx;

  FillBootOptionListFromCfgData (OsBootOptionList);

  // Disable PreOS checker if Fusa is not enabled.
  if (!FeaturePcdGet (PcdFusaEnabled)) {
    for (Idx = 0; Idx < OsBootOptionList->OsBootOptionCount; Idx++) {
      BootOption = &(OsBootOptionList->OsBootOption[Idx]);
      if ((BootOption->BootFlags & BOOT_FLAGS_PREOS) != 0) {
        BootOption->BootFlags &= ~BOOT_FLAGS_PREOS;
      }
    }
  }
  //
  // Depends on the PCI root bridge, pluged PCI devices, the bus number for NVMe device
  // might be different. so update the NVMe bus number in the device table.
  //
  Instance = 0;
  for (Bus = 1; Bus <= PCI_MAX_BUS; Bus++) {
    PciDev = (volatile PCI_DEVICE_INDEPENDENT_REGION *) MM_PCI_ADDRESS (Bus, 0, 0, 0);
    if (PciDev->DeviceId != 0xFFFF) {
      if ((PciDev->ClassCode[0] == 2) && (PciDev->ClassCode[1] == 8) && (PciDev->ClassCode[2] == 1)) {
        SetDeviceAddr (OsBootDeviceNvme, Instance, (UINT32)(Bus << 16));
        Instance += 1;
      }
    }
  }

  return;
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
  UINT64                       PlatformInfoMsr;
  UINT16                       MeManuMode;
  UINT16                       PlatformPreProd;


  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if(PlatformData != NULL){

    //
    // Get for Sample part
    //
    PlatformInfoMsr = AsmReadMsr64 (MSR_PLATFORM_INFO);
    // Bit27 represents Sample part detail
    PlatformPreProd = (PlatformInfoMsr & BIT27) ? 1 : 0;

    //
    // Get Manufacturing Mode
    //
    HeciGetManufactureMode(&MeManuMode);

    LoaderPlatformInfo->HwState = (PlatformData->BtGuardInfo.VerifiedBoot << 0)
                                     | (PlatformData->BtGuardInfo.MeasuredBoot << 1)
                                     | (MeManuMode << 2)
                                     | (PlatformPreProd << 3) ;
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
 Update the frame buffer info by reading the PCI address

 @param[out] GfxInfo   pointer to global HOB data structure.
 */
VOID
EFIAPI
UpdateFrameBufferInfo (
  OUT  EFI_PEI_GRAPHICS_INFO_HOB   *GfxInfo
)
{
  UINT64  GfxBar;

  if (PcdGetBool (PcdIntelGfxEnabled)) {
    GfxBar  = LShiftU64 (PciRead32 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0x1C)), 32);
    GfxBar += (PciRead32 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0x18)) & 0xFFFFFF00);
    GfxInfo->FrameBufferBase = (PHYSICAL_ADDRESS) GfxBar;
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
  GfxDeviceInfo->BarIndex = 1;
  GfxDeviceInfo->VendorId = PciRead16 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0));
  GfxDeviceInfo->DeviceId = PciRead16 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 2));
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
  LdrSmmInfo->SmiStsReg.SmiApmPos  = (UINT8)HighBitSet32 (B_PCH_SMI_STS_APM);
  LdrSmmInfo->SmiStsReg.Address    = (UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_STS);

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

  // Just compare Guid memory addresses which are in Stage2 data section instead of CompareGuid ()
  if (Guid == &gEfiGraphicsInfoHobGuid) {
    UpdateFrameBufferInfo (HobInfo);
  } else if (Guid == &gEfiGraphicsDeviceInfoHobGuid) {
    UpdateFrameBufferDeviceInfo (HobInfo);
  } else if (Guid == &gOsBootOptionGuid) {
    UpdateOsBootMediumInfo (HobInfo);
  } else if (Guid == &gSmmInformationGuid) {
    UpdateSmmInfo (HobInfo);
  } else if (Guid == &gLoaderPlatformInfoGuid) {
    UpdateLoaderPlatformInfo (HobInfo);
  }
}

/**
  This function updates CpuSsdt table PNVS dynamically.

  @param[in]  Table         Pointer to ACPI table
  @param[in]  Gnvs          Pointer to platform global NVS data

  @retval                   none

**/
VOID
PatchCpuSsdtTable (
  IN EFI_ACPI_DESCRIPTION_HEADER   *Table,
  IN GLOBAL_NVS_AREA               *GlobalNvs
  )
{
  CPU_NVS_AREA            *CpuNvs;
  UINT8                   *CurrPtr;
  UINT32                  *Signature;

  CpuNvs      = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;
  for (CurrPtr = (UINT8 *) Table; CurrPtr <= ((UINT8 *) Table + Table->Length); CurrPtr++) {
    Signature = (UINT32 *) (CurrPtr + 1);
    ///
    /// Update the CPU GlobalNvs area
    ///
    if ((*CurrPtr == AML_EXT_REGION_OP) && *Signature == SIGNATURE_32 ('P', 'N', 'V', 'S')) {
      ASSERT (*(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) == 0xFFFF0000);
      ASSERT (*(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) == 0xAA55);
      ///
      /// Cpu Nvs Area address
      ///
      DEBUG ((DEBUG_INFO, "CPU PNVS Base Old=0x%08X New=0x%08X\n", *(UINT32 *)(CurrPtr + 1 + sizeof (*Signature) + 2), CpuNvs));
      *(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) = (UINT32) (UINTN) CpuNvs;

      ///
      /// Cpu Nvs Area size
      ///
      DEBUG ((DEBUG_INFO, "CPU PNVS Size Old=0x%04X New=0x%04X\n", *(UINT16 *)(CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1), sizeof (CPU_NVS_AREA)));
      *(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) = sizeof (CPU_NVS_AREA);

      break;
    }
  }
}

/**
  Calculate relative power

  @param[in]  BaseRatio     Maximum bus ratio
  @param[in]  CurrRatio     Current bus ratio to get relative power
  @param[in]  TdpMilliWatt  Maximum power in mW

  @retval                   Calculated power value in mW

**/
UINT32
EFIAPI
CalculateRelativePower (
  IN  UINT16  BaseRatio,
  IN  UINT16  CurrRatio,
  IN  UINT32  TdpMilliWatt
  )
{
  UINT64  Power1;
  UINT64  Power2;
  UINT32  BasePower;

  ASSERT (CurrRatio <= BaseRatio);

  BasePower = (TdpMilliWatt / 1000);

  Power1 = (110000 - ((BaseRatio - CurrRatio) * 625));
  Power1 = DivU64x32 (Power1, 11);
  Power1 = MultU64x64 (Power1, Power1);

  Power2 = ((CurrRatio * 100) / BaseRatio);
  Power2 = DivU64x32 (MultU64x32 (MultU64x64 (Power2, Power1), BasePower), 10000000);

  return (UINT32)Power2;
}

/**
  Update ACPI CPU P-state PSS table

  @param[in]  PssTbl        Pointer to Cpu0Ist table
  @param[in]  GlobalNvs     Pointer to platform global NVS data

  @retval EFI_SUCCESS       Updated PSS successfully
  @retval Ohteres           Errors during updating PSS table

**/
EFI_STATUS
AcpiPatchPss (
  IN  EFI_ACPI_DESCRIPTION_HEADER   *Table,
  IN  GLOBAL_NVS_AREA               *GlobalNvs
  )
{
  UINT8               *Ptr;
  UINT8               *End;
  UINT8               *Lpss;
  UINT8               *Tpss;
  UINT16              PackageTdp;
  UINT32              PackageTdpWatt;
  UINT8               ProcessorPowerUnit;
  MSR_REGISTER        MsrValue;
  PSS_PARAMS          PssParams;

  Ptr = (UINT8 *)Table;
  End = (UINT8 *)Table+ Table->Length;
  for (Lpss = NULL, Tpss = NULL; Ptr < End; Ptr++) {
    if ((Lpss == NULL) && (*(UINT32 *)Ptr == SIGNATURE_32 ('L', 'P', 'S', 'S')) && (*(Ptr - 1) == AML_NAME_OP)) {
      Lpss = Ptr;
    }
    if ((Tpss == NULL) && (*(UINT32 *)Ptr == SIGNATURE_32 ('T', 'P', 'S', 'S')) && (*(Ptr - 1) == AML_NAME_OP)) {
      Tpss = Ptr;
    }

    if ((Lpss != NULL) && (Tpss != NULL)) {
      break;
    }
  }

  if ((Lpss == NULL) && (Tpss == NULL)) {
    DEBUG ((DEBUG_INFO, "Failed to find LPSS/TPSS in Cpu0Ist\n"));
    return EFI_NOT_FOUND;
  }

  MsrValue.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  PssParams.MaxBusRatio = MsrValue.Bytes.SecondByte;
  PssParams.MinBusRatio = MsrValue.Bytes.SixthByte;
  if ((GlobalNvs->CpuNvs.PpmFlags & PPM_TURBO) != 0) {
    MsrValue.Qword = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
    PssParams.TurboBusRatio = (UINT8)(MsrValue.Dwords.Low & 0xFF);
  } else {
    PssParams.TurboBusRatio = 0;
  }

  MsrValue.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);
  ProcessorPowerUnit = (UINT8)(MsrValue.Bytes.FirstByte & 0xF);
  if (ProcessorPowerUnit == 0) {
    ProcessorPowerUnit = 1;
  } else {
    ProcessorPowerUnit = (UINT8) LShiftU64 (2, (ProcessorPowerUnit - 1));
  }

  MsrValue.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  PackageTdp = (UINT16)(MsrValue.Dwords.Low & 0x7FFF);
  PackageTdpWatt = (UINT32)DivU64x32 (PackageTdp, ProcessorPowerUnit);

  PssParams.PackageMaxPower = (PackageTdpWatt * 1000);
  PssParams.PackageMinPower = CalculateRelativePower (PssParams.MaxBusRatio,
                                                      PssParams.MinBusRatio,
                                                      PssParams.PackageMaxPower);
  PssParams.GetRelativePower = CalculateRelativePower;
  PssParams.DoListAll = FALSE;

  if (Lpss != NULL) {
    AcpiPatchPssTable (Lpss, &PssParams);
  }

  if (Tpss != NULL) {
    AcpiPatchPssTable (Tpss, &PssParams);
  }

  return EFI_SUCCESS;
}

/**
  Update PCH NVS and SA NVS area address and size in ACPI table

  @param[in] Current       Pointer to ACPI description header

  @retval   EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
PlatformUpdateAcpiTable (
  IN UINT8                   *Current
)
{
  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE *MmCfg;
  EFI_ACPI_DESCRIPTION_HEADER *Table;
  UINT8                       *Ptr;
  UINT8                       *End;
  GLOBAL_NVS_AREA             *GlobalNvs;
  UINT32                       Base;
  UINT16                       Size;
  EFI_STATUS                   Status;
  PLATFORM_DATA               *PlatformData;
  SILICON_CFG_DATA            *SiCfgData;
  VOID                        *FspHobList;

  GlobalNvs  = (GLOBAL_NVS_AREA *)(UINTN) PcdGet32 (PcdAcpiGnvsAddress);

  Table = (EFI_ACPI_DESCRIPTION_HEADER *) Current;
  Ptr  = (UINT8 *)Table;
  End  = (UINT8 *)Table + Table->Length;

  if (Table->Signature == EFI_ACPI_5_0_EMBEDDED_CONTROLLER_BOOT_RESOURCES_TABLE_SIGNATURE) {
    SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
    if ((SiCfgData == NULL) || (SiCfgData->EcEnable == 0)) {
      return EFI_UNSUPPORTED;
    }
  } else if (Table->Signature == EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    for (; Ptr < End; Ptr++) {
      if (*(Ptr-1) != AML_NAME_OP)
        continue;
      if (*(UINT32 *)Ptr == SIGNATURE_32 ('P','N','V','B')) {
        Base = (UINT32) (UINTN) &GlobalNvs->PchNvs;
        DEBUG ((DEBUG_INFO, "PNVS Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
      } else if (*(UINT32 *)Ptr == SIGNATURE_32 ('P','N','V','L')) {
        Size = sizeof (PCH_NVS_AREA);
        DEBUG ((DEBUG_INFO, "PNVL Old=0x%08X New=0x%08X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
        // PNVL is after PNVB
        break;
      }
    }
  } else if (Table->Signature == \
    EFI_ACPI_5_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE) {
    MmCfg = (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE *)
            ((EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER *)Ptr + 1);
    Base  = 0;
    while ((UINT8 *)MmCfg < End) {
      MmCfg->BaseAddress = PcdGet64 (PcdPciExpressBaseAddress) + Base;
      Base += 0x10000000;
      MmCfg++;
    }
  } else if (Table->OemTableId == SIGNATURE_64 ('S', 'a', 'S', 's', 'd', 't', ' ', 0)) {
    for (; Ptr < End; Ptr++) {
      if (*(UINT32 *)Ptr == SIGNATURE_32 ('S','A','N','V')) {
        Base = (UINT32) (UINTN) &GlobalNvs->SaNvs;
        DEBUG ((DEBUG_INFO, "SANV Base Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 6), Base));
        *(UINT32 *)(Ptr + 6) = Base;

        Size = sizeof (SYSTEM_AGENT_NVS_AREA);
        DEBUG ((DEBUG_INFO, "SANV Size Old=0x%04X New=0x%04X\n", *(UINT16 *)(Ptr + 11), Size));
        *(UINT16 *)(Ptr + 11) = Size;
        break;
      }
    }
  } else if (Table->OemTableId == SIGNATURE_64 ('C', 'p', 'u', 'S', 's', 'd', 't', 0)) {
    PatchCpuSsdtTable (Table, GlobalNvs);
  } else if (Table->OemTableId == SIGNATURE_64 ('C', 'p', 'u', '0', 'I', 's', 't', 0)) {
    //
    // Temporarily disable Pss table patch during firmware update
    //
    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      AcpiPatchPss (Table, GlobalNvs);
    }
  } else if (Table->Signature == EFI_BDAT_TABLE_SIGNATURE) {
    FspHobList = GetFspHobListPtr ();
    if (FspHobList != NULL) {
      UpdateBdatAcpiTable (Table, FspHobList);
      DEBUG ((DEBUG_INFO, "Updated BDAT Table in AcpiTable Entries\n"));
    }
  } else if (Table->Signature == EFI_ACPI_6_1_LOW_POWER_IDLE_TABLE_STRUCTURE_SIGNATURE){
    UINT8                                  LpitStateEntries = 0;
    EFI_ACPI_6_1_GENERIC_ADDRESS_STRUCTURE SetResidencyCounter[3] = { ACPI_LPI_RES_SLP_S0_COUNTER, ACPI_LPI_RES_C10_COUNTER, ACPI_LPI_RES_PS_ON_COUNTER };
    UINT64                                 ResidencyCounterFrequency = 0;
    LpitStateEntries = (UINT8)(((EFI_ACPI_DESCRIPTION_HEADER *)Table)->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(ACPI_LPI_NATIVE_CSTATE_DESCRIPTOR);
    if (LpitStateEntries != 0) {
      //
      // LpitResidencyCounter : 0 - SLP_S0 Based, 1 - C10 Counter, 2 - ATX Shutdown/PS_ON
      //
        //
        // Read PWRM Base Address to fill in Residency counter Address Space
        //
        SetResidencyCounter[0].Address = (UINT64)PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_SLP_S0_RESIDENCY_COUNTER;
        ResidencyCounterFrequency = 10000; //Counter runs at 100us granularity which implies 10KHz frequency (10000Hz)
        if (IsPchLp ()) {
          ResidencyCounterFrequency = 8197;  //Counter runs at 122us granularity which implies 10KHz frequency (8197Hz)
        }
      (((ACPI_LOW_POWER_IDLE_TABLE *)Table)->LpiStates[LpitStateEntries - 1].ResidencyCounter) = SetResidencyCounter[0];
      (((ACPI_LOW_POWER_IDLE_TABLE *)Table)->LpiStates[LpitStateEntries - 1].ResidencyCounterFrequency) = ResidencyCounterFrequency;
    }
  } else if (Table->Signature == EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
    if (S0IX_STATUS() == 1) {
      EFI_ACPI_6_1_FIXED_ACPI_DESCRIPTION_TABLE  *FadtTable;
      DEBUG ((DEBUG_INFO, "Update FADT ACPI table\n"));
      FadtTable = (EFI_ACPI_6_1_FIXED_ACPI_DESCRIPTION_TABLE*) Table;
      FadtTable->Flags |= (EFI_ACPI_6_1_PCI_EXP_WAK);
      FadtTable->Flags |= (EFI_ACPI_6_1_LOW_POWER_S0_IDLE_CAPABLE);
      FadtTable->Flags |= (EFI_ACPI_6_1_PWR_BUTTON);
    }
  } else if (FeaturePcdGet (PcdVtdEnabled) && (Table->Signature == EFI_ACPI_6_4_DMA_REMAPPING_TABLE_SIGNATURE)) {
    DEBUG ((DEBUG_INFO, "Updated DMAR Table entries\n"));
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if ((PlatformData != NULL) && (PlatformData->PlatformFeatures.VtdEnable == 1)) {
      DmarTableUpdate (Table);
    }
  }

  if (MEASURED_BOOT_ENABLED()) {
    if ((Table->Signature  == EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE) ||
        (Table->OemTableId == ACPI_SSDT_TPM2_DEVICE_OEM_TABLE_ID)) {
      Status = UpdateTpm2AcpiTable(Table);
      ASSERT_EFI_ERROR (Status);
    }
  }

  if (FeaturePcdGet (PcdPsdBiosEnabled)) {
    if (Table->Signature == EFI_ACPI_PSD_SIGNATURE) {
      PSD_CFG_DATA *PsdCfgData = NULL;
      PsdCfgData = (PSD_CFG_DATA *)FindConfigDataByTag (CDATA_PSD_TAG);
      if (PsdCfgData != NULL) {
        if (PsdCfgData->EnablePsd == 1) {
          UpdateAcpiPsdTable ( (VOID* )Current);
          DEBUG ( (DEBUG_INFO, "Updated Psd Table in AcpiTable Entries\n") );
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 Update CPU NVS from CpuInitDataHob

 @param[in, out]  CpuNvs           Pointer to CPU NVS region

**/
VOID
UpdateCpuNvs (
  IN OUT CPU_NVS_AREA                   *CpuNvs
  )
{
  EFI_HOB_GUID_TYPE                     *GuidHob;
  CPU_INIT_DATA_HOB                     *CpuInitDataHob;
  CPU_CONFIG_DATA                       *CpuConfigData;
  VOID                                  *FspHobList;
  UINT16                     C6Latency = 0;
  UINT16                     C7Latency = 0;
  UINT16                     C8Latency = 0;
  UINT16                     C9Latency = 0;
  UINT16                     C10Latency = 0;
  CPU_SKU                    CpuSku;
  MSR_REGISTER               TempMsr;
  UINT8                      MaxRefTemp;
  UINT8                      Index;

  if (CpuNvs == NULL) {
    DEBUG ((DEBUG_ERROR, "Invalid Cpu Nvs pointer!!!\n"));
    return;
  }

  ///
  /// Get CPU Init Data Hob
  ///
  GuidHob = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gCpuInitDataHobGuid, FspHobList);
  }
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "CPU Data HOB not available\n"));
    return;
  }

  CpuInitDataHob = GET_GUID_HOB_DATA (GuidHob);
  CpuConfigData = (CPU_CONFIG_DATA *)(UINTN)CpuInitDataHob->CpuConfigData;
  CpuSku = GetCpuSku();

  CpuNvs->Cpuid = (UINT32)GetCpuFamily() | (UINT32)GetCpuStepping();
  CpuNvs->Revision = CPU_NVS_AREA_REVISION;
  ///
  /// Calculate the number of Oc bins supported. Read in MSR 194h FLEX_RATIO bits (19:17)
  ///
  TempMsr.Qword = AsmReadMsr64(MSR_FLEX_RATIO);
  CpuNvs->OcBins = (UINT8)RShiftU64((TempMsr.Dwords.Low & B_MSR_FLEX_RATIO_OC_BINS_MASK), N_MSR_FLEX_RATIO_OC_BINS_OFFSET);

  ///
  /// Update NVS ASL items.
  ///

  ///
  /// Intel Turbo Boost Max Technology 3.0
  ///
  CpuNvs->ItbmSmi = 0x29; ///< SMI for resuming the periodic SMM if the OS supports frequency re-read.

  ///
  /// Automatic Thermal Reporting for Thermal Management
  ///
  if (CpuConfigData->AutoThermalReporting) {
    ///
    /// Thermal Reporting for Critical trip
    /// MSR 1A2 bits 23:16 define the temperature that this specific processor can
    /// function upto. It is recommended that this value + 5  be used as default Critical trip point
    /// _CRT.
    ///
    TempMsr.Qword = AsmReadMsr64(MSR_TEMPERATURE_TARGET);
    TempMsr.Qword &= (UINT64)B_MSR_TEMPERATURE_TARGET_REF_TEMP_MASK;
    MaxRefTemp = (UINT8)RShiftU64(TempMsr.Qword, N_MSR_TEMPERATURE_TARGET_REF_TEMP_OFFSET);
    CpuNvs->AutoCriticalTripPoint = MaxRefTemp + 5;
    ///
    /// Thermal Reporting for Active Thermal Management
    /// It is recommended that the processor specific value in MSR 1A2 bits 15:8
    /// be used as the highest Active trip point i.e. _AC0.
    ///
    TempMsr.Qword = AsmReadMsr64(MSR_TEMPERATURE_TARGET);
    TempMsr.Qword &= (UINT64)B_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFST_MASK;
    CpuNvs->AutoActiveTripPoint = MaxRefTemp - (UINT8)RShiftU64(
      TempMsr.Qword,
      N_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFST_OFFSET
      );

    ///
    /// Thermal Reporting for Passive Thermal Management
    /// On all turbo enabled systems, it is recommended that the ACPI _PSV point be
    /// set to a temperature above the Active cooling temperature and Tcc activation
    /// temperature.
    /// If platform embedded controller will issue PECI commands to reduce power as a
    /// passive thermal action, then it is recommended to use the package's max temperature
    /// for passive thermal control.
    ///
    TempMsr.Qword = AsmReadMsr64(MSR_TEMPERATURE_TARGET);
    TempMsr.Qword &= B_MSR_TEMPERATURE_TARGET_TJ_MAX_TCC_OFFSET_MASK;
    CpuNvs->AutoPassiveTripPoint = CpuNvs->AutoCriticalTripPoint + 3;
  }

  ///
  ///  Update the PPM NVS area PPM flags
  ///
  CpuNvs->PpmFlags = CpuInitDataHob->PpmFlags;

  ///
  /// For U/Y, hard coded latency values are used instead of calculations. Set values here.
  ///
  switch (CpuSku) {
    case EnumCpuUlt:
      C6Latency = 127;
      C7Latency = 253;
      C8Latency = 260;
      C9Latency = 487;
      C10Latency = 1048;
      break;

    case EnumCpuUlx:
      C6Latency = 126;
      C7Latency = 230;
      C8Latency = 239;
      C9Latency = 398;
      C10Latency = 993;
      break;

    default:
      break;
  }

  ///
  /// Update PPM NVRAM Values for C6
  ///
  if (CpuNvs->PpmFlags & C6_LONG_LATENCY_ENABLE) {
    CpuNvs->C6MwaitValue = MWAIT_C6_1;
  }
  else {
    CpuNvs->C6MwaitValue = MWAIT_C6;
  }

  ///
  /// Update PPM NVRAM Values for C7 - select the C-state supported among- C7 / C7S
  ///
  if (CpuNvs->PpmFlags & PPM_C7) { // Is C7 supported ?
    if (CpuNvs->PpmFlags & C7_LONG_LATENCY_ENABLE) {
      CpuNvs->C7MwaitValue = MWAIT_C7_1;
    }
    else {
      CpuNvs->C7MwaitValue = MWAIT_C7;
    }
  }
  if (CpuNvs->PpmFlags & PPM_C7S) { // Is C7S supported ?
    if (CpuNvs->PpmFlags & C7s_LONG_LATENCY_ENABLE) {
      CpuNvs->C7MwaitValue = MWAIT_C7_3;
    }
    else {
      CpuNvs->C7MwaitValue = MWAIT_C7_2;
    }
  }

  ///
  /// For ICL, hard coded values are used instead of calculations. Reinitialize here to simplify the code for C6 and C7.
  ///

  if (C6Latency != 0) {
    CpuNvs->C6Latency = C6Latency;
  }

  if (C7Latency != 0) {
    CpuNvs->C7Latency = C7Latency;
  }

  ///
  /// Update PPM NVRAM Values for CD - select the deepest C-state supported among- C8 / C9 / C10
  ///
  if (CpuNvs->PpmFlags & PPM_C10) { // C10 supported
    CpuNvs->CDIOLevel = PCH_ACPI_LV7;
    CpuNvs->CDPowerValue = C10_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD_2;
    if (C10Latency != 0) {
      CpuNvs->CDLatency = C10Latency;
    }
  }
  else if (CpuNvs->PpmFlags & PPM_C9) { // C9 supported
    CpuNvs->CDIOLevel = PCH_ACPI_LV6;
    CpuNvs->CDPowerValue = C9_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD_1;
    if (C9Latency != 0) {
      CpuNvs->CDLatency = C9Latency;
    }
  }
  else if (CpuNvs->PpmFlags & PPM_C8) { // C8 supported
    CpuNvs->CDIOLevel = PCH_ACPI_LV5;
    CpuNvs->CDPowerValue = C8_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD;
    if (C8Latency != 0) {
      CpuNvs->CDLatency = C8Latency;
    }
  }

  if (CpuNvs->PpmFlags & PPM_HWP) {
    CpuNvs->HwpEnable = 1;
  }

  CpuNvs->CtdpLevelsSupported = CpuInitDataHob->CtdpLevelsSupported;                        /// ConfigTdp Number Of Levels.
  CpuNvs->ConfigTdpBootModeIndex = CpuInitDataHob->ConfigTdpBootModeIndex;                 /// CTDP Boot Mode Index
  for (Index = 0; Index < 3; Index++) {
    CpuNvs->CtdpTar[Index] = CpuInitDataHob->CtdpTar[Index];                             /// CTDP Levels TAR
    CpuNvs->CtdpPowerLimit1[Index] = CpuInitDataHob->CtdpPowerLimit1[Index];             /// CTDP Levels Power Limit1
    CpuNvs->CtdpPowerLimit2[Index] = CpuInitDataHob->CtdpPowerLimit2[Index];             /// CTDP Levels Power Limit2
    CpuNvs->CtdpPowerLimitWindow[Index] = CpuInitDataHob->CtdpPowerLimitWindow[Index];    /// CTDP Levels Power Limit1 Time Window
    CpuNvs->CtdpCtc[Index] = CpuInitDataHob->CtdpCtc[Index];                             /// CTDP Levels CTC
    CpuNvs->CtdpPpc[Index] = CpuInitDataHob->CtdpPpc[Index];                             /// CTDP Levels PPC
  }
  CpuNvs->EnableItbm = CpuInitDataHob->EnableItbm;                                         /// Enable/Disable Intel Turbo Boost Max Technology 3.0.
  CpuNvs->EnableItbmDriver = CpuInitDataHob->EnableItbmDriver;                             /// Enable/Disable Intel Turbo Boost Max Technology 3.0 Driver.
  CpuNvs->LowestMaxPerf = CpuInitDataHob->LowestMaxPerf;                                   /// Max ratio of the slowest core.
  CpuNvs->ConfigurablePpc = CpuInitDataHob->ConfigurablePpc;                               /// Max ratio of the slowest core.

  //WA for resolving windows boot failure on TGl-H.
  if (IsPchLp ()) {
    CpuNvs->DtsIoTrapAddress = DTS_IO_TRAP_ADDRESS;
    CpuNvs->DtsIoTrapLength = DTS_IO_TRAP_LENGTH;
    CpuNvs->DtsAcpiEnable = DTS_ACPI_DISABLE;
  }

  ///
  /// Update TXT status for ACPI
  ///
  CpuNvs->TxtEnabled = 0;  // Default to disabled
  if (FeaturePcdGet (PcdTxtEnabled)) {
    FEATURES_CFG_DATA *FeaturesCfgData;
    FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
    if ((FeaturesCfgData != NULL) && (FeaturesCfgData->Features.TxtEnabled == 1)) {
      if (IsTxtEnabled()) {
        CpuNvs->TxtEnabled = 1;
        DEBUG ((DEBUG_INFO, "TXT is enabled in MSR 0x3A, setting TXTE=1\n"));
      } else {
        DEBUG ((DEBUG_INFO, "TXT config enabled but MSR 0x3A indicates TXT not enabled.\n"));
      }
    }
  }

  DEBUG ((DEBUG_INFO, "Update Cpu Nvs Done\n"));

  DEBUG ((DEBUG_INFO, "Revision 0x%X, PpmFlags 0x%08X, TxtEnabled 0x%X\n", CpuNvs->Revision, CpuNvs->PpmFlags, CpuNvs->TxtEnabled));
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
  GLOBAL_NVS_AREA         *GlobalNvs;
  PLATFORM_NVS_AREA       *PlatformNvs;
  PCH_NVS_AREA            *PchNvs;
  CPU_NVS_AREA            *CpuNvs;
  SYSTEM_AGENT_NVS_AREA   *SaNvs;
  SYS_CPU_INFO            *SysCpuInfo;
  SILICON_CFG_DATA        *SiCfgData;
  EFI_CPUID_REGISTER      CpuidRegs;
  UINT8                   Index;
  UINT8                   Length;
  UINT8                   PchMaxPciePortNum;
  GPIO_GROUP              GroupToGpeDwX[3];
  UINT32                  GroupDw[3];
  FSPS_UPD                *FspsUpd;
  FSP_S_CONFIG            *FspsConfig;

  FspsUpd     = (FSPS_UPD *)(UINTN)PcdGet32 (PcdFspsUpdPtr);
  FspsConfig  = &FspsUpd->FspsConfig;

  SysCpuInfo  = NULL;
  GlobalNvs   = (GLOBAL_NVS_AREA *) GnvsIn;
  PlatformNvs = (PLATFORM_NVS_AREA *) &GlobalNvs->PlatformNvs;
  PchNvs      = (PCH_NVS_AREA *) &GlobalNvs->PchNvs;
  CpuNvs      = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;
  SaNvs       = (SYSTEM_AGENT_NVS_AREA *) &GlobalNvs->SaNvs;
  ZeroMem (GlobalNvs, sizeof (GLOBAL_NVS_AREA));

  PlatformNvs->Ps2MouseEnable               = 0x0;
  PlatformNvs->Ps2KbMsEnable                = 0x0;

  PchNvs->PchSeries     = PchSeries ();
  PchNvs->PchGeneration = (UINT16) PchGeneration ();
  PchNvs->PchStepping   = (UINT16) PchStepping ();
  PchNvs->SGIR = FspsConfig->GpioIrqRoute ;
  PchNvs->SWMC = GetPchHdaMaxSndwLinkNum();
  Length = GetPchMaxPciePortNum ();

  PchNvs->EnableTimedGpio0 = FspsConfig->EnableTimedGpio0;
  PchNvs->EnableTimedGpio1 = FspsConfig->EnableTimedGpio1;

  PchMaxPciePortNum = GetPchMaxPciePortNum ();
  for (Index = 0; Index < PchMaxPciePortNum; Index++) {
    PchNvs->LtrEnable[Index]                = FspsConfig->PcieRpLtrEnable[Index];
    PchNvs->PcieLtrMaxSnoopLatency[Index]   = FspsConfig->PcieRpLtrMaxSnoopLatency[Index];
    PchNvs->PcieLtrMaxNoSnoopLatency[Index] = FspsConfig->PcieRpLtrMaxNoSnoopLatency[Index];
  }
  //
  // Update HPET base address.
  //
  PchNvs->HPTE          = TRUE;
  PchNvs->HPTB          = HPET_BASE_ADDRESS;

  //
  // Update SBREG_BAR.
  //
  PchNvs->SBRG          = PCH_PCR_BASE_ADDRESS;

  //
  // Update PMC ACPIBASE and PWRMBASE
  //
  PchNvs->PMBS          = ACPI_BASE_ADDRESS;
  PchNvs->PWRM          = PCH_PWRM_BASE_ADDRESS;
  GpioGetGroupDwToGpeDwX (
    &GroupToGpeDwX[0], &GroupDw[0],
    &GroupToGpeDwX[1], &GroupDw[1],
    &GroupToGpeDwX[2], &GroupDw[2]
    );
  PchNvs->GEI0                          = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[0]);
  PchNvs->GEI1                          = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[1]);
  PchNvs->GEI2                          = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[2]);
  PchNvs->GED0                          = (UINT8) GroupDw[0];
  PchNvs->GED1                          = (UINT8) GroupDw[1];
  PchNvs->GED2                          = (UINT8) GroupDw[2];

  Length = GetPchMaxSerialIoSpiControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->SM0[Index] = FspsConfig->SerialIoSpiMode[Index];
  }
  Length = GetPchMaxSerialIoI2cControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->IM0[Index] = FspsConfig->SerialIoI2cMode[Index];
  }
  Length = GetPchMaxSerialIoUartControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->UM0[Index] = FspsConfig->SerialIoUartMode[Index];
    PchNvs->UD0[Index] = FspsConfig->SerialIoUartDmaEnable[Index];
    PchNvs->UP0[Index] = FspsConfig->SerialIoUartPowerGating[Index];
  }
  PchNvs->CnviMode           = FspsConfig->CnviMode;
  PchNvs->CnviBtCore         = FspsConfig->CnviBtCore;
  PchNvs->CnviBtAudioOffload = FspsConfig->CnviBtAudioOffload;
  PchNvs->PsOnEnable         = FspsConfig->PsOnEnable;
  PchNvs->CnviPortId         = PID_CNVI;
  PchNvs->IclkPid            = PID_ICLK;
  PchNvs->GBES               = PchIsGbeSupported();
  PchNvs->CpuSku             = GetCpuSku();
  SysCpuInfo = MpGetInfo ();
  if ( SysCpuInfo != NULL ) {
    PlatformNvs->ThreadCount      = (UINT8)SysCpuInfo->CpuCount;
  }

  // TSN
  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if ((SiCfgData != NULL) && (SiCfgData->PchTsnEnable[0] || SiCfgData->PchTsnEnable[1])) {
    PlatformNvs->TsnPcsEnabled  = 1;
  }

  // Update Platform
  PlatformNvs->EnableDptf = 0;
  PlatformNvs->WifiEnable = 0;
  PlatformNvs->XdciFnEnable = 0;
  PlatformNvs->PowerState = 1;
  PlatformNvs->NativePCIESupport = 1;
  PlatformNvs->PciDelayOptimizationEcr = 0;
  PlatformNvs->ApicEnable = 1;
  PlatformNvs->Ps2MouseEnable = 1;
  PlatformNvs->UsbTypeCSupport = 1;

  if ( (SiCfgData != NULL) && (SiCfgData->EcEnable == 1)){
    PlatformNvs->EcAvailable                  = 1;
    PlatformNvs->EcLowPowerMode               = 1;
    PlatformNvs->EcSmiGpioPin                 = 0;
    PlatformNvs->EcLowPowerModeGpioPin        = 0;

    if (IsPchLp ()) {
      PlatformNvs->EcSmiGpioPin                 = GPIO_VER2_LP_GPP_E7;
      PlatformNvs->EcLowPowerModeGpioPin        = GPIO_VER2_LP_GPP_E8;
    } else if (IsPchH ()) {
      PlatformNvs->EcSmiGpioPin                 = GPIO_VER2_H_GPP_E3;
      PlatformNvs->EcLowPowerModeGpioPin        = GPIO_VER2_H_GPP_B23;
    }

  }

  PlatformNvs->Ac1TripPoint                 = 55;
  PlatformNvs->Ac0TripPoint                 = 71;
  PlatformNvs->Ac1FanSpeed                  = 75;
  PlatformNvs->Ac0FanSpeed                  = 100;
  PlatformNvs->PassiveThermalTripPoint      = 95;
  PlatformNvs->PassiveTc1Value              = 1;
  PlatformNvs->PassiveTc2Value              = 5;
  PlatformNvs->PassiveTspValue              = 10;
  PlatformNvs->CriticalThermalTripPoint     = 110;
  PlatformNvs->EnableSaDevice               = 1;
  PlatformNvs->EnableDigitalThermalSensor   = 0;
  PlatformNvs->Rtd3Support                  = 1;
  PlatformNvs->TenSecondPowerButtonEnable   = 0x9;
  PlatformNvs->HidEventFilterEnable         = 0x01;
  PlatformNvs->LowPowerS0Idle               = S0IX_STATUS();

  // Bit[1:0] - Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
  // Bit[2]   - En/Dis UART0
  // Bit[3]   - En/Dis UART1
  // Bit[4]   - En/Dis PCIe NVMe/AHCI
  // Bit[5]   - En/Dis I2C0
  // Bit[6]   - En/Dis I2C1
  // Bit[7]   - En/Dis XHCI
  // Bit[9:8] - En/Dis HD Audio (includes ADSP)
  // Bit[10]  - En/Dis GFX
  // Bit[11]  - En/Dis CPU
  // Bit[12]  - En/Dis EMMC
  // Bit[13]  - En/Dis SDXC
  // Bit[14]  - En/Dis I2C2
  // Bit[15]  - En/Dis I2C3
  // Bit[16]  - En/Dis I2C4
  // Bit[17]  - En/Dis I2C5
  // Bit[18]  - En/Dis UART2
  // Bit[19]  - En/Dis SPI0
  // Bit[20]  - En/Dis SPI1
  // Bit[21]  - En/Dis SPI2
  // Bit[22]  - En/Dis IPU0
  // Bit[23]  - En/Dis CSME
  // Bit[24]  - En/Dis LAN(GBE)
  // Bit[25]  - En/Dis PEG NVMe/AHCI
  // Bit[26]  - En/Dis THC0
  // Bit[27]  - En/Dis THC1
  // Bit[28]  - En/Dis IDA
  // Bit[29]  - En/Dis I2C6
  // Bit[30]  - En/Dis TCSS IPs
  // Bit[31]  - En/Dis GNA0
  // Bit[32]  - En/Dis VMD
  // Bit[33]  - En/Dis DG PEG0
  // Bit[34]  - En/Dis HECI3
  // Bit[35]  - En/Dis DG PEG1
  // Bit[36]  - En/Dis PEP Constraint Override
  // Bit[38:37] - PCIe RP03 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[40:39] - PCIe RP05 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[42:41] - PCIe RP09 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[44:43] - PEG0 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[46:45] - PEG1 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[48:47] - PEG2 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[50:49] - PEG3 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[52:51] - PCIe RP01 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[54:53] - PCIe RP08 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[56:55] - PCIe RP17 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[58:57] - PCIe RP21 (0: No Constraints or 1: D0/F1 or 3:D3)
  PlatformNvs->LowPowerS0IdleConstraint = GetLowPowerS0IdleConstraint();
  PlatformNvs->MipiCamCvfSupport        = 0;
  PlatformNvs->Dg1VramSRGpio            = GPIO_VER2_LP_GPP_F20;
  PlatformNvs->Dg1VramSRGpioPolarity    = 0;
  PlatformNvs->CriticalThermalTripPoint = 119;
  PlatformNvs->Revision                 = 1;
  PlatformNvs->PlatformId               = 1;

  AsmCpuid(1, &CpuidRegs.RegEax, 0, 0, 0);
  PlatformNvs->PlatformCpuId            = (CpuidRegs.RegEax & 0x0FFFFF);
  PlatformNvs->PlatformFlavor            = 1;
  PlatformNvs->Rtd3P0dl            = 0x64;
  PlatformNvs->CSNotifyEC    = 1;
  PlatformNvs->Rtd3AudioDelay    = 0xC8;
  PlatformNvs->Rtd3SensorHub    = 0x44;
  PlatformNvs->Rtd3TouchPanelDelay    = 0x44;
  PlatformNvs->Rtd3TouchPadDelay    = 0x44;
  PlatformNvs->VRRampUpDelay    = 0x10;
  PlatformNvs->Rtd3Config0    = 0x8;
  PlatformNvs->Ps2KbMsEnable                = 0x1;
  PlatformNvs->SSH0 = 0x264;
  PlatformNvs->SSL0 = 0x2C2;
  PlatformNvs->SSD0 = 0x1E;
  PlatformNvs->FMH0 = 0x6E;
  PlatformNvs->FML0 = 0xCF;
  PlatformNvs->FMD0 = 0x1E;
  PlatformNvs->FPH0 = 0x2B;
  PlatformNvs->FPL0 = 0x4C;
  PlatformNvs->FPD0 = 0x1E;
  PlatformNvs->HSH0 = 0xB;
  PlatformNvs->HSL0 = 0x16;
  PlatformNvs->HSD0 = 0x5;
  PlatformNvs->SSH1 = 0x264;
  PlatformNvs->SSL1 = 0x2C2;
  PlatformNvs->SSD1 = 0x1E;
  PlatformNvs->FMH1 = 0x6E;
  PlatformNvs->FML1 = 0xCF;
  PlatformNvs->FMD1 = 0x1E;
  PlatformNvs->FPH1 = 0x2B;
  PlatformNvs->FPL1 = 0x4C;
  PlatformNvs->FPD1 = 0x1E;
  PlatformNvs->HSH1 = 0xB;
  PlatformNvs->HSL1 = 0x16;
  PlatformNvs->HSD1 = 0x5;
  PlatformNvs->SSH2 = 0x264;
  PlatformNvs->SSL2 = 0x2C2;
  PlatformNvs->SSD2 = 0x1E;
  PlatformNvs->FMH2 = 0x6E;
  PlatformNvs->FML2 = 0xCF;
  PlatformNvs->FMD2 = 0x1E;
  PlatformNvs->FPH2 = 0x2B;
  PlatformNvs->FPL2 = 0x4C;
  PlatformNvs->FPD2 = 0x1E;
  PlatformNvs->HSH2 = 0xB;
  PlatformNvs->HSL2 = 0x16;
  PlatformNvs->HSD2 = 0x5;
  PlatformNvs->SSH3 = 0x264;
  PlatformNvs->SSL3 = 0x2C2;
  PlatformNvs->SSD3 = 0x1E;
  PlatformNvs->FMH3 = 0x6E;
  PlatformNvs->FML3 = 0xCF;
  PlatformNvs->FMD3 = 0x1E;
  PlatformNvs->FPH3 = 0x2B;
  PlatformNvs->FPL3 = 0x4C;
  PlatformNvs->FPD3 = 0x1E;
  PlatformNvs->HSH3 = 0xB;
  PlatformNvs->HSL3 = 0x16;
  PlatformNvs->HSD3 = 0x5;
  PlatformNvs->SSH4 = 0x264;
  PlatformNvs->SSL4 = 0x2C2;
  PlatformNvs->SSD4 = 0x1E;
  PlatformNvs->FMH4 = 0x6E;
  PlatformNvs->FML4 = 0xCF;
  PlatformNvs->FMD4 = 0x1E;
  PlatformNvs->FPH4 = 0x2B;
  PlatformNvs->FPL4 = 0x4C;
  PlatformNvs->FPD4 = 0x1E;
  PlatformNvs->HSH4 = 0xB;
  PlatformNvs->HSL4 = 0x16;
  PlatformNvs->HSD4 = 0x5;
  PlatformNvs->SSH5 = 0x2C2;
  PlatformNvs->SSL5 = 0x264;
  PlatformNvs->SSD5 = 0x1E;
  PlatformNvs->FMH5 = 0x6E;
  PlatformNvs->FML5 = 0xCF;
  PlatformNvs->FMD5 = 0x1E;
  PlatformNvs->FPH5 = 0x2B;
  PlatformNvs->FPL5 = 0x4C;
  PlatformNvs->FPD5 = 0x1E;
  PlatformNvs->HSH5 = 0xB;
  PlatformNvs->HSL5 = 0x16;
  PlatformNvs->HSD5 = 0x5;
  PlatformNvs->M0C0 = 0xC8;
  PlatformNvs->M1C0 = 0x7D0;
  PlatformNvs->M0C1 = 0xC8;
  PlatformNvs->M1C1 = 0x7D0;
  PlatformNvs->M0C2 = 0xC8;
  PlatformNvs->M1C2 = 0x7D0;
  PlatformNvs->M0C3 = 0xC8;
  PlatformNvs->M1C3 = 0x7D0;
  PlatformNvs->M0C4 = 0xC8;
  PlatformNvs->M1C4 = 0x7D0;
  PlatformNvs->M0C5 = 0xC8;
  PlatformNvs->M1C5 = 0x7D0;
  PlatformNvs->M0C6 = 0xC8;
  PlatformNvs->M1C6 = 0x7D0;
  PlatformNvs->M0C7 = 0xC8;
  PlatformNvs->M1C7 = 0x7D0;
  PlatformNvs->M0C8 = 0xC8;
  PlatformNvs->M1C8 = 0x7D0;
  PlatformNvs->M0C9 = 0xC8;
  PlatformNvs->M1C9 = 0xC8;
  PlatformNvs->M0CA = 0xC8;
  PlatformNvs->M1CA = 0xC8;
  PlatformNvs->M0CB = 0xC8;
  PlatformNvs->M1CB = 0xC8;
  PlatformNvs->PL1LimitCSValue = 0x1194;
  PlatformNvs->EnableInt3400Device = 0x1;
  PlatformNvs->PsmSplcDomainType1 = 0x9;
  PlatformNvs->PsmSplcPowerLimit1 = 0xFA0;
  PlatformNvs->PsmSplcTimeWindow1 = 0x7530;
  PlatformNvs->PsmDplcDomainType1 = 0x9;
  PlatformNvs->PsmDplcDomainPreference1 = 0x9;
  PlatformNvs->PsmDplcDefaultPowerLimit1 = 0xB0;
  PlatformNvs->PsmDplcDefaultTimeWindow1 = 0x7530;
  PlatformNvs->PsmDplcMinimumPowerLimit1 = 0x4B0;
  PlatformNvs->PsmDplcMaximumPowerLimit1 = 0x4B0;
  PlatformNvs->PsmDplcMaximumTimeWindow1 = 0x3E8;
  PlatformNvs->SDS9 = 0x1;
  PlatformNvs->TPLS = 0x1;
  PlatformNvs->HebcValue = 0x233F3;
  PlatformNvs->PcdBatteryPresent = 0x3;
  PlatformNvs->PcdRealBattery1Control = 0x1;
  PlatformNvs->PcdRealBattery2Control = 0x2;
  PlatformNvs->PcdConvertableDockSupport = 0x1;

  if ((SiCfgData != NULL) && (SiCfgData->EcEnable == 1)) {
    PlatformNvs->PcdEcHotKeyF3Support = 0x1;
    PlatformNvs->PcdEcHotKeyF4Support = 0x1;
    PlatformNvs->PcdEcHotKeyF5Support = 0x1;
    PlatformNvs->PcdEcHotKeyF6Support = 0x1;
    PlatformNvs->PcdEcHotKeyF7Support = 0x1;
    PlatformNvs->PcdEcHotKeyF8Support = 0x1;
    PlatformNvs->PcdAcpiEnableAllButtonSupport = 0x1;
    PlatformNvs->PcdAcpiHidDriverButtonSupport = 0x1;
  }

  PlatformNvs->PcdVirtualButtonVolumeUpSupport = 0x1;
  PlatformNvs->PcdVirtualButtonVolumeDownSupport = 0x1;
  PlatformNvs->PcdVirtualButtonHomeButtonSupport = 0x1;
  PlatformNvs->PcdVirtualButtonRotationLockSupport = 0x1;
  PlatformNvs->PcdSlateModeSwitchSupport = 0x1;
  PlatformNvs->PcdAcDcAutoSwitchSupport = 0x1;
  PlatformNvs->PcdPmPowerButtonGpioPin = 0x9050003;
  PlatformNvs->UCMS = 0x1;
  PlatformNvs->WwanPerstGpio = 0x9000011;
  PlatformNvs->PcieSlot1WakeGpio = 0x90C0005;
  PlatformNvs->PcieSlot1RpNumber = 0x5;
  PlatformNvs->PcieSlot2WakeGpio = 0x90C0004;
  PlatformNvs->PcieSlot2RpNumber = 0x8;
  PlatformNvs->WwanFullCardPowerOffGpio = 0x90B000B;
  PlatformNvs->WwanFullCardPowerOffGpioPolarity = 0x1;
  PlatformNvs->WwanBbrstGpio = 0x90B000A;
  PlatformNvs->WwanWakeGpio = 0x90B0009;
  PlatformNvs->TouchpanelPowerEnableGpio = 0x907000E;
  PlatformNvs->TouchpanelPowerRstGpio = 0x90E0006;
  PlatformNvs->Touchpanel1IrqGpio = 0x90C0012;
  PlatformNvs->Touchpanel1PowerEnableGpio = 0x90C0007;
  PlatformNvs->Touchpanel1PowerRstGpio = 0x90C0011;
  PlatformNvs->TouchpanelIrqGpioPolarity = 0x1;
  PlatformNvs->TouchpanelPowerEnableGpioPolarity = 0x1;
  PlatformNvs->TouchpanelPowerRstGpioPolarity = 0x1;
  PlatformNvs->Touchpanel1IrqGpioPolarity = 0x1;
  PlatformNvs->Touchpanel1PowerEnableGpioPolarity = 0x1;
  PlatformNvs->Touchpanel1PowerRstGpioPolarity = 0x1;
  PlatformNvs->PcieSlot1PowerEnableGpio = 0x902000E;
  PlatformNvs->PcieSlot1RstGpio = 0x090B000D;
  PlatformNvs->PcieSlot2PowerEnableGpio = 0x90C0009;
  PlatformNvs->PcieSlot2PowerEnableGpioPolarity = 0x1;
  PlatformNvs->PcieSlot2RstGpio = 0x90C000A;
  PlatformNvs->SataPortPowerEnableGpio = 0x9000004;
  PlatformNvs->SataPortPowerEnableGpioPolarity = 0x1;
  PlatformNvs->PchM2SsdPowerEnableGpio = 0x9000010;
  PlatformNvs->PchM2SsdPowerEnableGpioPolarity = 0x1;
  PlatformNvs->PchM2SsdRstGpio = 0x9070000;
  PlatformNvs->M2Ssd2PowerEnableGpio = 0x9080010;
  PlatformNvs->M2Ssd2PowerEnableGpioPolarity = 0x1;
  PlatformNvs->M2Ssd2RstGpio = 0x902000B;
  PlatformNvs->SdevXhciRootPortNumber2 = 0x1;
  PlatformNvs->DeepestUSBSleepWakeCapability = 0x4;


  // System Agent
  SaNvs->XPcieCfgBaseAddress      = (UINT32)(PcdGet64(PcdPciExpressBaseAddress));
  SaNvs->Mmio64Base               = PcdGet64(PcdPciResourceMem64Base);
  SaNvs->Mmio64Length             = 0x4000000000ULL;
  SaNvs->Mmio32Base               = PcdGet32(PcdPciResourceMem32Base);
  if (SaNvs->Mmio32Base < SaNvs->XPcieCfgBaseAddress) {
    SaNvs->Mmio32Length = SaNvs->XPcieCfgBaseAddress - SaNvs->Mmio32Base;
  } else if (SaNvs->Mmio32Base < 0xF0000000) {
    SaNvs->Mmio32Length = 0xF0000000 - SaNvs->Mmio32Base;
  } else {
    DEBUG((DEBUG_INFO, "acpi: Unable to configure M32L with M32B=0x%08X\n", SaNvs->Mmio32Base));
  }

  AsmCpuid(1, &CpuidRegs.RegEax, 0, 0, 0);
  SaNvs->CpuIdInfo                = (CpuidRegs.RegEax & 0x0FFFFF);

  SaNvs->CpuPcieRp0Enable = 1;
  SaNvs->CpuPcieRp1Enable = 1;
  SaNvs->CpuPcieRp2Enable = 1;
  SaNvs->CpuPcieRp3Enable = 1;
  SaNvs->SimicsEnvironment = 0;

  SaNvs->AlsEnable = 0;
  SaNvs->IgdState = 0;
  SaNvs->BrightnessPercentage = 100;
  SaNvs->IgdBootType = 0;
  SaNvs->IgdPanelType = 0;
  SaNvs->IgdPanelScaling = 0;
  SaNvs->IgdDvmtMemSize = 2;
  SaNvs->IgdFunc1Enable = 0;
  SaNvs->IgdSciSmiMode = 0;
  SaNvs->GfxTurboIMON = 31;
  SaNvs->EdpValid = 0;
  SaNvs->MaxPegPortNumber = GetMaxCpuPciePortNum ();
  SaNvs->CpuPcieRtd3 = 1;


  SaNvs->DelayAfterPwrEn = 0x12C;
  SaNvs->DelayAfterHoldReset = 0x64;
  SaNvs->Peg0LtrEnable = 1;
  SaNvs->Peg0ObffEnable = 1;
  SaNvs->Peg1LtrEnable = 1;
  SaNvs->Peg1ObffEnable = 1;
  SaNvs->Peg2LtrEnable = 1;
  SaNvs->Peg2ObffEnable = 1;
  SaNvs->Peg3LtrEnable = 1;
  SaNvs->Peg3ObffEnable = 1;
  SaNvs->PegLtrMaxSnoopLatency = 0x846;
  SaNvs->PegLtrMaxNoSnoopLatency = 0x846;
  SaNvs->HgMode = 0x03;
  SaNvs->LtrEnable[0] = 1;
  SaNvs->LtrEnable[1] = 1;
  SaNvs->LtrEnable[2] = 1;
  SaNvs->LtrEnable[3] = 1;
  SaNvs->PcieLtrMaxSnoopLatency[0] = 0x8C8;
  SaNvs->PcieLtrMaxSnoopLatency[1] = 0x8C8;
  SaNvs->PcieLtrMaxSnoopLatency[2] = 0x8C8;
  SaNvs->PcieLtrMaxSnoopLatency[3] = 0x8C8;
  SaNvs->PcieLtrMaxNoSnoopLatency[0] = 0x8C8;
  SaNvs->PcieLtrMaxNoSnoopLatency[1] = 0x8C8;
  SaNvs->PcieLtrMaxNoSnoopLatency[2] = 0x8C8;
  SaNvs->PcieLtrMaxNoSnoopLatency[3] = 0x8C8;
  SaNvs->SlotSelection = 1;

  UpdateCpuNvs (CpuNvs);
  PlatformNvs->PpmFlags           = CpuNvs->PpmFlags;

  SocUpdateAcpiGnvs ((VOID *)GnvsIn);
}

