/** @file
  Policy definition for PCIe Config Block

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCIE_PREMEM_CONFIG_H__
#define __PCIE_PREMEM_CONFIG_H__

#include <GpioConfig.h>

///
/// SA GPIO Data Structure
///
typedef struct {
  GPIO_PAD      GpioPad;        ///< Offset 0: GPIO Pad
  UINT8         Value;          ///< Offset 4: GPIO Value
  UINT8         Rsvd0[3];       ///< Offset 5: Reserved for 4 bytes alignment
  UINT32        Active  :1;     ///< Offset 8: 0=Active Low; 1=Active High
  UINT32        RsvdBits0:31;
} SA_GPIO_INFO_PCIE;

///
/// SA Board PEG GPIO Info
///
typedef struct {
  SA_GPIO_INFO_PCIE  SaPeg0ResetGpio;    ///< Offset 0:  PEG0 PERST# GPIO assigned, must be a PCH GPIO pin
  SA_GPIO_INFO_PCIE  SaPeg3ResetGpio;    ///< Offset 12: PEG3 PERST# GPIO assigned, must be a PCH GPIO pin
  BOOLEAN            GpioSupport;        ///< Offset 24: 1=Supported; 0=Not Supported
  UINT8              Rsvd0[3];           ///< Offset 25: Reserved for 4 bytes alignment
} PEG_GPIO_DATA;

#endif /* __PCIE_PREMEM_CONFIG_H__ */
