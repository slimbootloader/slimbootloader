/** @file
  Intel(R) SGX library.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CCT_VS_VAR_INIT_DATA_H
#define _CCT_VS_VAR_INIT_DATA_H

#include "VariableCommon.h"

//
// This file provides a public API
//
#pragma pack(1)

typedef struct CCT_VS_META_NODE {
  CCT_VS_VAR_DESCRIPTOR    Descriptor;
  EFI_PHYSICAL_ADDRESS     DataPtr;
  EFI_PHYSICAL_ADDRESS     NextNodePtr;
} CCT_VS_METADATA_NODE;

#pragma pack()

EFI_STATUS
EFIAPI
CctVsMetaDataNodeParse (
  IN CCT_VS_METADATA_NODE       *StartMetadataNode,
  IN CCT_VS_VAR_DATA_INIT_PHASE CurrentPhase
  );

#endif // _CCT_VS_VAR_INIT_DATA_H
