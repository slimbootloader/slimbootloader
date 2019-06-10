/** @file
  Register names for USB Host and device controller

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _REGS_USB_H_
#define _REGS_USB_H_

#define R_XDCI_CFG_PMCSR                      0x84

#define R_XHCI_CFG_PCE                        0xA2

#define R_XDCI_MEM_APBFC_U3PMU_CFG2           0x10F810
#define R_XDCI_MEM_APBFC_U3PMU_CFG4           0x10F818
#define R_XDCI_MEM_APBFC_U3PMU_CFG5           0x10F81C

#endif // _REGS_USB_H_
