/** @file
  Register names for CNL PCH PMC device

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

  Copyright (c) 2017 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_PMC_CNL_H_
#define _PCH_REGS_PMC_CNL_H_

//
// PMC SSRAM Registers (D20:F2)
//
#define PCI_DEVICE_NUMBER_PCH_PMC_SSRAM                     20
#define PCI_FUNCTION_NUMBER_PCH_PMC_SSRAM                   2

//
// PMC Registers (D31:F2)
//
#define PCI_DEVICE_NUMBER_PCH_PMC                           31
#define PCI_FUNCTION_NUMBER_PCH_PMC                         2

//
// TBD :  This needs to be defined. Can I use the below value for ICL and CNL both for D20:F2 and D31:F2.
//
#define B_PCH_PMC_PM_DATA_BAR                               0xFFFFC000


//
// PWRM Registers
//
#define R_CNL_PCH_PWRM_GPIO_CFG                                 0x1920

#define R_CNL_PCH_PWRM_MODPHY_PM_CFG5                           0x10D0


/// @todo SERVER - CDF - Update SATA 1..3 function disable bits
#define B_CDF_PCH_PWRM_NST_PG_FDIS_1_SATA_1_FDIS_PMC            BIT27  ///< SATA 1 Function Disable (CDF PCH)
#define B_CDF_PCH_PWRM_NST_PG_FDIS_1_SATA_2_FDIS_PMC            BIT27  ///< SATA 2 Function Disable (CDF PCH)
#define B_CDF_PCH_PWRM_NST_PG_FDIS_1_SATA_3_FDIS_PMC            BIT27  ///< SATA 3 Function Disable (CDF PCH)

/// @todo SERVER - CDF - Update SATA 1..3 fuse/ss disable bits
#define B_CDF_PCH_PWRM_FUSE_DIS_RD_2_SATA_1_FUSE_SS_DIS         BIT15  ///< SATA 1 Fuse or Soft Strap Disable (CDF PCH)
#define B_CDF_PCH_PWRM_FUSE_DIS_RD_2_SATA_2_FUSE_SS_DIS         BIT15  ///< SATA 2 Fuse or Soft Strap Disable (CDF PCH)
#define B_CDF_PCH_PWRM_FUSE_DIS_RD_2_SATA_3_FUSE_SS_DIS         BIT15  ///< SATA 3 Fuse or Soft Strap Disable (CDF PCH)


//
// ICLK Registers
//
#define V_CNL_PCH_PCR_ICLK_CMU_ONE_DWORD0_MUXSEL_CPUBCLK_OCPLL  1

#endif // _PCH_REGS_PMC_CNL_H_
