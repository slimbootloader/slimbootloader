/** @file

Copyright (c) 2020, Intel Corporation. All rights reserved.

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PRE_OS_H__
#define __PRE_OS_H__


typedef struct {
  UINT32 Eax;
  UINT32 Ebx;
  UINT32 Ecx;
  UINT32 Edx;
  UINT32 Esi;
  UINT32 Edi;
  UINT32 Ebp;
  UINT32 Eip;
  UINT32 Eflags;
} OS_BOOT_INFO;


typedef struct {
  UINT32        Version;
  UINT32        HeapSize;
  UINT32        HeapAddr;
  OS_BOOT_INFO  OsBootState;
  UINT32        HobListPtr;
} PRE_OS_PARAM;

typedef VOID (*PRE_OS_ENTRYPOINT) (VOID *Params);



#endif
