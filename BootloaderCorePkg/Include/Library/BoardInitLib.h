/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOARD_INIT_LIB_H_

#include <Guid/GraphicsInfoHob.h>
#include <UniversalPayload/SerialPortInfo.h>
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
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
  );

/**
  Update FSP-S/FSP-M UPD config data

  @param  FspUpdPtr            The pointer to the FSP-S/FSP-M UPD to be updated.
**/
VOID
EFIAPI
UpdateFspConfig (
  VOID     *FspUpdPtr
  );

/**
  Disable watch dog timer (Halt TCO timer).
**/
VOID
EFIAPI
DisableWatchDogTimer (
  VOID
  );

/**
  Enables the execution by writing to the MSR.
**/
VOID
EFIAPI
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
EFIAPI
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
EFIAPI
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
