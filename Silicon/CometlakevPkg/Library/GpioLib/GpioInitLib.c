/** @file
  The platform GPIO init library.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <GpioPinsCmlvLp.h>
#include <GpioPinsCmlvH.h>
#include <RegAccess.h>
#include "GpioInitLib.h"

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO mPchLpGpioGroupInfo[] = {
  {PID_GPIOCOM0, R_CNL_PCH_LP_GPIO_PCR_GPP_A_PAD_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_A_HOSTSW_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_A_GPI_IS, R_CNL_PCH_LP_GPIO_PCR_GPP_A_GPI_IE, R_CNL_PCH_LP_GPIO_PCR_GPP_A_GPI_GPE_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_A_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           R_CNL_PCH_LP_GPIO_PCR_GPP_A_PADCFGLOCK,   R_CNL_PCH_LP_GPIO_PCR_GPP_A_PADCFGLOCKTX,   R_CNL_PCH_LP_GPIO_PCR_GPP_A_PADCFG_OFFSET, CNL_PCH_LP_GPIO_GPP_A_PAD_MAX}, //CNL PCH-LP GPP_A
  {PID_GPIOCOM0, R_CNL_PCH_LP_GPIO_PCR_GPP_B_PAD_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_B_HOSTSW_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_B_GPI_IS, R_CNL_PCH_LP_GPIO_PCR_GPP_B_GPI_IE, R_CNL_PCH_LP_GPIO_PCR_GPP_B_GPI_GPE_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_B_GPI_GPE_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_B_SMI_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_B_SMI_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_B_NMI_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_B_NMI_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_B_PADCFGLOCK,   R_CNL_PCH_LP_GPIO_PCR_GPP_B_PADCFGLOCKTX,   R_CNL_PCH_LP_GPIO_PCR_GPP_B_PADCFG_OFFSET, CNL_PCH_LP_GPIO_GPP_B_PAD_MAX}, //CNL PCH-LP GPP_B
  // Removed GPIO EN and Sts fields to stop auto wake and fix irq9 storm
  {PID_GPIOCOM1, R_CNL_PCH_LP_GPIO_PCR_GPP_C_PAD_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_C_HOSTSW_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_C_GPI_IS, R_CNL_PCH_LP_GPIO_PCR_GPP_C_GPI_IE, NO_REGISTER_FOR_PROPERTY, NO_REGISTER_FOR_PROPERTY, R_CNL_PCH_LP_GPIO_PCR_GPP_C_SMI_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_C_SMI_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_C_NMI_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_C_NMI_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_C_PADCFGLOCK,   R_CNL_PCH_LP_GPIO_PCR_GPP_C_PADCFGLOCKTX,   R_CNL_PCH_LP_GPIO_PCR_GPP_C_PADCFG_OFFSET, CNL_PCH_LP_GPIO_GPP_C_PAD_MAX}, //CNL PCH-LP GPP_C
  {PID_GPIOCOM1, R_CNL_PCH_LP_GPIO_PCR_GPP_D_PAD_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_D_HOSTSW_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_D_GPI_IS, R_CNL_PCH_LP_GPIO_PCR_GPP_D_GPI_IE, R_CNL_PCH_LP_GPIO_PCR_GPP_D_GPI_GPE_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_D_GPI_GPE_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_D_SMI_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_D_SMI_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_D_NMI_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_D_NMI_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_D_PADCFGLOCK,   R_CNL_PCH_LP_GPIO_PCR_GPP_D_PADCFGLOCKTX,   R_CNL_PCH_LP_GPIO_PCR_GPP_D_PADCFG_OFFSET, CNL_PCH_LP_GPIO_GPP_D_PAD_MAX}, //CNL PCH-LP GPP_D
  {PID_GPIOCOM1, R_CNL_PCH_LP_GPIO_PCR_GPP_E_PAD_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_E_HOSTSW_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_E_GPI_IS, R_CNL_PCH_LP_GPIO_PCR_GPP_E_GPI_IE, R_CNL_PCH_LP_GPIO_PCR_GPP_E_GPI_GPE_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_E_GPI_GPE_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_E_SMI_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_E_SMI_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_E_NMI_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_E_NMI_EN, R_CNL_PCH_LP_GPIO_PCR_GPP_E_PADCFGLOCK,   R_CNL_PCH_LP_GPIO_PCR_GPP_E_PADCFGLOCKTX,   R_CNL_PCH_LP_GPIO_PCR_GPP_E_PADCFG_OFFSET, CNL_PCH_LP_GPIO_GPP_E_PAD_MAX}, //CNL PCH-LP GPP_E
  {PID_GPIOCOM1, R_CNL_PCH_LP_GPIO_PCR_GPP_F_PAD_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_F_HOSTSW_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_F_GPI_IS, R_CNL_PCH_LP_GPIO_PCR_GPP_F_GPI_IE, R_CNL_PCH_LP_GPIO_PCR_GPP_F_GPI_GPE_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_F_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           R_CNL_PCH_LP_GPIO_PCR_GPP_F_PADCFGLOCK,   R_CNL_PCH_LP_GPIO_PCR_GPP_F_PADCFGLOCKTX,   R_CNL_PCH_LP_GPIO_PCR_GPP_F_PADCFG_OFFSET, CNL_PCH_LP_GPIO_GPP_F_PAD_MAX}, //CNL PCH-LP GPP_F
  {PID_GPIOCOM1, R_CNL_PCH_LP_GPIO_PCR_GPP_G_PAD_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_G_HOSTSW_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_G_GPI_IS, R_CNL_PCH_LP_GPIO_PCR_GPP_G_GPI_IE, R_CNL_PCH_LP_GPIO_PCR_GPP_G_GPI_GPE_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_G_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           R_CNL_PCH_LP_GPIO_PCR_GPP_G_PADCFGLOCK,   R_CNL_PCH_LP_GPIO_PCR_GPP_G_PADCFGLOCKTX,   R_CNL_PCH_LP_GPIO_PCR_GPP_G_PADCFG_OFFSET, CNL_PCH_LP_GPIO_GPP_G_PAD_MAX}, //CNL PCH-LP GPP_G
  {PID_GPIOCOM1, R_CNL_PCH_LP_GPIO_PCR_GPP_H_PAD_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_H_HOSTSW_OWN, R_CNL_PCH_LP_GPIO_PCR_GPP_H_GPI_IS, R_CNL_PCH_LP_GPIO_PCR_GPP_H_GPI_IE, R_CNL_PCH_LP_GPIO_PCR_GPP_H_GPI_GPE_STS, R_CNL_PCH_LP_GPIO_PCR_GPP_H_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           R_CNL_PCH_LP_GPIO_PCR_GPP_H_PADCFGLOCK,   R_CNL_PCH_LP_GPIO_PCR_GPP_H_PADCFGLOCKTX,   R_CNL_PCH_LP_GPIO_PCR_GPP_H_PADCFG_OFFSET, CNL_PCH_LP_GPIO_GPP_H_PAD_MAX}, //CNL PCH-LP GPP_H
  {PID_GPIOCOM3, R_CNL_PCH_H_GPIO_PCR_GPP_I_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_I_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_I_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_I_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_I_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_I_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_I_PAD_MAX}, //CNL PCH-H GPP_I
  {PID_GPIOCOM2, R_CNL_PCH_LP_GPIO_PCR_GPD_PAD_OWN,   R_CNL_PCH_LP_GPIO_PCR_GPD_HOSTSW_OWN,   R_CNL_PCH_LP_GPIO_PCR_GPD_GPI_IS,   R_CNL_PCH_LP_GPIO_PCR_GPD_GPI_IE,   R_CNL_PCH_LP_GPIO_PCR_GPD_GPI_GPE_STS,   R_CNL_PCH_LP_GPIO_PCR_GPD_GPI_GPE_EN,   NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           NO_REGISTER_FOR_PROPERTY,            NO_REGISTER_FOR_PROPERTY,           R_CNL_PCH_LP_GPIO_PCR_GPD_PADCFGLOCK,     R_CNL_PCH_LP_GPIO_PCR_GPD_PADCFGLOCKTX,     R_CNL_PCH_LP_GPIO_PCR_GPD_PADCFG_OFFSET,   CNL_PCH_LP_GPIO_GPD_PAD_MAX},   //CNL PCH-LP GPD
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO mPchHGpioGroupInfo[] = {
  {PID_GPIOCOM0, R_CNL_PCH_H_GPIO_PCR_GPP_A_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_A_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_A_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_A_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_A_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_A_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_A_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_A_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_A_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_A_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_A_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_A_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_A_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_A_PAD_MAX}, //CNL PCH-H GPP_A
  {PID_GPIOCOM0, R_CNL_PCH_H_GPIO_PCR_GPP_B_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_B_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_B_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_B_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_B_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_B_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_B_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_B_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_B_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_B_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_B_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_B_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_B_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_B_PAD_MAX}, //CNL PCH-H GPP_B
  {PID_GPIOCOM1, R_CNL_PCH_H_GPIO_PCR_GPP_C_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_C_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_C_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_C_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_C_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_C_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_C_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_C_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_C_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_C_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_C_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_C_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_C_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_C_PAD_MAX}, //CNL PCH-H GPP_C
  {PID_GPIOCOM1, R_CNL_PCH_H_GPIO_PCR_GPP_D_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_D_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_D_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_D_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_D_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_D_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_D_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_D_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_D_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_D_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_D_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_D_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_D_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_D_PAD_MAX}, //CNL PCH-H GPP_D
  {PID_GPIOCOM1, R_CNL_PCH_H_GPIO_PCR_GPP_E_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_E_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_E_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_E_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_E_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_E_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_E_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_E_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_E_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_E_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_E_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_E_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_E_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_E_PAD_MAX}, //CNL PCH-H GPP_E
  {PID_GPIOCOM1, R_CNL_PCH_H_GPIO_PCR_GPP_F_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_F_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_F_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_F_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_F_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_F_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_F_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_F_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_F_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_F_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_F_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_F_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_F_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_F_PAD_MAX}, //CNL PCH-H GPP_F
  {PID_GPIOCOM1, R_CNL_PCH_H_GPIO_PCR_GPP_G_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_G_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_G_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_G_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_G_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_G_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_G_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_G_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_G_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_G_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_G_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_G_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_G_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_G_PAD_MAX}, //CNL PCH-H GPP_G
  {PID_GPIOCOM1, R_CNL_PCH_H_GPIO_PCR_GPP_H_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_H_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_H_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_H_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_H_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_H_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_H_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_H_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_H_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_H_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_H_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_H_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_H_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_H_PAD_MAX}, //CNL PCH-H GPP_H
  {PID_GPIOCOM3, R_CNL_PCH_H_GPIO_PCR_GPP_I_PAD_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_I_HOSTSW_OWN, R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_IS, R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_IE, R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_GPE_STS, R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_GPE_EN, R_CNL_PCH_H_GPIO_PCR_GPP_I_SMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_I_SMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_I_NMI_STS, R_CNL_PCH_H_GPIO_PCR_GPP_I_NMI_EN, R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFGLOCK,   R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFGLOCKTX,   R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFG_OFFSET, CNL_PCH_H_GPIO_GPP_I_PAD_MAX}, //CNL PCH-H GPP_I
  {PID_GPIOCOM2, R_CNL_PCH_H_GPIO_PCR_GPD_PAD_OWN,   R_CNL_PCH_H_GPIO_PCR_GPD_HOSTSW_OWN,   R_CNL_PCH_H_GPIO_PCR_GPD_GPI_IS,   R_CNL_PCH_H_GPIO_PCR_GPD_GPI_IE,   R_CNL_PCH_H_GPIO_PCR_GPD_GPI_GPE_STS,   R_CNL_PCH_H_GPIO_PCR_GPD_GPI_GPE_EN,   NO_REGISTER_FOR_PROPERTY,           NO_REGISTER_FOR_PROPERTY,          NO_REGISTER_FOR_PROPERTY,           NO_REGISTER_FOR_PROPERTY,          R_CNL_PCH_H_GPIO_PCR_GPD_PADCFGLOCK,     R_CNL_PCH_H_GPIO_PCR_GPD_PADCFGLOCKTX,     R_CNL_PCH_H_GPIO_PCR_GPD_PADCFG_OFFSET,   CNL_PCH_H_GPIO_GPD_PAD_MAX},   //CNL PCH-H GPD
};

/**
  This procedure will retrieve address and length of GPIO info table

  @param[out]  GpioGroupInfoTableLength   Length of GPIO group table

  @retval Pointer to GPIO group table

**/
CONST GPIO_GROUP_INFO*
EFIAPI
GpioGetGroupInfoTable (
  OUT UINT32              *GpioGroupInfoTableLength
  )
{
  if (IsPchLp ()) {
    *GpioGroupInfoTableLength = ARRAY_SIZE (mPchLpGpioGroupInfo);
    return mPchLpGpioGroupInfo;
  } else {
    *GpioGroupInfoTableLength = ARRAY_SIZE (mPchHGpioGroupInfo);
    return mPchHGpioGroupInfo;
  }
}

/**
  This internal procedure will check if group is within DeepSleepWell.

  @param[in]  Group               GPIO Group

  @retval GroupWell               TRUE:  This is DSW Group
                                  FALSE: This is not DSW Group
**/
BOOLEAN
EFIAPI
GpioIsDswGroup (
  IN  GPIO_GROUP         Group
  )
{
  if ((Group == GPIO_CNL_LP_GROUP_GPD) || (Group == GPIO_CNL_H_GROUP_GPD)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  This internal procedure will calculate PadRstCfg register value based
  on provided GPIO Reset configuration for a certain pad.

  @param[in]  GpioPad                   GPIO Pad
  @param[in]  GpioResetConfig           GPIO Reset configuration
  @param[out] PadRstCfg                 GPIO PadRstCfg value

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid configuration
**/
EFI_STATUS
EFIAPI
GpioPadRstCfgFromResetConfig (
  IN  GPIO_PAD           GpioPad,
  IN  GPIO_RESET_CONFIG  GpioResetConfig,
  OUT UINT32             *PadRstCfg
  )
{
  GPIO_GROUP           Group;

  Group = GPIO_GET_GROUP_FROM_PAD (GpioPad);

  switch (GpioResetConfig) {
    case GpioResetDefault:
      *PadRstCfg = 0x0;
      break;
    case GpioHostDeepReset:
      *PadRstCfg = V_GPIO_PCR_RST_CONF_DEEP_RST;
      break;
    case GpioPlatformReset:
      *PadRstCfg = V_GPIO_PCR_RST_CONF_GPIO_RST;
      break;
    case GpioResumeReset:
      if (GpioIsDswGroup (Group)) {
        *PadRstCfg = V_GPIO_PCR_RST_CONF_RESUME_RST;
      } else {
        *PadRstCfg = V_GPIO_PCR_RST_CONF_POW_GOOD;
      }
      break;
    case GpioDswReset:
      if (GpioIsDswGroup (Group)) {
        *PadRstCfg = V_GPIO_PCR_RST_CONF_POW_GOOD;
      } else {
        DEBUG ((DEBUG_ERROR, "GPIO ERROR: Only GPD group pads can use GpioDswReset\n"));
        goto Error;
      }
      break;
    default:
      goto Error;
  }

  return EFI_SUCCESS;
Error:
  ASSERT (FALSE);
  return EFI_INVALID_PARAMETER;
}

/**
  This procedure will calculate PADCFG register value based on GpioConfig data

  @param[in]  GpioPad                   GPIO Pad
  @param[in]  GpioConfig                GPIO Configuration data
  @param[out] PadCfgDwReg               PADCFG DWx register value
  @param[out] PadCfgDwRegMask           Mask with PADCFG DWx register bits to be modified
**/
EFI_STATUS
EFIAPI
GpioPadCfgRegValueFromGpioConfig (
  IN  GPIO_PAD           GpioPad,
  IN  CONST GPIO_CONFIG  *GpioConfig,
  OUT UINT32             *PadCfgDwReg,
  OUT UINT32             *PadCfgDwRegMask
  )
{
  UINT32               PadRstCfg;
  EFI_STATUS           Status;

  //
  // Configure Reset Type (PadRstCfg)
  // Reset configuration depends on group type.
  // This field requires support for new and deprecated settings.
  //
  Status = GpioPadRstCfgFromResetConfig (
             GpioPad,
             GpioConfig->PowerConfig,
             &PadRstCfg
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PadCfgDwRegMask[0] |= ((((GpioConfig->PowerConfig & B_GPIO_RESET_CONFIG_RESET_MASK) >> N_GPIO_RESET_CONFIG_RESET_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_RST_CONF);
  PadCfgDwReg[0] |= PadRstCfg << N_GPIO_PCR_RST_CONF;

  //
  // Configure how interrupt is triggered (RxEvCfg)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->InterruptConfig & B_GPIO_INT_CONFIG_INT_TYPE_MASK) >> N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_RX_LVL_EDG);
  PadCfgDwReg[0] |= (((GpioConfig->InterruptConfig & B_GPIO_INT_CONFIG_INT_TYPE_MASK) >> (N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS + 1)) << N_GPIO_PCR_RX_LVL_EDG);

  //
  // Configure interrupt generation (GPIRoutIOxAPIC/SCI/SMI/NMI)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->InterruptConfig & B_GPIO_INT_CONFIG_INT_SOURCE_MASK) >> N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS) == GpioHardwareDefault)  ? 0x0 : (B_GPIO_PCR_RX_NMI_ROUTE | B_GPIO_PCR_RX_SCI_ROUTE | B_GPIO_PCR_RX_SMI_ROUTE | B_GPIO_PCR_RX_APIC_ROUTE));
  PadCfgDwReg[0] |= (((GpioConfig->InterruptConfig & B_GPIO_INT_CONFIG_INT_SOURCE_MASK) >> (N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS + 1)) << N_GPIO_PCR_RX_NMI_ROUTE);

  //
  // Configure GPIO direction (GPIORxDis and GPIOTxDis)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->Direction & B_GPIO_DIRECTION_DIR_MASK) >> N_GPIO_DIRECTION_DIR_BIT_POS) == GpioHardwareDefault) ? 0x0 : (B_GPIO_PCR_RXDIS | B_GPIO_PCR_TXDIS));
  PadCfgDwReg[0] |= (((GpioConfig->Direction & B_GPIO_DIRECTION_DIR_MASK) >> (N_GPIO_DIRECTION_DIR_BIT_POS + 1)) << N_GPIO_PCR_TXDIS);

  //
  // Configure GPIO input inversion (RXINV)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->Direction & B_GPIO_DIRECTION_INV_MASK) >> N_GPIO_DIRECTION_INV_BIT_POS) == GpioHardwareDefault) ?  0x0 : B_GPIO_PCR_RXINV);
  PadCfgDwReg[0] |= (((GpioConfig->Direction & B_GPIO_DIRECTION_INV_MASK) >> (N_GPIO_DIRECTION_INV_BIT_POS + 1)) << N_GPIO_PCR_RXINV);

  //
  // Configure GPIO output state (GPIOTxState)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->OutputState & B_GPIO_OUTPUT_MASK) >> N_GPIO_OUTPUT_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_TX_STATE);
  PadCfgDwReg[0] |= (((GpioConfig->OutputState & B_GPIO_OUTPUT_MASK) >> (N_GPIO_OUTPUT_BIT_POS + 1)) << N_GPIO_PCR_TX_STATE);

  //
  // Configure GPIO RX raw override to '1' (RXRAW1)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->OtherSettings & B_GPIO_OTHER_CONFIG_RXRAW_MASK) >> N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_RX_RAW1);
  PadCfgDwReg[0] |= (((GpioConfig->OtherSettings & B_GPIO_OTHER_CONFIG_RXRAW_MASK) >> (N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS + 1)) << N_GPIO_PCR_RX_RAW1);

  //
  // Configure GPIO Pad Mode (PMode)
  //
  PadCfgDwRegMask[0] |= ((((GpioConfig->PadMode & B_GPIO_PAD_MODE_MASK) >> N_GPIO_PAD_MODE_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_PAD_MODE);
  PadCfgDwReg[0] |= (((GpioConfig->PadMode & B_GPIO_PAD_MODE_MASK) >> (N_GPIO_PAD_MODE_BIT_POS + 1)) << N_GPIO_PCR_PAD_MODE);

  //
  // Configure GPIO termination (Term)
  //
  PadCfgDwRegMask[1] |= ((((GpioConfig->ElectricalConfig & B_GPIO_ELECTRICAL_CONFIG_TERMINATION_MASK) >> N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_GPIO_PCR_TERM);
  PadCfgDwReg[1] |= (((GpioConfig->ElectricalConfig & B_GPIO_ELECTRICAL_CONFIG_TERMINATION_MASK) >> (N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS + 1)) << N_GPIO_PCR_TERM);

  return EFI_SUCCESS;
}

/**
  This procedure will read GPIO register

  @param[in] RegType              GPIO register type
  @param[in] Group                GPIO group
  @param[in] DwNum                Register number for current group (parameter applicable in accessing whole register).
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[out] ReadVal             Read data
**/
VOID
GpioReadReg (
  IN GPIO_REG               RegType,
  IN GPIO_GROUP             Group,
  IN UINT32                 DwNum,
  OUT UINT32                *ReadVal
  )
{
  UINT32                 RegOffset;
  UINT32                 GroupIndex;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;

  RegOffset = NO_REGISTER_FOR_PROPERTY;
  GroupIndex = (UINT32) GPIO_GET_GROUP_INDEX (Group);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  switch (RegType) {
    case GpioHostOwnershipRegister:
      RegOffset = GpioGroupInfo[GroupIndex].HostOwnOffset;
      break;
    case GpioGpeEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].GpiGpeEnOffset;
      break;
    case GpioGpeStatusRegister:
      RegOffset = GpioGroupInfo[GroupIndex].GpiGpeStsOffset;
      break;
    case GpioSmiEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].SmiEnOffset;
      break;
    case GpioSmiStatusRegister:
      RegOffset = GpioGroupInfo[GroupIndex].SmiStsOffset;
      break;
    case GpioNmiEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].NmiEnOffset;
      break;
    case GpioPadConfigLockRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockOffset;
      break;
    case GpioPadLockOutputRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockTxOffset;
      break;
    default:
      break;
  }

  //
  // Check if selected register exists
  //
  if (RegOffset == NO_REGISTER_FOR_PROPERTY) {
    *ReadVal = 0;
    ASSERT (FALSE);
    return;
  }

  //
  // If there are more then 32 pads per group then certain
  // group information would be split into more then one DWord register.
  //
  if ((RegType == GpioPadConfigLockRegister) || (RegType == GpioPadLockOutputRegister)) {
    //
    // PadConfigLock and OutputLock registers when used for group containing more than 32 pads
    // are not placed in a continuous way, e.g:
    // 0x0 - PadConfigLock_DW0
    // 0x4 - OutputLock_DW0
    // 0x8 - PadConfigLock_DW1
    // 0xC - OutputLock_DW1
    //
    RegOffset += DwNum * 0x8;
  } else {
    RegOffset += DwNum * 0x4;
  }

  *ReadVal = MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, RegOffset));
}


/**
  This procedure will check state of Pad Config Lock for pads within one group

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLock register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[out] PadCfgLockRegVal    Value of PadCfgLock register
                                  Bit position - PadNumber
                                  Bit value - 0: NotLocked, 1: Locked

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or DwNum parameter number
**/
EFI_STATUS
EFIAPI
GpioGetPadCfgLockForGroupDw (
  IN  GPIO_GROUP                  Group,
  IN  UINT32                      DwNum,
  OUT UINT32                      *PadCfgLockRegVal
  )
{
  GpioReadReg (
    GpioPadConfigLockRegister,
    Group,
    DwNum,
    PadCfgLockRegVal
    );

  return EFI_SUCCESS;
}

/**
  This procedure will check state of Pad Config Tx Lock for pads within one group

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLockTx register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[out] PadCfgLockTxRegVal  Value of PadCfgLockTx register
                                  Bit position - PadNumber
                                  Bit value - 0: NotLockedTx, 1: LockedTx

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or DwNum parameter number
**/
EFI_STATUS
EFIAPI
GpioGetPadCfgLockTxForGroupDw (
  IN  GPIO_GROUP                  Group,
  IN  UINT32                      DwNum,
  OUT UINT32                      *PadCfgLockTxRegVal
  )
{
  GpioReadReg (
    GpioPadLockOutputRegister,
    Group,
    DwNum,
    PadCfgLockTxRegVal
    );

  return EFI_SUCCESS;
}

/**
  This procedure will write GPIO Lock/LockTx register using SBI.

  @param[in] RegType              GPIO register (Lock or LockTx)
  @param[in] Group                GPIO group number
  @param[in] DwNum                Register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in] LockRegAndMask       Mask which will be AND'ed with Lock register value
  @param[in] LockRegOrMask        Mask which will be Or'ed with Lock register value

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_UNSUPPORTED         Feature is not supported for this group or pad
**/
STATIC
EFI_STATUS
GpioWriteLockReg (
  IN GPIO_REG                  RegType,
  IN GPIO_GROUP                Group,
  IN UINT32                    DwNum,
  IN UINT32                    LockRegAndMask,
  IN UINT32                    LockRegOrMask
  )
{
  UINT8                  Response;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 RegOffset;
  UINT32                 OldLockVal;
  UINT32                 NewLockVal;
  UINT32                 GroupIndex;
  EFI_STATUS             Status;

  OldLockVal = 0;
  NewLockVal = 0;

  RegOffset = NO_REGISTER_FOR_PROPERTY;
  GroupIndex = (UINT32) GPIO_GET_GROUP_INDEX (Group);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  switch (RegType) {
    case GpioPadConfigLockRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockOffset;
      GpioGetPadCfgLockForGroupDw (Group, DwNum, &OldLockVal);
      break;
    case GpioPadLockOutputRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockTxOffset;
      GpioGetPadCfgLockTxForGroupDw (Group, DwNum, &OldLockVal);
      break;
    default:
      break;
  }

  //
  // Check if selected register exists
  //
  if (RegOffset == NO_REGISTER_FOR_PROPERTY) {
    return EFI_UNSUPPORTED;
  }

  //
  // If there are more then 32 pads per group then certain
  // group information would be split into more then one DWord register.
  // PadConfigLock and OutputLock registers when used for group containing more than 32 pads
  // are not placed in a continuous way, e.g:
  // 0x0 - PadConfigLock_DW0
  // 0x4 - OutputLock_DW0
  // 0x8 - PadConfigLock_DW1
  // 0xC - OutputLock_DW1
  //
  RegOffset += DwNum *0x8;

  NewLockVal = (OldLockVal & LockRegAndMask) | LockRegOrMask;

  Status = PchSbiExecutionEx (
             GpioGroupInfo[GroupIndex].Community,
             RegOffset,
             GpioLockUnlock,
             FALSE,
             0x000F,
             0x0000,
             0x0000,
             &NewLockVal,
             &Response
             );
  ASSERT_EFI_ERROR (Status);
  return Status;
}


/**
  This procedure will clear PadCfgLock for selected pads within one group.
  This function should be used only inside SMI.

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLock register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToUnlock        Bitmask for pads which are going to be unlocked,
                                  Bit position - PadNumber
                                  Bit value - 0: DoNotUnlock, 1: Unlock

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioUnlockPadCfgForGroupDw (
  IN GPIO_GROUP                Group,
  IN UINT32                    DwNum,
  IN UINT32                    PadsToUnlock
  )
{
  return GpioWriteLockReg (
           GpioPadConfigLockRegister,
           Group,
           DwNum,
           ~PadsToUnlock,
           0
           );
}


/**
  This procedure will clear PadCfgLockTx for selected pads within one group.
  This function should be used only inside SMI.

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLockTx register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToUnlockTx      Bitmask for pads which are going to be unlocked,
                                  Bit position - PadNumber
                                  Bit value - 0: DoNotUnLockTx, 1: LockTx

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioUnlockPadCfgTxForGroupDw (
  IN GPIO_GROUP                Group,
  IN UINT32                    DwNum,
  IN UINT32                    PadsToUnlockTx
  )
{
  return GpioWriteLockReg (
           GpioPadLockOutputRegister,
           Group,
           DwNum,
           ~PadsToUnlockTx,
           0
           );
}
