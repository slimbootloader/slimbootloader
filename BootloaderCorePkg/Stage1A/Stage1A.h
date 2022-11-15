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
#include <Library/ContainerLib.h>
#include <Library/StageLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/TopSwapLib.h>
#include <Register/Intel/ArchitecturalMsr.h>
#include <Guid/FspHeaderFile.h>
#include <Guid/FlashMapInfoGuid.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <Guid/PcdDataBaseSignatureGuid.h>
#include <VerInfo.h>

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

#endif
