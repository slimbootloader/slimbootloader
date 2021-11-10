/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PAYLOAD_ENTRY_LIB_H__
#define __PAYLOAD_ENTRY_LIB_H__

typedef  struct {
  UINT32   Signature;
  UINT32   HeapBase;
  UINT32   HeapSize;
  UINT32   RsvdBase;
  UINT32   RsvdSize;
  UINT32   Argc;
  UINT32   Argv[4];
} PLD_EXTRA_MOD_ARGS;

typedef  EFI_STATUS (EFIAPI *PLD_MODULE_ENTRY) \
         (VOID *HobList, PLD_EXTRA_MOD_ARGS *Params);

/**
  The payload common Entry Point for C code.

  @param[in] Params         The HOB list pointer for payload.
  @param[in] PldBase        Address of the payload image base.

**/
VOID
EFIAPI
SecStartup (
  IN VOID     *Params,
  IN VOID     *PldBase
  );

#endif /* __PAYLOAD_ENTRY_LIB_H__ */
