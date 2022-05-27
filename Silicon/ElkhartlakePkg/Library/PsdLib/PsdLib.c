/** @file

  Copyright (c) 2018 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/SocInitLib.h>
#include <IndustryStandard/Acpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Tpm20.h>
#include <Library/HeciInitLib.h>
#include <Library/HeciInitLib/MkhiMsgs.h>
#include <Library/PciLib.h>
#include <Library/BootGuardLib.h>
#include <Library/SecureBootLib.h>
#include <Guid/PcdDataBaseSignatureGuid.h>
#include <Library/ConfigDataLib.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Guid/FlashMapInfoGuid.h>
#include <PlatformData.h>
#include <Library/BootGuardLib.h>
#include <Library/PsdLib.h>
#include <Register/SaRegs.h>
#include <PchAccess.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataStruct.h>
#include <Library/HeciInitLib.h>
#include <Library/HeciInitLib/MeBiosPayloadData.h>

#define PSD_VERSION_MAJOR                               0x0000
#define PSD_VERSION_MINOR                               0x0003
#define PSD_HROT_NONE                                   0
#define PSD_HROT_ROM                                    1
#define PSD_HROT_TXE                                    2
#define PSD_HROT_CSE                                    3
#define PSD_HROT_ACM                                    4
#define PSD_HROT_TXT                                    5

/**
  Wrapper function to Get EOM status from CSE Status Register.

  @param[in,out] EomState         Pointer to The EOM state value.

  @retval EFI_SUCCESS         The EOM Get Status successfully.
  @retval Others              The EOM Get from CSE doesn't get status.

 **/
EFI_STATUS
GetEomState (
    IN OUT UINT8 *EomState
    )
{
  EFI_STATUS            Status;
  UINT32                FwSts;

  if(EomState == NULL) {
    DEBUG ((DEBUG_ERROR, "EomState is not valid pointer\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = ReadHeciFwStatus( &FwSts);
  if (Status == EFI_SUCCESS ) {
    *EomState = (UINT8)((FwSts & BIT4) >> 4);
   }
  return Status;
}

/**
Get Sec FW Version.
@param[in]: SecVersion Pointer to  Sec FW Version structure.
@return EFI_SUCCESS.
@return EFI_ERROR.

**/
EFI_STATUS
GetSecFwVersion (
    SEC_VERSION_INFO *SecVersion
    )
{
  EFI_STATUS            Status;
  GET_FW_VER_ACK    MsgGenGetFwVersionAckData;

  if(SecVersion == NULL) {
    DEBUG ((DEBUG_ERROR, "GetSecFwVersion Failed Status=0x%x\n",EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }

  Status = HeciGetFwVersionMsg( (UINT8 *)&MsgGenGetFwVersionAckData);
  if (Status == EFI_SUCCESS) {
    SecVersion->CodeMajor = MsgGenGetFwVersionAckData.Data.CodeMajor;
    SecVersion->CodeMinor = MsgGenGetFwVersionAckData.Data.CodeMinor;
    SecVersion->CodeHotFix = MsgGenGetFwVersionAckData.Data.CodeHotFix;
    SecVersion->CodeBuildNo = MsgGenGetFwVersionAckData.Data.CodeBuildNo;
  }

  return Status;
}

/**
Get Sec Capabilities of CSE.
@param[in] SecCapability Pointer to Sec Caps.
@retval EFI_SUCCESS Success get all FW hash value
@retval EFI_ERROR Unable to get hash value

**/
EFI_STATUS
GetSecCapability (
  UINT32    *SecCapability
  )
{
  EFI_STATUS                 Status;
  ME_BIOS_PAYLOAD            *MbpBiosPayload;
  VOID                       *FspHobList;
  UINT8                      *DataPtr;
  UINT32                     MbpDataHobLen;
  GET_FW_CAPSKU              MsgGenGetFwCapsSku;
  GET_FW_CAPS_SKU_ACK        MsgGenGetFwCapsSkuAck;

  if(SecCapability == NULL) {
    DEBUG ((DEBUG_ERROR, "GetSecCapability Failed Status=0x%x\n",EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }

  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL ) {
    DataPtr = GetGuidHobData (FspHobList, &MbpDataHobLen, &gMeBiosPayloadHobGuid);
    if ((DataPtr != NULL) && (MbpDataHobLen > 0)) {
      MbpBiosPayload = (ME_BIOS_PAYLOAD *) (DataPtr + 4);
      if ((MbpBiosPayload != NULL) && (MbpBiosPayload->FwCapsSku.Available)) {
        *SecCapability = MbpBiosPayload->FwCapsSku.FwCapabilities.Data;

        return EFI_SUCCESS;
      }
    }
  }
  // Fill the FW capability data if MBP doesn't exist
  DEBUG((DEBUG_INFO, "GetSecCapability from HECI\n"));

  Status = HeciGetFwCapsSkuMsg ( (UINT8 *)&MsgGenGetFwCapsSku, (UINT8 *)&MsgGenGetFwCapsSkuAck);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  *SecCapability = MsgGenGetFwCapsSkuAck.Data.FWCap.Data;
  return EFI_SUCCESS;
}

/**

  Update Platform Service Discovery Table.
  @param[in] Table Pointer of ACPI Table Data.
  @retval EFI_SUCCESS Installed PSD ACPI table successfully.
  @retval EFI_ ERROR.

**/
EFI_STATUS
EFIAPI
UpdateAcpiPsdTable (
  IN VOID* Table
  )
{
  EFI_ACPI_PSD_TABLE             *mPsdt;
  PLATFORM_DATA                   *PlatformData;
  EFI_STATUS                      Status;

  DEBUG((DEBUG_INFO, "UpdateAcpiPsdTable start\n"));
  if ( Table == NULL) {
    DEBUG((DEBUG_WARN, "EFI_ACPI_PSD_TABLE IS NULL\n"));
    return EFI_BUFFER_TOO_SMALL;
  }
  mPsdt = (EFI_ACPI_PSD_TABLE*)Table;
  // Populate Platfrom security capabilities in table structure
  mPsdt->Header.Signature               = EFI_ACPI_PSD_SIGNATURE;
  mPsdt->Header.Checksum                = 0;

  if( &(mPsdt->Header.OemId) == NULL) {
    return RETURN_BUFFER_TOO_SMALL;
  }
  CopyMem(&mPsdt->Header.OemId, PSDS_EFI_ACPI_OEM_ID, 6);
  mPsdt->Header.OemTableId              = PSDS_EFI_ACPI_OEM_TABLE_ID;
  mPsdt->Header.OemRevision             = PSDS_EFI_ACPI_OEM_REVISION;
  mPsdt->Header.CreatorId               = PSDS_EFI_ACPI_CREATOR_ID;
  mPsdt->Header.CreatorRevision         = PSDS_EFI_ACPI_CREATOR_REVISION;

  mPsdt->PsdVersion.PsdVerMajor = PSD_VERSION_MAJOR;
  mPsdt->PsdVersion.PsdVerMinor = PSD_VERSION_MINOR;

  //Eom State,
  Status = GetEomState(&mPsdt->EomState);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetEomState failed =%x\n",Status));
  }

  //Sec Capabilities,
  Status = GetSecCapability( &(mPsdt->CsmeSecCapabilities) );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetSecCapability failed =%x\n",Status));
  }

  //FW version,
  Status = GetSecFwVersion( &(mPsdt->FwVer) );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetSecCFwVersion failed =%x\n",Status));
  }

  if( &(mPsdt->FwVendor) == NULL) {
    return RETURN_BUFFER_TOO_SMALL;
  }

  CopyMem(&mPsdt->FwVendor, EFI_ACPI_PSD_FW_VENDOR, EFI_ACPI_PSD_FW_VENDOR_SIZE);
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr();
  if (PlatformData == NULL) {
    DEBUG(( DEBUG_ERROR, "PSD Values:  GetPlatformDataPtr Failed\n" ));
    return  EFI_UNSUPPORTED;
  }

  //000 - Secure boot is Disabled; 010 - Boot Guard Enabled; 100 - Bootloader Verified boot Enabled.
  mPsdt->SecureBoot = (UINT8)(((PlatformData->BtGuardInfo.VerifiedBoot) << 1)| (FeaturePcdGet (PcdVerifiedBootEnabled)) << 2);
  //Measured boot enabled.
  mPsdt->MeasuredBoot = (UINT8)((PlatformData->BtGuardInfo.MeasuredBoot));

  //0 - No HWRoT; 1 - ROM based RoT; 2 - TXE; 3 - CSE; 4 - ACM; 5 - TXT
  mPsdt->HwrotType = PSD_HROT_ACM;
  //DumpHex (2, 0, sizeof(EFI_ACPI_PSD_TABLE), (VOID *)Table);
  DEBUG( (DEBUG_INFO, "UpdateAcpiPsdTable() end\n") );

  return  EFI_SUCCESS;
}
