/** @file
  Register names for VTD block
  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position
  Copyright (c) 2016 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _VTD_REGS_H_
#define _VTD_REGS_H_

#define R_VTD_ECAP_OFFSET                       0x10
#define B_VTD_ECAP_REG_IR                       BIT3

///
/// Vt-d Engine base address.
///
#define R_MCHBAR_VTD1_OFFSET                 0x5400  ///< HW UNIT1 for IGD
#define R_MCHBAR_VTD2_LOW_OFFSET             0x7880  ///< HW UNIT2 for IPU
#define R_MCHBAR_ITBT_PCIE_0_VTD_LOW_OFFSET  0x7888  ///< HW UNIT4 for iTBT PCIE0
#define R_MCHBAR_ITBT_PCIE_1_VTD_LOW_OFFSET  0x7890  ///< HW UNIT5 for iTBT PCIE1
#define R_MCHBAR_ITBT_PCIE_2_VTD_LOW_OFFSET  0x7898  ///< HW UNIT6 for iTBT PCIE2
#define R_MCHBAR_ITBT_PCIE_3_VTD_LOW_OFFSET  0x78A0  ///< HW UNIT7 for iTBT PCIE3
#define R_MCHBAR_VTD3_OFFSET                 0x5410  ///< HW UNIT3 for all other - PEG, USB, SATA etc


#endif
