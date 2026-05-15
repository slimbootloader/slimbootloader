/**@file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __UCSI__H__
#define __UCSI__H__


#define UCXX_DISABLE                            0x00
#define UCSI_ENABLE                             0x01
#define UCSI2x_ENABLE                           0x02
#define UCMX_ENABLE                             0x03

#pragma pack (1)
//
// USB Type-C Connector System Software Interface (UCSI) Data Structure
//
typedef struct  {
  UINT16   Version;     // PPM -> OPM, Version JJ.M.N (JJ - major version, M - minor version, N - sub-minor version). Version 2.1.3 is represented with value 0x0213 (16 bits)
  UINT16   Reserved;    // Reserved (16 bits)
  UINT8    CCI[4];      // PPM->OPM CCI indicator (32 bits)
  UINT8    Control[8];  // OPM->PPM Control message (64 bits)
  UINT8    MsgIn[16];  // PPM->OPM Message In, (128 bits)
  UINT8    MsgOut[16]; // OPM->PPM Message Out, (128 bits)
} UCSI_OPM_BUF;
#pragma pack ()

#endif