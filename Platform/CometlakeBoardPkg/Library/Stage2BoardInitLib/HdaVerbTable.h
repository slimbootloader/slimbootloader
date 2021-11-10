/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __HDA_VERB_TABLE_H__
#define __HDA_VERB_TABLE_H__

#include <Library/BaseLib.h>

#define NHLT_ACPI_TABLE_SIGNATURE  SIGNATURE_32 ('N', 'H', 'L', 'T')

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

#ifdef _MSC_VER
//
// Disable "zero-sized array in struct/union" extension warning.
// Used for neater verb table definitions.
//
#pragma warning (push)
#pragma warning (disable: 4200)
#endif
typedef struct  {
  PCH_HDA_VERB_TABLE_HEADER  Header;
  UINT32 Data[];
} HDAUDIO_VERB_TABLE;
#ifdef _MSC_VER
#pragma warning (pop)
#endif


CONST HDAUDIO_VERB_TABLE HdaVerbTableDisplayAudio = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: CML Display Audio Codec
  //  Revision ID = 0xFF
  //  Codec Vendor: 0x8086280B
  //
  0x8086, 0x280B,
  0xFF, 0xFF,
  //
  // Display Audio Verb Table
  //
  // For GEN9, the Vendor Node ID is 08h
  // Port to be exposed to the inbox driver in the vanilla mode: PORT C - BIT[7:6] = 01b
  0x00878140,
  // Pin Widget 5 - PORT B - Configuration Default: 0x18560010
  0x00571C10,
  0x00571D00,
  0x00571E56,
  0x00571F18,
  // Pin Widget 6 - PORT C - Configuration Default: 0x18560020
  0x00671C20,
  0x00671D00,
  0x00671E56,
  0x00671F18,
  // Pin Widget 7 - PORT D - Configuration Default: 0x18560030
  0x00771C30,
  0x00771D00,
  0x00771E56,
  0x00771F18,
  // Disable the third converter and third Pin (NID 08h)
  0x00878140
);

//
// CML H/S/OC Audio Codec
//
CONST HDAUDIO_VERB_TABLE CmlHdaVerbTableAlc711 = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC711) CML H/S/OC RVP
  //  Revision ID = 0xff
  //  Codec Verb Table for CML PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0711
  //
  0x10EC, 0x0711,
  0xFF, 0xFF,


  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC11DE
  0x001720DE,
  0x00172111,
  0x001722EC,
  0x00172310,


  //===== Pin Widget Verb-table =====
  //Widget node 0x01 :
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  //Pin widget 0x12 - DMIC
  0x01271C00,
  0x01271D00,
  0x01271E00,
  0x01271F40,
  //Pin widget 0x13 - DMIC
  0x01371CF0,
  0x01371D11,
  0x01371E11,
  0x01371F41,
  //Pin widget 0x14 - SPEAKER-OUT (Port-D)
  0x01471CF0,
  0x01471D11,
  0x01471E11,
  0x01471F41,
  //Pin widget 0x17 - I2S-OUT
  0x01771CF0,
  0x01771D11,
  0x01771E11,
  0x01771F41,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871CF0,
  0x01871D11,
  0x01871E11,
  0x01871F41,
  //Pin widget 0x19 - I2S-IN
  0x01971C20,
  0x01971D10,
  0x01971EA1,
  0x01971F02,
  //Pin widget 0x1A - LINE1 (Port-C)
  0x01A71CF0,
  0x01A71D11,
  0x01A71E11,
  0x01A71F41,
  //Pin widget 0x1D - PC-BEEP
  0x01D71C2D,
  0x01D71D06,
  0x01D71E45,
  0x01D71F40,
  //Pin widget 0x1E - S/PDIF-OUT
  0x01E71CF0,
  0x01E71D11,
  0x01E71E11,
  0x01E71F41,
  //Pin widget 0x1F - S/PDIF-IN
  0x01F71CF0,
  0x01F71D11,
  0x01F71E11,
  0x01F71F41,
  //Pin widget 0x21 - HP-OUT (Port-A)
  0x02171C10,
  0x02171D10,
  0x02171E21,
  0x02171F02,
  //Pin widget 0x20
  0x02050091,
  0x02040000,
  0x0205004A,
  0x0204283B
);

#endif /* __HDA_VERB_TABLE_H__ */
