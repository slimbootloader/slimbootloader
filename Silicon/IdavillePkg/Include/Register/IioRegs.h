/** @file
  Register names for IIO

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __IIO_REGS_H__
#define __IIO_REGS_H__


#define R_IIO_VTBAR                               0x180             // Base Address Register for Intel VT-d Registers
#define B_IIO_VT_BAR_EN                           BIT0            // Intel VT-d Base Address Enable

#define R_VTD_EXT_CAP_LOW                         0x10
#define B_VTD_EXT_CAP_LOW_DT                      BIT2
#define B_VTD_EXT_CAP_LOW_IR                      BIT2

#endif /* __IIO_REGS_H__ */
