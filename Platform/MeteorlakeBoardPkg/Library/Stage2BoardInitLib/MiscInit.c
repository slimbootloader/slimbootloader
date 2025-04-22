/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"


/**
  Update current boot Payload ID.

**/
VOID
UpdatePayloadId (
  VOID
  )
{
  UINT32            PayloadId;
  GEN_CFG_DATA      *GenericCfgData;
  UINT8             CmosData;

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

}
