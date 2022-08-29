/** @file
  This file defines the hob structure for the TPM event log.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __TPM_EVENT_LOG_INFO_GUID_H__
#define __TPM_EVENT_LOG_INFO_GUID_H__

#define PAYLOAD_TPM_EVENT_LOG_HOB_REVISION 0x1

///
/// Tpm event log info Hob GUID
///
extern EFI_GUID gTpmEventLogInfoGuid;

typedef struct {
  UINT8  Revision;
  UINT8  Reserved[3];
  UINT32 Tcg2EventSize;
  UINT64 Tcg2Lasa;
} TPM_EVENT_LOG_INFO;

#endif
