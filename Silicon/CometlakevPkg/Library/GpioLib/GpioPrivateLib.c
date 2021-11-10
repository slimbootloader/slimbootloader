/** @file
  The platform GPIO private library code.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <GpioPinsCmlvLp.h>
#include <GpioPinsCmlvH.h>
#include "GpioInitLib.h"
#include <Library/GpioSiLib.h>
#include <Library/PchInfoLib.h>
#include <Library/GpioLib.h>
#include <GpioConfig.h>
#include <RegAccess.h>

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_TO_GPE_MAPPING mPchLpGpioGroupToGpeMapping[] = {
  {GPIO_CNL_LP_GROUP_GPP_A,  0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_A, V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_A},
  {GPIO_CNL_LP_GROUP_GPP_B,  0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_B, V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_B},
  {GPIO_CNL_LP_GROUP_GPP_C,  0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_C, V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_C},
  {GPIO_CNL_LP_GROUP_GPP_D,  0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_D, V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_D},
  {GPIO_CNL_LP_GROUP_GPP_E,  0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_E, V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_E},
  {GPIO_CNL_LP_GROUP_GPP_F,  0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_F, V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_F},
  {GPIO_CNL_LP_GROUP_GPP_G,  0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_G, V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_G},
  {GPIO_CNL_LP_GROUP_GPP_H,  0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_H, V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPP_H},
  {GPIO_CNL_LP_GROUP_GPD,    0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_GPD,   V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_GPD},
  {GPIO_CNL_LP_GROUP_VGPIO , 0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_VGPIO, V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_VGPIO},
  {GPIO_CNL_LP_GROUP_SPI,    0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_SPI,   V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_SPI},
  {GPIO_CNL_LP_GROUP_AZA,    0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_AZA,   V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_AZA},
  {GPIO_CNL_LP_GROUP_JTAG,   0, V_CNL_PCH_LP_PMC_PWRM_GPIO_CFG_JTAG,  V_CNL_PCH_LP_GPIO_PCR_MISCCFG_GPE0_JTAG}
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_TO_GPE_MAPPING mPchHGpioGroupToGpeMapping[] = {
  {GPIO_CNL_H_GROUP_GPP_A,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_A, V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_A},
  {GPIO_CNL_H_GROUP_GPP_B,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_B, V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_B},
  {GPIO_CNL_H_GROUP_GPP_C,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_C, V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_C},
  {GPIO_CNL_H_GROUP_GPP_D,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_D, V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_D},
  {GPIO_CNL_H_GROUP_GPP_E,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_E, V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_E},
  {GPIO_CNL_H_GROUP_GPP_F,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_F, V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_F},
  {GPIO_CNL_H_GROUP_GPP_G,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_G, V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_G},
  {GPIO_CNL_H_GROUP_GPP_H,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_H, V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_H},
  {GPIO_CNL_H_GROUP_GPP_I,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_I, V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_I},
  {GPIO_CNL_H_GROUP_GPD,    0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPD,   V_CMLV_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPD},
};

/**
  This procedure will get group index (0 based) from group

  @param[in] GpioGroup        Gpio Group

  @retval Value               Group Index
**/
UINT32
EFIAPI
GpioGetGroupIndexFromGroup (
  IN GPIO_GROUP        GpioGroup
  )
{
  return (UINT32) GPIO_GET_GROUP_INDEX (GpioGroup);
}

/**
  This procedure will get group index (0 based)

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group Index
**/
UINT32
EFIAPI
GpioGetGroupIndexFromGpioPad (
  IN GPIO_PAD        GpioPad
  )
{
  return (UINT32) GPIO_GET_GROUP_INDEX_FROM_PAD (GpioPad);
}

/**
  This procedure will get pad number (0 based) from Gpio Pad

  @param[in] GpioPad          Gpio Pad

  @retval Value               Pad Number
**/
UINT32
EFIAPI
GpioGetPadNumberFromGpioPad (
  IN GPIO_PAD        GpioPad
  )
{
  return (UINT32) GPIO_GET_PAD_NUMBER (GpioPad);
}

/**
  This procedure will get number of groups

  @param[in] none

  @retval Value               Group number
**/
UINT32
EFIAPI
GpioGetNumberOfGroups (
  VOID
  )
{
  UINT32                 GpioGroupInfoLength;

  GpioGetGroupInfoTable (&GpioGroupInfoLength);
  return GpioGroupInfoLength;
}

/**
  This procedure will get group number

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group number
**/
GPIO_GROUP
EFIAPI
GpioGetGroupFromGpioPad (
  IN GPIO_PAD         GpioPad
  )
{
  return GPIO_GET_GROUP_FROM_PAD (GpioPad);
}

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
EFIAPI
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
EFIAPI
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

/**
  Get information for GPIO Group required to program GPIO and PMC for desired 1-Tier GPE mapping

  @param[out] GpioGroupToGpeMapping        Table with GPIO Group to GPE mapping
  @param[out] GpioGroupToGpeMappingLength  GPIO Group to GPE mapping table length
**/
VOID
EFIAPI
GpioGetGroupToGpeMapping (
  OUT GPIO_GROUP_TO_GPE_MAPPING  **GpioGroupToGpeMapping,
  OUT UINT32                     *GpioGroupToGpeMappingLength
  )
{
  if (IsPchLp ()) {
    *GpioGroupToGpeMapping = mPchLpGpioGroupToGpeMapping;
    *GpioGroupToGpeMappingLength = ARRAY_SIZE (mPchLpGpioGroupToGpeMapping);
  } else {
    *GpioGroupToGpeMapping = mPchHGpioGroupToGpeMapping;
    *GpioGroupToGpeMappingLength = ARRAY_SIZE (mPchHGpioGroupToGpeMapping);
  }
}

/**
  Get GPIO Chipset ID specific to PCH generation and series
**/
UINT32
EFIAPI
GpioGetThisChipsetId (
  VOID
  )
{
  if (IsPchLp ()) {
    return GPIO_CNL_LP_CHIPSET_ID;
  } else {
    return GPIO_CNL_H_CHIPSET_ID;
  }
}

