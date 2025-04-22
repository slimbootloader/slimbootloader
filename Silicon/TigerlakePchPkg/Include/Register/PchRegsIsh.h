/** @file
  Register names for PCH Integrated Sensor Hub (ISH3.0)

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_ISH_H_
#define _PCH_REGS_ISH_H_

//
// ISH Controller Registers
//
#ifdef PCH_ICL
// ICL-PCH D18:F0
#define PCI_DEVICE_NUMBER_PCH_ISH             18
#else
// CNL-PCH D19:F0
#define PCI_DEVICE_NUMBER_PCH_ISH             19
#endif
#define PCI_FUNCTION_NUMBER_PCH_ISH           0


//
// Number of pins used by ISH controllers
//
#define PCH_ISH_PINS_PER_I2C_CONTROLLER               2
#define PCH_ISH_PINS_PER_UART_CONTROLLER              4
#define PCH_ISH_PINS_PER_SPI_CONTROLLER               4

#endif
