/** @file

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

///
/// Use this macro to create HDAUDIO_VERB_TABLE and populate size automatically
///
#define HDAUDIO_VERB_TABLE_INIT(Vid,Did,Rid,Sdi,...) \
{ \
  { Vid, Did, Rid, Sdi, (sizeof((UINT32[]){__VA_ARGS__})/sizeof(UINT32)) }, \
  { __VA_ARGS__ } \
}

/**
  Azalia verb table header
  Every verb table should contain this defined header and followed by azalia verb commands.
**/
typedef struct {
  UINT16  VendorId;             ///< Codec Vendor ID
  UINT16  DeviceId;             ///< Codec Device ID
  UINT8   RevisionId;           ///< Revision ID of the codec. 0xFF matches any revision.
  UINT8   SdiNum;               ///< SDI number, 0xFF matches any SDI.
  UINT16  DataDwords;           ///< Number of data DWORDs following the header.
} PCH_HDA_VERB_TABLE_HEADER;

typedef struct  {
  PCH_HDA_VERB_TABLE_HEADER  Header;
  UINT32 Data[];
} HDAUDIO_VERB_TABLE;

HDAUDIO_VERB_TABLE HdaVerbTableAlc897 = HDAUDIO_VERB_TABLE_INIT(
  0x10EC, 0x0897,
  0xFF, 0xFF,
  //HDA Codec Subsystem ID  : 0x10EC1268
  0x00172068,
  0x00172112,
  0x001722EC,
  0x00172310
  //Others, pleaes get from Vender.

); // HdaVerbTableAlc897
