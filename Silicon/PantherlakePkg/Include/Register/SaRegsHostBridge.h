/**@file
  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SA_REGS_HOST_BRIDGE_H_
#define _SA_REGS_HOST_BRIDGE_H_

#define MCHBAR_HOSTBRIDGE_CFG_REG        0x48
#define R_IOC_CFG_DMI3DP_SBUSN           0x6DA8

#define SA_SEG_NUM              0x00

//
// DEVICE 0 (Memory Controller Hub)
//
#define SA_MC_BUS          0x00
#define SA_MC_DEV          0x00
#define SA_MC_FUN          0x00
#define R_SA_MC_DEVICE_ID  0x02


///
/// Maximum number of SDRAM channels supported by the memory controller
///
#define SA_MC_MAX_CHANNELS 2


///
/// Maximum number of DIMM sockets supported by each channel
///
#define SA_MC_MAX_SLOTS 2

///
/// Maximum number of sides supported per DIMM
///
#define SA_MC_MAX_SIDES 2

/**
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
**/


//
// PantherLake CPU mobile SA Device IDs B0:D0:F0
//
#define PTL_U_SA_DEVICE_ID_4C               0xB000   ///< PantherLake U            (4+0+4)      SA DID
#define PTL_H_12XE_SA_DEVICE_ID_4C_8A       0xB001   ///< PantherLake H 12Xe       (4+8+4+12Xe) SA DID
#define PTL_H_4XE_SA_DEVICE_ID_4C_8A        0xB002   ///< PantherLake H 4Xe        (4+8+4+4Xe)  SA DID
#define PTL_U_SA_DEVICE_ID_2C               0xB003   ///< PantherLake U            (2+0+4)      SA DID
#define PTL_H_12XE_SA_DEVICE_ID_4C_4A       0xB004   ///< PantherLake H 12Xe       (4+4+4+12Xe) SA DID
#define PTL_H_4XE_SA_DEVICE_ID_4C_4A        0xB005   ///< PantherLake H 4Xe        (4+4+4+4Xe)  SA DID
#define PTL_H_12XE_SA_DEVICE_ID_2C_8A       0xB009   ///< PantherLake H 12Xe       (2+8+4+12Xe) SA DID
#define PTL_H_12XE_SA_DEVICE_ID_2C_4A       0xB00A   ///< PantherLake H 12Xe       (2+4+4+12Xe) SA DID
#define PTL_H_4XE_SA_DEVICE_ID_2C_8A        0xB00B   ///< PantherLake H 4Xe        (2+8+4+4Xe)  SA DID
#define PTL_H_4XE_SA_DEVICE_ID_2C_4A        0xB00C   ///< PantherLake H 4Xe        (2+4+4+4Xe)  SA DID
#define PTL_U_NEX_SA_DEVICE_ID_4C           0xB007   ///< PantherLake U      (NEX) (4+0+4)      SA DID
#define PTL_H_12XE_NEX_SA_DEVICE_ID_4C_8A   0xB008   ///< PantherLake H 12Xe (NEX) (4+8+4+12Xe) SA DID
#define PTL_U_NEX_SA_DEVICE_ID_2C           0xB010   ///< PantherLake U      (NEX) (2+0+4)      SA DID
#define PTL_H_12XE_NEX_SA_DEVICE_ID_4C_4A   0xB011   ///< PantherLake H 12Xe (NEX) (4+4+4+12Xe) SA DID
#define PTL_H_12XE_NEX_SA_DEVICE_ID_2C_8A   0xB012   ///< PantherLake H 12Xe (NEX) (2+8+4+12Xe) SA DID
#define PTL_H_12XE_NEX_SA_DEVICE_ID_2C_4A   0xB013   ///< PantherLake H 12Xe (NEX) (2+4+4+12Xe) SA DID
#define PTL_H_4XE_NEX_SA_DEVICE_ID_4C_8A    0xB014   ///< PantherLake H 4Xe  (NEX) (4+8+4+4Xe)  SA DID
#define PTL_H_4XE_NEX_SA_DEVICE_ID_4C_4A    0xB015   ///< PantherLake H 4Xe  (NEX) (4+4+4+4Xe)  SA DID
#define PTL_H_4XE_NEX_SA_DEVICE_ID_2C_8A    0xB016   ///< PantherLake H 4Xe  (NEX) (2+8+4+4Xe)  SA DID
#define PTL_H_4XE_NEX_SA_DEVICE_ID_2C_4A    0xB017   ///< PantherLake H 4Xe  (NEX) (2+4+4+4Xe)  SA DID
#define PTL_U_NEX_SA_DEVICE_ID_1C           0xB028   ///< PantherLake U      (NEX) (1+0+4)      SA DID
#define PTL_H_4XE_NEX_SA_DEVICE_ID_1C       0xB029   ///< PantherLake H 4Xe  (NEX) (1+0+4+4Xe)  SA DID
#define PTL_H_4XE_NEX_SA_DEVICE_ID_4C       0xB02A   ///< PantherLake H 4Xe  (NEX) (4+0+4+4Xe)  SA DID

//
// WildcatLake CPU mobile SA Device IDs B0:D0:F0
//
#define WCL_SA_DEVICE_ID_2C_4LP             0xFD00   ///< WildcatLake (2+0+4) SA DID
#define WCL_SA_DEVICE_ID_2C_2LP             0xFD01   ///< WildcatLake (2+0+2) SA DID
#define WCL_SA_DEVICE_ID_2C                 0xFD02   ///< WildcatLake (2+0+0) SA DID
#define WCL_SA_DEVICE_ID_1C_4LP             0xFD03   ///< WildcatLake (1+0+4) SA DID
#define WCL_SA_DEVICE_ID_4LP                0xFD04   ///< WildcatLake (0+0+4) SA DID
#define WCL_SA_DEVICE_ID_2C_4LP_NEX         0xFD05   ///< WildcatLake (2+0+4) SA DID
#define WCL_SA_DEVICE_ID_1C_4LP_NEX         0xFD06   ///< WildcatLake (1+0+4) SA DID


/**
 <b>Description</b>:
 - This is the base address for the Host Memory Mapped Configuration space.  There is no physical memory within this 32KB window that can be addressed.  The 32KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space.  On reset, the Host MMIO Memory Mapped Configuation space is disabled and must be enabled by writing a 1 to MCHBAREN [Dev 0, offset48h, bit 0].
 - All the bits in this register are locked in LT mode.
 - The register space contains memory control, initialization, timing, and buffer strength registers; clocking registers; and power and thermal management registers.
**/
#define R_SA_MCHBAR  (0x48)
/**
 Description of MCHBAREN (0:0)
 - 0: MCHBAR is disabled and does not claim any memory
 - 1: MCHBAR memory mapped accesses are claimed and decoded appropriately
 - This register is locked by LT.
**/
/**
 Description of MCHBAR (15:38)
 - This field corresponds to bits 38 to 15 of the base address Host Memory Mapped configuration space.  BIOS will program this register resulting in a base address for a 32KB block of contiguous memory address space.  This register ensures that a naturally aligned 32KB space is allocated within the first 512GB of addressable memory space. System Software uses this base address to program the Host Memory Mapped register set. All the bits in this register are locked in LT mode.
**/


/**
 <b>Description</b>:
 - All the bits in this register are LT lockable.
**/
#define R_SA_GGC (0x50)
/**
 Description of GGCLCK (0:0)
 - When set to 1b, this bit will lock all bits in this register.
**/
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


/**
 Associating each PCIe typeC root port with a function number.
**/
#define R_SA_PCIEFUNCMAP  (0x7C)
/**
 Description of R_SA_PCIEFUNCMAP (0:2)
 - PCIE0 FUNCTION - This field is programmed by BIOS and indicates the function# the PCIE TypeC port is associated with.
**/
/**
 Description of R_SA_PCIEFUNCMAP (3:5)
 - PCIE1 FUNCTION - This field is programmed by BIOS and indicates the function# the PCIE TypeC port is associated with.
**/
/**
 Description of R_SA_PCIEFUNCMAP (6:8)
 - PCIE2 FUNCTION - This field is programmed by BIOS and indicates the function# the PCIE TypeC port is associated with.
**/
/**
 Description of R_SA_PCIEFUNCMAP (9:11)
 - PCIE3 FUNCTION - This field is programmed by BIOS and indicates the function# the PCIE TypeC port is associated with.
**/
/**
 Description of R_SA_PCIEFUNCMAP (12:14)
 - PCIE4 FUNCTION - This field is programmed by BIOS and indicates the function# the PCIE TypeC port is associated with.
**/
/**
 Description of R_SA_PCIEFUNCMAP (15:17)
 - PCIE5 FUNCTION - This field is programmed by BIOS and indicates the function# the PCIE TypeC port is associated with.
**/
/**
 Description of R_SA_PCIEFUNCMAP (18:20)
 - PCIE6 FUNCTION - This field is programmed by BIOS and indicates the function# the PCIE TypeC port is associated with.
**/
/**
 Description of R_SA_PCIEFUNCMAP (21:23)
 - PCIE7 FUNCTION - This field is programmed by BIOS and indicates the function# the PCIE TypeC port is associated with.
**/

#define PAM0_0_0_0_PCI_REG                                             (0x00000080)
#define PAM4_0_0_0_PCI_REG                                             (0x00000084)

/// Description:
///  This Register contains the size of physical memory.  BIOS determines the memory size reported to the OS using this Register.
///
#define R_SA_TOM (0xa0)
//  This Register contains the size of physical memory.
#define B_SA_TOM_MASK    (0x7ffff00000ULL)

///  This register contains the base address of stolen DRAM memory for the GTT. BIOS determines the base of GTT stolen memory by subtracting the GTT graphics stolen memory size (PCI Device 0 offset 52 bits 9:8) from the Graphics Base of  Data Stolen Memory (PCI Device 0 offset B0 bits 31:20).
///
#define R_SA_BGSM  (0xb4)

///
/// Description:
///  This register contains the base address of TSEG DRAM memory. BIOS determines the base of TSEG memory which must be at or below Graphics Base of GTT Stolen Memory (PCI Device 0 Offset B4 bits 31:20).
///
#define R_SA_TSEGMB  (0xb8)

///
/// Description:
///  This register contains the Top of low memory address.
///
#define R_SA_TOLUD (0xbc)
//  This register contains the Top of low memory address.
#define B_SA_TOLUD_MASK  (0xfff00000)

///
/// Description:
///  This 64 bit register defines the Top of Upper Usable DRAM.
///  Configuration software must set this value to TOM minus all EP stolen memory if reclaim is disabled.  If reclaim is enabled, this value must be set to reclaim limit + 1byte, 1MB aligned, since reclaim limit is 1MB aligned. Address bits 19:0 are assumed to be 000_0000h for the purposes of address comparison. The Host interface positively decodes an address towards DRAM if the incoming address is less than the value programmed in this register and greater than or equal to 4GB.
///  BIOS Restriction: Minimum value for TOUUD is 4GB.
///  These bits are LT lockable.
///
#define R_SA_TOUUD (0xa8)
//  This 64 bit register defines the Top of Upper Usable DRAM.
#define B_SA_TOUUD_MASK  (0x7ffff00000ULL)

#define R_SA_MC_CAPID0_A_OFFSET    0xE4

#endif
