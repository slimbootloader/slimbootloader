/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef __FSP_VARIABLE_SERVICES_LIB_INTERNAL_H__
#define __FSP_VARIABLE_SERVICES_LIB_INTERNAL_H__

#include <Uefi.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/FspVariableServicesLib.h>
#include <Library/VariableLib.h>
#include <Library/BlMemoryAllocationLib.h>

#define FSP_LITE_VAR_SIGNATURE            SIGNATURE_32('F', 'S', 'P', 'V')
#define ASCII_GUID_LENGTH                 36    //ASCII GUIDs have 36 CHARs (32 + 4 hyphens)

typedef struct {
  UINT32    Signature;          //'FSPV'
  UINT32    Attributes;         // EFI attributes field
  UINT8     Data[0];
} FSP_LITE_VARIABLE_HEADER;


#endif //__FSP_VARIABLE_SERVICES_LIB_INTERNAL_H__
