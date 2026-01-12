/** @file
  Register names for IGD block

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _IGD_REGS_H_
#define _IGD_REGS_H_

///
/// Device 2 Register Equates
///
//
// The following equates must be reviewed and revised when the specification is ready.
//
#define IGD_BUS_NUM          0x00
#define IGD_DEV_NUM          0x02
#define IGD_FUN_NUM          0x00

#define V_SA_PCI_DEV_2_GT0_PTL_U_ID      0xB090   //
#define V_SA_PCI_DEV_2_GT0_PTL_H_ID      0xB080   //PTL-H 12Xe3
#define V_SA_PCI_DEV_2_GT2_PTL_H_ID      0xB0A0  //PTL-H 4Xe3

///
/// GTTMMADR aligned to 16MB (Base address = [38:24])
///
#define R_SA_IGD_GTTMMADR          0x10


///
/// GT MMIO Registers for CD Clock Programming
///
#define B_SA_GTTMMADR_DSSM_REFERENCE_FREQ_OFFSET 29

#define GTT_MM_ADDR                          0xCF000000
#define LMEM_BAR                             0xD0000000
#endif
