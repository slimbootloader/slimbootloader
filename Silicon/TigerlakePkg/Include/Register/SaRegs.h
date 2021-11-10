/** @file
  Register names for System Agent (SA) registers
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

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SA_REGS_H_
#define _SA_REGS_H_

#include <Register/SaRegsIgd.h>
#include <Register/SaRegsPeg.h>
#include <Register/SaRegsIpu.h>
#include <Register/SaRegsGna.h>

//
// DEVICE 0 (Memory Controller Hub)
//
#define SA_MC_BUS          0x00
#define SA_MC_DEV          0x00
#define SA_MC_FUN          0x00

//
// TigerLake Mobile SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_MB_ULT_1 0x9A14   ///< TigerLake Ult (TGL-U 4+2)
#define V_SA_DEVICE_ID_MB_ULT_2 0x9A04   ///< TigerLake Ult (TGL-U 2+2)

#define V_SA_DEVICE_ID_MB_ULX_1 0x9A12   ///< TigerLake Ulx (TGL-Y 4+2)
#define V_SA_DEVICE_ID_MB_ULX_2 0x9A02   ///< TigerLake Ulx (TGL-Y 2+2)

//
// TigerLake CPU Halo SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_HALO_1   0x9A28   ///< TigerLake Halo (6+1) SA DID
#define V_SA_DEVICE_ID_HALO_2   0x9A38   ///< TigerLake Halo (8+2) SA DID

//
// TigerLake CPU Desktop SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_DT_1     0x9A2C   ///< TigerLake Desktop (6+1) SA DID
#define V_SA_DEVICE_ID_DT_2     0x9A1C   ///< TigerLake Desktop (4+1) SA DID

/**
 <b>Description</b>:
 - This is the base address for the Host Memory Mapped Configuration space.  There is no physical memory within this 32KB window that can be addressed.  The 32KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space.  On reset, the Host MMIO Memory Mapped Configuation space is disabled and must be enabled by writing a 1 to MCHBAREN [Dev 0, offset48h, bit 0].
 - All the bits in this register are locked in LT mode.
 - The register space contains memory control, initialization, timing, and buffer strength registers; clocking registers; and power and thermal management registers.
**/
#define R_SA_MCHBAR  (0x48)

///
/// Description of TOLUD (20:31)
///  This register contains bits 31 to 20 of an address one byte above the maximum DRAM memory below 4G that is usable by the operating system. Address bits 31 down to 20 programmed to 01h implies a minimum memory size of 1MB. Configuration software must set this value to the smaller of the following 2 choices: maximum amount memory in the system minus ME stolen memory plus one byte or the minimum address allocated for PCI memory. Address bits 19:0 are assumed to be 0_0000h for the purposes of address comparison. The Host interface positively decodes an address towards DRAM if the incoming address is less than the value programmed in this register.
///  The Top of Low Usable DRAM is the lowest address above both Graphics Stolen memory and Tseg. BIOS determines the base of Graphics Stolen Memory by subtracting the Graphics Stolen Memory Size from TOLUD and further decrements by Tseg size to determine base of Tseg. All the Bits in this register are locked in LT mode.
///  This register must be 1MB aligned when reclaim is enabled.
///
#define R_SA_MC_CAPID0_A_OFFSET    0xE4

#endif
