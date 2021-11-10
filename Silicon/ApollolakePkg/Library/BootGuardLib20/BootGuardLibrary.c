/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BootGuardLib20.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/TpmLib.h>


#define  BASE_4GB  0x0000000100000000ULL
//
// FIT definition
//
#define FIT_TABLE_SIGNATURE                  SIGNATURE_64('_','F','I','T','_',' ',' ',' ')

#define FIT_TABLE_TYPE_HEADER                0x0
#define FIT_TABLE_TYPE_TXE_SECURE_BOOT       0x10
#define FIT_ENTRY_SUB_TYPE_TXE_HASH          0x2
#define FIT_ENTRY_SUB_TYPE_BOOT_POLICY       0x3
#define FIT_ENTRY_SUB_TYPE_IBBL_HASH         0x7
#define FIT_ENTRY_SUB_TYPE_IBB_HASH          0x8

typedef struct {
  UINT64 Address;
  UINT8  Size[3];
  UINT8  Reserved;
  UINT16 Version;
  UINT8  Type : 7;
  UINT8  Cv   : 1;
  UINT8  Chksum;
} FIRMWARE_INTERFACE_TABLE_ENTRY;



/**
  Find FIT Entry address data by type.

  @param[in] Type  FIT Entry type

  @return  FIT entry address
**/
VOID *
FindFitEntryData (
  IN UINT8 Type,
  IN UINT8 SubType
  )
{
  FIRMWARE_INTERFACE_TABLE_ENTRY *FitEntry;
  UINT32                         EntryNum;
  UINT64                         FitTableOffset;
  UINT32                         Index;

  FitTableOffset = *(UINT64 *)(UINTN)(BASE_4GB - 0x40);
  FitEntry = (FIRMWARE_INTERFACE_TABLE_ENTRY *)(UINTN)FitTableOffset;
  if (FitEntry[0].Address != FIT_TABLE_SIGNATURE) {
    return NULL;
  }
  if (FitEntry[0].Type != FIT_TABLE_TYPE_HEADER) {
    return NULL;
  }
  EntryNum = *(UINT32 *)(&FitEntry[0].Size[0]) & 0xFFFFFF;
  for (Index = 0; Index < EntryNum; Index++) {
    if ((FitEntry[Index].Type == Type) && (FitEntry[Index].Reserved == SubType)) {
      return (VOID *)(UINTN)FitEntry[Index].Address;
    }
  }

  return NULL;
}


/**
  It locates the FIT table and retrieves the BtGuard data such as BPM, Txe hash, Ibbl hash
  and location of IBB hash.

  @param BtGuardInfo    Pointer to the location where data needs to be saved.

**/
VOID
FetchPreRBPData(
  OUT BOOT_GUARD_INFO *BtGuardInfo
  )
{
  VOID *FitData;

  FitData = FindFitEntryData (  FIT_TABLE_TYPE_TXE_SECURE_BOOT,   FIT_ENTRY_SUB_TYPE_BOOT_POLICY  );
  if (FitData != NULL) {
    CopyMem ((VOID*)&(BtGuardInfo->Bpm), FitData, sizeof (BtGuardInfo->Bpm));
    DEBUG ((DEBUG_INFO, "BtGuard: VB : %d, MB : %d \n", BtGuardInfo->Bpm.Vb, BtGuardInfo->Bpm.Mb));
  }

  if (BtGuardInfo->Bpm.Mb) {
    FitData = FindFitEntryData (  FIT_TABLE_TYPE_TXE_SECURE_BOOT,   FIT_ENTRY_SUB_TYPE_TXE_HASH  );
    if (FitData != NULL) {
      CopyMem ((VOID*)(BtGuardInfo->TxeHash), FitData, sizeof (BtGuardInfo->TxeHash));
      DEBUG ((DEBUG_INFO, "TXE Hash: 0x%08x\n", BtGuardInfo->TxeHash));
      DumpHex (2, 0, 32, (VOID *)BtGuardInfo->TxeHash);
    }

    FitData = FindFitEntryData (  FIT_TABLE_TYPE_TXE_SECURE_BOOT,   FIT_ENTRY_SUB_TYPE_IBBL_HASH  );
    if (FitData != NULL) {
      CopyMem ((VOID*)(BtGuardInfo->IbblHash), FitData, sizeof (BtGuardInfo->IbblHash));
      DEBUG ((DEBUG_INFO, "IBBL Hash: 0x%08x\n", BtGuardInfo->IbblHash));
      DumpHex (2, 0, 32, (VOID *)BtGuardInfo->IbblHash);
    }

    // IBB hash will be populated by TXE after RBP. Just cache the IBB hash location here.
    FitData = FindFitEntryData (  FIT_TABLE_TYPE_TXE_SECURE_BOOT,   FIT_ENTRY_SUB_TYPE_IBB_HASH  );
    if (FitData != NULL) {
      BtGuardInfo->IbbHash.HashPtr = (UINT32)(UINTN)FitData;
      DEBUG ((DEBUG_INFO, "IBB Hash: 0x%08x\n", BtGuardInfo->IbbHash.HashPtr));
    }
  }

}


/**
  TXE populates the IBB hash after Ring Buffer Protocol is finished.

  @param BtGuardInfo    Pointer to the location where data needs to be saved.

**/
VOID
FetchPostRBPData(
  OUT BOOT_GUARD_INFO *BtGuardInfo
  )
{
  UINT8 IbbHash[SHA256_DIGEST_SIZE];

  if ((BtGuardInfo->Bpm.Mb) && (BtGuardInfo->IbbHash.HashPtr != 0)) {
    CopyMem (IbbHash, (VOID*)(UINTN)(BtGuardInfo->IbbHash.HashPtr), sizeof (IbbHash));
    CopyMem (BtGuardInfo->IbbHash.Hash, IbbHash, sizeof (BtGuardInfo->IbbHash));

    DEBUG ((DEBUG_INFO, "IBB Hash: 0x%08x\n", BtGuardInfo->IbbHash.Hash));
    DumpHex (2, 0, 32, (VOID *)BtGuardInfo->IbbHash.Hash);
  }
}

/**
  It extends the TXE, Ibbl and Ibb hash into TPM.

  @param BtGuardInfo    Pointer to the location where BtGuard hashes are saved.

**/
VOID
ExtendBtGuardEvents(
  IN BOOT_GUARD_INFO *BtGuardInfo
  )
{
  if (BtGuardInfo != NULL) {
    TpmExtendPcrAndLogEvent ( 0, TPM_ALG_SHA256, BtGuardInfo->TxeHash,
      EV_POST_CODE, POST_CODE_STR_LEN, (UINT8 *)EV_POSTCODE_INFO_POST_CODE);

    TpmExtendPcrAndLogEvent ( 0, TPM_ALG_SHA256, BtGuardInfo->IbblHash,
      EV_POST_CODE, POST_CODE_STR_LEN, (UINT8 *)EV_POSTCODE_INFO_POST_CODE);

    TpmExtendPcrAndLogEvent ( 0, TPM_ALG_SHA256, BtGuardInfo->IbbHash.Hash,
      EV_POST_CODE, POST_CODE_STR_LEN, (UINT8 *)EV_POSTCODE_INFO_POST_CODE);
  }

}
