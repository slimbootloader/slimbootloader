/** @file
  Header file for TCC Lib.

  Copyright (c) 2021 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TCC_INFO_H_
#define _TCC_INFO_H_

#include <Library/TccPlatformLib.h>
///
/// TCC Data Hob
///
typedef struct {
  UINT64                BiosConfigSubRegionAddress;
  UINT32                BiosConfigSubRegionSize;
  UINT64                BufferSubregionAddress;
  UINT32                BufferSubregionSize;
  EFI_PHYSICAL_ADDRESS  BufferAddress[MAX_TCC_BUFFERS];
  UINT64                StreamCfgSubRegionAddress;
  UINT32                StreamCfgSubRegionSize;
  EFI_PHYSICAL_ADDRESS  PtcmBinaryAddress;
  UINT32                PtcmBinarySize;
} TCC_INFO_DATA;

typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType; ///< EFI Hob Guid Type.
  TCC_INFO_DATA     Data;           ///< TCC Info Data.
} TCC_INFO_HOB;                     ///< TCC Info HOB


#endif
