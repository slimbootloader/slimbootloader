/** @file
  DTPR ACPI Table Header File for SBL
  Based on DtprDxe reference implementation

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _DTPR_H_
#define _DTPR_H_

#include <IndustryStandard/Acpi.h>
#include <Uefi/UefiBaseType.h>

#define DTPR_ACPI_TABLE_SIGNATURE  SIGNATURE_32('D', 'T', 'P', 'R')

#define MAX_TPR_INSTANCES      8
#define MAX_TPR_PER_INSTANCE   2
#define MAX_DRAIN_CONTROLS     64

#define MAX_TPR_COUNT             2
#define MAX_INS_COUNT             2
#define MAX_SRL_REG_COUNT         10

#pragma pack(push, 1)

typedef struct
{
  UINT64                         TprBase;
} TPR_BASE_LIMIT_STRUCT;

typedef struct
{
  UINT32                         Flags;
  UINT32                         TprCnt;
  TPR_BASE_LIMIT_STRUCT          TprArr[MAX_TPR_COUNT];
} TPR_INS_STRUCT;

typedef struct
{
  EFI_ACPI_DESCRIPTION_HEADER    Header;
  UINT32                         Flags;
  UINT32                         InsCnt;
  TPR_INS_STRUCT                 TprIns[MAX_INS_COUNT];
  UINT32                         SrlCnt;
  UINT64                         SrlRegArr[MAX_SRL_REG_COUNT];
} DTPR_TABLE_STRUCT;

//
// TPR INFO HOB structures
//
typedef struct {
  UINT8                TprCount;
  UINT64               Tpr0BaseOffset;
  UINT64               Tpr0LimitOffset;
  UINT64               Tpr1BaseOffset;
  UINT64               Tpr1LimitOffset;
} TPR_INSTANCE_INFO;

typedef struct {
  UINT64               GtDrainControl;
  UINT64               MediaDrainControl;
  UINT64               DisplayDrainControl;
  UINT64               IaxDrainControl;
  UINT64               IocDrainControl;
  UINT64               IvtuDrainControl;
  UINT64               SvtuDrainControl;
  UINT64               IpuDrainControl;
  UINT64               NpuDrainControl;
  UINT64               PunitDrainControl;
} DRAIN_CTL_REG;

///
/// TPR Info Hob (from FSP)
/// Note: When retrieved via GET_GUID_HOB_DATA, the EFI_HOB_GUID_TYPE header
/// is automatically skipped, so access fields directly starting from Tpr0Base
///
typedef struct {
  UINT64               Tpr0Base;
  UINT64               Tpr0Limit;
  UINT64               Tpr1Base;
  UINT64               Tpr1Limit;
  UINT8                InstanceCount;
  TPR_INSTANCE_INFO    InstanceArray[2];
  UINT8                DrainControlCount;
  DRAIN_CTL_REG        DrainControlArray;
} TPR_INFO_HOB;

#pragma pack(pop)

#endif // _DTPR_H_
