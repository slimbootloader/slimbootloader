/** @file
  Header file for TSN Sub Region definition

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _TSN_SUB_REGION_H_
#define _TSN_SUB_REGION_H_

#define TSN_PORT_MAX       2
#define MAC_ADDR_SUB_REGION_SIGNATURE_SIZE    256                        // Assume SHA256
#define MAC_ADDR_SUB_REGION_SIZE_MAX          4096

#pragma pack(1)

typedef struct {
  UINT32 RsvdBits1     : 12;                   ///< [11:0]  Reserved bits
  UINT32 Func          : 3;                    ///< [14:12] Function Id
  UINT32 Dev           : 5;                    ///< [19:15] Device Id
  UINT32 Bus           : 8;                    ///< [27:20] Bud Id
  UINT32 RsvdBits0     : 4;                    ///< [31:28] Reserved bits
} BDF;

//
// This struct will be consumes by BIOS while TSN MAC
//
typedef struct {
  BDF                   Bdf;                                             // Associated PCI (Bus:Dev:Fnc), ECAM format
  union {
    UINT64              U64MacAddr;                                      // MAC address associated with this port
    UINT32              U32MacAddr[2];                                   // MAC address associated with this port
    UINT8               U8MacAddr[6];                                    // MAC address associated with this port
  } MacAddr;
} TSN_MAC_SUB_ADDR;

typedef struct {
  UINT32                Version;                                         // Data format version
  UINT32                NumPorts;                                        // Number of valid ia_tsn_mac_addrs in this sturctures
  TSN_MAC_SUB_ADDR      Port[TSN_PORT_MAX];                              // TSN MAC address structures
  UINT8                 Signature[MAC_ADDR_SUB_REGION_SIGNATURE_SIZE];   // Signature of the relevant data in this structure
} MAC_CONFIG_DATA;

typedef union {
  UINT8                 Data[MAC_ADDR_SUB_REGION_SIZE_MAX];              // Ensures the data structure consumes SIZE_MAX
  MAC_CONFIG_DATA       MacConfigData;
} TSN_MAC_ADDR_SUB_REGION;

#pragma pack()

#endif
