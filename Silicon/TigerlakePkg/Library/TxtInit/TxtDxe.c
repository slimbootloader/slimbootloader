/** @file
  This is the main DXE file for TXT. It represents an abstract outline of the
  steps required during DXE for enabling TXT. Each individual step is further
  abstracted behind a function call interface. This is intended to minimize
  the need to modify this file when porting TXT to future platforms.

@copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2020 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:

**/


#include "TxtDxeLib.h"

#if UEFI_TXT_ENABLED

GLOBAL_REMOVE_IF_UNREFERENCED TXT_DXE_LIB_CONTEXT mTxtDxeCtx;


/**
  This function gets registered as a callback to run the SCHECK function
  from the TXT BIOS ACM as a result of Boot Events.

  @param[in]  Event    A pointer to the Event that triggered the callback.
  @param[in]  Context  A pointer to private data registered with the callback function.
  **/
VOID
EFIAPI
ScheckCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS       Status;
  VOID             *ProtocolPointer;

  ///
  /// Check if this is first time called by EfiCreateProtocolNotifyEvent() or not,
  /// if it is, we will skip it until real event is triggered
  ///
  Status = gBS->LocateProtocol (&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_SUCCESS != Status) {
    return;
  }

//
// TXT:RestrictedBegin
//
  ///
  /// If TXT_SKIP_SCHECK_REQUIRED flag is set, skip DoScheck.
  ///
  if (mTxtDxeCtx.TxtInfoData->Flags & TXT_SKIP_SCHECK_REQUIRED) {
    gBS->CloseEvent (Event);
  }
//
// TXT:RestrictedEnd
//

  DEBUG ((DEBUG_INFO, "TXTDXE::Running of DoScheck\n"));
  DoScheck (&mTxtDxeCtx);

  ///
  /// Closed the event to avoid call twice when launch shell
  ///
  gBS->CloseEvent (Event);
}


/**
  This is the entry point to the TXT DXE Driver.  This routine checks to see if
  the platform should be configured for TXT and if so, configures the platform
  by reserving and initializing TXT Configuration Space and TXT Device Memory and
  registering a callback to run SCHECK from the TXT BIOS ACM prior to boot.

  If the platform should not be configured for TXT, this routine checks the
  establishment bit in the TPM and resets it if it is asserted.

  @param[in]  ImageHandle       A handle for this module
  @param[in]  SystemTable       A pointer to the EFI System Table

  @retval     EFI_SUCCESS       If TXT initialization succeed
  @retval     EFI_UNSUPPORTED   If TXT criterias are not met
**/
EFI_STATUS
EFIAPI
DriverEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  void * Registration;

  ///
  /// Initialize the platform specific code
  ///
  Status = InitializeTxtDxeLib (ImageHandle, SystemTable, &mTxtDxeCtx);
  ///
  /// If failure - assume TXT is not enabled
  ///
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TXTDXE::InitializeTxtDxeLib failed.... Unloading\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// If ESTS.TXTRESET bit is set, skip all other functions since
  /// attempt to execute GETSEC will hang system. Skipping allows to
  /// boot to OS and let MLE assess situation.
  ///
  if (IsTxtResetSet (&mTxtDxeCtx)) {
    DEBUG ((DEBUG_ERROR, "TXTDXE::TXT_RESET bit is set.... Unloading\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// If TXT is enabled, configure platform appropriately.
  /// Code assumes that if TXT is enabled by CPU driver than all checks
  /// are passed, i.e. TPM is present, CPU and CS are TXT capable.
  ///
  ///
  /// Add to check CPU TXT capable in case CPU drivers do not check additional requirements
  ///
  if ((mTxtDxeCtx.TxtInfoData->ChipsetIsTxtCapable) && (IsTxtProcessor ()) && (IsTxtEnabled (&mTxtDxeCtx))) {
    DEBUG ((DEBUG_INFO, "TXTDXE::TXT Enabled\n"));

    ///
    /// Allocate and Initialize TXT Device Memory
    ///
    Status = SetupTxtDeviceMemory (&mTxtDxeCtx);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "TXTDXE::SetupTxtDeviceMemory failed.... Unloading\n"));
      return EFI_UNSUPPORTED;
    }

    ///
    /// Create callback to run SCHECK; this should execute before global SMI enable is locked
    ///
    EfiCreateProtocolNotifyEvent (
      &gEfiPciEnumerationCompleteProtocolGuid,
      TPL_CALLBACK,
      ScheckCallback,
      NULL,
      &Registration
      );

  } else {
    ///
    /// TXT is not enabled, so make sure TPM Establishment
    /// bit is de-asserted
    ///
    DEBUG ((DEBUG_INFO, "TXTDXE::TXT Disabled\n"));

    if (IsTxtEstablished (&mTxtDxeCtx)) {
      ///
      /// We can invoke BIOS ACM function only if CS and CPU are TXT
      /// capable
      ///
      if ((mTxtDxeCtx.TxtInfoData->ChipsetIsTxtCapable) &&
          (IsTxtProcessor ()) &&
          !(mTxtDxeCtx.TxtInfoData->Flags & TPM_INIT_FAILED)
          ) {
        DEBUG ((DEBUG_INFO, "TXTDXE::Resetting TPM Establishment bit\n"));
        ResetTpmEstBit (&mTxtDxeCtx);
      }
    }
    ///
    /// Reset AUX
    ///
    Status = ResetTpmAux (&mTxtDxeCtx);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

#endif //UEFI_TXT_ENABLED