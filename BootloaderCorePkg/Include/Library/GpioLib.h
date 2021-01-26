/** @file

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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


/**
  Configure the GPIO pins, available as part of platform specific GPIO CFG DATA.
  If the pins are not part of GPIO CFG DATA, call GpioPadConfigTable() directly
  with the appropriate arguments.

  @param    Tag         Tag ID of the Gpio Cfg data item
  @param    Entries     Number of entries in Gpio Table
  @param    DataBuffer  Pointer to the Gpio Table to be programmed

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_NOT_FOUND                 If Gpio Config Data cant be found
**/
EFI_STATUS
EFIAPI
ConfigureGpio (
  IN  UINT16  Tag,
  IN  UINT32  Entries,
  IN  UINT8   *DataBuffer
  );


#endif /* __GPIO_LIB_H__ */
