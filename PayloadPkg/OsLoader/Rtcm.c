/** @file

  Copyright (c) 2019 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include "OsLoader.h"

#define SIGNATURE_IPFW    SIGNATURE_32 ('I', 'P', 'F', 'W')
#define SIGNATURE_RTCM    SIGNATURE_32 ('T', 'C', 'C', 'R')

typedef VOID (*RTCM_ENTRY)(VOID *Params);

/**
  Load and Run RTCM.

  This function will load and run RTCM.

  @retval  EFI_NOT_FOUND    RTCM binary file was not found.
  @retval  EFI_LOAD_ERROR   RTCM binary file loading failed
  @retval  EFI_SUCCESS      RTCM load and run successfully.
**/
EFI_STATUS
LoadAndRunRtcm(
  IN VOID
)
{
  EFI_STATUS   Status;
  UINT32       Length;
  VOID         *Buffer;
  VOID         *RtcmRunTimeBuffer;
  RTCM_ENTRY   EntryPoint;
  VOID         *RtcmEntry;

  Buffer = NULL;
  Length = 0;
  RtcmRunTimeBuffer = 0;

  Status = LoadComponent(SIGNATURE_IPFW, SIGNATURE_RTCM, &Buffer, &Length);
  if (EFI_ERROR(Status) || (Buffer == NULL) || (Length == 0))
  {
    return EFI_NOT_FOUND;
  }

  if (!IsTePe32Image(Buffer, NULL))
  {
    DEBUG((DEBUG_INFO, "RTCM Image file is not a Pe32Imag \n"));
    return EFI_LOAD_ERROR;
  }

  RtcmRunTimeBuffer = AllocateRuntimePages(EFI_SIZE_TO_PAGES(Length));
  if (!RtcmRunTimeBuffer)
  {
    DEBUG((DEBUG_INFO, "Runtime memory allocation for RTCM failed \n"));
    return EFI_LOAD_ERROR;
  }

  CopyMem(RtcmRunTimeBuffer, Buffer, (UINT32)(UINTN)Length);

  Status = PeCoffRelocateImage((UINT32)(UINTN)RtcmRunTimeBuffer);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_INFO, "RTCM image relocation failed with Status: %r \n", Status));
    return EFI_LOAD_ERROR;
  }

  Status = PeCoffLoaderGetEntryPoint(RtcmRunTimeBuffer, (VOID **)&RtcmEntry);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_INFO, "Get RTCM image EntryPoint failed with Status: %r \n", Status));
    return EFI_LOAD_ERROR;
  }

  EntryPoint = (RTCM_ENTRY)(UINTN)RtcmEntry;
  EntryPoint((VOID *)(UINTN)PcdGet32(PcdPayloadHobList));

  return EFI_SUCCESS;
}
