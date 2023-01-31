/** @file

  Copyright (c) 2020-2023, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <IndustryStandard/Acpi.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Acpi.h>
#include <Library/HeciLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <PlatformData.h>
#include <PsdLib.h>
#include <MeBiosPayloadData.h>

#define PSD_VERSION_MAJOR                               0x0000
#define PSD_VERSION_MINOR                               0x0003
#define PSD_HROT_NONE                                   0
#define PSD_HROT_ROM                                    1
#define PSD_HROT_TXE                                    2
#define PSD_HROT_CSE                                    3
#define PSD_HROT_ACM                                    4
#define PSD_HROT_TXT                                    5

/**
 Get ME BIOS payload HOB data.

  @retval The ME BIOS payload HOB data

 **/
ME_BIOS_PAYLOAD *
GetMeBiosPayloadHobData (
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
 Get EOM state from CSE.

  @param[in,out] EomState         Pointer to The EOM state value.

  @retval EFI_SUCCESS             The EOM state was retrieved successfully.
  @retval Others                  An error occured.

 **/
EFI_STATUS
GetEomState (
    IN OUT UINT8 *EomState
    )
{
  EFI_STATUS    Status;
  UINT16        MeManuMode;

  if (EomState == NULL) {
    DEBUG ((DEBUG_ERROR, "EomState is not a valid pointer\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = HeciGetManufactureMode (&MeManuMode);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HeciGetManufactureMode Failed Status=0x%x\n", Status));
    return Status;
  }

  *EomState = (UINT8)MeManuMode;
  return EFI_SUCCESS;
}

/**
  Get Sec FW version from CSE.

  @param[in,out] SecVersion     Pointer to Sec FW version structure.

  @return EFI_SUCCESS           The Sec FW version was retrieved successfully.
  @return EFI_ERROR             An error occurred.

**/
EFI_STATUS
GetSecFwVersion (
    IN OUT SEC_VERSION_INFO *SecVersion
    )
{
  EFI_STATUS              Status;
  GEN_GET_FW_VER_ACK      MsgGenGetFwVersionAckData;
  ME_BIOS_PAYLOAD         *MbpHobData;

  if (SecVersion == NULL) {
    DEBUG ((DEBUG_ERROR, "SecVersion is not a valid pointer\n"));
    return EFI_INVALID_PARAMETER;
  }

  // Try with HOB first as HECI calls are costly
  MbpHobData = GetMeBiosPayloadHobData ();
  if (MbpHobData != NULL) {
    SecVersion->CodeMajor   =  MbpHobData->FwVersionName.MajorVersion;
    SecVersion->CodeMinor   =  MbpHobData->FwVersionName.MinorVersion;
    SecVersion->CodeBuildNo =  MbpHobData->FwVersionName.BuildVersion;
    SecVersion->CodeHotFix  =  MbpHobData->FwVersionName.HotfixVersion;
    return EFI_SUCCESS;
  }

  DEBUG((DEBUG_ERROR, "No MBP HOB available, calling HECI API\n"));
  Status = HeciGetFwVersionMsg ((UINT8 *)&MsgGenGetFwVersionAckData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HeciGetFwVersionMsg Failed Status=0x%x\n", Status));
    return Status;
  }

  SecVersion->CodeMajor   =   MsgGenGetFwVersionAckData.Data.CodeMajor;
  SecVersion->CodeMinor   =   MsgGenGetFwVersionAckData.Data.CodeMinor;
  SecVersion->CodeBuildNo =   MsgGenGetFwVersionAckData.Data.CodeBuildNo;
  SecVersion->CodeHotFix  =   MsgGenGetFwVersionAckData.Data.CodeHotFix;
  return EFI_SUCCESS;
}

/**
  Get Sec capabilities from CSE.

  @param[in] SecCapability    Pointer to Sec Capability structure.

  @retval EFI_SUCCESS         The Sec FW capabilities were retrieved successfully.
  @retval EFI_ERROR           An error occurred.

**/
EFI_STATUS
GetSecCapability (
  UINT32    *SecCapability
  )
{
  EFI_STATUS                      Status;
  GEN_GET_FW_CAPS_SKU_ACK         MsgGenGetFwCapsSkuAck;
  ME_BIOS_PAYLOAD                *MbpHobData;

  if (SecCapability == NULL) {
    DEBUG ((DEBUG_ERROR, "SecCapability is not a valid pointer\n"));
    return EFI_INVALID_PARAMETER;
  }

  // Try with HOB first as HECI calls are costly
  MbpHobData = GetMeBiosPayloadHobData ();
  if ((MbpHobData != NULL) && (MbpHobData->FwCapsSku.Available)) {
    *SecCapability = MbpHobData->FwCapsSku.FwCapabilities.Data;
    return EFI_SUCCESS;
  }

  DEBUG((DEBUG_ERROR, "No MBP HOB available, calling HECI API\n"));
  Status = HeciGetFwCapsSkuMsg ((UINT8 *)&MsgGenGetFwCapsSkuAck);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HeciGetFwCapsSkuMsg Failed Status=0x%x\n", Status));
    return Status;
  }

  *SecCapability = MsgGenGetFwCapsSkuAck.Data.FWCap.Data;
  return EFI_SUCCESS;
}

/**
  Update Platform Service Discovery Table.

  @param[in] Table      Pointer to ACPI Table Data.

  @retval EFI_SUCCESS   The PSD ACPI table was installed successfully.
  @retval EFI_ ERROR    An error occurred.

**/
EFI_STATUS
EFIAPI
UpdateAcpiPsdTable (
  IN VOID* Table
  )
{
  EFI_ACPI_PSD_TABLE             *mPsdt;
  PLATFORM_DATA                  *PlatformData;
  EFI_STATUS                     Status;

  DEBUG((DEBUG_VERBOSE, "UpdateAcpiPsdTable start\n"));
  if (Table == NULL) {
    DEBUG((DEBUG_WARN, "Table is not a valid pointer\n"));
    return EFI_INVALID_PARAMETER;
  }

  mPsdt = (EFI_ACPI_PSD_TABLE*)Table;
  mPsdt->Header.Signature         = EFI_ACPI_PSD_SIGNATURE;
  mPsdt->Header.Length            = sizeof(EFI_ACPI_PSD_TABLE);
  mPsdt->Header.Revision          = EFI_ACPI_PSD_TABLE_REVISION;
  mPsdt->Header.Checksum          = 0;

  if( &(mPsdt->Header.OemId) == NULL) {
    return RETURN_BUFFER_TOO_SMALL;
  }
  CopyMem (&mPsdt->Header.OemId, PSDS_EFI_ACPI_OEM_ID, sizeof(mPsdt->Header.OemId));

  mPsdt->Header.OemTableId        = PSDS_EFI_ACPI_OEM_TABLE_ID;
  mPsdt->Header.OemRevision       = PSDS_EFI_ACPI_OEM_REVISION;
  mPsdt->Header.CreatorId         = PSDS_EFI_ACPI_CREATOR_ID;
  mPsdt->Header.CreatorRevision   = PSDS_EFI_ACPI_CREATOR_REVISION;

  mPsdt->PsdVersion.PsdVerMajor   = PSD_VERSION_MAJOR;
  mPsdt->PsdVersion.PsdVerMinor   = PSD_VERSION_MINOR;

  // Populate EOM state
  Status = GetEomState (&mPsdt->EomState);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetEomState failed =%x\n", Status));
  }

  // Populate Sec capabilities
  Status = GetSecCapability (&mPsdt->CsmeSecCapabilities);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetSecCapability failed =%x\n", Status));
  }

  // Populate Sec FW version
  Status = GetSecFwVersion (&mPsdt->FwVer);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetSecFwVersion failed =%x\n", Status));
  }

  if( &(mPsdt->FwVendor) == NULL) {
    return RETURN_BUFFER_TOO_SMALL;
  }
  CopyMem (&mPsdt->FwVendor, EFI_ACPI_PSD_FW_VENDOR, EFI_ACPI_PSD_FW_VENDOR_SIZE);

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData == NULL) {
    DEBUG(( DEBUG_ERROR, "GetPlatformDataPtr Failed\n"));
    return EFI_UNSUPPORTED;
  }

  // Check if verifed boot is reflected in BtG profile as well as in SBL
  mPsdt->SecureBoot = (UINT8)(((PlatformData->BtGuardInfo.VerifiedBoot) << 1) | (FeaturePcdGet (PcdVerifiedBootEnabled)) << 2);

  // Check if measured boot is reflected in BtG profile
  mPsdt->MeasuredBoot = (UINT8)((PlatformData->BtGuardInfo.MeasuredBoot));

  // SBL always uses an ACM HWROT
  mPsdt->HwrotType              = PSD_HROT_ACM;
  DumpHex (2, 0, sizeof(EFI_ACPI_PSD_TABLE), (VOID *)Table);
  DEBUG( (DEBUG_VERBOSE, "UpdateAcpiPsdTable() end\n") );

  return  EFI_SUCCESS;
}
