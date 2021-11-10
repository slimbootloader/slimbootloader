/** @file
  Register names for IGD block
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

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _SA_REGS_IGD_H_
#define _SA_REGS_IGD_H_

///
/// Device 2 Register Equates
///
//
// The following equates must be reviewed and revised when the specification is ready.
//
#define SA_IGD_BUS           0x00
#define SA_IGD_DEV           0x02
#define SA_IGD_FUN_0         0x00


///
/// GTTMMADR aligned to 16MB (Base address = [38:24])
///
#define R_SA_IGD_GTTMMADR          0x10

#ifdef CPU_ICL
#define R_SA_IGD_MSAC_OFFSET       0x0060  ///< Multisize Aperture Control
#else
#define R_SA_IGD_MSAC_OFFSET       0x0062  ///< Multisize Aperture Control
#endif


#ifdef CPU_CFL
#define R_SA_GTTMMADR_GTDOORBELL_OFFSET 0x124828  ///< iTouch GT Doorbell Register
#define R_SA_GTTMMADR_GSA_TOUCH_OFFSET  0x1300B4  ///< iTouch GSA Touch Register
#endif


#ifndef CPU_CFL
#define R_SA_GTTMMADR_GTDOORBELL_OFFSET 0x10c008  ///< iTouch GT Doorbell BDF Register
#define R_SA_GTTMMADR_GSA_TOUCH_OFFSET  0x101078  ///< GSA Touch DBF Register

#endif //CPU_CFL


///
/// DiSM Registers for 2LM Sku
///
#define R_SA_GTTMMADR_DISM_BASE_LSB_OFFSET  (0x108410)
#define R_SA_GTTMMADR_DISM_LIMIT_LSB_OFFSET (0x108418)
#endif
