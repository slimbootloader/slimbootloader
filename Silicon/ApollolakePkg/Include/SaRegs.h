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
    "_SA_[generation_name]_" in register/bit names. e.g., "_SA_BXT_"
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SA generation will be just named
    as "_SA_" without [generation_name] inserted.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SA_REGS_H_
#define _SA_REGS_H_

/*
< Extended Configuration Base Address.*/

#define MCHBAR_RANGE_PUNIT             0x7000
#define BUNIT_BMISC_MCHBAR_OFFSET      0x6800
#define BSMR_SMM_OPEN_FOR_IA           0x14

//
// Default Vendor ID and Subsystem ID
//
#define V_INTEL_VENDOR_ID 0x8086      ///< Default Intel Vendor ID
#define V_SA_DEFAULT_SID  0x7270      ///< Default Intel Subsystem ID

//
// DEVICE 0 (Memory Controller Hub)
//
#define SA_MC_BUS                      0x00
#define SA_MC_DEV                      0x00
#define SA_MC_FUN                      0x00
#define R_SA_MC_VENDOR_ID              0x00
#define R_SA_MC_DEVICE_ID              0x02
#define R_SA_MC_REVISION_ID            0x08
#define V_SA_MC_VID                    0x8086
#define V_SA_MC_DID0                   0x0AF0  //Broxton
#define V_SA_MC_DID1                   0x1AF0  //Broxton1
#define V_SA_MC_DID2                   0x4AF0  //Broxton-X
#define V_SA_MC_DID3                   0x5AF0  //Broxton-P
#define R_SA_MC_CAPID0_A               0xE4
#define R_SA_MC_CAPID0_B               0xE8
#define R_SA_MCHBAR_REG                0x48

//
// Silicon Steppings
//
#define V_SA_MC_RID_0                  0x00
#define V_SA_MC_RID_1                  0x01
#define V_SA_MC_RID_3                  0x03
#define V_SA_MC_RID_4                  0x04
#define V_SA_MC_RID_5                  0x05
#define V_SA_MC_RID_6                  0x06
#define V_SA_MC_RID_7                  0x07
#define V_SA_MC_RID_8                  0x08
#define V_SA_MC_RID_9                  0x09
#define V_SA_MC_RID_A                  0x0A
#define V_SA_MC_RID_B                  0x0B
#define V_SA_MC_RID_C                  0x0C

///
/// Maximum number of SDRAM channels supported by the memory controller
///
#define SA_MC_MAX_CHANNELS 4
///
/// Maximum number of DIMM sockets supported by each channel
///
#define SA_MC_MAX_SLOTS 1

///
/// Maximum number of sides supported per DIMM
///
#define SA_MC_MAX_SIDES 2

///
/// Maximum number of DIMM sockets supported by the memory controller
///
#define SA_MC_MAX_SOCKETS (SA_MC_MAX_CHANNELS * SA_MC_MAX_SLOTS)

///
/// Maximum number of rows supported by the memory controller
///
#define SA_MC_MAX_RANKS (SA_MC_MAX_SOCKETS * SA_MC_MAX_SIDES)

///
/// Maximum number of rows supported by the memory controller
///
#define SA_MC_MAX_ROWS (SA_MC_MAX_SIDES * SA_MC_MAX_SOCKETS)

///
/// Maximum memory supported by the memory controller
/// 4 GB in terms of KB
///
#define SA_MC_MAX_MEM_CAPACITY (4 * 1024 * 1024)

///
/// Define the SPD Address for DIMM 0
///
#define SA_MC_DIMM0_SPD_ADDRESS 0xA0

///
/// Define the maximum number of data bytes on a system with no ECC memory support.
///
#define SA_MC_MAX_BYTES_NO_ECC (8)

///
/// Define the maximum number of SPD data bytes on a DIMM.
///
#define SA_MC_MAX_SPD_SIZE (512)

/**
 <b>Description</b>:
 - GMCH Graphics Control Register
**/
#define R_SA_GGC (0x50)
/**
 Description of GGCLCK (0:0)
 - When set to 1b, this bit will lock all bits in this register.
**/
#define N_SA_GGC_GGCLCK_OFFSET   (0x0)
#define S_SA_GGC_GGCLCK_WIDTH    (0x1)
#define B_SA_GGC_GGCLCK_MASK     (0x1)
#define V_SA_GGC_GGCLCK_DEFAULT  (0x0)
/**
 Description of IVD (1:1)
 - 0: Enable.  Device 2 (IGD) claims VGA memory and IO cycles, the Sub-Class Code within Device 2 Class Code register is 00.
 - 1: Disable.  Device 2 (IGD) does not claim VGA cycles (Mem and IO), and the Sub- Class Code field within Device 2 function 0 Class Code register is 80.
 - BIOS Requirement:  BIOS must not set this bit to 0 if the GMS field (bits 7:3 of this register) pre-allocates no memory.
 - This bit MUST be set to 1 if Device 2 is disabled disabled DEVEN_0_0_0_PCI.D2F0EN 0.
 **/
#define N_SA_GGC_IVD_OFFSET      (0x1)
#define S_SA_GGC_IVD_WIDTH       (0x1)
#define B_SA_GGC_IVD_MASK        (0x2)
#define V_SA_GGC_IVD_DEFAULT     (0x0)
/**
 Description of VAMEM (2:2) Enables the use of the iGFX enbines for Versatile Acceleration.
 - 1  iGFX engines are in Versatile Acceleration Mode. Device 2 Class Code is 048000h.
 - 0  iGFX engines are in iGFX Mode. Device 2 Class Code is 030000h.
**/
#define N_SA_GGC_VAMEM_OFFSET    (0x2)
#define S_SA_GGC_VAMEM_WIDTH     (0x1)
#define B_SA_GGC_VAMEM_MASK      (0x4)
#define V_SA_GGC_VAMEM_DEFAULT   (0x0)
/**
 Description of RSVD0 (5:3)
**/
#define N_SA_GCC_RSVD0_OFFSET    (0x3)
#define S_SA_GCC_RSVD0_WIDTH     (0x3)
#define B_SA_GCC_RSVD0_MASK      (0x38)
#define V_SA_GCC_RSVD0_DEFAULT   (0x0)
/**
  Description of GGMS (7:6)
 - This field is used to select the amount of Main Memory that is pre-allocated to support the Internal Graphics Translation Table.  The BIOS ensures that memory is pre-allocated only when Internal graphics is enabled.
 - GSM is assumed to be a contiguous physical DRAM space with DSM, and BIOS needs to allocate a contiguous memory chunk.  Hardware will derive the base of GSM from DSM only using the GSM size programmed in the register.
 - Valid options:
 - 0h: 0 MB of memory pre-allocated for GTT.
 - 1h: 2 MB of memory pre-allocated for GTT.
 - 2h: 4 MB of memory pre-allocated for GTT.
 - 3h: 8 MB of memory pre-allocated for GTT.
**/
#define N_SA_GGC_GGMS_OFFSET  (0x6)
#define S_SA_GGC_GGMS_WIDTH   (0x2)
#define B_SA_GGC_GGMS_MASK    (0xc0)
#define V_SA_GGC_GGMS_DEFAULT (0x0)
#define V_SA_GGC_GGMS_DIS     0
#define V_SA_GGC_GGMS_2MB     1
#define V_SA_GGC_GGMS_4MB     2
#define V_SA_GGC_GGMS_8MB     3
/**
 Description of GMS (15:8)
 - This field is used to select the amount of Main Memory that is pre-allocated to support the Internal Graphics device in VGA (non-linear) and Native (linear) modes.  The BIOS ensures that memory is pre-allocated only when Internal graphics is enabled.
 - This register is also LT lockable.
 - Valid options are 0 (0x0) to 2016MB (0x3F) in multiples of 32 MB
 - Default is 32MB
 - All other values are reserved
 - Hardware does not clear or set any of these bits automatically based on IGD being disabled/enabled.
 - BIOS Requirement: BIOS must not set this field to 0h if IVD (bit 1 of this register) is 0.
**/
#define N_SA_GGC_GMS_OFFSET  (0x8)
#define S_SA_GGC_GMS_WIDTH   (0x8)
#define B_SA_GGC_GMS_MASK    (0xff00)
#define V_SA_GGC_GMS_DEFAULT (0x01)
#define V_SA_GGC_GMS_2016MB  0x3F
/**
 Description of RSVD1 (31:16)
**/
#define N_SA_GCC_RSVD1_OFFSET    (0x10)
#define S_SA_GCC_RSVD1_WIDTH     (0xff)
#define B_SA_GCC_RSVD1_MASK      (0xffff0000)
#define V_SA_GCC_RSVD1_DEFAULT   (0x0)


/**
 Description:
 - Allows for enabling/disabling of PCI devices and functions that are within the CPU package. The table below the bit definitions describes the behavior of all combinations of transactions to devices controlled by this register.
  All the bits in this register are LT Lockable.
**/
#define R_SA_DEVEN (0x54)
/**
 Description of D0F0EN (0:0)
 - Bus 0 Device 0 Function 0 may not be disabled and is therefore hardwired to 1.
**/
#define N_SA_DEVEN_D0F0EN_OFFSET   (0x0)
#define S_SA_DEVEN_D0F0EN_WIDTH    (0x1)
#define B_SA_DEVEN_D0F0EN_MASK     (0x1)
#define V_SA_DEVEN_D0F0EN_DEFAULT  (0x1)
/**
 Description of D0F1EN (1:1)
 - 0: Bus 0 Device 1 Function 2 is disabled and hidden.
 - 1: Bus 0 Device 1 Function 2 is enabled and visible.
 - This bit will remain 0 if PEG12 capability is disabled.
**/
#define N_SA_DEVEN_D0F1EN_OFFSET   (0x1)
#define S_SA_DEVEN_D0F1EN_WIDTH    (0x1)
#define B_SA_DEVEN_D0F1EN_MASK     (0x2)
#define V_SA_DEVEN_D0F1EN_DEFAULT  (0x2)
/**
 Description of RSVD0 (2:2)
**/
#define N_SA_DEVEN_RSVD0_OFFSET    (0x2)
#define S_SA_DEVEN_RSVD0_WIDTH     (0x1)
#define B_SA_DEVEN_RSVD0_MASK      (0x4)
#define V_SA_DEVEN_RSVD0_DEFAULT   (0x0)
/**
 Description of RSVD (3:3)
**/
#define N_SA_DEVEN_RSVD_OFFSET     (0x3)
#define S_SA_DEVEN_RSVD_WIDTH      (0x1)
#define B_SA_DEVEN_RSVD_MASK       (0x8)
#define V_SA_DEVEN_RSVD_DEFAULT    (0x0)
/**
 Description of D2F0EN (4:4)
 - 0:  Bus 0 Device 2 is disabled and hidden
 - 1:  Bus 0 Device 2 is enabled and visible
 - This bit will remain 0 if Device 2 capability is disabled.
**/
#define N_SA_DEVEN_D2F0EN_OFFSET   (0x4)
#define S_SA_DEVEN_D2F0EN_WIDTH    (0x1)
#define B_SA_DEVEN_D2F0EN_MASK     (0x10)
#define V_SA_DEVEN_D2F0EN_DEFAULT  (0x10)
/**
 Description of D3F0EN (5:5)
 - 0:  Bus 0 Device 3 is disabled and hidden
 - 1:  Bus 0 Device 3 is enabled and visible
 - This bit will remain 0 if Device 3 capability is disabled.
**/
#define N_SA_DEVEN_D3F0EN_OFFSET   (0x5)
#define S_SA_DEVEN_D3F0EN_WIDTH    (0x1)
#define B_SA_DEVEN_D3F0EN_MASK     (0x20)
#define V_SA_DEVEN_D3F0EN_DEFAULT  (0x20)
/**
 Description of RSVD1 (31:6)
**/
#define N_SA_DEVEN_RSVD1_OFFSET    (0x6)
#define S_SA_DEVEN_RSVD1_WIDTH     (0x1A)
#define B_SA_DEVEN_RSVD1_MASK      (0xFFFFFFC0)
#define V_SA_DEVEN_RSVD1_DEFAULT   (0x0)

#define R_SA_TOLUD (0xbc)

#define CUNIT_MCHBAR_REG 0x48
#define PUNIT_MAILBOX_DATA 0x80
#define PUNIT_MAILBOX_INTERFACE 0x84
#define SA_IGD_BUS              0x00
#define SA_IGD_DEV              0x02
#define SA_IGD_FUN_0            0x00
#define SA_IGD_FUN_1            0x01
#define SA_IGD_DEV_FUN          (SA_IGD_DEV << 3)
#define SA_IGD_BUS_DEV_FUN      (SA_MC_BUS << 8) + SA_IGD_DEV_FUN

#define R_SA_IGD_VID            0x00
#define V_SA_IGD_VID            0x8086
#define V_SA_IGD_DID            0x0A84
#define V_SA_IGD_DID_BXTP       0x5A84 // 18EU SKU
#define V_SA_IGD_DID_BXTP_1     0x5A85 // 12EU SKU
#define R_SA_IGD_CMD            0x04
#define R_SA_IGD_GTTMMADR       0x10
#define R_SA_IGD_GMADR          0x18
#define R_SA_IGD_MSAC_OFFSET    0x62  ///< Multisize Aperture Control

#define IGD_SWSCI_OFFSET        0x00E0
#define IGD_ASLS_OFFSET         0x00FC

#define  TSEG                   0xb8    // TSEG base
#define  BGSM                   0xb4    // Base GTT Stolen Memory

//  This 64 bit register defines the Top of Upper Usable DRAM.
#define R_SA_TOUUD              (0xa8)
#define B_SA_TOUUD_MASK         (0x7ffff00000ULL)

//  This register contains the Top of low memory address.
#define R_SA_TOLUD              (0xbc)
#define B_SA_TOLUD_MASK         (0xfff00000)

#endif
