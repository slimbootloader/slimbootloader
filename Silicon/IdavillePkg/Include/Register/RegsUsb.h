/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _REGS_USB_H_
#define _REGS_USB_H_
//
// USB3 (XHCI) related definitions
//
#ifdef EBG_SC_FLAG
#define PCI_DEVICE_NUMBER_PCH_XHCI                              20
#define PCI_FUNCTION_NUMBER_PCH_XHCI                            0
#else
#define PCI_DEVICE_NUMBER_PCH_XHCI                              30
#define PCI_FUNCTION_NUMBER_PCH_XHCI                            0
#endif //EBG_SC_FLAG
//
//
// XHCI PCI Config Space registers
//
#define R_XHCI_CFG_PCE                                          0xA2


//
// 0x2000 - 0x21FF - Runtime Registers
// 0x3000 - 0x307F - Doorbell Registers
//
#define N_XHCI_MEM_PGCBCTRL_IP_INACCESSIBLE_HYSTERESIS_TIMER 25
#define V_XHCI_MEM_PGCBCTRL_IP_INACCESSIBLE_HYSTERESIS_TIMER 6


#endif // _REGS_USB_H_
