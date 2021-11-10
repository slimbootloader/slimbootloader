/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _STAGE_1B_H_
#define _STAGE_1B_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DecompressLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BoardInitLib.h>
#include <Library/LoaderLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/CryptoLib.h>
#include <Library/FspSupportLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/PagingLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/TimeStampLib.h>
#include <Library/FspApiLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/SecureBootLib.h>
#include <Library/CryptoLib.h>
#include <Library/CpuExceptionLib.h>
#include <Library/DebugDataLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Library/LocalApicLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/TpmLib.h>
#include <Library/StageLib.h>
#include <Library/ContainerLib.h>
#include <Guid/PcdDataBaseSignatureGuid.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <VerInfo.h>

/**
  Continue Stage1B execution.

  This function will continue Stage1B execution with a new memory based stack.

  @param  Context1        Context1 parameter passed into SwitchStack().
  @param  Context2        Context2 parameter passed into SwitchStack().

**/
VOID
EFIAPI
ContinueFunc (
  IN      VOID                      *Context1,  OPTIONAL
  IN      VOID                      *Context2   OPTIONAL
  );

/**
  Remap the current stage image.

  It will create page tables to remap current Stage1B from SRAM to CAR.
  It is necessary in case that SRAM is destroied during following stage loading.
**/
VOID
RemapStage (
  VOID
  );

#endif
