/** @file
  This file defines the hob structure for the SMM related info.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SMM_INFORMATION_GUID_H__
#define __SMM_INFORMATION_GUID_H__

///
/// SMM Information GUID
///
extern EFI_GUID gSmmInformationGuid;

//
// Set this flag if 4KB SMRAM at SmmBase used for communication
// between bootloader and UEFI payload.
//
#define   SMM_FLAGS_4KB_COMMUNICATION  BIT0

typedef enum {
  REG_TYPE_MEM,
  REG_TYPE_IO,
  REG_TYPE_PCICFG,
  REG_TYPE_MMIO,
} REG_TYPE;

typedef enum {
  WIDE8   = 1,
  WIDE16  = 2,
  WIDE32  = 4
} REG_WIDTH;

#pragma pack(1)

///
/// SMI control register
///
typedef struct {
  UINT8   RegType;
  UINT8   RegWidth;
  /// The bit index for APMC Enable (APMC_EN).
  UINT8   SmiApmPos;
  /// The bit index for Global SMI Enable (GBL_SMI_EN)
  UINT8   SmiGblPos;
  /// The bit index for End of SMI (EOS)
  UINT8   SmiEosPos;
  UINT8   Rsvd[3];
  /// Address for SMM control and enable register
  UINT32  Address;
} SMI_CTRL_REG;

///
/// SMI status register
///
typedef struct {
  UINT8   RegType;
  UINT8   RegWidth;
  /// The bit index for APM Status (APM_STS).
  UINT8   SmiApmPos;
  UINT8   Rsvd[5];
  /// Address for SMM status register
  UINT32  Address;
} SMI_STS_REG;

///
/// SMI lock register
///
typedef struct {
  UINT8   RegType;
  UINT8   RegWidth;
  /// The bit index for SMI Lock (SMI_LOCK)
  UINT8   SmiLockPos;
  UINT8   Rsvd;
  /// Register address for SMM SMI lock
  UINT32  Address;
} SMI_LOCK_REG;

typedef struct {
  UINT8                       Revision;
  UINT8                       Flags;
  UINT8                       Reserved[2];
  UINT32                      SmmBase;
  UINT32                      SmmSize;
  SMI_CTRL_REG                SmiCtrlReg;
  SMI_STS_REG                 SmiStsReg;
  SMI_LOCK_REG                SmiLockReg;
} LDR_SMM_INFO;

#pragma pack()


#endif
