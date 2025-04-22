/** @file
  Register names for USB Host and device controller

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _USB_REGS_H_
#define _USB_REGS_H_


#define R_XHCI_CFG_PWR_CNTL_STS             0x74
#define R_XHCI_CFG_PCE                      0xA2


//
// xDCI (OTG) PCI Config Space Registers
//
#define R_XDCI_CFG_PMCSR                      0x84      ///< Power Management Control and Status Register

//
// xDCI (OTG) MMIO registers
//
#define R_XDCI_MEM_GCTL                         0xC110  ///< Xdci Global Ctrl
#define R_XDCI_MEM_APBFC_U3PMU_CFG2             0x10F810
#define R_XDCI_MEM_APBFC_U3PMU_CFG4             0x10F818
#define R_XDCI_MEM_APBFC_U3PMU_CFG5             0x10F81C


//
// USB2 Private Configuration Registers
// USB2 HIP design featured
// (PID:USB2)
// Doesn't apply to CPU (only USB3 functionality present)
//
#define N_USB2_PCR_SFRCONFIG_0_SFRTIMER_COEF  4


#endif // _USB_REGS_H_
