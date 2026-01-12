/**@file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __UCSI__H__
#define __UCSI__H__

// PCH USB2 Port No may vary from 1 to 15.
#define TCSS_IS_VALIDATE_PCH_USB_PORT_NUMBER(PORT)\
  (((PORT > 0) && (PORT < 0x10)) ? TRUE : FALSE)

#define TCSS_PLATFORM_POR                       0x00
#define TCSS_FORCE_ENABLE                       0x01
#define TCSS_FORCE_DISABLE                      0x02

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
  UINT16   Reserved;    // Reversed (16 bits)
  UINT8    CCI[4];      // PPM->OPM CCI indicator (32 bits)
  UINT8    Control[8];  // OPM->PPM Control message (64 bits)
  UINT8    MsgIn[16];  // PPM->OPM Message In, (2048 bits)
  UINT8    MsgOut[16]; // OPM->PPM Message Out, (2048 bits)
} UCSI_OPM_BUF;
#pragma pack ()

#endif