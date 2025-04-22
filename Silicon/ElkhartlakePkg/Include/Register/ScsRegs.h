/** @file
  Register names for PCH Storage and Communication Subsystem

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

  Copyright (c) 2013 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _SCS_REGS_H_
#define _SCS_REGS_H_

//
// SCS Devices proprietary PCI Config Space Registers
//
#define R_SCS_CFG_PCS                     0x84                          ///< PME Control Status
#define R_SCS_CFG_PG_CONFIG               0xA2                          ///< PG Config
//
// PCR registers
//
#define R_SCS_PCR_4820                    0x4820
#define R_SCS_PCR_4020                    0x4020
#define R_SCS_PCR_5820                    0x5820
#define R_SCS_PCR_5C20                    0x5C20
#define R_SCS_PCR_1078                    0x1078
#define R_SCS_PCR_1C20                    0x1C20
#endif
