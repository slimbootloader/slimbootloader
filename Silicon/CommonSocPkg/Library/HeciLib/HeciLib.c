/** @file
  Heci driver core, determines the HECI device and initializes it.

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/HeciLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/MemoryAllocationLib.h>
#include <IndustryStandard/Pci.h>
#include "HeciRegs.h"

/**
  Return HECI1 Mode

  @param[out] MeMode              Pointer for HECI1 Mode report

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  OUT UINT32                     *MeMode
  )
{
  HECI_FWS_REGISTER                   MeFirmwareStatus;
  UINTN                               HeciMmPciAddress;

  if (MeMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HeciMmPciAddress = MeGetHeciMmPciAddress (0, 0);
  if (MmioRead16 (HeciMmPciAddress + PCI_DEVICE_ID_OFFSET) == 0xFFFF) {
    return EFI_NOT_FOUND;
  }

  MeFirmwareStatus.ul = MmioRead32 (HeciMmPciAddress + R_ME_HFS);
  switch (MeFirmwareStatus.r.MeOperationMode) {
    case ME_OPERATION_MODE_NORMAL:
      *MeMode = ME_MODE_NORMAL;
      break;

    case ME_OPERATION_MODE_DEBUG:
      *MeMode = ME_MODE_DEBUG;
      break;

    case ME_OPERATION_MODE_SOFT_TEMP_DISABLE:
      *MeMode = ME_MODE_TEMP_DISABLED;
      break;

    case ME_OPERATION_MODE_SECOVR_JMPR:
    case ME_OPERATION_MODE_SECOVR_HECI_MSG:
      *MeMode = ME_MODE_SECOVER;
      break;

    case ME_OPERATION_MODE_SPS:
      *MeMode = ME_MODE_SPS;
      break;

    default:
      *MeMode = ME_MODE_FAILED;
  }

  return EFI_SUCCESS;
}

/**
  Read Heci1 FwStatus

  @retval EFI_SUCCESS             HECI Status Read successfully.
  @retval Others                  There is failure when Reading HECI Status

 **/
EFI_STATUS
EFIAPI
HeciReadFwStatus (
  IN UINT32                      *FwSts
  )
{
  UINTN                               HeciMmPciAddress;

  if (FwSts == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  HeciMmPciAddress = MeGetHeciMmPciAddress (0, 0);
  if (MmioRead16 (HeciMmPciAddress + PCI_DEVICE_ID_OFFSET) == 0xFFFF) {
    return EFI_NOT_FOUND;
  }

  *FwSts = MmioRead32 (HeciMmPciAddress + R_ME_HFS);

  return EFI_SUCCESS;
}

/**
  Send Get Firmware Version Request

  @param[out] MsgAck              Return FW version from CSE.

  @exception EFI_UNSUPPORTED      Current Sec mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwVersionMsg (
  OUT UINT8                      *MsgAck
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  GEN_GET_FW_VER                  MsgGenGetFwVersion;
  GEN_GET_FW_VER_ACK             *MsgGenGetFwVersionAck;

  if (MsgAck == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MsgGenGetFwVersionAck = (GEN_GET_FW_VER_ACK *)MsgAck;

  //
  // Allocate MsgGenGetFwVersion data structure
  //
  MsgGenGetFwVersion.MkhiHeader.Data               = 0;
  MsgGenGetFwVersion.MkhiHeader.Fields.GroupId     = MKHI_GEN_GROUP_ID;
  MsgGenGetFwVersion.MkhiHeader.Fields.Command     = GEN_GET_FW_VERSION_CMD;
  MsgGenGetFwVersion.MkhiHeader.Fields.IsResponse  = 0;

  //
  // Send Get FW SKU Request
  //
  Length = sizeof (GEN_GET_FW_VER);
  Status = HeciSend (HECI1_DEVICE, (UINT32 *)&MsgGenGetFwVersion, Length,
                     BIOS_FIXED_HOST_ADDR, HECI_MKHI_MESSAGE_ADDR);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[HECI] HeciGetFwVersionMsg failed on send - %r\n", Status));
    return Status;
  }

  Length = sizeof (GEN_GET_FW_VER_ACK);
  Status = HeciReceive (HECI1_DEVICE, HECI_BLOCKING_MSG,
                        (UINT32 *)MsgGenGetFwVersionAck, &Length);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[HECI] HeciGetFwVersionMsg failed on read - %r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Send Get Firmware SKU Request

  @param[out] MsgGetFwCapsAck     Return message for Get Firmware Capability SKU ACK

  @exception EFI_UNSUPPORTED      Current Sec mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwCapsSkuMsg (
  OUT UINT8                      *MsgGetFwCapsAck
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  GEN_GET_FW_CAPSKU               MsgGenGetFwCapsSku;
  GEN_GET_FW_CAPS_SKU_ACK         *MsgGenGetFwCapsSkuAck;

  if (MsgGetFwCapsAck == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MsgGenGetFwCapsSkuAck = (GEN_GET_FW_CAPS_SKU_ACK *)MsgGetFwCapsAck;

  //
  // Allocate MsgGenGetFwVersion data structure.
  //
  MsgGenGetFwCapsSku.MkhiHeader.Data               = 0;
  MsgGenGetFwCapsSku.MkhiHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetFwCapsSku.MkhiHeader.Fields.Command     = FWCAPS_GET_RULE_CMD;
  MsgGenGetFwCapsSku.MkhiHeader.Fields.IsResponse  = 0x0;
  MsgGenGetFwCapsSku.Data.RuleId                   = 0x0;

  //
  // Send Get FW SKU Request to SEC
  //
  Length = sizeof (GEN_GET_FW_CAPSKU);
  Status = HeciSend (HECI1_DEVICE, (UINT32 *)&MsgGenGetFwCapsSku, Length,
                     BIOS_FIXED_HOST_ADDR, HECI_MKHI_MESSAGE_ADDR);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[HECI] HeciGetFwCapsSkuMsg failed on send - %r\n", Status));
    return Status;
  }

  Length = sizeof (GEN_GET_FW_CAPS_SKU_ACK);
  Status = HeciReceive (HECI1_DEVICE, HECI_BLOCKING_MSG,
                        (UINT32 *)MsgGenGetFwCapsSkuAck, &Length);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[HECI] HeciGetFwCapsSkuMsg failed on read - %r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  This function gets CSME boot time data from CSME.

  @param[out]  EarlyBootData         - Pointer to Array of TimeStamps in CSME Boot data structure
  @param[out]  EarlyBootLength       - Pointer to Number of elements in CSME Boot data structure
  @param[out]  EarlyBootDataVersion  - Pointer to Version of CSME Boot data structure

  @retval EFI_SUCCESS             Event data read successfully
  @retval EFI_INVALID_PARAMETER   Input parameters are not valid
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_NOT_READY           Device is not ready
**/
EFI_STATUS
HeciGetEarlyBootPerfData (
  OUT UINT32        **EarlyBootData,
  OUT UINT32        *EarlyBootLength,
  OUT UINT32        *EarlyBootDataVersion
  )
{
  EFI_STATUS                        Status;
  UINT32                            ReqLength;
  UINT32                            RespLength;
  GET_EARLY_BOOT_PERF_DATA_BUFFER   *PerfDataBuf;

  if ((EarlyBootData == NULL) || (EarlyBootLength == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ReqLength = sizeof (GET_EARLY_BOOT_PERF_DATA_CMD);
  RespLength = sizeof (GET_EARLY_BOOT_PERF_DATA_RESPONSE) + (EARLY_BOOT_PERF_DATA_LENGTH_VERSION_1 * sizeof(UINT32));
  PerfDataBuf = (GET_EARLY_BOOT_PERF_DATA_BUFFER*) AllocatePool (RespLength);
  if (PerfDataBuf == NULL) {
    DEBUG ((DEBUG_ERROR, "CSME PERF Data: Could not allocate Memory\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (PerfDataBuf, RespLength);

  PerfDataBuf->Request.Header.Fields.GroupId = BUP_COMMON_GROUP_ID;
  PerfDataBuf->Request.Header.Fields.Command = GET_EARLY_BOOT_PERFORMANCE_DATA_CMD;

  Status = HeciSendwAck (
                   HECI1_DEVICE,
                   (UINT32 *)PerfDataBuf,
                   ReqLength,
                   &RespLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_MKHI_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Heci Get Csme Perf Data failed with status %r\n", Status));
    return Status;
  }

  if ((PerfDataBuf->Response.Header.Fields.GroupId != BUP_COMMON_GROUP_ID) ||
      (PerfDataBuf->Response.Header.Fields.Command != GET_EARLY_BOOT_PERFORMANCE_DATA_CMD) ||
      (PerfDataBuf->Response.Header.Fields.IsResponse == 0) ||
      (PerfDataBuf->Response.Header.Fields.Result != 0)) {
    DEBUG ((DEBUG_ERROR, "Heci Get Csme Perf Data failed\n"));
    return EFI_DEVICE_ERROR;
  }

  *EarlyBootData     = PerfDataBuf->Response.BootPerformanceData;
  *EarlyBootLength   = PerfDataBuf->Response.BootDataLength;
  *EarlyBootDataVersion = PerfDataBuf->Response.BootDataVersion;

  DEBUG ((DEBUG_INFO, "Heci Get Csme Perf Data Msg successful.\n"));

  return EFI_SUCCESS;
}

/**
  Check for Manufacturing Mode

  @param[out]  MeManuMode         Manufacturing mode/production mode.

  @retval EFI_UNSUPPORTED         Current CSME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded

**/
EFI_STATUS
EFIAPI
HeciGetManufactureMode (
  OUT UINT16 *MeManuMode
  )
{
  UINTN                    HeciMmPciAddress;
  HECI_FW_STS6_REGISTER    MeFirmwareStatus;

  if (MeManuMode == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  HeciMmPciAddress = MeGetHeciMmPciAddress (0, 0);
  if (MmioRead16 (HeciMmPciAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    return EFI_NOT_FOUND;
  }

  MeFirmwareStatus.ul = MmioRead32 (HeciMmPciAddress + R_ME_HFS_6);
  DEBUG ((DEBUG_INFO, "HECI1 MeFirmwareStatus = %x \n",  MeFirmwareStatus.ul));

  if (MeFirmwareStatus.r.FpfSocConfigLock == 0) {
    // Platform is still in manufaturing.
    // Field programmable fuses(FPF)fuses are NOT burned yet
    *MeManuMode = 1;
  } else {
    // Platform is in production mode.
    // Field programmable fuses(FPF)fuses are burned
    *MeManuMode = 0;
  }

  return EFI_SUCCESS;
}

/**
  Register HECI service

  @retval EFI_SUCCESS             HECI service is registered successfully.
  @retval Others                  There is failure when register HECI service
**/
EFI_STATUS
EFIAPI
HeciRegisterHeciService (
  VOID
  )
{
  EFI_STATUS      Status;

  Status = RegisterService ((VOID *)MeGetHeciServiceInstance ());

  return Status;
}
