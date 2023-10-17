
/** @file
  Header file for common part for GPIO V2 Controller

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _GPIOV2_CONTROLLER_INIT_IBL_H_
#define _GPIOV2_CONTROLLER_INIT_IBL_H_

#include <Library/GpioV2ControllerInterface.h>
#include <Library/P2SbController.h>

#define GPIO_NAME_LENGTH_MAX          32
#define MAX_GPIO_PINS                 100
#define GPIOV2_NAME_LENGTH_MAX        32
#define GPIOV2_CONTROLLER_HID_LENGTH  8

/**
  This procedure retrieves number of GPIO communities.

  @retval  Number of GPIO communities
**/
UINT32
GpioGetCommunitiesNum (
  VOID
  );

/**
  This procedure retrieves pointer to array of GPIO communities.

  @retval  pointer to array of GPIO communities
**/
GPIOV2_COMMUNITY *
GpioGetCommunities (
  IN UINT32 CommunityIndex
  );

/**
  This function returns whether given GPIO pad is a hard GPIO.
  It checks whether given GPIO pad does not belong to the virtual GPIO community.

  @param[in] GpioPad  Gpio pad
  @retval             TRUE if given pad is hard GPIO, FALSE otherwise
**/
BOOLEAN
IsHardGpio (
  GPIOV2_PAD  GpioPad
  );

GPIOV2_GROUP
SocGpioGetGroups (
  IN UINT32 CommunityIndex,
  IN UINT32 GroupIndex
  );

/**
  This procedure retrieves register offset

  @param[in] CommunityIndex      GPIO community group index
  @retval P2sb side band register base address
**/
P2SB_SIDEBAND_REGISTER_ACCESS
GetP2sbAddress (
 IN UINT32  CommunityIndex
);

EFI_STATUS
SocInstallCommunityAccess (
 OUT P2SB_SIDEBAND_REGISTER_ACCESS   *CommunityAccess
);

/**
  This procedure will confirm All pads in GpioPadsConfigTable belong to the same ChipsetId.

  @param[in] GpioPad     Gpio pad

  @retval Status
**/
EFI_STATUS
GetGpioV2ServicesFromPad (
  IN  GPIOV2_PAD                GpioPad
  );

/**
  This procedure retrieves ChipID for the platform.

  @retval  ChipId for the platform
**/
UINT32
GpioGetChipId ( VOID );

/**
  return support status for P2SB PCR 20-bit addressing

  @retval    TRUE
  @retval    FALSE
**/
BOOLEAN
IsP2sb20bPcrSupported (
  VOID
  );

/**
  Get P2SB instance for SOC

  @param[in, out] None

  @retval     P2SB_CONTROLLER           - P2SB_CONTROLLER Structure

**/
P2SB_CONTROLLER
SocGetP2SbController (
  VOID
  );

/**
  Get P2SB instance for SOC

  @param[in, out] None

  @retval     P2SB_CONTROLLER           - P2SB_CONTROLLER Structure

**/
UINTN
GetVgpioBaseAddress (
  VOID
  );
#endif //_GPIOV2_CONTROLLER_INIT_IBL_H_
