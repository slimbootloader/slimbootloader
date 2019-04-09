/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BOARD_INIT_LIB_H_

#include <Guid/GraphicsInfoHob.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/SerialPortInfoGuid.h>
#include <Guid/LoaderFspInfoGuid.h>
#include <Guid/MemoryMapInfoGuid.h>
#include <Guid/PerformanceInfoGuid.h>
#include <Guid/OsBootOptionGuid.h>
#include <Guid/BootLoaderServiceGuid.h>
#include <Guid/FlashMapInfoGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Service/PlatformService.h>

/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
  );

/**
  Update FSP-S/FSP-M UPD config data

  @param  FspUpdPtr            The pointer to the FSP-S/FSP-M UPD to be updated.
**/
VOID
UpdateFspConfig (
  VOID     *FspUpdPtr
  );

/**
  Disable watch dog timer (Halt TCO timer).
**/
VOID
DisableWatchDogTimer (
  VOID
  );

/**
  Enables the execution by writing to the MSR.
**/
VOID
EnableCodeExecution (
  VOID
  );

/**
  Search for the saved MrcParam to initialize Memory for fastboot.

  @retval Found MrcParam or NULL
**/
VOID *
EFIAPI
FindNvsData (
  VOID
  );

/**
  Save MRC data onto the Flash region from NVS buffer.

  @param Param pointer to an NVS data
  @param Length Length of the NVS data(MRC)

  @retval EFI_SUCCESS
  @retval RETURN_ALREADY_STARTED
  @retval EFI_DEVICE_ERROR
  @retval EFI_INVALID_PARAMETER
 **/
EFI_STATUS
EFIAPI
SaveNvsData (
  IN  VOID    *Param,
  IN  UINT32   Length
  );

/**

  Builds a Handoff Information Table HOB.

  @param BootMode        - Current Bootmode
  @param MemoryBegin     - Start Memory Address.
  @param MemoryLength    - Length of Memory.

  @return EFI_SUCCESS Always success to initialize HOB.

**/
EFI_STATUS
BuildHobHandoffInfoTable (
  IN EFI_BOOT_MODE         BootMode,
  IN EFI_PHYSICAL_ADDRESS  MemoryBegin,
  IN UINT64                MemoryLength
  );

/**
 Update Hob Info with platform specific data

 @param  Guid          The GUID to tag the customized HOB.
 @param  HobInfo       The start address of GUID HOB data.
 **/
VOID
EFIAPI
PlatformUpdateHobInfo (
  IN CONST EFI_GUID              *Guid,
  OUT      VOID                  *HobInfo
  );

/**
  Update PCH NVS and SA NVS area address and size in ACPI table.

  @param[in] Current       Pointer to ACPI description header
  @return EFI_STATUS Success.
  @return EFI_DEVICE_ERROR ACPI Device error.

**/
EFI_STATUS
PlatformUpdateAcpiTable (
  IN UINT8    *Current
  );

/**
  Dynamic update of Global NVS data.

  @param[in] GnvsIn       Pointer to global nvs region

**/
VOID
EFIAPI
PlatformUpdateAcpiGnvs (
  IN VOID  *GnvsIn
  );

/**
  Update the Smbios Tables

  @param[in]  SmbiosEntry   Pointer to the beginning of the Smbios entry+table range
**/
VOID
EFIAPI
PlatformUpdateSmbios (
  IN  UINT32    SmbiosEntry
  );

/**
  Get size of Platform Specific Data.

  @param[in] none

  @retval    UINT32     Size of Platform Specific Data

**/
UINT32
EFIAPI
GetPlatformDataSize (
  IN  VOID
  );

#endif
