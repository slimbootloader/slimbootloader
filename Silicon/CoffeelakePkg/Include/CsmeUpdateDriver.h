/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __CSME_UPDATE_DRIVER_H__
#define __CSME_UPDATE_DRIVER_H__

#include "FwuCbContext.h"
#include "ErrorList.h"
#include "MeFwUpdateLib.h"

#define CMD_LINE_STATUS_UPDATE_1        0
#define CMD_LINE_STATUS_UPDATE_2        1
#define CMD_LINE_STATUS_UPDATE_3        2
#define CMD_LINE_STATUS_UPDATE_4        3

typedef
UINT32 
( *FWU_FULL_UPDATE_FROM_BUFFER) (
  IN  UINT8         *Buffer,
  IN  UINT32        BufferLength,
  IN  EFI_GUID         *OemId,
  IN  void(*Func)(UINT32, UINT32)
  );

typedef
UINT32 
( *FWU_PARTIAL_UPDATE_FROM_BUFFER) (
  IN  UINT8         *Buffer,
  IN  UINT32        BufferLength,
  IN  UINT32        PartitionId,
  IN  void(*Func)(UINT32, UINT32)
  );

typedef
UINT32 
( *FWU_CHECK_UPDATE_PROGRESS) (
  OUT BOOLEAN       *InProgress,
  OUT UINT32        *CurrentPercent,
  OUT UINT32        *FwUpdateStatus,
  OUT UINT32        *NeededResetType
  );

typedef
UINT32 
( *FWU_ENABLED_STATE) (
  OUT UINT16       *EnabledState
  );

typedef
UINT32 
( *FWU_OEMID) (
  OUT EFI_GUID       *OemId
  );

typedef
UINT32 
( *FWU_FW_TYPE) (
  OUT UINT32       *FwType
  );

typedef
UINT32 
( *FWU_PCH_SKU) (
  OUT UINT32       *PchSku
  );

typedef
UINT32 
( *FWU_PARTITION_VERSION_FROM_FLASH) (
  IN  UINT32       PartitionId,
  OUT UINT16      *Major,
  OUT UINT16      *Minor,
  OUT UINT16      *Hotfix,
  OUT UINT16      *Build
  );

typedef
UINT32 
( *FWU_PARTITION_VERSION_FROM_BUFFER) (
  IN  UINT8       *Buffer,
  IN  UINT32      BufferLength,
  IN  UINT32      PartitionId,
  OUT UINT16      *Major,
  OUT UINT16      *Minor,
  OUT UINT16      *Hotfix,
  OUT UINT16      *Build
  );

typedef
UINT32 
( *FWU_PARTITION_VENDORID_FROM_FLASH) (
  IN  UINT32      PartitionId,
  OUT UINT32      *VendorId
  );

typedef
UINT32 
( *FWU_SAVERESTORE_POINT_TO_BUFFER) (
  OUT UINT8       **buffer,
  OUT UINT32      *bufferLength
  );

typedef
UINT32 
( *FWU_SET_ISH_CONFIG) (
  IN  UINT8       *Buffer,
  IN  UINT32      BufferLength
  );

typedef
UINT32 
( *FWU_GET_ISH_PDT_VERSION) (
  OUT UINT8       *PdtVersion,
  OUT UINT8       *VdvVersion
  );

typedef struct {
  FWU_FULL_UPDATE_FROM_BUFFER         FwuFullUpdateFromBuffer;
  FWU_PARTIAL_UPDATE_FROM_BUFFER      FwuPartialUpdateFromBuffer;
  FWU_CHECK_UPDATE_PROGRESS           FwuCheckUpdateProgress;
  FWU_ENABLED_STATE                   FwuEnabledState;
  FWU_OEMID                           FwuOemId;
  FWU_FW_TYPE                         FwuFwType;
  FWU_PCH_SKU                         FwuPchSku;
  FWU_PARTITION_VERSION_FROM_FLASH    FwuPartitionVersionFromFlash;
  FWU_PARTITION_VERSION_FROM_BUFFER   FwuPartitionVersionFromBuffer;
  FWU_PARTITION_VENDORID_FROM_FLASH   FwuPartitionVendorIdFromFlash;
  FWU_SAVERESTORE_POINT_TO_BUFFER     FwuSaveRestorePointToBuffer;
  FWU_SET_ISH_CONFIG                  FwuSetIshConfig;
  FWU_GET_ISH_PDT_VERSION             FwuGetIshPdtVersion;
} CSME_UPDATE_DRIVER_OUTPUT;

typedef struct {
  FWU_CB_CONTEXT            *FwuCbContext;
  CSME_UPDATE_DRIVER_OUTPUT **OutputFunc;
} CSME_UPDATE_DRIVER_PARAMS;

#endif
