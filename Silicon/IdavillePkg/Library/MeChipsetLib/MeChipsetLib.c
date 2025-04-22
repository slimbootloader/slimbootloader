/** @file
  Heci platform specific library

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/HeciLib.h>
#include <Library/BootloaderCommonLib.h>
#include <MeChipset.h>

STATIC CONST HECI_SERVICE mHeciService = {
  .Header.Signature   = HECI_SERVICE_SIGNATURE,
  .Header.Version     = HECI_SERVICE_VERSION,
  .HeciSend           = HeciServiceSend,
  .HeciReceive        = HeciServiceReceive,
  .HeciResetInterface = HeciServiceResetInterface,
  .SimpleHeciCommand  = NULL,
  .HeciUserCommand    = NULL,
};

/**
  This function provides a PCI config space memory address to access a HECI
  device and its register.

  @param[in] HeciDev              The HECI device to be accessed
  @param[in] Register             A register offset

  @retval HeciMmPciAddress        PCI config space memory address

**/
UINTN
EFIAPI
MeGetHeciMmPciAddress (
  IN HECI_DEVICE                  HeciDev,
  IN UINTN                        Register
  )
{
  UINTN   HeciMmPciAddress;

  switch (HeciDev) {
    case HECI1_DEVICE:
      HeciMmPciAddress = (UINTN)MM_PCI_ADDRESS (
        ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER + HeciDev, Register);
      break;
    default:
      HeciMmPciAddress = 0;
      DEBUG ((DEBUG_ERROR, "[HeciInfo] Invalid HECI%d device!", HeciDev));
      ASSERT (FALSE);
      break;
  }

  return HeciMmPciAddress;
}

/**
  Whether HECI timeouts are enabled or not.

  @retval TRUE             Timeout is enabled
  @retval FALSE            Timeout is disabled

**/
BOOLEAN
EFIAPI
MeHeciTimeoutsEnabled (
  VOID
  )
{
  return TRUE;
}

/**
  Ish HECI device

  @retval Ish HECI device
  @retval 0xFF if Ish HECI device is not supported

**/
HECI_DEVICE
EFIAPI
MeGetIshHeciDevice (
  VOID
  )
{
  return HECI_INVALID_DEVICE;
}

/**
  Provide Heci Service Instance

  @retval HECI_SERVICE            Return Heci Service Instance Pointer

 **/
CONST
HECI_SERVICE *
EFIAPI
MeGetHeciServiceInstance (
  VOID
  )
{
  return &mHeciService;
}
