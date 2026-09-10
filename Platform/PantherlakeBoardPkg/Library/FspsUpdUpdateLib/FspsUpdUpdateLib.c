/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
#include <CpuRegs.h>
#include <Library/PchInfoLib.h>
#include <PlatformBase.h>
#include <Library/IoLib.h>
#include <Library/SpiFlashLib.h>
#include <Register/PchRegsPmc.h>
#include <Library/IgdOpRegionLib.h>
#include <Library/WatchDogTimerLib.h>
#include <PlatformData.h>
#include <Library/ContainerLib.h>
#include <Library/PciePm.h>
#include <Library/FusaConfigLib.h>

#define MAX_TCSS_USB3_PORTS                4

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

UINT8
PtlPcdGetUsb2MaxPhysicalPortNum (
  VOID
  );

UINT8
PtlPcdGetMaxPcieControllerNum (
  VOID
  );

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
//
// mPcdPDevIntConfig table contains data pertaining to the INTx->PIRQy mapping for devices that exist on PCD-P
//
GLOBAL_REMOVE_IF_UNREFERENCED SI_PCH_DEVICE_INTERRUPT_CONFIG mPtlPcdPDevIntConfig[] = {
  // {31, 0, PchNoInt, 0},  // LPC/eSPI Interface
  // {31, 1, PchNoInt, 0},  // P2SB
  // {31, 2, PchNoInt, 0},  // PMC
  { 31, 3, PchIntB, 17}, // ACE
  { 31, 4, PchIntC, 18}, // SMBus Controller
  // {31, 5, PchNoInt, 0},  // SPI
  { 31, 6, PchIntD, 19}, // GbE Controller
  { 31, 7, PchIntA, 16}, // TraceHub
  { 30, 0, PchIntA, 16}, // SerialIo: UART #0
  { 30, 1, PchIntB, 17}, // SerialIo: UART #1
  { 30, 2, PchIntC, 18}, // SerialIo: SPI #0
  { 30, 3, PchIntD, 19}, // SerialIo: SPI #1
  // {30, 6, PchNoInt, 0},  // Reserved for HPET
  // {30, 7, PchNoInt, 0},  // Reserved for IOAPIC
  { 28, 0, PchIntA, 16}, // PCI Express Port 1
  { 28, 1, PchIntB, 17}, // PCI Express Port 2
  { 28, 2, PchIntC, 18}, // PCI Express Port 3
  { 28, 3, PchIntD, 19}, // PCI Express Port 4
  { 28, 4, PchIntA, 16}, // PCI Express Port 5
  { 28, 5, PchIntB, 17}, // PCI Express Port 6
  { 28, 6, PchIntC, 18}, // PCI Express Port 7
  { 28, 7, PchIntD, 19}, // PCI Express Port 8
  { 25, 0, PchIntA, 20}, // SerialIo I2C Controller #4
  { 25, 1, PchIntB, 21}, // SerialIo I2C Controller #5
  { 25, 2, PchIntC, 24}, // SerialIo UART #2
  // {24, 0, PchNoInt, 0},  // SSE: HECI #1
  // {24, 1, PchNoInt, 0},  // SSE: HECI #2
  // {24, 2, PchNoInt, 0},  // SSE: HECI #3
  { 23, 0, PchIntA, 25}, // UFS
  { 22, 0, PchIntA, 16}, // CSME: HECI #1
  { 22, 1, PchIntB, 17}, // CSME: HECI #2
  { 22, 2, PchIntC, 18}, // CSME: IDE-Redirection (IDE-R)
  { 22, 3, PchIntD, 19}, // CSME: Keyboard and Text (KT) Redirection
  { 22, 4, PchIntA, 16}, // CSME: HECI #3
  { 22, 5, PchIntB, 17}, // CSME: HECI #4
  // {22, 7, PchNoInt, 0},  // CSME: WLAN
  { 21, 0, PchIntA, 27}, // SerialIo I2C Controller #0
  { 21, 1, PchIntB, 28}, // SerialIo I2C Controller #1
  { 21, 2, PchIntC, 29}, // SerialIo I2C Controller #2
  { 21, 3, PchIntD, 30}, // SerialIo I2C Controller #3
  { 20, 0, PchIntA, 16}, // USB 3.0 xHCI Controller
  { 20, 1, PchIntB, 17}, // USB Device Controller (OTG)
  // {20, 2, PchNoInt, 0},  // Shared SRAM
  { 20, 3, PchIntC, 18}, // CNVi WiFi
  // {20, 4, PchNoInt, 0},  // TraceHub Phantom (ACPI) Function
  // {20, 5, PchNoInt, 0},  // IEH 8-bit
  // {20, 6, PchNoInt, 0},  // DBC.DBC Phantom (ACPI) Function
  // {20, 7, PchNoInt, 0}, // CNVi Bluetooth
  // {19, 0, PchNoInt, 0},  // PSE: HECI #1
  // {19, 1, PchNoInt, 0},  // PSE: HECI #1
  // {19, 2, PchNoInt, 0},  // PSE: HECI #1
  { 18, 0, PchIntA, 26}, // Integrated Sensor Hub
  // {18, 1, PchNoInt, 0},  // P2SB 16-bit
  // {18, 2, PchNoInt, 0},  // CSME: PMT Phantom (ACPI) function
  // {18, 3, PchNoInt, 0},  // IEH 16-bit
  // {18, 4, PchNoInt, 0},  // CSME: fTPM DMA Phantom (ACPI) function
  // {18, 6, PchNoInt, 0},  // SerialIo: SPI #2 - disabled
  { 17, 0, PchIntA, 31}, // SerialIo I3C Controller #1
  { 17, 1, PchIntB, 32}, // SerialIo I3C Controller #2
  // {17, 2, PchNoInt, 0},  // TraceHub Phantom (ACPI) Function
  { 16, 0, PchIntA, 23}, // THC #0
  { 16, 1, PchIntB, 22}, // THC #1
  //{ 13, 0, PchNoInt, 0}, // TCSS xHCI Controller
  //{ 13, 1, PchNoInt, 0}, // TCSS XDCI
  //{ 13, 2, PchNoInt, 0}, // TCSS TBT DMA0
  //{ 13, 3, PchNoInt, 0}, // TCSS TBT DMA1
  // {13, 7, PchNoInt, 0},  // TCSS XHCI VTIO Phantom (ACPI) Function
  // {  7, 0, PchNoInt, 0}, // TCSS PCI Express Port 21
  // {  7, 1, PchNoInt, 0}, // TCSS PCI Express Port 22
  // {  7, 2, PchNoInt, 0}, // TCSS PCI Express Port 23
  // {  7, 3, PchNoInt, 0}, // TCSS PCI Express Port 24
  {  6, 0, PchIntA, 16}, // PCI Express Root Port #9
  {  6, 1, PchIntB, 17}, // PCI Express Root Port #10
};

  GLOBAL_REMOVE_IF_UNREFERENCED SI_PCH_DEVICE_INTERRUPT_CONFIG mPtlPcdHDevIntConfig [] = {
  // {31, 0, PchNoInt, 0},  // LPC/eSPI Interface
  // {31, 1, PchNoInt, 0},  // P2SB
  // {31, 2, PchNoInt, 0},  // PMC
  { 31, 3, PchIntB, 17}, // ACE
  { 31, 4, PchIntC, 18}, // SMBus Controller
  // {31, 5, PchNoInt, 0},  // SPI
  { 31, 6, PchIntD, 19}, // GbE Controller
  { 31, 7, PchIntA, 16}, // TraceHub
  { 30, 0, PchIntA, 16}, // SerialIo: UART #0
  { 30, 1, PchIntB, 17}, // SerialIo: UART #1
  { 30, 2, PchIntC, 18}, // SerialIo: SPI #0
  { 30, 3, PchIntD, 19}, // SerialIo: SPI #1
  // {30, 6, PchNoInt, 0},  // Reserved for HPET
  // {30, 7, PchNoInt, 0},  // Reserved for IOAPIC
  { 28, 0, PchIntA, 16}, // PCI Express Port 1
  { 28, 1, PchIntB, 17}, // PCI Express Port 2
  { 28, 2, PchIntC, 18}, // PCI Express Port 3
  { 28, 3, PchIntD, 19}, // PCI Express Port 4
  { 28, 4, PchIntA, 16}, // PCI Express Port 5
  { 28, 5, PchIntB, 17}, // PCI Express Port 6
  { 28, 6, PchIntC, 18}, // PCI Express Port 7
  { 28, 7, PchIntD, 19}, // PCI Express Port 8
  { 25, 0, PchIntA, 20}, // SerialIo I2C Controller #4
  { 25, 1, PchIntB, 21}, // SerialIo I2C Controller #5
  { 25, 2, PchIntC, 24}, // SerialIo UART #2
  // {24, 0, PchNoInt, 0},  // SSE: HECI #1
  // {24, 1, PchNoInt, 0},  // SSE: HECI #2
  // {24, 2, PchNoInt, 0},  // SSE: HECI #3
  { 22, 0, PchIntA, 16}, // CSME: HECI #1
  { 22, 1, PchIntB, 17}, // CSME: HECI #2
  { 22, 2, PchIntC, 18}, // CSME: IDE-Redirection (IDE-R)
  { 22, 3, PchIntD, 19}, // CSME: Keyboard and Text (KT) Redirection
  { 22, 4, PchIntA, 16}, // CSME: HECI #3
  { 22, 5, PchIntB, 17}, // CSME: HECI #4
  // {22, 7, PchNoInt, 0},  // CSME: WLAN
  { 21, 0, PchIntA, 27}, // SerialIo I2C Controller #0
  { 21, 1, PchIntB, 28}, // SerialIo I2C Controller #1
  { 21, 2, PchIntC, 29}, // SerialIo I2C Controller #2
  { 21, 3, PchIntD, 30}, // SerialIo I2C Controller #3
  { 20, 0, PchIntA, 16}, // USB 3.0 xHCI Controller
  { 20, 1, PchIntB, 17}, // USB Device Controller (OTG)
  // {20, 2, PchNoInt, 0},  // Shared SRAM
  { 20, 3, PchIntC, 18}, // CNVi WiFi
  // {20, 4, PchNoInt, 0},  // TraceHub Phantom (ACPI) Function
  // {20, 5, PchNoInt, 0},  // IEH 8-bit
  // {20, 6, PchNoInt, 0},  // DBC.DBC Phantom (ACPI) Function
  // {20, 7, PchNoInt, 0}, // CNVi Bluetooth
  // {19, 0, PchNoInt, 0},  // PSE: HECI #1
  // {19, 1, PchNoInt, 0},  // PSE: HECI #1
  // {19, 2, PchNoInt, 0},  // PSE: HECI #1
  { 18, 0, PchIntA, 26}, // Integrated Sensor Hub
  // {18, 1, PchNoInt, 0},  // P2SB 16-bit
  // {18, 2, PchNoInt, 0},  // CSME: PMT Phantom (ACPI) function
  // {18, 3, PchNoInt, 0},  // IEH 16-bit
  // {18, 4, PchNoInt, 0},  // CSME: fTPM DMA Phantom (ACPI) function
  // {18, 6, PchNoInt, 0},  // SerialIo: SPI #2 - disabled
  { 17, 0, PchIntA, 31}, // SerialIo I3C Controller #1
  { 17, 1, PchIntB, 32}, // SerialIo I3C Controller #2
  // {17, 2, PchNoInt, 0},  // TraceHub Phantom (ACPI) Function
  // {17, 3, PchNoInt, 0},  // LPSS Reserved #1
  // {17, 4, PchNoInt, 0},  // LPSS Reserved #2
  { 16, 0, PchIntA, 23}, // THC #0
  { 16, 1, PchIntB, 22}, // THC #1
  //{ 13, 0, PchNoInt, 0}, // TCSS xHCI Controller
  //{ 13, 1, PchNoInt, 0}, // TCSS XDCI
  //{ 13, 2, PchNoInt, 0}, // TCSS TBT DMA0
  //{ 13, 3, PchNoInt, 0}, // TCSS TBT DMA1
  // {13, 7, PchNoInt, 0},  // TCSS XHCI VTIO Phantom (ACPI) Function
  // {  7, 0, PchNoInt, 0}, // TCSS PCI Express Port 21
  // {  7, 1, PchNoInt, 0}, // TCSS PCI Express Port 22
  // {  7, 2, PchNoInt, 0}, // TCSS PCI Express Port 23
  // {  7, 3, PchNoInt, 0}, // TCSS PCI Express Port 24
  {  6, 0, PchIntA, 16}, // PCI Express Root Port #9
  {  6, 1, PchIntB, 17}, // PCI Express Root Port #10
  {  6, 2, PchIntC, 18}, // PCI Express Root Port #11
  {  6, 3, PchIntD, 19}, // PCI Express Root Port #12
};

GLOBAL_REMOVE_IF_UNREFERENCED SI_PCH_DEVICE_INTERRUPT_CONFIG mWclPcdDevIntConfig [] = {
  // {31, 0, PchNoInt, 0},  // LPC/eSPI Interface
  // {31, 1, PchNoInt, 0},  // P2SB
  // {31, 2, PchNoInt, 0},  // PMC
  { 31, 3, PchIntB, 17}, // ACE
  { 31, 4, PchIntC, 18}, // SMBus Controller
  // {31, 5, PchNoInt, 0},  // SPI
  { 31, 6, PchIntD, 19}, // GbE Controller
  { 31, 7, PchIntA, 16}, // TraceHub
  { 30, 0, PchIntA, 16}, // SerialIo: UART #0
  { 30, 1, PchIntB, 17}, // SerialIo: UART #1
  { 30, 2, PchIntC, 18}, // SerialIo: SPI #0
  { 30, 3, PchIntD, 19}, // SerialIo: SPI #1
  // {30, 6, PchNoInt, 0},  // Reserved for HPET
  // {30, 7, PchNoInt, 0},  // Reserved for IOAPIC
  { 28, 0, PchIntA, 16}, // PCI Express Port 1
  { 28, 1, PchIntB, 17}, // PCI Express Port 2
  { 28, 2, PchIntC, 18}, // PCI Express Port 3
  { 28, 3, PchIntD, 19}, // PCI Express Port 4
  { 25, 0, PchIntA, 20}, // SerialIo I2C Controller #4
  { 25, 1, PchIntB, 21}, // SerialIo I2C Controller #5
  { 25, 2, PchIntC, 24}, // SerialIo UART #2
  // {24, 0, PchNoInt, 0},  // SSE: HECI #1
  // {24, 1, PchNoInt, 0},  // SSE: HECI #2
  // {24, 2, PchNoInt, 0},  // SSE: HECI #3
  { 23, 0, PchIntA, 25}, // UFS
  { 22, 0, PchIntA, 16}, // CSME: HECI #1
  { 22, 1, PchIntB, 17}, // CSME: HECI #2
  { 22, 2, PchIntC, 18}, // CSME: IDE-Redirection (IDE-R)
  { 22, 3, PchIntD, 19}, // CSME: Keyboard and Text (KT) Redirection
  { 22, 4, PchIntA, 16}, // CSME: HECI #3
  { 22, 5, PchIntB, 17}, // CSME: HECI #4
  // {22, 7, PchNoInt, 0},  // CSME: WLAN
  { 21, 0, PchIntA, 27}, // SerialIo I2C Controller #0
  { 21, 1, PchIntB, 28}, // SerialIo I2C Controller #1
  { 21, 2, PchIntC, 33}, // SerialIo I2C Controller #2
  { 21, 3, PchIntD, 30}, // SerialIo I2C Controller #3
  // {21, 7, PchNoInt, 0},  // Northpeak Phantom (ACPI) Function
  { 20, 0, PchIntA, 16}, // USB 3.0 xHCI Controller
  { 20, 1, PchIntB, 17}, // USB Device Controller (OTG)
  // {20, 2, PchNoInt, 0},  // Shared SRAM
  { 20, 3, PchIntC, 18}, // CNVi WiFi
  // {20, 4, PchNoInt, 0},  // TraceHub Phantom (ACPI) Function
  // {20, 5, PchNoInt, 0},  // IEH 8-bit
  // {20, 6, PchNoInt, 0},  // DBC.DBC Phantom (ACPI) Function
  // {20, 7, PchNoInt, 0}, // CNVi Bluetooth
  // {19, 0, PchNoInt, 0},  // PSE: HECI #1
  // {19, 1, PchNoInt, 0},  // PSE: HECI #1
  // {19, 2, PchNoInt, 0},  // PSE: HECI #1
  { 18, 0, PchIntA, 26}, // Integrated Sensor Hub
  // {18, 1, PchNoInt, 0},  // P2SB 16-bit
  // {18, 2, PchNoInt, 0},  // CSME: PMT Phantom (ACPI) function
  // {18, 3, PchNoInt, 0},  // IEH 16-bit
  // {18, 4, PchNoInt, 0},  // CSME: fTPM DMA Phantom (ACPI) function
  // {18, 5, PchNoInt, 0},  // AON Vision (AVB)
  // {18, 6, PchNoInt, 0},  // SerialIo: SPI #2 - disabled
  { 17, 0, PchIntA, 31}, // SerialIo I3C Controller #1
  { 17, 1, PchIntB, 32}, // SerialIo I3C Controller #2
  // {17, 2, PchNoInt, 0},  // TraceHub Phantom (ACPI) Function
  { 16, 0, PchIntA, 23}, // THC #0
  { 16, 1, PchIntB, 22}, // THC #1
  { 13, 0, PchIntA, 16}, // TCSS xHCI Controller
  { 13, 2, PchIntA, 16}, // TCSS TBT DMA0
  // {13, 7, PchNoInt, 0},  // TCSS XHCI VTIO Phantom (ACPI) Function
  {  7, 0, PchIntA, 16}, // TypeC Subsystem PCIe Root Port #21
  {  7, 1, PchIntB, 17}, // TypeC Subsystem PCIe Root Port #22
  {  6, 0, PchIntA, 16}, // PCI Express Root Port #9
  {  6, 1, PchIntB, 17}, // PCI Express Root Port #10
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
  UINT8                                      Index;
  UINT8                                      MaxPchPcieRootPorts;
  TCC_CFG_DATA                               *TccCfgData;

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
  if ((TccCfgData == NULL) || (TccCfgData->TccEnable == 0)) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "Tcc is enabled, setting Tcc Silicon Config\n"));

  // DEPRECATED in FSP and has no effect. Leave it enabled to avoid
  // confusion for now. Consider removing.
  FspsUpd->FspsConfig.TccMode                   = 1;            // TCC Mode

  // TCC related Silicon settings
  FspsUpd->FspsConfig.Eist                      = 1;            // Intel Speed Step->EnableGv
  FspsUpd->FspsConfig.EnableRp                  = 0;            // Disable RP in TCC mode

  MaxPchPcieRootPorts = PtlPcdGetMaxPcieControllerNum ();
  for (Index = 0; Index < MaxPchPcieRootPorts; Index++) {
    FspsUpd->FspsConfig.PciePtm[Index]              = 1;
  }

  return EFI_SUCCESS;
}
#endif

/**
  Update FSP-S UPD config data.

  @param  FspsUpdPtr    The pointer to the FSP-S UPD to be updated.

**/
VOID
EFIAPI
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
  UINT8                       Index;
  UINT8                       PrIndex;
  UINT32                      SmiEn;
  UINT32                      BaseAddress;
  UINT32                      TotalSize;
  UINT32                      Address;
  UINT32                      VarSize;
  BOOLEAN                     BiosProtected;
  EFI_STATUS                  Status;
  VBIOS_VBT_STRUCTURE         *VbtPtr;

  DEBUG((DEBUG_INFO, "FSP-S UpdateFspConfig\n"));

  Address              = 0;
  FspsUpd              = (FSPS_UPD *) FspsUpdPtr;
  FspsConfig           = &FspsUpd->FspsConfig;
  FspsUpd->FspUpdHeader.Signature = FSPS_UPD_SIGNATURE;
  FspsUpd->FspUpdHeader.Revision  = 0x2;

  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);

  //
  // Update device interrupt table
  //
  //
   if (PtlIsPcdP ()) {
    DEBUG ((DEBUG_INFO, "Updated PTL PCD P Interrupt table\n"));
    FspsConfig->DevIntConfigPtr   = (UINT32)(UINTN)mPtlPcdPDevIntConfig;
    FspsConfig->NumOfDevIntConfig = sizeof (mPtlPcdPDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);
  } else if (PtlIsPcdH()) {
    DEBUG ((DEBUG_INFO, "Updated PTL PCD H Interrupt table\n"));
    FspsConfig->DevIntConfigPtr   = (UINT32)(UINTN)mPtlPcdHDevIntConfig;
    FspsConfig->NumOfDevIntConfig = sizeof (mPtlPcdHDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);
  } else if (IsWclPcd()) {
    DEBUG ((DEBUG_INFO, "Updated WCL PCD Interrupt table\n"));
    FspsConfig->DevIntConfigPtr   = (UINT32)(UINTN)mWclPcdDevIntConfig;
    FspsConfig->NumOfDevIntConfig = sizeof (mWclPcdDevIntConfig) / sizeof (SI_PCH_DEVICE_INTERRUPT_CONFIG);
  } else {
    DEBUG ((DEBUG_ERROR, "Unsupported PCD.\n"));
  }

  if (!PcdGetBool(PcdEnableSetup) && (GetBootMode () != BOOT_ON_FLASH_UPDATE)) {
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
  // Disable flash write firstly, so that UEFI payload variable could work
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
      // Reject a size whose 512-byte round-up would overflow when added below.
      if ((VbtPtr->HeaderVbtSize) > (UINT16)(MAX_UINT16 - 0x200)) {
        DEBUG ((DEBUG_ERROR, "VBT HeaderVbtSize invalid, skip VbtSize update\n"));
      } else {
        FspsConfig->VbtSize = ((VbtPtr->HeaderVbtSize) & (UINT32)~(0x1FF)) + 0x200;
      }
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
    FspsUpd->FspsConfig.PchLegacyIoLowLatency     = SiCfgData->PchLegacyIoLowLatency;
    FspsUpd->FspsConfig.RenderStandby             = SiCfgData->RenderStandby; // IGFX RenderStandby

    // USB
    CopyMem (&FspsConfig->PortUsb20Enable, SiCfgData->PortUsb20Enable, sizeof(SiCfgData->PortUsb20Enable));
    CopyMem (&FspsConfig->Usb2OverCurrentPin, SiCfgData->Usb2OverCurrentPin, sizeof(SiCfgData->Usb2OverCurrentPin));
    CopyMem (&FspsConfig->Usb3OverCurrentPin, SiCfgData->Usb3OverCurrentPin, sizeof(SiCfgData->Usb3OverCurrentPin));
    CopyMem (&FspsConfig->UsbOverCurrentPinType, SiCfgData->UsbOverCurrentPinType, sizeof(SiCfgData->UsbOverCurrentPinType));
    CopyMem (&FspsConfig->Usb3CompatiblePort, SiCfgData->Usb3CompatiblePort, sizeof(SiCfgData->Usb3CompatiblePort));
    CopyMem (&FspsConfig->Usb3CompatiblePortNum, SiCfgData->Usb3CompatiblePortNum, sizeof(SiCfgData->Usb3CompatiblePortNum));
    CopyMem (&FspsConfig->PortUsb30Enable, SiCfgData->PortUsb30Enable, sizeof(SiCfgData->PortUsb30Enable));
    FspsConfig->PchXhciDwbEnable = 0;

    for (Index = 0; Index < PtlPcdGetUsb2MaxPhysicalPortNum(); Index++) {
      CopyMem (&FspsConfig->Usb2PhyPetxiset, SiCfgData->Usb2PhyPetxiset, sizeof(SiCfgData->Usb2PhyPetxiset));
      CopyMem (&FspsConfig->Usb2PhyTxiset, SiCfgData->Usb2PhyTxiset, sizeof(SiCfgData->Usb2PhyTxiset));
      CopyMem (&FspsConfig->Usb2PhyPredeemp, SiCfgData->Usb2PhyPredeemp, sizeof(SiCfgData->Usb2PhyPredeemp));
      CopyMem (&FspsConfig->Usb2PhyPehalfbit, SiCfgData->Usb2PhyPehalfbit, sizeof(SiCfgData->Usb2PhyPehalfbit));
    }

    // UFS
    CopyMem (&FspsConfig->UfsEnable, SiCfgData->UfsEnable, sizeof(SiCfgData->UfsEnable));

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
    CopyMem (&FspsConfig->PchIshI3cEnable, SiCfgData->PchIshI3cEnable, sizeof(SiCfgData->PchIshI3cEnable));

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
    CopyMem (&FspsConfig->PcieRpLtrOverrideSpecCompliant, SiCfgData->PcieRpLtrOverrideSpecCompliant, sizeof(SiCfgData->PcieRpLtrOverrideSpecCompliant));

    // Pcie Root Port MultiVc Mapping
#if PLATFORM_PTL
    CopyMem (&FspsConfig->PchPcieRpMultiVcEnabled, SiCfgData->PchPcieRpMultiVcEnabled, sizeof(SiCfgData->PchPcieRpMultiVcEnabled));
    CopyMem (&FspsConfig->PchPcieVc1TcMap, SiCfgData->PchPcieVc1TcMap, sizeof(SiCfgData->PchPcieVc1TcMap));
    CopyMem (&FspsConfig->PcieRootPortMvcTc1Map, SiCfgData->PcieRootPortMvcTc1Map, sizeof(SiCfgData->PcieRootPortMvcTc1Map));
    CopyMem (&FspsConfig->PcieRootPortMvcTc2Map, SiCfgData->PcieRootPortMvcTc2Map, sizeof(SiCfgData->PcieRootPortMvcTc2Map));
    CopyMem (&FspsConfig->PcieRootPortMvcTc3Map, SiCfgData->PcieRootPortMvcTc3Map, sizeof(SiCfgData->PcieRootPortMvcTc3Map));
    CopyMem (&FspsConfig->PcieRootPortMvcTc4Map, SiCfgData->PcieRootPortMvcTc4Map, sizeof(SiCfgData->PcieRootPortMvcTc4Map));
    CopyMem (&FspsConfig->PcieRootPortMvcTc5Map, SiCfgData->PcieRootPortMvcTc5Map, sizeof(SiCfgData->PcieRootPortMvcTc5Map));
    CopyMem (&FspsConfig->PcieRootPortMvcTc6Map, SiCfgData->PcieRootPortMvcTc6Map, sizeof(SiCfgData->PcieRootPortMvcTc6Map));
    CopyMem (&FspsConfig->PcieRootPortMvcTc7Map, SiCfgData->PcieRootPortMvcTc7Map, sizeof(SiCfgData->PcieRootPortMvcTc7Map));
#endif

    for (Index = 0; Index < (UINT8)PtlPcdGetMaxPcieControllerNum(); Index++) {
      FspsConfig->PcieRpLtrMaxSnoopLatency[Index]           = 0x100F;
      FspsConfig->PcieRpLtrMaxNoSnoopLatency[Index]         = 0x100F;
      FspsConfig->PcieRpSnoopLatencyOverrideValue[Index]    = 0x41;
      FspsConfig->PcieRpNonSnoopLatencyOverrideValue[Index] = 0x41;

      FspsConfig->PcieGen3EqPh3NoOfPresetOrCoeff[Index]     = 0x5;
      FspsConfig->PcieGen3EqPh3PostCursor0List[Index]       = 0xc;
      FspsConfig->PcieGen3EqPh3PostCursor2List[Index]       = 0xa;
      FspsConfig->PcieGen3EqPh3PreCursor9List[Index]        = 0x0;
      FspsConfig->PcieGen3EqPh3Preset0List[Index]           = 0x4;
      FspsConfig->PcieGen3EqPh3Preset1List[Index]           = 0x5;
      FspsConfig->PcieGen3EqPh3Preset2List[Index]           = 0x6;
      FspsConfig->PcieGen3EqPh3Preset3List[Index]           = 0x8;
      FspsConfig->PcieGen3EqPh3Preset4List[Index]           = 0x8;
      FspsConfig->PcieGen3EqPh3Preset5List[Index]           = 0x0;
      FspsConfig->PcieGen3EqPh3Preset6List[Index]           = 0x0;

      FspsConfig->PcieGen3PcetTimer[Index]                  = 0x2;
      FspsConfig->PcieGen4PcetTimer[Index]                  = 0x2;
      FspsConfig->PcieGen5PcetTimer[Index]                  = 0x2;
      FspsConfig->PcieGen3EqPh1DpTxPreset[Index]            = 0x7;
      FspsConfig->PcieGen4EqPh1DpTxPreset[Index]            = 0x7;
      FspsConfig->PcieGen5EqPh1DpTxPreset[Index]            = 0x5;

      FspsConfig->PcieGen4EqPh3PostCursor0List[Index]       = 0x4;
      FspsConfig->PcieGen4EqPh3PostCursor1List[Index]       = 0x5;
      FspsConfig->PcieGen4EqPh3PostCursor2List[Index]       = 0x6;
      FspsConfig->PcieGen4EqPh3PostCursor3List[Index]       = 0x0;
      FspsConfig->PcieGen4EqPh3PostCursor4List[Index]       = 0x0;
      FspsConfig->PcieGen4EqPh3PostCursor5List[Index]       = 0x7;
      FspsConfig->PcieGen4EqPh3PostCursor6List[Index]       = 0x0;
      FspsConfig->PcieGen4EqPh3NoOfPresetOrCoeff[Index]     = 0x3;
      FspsConfig->PcieGen5EqPh3NoOfPresetOrCoeff[Index]     = 0x4;

      FspsConfig->PcieGen5EqPh3PostCursor0List[Index]       = 0x4;
      FspsConfig->PcieGen5EqPh3PostCursor1List[Index]       = 0x5;
      FspsConfig->PcieGen5EqPh3PostCursor2List[Index]       = 0x6;
      FspsConfig->PcieGen5EqPh3PostCursor3List[Index]       = 0x9;
      FspsConfig->PcieRpUnsupportedRequestReport[Index]     = 0x1;
      FspsConfig->PcieRpFatalErrorReport[Index]             = 0x1;
      FspsConfig->PcieRpNoFatalErrorReport[Index]           = 0x1;
      FspsConfig->PcieRpCorrectableErrorReport[Index]       = 0x1;
      FspsConfig->PcieRpSnoopLatencyOverrideMode[Index]     = 0x1;
      FspsConfig->PcieRpNonSnoopLatencyOverrideMode[Index]  = 0x1;
      FspsConfig->PcieRpPhysicalSlotNumber[Index]           = (UINT8)Index;
    }

    CopyMem (&FspsConfig->TurboRatioLimitRatio, SiCfgData->TurboRatioLimitRatio, sizeof(SiCfgData->TurboRatioLimitRatio));
    CopyMem (&FspsConfig->AtomTurboRatioLimitRatio, SiCfgData->AtomTurboRatioLimitRatio, sizeof(SiCfgData->AtomTurboRatioLimitRatio));
    CopyMem (&FspsConfig->TurboRatioLimitNumCore, SiCfgData->TurboRatioLimitNumCore, sizeof(SiCfgData->TurboRatioLimitNumCore));
    CopyMem (&FspsConfig->AtomTurboRatioLimitNumCore, SiCfgData->AtomTurboRatioLimitNumCore, sizeof(SiCfgData->AtomTurboRatioLimitNumCore));

    FspsConfig->PchPmSlpS3MinAssert = SiCfgData->PchPmSlpS3MinAssert;
    FspsConfig->PchPmSlpSusMinAssert = SiCfgData->PchPmSlpSusMinAssert;

    FspsConfig->CpuCrashLogEnable               = SiCfgData->CpuCrashLogEnable;
    FspsConfig->IehMode                         = SiCfgData->IehMode;
    FspsConfig->OpioRecenter                    = SiCfgData->OpioRecenter;
    FspsConfig->D3HotEnable                     = SiCfgData->D3HotEnable;
    FspsConfig->D3ColdEnable                    = SiCfgData->D3ColdEnable;
    FspsConfig->TcCstateLimit                   = SiCfgData->TcCstateLimit;
    FspsConfig->VccSt                           = SiCfgData->VccSt;
    FspsConfig->L2QosEnumerationEn              = SiCfgData->L2QosEnumerationEn;
    FspsConfig->EnableTimedGpio0                = SiCfgData->EnableTimedGpio0;
    FspsConfig->EnableTimedGpio1                = SiCfgData->EnableTimedGpio1;

    FspsConfig->UsbTcPortEn                     = SiCfgData->UsbTcPortEn;
    FspsConfig->TcssTbtPerfBoost                = SiCfgData->TcssTbtPerfBoost;
    CopyMem (&FspsConfig->PortResetMessageEnable, SiCfgData->PortResetMessageEnable, sizeof(SiCfgData->PortResetMessageEnable));
    FspsConfig->EnableTcoTimer                  = SiCfgData->EnableTcoTimer;

  } //End of SiCfgData Ptr

  if (GetPayloadId () == 0) {
    // Disable SMI sources
    SmiEn = IoRead32((UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
    IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), SmiEn & (UINT32) (~B_ACPI_IO_SMI_EN_GBL_SMI_EN));
    // Lock down SMI
    FspsConfig->PchLockDownGlobalSmi = 0x1;
  } else if (GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE){
    FspsConfig->PchLockDownGlobalSmi = 0;
  }

  // Force Eiss and BiosLock off for now.
  // Enable it later in OS loader/EndofStages
  FspsConfig->PchLockDownBiosLock = 0x0;
  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    FspsUpd->FspsConfig.SiSkipBiosDoneWhenFwUpdate = TRUE;
    FspsUpd->FspsConfig.PchLockDownBiosInterface   = FALSE;
    FspsUpd->FspsConfig.PchSbAccessUnlock          = TRUE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock       = FALSE;
    DEBUG ((DEBUG_INFO, "Firmware update mode, unlock Bios setting\n"));
  } else {
    FspsUpd->FspsConfig.PchLockDownBiosInterface   = TRUE;
    FspsUpd->FspsConfig.PchSbAccessUnlock          = FALSE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock       = TRUE;
  }

  FspsConfig->AmtEnabled = 0;

  PowerCfgData = (POWER_CFG_DATA *) FindConfigDataByTag (CDATA_POWER_TAG);
  if (PowerCfgData == NULL) {
    DEBUG ((DEBUG_ERROR, "Missing power Cfg Data!\n"));
  } else {
    //CPU Power Management Basic Config
    FspsConfig->Hwp                           = PowerCfgData->Hwp;
    FspsConfig->TurboPowerLimitLock           = PowerCfgData->TurboPowerLimitLock;
    FspsConfig->TurboMode                     = PowerCfgData->TurboMode;
    FspsConfig->HwpInterruptControl           = PowerCfgData->HwpInterruptControl;

    FspsConfig->EnableHwpAutoPerCorePstate    = PowerCfgData->EnableHwpAutoPerCorePstate;
    FspsConfig->EnableHwpAutoEppGrouping      = PowerCfgData->EnableHwpAutoEppGrouping;
    FspsConfig->MinRingRatioLimit             = PowerCfgData->MinRingRatioLimit;
    FspsConfig->MaxRingRatioLimit             = PowerCfgData->MaxRingRatioLimit;
    FspsConfig->NumberOfEntries               = PowerCfgData->NumberOfEntries;
    FspsConfig->MlcStreamerPrefetcher         = PowerCfgData->MlcStreamerPrefetcher;
    FspsConfig->MlcSpatialPrefetcher          = PowerCfgData->MlcSpatialPrefetcher;
    FspsConfig->MonitorMwaitEnable            = PowerCfgData->MonitorMwaitEnable;
    FspsConfig->MachineCheckEnable            = PowerCfgData->MachineCheckEnable;
    FspsConfig->ProcessorTraceOutputScheme    = PowerCfgData->ProcessorTraceOutputScheme;
    FspsConfig->ProcessorTraceEnable          = PowerCfgData->ProcessorTraceEnable;
    FspsConfig->EnableFastMsrHwpReq           = PowerCfgData->EnableFastMsrHwpReq;
    FspsConfig->CpuBistData                   = PowerCfgData->CpuBistData;
    FspsConfig->CpuMpPpi                      = PowerCfgData->CpuMpPpi;

    CopyMem (FspsConfig->StateRatio, PowerCfgData->StateRatio, sizeof(PowerCfgData->StateRatio));
    CopyMem (FspsConfig->StateRatioMax16, PowerCfgData->StateRatioMax16, sizeof(PowerCfgData->StateRatioMax16));

    //CPU Power Management Custom Config
    FspsConfig->MaxRatio                      = PowerCfgData->MaxRatio;

    //CPU Power Management Psys Config

    //CPU Power Management Test Config
    FspsConfig->Eist                          = PowerCfgData->Eist;
    FspsConfig->EnergyEfficientPState         = PowerCfgData->EnergyEfficientPState;
    FspsConfig->EnergyEfficientTurbo          = PowerCfgData->EnergyEfficientTurbo;
    FspsConfig->EnableAllThermalFunctions     = PowerCfgData->EnableAllThermalFunctions;
    FspsConfig->Cx                            = PowerCfgData->Cx;
    FspsConfig->PmgCstCfgCtrlLock             = PowerCfgData->PmgCstCfgCtrlLock;
    FspsConfig->C1StateAutoDemotion           = PowerCfgData->C1StateAutoDemotion;
    FspsConfig->C1StateUnDemotion             = PowerCfgData->C1StateUnDemotion;
    FspsConfig->CStatePreWake                 = PowerCfgData->CStatePreWake;
    FspsConfig->TimedMwait                    = PowerCfgData->TimedMwait;
    FspsConfig->RaceToHalt                    = PowerCfgData->RaceToHalt;
    FspsConfig->PkgCStateLimit                = PowerCfgData->PkgCStateLimit;
    FspsConfig->TStates                       = PowerCfgData->TStates;
    FspsConfig->PkgCStateDemotion             = PowerCfgData->PkgCStateDemotion;
    FspsConfig->PkgCStateUnDemotion           = PowerCfgData->PkgCStateUnDemotion;
  }

  // MISC
  FspsConfig->PchFivrExtV1p05RailVoltage = 0x0;
  FspsConfig->PchFivrExtVnnRailVoltage = 0x0;
  FspsConfig->PchFivrExtVnnRailSxVoltage = 0x0;
  FspsConfig->PchFivrVccinAuxLowToHighCurModeVolTranTime = 0x0;
  FspsConfig->PchFivrVccinAuxRetToHighCurModeVolTranTime = 0x0;
  FspsConfig->PchFivrVccinAuxRetToLowCurModeVolTranTime = 0x0;
  FspsConfig->PchFivrVccinAuxOffToHighCurModeVolTranTime = 0x0;
  FspsConfig->PchFivrDynPm = 0x1;
  FspsConfig->PchFivrExtV1p05RailIccMaximum = 0x0;
  FspsConfig->PchFivrExtVnnRailIccMaximum = 0x0;
  FspsConfig->PchFivrExtVnnRailSxIccMaximum = 0x0;
  FspsConfig->PchPmSlpAMinAssert = 0x0;
  FspsConfig->PchPmPwrBtnOverridePeriod = 0x3;
  FspsConfig->PchEspiLgmrEnable = 0x0;
  FspsConfig->PmcModPhySusPgEnable = 0x0;
  FspsConfig->MeUnconfigOnRtcClear = 0x1;
  FspsConfig->PsOnEnable = 0x0;
  FspsConfig->PmcLpmS0ixSubStateEnableMask = 0x7;
  FspsConfig->PmcUsb2PhySusPgEnable = 0x0;
  FspsConfig->PtmEnabled[0]                   = 0x1;
  FspsConfig->PtmEnabled[1]                   = 0x1;
  FspsConfig->PtmEnabled[2]                   = 0x1;
  FspsConfig->PtmEnabled[3]                   = 0x1;
  FspsConfig->UfsDeviceConnected[0]           = 0x0;
  FspsConfig->UfsInlineEncryption[0]          = 0x1;
  FspsConfig->MaxRatio                        = 0x4;
  FspsConfig->X2ApicEnable                    = 0x1;
  FspsConfig->PchPmDisableEnergyReport        = 0x0;

  #if FixedPcdGetBool (PcdWclSupport) == 1
  FspsConfig->PchUnlockGpioPads               = 0x0;
  FspsConfig->XdciEnable                      = 0x1;

  FspsConfig->SerialIoUartRxPinMuxPolicy[1]   = 0x24806;
  FspsConfig->SerialIoUartTxPinMuxPolicy[1]   = 0x24807;
  FspsConfig->PchSerialIoI2cSdaPinMux[2]      = 0x22804;
  FspsConfig->PchSerialIoI2cSdaPinMux[3]      = 0x22806;
  FspsConfig->PchSerialIoI2cSdaPinMux[4]      = 0x3048D;
  FspsConfig->PchSerialIoI2cSdaPinMux[5]      = 0x3040D;
  FspsConfig->PchSerialIoI2cSclPinMux[2]      = 0x22805;
  FspsConfig->PchSerialIoI2cSclPinMux[3]      = 0x22807;
  FspsConfig->PchSerialIoI2cSclPinMux[4]      = 0x3048C;
  FspsConfig->PchSerialIoI2cSclPinMux[5]      = 0x3040C;

  FspsConfig->SerialIoI3cSdaPinMux[1]         = 0x24815;
  FspsConfig->SerialIoI3cSclPinMux[1]         = 0x24816;
  FspsConfig->SerialIoI3cSclFbPinMux[1]       = 0;
  FspsConfig->SerialIoLpssSpiClkPinMux[0]     = 0x30410;
  FspsConfig->SerialIoLpssSpiCsPinMux[0]      = 0x30412;
  FspsConfig->SerialIoLpssSpiMosiPinMux[0]    = 0x3040E;
  FspsConfig->SerialIoLpssSpiMisoPinMux[0]    = 0x3040F;

  FspsConfig->PchPmSlpAMinAssert              = 0x0;
  FspsConfig->TcssAuxOri = 0x0;

  FspsConfig->UfsDeviceConnected[0]           = 0x1;
  #else
  FspsConfig->PchUnlockGpioPads               = 0x1;
  FspsConfig->XdciEnable                      = 0x0;

  FspsConfig->SerialIoUartRxPinMuxPolicy[1]   = 0x1a488e;
  FspsConfig->SerialIoUartTxPinMuxPolicy[1]   = 0x1a488f;
  FspsConfig->PchSerialIoI2cSdaPinMux[2]      = 0x1a2884;
  FspsConfig->PchSerialIoI2cSdaPinMux[3]      = 0x1a2886;
  FspsConfig->PchSerialIoI2cSdaPinMux[4]      = 0x1a5012;
  FspsConfig->PchSerialIoI2cSdaPinMux[5]      = 0x1b040d;
  FspsConfig->PchSerialIoI2cSclPinMux[2]      = 0x1a2885;
  FspsConfig->PchSerialIoI2cSclPinMux[3]      = 0x1a2887;
  FspsConfig->PchSerialIoI2cSclPinMux[4]      = 0x1a5013;
  FspsConfig->PchSerialIoI2cSclPinMux[5]      = 0x1b040c;

  FspsConfig->SerialIoI3cSdaPinMux[1]         = 0x1a4895;
  FspsConfig->SerialIoI3cSclPinMux[1]         = 0x1a4896;
  FspsConfig->SerialIoI3cSclFbPinMux[1]       = 0;
  FspsConfig->SerialIoLpssSpiClkPinMux[0]     = 0x1aa48b;
  FspsConfig->SerialIoLpssSpiCsPinMux[0]      = 0x1aa491;
  FspsConfig->SerialIoLpssSpiMosiPinMux[0]    = 0x1aa48c;
  FspsConfig->SerialIoLpssSpiMisoPinMux[0]    = 0x1aa48d;

  FspsConfig->PchPmSlpAMinAssert              = 0x4;
  FspsConfig->TcssAuxOri = 0x1;
  #endif

  FspsConfig->CnviBtAudioOffload              = 0x1;
  FspsConfig->PmcV1p05PhyExtFetControlEn      = 0x1;

  FspsConfig->VmdEnable = 0;

  if (FeaturePcdGet (PcdEnablePciePm)) {
    StoreRpConfig (FspsConfig);
  }

  // EndOfPost Upd
  FspsUpd->FspsConfig.EndOfPostMessage = 1;
  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.S0ix == 1) {
      FspsConfig->Cx = 1;
      FspsConfig->XdciEnable = 0;
      FspsConfig->PchLanEnable = 0;

      ZeroMem (FspsConfig->PchIshI2cEnable, sizeof (FspsConfig->PchIshI2cEnable));
      ZeroMem (FspsConfig->PchIshI3cEnable, sizeof (FspsConfig->PchIshI3cEnable));
      ZeroMem (FspsConfig->PchPcieRpMultiVcEnabled, sizeof (FspsConfig->PchPcieRpMultiVcEnabled));
      ZeroMem (FspsConfig->PchIshGpEnable, sizeof (FspsConfig->PchIshGpEnable));
      FspsConfig->RenderStandby = 1;
      FspsConfig->Enable8254ClockGating = 1;
      FspsConfig->PchFivrDynPm = 1;
      FspsConfig->D3HotEnable = 1;
      FspsConfig->D3ColdEnable = 1;
      FspsConfig->NpuEnable = 0x0;
      FspsConfig->PkgCStateDemotion = 0;
      FspsConfig->PkgCStateUnDemotion = 0;
      FspsConfig->C1StateAutoDemotion = 0;
      FspsConfig->C1StateUnDemotion = 0;
      DEBUG ((DEBUG_INFO, "Stage 2 S0ix config applied.\n"));
    }
  }

#if FixedPcdGet8 (PcdTccEnabled)
  Status = TccModePostMemConfig (FspsUpd);
#endif

  //TXT
  if (FeaturePcdGet (PcdTxtEnabled)) {
    FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
    if (FeaturesCfgData != NULL) {
      if (FeaturesCfgData->Features.TxtEnabled == 1) {
        DEBUG((DEBUG_INFO, "Enabling TXT in FSP-S UPD's\n"));
        FspsConfig->TxtEnable = 0x1;
      }
    }
 }

  Status = FusaConfigPostMem(FspsUpdPtr);
  DEBUG((DEBUG_INFO, "FusaConfigPostMem Status %r\n", Status));
}
