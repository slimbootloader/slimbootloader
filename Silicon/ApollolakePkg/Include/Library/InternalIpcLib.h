/** @file
  Header file for Base IPC library.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __INTERNAL_IPC_LIB_H__
#define __INTERNAL_IPC_LIB_H__

//
// Produced library class
//
#include "PmcIpcLib.h"


/**
  Checks the IPC channel is available for a new request.

  @param[in]  BaseAddress    The base address of the IPC register bank.

  @retval     EFI_SUCCESS    Ready for a new IPC.
  @retval     EFI_NOT_READY  Not ready for a new IPC.

**/
RETURN_STATUS
InternalReadyForNewIpc (
  IN      UINTN   BaseAddress
  );


/**
  Sends an IPC from the x86 CPU to the PMC and waits for the PMC to process the
  specified opcode.

  @param[in]       BaseAddress  The base address of the IPC register bank
  @param[in]       MessageId    The message identifier to send in the IPC packet.
  @param[in]       CommandID    The SubCommand identifier to send in the IPC packet.
  @param[in, out]  Buffer       Pointer to buffer associated with MessageId.  This is an optional
                                patrametert that may be NULL for some MessageId values.
  @param[in, out]  BufferSize   The size, in bytes, of Buffer.  Ignored if Buffer is NULL.

  @retval          IPC_STATUS.Uint32

**/
UINT32
InternalIpcSendCommandEx (
  IN      UINTN   BaseAddress,
  IN      UINT32  MessageId,
  IN      UINT8   CommandId,
  IN OUT  VOID    *Buffer,    OPTIONAL
  IN OUT  UINTN   BufferSize
  );

#endif

