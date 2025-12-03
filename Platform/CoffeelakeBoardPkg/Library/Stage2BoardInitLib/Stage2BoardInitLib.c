/** @file

  Copyright (c) 2017 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <FspsUpd.h>
#include <GlobalNvsAreaDef.h>
#include <Library/BootloaderCoreLib.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <BlCommon.h>
#include <Guid/BootLoaderServiceGuid.h>
#include <Library/SpiBlockIoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Service/SpiFlashService.h>
#include <Library/PrintLib.h>
#include <ConfigDataDefs.h>
#include <Library/ConfigDataLib.h>
#include <Library/PchInfoLib.h>
#include <Library/IgdOpRegionLib.h>
#include <Library/BdatLib.h>
#include <Library/VariableLib.h>
#include <Guid/SmmInformationGuid.h>
#include <GpioPinsCnlH.h>
#include <GpioPinsCnlLp.h>
#include "HdaVerbTable.h"
#include <Library/TpmLib.h>
#include <Library/PchSerialIoLib.h>
#include <Library/SgxLib.h>
#include <PlatformBase.h>
#include <Library/GpioLib.h>
#include <Library/GpioSiLib.h>
#include <RegAccess.h>
#include <CpuPowerMgmt.h>
#include <PowerMgmtNvsStruct.h>
#include <CpuInitDataHob.h>
#include <Library/HobLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/DmaRemappingTable.h>
#include <Library/VTdLib.h>
#include <Library/MpInitLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Library/PciCf8Lib.h>
#include <Library/BoardSupportLib.h>
#include <PlatformData.h>
#include <PsdLib.h>
#include <Library/SmbiosInitLib.h>
#include <IndustryStandard/SmBios.h>
#include <VerInfo.h>
#include <Library/VtdPmrLib.h>
#include <Library/S3SaveRestoreLib.h>
#include "GpioTables.h"
#include <Library/PchSpiLib.h>
#include <Register/RegsSpi.h>
#include <Library/HeciLib.h>
#include <Library/PlatformHookLib.h>

#define DEFAULT_GPIO_IRQ_ROUTE                      14

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

#define R_SA_SMRAMC  (0x88)

#define PCI_TO_CF8_ADDRESS(A) \
  ((UINT32) ((((A) >> 4) & 0x00ffff00) | ((A) & 0xfc) | 0x80000000))

#define GPIO_CFG_PIN_TO_PAD(A) \
  ((UINT32) (((A).PinNum) | (((A).Group) << 16)))

///
/// Overcurrent pins, the values match the setting of EDS, please refer to EDS for more details
///
typedef enum {
  UsbOverCurrentPin0 = 0,
  UsbOverCurrentPin1,
  UsbOverCurrentPin2,
  UsbOverCurrentPin3,
  UsbOverCurrentPin4,
  UsbOverCurrentPin5,
  UsbOverCurrentPin6,
  UsbOverCurrentPin7,
  UsbOverCurrentPinMax,
  UsbOverCurrentPinSkip = 0xFF
} USB_OVERCURRENT_PIN;

typedef struct {
  UINT8 DevNum;
  UINT8 Pid;
  UINT8 RpNumBase;
} PCH_PCIE_CONTROLLER_INFO;

CONST PCH_PCIE_CONTROLLER_INFO mPchPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PID_SPA,  0 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PID_SPB,  4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PID_SPC,  8 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PID_SPD, 12 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PID_SPE, 16 }, // PCH-H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PID_SPF, 20 }  // PCH-H only
};

CONST UINT8 mPxRcConfig[8] = { 11, 10, 11, 11, 11, 11, 11, 11 };

CONST SI_PCH_DEVICE_INTERRUPT_CONFIG mPchDevIntConfig[] = {
  {31, 3, SiPchIntA, 16}, // cAVS(Audio, Voice, Speach), INTA is default, programmed in PciCfgSpace 3Dh
  {31, 4, SiPchIntA, 16}, // SMBus Controller, no default value, programmed in PciCfgSpace 3Dh
  {31, 7, SiPchIntA, 16}, // TraceHub, INTA is default, RO register
  {30, 0, SiPchIntA, 20}, // SerialIo: UART #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[7]
  {30, 1, SiPchIntB, 21}, // SerialIo: UART #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[8]
  {30, 2, SiPchIntC, 22}, // SerialIo: SPI #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[10]
  {30, 3, SiPchIntD, 23}, // SerialIo: SPI #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[11]
  {28, 0, SiPchIntA, 16}, // PCI Express Port 1, INT is default, programmed in PciCfgSpace + FCh
  {28, 1, SiPchIntB, 17}, // PCI Express Port 2, INT is default, programmed in PciCfgSpace + FCh
  {28, 2, SiPchIntC, 18}, // PCI Express Port 3, INT is default, programmed in PciCfgSpace + FCh
  {28, 3, SiPchIntD, 19}, // PCI Express Port 4, INT is default, programmed in PciCfgSpace + FCh
  {28, 4, SiPchIntA, 16}, // PCI Express Port 5, INT is default, programmed in PciCfgSpace + FCh
  {28, 5, SiPchIntB, 17}, // PCI Express Port 6, INT is default, programmed in PciCfgSpace + FCh
  {28, 6, SiPchIntC, 18}, // PCI Express Port 7, INT is default, programmed in PciCfgSpace + FCh
  {28, 7, SiPchIntD, 19}, // PCI Express Port 8, INT is default, programmed in PciCfgSpace + FCh
  {25, 2, SiPchIntC, 34}, // SerialIo UART Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[9]
  {23, 0, SiPchIntA, 16}, // SATA Controller, INTA is default, programmed in PciCfgSpace + 3Dh
  {22, 0, SiPchIntA, 16}, // CSME: HECI #1
  {22, 1, SiPchIntB, 17}, // CSME: HECI #2
  {22, 4, SiPchIntA, 16}, // CSME: HECI #3
  {21, 0, SiPchIntA, 16}, // SerialIo I2C Controller #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[1]
  {21, 1, SiPchIntB, 17}, // SerialIo I2C Controller #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[2]
  {21, 2, SiPchIntC, 18}, // SerialIo I2C Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[3]
  {21, 3, SiPchIntD, 19}, // SerialIo I2C Controller #3, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[4]
  {20, 0, SiPchIntA, 16}, // USB 3.0 xHCI Controller, no default value, programmed in PciCfgSpace 3Dh
  {20, 1, SiPchIntB, 17}, // USB Device Controller (OTG)
  {20, 3, SiPchIntA, 16}, // CNVi WiFir
  {20, 5, SiPchIntD, 19}, // SCS: SDCard
  {18, 5, SiPchIntA, 16}, // SCS: UFS

  /* from mPchLpOnlyDevIntConfig */
  {31, 6, SiPchIntA, 16}, // GbE Controller, INTA is default, programmed in PciCfgSpace 3Dh
  {29, 0, SiPchIntA, 16}, // PCI Express Port 9, INT is default, programmed in PciCfgSpace + FCh
  {29, 1, SiPchIntB, 17}, // PCI Express Port 10, INT is default, programmed in PciCfgSpace + FCh
  {29, 2, SiPchIntC, 18}, // PCI Express Port 11, INT is default, programmed in PciCfgSpace + FCh
  {29, 3, SiPchIntD, 19}, // PCI Express Port 12, INT is default, programmed in PciCfgSpace + FCh
  {29, 4, SiPchIntA, 16}, // PCI Express Port 13, INT is default, programmed in PciCfgSpace + FCh
  {29, 5, SiPchIntB, 17}, // PCI Express Port 14, INT is default, programmed in PciCfgSpace + FCh
  {29, 6, SiPchIntC, 18}, // PCI Express Port 15, INT is default, programmed in PciCfgSpace + FCh
  {29, 7, SiPchIntD, 19}, // PCI Express Port 16, INT is default, programmed in PciCfgSpace + FCh
  {27, 0, SiPchIntA, 16}, //
  {27, 1, SiPchIntB, 17}, //
  {27, 2, SiPchIntC, 18}, //
  {27, 3, SiPchIntD, 19}, //
  {27, 4, SiPchIntA, 16}, //
  {27, 5, SiPchIntB, 17}, //
  {27, 6, SiPchIntC, 18}, //
  {27, 7, SiPchIntD, 19}, //
  {22, 2, SiPchIntC, 18}, // CSME: IDE-Redirection (IDE-R)
  {22, 3, SiPchIntD, 19}, // CSME: Keyboard and Text (KT) Redirection
  {19, 0, SiPchIntA, 20}, // Integrated Sensor Hub
  {18, 0, SiPchIntA, 16}, // Thermal Subsystem
  {18, 6, SiPchIntB, 24}  // SerialIo: SPI #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[12]
};

CONST SI_PCH_DEVICE_INTERRUPT_CONFIG mPchLpDevIntConfig[] = {
  {31, 3, SiPchIntA, 16}, // HF Audio (Audio, Voice, Speach)
  {31, 4, SiPchIntA, 16}, // SMBus Controller
  {31, 6, SiPchIntA, 16}, // GbE Controller
  {31, 7, SiPchIntA, 16}, // TraceHub
  {30, 0, SiPchIntA, 20}, // UART #0
  {30, 1, SiPchIntB, 21}, // UART #1
  {30, 2, SiPchIntC, 22}, // SPI #0
  {30, 3, SiPchIntD, 23}, // SPI #1
  {29, 0, SiPchIntA, 16}, // PCI Express Port 9
  {29, 1, SiPchIntB, 17}, // PCI Express Port 10
  {29, 2, SiPchIntC, 18}, // PCI Express Port 11
  {29, 3, SiPchIntD, 19}, // PCI Express Port 12
  {29, 4, SiPchIntA, 16}, // PCI Express Port 13
  {29, 5, SiPchIntB, 17}, // PCI Express Port 14
  {29, 6, SiPchIntC, 18}, // PCI Express Port 15
  {29, 7, SiPchIntD, 19}, // PCI Express Port 16
  {28, 0, SiPchIntA, 16}, // PCI Express Port 1
  {28, 1, SiPchIntB, 17}, // PCI Express Port 2
  {28, 2, SiPchIntC, 18}, // PCI Express Port 3
  {28, 3, SiPchIntD, 19}, // PCI Express Port 4
  {28, 4, SiPchIntA, 16}, // PCI Express Port 5
  {28, 5, SiPchIntB, 17}, // PCI Express Port 6
  {28, 6, SiPchIntC, 18}, // PCI Express Port 7
  {28, 7, SiPchIntD, 19}, // PCI Express Port 8
  {26, 0, SiPchIntA, 16}, // eMMC
  {25, 0, SiPchIntA, 32}, // I2C Controller #4
  {25, 1, SiPchIntB, 33}, // I2C Controller #5
  {25, 2, SiPchIntC, 34}, // UART #2
  {23, 0, SiPchIntA, 16}, // SATA Controller
  {22, 0, SiPchIntA, 16}, // MEI HECI #1
  {22, 1, SiPchIntB, 17}, // MEI HECI #2
  {22, 2, SiPchIntC, 18}, // IDE-Redirection (IDE-R)
  {22, 3, SiPchIntD, 19}, // Keyboard and Text (KT) Redirection
  {22, 4, SiPchIntA, 16}, // MEI HECI #3
  {22, 5, SiPchIntB, 17}, // MEI HECI #4
  {21, 0, SiPchIntA, 16}, // I2C Controller #0
  {21, 1, SiPchIntB, 17}, // I2C Controller #1
  {21, 2, SiPchIntC, 18}, // I2C Controller #2
  {21, 3, SiPchIntD, 19}, // I2C Controller #3
  {20, 0, SiPchIntA, 16}, // USB 3.0 xHCI Controller
  {20, 1, SiPchIntB, 17}, // USB Device Controller (OTG)
  {20, 3, SiPchIntA, 16}, // CNVi WiFi
  {20, 5, SiPchIntD, 19}, // SDXC
  {19, 0, SiPchIntA, 20}, // Integrated Sensor Hub
  {18, 0, SiPchIntA, 16}, // Thermal Subsystem
  {18, 5, SiPchIntA, 16}, // UFS
  {18, 6, SiPchIntB, 24}  // SPI #2
};

CONST UINT8 mPchSerialIoDevMode[PCH_MAX_SERIALIO_CONTROLLERS] = {
  1 /* I2C0 */,  1 /* I2C1 */,  0 /* I2C2 */, 0 /* I2C3  */, 0 /* I2C4  */, 0 /* I2C5  */,
  0 /* SPI0 */,  0 /* SPI1 */,  0 /* SPI2 */, 1 /* UART0 */, 0 /* UART1 */, 3 /* UART2 */
};

CONST UINT8 mPchLpSerialIoDevMode[PCH_MAX_SERIALIO_CONTROLLERS] = {
  1 /* I2C0 */,  1 /* I2C1 */,  0 /* I2C2 */, 0 /* I2C3  */, 0 /* I2C4  */, 0 /* I2C5  */,
  0 /* SPI0 */,  0 /* SPI1 */,  0 /* SPI2 */, 0 /* UART0 */, 0 /* UART1 */, 0 /* UART2 */
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
  Clear SMI sources

**/
VOID
ClearSmi (
  VOID
)
{
  UINT32                SmiEn;
  UINT32                SmiSts;

  SmiEn = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
  if (((SmiEn & B_ACPI_IO_SMI_EN_GBL_SMI) !=0) && ((SmiEn & B_ACPI_IO_SMI_EN_EOS) !=0)) {
    return;
  }

  //
  // Clear the status before setting smi enable
  //
  SmiSts = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4));
  SmiSts |=
    (
      B_ACPI_IO_SMI_STS_SMBUS |
      B_ACPI_IO_SMI_STS_PERIODIC |
      B_ACPI_IO_SMI_STS_TCO |
      B_ACPI_IO_SMI_STS_SWSMI_TMR |
      B_ACPI_IO_SMI_STS_APM |
      B_ACPI_IO_SMI_STS_ON_SLP_EN |
      B_ACPI_IO_SMI_STS_BIOS
    );
  IoWrite32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4), SmiSts);
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
PatchCpuIstTable (
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
  This function updates CpuSsdt table PNVS/LPSS dynamically.

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
  CurrPtr     = (UINT8 *) Table;
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
  Update bootloader reserved region.

  This function will erase bootloader reserved region when statemachine flag
  indicates a value of 0xFC.

  This condition will occur when power cycle happens in between clearing
  IBB signal and setting state machine flag to 0xFF.

  @retval  EFI_SUCCESS           The operation completed successfully.
  @retval  EFI_NOT_FOUND         Flash map pointer not founc
  @retval  others                Error setting state machine flag to 0xFF.
**/
EFI_STATUS
UpdateBlRsvdRegion ()
{
  UINT32      RsvdBase;
  UINT32      RsvdSize;
  EFI_STATUS  Status;
  FLASH_MAP     *FlashMap;
  FW_UPDATE_STATUS  FwUpdStatus;

  //
  // Get flash map pointer
  //
  FlashMap = GetFlashMapPtr();
  if (FlashMap == NULL) {
    DEBUG((DEBUG_ERROR, "Could not get flash map\n"));
    return EFI_NOT_FOUND;
  }

  //
  // If StateMachine is 0xFC, erase the bootloader reserved region
  //
  Status = GetComponentInfo(FLASH_MAP_SIG_BLRESERVED, &RsvdBase, &RsvdSize);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Getting component info for bl rsvd region failed with status: %r\n", Status));
    return Status;
  }

  CopyMem (&FwUpdStatus, (VOID *)(UINTN)RsvdBase, sizeof(FW_UPDATE_STATUS));

  if (FwUpdStatus.StateMachine == FW_UPDATE_SM_PART_AB) {
    Status = SpiFlashErase (FlashRegionBios, FlashMap->RomSize - (~RsvdBase + 1), RsvdSize);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "Erasing Bootloader Reserved region failed with status: %r\n", Status));
      return Status;
    }
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
  CHAR8                *SmbiosStrTbl;
  UINT16                PlatformId;
  UINTN                 Length;
  CHAR8                *TempSmbiosStrTbl;
  BOOT_LOADER_VERSION  *VerInfoTbl;

  PlatformId    = GetPlatformId ();
  TempSmbiosStrTbl  = (CHAR8 *) AllocateTemporaryMemory (0);
  SmbiosStrTbl = TempSmbiosStrTbl;

  //
  // SMBIOS_TYPE_BIOS_INFORMATION
  //
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BIOS_INFORMATION, 1, "Intel Corporation");

  VerInfoTbl = GetVerInfoPtr ();
  if (VerInfoTbl != NULL) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf),
      "SB_CFL.%03d.%03d.%03d.%03d.%03d.%05d.%c-%016lX%a\0",
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
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BIOS_INFORMATION, 2, TempStrBuf);
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BIOS_INFORMATION, 3, __DATE__);

  //
  // SMBIOS_TYPE_SYSTEM_INFORMATION
  //
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    1, "Intel Corporation");
  if ((PlatformId == PLATFORM_ID_CFL_S) || (PlatformId == PLATFORM_ID_CFL_H)) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "CoffeeLake Client Platform");
  } else if (PlatformId == PLATFORM_ID_WHL) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "WhiskeyLake Client Platform");
  } else {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Unknown");
  }
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    2, TempStrBuf);
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    3, "0.1");
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    4, "System Serial Number");
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    5, "System SKU Number");
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_SYSTEM_INFORMATION,
    6, "CannonLake Client System");

  //
  // SMBIOS_TYPE_BASEBOARD_INFORMATION
  //
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BASEBOARD_INFORMATION,
    1, "Intel Corporation");
  if (PlatformId == PLATFORM_ID_CFL_S) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "CoffeeLake S 82 UDIMM RVP");
  } else if (PlatformId == PLATFORM_ID_CFL_H) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "CoffeeLake H DDR4 RVP");
  } else if (PlatformId == PLATFORM_ID_WHL) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "WhiskeyLake U DDR4 ERB");
  } else {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Unknown");
  }
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BASEBOARD_INFORMATION,
    2, TempStrBuf);
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BASEBOARD_INFORMATION,
    3, "1");
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BASEBOARD_INFORMATION,
    4, "Board Serial Number");

  //
  // SMBIOS_TYPE_PROCESSOR_INFORMATION : TBD
  //

  //
  // SMBIOS_TYPE_END_OF_TABLE
  //
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_END_OF_TABLE,
    0, NULL);

  Length = SmbiosStrTbl - TempSmbiosStrTbl;
  SmbiosStrTbl = AllocatePool (Length);
  if (SmbiosStrTbl == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (SmbiosStrTbl, TempSmbiosStrTbl, Length);
  (VOID) PcdSet32S (PcdSmbiosStringsPtr, (UINT32)(UINTN)SmbiosStrTbl);

  return EFI_SUCCESS;
}


/**
  Print the output of the GPIO Config table that was read from CfgData.

  @param GpioPinNum           Number of GPIO entries in the table.

  @param GpioConfData         GPIO Config Data that was read from the Configuration region either from internal or external source.

**/
VOID
PrintGpioConfigTable (
  IN UINT32              GpioPinNum,
  IN VOID*               GpioConfData
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
  UINT32            *GpioItem;
  GPIO_PAD_FIELD    GpioPad = { 0, 0, 0, 0 };

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
  Configure GPIOs

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid group or pad number
  @retval EFI_NOT_FOUND                 GPIO configuration data not found
  #retval EFI_LOAD_ERROR                GPIO configuration data corrupted
**/
EFI_STATUS
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
  UINT8              ChipsetId;

  //Find the GPIO CFG HDR
  GpioCfgCurrHdr = (GPIO_CFG_HDR *)FindConfigDataByTag (CDATA_GPIO_TAG);
  if (GpioCfgCurrHdr == NULL) {
    return EFI_NOT_FOUND;
  }

  GpioEntries    = 0;
  GpioCfgBaseHdr = NULL;

  //Find the GPIO CFG Data based on Platform ID. GpioTableData is the start of the GPIO entries
  if (GpioCfgCurrHdr->GpioBaseTableId < 16) {
    GpioCfgBaseHdr = (GPIO_CFG_HDR *)FindConfigDataByPidTag (GpioCfgCurrHdr->GpioBaseTableId, CDATA_GPIO_TAG);
    if (GpioCfgBaseHdr == NULL) {
      DEBUG ((DEBUG_ERROR, "Cannot find base GPIO table for platform ID %d\n", GpioCfgCurrHdr->GpioBaseTableId));
      return EFI_NOT_FOUND;
    }
    if (GpioCfgCurrHdr->GpioItemSize != GpioCfgBaseHdr->GpioItemSize) {
      DEBUG ((DEBUG_ERROR, "Inconsistent GPIO item size\n"));
      return EFI_LOAD_ERROR;
    }
    GpioCfgHdr = GpioCfgBaseHdr;
  } else {
    GpioCfgHdr = GpioCfgCurrHdr;
  }

  Offset     = 0;
  GpioTable  = (UINT8 *)AllocateTemporaryMemory (0);  //allocate new buffer
  GpioCfgDataBuffer = GpioTable;

  ChipsetId = CNL_UNKNOWN_CHIPSET_ID;
  if (IsPchH ()) {
    ChipsetId = CNL_H_CHIPSET_ID;
  } else if (IsPchLp ()) {
    ChipsetId = CNL_LP_CHIPSET_ID;
  } else {
    CpuHalt (NULL);
  }

  for (Index = 0; Index  < GpioCfgHdr->GpioItemCount; Index++) {
    if (GpioCfgCurrHdr->GpioBaseTableBitMask[Index >> 3] & (1 << (Index & 7))) {
      GpioTable = FillGpioTable (GpioTable, GpioCfgHdr, Offset, ChipsetId);
      GpioEntries++;
    }
    Offset += GpioCfgHdr->GpioItemSize;
  }

  Offset = 0;
  if (GpioCfgBaseHdr != NULL) {
    for (Index = 0; Index  < GpioCfgCurrHdr->GpioItemCount; Index++) {
      GpioTable = FillGpioTable (GpioTable, GpioCfgCurrHdr, Offset, ChipsetId);
      GpioEntries++;
      Offset += GpioCfgCurrHdr->GpioItemSize;
    }
  }

  DEBUG_CODE_BEGIN ();
  PrintGpioConfigTable (GpioEntries, GpioCfgDataBuffer);
  DEBUG_CODE_END ();

  return GpioConfigurePads (GpioEntries, (GPIO_INIT_CONFIG *) GpioCfgDataBuffer);
}

/**
  Update current boot Payload ID.

**/
VOID
UpdatePayloadId (
  VOID
  )
{
  EFI_STATUS      Status;
  GEN_CFG_DATA    *GenericCfgData;
  SILICON_CFG_DATA  *SiliconCfgData;
  UINT32          PayloadSelGpioData;
  UINT32          PayloadSelGpioPad;
  UINT32          PayloadId;

  PayloadId = GetPayloadId ();
  GenericCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenericCfgData != NULL) {
    if (GenericCfgData->PayloadId == AUTO_PAYLOAD_ID_SIGNATURE) {
      PayloadId = 0;
    } else {
      PayloadId = GenericCfgData->PayloadId;
    }
  }

  //
  // Switch payloads based on configured GPIO pin
  //
  SiliconCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if ((SiliconCfgData != NULL) && (SiliconCfgData->PayloadSelGpio.Enable != 0)){
    if (IsPchLp() == TRUE) {
      PayloadSelGpioPad = GPIO_CFG_PIN_TO_PAD(SiliconCfgData->PayloadSelGpio) | (GPIO_CNL_LP_CHIPSET_ID << 24);
    } else {
      PayloadSelGpioPad = GPIO_CFG_PIN_TO_PAD(SiliconCfgData->PayloadSelGpio) | (GPIO_CNL_H_CHIPSET_ID << 24);
    }
    Status = GpioGetInputValue (PayloadSelGpioPad, &PayloadSelGpioData);
    if (!EFI_ERROR (Status)) {
      if (PayloadSelGpioData == 1) {
        PayloadId = 0;
      } else {
        if ((GenericCfgData != NULL) && (GenericCfgData->PayloadId == AUTO_PAYLOAD_ID_SIGNATURE)) {
          PayloadId = UEFI_PAYLOAD_ID_SIGNATURE;
        }
      }
      DEBUG ((DEBUG_INFO, "Set PayloadId to 0x%08X based on GPIO config\n", PayloadId));
    }
  }

  SetPayloadId (PayloadId);
}

/**
  Build VT-d information to prepare PMR program

**/
STATIC
VOID
BuildVtdInfo (
  VOID
  )
{
  VTD_INFO     *VtdInfo;
  UINTN         McD0BaseAddress;
  UINT32        MchBar;
  UINT32        Idx;
  UINT32        VtdIdx;
  UINT32        Data;
  UINT32        RegOff[2] = {R_SA_MCHBAR_VTD1_OFFSET, R_SA_MCHBAR_VTD3_OFFSET};

  VtdInfo = &((PLATFORM_DATA *)GetPlatformDataPtr ())->VtdInfo;
  McD0BaseAddress = MM_PCI_ADDRESS (SA_MC_BUS, 0, 0, 0);
  MchBar          = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR) & ~BIT0;
  VtdInfo->HostAddressWidth = 39;

  VtdIdx = 0;
  for (Idx = 0; Idx < ARRAY_SIZE(RegOff); Idx++) {
    Data = MmioRead32 (MchBar + RegOff[Idx]) & ~3;
    if (Data != 0) {
      DEBUG ((DEBUG_INFO, "VT-d Engine %d @ 0x%08X\n", VtdIdx, Data));
      VtdInfo->VTdEngineAddress[VtdIdx++] = Data;
      ASSERT (VtdIdx <= ARRAY_SIZE(VtdInfo->VTdEngineAddress));
    }
  }

  VtdInfo->VTdEngineCount = VtdIdx;
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
  Initialize Board specific things in Stage2 Phase

  @param[in]  InitPhase            Indicates a board init phase to be initialized

**/
VOID
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE    InitPhase
)
{
  SILICON_CFG_DATA  *SiliconCfgData;
  EFI_STATUS      Status;
  UINT32          RgnBase;
  UINT32          RgnSize;
  UINTN           LpcBase;
  UINTN           SpiBaseAddress;
  UINT16          TcoBase;
  UINT32          SaMcAddress;
  UINT32          AddressPort;
  UINTN           SpiBar0;
  UINT32          Length;
  UINT32          TsegBase;
  UINT32          TsegSize;
  BL_SW_SMI_INFO            *BlSwSmiInfo;
  VTD_INFO                  *VtdInfo;
  EFI_PEI_GRAPHICS_INFO_HOB *FspGfxHob;
  VOID                      *FspHobList;

  switch (InitPhase) {
  case PreSiliconInit:
    EnableLegacyRegions ();
    GpioInit ();
    SpiConstructor ();

    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      UpdatePayloadId ();
      UpdateBlRsvdRegion ();
    }
    Status = GetComponentInfo (FLASH_MAP_SIG_VARIABLE, &RgnBase, &RgnSize);
    if (!EFI_ERROR(Status)) {
      VariableConstructor (RgnBase, RgnSize);
    }
    break;
  case PostSiliconInit:
    // Set TSEG base/size PCD
    TsegBase = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + TSEG) & ~0xF;
    TsegSize = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + BGSM) & ~0xF;
    TsegSize -= TsegBase;
    (VOID) PcdSet32S (PcdSmramTsegBase, TsegBase);
    (VOID) PcdSet32S (PcdSmramTsegSize, (UINT32)TsegSize);

    //
    // Reinitialize PCI bus master and memory space for Graphics device
    // Reinitialize the BAR for Graphics device
    //
    if (PcdGetBool (PcdFramebufferInitEnabled)) {
      FspGfxHob = NULL;
      FspHobList = GetFspHobListPtr ();
      if (FspHobList != NULL) {
        FspGfxHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (FspHobList, &Length,
                      &gEfiGraphicsInfoHobGuid);
      }
      if (FspGfxHob != NULL) {
        PciAnd8 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_COMMAND_OFFSET), \
                   (UINT8)(~EFI_PCI_COMMAND_BUS_MASTER));
        PciWrite32 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_BASE_ADDRESSREG_OFFSET + 0x8), \
                   (UINT32)FspGfxHob->FrameBufferBase);
        //
        // Programming dummy value for the BAR
        // This is just to no leave the device without BAR, this value will be reprogrammed during
        // PCI enumeration.
        //
        PciWrite32 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_BASE_ADDRESSREG_OFFSET), \
                   (UINT32)0xC0000000);
        PciWrite8 (PCI_LIB_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_COMMAND_OFFSET), \
                   EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
      }
    }

    //
    // SMRAMC LOCK must use CF8/CFC access
    //
    SaMcAddress = PCI_CF8_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC);
    AddressPort = IoRead32 (0xCF8);
    IoWrite32 (0xCF8, PCI_TO_CF8_ADDRESS (SaMcAddress));
    IoOr8 (0xCFC + (UINT16)(SaMcAddress & 3), BIT4 );
    IoWrite32 (0xCF8, AddressPort);

    //
    // Initialize Smbios Info for SmbiosInit
    //
    if (FeaturePcdGet (PcdSmbiosEnabled)) {
      InitializeSmbiosInfo ();
    }

    // Enable DMA protection
    if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
      BuildVtdInfo ();
      VtdInfo = &((PLATFORM_DATA *)GetPlatformDataPtr ())->VtdInfo;
      SetDmaProtection (VtdInfo, TRUE);
    }
    break;
  case PrePciEnumeration:
    break;
  case PostPciEnumeration:
    if (GetBootMode() == BOOT_ON_S3_RESUME) {
      //
      // Clear Smi
      //
      ClearSmi ();
      RestoreS3RegInfo (FindS3Info (S3_SAVE_REG_COMM_ID));
      BlSwSmiInfo = NULL;

      //
      // If UEFI payload registered a software SMI handler
      // for bootloader to restore SMRR base and mask in
      // S3 resume path, trigger sw smi
      //
      BlSwSmiInfo = FindS3Info (BL_SW_SMI_COMM_ID);
      if (BlSwSmiInfo != NULL) {
        TriggerPayloadSwSmi (BlSwSmiInfo->BlSwSmiHandlerInput);
      }
    }
    break;
  case PrePayloadLoading:
    IgdOpRegionPlatformInit();
    if (GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE && GetBootMode() != BOOT_ON_S3_RESUME) {
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
    break;
  case EndOfStages:
    // Register Heci Service
    HeciRegisterHeciService ();
    // Lock down SPI for all other payload entry except FWUpdate and OSloader
    // as this phase is too early for them to lock it here
    SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
    SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() != 0)) {
      // Set the BIOS Lock Enable and EISS bits
      MmioOr8 (SpiBaseAddress + R_SPI_BCR, (UINT8) (B_SPI_BCR_BLE | B_SPI_BCR_EISS));
    }
    break;
  case ReadyToBoot:
    //
    // Do necessary locks, and clean up before jumping tp OS
    //
    SiliconCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
    if ( (SiliconCfgData != NULL) && (SiliconCfgData->ECEnable == 1)){
      //
      // Enable decoding of I/O locations 62h and 66h to LPC
      //
      LpcBase = MmPciBase (0, PCI_DEVICE_NUMBER_PCH_LPC, 0);
      MmioOr16 (LpcBase + R_LPC_CFG_IOE, B_LPC_CFG_IOE_ME1);

      //
      // Enable EC's ACPI mode to control power to motherboard during Sleep (S3)
      //
      IoWrite16 (EC_C_PORT, EC_C_ACPI_ENABLE);
    }

    // Lock down SPI for everything
    SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
    SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

    // Set the BIOS Lock Enable and EISS bits
    MmioOr8 (SpiBaseAddress + R_SPI_BCR, (UINT8) (B_SPI_BCR_BLE | B_SPI_BCR_EISS));

    //
    // Set Bios Interface Lock-Down (BILD)
    // B0:D31:F05  Offset DCh Bit 07
    //

    // Set the BILD
    MmioOr8 (SpiBaseAddress + R_SPI_BCR, (UINT8)B_SPI_BCR_BILD);

    //
    // set Flash Configuration Lock-Down (FLOCKDN)
    // B0:D31:F05  Offset 04h Bit 15
    //
    SpiBar0 = (UINTN)AcquireSpiBar0 (SpiBaseAddress);
    if (SpiBar0 != 0) {
      MmioOr16 (SpiBar0 + R_SPI_HSFS, (UINT16)B_SPI_HSFS_FLOCKDN);
    }

    //
    // set SMI LOCK (SMI_LOCK)
    // B0:D31:F02  Offset 1024h Bit 4
    //
    MmioOr8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B, (UINT8)B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK);

    //
    // Lock down Tco WDT just before handling off to OS
    //
    TcoBase = MmioRead16(PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_TCOBASE)) & B_PCH_DMI_PCR_TCOBASE_TCOBA;
    IoOr16 ((TcoBase + R_TCO_IO_TCO1_CNT), B_TCO_IO_TCO1_CNT_LOCK);
    break;
  case EndOfFirmware:
    ClearFspHob ();
    if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
      // Disable DMA protection
      VtdInfo = &((PLATFORM_DATA *)GetPlatformDataPtr ())->VtdInfo;
      SetDmaProtection (VtdInfo, FALSE);
    }
    break;
  default:
    break;
  }
}

/**
  Finds PCI Function Number of SerialIo devices.

  @param[in] SerialIoNumber             Serial IO device

  @retval                               SerialIo irq number
**/
UINT8
FindSerialIoIrq (
  IN PCH_SERIAL_IO_CONTROLLER  Device
  )
{
  UINT8                           DevNum;
  UINT8                           FuncNum;
  UINT8                           Index;
  UINT8                           Count;
  UINT8                           Irq;
  CONST SI_PCH_DEVICE_INTERRUPT_CONFIG  *IntConfig;

  DevNum  = GetSerialIoDeviceNumber (Device);
  FuncNum = GetSerialIoFunctionNumber (Device);

  if (IsPchLp() == TRUE) {
    IntConfig = mPchLpDevIntConfig;
    Count     = ARRAY_SIZE (mPchLpDevIntConfig);
  } else {
    IntConfig = mPchDevIntConfig;
    Count     = ARRAY_SIZE (mPchDevIntConfig);
  }

  for (Irq = 0, Index = 0; Index < Count; Index ++) {
    if ((IntConfig[Index].Device == DevNum) && (IntConfig[Index].Function == FuncNum)) {
      Irq = IntConfig[Index].Irq;
      break;
    }
  }

  return Irq;
}

/**
  Update FSP-S UPD config data

  @param[in]  FspsUpdPtr           The pointer to the FSP-S UPD to be updated.

**/
VOID
EFIAPI
UpdateFspConfig (
  IN  VOID     *FspsUpdPtr
)
{
  FSPS_UPD                  *FspsUpd;
  UINT32                    Index;
  UINT32                    Length;
  UINT32                    BaseAddress;
  UINT32                    RegionSize;
  EFI_STATUS                Status;
  FSP_S_TEST_CONFIG         *FspsTestConfig;
  UINT32                    *HdaVerbTablePtr;
  UINT8                     HdaVerbTableNum;
  UINT16                    PlatformId;
  PCIE_CFG_DATA             *PcieCfgData;
  GPU_CFG_DATA              *GpuCfgData;
  UINT8                     Data8;
  SILICON_CFG_DATA          *SiliconCfgData;
  POWER_CFG_DATA            *PowerCfgData;

  PlatformId                              = GetPlatformId ();
  FspsUpd                                 = (FSPS_UPD *) FspsUpdPtr;

  FspsUpd->FspsConfig.PchPmSlpS3MinAssert       = 0;
  FspsUpd->FspsConfig.PchPmSlpS4MinAssert       = 0;
  FspsUpd->FspsConfig.PchPmSlpSusMinAssert      = 0;
  FspsUpd->FspsConfig.PchPmSlpAMinAssert        = 0;
  FspsUpd->FspsConfig.PchPmLpcClockRun          = 1;
  FspsUpd->FspsConfig.EnableTcoTimer            = 0;
  FspsUpd->FspsConfig.Enable8254ClockGating     = 0;
  FspsUpd->FspsConfig.Enable8254ClockGatingOnS3 = 0;

  FspsUpd->FspsConfig.ScsEmmcEnabled      = 1;
  FspsUpd->FspsConfig.ScsEmmcHs400Enabled = (UINT8)((FeaturePcdGet (PcdEmmcHs400SupportEnabled) != 0) ? 1 : 0);
  FspsUpd->FspsConfig.ScsSdCardEnabled    = 0;
  FspsUpd->FspsConfig.ScsUfsEnabled       = 0;

  FspsUpd->FspsConfig.SataPwrOptEnable    = 1;

  if (IsPchLp()) {
    FspsUpd->FspsConfig.DevIntConfigPtr   = (UINT32)(UINTN) mPchLpDevIntConfig;
    FspsUpd->FspsConfig.NumOfDevIntConfig = ARRAY_SIZE (mPchLpDevIntConfig);
  } else {
    FspsUpd->FspsConfig.DevIntConfigPtr   = (UINT32)(UINTN) mPchDevIntConfig;
    FspsUpd->FspsConfig.NumOfDevIntConfig = ARRAY_SIZE (mPchDevIntConfig);
    FspsUpd->FspsConfig.EnableTcoTimer    = 1;
  }

  FspsUpd->FspsConfig.GpioIrqRoute        = DEFAULT_GPIO_IRQ_ROUTE;
  FspsUpd->FspsConfig.SciIrqSelect        = 9;
  FspsUpd->FspsConfig.TcoIrqEnable        = 0;
  FspsUpd->FspsConfig.TcoIrqSelect        = 9;

  HdaVerbTablePtr = (UINT32 *) AllocateZeroPool (6 * sizeof (UINT32));  // max 6 tables supported for now
  if (HdaVerbTablePtr == NULL) {
    return;
  }
  HdaVerbTableNum = 0;
  HdaVerbTablePtr[HdaVerbTableNum++]   = (UINT32)(UINTN) &HdaVerbTableDisplayAudio;
  if (IsPchLp()) {
    HdaVerbTablePtr[HdaVerbTableNum++] = (UINT32)(UINTN) &WhlHdaVerbTableAlc700;
  } else {
    HdaVerbTablePtr[HdaVerbTableNum++] = (UINT32)(UINTN) &CflSHdaVerbTableAlc700;
  }
  FspsUpd->FspsConfig.PchHdaVerbTablePtr      = (UINT32)(UINTN) HdaVerbTablePtr;
  FspsUpd->FspsConfig.PchHdaVerbTableEntryNum = HdaVerbTableNum;

  if (PcdGetBool (PcdFramebufferInitEnabled)) {
    FspsUpd->FspsConfig.GraphicsConfigPtr   = (UINT32)GetVbtAddress ();
  } else {
    FspsUpd->FspsConfig.GraphicsConfigPtr = 0;
  }

  CopyMem (&FspsUpd->FspsConfig.PxRcConfig, mPxRcConfig, sizeof(mPxRcConfig));

  //
  // SerialIo config
  //
  if (IsPchLp()) {
    CopyMem (&FspsUpd->FspsConfig.SerialIoDevMode, mPchLpSerialIoDevMode, PCH_MAX_SERIALIO_CONTROLLERS);
  } else {
    CopyMem (&FspsUpd->FspsConfig.SerialIoDevMode, mPchSerialIoDevMode, PCH_MAX_SERIALIO_CONTROLLERS);
  }

  // Set debug uart in PCI mode
  Data8 = GetDebugPort ();
  if (Data8 < PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    FspsUpd->FspsConfig.SerialIoDevMode[PCH_MAX_SERIALIO_I2C_CONTROLLERS + PCH_MAX_SERIALIO_SPI_CONTROLLERS + Data8] = 1;
  }

  FspsUpd->FspsConfig.SerialIoSpiCsPolarity[0]              = 1;
  FspsUpd->FspsConfig.SerialIoSpiCsPolarity[1]              = 0;
  FspsUpd->FspsConfig.SerialIoSpiCsPolarity[2]              = 0;
  FspsUpd->FspsConfig.SerialIoUartHwFlowCtrl[0]             = 1;
  FspsUpd->FspsConfig.SerialIoUartHwFlowCtrl[1]             = 1;
  FspsUpd->FspsConfig.SerialIoUartHwFlowCtrl[2]             = 1;
  FspsUpd->FspsConfig.PchSerialIoI2cPadsTermination[0]      = 1;
  FspsUpd->FspsConfig.PchSerialIoI2cPadsTermination[1]      = 1;
  FspsUpd->FspsConfig.PchSerialIoI2cPadsTermination[2]      = 1;
  FspsUpd->FspsConfig.PchSerialIoI2cPadsTermination[3]      = 1;
  FspsUpd->FspsConfig.PchSerialIoI2cPadsTermination[4]      = 1;
  FspsUpd->FspsConfig.PchSerialIoI2cPadsTermination[5]      = 1;

  //
  // USB config
  //
  FspsUpd->FspsConfig.XdciEnable                            = FALSE;
  FspsUpd->FspsConfig.PchEnableComplianceMode               = FALSE;
  FspsUpd->FspsConfig.UsbPdoProgramming                     = TRUE;
  FspsUpd->FspsConfig.PchUsbOverCurrentEnable               = TRUE;
  FspsUpd->FspsConfig.PchUsb2PhySusPgEnable                 = TRUE;
  FspsUpd->FspsTestConfig.PchXhciOcLock                     = TRUE;

  Length = GetPchXhciMaxUsb2PortNum ();
  for (Index = 0; Index < Length; Index++) {
    FspsUpd->FspsConfig.PortUsb20Enable[Index]              = TRUE;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[Index]           = UsbOverCurrentPinMax;
    FspsUpd->FspsConfig.Usb2AfePetxiset[Index]              = 7;
    FspsUpd->FspsConfig.Usb2AfeTxiset[Index]                = 5;
    FspsUpd->FspsConfig.Usb2AfePredeemp[Index]              = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[Index]             = 0;
  }

  if (PlatformId == PLATFORM_ID_UPXTREME) {
    // Workaround for USB issue on port 8, it does not respond to USB enumeration.
    // Disable this port for now
    FspsUpd->FspsConfig.PortUsb20Enable[7] = FALSE;
  }

  Length = GetPchXhciMaxUsb3PortNum ();
  for (Index = 0; Index < Length; Index++) {
    FspsUpd->FspsConfig.PortUsb30Enable[Index]              = TRUE;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[Index]           = UsbOverCurrentPinMax;
    FspsUpd->FspsConfig.Usb3HsioTxDeEmphEnable[Index]       = 0;
    FspsUpd->FspsConfig.Usb3HsioTxDeEmph[Index]             = 0;
    FspsUpd->FspsConfig.Usb3HsioTxDownscaleAmpEnable[Index] = 0;
    FspsUpd->FspsConfig.Usb3HsioTxDownscaleAmp[Index]       = 0;
    FspsUpd->FspsConfig.PchUsbHsioRxTuningEnable[Index]     = 0;
    FspsUpd->FspsConfig.PchUsbHsioRxTuningParameters[Index] = 3;
    FspsUpd->FspsConfig.PchUsbHsioFilterSel[Index]          = 0;
  }

  if (IsPchLp()) {
    FspsUpd->FspsConfig.Usb2OverCurrentPin[0]               = UsbOverCurrentPin2;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[1]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[2]               = UsbOverCurrentPin2;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[3]               = UsbOverCurrentPin2;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[4]               = UsbOverCurrentPin3;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[5]               = UsbOverCurrentPin3;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[6]               = UsbOverCurrentPin3;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[7]               = UsbOverCurrentPin3;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[8]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[9]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[10]              = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[11]              = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[12]              = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[13]              = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[14]              = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[15]              = UsbOverCurrentPinSkip;

    FspsUpd->FspsConfig.Usb3OverCurrentPin[0]               = UsbOverCurrentPin2;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[1]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[2]               = UsbOverCurrentPin2;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[3]               = UsbOverCurrentPin2;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[4]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[5]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[6]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[7]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[8]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[9]               = UsbOverCurrentPinSkip;

    Length = GetPchUsb2MaxPhysicalPortNum ();
    for (Index = 0; Index < Length; Index++) {
      FspsUpd->FspsConfig.Usb2AfePetxiset[Index]              = 6;
      FspsUpd->FspsConfig.Usb2AfeTxiset[Index]                = 0;
      FspsUpd->FspsConfig.Usb2AfePredeemp[Index]              = 3;
      FspsUpd->FspsConfig.Usb2AfePehalfbit[Index]             = 0;
    }
  } else {
    FspsUpd->FspsConfig.Usb2OverCurrentPin[0]               = UsbOverCurrentPin4;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[1]               = UsbOverCurrentPin0;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[2]               = UsbOverCurrentPin2;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[3]               = UsbOverCurrentPin5;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[4]               = UsbOverCurrentPin5;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[5]               = UsbOverCurrentPin0;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[6]               = UsbOverCurrentPin1;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[7]               = UsbOverCurrentPin1;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[8]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[9]               = UsbOverCurrentPin3;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[10]              = UsbOverCurrentPin3;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[11]              = UsbOverCurrentPin6;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[12]              = UsbOverCurrentPin6;
    FspsUpd->FspsConfig.Usb2OverCurrentPin[13]              = UsbOverCurrentPin0;

    FspsUpd->FspsConfig.Usb3OverCurrentPin[0]               = UsbOverCurrentPin4;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[1]               = UsbOverCurrentPin0;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[2]               = UsbOverCurrentPin2;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[3]               = UsbOverCurrentPin5;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[4]               = UsbOverCurrentPin5;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[5]               = UsbOverCurrentPin0;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[6]               = UsbOverCurrentPin1;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[7]               = UsbOverCurrentPin1;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[8]               = UsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.Usb3OverCurrentPin[9]               = UsbOverCurrentPin3;

    FspsUpd->FspsConfig.Usb2AfePetxiset[0]                  = 7;
    FspsUpd->FspsConfig.Usb2AfeTxiset[0]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[0]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[0]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[1]                  = 6;
    FspsUpd->FspsConfig.Usb2AfeTxiset[1]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[1]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[1]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[2]                  = 6;
    FspsUpd->FspsConfig.Usb2AfeTxiset[2]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[2]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[2]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[3]                  = 6;
    FspsUpd->FspsConfig.Usb2AfeTxiset[3]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[3]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[3]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[4]                  = 6;
    FspsUpd->FspsConfig.Usb2AfeTxiset[4]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[4]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[4]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[5]                  = 6;
    FspsUpd->FspsConfig.Usb2AfeTxiset[5]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[5]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[5]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[6]                  = 7;
    FspsUpd->FspsConfig.Usb2AfeTxiset[6]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[6]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[6]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[7]                  = 7;
    FspsUpd->FspsConfig.Usb2AfeTxiset[7]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[7]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[7]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[8]                  = 6;
    FspsUpd->FspsConfig.Usb2AfeTxiset[8]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[8]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[8]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[9]                  = 6;
    FspsUpd->FspsConfig.Usb2AfeTxiset[9]                    = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[9]                  = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[9]                 = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[10]                 = 6;
    FspsUpd->FspsConfig.Usb2AfeTxiset[10]                   = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[10]                 = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[10]                = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[11]                 = 7;
    FspsUpd->FspsConfig.Usb2AfeTxiset[11]                   = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[11]                 = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[11]                = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[12]                 = 7;
    FspsUpd->FspsConfig.Usb2AfeTxiset[12]                   = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[12]                 = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[12]                = 0;

    FspsUpd->FspsConfig.Usb2AfePetxiset[13]                 = 7;
    FspsUpd->FspsConfig.Usb2AfeTxiset[13]                   = 0;
    FspsUpd->FspsConfig.Usb2AfePredeemp[13]                 = 3;
    FspsUpd->FspsConfig.Usb2AfePehalfbit[13]                = 0;
  }

  GpuCfgData = (GPU_CFG_DATA *) FindConfigDataByTag (CDATA_GPU_TAG);
  if (GpuCfgData != NULL) {
    FspsUpd->FspsConfig.DdiPortEdp  = GpuCfgData->DdiPortEdp;
    FspsUpd->FspsConfig.DdiPortBHpd = GpuCfgData->DdiPortBHpd;
    FspsUpd->FspsConfig.DdiPortCHpd = GpuCfgData->DdiPortCHpd;
    FspsUpd->FspsConfig.DdiPortDHpd = GpuCfgData->DdiPortDHpd;
    FspsUpd->FspsConfig.DdiPortFHpd = GpuCfgData->DdiPortFHpd;
    FspsUpd->FspsConfig.DdiPortBDdc = GpuCfgData->DdiPortBDdc;
    FspsUpd->FspsConfig.DdiPortCDdc = GpuCfgData->DdiPortCDdc;
    FspsUpd->FspsConfig.DdiPortDDdc = GpuCfgData->DdiPortDDdc;
    FspsUpd->FspsConfig.DdiPortFDdc = GpuCfgData->DdiPortFDdc;
    DEBUG ((DEBUG_INFO, "DdiPort Config: %d %d %d %d %d %d %d %d %d\n",
      FspsUpd->FspsConfig.DdiPortEdp,
      FspsUpd->FspsConfig.DdiPortBHpd, FspsUpd->FspsConfig.DdiPortCHpd,
      FspsUpd->FspsConfig.DdiPortDHpd, FspsUpd->FspsConfig.DdiPortFHpd,
      FspsUpd->FspsConfig.DdiPortBDdc, FspsUpd->FspsConfig.DdiPortCDdc,
      FspsUpd->FspsConfig.DdiPortDDdc, FspsUpd->FspsConfig.DdiPortFDdc));
  }

  //
  // PCIE config
  //
  if (PlatformId == PLATFORM_ID_CFL_S) {
    FspsUpd->FspsConfig.PegPhysicalSlotNumber[0]              = 1;
    FspsUpd->FspsConfig.PegPhysicalSlotNumber[1]              = 2;
    FspsUpd->FspsConfig.PegPhysicalSlotNumber[2]              = 3;
  }

  PcieCfgData = (PCIE_CFG_DATA *) FindConfigDataByTag (CDATA_PCIE_TAG);
  if (PcieCfgData == NULL) {
    DEBUG ((DEBUG_ERROR, "Missing PCIE RP Cfg Data!\n"));
  } else {
    Length = GetPchMaxPciePortNum ();
    FspsUpd->FspsConfig.PcieRpPtmMask                         = 0;
    FspsUpd->FspsConfig.PcieRpDpcMask                         = 0;
    FspsUpd->FspsConfig.PcieRpDpcExtensionsMask               = 0;
    for (Index = 0; Index < Length; Index++) {
      FspsUpd->FspsConfig.PcieRpAspm[Index]                   = (UINT8) PcieCfgData->RpFeatures0[Index].Aspm;
      FspsUpd->FspsConfig.PcieRpPmSci[Index]                  = (UINT8) PcieCfgData->RpFeatures0[Index].PmSciEn;
      FspsUpd->FspsConfig.PcieRpAcsEnabled[Index]             = (UINT8) PcieCfgData->RpFeatures0[Index].AcsEn;
      FspsUpd->FspsConfig.PcieRpPtmMask                       |= (BIT0 << Index);
      FspsUpd->FspsConfig.PcieRpDpcMask                       |= (BIT0 << Index);
      FspsUpd->FspsConfig.PcieRpDpcExtensionsMask             |= (BIT0 << Index);
      FspsUpd->FspsConfig.PcieRpMaxPayload[Index]             = (UINT8) PcieCfgData->RpFeatures0[Index].MaxPld;
      FspsUpd->FspsConfig.PcieRpSlotImplemented[Index]        = (UINT8) PcieCfgData->RpFeatures0[Index].SlotImplemented;
      FspsUpd->FspsConfig.PcieRpPhysicalSlotNumber[Index]     = (UINT8) Index;
      FspsUpd->FspsConfig.PcieRpL1Substates[Index]            = (UINT8) PcieCfgData->RpFeatures0[Index].L1SubStates;
      FspsUpd->FspsConfig.PcieRpEnableCpm[Index]              = (UINT8) PcieCfgData->RpFeatures0[Index].CpmEn;
      FspsUpd->FspsConfig.PcieRpGen3EqPh3Method[Index]        = (UINT8) PcieCfgData->RpFeatures0[Index].Gen3EqPh3Method;

      FspsUpd->FspsConfig.PcieRpLtrEnable[Index]                = (UINT8)  PcieCfgData->RpFeatures0[Index].LtrEn;
      FspsUpd->FspsTestConfig.PcieRpLtrMaxSnoopLatency[Index]   = (UINT16) PcieCfgData->RpFeatures0[Index].MaxSnoopLat;
      FspsUpd->FspsTestConfig.PcieRpLtrMaxNoSnoopLatency[Index] = (UINT16) PcieCfgData->RpFeatures0[Index].MaxNoSnoopLat;

      FspsUpd->FspsTestConfig.PcieRpSnoopLatencyOverrideMode[Index]           = (UINT8) PcieCfgData->RpFeatures0[Index].SnoopLatMode;
      FspsUpd->FspsTestConfig.PcieRpSnoopLatencyOverrideMultiplier[Index]     = (UINT8) PcieCfgData->RpFeatures0[Index].SnoopLatMul;
      FspsUpd->FspsTestConfig.PcieRpSnoopLatencyOverrideValue[Index]          = (UINT16)PcieCfgData->RpFeatures0[Index].SnoopLatVal;
      FspsUpd->FspsTestConfig.PcieRpNonSnoopLatencyOverrideMode[Index]        = (UINT8) PcieCfgData->RpFeatures0[Index].NoSnoopLatMode;
      FspsUpd->FspsTestConfig.PcieRpNonSnoopLatencyOverrideMultiplier[Index]  = (UINT8) PcieCfgData->RpFeatures0[Index].NoSnoopLatMul;
      FspsUpd->FspsTestConfig.PcieRpNonSnoopLatencyOverrideValue[Index]       = (UINT16)PcieCfgData->RpFeatures0[Index].NoSnoopLatVal;

      FspsUpd->FspsTestConfig.PcieRpUptp[Index]               = (UINT8) PcieCfgData->RpFeatures0[Index].Uptp;
      FspsUpd->FspsTestConfig.PcieRpDptp[Index]               = (UINT8) PcieCfgData->RpFeatures0[Index].Dptp;

      FspsUpd->FspsConfig.PcieEqPh3LaneParamCm[Index]         = (UINT8) PcieCfgData->RpFeatures0[Index].EqPh3Cm;
      FspsUpd->FspsConfig.PcieEqPh3LaneParamCp[Index]         = (UINT8) PcieCfgData->RpFeatures0[Index].EqPh3Cp;

      FspsUpd->FspsConfig.PcieRpClkReqDetect[Index]           = (UINT8) PcieCfgData->RpFeatures0[Index].ClkReqDetect;
      FspsUpd->FspsConfig.PcieRpAdvancedErrorReporting[Index] = (UINT8) PcieCfgData->RpFeatures0[Index].AdvErrReport;
    }

    Length = GetPchMaxPcieClockNum ();
    for (Index = 0; Index < Length; Index++) {
      FspsUpd->FspsConfig.PcieClkSrcClkReq[Index]             = (UINT8)PcieCfgData->ClkFeatures0[Index].ClkReq;
      FspsUpd->FspsConfig.PcieClkSrcUsage[Index]              = (UINT8)PcieCfgData->ClkFeatures0[Index].ClkUsage;
    }
  }

  FspsUpd->FspsConfig.PcieSwEqCoeffListCm[0]                = 4;
  FspsUpd->FspsConfig.PcieSwEqCoeffListCp[0]                = 8;
  FspsUpd->FspsConfig.PcieSwEqCoeffListCm[1]                = 6;
  FspsUpd->FspsConfig.PcieSwEqCoeffListCp[1]                = 2;
  FspsUpd->FspsConfig.PcieSwEqCoeffListCm[2]                = 8;
  FspsUpd->FspsConfig.PcieSwEqCoeffListCp[2]                = 6;
  FspsUpd->FspsConfig.PcieSwEqCoeffListCm[3]                = 10;
  FspsUpd->FspsConfig.PcieSwEqCoeffListCp[3]                = 8;
  FspsUpd->FspsConfig.PcieSwEqCoeffListCm[4]                = 12;
  FspsUpd->FspsConfig.PcieSwEqCoeffListCp[4]                = 2;

  if (PlatformId == PLATFORM_ID_CFL_S) {
    FspsUpd->FspsConfig.VrPowerDeliveryDesign = 0x0D;
  } else if (PlatformId == PLATFORM_ID_CFL_H) {
    FspsUpd->FspsConfig.VrPowerDeliveryDesign = 0x1F;
  }

  //
  // CPU power management config
  //
  FspsUpd->FspsConfig.TdcEnable[0]                        = 0x1;
  FspsUpd->FspsConfig.TdcEnable[1]                        = 0x1;
  FspsUpd->FspsConfig.TdcEnable[3]                        = 0x1;
  FspsUpd->FspsConfig.TdcPowerLimit[1]                    = 0x2b0;
  FspsUpd->FspsConfig.TdcPowerLimit[3]                    = 0xc8;
  FspsUpd->FspsConfig.AcLoadline[0]                       = 0x406;
  FspsUpd->FspsConfig.AcLoadline[1]                       = 0xb4;
  FspsUpd->FspsConfig.AcLoadline[3]                       = 0x10e;
  FspsUpd->FspsConfig.DcLoadline[0]                       = 0x406;
  FspsUpd->FspsConfig.DcLoadline[1]                       = 0xb4;
  FspsUpd->FspsConfig.DcLoadline[3]                       = 0x10e;
  FspsUpd->FspsConfig.Psi1Threshold[0]                    = 0x50;
  FspsUpd->FspsConfig.Psi1Threshold[1]                    = 0x50;
  FspsUpd->FspsConfig.Psi1Threshold[2]                    = 0x50;
  FspsUpd->FspsConfig.Psi1Threshold[3]                    = 0x50;
  FspsUpd->FspsConfig.Psi1Threshold[4]                    = 0x50;
  FspsUpd->FspsConfig.Psi2Threshold[0]                    = 0x14;
  FspsUpd->FspsConfig.Psi2Threshold[1]                    = 0x14;
  FspsUpd->FspsConfig.Psi2Threshold[2]                    = 0x14;
  FspsUpd->FspsConfig.Psi2Threshold[3]                    = 0x14;
  FspsUpd->FspsConfig.Psi2Threshold[4]                    = 0x14;
  FspsUpd->FspsConfig.Psi3Threshold[0]                    = 0x4;
  FspsUpd->FspsConfig.Psi3Threshold[1]                    = 0x4;
  FspsUpd->FspsConfig.Psi3Threshold[2]                    = 0x4;
  FspsUpd->FspsConfig.Psi3Threshold[3]                    = 0x4;
  FspsUpd->FspsConfig.Psi3Threshold[4]                    = 0x4;
  FspsUpd->FspsConfig.IccMax[0]                           = 0x2c;
  FspsUpd->FspsConfig.IccMax[1]                           = 0x230;
  FspsUpd->FspsConfig.IccMax[3]                           = 0x80;
  FspsUpd->FspsConfig.McivrSpreadSpectrum                 = 0x3;

  FspsUpd->FspsTestConfig.TccActivationOffset             = 0x0;
  FspsUpd->FspsTestConfig.TccOffsetClamp                  = 0x0;

  FspsUpd->FspsTestConfig.PowerLimit1                     = 0x0;
  FspsUpd->FspsTestConfig.PowerLimit2Power                = 0x0;
  FspsUpd->FspsTestConfig.PowerLimit3                     = 0x0;
  FspsUpd->FspsTestConfig.PowerLimit4                     = 0x0;
  FspsUpd->FspsTestConfig.Custom1PowerLimit1              = 0x0;
  FspsUpd->FspsTestConfig.Custom1PowerLimit2              = 0x0;
  FspsUpd->FspsTestConfig.Custom2PowerLimit1              = 0x0;
  FspsUpd->FspsTestConfig.Custom2PowerLimit2              = 0x0;
  FspsUpd->FspsTestConfig.Custom3PowerLimit1              = 0x0;
  FspsUpd->FspsTestConfig.Custom3PowerLimit2              = 0x0;
  FspsUpd->FspsTestConfig.Custom1PowerLimit1Time          = 0x0;
  FspsUpd->FspsTestConfig.Custom1TurboActivationRatio     = 0x0;
  FspsUpd->FspsTestConfig.Custom2PowerLimit1Time          = 0x0;
  FspsUpd->FspsTestConfig.Custom2TurboActivationRatio     = 0x0;
  FspsUpd->FspsTestConfig.Custom3PowerLimit1Time          = 0x0;
  FspsUpd->FspsTestConfig.Custom3TurboActivationRatio     = 0x0;

  FspsUpd->FspsTestConfig.VoltageOptimization             = 0x0;
  FspsUpd->FspsTestConfig.TStates                         = 0x0;
  FspsUpd->FspsTestConfig.ProcHotResponse                 = 0x0;
  FspsUpd->FspsTestConfig.Cx                              = 0x1;
  FspsUpd->FspsTestConfig.PkgCStateLimit                  = 0xff;
  FspsUpd->FspsTestConfig.MaxRatio                        = 0x8;
  FspsUpd->FspsTestConfig.PsysPmax                        = 0x0;
  FspsUpd->FspsTestConfig.CstateLatencyControl0Irtl       = 0x4e;

  PowerCfgData = (POWER_CFG_DATA *) FindConfigDataByTag (CDATA_POWER_TAG);
  if (PowerCfgData == NULL) {
    DEBUG ((DEBUG_ERROR, "Missing power Cfg Data!\n"));
  } else {
    if ((PlatformId == PLATFORM_ID_WHL) || (PlatformId == PLATFORM_ID_CFL_H)){
      FspsUpd->FspsConfig.PchPwrOptEnable = 0x1;
    }
    FspsUpd->FspsTestConfig.OneCoreRatioLimit = PowerCfgData->OneCoreRatioLimit;
    FspsUpd->FspsTestConfig.TwoCoreRatioLimit = PowerCfgData->TwoCoreRatioLimit;
    FspsUpd->FspsTestConfig.ThreeCoreRatioLimit = PowerCfgData->ThreeCoreRatioLimit;
    FspsUpd->FspsTestConfig.FourCoreRatioLimit = PowerCfgData->FourCoreRatioLimit;
    FspsUpd->FspsTestConfig.FiveCoreRatioLimit = PowerCfgData->FiveCoreRatioLimit;
    FspsUpd->FspsTestConfig.SixCoreRatioLimit = PowerCfgData->SixCoreRatioLimit;
    FspsUpd->FspsTestConfig.SevenCoreRatioLimit = PowerCfgData->SevenCoreRatioLimit;
    FspsUpd->FspsTestConfig.EightCoreRatioLimit = PowerCfgData->EightCoreRatioLimit;

    FspsUpd->FspsConfig.DmiTS0TW = PowerCfgData->DmiTS0TW;
    FspsUpd->FspsConfig.DmiTS1TW = PowerCfgData->DmiTS1TW;
    FspsUpd->FspsConfig.DmiTS2TW = PowerCfgData->DmiTS2TW;

    FspsUpd->FspsConfig.PchMemoryPmsyncEnable[0] = PowerCfgData->MemoryThermalMgmt;
    FspsUpd->FspsConfig.PchMemoryC0TransmitEnable[0] = PowerCfgData->MemoryThermalMgmt;
    FspsUpd->FspsConfig.PchMemoryPmsyncEnable[1] = PowerCfgData->MemoryThermalMgmt;
    FspsUpd->FspsConfig.PchMemoryC0TransmitEnable[1] = PowerCfgData->MemoryThermalMgmt;
    if (PlatformId == PLATFORM_ID_WHL) {
      FspsUpd->FspsConfig.PchPmSlpS0Vm075VSupport = 0x1;
      FspsUpd->FspsConfig.PchHotEnable = 0x1;
    }
  }

  SiliconCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if (SiliconCfgData != NULL) {
    FspsUpd->FspsConfig.DebugInterfaceEnable        = SiliconCfgData->DebugInterfaceEnable;
  }

  /* Flash protection range setting */
  Status = SpiGetRegionAddress (FlashRegionBios, &BaseAddress, &RegionSize);
  if (!EFI_ERROR (Status)) {
    FspsUpd->FspsConfig.PchWriteProtectionEnable[0] = 0x0;
    FspsUpd->FspsConfig.PchReadProtectionEnable[0]  = 0x0;
    FspsUpd->FspsConfig.PchProtectedRangeBase[0]    = (UINT16)(BaseAddress >> 12);
    FspsUpd->FspsConfig.PchProtectedRangeLimit[0]   = (UINT16)(((BaseAddress + RegionSize) - 1) >> 12);
  }

  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    FspsTestConfig = &FspsUpd->FspsTestConfig;
    FspsTestConfig->PchLockDownBiosInterface = FALSE;
    FspsUpd->FspsConfig.PchLockDownRtcMemoryLock       = FALSE;
    FspsTestConfig->PchSbAccessUnlock        = TRUE;
    DEBUG ((DEBUG_INFO, "Firmware update mode, unlock Bios setting\n"));
  }

  if (!(UpdateFspsSgxConfig (FspsUpd))) {
    DEBUG ((DEBUG_INFO, "FSP-S variables for Intel(R) SGX were NOT updated.\n"));
  }
}

/**
  This function will set the DISB - DRAM Initialization Scratchpad Bit.

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
  Save MRC data into the reserved SPI region

  @param[in]  Buffer            The pointer to MRC data to be saved.
  @param[in]  Length            The length of MRC data to be saved.
  @return     Resulting of saving MRC data to SPI

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
 Update the OS boot option

 @param OsBootOptionList pointer to boot option list.
 **/
VOID
EFIAPI
UpdateOsBootMediumInfo (
  OUT  OS_BOOT_OPTION_LIST  *OsBootOptionList
  )
{
  FillBootOptionListFromCfgData (OsBootOptionList);

  return;
}

/**
  Update Frame Buffer Information

  @param[out]  GfxInfo    Frame Buffer Information to be updated

**/
VOID
EFIAPI
UpdateFrameBufferInfo (
  OUT  EFI_PEI_GRAPHICS_INFO_HOB   *GfxInfo
)
{
  if (PcdGetBool (PcdIntelGfxEnabled)) {
    GfxInfo->FrameBufferBase  = LShiftU64 (PciRead32 (PCI_LIB_ADDRESS(0, 2, 0, 0x1C)), 32);
    GfxInfo->FrameBufferBase += (PciRead32 (PCI_LIB_ADDRESS(0, 2, 0, 0x18)) & 0xFFFFFF00);
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
    GfxDeviceInfo->VendorId = PciRead16 (PCI_LIB_ADDRESS (0, 2, 0, 0));
    GfxDeviceInfo->DeviceId = PciRead16 (PCI_LIB_ADDRESS (0, 2, 0, 2));
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
 Update loader platform info.

 @param[out] LoaderPlatformInfo pointer to platform info HOB

**/
VOID
UpdateLoaderPlatformInfo (
  OUT LOADER_PLATFORM_INFO        *LoaderPlatformInfo
)
{

  PLATFORM_DATA               *PlatformData;

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();

  if(PlatformData != NULL) {
    LoaderPlatformInfo->HwState = PlatformData->BtGuardInfo.VerifiedBoot | (PlatformData->BtGuardInfo.MeasuredBoot << 1);
    LoaderPlatformInfo->Flags   = FLAGS_SPI_DISABLE_SMM_WRITE_PROTECT;

    if (PlatformData->BtGuardInfo.TpmType == dTpm20)
      LoaderPlatformInfo->TpmType = TPM_TYPE_DTPM20;
    else if (PlatformData->BtGuardInfo.TpmType == Ptt)
      LoaderPlatformInfo->TpmType = TPM_TYPE_PTT;
    else if (PlatformData->BtGuardInfo.TpmType == TpmNone)
      LoaderPlatformInfo->TpmType = TPM_TYPE_NONE;

    DEBUG ((DEBUG_INFO, "Stage2: HwState 0x%x\n", LoaderPlatformInfo->HwState));
  }
}

/**
 Update Hob Info with platform specific data

 @param  Guid          The GUID to tag the customized HOB.
 @param  HobInfo       The start address of GUID HOB data.
 **/VOID
EFIAPI
PlatformUpdateHobInfo (
  IN CONST EFI_GUID              *Guid,
  OUT      VOID                  *HobInfo
)
{
  ASSERT (Guid != NULL);
  ASSERT (HobInfo != NULL);

  if ((Guid == NULL) || (HobInfo == NULL)) {
    return;
  }

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
  Update Platform related ACPI Tables

  @param[in]  Current         A specific ACPI table pointer to be updated

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
  SILICON_CFG_DATA            *SiliconCfgData;
  MEMORY_CFG_DATA             *MemCfgData;
  UINTN                        DmarTableFlags;
  VOID                        *FspHobList;

  GlobalNvs  = (GLOBAL_NVS_AREA *)(UINTN) PcdGet32 (PcdAcpiGnvsAddress);

  Table = (EFI_ACPI_DESCRIPTION_HEADER *) Current;
  Ptr  = (UINT8 *)Table;
  End  = (UINT8 *)Table + Table->Length;

  if (Table->Signature == EFI_ACPI_5_0_EMBEDDED_CONTROLLER_BOOT_RESOURCES_TABLE_SIGNATURE) {
    SiliconCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
    if ((SiliconCfgData == NULL) || (SiliconCfgData->ECEnable == 0)) {
      return EFI_UNSUPPORTED;
    }
  }

  if (Table->Signature == EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    for (; Ptr < End; Ptr++) {
      if (*(Ptr-1) != AML_NAME_OP)
        continue;
      if (*(UINT32 *)Ptr == SIGNATURE_32 ('P','N','V','B')) {
        Base = (UINT32) (UINTN) &GlobalNvs->PchNvs;
        DEBUG ((DEBUG_INFO, "PNVB Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
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
  } else if (Table->Signature == NHLT_ACPI_TABLE_SIGNATURE) {
    GlobalNvs->PchNvs.NHLA  = (UINT64)(UINTN) Table;
    GlobalNvs->PchNvs.NHLL  = Table->Length;
    DEBUG ((DEBUG_INFO, "NHLT Base 0x%08X, Size 0x%08X\n", (UINT32)(UINTN)GlobalNvs->PchNvs.NHLA, GlobalNvs->PchNvs.NHLL));
  } else if (Table->OemTableId == SIGNATURE_64 ('C', 'p', 'u', 'S', 's', 'd', 't', 0)) {
    PatchCpuSsdtTable (Table, GlobalNvs);
  } else if (Table->OemTableId == SIGNATURE_64 ('C', 'p', 'u', '0', 'I', 's', 't', 0)) {
    PatchCpuIstTable (Table, GlobalNvs);
  }

  if (Table->Signature == EFI_BDAT_TABLE_SIGNATURE) {
    FspHobList = GetFspHobListPtr ();
    if (FspHobList != NULL) {
      UpdateBdatAcpiTable (Table, FspHobList);
      DEBUG ( (DEBUG_INFO, "Updated BDAT Table in AcpiTable Entries\n") );
    }
  }

  if (FeaturePcdGet (PcdVtdEnabled)) {
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if (PlatformData != NULL) {
      if (PlatformData->PlatformFeatures.VtdEnable == 1) {
        if (Table->Signature == EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE) {
          DEBUG ((DEBUG_INFO, "Updated DMAR Table in AcpiTable Entries\n"));
          SiliconCfgData  = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
          MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
          DmarTableFlags = 0;
          if (MemCfgData != NULL) {
            if (MemCfgData->X2ApicOptOut == 1) {
              DmarTableFlags  |= DMAR_TABLE_FLAGS_X2APIC_OPT_OUT;
            }
          }
          if ((SiliconCfgData != NULL) && SiliconCfgData->InterruptRemappingSupport != 0) {
            DmarTableFlags  |= DMAR_TABLE_FLAGS_INT_REMAPPING_SUPPORT;
          }
          UpdateDmarAcpi(Table, DmarTableFlags);
        }
      }
    }
  }

  if (FeaturePcdGet (PcdMeasuredBootEnabled)){
    if ((Table->Signature == EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE) ||
          (Table->OemTableId == ACPI_SSDT_TPM2_DEVICE_OEM_TABLE_ID)) {

      if ((GetFeatureCfg () & FEATURE_MEASURED_BOOT) != 0) {
        Status = UpdateTpm2AcpiTable(Table);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "UpdateTpm2AcpiTable fails! - %r\n", Status));
          ASSERT_EFI_ERROR (Status);
        }
          DEBUG ((DEBUG_ERROR, "UpdateTpm2AcpiTable - %r\n", Status));
          return Status;
      }
      return EFI_UNSUPPORTED;
    }
  }

  if (FeaturePcdGet (PcdPsdBiosEnabled)) {
    if (Table->Signature == EFI_ACPI_PSD_SIGNATURE) {
      PSD_CFG_DATA *PsdCfgData = NULL;
      PsdCfgData = (PSD_CFG_DATA *)FindConfigDataByTag (CDATA_PSD_TAG);
      if (PsdCfgData != NULL) {
        if (PsdCfgData->EnablePsd == 1) {
          UpdateAcpiPsdTable ( (VOID* )Current );
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
  VOID                                  *FspHobList;

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
  CopyMem (CpuNvs, (VOID *)(UINTN)CpuInitDataHob->CpuGnvsPointer, sizeof (CPU_NVS_AREA));
  DEBUG ((DEBUG_INFO, "Update Cpu Nvs Done\n"));

  DEBUG ((DEBUG_INFO, "Revision 0x%X, PpmFlags 0x%08X\n", CpuNvs->Revision, CpuNvs->PpmFlags));
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
  CONST UINT8             *PchSerialIoDevModeTable;
  UINT8                    Index;
  UINT8                    Length;
  UINT8                    RpNum;
  UINT8                    RpDev;
  UINT8                    RpFun;
  UINT8                    FuncIndex;
  UINT32                   Data32;
  UINT16                   PlatformId;
  GPIO_GROUP               GroupToGpeDwX[3];
  UINT32                   GroupDw[3];
  SILICON_CFG_DATA        *SiliconCfgData;

  GlobalNvs   = (GLOBAL_NVS_AREA *) GnvsIn;
  PlatformNvs = (PLATFORM_NVS_AREA *) &GlobalNvs->PlatformNvs;
  PchNvs      = (PCH_NVS_AREA *) &GlobalNvs->PchNvs;
  CpuNvs      = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;
  SaNvs       = (SYSTEM_AGENT_NVS_AREA *) &GlobalNvs->SaNvs;
  ZeroMem (GlobalNvs, sizeof (GLOBAL_NVS_AREA));

  PlatformId = GetPlatformId ();

  PlatformNvs->Ps2MouseEnable               = 0x0;
  PlatformNvs->Ps2KbMsEnable                = 0x0;

  // HD-Audio - board specific
  PlatformNvs->I2SC     = 0;
  if (IsPchLp()) {
    PlatformNvs->I2SI   = GPIO_CNL_LP_GPP_C8;
    PlatformNvs->I2SB   = 0;
  } else {
    PlatformNvs->I2SI   = GPIO_CNL_H_GPP_F5;
    PlatformNvs->I2SB   = 2;
  }
  PlatformNvs->HdaDspPpModuleMask = 0;

  if ((PlatformId == PLATFORM_ID_CFL_H) ||  (PlatformId == PLATFORM_ID_CFL_S)) {
    PlatformNvs->PcdH8S2113SIO              = 1;
  }

  PlatformNvs->ApicEnable                   = 1;
  SiliconCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if ( (SiliconCfgData != NULL) && (SiliconCfgData->ECEnable == 1)){
    PlatformNvs->EcAvailable                  = 1;
    PlatformNvs->EcLowPowerMode               = 0;
    PlatformNvs->EcSmiGpioPin                 = 0;
    PlatformNvs->EcLowPowerModeGpioPin        = 0;
    if (PlatformNvs->EcAvailable == 1) {
      if (PlatformId == PLATFORM_ID_WHL) {
        PlatformNvs->EcSmiGpioPin             = GPIO_CNL_LP_GPP_E3;
        PlatformNvs->EcLowPowerModeGpioPin    = GPIO_CNL_LP_GPP_B23;
      } else if (PlatformId == PLATFORM_ID_CFL_H) {
        PlatformNvs->EcSmiGpioPin             = GPIO_CNL_H_GPP_E3;
        PlatformNvs->EcLowPowerModeGpioPin    = GPIO_CNL_H_GPP_B23;
      } else if (PlatformId == PLATFORM_ID_CFL_S) {
        PlatformNvs->EcSmiGpioPin             = GPIO_CNL_H_GPP_I3;
        PlatformNvs->EcLowPowerModeGpioPin    = 0;
      }
    }
  }
  if (SiliconCfgData != NULL) {
    if (PlatformId == PLATFORM_ID_WHL) {
      PlatformNvs->PlatformFlavor           = SiliconCfgData->EnableLegacySerial;
    }
  }

  PlatformNvs->PowerState                   = 1;
  PlatformNvs->Ac1TripPoint                 = 55;
  PlatformNvs->Ac0TripPoint                 = 71;
  PlatformNvs->Ac1FanSpeed                  = 75;
  PlatformNvs->Ac0FanSpeed                  = 100;
  PlatformNvs->PassiveThermalTripPoint      = 95;
  PlatformNvs->PassiveTc1Value              = 1;
  PlatformNvs->PassiveTc2Value              = 5;
  PlatformNvs->PassiveTspValue              = 10;
  PlatformNvs->CriticalThermalTripPoint     = 110;

  PlatformNvs->EnableDptf                   = 0; // not supported for now
  PlatformNvs->EnableSaDevice               = 1;

  PchNvs->PchSeries     = PchSeries ();
  PchNvs->PchGeneration = (UINT16) PchGeneration ();
  PchNvs->PchStepping   = (UINT16) PchStepping ();

  Length = GetPchMaxPciePortNum ();
  for (RpNum = 0; RpNum < Length; RpNum++) {
    Index     = RpNum / PCH_PCIE_CONTROLLER_PORTS;
    FuncIndex = RpNum - mPchPcieControllerInfo[Index].RpNumBase;
    RpDev     = mPchPcieControllerInfo[Index].DevNum;
    Data32    = MmioRead32 (PCH_PCR_ADDRESS (mPchPcieControllerInfo[Index].Pid, R_SPX_PCR_PCD));
    RpFun     = (Data32 >> (FuncIndex * S_SPX_PCR_PCD_RP_FIELD)) & B_SPX_PCR_PCD_RP1FN;

    Data32 = ((UINT8) RpDev << 16) | (UINT8) RpFun;
    PchNvs->RpAddress[RpNum] = Data32;
    DEBUG ((DEBUG_INFO, "RpAddress[%d] = 0x%08X\n", RpNum, PchNvs->RpAddress[RpNum]));

    // Need to match with FSP-S UPD
    PchNvs->PcieLtrMaxSnoopLatency[RpNum]   = 0x1003;
    PchNvs->PcieLtrMaxNoSnoopLatency[RpNum] = 0x1003;
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
  PchNvs->SdPowerEnableActiveHigh          = 1;
  PchNvs->GBES          = 1;

  PchNvs->EmmcEnabled = 1;
  PchNvs->SdPowerEnableActiveHigh = 1;
  PchNvs->EMH4 = 1;
  PchNvs->EMDS = 0x4;

  //
  // GPIO device
  //
  PchNvs->SGIR  = DEFAULT_GPIO_IRQ_ROUTE;
  PchNvs->GPHD  = 0;

  GpioGetGroupDwToGpeDwX (
    &GroupToGpeDwX[0], &GroupDw[0],
    &GroupToGpeDwX[1], &GroupDw[1],
    &GroupToGpeDwX[2], &GroupDw[2]
    );

  PchNvs->GEI0 = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[0]);
  PchNvs->GEI1 = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[1]);
  PchNvs->GEI2 = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[2]);
  PchNvs->GED0 = (UINT8) GroupDw[0];
  PchNvs->GED1 = (UINT8) GroupDw[1];
  PchNvs->GED2 = (UINT8) GroupDw[2];
  DEBUG ((DEBUG_INFO, "GEI [0x%X 0x%X 0x%X], GED [0x%X 0x%X 0x%X]\n",
    PchNvs->GEI0, PchNvs->GEI1, PchNvs->GEI2,
    PchNvs->GED0, PchNvs->GED1, PchNvs->GED2));

  if (IsPchLp()) {
    PchSerialIoDevModeTable = mPchLpSerialIoDevMode;
  } else {
    PchSerialIoDevModeTable = mPchSerialIoDevMode;
  }

  for (Index = 0; Index < PCH_MAX_SERIALIO_CONTROLLERS; Index ++) {
    PchNvs->SMD[Index] = PchSerialIoDevModeTable[Index];
    PchNvs->SIR[Index] = FindSerialIoIrq (Index);
    PchNvs->SB0[Index] = FindSerialIoBar (Index, 0);
    PchNvs->SB1[Index] = FindSerialIoBar (Index, 1);

    DEBUG ((DEBUG_INFO, "SerialIo[%d] %d, %d, 0x%08X, 0x%08X\n", Index, \
      PchNvs->SMD[Index], PchNvs->SIR[Index], PchNvs->SB0[Index], PchNvs->SB1[Index]));
  }

  //
  // Update platformId in NVS area
  //
  PlatformNvs->PlatformId = (UINT8) GetPlatformId ();

  SaNvs->XPcieCfgBaseAddress = (UINT32)(PcdGet64(PcdPciExpressBaseAddress));

  SaNvs->Mmio32Base   = PcdGet32(PcdPciResourceMem32Base);
  // PcdPciExpressBaseAddress is 0xE000_0000
  if (SaNvs->Mmio32Base < SaNvs->XPcieCfgBaseAddress) {
    SaNvs->Mmio32Length = SaNvs->XPcieCfgBaseAddress - SaNvs->Mmio32Base;
  } else {
    DEBUG((DEBUG_INFO, "acpi: Unable to configure M32L with M32B=0x%08X\n", SaNvs->Mmio32Base));
  }

  SaNvs->Mmio64Base   = PcdGet64(PcdPciResourceMem64Base);
  SaNvs->Mmio64Length = SaNvs->Mmio64Base;

  SaNvs->AlsEnable                    = 0;
  SaNvs->IgdState                     = 1;
  SaNvs->BrightnessPercentage         = 100;
  SaNvs->IgdBootType                  = 0;
  SaNvs->IgdPanelType                 = 0;
  SaNvs->IgdPanelScaling              = 0;
  SaNvs->IgdDvmtMemSize               = 2;
  SaNvs->IgdFunc1Enable               = 0;
  Data32                              = PciRead32 (PCI_LIB_ADDRESS(0, 0, 0, 0x48));
  Data32                             &= (UINT32) ~BIT0;
  SaNvs->IgdHpllVco                   = MmioRead8 (Data32 + 0xC0F) & 0x07;
  SaNvs->IgdSciSmiMode                = 0;
  SaNvs->GfxTurboIMON                 = 31;
  SaNvs->EdpValid                     = 0;

  UpdateCpuNvs (CpuNvs);
  PlatformNvs->PpmFlags = CpuNvs->PpmFlags;
  SysCpuInfo = MpGetInfo ();
  if (SysCpuInfo != NULL) {
    PlatformNvs->ThreadCount  = (UINT8) SysCpuInfo->CpuCount;
  }

  UpdateSgxNvs (CpuNvs);
  SocUpdateAcpiGnvs ((VOID *)GnvsIn);
}
