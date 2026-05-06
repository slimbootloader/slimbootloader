/** @file
  GpioSiLib implementation for GNR-D platform

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsPcr.h>
#include "IblGpioTopologyGnrD.h"


//
// controller structure
//
GPIOV2_CONTROLLER   mGpioController[] = {
  {
    // GPIO in PCH
    .CommunityNum = sizeof (GnrSocCommunities) / sizeof (GPIOV2_COMMUNITY),
    .Communities  = GnrSocCommunities,
    .ChipsetId    = GPIOV2_IBL_CHIPSET_ID,
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

  for (Index = 0; Index < sizeof (mGpioController) / sizeof (GPIOV2_CONTROLLER); Index++) {
    if (mGpioController[Index].ChipsetId == ChipsetId) {
      return &mGpioController[Index];
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

  return GPIOV2_IBL_CHIPSET_ID;

}

