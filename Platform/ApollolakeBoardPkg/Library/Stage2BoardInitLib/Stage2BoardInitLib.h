/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/GpioLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/IgdOpRegionLib.h>
#include <Library/MmcAccessLib.h>
#include <Library/MmcTuningLib.h>
#include <Library/SerialPortLib.h>
#include <Library/RleCompressLib.h>
#include <Library/VariableLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/FspSupportLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Pi/PiBootMode.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/HeciLib.h>
#include <Library/IoLib.h>
#include <Library/BpdtLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/BoardSupportLib.h>
#include <FspsUpd.h>
#include <GpioDefines.h>
#include <GlobalNvsArea.h>
#include <IndustryStandard/Acpi.h>
#include <Library/SpiFlashLib.h>
#include <Library/TpmLib.h>
#include <Library/VTdLib.h>
#include <Library/DmarLib.h>
#include <Library/VtdPmrLib.h>
#include <RegAccess.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/Pci22.h>
#include <ConfigDataDefs.h>
#include <PlatformData.h>
#include <Guid/OsConfigDataHobGuid.h>
#include <Library/HobLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/CryptoLib.h>
#include <Guid/SeedInfoHobGuid.h>
#include <Library/SeedListInfoLib.h>
#include <Library/VariableLib.h>
#include <CpuRegs.h>
#include <SaRegs.h>
#include <Service/PlatformService.h>
#include <Library/ResetSystemLib.h>
#include <Library/IocIpcLib.h>
#include <PlatformHookSupport.h>
#include <Library/MpInitLib.h>
#include <Library/BootOptionLib.h>
#include <ConfigDataCommonStruct.h>
#include <PsdLib.h>
#include <Guid/SmmInformationGuid.h>
#include <Library/HdaLib.h>
#include <Register/RegsSpi.h>
#include <Register/Intel/Cpuid.h>
#include <CseMsg.h>
#include <Library/GpioLibApl.h>
#include <Library/PlatformHookLib.h>

#define IOC_UART_PPR_CLK_N_DIV        0x64
#define IOC_UART_PPR_CLK_M_DIV        0x40
#define R_XHCI_MEM_DUAL_ROLE_CFG0     0x80D8

#define FVID_MAX_POWER                35000
#define FVID_MIN_POWER                15000

typedef struct {
  UINT32    VendorDeviceId;   // Codec Vendor/Device ID
  UINT8     RevisionId;       // Revision ID of the codec. 0xFF matches any revision.
  UINT8     SdiNo;            // SDI number, 0xFF matches any SDI.
  UINT16    DataDwords;       // Number of data DWORDs following the header.
  UINT8     FrontPanelSupport;
  UINT8     Padding[3];
} BL_HDA_VERB_TABLE_HEADER;

typedef struct {
  BL_HDA_VERB_TABLE_HEADER     VerbTableHeader;
  UINT32                       VerbTableData[];
} BL_HDAUDIO_VERB_TABLE;

typedef enum  {
  UserSeed,
  DeviceSeed,
  RpmbKey
} SEED_TYPE;

/**
 Append each seed into the Seed List HOB

 @param[out] SeedListInfoHob         pointer to SeedList HOB data structure.
 **/
EFI_STATUS
UpdateSeedListInfo (
  OUT SEED_LIST_INFO_HOB  *SeedListInfoHob
);

/**
  This is to generate seeds.

  @retval     EFI_SUCCESS          Successfully generated seeds to seedstore.
  @retval     EFI_PROTOCOL_ERROR   Successfully generated seeds to seedstore.
  @retval     EFI_OTHERS           Error in getting the serial number from SPI.
**/
EFI_STATUS
GenerateSeeds (
  VOID
  );
