/** @file
  This file contains definitions of TCC Tuning Data.

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TCC_DATA_GUID_H_
#define _TCC_DATA_GUID_H_


#define MAX_TCC_BUFFERS   (9)

#pragma pack (push,1)
extern EFI_GUID gTccInfoHobGuid;
///
/// TCC Data Hob
///
typedef struct {
  UINT64                BufferSubregionAddress;
  UINT32                BufferSubregionSize;
  EFI_PHYSICAL_ADDRESS  BufferAddress[MAX_TCC_BUFFERS];
  EFI_PHYSICAL_ADDRESS  PtcmBinaryAddress;
  UINT32                PtcmBinarySize;
  UINT64                StreamCfgSubregionAddress;
  UINT32                StreamCfgSubregionSize;
} TCC_INFO_DATA;


typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType; ///< EFI Hob Guid Type.
  TCC_INFO_DATA     Data;           ///< TCC Info Data.
} TCC_INFO_HOB;                     ///< TCC Info HOB
#pragma pack (pop)
#endif
