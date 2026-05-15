/** @file
  This file provides GPIO topology for GNR SOC.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/PmcRegsIbl.h>
#include <Register/GpioRegsIbl.h>
#include <Register/GpioV2ChipsetId.h>
#include <GpioV2Config.h>
#include <Library/GpioV2SiLib.h>
#include <GpioV2PinsIbl.h>
#include <Library/BaseLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Include/IblSbPortIds.h>

#define NO_REGISTER_FOR_PROPERTY 0xFFFF
#define GPIO_HID_IBL GPIO_IBL_ACPI_HID

#define PAD_DW_SIZE  0x8
GPIOV2_PAD Community0Group0Pads[] = {
  GPIOV2_IBL_GPP_C0,
  GPIOV2_IBL_GPP_C1,
  GPIOV2_IBL_GPP_C2,
  GPIOV2_IBL_GPP_C3,
  GPIOV2_IBL_GPP_C4,
  GPIOV2_IBL_GPP_C5,
  GPIOV2_IBL_GPP_C6,
  GPIOV2_IBL_GPP_C7,
  GPIOV2_IBL_GPP_C8,
  GPIOV2_IBL_GPP_C9,
  GPIOV2_IBL_GPP_C10,
  GPIOV2_IBL_GPP_C11,
  GPIOV2_IBL_GPP_C12,
  GPIOV2_IBL_GPP_C13,
  GPIOV2_IBL_GPP_C14,
  GPIOV2_IBL_GPP_C15,
  GPIOV2_IBL_GPP_C16,
  GPIOV2_IBL_GPP_C17,
  GPIOV2_IBL_GPP_C18,
  GPIOV2_IBL_GPP_C19,
  GPIOV2_IBL_GPP_C20,
  GPIOV2_IBL_GPP_C21,
  GPIOV2_IBL_GPP_C22,
  GPIOV2_IBL_GPP_C23
};

GPIOV2_PAD Community0Group1Pads[] = {
  GPIOV2_IBL_GPP_D0,
  GPIOV2_IBL_GPP_D1,
  GPIOV2_IBL_GPP_D2,
  GPIOV2_IBL_GPP_D3,
  GPIOV2_IBL_GPP_D4,
  GPIOV2_IBL_GPP_D5,
  GPIOV2_IBL_GPP_D6,
  GPIOV2_IBL_GPP_D7,
  GPIOV2_IBL_GPP_D8,
  GPIOV2_IBL_GPP_D9,
  GPIOV2_IBL_GPP_D10,
  GPIOV2_IBL_GPP_D11,
  GPIOV2_IBL_GPP_D12,
  GPIOV2_IBL_GPP_D13,
  GPIOV2_IBL_GPP_D14,
  GPIOV2_IBL_GPP_D15
};

// Com 0
GPIOV2_GROUP Community0Groups[] = {
  {
    .Name              = "GPP_C",
    .GpioPadGroup      = GPIOV2_IBL_GROUP_C,
    .GroupToGpeMapping = { V_IBL_PCH_PMC_PWRM_GPIO_CFG_GPP_C, 0 },
    .RegisterOffsets   = { NO_REGISTER_FOR_PROPERTY, R_IBL_GPIO_PCR_GPP_C_PADCFGLOCK, R_IBL_GPIO_PCR_GPP_C_PADCFGLOCKTX, R_IBL_GPIO_PCR_GPP_C_HOSTSW_OWN, R_IBL_GPIO_PCR_GPP_C_GPI_IS, R_IBL_GPIO_PCR_GPP_C_GPI_IE, R_IBL_GPIO_PCR_GPP_C_GPI_GPE_STS, R_IBL_GPIO_PCR_GPP_C_GPI_GPE_EN, R_IBL_GPIO_PCR_GPP_C_SMI_STS, R_IBL_GPIO_PCR_GPP_C_SMI_EN, R_IBL_GPIO_PCR_GPP_C_NMI_STS, R_IBL_GPIO_PCR_GPP_C_NMI_EN, R_IBL_GPIO_PCR_GPP_C_PADCFG_OFFSET },
    .PadsNum           = sizeof (Community0Group0Pads) / sizeof (GPIOV2_PAD),
    .Pads              = Community0Group0Pads,
    .PadDwSize         = PAD_DW_SIZE
  },
  {
    .Name              = "GPP_D",
    .GpioPadGroup      = GPIOV2_IBL_GROUP_D,
    .GroupToGpeMapping = { V_IBL_PCH_PMC_PWRM_GPIO_CFG_GPP_D, 0 },
    .RegisterOffsets   = { NO_REGISTER_FOR_PROPERTY, R_IBL_GPIO_PCR_GPP_D_PADCFGLOCK, R_IBL_GPIO_PCR_GPP_D_PADCFGLOCKTX, R_IBL_GPIO_PCR_GPP_D_HOSTSW_OWN, R_IBL_GPIO_PCR_GPP_D_GPI_IS, R_IBL_GPIO_PCR_GPP_D_GPI_IE, R_IBL_GPIO_PCR_GPP_D_GPI_GPE_STS, R_IBL_GPIO_PCR_GPP_D_GPI_GPE_EN, R_IBL_GPIO_PCR_GPP_D_SMI_STS, R_IBL_GPIO_PCR_GPP_D_SMI_EN, R_IBL_GPIO_PCR_GPP_D_NMI_STS, R_IBL_GPIO_PCR_GPP_D_NMI_EN, R_IBL_GPIO_PCR_GPP_D_PADCFG_OFFSET },
    .PadsNum           = sizeof (Community0Group1Pads) / sizeof (GPIOV2_PAD),
    .Pads              = Community0Group1Pads,
    .PadDwSize         = PAD_DW_SIZE
  }
};


//
// Communities structure
//
GPIOV2_COMMUNITY GnrSocCommunities[] = {
  {
    // Community 0 groups info
    .RegisterOffsets = { R_IBL_GPIO_PCR_MISCCFG },
    .GroupsNum       = sizeof (Community0Groups) / sizeof (GPIOV2_GROUP),
    .Groups          = Community0Groups,
    .Pid             = IBL_PID_GPIO,
  }
};

