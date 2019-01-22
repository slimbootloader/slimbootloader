/** @file
  This file defines the hob structure for the SMM related info.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SMM_INFORMATION_GUID_H__
#define __SMM_INFORMATION_GUID_H__

///
/// Serial Port Information GUID
///
extern EFI_GUID gSmmInformationGuid;

typedef struct {
  UINT8    Revision;
  UINT8    Flags;
  UINT8    Reserved[2];
  UINT32   SmmBase;
  UINT32   SmmSize;
} SMM_INFORMATION;

#endif
