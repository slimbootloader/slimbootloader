/** @file

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __GPIO_LIB_H__
#define __GPIO_LIB_H__

/**
  Set GPIO PAD DW 0 and 1 registers for S/W/SW/N/NW GPIO communities

  @param[in] Gpio_Pin_Num    GPIO Pin Number to configure
  @param[in] Gpio_Conf_Data  GPIO_CONF_PAD_INIT data array
                             for each GPIO communities.
**/
VOID
GpioPadConfigTable (
  IN UINT32              Gpio_Pin_Num,
  IN VOID               *Gpio_Conf_Data
  );

#endif /* __GPIO_LIB_H__ */