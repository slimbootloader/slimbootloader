/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CsmeUpdateDriver.h"

typedef VOID *(EFIAPI *CSME_ALLOCATE_ZERO_POOL_CB)(UINTN AllocationSize);

/**
  Entry point to the csme update driver.

  @param Params            CSME Update Driver Structure pointer.

**/
VOID
EFIAPI
SecStartup (
  VOID *Params
  )
{
  CSME_UPDATE_DRIVER_PARAMS   *DriverParams;
  CSME_UPDATE_DRIVER_OUTPUT   *DriverOutput;
  CSME_ALLOCATE_ZERO_POOL_CB  CsmeAllocateZeroPool;

  if (Params == NULL) {
    return;
  }

  DriverParams = (CSME_UPDATE_DRIVER_PARAMS *)Params;

  //
  // Assign the input structure content to global function structure
  //
  g_FwuCbCtxt.AllocatePool     = DriverParams->CsmeUpdDriverInFunc->AllocatePool;
  g_FwuCbCtxt.AllocateZeroPool = DriverParams->CsmeUpdDriverInFunc->AllocateZeroPool;
  g_FwuCbCtxt.FreePool         = DriverParams->CsmeUpdDriverInFunc->FreePool;
  g_FwuCbCtxt.CopyMem          = DriverParams->CsmeUpdDriverInFunc->CopyMem;
  g_FwuCbCtxt.SetMem           = DriverParams->CsmeUpdDriverInFunc->SetMem;
  g_FwuCbCtxt.CompareMem       = DriverParams->CsmeUpdDriverInFunc->CompareMem;
  g_FwuCbCtxt.Stall            = DriverParams->CsmeUpdDriverInFunc->Stall;
  g_FwuCbCtxt.PciRead          = DriverParams->CsmeUpdDriverInFunc->PciRead;
  g_FwuCbCtxt.HeciReadMessage  = DriverParams->CsmeUpdDriverInFunc->HeciReadMessage;
  g_FwuCbCtxt.HeciSendMessage  = DriverParams->CsmeUpdDriverInFunc->HeciSendMessage;
  g_FwuCbCtxt.HeciReset        = DriverParams->CsmeUpdDriverInFunc->HeciReset;

  CsmeAllocateZeroPool = (CSME_ALLOCATE_ZERO_POOL_CB)((UINTN)DriverParams->CsmeUpdDriverInFunc->AllocateZeroPool);

  //
  // Allocate memory for output structure
  // if we cannot allocate, Make Params pointing to NULL indicating error
  //
  DriverOutput = (CSME_UPDATE_DRIVER_OUTPUT *)CsmeAllocateZeroPool(sizeof(CSME_UPDATE_DRIVER_OUTPUT));
  if (DriverOutput == NULL) {
    Params = NULL;
    return;
  }

  //
  // Populate output structure
  //
  DriverOutput->FwuFullUpdateFromBuffer        = FwuFullUpdateFromBuffer;
  DriverOutput->FwuPartialUpdateFromBuffer     = FwuPartialUpdateFromBuffer;
  DriverOutput->FwuCheckUpdateProgress         = FwuCheckUpdateProgress;
  DriverOutput->FwuEnabledState                = FwuEnabledState;
  DriverOutput->FwuOemId                       = FwuOemId;
  DriverOutput->FwuFwType                      = FwuFwType;
  DriverOutput->FwuPchSku                      = FwuPchSku;
  DriverOutput->FwuPartitionVersionFromFlash   = FwuPartitionVersionFromFlash;
  DriverOutput->FwuPartitionVersionFromBuffer  = FwuPartitionVersionFromBuffer;
  DriverOutput->FwuPartitionVendorIdFromFlash  = FwuPartitionVendorIdFromFlash;
  DriverOutput->FwuSaveRestorePointToBuffer    = FwuSaveRestorePointToBuffer;
  DriverOutput->FwuSetIshConfig                = FwuSetIshConfig;
  DriverOutput->FwuGetIshPdtVersion            = FwuGetIshPdtVersion;

  *(DriverParams->OutputFunc) = DriverOutput;
}
