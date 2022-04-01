/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __STAGE2_BOARD_INIT_LIB_H__
#define __STAGE2_BOARD_INIT_LIB_H__

#include <PiPei.h>
#include <Pi/PiBootMode.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/VariableLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/IoLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/CryptoLib.h>
#include <Library/VariableLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/MpInitLib.h>
#include <Library/BootOptionLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/SmbiosInitLib.h>
#include <Library/BdatLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/SerialPortInfoGuid.h>
#include <Guid/OsConfigDataHobGuid.h>
#include <Guid/SmmInformationGuid.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/SmBios.h>
#include <Service/PlatformService.h>
#include <ConfigDataCommonStruct.h>
#include <FspsUpd.h>
#include <GlobalNvsAreaDef.h>
#include "InterruptInit.h"
#include <Register/SaRegsHostBridge.h>
#include <Register/PmcRegs.h>
#include <PlatformBase.h>
#include <PchReservedResources.h>
#include <Library/GpioLib.h>
#include <Library/GpioSiLib.h>
#include <ConfigDataDefs.h>
#include <Library/PchInfoLib.h>
#include <CpuInitDataHob.h>
#include <CpuDataStruct.h>
#include <CpuRegs.h>
#include <IgdOpRegionDefines.h>
#include <Lpit.h>
#include <PchPcieRpInfo.h>
#include <PlatformBoardId.h>
#include <Register/Cpuid.h>
#include <Register/PchRegsSerialIoVer4.h>
#include <Register/PchPcieRpRegs.h>
#include <Register/PchBdfAssignment.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsLpc.h>
#include <Register/SpiRegs.h>
#include <PchLimits.h>
#include <Library/S3SaveRestoreLib.h>
#include <Library/IgdOpRegionLib.h>
#include <Register/IgdRegs.h>
#include <CpuPowerMgmt.h>
#include <Library/MpInitLib.h>
#include <Library/HeciLib.h>
#include <Library/PchInfoLib.h>
#include <Library/DmaRemappingTable.h>
#include <Library/VTdLib.h>
#include <Library/CpuPcieHsPhyInitLib.h>
#include <Include/TccConfigSubRegions.h>
#include <Library/PchPciBdfLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchPcieRpLib.h>
#include <Register/PchRegsPcr.h>
#include <Library/WatchDogTimerLib.h>
#include <LowPowerSupport.h>
#include <PsdLib.h>
#include "GpioPinsVer4S.h"
#include "GpioPinsVer2Lp.h"
#include <Library/TimerLib.h>
#include <Library/PrintLib.h>

#define NHLT_ACPI_TABLE_SIGNATURE  SIGNATURE_32 ('N', 'H', 'L', 'T')
#define V_EPOC_XTAL_38_4_MHZ  0x249F000
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

#define MAX_THREAD_PER_SOCKET (36)

#define PROC_LOCALNMI(socket, thread)                     \
  {                                                       \
    EFI_ACPI_4_0_LOCAL_APIC_NMI,                          \
    sizeof (EFI_ACPI_4_0_LOCAL_APIC_NMI_STRUCTURE),       \
    (socket * MAX_THREAD_PER_SOCKET + thread),            \
    0x000D,                                               \
    0x01,                                                 \
  },

#define V_BIG_CORE_TYPE             0x40
#define V_ATOM_CORE_TYPE            0x20
#define ATOM_MODULE_ID_1            0x8
#define ATOM_MODULE_ID_2            0x9

/**
  Time spent in the Package C-State.  It is given in units compatible to P1 clock frequency (Guaranteed / Maximum Core Non-Turbo Frequency).
  This time will be updated by PCODE only after the C-State exit (the update of this register has lower priority than actually ensuring that the C-State exit occurs).
  For C2, this value includes any forced or popup time from other states.
  For C3/C6/C7/C8/C9/C10, this value does not include popup periods, but advances even if VR actions are disabled.
**/
#define MSR_PC10_RCNTR 0x00000632

typedef struct {
  UINT8 DevNum;
  UINT8 Pid;
  UINT8 RpNumBase;
} PCH_PCIE_CONTROLLER_INFO;

 typedef struct {
  UINT8  SerialIoMode;
  UINT64 SerialIoPCIeConfig;
} PCH_SERIAL_IO_CONFIG_INFO;


typedef struct {
  UINT8  SerialIoUARTMode;
  UINT64 SerialIoUARTPCIeConfig;
  UINT8  SerialIoUARTDMAEnable;
  UINT8 SerialIoUARTPowerGating;
  UINT8 SerialIoUARTIrq;
 } PCH_SERIAL_IO_UART_CONFIG_INFO;

VOID
EnableLegacyRegions (
  VOID
);

VOID
GetCoreRatioAndNum (
  IN OUT ADL_MSR_BIGCORE_TURBO_RATIO_LIMIT_REGISTER *BigCoreRatioLimit,
  IN OUT ADL_MSR_BIGCORE_TURBO_RATIO_LIMIT_CORES_REGISTER *BigCoreNumCore,
  IN OUT ADL_MSR_ATOM_TURBO_RATIO_LIMIT_REGISTER *AtomRatioLimit,
  IN OUT ADL_MSR_ATOM_TURBO_RATIO_LIMIT_CORES_REGISTER *AtomNumCore
);

/**
  Initialize Interrupt Routing

**/
VOID
InterruptRoutingInit (
  VOID
  );

/**
  Initialize necessary information for Smbios

  @retval EFI_SUCCESS             Initialized necessary information successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory for Smbios info

**/
EFI_STATUS
InitializeSmbiosInfo (
  VOID
  );

/**
  Update current boot Payload ID.

**/
VOID
UpdatePayloadId (
  VOID
  );

/**
  Detect the type of core.

  @retval the core type which is running
    10h - Quark
    20h - Atom
    30H - Knights
    40H - Core
**/
UINT8
DetectCoreType (
  VOID
  );

EFI_STATUS
UpdateAcpiRtctTable (
  IN EFI_ACPI_DESCRIPTION_HEADER *RtctTable
  );


/**
Return Cpu stepping type

@retval UINT8                   Cpu stepping type
**/
CPU_STEPPING
EFIAPI
GetCpuStepping(
  VOID
  );

/**
  Return CPU Family ID

  @retval CPU_FAMILY              CPU Family ID
**/
CPU_FAMILY
EFIAPI
GetCpuFamily (
  VOID
  );

/**
  Detect if Hetero Core is supported.

  Check for CPU whether is Hetrogenous or not.

  @retval TRUE - Processor support HeteroCore
  @retval FALSE - Processor doesnt support HeteroCore
**/
BOOLEAN
IsHeteroCoreSupported (
  VOID
  );

/**
  Detect the type of core.

  @retval the core type which is running
    10h - Quark
    20h - Atom
    30H - Knights
    40H - Core
**/
UINT8
DetectCoreType (
  VOID
  );

/**
  Check if this is non-core processor - HT AP thread

  @retval TRUE if this is HT AP thread
  @retval FALSE if this is core thread
**/
BOOLEAN
IsSecondaryThread (
  VOID
  );

#endif // __STAGE2_BOARD_INIT_LIB_H__
