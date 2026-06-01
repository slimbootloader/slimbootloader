/** @file
  UI Descriptor binary format structures.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _UI_DESCRIPTOR_H_
#define _UI_DESCRIPTOR_H_

#include <PiPei.h>

#define UI_DESC_SIGNATURE   SIGNATURE_32('U','I','D','B')
#define UI_DESC_VERSION     2

#pragma pack(1)

typedef struct {
  UINT32  Signature;         // 'UIDB'
  UINT8   Version;
  UINT8   Reserved;
  UINT16  PageCount;
  UINT16  FieldCount;
  UINT16  OptionCount;
  UINT16  ConditionCount;
  UINT16  Reserved2;
  UINT32  StringTableOffset;
  UINT32  TotalSize;
} UI_DESC_HEADER;

typedef struct {
  UINT16  PageId;
  UINT16  ParentPageId;
  UINT32  NameStrOffset;
  UINT16  FieldStartIdx;
  UINT16  FieldCount;
} UI_PAGE_ENTRY;

typedef struct {
  UINT16  TagId;
  UINT16  BitOffset;
  UINT16  BitLength;
  UINT8   FieldType;
  UINT8   ConditionIdx;
  UINT32  NameStrOffset;
  UINT32  HelpStrOffset;
  UINT32  PathStrOffset;
  UINT16  OptionStartIdx;
  UINT16  OptionCount;
} UI_FIELD_ENTRY;

typedef struct {
  UINT32  Value;
  UINT32  LabelStrOffset;
} UI_OPTION_ENTRY;

typedef struct {
  UINT16  TagId;
  UINT16  BitOffset;
  UINT16  BitLength;
  UINT8   Operator;
  UINT8   Reserved;
  UINT32  CompareValue;
} UI_CONDITION_ENTRY;

#pragma pack()

//
// Field types
//
#define UI_FIELD_TYPE_COMBO     0
#define UI_FIELD_TYPE_EDITNUM   1
#define UI_FIELD_TYPE_EDITTEXT  2
#define UI_FIELD_TYPE_RESERVED  3
#define UI_FIELD_TYPE_TABLE     4

//
// Condition operators
//
#define UI_COND_OP_EQUAL       0
#define UI_COND_OP_NOT_EQUAL   1
#define UI_COND_OP_GREATER     2
#define UI_COND_OP_LESS        3

#endif // _UI_DESCRIPTOR_H_
