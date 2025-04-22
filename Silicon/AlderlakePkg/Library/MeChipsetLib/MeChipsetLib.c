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
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <MeChipset.h>
#include <MeBiosPayloadData.h>

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
    case HECI2_DEVICE:
    case HECI3_DEVICE:
    case HECI4_DEVICE:
    case ISH_HECI:
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
  return ISH_HECI;
}

/**
  Get ME BIOS Payload data pointer

  @return MbpHeader             A pointer to MBP data header.
  @return NULL                  NULL in case MBP not found.

 **/
VOID *
EFIAPI
MeGetMeBiosPayloadHob (
  VOID
  )
{
  UINT32                    Length;
  VOID                     *FspHobList;
  VOID                     *MbpHeader;

  MbpHeader  = NULL;
  Length     = 0;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    MbpHeader = GetGuidHobData (FspHobList, &Length, &gMeBiosPayloadHobGuid);
  }
  if ((MbpHeader == NULL) || (Length == 0)){
    DEBUG ((DEBUG_ERROR, "MBP Data Hob not Found!\n"));
    MbpHeader = NULL;
  }

  return MbpHeader;
}

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
  )
{
  UINT8                    *DataPtr;
  MBP_FW_VERSION_NAME      *MbpFwVersion;

  if (AckData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DataPtr = (UINT8 *)MeGetMeBiosPayloadHob ();
  if (DataPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  MbpFwVersion = (MBP_FW_VERSION_NAME *)(DataPtr + 4);
  AckData->CodeMinor   = MbpFwVersion->MinorVersion;
  AckData->CodeMajor   = MbpFwVersion->MajorVersion;
  AckData->CodeBuildNo = MbpFwVersion->BuildVersion;
  AckData->CodeHotFix  = MbpFwVersion->HotfixVersion;
  DEBUG ((DEBUG_INFO, "FwVersion found from MBP!\n"));

  return EFI_SUCCESS;
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
