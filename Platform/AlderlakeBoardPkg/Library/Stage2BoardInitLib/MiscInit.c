/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"
#include <GpioPinsVer4S.h>
#include <GpioPinsVer2Lp.h>

#define GPIO_CFG_PIN_TO_PAD(A) \
  ((UINT32) (((A).PinNumber) | (((A).PadGroup) << 16)))

/**
  Update current boot Payload ID.

**/
VOID
UpdatePayloadId (
  VOID
  )
{
  EFI_STATUS        Status;
  UINT32            PayloadId;
  GEN_CFG_DATA      *GenericCfgData;
  PLDSEL_CFG_DATA   *PldSelCfgData;
  UINT32            PayloadSelGpioData;
  UINT32            PayloadSelGpioPad;
  UINT8             CmosData;

  PayloadSelGpioData = 0;

  PayloadId = GetPayloadId ();
  GenericCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenericCfgData != NULL) {
    if (GenericCfgData->PayloadId == AUTO_PAYLOAD_ID_SIGNATURE) {
      PayloadId = 0;
    } else {
      //
      // If PayloadId is not 'AUTO' then we should use the value
      // provided by CFG data; might want to always use UEFI or
      // OS Loader.
      //
      PayloadId = GenericCfgData->PayloadId;
      SetPayloadId (PayloadId);
      return;
    }
  }

  //
  // Use CMOS offset 0x20 (use as magic check) and 0x21 (payloadId) for payload detection.
  //
  // Read CMOS offset 20 value
  IoWrite8 (CMOS_EXTENDED_ADDREG, CMOS_EXTENDED_OFFSET_20);
  CmosData = IoRead8 (CMOS_EXTENDED_DATAREG);
  DEBUG ((DEBUG_INFO, "CMOS boot Magic [0x%x]\n", CmosData));

  if (CmosData == CMOS_VALUE_SWITCH_PLD) {
    // Read payload target
    IoWrite8 (CMOS_EXTENDED_ADDREG, CMOS_EXTENDED_OFFSET_21);
    CmosData  = IoRead8 (CMOS_EXTENDED_DATAREG);
    DEBUG ((DEBUG_INFO, "CMOS boot target [0x%x]\n", CmosData));

    switch (CmosData) {
      case 0:
        SetPayloadId (0);
        break;
      case 1:
        SetPayloadId (UEFI_PAYLOAD_ID_SIGNATURE);
        break;
      case 2:
        SetPayloadId (LINX_PAYLOAD_ID_SIGNATURE);
        break;
      default:
       SetPayloadId (0);
       break;
    }

    return;
  }

  //
  // Switch payloads based on configured GPIO pin
  //
  PldSelCfgData = (PLDSEL_CFG_DATA *)FindConfigDataByTag (CDATA_PLDSEL_TAG);
  if ((PldSelCfgData != NULL) && (PldSelCfgData->PldSelGpio.Enable != 0)){
    if (IsPchS ()) {
      PayloadSelGpioPad = GPIO_CFG_PIN_TO_PAD(PldSelCfgData->PldSelGpio) | (GPIO_VER4_S_CHIPSET_ID << 24);
    } else if (IsPchP ()) {
      PayloadSelGpioPad = GPIO_CFG_PIN_TO_PAD(PldSelCfgData->PldSelGpio) | (GPIO_VER2_LP_CHIPSET_ID << 24);
    } else {
      DEBUG ((DEBUG_WARN, "Unknown PCH in UpdatePayloadId\n"));
      return;
    }
    Status = GpioGetInputValue (PayloadSelGpioPad, &PayloadSelGpioData);
    if (!EFI_ERROR (Status)) {
      if (PayloadSelGpioData != PldSelCfgData->PldSelGpio.PinPolarity) {
        PayloadId = UEFI_PAYLOAD_ID_SIGNATURE;
      } else {
        PayloadId = 0;
      }
      DEBUG ((DEBUG_INFO, "Set PayloadId to 0x%08X based on GPIO config\n", PayloadId));
    }
  }

  SetPayloadId (PayloadId);
}
