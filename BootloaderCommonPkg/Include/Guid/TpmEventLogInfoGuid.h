/** @file
  This file defines the hob structure for the TPM event log.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __TPM_EVENT_LOG_INFO_GUID_H__
#define __TPM_EVENT_LOG_INFO_GUID_H__

///
/// Tpm event log info Hob GUID
///
extern EFI_GUID gTpmEventLogInfoGuid;

typedef struct {
  UINT8  Revision;
  UINT8  Reserved0[3];
  UINT32 EventSize;
  UINT32 Tcg12Lasa;
  UINT32 Tcg12LastEvt;
  UINT32 Tcg2Lasa;
  UINT32 Tcg2LasaEven;
} TPM_EVENT_LOG_INFO;

#endif
