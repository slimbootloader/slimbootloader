/** @file
  Register names for USB Host and device controller

  Conventions:

  - Register definition format:
    Prefix_[GenerationName]_[ComponentName]_SubsystemName_RegisterSpace_RegisterName
  - Prefix:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register size
    Definitions beginning with "N_" are the bit position
  - [GenerationName]:
    Three letter acronym of the generation is used (e.g. SKL,KBL,CNL etc.).
    Register name without GenerationName applies to all generations.
  - [ComponentName]:
    This field indicates the component name that the register belongs to (e.g. PCH, SA etc.)
    Register name without ComponentName applies to all components.
    Register that is specific to -H denoted by "_PCH_H_" in component name.
    Register that is specific to -LP denoted by "_PCH_LP_" in component name.
  - SubsystemName:
    This field indicates the subsystem name of the component that the register belongs to
    (e.g. PCIE, USB, SATA, GPIO, PMC etc.).
  - RegisterSpace:
    MEM - MMIO space register of subsystem.
    IO  - IO space register of subsystem.
    PCR - Private configuration register of subsystem.
    CFG - PCI configuration space register of subsystem.
  - RegisterName:
    Full register name.

  Copyright (c) 1999 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _USB_REGS_H_
#define _USB_REGS_H_

//
// USB3 (XHCI) related definitions
// @todo: Add CPU PCI defs for xHCI
//
#define PCI_BUS_NUMBER_PCH_XHCI             0
#define PCI_DEVICE_NUMBER_PCH_XHCI          20
#define PCI_FUNCTION_NUMBER_PCH_XHCI        0


#define R_XHCI_CFG_PCE                      0xA2

//
// xDCI (OTG) USB Device Controller
//
#define PCI_DEVICE_NUMBER_PCH_XDCI              20
#define PCI_FUNCTION_NUMBER_PCH_XDCI            1

//
// xDCI (OTG) PCI Config Space Registers
//
#define R_XDCI_CFG_PMCSR                      0x84      ///< Power Management Control and Status Register

//
// xDCI (OTG) MMIO registers
//
#define R_XDCI_MEM_GCTL                       0xC110  ///< Xdci Global Ctrl
#define R_XDCI_MEM_APBFC_U3PMU_CFG2           0x10F810
#define R_XDCI_MEM_APBFC_U3PMU_CFG4           0x10F818
#define R_XDCI_MEM_APBFC_U3PMU_CFG5           0x10F81C


//
// USB2 Private Configuration Registers
// USB2 HIP design featured
// (PID:USB2)
// Doesn't apply to CPU (only USB3 functionality present)
//
#define N_USB2_PCR_SFRCONFIG_0_SFRTIMER_COEF  4

#endif // _USB_REGS_H_
