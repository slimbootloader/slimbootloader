/** @file
  Prototypes for IIO subsystem information API functions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SUBSYSTEM_INFO_LIB_H_
#define _SUBSYSTEM_INFO_LIB_H_

#include <IioSocCfgGnrD.h>

#pragma pack(1)

typedef enum {
  SUBSYS_TYPE_NONE = 0,
  SUBSYS_TYPE_IOAT,
  SUBSYS_TYPE_PXP,
  SUBSYS_TYPE_MAX
} SUBSYS_MAJOR_TYPE;

typedef struct {
  UINT8     Dsa:1;
  UINT8     Iax:1;
  UINT8     Cpm:1;
  UINT8     Hqm:1;
  UINT8     Cpm1:1;
  UINT8     Hqm1:1;
  UINT8     Tip:1;
  UINT8     Cpk:1;
  UINT8     Rsv1:8;
  UINT8     MinorType:4;
  UINT8     Rsv:3;
  UINT8     Present:1;
} ACC_CAP;

typedef struct {
  UINT8    RawData[3];
  UINT8    Npk:1;
  UINT8    Pcie:1;
  UINT8    Rsv:5;
  UINT8    Present:1;
} INFRA_CAP;

typedef struct {
  UINT16    Pcie:1;
  UINT16    Cxl:1;
  UINT16    Rsv:13;
  UINT16    Present:1;
} PXP_CAP;

typedef struct {
  UINT8    RawData[4];
  UINT8    Stack;
} ADDR_MAP;

typedef struct {
  UINT8    RawData[6];
  UINT8    CheckSum;
  UINT8    MajorType:4;
  UINT8    Rsv:3;
  UINT8    Present:1;
} SUBSYS_HEADER;

typedef union {
  UINT32           Data32[2];
  SUBSYS_HEADER    Header;
  ADDR_MAP         AddrMap;
  PXP_CAP          PxpCap;
  INFRA_CAP        InfraCap;
  ACC_CAP          AccCap;
} SUBSYS_INFO_ENTRY;


typedef struct {
  SUBSYS_INFO_ENTRY    IoatInfoEntrys[MAX_IIO_IOAT_PER_SOCKET];
  SUBSYS_INFO_ENTRY    PxpInfoEntrys[MAX_IIO_PCIE_PER_SOCKET];
} SOCKET_SUBSYS_INFO;

typedef struct {
  SOCKET_SUBSYS_INFO    Socket[1];
} SYS_INFO;

#pragma pack()

/**
  Get subsystem information.

  @param[in]  Socket          - Socket index.
  @param[in]  Inst            - Instance in socket.
  @param[in]  SubsysType      - subsystem type.
  @param[out] SubsysInfoEntry - Pointer to the subsystem information.

  @return True if subsystem information was found, false otherwise.

**/
BOOLEAN
GetSubsysInfo (
  UINT8                     Socket,
  UINT8                     Inst,
  UINT8                     SubsysType,
  SUBSYS_INFO_ENTRY         *SubsysInfoEntry
);

/**
  Get Max subsystem number in Socket.

  @param[in]  Socket          - Socket index.
  @param[in]  SubsysType      - subsystem type.

  @return Max subsystem number.

**/
UINT8
GetMaxSubsys (
  UINT8                     Socket,
  UINT8                     SubsysType
);

/**
  Initialize IIO subsystem information.

  @retval True if Hob create successful, false otherwise.

**/
BOOLEAN
IioSubsysInfoInit (
  VOID
  );

#endif // _SUBSYSTEM_INFO_LIB_H_
