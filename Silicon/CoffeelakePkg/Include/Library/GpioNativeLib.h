/** @file
  Header file for GpioLib for native and Si specific usage.
  All function in this library is available for PEI, DXE, and SMM,

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _GPIO_NATIVE_LIB_H_
#define _GPIO_NATIVE_LIB_H_

#include <GpioConfig.h>

/**
  This procedure will get number of pads for certain GPIO group

  @param[in] Group            GPIO group number

  @retval Value               Pad number for group
                              If illegal group number then return 0
**/
UINT32
GpioGetPadPerGroup (
  IN GPIO_GROUP        Group
  );

/**
  This procedure will get number of groups

  @param[in] none

  @retval Value               Group number
**/
UINT32
GpioGetNumberOfGroups (
  VOID
  );
/**
  This procedure will get lowest group

  @param[in] none

  @retval Value               Lowest Group
**/
GPIO_GROUP
GpioGetLowestGroup (
  VOID
  );

/**
  This procedure will get highest group

  @param[in] none

  @retval Value               Highest Group
**/
GPIO_GROUP
GpioGetHighestGroup (
  VOID
  );

/**
  This procedure will get group

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group
**/
GPIO_GROUP
GpioGetGroupFromGpioPad (
  IN GPIO_PAD        GpioPad
  );

/**
  This procedure will get group index (0 based) from GpioPad

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group Index
**/
UINT32
GpioGetGroupIndexFromGpioPad (
  IN GPIO_PAD        GpioPad
  );

/**
  This procedure will get group index (0 based) from group

  @param[in] GpioGroup        Gpio Group

  @retval Value               Group Index
**/
UINT32
GpioGetGroupIndexFromGroup (
  IN GPIO_GROUP        GpioGroup
  );

/**
  This procedure will get group from group index (0 based)

  @param[in] GroupIndex        Group Index

  @retval GpioGroup            Gpio Group
**/
GPIO_GROUP
GpioGetGroupFromGroupIndex (
  IN UINT32        GroupIndex
  );

/**
  This procedure will get pad number (0 based) from Gpio Pad

  @param[in] GpioPad          Gpio Pad

  @retval Value               Pad Number
**/
UINT32
GpioGetPadNumberFromGpioPad (
  IN GPIO_PAD        GpioPad
  );

/**
  This procedure will return GpioPad from Group and PadNumber

  @param[in] Group              GPIO group
  @param[in] PadNumber          GPIO PadNumber

  @retval GpioPad               GpioPad
**/
GPIO_PAD
GpioGetGpioPadFromGroupAndPadNumber (
  IN GPIO_GROUP      Group,
  IN UINT32          PadNumber
  );

/**
  This procedure will return GpioPad from GroupIndex and PadNumber

  @param[in] GroupIndex         GPIO GroupIndex
  @param[in] PadNumber          GPIO PadNumber

  @retval GpioPad               GpioPad
**/
GPIO_PAD
GpioGetGpioPadFromGroupIndexAndPadNumber (
  IN UINT32          GroupIndex,
  IN UINT32          PadNumber
  );

/**
  This function checks if SATA GP pin is enabled

  @param[in]  SataCtrlIndex       SATA Controller Index
  @param[in]  SataPort            SATA port number

  @retval TRUE                    SATA GPx is enabled (pad is in required native mode)
          FALSE                   SATA GPx is not enabled
**/
BOOLEAN
GpioIsSataGpEnabled (
  IN  UINT32          SataCtrlIndex,
  IN  UINTN           SataPort
  );

/**
  This function checks if SataDevSlp pin is in native mode

  @param[in]  SataCtrlIndex       SATA controller index
  @param[in]  SataPort            SATA port
  @param[out] DevSlpPad           DevSlpPad

  @retval TRUE                    DevSlp is in native mode
          FALSE                   DevSlp is not in native mode
**/
BOOLEAN
GpioIsSataDevSlpPinEnabled (
  IN  UINT32          SataCtrlIndex,
  IN  UINTN           SataPort,
  OUT GPIO_PAD        *DevSlpPad
  );

/**
  This function checks if GPIO pin is a GSPI chip select pin

  @param[in]  GpioPad             GPIO pad
  @param[in]  PadMode             GPIO pad mode

  @retval TRUE                    Pin is in GPIO mode
          FALSE                   Pin is in native mode
**/
BOOLEAN
GpioIsGpioPadAGSpiCsbPin (
  IN  GPIO_PAD        GpioPad,
  IN  GPIO_PAD_MODE   PadMode
  );

#endif // _GPIO_NATIVE_LIB_H_
