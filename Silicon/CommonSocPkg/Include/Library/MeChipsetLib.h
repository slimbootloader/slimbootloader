/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _HECI_CHIPSET_LIB_H_
#define _HECI_CHIPSET_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <MeChipset.h>
#include <MkhiMsgs.h>
#include <Service/HeciService.h>

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
  );

/**
  Whether HECI timeouts are enabled or not.

  @retval TRUE             Timeout is enabled
  @retval FALSE            Timeout is disabled

**/
BOOLEAN
EFIAPI
MeHeciTimeoutsEnabled (
  VOID
  );

/**
  Ish HECI device

  @retval Ish HECI device
  @retval 0xFF if Ish HECI device is not supported

**/
HECI_DEVICE
EFIAPI
MeGetIshHeciDevice (
  VOID
  );

/**
  Get ME BIOS Payload data pointer

  @return MbpHeader             A pointer to MBP data header.
  @return NULL                  NULL in case MBP not found.

 **/
VOID *
EFIAPI
MeGetMeBiosPayloadHob (
  VOID
  );

/**
  This function return version info from MBP Hob.

  @param[in] AckData                Return FW version from MBP

  @retval    EFI_SUCCESS            Command succeeded
  @retval    EFI_INVALID_PARAMETER  No AckData pointer
  @retval    EFI_NOT_FOUND          No version info found from MBP

**/
EFI_STATUS
EFIAPI
MeGetFwVersionFromMbp (
  IN  GEN_GET_FW_VER_ACK_DATA *AckData
  );

/**
  Provide Heci Service Instance

  @retval HECI_SERVICE            Return Heci Service Instance Pointer

 **/
CONST
HECI_SERVICE *
EFIAPI
MeGetHeciServiceInstance (
  VOID
  );

#endif // _HECI_CHIPSET_LIB_H_
