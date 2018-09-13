/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PAYLOAD_ENTRY_LIB_H__
#define __PAYLOAD_ENTRY_LIB_H__

/**
  The payload common Entry Point for C code.

  @param[in] Params         The HOB list pointer for payload.

**/
VOID
EFIAPI
SecStartup (
  IN VOID     *Params
  );

#endif /* __PAYLOAD_ENTRY_LIB_H__ */
