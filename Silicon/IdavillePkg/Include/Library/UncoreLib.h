/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __UNCORE_LIB_H__
#define __UNCORE_LIB_H__

typedef enum {
  TypeLcc = 0,
  TypeMcc,
  TypeHcc,
  TypeXcc,
  TypeChopTypeMax = 0xff
} TYPE_CHOPTYPE_ENUM;

/**
  Returns the bit mask of the total Iios enabled for a given CPU socket.

  @param[in]  SocId             - Soc ID of the CPU who's enabled IIOs info is requested, 0,1,2,3,etc
  @param[out] IioPresentBitMask - Bit mask of the enabled IIOs in given socket

  @retval EFI_SUCCESS      - on successful completion
  @retval Others           - on any errors

**/
EFI_STATUS
EFIAPI
GetIioPresentBitMask (
  IN  UINT8                      SocId,
  OUT UINT32                    *IioPresentBitMask
  );

/**
  Returns the total Iios enabled for a given CPU socket.

  @param[in]  SocId             - Soc ID of the CPU who's enabled IIOs info is requested, 0,1,2,3,etc
  @param[out] IIOCount          - Total IIOs in given socket

  @retval EFI_SUCCESS      - on successful completion
  @retval Others           - on any errors

**/
EFI_STATUS
EFIAPI
GetIioCount (
  IN  UINT8                      SocId,
  OUT UINT8                     *IioCount
  );

/**
  Get the Chop Type

  @param[in] SocId        - Socket ID

  @retval                 - The Chop Type

**/
TYPE_CHOPTYPE_ENUM
EFIAPI
GetChopType (
  IN  UINT8 SocId
  );

/**
  Get desired simultaneous multithreading

  @retval                 0: Disable SMT, 1: Enable SMT

**/
UINT8
EFIAPI
GetDesiredSmtEnable (
  VOID
  );

#endif // __UNCORE_LIB_H__
