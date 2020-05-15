/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOARD_SUPPORT_LIB_H_
#define _BOARD_SUPPORT_LIB_H_

#include <Guid/OsBootOptionGuid.h>
#include <Library/FirmwareUpdateLib.h>

/**
  A function pointer to get relative power number in mW

  @param[in]  BaseRatio     Base bus ratio
  @param[in]  CurrRatio     Current bus ratio to get relative power
  @param[in]  BasePower     Base power number in mW

  @retval                   Calculated power number in mW

**/
typedef UINT32 (EFIAPI *GET_RELATIVE_POWER_FUNC) (
  UINT16  BaseRatio,
  UINT16  CurrRatio,
  UINT32  BasePower
  );

/**
  All PSS calculation related parameters. GetRelativePower can be optional.

  @param[in]      TurboBusRatio     Turbo bus ratio
  @param[in]      MaxBusRatio       Maximum bus ratio
  @param[in]      MinBusRatio       Mimimum bus ratio
  @param[in]      PackageMaxPower   Maximum package power
  @param[in]      PackageMinPower   Minimum package power
  @param[in]      GetRelativePower  A func pointer to get relative power
  @param[in]      DoListAll         List all from LFM to Turbo

**/
typedef struct {
  UINT16                    TurboBusRatio;
  UINT16                    MaxBusRatio;
  UINT16                    MinBusRatio;
  UINT32                    PackageMaxPower;
  UINT32                    PackageMinPower;
  GET_RELATIVE_POWER_FUNC   GetRelativePower;
  BOOLEAN                   DoListAll;
} PSS_PARAMS;

/**
  Fill the boot option list data with CFGDATA info

  @param[in, out]   OsBootOptionList pointer to boot option list.
 **/
VOID
EFIAPI
FillBootOptionListFromCfgData (
  IN OUT   OS_BOOT_OPTION_LIST   *OsBootOptionList
);


/**
  Set the platform name with CFGDATA info

 **/
VOID
EFIAPI
PlatformNameInit (
  VOID
);

/**
  Load the configuration data blob from SPI flash into destination buffer.
  It supports the sources: PDR, BIOS for external Cfgdata.

  @param[in]    Dst        Destination address to load configuration data blob.
  @param[in]    Src        Source address to load configuration data blob.
  @param[in]    Len        The destination address buffer size.

  @retval  EFI_SUCCESS             Configuration data blob was loaded successfully.
  @retval  EFI_NOT_FOUND           Configuration data blob cannot be found.
  @retval  EFI_OUT_OF_RESOURCES    Destination buffer is too small to hold the
                                   configuration data blob.
  @retval  Others                  Failed to load configuration data blob.

**/
EFI_STATUS
EFIAPI
SpiLoadExternalConfigData (
  IN UINT32  Dst,
  IN UINT32  Src,
  IN UINT32  Len
  );

/**
  Check state machine.

  This function will check state machine to see if capsule is pending

  @param[in]    pFwUpdStatus     Pointer to FW_UPDATE_STATUS structure.

  @retval  EFI_SUCCESS           State machine initialized in reserved region.
  @retval  EFI_UNSUPPORTED       Failure occured during state machine init.
  @retval  others                Error occured during state machine init.
**/
EFI_STATUS
CheckStateMachine (
  IN FW_UPDATE_STATUS    *pFwUpdStatus
  );

/**
  Patch ACPI CPU Pss Table

  This function will patch PSS table. Caller MUST guarantee valid table address.

  @param[in,out]  PssTableAddr      Pointer to PSS Table to be updated
  @param[in]      PssParams         All PSS calculation related info
  @retval         EFI_SUCCESS       Patch done successfully
  @retval         others            Error occured during patching the table

**/
EFI_STATUS
AcpiPatchPssTable (
  IN OUT        UINT8                    *PssTableAddr,
  IN      CONST PSS_PARAMS               *PssParams
  );

#endif
