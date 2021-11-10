/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOOT_GUARD_LIB_20_H_
#define _BOOT_GUARD_LIB_20_H_

#include <IndustryStandard/Tpm20.h>

//
// Boot Policy configuration for Boot Guard
//
typedef struct {
  UINT16 Pbe: 1;
  UINT16 Bbi: 1;
  UINT16 Vb: 1;
  UINT16 Mb: 1;
  UINT16 KmId: 4;
  UINT16 Dcd: 1;
  UINT16 Sb_S3_Opt: 1;
  UINT16 Res: 6;
} BOOT_GUARD_BOOT_POLICY;


//
// Boot Guard info populated by TXE
//
typedef struct {
  BOOT_GUARD_BOOT_POLICY     Bpm;
  UINT8                      TxeHash[SHA256_DIGEST_SIZE];
  UINT8                      IbblHash[SHA256_DIGEST_SIZE];
  union {
    UINT32 HashPtr;
    UINT8  Hash[SHA256_DIGEST_SIZE];
  }IbbHash;
} BOOT_GUARD_INFO;

/**
  It locates the FIT table and retrieves the BtGuard data such as BPM, Txe hash, Ibbl hash
  and location of IBB hash.

  @param BtGuardInfo    Pointer to the location where data needs to be saved.

**/
VOID
FetchPreRBPData(
  OUT BOOT_GUARD_INFO *BtGuardInfo
  );

/**
  TXE populates the IBB hash after Ring Buffer Protocol is finished.

  @param BtGuardInfo    Pointer to the location where data needs to be saved.

**/
VOID
FetchPostRBPData(
  OUT BOOT_GUARD_INFO *BtGuardInfo
  );

/**
  It extends the TXE, Ibbl and Ibb hash into TPM.

  @param BtGuardInfo    Pointer to the location where BtGuard hashes are saved.

**/
VOID
ExtendBtGuardEvents(
  IN BOOT_GUARD_INFO *BtGuardInfo
  );

#endif
