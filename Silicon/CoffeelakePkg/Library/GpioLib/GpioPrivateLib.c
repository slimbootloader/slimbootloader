/** @file
  The platform GPIO private library code.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <GpioPinsCnlLp.h>
#include <GpioPinsCnlH.h>
#include "GpioLibrary.h"
#include "GpioInitLib.h"

/**
  This procedure will return Port ID of GPIO Community from GpioPad

  @param[in] GpioPad            GpioPad

  @retval GpioCommunityPortId   Port ID of GPIO Community
**/
UINT8
GpioGetGpioCommunityPortIdFromGpioPad (
  IN GPIO_PAD        GpioPad
  )
{
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 GroupIndex;

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  return GpioGroupInfo[GroupIndex].Community;
}

/**
  This procedure will return PadCfg address from GpioPad

  @param[in] GpioPad            GpioPad

  @retval GpioPadCfgAddress     PadCfg Address of GpioPad
**/
UINT32
GpioGetGpioPadCfgAddressFromGpioPad (
  IN GPIO_PAD        GpioPad
  )
{
  UINT32                 PadCfgRegAddress;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Create Pad Configuration register offset
  //
  PadCfgRegAddress = GpioGroupInfo[GroupIndex].PadCfgOffset + S_GPIO_PCR_PADCFG * PadNumber;

  return PadCfgRegAddress;
}

/**
  This procedure is used to check if GpioPad is valid for certain chipset

  @param[in]  GpioPad             GPIO pad

  @retval TRUE                    This pin is valid on this chipset
          FALSE                   Incorrect pin
**/
BOOLEAN
GpioIsCorrectPadForThisChipset (
  IN  GPIO_PAD        GpioPad
  )
{
  return ((GPIO_GET_CHIPSET_ID (GpioPad) == GpioGetThisChipsetId ()) &&
         (GpioGetGroupIndexFromGpioPad (GpioPad) < GpioGetNumberOfGroups ()));
}

/**
  This procedure will check if GpioPad argument is valid.
  Function will check below conditions:
   - GpioPad represents a pad for current PCH
   - GpioPad belongs to valid GpioGroup
   - GPIO PadNumber is not greater than number of pads for this group

  @param[in] GpioPad       GPIO pad

  @retval TRUE             GPIO pad is valid and can be used with GPIO lib API
  @retval FALSE            GPIO pad is invalid and cannot be used with GPIO lib API
**/
BOOLEAN
GpioIsPadValid (
  IN GPIO_PAD             GpioPad
  )
{
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 PadNumber;

  if (!GpioIsCorrectPadForThisChipset (GpioPad)) {
    DEBUG ((DEBUG_ERROR, "GPIO ERROR: Incorrect GpioPad (0x%08x) used on this chipset!\n", GpioPad));
    goto Error;
  }

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Check if legal pin number
  //
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);
  if (PadNumber >= GpioGroupInfo[GpioGetGroupIndexFromGpioPad (GpioPad)].PadPerGroup) {
    DEBUG ((DEBUG_ERROR, "GPIO ERROR: Pin number (%d) exceeds possible range for this group\n", PadNumber));
    goto Error;
  }

  return TRUE;
Error:
  ASSERT (FALSE);
  return FALSE;
}

/**
  This procedure will check if GpioPad is owned by host.

  @param[in] GpioPad       GPIO pad

  @retval TRUE             GPIO pad is owned by host
  @retval FALSE            GPIO pad is not owned by host and should not be used with GPIO lib API
**/
BOOLEAN
GpioIsPadHostOwned (
  IN GPIO_PAD             GpioPad
  )
{
  GPIO_PAD_OWN         PadOwnVal;

  //
  // Check if selected GPIO Pad is not owned by CSME/ISH
  // If GPIO is not owned by Host all access to PadCfg will be dropped
  //
  GpioGetPadOwnership (GpioPad, &PadOwnVal);
  if (PadOwnVal != GpioPadOwnHost) {
    DEBUG ((DEBUG_ERROR, "GPIO ERROR: GpioPad is not owned by host!\n"));//, GpioName (GpioPad)));
    return FALSE;
  }

  return TRUE;
}

/**
  This procedure will read GPIO Pad Configuration register

  @param[in] GpioPad          GPIO pad
  @param[in] DwReg            Choose PADCFG register: 0:DW0, 1:DW1

  @retval PadCfgRegValue      PADCFG_DWx value
**/
UINT32
GpioReadPadCfgReg (
  IN GPIO_PAD             GpioPad,
  IN UINT8                DwReg
  )
{
  UINT32                 PadCfgReg;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Create Pad Configuration register offset
  //
  PadCfgReg = GpioGroupInfo[GroupIndex].PadCfgOffset + S_GPIO_PCR_PADCFG * PadNumber + 0x4 * DwReg;

  return MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, PadCfgReg));
}


