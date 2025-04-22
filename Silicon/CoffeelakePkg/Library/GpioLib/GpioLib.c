/** @file
  This file contains routines for GPIO

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "GpioInitLib.h"
#include <GpioPinsCnlLp.h>
#include <GpioPinsCnlH.h>
#include <Library/GpioLib.h>
#include <Library/GpioSiLib.h>

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

  Data32 = MmioRead32 ((UINTN) (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GPIO_CFG));

  *GpeDw0Value = ((Data32 & B_PMC_PWRM_GPIO_CFG_GPE0_DW0) >> N_PMC_PWRM_GPIO_CFG_GPE0_DW0);
  *GpeDw1Value = ((Data32 & B_PMC_PWRM_GPIO_CFG_GPE0_DW1) >> N_PMC_PWRM_GPIO_CFG_GPE0_DW1);
  *GpeDw2Value = ((Data32 & B_PMC_PWRM_GPIO_CFG_GPE0_DW2) >> N_PMC_PWRM_GPIO_CFG_GPE0_DW2);
}

/**
  This procedure will get Group to GPE mapping. If group has more than 32 bits
  it is possible to map only single DW of pins (e.g. 0-31, 32-63) because
  ACPI GPE_DWx register is 32 bits large.

  @param[out]  GroupToGpeDw0       GPIO group mapped to GPE_DW0
  @param[out]  GroupDwForGpeDw0    DW of pins mapped to GPE_DW0
  @param[out]  GroupToGpeDw1       GPIO group mapped to GPE_DW1
  @param[out]  GroupDwForGpeDw1    DW of pins mapped to GPE_DW1
  @param[out]  GroupToGpeDw2       GPIO group mapped to GPE_DW2
  @param[out]  GroupDwForGpeDw2    DW of pins mapped to GPE_DW2

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioGetGroupDwToGpeDwX (
  OUT GPIO_GROUP                *GroupToGpeDw0,
  OUT UINT32                    *GroupDwForGpeDw0,
  OUT GPIO_GROUP                *GroupToGpeDw1,
  OUT UINT32                    *GroupDwForGpeDw1,
  OUT GPIO_GROUP                *GroupToGpeDw2,
  OUT UINT32                    *GroupDwForGpeDw2
  )
{
  UINT32                     PmcGpeDwXValue[3];
  GPIO_GROUP                 GroupToGpeDwX[3];
  UINT32                     GroupDwForGpeDwX[3];
  UINT8                      GpeDwXIndex;
  UINT32                     Index;
  GPIO_GROUP_TO_GPE_MAPPING  *GpioGpeMap;
  UINT32                     GpioGpeMapLength;

  ZeroMem (GroupToGpeDwX, sizeof (GroupToGpeDwX));
  ZeroMem (GroupDwForGpeDwX, sizeof (GroupDwForGpeDwX));

  PmcGetGpioGpe (&PmcGpeDwXValue[0], &PmcGpeDwXValue[1], &PmcGpeDwXValue[2]);

  GpioGetGroupToGpeMapping (&GpioGpeMap, &GpioGpeMapLength);

  for (GpeDwXIndex = 0; GpeDwXIndex < 3; GpeDwXIndex++) {
    for (Index = 0; Index < GpioGpeMapLength; Index++) {

      if (GpioGpeMap[Index].PmcGpeDwxVal == PmcGpeDwXValue[GpeDwXIndex]) {
        GroupToGpeDwX[GpeDwXIndex] = GpioGpeMap[Index].Group;
        GroupDwForGpeDwX[GpeDwXIndex] = GpioGpeMap[Index].GroupDw;
        break;
      }
    }
  }

  if ((GroupToGpeDwX[0] == 0) ||
      (GroupToGpeDwX[1] == 0) ||
      (GroupToGpeDwX[2] == 0)) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  *GroupToGpeDw0 = GroupToGpeDwX[0];
  *GroupDwForGpeDw0 = GroupDwForGpeDwX[0];
  *GroupToGpeDw1 = GroupToGpeDwX[1];
  *GroupDwForGpeDw1 = GroupDwForGpeDwX[1];
  *GroupToGpeDw2 = GroupToGpeDwX[2];
  *GroupDwForGpeDw2 = GroupDwForGpeDwX[2];

  return EFI_SUCCESS;
}

/**
  This procedure will get Gpio Pad Ownership

  @param[in] GpioPad              GPIO pad
  @param[out] PadOwnVal           Value of Pad Ownership

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioGetPadOwnership (
  IN  GPIO_PAD                GpioPad,
  OUT GPIO_PAD_OWN            *PadOwnVal
  )
{
  UINT32                 Mask;
  UINT32                 RegOffset;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;
  CONST GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 PadOwnRegValue;

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Calculate RegOffset using Pad Ownership offset and GPIO Pad number.
  // One DWord register contains information for 8 pads.
  //
  RegOffset = GpioGroupInfo[GroupIndex].PadOwnOffset + (PadNumber >> 3) * 0x4;

  //
  // Calculate pad bit position within DWord register
  //
  PadNumber %= 8;
  Mask = (BIT1 | BIT0) << (PadNumber * 4);

  PadOwnRegValue = MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, RegOffset));

  *PadOwnVal = (GPIO_PAD_OWN) ((PadOwnRegValue & Mask) >> (PadNumber * 4));

  return EFI_SUCCESS;
}

/**
  This internal procedure will calculate GPIO_RESET_CONFIG value  (new type)
  based on provided PadRstCfg for a specific GPIO Pad.

  @param[in]  GpioPad               GPIO Pad
  @param[in]  PadRstCfg             GPIO PadRstCfg value

  @retval GpioResetConfig           GPIO Reset configuration (new type)
**/
GPIO_RESET_CONFIG
EFIAPI
GpioResetConfigFromPadRstCfg (
  IN  GPIO_PAD           GpioPad,
  IN  UINT32             PadRstCfg
  )
{
  GPIO_GROUP           Group;

  static GPIO_RESET_CONFIG  GppPadRstCfgToGpioResetConfigMap[] = {
                              GpioResumeReset,
                              GpioHostDeepReset,
                              GpioPlatformReset};
  static GPIO_RESET_CONFIG  GpdPadRstCfgToGpioResetConfigMap[] = {
                              GpioDswReset,
                              GpioHostDeepReset,
                              GpioPlatformReset,
                              GpioResumeReset};

  Group = GpioGetGroupFromGpioPad (GpioPad);

  if (GpioIsDswGroup (Group) && PadRstCfg < 4) {
    return GpdPadRstCfgToGpioResetConfigMap[PadRstCfg];
  } else if (PadRstCfg < 3) {
    return GppPadRstCfgToGpioResetConfigMap[PadRstCfg];
  } else {
    ASSERT (FALSE);
    return GpioResetDefault;
  }
}

/**
  This internal procedure will get GPIO_CONFIG data from PADCFG registers value

  @param[in]  GpioPad                   GPIO Pad
  @param[in]  PadCfgDwReg               PADCFG DWx register values
  @param[out] GpioData                  GPIO Configuration data

  @retval Status
**/
STATIC
VOID
GpioConfigFromPadCfgRegValue (
  IN GPIO_PAD      GpioPad,
  IN CONST UINT32  *PadCfgDwReg,
  OUT GPIO_CONFIG  *GpioConfig
  )
{
  UINT32               PadRstCfg;

  //
  // Get Reset Type (PadRstCfg)
  //
  PadRstCfg = (PadCfgDwReg[0] & B_GPIO_PCR_RST_CONF) >> N_GPIO_PCR_RST_CONF;

  GpioConfig->PowerConfig = GpioResetConfigFromPadRstCfg (
                              GpioPad,
                              PadRstCfg
                              );

  //
  // Get how interrupt is triggered (RxEvCfg)
  //
  GpioConfig->InterruptConfig = ((PadCfgDwReg[0] & B_GPIO_PCR_RX_LVL_EDG) >> (N_GPIO_PCR_RX_LVL_EDG - (N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS + 1))) | (0x1 << N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS);

  //
  // Get interrupt generation (GPIRoutIOxAPIC/SCI/SMI/NMI)
  //
  GpioConfig->InterruptConfig |= ((PadCfgDwReg[0] & (B_GPIO_PCR_RX_NMI_ROUTE | B_GPIO_PCR_RX_SCI_ROUTE | B_GPIO_PCR_RX_SMI_ROUTE | B_GPIO_PCR_RX_APIC_ROUTE)) >> (N_GPIO_PCR_RX_NMI_ROUTE - (N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS + 1))) | (0x1 << N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS);

  //
  // Get GPIO direction (GPIORxDis and GPIOTxDis)
  //
  GpioConfig->Direction = ((PadCfgDwReg[0] & (B_GPIO_PCR_RXDIS | B_GPIO_PCR_TXDIS)) >> (N_GPIO_PCR_TXDIS - (N_GPIO_DIRECTION_DIR_BIT_POS + 1))) | (0x1 << N_GPIO_DIRECTION_DIR_BIT_POS);

  //
  // Get GPIO input inversion (RXINV)
  // (Only meaningful if input enabled)
  //
  if((PadCfgDwReg[0] & B_GPIO_PCR_RXDIS) == 0) {
    GpioConfig->Direction |= ((PadCfgDwReg[0] & B_GPIO_PCR_RXINV) >> (N_GPIO_PCR_RXINV - (N_GPIO_DIRECTION_INV_BIT_POS + 1))) | (0x1 << N_GPIO_DIRECTION_INV_BIT_POS);
  }

  //
  // Get GPIO output state (GPIOTxState)
  //
  GpioConfig->OutputState = ((PadCfgDwReg[0] & B_GPIO_PCR_TX_STATE) << (N_GPIO_PCR_TX_STATE + (N_GPIO_OUTPUT_BIT_POS + 1))) | (0x1 << N_GPIO_OUTPUT_BIT_POS);

  //
  // Configure GPIO RX raw override to '1' (RXRAW1)
  //
  GpioConfig->OtherSettings = ((PadCfgDwReg[0] & B_GPIO_PCR_RX_RAW1) >> (N_GPIO_PCR_RX_RAW1 - (N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS + 1))) | (0x1 << N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS);

  //
  // Get GPIO Pad Mode (PMode)
  //
  GpioConfig->PadMode = ((PadCfgDwReg[0] & B_GPIO_PCR_PAD_MODE) >> (N_GPIO_PCR_PAD_MODE - (N_GPIO_PAD_MODE_BIT_POS + 1))) | (0x1 << N_GPIO_PAD_MODE_BIT_POS);

  //
  // Get GPIO termination (Term)
  //
  GpioConfig->ElectricalConfig = ((PadCfgDwReg[1] & B_GPIO_PCR_TERM) >> (N_GPIO_PCR_TERM - (N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS + 1))) | (0x1 << N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS);
}

/**
  This procedure will read multiple GPIO settings

  @param[in]  GpioPad                   GPIO Pad
  @param[out] GpioData                  GPIO data structure

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid group or pad number
**/
EFI_STATUS
EFIAPI
GpioGetPadConfig (
  IN  GPIO_PAD               GpioPad,
  OUT GPIO_CONFIG            *GpioData
  )
{
  UINT32               PadCfgDwReg[GPIO_PADCFG_DW_REG_NUMBER];
  UINT32               RegVal;
  GPIO_GROUP           Group;
  UINT32               PadNumber;
  UINT32               PadBitPosition;

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);
  PadBitPosition = GPIO_GET_PAD_POSITION (PadNumber);

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Read PADCFG DW0 register
  //
  PadCfgDwReg[0] = GpioReadPadCfgReg (GpioPad, 0);

  //
  // Read PADCFG DW1 register
  //
  PadCfgDwReg[1] = GpioReadPadCfgReg (GpioPad, 1);

  //
  // Read PADCFG DW2 register
  //
  PadCfgDwReg[2] = GpioReadPadCfgReg (GpioPad, 2);

  GpioConfigFromPadCfgRegValue (
    GpioPad,
    PadCfgDwReg,
    GpioData
    );

  //
  // Read HOSTSW_OWN registers
  //
  GpioReadReg (
    GpioHostOwnershipRegister,
    Group,
    GPIO_GET_DW_NUM (PadNumber),
    &RegVal
    );

  //
  // Get Host Software Ownership
  //
  GpioData->HostSoftPadOwn = (((RegVal >> PadBitPosition) & 0x1) << (N_GPIO_HOSTSW_OWN_BIT_POS + 1)) | (0x1 << N_GPIO_HOSTSW_OWN_BIT_POS);

  //
  // Read PADCFGLOCK register
  //
  GpioReadReg (
    GpioPadConfigLockRegister,
    Group,
    GPIO_GET_DW_NUM (PadNumber),
    &RegVal
    );

  //
  // Get Pad Configuration Lock state
  //
  GpioData->LockConfig = ((!((RegVal >> PadBitPosition) & 0x1)) << 1) | 0x1;

  //
  // Read PADCFGLOCKTX register
  //
  GpioReadReg (
    GpioPadLockOutputRegister,
    Group,
    GPIO_GET_DW_NUM (PadNumber),
    &RegVal
    );

  //
  // Get Pad Configuration Lock Tx state
  //
  GpioData->LockConfig |= ((!((RegVal >> PadBitPosition) & 0x1)) << 2) | 0x1;

  return EFI_SUCCESS;
}

/**
  This procedure will get GPIO input level

  @param[in] GpioPad              GPIO pad
  @param[out] InputVal            GPIO Input value
                                  0: InputLow, 1: InpuHigh

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioGetInputValue (
  IN GPIO_PAD       GpioPad,
  OUT UINT32        *InputVal
  )
{
  UINT32      PadCfgReg;
  GPIO_CONFIG PadConfig;
  EFI_STATUS  Status;

  if (InputVal == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadValid (GpioPad)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return EFI_UNSUPPORTED;
  }

  Status = GpioGetPadConfig (GpioPad,&PadConfig);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((PadConfig.Direction != GpioDirIn) && (PadConfig.Direction != GpioDirInInv)){
    return EFI_UNSUPPORTED;
  }

  PadCfgReg = GpioReadPadCfgReg (GpioPad, 0);

  *InputVal = (PadCfgReg & B_GPIO_PCR_RX_STATE) >> N_GPIO_PCR_RX_STATE;

  return EFI_SUCCESS;
}
