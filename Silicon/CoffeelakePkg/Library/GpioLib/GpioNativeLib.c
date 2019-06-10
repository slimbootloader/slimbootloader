/** @file
  This file contains routines for GPIO native and chipset specific usage

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "GpioLibrary.h"
#include "GpioInitLib.h"

/**
  This procedure will get group index (0 based) from group

  @param[in] GpioGroup        Gpio Group

  @retval Value               Group Index
**/
UINT32
GpioGetGroupIndexFromGroup (
  IN GPIO_GROUP        GpioGroup
  )
{
  return (UINT32) GPIO_GET_GROUP_INDEX (GpioGroup);
}

/**
  This procedure will get group index (0 based)

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group Index
**/
UINT32
GpioGetGroupIndexFromGpioPad (
  IN GPIO_PAD        GpioPad
  )
{
  return (UINT32) GPIO_GET_GROUP_INDEX_FROM_PAD (GpioPad);
}

/**
  This procedure will get pad number (0 based) from Gpio Pad

  @param[in] GpioPad          Gpio Pad

  @retval Value               Pad Number
**/
UINT32
GpioGetPadNumberFromGpioPad (
  IN GPIO_PAD        GpioPad
  )
{
  return (UINT32) GPIO_GET_PAD_NUMBER (GpioPad);
}

/**
  This procedure will get number of groups

  @param[in] none

  @retval Value               Group number
**/
UINT32
GpioGetNumberOfGroups (
  VOID
  )
{
  UINT32                 GpioGroupInfoLength;

  GpioGetGroupInfoTable (&GpioGroupInfoLength);
  return GpioGroupInfoLength;
}

/**
  This procedure will get group number

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group number
**/
GPIO_GROUP
GpioGetGroupFromGpioPad (
  IN GPIO_PAD         GpioPad
  )
{
  return GPIO_GET_GROUP_FROM_PAD (GpioPad);
}
