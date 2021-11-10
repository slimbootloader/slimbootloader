/** @file
  Register names for PCH Programmable Service Engine (PSE)

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

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PSE_REGS_H_
#define _PSE_REGS_H_


//
// PSE Controller Registers
//
// EHL-PCH D29:F0
#define PCI_DEVICE_NUMBER_PCH_PSE_LH2PSE      29


//
// Memory mapped IO space registers
//
#define R_PSE_MEM_FW_STS                      0x34       ///< PSE Host Firmware status Register
#define B_PSE_MEM_FW_STS_STATUS               0x0000F000 ///< mask to get fw state bits(12:15)
#define N_PSE_MEM_FW_STS_STATUS               12

#define R_PSE_MEM_HOST_COMM                   0x38       ///< Host Communication Register
#define B_PSE_MEM_HOST_COMM_HOST_READY        BIT7       ///< bit corresponds host ready bit in Host Status Register (HOST_COMM)
#define B_PSE_MEM_HOST_COMM_DRIVER_READY      BIT1

#define R_PSE_MEM_HOST2PSE_DRBL               0x48       ///< Inbound doorbell register Host to PSE
#define R_PSE_MEM_PSE2HOST_DRBL               0x54       ///< Outbound doorbell register PSE to Host
#define B_PSE_MEM_DRBL_BUSY                   BIT31      ///< Busy bit, common for both doorbells

#define R_PSE_MEM_HOST2PSE_MSG                0xE0       ///< HOST to PSE message registers
#define R_PSE_MEM_PSE2HOST_MSG                0x60       ///< PSE to HOST message registers
#define R_PSE_MEM_REMAP2                      0x368
#define DRIVER_LOADED_BIT                     BIT0
#define R_PSE_MEM_REMAP3                      0x36C


// PSE DMA registers
#define V_PCH_PSE_DMAC_BAR_SIZE       0x4000
#endif
