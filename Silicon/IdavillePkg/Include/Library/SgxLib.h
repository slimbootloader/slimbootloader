/** @file
  Intel(R) SGX library.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef SGXLIB_H
#define SGXLIB_H

#include <FspmUpd.h>
#include <FspsUpd.h>
//
// SGX OS Agent external interface
//
#define SGX_REGISTRATION_CONFIG_VARIABLE_NAME                  L"SgxRegistrationConfiguration"
#define SGX_REGISTRATION_REQUEST_VARIABLE_NAME                 L"SgxRegistrationServerRequest"
#define SGX_REGISTRATION_PACKAGE_INFO_VARIABLE_NAME            L"SgxRegistrationPackageInfo"
#define SGX_REGISTRATION_RESPONSE_VARIABLE_NAME                L"SgxRegistrationServerResponse"
#define SGX_REGISTRATION_STATUS_VARIABLE_NAME                  L"SgxRegistrationStatus"
#define SGX_UEFI_REGISTRATION_CONFIG_VARIABLE_NAME             L"SgxUefiRegistrationConfiguration"
#define SGX_UEFI_REGISTRATION_REQUEST_VARIABLE_NAME            L"SgxUefiRegistrationServerRequest"
#define SGX_UEFI_REGISTRATION_RESPONSE_VARIABLE_NAME           L"SgxUefiRegistrationServerResponse"
#define SGX_UEFI_REGISTRATION_STATUS_VARIABLE_NAME             L"SgxUefiRegistrationStatus"
#define SGX_LEGACY_REGISTRATION_SW_GUARD_STATUS_VARIABLE_NAME  L"SOFTWAREGUARDSTATUS"
#define SGX_LEGACY_REGISTRATION_EPC_BIOS_VARIABLE_NAME         L"EPCBIOS"
#define SGX_LEGACY_REGISTRATION_EPC_SW_VARIABLE_NAME           L"EPCSW"
#define SGX_UEFIFW_NONVOLATILVE_REGISTRATION_STATE_VARIABLE_NAME L"SgxRegistrationState"
#define SGX_UEFIFW_KEY_BLOB_VARIABLE_NAME                        L"SgxKeyBlobs"
#define SGX_UEFIFW_PLATFORM_MANIFEST_VARIABLE_NAME               L"SgxPlatformManifest"
#define SECURITY_IP_INTERDEPENDENCE_HASH_NAME                    L"SecurityIpInterdependenceHash"
#define SGX_SBL_DISABLED_SCENARIO_NAME                           L"SgxSblDisabledScenario"

extern EFI_GUID gSgxUefiFwRegistrationStateVariableGuid;
extern EFI_GUID gSgxUefiFwKeyBlobsVariableGuid;
extern EFI_GUID gSgxUefiFwPlatformManifestVariableGuid;

// Registration Protocol Variables UUID
extern EFI_GUID gSgxRegistrationConfigVariableGuid;
extern EFI_GUID gSgxRegistrationServerRequestVariableGuid;
extern EFI_GUID gSgxRegistrationPackageInfoVariableGuid;
extern EFI_GUID gSgxRegistrationServerResponseVariableGuid;
extern EFI_GUID gSgxRegistrationStatusVariableGuid;
// Backup Registration Protocol Variables UUID in Flash
extern EFI_GUID gSgxUefiRegistrationConfigVariableGuid;
extern EFI_GUID gSgxUefiRegistrationServerRequestVariableGuid;
extern EFI_GUID gSgxUefiRegistrationServerResponseVariableGuid;
extern EFI_GUID gSgxUefiRegistrationStatusVariableGuid;
// Other Registration Variables UUID
extern EFI_GUID gSgxRegistrationServerInfoGuid;
// Legacy Registration Variables UUID
extern EFI_GUID gSgxLegacyRegistrationSoftwareGuardStatusVariableGuid;
extern EFI_GUID gSgxLegacyRegistrationEpcBiosVariableGuid;
extern EFI_GUID gSgxLegacyRegistrationEpcSwVariableGuid;
// Note this is for Idaville pkg only
//
// SGX OS Agent external interface
//
#define SGX_REGISTRATION_CONFIG_VARIABLE_NAME                  L"SgxRegistrationConfiguration"
#define SGX_REGISTRATION_REQUEST_VARIABLE_NAME                 L"SgxRegistrationServerRequest"
#define SGX_REGISTRATION_PACKAGE_INFO_VARIABLE_NAME            L"SgxRegistrationPackageInfo"
#define SGX_REGISTRATION_RESPONSE_VARIABLE_NAME                L"SgxRegistrationServerResponse"
#define SGX_REGISTRATION_STATUS_VARIABLE_NAME                  L"SgxRegistrationStatus"
#define SGX_UEFI_REGISTRATION_CONFIG_VARIABLE_NAME             L"SgxUefiRegistrationConfiguration"
#define SGX_UEFI_REGISTRATION_REQUEST_VARIABLE_NAME            L"SgxUefiRegistrationServerRequest"
#define SGX_UEFI_REGISTRATION_RESPONSE_VARIABLE_NAME           L"SgxUefiRegistrationServerResponse"
#define SGX_UEFI_REGISTRATION_STATUS_VARIABLE_NAME             L"SgxUefiRegistrationStatus"
#define SGX_LEGACY_REGISTRATION_SW_GUARD_STATUS_VARIABLE_NAME  L"SOFTWAREGUARDSTATUS"
#define SGX_LEGACY_REGISTRATION_EPC_BIOS_VARIABLE_NAME         L"EPCBIOS"
#define SGX_LEGACY_REGISTRATION_EPC_SW_VARIABLE_NAME           L"EPCSW"
#define SGX_UEFIFW_NONVOLATILVE_REGISTRATION_STATE_VARIABLE_NAME L"SgxRegistrationState"
#define SGX_UEFIFW_KEY_BLOB_VARIABLE_NAME                        L"SgxKeyBlobs"
#define SGX_UEFIFW_PLATFORM_MANIFEST_VARIABLE_NAME               L"SgxPlatformManifest"

// Registration Protocol Variables UUID
extern EFI_GUID gSgxRegistrationConfigVariableGuid;
extern EFI_GUID gSgxRegistrationServerRequestVariableGuid;
extern EFI_GUID gSgxRegistrationPackageInfoVariableGuid;
extern EFI_GUID gSgxRegistrationServerResponseVariableGuid;
extern EFI_GUID gSgxRegistrationStatusVariableGuid;
// Backup Registration Protocol Variables UUID in Flash
extern EFI_GUID gSgxUefiRegistrationConfigVariableGuid;
extern EFI_GUID gSgxUefiRegistrationServerRequestVariableGuid;
extern EFI_GUID gSgxUefiRegistrationServerResponseVariableGuid;
extern EFI_GUID gSgxUefiRegistrationStatusVariableGuid;
// Other Registration Variables UUID
extern EFI_GUID gSgxRegistrationServerInfoGuid;
// Legacy Registration Variables UUID
extern EFI_GUID gSgxLegacyRegistrationSoftwareGuardStatusVariableGuid;
extern EFI_GUID gSgxLegacyRegistrationEpcBiosVariableGuid;
extern EFI_GUID gSgxLegacyRegistrationEpcSwVariableGuid;
extern EFI_GUID gSgxUefiFwRegistrationStateVariableGuid;
extern EFI_GUID gSgxUefiFwKeyBlobsVariableGuid;
extern EFI_GUID gSgxUefiFwPlatformManifestVariableGuid;


#define SHA384_DIGEST_SIZE                  48


typedef enum {
  SgxUefiEnabled       = 0x00,
  SgxUefiDisabled      = 0x01,
  SgxUefiStatusRead    = 0x02,
  SgxUefiDisabledFinal = 0x03
} SGX_SBL_DISABLED_SCENARIO;

typedef struct {
  UINT64 EpcBaseAddress;
  UINT64 EpcLengthAddress;
  UINT32 NumOfEpc;
} SGX_EPC_ADDRESS;

typedef struct {
  UINT8 Revision;
  UINT8 Reserved0[3];
  UINT8 SgxSblCfgState;
  UINT8 SgxStatusSize;
  UINT8 Reserved1[2];
  UINT64 SgxStatusAddr;
  SGX_EPC_ADDRESS EpcAddress;
} SGX_SBL_DATA_INFO_HOB;
//////////////////////////////////////////////////////////////////////////////////////////////////
/**
  Check if SGX instruction set is supported by the processor

  @retval TRUE if SGX capability is supported by processor
  @retval FALSE if SGX capability is not supported by processor

**/
BOOLEAN
EFIAPI
IsSgxCapSupported (
  VOID
  );

/**
  Check if input PRMRR value is supported by the processor

  @param[in]  PrmrrSizeFromConfig  PrmrrSize value entered in SGX config

  @retval     TRUE if PrmrrSizeFromConfig is supported, FALSE otherwise

**/
BOOLEAN
EFIAPI
IsPrmrrSizeSupported (
  IN UINT32 PrmrrSizeFromConfig
  );

/**
  Update FSP-M UPD SGX config data

  @param[in]  FspmUpd           Pointer to the FSP-m UPD to be updated

  @retval TRUE if SGX config was found and set
  @retval FALSE if SGX config was not found

**/
BOOLEAN
EFIAPI
UpdateFspmSgxConfig (
  IN FSPM_UPD *FspmUpd
  );

/**
  Update FSP-S UPD SGX config data

  @param[in]  FspsUpd           Pointer to the FSP-S UPD to be updated

  @retval TRUE if SGX config was found and set
  @retval FALSE if SGX config was not found

**/
BOOLEAN
EFIAPI
UpdateFspsSgxConfig (
  IN FSPS_UPD *FspsUpd
  );

/**
  Get SGX Registration Config Variable
  @param[out]  SgxRegistrationConfig    SGX Registration config
  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxRegistrationConfigVariable (
  VOID
);

/**
  Get SGX Registration Package Info Variable
  @param[out]  SgxRegistrationPackageInfo    SGX Registration Package info
  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxRegistrationPackageInfoVariable (
  VOID
);

/**
  Get SGX Registration Server Response Variable
  @param[out]  SgxRegistrationResponse    SGX Registration Server Response
  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxRegistrationServerResponseVariable (
  VOID
);

/**
  Get SGX Registration Status Variable
  @param[out]  SgxRegistrationStatus    SGX Registration status
  @retval  EFI_STATUS    Result of nvm read
**/
EFI_STATUS
EFIAPI
RdNvGetSgxRegistrationStatusVariable (
  VOID
);

/**
  Free Allocated memory for SGX variables
**/
VOID
EFIAPI
FreeSgxMem (
  VOID
);

VOID
EFIAPI
BuildSgxSblDataInfoHob (
  VOID
);

#endif // SGXLIB_H
