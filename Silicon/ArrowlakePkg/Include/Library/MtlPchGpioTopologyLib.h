
/** @file
  Header file for common part for Gpio V2 Controller PEI and DXE modules

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GPIOV2_TOPOLOGY_MTL_PCH_H_
#define _GPIOV2_TOPOLOGY_MTL_PCH_H_

//#include <Library/GpioV2AccessLib.h>
#include <GpioV2ControllerInterface.h>
#include <Include/GpioV2Pad.h>


/**
This procedure retrieves number of communities for PCH with index 'PchIndex'

@param[in] PchIndex             MTL SOC/PCH  index

@retval                         Number of communities on requested PCH
**/
UINT32
MtlPchGpioGetCommunitiesNum (
  IN UINT32 PchIndex
);

/**
This procedure retrieves pointer to array of communities for PCH with index 'PchIndex'

@param[in] PchIndex             MTL SOC/PCH index

@retval                         pointer to array of communities on requested PCH
**/
GPIOV2_COMMUNITY*
MtlPchGpioGetCommunities (
  IN UINT32 PchIndex
);

GPIOV2_GROUP
MtlPchGpioGetGroups (
  IN UINT32 CommunityIndex,
  IN UINT32 GroupIndex
);

/**
  This procedure retrieves number of native signals for LNL SOC

  @retval  Number of native signals on LNL SOC
**/
UINT32
MtlPchGpioGetNativeSignalsNum (
  VOID
  );

/**
  This procedure retrieves pointer to array of native signals for LNL SOC

  @retval  pointer to array of native signals on LNL SOC
**/
GPIOV2_NATIVE_SIGNAL_DATA *
MtlPchGpioGetNativeSignals (
  VOID
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
MtlPchGpioGetRegOffset (
  IN  GPIOV2_PAD          GpioPad,
  IN  GPIOV2_REGISTER     Register,
  OUT UINT32              *RegisterOffset
  );

EFI_STATUS
MtlPchInstallCommunityAccess (
 OUT P2SB_SIDEBAND_REGISTER_ACCESS   *CommunityAccess
);
#endif //_GPIOV2_TOPOLOGY_MTL_PCH_H_
