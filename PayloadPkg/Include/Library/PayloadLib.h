/** @file

Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.

This program and the accompanying materials are licensed and made available
under the terms and conditions of the BSD License which accompanies this
distribution. The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PAYLOADLIB_H__
#define __PAYLOADLIB_H__

#include <PiPei.h>
#include <Guid/LoaderFspInfoGuid.h>
#include <Guid/MemoryMapInfoGuid.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/PerformanceInfoGuid.h>
#include <Guid/LoaderLibraryDataGuid.h>
#include <Library/BaseLib.h>
#include <Library/BootloaderCommonLib.h>

#define  PLD_GDATA_SIGNATURE     SIGNATURE_32('P', 'L', 'D', 'G')

typedef struct {
  UINT32           Signature;
  VOID             *LibDataPtr;
  VOID             *ServiceList;
  VOID             *PcdDataPtr;
  VOID             *LogBufPtr;
  VOID             *CfgDataPtr;
  VOID             *DeviceTable;
  UINT32           LdrFeatures;
  BL_PERF_DATA     PerfData;
} PAYLOAD_GLOBAL_DATA;


/**
  Returns the System table info HOB data.

  @retval   Pointer to the system table info hob

**/
SYSTEM_TABLE_INFO *
GetSystemTableInfo (
  VOID
  );

/**
  Returns the pointer to the Loader platform info.

  @return The pointer to the Loader platform info.

**/
VOID *
EFIAPI
GetLoaderPlatformInfoPtr (
  VOID
  );

/**
  function to parse ACPI table.

  @param    AcpiBase   Acpi Base adress

**/
VOID
ParseAcpiTableInfo (
  UINT32  AcpiBase
  );

/**
  Returns the memory map info HOB data.

  @retval   Pointer to the memory map info hob

**/
MEMORY_MAP_INFO *
EFIAPI
GetMemoryMapInfo (
  VOID
  );

/**
  Returns the maximum available memory region.

  Find available memory region from memory map info hob

  @param[out] Base  Base address of the available memory region.
  @param[out] Size  Size of the available memory region.

  @retval RETURN_SUCCESS        Found the memory map entry.
  @retval RETURN_NOT_FOUND      Memory map entry not found.

**/
RETURN_STATUS
EFIAPI
GetMaxAvailableRamRegion (
  UINT64  *Base,
  UINT64  *Size
  );

/**
  Returns the payload reserved memory region.

  Find payload reserved memory region from memory map info hob

  @param[out] Base  Base address of reserved memory region.
  @param[out] Size  Size of the reserved memory region.

  @retval RETURN_SUCCESS        Found the memory map entry.
  @retval RETURN_NOT_FOUND      Memory map entry not found.

**/
RETURN_STATUS
EFIAPI
GetPayloadReservedRamRegion (
  UINT64  *Base,
  UINT64  *Size
  );

/**
  Returns the timestamps data.

  Finds timestamps data from loader performance info hob

  @param[out] PerfData  Pointer to loader performance data.

  @retval EFI_SUCCESS        Found the timestamps data.
  @retval RETURN_NOT_FOUND   Loader performance hob not found.

**/
EFI_STATUS
GetLoaderPerformanceInfo (
  BL_PERF_DATA  *PerfData
  );

/**
  Payload main entry.

  This function is the main entry for payload.

  @param[in]  Param        Parameter passed from PayloadEntryLib SwitchStack().

**/
VOID
EFIAPI
PayloadMain (
  IN  VOID *Param
  );

#endif /* __PAYLOADLIB_H__ */
