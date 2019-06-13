/** @file
  Register names for USB Host and device controller

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _REGS_USB_H_
#define _REGS_USB_H_

#define R_XDCI_CFG_PMCSR                      0x84

#define R_XHCI_CFG_PCE                        0xA2

#define R_XDCI_MEM_APBFC_U3PMU_CFG2           0x10F810
#define R_XDCI_MEM_APBFC_U3PMU_CFG4           0x10F818
#define R_XDCI_MEM_APBFC_U3PMU_CFG5           0x10F81C

#endif // _REGS_USB_H_
