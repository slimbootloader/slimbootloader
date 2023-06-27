/** @file

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
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
#include "TsnSubRegion.h"
#include <Library/PciePm.h>
#include <IndustryStandard/UefiTcgPlatform.h>
#include <Library/TpmLib.h>
#include <Library/FusaConfigLib.h>

#define CPU_PCIE_DT_HALO_MAX_ROOT_PORT     3
#define CPU_PCIE_ULT_ULX_MAX_ROOT_PORT     3
#define MAX_TCSS_USB3_PORTS                4
#define TURBO_RATIO_LIMIT_ARRAY_SIZE       8

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

//
// This table contains data on INTx and IRQ for PCH-N
//
SI_PCH_DEVICE_INTERRUPT_CONFIG mPchNDevIntConfig[] = {
//  {31, 0, PchNoInt, 0}, // LPC/eSPI Interface, doesn't use interrupts
//  {31, 1, PchNoInt, 0}, // P2SB, doesn't use interrupts
//  {31, 2, PchNoInt, 0}, // PMC , doesn't use interrupts
  {31, 3, SiPchIntD, 19}, // cAVS(Audio, Voice, Speach), INTD is default, programmed in PciCfgSpace 3Dh
  {31, 4, SiPchIntA, 16}, // SMBus Controller, no default value, programmed in PciCfgSpace 3Dh
//  {31, 5, PchNoInt, 0}, // SPI , doesn't use interrupts
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
  {26, 0, SiPchIntA, 16}, // SCS: eMMC //ADLN specific
  {25, 2, SiPchIntC, 42}, // SerialIo UART Controller #2, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[9]
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
  {21, 3, SiPchIntD, 33}, // SerialIo I2C Controller #3, INTA is default, programmed in PCR[SERIALIO] + PCICFGCTRL[4] //ADLN specific
//  {21, 7, SiPchIntA, 16}, // Northpeak Phantom (ACPI) Dunction - not exposed to OS
  {20, 0, SiPchIntA, 16}, // USB 3.0 xHCI Controller, no default value, programmed in PciCfgSpace 3Dh
  {20, 1, SiPchIntB, 17}, // USB Device Controller (OTG)
  {20, 3, SiPchIntA, 16}, // CNVi WiFi
  //{20, 2, PchNoInt, 0}, // Shared SRAM, no interrupts
//  {20, 4, 0, 0}, // TraceHub Phantom (ACPI) Function
//  {18, 2, PchNoInt, 0}, // CSME: PMT,  doesn't use interrupts
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
  // Starting from here is for all Lp series: mPchLpOnlyDevIntConfig which includes N as well
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
  {18, 7, SiPchIntC, 18}  // SCS: UFS
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
    case CPUID_FULL_FAMILY_MODEL_ALDERLAKE_DT_HALO:
      return CPU_PCIE_DT_HALO_MAX_ROOT_PORT;
    default:
      return CPU_PCIE_ULT_ULX_MAX_ROOT_PORT;
  }
}

#if FixedPcdGet8 (PcdTccEnabled)
/**
  Update FSP-S UPD config data for TCC mode and tuning

  @param  FspmUpd                       The pointer to the FSP-S UPD to be updated.

  @retval EFI_NOT_FOUND                 Platform Features or Tcc mode not found
          EFI_ERROR                     Error trying to load sub-region
          EFI_SUCCESS                   Successfully loaded subregions
**/
EFI_STATUS
TccModePostMemConfig (
  FSPS_UPD  *FspsUpd
)
{
  UINT32                                    *TccCacheConfigBase;
  UINT32                                     TccCacheConfigSize;
  UINT32                                    *TccCrlBase;
  UINT32                                     TccCrlSize;
  UINT32                                    *TccStreamBase;
  UINT32                                     TccStreamSize;
  UINT8                                      Index;
  UINT8                                      MaxPchPcieRootPorts;
  UINT8                                      MaxCpuPciePorts;
  BIOS_SETTINGS                             *PolicyConfig;
  TCC_STREAM_CONFIGURATION                  *StreamConfig;
  TCC_CFG_DATA                              *TccCfgData;
  EFI_STATUS                                 Status;
  PLATFORM_DATA                             *PlatformData;
  EFI_PLATFORM_FIRMWARE_BLOB                 TccStreamCfgBlob;
  EFI_PLATFORM_FIRMWARE_BLOB                 TccCacheCfgBlob;
  EFI_PLATFORM_FIRMWARE_BLOB                 TccCrlBlob;

  Status = EFI_SUCCESS;

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
  if ((TccCfgData == NULL) || (TccCfgData->TccEnable == 0)) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "Tcc is enabled, setting Tcc Silicon Config\n"));

  // TCC related Silicon settings
  //FspsUpd->FspsConfig.Eist                    = 0;            // Intel Speed Step->EnableGv
  //FspsUpd->FspsConfig.Hwp                     = 0;            // Intel Speed Shift ADL B0 is causing issues.
  FspsUpd->FspsConfig.Cx                        = 0;            // Intel C-states
  FspsUpd->FspsConfig.PsfTccEnable              = 1;            // Enable will decrease psf transaction latency by disabling some psf power management features
  FspsUpd->FspsConfig.PchDmiAspmCtrl            = 0;            // ASPM configuration on the PCH side of the DMI/OPI Link
  FspsUpd->FspsConfig.PchLegacyIoLowLatency     = 1;
  FspsUpd->FspsConfig.PchTsnMultiVcEnable       = 1;
  FspsUpd->FspsConfig.OpioRecenter              = 0;

  MaxPchPcieRootPorts = GetPchMaxPciePortNum ();
  for (Index = 0; Index < MaxPchPcieRootPorts; Index++) {
    FspsUpd->FspsConfig.PcieRpAspm[Index]           = 0 ;       // The Pcie ASPM configuration of the root port
    FspsUpd->FspsConfig.PcieRpL1Substates[Index]    = 0;        // The Pcie L1 Substates configuration of the root port
    FspsUpd->FspsConfig.PciePtm[Index]              = 1;
  }

  FspsUpd->FspsConfig.RenderStandby                    = 0;      // IGFX RenderStandby
  MaxCpuPciePorts = GetMaxCpuPciePortNum();
  for (Index = 0; Index < MaxCpuPciePorts; Index++) {
    FspsUpd->FspsConfig.CpuPcieRpAspm[Index]           = 0;     // Cpu Aspm
    FspsUpd->FspsConfig.CpuPcieRpL1Substates[Index]    = 0;     // Cpu L1 substates
    FspsUpd->FspsConfig.CpuPcieRpPtmEnabled[Index]     = 1;
    FspsUpd->FspsConfig.CpuPcieClockGating[Index]      = 0;     // Describes whether PCI Express Clock Gating for each root port is enabled by platform modules
    // FspsUpd->FspsConfig.CpuPcieRpVcEnabled[Index]   = 1;
    FspsUpd->FspsConfig.CpuPcieRpMultiVcEnabled[Index] = 1;
  }

  FspsUpd->FspsConfig.SoftwareSramEn  = TccCfgData->TccSoftSram;
  FspsUpd->FspsConfig.DsoTuningEn     = TccCfgData->TccTuning;
  FspsUpd->FspsConfig.TccErrorLogEn   = TccCfgData->TccErrorLog;
  FspsUpd->FspsConfig.IfuEnable       = 0;

  if (!IsWdtFlagsSet(WDT_FLAG_TCC_DSO_IN_PROGRESS)) {
    //
    // If FSPM doesn't enable TCC DSO timer, FSPS should also skip TCC DSO.
    //
    DEBUG ((DEBUG_INFO, "DSO Tuning skipped.\n"));
    FspsUpd->FspsConfig.TccStreamCfgStatus = 1;
  } else if (TccCfgData->TccTuning != 0) {
    // Reload Watch dog timer
    WdtReloadAndStart (WDT_TIMEOUT_TCC_DSO, WDT_FLAG_TCC_DSO_IN_PROGRESS);

    // Load TCC stream config from container
    TccStreamBase = NULL;
    TccStreamSize = 0;
    Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'T'),
                            (VOID **)&TccStreamBase, &TccStreamSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "TCC Stream not found! %r\n", Status));
    } else {
      if ((TccStreamSize >= sizeof (TCC_STREAM_CONFIGURATION)) && (TccStreamBase != NULL)) {
        FspsUpd->FspsConfig.TccStreamCfgBase = (UINT32)(UINTN)TccStreamBase;
        FspsUpd->FspsConfig.TccStreamCfgSize = TccStreamSize;

        DEBUG ((DEBUG_INFO, "Load Tcc Stream @0x%p, size = 0x%x\n", TccStreamBase, TccStreamSize));
        DEBUG ((DEBUG_INFO, "Dump Tcc Stream with MIN(0x40, TccStreamSize):\n"));
        DumpHex (2, 0, MIN(0x40, TccStreamSize), TccStreamBase);

        if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
          TccStreamCfgBlob.BlobBase = (UINT64)(UINTN)TccStreamBase;
          TccStreamCfgBlob.BlobLength = TccStreamSize;
          TpmHashAndExtendPcrEventLog (0, (UINT8 *)TccStreamBase, TccStreamSize, EV_PLATFORM_CONFIG_FLAGS, sizeof(TccStreamCfgBlob), (UINT8 *)&TccStreamCfgBlob);
        }

       // Override Tcc settings from Streams
          StreamConfig   = (TCC_STREAM_CONFIGURATION *) TccStreamBase;
          PolicyConfig = (BIOS_SETTINGS *) &StreamConfig->BiosSettings;

          if (StreamConfig->Version == TCC_STREAM_CONFIGURATION_VERSION) {
            FspsUpd->FspsConfig.Eist                  = PolicyConfig->Pstates;
            FspsUpd->FspsConfig.Hwp                   = PolicyConfig->HwpEn;
            FspsUpd->FspsConfig.Cx                    = PolicyConfig->Cstates;
            FspsUpd->FspsConfig.TurboMode             = PolicyConfig->Turbo;
            FspsUpd->FspsConfig.PsfTccEnable          = PolicyConfig->FabricPm;
            FspsUpd->FspsConfig.PchDmiAspmCtrl        = PolicyConfig->DmiAspm;
            FspsUpd->FspsConfig.PchLegacyIoLowLatency = PolicyConfig->PchPwrClkGate;
            DEBUG ((DEBUG_INFO, "Pstates\n HwpEn: %d\n EnableHwp: %d\n EnableCx: %d\n TurboMode:%d\n PsfTccEnable:%d\n PchDmiAspmCtrl:%d\n PchLegacyIoLowLatency:%d\n",PolicyConfig->Pstates, PolicyConfig->HwpEn, PolicyConfig->Cstates, PolicyConfig->Turbo, PolicyConfig->FabricPm, PolicyConfig->DmiAspm, PolicyConfig->PchPwrClkGate ));
            for (Index = 0; Index < MaxPchPcieRootPorts; Index++) {
              FspsUpd->FspsConfig.PcieRpAspm[Index]        = PolicyConfig->PchPcieAspm;
              FspsUpd->FspsConfig.PcieRpL1Substates[Index] = PolicyConfig->PchPcieRpL1;
            }
            FspsUpd->FspsConfig.RenderStandby      = PolicyConfig->GtRstRc6;

            for (Index = 0; Index < MaxCpuPciePorts; Index++) {
               FspsUpd->FspsConfig.CpuPcieClockGating[Index]   = PolicyConfig->PcieClkGate;
               FspsUpd->FspsConfig.CpuPcieRpAspm[Index]        = PolicyConfig->CpuPcieAspm;
               FspsUpd->FspsConfig.CpuPcieRpL1Substates[Index] = PolicyConfig->CpuPcieRpL1;
               DEBUG ((DEBUG_INFO, " PcieClkGate[%d]: %d\n PcieRootPortL1SubStates[%d]: %d\n PcieRootPortClockGating[%d]: %d\n", Index, PolicyConfig->PcieClkGate, Index, PolicyConfig->CpuPcieAspm, Index, PolicyConfig->CpuPcieRpL1));
            }

            PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
            if(PlatformData != NULL){
              PlatformData->PlatformFeatures.TccRtd3Support    = PolicyConfig->Dstates;
              PlatformData->PlatformFeatures.TccLowPowerS0Idle = PolicyConfig->Sstates;
              PlatformData->PlatformFeatures.TccDsoTuning      = TRUE;
            }
          }
      }
    }
  }

  // Load Tcc cache config binary from container
  TccCacheConfigBase = NULL;
  TccCacheConfigSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'C'),
                                (VOID **)&TccCacheConfigBase, &TccCacheConfigSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TCC  Cache config not found! %r\n", Status));
  } else {
    FspsUpd->FspsConfig.TccCacheCfgBase = (UINT32)(UINTN)TccCacheConfigBase;
    FspsUpd->FspsConfig.TccCacheCfgSize = TccCacheConfigSize;
    DEBUG ((DEBUG_INFO, "Load Tcc Cache @0x%p, size = 0x%x\n", TccCacheConfigBase, TccCacheConfigSize));

    if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
      TccCacheCfgBlob.BlobBase = (UINT64)(UINTN)TccCacheConfigBase;
      TccCacheCfgBlob.BlobLength = TccCacheConfigSize;
      TpmHashAndExtendPcrEventLog (0, (UINT8 *)TccCacheConfigBase, TccCacheConfigSize, EV_PLATFORM_CONFIG_FLAGS, sizeof(TccCacheCfgBlob), (UINT8 *)&TccCacheCfgBlob);
    }

    FspsUpd->FspsConfig.TccMode = 1;
#if FixedPcdGet8(PcdAdlNSupport) == 0
    FspsUpd->FspsConfig.L2QosEnumerationEn = 1;
#endif
  }

  // Load Tcc Crl binary from container
  TccCrlBase = NULL;
  TccCrlSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'M'),
                                (VOID **)&TccCrlBase, &TccCrlSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TCC Crl not found! %r\n", Status));
  } else {
    FspsUpd->FspsConfig.TccCrlBinBase = (UINT32)(UINTN)TccCrlBase;
    FspsUpd->FspsConfig.TccCrlBinSize = TccCrlSize;
    DEBUG ((DEBUG_INFO, "Load Tcc Crl @0x%p, size = 0x%x\n", TccCrlBase, TccCrlSize));
    DumpHex (2, 0, MIN(0x40, TccCrlSize), TccCrlBase);

    if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
      TccCrlBlob.BlobBase = (UINT64)(UINTN)TccCrlBase;
      TccCrlBlob.BlobLength = TccCrlSize;
      TpmHashAndExtendPcrEventLog (0, (UINT8 *)TccCrlBase, TccCrlSize, EV_EFI_PLATFORM_FIRMWARE_BLOB, sizeof(TccCrlBlob), (UINT8 *)&TccCrlBlob);
    }
  }

  return Status;
}
#endif

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
  TSN_MAC_ADDR_SUB_REGION     *TsnSubRegion;
  UINT32                      *TsnMacAddrBase;
  UINT32                      TsnMacAddrSize;
  UINT32                      BaseAddress;
  UINT32                      TotalSize;
  UINT32                      Address;
  UINT32                      VarSize;
  BOOLEAN                     BiosProtected;
  EFI_STATUS                  Status;
  VBIOS_VBT_STRUCTURE         *VbtPtr;
  EFI_PLATFORM_FIRMWARE_BLOB  TsnCfgBlob;

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
  if (IsPchS ()) {
    DEBUG ((DEBUG_INFO, "Pch-S is detected\n"));
    FspsConfig->DevIntConfigPtr   = (UINT32)(UINTN)mPchSDevIntConfig;
    FspsConfig->NumOfDevIntConfig = sizeof (mPchSDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);
  } else if (IsPchP ()) {
    DEBUG ((DEBUG_INFO, "Pch-P is detected\n"));
    FspsConfig->DevIntConfigPtr   = (UINT32)(UINTN)mPchPDevIntConfig;
    FspsConfig->NumOfDevIntConfig = sizeof (mPchPDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);
  } else if (IsPchN ()) {
    DEBUG ((DEBUG_INFO, "Pch-N is detected\n"));
    FspsConfig->DevIntConfigPtr   = (UINT32)(UINTN)mPchNDevIntConfig;
    FspsConfig->NumOfDevIntConfig = sizeof (mPchNDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);
  } else {
    DEBUG ((DEBUG_ERROR, "Unsupported PCH.\n"));
  }

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

  if (PcdGetBool (PcdFramebufferInitEnabled) && (GetBootMode() != BOOT_ON_S3_RESUME)) {
    DEBUG ((DEBUG_INFO, "Frame Buffer Enabled\n"));
    FspsConfig->GraphicsConfigPtr = (UINT32)GetVbtAddress ();
    VbtPtr = (VBIOS_VBT_STRUCTURE*)(UINTN)(FspsConfig->GraphicsConfigPtr);
    //
    // Update VBT for the board
    //
    Status = UpdateVbt (VbtPtr);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Could not update VBT\n"));
    }
    FspsConfig->PeiGraphicsPeimInit = 1;
    if (VbtPtr != NULL) {
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
    FspsUpd->FspsConfig.AcSplitLock               = SiCfgData->AcSplitLock;   // AC check on split locks
#if FixedPcdGet8 (PcdTccEnabled)
    FspsUpd->FspsConfig.PsfTccEnable              = SiCfgData->PsfTccEnable;  // Enable will decrease psf transaction latency by disabling psf power mgmt features
#endif
    FspsUpd->FspsConfig.PchDmiAspmCtrl            = SiCfgData->PchDmiAspmCtrl;// ASPM configuration on the PCH side of the DMI/OPI Link
    FspsUpd->FspsConfig.PchLegacyIoLowLatency     = SiCfgData->PchLegacyIoLowLatency;
    FspsUpd->FspsConfig.RenderStandby             = SiCfgData->RenderStandby; // IGFX RenderStandby
    FspsUpd->FspsConfig.PmSupport                 = SiCfgData->PmSupport;     // IGFX PmSupport
    MaxPcieRootPorts = GetMaxCpuPciePortNum ();
    for (Index = 0; Index < MaxPcieRootPorts; Index++) {
      FspsUpd->FspsConfig.CpuPcieClockGating[Index]      = SiCfgData->CpuPcieClockGating;   // Describes whether PCI Express Clock Gating for each root port is enabled by platform modules
      FspsUpd->FspsConfig.CpuPcieRpAspm[Index]           = SiCfgData->CpuPcieRpAspm[Index];        // Cpu Aspm
      FspsUpd->FspsConfig.CpuPcieRpL1Substates[Index]    = SiCfgData->CpuPcieRpL1Substates[Index]; // Cpu L1 substates
      FspsUpd->FspsConfig.CpuPcieRpPtmEnabled[Index]     = SiCfgData->CpuPcieRpPtmEnabled[Index] ;
      FspsUpd->FspsConfig.CpuPcieRpMultiVcEnabled[Index] = SiCfgData->CpuPcieRpMultiVcEnabled[Index];
    }

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
    for (Index = 0; Index < Length ; Index++) {
      FspsConfig->SerialIoUartMode[Index]        = SiCfgData->SerialIoUartMode[Index];
      FspsConfig->SerialIoUartPowerGating[Index] = SiCfgData->SerialIoUartPowerGating[Index];
      FspsConfig->SerialIoUartDmaEnable[Index]   = SiCfgData->SerialIoUartDmaEnable[Index];
      FspsConfig->SerialIoUartAutoFlow[Index]    = SiCfgData->SerialIoUartAutoFlow[Index];
    }
    DebugPort = GetDebugPort ();
    if (DebugPort < Length) {
      // Inform FSP to skip debug UART init
      FspsConfig->SerialIoDebugUartNumber = DebugPort;
      FspsConfig->SerialIoUartMode[DebugPort] = 0x4;
    }

    // SPI
    FspsConfig->SerialIoSpiMode[1] = 0x1;
    CopyMem (&FspsConfig->SerialIoSpiCsPolarity, SiCfgData->SerialIoSpiCsPolarity, sizeof(SiCfgData->SerialIoSpiCsPolarity));

    // I2C
    Length = GetPchMaxSerialIoI2cControllersNum ();
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

    // Pcie Root Port
    MaxPcieRootPorts = GetPchMaxPciePortNum ();
    for (Index = 0; Index < MaxPcieRootPorts; Index++) {
      FspsConfig->PcieRpMaxPayload[Index]     = SiCfgData->PcieRpMaxPayload[Index];
      FspsConfig->PcieRpL1Substates[Index]    = SiCfgData->PcieRpL1Substates[Index];
      FspsConfig->PcieRpLtrEnable[Index]      = SiCfgData->PcieRpLtrEnable[Index];
      FspsConfig->PcieRpClkReqDetect[Index]   = SiCfgData->PcieRpClkReqDetect[Index];
      FspsConfig->PcieRpAdvancedErrorReporting[Index] = SiCfgData->PcieRpAdvancedErrorReporting[Index];
      FspsConfig->PcieRpAspm[Index]                   = SiCfgData->PcieRpAspm[Index];
      FspsConfig->PcieRpLtrMaxSnoopLatency[Index]     = 0x1003;
      FspsConfig->PcieRpLtrMaxNoSnoopLatency[Index]   = 0x1003;
      FspsConfig->PciePtm[Index]                      = SiCfgData->PciePtm[Index];
      FspsConfig->PcieRpPcieSpeed[Index]              = SiCfgData->PcieRpPcieSpeed[Index];
    }

    for (Index = 0; Index < 4; Index++) {
      FspsConfig->PtmEnabled[Index]  = SiCfgData->PtmEnabled[Index];
    }

    FspsConfig->IopFusaConfigEnable = 0x1;
    FspsConfig->EnableTimedGpio0 = SiCfgData->EnableTimedGpio0;
    FspsConfig->EnableTimedGpio1 = SiCfgData->EnableTimedGpio1;
    FspsConfig->PchPmSlpAMinAssert = SiCfgData->PchPmSlpAMinAssert;

    FspsConfig->EnableTcoTimer = 0x1;

    // UFS
    if (IsPchLp ()) {
      FspsConfig->UfsEnable[0] = SiCfgData->PchUfsEnable[0];
      FspsConfig->UfsEnable[1] = SiCfgData->PchUfsEnable[1];
    }

    // When fast boot is enabled, program SLP_A_MIN_ASST_WDTH to 0
    // to reduce boottime.
    if (PcdGetBool (PcdFastBootEnabled)) {
      FspsConfig->PchPmSlpAMinAssert = 1;
    }

    // TSN feature support
#if PLATFORM_RPLS || PLATFORM_RPLP || PLATFORM_RPLPCRB || PLATFORM_ADLP || PLATFORM_ADLPS
    FspsConfig->PchTsnEnable[0] = SiCfgData->PchTsnEnable;
    FspsConfig->PchTsnEnable[1] = SiCfgData->PchTsnEnable;
#else
    FspsConfig->PchTsnEnable = SiCfgData->PchTsnEnable;
#endif
    if(SiCfgData->PchTsnEnable == 1) {
      FspsConfig->PchTsnMultiVcEnable = SiCfgData->PchTsnMultiVcEnable;

      if (IsPchLp ()) {
        FspsConfig->PchTsnLinkSpeed = SiCfgData->PchTsnLinkSpeed;
      }

      TsnMacAddrBase      = NULL;
      TsnMacAddrSize      = 0;
      Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'M', 'A', 'C'),
                              (VOID **)&TsnMacAddrBase, &TsnMacAddrSize);
      if (!EFI_ERROR(Status)) {
          TsnSubRegion = (TSN_MAC_ADDR_SUB_REGION*) TsnMacAddrBase;

        FspsConfig->PchTsnMacAddressHigh  = TsnSubRegion->MacConfigData.Port[0].MacAddr.U32MacAddr[1];
        FspsConfig->PchTsnMacAddressLow   = TsnSubRegion->MacConfigData.Port[0].MacAddr.U32MacAddr[0];
        FspsConfig->PchTsn1MacAddressHigh = TsnSubRegion->MacConfigData.Port[1].MacAddr.U32MacAddr[1];
        FspsConfig->PchTsn1MacAddressLow  = TsnSubRegion->MacConfigData.Port[1].MacAddr.U32MacAddr[0];

        if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
          TsnCfgBlob.BlobBase = (UINT64)(UINTN)TsnMacAddrBase;
          TsnCfgBlob.BlobLength = TsnMacAddrSize;
          TpmHashAndExtendPcrEventLog (0, (UINT8 *)TsnMacAddrBase, TsnMacAddrSize, EV_PLATFORM_CONFIG_FLAGS, sizeof(TsnCfgBlob), (UINT8 *)&TsnCfgBlob);
        }

        DEBUG ((DEBUG_INFO, "TSN MAC subregion completed %r\n", Status));
      } else {
        DEBUG ((DEBUG_ERROR, "TSN MAC subregion not found! %r\n", Status));
      }
    }
    FspsConfig->IehMode               = SiCfgData->IehMode;
    FspsConfig->OpioRecenter          = SiCfgData->OpioRecenter;
    FspsConfig->D3HotEnable           = SiCfgData->D3HotEnable;
    FspsConfig->D3ColdEnable          = SiCfgData->D3ColdEnable;
    FspsConfig->TcCstateLimit         = SiCfgData->TcCstateLimit;
    FspsConfig->VccSt                 = SiCfgData->VccSt;
    // Cpu Pcie
    FspsConfig->CpuPciePowerGating[0] = SiCfgData->CpuPciePowerGating;
    FspsConfig->CpuPciePowerGating[1] = SiCfgData->CpuPciePowerGating;
    FspsConfig->CpuPciePowerGating[2] = SiCfgData->CpuPciePowerGating;
    FspsConfig->CpuPciePowerGating[3] = SiCfgData->CpuPciePowerGating;
#if !defined(PLATFORM_ADLN) && !defined(PLATFORM_ASL)
    FspsConfig->L2QosEnumerationEn = SiCfgData->L2QosEnumerationEn;
#endif
  }

  for (Index = 0; Index < 8; Index++) {
    FspsConfig->PcieRpSnoopLatencyOverrideMode[Index]    = 0x2;
    FspsConfig->PcieRpNonSnoopLatencyOverrideMode[Index] = 0x2;
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

  // Cpu Pcie
  FspsConfig->CpuPcieRpFunctionSwap = 0x0;
  for (Index = 0; Index < 3; Index++) {
    FspsConfig->CpuPcieRpMaxPayload[Index] = 0x1;
    FspsConfig->CpuPcieRpLtrEnable[Index]  = 0x1;
    FspsConfig->CpuPcieRpPmSci[Index]      = 0x1;
  }

  // PCH Flash protection
  FspsConfig->PchPwrOptEnable             = 0x1;

  // MISC
  FspsConfig->PchFivrExtV1p05RailVoltage = 0x1a4;
  FspsConfig->PchFivrExtVnnRailVoltage = 0x1a4;
  FspsConfig->PchFivrExtVnnRailSxVoltage = 0x1a4;
  FspsConfig->PchFivrVccinAuxLowToHighCurModeVolTranTime = 0xc;
  FspsConfig->PchFivrVccinAuxRetToHighCurModeVolTranTime = 0x36;
  FspsConfig->PchFivrVccinAuxRetToLowCurModeVolTranTime  = 0x2b;
  FspsConfig->PchFivrVccinAuxOffToHighCurModeVolTranTime = 0x96;
  FspsConfig->PchFivrDynPm = 0x1;
  FspsConfig->PchFivrExtV1p05RailIccMaximum = 0x1f4;
  FspsConfig->PchFivrExtVnnRailIccMaximum = 0x1f4;
  FspsConfig->PchFivrExtVnnRailSxIccMaximum = 0x1f4;
  FspsConfig->PchMemoryPmsyncEnable[0] = 0x1;
  FspsConfig->PchMemoryPmsyncEnable[1] = 0x1;
  FspsConfig->PchMemoryC0TransmitEnable[0] = 0x1;
  FspsConfig->PchMemoryC0TransmitEnable[1] = 0x1;
  FspsConfig->PchPmSlpS3MinAssert = 0x3;
  FspsConfig->PchPmSlpS4MinAssert = 0x1;
  FspsConfig->PchPmSlpSusMinAssert = 0x4;
  FspsConfig->PchPmPwrBtnOverridePeriod = 0x0;
  FspsConfig->PchEspiLgmrEnable = 0x0;
  FspsConfig->ForcMebxSyncUp = 0x0;
  FspsConfig->PmcModPhySusPgEnable = 0x0;
  FspsConfig->IomTypeCPortPadCfg[2] = 0x6040000;
  FspsConfig->IomTypeCPortPadCfg[3] = 0x6040011;
  FspsConfig->DmiTS0TW = 0x3;
  FspsConfig->DmiTS1TW = 0x2;
  FspsConfig->DmiTS2TW = 0x1;
  FspsConfig->MeUnconfigOnRtcClear = 0x1;
  FspsConfig->PsOnEnable = 0x1;
  FspsConfig->PmcAdrTimerEn = 0x1;
  FspsConfig->PmcAdrTimer1Val = 0x20;
  FspsConfig->PmcAdrMultiplier1Val = 0x6;
  FspsConfig->PmcAdrHostPartitionReset = 0x1;
  FspsConfig->PmcLpmS0ixSubStateEnableMask = 0x3;
  FspsConfig->PmcUsb2PhySusPgEnable = 0x0;
  FspsConfig->SkipCdClockInit = 0x0;
  FspsConfig->SerialIoSpiDelayRxClk[1] = 0x0;
  FspsConfig->SerialIoSpiDelayRxClk[2] = 0x0;
  FspsConfig->SerialIoSpiDelayRxClk[3] = 0x0;
  FspsConfig->VccInAuxImonIccImax = 0x0;
  FspsConfig->CpuPcieRpGen3Uptp[0] = 0x5;
  FspsConfig->CpuPcieRpGen4Uptp[0] = 0x8;
  FspsConfig->CpuPcieRpGen4Uptp[1] = 0x7;
  FspsConfig->CpuPcieRpGen4Uptp[2] = 0x7;
  FspsConfig->CpuPcieRpGen4Dptp[0] = 0x9;
  FspsConfig->CpuPcieRpGen4Dptp[1] = 0x5;
  FspsConfig->CpuPcieRpGen4Dptp[2] = 0x5;
  FspsConfig->CpuPcieRpGen5Uptp[0] = 0x5;
  FspsConfig->CpuPcieRpGen5Uptp[1] = 0x5;
  FspsConfig->CpuPcieRpGen5Uptp[2] = 0x5;

  // EndOfPost Upd
  FspsUpd->FspsConfig.EndOfPostMessage = 1;

  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.S0ix == 1) {
      FspsConfig->XdciEnable = 0;
      FspsConfig->EnableItbm = 0;
      FspsConfig->EnableTimedGpio0 = 0;
      FspsConfig->EnableTimedGpio1 = 0;
      FspsConfig->PchLanEnable = 0x0;
      ZeroMem (FspsConfig->CpuPcieRpMultiVcEnabled, sizeof (FspsConfig->CpuPcieRpMultiVcEnabled));
      ZeroMem (FspsConfig->PchIshI2cEnable, sizeof (FspsConfig->PchIshI2cEnable));
      ZeroMem (FspsConfig->PchIshGpEnable, sizeof (FspsConfig->PchIshGpEnable));
      DEBUG ((DEBUG_INFO, "Stage 2 S0ix config applied.\n"));
    }
  }

  for (Index = 0; Index < 3; Index++) {
    FspsConfig->CpuPcieRpLtrMaxSnoopLatency[Index] = 0x100f;
    FspsConfig->CpuPcieRpLtrMaxNoSnoopLatency[Index] = 0x100f;
  }

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
#if 0
    FspsUpd->FspsConfig.SiSkipBiosDoneWhenFwUpdate = TRUE;
#endif
    FspsUpd->FspsConfig.PchLockDownBiosInterface = FALSE;
    FspsUpd->FspsConfig.PchSbAccessUnlock        = TRUE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock = FALSE;
    DEBUG ((DEBUG_INFO, "Firmware update mode, unlock Bios setting\n"));
  } else {
    FspsUpd->FspsConfig.PchLockDownBiosInterface = TRUE;
    FspsUpd->FspsConfig.PchSbAccessUnlock        = FALSE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock = TRUE;
  }

  FspsConfig->AmtEnabled = 0x0;
  FspsConfig->VccInAuxImonSlope = 0x6f;

  PowerCfgData = (POWER_CFG_DATA *) FindConfigDataByTag (CDATA_POWER_TAG);
  if (PowerCfgData == NULL) {
    DEBUG ((DEBUG_ERROR, "Missing power Cfg Data!\n"));
  } else {
    //CPU Power Management Basic Config
    FspsConfig->Hwp                           = PowerCfgData->Hwp;
    FspsConfig->HdcControl                    = PowerCfgData->HdcControl;
    FspsConfig->EnableItbm                    = PowerCfgData->EnableItbm;
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
    FspsConfig->EnableEpbPeciOverride         = PowerCfgData->EnableEpbPeciOverride;
    FspsConfig->EnableFastMsrHwpReq           = PowerCfgData->EnableFastMsrHwpReq;
    FspsConfig->MinRingRatioLimit             = PowerCfgData->MinRingRatioLimit;
    FspsConfig->MaxRingRatioLimit             = PowerCfgData->MaxRingRatioLimit;

     //CPU VR Power Management Custom Config
    CopyMem (FspsConfig->VrConfigEnable,      PowerCfgData->VrConfigEnable,        sizeof(PowerCfgData->VrConfigEnable));
    CopyMem (FspsConfig->TdcCurrentLimit,     PowerCfgData->TdcCurrentLimit,       sizeof(PowerCfgData->TdcCurrentLimit));
    CopyMem (FspsConfig->DcLoadline,          PowerCfgData->DcLoadline,            sizeof(PowerCfgData->DcLoadline));
    CopyMem (FspsConfig->AcLoadline,          PowerCfgData->AcLoadline,            sizeof(PowerCfgData->AcLoadline));
    CopyMem (FspsConfig->Psi1Threshold,       PowerCfgData->Psi1Threshold,         sizeof(PowerCfgData->Psi1Threshold));
    CopyMem (FspsConfig->Psi2Threshold,       PowerCfgData->Psi2Threshold,         sizeof(PowerCfgData->Psi2Threshold));
    CopyMem (FspsConfig->Psi3Threshold,       PowerCfgData->Psi3Threshold,         sizeof(PowerCfgData->Psi3Threshold));
    CopyMem (FspsConfig->Psi3Enable,          PowerCfgData->Psi3Enable,            sizeof(PowerCfgData->Psi3Enable));
    CopyMem (FspsConfig->Psi4Enable,          PowerCfgData->Psi4Enable,            sizeof(PowerCfgData->Psi4Enable));
    CopyMem (FspsConfig->ImonSlope,           PowerCfgData->ImonSlope,             sizeof(PowerCfgData->ImonSlope));
    CopyMem (FspsConfig->IccMax,              PowerCfgData->IccMax,                sizeof(PowerCfgData->IccMax));
    CopyMem (FspsConfig->ImonOffset,          PowerCfgData->ImoniOffset,           sizeof(PowerCfgData->ImoniOffset));

    CopyMem (FspsConfig->VrVoltageLimit,      PowerCfgData->VrVoltageLimit,        sizeof(PowerCfgData->VrVoltageLimit));
    CopyMem (FspsConfig->TdcEnable,           PowerCfgData->TdcEnable,             sizeof(PowerCfgData->TdcEnable));
    CopyMem (FspsConfig->TdcTimeWindow,       PowerCfgData->TdcTimeWindow,         sizeof(PowerCfgData->TdcTimeWindow));

    FspsConfig->AcousticNoiseMitigation       = 0x0;
    FspsConfig->FivrSpreadSpectrum            = 0x8;

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
    FspsConfig->PsysPmax                      = PowerCfgData->PsysPmax;
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
    FspsConfig->CstateLatencyControl1TimeUnit = PowerCfgData->CstateLatencyControl1TimeUnit;
    FspsConfig->CstateLatencyControl2TimeUnit = PowerCfgData->CstateLatencyControl2TimeUnit;
    FspsConfig->CstateLatencyControl3TimeUnit = PowerCfgData->CstateLatencyControl3TimeUnit;
    FspsConfig->CstateLatencyControl4TimeUnit = PowerCfgData->CstateLatencyControl4TimeUnit;
    FspsConfig->CstateLatencyControl5TimeUnit = PowerCfgData->CstateLatencyControl5TimeUnit;
    FspsConfig->PkgCStateLimit                = PowerCfgData->PkgCStateLimit;
    FspsConfig->CstateLatencyControl1Irtl     = PowerCfgData->CstateLatencyControl1Irtl;
    FspsConfig->CstateLatencyControl2Irtl     = PowerCfgData->CstateLatencyControl2Irtl;
    FspsConfig->CstateLatencyControl3Irtl     = PowerCfgData->CstateLatencyControl3Irtl;
    FspsConfig->CstateLatencyControl4Irtl     = PowerCfgData->CstateLatencyControl4Irtl;
    FspsConfig->CstateLatencyControl5Irtl     = PowerCfgData->CstateLatencyControl5Irtl;

    // Cpu power related settings
    for (Index = 0; Index < TURBO_RATIO_LIMIT_ARRAY_SIZE; Index++) {
      FspsConfig->TurboRatioLimitRatio[Index] = PowerCfgData->TurboRatioLimitRatio[Index];
      FspsConfig->TurboRatioLimitNumCore[Index] = PowerCfgData->TurboRatioLimitNumCore[Index];
      FspsConfig->AtomTurboRatioLimitRatio[Index] = PowerCfgData->AtomTurboRatioLimitRatio[Index];
      FspsConfig->AtomTurboRatioLimitNumCore[Index] = PowerCfgData->AtomTurboRatioLimitNumCore[Index];
    }
  }

  FspsConfig->TdcTimeWindow[0] = 0x3e8;
  FspsConfig->TdcTimeWindow[1] = 0x3e8;
  FspsConfig->Device4Enable = 0x1;

  if (IsPchS ()) {
    FspsConfig->CpuPcieFunc0LinkDisable[2] = 0x0;
    FspsConfig->CseDataResilience = 0x1;
    FspsConfig->VerticalResolution = 0x0;
    FspsConfig->ThcIdleLtr[1] = 0xffffffff;
    FspsConfig->ThcHidResetPadTrigger[1] = 0x0;
    FspsConfig->ThcHidConnectionSpeed[1] = 0x1036640;
    FspsConfig->AtomTurboRatioLimitNumCore[0] = 0x0;
    FspsConfig->AtomTurboRatioLimitNumCore[1] = 0x0;
    FspsConfig->AtomTurboRatioLimitNumCore[2] = 0x0;
    FspsConfig->AtomTurboRatioLimitNumCore[3] = 0x0;
    FspsConfig->AtomTurboRatioLimitNumCore[4] = 0x0;
    FspsConfig->AtomTurboRatioLimitNumCore[5] = 0x0;
    FspsConfig->AtomTurboRatioLimitNumCore[6] = 0x0;
  }

  if (IsPchLp ()) {
    for (Index = 0; Index < 4; Index++) {
      FspsConfig->CpuPcieRpLtrMaxSnoopLatency[Index]   = 0x100f;
      FspsConfig->CpuPcieRpLtrMaxNoSnoopLatency[Index] = 0x100f;
    }
    FspsConfig->IopFusaConfigEnable = 0x0;
    FspsConfig->SataPortDevSlpPinMux[0] = 0x59673e0c;
    FspsConfig->SataPortDevSlpPinMux[1] = 0x5967400d;
    FspsConfig->SerialIoSpiDelayRxClk[1] = 0x0;
    FspsConfig->SerialIoSpiDelayRxClk[2] = 0x0;
    FspsConfig->SerialIoSpiDelayRxClk[3] = 0x0;
    FspsConfig->PchSerialIoI2cSdaPinMux[0] = 0x1947c404;
    FspsConfig->PchSerialIoI2cSdaPinMux[1] = 0x1947c606;
    FspsConfig->PchSerialIoI2cSdaPinMux[4] = 0;
    FspsConfig->PchSerialIoI2cSdaPinMux[7] = 0x1947d20c;
    FspsConfig->PchSerialIoI2cSclPinMux[0] = 0x1947a405;
    FspsConfig->PchSerialIoI2cSclPinMux[1] = 0x1947a607;
    FspsConfig->PchSerialIoI2cSclPinMux[4] = 0;
    FspsConfig->PchSerialIoI2cSclPinMux[7] = 0x1947b20d;
    FspsConfig->IshGpGpioPinMuxing[4] = 0x4900a803;
    FspsConfig->IshGpGpioPinMuxing[5] = 0x4900aa04;
    FspsConfig->IshGpGpioPinMuxing[6] = 0x4907ac0c;
    FspsConfig->IshGpGpioPinMuxing[7] = 0x5900ae0f;
    FspsConfig->PchFivrExtV1p05RailSupportedVoltageStates = 0x2;
    FspsConfig->PchFivrExtV1p05RailVoltage = 0x0;
    FspsConfig->PchFivrExtVnnRailVoltage = 0x0;
    FspsConfig->PchFivrExtVnnRailSxVoltage = 0x0;
    FspsConfig->PchFivrVccinAuxLowToHighCurModeVolTranTime = 0xc;
    FspsConfig->PchFivrVccinAuxRetToHighCurModeVolTranTime = 0x36;
    FspsConfig->PchFivrVccinAuxRetToLowCurModeVolTranTime = 0x2b;
    FspsConfig->PchFivrVccinAuxOffToHighCurModeVolTranTime = 0x96;
    FspsConfig->PchFivrExtVnnRailSxIccMaximum = 0x0;
    FspsConfig->CnviRfResetPinMux = 0x194ce404;
    FspsConfig->CnviClkreqPinMux = 0x294ce605;
    FspsConfig->PmcUsb2PhySusPgEnable = 0x1;
    FspsConfig->PmcModPhySusPgEnable = 0x1;
    FspsConfig->IomTypeCPortPadCfg[2] = 0x0;
    FspsConfig->IomTypeCPortPadCfg[3] = 0x0;
    FspsConfig->UsbTcPortEn = 0xF;
    FspsConfig->ITbtPcieTunnelingForUsb4 = 0x1;
    FspsConfig->ITbtPcieRootPortEn[0] = 0x1;
    FspsConfig->ITbtPcieRootPortEn[1] = 0x1;
    FspsConfig->ITbtPcieRootPortEn[2] = 0x1;
    FspsConfig->ITbtPcieRootPortEn[3] = 0x1;
    FspsConfig->SaPcieItbtRpSnoopLatencyOverrideValue[2] = 0xc8;
    FspsConfig->TdcTimeWindow[0] = 0x3e8;
    FspsConfig->TdcTimeWindow[1] = 0x3e8;
    FspsConfig->IehMode = 0x0;
    FspsConfig->PortResetMessageEnable[0] = 0x1;
    FspsConfig->PortResetMessageEnable[1] = 0x1;
    FspsConfig->PortResetMessageEnable[2] = 0x1;
    FspsConfig->PortResetMessageEnable[4] = 0x1;
    FspsConfig->PkgCStateLimit = 0xff;
    FspsConfig->CstCfgCtrIoMwaitRedirection = 0;
    FspsConfig->PmcLpmS0ixSubStateEnableMask = 0x9;
    FspsConfig->VbtSize           = 0x2200;
    FspsConfig->CpuPcieRpGen3Uptp[2] = 0x5;
    FspsConfig->CpuPcieRpGen4Uptp[2] = 0x8;
    FspsConfig->CpuPcieRpGen4Dptp[2] = 0x9;
    FspsConfig->PchPmPwrBtnOverridePeriod = 0x3;
    FspsConfig->PsOnEnable = 0x0;
    FspsConfig->IomStayInTCColdSeconds = 0x0;
    FspsConfig->IomBeforeEnteringTCColdSeconds = 0x0;
    FspsConfig->Irms[0] = 0x1;
    FspsConfig->Irms[1] = 0x1;
    FspsConfig->AtomTurboRatioLimitNumCore[1] = 0x2;
    FspsConfig->AtomTurboRatioLimitNumCore[2] = 0x3;
    FspsConfig->AtomTurboRatioLimitNumCore[3] = 0x4;
    FspsConfig->AtomTurboRatioLimitNumCore[4] = 0x5;
    FspsConfig->AtomTurboRatioLimitNumCore[5] = 0x6;
    FspsConfig->AtomTurboRatioLimitNumCore[6] = 0x7;
    FspsConfig->AtomTurboRatioLimitNumCore[7] = 0x8;

    switch (GetPlatformId ()) {
      case PLATFORM_ID_ADL_P_DDR5_RVP:
        FspsConfig->Usb4CmMode = 0x0;
        break;
      case PLATFORM_ID_ADL_N_DDR5_CRB:
        FspsConfig->PchLanEnable = 0x0;
        FspsConfig->PchPmVrAlert = 0x1;
        FspsConfig->PchFivrExtV1p05RailEnabledStates = 0x1f;
        FspsConfig->PchFivrExtVnnRailEnabledStates = 0x3f;
        FspsConfig->PchFivrExtVnnRailSupportedVoltageStates = 0x4;
        FspsConfig->PchFivrVccinAuxOffToHighCurModeVolTranTime = 0x96;
        FspsConfig->IomStayInTCColdSeconds = 0x0;
        FspsConfig->IomBeforeEnteringTCColdSeconds = 0x0;
        FspsConfig->UsbTcPortEn = 0x3;
        FspsConfig->CpuPcieRpPmSci[0] = 0x0;
        FspsConfig->CpuPcieRpPmSci[1] = 0x0;
        FspsConfig->CpuPcieRpPmSci[2] = 0x0;
        FspsConfig->CpuPcieRpMaxPayload[0] = 0x0;
        FspsConfig->CpuPcieRpMaxPayload[1] = 0x0;
        FspsConfig->CpuPcieRpMaxPayload[2] = 0x0;
        FspsConfig->CpuPcieRpLtrEnable[0] = 0x0;
        FspsConfig->CpuPcieRpLtrEnable[1] = 0x0;
        FspsConfig->CpuPcieRpLtrEnable[2] = 0x0;
        FspsConfig->CpuPcieRpGen3Uptp[0] = 0x7;
        FspsConfig->CpuPcieRpGen3Uptp[2] = 0x7;
        FspsConfig->CpuPcieRpGen4Uptp[1] = 0x8;
        FspsConfig->CpuPcieRpGen4Dptp[1] = 0x9;
        FspsConfig->CpuPcieRpGen5Uptp[0] = 0x7;
        FspsConfig->CpuPcieRpGen5Uptp[1] = 0x7;
        FspsConfig->CpuPcieRpGen5Uptp[2] = 0x7;
        FspsConfig->Usb4CmMode = 0x0;
        FspsConfig->EnergyEfficientTurbo = 0x1;
        FspsConfig->PsysPmax = 0x0;
        FspsConfig->CnviBtAudioOffload = 0x1;
        FspsConfig->IomTypeCPortPadCfg[0] = 0x90e0016;
        FspsConfig->IomTypeCPortPadCfg[1] = 0x90e0017;
        FspsConfig->VmdEnable = 0;
        FspsConfig->Irms[0] = 0x0;
        FspsConfig->Irms[1] = 0x0;
        FspsConfig->ThcMode[0] = 1;
        FspsConfig->ThcMode[1] = 1;
        FspsConfig->PortResetMessageEnable[1] = 0x0;
        FspsConfig->PortResetMessageEnable[2] = 0x0;
        FspsConfig->PortResetMessageEnable[4] = 0x0;
        FspsConfig->AmtEnabled = 0x0;
        FspsConfig->LidStatus = 0x5b;
        FspsConfig->TcssAuxOri = 0x1;
        FspsConfig->Device4Enable = 0x0; //this controls the thermal device (B0,D4,F0)
#ifdef PLATFORM_ADLN
        FspsConfig->PchFivrVccstIccMaxControl = 0x1;
#endif
        CopyMem (FspsConfig->AtomTurboRatioLimitNumCore, PowerCfgData->AtomTurboRatioLimitNumCore, sizeof(PowerCfgData->AtomTurboRatioLimitNumCore));
        break;
      case PLATFORM_ID_ADL_N_LPDDR5_RVP:
        FspsConfig->PchLanEnable = 0x0;
        FspsConfig->PchPmVrAlert = 0x1;
        FspsConfig->CpuUsb3OverCurrentPin[1] = 0x5;
        FspsConfig->IomStayInTCColdSeconds = 0x0;
        FspsConfig->IomBeforeEnteringTCColdSeconds = 0x0;
        FspsConfig->UsbTcPortEn = 0x3;
        FspsConfig->TdcTimeWindow[1] = 0x3e8;
        FspsConfig->PchPmSlpS3MinAssert = 0x0;
        FspsConfig->PchPmSlpSusMinAssert = 0x0;
        FspsConfig->PortResetMessageEnable[2] = 0x0;
        FspsConfig->PortResetMessageEnable[4] = 0x0;
        FspsConfig->MeUnconfigOnRtcClear = 0x2;
        FspsConfig->CpuPcieRpPmSci[0] = 0x0;
        FspsConfig->CpuPcieRpPmSci[1] = 0x0;
        FspsConfig->CpuPcieRpPmSci[2] = 0x0;
        FspsConfig->CpuPcieRpMaxPayload[0] = 0x0;
        FspsConfig->CpuPcieRpMaxPayload[1] = 0x0;
        FspsConfig->CpuPcieRpMaxPayload[2] = 0x0;
        FspsConfig->CpuPcieRpLtrEnable[0] = 0x0;
        FspsConfig->CpuPcieRpLtrEnable[1] = 0x0;
        FspsConfig->CpuPcieRpLtrEnable[2] = 0x0;
        FspsConfig->Usb4CmMode = 0x0;
        FspsConfig->EnergyEfficientTurbo = 0x1;
        FspsConfig->PkgCStateLimit = 0x6;
        FspsConfig->PsysPmax = 0x0;
        FspsConfig->PchUnlockGpioPads = 0x1;

        for (Index = 0; Index < 4; Index++) {
          FspsConfig->CpuPcieRpLtrMaxSnoopLatency[Index]   = 0x1003;
          FspsConfig->CpuPcieRpLtrMaxNoSnoopLatency[Index] = 0x1003;
        }

        FspsConfig->CpuPcieRpGen3Uptp[0] = 0x7;
        FspsConfig->CpuPcieRpGen3Uptp[2] = 0x7;
        FspsConfig->CpuPcieRpGen4Uptp[1] = 0x8;
        FspsConfig->CpuPcieRpGen4Dptp[1] = 0x9;
        FspsConfig->CpuPcieRpGen5Uptp[0] = 0x7;
        FspsConfig->CpuPcieRpGen5Uptp[1] = 0x7;
        FspsConfig->CpuPcieRpGen5Uptp[2] = 0x7;
        break;
      case PLATFORM_ID_ADL_PS_DDR5_RVP:
        FspsConfig->AmtEnabled = 0x0;
        FspsConfig->FwProgress = 0x0;
        FspsConfig->Usb4CmMode = 0x0;
        FspsConfig->CnviBtAudioOffload = 0x1;
        FspsConfig->VccInAuxImonSlope = 0x64;
        break;
      case PLATFORM_ID_ADL_PS_DDR5_CRB:
        FspsConfig->AmtEnabled = 0x0;
        FspsConfig->Usb4CmMode = 0x0;
        FspsConfig->Device4Enable = 0x0;
        FspsConfig->CnviBtAudioOffload = 0x1;
        FspsConfig->VccInAuxImonSlope = 0x64;
        break;
      default:
        break;
    }
  }

#if FixedPcdGet8 (PcdTccEnabled)
  Status = TccModePostMemConfig (FspsUpd);
#endif

  if (FeaturePcdGet (PcdEnablePciePm)) {
    StoreRpConfig (FspsConfig);
  }
  Status = FusaConfigPostMem(FspsUpdPtr);
  DEBUG((DEBUG_INFO, "FusaConfigPostMem Status %r\n", Status));
}
