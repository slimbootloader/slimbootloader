/** @file
  Register names for SATA controllers

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
#ifndef _SATA_REGS_H_
#define _SATA_REGS_H_

//
// RST PCIe Storage Remapping Registers
//
#define V_SATA_MEM_RST_PCIE_STORAGE_REMAP_RP_OFFSET     0x100                           ///< Remapped Root Port Offset Value
#define R_SATA_CFG_RST_PCIE_STORAGE_CAIR                0x308                           ///< Configuration Access Index Register
#define R_SATA_CFG_RST_PCIE_STORAGE_MXCRC               0x33C       ///< MSI-X Capability Remap Configuration
#define R_SATA_CFG_RST_PCIE_STORAGE_CRGC                0xFC0       ///< RST PCIe Storage Cycle Router Global Control

#endif
