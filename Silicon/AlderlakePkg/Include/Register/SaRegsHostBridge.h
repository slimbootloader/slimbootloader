/**@file
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

  Copyright (c) 2021-2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SA_REGS_HOST_BRIDGE_H_
#define _SA_REGS_HOST_BRIDGE_H_

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


//
// AlderLake CPU Desktop SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_DT_1       0x4660   ///< AlderLake Desktop (8+8+GT) SA DID
#define V_SA_DEVICE_ID_DT_2       0x4664   ///< AlderLake Desktop (8+6(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_3       0x4668   ///< AlderLake Desktop (8+4(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_4       0x466C   ///< AlderLake Desktop (8+2(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_5       0x4670   ///< AlderLake Desktop (8+0(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_6       0x4640   ///< AlderLake Desktop (6(f)+8+GT) SA DID
#define V_SA_DEVICE_ID_DT_7       0x4644   ///< AlderLake Desktop (6(f)+6(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_8       0x4648   ///< AlderLake Desktop (6(f)+4(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_9       0x464C   ///< AlderLake Desktop (6(f)+2(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_10      0x4650   ///< AlderLake Desktop (6+0+GT) SA DID
#define V_SA_DEVICE_ID_DT_11      0x4630   ///< AlderLake Desktop (4(f)+0+GT) SA DID
#define V_SA_DEVICE_ID_DT_12      0x4610   ///< AlderLake Desktop (2(f)+0+GT) SA DID
#define V_SA_DEVICE_ID_DT_13      0x4673   ///< AlderLake Desktop (8+6(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_15      0x4663   ///< AlderLake Desktop BGA (8+8(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_16      0x466B   ///< AlderLake Desktop BGA (8+4(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_17      0x4653   ///< AlderLake Desktop BGA (6+0(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_18      0x4633   ///< AlderLake Desktop BGA (4+0(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_19      0x4637   ///< AlderLake Mobile S BGA (8+8(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_20      0x463B   ///< AlderLake Mobile S BGA (6+8(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_21      0x4623   ///< AlderLake Mobile S BGA (4+8(f)+GT) SA DID
#define V_SA_DEVICE_ID_DT_22      0x462B   ///< AlderLake Mobile S BGA (4+4(f)+GT) SA DID

//
// AlderLake CPU Mobile SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_MB_ULT_1    0x4641   ///< AlderLake P (6+8+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_2    0x4649   ///< AlderLake P (6+4(f)+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_3    0x4621   ///< AlderLake P (4(f)+8+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_4    0x4609   ///< AlderLake P (2(f)+4(f)+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_5    0x4601   ///< AlderLake P (2+8+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_6    0x4661   ///< AlderLake P (6+8+2) SA DID
#define V_SA_DEVICE_ID_MB_ULT_7    0x4629   ///< AlderLake P (4+4+1) SA DID
#define V_SA_DEVICE_ID_MB_ULT_8    0x4619   ///< AlderLake P (1+4+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_9    0x4659   ///< AlderLake P (1+8+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_10   0x4645   ///< AlderLake P (6+6+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_11   0x4603   ///< AlderLake PS (2+8+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_12   0x4643   ///< AlderLake PS (6+8+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_13   0x4627   ///< AlderLake PS (4+8+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_14   0x460B   ///< AlderLake PS (2+4+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_15   0x464B   ///< AlderLake PS (4+4+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULT_16   0x467B   ///< AlderLake PS (1+4+GT) SA DID
#define V_SA_DEVICE_ID_MB_ULX_8    0x461B   ///< AlderLake N Pentium (0+4+0) SA DID
#define V_SA_DEVICE_ID_MB_ULX_9    0x461C   ///< AlderLake N Celeron (0+4+0) SA DID
#define V_SA_DEVICE_ID_MB_ULX_10   0x4678   ///< AlderLake N IOT_PC(0+4+0) 24EU SA DID
#define V_SA_DEVICE_ID_MB_ULX_11   0x4679   ///< AlderLake N IOT_EBM(0+4+0) 24EU SA DID
#define V_SA_DEVICE_ID_MB_ULX_12   0x467C   ///< AlderLake N IOT_EBM(0+2+0) 16EU SA DID
/**
 <b>Description</b>:
  This is the base address for the PCI Express Egress Port MMIO Configuration space.  There is no physical memory within this 4KB window that can be addressed.  The 4KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space.  On reset, the EGRESS port MMIO configuration space is disabled and must be enabled by writing a 1 to PXPEPBAREN [Dev 0, offset 40h, bit 0].
  All the bits in this register are locked in LT mode.
**/

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
 Description of IVD (1:1)
 - 0: Enable.  Device 2 (IGD) claims VGA memory and IO cycles, the Sub-Class Code within Device 2 Class Code register is 00.
 - 1: Disable.  Device 2 (IGD) does not claim VGA cycles (Mem and IO), and the Sub- Class Code field within Device 2 function 0 Class Code register is 80.
 - BIOS Requirement:  BIOS must not set this bit to 0 if the GMS field (bits 7:3 of this register) pre-allocates no memory.
 - This bit MUST be set to 1 if Device 2 is disabled either via a fuse or fuse override (CAPID0[46] = 1) or via a register (DEVEN[3] = 0).
 - This register is locked by LT lock.
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
 Description of VAMEN (2:2)
 - Enables the use of the iGFX enbines for Versatile Acceleration.
 - 1 - iGFX engines are in Versatile Acceleration Mode.  Device 2 Class Code is 048000h.
 - 0 - iGFX engines are in iGFX Mode.  Device 2 Class Code is 030000h.
**/

/**
 Description:
 - Allows for enabling/disabling of PCI devices and functions that are within the CPU package. The table below the bit definitions describes the behavior of all combinations of transactions to devices controlled by this register.
  All the bits in this register are LT Lockable.
**/

/**
 Description of D0EN (0:0)
 - Bus 0 Device 0 Function 0 may not be disabled and is therefore hardwired to 1.
**/
/**
 Description of D2EN (4:4)
 - 0:  Bus 0 Device 2 is disabled and hidden
 - 1:  Bus 0 Device 2 is enabled and visible
 - This bit will remain 0 if Device 2 capability is disabled.
**/

/**
 Description of D1F0EN (3:3)
 - 0: Bus 0 Device 1 Function 0 is disabled and not visible.
 - 1: Bus 0 Device 1 Function 0 is enabled and visible.
 - This bit will remain 0 if Device 1 Function 0 capability is disabled.
**/
/**
 Description of D1F1EN (2:2)
 - 0: Bus 0 Device 1 Function 1 is disabled and not visible.
 - 1: Bus 0 Device 1 Function 1 is enabled and visible.
 - This bit will remain 0 if Device 1 Function 1 capability is disabled.
**/
/**
 Description of D1F2EN (1:1)
 - 0: Bus 0 Device 1 Function 2 is disabled and not visible.
 - 1: Bus 0 Device 1 Function 2 is enabled and visible.
 - This bit will remain 0 if Device 1 Function 2 capability is disabled.
**/

// 2LM
/**
Description of D3F0EN (5:5) NVME
- 0: Bus 0 Device 3 Function 0 is disabled and not visible.
- 1: Bus 0 Device 3 Function 0 is enabled and visible.
- This bit will remain 0 if Device 3 Function 0 capability is disabled.
**/

// 2LM
/**
Description of D3F7EN (6:6) 2LM
- 0: Bus 0 Device 3 Function 7 is disabled and not visible.
- 1: Bus 0 Device 3 Function 7 is enabled and visible.
- This bit will remain 0 if Device 3 Function 7 capability is disabled.
**/

/**
 Description of D5EN (10:10)
 - 0: Bus 0 Device 5 is disabled and not visible.
 - 1: Bus 0 Device 5 is enabled and visible.
 - This bit will remain 0 if Device 5 capability is disabled.
**/
/**
 Description of D6EN (13:13)
 - 0: Bus 0 Device 6 is disabled and not visible.
 - 1: Bus 0 Device 6 is enabled and visible.
 - This bit will remain 0 if Device 6 capability is disabled.
**/
/**
 Description of D8EN (15:15)
 - 0: Bus 0 Device 8 is disabled and not visible.
 - 1: Bus 0 Device 8 is enabled and visible.
 - This bit will remain 0 if Device 8 capability is disabled.
**/
/**
 Description of D4EN (7:7)
 - 0: Bus 0 Device 4 is disabled and not visible.
 - 1: Bus 0 Device 4 is enabled and visible.
 - This bit will remain 0 if Device 4 capability is disabled.
 Update: DEVEN register change
**/

/**
 Description of D7EN (14:14)
 - 0: Bus 0 Device 7 is disabled and not visible.
 - 1: Bus 0 Device 7 is enabled and visible.
 - Non-production BIOS code should provide a setup option to enable Bus 0 Device 7.  When enabled, Bus 0 Device 7 must be initialized in accordance to standard PCI device initialization procedures.
 Update: DEVEN register change
**/

/**
Description of D14F0EN (14:14)
- 0: Bus 0 Device 14 Function 0 is disabled and not visible.
- 1: Bus 0 Device 14 Function 0 is enabled and visible.
- Non-production BIOS code should provide a setup option to enable Bus 0 Device 14 Function 0.  When enabled, Bus 0 Device 14 Function 0 must be initialized in accordance to standard PCI device initialization procedures.
Update: DEVEN register change
**/

/**
Description of SPARE11 (11:11)
Spare - bit 11
**/

/**
Description of D10F0EN (17:17)
- 0: Bus 0 Device 10 Function 0 is disabled and not visible.
- 1: Bus 0 Device 10 Function 0 is enabled and visible.
- Non-production BIOS code should provide a setup option to enable Bus 0 Device 10 Function 0.  When enabled, Bus 0 Device 10 Function 0 must be initialized in accordance to standard PCI device initialization procedures.
Update: DEVEN register change
**/
/**
Description of D9F0EN (12:12)
- 0: Bus 0 Device 9 Function 0 is disabled and not visible.
- 1: Bus 0 Device 9 Function 0 is enabled and visible.
- Non-production BIOS code should provide a setup option to enable Bus 0 Device 9 Function 0.  When enabled, Bus 0 Device 14 Function 0 must be initialized in accordance to standard PCI device initialization procedures.
Update: DEVEN register change
**/

/**
 Description of ITH (12:12)
 - 0: Intel Trace Hub is disabled.
 - 1: Intel Trace Hub is enabled.
 Update: DEVEN register change
**/

/**
 Description of EPBAREN (27:27)
 - 0:  EPBAR is disabled and does not claim any memory.
 - 1:  EPBAR memory mapped accesses are claimed and decoded approprately. This bit is Intel Reserved
**/
/**
 Description of MCHBAREN (28:28)
 - 0:  MCHBAR is disabled and does not claim any memory.
 - 1:  MCHBAR memory mapped accesses are claimed and decoded approprately. This bit is Intel Reserved
**/
/**
 Description of DMIBAREN (29:29)
 - 0:  DMIBAR is disabled and does not claim any memory.
 - 1:  DMIBAR memory mapped accesses are claimed and decoded approprately. This bit is Intel Reserved
**/
/**
 Description of PCIEXBAREN (31:31)
 - 0:  The PCIEXBAR register is disabled. Memory read and write transactions proceed as if there were no PCIEXBAR register. PCIEXBAR bits 31:28 are R/W with no functionality behind them.
 - 1:  The PCIEXBAR register is enabled. Memory read and write transactions whose address bits 31:28 match PCIEXBAR 31:28 will be translated to configuration reads and writes within the GMCH. These translated cycles are routed as shown in the table above.
**/
/**
 Description of RSVD (31:31)
**/

/**
 Description
 - Protected Audio Video Path Control
 - All the bits in this register are locked by LT.  When locked the R/W bits are RO.
**/


/**
 Description of PCME (0:0)
 - This field enables Protected Content Memory within Graphics Stolen Memory.
 - This register is locked (becomes read-only) when PAVPLCK = 1b.
 - This register is read-only (stays at 0b) when PAVP fuse is set to "disabled"
 - 0: Protected Content Memory is disabled
 - 1: Protected Content Memory is enabled
**/
/**
 Description of PAVPE (1:1)
 - 0: PAVP path is disabled
 - 1: PAVP path is enabled
 - This register is locked (becomes read-only) when PAVPLCK = 1b
 - This register is read-only (stays at 0b) when PAVP capability is set to "disabled" as defined by CAPID0_B[PAVPE].
**/
/**
 Description of PAVPLCK (2:2)
 - This bit will lock all writeable contents in this register when set (including itself).
 - This bit will be locked if PAVP is fused off.
**/
/**
 Description of HVYMODSEL (3:3)
 - 0b   Lite Mode
**/

/**
 Description of WOPCMSZ (8:7)
 - 00b - 1MB (default)
**/

/**
 Description of PCMBASE (20:31)
 - This field is used to set the base of Protected Content Memory.
 - This corresponds to bits 31:20 of the system memory address range, giving a 1MB granularity. This value MUST be at least 1MB above the base and below the top of stolen memory.
 - This register is locked (becomes read-only) when PAVPE = 1b.
**/

/**
 Description of LOCK (0:0)
 - This bit will lock all writeable settings in this register, including itself.
**/
/**
 Description of PRS (1:1)
 - This field indicates the status of DPR.
 - 0: DPR protection disabled
 - 1: DPR protection enabled
**/
/**
 Description of EPM (2:2)
 - This field controls DMA accesses to the DMA Protected Range (DPR) region.
 - 0: DPR is disabled
 - 1: DPR is enabled.  All DMA requests accessing DPR region are blocked.
 - HW reports the status of DPR enable/disable through the PRS field in this register.
**/
/**
 Description of DPRSIZE (11:4)
 - This field is used to specify the size of memory protected from DMA access in MB
 - The maximum amount of memory that will be protected is 255MB
 - The Top of protected range is the base of TSEG-1
**/
/**
 Description of TOPOFDPR (31:20)
 - This is the Top address 1 of DPR - Base of TSEG
**/

/**
 Description:
  This is the base address for the PCI Express configuration space.  This window of addresses contains the 4KB of configuration space for each PCI Express device that can potentially be part of the PCI Express Hierarchy associated with the Uncore.  There is no actual physical memory within this window of up to 256MB that can be addressed.  The actual size of this range is determined by a field in this register.
  Each PCI Express Hierarchy requires a PCI Express BASE register.  The Uncore supports one PCI Express Hierarchy.  The region reserved by this register does not alias to any PCI2.3 compliant memory mapped space.  For example, the range reserved for MCHBAR is outside of PCIEXBAR space.
  On reset, this register is disabled and must be enabled by writing a 1 to the enable field in this register.  This base address shall be assigned on a boundary consistent with the number of buses (defined by the length field in this register), above TOLUD and still within 39-bit addressable memory space.
  The PCI Express Base Address cannot be less than the maximum address written to the Top of physical memory register (TOLUD).  Software must guarantee that these ranges do not overlap with known ranges located above TOLUD.
  Software must ensure that the sum of the length of the enhanced configuration region + TOLUD + any other known ranges reserved above TOLUD is not greater than the 39-bit addessable limit of 512GB.  In general, system implementation and the number of PCI/PCI Express/PCI-X buses supported in the hierarchy will dictate the length of the region.
  All the bits in this register are locked in LT mode.
**/
/**
 Description of PCIEXBAREN (0:0)
 - 0: The PCIEXBAR register is disabled.  Memory read and write transactions proceed s if there were no PCIEXBAR register.  PCIEXBAR bits 38:26 are R/W with no functionality behind them.
 - 1: The PCIEXBAR register is enabled.  Memory read and write transactions whose address bits 38:26 match PCIEXBAR will be translated to configuration reads and writes within the Uncore.  These Translated cycles are routed as shown in the above table.
 - This register is locked by LT.
**/
/**
 Description of LENGTH (1:3)
 - This field describes the length of this region.
 - 00: 256MB (buses 0-255).  Bits 38:28 are decoded in the PCI Express Base Address Field.
 - 01: 128MB (buses 0-127).  Bits 38:27 are decoded in the PCI Express Base Address Field.
 - 10: 64MB (buses 0-63).  Bits 38:26 are decoded in the PCI Express Base Address Field.
 - 11: 512MB (buses 0-255)(Segment0 + Segment1).  Bits 38:29 are decoded in the PCI Express Base Address Field.
 - This register is locked by LT.
**/

/**
 Description of ADMSK64 (26:26)
  This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register.
**/
/**
 Description of ADMSK128 (27:27)
  This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register.
**/

/**
Description of ADMSK256 (28:28)
This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register.
**/
/**
Description of ADMSK512 (29:29)
This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register.
**/
/**
Description of ADMSK512 (30:30)
This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register.
**/

/**
 Description of PCIEXBAR (31:38)
 - This field corresponds to bits 38 to 31 of the base address for PCI Express enhanced configuration space.  BIOS will program this register resulting in a base address for a contiguous memory address space.  The size of the range is defined by bits [2:1] of this register.
 - This Base address shall be assigned on a boundary consistent with the number of buses (defined by the Length field in this register) above TOLUD and still within the 39-bit addressable memory space.  The address bits decoded depend on the length of the region defined by this register.
 - This register is locked by LT.
 - The address used to access the PCI Express configuration space for a specific device can be determined as follows:
 - PCI Express Base Address + Bus Number * 1MB + Device Number * 32KB + Function Number * 4KB
 - This address is the beginning of the 4KB space that contains both the PCI compatible configuration space and the PCI Express extended configuration space.
**/

/**
 This is the base address for the Root Complex configuration space. This window of addresses contains the Root Complex Register set for the PCI Express Hierarchy associated with the Host Bridge. There is no physical memory within this 4KB window that can be addressed. The 4KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space. On reset, the Root Complex configuration space is disabled and must be enabled by writing a 1 to DMIBAREN [Dev 0, offset 68h, bit 0] All the bits in this register are locked in LT mode.
**/
/**
 Description of DMIBAREN (0:0)
 - 0: DMIBAR is disabled and does not claim any memory
 - 1: DMIBAR memory mapped accesses are claimed and decoded appropriately
 - This register is locked by LT.
**/
/**
 Description of DMIBAR (12:38)
 - This field corresponds to bits 38 to 12 of the base address DMI configuration space. BIOS will program this register resulting in a base address for a 4KB block of contiguous memory address space. This register ensures that a naturally aligned 4KB space is allocated within the first 512GB of addressable memory space. System Software uses this base address to program the DMI register set. All the Bits in this register are locked in LT mode.
**/

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

#ifdef CPU_CFL
/**
 Description:
 - This register determines the Base Address register of the memory range that is pre-allocated to the Manageability Engine.  Together with the MESEG_MASK register it controls the amount of memory allocated to the ME.
 - This register must be initialized by the configuration software.  For the purpose of address decode address bits A[19:0] are assumed to be 0. Thus, the bottom of the defined memory address range will be aligned to a 1MB boundary.
 - This register is locked by LT.
**/
/**
 Description of MEBASE (20:38)
 - Corresponds to A[38:20] of the base address memory range that is allocated to the ME.
**/

/**
 Description:
 - This register determines the Mask Address register of the memory range that is pre-allocated to the Manageability Engine.  Together with the MESEG_BASE register it controls the amount of memory allocated to the ME.
 - This register is locked by LT.
**/
/**
 Description of MELCK (10:10)
 - This field indicates whether all bits in the MESEG_BASE and MESEG_MASK registers are locked.  When locked, updates to any field for these registers must be dropped.
**/
/**
 Description of ME_STLEN_EN (11:11)
 - Indicates whether the ME stolen  Memory range is enabled or not.
**/
/**
 Description of MEMASK (20:38)
 - This field indicates the bits that must match MEBASE in order to qualify as an ME Memory Range access.
 - For example, if the field is set to 7FFFFh, then ME Memory is 1MB in size.
 - Another example is that if the field is set to 7FFFEh, then ME Memory is 2MB in size.
 - In other words, the size of ME Memory Range is limited to power of 2 times 1MB.
**/
#endif

/**
 Description:
 - This register controls the read, write and shadowing attributes of the BIOS range from F_0000h to F_FFFFh.  The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range.  Seven Programmable Attribute Map (PAM) registers are used to support these features.  Cacheability of these areas is controlled via the MTRR register in the core.
 - Two bits are used to specify memory attributes for each memory segment.  These bits apply to host accesses to the PAM areas.  These attributes are:
 - RE - Read Enable.  When RE=1, the host read accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when RE=0, the host read accesses are directed to DMI.
 - WE - Write Enable.  When WE=1, the host write accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when WE=0, the host read accesses are directed to DMI.
 - The RE and WE attributes permit a memory segment to be Read Only, Write Only, Read/Write or Disabled.  For example, if a memory segment has RE=1 and WE=0, the segment is Read Only.
**/


///
/// Description:
///
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

//
// MCHBAR IO Register Offset Equates
//
/**
 TCSS_DEVEN register
 Description of PCIE0 (0:0)
 PCIE0 Enable - 0: PCIE0 is disabled and hidden. 1: PCIE0 is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of PCIE1 (1:1)
 PCIE1 Enable - 0: PCIE1 is disabled and hidden. 1: PCIE1 is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of PCIE2 (2:2)
 PCIE2 Enable - 0: PCIE2 is disabled and hidden. 1: PCIE2 is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of PCIE3 (3:3)
 PCIE3 Enable - 0: PCIE3 is disabled and hidden. 1: PCIE3 is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of PCIE4 (4:4)
 PCIE4 Enable - 0: PCIE4 is disabled and hidden. 1: PCIE4 is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of PCIE5 (5:5)
 PCIE5 Enable - 0: PCIE5 is disabled and hidden. 1: PCIE5 is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of XHCI (:8)
 XHCI Enable - 0: XHCI is disabled and hidden. 1: XHCI is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of XDCI (9:9)
 xDCI Enable - 0: xDCI is disabled and hidden. 1: xDCI is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of DMA0 (10:10)
 DMA0 Enable - 0: DMA0 is disabled and hidden. 1: DMA0 is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of DMA1 (11:11)
 DMA1 Enable - 0: DMA1 is disabled and hidden. 1: DMA1 is enabled and visible.
**/
/**
 TCSS_DEVEN register
 Description of DMA2 (12:12)
 DMA2 Enable - 0: DMA2 is disabled and hidden. 1: DMA2 is enabled and visible.
**/


//Description:
//  This register enables various errors to generate an SMI DMI special cycle. When an error flag is set in the ERRSTS register, it can generate an SERR, SMI, or SCI DMI special cycle when enabled in the ERRCMD, SMICMD, or SCICMD registers, respectively. Note that one and only one message type can be enabled.
#ifndef SCICMD
#define SCICMD (0xce)
#endif//SCICMD


///
/// VCU Miscellaneous Controls
///
#ifndef SLE_FLAG
#define V_SA_VCU_STATUS_BUSY_LIMIT     100
#define V_SA_VCU_RESPONSE_RETRY_LIMIT  10
#else
#define V_SA_VCU_STATUS_BUSY_LIMIT     1
#define V_SA_VCU_RESPONSE_RETRY_LIMIT  1
#endif
///
/// Miscellaneous 64bit Masks
/// Description of GMADRBAR bit (38:27)
/// Description of VTDBAR bit (38:12)
///
#endif
