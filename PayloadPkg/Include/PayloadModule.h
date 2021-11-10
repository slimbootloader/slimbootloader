/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef __PLD_MODULE_H__
#define __PLD_MODULE_H__

typedef VOID * (EFIAPI *GET_PROC_ADDRESS) (IN CHAR8 *Name);

#pragma pack(1)

typedef struct {
  GET_PROC_ADDRESS           GetProcAddress;
  CHAR8                     *CmdLineBuf;
  UINTN                      CmdLineLen;
} PLD_MOD_PARAM;

#pragma pack()

typedef VOID   (EFIAPI *PLD_MOD_ENTRY) (PLD_MOD_PARAM *Params);

#endif
