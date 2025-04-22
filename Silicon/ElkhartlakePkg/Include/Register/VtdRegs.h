/** @file
  Register names for VTD block
  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _VTD_REGS_H_
#define _VTD_REGS_H_

#define VTD_ENGINE_NUMBER                       3

///
/// Vt-d Engine base address.
///
#define R_MCHBAR_VTD1_OFFSET                 0x5400  ///< HW UNIT1 for IGD
#define R_MCHBAR_VTD3_OFFSET                 0x5410  ///< HW UNIT3 for all other - PEG, USB, SATA etc


#endif
