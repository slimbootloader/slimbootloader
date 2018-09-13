/** @file
  Routines supporting partition discovery and
  logical device reading

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials are licensed and made available
under the terms and conditions of the BSD License which accompanies this
distribution. The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PARTITION_LIB_H_
#define _PARTITION_LIB_H_

#include <BlockDevice.h>

#define  PART_INFO_SIGNATURE      SIGNATURE_32 ('P', 'A', 'R', 'T')

#define  PART_MAX_BLOCK_SIZE      8192
#define  PART_MAX_BLOCK_DEVICE    64

#define  GPT_MAX_NUM_PARTITIONS   128

enum {
  EnumPartTypeUnknown = 0,
  EnumPartTypeMbr,
  EnumPartTypeGpt,
  EnumPartTypeMax
};

//
// The block device
//
typedef struct {
  UINT64                        StartBlock;
  UINT64                        LastBlock;
} LOGICAL_BLOCK_DEVICE;

typedef struct {
  UINT32                        Signature;
  BOOLEAN                       PartitionChecked;
  UINT32                        PartitionType;
  UINT32                        HarewareDevice;
  UINT32                        BlockDeviceCount;
  UINT64                        BlockData[PART_MAX_BLOCK_SIZE / 8];
  LOGICAL_BLOCK_DEVICE          BlockDevice[PART_MAX_BLOCK_DEVICE];
  DEVICE_BLOCK_INFO             BlockInfo;
} PART_BLOCK_DEVICE;

/**
Get logical partition information

This function will fill in logical partition information.

  @param[in]     SwPart           Software partition index.
  @param[in]     PartHandle       Partition handle.
  @param[in,out] LogicBlkDev      Pointer of the logic block device LOGICAL_BLOCK_DEVICE
                                  structure to receive block start and end information.
  @retval EFI_SUCCESS             The logic partition information was filled successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameters.

**/
EFI_STATUS
EFIAPI
GetLogicalPartitionInfo (
  IN     UINT32                 SwPart,
  IN     EFI_HANDLE             PartHandle,
  IN OUT LOGICAL_BLOCK_DEVICE  *LogicBlkDev
  );

/**
Find partitions from OS boot medium

This function will check hardware partition for MBR, GPT or NONE parition.

  @param[in]   HwDevice      The hardware device index.
  @param[out]  PartHandle    The pointer to return parition handle

  @retval EFI_SUCCESS        The partition was found successfully.
  @retval EFI_NOT_FOUND      No partition was found.
  @retval EFI_DEVICE_ERROR   Error occured for device access.
  @retval EFI_INVALID_PARAMETER   Invalid parameters.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.

**/
EFI_STATUS
EFIAPI
FindPartitions (
  IN   UINT32               HwDevice,
  OUT  EFI_HANDLE          *PartHandle
  );

#endif
