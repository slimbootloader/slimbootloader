/** @file
  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _TXT_CTX_H_
#define _TXT_CTX_H_

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <TxtInfoHob.h>
#include <Library/TimerLib.h>
#include <Register/Cpuid.h>


///
/// The following switch is used in EfiScriptLib.h file. If enabled -
/// forces linking to EfiScriptLib library
///
#define BASE_ADDR_MASK                      0xFFFFF000
#define TEST_PATTERN                        0x5A5A5A5A5A5A5A5A
#define MP_TIMEOUT_FOR_STARTUP_ALL_APS      0           ///< Set 0 for BSP always wait for APs

///
/// Chispet register
///
#define TXT_OPT_IN_VMX_AND_SMX_MSR_VALUE    0xFF03

#define LOCAL_APIC_THERMAL_DEF              0x330

#pragma pack(push, 1)
typedef struct _TXT_LIB_CONTEXT_ {
  EFI_HANDLE               ImageHandle;
  UINTN                    CpuCount;
  TXT_INFO_DATA            *TxtInfoData;
} TXT_LIB_CONTEXT;
#pragma pack(pop)

/**
  This routine initializes and collects all Protocols and data required
  by the routines in this file.

  @param[in]      ImageHandle      A pointer to the Image Handle for this file.
  @param[in]      SystemTable      A pointer to the EFI System Table
  @param[in][out] TxtLibCtx        A pointer to a caller allocated data structure that contains
                                   all of the Protocols and data required by the routines
                                   in this file.

  @retval         EFI_SUCCESS      Return EFI_SUCCESS if no error happen
  @retval         EFI_NOT_FOUND    If TxtInfoHob is not found
**/
EFI_STATUS
InitializeTxtLib (
  IN EFI_HANDLE              ImageHandle,
  //IN EFI_SYSTEM_TABLE        *SystemTable,
  IN OUT TXT_LIB_CONTEXT *TxtLibCtx
  );

/**
  Determines whether or not the current processor is TXT Capable.

  @retval TRUE   If the current processor supports TXT
  @retval FALSE  If the current processor does not support TXT
**/
BOOLEAN
IsTxtProcessor (
  VOID
  );

/**
  Determines whether or not the platform has executed an TXT launch by
  examining the TPM Establishment bit.

  @param[in]  TxtLibCtx  A pointer to an initialized TXT DXE Context data structure

  @retval     TRUE       If the TPM establishment bit is asserted.
  @retval     FALSE      If the TPM establishment bit is unasserted.
**/
BOOLEAN
IsTxtEstablished (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  );

/**
  Determines whether or not the platform has executed an TXT launch by
  examining the TPM Establishment bit.

  @param[in]  TxtLibCtx A pointer to an initialized TXT DXE Context data structure

  @retval     TRUE      If the TPM establishment bit is asserted.
  @retval     FALSE     If the TPM establishment bit is unasserted.
**/
BOOLEAN
IsTxtResetSet (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  );

/**
  Determines whether or not the platform requires initialization for TXT use.

  @param[in]  TxtLibCtx   A pointer to an initialized TXT DXE Context data structure

  @retval     TRUE        If the the platoform should be configured for TXT.
  @retval     FALSE       If TXT is not to be used.
**/
BOOLEAN
IsTxtEnabled (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  );

/**
  Allocates 1 MB of 1MB-aligned memory for use as TXT Device Memory.  Records
  the location of TXT Device Memory in TXT Chipset registers and then adds
  programming instructions for these registers into BootScript.

  @param[in]  TxtLibCtx     A pointer to an initialized TXT DXE Context data structure

  @retval     EFI_SUCCESS   TXT Device memory has been successfully initialized.
  @retval     EFI_ERROR     TXT Device memory not awailable.
**/
EFI_STATUS
SetupTxtDeviceMemory (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  );

/**
  Invokes TxtLibLaunchBiosAcm to execute the SCHECK function.

  @param[in]  TxtLibCtx    A pointer to an initialized TXT DXE Context data structure

  @retval     EFI_SUCCESS  Always.
**/
EFI_STATUS
DoScheck (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  );

/**
  Invokes TxtLibLaunchBiosAcm to reset the TPM's establishment bit.

  @param[in]  TxtLibCtx    A pointer to an initialized TXT DXE Context data structure

  @retval     EFI_SUCCESS  Always.
**/
EFI_STATUS
ResetTpmEstBit (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  );

/**
  Sets up the system and then launches the TXT BIOS ACM to run the function
  requested by AcmFunction.

  @param[in]  AcmBase      Base address of BIOS ACM location
  @param[in]  Funct        Function number of BIOS ACM to be executed

  @retval     EFI_SUCCESS  Always.
**/
VOID
LaunchBiosAcm (
  IN UINT64 AcmBase,
  IN UINT64 Funct
  );

/**
  Sets up the system and then launches the TXT BIOS ACM to run the function
  requested by AcmFunction.

  @param[in]  AcmFunction    Constant that represents the function from the BIOS ACM
                             that should be executed.
  @param[in]  TxtLibCtx      A pointer to an initialized TXT DXE Context data structure

  @retval     EFI_SUCCESS    Always.
**/
EFI_STATUS
TxtLibLaunchBiosAcm (
  IN TXT_LIB_CONTEXT *TxtLibCtx,
  IN UINT64              AcmFunction
  );

/**
  AP initial routine executed through MP service for TXT SCHECK

  @param[in]  Buffer  Not used.
**/
VOID
EFIAPI
DoApInit (
  IN VOID *Buffer
  );

/**
  Save AP configuration routine executed through MP service for TXT SCHECK

  @param[in]  Buffer  Not used.
**/
VOID
EFIAPI
ApSaveConfig (
  IN VOID *Buffer
  );

/**
  Restore AP configuration routine executed through MP service for TXT SCHECK

  @param[in]  Buffer  Not used.
**/
VOID
EFIAPI
ApRestoreConfig (
  IN VOID *Buffer
  );

/**
  Disable or restore possible SMI sources before or after POST SCHECK

  @param[in]  TxtLibCtx   A pointer to an initialized TXT DXE Context data structure
  @param[in]  Operation   Boolean value telling what operation is requested:
                          TRUE  - to save and then disable possible SMI sources
                          FALSE - to restore original SMI settings

  @retval    EFI_SUCCESS  always return EFI_SUCCESS
**/
EFI_STATUS
DisableSmiSources (
  IN TXT_LIB_CONTEXT *TxtLibCtx,
  IN BOOLEAN             Operation
  );

/**
  This function gets called before/after run the SCHECK function, intend to avoid platform specific SMIs to interfere in BIOS POST
  if BIOS SMM MP services or synchonization code is not well considered that some of APs are not waken up from Wait-for-SIPI state.
  Function should preserve original SMI enabling setting in augument is TRUE, and then restore it in augurment is FALSE.
  The caller is DisableSmiSources() in TxtLib.c

  @param[in]  TxtLibCtx
  @param[in]  Operation = TRUE     Calling before SCHECK to saved and disable platform specific SMIs setting
                       = FALSE    Calling after SCHECK to restore platform specific SMIs setting

  @retval     EFI_SUCCESS          Always.
**/
extern
EFI_STATUS
EFIAPI
TxtLibOemDisableSmi (
  IN TXT_LIB_CONTEXT *TxtLibCtx,
  IN BOOLEAN             Operation
  );

/**
  Read policy protocol to reset AUX content

  @param[in]  TxtLibCtx       A pointer to an initialized TXT DXE Context data structure

  @retval     EFI_SUCCESS     No error happend
  @retval     EFI_NOT_FOUND   TxtPolicyProtocol is not found
**/
EFI_STATUS
ResetTpmAux (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  );

#endif

