/** @file

  Copyright (c) 2020 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Pi/PiBootMode.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/ConfigDataLib.h>
#include <FspsUpd.h>
#include <PlatformBoardId.h>
#include <Register/Cpuid.h>
#include <Library/BoardSupportLib.h>
#include <IgdOpRegionDefines.h>
#include <PchLimits.h>
#include <ConfigDataDefs.h>
#include <ConfigDataCommonStruct.h>
#include <CpuDataStruct.h>
#include <CpuRegs.h>
#include <Library/PchInfoLib.h>
#include <PlatformBase.h>
#include <Library/IoLib.h>
#include <Library/SpiFlashLib.h>
#include <Register/PmcRegs.h>
#include <Library/IgdOpRegionLib.h>
#include <Library/VTdLib.h>
#include <Include/TccConfigSubRegions.h>
#include <Library/WatchDogTimerLib.h>
#include <PlatformData.h>
#include <Library/ContainerLib.h>
#include "HdaVerbTable.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include "TsnSubRegion.h"

#define CPU_PCIE_DT_HALO_MAX_ROOT_PORT     3
#define CPU_PCIE_ULT_ULX_MAX_ROOT_PORT     3
#define MAX_TCSS_USB3_PORTS                4

//MTL

#define VALID_DEV_NO    0  ///< Device does not exist
#define VALID_DEV_YES   1  ///< Device exists

#define PCIE_RP_NO      0  ///< Device is not a PCIe root port
#define PCIE_RP_YES     1  ///< Device is a PCIe root port

#define INTERRUPT_NO    0  ///< Device doesn't generate interrupts
#define INTERRUPT_YES   1  ///< Device generates interrupts

#define USES_PIRQ_NO    0  ///< Device does not route its interrupt to PIRQ
#define USES_PIRQ_YES   1  ///< Device is under INTx->PIRQy mapping

#define INTP_NO_REQ     0  ///< There is no requirement regarding INTP programming
#define INTP_MUST_SET   1  ///< INTP is RWO and needs to be programmed
#define INTP_IS_INTA    2  ///< INTP is required to be set to INTA
#define INTP_IS_INTB    3  ///< INTP is required to be set to INTB
#define INTP_IS_INTC    4  ///< INTP is required to be set to INTC
#define INTP_IS_INTD    5  ///< INTP is required to be set to INTD

#define IRQ_UNIQUE_NO       0  ///< IRQx can be shareable for this device
#define IRQ_UNIQUE_SERIALIO 1  ///< SerialIo devices need unique IRQ
#define IRQ_UNIQUE_SCS      2  ///< SCS devices need unique IRQ
#define IRQ_UNIQUE_OTG      3  ///< OTG device need unique IRQ
#define IRQ_UNIQUE_ISH      4  ///< ISH device need unique IRQ

//
// --------------------- Interrupts Config ------------------------------
//
typedef enum {
  PchNoInt,        ///< No Interrupt Pin
  PchIntA,
  PchIntB,
  PchIntC,
  PchIntD
} PCH_INT_PIN;

//
// mSocMDevIntConfig table contains data pertaining to the INTx->PIRQy mapping for devices that exist on SOC-M
//
GLOBAL_REMOVE_IF_UNREFERENCED SI_PCH_DEVICE_INTERRUPT_CONFIG mSocMDevIntConfig[] = {
  // {31, 0, PchNoInt, 0}, // LPC/eSPI Interface, doesn't use interrupts
  // {31, 1, PchNoInt, 0}, // P2SB, doesn't use interrupts
  // {31, 2, PchNoInt, 0}, // PMC , doesn't use interrupts
  { 31, 3, PchIntB, 17 },  // ACE
  { 31, 4, PchIntC, 18 },  // SMBus Controller
  // {31, 5, PchNoInt, 0}, // SPI , doesn't use interrupts
  { 31, 6, PchIntD, 19 },  // GbE Controller
  { 31, 7, PchIntA, 16 },  // TraceHub, INTA is required
  { 30, 0, PchIntA, 16 },  // SerialIo: UART #0 and TSN #0 shared
  { 30, 1, PchIntB, 17 },  // SerialIo: UART #1 and TSN #1 shared
  { 30, 2, PchIntC, 27 },  // SerialIo: SPI #0
  { 30, 3, PchIntD, 28 },  // SerialIo: SPI #1
  // {30, 6, PchNoInt, 0}, // Reserved for HPET
  // {30, 7, PchNoInt, 0}, // Reserved for IOAPIC
  { 28, 0, PchIntA, 16 },  // PCI Express Port 1
  { 28, 1, PchIntB, 17 },  // PCI Express Port 2
  { 28, 2, PchIntC, 18 },  // PCI Express Port 3
  { 28, 3, PchIntD, 19 },  // PCI Express Port 4
  { 28, 4, PchIntA, 16 },  // PCI Express Port 5
  { 28, 5, PchIntB, 17 },  // PCI Express Port 6
  { 28, 6, PchIntC, 18 },  // PCI Express Port 7
  { 28, 7, PchIntD, 19 },  // PCI Express Port 8
  { 25, 0, PchIntA, 29 },  // SerialIo I2C Controller #4
  { 25, 1, PchIntB, 30 },  // SerialIo I2C Controller #5
  { 25, 2, PchIntC, 31 },  // SerialIo UART #2
  // {24, 0, PchNoInt, 0}, // Reserved (used by RST PCIe Storage Cycle Router)
  { 23, 0, PchIntA, 16 },  // SATA Controller
  { 22, 0, PchIntA, 16 },  // CSME: HECI #1
  { 22, 1, PchIntB, 17 },  // CSME: HECI #2
  { 22, 2, PchIntC, 18 },  // CSME: IDE-Redirection (IDE-R)
  { 22, 3, PchIntD, 19 },  // CSME: Keyboard and Text (KT) Redirection
  { 22, 4, PchIntA, 16 },  // CSME: HECI #3
  { 22, 5, PchIntB, 17 },  // CSME: HECI #4
  // {22, 7, PchNoInt, 0}, // CSME: WLAN, no interrupts
  { 21, 0, PchIntA, 32 },  // SerialIo I2C Controller #0
  { 21, 1, PchIntB, 33 },  // SerialIo I2C Controller #1
  { 21, 2, PchIntC, 34 },  // SerialIo I2C Controller #2
  //
  // SerialIo I2C Controller #3 and SerialIo I3C Controller share an IRQ > 23 and therefore cannot be enabled at the same time.
  //
  //
  // This is a temporary workaround for an ITSS S21 issue on MTL SOC-M, see https://hsdes.intel.com/appstore/article/#/22011634494 //PCH:RestrictedContent
  //
  { 21, 3, PchIntD, 35 },  // SerialIo I2C Controller #3
  { 21, 4, PchIntD, 35 },  // SerialIo I3C Controller
  // {21, 7, PchNoInt, 0}, // Northpeak Phantom (ACPI) Function - not exposed to OS
  { 20, 0, PchIntA, 16 },  // USB 3.0 xHCI Controller
  { 20, 1, PchIntB, 17 },  // USB Device Controller (OTG)
  // {20, 2, PchNoInt, 0}, // Shared SRAM, no interrupts
  { 20, 3, PchIntC, 18 },  // CNVi WiFi
  // {20, 4, PchNoInt, 0}, // TraceHub Phantom (ACPI) Function
  // {20, 5, PchNoInt, 0}  // IEH, doesn't use interrupts
  { 18, 0, PchIntA, 26 },  // Integrated Sensor Hub
  // {18, 2, PchNoInt, 0}, // CSME: PMT Phantom (ACPI) function
  // {18, 4, PchNoInt, 0}, // CSME: fTPM DMA Phantom (ACPI) function
  { 18, 6, PchIntB, 37 },  // SerialIo: SPI #2
  { 18, 7, PchIntC, 25 },  // UFS
  { 16, 0, PchIntA, 23 },  // THC #0
  { 16, 1, PchIntB, 22 },  // THC #1
  {  6, 0, PchIntA, 16 },  // PCI Express Port 9
  {  6, 1, PchIntB, 17 },
  {  6, 2, PchIntC, 18 },
  {  1, 0, PchIntA, 16 },
};

//
// mIoePDevIntConfig[] table contains data pertaining to the INTx->PIRQy mapping for devices that exist on IOE-P
//
GLOBAL_REMOVE_IF_UNREFERENCED SI_PCH_DEVICE_INTERRUPT_CONFIG mIoePDevIntConfig[] = {
  {  6, 1, PchIntB, 17 },  // PCI Express Port 10
  {  6, 2, PchIntC, 18 },  // PCI Express Port 11
  {  1, 0, PchIntA, 16 },  // PCI Express Port 12
};

//MTL

//
// This table contains data on INTx and IRQ for PCH-S
//
SI_PCH_DEVICE_INTERRUPT_CONFIG mPchSDevIntConfig[] = {
//  {31, 0, SiPchNoInt, 0}, // LPC/eSPI Interface, doesn't use interrupts
//  {31, 1, SiPchNoInt, 0}, // P2SB, doesn't use interrupts
//  {31, 2, SiPchNoInt, 0}, // PMC , doesn't use interrupts
  {31, 3, SiPchIntB, 17}, // cAVS(Audio, Voice, Speach)
  {31, 4, SiPchIntC, 18}, // SMBus Controller, no default value, programmed in PciCfgSpace 3Dh
//  {31, 5, SiPchNoInt, 0}, // SPI , doesn't use interrupts
  {31, 6, SiPchIntD, 19}, // GbE Controller
  {31, 7, SiPchIntA, 16}, // TraceHub, INTA is required
  {30, 0, SiPchIntA, 16}, // SerialIo: UART #0 and TSN #0 shared
  {30, 1, SiPchIntB, 17}, // SerialIo: UART #1 and TSN #1 shared
  {30, 2, SiPchIntC, 36}, // SerialIo: SPI #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[10]
  {30, 3, SiPchIntD, 37}, // SerialIo: SPI #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[11]
  {30, 4, SiPchIntA, 16}, // TSN Controller, INTA is default
//  {30, 6, SiPchIntA, 17}, // Reserved for HPET
//  {30, 7, SiPchIntB, 18}, // Reserved for IOAPIC
  {29, 0, SiPchIntA, 16}, // PCI Express Port 9, INT is default, programmed in PciCfgSpace + FCh
  {29, 1, SiPchIntB, 17}, // PCI Express Port 10, INT is default, programmed in PciCfgSpace + FCh
  {29, 2, SiPchIntC, 18}, // PCI Express Port 11, INT is default, programmed in PciCfgSpace + FCh
  {29, 3, SiPchIntD, 19}, // PCI Express Port 12, INT is default, programmed in PciCfgSpace + FCh
  {28, 0, SiPchIntA, 16}, // PCI Express Port 1, INT is default, programmed in PciCfgSpace + FCh
  {28, 1, SiPchIntB, 17}, // PCI Express Port 2, INT is default, programmed in PciCfgSpace + FCh
  {28, 2, SiPchIntC, 18}, // PCI Express Port 3, INT is default, programmed in PciCfgSpace + FCh
  {28, 3, SiPchIntD, 19}, // PCI Express Port 4, INT is default, programmed in PciCfgSpace + FCh
  {28, 4, SiPchIntA, 16}, // PCI Express Port 5, INT is default, programmed in PciCfgSpace + FCh
  {28, 5, SiPchIntB, 17}, // PCI Express Port 6, INT is default, programmed in PciCfgSpace + FCh
  {28, 6, SiPchIntC, 18}, // PCI Express Port 7, INT is default, programmed in PciCfgSpace + FCh
  {28, 7, SiPchIntD, 19}, // PCI Express Port 8, INT is default, programmed in PciCfgSpace + FCh
  {25, 0, SiPchIntA, 31}, // SerialIo I2C Controller #4, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[5]
  {25, 1, SiPchIntB, 32}, // SerialIo I2C Controller #5, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[6]
  {25, 2, SiPchIntC, 33}, // SerialIo UART Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[9]
//  {24, 0, 0, 0}, // Reserved (used by RST PCIe Storage Cycle Router)
  {23, 0, SiPchIntA, 16}, // SATA Controller, INTA is default, programmed in PciCfgSpace + 3Dh
  {22, 0, SiPchIntA, 16}, // CSME: HECI #1
  {22, 1, SiPchIntB, 17}, // CSME: HECI #2
  {22, 2, SiPchIntC, 18}, // CSME: IDE-Redirection (IDE-R)
  {22, 3, SiPchIntD, 19}, // CSME: Keyboard and Text (KT) Redirection
  {22, 4, SiPchIntA, 16}, // CSME: HECI #3
  {22, 5, SiPchIntD, 19}, // CSME: HECI #4
//  {22, 7, SiPchIntA, 16}, // CSME: WLAN, no interrupts
  {21, 0, SiPchIntA, 27}, // SerialIo I2C Controller #0, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[1]
  {21, 1, SiPchIntB, 40}, // SerialIo I2C Controller #1, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[2]
  {21, 2, SiPchIntC, 29}, // SerialIo I2C Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[3]
  {21, 3, SiPchIntD, 30}, // SerialIo I2C Controller #3, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[4]
//  {21, 7, SiPchIntA, 16}, // Northpeak Phantom (ACPI) Function - not exposed to OS
  {20, 0, SiPchIntA, 16}, // USB 3.0 xHCI Controller, no default value, programmed in PciCfgSpace 3Dh
  {20, 1, SiPchIntB, 17}, // USB Device Controller (OTG)
//  {20, 2, SiPchNoInt, 0}, // Shared SRAM, no interrupts
  {20, 3, SiPchIntC, 18}, // CNVi WiFi
//  {20, 4, 0, 0}, // TraceHub Phantom (ACPI) Function
  {19, 0, SiPchIntA, 20}, // SerialIo: SPI #3
  {18, 0, SiPchIntA, 26}, // Integrated Sensor Hub
//  {18, 2, SiPchIntB, 17}, // CSME: PMT Phantom (ACPI) function
//  {18, 4, SiPchIntC, 18}, // CSME: fTPM DMA Phantom (ACPI) function
  {18, 6, SiPchIntB, 39}, // SerialIo: SPI #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[12]
  {17, 0, SiPchIntA, 25}, // SerialIo: UART #3
//  {17, 4, SiPchIntA, 24}, // SerialIo: SPI #5
//  {17, 5, SiPchIntB, 25}, // SerialIo: SPI #6
  {16, 0, SiPchIntC, 18}, // SerialIo: I2C #6
  {16, 1, SiPchIntD, 19}, // SerialIo: I2C #7, additional INT reserved, not used for PCH-S
//  {16, 5, SiPchNoInt, 0}, // IEH, doesn't generate interrupts
  {16, 6, SiPchIntA, 23}, // THC #0
  {16, 7, SiPchIntB, 22}, // THC #1
  {30, 5, SiPchIntB, 17}, // TSN Controller 2, INTA is default
  {29, 4, SiPchIntA, 16}, // PCI Express Port 13, INT is default, programmed in PciCfgSpace + FCh
  {29, 5, SiPchIntB, 17}, // PCI Express Port 14, INT is default, programmed in PciCfgSpace + FCh
  {29, 6, SiPchIntC, 18}, // PCI Express Port 15, INT is default, programmed in PciCfgSpace + FCh
  {29, 7, SiPchIntD, 19}, // PCI Express Port 16, INT is default, programmed in PciCfgSpace + FCh
  {27, 0, SiPchIntA, 16}, // PCI Express Port 17, INT is default, programmed in PciCfgSpace + FCh
  {27, 1, SiPchIntB, 17}, // PCI Express Port 18, INT is default, programmed in PciCfgSpace + FCh
  {27, 2, SiPchIntC, 18}, // PCI Express Port 19, INT is default, programmed in PciCfgSpace + FCh
  {27, 3, SiPchIntD, 19}, // PCI Express Port 20, INT is default, programmed in PciCfgSpace + FCh
  {27, 4, SiPchIntA, 16}, // PCI Express Port 21, INT is default, programmed in PciCfgSpace + FCh
  {27, 5, SiPchIntB, 17}, // PCI Express Port 22, INT is default, programmed in PciCfgSpace + FCh
  {27, 6, SiPchIntC, 18}, // PCI Express Port 23, INT is default, programmed in PciCfgSpace + FCh
  {27, 7, SiPchIntD, 19}, // PCI Express Port 24, INT is default, programmed in PciCfgSpace + FCh
  {26, 0, SiPchIntA, 16}, // PCI Express Port 25, INT is default, programmed in PciCfgSpace + FCh
  {26, 1, SiPchIntB, 17}, // PCI Express Port 26, INT is default, programmed in PciCfgSpace + FCh
  {26, 2, SiPchIntC, 18}, // PCI Express Port 27, INT is default, programmed in PciCfgSpace + FCh
  {26, 3, SiPchIntD, 19}, // PCI Express Port 28, INT is default, programmed in PciCfgSpace + FCh
};


//
// This table contains data on INTx and IRQ for PCH-P
//
SI_PCH_DEVICE_INTERRUPT_CONFIG mPchPDevIntConfig[] = {
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
  {19, 0, SiPchIntA, 20}, // SerialIo: SPI #3
//  {19, 1, SiPchIntB, 24}, // SerialIo: SPI #4
  {18, 0, SiPchIntA, 26}, // Integrated Sensor Hub
  {18, 6, SiPchIntB, 39}, // SerialIo: SPI #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[12]
  {17, 0, SiPchIntA, 25}, // SerialIo: UART #3
  {17, 1, SiPchIntB, 35}, // SerialIo: UART #4
  {17, 2, SiPchIntC, 28}, // SerialIo: UART #5
  {17, 3, SiPchIntD, 34}, // SerialIo: UART #6
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
  {19, 1, SiPchIntB, 21}, // SerialIo: SPI #4
  {19, 2, SiPchIntC, 24}, // SerialIo: SPI #5 (LP)
  {19, 3, SiPchIntD, 38}, // SerialIo: SPI #6 (LP)
  {18, 7, SiPchIntC, 18},  // SCS: UFS
  //{16, 2, SiPchIntC, 18}, // BT CNVi - Not present
  //{16, 3, SiPchIntD, 19}  // ISH: FPAK - Not present
};

/**
  Return CPU Family ID

  @retval CPU_FAMILY              CPU Family ID
**/
CPU_FAMILY
EFIAPI
GetCpuFamily1 (
  VOID
  )
{
  CPUID_VERSION_INFO_EAX  Eax;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, NULL, NULL, NULL);
  return (Eax.Uint32 & CPUID_FULL_FAMILY_MODEL);
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
  switch (GetCpuFamily1()) {
    default:
      return CPU_PCIE_ULT_ULX_MAX_ROOT_PORT;
  }
}

/**
  Update FSP-S UPD config data.

  @param  FspsUpdPtr    The pointer to the FSP-S UPD to be updated.

**/
VOID
UpdateFspConfig (
  VOID     *FspsUpdPtr
  )
{
  FSPS_UPD                    *FspsUpd;
  FSP_S_CONFIG                *FspsConfig;
  SILICON_CFG_DATA            *SiCfgData;
  FEATURES_CFG_DATA           *FeaturesCfgData;
  POWER_CFG_DATA              *PowerCfgData;
  UINT8                       DebugPort;
  UINTN                       Length;
  UINT32                      SataCtrlIndex;
  UINT8                       Index;
  UINT8                       MaxPcieRootPorts;
  UINT8                       MaxSataPorts;
  UINT8                       PrIndex;
  UINT32                      SmiEn;
  UINT32                      BaseAddress;
  UINT32                      TotalSize;
  UINT32                      Address;
  UINT32                      VarSize;
  BOOLEAN                     BiosProtected;
  EFI_STATUS                  Status;
  VBIOS_VBT_STRUCTURE         *VbtPtr;
  UINT32                    *HdaVerbTablePtr;
  UINT8                     HdaVerbTableNum;

  Address              = 0;
  FspsUpd              = (FSPS_UPD *) FspsUpdPtr;
  FspsConfig           = &FspsUpd->FspsConfig;

  FspsUpd->FspUpdHeader.Signature = FSPS_UPD_SIGNATURE;
  FspsUpd->FspUpdHeader.Revision  = 0x2;

  FspsUpd->FspsArchUpd.EnableMultiPhaseSiliconInit = 0;

  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);

  //
  // Update device interrupt table
  //
  FspsConfig->DevIntConfigPtr   = (UINT32)(UINTN)mSocMDevIntConfig;
  FspsConfig->NumOfDevIntConfig = sizeof (mSocMDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);
  DEBUG ((DEBUG_INFO, "Updated Interrupt table\n"));


  if (GetBootMode () != BOOT_ON_FLASH_UPDATE) {
    BiosProtected = FALSE;
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
    }
    DEBUG (((BiosProtected) ? DEBUG_INFO : DEBUG_WARN, "BIOS SPI region will %a protected\n", (BiosProtected) ? "be" : "NOT BE"));
  }
  // Disble flash write firstly, so that UEFI payload variable could work
  // Later need refine the range to exclue UEFI variable range.
  //
  FspsConfig->PchWriteProtectionEnable[0]     = 0x0;

  if (PcdGetBool (PcdFramebufferInitEnabled)) {
    DEBUG ((DEBUG_INFO, "Frame Buffer Enabled\n"));
    FspsConfig->GraphicsConfigPtr = (UINT32)GetVbtAddress ();
    VbtPtr = (VBIOS_VBT_STRUCTURE*)(UINTN)(FspsConfig->GraphicsConfigPtr);
    FspsConfig->PeiGraphicsPeimInit = 1;
    if (VbtPtr != NULL) {
      DEBUG ((DEBUG_INFO, "VbtPtr != NULL\n"));
      FspsConfig->VbtSize = ((VbtPtr->HeaderVbtSize) & (UINT32)~(0x1FF)) + 0x200;
    }
    FspsConfig->LidStatus = 0x1;
  } else {
    DEBUG ((DEBUG_INFO, "Frame Buffer Disabled\n"));
    GetVbtAddress ();
    FspsConfig->GraphicsConfigPtr = 0;
    FspsConfig->PeiGraphicsPeimInit = 0;
    FspsConfig->LidStatus = 0x0;
  }

  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if (SiCfgData == NULL) {
    DEBUG ((DEBUG_INFO, "Failed to find Silicon Cfg Data\n"));
  } else {
    FspsUpd->FspsConfig.PchLegacyIoLowLatency     = SiCfgData->PchLegacyIoLowLatency;
    FspsUpd->FspsConfig.RenderStandby             = SiCfgData->RenderStandby; // IGFX RenderStandby

    // SATA
    SataCtrlIndex = 0;
    MaxSataPorts = MaxSataPortNum (SataCtrlIndex);
    for (SataCtrlIndex = 0; SataCtrlIndex < MAX_SATA_CONTROLLER; SataCtrlIndex++) {
      FspsConfig->SataEnable       = SiCfgData->SataEnable;
      FspsConfig->SataPwrOptEnable = SiCfgData->SataPwrOptEnable;
      FspsConfig->SataLedEnable    = SiCfgData->SataLedEnable;
      for (Index = 0; Index < MaxSataPorts; Index++) {
        FspsConfig->SataPortsDmVal[Index]   = SiCfgData->SataPortsDmVal[Index];
        FspsConfig->SataPortsDitoVal[Index] = SiCfgData->SataPortsDitoVal[Index];
        FspsConfig->SataPortsDevSlp[Index]  = SiCfgData->SataPortsDevSlp[Index];
        FspsConfig->SataPortsEnable[Index]  = SiCfgData->SataPortsEnable[Index];
      }
    }

    // USB
    CopyMem (&FspsConfig->PortUsb20Enable, SiCfgData->PortUsb20Enable, sizeof(SiCfgData->PortUsb20Enable));
    CopyMem (&FspsConfig->Usb2OverCurrentPin, SiCfgData->Usb2OverCurrentPin, sizeof(SiCfgData->Usb2OverCurrentPin));
    CopyMem (&FspsConfig->Usb3OverCurrentPin, SiCfgData->Usb3OverCurrentPin, sizeof(SiCfgData->Usb3OverCurrentPin));
    CopyMem (&FspsConfig->PortUsb30Enable, SiCfgData->PortUsb30Enable, sizeof(SiCfgData->PortUsb30Enable));
    for (Index = 0; Index < GetPchUsb2MaxPhysicalPortNum (); Index++) {
      CopyMem (&FspsConfig->Usb2PhyPetxiset, SiCfgData->Usb2PhyPetxiset, sizeof(SiCfgData->Usb2PhyPetxiset));
      CopyMem (&FspsConfig->Usb2PhyTxiset, SiCfgData->Usb2PhyTxiset, sizeof(SiCfgData->Usb2PhyTxiset));
      CopyMem (&FspsConfig->Usb2PhyPredeemp, SiCfgData->Usb2PhyPredeemp, sizeof(SiCfgData->Usb2PhyPredeemp));
      CopyMem (&FspsConfig->Usb2PhyPehalfbit, SiCfgData->Usb2PhyPehalfbit, sizeof(SiCfgData->Usb2PhyPehalfbit));
    }

    //PCH Serial UART
    Length = GetPchMaxSerialIoUartControllersNum ();
    DEBUG ((DEBUG_INFO, "GetPchMaxSerialIoUartControllersNum: %d\n", Length));
    for (Index = 0; Index < Length ; Index++) {
      FspsConfig->SerialIoUartMode[Index]        = SiCfgData->SerialIoUartMode[Index];
      FspsConfig->SerialIoUartPowerGating[Index] = SiCfgData->SerialIoUartPowerGating[Index];
      FspsConfig->SerialIoUartDmaEnable[Index]   = SiCfgData->SerialIoUartDmaEnable[Index];
      FspsConfig->SerialIoUartAutoFlow[Index]    = SiCfgData->SerialIoUartAutoFlow[Index];
    }
    DebugPort = GetDebugPort ();
    if (DebugPort < Length) {
      // Inform FSP to skip debug UART init
      FspsConfig->SerialIoUartMode[DebugPort] = 0x4;
    }

    // SPI
    FspsConfig->SerialIoSpiMode[1] = 0x1;
    CopyMem (&FspsConfig->SerialIoSpiCsPolarity, SiCfgData->SerialIoSpiCsPolarity, sizeof(SiCfgData->SerialIoSpiCsPolarity));

    // I2C
    Length = GetPchMaxSerialIoI2cControllersNum ();
    DEBUG ((DEBUG_INFO, "GetPchMaxSerialIoI2cControllersNum: %d\n", Length));
    for (Index = 0; Index < Length ; Index++) {
      FspsConfig->SerialIoI2cMode[Index]               = SiCfgData->SerialIoI2cMode[Index];
      FspsConfig->PchSerialIoI2cPadsTermination[Index] = SiCfgData->PchSerialIoI2cPadsTermination[Index];
    }

    for (Index = 0; Index < MAX_TCSS_USB3_PORTS; Index++) {
      FspsConfig->CpuUsb3OverCurrentPin[Index] = SiCfgData->CpuUsb3OverCurrentPin[Index];
    }

    // ISH
    CopyMem (&FspsConfig->PchIshI2cEnable, SiCfgData->PchIshI2cEnable, sizeof(SiCfgData->PchIshI2cEnable));
    CopyMem (&FspsConfig->PchIshGpEnable, SiCfgData->PchIshGpEnable, sizeof(SiCfgData->PchIshGpEnable));
    FspsConfig->PchIshI3cEnable = SiCfgData->PchIshI3cEnable;

    // Pcie Root Port
    CopyMem (&FspsConfig->PcieRpL1Substates, SiCfgData->PcieRpL1Substates, sizeof(SiCfgData->PcieRpL1Substates));
    CopyMem (&FspsConfig->PcieRpLtrEnable, SiCfgData->PcieRpLtrEnable, sizeof(SiCfgData->PcieRpLtrEnable));
    CopyMem (&FspsConfig->PcieRpClkReqDetect, SiCfgData->PcieRpClkReqDetect, sizeof(SiCfgData->PcieRpClkReqDetect));
    CopyMem (&FspsConfig->PcieRpAspm, SiCfgData->PcieRpAspm, sizeof(SiCfgData->PcieRpAspm));
    CopyMem (&FspsConfig->PciePtm, SiCfgData->PciePtm, sizeof(SiCfgData->PciePtm));
    CopyMem (&FspsConfig->PcieRpPcieSpeed, SiCfgData->PcieRpPcieSpeed, sizeof(SiCfgData->PcieRpPcieSpeed));
    CopyMem (&FspsConfig->PcieRpMaxPayload, SiCfgData->PcieRpMaxPayload, sizeof(SiCfgData->PcieRpMaxPayload));
    CopyMem (&FspsConfig->PcieRpAdvancedErrorReporting, SiCfgData->PcieRpAdvancedErrorReporting, sizeof(SiCfgData->PcieRpAdvancedErrorReporting));
    CopyMem (&FspsConfig->PtmEnabled, SiCfgData->PtmEnabled, sizeof(SiCfgData->PtmEnabled));

    MaxPcieRootPorts = GetPchMaxPciePortNum ();
    for (Index = 0; Index < MaxPcieRootPorts; Index++) {
      FspsConfig->PcieRpLtrMaxSnoopLatency[Index]           = 0x100F;
      FspsConfig->PcieRpLtrMaxNoSnoopLatency[Index]         = 0x100F;
      FspsConfig->PcieRpSnoopLatencyOverrideValue[Index]    = 0x41;
      FspsConfig->PcieRpNonSnoopLatencyOverrideValue[Index] = 0x41;
      FspsConfig->PcieGen3EqPh3NoOfPresetOrCoeff[Index]     = 0x5;
      FspsConfig->PcieGen3EqPh3PostCursor0List[Index]       = 0xc;
      FspsConfig->PcieGen3EqPh3PostCursor2List[Index]       = 0xa;
      FspsConfig->PcieGen3EqPh3PreCursor9List[Index]        = 0x0;
      FspsConfig->PcieGen3EqPh3Preset4List[Index]           = 0x8;
      FspsConfig->PcieGen3EqPh3Preset5List[Index]           = 0x0;
      FspsConfig->PcieGen4EqPh1DpTxPreset[Index]            = 0x7;
      FspsConfig->PcieGen4EqPh3NoOfPresetOrCoeff[Index]     = 0x3;
      FspsConfig->PcieGen4EqPh3PostCursor0List[Index]       = 0xc;
      FspsConfig->PcieGen4EqPh3PostCursor2List[Index]       = 0xa;
      FspsConfig->PcieGen5EqPh3NoOfPresetOrCoeff[Index]     = 0x4;
      FspsConfig->PcieGen5EqPh3PostCursor0List[Index]       = 0xe;
      FspsConfig->PcieGen5EqPh3PostCursor1List[Index]       = 0xa;
      FspsConfig->PcieGen5EqPh3PostCursor2List[Index]       = 0xc;
      FspsConfig->PcieGen5EqPh3PostCursor7List[Index]       = 0xa;
      FspsConfig->PcieGen5EqPh3PreCursor9List[Index]        = 0x8;
      FspsConfig->PcieRpUnsupportedRequestReport[Index]     = 0x1;
      FspsConfig->PcieRpFatalErrorReport[Index]             = 0x1;
      FspsConfig->PcieRpNoFatalErrorReport[Index]           = 0x1;
      FspsConfig->PcieRpCorrectableErrorReport[Index]       = 0x1;
      FspsConfig->PcieRpSnoopLatencyOverrideMode[Index]     = 0x2;
      FspsConfig->PcieRpNonSnoopLatencyOverrideMode[Index]  = 0x2;
      FspsConfig->PcieRpPhysicalSlotNumber[Index]           = (UINT8)Index;
    }
    FspsConfig->PcieGen3EqPh3Preset4List[11]               = 0x9;
    FspsConfig->PcieGen4EqPh3NoOfPresetOrCoeff[11]         = 0x5;

    for (Index = 0; Index < 8; Index++) {
      FspsConfig->TurboRatioLimitRatio[Index]                = SiCfgData->TurboRatioLimitRatio[Index];
      FspsConfig->AtomTurboRatioLimitRatio[Index]            = SiCfgData->AtomTurboRatioLimitRatio[Index];
    }
  } //End of SiCfgData Ptr

  for (Index = 0; Index < 8; Index++) {
    FspsConfig->PcieRpSnoopLatencyOverrideMode[Index]      = 0x2;
    FspsConfig->PcieRpNonSnoopLatencyOverrideMode[Index]   = 0x2;
    FspsConfig->TurboRatioLimitNumCore[Index]              = 0x0;
    FspsConfig->AtomTurboRatioLimitNumCore[Index]          = 0x0;
  }

  // Set VerbTable is disabled by default. Enable it only when specified by config data.
  if (FeaturesCfgData != NULL && (FeaturesCfgData->Features.HdaVerbTable != 0)){
    HdaVerbTablePtr = (UINT32 *) AllocateZeroPool (2 * sizeof (UINT32));  // max 6 tables supported for now
    if (HdaVerbTablePtr != NULL) {
      HdaVerbTableNum = 0;
      HdaVerbTablePtr[HdaVerbTableNum++]   = (UINT32)(UINTN) &HdaVerbTableAlc897;
      FspsConfig->PchHdaVerbTablePtr      = (UINT32)(UINTN) HdaVerbTablePtr;
      FspsConfig->PchHdaVerbTableEntryNum = HdaVerbTableNum;
    }
  }
  if(GetPayloadId () == 0) {
    // Disable SMI sources
    SmiEn = IoRead32((UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
    IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), SmiEn & (UINT32) (~B_ACPI_IO_SMI_EN_GBL_SMI));
    // Lock down SMI
    FspsConfig->PchLockDownGlobalSmi = 0x1;
  }

  //Disable VMD by default
  FspsConfig->VmdEnable = 0;

  // PCH Flash protection
  FspsConfig->PchPwrOptEnable             = 0x1;

  // MISC
  FspsConfig->PchFivrExtV1p05RailVoltage = 0x0;
  FspsConfig->PchFivrExtVnnRailVoltage = 0x0;
  FspsConfig->PchFivrExtVnnRailSxVoltage = 0x0;
  FspsConfig->PchFivrVccinAuxLowToHighCurModeVolTranTime = 0x0;
  FspsConfig->PchFivrVccinAuxRetToHighCurModeVolTranTime = 0x0;
  FspsConfig->PchFivrVccinAuxRetToLowCurModeVolTranTime = 0x0;
  FspsConfig->PchFivrVccinAuxOffToHighCurModeVolTranTime = 0x0;
  FspsConfig->PchFivrDynPm = 0x0;
  FspsConfig->PchFivrExtV1p05RailIccMaximum = 0x0;
  FspsConfig->PchFivrExtVnnRailIccMaximum = 0x0;
  FspsConfig->PchFivrExtVnnRailSxIccMaximum = 0x0;
  FspsConfig->PchPmSlpAMinAssert = 0x0;
  FspsConfig->PchPmPwrBtnOverridePeriod = 0x3;
  FspsConfig->PchEspiLgmrEnable = 0x0;
  FspsConfig->PmcModPhySusPgEnable = 0x0;
  FspsConfig->D3HotEnable = 0x1;
  FspsConfig->IomTypeCPortPadCfg[0] = 0x140481;
  FspsConfig->IomTypeCPortPadCfg[1] = 0x140482;
  FspsConfig->IomTypeCPortPadCfg[2] = 0x0;
  FspsConfig->IomTypeCPortPadCfg[3] = 0x0;
  FspsConfig->DmiTS0TW = 0x1;
  FspsConfig->DmiTS1TW = 0x1;
  FspsConfig->DmiTS2TW = 0x2;
  FspsConfig->MeUnconfigOnRtcClear = 0x1;
  FspsConfig->PsOnEnable = 0x1;
  FspsConfig->PmcAdrTimerEn = 0x1;
  FspsConfig->PmcAdrTimer1Val = 0x20;
  FspsConfig->PmcAdrMultiplier1Val = 0x6;
  FspsConfig->PmcAdrHostPartitionReset = 0x1;
  FspsConfig->PmcLpmS0ixSubStateEnableMask = 0x7;
  FspsConfig->PmcUsb2PhySusPgEnable = 0x0;


  // Force Eiss and BiosLock off for now.
  // Enable it later in OS loader/EndofStages
  FspsConfig->PchLockDownBiosLock = 0x0;

  // Disable IEH
  FspsConfig->IehMode = 0x0;
  FspsConfig->PchCrid = 0x0;
  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    //
    // WA - Microcode issue is causing assert in FSP during
    // firmware update mode. Avoiding this UPD set until
    // microcode issue is resolved.
    //
    FspsUpd->FspsConfig.PchLockDownBiosInterface = FALSE;
    FspsUpd->FspsConfig.PchSbAccessUnlock        = TRUE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock = FALSE;
    DEBUG ((DEBUG_INFO, "Firmware update mode, unlock Bios setting\n"));
  } else {
    FspsUpd->FspsConfig.PchLockDownBiosInterface = TRUE;
    FspsUpd->FspsConfig.PchSbAccessUnlock        = FALSE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock = TRUE;
  }

  FspsConfig->AmtEnabled = 0x1;
  FspsConfig->EnableTcoTimer = 0x1;

  PowerCfgData = (POWER_CFG_DATA *) FindConfigDataByTag (CDATA_POWER_TAG);
  if (PowerCfgData == NULL) {
    DEBUG ((DEBUG_ERROR, "Missing power Cfg Data!\n"));
  } else {
    //CPU Power Management Basic Config
    FspsConfig->Hwp                           = PowerCfgData->Hwp;
    FspsConfig->PowerLimit2                   = PowerCfgData->PowerLimit2;
    FspsConfig->TurboPowerLimitLock           = PowerCfgData->TurboPowerLimitLock;
    FspsConfig->PowerLimit3DutyCycle          = PowerCfgData->PowerLimit3DutyCycle;
    FspsConfig->PowerLimit3Lock               = PowerCfgData->PowerLimit3Lock;
    FspsConfig->PowerLimit4Lock               = PowerCfgData->PowerLimit4Lock;
    FspsConfig->TccOffsetClamp                = PowerCfgData->TccOffsetClamp;
    FspsConfig->TccOffsetLock                 = PowerCfgData->TccOffsetLock;
    FspsConfig->TurboMode                     = PowerCfgData->TurboMode;
    FspsConfig->TccActivationOffset           = PowerCfgData->TccActivationOffset;
    FspsConfig->PowerLimit1                   = PowerCfgData->PowerLimit1;
    FspsConfig->PowerLimit2Power              = PowerCfgData->PowerLimit2Power;
    FspsConfig->PowerLimit3                   = PowerCfgData->PowerLimit3;
    FspsConfig->PowerLimit4                   = PowerCfgData->PowerLimit4;
    FspsConfig->PowerLimit1Time               = PowerCfgData->PowerLimit1Time;
    FspsConfig->PowerLimit3Time               = PowerCfgData->PowerLimit3Time;
    FspsConfig->TccOffsetTimeWindowForRatl    = PowerCfgData->TccOffsetTimeWindowForRatl;
    FspsConfig->HwpInterruptControl           = PowerCfgData->HwpInterruptControl;
    FspsConfig->EnablePerCorePState           = PowerCfgData->EnablePerCorePState;
    FspsConfig->EnableHwpAutoPerCorePstate    = PowerCfgData->EnableHwpAutoPerCorePstate;
    FspsConfig->EnableHwpAutoEppGrouping      = PowerCfgData->EnableHwpAutoEppGrouping;
    FspsConfig->MinRingRatioLimit             = PowerCfgData->MinRingRatioLimit;
    FspsConfig->MaxRingRatioLimit             = PowerCfgData->MaxRingRatioLimit;

    //CPU Power Management Custom Config
    FspsConfig->MaxRatio                      = PowerCfgData->MaxRatio;
    FspsConfig->Custom1PowerLimit1            = PowerCfgData->Custom1PowerLimit1;
    FspsConfig->Custom1PowerLimit2            = PowerCfgData->Custom1PowerLimit2;
    FspsConfig->Custom1PowerLimit1Time        = PowerCfgData->Custom1PowerLimit1Time;
    FspsConfig->Custom1TurboActivationRatio   = PowerCfgData->Custom1TurboActivationRatio;
    FspsConfig->Custom2PowerLimit1            = PowerCfgData->Custom2PowerLimit1;
    FspsConfig->Custom2PowerLimit2            = PowerCfgData->Custom2PowerLimit2;
    FspsConfig->Custom2PowerLimit1Time        = PowerCfgData->Custom2PowerLimit1Time;
    FspsConfig->Custom2TurboActivationRatio   = PowerCfgData->Custom2TurboActivationRatio;
    FspsConfig->Custom3PowerLimit1            = PowerCfgData->Custom3PowerLimit1;
    FspsConfig->Custom3PowerLimit2            = PowerCfgData->Custom3PowerLimit2;
    FspsConfig->Custom3PowerLimit1Time        = PowerCfgData->Custom3PowerLimit1Time;
    FspsConfig->Custom3TurboActivationRatio   = PowerCfgData->Custom3TurboActivationRatio;

    //CPU Power Management Psys Config
    FspsConfig->PsysPowerLimit1Power          = PowerCfgData->PsysPowerLimit1Power;
    FspsConfig->PsysPowerLimit2Power          = PowerCfgData->PsysPowerLimit2Power;
    FspsConfig->PsysPowerLimit1               = PowerCfgData->PsysPowerLimit1;
    FspsConfig->PsysPowerLimit1Time           = PowerCfgData->PsysPowerLimit1Time;
    FspsConfig->PsysPowerLimit2               = PowerCfgData->PsysPowerLimit2;

    //CPU Power Management Test Config
    FspsConfig->Eist                          = PowerCfgData->Eist;
    FspsConfig->EnergyEfficientPState         = PowerCfgData->EnergyEfficientPState;
    FspsConfig->EnergyEfficientTurbo          = PowerCfgData->EnergyEfficientTurbo;
    FspsConfig->TStates                       = PowerCfgData->TStates;
    FspsConfig->BiProcHot                     = PowerCfgData->BiProcHot;
    FspsConfig->DisableProcHotOut             = PowerCfgData->DisableProcHotOut;
    FspsConfig->ProcHotResponse               = PowerCfgData->ProcHotResponse;
    FspsConfig->DisableVrThermalAlert         = PowerCfgData->DisableVrThermalAlert;
    FspsConfig->EnableAllThermalFunctions     = PowerCfgData->EnableAllThermalFunctions;
    FspsConfig->ThermalMonitor                = PowerCfgData->ThermalMonitor;
    FspsConfig->Cx                            = PowerCfgData->Cx;
    FspsConfig->PmgCstCfgCtrlLock             = PowerCfgData->PmgCstCfgCtrlLock;
    FspsConfig->C1e                           = PowerCfgData->C1e;
    FspsConfig->C1StateAutoDemotion           = PowerCfgData->C1StateAutoDemotion;
    FspsConfig->C1StateUnDemotion             = PowerCfgData->C1StateUnDemotion;
    FspsConfig->CStatePreWake                 = PowerCfgData->CStatePreWake;
    FspsConfig->TimedMwait                    = PowerCfgData->TimedMwait;
    FspsConfig->CstCfgCtrIoMwaitRedirection   = PowerCfgData->CstCfgCtrIoMwaitRedirection;
    FspsConfig->ProcHotLock                   = PowerCfgData->ProcHotLock;
    FspsConfig->ConfigTdpLevel                = PowerCfgData->ConfigTdpLevel;
    FspsConfig->RaceToHalt                    = PowerCfgData->RaceToHalt;
    FspsConfig->PkgCStateLimit                = PowerCfgData->PkgCStateLimit;
  }

  //Misc Fsps Upd
  FspsConfig->D3ColdEnable                    = 0x0;
  if (FspsConfig->VmdEnable == 0x1){
    for (Index = 0 ; Index < 31 ; Index++){
      FspsConfig->VmdPort[Index] = 0;
    }
  }
  CopyMem (&FspsConfig->EnableTcssCovTypeA, SiCfgData->EnableTcssCovTypeA, sizeof(SiCfgData->EnableTcssCovTypeA));
  FspsConfig->ITbtPcieRootPortEn[0]           = 0x1;
  FspsConfig->ITbtPcieRootPortEn[1]           = 0x1;
  FspsConfig->ITbtPcieRootPortEn[2]           = 0x1;
  FspsConfig->ITbtPcieRootPortEn[3]           = 0x1;
  FspsConfig->UsbTcPortEn                     = 0xf;
  FspsConfig->CpuCrashLogEnable               = 0x0;
  FspsConfig->PtmEnabled[0]                   = 0x1;
  FspsConfig->PtmEnabled[1]                   = 0x1;
  FspsConfig->PtmEnabled[2]                   = 0x1;
  FspsConfig->PtmEnabled[3]                   = 0x1;
  FspsConfig->UfsEnable[0]                    = 0x1;
  FspsConfig->UfsInlineEncryption[0]          = 0x1;
  FspsConfig->PortResetMessageEnable[0]       = 0x1;
  FspsConfig->PortResetMessageEnable[1]       = 0x1;
  FspsConfig->PortResetMessageEnable[2]       = 0x1;
  FspsConfig->PortResetMessageEnable[3]       = 0x1;
  FspsConfig->PchDmiAspmCtrl                  = 0x4;
  FspsConfig->ThermalMonitor                  = 0x0;
  FspsConfig->MaxRatio                        = 0x4;
  FspsConfig->X2ApicEnable                    = 0x0;
  FspsConfig->PchUnlockGpioPads               = 0x1;
  FspsConfig->PchPmDisableEnergyReport        = 0x0;
  FspsConfig->XdciEnable                      = 0x0;
  FspsConfig->IshUartRxPinMuxing[1]           = 0x146806;
  FspsConfig->IshUartTxPinMuxing[1]           = 0x146807;
  FspsConfig->IshI2cSdaPinMuxing[2]           = 0x143012;
  FspsConfig->IshI2cSclPinMuxing[2]           = 0x143013;
  FspsConfig->IshSpiMosiPinMuxing[0]          = 0x147088;
  FspsConfig->IshSpiMisoPinMuxing[0]          = 0x147087;
  FspsConfig->IshSpiClkPinMuxing[0]           = 0x147086;
  FspsConfig->IshSpiCsPinMuxing[0]            = 0x147085;
  FspsConfig->SerialIoUartRxPinMuxPolicy[1]   = 0x144806;
  FspsConfig->SerialIoUartTxPinMuxPolicy[1]   = 0x144807;
  FspsConfig->PchSerialIoI2cSdaPinMux[2]      = 0x142804;
  FspsConfig->PchSerialIoI2cSdaPinMux[3]      = 0x142806;
  FspsConfig->PchSerialIoI2cSdaPinMux[4]      = 0x15048c;
  FspsConfig->PchSerialIoI2cSdaPinMux[5]      = 0x15088d;
  FspsConfig->PchSerialIoI2cSclPinMux[2]      = 0x142805;
  FspsConfig->PchSerialIoI2cSclPinMux[3]      = 0x142807;
  FspsConfig->PchSerialIoI2cSclPinMux[4]      = 0x15048d;
  FspsConfig->PchSerialIoI2cSclPinMux[5]      = 0x15088c;
  FspsConfig->SerialIoI3cSdaPinMux[1]         = 0x144815;
  FspsConfig->SerialIoI3cSclPinMux[1]         = 0x144816;
  FspsConfig->SerialIoI3cSclFbPinMux[1]       = 0x144818;
  FspsConfig->IshGpGpioPinMuxing[5]           = 0x149016;
  FspsConfig->IshGpGpioPinMuxing[6]           = 0x149017;
  FspsConfig->IshGpGpioPinMuxing[8]           = 0x149014;
  FspsConfig->IshGpGpioPinMuxing[9]           = 0x150897;
  FspsConfig->IshGpGpioPinMuxing[10]          = 0x150490;
  FspsConfig->IshGpGpioPinMuxing[11]          = 0x150889;
  FspsConfig->VccSt                           = 0x1;
  FspsConfig->PchPmSlpAMinAssert              = 0x4;
  FspsConfig->CnviBtAudioOffload              = 0x1;
  FspsConfig->FwProgress                      = 0x1;
  FspsConfig->PmcV1p05PhyExtFetControlEn      = 0x1;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[0] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[1] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[2] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[3] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[4] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[5] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[6] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[7] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[8] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[9] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[10] = 0x0;
  FspsConfig->PcieRpLtrOverrideSpecComplaint[11] = 0x0;
  FspsConfig->TcssAuxOri = 0x1;
  FspsConfig->SerialIoSpiCsPinMux[0] = 0x14a48a;
  FspsConfig->SerialIoSpiClkPinMux[0] = 0x14a48b;
  FspsConfig->SerialIoSpiMisoPinMux[0] = 0x14a48c;
  FspsConfig->SerialIoSpiMosiPinMux[0] = 0x14a48d;
  FspsConfig->DmiPowerGatingDis = 0x0;

  // EndOfPost Upd
  FspsUpd->FspsConfig.EndOfPostMessage = 1;
  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.S0ix == 1) {
      FspsConfig->C1e = 1;
      FspsConfig->Cx = 1;
      FspsConfig->XdciEnable = 0;
      FspsConfig->PchLanEnable = 0x0;
      FspsConfig->GnaEnable = 0x0;
      FspsConfig->VpuEnable = 0x0;
      ZeroMem (FspsConfig->PchIshI2cEnable, sizeof (FspsConfig->PchIshI2cEnable));
      ZeroMem (FspsConfig->PchIshGpEnable, sizeof (FspsConfig->PchIshGpEnable));
      ZeroMem (FspsConfig->ITbtPcieRootPortEn, sizeof (FspsConfig->ITbtPcieRootPortEn));
      FspsConfig->RenderStandby = 1;
      FspsConfig->Enable8254ClockGating = 1;
      FspsConfig->PchFivrDynPm = 1;
      FspsConfig->D3HotEnable = 0;
      FspsConfig->D3ColdEnable = 1;
      DEBUG ((DEBUG_INFO, "Stage 2 S0ix config applied.\n"));
    }
  }
}
