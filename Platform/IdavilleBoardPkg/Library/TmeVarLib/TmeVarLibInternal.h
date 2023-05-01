/** @file

  Copyright (c) 2021 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TME_VAR_LIB_INTERNAL_H_
#define _TME_VAR_LIB_INTERNAL_H_

#define TME_KEY_VAR     L"TME_KEYS"

typedef struct {
    UINT64   TME_Key0;
    UINT64   TME_Key1;
    UINT8    Lock;
    UINT8    Enable;
    UINT8    KeySave;
    UINT8    Reserved[5];
} TME_STATUS_HOB_DATA;

extern EFI_GUID gEfiFspTmeStatusDataHobGuid;

#endif // _TME_VAR_LIB_INTERNAL_H_
