/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PAYLOAD_ENTRY_LIB_H__
#define __PAYLOAD_ENTRY_LIB_H__

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
