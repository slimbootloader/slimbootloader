/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _STAGE_1A_H_
#define _STAGE_1A_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ExtraBaseLib.h>
#include <Library/PcdLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/LoaderLib.h>
#include <Library/SecureBootLib.h>
#include <Library/CryptoLib.h>
#include <Library/DecompressLib.h>
#include <Library/TimeStampLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/CpuExceptionLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Library/DebugAgentLib.h>
#include <Guid/FspHeaderFile.h>
#include <Guid/FlashMapInfoGuid.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <Guid/PcdDataBaseSignatureGuid.h>
#include <VerInfo.h>

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
  Continue Stage 1A execution.

  This function will continue Stage1A execution for a new location.

  @param[in]  Params          Pointer to STAGE1A_ASM_PARAM in main memory.

**/
VOID
EFIAPI
ContinueFunc (
  IN VOID  *Params
  );

/**
  Execute necessary actions after a stage is relocated.

**/
VOID
PostStageRelocation (
  VOID
  );

#endif
