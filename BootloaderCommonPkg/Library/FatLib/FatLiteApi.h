/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _FAT_LITE_API_H_
#define _FAT_LITE_API_H_

//
// API data structures
//
typedef VOID  *PEI_FILE_HANDLE;

typedef enum {
  Fat12,
  Fat16,
  Fat32,
  FatUnknown
} PEI_FAT_TYPE;

#endif
