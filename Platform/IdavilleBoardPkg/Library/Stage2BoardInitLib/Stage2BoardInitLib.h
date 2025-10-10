/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
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
#include <Library/StageCommonBoardInitLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/IoLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/CryptoLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/MpInitLib.h>
#include <Library/BootOptionLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/SmbiosInitLib.h>
#include <Library/GpioLib.h>
#include <Library/GpioSiLib.h>
#include <Library/AspeedGfxLib.h>
#include <GpioConfig.h>
#include <Library/UncoreLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcieRpLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/VTdLib.h>
#include <Library/TpmLib.h>
#include <Library/HeciLib.h>
#include <Library/PrintLib.h>
#include <Library/PciEnumerationLib.h>
#include <MkhiMsgs.h>
#include <GpioPinsCdf.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/OsConfigDataHobGuid.h>
#include <Guid/SmmInformationGuid.h>
#include <UniversalPayload/PciRootBridges.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/SmBios.h>
#include <Service/PlatformService.h>
#include <ConfigDataCommonStruct.h>
#include <FspsUpd.h>
#include <FspmUpd.h>
#include <GlobalNvsArea.h>
#include <ConfigDataDefs.h>
#include <PchReservedResources.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsP2sb.h>
#include <Register/PchRegsSpi.h>
#include <Register/CpuRegs.h>
#include <Register/RegsSpi.h>
#include <Acpi/AcpiTablePlatform.h>
#include <CpuDataStruct.h>
#include <PlatformData.h>
#include <PlatformBase.h>
#include <Library/UefiVariableLib.h>
#include <Library/TccLib.h>
#include <Library/TmeVarLib.h>
#include <Library/BdatLib.h>
#include <PsdLib.h>
#include <TccConfigSubRegions.h>
#include <Library/SgxLib.h>
#include <Register/Intel/ArchitecturalMsr.h>
#include <Register/Intel/Cpuid.h>
#include <Library/WatchDogTimerLib.h>

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

#pragma pack (push,1)

///
/// The PCH_DEVICE_INTERRUPT_CONFIG block describes interrupt pin, IRQ and interrupt mode for PCH device.
///
typedef struct {
  UINT8        Device;                  ///< Device number
  UINT8        Function;                ///< Device function
  UINT8        IntX;                    ///< Interrupt pin: INTA-INTD (see PCH_INT_PIN)
  UINT8        Irq;                     ///< IRQ to be set for device.
} PCH_DEVICE_INTERRUPT_CONFIG;

#pragma pack (pop)


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
  Initialize available platform services for Payload

**/
VOID
PlatformServiceInit (
  VOID
  );

/**
  Platform specific Post Silicon Init

**/
VOID
PlatformPostSiliconInit (
  VOID
  );

/**
  Platform specific Pre Pci Enumeration

**/
VOID
PlatformPrePciEnumeration (
  VOID
  );

/**
  Platform specific Post Pci Enumeration

**/
VOID
PlatformPostPciEnumeration (
  VOID
  );

/**
  Dump Debug Info

**/
VOID
DumpDebugInfo (
  VOID
  );

/**
  Patch DSDT Table

  @param[in] Table            Pointer to DSDT ACPI Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchDsdtTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  );

/**
  Updates DMA Remapping Table (DMAR)

  @param[in] Table            Pointer to DMAR ACPI Table

  @retval    EFI_SUCCESS      Returned Successfully
  @retval    EFI_UNSUPPORTED  Returned Unsupported

**/
EFI_STATUS
PatchDmarTable(
   IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
   );

/**
  Updates BDAT

  @param[in] Table            Pointer to BDAT ACPI Table

  @retval    EFI_SUCCESS      Returned Successfully
  @retval    EFI_UNSUPPORTED  Returned Unsupported

**/
EFI_STATUS
PatchBdatTable(
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  );

/**
  Patch TPM2 ACPI Table

  @param[in] Table            Pointer to TPM2 ACPI Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchTpm2AcpiTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  );

/**
  Patch RTCT ACPI Table

  @param[in] Table            Pointer to RTCT ACPI Table

  @retval    RETURN_SUCCESS   Operation completed successfully.
  @retval    EFI_UNSUPPORTED  Returned Unsupported

**/
RETURN_STATUS
PatchRtctAcpiTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  );

/**
  Update the CPU PM SSDT table

  @param[in] Table            Pointer to CPU PM SSDT Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchCpuPmSsdtTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  );

/**
  Update the OEM1 P-State SSDT table (EIST)

  @param[in] Table            Pointer to CPU EIST SSDT Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchOem1SsdtTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  );

/**
  Update the SPS NM SSDT table

  @param[in] Table            Pointer to SPS NM SSDT Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchSpsNmSsdtTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  );

/**
  Initialize BMC Video for Splash screen

**/
VOID
InitializeBmcVideo (
  VOID
  );

#endif // __STAGE2_BOARD_INIT_LIB_H__
