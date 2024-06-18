/** @file
  This file contains definitions of IOE Info HOB.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _MTL_IOE_INFO_HOB_H_
#define _MTL_IOE_INFO_HOB_H_

extern EFI_GUID gMtlIoeInfoHobGuid;

#pragma pack (push,1)

/**
  MTL_IOE_INFO_HOB is used to identify MTL IOE series
  and stepping after the IOE PMC device has been hidden
  and is no longer accessible on PCI bus.
**/
typedef struct {
  UINT32       IoePmcRevId      :  8; //< IO Expander PMC Revision ID
  UINT32       IoePmcDevId      : 16; //< IO Expander PMC Device ID
  UINT32       ReservedBits     :  8;
} MTL_IOE_INFO_HOB;

#pragma pack (pop)

#endif
