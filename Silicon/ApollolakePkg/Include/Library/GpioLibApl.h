/** @file

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SI_GPIO_LIB_H__
#define __SI_GPIO_LIB_H__

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <GpioConfig.h>
#include <GpioDefines.h>

/**
  Program SMIP GPIO PAD DW 0 and 1 registers for South/West/SouthWest/North/NorthWest GPIO communities.

  @param[in]  Gpio_Pin_Num         - GPIO Pin Number to configure.
  @param[in]  Gpio_Smip_Conf_Data  - Data array from SMIP setting for each GPIO communities.
**/
VOID
GpioSmipPadConfigTable (
  IN UINT32                    Gpio_Pin_Num,
  IN GPIO_CONFIG_SMIP*         Gpio_Smip_Conf_Data
);

/**
  Common interface to read an 32-bit specific GPIO pad register.

  It is recommand to use the definition of GPIO pad in GpioLib.h
  e.g. GpioPadRead (N_GPIO_20)

  @param[in]  CommAndOffset    It contains GPIO community and offset.

  @retval     UINT32           The 32-bit value read from GpioRead operation.

**/
UINT32
EFIAPI
GpioPadRead (
  IN UINT32    CommAndOffset
  );


/**
  Common interface to write an 32-bit to a specific GPIO pad register.

  It is recommand to use the definition of GPIO pad in GpioLib.h
  e.g. GpioPadWrite (N_GPIO_20, 0xXXXXXXXX)

  @param[in]  CommAndOffset   It contains GPIO community and offset.
  @param[in]  Value           The value to write.

  @retval     EFI_SUCCESS     Write to a specific GPIO pad register successfully.
  @retval     Others          Status depends on GpioWrite operation.

**/
EFI_STATUS
EFIAPI
GpioPadWrite (
  IN UINT32    CommAndOffset,
  IN UINT32    Value
  );

/**
  This procedure will read GPIO register

  @param[in] Community   GPIO register community
  @param[in] Offset      GPIO register offset within this community

  @retval    Value       Value read from the GPIO register.

**/
UINT32
EFIAPI
GpioRead(
  IN UINT8 Community,
  IN UINT16 Offset
);

/**
  This procedure will write GPIO register

  @param[in] Community    GPIO register community
  @param[in] Offset       GPIO register offset within this community
  @param[in] Value        Value to be written to the GPIO register

  @retval    EFI_SUCCESS  Value read from the GPIO register

**/
EFI_STATUS
EFIAPI
GpioWrite(
  IN UINT8 Community,
  IN UINT16 Offset,
  IN UINT32 Value
);

/**
  This procedure will get GPIO input level

  @param[in]  GpioPad             GPIO pad
  @param[in]  Term                Termination
  @param[out] InputVal            GPIO Input value
                                  0: InputLow, 1: InputHigh

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioGetInputValueWithTerm (
  IN  UINT32       GpioPad,
  IN  UINT32       Term,
  OUT UINT32      *InputVal
);


/**
  This procedure will set PadCfgLock for selected pad

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioLockPadCfg (
  IN GPIO_PAD                   GpioPad
  );


/**
  This procedure will set PadCfgLockTx for selected pad

  @param[in] GpioPad              GPIO pad

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioLockPadCfgTx (
  IN GPIO_PAD                   GpioPad
  );


#endif /* __SI_GPIO_LIB_H__ */
