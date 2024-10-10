/** @file
  GpioV2SiLib implementation Arrow lake platform

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/GpioV2Lib.h>
#include <GpioV2Config.h>
#include <Register/GpioV2ChipsetId.h>
#include <Library/PchInfoLib.h>
#include <Register/PchRegsGpio.h>
#include <Register/PchRegsPcr.h>
#include "MtlSocGpioTopology.h"


//
// controller structure
//
GPIOV2_CONTROLLER   mGpipController[] = {
  {
    // GPIO in SOC-M
    .CommunityNum = sizeof (MtlSocMCommunities) / sizeof (GPIOV2_COMMUNITY),
    .Communities  = MtlSocMCommunities,
    .ChipsetId    = GPIOV2_MTL_SOC_M_CHIPSET_ID,
    .P2sbBase     = PCI_LIB_ADDRESS(0, 31, 1, 0),
    .SbRegBar     = PCH_PCR_BASE_ADDRESS
  }
};


/**
  This procedure retrieves pointer a P2SB controller where PAD belong to

  @param[in] GpioPad              GPIO PAD

  @retval                         A P2SB controller pointer
**/
GPIOV2_CONTROLLER *
EFIAPI
GpioGetController (
  IN GPIOV2_PAD            GpioPad
  )
{
  UINT16                   ChipsetId;
  UINTN                    Index;

  ChipsetId = GPIOV2_PAD_GET_CHIPSETID (GpioPad);

  for (Index = 0; Index < sizeof (mGpipController) / sizeof (GPIOV2_CONTROLLER); Index++) {
    if (mGpipController[Index].ChipsetId == ChipsetId) {
      return &mGpipController[Index];
    }
  }
  ASSERT (FALSE);
  return NULL;
}


/**
  Get GPIO Chipset ID of the GPIO controllers that are supported by current platform.
**/
UINT32
EFIAPI
GpioGetThisChipsetId (
  VOID
  )
{

  return GPIOV2_MTL_SOC_M_CHIPSET_ID;

}

