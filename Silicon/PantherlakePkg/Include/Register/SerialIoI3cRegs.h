/** @file
  Register names for SERIAL_IO_I3C

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

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SERIALIOI3CREGS_H_
#define _SERIALIOI3CREGS_H_

//
// I3C0 registers
//

#define R_SERIAL_IO_I3C0_MEM_QUEUE_THLD_CTRL              0xD0            ///< REG QUEUE_THLD_CTRL
#define R_SERIAL_IO_I3C0_MEM_DATA_BUFFER_THLD_CTRL        0xD4U           ///< REG DATA_BUFFER_THLD_CTRL
#define R_SERIAL_IO_I3C0_MEM_SCL_I3C_OD_TIMING            0x214U          ///< REG SCL_I3C_OD_TIMING
#define R_SERIAL_IO_I3C0_MEM_SCL_I3C_PP_TIMING            0x218U          ///< REG SCL_I3C_PP_TIMING
#define R_SERIAL_IO_I3C0_MEM_SCL_I2C_FM_TIMING            0x21C           ///< REG SCL_I2C_FM_TIMING
#define R_SERIAL_IO_I3C0_MEM_SCL_I2C_FMP_TIMING           0x220           ///< REG SCL_I2C_FMP_TIMING
#define R_SERIAL_IO_I3C0_MEM_SCL_I2C_SS_TIMING            0x224           ///< REG SCL_I2C_SS_TIMING
#define R_SERIAL_IO_I3C0_MEM_SCL_EXT_LCNT_TIMING          0x228           ///< REG SCL_EXT_LCNT_TIMING
#define R_SERIAL_IO_I3C0_MEM_SCL_EXT_TERMN_LCNT_TIMING    0x22C           ///< REG SCL_EXT_TERMN_LCNT_TIMING
#define R_SERIAL_IO_I3C0_MEM_SDA_HOLD_SWITCH_DLY_TIMING   0x230           ///< REG SDA_HOLD_SWITCH_DLY_TIMING
#define R_SERIAL_IO_I3C0_MEM_BUS_FREE_TIMING              0x234U          ///< REG BUS_FREE_TIMING


//
// I3C2 registers
//
#define R_SERIAL_IO_I3C2_MEM_QUEUE_THLD_CTRL              0xD0            ///< REG QUEUE_THLD_CTRL
#define R_SERIAL_IO_I3C2_MEM_DATA_BUFFER_THLD_CTRL        0xD4            ///< REG DATA_BUFFER_THLD_CTRL
#define R_SERIAL_IO_I3C2_MEM_SCL_I3C_OD_TIMING            0x214           ///< REG SCL_I3C_OD_TIMING
#define R_SERIAL_IO_I3C2_MEM_SCL_I3C_PP_TIMING            0x218           ///< REG SCL_I3C_PP_TIMING
#define R_SERIAL_IO_I3C2_MEM_SCL_I2C_FM_TIMING            0x21C           ///< REG SCL_I2C_FM_TIMING
#define R_SERIAL_IO_I3C2_MEM_SCL_I2C_FMP_TIMING           0x220           ///< REG SCL_I2C_FMP_TIMING
#define R_SERIAL_IO_I3C2_MEM_SCL_I2C_SS_TIMING            0x224           ///< REG SCL_I2C_SS_TIMING
#define R_SERIAL_IO_I3C2_MEM_SCL_EXT_LCNT_TIMING          0x228           ///< REG SCL_EXT_LCNT_TIMING
#define R_SERIAL_IO_I3C2_MEM_SCL_EXT_TERMN_LCNT_TIMING    0x22C           ///< REG SCL_EXT_TERMN_LCNT_TIMING
#define R_SERIAL_IO_I3C2_MEM_SDA_HOLD_SWITCH_DLY_TIMING   0x230           ///< REG SDA_HOLD_SWITCH_DLY_TIMING
#define R_SERIAL_IO_I3C2_MEM_BUS_FREE_TIMING              0x234           ///< REG BUS_FREE_TIMING
#endif      // _SERIALIOI3CREGS_H_
