/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/GpioLib.h>
#include <Library/SiGpioLib.h>
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
#include <Guid/FrameBufferInfoGuid.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/SerialPortInfoGuid.h>
#include <Pi/PiBootMode.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/HeciLib.h>
#include <Library/IoLib.h>
#include <Library/BpdtLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <FspsUpd.h>
#include <GpioDefines.h>
#include <GlobalNvsArea.h>
#include <IndustryStandard/Acpi.h>
#include <Library/SpiFlashLib.h>
#include <Library/TpmLib.h>
#include <Library/VtdLib.h>
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
#include <Service/PlatformService.h>
#include <Library/ResetSystemLib.h>
#include <Library/IocIpcLib.h>
#include <PlatformHookSupport.h>
#include <Library/MpInitLib.h>
#include <Library/BootOptionLib.h>
#include <ConfigDataCommonStruct.h>
#include <PsdLib.h>

#define IOC_UART_PPR_CLK_N_DIV        0x64
#define IOC_UART_PPR_CLK_M_DIV        0x40
#define R_XHCI_MEM_DUAL_ROLE_CFG0     0x80D8

#define NHLT_ACPI_TABLE_SIGNATURE  SIGNATURE_32 ('N', 'H', 'L', 'T')

#define NATIVE_PSTATE_LATENCY         10
#define PSTATE_BM_LATENCY             10
#define FVID_MAX_POWER                35000
#define FVID_MIN_POWER                15000

#pragma pack (1)
typedef struct {
  UINT8     NameOp;           // 12h ; First opcode is a NameOp
  UINT8     PackageLead;      // 20h ; Package OpCode
  UINT8     NumEntries;       // 06h ; Number of entries
  UINT8     DwordPrefix1;     // 0Ch
  UINT32    CoreFrequency;    // 00h
  UINT8     DwordPrefix2;     // 0Ch
  UINT32    Power;            // 00h
  UINT8     DwordPrefix3;     // 0Ch
  UINT32    TransLatency;     // 00h
  UINT8     DwordPrefix4;     // 0Ch
  UINT32    BMLatency;        // 00h
  UINT8     DwordPrefix5;     // 0Ch
  UINT32    Control;          // 00h
  UINT8     DwordPrefix6;     // 0Ch
  UINT32    Status;           // 00h
} PSS_PACKAGE_LAYOUT;
#pragma pack()

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

/**
  Get the serial port stride size.

**/
UINT8
GetSerialPortStrideSize (
  VOID
  );

/**
  Get the physical serial port base address.

**/
UINT32
GetSerialPortBase (
  VOID
  );
