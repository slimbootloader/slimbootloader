/** @file
  Header file for TsnLib

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _TSN_LIB_H_
#define _TSN_LIB_H_

#define TSN_CONFIG_REVISION 3

#define MAC_ADDR_SUB_REGION_SIZE_MAX          4096
#ifndef TSN_PORT_MAX
#define TSN_PORT_MAX                          2
#endif
#define MAC_ADDR_SUB_REGION_SIGNATURE_SIZE    256                        // Assume SHA256

#pragma pack(1)

typedef struct {
  UINT32 Seg     : 8;
  UINT32 Bus     : 8;
  UINT32 Dev     : 5;
  UINT32 Func    : 3;
  UINT32 PcieCap : 8;
} SBDF;

//
// This struct will be consumes by BIOS FW
//
typedef struct {
  SBDF                  Bdf;                                             // Associated PCI (Bus:Dev:Fnc), ECAM format
  union {
    UINT64              U64MacAddr;                                      // MAC address associated with this port
    UINT32              U32MacAddr[2];                                   // MAC address associated with this port
    UINT8               U8MacAddr[6];                                    // MAC address associated with this port
  } MacAddr;
} TSN_MAC_ADDR_DATA;

typedef struct {
  UINT32                Version;                                         // Data format version
  UINT32                NumPorts;                                        // Number of valid ia_tsn_mac_addrs in this sturctures
  TSN_MAC_ADDR_DATA     Port[TSN_PORT_MAX];                              // TSN MAC address structures
  UINT8                 Signature[MAC_ADDR_SUB_REGION_SIGNATURE_SIZE];   // Signature of the relevant data in this structure
} MAC_CONFIG_DATA;

typedef union {
  UINT8                 Data[MAC_ADDR_SUB_REGION_SIZE_MAX];              // Ensures the data structure consumes SIZE_MAX
  MAC_CONFIG_DATA       Config;
} TSN_MAC_ADDR_SUB_REGION;

#pragma pack()

#endif
