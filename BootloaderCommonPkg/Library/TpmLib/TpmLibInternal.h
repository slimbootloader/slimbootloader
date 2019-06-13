/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TPM_LIB_INTERNAL_H_
#define _TPM_LIB_INTERNAL_H_

typedef struct {
  UINT8  TpmReady;
  UINT32 ActivePcrBanks;
  UINT64 LogAreaStartAddress;
  UINT32 LogAreaMinLength;

} TPM_LIB_PRIVATE_DATA;


/**
  Get a pointer to the TPM_LIB_PRIVATE_DATA instance.

  @retval A pointer to the TPM_LIB_PRIVATE_DATA instance.
**/
TPM_LIB_PRIVATE_DATA *
TpmLibGetPrivateData (
  VOID
  );


/**
  Set Tpm Ready status in TPM_LIB_PRIVATE_DATA instance.

  @param  TpmReady       TPM status for Bootloader
  @retval EFI_SUCCESS    Operation executed successfully.
  @retval EFI_NOT_FOUND  TPM Lib data not found.
**/
RETURN_STATUS
TpmLibSetReadyStatus (
  IN UINT8 TpmReady
  );

/**
  Set TCG Event Log area info in TPM_LIB_PRIVATE_DATA instance.

  @param  Lasa       Log area start address
  @param  Laml       Log area minimum length

  @retval EFI_SUCCESS    Operation executed successfully.
  @retval EFI_NOT_FOUND  TPM Lib data not found.
**/
RETURN_STATUS
TpmLibSetEventLogData (
  IN UINT64 Lasa,
  IN UINT32 Laml
  );


#endif  //_TPM_LIB_INTERNAL_H
