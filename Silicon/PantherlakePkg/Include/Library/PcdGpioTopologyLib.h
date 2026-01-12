/** @file
  Header file for common part for Gpio V2 Controller PEI and DXE modules

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _GPIOV2_TOPOLOGY_PTL_SOC_H_
#define _GPIOV2_TOPOLOGY_PTL_SOC_H_

#include <Library/GpioV2Lib.h>
#include <Library/GpioV2SiLib.h>

/**
  This procedure retrieves number of communities for PTL SOC.

  @retval  Number of communities on PTL SOC
**/
UINT32
PtlPcdGpioGetCommunitiesNum (
  UINT32  CommunityIndex
  );

/**
  This procedure retrieves pointer to array of communities for PTL SOC.

  @retval  pointer to array of communities on PTL SOC.
**/
GPIOV2_COMMUNITY *
PtlPcdGpioGetCommunities (
  UINT32  CommunityIndex
  );

/**
  This procedure retrieves Community PID based on community index.

  @param[in] CommunityIndex       Community Index

  @retval                         PID of requested community index.
**/
UINT32
PtlPcdGpioGetCommunityPid (
  IN UINT32 CommunityIndex
  );

/**
  This procedure retrieves register offset for given Gpio Pad

  @param[in]  GpioPad             Gpio Pad. Please refer to GpioPinsYYY.h - where YYY name of the platform (eg. MTL, EBG, ...)
  @param[in]  Register            Register for which user want to retrieve offset. Please refer to GpioV2Pad.h
  @param[out] RegisterOffset      Pointer to a buffer for register offset

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
PtlPcdGpioGetRegOffset (
  IN  GPIOV2_PAD       GpioPad,
  IN  GPIOV2_REGISTER  Register,
  OUT UINT32           *RegisterOffset
  );

GPIOV2_GROUP
PtlPcdGpioGetGroups(
  IN UINT32 CommunityIndex,
  IN UINT32 GroupIndex
);

EFI_STATUS
PtlPcdInstallCommunityAccess (
 OUT P2SB_SIDEBAND_REGISTER_ACCESS   *CommunityAccess
);
#endif //_GPIOV2_TOPOLOGY_PTL_SOC_H_
