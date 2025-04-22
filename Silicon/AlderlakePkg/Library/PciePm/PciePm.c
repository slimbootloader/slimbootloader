/** @file
  This file enables PCIe PM configuration

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <FspsUpd.h>
#include <Library/DebugLib.h>
#include <Library/PciePm.h>
#include <PcieRegs.h>
#include <Register/PchPcieRpRegs.h>
#include <Library/PciSegmentLib.h>
#include <Library/PcieHelperLib.h>
#include <Library/PchPciBdfLib.h>
#include <Library/PchPcieRpConfig.h>
#include <Library/PciExpressHelpersLib.h>
#include <Library/CpuPcieHsPhyInitLib.h>
#include <Library/PchInfoLib.h>
#include <PcieRegs.h>
#include <PchAccess.h>
#include <CpuDataStruct.h>
#include <CpuRegs.h>
#include <Register/Cpuid.h>
#include <Register/SaRegsHostBridge.h>
#include <Library/PciLib.h>
#include <Library/PlatformInfo.h>

#define SA_SEG_NUM         0x00
#define SA_MC_BUS          0x00

#define PCI_CLASS_NETWORK             0x02
#define PCI_CLASS_NETWORK_ETHERNET    0x00
#define PCI_CLASS_NETWORK_OTHER       0x80

#define CPU_PCIE_MAX_ROOT_PORTS       4
#define PCIE_HWEQ_COEFFS_MAX          5

#define CPU_PCIE_ULT_MAX_ROOT_PORT         3
#define CPU_PCIE_ULX_MAX_ROOT_PORT         1
#define CPU_PCIE_DT_HALO_MAX_ROOT_PORT     3
#define CPU_PCIE_ULT_MAX_CONTROLLER        3
#define CPU_PCIE_ULX_MAX_CONTROLLER        1
#define CPU_PCIE_DT_HALO_MAX_CONTROLLER    2

GLOBAL_REMOVE_IF_UNREFERENCED PCH_PCIE_DEVICE_OVERRIDE mPcieDeviceTable[] = {
  //
  // Intel PRO/Wireless
  //
  { 0x8086, 0x422b, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x422c, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x4238, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x4239, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel WiMAX/WiFi Link
  //
  { 0x8086, 0x0082, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0085, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0083, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0084, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0086, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0087, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0088, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0089, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x008F, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0090, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Crane Peak WLAN NIC
  //
  { 0x8086, 0x08AE, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x08AF, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Crane Peak w/BT WLAN NIC
  //
  { 0x8086, 0x0896, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0897, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Kelsey Peak WiFi, WiMax
  //
  { 0x8086, 0x0885, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0886, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 105
  //
  { 0x8086, 0x0894, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0895, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 135
  //
  { 0x8086, 0x0892, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0893, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 2200
  //
  { 0x8086, 0x0890, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0891, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 2230
  //
  { 0x8086, 0x0887, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0888, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 6235
  //
  { 0x8086, 0x088E, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x088F, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel CampPeak 2 Wifi
  //
  { 0x8086, 0x08B5, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x08B6, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel WilkinsPeak 1 Wifi
  //
  { 0x8086, 0x08B3, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0158, 0x0000000F, 0, 0, 0, 0, 0 },
  { 0x8086, 0x08B4, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0158, 0x0000000F, 0, 0, 0, 0, 0 },
  //
  // Intel Wilkins Peak 2 Wifi
  //
  { 0x8086, 0x08B1, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0158, 0x0000000F, 0, 0, 0, 0, 0 },
  { 0x8086, 0x08B2, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0158, 0x0000000F, 0, 0, 0, 0, 0 },
  //
  // Intel Wilkins Peak PF Wifi
  //
  { 0x8086, 0x08B0, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Teton Glacier Endpoint
  //
  { 0x8086, 0x0975, 0xff, 0, 0, 0, PchPcieL1SubstatesOverride, 0, 0xff, 0x3C, 0, 5, 0, 0 },
  //
  // Pyramid Glacier Endpoint
  //
  { 0x8086, 0x09AD, 0xff, 0, 0, 0, PchPcieL1SubstatesOverride, 0, 0xff, 0x3C, 0, 5, 0, 0 },

  //
  // End of Table
  //
  { 0 }
};

GLOBAL_REMOVE_IF_UNREFERENCED PCH_PCIE_ROOT_PORT_CONFIG     mPcieRootPortConfig[PCH_MAX_PCIE_ROOT_PORTS];
GLOBAL_REMOVE_IF_UNREFERENCED PCH_PCIE_ROOT_PORT_CONFIG     mCpuPcieRootPortConfig[CPU_PCIE_MAX_ROOT_PORTS];

/**
  Store Root Port Config based on FSP-s UPDs

  @param[in]  FspsConfig       The pointer to FSP-S Config

**/
VOID
EFIAPI
StoreRpConfig (
  VOID *FspsConfig
)
{
  FSP_S_CONFIG  *Config;
  UINT8         MaxPchPcieRootPorts;
  UINT8         MaxCpuPciePorts;
  UINT8         Index;

  Config = (FSP_S_CONFIG *)FspsConfig;
  if (Config == NULL) {
    return;
  }

  MaxPchPcieRootPorts = GetPchMaxPciePortNum ();
  for (Index = 0; Index < MaxPchPcieRootPorts; Index++) {
    mPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.LtrMaxNoSnoopLatency = Config->PcieRpLtrMaxNoSnoopLatency[Index];
    mPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.LtrMaxSnoopLatency = Config->PcieRpLtrMaxSnoopLatency[Index];
    mPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.LtrConfigLock = Config->PcieRpLtrConfigLock[Index];
    mPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.SnoopLatencyOverrideMode = Config->PcieRpSnoopLatencyOverrideMode[Index];
    mPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.SnoopLatencyOverrideValue = Config->PcieRpSnoopLatencyOverrideValue[Index];
    mPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.SnoopLatencyOverrideMultiplier = Config->PcieRpSnoopLatencyOverrideMultiplier[Index];
    mPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.NonSnoopLatencyOverrideMode = Config->PcieRpNonSnoopLatencyOverrideMode[Index];
    mPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.NonSnoopLatencyOverrideValue = Config->PcieRpNonSnoopLatencyOverrideValue[Index];
    mPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.NonSnoopLatencyOverrideMultiplier = Config->PcieRpNonSnoopLatencyOverrideMultiplier[Index];

    mPcieRootPortConfig[Index].PcieRpCommonConfig.EnableCpm = Config->PcieRpEnableCpm[Index];
  }

  MaxCpuPciePorts = GetMaxCpuPciePortNum();
  for (Index = 0; Index < MaxCpuPciePorts; Index++) {
    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.LtrMaxNoSnoopLatency = Config->CpuPcieRpLtrMaxNoSnoopLatency[Index];
    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.LtrMaxSnoopLatency = Config->CpuPcieRpLtrMaxSnoopLatency[Index];
    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.LtrConfigLock = Config->CpuPcieRpLtrConfigLock[Index];
    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.SnoopLatencyOverrideMode = Config->CpuPcieRpSnoopLatencyOverrideMode[Index];
    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.SnoopLatencyOverrideValue = Config->CpuPcieRpSnoopLatencyOverrideValue[Index];
    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.SnoopLatencyOverrideMultiplier = Config->CpuPcieRpSnoopLatencyOverrideMultiplier[Index];
    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.NonSnoopLatencyOverrideMode = Config->CpuPcieRpNonSnoopLatencyOverrideMode[Index];
    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.NonSnoopLatencyOverrideValue = Config->CpuPcieRpNonSnoopLatencyOverrideValue[Index];
    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.PcieRpLtrConfig.NonSnoopLatencyOverrideMultiplier = Config->CpuPcieRpNonSnoopLatencyOverrideMultiplier[Index];

    mCpuPcieRootPortConfig[Index].PcieRpCommonConfig.EnableCpm = Config->CpuPcieRpEnableCpm[Index];
  }
}

/**
  Get CPU Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
EFIAPI
GetCpuPcieRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFun
  )
{
  if (RpNumber > GetMaxCpuPciePortNum ()) {
    DEBUG ((DEBUG_ERROR, "GetCpuPcieRpDevFun invalid RpNumber %x", RpNumber));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  switch (RpNumber) {
    case 0:
      *RpDev = 6;
      *RpFun = 0;
      break;
    case 1:
      *RpDev = 1;
      *RpFun = 0;
      break;
    case 2:
      if (GetCpuSku () == EnumCpuTrad) {
        *RpDev = 1;
        *RpFun = 1;
      } else {
        *RpDev = 6;
        *RpFun = 2;
      }
      break;
    default:
      *RpDev = 6;
      *RpFun = 0;
      break;
  }
  return EFI_SUCCESS;
}


/**
  Config CPU PCIE power management settings
**/
VOID
CpuPciePmConfig (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT32      PortIndex;
  UINT64      RpBase;
  UINT8       MaxPciePortNum;
  UINTN       RpDevice;
  UINTN       RpFunction;
  UINT8       TempRootPortBusNumMin;
  UINT8       TempRootPortBusNumMax;

  TempRootPortBusNumMin = 2;
  TempRootPortBusNumMax = 10;

  MaxPciePortNum = GetMaxCpuPciePortNum ();

  for (PortIndex = 0; PortIndex < MaxPciePortNum; PortIndex++) {
    Status = GetCpuPcieRpDevFun (PortIndex, &RpDevice, &RpFunction);
    if (EFI_ERROR (Status)) {
      continue;
    }

    RpBase = PCI_SEGMENT_LIB_ADDRESS (SA_SEG_NUM, SA_MC_BUS, (UINT32) RpDevice, (UINT32) RpFunction, 0);

    if (PciSegmentRead16 (RpBase) != 0xFFFF) {
      RootportDownstreamPmConfiguration (
        SA_SEG_NUM,
        SA_MC_BUS,
        (UINT8)RpDevice,
        (UINT8)RpFunction,
        TempRootPortBusNumMin,
        TempRootPortBusNumMax,
        &mCpuPcieRootPortConfig[PortIndex].PcieRpCommonConfig,
        0,
        NULL
      );
    }
  }
}

/**
  Config PCH PCIE power management settings
**/
VOID
PchPciePmConfig (
  VOID
  )
{
  UINT32  PortIndex;
  UINT64  RpBase;
  UINT8   MaxPciePortNum;
  UINT8   TempRootPortBusNumMin;
  UINT8   TempRootPortBusNumMax;

  TempRootPortBusNumMin = 2;
  TempRootPortBusNumMax = 10;

  MaxPciePortNum = GetPchMaxPciePortNum ();

  for (PortIndex = 0; PortIndex < MaxPciePortNum; PortIndex++) {
    RpBase = PchPcieRpPciCfgBase (PortIndex);

    if (PciSegmentRead16 (RpBase) != 0xFFFF) {
      RootportDownstreamPmConfiguration (
        DEFAULT_PCI_SEGMENT_NUMBER_PCH,
        DEFAULT_PCI_BUS_NUMBER_PCH,
        PchPcieRpDevNumber (PortIndex),
        PchPcieRpFuncNumber (PortIndex),
        TempRootPortBusNumMin,
        TempRootPortBusNumMax,
        &mPcieRootPortConfig[PortIndex].PcieRpCommonConfig,
        ARRAY_SIZE (mPcieDeviceTable) - 1,
        mPcieDeviceTable
      );

    }
  }
}

/**
  Config PCIE power management settings
**/
VOID
EFIAPI
PciePmConfig (
  VOID
)
{
  CpuPciePmConfig ();
  PchPciePmConfig ();
}
