/** @file
  Library instances for ACPI Platform Security Discovery Table (PSD).

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/PsdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HeciLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <IndustryStandard/Acpi.h>
#include <MeBiosPayloadData.h>
#include <Library/BootloaderCoreLib.h>

#define PSD_VERSION_MAJOR                               0x0000
#define PSD_VERSION_MINOR                               0x0003
#define PSD_HROT_NONE                                   0
#define PSD_HROT_ROM                                    1
#define PSD_HROT_TXE                                    2
#define PSD_HROT_CSE                                    3
#define PSD_HROT_ACM                                    4
#define PSD_HROT_TXT                                    5

//
// ACPI PSD table template
//
CONST
EFI_ACPI_PSD_TABLE mAcpiPsdTableTemplate = {
  .Header.Signature       = EFI_ACPI_PSD_SIGNATURE,
  .Header.Length          = sizeof (EFI_ACPI_PSD_TABLE),
  .Header.Revision        = EFI_ACPI_PSD_TABLE_REVISION,
  .Header.OemTableId      = FixedPcdGet64 (PcdAcpiDefaultOemTableId),
  .Header.OemRevision     = FixedPcdGet32 (PcdAcpiDefaultOemRevision),
  .Header.CreatorId       = FixedPcdGet32 (PcdAcpiDefaultCreatorId),
  .Header.CreatorRevision = FixedPcdGet32 (PcdAcpiDefaultCreatorRevision),
};

/**
 Get ME BIOS payload HOB data.

  @retval The ME BIOS payload HOB data

 **/
ME_BIOS_PAYLOAD *
GetMeBiosPayloadHobData (
  VOID
  )
{
  VOID                    *FspHobListPtr;
  UINT32                  MbpDataHobLen;
  UINT8                   *DataPtr;

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
  IN OUT UINT8            *EomState
  )
{
  EFI_STATUS              Status;
  UINT16                  MeManuMode;

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
    SecVersion->CodeMajor   = MbpHobData->FwVersionName.MajorVersion;
    SecVersion->CodeMinor   = MbpHobData->FwVersionName.MinorVersion;
    SecVersion->CodeBuildNo = MbpHobData->FwVersionName.BuildVersion;
    SecVersion->CodeHotFix  = MbpHobData->FwVersionName.HotfixVersion;
    return EFI_SUCCESS;
  }

  DEBUG((DEBUG_ERROR, "No MBP HOB available, calling HECI API\n"));
  Status = HeciGetFwVersionMsg ((UINT8 *)&MsgGenGetFwVersionAckData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HeciGetFwVersionMsg Failed Status=0x%x\n", Status));
    return Status;
  }

  SecVersion->CodeMajor    = MsgGenGetFwVersionAckData.Data.CodeMajor;
  SecVersion->CodeMinor    = MsgGenGetFwVersionAckData.Data.CodeMinor;
  SecVersion->CodeBuildNo  = MsgGenGetFwVersionAckData.Data.CodeBuildNo;
  SecVersion->CodeHotFix   = MsgGenGetFwVersionAckData.Data.CodeHotFix;
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
  UINT32                  *SecCapability
  )
{
  EFI_STATUS              Status;
  GEN_GET_FW_CAPS_SKU_ACK MsgGenGetFwCapsSkuAck;
  ME_BIOS_PAYLOAD         *MbpHobData;

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
  Initialize and add an ACPI PSD (Platform Security Discovery) table.

  This function initializes the ACPI PSD table with standard fields such as
  signature, length, revision, OEM information, and creator details.

  @param[in]                   Table  Pointer to the ACPI table to be initialized.

  @retval EFI_SUCCESS            The header was initialized successfully.
  @retval EFI_INVALID_PARAMETER  Invalid parameter provided.
**/
EFI_STATUS
EFIAPI
UpdateAcpiPsdTable (
  IN VOID*               Table
  )
{
  EFI_ACPI_PSD_TABLE      *Psdt;
  EFI_STATUS              Status;

  if (Table == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Psdt = (EFI_ACPI_PSD_TABLE *)Table;
  CopyMem (Psdt, &mAcpiPsdTableTemplate, sizeof (EFI_ACPI_PSD_TABLE));
  CopyMem (Psdt->Header.OemId, FixedPcdGetPtr (PcdAcpiDefaultOemId), 6);

  Psdt->PsdVersion.PsdVerMajor = PSD_VERSION_MAJOR;
  Psdt->PsdVersion.PsdVerMinor = PSD_VERSION_MINOR;

  // Populate EOM state
  Status = GetEomState (&Psdt->EomState);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetEomState failed =%x\n", Status));
    return Status;
  }

  // Populate Sec capabilities
  Status = GetSecCapability (&Psdt->CsmeSecCapabilities);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetSecCapability failed =%x\n", Status));
    return Status;
  }

  // Populate Sec FW version
  Status = GetSecFwVersion (&Psdt->FwVer);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetSecFwVersion failed =%x\n", Status));
    return Status;
  }

  CopyMem (&Psdt->FwVendor, EFI_ACPI_PSD_FW_VENDOR, EFI_ACPI_PSD_FW_VENDOR_SIZE);

  // Platform supports ACM as HWRoT
  // 0 - No HWRoT; 1 - ROM based RoT; 2 - TXE; 3 - CSE; 4 - ACM; 5 - TXT
  Psdt->HwrotType = PSD_HROT_ACM;

  DEBUG ((DEBUG_INFO, "PSD table initialized\n"));
  return EFI_SUCCESS;
}
