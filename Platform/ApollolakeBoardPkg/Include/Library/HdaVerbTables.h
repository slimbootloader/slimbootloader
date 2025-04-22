/** @file

  Copyright (c) 2014 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __HDA_VERBTABLES_H__
#define __HDA_VERBTABLES_H__

#pragma pack (push,1)

typedef struct {
  UINT32  VendorDeviceId;       ///< Codec Vendor/Device ID
  UINT8   RevisionId;           ///< Revision ID of the codec. 0xFF matches any revision.
  UINT8   SdiNo;                ///< SDI number, 0xFF matches any SDI.
  UINT16  DataDwords;           ///< Number of data DWORDs following the header.
} HDA_VERB_TABLE_HEADER;

typedef struct {
  HDA_VERB_TABLE_HEADER         VerbTableHeader;
  UINT32                        VerbTableData[];
} HDAUDIO_VERB_TABLE;
#pragma pack (pop)

extern HDAUDIO_VERB_TABLE  HdaVerbTableAlc662;

#endif /* __HDA_VERBTABLES_H__ */
