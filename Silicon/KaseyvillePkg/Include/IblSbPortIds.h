/** @file
  Sideband Target Port IDs for Integrated Boot Logic (IBL)

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _IBL_REGS_PCR_H_
#define _IBL_REGS_PCR_H_

//
// Note 1:
// Integrated Boot Logic (IBL) has P2SB device emulated in firmware
//
#define IBL_PID_SFR        0x04 // Used for implementing special private functions Special Functions Registers
#define IBL_PID_ESPISPI    0x02 // eSPI
#define IBL_PID_ITSS       0x00 // ITSS
#define IBL_PID_GPIO       0x03 // GPIO
#define IBL_PID_PSTH       0x01 // IO-Trap

#endif
