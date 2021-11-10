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

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _SA_REGS_HOST_BRIDGE_H_
#define _SA_REGS_HOST_BRIDGE_H_

//
// DEVICE 0 (Memory Controller Hub)
//
#define SA_MC_BUS          0x00
#define SA_MC_DEV          0x00
#define SA_MC_FUN          0x00

//
// ElkhartLake Mobile SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_EHL_0        0x4500   ///< Super SKU1

#define V_SA_DEVICE_ID_EHL_1        0x4510   ///< Super SKU2
#define V_SA_DEVICE_ID_EHL_2        0x4512   ///< SKU 2 CORE
#define V_SA_DEVICE_ID_EHL_3        0x4514   ///< SKU 2 CORE
#define V_SA_DEVICE_ID_EHL_4        0x4516   ///< SKU 2 CORE
#define V_SA_DEVICE_ID_EHL_5        0x4518   ///< SKU 2 CORE
#define V_SA_DEVICE_ID_EHL_6        0x451A   ///< SKU 2 CORE
#define V_SA_DEVICE_ID_EHL_7        0x451C   ///< SKU 2 CORE
#define V_SA_DEVICE_ID_EHL_8        0x451E   ///< SKU 2 CORE

#define V_SA_DEVICE_ID_EHL_9        0x4520   ///< Super SKU3
#define V_SA_DEVICE_ID_EHL_10       0x4522   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_11       0x4524   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_12       0x4526   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_13       0x4528   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_14       0x452A   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_15       0x452C   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_16       0x452E   ///< SKU 4 CORE

#define V_SA_DEVICE_ID_EHL_17       0x4530   ///< Super SKU4
#define V_SA_DEVICE_ID_EHL_18       0x4532   ///< SKU 4 CORE (FUSA)
#define V_SA_DEVICE_ID_EHL_19       0x4534   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_20       0x4536   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_21       0x4538   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_22       0x453A   ///< SKU 4 CORE
#define V_SA_DEVICE_ID_EHL_23       0x453C   ///< SKU 4 CORE

/**
 <b>Description</b>:
 - This is the base address for the Host Memory Mapped Configuration space.  There is no physical memory within this 32KB window that can be addressed.  The 32KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space.  On reset, the Host MMIO Memory Mapped Configuation space is disabled and must be enabled by writing a 1 to MCHBAREN [Dev 0, offset48h, bit 0].
 - All the bits in this register are locked in LT mode.
 - The register space contains memory control, initialization, timing, and buffer strength registers; clocking registers; and power and thermal management registers.
**/
#define R_SA_MCHBAR  (0x48)

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


//  This Register contains the size of physical memory.
#define R_SA_TOM         (0xa0)
#define B_SA_TOM_MASK    (0x7ffff00000ULL)

//  This 64 bit register defines the Top of Upper Usable DRAM.
#define R_SA_TOUUD       (0xa8)
#define B_SA_TOUUD_MASK  (0x7ffff00000ULL)


//  This register contains the Top of low memory address.
#define R_SA_TOLUD       (0xbc)
#define B_SA_TOLUD_MASK  (0xfff00000)


#define R_SA_MC_CAPID0_A_OFFSET    0xE4

#endif
