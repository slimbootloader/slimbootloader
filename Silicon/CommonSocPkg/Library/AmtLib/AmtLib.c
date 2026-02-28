/** @file
  Implementation file for Amt functionality and configuration

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/HeciLib.h>
#include <Library/HobLib.h>
#include <MeBiosPayloadData.h>
#include <Library/HeciMeExtLib.h>
#include <MkhiMsgs.h>
#include <Library/AmtLib.h>
#include <Library/BootloaderCoreLib.h>

/**
 Get ME BIOS payload HOB data.

  @retval The ME BIOS payload HOB data

 **/
ME_BIOS_PAYLOAD *
GetMeBiosPayloadHobDataAmt (
    VOID
    )
{
  VOID                            *FspHobListPtr;
  UINT32                          MbpDataHobLen;
  UINT8                           *DataPtr;

  // HOB is an FSP HOB
  FspHobListPtr = GetFspHobListPtr();
  if (FspHobListPtr != NULL) {
    DataPtr = (UINT8 *)GetGuidHobData (FspHobListPtr, &MbpDataHobLen, &gMeBiosPayloadHobGuid);
    if ((DataPtr != NULL) && (MbpDataHobLen > 0)) {
      // Skip to data within the HOB
      return (ME_BIOS_PAYLOAD *)(DataPtr + 4);
    }
  }

  return NULL;
}

/**
  Disable AMT features and clear invocation codes.

  @return EFI_SUCCESS           The Sec FW version was retrieved successfully.
  @return EFI_ERROR             An error occurred.

**/
EFI_STATUS
DisableAmtFeatures (
   )
{
  EFI_STATUS                      Status;
  GEN_GET_FW_CAPS_SKU_ACK         MsgGenGetFwCapsSkuAck;
  GEN_GET_FW_CAPS_SKU_ACK         MsgGenGetFwAllCapsSkuAck;
  UINT32                          RuleData;
  UINT32                          AmtBits;
  ME_BIOS_PAYLOAD                 *MbpHobData;
  UINT32                          FwFeaturesCaps;

  // AMT support flags
  AmtBits = BIT0|BIT1|BIT2;

  // Try with HOB first as HECI calls are costly
  MbpHobData = GetMeBiosPayloadHobDataAmt ();
  if ((MbpHobData != NULL) && (MbpHobData->FwFeaturesState.Available)) {
    FwFeaturesCaps = MbpHobData->FwFeaturesState.FwFeatures.Data;
    DEBUG((DEBUG_INFO, "FwFeaturesCaps = %x\n", FwFeaturesCaps));

    if (FwFeaturesCaps & AmtBits) {
      Status = HeciGetUserCapsSkuMsg ((UINT8 *)&MsgGenGetFwCapsSkuAck);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "HeciGetUserCapsSkuMsg Failed Status=0x%x\n", Status));
        return Status;
      } else {
        DEBUG((DEBUG_INFO, "HeciGetUserCapsSkuMsg data = %x\n", MsgGenGetFwCapsSkuAck.Data.FWCap.Data));
      }

      Status = HeciGetFwAllCapsSkuMsg ((UINT8 *)&MsgGenGetFwAllCapsSkuAck);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "HeciGetFwAllCapsSkuMsg Failed Status=0x%x\n", Status));
        return Status;
      } else {
        DEBUG((DEBUG_INFO, "HeciGetFwAllCapsSkuMsg Data = %x\n", MsgGenGetFwAllCapsSkuAck.Data.FWCap.Data));
      }

      // If AMT is enabled in fwcaps, disable it
      if (MsgGenGetFwAllCapsSkuAck.Data.FWCap.Data & AmtBits) {
        RuleData = MsgGenGetFwAllCapsSkuAck.Data.FWCap.Data & (UINT32)~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT31);
        DEBUG((DEBUG_INFO, "Set FW ALL caps Rule Data = %x\n", RuleData));
        Status = HeciSetFwAllCapsSkuMsg (RuleData);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "HeciGetFwAllCapsSkuMsg Failed Status=0x%x\n", Status));
          return Status;
        }
      }

      // If AMT is enabled
      if (MsgGenGetFwCapsSkuAck.Data.FWCap.Data & AmtBits) {
        RuleData = MsgGenGetFwCapsSkuAck.Data.FWCap.Data & (UINT32)~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT31);
        DEBUG((DEBUG_INFO, "Set User Caps Rule Data = %x\n", RuleData));
        Status = HeciSetUserCapsSkuMsg (RuleData);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "HeciSetUserCapsSkuMsg Failed Status=0x%x\n", Status));
          return Status;
        }
      }

      Status = HeciGetInvocationCode (&RuleData);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "HeciGetInvocationCode Failed Status=0x%x\n", Status));
        return Status;
      } else {
        DEBUG ((DEBUG_INFO, "HeciGetInvocationCode Data = %x\n", RuleData));
      }

      if (RuleData) {
        Status = HeciClearInvocationCode (RuleData);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "HeciClearInvocationCode Failed Status=0x%x\n", Status));
          return Status;
        } else {
          DEBUG ((DEBUG_INFO, "HeciClearInvocationCode Data = %x\n", RuleData));
        }
      }
    }
  }

  return EFI_SUCCESS;
}
