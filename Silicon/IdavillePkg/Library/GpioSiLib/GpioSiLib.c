/** @file
  GpioSiLib implementation for ICX-D platform

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/GpioSiLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsPmcCdf.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsGpioCdf.h>
#include <Register/PchRegsGpio.h>
#include <GpioPinsCdf.h>
#include <Library/ConfigDataLib.h>

#define GPIO_PAD_DEF(Group,Pad)                (UINT32)(((Group) << 16) + (Pad))
#define GPIO_GROUP_DEF(GroupIndex,ChipsetId)   ((GroupIndex) | ((ChipsetId) << 8))
#define GPIO_GET_GROUP_INDEX(Group)            ((Group) & 0x1F)
#define GPIO_GET_GROUP_FROM_PAD(GpioPad)       (((GpioPad) & 0x0F1F0000) >> 16)
#define GPIO_GET_GROUP_INDEX_FROM_PAD(GpioPad) GPIO_GET_GROUP_INDEX (GPIO_GET_GROUP_FROM_PAD(GpioPad))
#define GPIO_GET_PAD_NUMBER(GpioPad)           ((GpioPad) & 0x1FF)
#define GPIO_GET_CHIPSET_ID(GpioPad)           (((GpioPad) >> 24) & 0xF)

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO mCdfPchHGpioGroupInfo[] = {
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_A_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_A_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_A_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_A_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_A_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_A_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_A_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_A_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_A_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_A_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_A_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_A_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_A_PADCFG_OFFSET, V_PCH_GPIO_GPP_A_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_B_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_B_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_B_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_B_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_B_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_B_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_B_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_B_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_B_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_B_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_B_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_B_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_B_PADCFG_OFFSET, V_PCH_GPIO_GPP_B_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_C_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_C_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_C_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_C_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_C_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_C_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_C_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_C_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_C_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_C_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_C_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_C_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_C_PADCFG_OFFSET, V_PCH_GPIO_GPP_C_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_D_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_D_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_D_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_D_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_D_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_D_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_D_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_D_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_D_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_D_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_D_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_D_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_D_PADCFG_OFFSET, V_PCH_GPIO_GPP_D_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_E_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_E_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_E_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_E_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_E_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_E_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_E_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_E_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_E_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_E_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_E_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_E_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_E_PADCFG_OFFSET, V_PCH_GPIO_GPP_E_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_F_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_F_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_F_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_F_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_F_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_F_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_F_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_F_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_F_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_F_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_F_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_F_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_F_PADCFG_OFFSET, V_PCH_GPIO_GPP_F_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_G_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_G_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_G_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_G_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_G_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_G_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_G_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_G_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_G_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_G_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_G_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_G_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_G_PADCFG_OFFSET, V_PCH_GPIO_GPP_G_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_H_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_H_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_H_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_H_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_H_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_H_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_H_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_H_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_H_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_H_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_H_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_H_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_H_PADCFG_OFFSET, V_PCH_GPIO_GPP_H_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_I_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_I_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_I_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_I_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_I_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_I_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_I_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_I_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_I_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_I_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_I_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_I_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_I_PADCFG_OFFSET, V_PCH_GPIO_GPP_I_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_J_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_J_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_J_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_J_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_J_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_J_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_J_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_J_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_J_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_J_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_J_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_J_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_J_PADCFG_OFFSET, V_PCH_GPIO_GPP_J_PAD_MAX},
  {PID_GPIOCOM1, R_CDF_PCH_GPIO_PCR_GPP_K_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_K_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_K_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_K_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_K_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_K_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_K_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_K_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_K_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_K_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_K_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_K_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_K_PADCFG_OFFSET, V_PCH_GPIO_GPP_K_PAD_MAX},
  {PID_GPIOCOM0, R_CDF_PCH_GPIO_PCR_GPP_L_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_L_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_L_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_L_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_L_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_L_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_L_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_L_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_L_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_L_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_L_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_L_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_L_PADCFG_OFFSET, V_PCH_GPIO_GPP_L_PAD_MAX},
  {PID_GPIOCOM0, R_CDF_PCH_GPIO_PCR_GPP_M_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_M_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_M_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_M_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_M_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_M_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_M_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_M_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_M_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_M_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_M_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_M_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_M_PADCFG_OFFSET, V_PCH_GPIO_GPP_M_PAD_MAX},
  {PID_GPIOCOM0, R_CDF_PCH_GPIO_PCR_GPP_N_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_N_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_N_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_N_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_N_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_N_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_N_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_N_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_N_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_N_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_N_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_N_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_N_PADCFG_OFFSET, V_PCH_GPIO_GPP_N_PAD_MAX},
  {PID_GPIOCOM0, R_CDF_PCH_GPIO_PCR_GPP_O_PAD_OWN, R_CDF_PCH_GPIO_PCR_GPP_O_HOSTSW_OWN, R_CDF_PCH_GPIO_PCR_GPP_O_GPI_IS, R_CDF_PCH_GPIO_PCR_GPP_O_GPI_IE, R_CDF_PCH_GPIO_PCR_GPP_O_GPI_GPE_STS, R_CDF_PCH_GPIO_PCR_GPP_O_GPI_GPE_EN, R_CDF_PCH_GPIO_PCR_GPP_O_SMI_STS, R_CDF_PCH_GPIO_PCR_GPP_O_SMI_EN, R_CDF_PCH_GPIO_PCR_GPP_O_NMI_STS, R_CDF_PCH_GPIO_PCR_GPP_O_NMI_EN, R_CDF_PCH_GPIO_PCR_GPP_O_PADCFGLOCK, R_CDF_PCH_GPIO_PCR_GPP_O_PADCFGLOCKTX, V_CDF_PCH_GPIO_PCR_GPP_O_PADCFG_OFFSET, V_PCH_GPIO_GPP_O_PAD_MAX},
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_TO_GPE_MAPPING mCdfPchGpioGroupToGpeMapping[] = {
  {GPIO_CDF_GROUP_GPP_A,  0, V_CNL_PCH_H_PMC_PWRM_GPIO_CFG_GPP_A, V_CNL_PCH_H_GPIO_PCR_MISCCFG_GPE0_GPP_A},
};

GLOBAL_REMOVE_IF_UNREFERENCED PCH_SBI_PID mGpioComSbiIds []=
{
  PID_GPIOCOM0, PID_GPIOCOM1
};

//
// GPIO_PAD Fileds
//
typedef union {
  struct {
    UINT32    PadNum      :16;
    UINT32    GrpIdx      :8;
    UINT32    ChipsetId   :4;
    UINT32    Rsvd        :4;
  } PadField;
  UINT32      Pad;
} PAD_INFO;

//
// GPIO_CFG_DATA DW1 fields
//
typedef struct {
  UINT32    Rsvd1       :16;
  UINT32    PadNum      :8;
  UINT32    GrpIdx      :5;
  UINT32    Rsvd2       :3;
} GPIO_CFG_DATA_DW1;

/**
  This function gets Group to GPE0 configuration

  @param[out] GpeDw0Value       GPIO Group to GPE_DW0 assignment
  @param[out] GpeDw1Value       GPIO Group to GPE_DW1 assignment
  @param[out] GpeDw2Value       GPIO Group to GPE_DW2 assignment
**/
VOID
EFIAPI
PmcGetGpioGpe (
  OUT UINT32    *GpeDw0Value,
  OUT UINT32    *GpeDw1Value,
  OUT UINT32    *GpeDw2Value
  )
{
  UINT32 Data32;

  Data32 = MmioRead32 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GPIO_CFG);

  *GpeDw0Value = ((Data32 & B_PMC_PWRM_GPIO_CFG_GPE0_DW0) >> N_PMC_PWRM_GPIO_CFG_GPE0_DW0);
  *GpeDw1Value = ((Data32 & B_PMC_PWRM_GPIO_CFG_GPE0_DW1) >> N_PMC_PWRM_GPIO_CFG_GPE0_DW1);
  *GpeDw2Value = ((Data32 & B_PMC_PWRM_GPIO_CFG_GPE0_DW2) >> N_PMC_PWRM_GPIO_CFG_GPE0_DW2);

}


/**
  Return opcode supported for writing to GPIO lock unlock register

  @retval UINT8   Lock Opcode
**/
UINT8
EFIAPI
GpioGetLockOpcode (
  VOID
  )
{
  return GpioLockUnlock;
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
  // TODO: Update code for CDF- check wich groups are in DSW
  return FALSE;
}


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
  *GpioGroupInfoTableLength = ARRAY_SIZE (mCdfPchHGpioGroupInfo);
  return mCdfPchHGpioGroupInfo;
}


/**
  Get GPIO Chipset ID specific to PCH generation and series

  @retval PCH Chipset ID
**/
UINT32
EFIAPI
GpioGetThisChipsetId (
  VOID
  )
{
  return GPIO_CDF_H_CHIPSET_ID;
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
  *GpioGroupToGpeMapping = mCdfPchGpioGroupToGpeMapping;
  *GpioGroupToGpeMappingLength = ARRAY_SIZE (mCdfPchGpioGroupToGpeMapping);
}


/**
  This function provides GPIO Community PortIDs

  @param[out] NativePinsTable                Table with GPIO COMMx SBI PortIDs

  @retval     Number of communities
**/
UINT32
EFIAPI
GpioGetComSbiPortIds (
  OUT GPIO_PCH_SBI_PID    **GpioComSbiIds
  )
{
  *GpioComSbiIds = mGpioComSbiIds;
  return ARRAY_SIZE (mGpioComSbiIds);
}

/**
  Get PCH PCR Address for this Register

  @param[in] Pid                        Port ID of the SBI message
  @param[in] Offset                     Offset of the Cfg Regsiter

  @retval UINT32                        Pch Pcr Address to access this register
**/
UINT32
EFIAPI
GetPchPcrAddress (
  IN     GPIO_PCH_SBI_PID               Pid,
  IN     UINT32                         Offset
  )
{
  return PCH_PCR_ADDRESS (Pid, Offset);
}


/**
  Get TypeC SBU Gpio Pad table

  @param[out] TableLength       Length of the TypeC SBU Gpio Pad table

  @retval Pointer to TypeC SBU Gpio Pad table
**/
GPIO_PAD*
EFIAPI
GpioGetTypeCSbuGpioPad (
  IN OUT   UINT32      *TableLength
)
{
  *TableLength = 0;
  return NULL;
}

/**
  Get GPIO PCR Pad Cfg Offset

  @retval PAD CFG Offset
**/
UINT8
EFIAPI
GpioGetPcrPadCfgOffset(
  VOID
  )
{
  return S_GPIO_PCR_PADCFG;
}


/**
  This procedure will return GpioPad from Group and PadNumber

  @param[in] Group              GPIO group
  @param[in] PadNumber          GPIO PadNumber

  @retval GpioPad               GpioPad
**/
GPIO_PAD
EFIAPI
GpioGetGpioPadFromGroupAndPadNumber (
  IN GPIO_GROUP      Group,
  IN UINT32          PadNumber
  )
{
  return GPIO_PAD_DEF (Group,PadNumber);
}


/**
  This procedure will return GpioPad from GroupIndex and PadNumber

  @param[in] GroupIndex         GPIO GroupIndex
  @param[in] PadNumber          GPIO PadNumber

  @retval GpioPad               GpioPad
**/
GPIO_PAD
EFIAPI
GpioGetGpioPadFromGroupIndexAndPadNumber (
  IN UINT32          GroupIndex,
  IN UINT32          PadNumber
  )
{
  GPIO_GROUP Group;

  Group = GPIO_GROUP_DEF (GroupIndex, GpioGetThisChipsetId ());
  return GPIO_PAD_DEF (Group, PadNumber);
}


/**
  This procedure will get group from group index (0 based)

  @param[in] GroupIndex        Group Index

  @retval GpioGroup            Gpio Group
**/
GPIO_GROUP
EFIAPI
GpioGetGroupFromGroupIndex (
  IN UINT32        GroupIndex
  )
{
  return GPIO_GROUP_DEF (GroupIndex, GpioGetThisChipsetId ());
}


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
  This procedure will get Chipset ID from Gpio Pad

  @param[in] GpioPad          Gpio Pad

  @retval Value               PCH Chipset ID
**/
UINT32
EFIAPI
GpioGetChipsetIdFromGpioPad (
  IN GPIO_PAD        GpioPad
  )
{
  return (UINT32) GPIO_GET_CHIPSET_ID (GpioPad);
}

/**
  This procedure will get Gpio Pad from Cfg Dword

  @param[in]  GpioItem         Pointer to the Gpio Cfg Data Item
  @param[out] GpioPad          Gpio Pad
**/
VOID
EFIAPI
GpioGetGpioPadFromCfgDw (
  IN  UINT32        *GpioItem,
  OUT GPIO_PAD      *GpioPad
  )
{
  GPIO_CFG_DATA_DW1    *Dw1;
  PAD_INFO              PadInfo;

  Dw1 = (GPIO_CFG_DATA_DW1 *) (&GpioItem[1]);

  PadInfo.PadField.PadNum    = (UINT16) Dw1->PadNum;
  PadInfo.PadField.GrpIdx    = (UINT8)  Dw1->GrpIdx;
  PadInfo.PadField.ChipsetId = GpioGetThisChipsetId ();
  *GpioPad = PadInfo.Pad;

  //
  // Remove PadInfo data from DW1
  //
  Dw1->PadNum = 0;
  Dw1->GrpIdx = 0;
}
