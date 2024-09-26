/** @file
  Header file for FSP PCH config HOB.

@copyright
  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MTL_PCH_CONFIG_HOB_H_
#define _MTL_PCH_CONFIG_HOB_H_

extern EFI_GUID  gFspPchConfigGuid;

#pragma pack (push,1)

typedef struct {
  UINT8              SegmentNumber;
  UINT8              BusNumber;
  UINT8              SubordinateBusNumber;
  UINT32             ReservedMmioBase;
  UINT32             ReservedMmioLimit;
  UINT64             ReservedMmio64Base;
  UINT64             ReservedMmio64Limit;
  UINT16             ReservedIoBase;
  UINT16             ReservedIoLimit;
  //FABRIC_SEVERITY    FabricSeverity;
} MTL_PCH_CONFIGURATION;

#pragma pack (pop)
#endif
