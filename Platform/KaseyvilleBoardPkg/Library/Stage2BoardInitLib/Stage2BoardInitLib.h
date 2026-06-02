/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/MpInitLib.h>
#include <Library/BootOptionLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/SmbiosInitLib.h>
#include <Library/AspeedGfxLib.h>
#include <Library/PchInfoLib.h>
#include <Library/KsvVtdLib.h>
#include <Library/DmarLib.h>
#include <Library/MadtLib.h>
#include <Library/TpmLib.h>
#include <Library/PrintLib.h>
#include <Library/PciEnumerationLib.h>
#include <Guid/SmmInformationGuid.h>
#include <UniversalPayload/PciRootBridges.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/SmBios.h>
#include <FspsUpd.h>
#include <FspmUpd.h>
#include <GlobalNvsArea.h>
#include <ConfigDataDefs.h>
#include <PchReservedResources.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsP2sb.h>
#include <Register/CpuRegs.h>
#include <Register/RegsSpi.h>
#include <Acpi/AcpiTablePlatform.h>
#include <CpuDataStruct.h>
#include <PlatformData.h>
#include <PlatformBase.h>
#include <Library/UefiVariableLib.h>
#include <Library/BdatLib.h>
#include <Library/SgxLib22.h>
#include <IioUniversalDataHob.h>
#include <Library/GpioV2Lib.h>
#include <GpioV2Config.h>
#include <Register/GpioV2ChipsetId.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Register/SpiRegs.h>
#include <Library/TimerLib.h>
#include <FlashProtectionConfig.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PciSegmentLib.h>
#include <Library/FspSupportLib.h>
#include <Library/McfgLib.h>
#include <Library/VariableLib.h>

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
  Patch DBG2 ACPI Table

  @param[in] Table            Pointer to DBG2 ACPI Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchDbg2Table (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
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
  Patch DSDT Table AML code

  @param[in] Table            Pointer to DSDT ACPI Table
  @param[in] Gnvs             Pointer to global NVS data

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
EFI_STATUS
PatchDsdtTableAML (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER   *Table,
  IN OUT GLOBAL_NVS_AREA               *Gnvs
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
  Build MADT table at runtime using MadtLib

  @param[in,out] Table       Pointer to MADT ACPI Table

  @retval    EFI_SUCCESS     Table built successfully
**/
EFI_STATUS
PatchMadtTable (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER           *Table
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
  Update the MCFG table with platform-specific configuration data.

  This serves as the main entry point for building the MCFG table at runtime.
  It initializes the table header and adds configuration space structures
  based on platform configuration.

  @param[in, out] AcpiTable   Pointer to the ACPI table buffer.

  @retval         EFI_SUCCESS            Returned Successfully
  @retval         EFI_INVALID_PARAMETER  Returned Invalid parameter

**/
EFI_STATUS
PatchMcfgTable(
  IN OUT EFI_ACPI_DESCRIPTION_HEADER           *Table
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
  Initialize BMC Video for Splash screen

**/
VOID
InitializeBmcVideo (
  VOID
  );

#endif // __STAGE2_BOARD_INIT_LIB_H__
