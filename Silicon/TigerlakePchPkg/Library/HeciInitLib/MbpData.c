/** @file
  Retrieve MbpData and Print data.

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BootloaderCoreLib.h>
#include <Library/DebugLib.h>
#include "MeBiosPayloadData.h"

/**
  Dump MBP_DATA_PROTOCOL

  @param[in] MbpPtr              Pointer to MBP DATA HOB

**/
VOID
MbpDebugPrint (
  IN ME_BIOS_PAYLOAD       **MbpData
  )
{
  ME_BIOS_PAYLOAD *MbpPtr = *MbpData;
  if (MbpPtr == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO, "\n------------------------ MeBiosPayload Data -----------------\n"));
  DEBUG ((DEBUG_INFO, "MeBiosPayload FwVersionName ---\n"));
  DEBUG ((DEBUG_INFO, " ME FW MajorVersion  : 0x%x\n", MbpPtr->FwVersionName.MajorVersion));
  DEBUG ((DEBUG_INFO, " ME FW MinorVersion  : 0x%x\n", MbpPtr->FwVersionName.MinorVersion));
  DEBUG ((DEBUG_INFO, " ME FW HotfixVersion : 0x%x\n", MbpPtr->FwVersionName.HotfixVersion));
  DEBUG ((DEBUG_INFO, " ME FW BuildVersion  : 0x%x\n", MbpPtr->FwVersionName.BuildVersion));


  if (MbpPtr->FwCapsSku.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload FwCapabilities ---\n"));
    DEBUG ((DEBUG_INFO, " FullNet         : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.FullNet));
    DEBUG ((DEBUG_INFO, " StdNet          : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.StdNet));
    DEBUG ((DEBUG_INFO, " Manageability   : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.Manageability));
    DEBUG ((DEBUG_INFO, " EyeRiver        : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.EyeRiver));
    DEBUG ((DEBUG_INFO, " IntegratedTouch : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.IntegratedTouch));
    DEBUG ((DEBUG_INFO, " IntelCLS        : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.IntelCLS));
    DEBUG ((DEBUG_INFO, " ISH             : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.ISH));
    DEBUG ((DEBUG_INFO, " PAVP            : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.PAVP));
    DEBUG ((DEBUG_INFO, " IPV6            : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.IPV6));
    DEBUG ((DEBUG_INFO, " KVM             : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.KVM));
    DEBUG ((DEBUG_INFO, " DAL             : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.DAL));
    DEBUG ((DEBUG_INFO, " TLS             : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.TLS));
    DEBUG ((DEBUG_INFO, " WLAN            : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.WLAN));
    DEBUG ((DEBUG_INFO, " PTT             : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.PTT));
  }

  if (MbpPtr->FwFeaturesState.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload FwFeaturesState ---\n"));
    DEBUG ((DEBUG_INFO, " FullNet         : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.FullNet));
    DEBUG ((DEBUG_INFO, " StdNet          : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.StdNet));
    DEBUG ((DEBUG_INFO, " Manageability   : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.Manageability));
    DEBUG ((DEBUG_INFO, " EyeRiver        : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.EyeRiver));
    DEBUG ((DEBUG_INFO, " IntegratedTouch : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.IntegratedTouch));
    DEBUG ((DEBUG_INFO, " IntelCLS        : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.IntelCLS));
    DEBUG ((DEBUG_INFO, " ISH             : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.ISH));
    DEBUG ((DEBUG_INFO, " PAVP            : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.PAVP));
    DEBUG ((DEBUG_INFO, " IPV6            : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.IPV6));
    DEBUG ((DEBUG_INFO, " KVM             : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.KVM));
    DEBUG ((DEBUG_INFO, " DAL             : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.DAL));
    DEBUG ((DEBUG_INFO, " TLS             : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.TLS));
    DEBUG ((DEBUG_INFO, " WLAN            : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.WLAN));
    DEBUG ((DEBUG_INFO, " PTT             : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.PTT));
  }

  if (MbpPtr->FwPlatType.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload ME Platform TYpe ---\n"));
    DEBUG ((DEBUG_INFO, " PlatformTargetUsageType : 0x%x\n", MbpPtr->FwPlatType.RuleData.Fields.PlatformTargetUsageType));
    DEBUG ((DEBUG_INFO, " SuperSku                : 0x%x\n", MbpPtr->FwPlatType.RuleData.Fields.SuperSku));
    DEBUG ((DEBUG_INFO, " IntelMeFwImageType      : 0x%x\n", MbpPtr->FwPlatType.RuleData.Fields.IntelMeFwImageType));
    DEBUG ((DEBUG_INFO, " PlatformBrand           : 0x%x\n", MbpPtr->FwPlatType.RuleData.Fields.PlatformBrand));
  }

  DEBUG ((DEBUG_INFO, "MeBiosPayload IccProfile ---\n"));
  DEBUG ((DEBUG_INFO, " IccNumOfProfiles     : 0x%x\n",  MbpPtr->IccProfile.IccNumOfProfiles));
  DEBUG ((DEBUG_INFO, " IccProfileIndex      : 0x%x\n",  MbpPtr->IccProfile.IccProfileIndex));
  DEBUG ((DEBUG_INFO, " IccProfileChangeable : 0x%x\n",  MbpPtr->IccProfile.IccProfileChangeable));
  DEBUG ((DEBUG_INFO, " IccLockMaskType      : 0x%x\n",  MbpPtr->IccProfile.IccLockMaskType));

  if (MbpPtr->HwaRequest.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload HwaRequest ---\n"));
    DEBUG ((DEBUG_INFO, " MediaTablePush : 0x%x\n", MbpPtr->HwaRequest.Data.Fields.MediaTablePush));
  }

  if (MbpPtr->UnconfigOnRtcClearState.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload UnconfigOnRtcClearState ---\n"));
    DEBUG ((DEBUG_INFO, " UnconfigOnRtcClearData : 0x%x\n", MbpPtr->UnconfigOnRtcClearState.UnconfigOnRtcClearData.DisUnconfigOnRtcClearState));
  }

  if (MbpPtr->MeasuredBootSupport.Available) {
    DEBUG ((DEBUG_INFO, "ME Measured Boot Support available ---\n"));
    DEBUG ((DEBUG_INFO, "MeasuredBootSupport.MeasuredBootData.MeasuredBoot 0x%x\n", MbpPtr->MeasuredBootSupport.MeasuredBootData.MeasuredBoot));
    }

  if (MbpPtr->ArbSvnState.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload ArbSvnState ---\n"));
    DEBUG ((DEBUG_INFO, " Flags         : 0x%x\n", MbpPtr->ArbSvnState.ArbSvnData.Flags));
    DEBUG ((DEBUG_INFO, " MinCseArbSvn  : 0x%x\n", MbpPtr->ArbSvnState.ArbSvnData.MinCseArbSvn));
    DEBUG ((DEBUG_INFO, " CurrCseArbSvn : 0x%x\n", MbpPtr->ArbSvnState.ArbSvnData.CurrCseArbSvn));
  }

  if (MbpPtr->OemKeyRevoke.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "OemKeyRevocation Extension Available !! \n"));
  }

  DEBUG ((DEBUG_INFO, "\n------------------------ MeBiosPayload Data End--------------\n"));
}


/**
  Retrieve MBP data

  @retval     RETURN_SUCCESS       If getting the MBP data successfully
  @retval     EFI_NOT_FOUND        Failed to get MBP data
**/
EFI_STATUS
RetrieveMBPData (
  VOID
  )
{
  ME_BIOS_PAYLOAD       *MbpBiosPayload;
  VOID                  *FspHobList;
  UINT8                 *DataPtr;
  UINT32                MbpDataHobLen;

  FspHobList = GetFspHobListPtr ();
  if ( FspHobList == NULL ) {
    return EFI_NOT_FOUND;
  }

  DataPtr = GetGuidHobData (FspHobList, &MbpDataHobLen, &gMeBiosPayloadHobGuid);
  if ((DataPtr != NULL) && (MbpDataHobLen > 0)) {
    MbpBiosPayload = (ME_BIOS_PAYLOAD *) (DataPtr+4);
    // Get  address for ME_BIOS_PAYLOAD_HOB.ME_BIOS_PAYLOAD
    MbpDebugPrint(&MbpBiosPayload);
  } else {
    DEBUG ((DEBUG_INFO, "Could not find Mbp hob.\n"));
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}
