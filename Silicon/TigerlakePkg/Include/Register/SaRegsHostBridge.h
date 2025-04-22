/** @file
  Register names for Host Bridge block
  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position
  - In general, SA registers are denoted by "_SA_" in register names
  - Registers / bits that are different between SA generations are denoted by
    "_SA_[generation_name]_" in register/bit names. e.g., "_SA_HSW_"
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SA generation will be just named
    as "_SA_" without [generation_name] inserted.

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _SA_REGS_HOST_BRIDGE_H_
#define _SA_REGS_HOST_BRIDGE_H_

//
// DEVICE 0 (Memory Controller Hub)
//
#define R_SA_MC_CAPID0_B   0xE8

/**
 <b>Description</b>:
 - All the bits in this register are LT lockable.
**/
#define R_SA_GGC (0x50)

/**
 Description of GMS (8:15)
 - This field is used to select the amount of Main Memory that is pre-allocated to support the Internal Graphics device in VGA (non-linear) and Native (linear) modes.  The BIOS ensures that memory is pre-allocated only when Internal graphics is enabled.
 - This register is also LT lockable.
 - Valid options are 0 (0x0) to 2048MB (0x40) in multiples of 32 MB
 - All other values are reserved
 - Hardware does not clear or set any of these bits automatically based on IGD being disabled/enabled.
 - BIOS Requirement: BIOS must not set this field to 0h if IVD (bit 1 of this register) is 0.
**/
#define N_SA_GGC_GMS_OFFSET  (0x8)
#define B_SA_GGC_GMS_MASK    (0xff00)

/**
 Description of GGMS (6:7)
 - This field is used to select the amount of Main Memory that is pre-allocated to support the Internal Graphics Translation Table.  The BIOS ensures that memory is pre-allocated only when Internal graphics is enabled.
 - GSM is assumed to be a contiguous physical DRAM space with DSM, and BIOS needs to allocate a contiguous memory chunk.  Hardware will derive the base of GSM from DSM only using the GSM size programmed in the register.
 - Valid options:
 - 0h: 0 MB of memory pre-allocated for GTT.
 - 1h: 2 MB of memory pre-allocated for GTT.
 - 2h: 4 MB of memory pre-allocated for GTT.
 - 3h: 8 MB of memory pre-allocated for GTT.
 - Others: Reserved
 - Hardware functionality in case of programming this value to Reserved is not guaranteed.
**/
#define N_SA_GGC_GGMS_OFFSET  (0x6)
#define B_SA_GGC_GGMS_MASK    (0xc0)
#define V_SA_GGC_GGMS_8MB     3

//  This register contains the base address of stolen DRAM memory for the GTT. BIOS determines the base of GTT stolen memory by subtracting the GTT graphics stolen memory size (PCI Device 0 offset 52 bits 9:8) from the Graphics Base of  Data Stolen Memory (PCI Device 0 offset B0 bits 31:20).
#define R_SA_BGSM        (0xb4)

//  This register contains the base address of TSEG DRAM memory. BIOS determines the base of TSEG memory which must be at or below Graphics Base of GTT Stolen Memory (PCI Device 0 Offset B4 bits 31:20).
#define R_SA_TSEGMB      (0xb8)

//  This Register contains the size of physical memory.
#define R_SA_TOM         (0xa0)
#define B_SA_TOM_MASK    (0x7ffff00000ULL)

//  This 64 bit register defines the Top of Upper Usable DRAM.
#define R_SA_TOUUD       (0xa8)
#define B_SA_TOUUD_MASK  (0x7ffff00000ULL)


//  This register contains the Top of low memory address.
#define R_SA_TOLUD       (0xbc)
#define B_SA_TOLUD_MASK  (0xfff00000)


#endif
