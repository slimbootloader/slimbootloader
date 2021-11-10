/** @file

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2.h"

UINT8   mFspPhaseMask;

// Create a platform service
const PLATFORM_SERVICE   mPlatformService = {
  .Header.Signature = PLATFORM_SERVICE_SIGNATURE,
  .Header.Version   = PLATFORM_SERVICE_VERSION,
  .NotifyPhase      = BoardNotifyPhase
};


/**
  Platform notify service.

  @param[in] Phase          Boot phase to notify.

**/
VOID
EFIAPI
BoardNotifyPhase (
  IN BOARD_INIT_PHASE   Phase
  )
{
  FSP_INIT_PHASE  FspPhase;
  UINT8           FspPhaseMask;
  EFI_STATUS      Status;

  // This is board notification from payload
  FspPhaseMask = 0;
  if (Phase == PostPciEnumeration) {
    FspPhase     = EnumInitPhaseAfterPciEnumeration;
    FspPhaseMask = BIT0;
  } else if (Phase == ReadyToBoot) {
    FspPhase     = EnumInitPhaseReadyToBoot;
    FspPhaseMask = BIT1;
  } else if (Phase == EndOfFirmware) {
    FspPhase = EnumInitPhaseEndOfFirmware;
    FspPhaseMask = BIT2;
  }

  if (FspPhaseMask != 0) {

    if (FspPhaseMask & BIT1) {
      // Sigal MP to stop
      MpInit (EnumMpInitDone);
    }

    if ((FspPhaseMask & mFspPhaseMask) == 0) {
      // Only call FSP notify once
      Status = CallFspNotifyPhase (FspPhase);
      FspResetHandler (Status);
      ASSERT_EFI_ERROR (Status);

      mFspPhaseMask |= FspPhaseMask;
    }

    if (FspPhaseMask & (BIT1 | BIT2)) {
      // Only call board init for ReadyToBoot & EndOfFirmware
      BoardInit (Phase);
    }

    if (FspPhaseMask & BIT2) {
      InitializeDebugAgent (DEBUG_AGENT_INIT_DXE_UNLOAD, NULL, NULL);
    }
  }

}

/**
  Display graphical splash screen

  @retval EFI_SUCCESS     Splash screen was successfully displayed
  @retval EFI_NOT_FOUND   Frame buffer hob not found
  @retval EFI_UNSUPPORTED BmpImage is not a valid *.BMP image

**/
EFI_STATUS
EFIAPI
DisplaySplash (
  VOID
  )
{
  EFI_STATUS                           Status;
  VOID                                *SplashLogoBmp;
  EFI_PEI_GRAPHICS_INFO_HOB           *GfxInfoHob;

  // Get framebuffer info
  GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (NULL, NULL, &gEfiGraphicsInfoHobGuid);
  if (GfxInfoHob == NULL) {
    return EFI_NOT_FOUND;
  }

  // Convert image from BMP format and write to frame buffer
  SplashLogoBmp = (VOID *)(UINTN)PCD_GET32_WITH_ADJUST (PcdSplashLogoAddress);
  ASSERT (SplashLogoBmp != NULL);
  Status = DisplayBmpToFrameBuffer (SplashLogoBmp, NULL, 0, GfxInfoHob);

  return Status;
}

/**
  Print the stack/HOB and heap usage information.

**/
VOID
EFIAPI
PrintStackHeapInfo (
  VOID
  )
{
  LOADER_GLOBAL_DATA             *LdrGlobal;
  EFI_HOB_HANDOFF_INFO_TABLE     *HandOffHob;
  UINT32                          StackBot;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  HandOffHob = (EFI_HOB_HANDOFF_INFO_TABLE *)LdrGlobal->LdrHobList;

  StackBot = DetectUsedStackBottom ((UINT32)HandOffHob->EfiMemoryTop,
                                    (UINT32)HandOffHob->EfiMemoryTop - (UINT32)HandOffHob->EfiFreeMemoryBottom);
  DEBUG ((
           DEBUG_INFO,
           "Stage2 stack: 0x%X (stack used 0x%X, HOB used 0x%X, 0x%X free)\n",
           (UINT32)HandOffHob->EfiMemoryTop - (UINT32)HandOffHob->EfiMemoryBottom,
           (UINT32)HandOffHob->EfiMemoryTop - StackBot,
           (UINT32)HandOffHob->EfiFreeMemoryBottom - (UINT32)HandOffHob->EfiMemoryBottom,
           StackBot - (UINT32)HandOffHob->EfiFreeMemoryBottom
           ));

  DEBUG ((
           DEBUG_INFO,
           "Stage2 heap: 0x%X (0x%X used, 0x%X free, 0x%x max used)\n",
           LdrGlobal->MemPoolEnd - LdrGlobal->MemPoolStart,
           (LdrGlobal->MemPoolEnd - LdrGlobal->MemPoolCurrTop) +
           (LdrGlobal->MemPoolCurrBottom - LdrGlobal->MemPoolStart),
           LdrGlobal->MemPoolCurrTop - LdrGlobal->MemPoolCurrBottom,
           LdrGlobal->MemPoolMaxUsed
           ));
}


/**
  Initialize services so that payload can consume.

**/
VOID
EFIAPI
InitializeService (
  VOID
  )
{
  LOADER_GLOBAL_DATA             *LdrGlobal;
  SERVICES_LIST                  *ServiceList;

  // Initialize HOB
  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();

  // Zero out all services installed in previous stages.
  // Those service might have been expired and stage2 needs to reinstall if required.
  ServiceList = (SERVICES_LIST *)LdrGlobal->ServicePtr;
  if (ServiceList != NULL) {
    ZeroMem (ServiceList->Header, sizeof (SERVICE_COMMON_HEADER *) * ServiceList->Count);
  }

  // Register platform service
  RegisterService ((VOID *)&mPlatformService);

}
