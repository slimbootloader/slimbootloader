/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
#include <Library/CpuExceptionLib.h>
#include <Library/DebugDataLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Library/LocalApicLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/TpmLib.h>
#include <Guid/PcdDataBaseSignatureGuid.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <VerInfo.h>
#include <HashStore.h>


/**
  Continue Stage1B execution.

  This function will continue Stage1B execution with a new memory based stack.

  @param  Context1        Context1 parameter passed into SwitchStack().
  @param  Context2        Context2 parameter passed into SwitchStack().

**/
VOID
ContinueFunc (
  IN      VOID                      *Context1,  OPTIONAL
  IN      VOID                      *Context2   OPTIONAL
  );

/**
  Load IDT table for current processor.

  It function initializes the exception handlers in IDT table and
  loads it into current processor.  It also try to store a specific DWORD
  data just before actual IDT base. It is used to set/get bootloader
  global data structure pointer.

  @param  IdtTable  Contain the IDT table pointer.
                    It must point to a STAGE_IDT_TABLE buffer preallocated.

  @param  Data      It contains a DWORD data that will be stored just before the actual IDT table.
                    It can be used to set/get global data using IDT pointer.

**/
VOID
LoadIdt (
  IN  STAGE_IDT_TABLE   *IdtTable,
  IN  UINT32             Data
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
