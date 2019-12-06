/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _STAGE2_H_
#define _STAGE2_H_

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DecompressLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PagingLib.h>
#include <Library/FspSupportLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/MpInitLib.h>
#include <Library/PciEnumerationLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/AcpiInitLib.h>
#include <Library/FspApiLib.h>
#include <Library/LoaderLib.h>
#include <Library/GraphicsLib.h>
#include <Library/CpuExceptionLib.h>
#include <Library/CryptoLib.h>
#include <Library/SecureBootLib.h>
#include <Library/TpmLib.h>
#include <Library/LinuxLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Library/LiteFvLib.h>
#include <Library/SortLib.h>
#include <Library/ContainerLib.h>
#include <Guid/BootLoaderServiceGuid.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <Guid/SeedInfoHobGuid.h>
#include <Guid/KeyHashGuid.h>
#include <Guid/LoaderLibraryDataGuid.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/SmmInformationGuid.h>
#include <Service/PlatformService.h>
#include <Pi/PiBootMode.h>
#include <FspEas.h>
#include <Service/PlatformService.h>
#include <Library/ResetSystemLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/ElfLib.h>
#include <HashStore.h>
#include <VerInfo.h>

#include <Library/SmbiosInitLib.h>

#define UIMAGE_FIT_MAGIC               (0x56190527)

/**
  Unmap the previous mapped stage images.

  It will reload GDT and disable paging so that the memory goes back to 1:1 mapping.

**/
VOID
UnmapStage (
  VOID
  );

/**
  Build some basic HOBs

  After silicon initialization, the information is available
  to build some basic HOBs. These HOBs could be used/updated
  by stage2 code, or used by payload.

  @param Stage2Hob         Stage2 HOB pointer.

  @return                  The HOB list pointer.
**/
VOID *
EFIAPI
BuildBaseInfoHob (
  IN  STAGE2_HOB                   *Stage2Hob
  );

/**
  Build and update HOBs.

  Before jumping to payload, more information is available, so update some HOBs
  built early, and build more HOBs for payload.

  @param Stage2Hob         Stage2 HOB pointer.

  @return                  The HOB list pointer.
**/
VOID *
EFIAPI
BuildExtraInfoHob (
  IN  STAGE2_HOB                   *Stage2Hob
  );

/**
  Display graphical splash screen

  @retval EFI_SUCCESS     Splash screen was successfully displayed
  @retval EFI_UNSUPPORTED Frame buffer access not supported
  @retval EFI_UNSUPPORTED BmpImage is not a valid *.BMP image

**/
EFI_STATUS
EFIAPI
DisplaySplash (
  VOID
  );

/**
  Load payload from boot media to its execution address.

  @param[in]  Stage2Hob         STAGE2_HOB HOB pointer.
  @param[out] ExeAddress        The payload execution address.

  @retval     EFI_SUCCESS       Load payload successfully.
  @retval     others            Failed to load payload.

**/
EFI_STATUS
LoadPayloadImage (
  IN  STAGE2_HOB                  *Stage2Hob,
  OUT UINT32                     **ExeAddress
  );

/**
  Print the stack/HOB and heap usage information.

**/
VOID
EFIAPI
PrintStackHeapInfo (
  VOID
  );

/**
  Initialize services so that payload can consume.

**/
VOID
EFIAPI
InitializeService (
  VOID
  );

/**
  Platform notify service interface.

  @param[in] Phase          Boot phase to notify.

**/
VOID
BoardNotifyPhase (
  IN BOARD_INIT_PHASE   Phase
  );

#endif
