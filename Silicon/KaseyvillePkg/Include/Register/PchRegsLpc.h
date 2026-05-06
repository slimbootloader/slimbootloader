/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCH_REGS_LPC_H__
#define __PCH_REGS_LPC_H__

//
// PCI to LPC Bridge Registers (D31:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

#define R_PCH_IO_NMI_EN                           0x70

#define R_LPC_CFG_IOD                             0x80
#define N_LPC_CFG_IOD_LPT                         8
#define V_LPC_CFG_IOD_LPT_378                     0
#define B_LPC_CFG_IOE_ME2                         BIT13           ///< Microcontroller Enable #2, Enables decoding of I/O locations 4Eh and 4Fh to LPC
#define B_LPC_CFG_IOE_SE                          BIT12           ///< Super I/O Enable, Enables decoding of I/O locations 2Eh and 2Fh to LPC.
#define B_LPC_CFG_IOE_ME1                         BIT11           ///< Microcontroller Enable #1, Enables decoding of I/O locations 62h and 66h to LPC.
#define B_LPC_CFG_IOE_KE                          BIT10           ///< Keyboard Enable, Enables decoding of the keyboard I/O locations 60h and 64h to LPC.
#define B_LPC_CFG_IOE_HGE                         BIT9            ///< High Gameport Enable, Enables decoding of the I/O locations 208h to 20Fh to LPC.
#define B_LPC_CFG_IOE_LGE                         BIT8            ///< Low Gameport Enable, Enables decoding of the I/O locations 200h to 207h to LPC.
#define B_LPC_CFG_IOE_FDE                         BIT3            ///< Floppy Drive Enable, Enables decoding of the FDD range to LPC. Range is selected by LIOD.FDE
#define B_LPC_CFG_IOE_PPE                         BIT2            ///< Parallel Port Enable, Enables decoding of the LPT range to LPC. Range is selected by LIOD.LPT.


#define B_PCH_IO_NMI_EN_NMI_EN                    BIT7


#endif /* __PCH_REGS_LPC_H__ */
