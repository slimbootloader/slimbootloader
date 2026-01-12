/** @file
  Register names for VTD block

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _VTD_REGS_H_
#define _VTD_REGS_H_


///
/// Vt-d Engine base address.
///
#define R_MCHBAR_VTD1_OFFSET                 0x5410  ///< HW UNIT1 for IGD
#define R_MCHBAR_VTD2_LOW_OFFSET             0x7880  ///< HW UNIT2 for IPU
#define R_MCHBAR_VTD3_OFFSET                 0x5410  ///< HW UNIT3 for all other - PEG, USB, SATA etc
#define ECAP_VTD_IOMMU_REG                   0x10
#define B_ECAP_DEVICE_TLB_SUPPORT            BIT2

#endif
