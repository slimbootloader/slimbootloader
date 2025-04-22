/** @file
  This file defines the hob structure for the Secure boot information.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SECUREBOOT_INFO_GUID_H__
#define __SECUREBOOT_INFO_GUID_H__

///
/// Secure Boot info Hob GUID
///
extern EFI_GUID gSecureBootInfoGuid;

#define PAYLOAD_SECUREBOOT_INFO_HOB_REVISION  0x1

#define NO_TPM               0x0
#define TPM_TYPE_12          0x1
#define TPM_TYPE_20          0x2

#pragma pack(1)
typedef struct {
  UNIVERSAL_PAYLOAD_GENERIC_HEADER    Header;
  UINT8                               VerifiedBootEnabled;
  UINT8                               MeasuredBootEnabled;
  UINT8                               FirmwareDebuggerInitialized;
  UINT8                               TpmType;
  UINT32                              TpmPcrActivePcrBanks;
} SECUREBOOT_INFO;
#pragma pack()

#endif
