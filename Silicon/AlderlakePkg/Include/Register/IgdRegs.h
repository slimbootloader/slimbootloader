/** @file
  Register names for IGD block

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
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


#define V_SA_PCI_DEV_2_GT0_TGL_ID          0x9A7F ///< Dev2 TGL GT0

#define V_SA_PCI_DEV_2_GT0_ADL_S_ID      0x469F ///< Dev2 ADL-S GT0
#define V_SA_PCI_DEV_2_GT0_ADL_P_ID      0x46FF ///< Dev2 ADL-P GT0

///
/// GTTMMADR aligned to 16MB (Base address = [38:24])
///
#define R_SA_IGD_GTTMMADR          0x10


///
/// GT MMIO Registers for CD Clock Programming
///
#define B_SA_GTTMMADR_DSSM_REFERENCE_FREQ_OFFSET 29


#endif
