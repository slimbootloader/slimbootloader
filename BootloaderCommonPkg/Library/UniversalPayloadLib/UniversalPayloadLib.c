/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/UniversalPayloadLib.h>
#include <Library/ElfLib.h>

//#undef   DEBUG_VERBOSE
//#define  DEBUG_VERBOSE   DEBUG_INFO


/**
  Load universal payload image into memory.

  @param[in]   ImageBase    The universal payload image base
  @param[out]  PayloadInfo  Pointer to receive payload related info

  @retval     EFI_SUCCESS      The image was loaded successfully
              EFI_ABORTED      The image loading failed
              EFI_UNSUPPORTED  The relocation format is not supported

**/
EFI_STATUS
EFIAPI
LoadElfPayload (
  IN  VOID                     *ImageBase,
  OUT LOADED_PAYLOAD_INFO      *PayloadInfo
  )
{
  EFI_STATUS                     Status;
  ELF_IMAGE_CONTEXT              Context;
  UNIVERSAL_PAYLOAD_INFO_HEADER  *PldInfo;
  UINT16                         ExtraDataCount;
  UINT32                         Index;
  CHAR8                         *SectionName;
  UINTN                          Offset;
  UINTN                          Size;

  if ((ImageBase == NULL) || (PayloadInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = ParseElfImage (ImageBase, &Context);
  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }

  // Get UNIVERSAL_PAYLOAD_INFO and number of additional PLD sections.
  PldInfo        = NULL;
  ExtraDataCount = 0;
  for (Index = 0; Index < Context.ShNum; Index++) {
    Status = GetElfSectionName (&Context, Index, &SectionName);
    if (EFI_ERROR(Status)) {
      continue;
    }
    if (AsciiStrCmp(SectionName, UNIVERSAL_PAYLOAD_INFO_SEC_NAME) == 0) {
      Status = GetElfSectionPos (&Context, Index, &Offset, &Size);
      if (!EFI_ERROR(Status)) {
        PldInfo = (UNIVERSAL_PAYLOAD_INFO_HEADER *)(Context.FileBase + Offset);
      }
    } else if (AsciiStrnCmp(SectionName, UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX, UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX_LENGTH) == 0) {
      Status = GetElfSectionPos (&Context, Index, &Offset, &Size);
      if (!EFI_ERROR (Status) && (ExtraDataCount < ARRAY_SIZE(PayloadInfo->LoadedImage))) {
        AsciiStrCpyS (PayloadInfo->LoadedImage[ExtraDataCount].Identifier, sizeof(PayloadInfo->LoadedImage[ExtraDataCount].Identifier), SectionName + UNIVERSAL_PAYLOAD_EXTRA_SEC_NAME_PREFIX_LENGTH);
        PayloadInfo->LoadedImage[ExtraDataCount].Base = (UINTN)(Context.FileBase + Offset);
        PayloadInfo->LoadedImage[ExtraDataCount].Size = Size;
        ExtraDataCount++;
      }
    }
  }

  // Always try to run at preferred address
  if (Context.ReloadRequired) {
    Context.ImageAddress = Context.PreferredImageAddress;
  }

  // Load ELF into the required base
  Status = LoadElfImage (&Context);
  if (!EFI_ERROR(Status)) {
    if ((PldInfo != NULL) && (PldInfo->Identifier == UNIVERSAL_PAYLOAD_IDENTIFIER)) {
      CopyMem (&PayloadInfo->Info, PldInfo, sizeof(UNIVERSAL_PAYLOAD_INFO_HEADER));
      PayloadInfo->ImageCount  = ExtraDataCount;
    }
    PayloadInfo->Machine     = (Context.EiClass == ELF_CLASS32) ? IMAGE_FILE_MACHINE_I386 : IMAGE_FILE_MACHINE_X64;
    PayloadInfo->EntryPoint  = Context.EntryPoint;
    PayloadInfo->PayloadSize = Context.FileSize;
    PayloadInfo->PayloadBase = (UINTN)Context.FileBase;
  }

  return Status;
}
