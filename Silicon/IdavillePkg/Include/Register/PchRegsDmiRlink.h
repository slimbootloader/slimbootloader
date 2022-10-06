/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_DMI_RLINK_H_
#define _PCH_REGS_DMI_RLINK_H_


#define R_PCH_RLINK_PCR_LPCLGIR1               0x0E30                      ///< LPC Generic I/O Range 1
#define R_PCH_RLINK_PCR_LPCGMR                 0x0E40                      ///< LPC Generic Memory Range
#define R_PCH_RLINK_PCR_LPCBDE                 0x0E44                      ///< LPC BIOS Decode Enable

#define R_PCH_RLINK_PCR_GCS                    0x0E4C                      ///< Generic Control and Status
#define B_PCH_RLINK_PCR_BBS                    BIT10                       ///< Boot BIOS Strap

#define R_PCH_RLINK_PCR_LPCIOD                 0x0E70                      ///< LPC I/O Decode Ranges
#define R_PCH_RLINK_PCR_LPCIOE                 0x0E74                      ///< LPC I/O Enables
#define R_PCH_RLINK_PCR_TCOBASE                0x0E78                      ///< TCO Base Address
#define B_PCH_RLINK_PCR_TCOBASE_TCOBA          0xFFE0                      ///< TCO Base Address Mask

#define R_PCH_RLINK_PCR_SEGIR                  0x0EBC                      ///< Second ESPI Generic I/O Range
#define R_PCH_RLINK_PCR_SEGMR                  0x0EC0                      ///< Second ESPI Generic Memory Range


#endif //_PCH_REGS_DMI_RLINK_H_
