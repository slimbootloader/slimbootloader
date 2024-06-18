
/** @file
Header file for common part for Gpio V2 Controller PEI and DXE modules

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

@par Specification Reference:
**/

#ifndef _GPIOV2_CONTROLLER_INIT_MTL_H_
#define _GPIOV2_CONTROLLER_INIT_MTL_H_

#include <Include/GpioV2ControllerInterface.h>
#include <Include/GpioV2Pad.h>
#include <Uefi/UefiBaseType.h>
#include <Include/P2SbController.h>
#include <Include/P2SbSidebandAccessLib.h>


/**
This procedure retrieves number of communities for PCH with index 'PchIndex'

@param[in] PchIndex             MTL SOC/PCH  index

@retval                         Number of communities on requested PCH
**/
UINT32
MtlSocGpioGetCommunitiesNum(
  IN UINT32 PchIndex
);

/**
This procedure retrieves pointer to array of communities for PCH with index 'PchIndex'

@param[in] PchIndex             MTL SOC/PCH index

@retval                         pointer to array of communities on requested PCH
**/
GPIOV2_COMMUNITY*
MtlSocGpioGetCommunities(
  IN UINT32 PchIndex
);

GPIOV2_COMMUNITY*
MtlSocGpioGetCommunity (
    IN UINT32 CommunityIndex
);

GPIOV2_GROUP
MtlSocGpioGetGroups (
  IN UINT32 CommunityIndex,
  IN UINT32 GroupIndex
);

EFI_STATUS
MtlSocGetP2SbController (
  IN OUT P2SB_CONTROLLER         *P2SbController
);

EFI_STATUS
MtlSocInstallCommunityAccess (
 //IN UINT32            CommunityIndex,
 OUT P2SB_SIDEBAND_REGISTER_ACCESS   *CommunityAccess
);

/* GPIOV2_UNLOCK_GROUP_DATA*
MtlSocGpioUnlockData (
  IN UINT32 CommunityIndex
); */

/* P2SB_SIDEBAND_REGISTER_ACCESS
MtlSocGetCommunityAccess (
 IN UINT32            CommunityIndex
); */

/**
  This procedure retrieves register offset for given Gpio Pad

  @param[in]  GpioPad             Gpio Pad. Please refer to GpioPinsYYY.h - where YYY name of the platform (eg. MTL, EBG, ...)
  @param[in]  Register            Register for which user want to retrieve offset. Please refer to GpioV2Pad.h
  @param[out] RegisterOffset      Pointer to a buffer for register offset

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
MtlSocGpioGetRegOffset (
  IN  GPIOV2_PAD          GpioPad,
  IN  GPIOV2_REGISTER     Register,
  OUT UINT32              *RegisterOffset
  );
#endif //_GPIOV2_CONTROLLER_INIT_MTL_H_
