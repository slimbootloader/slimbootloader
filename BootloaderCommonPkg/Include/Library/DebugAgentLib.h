/** @file
  Debug Agent Library provide source-level debug capability.

Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __DEBUG_AGENT_LIB_H__
#define __DEBUG_AGENT_LIB_H__

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>

#define DEBUG_AGENT_INIT_PREMEM_SEC              1
#define DEBUG_AGENT_INIT_POSTMEM_SEC             2
#define DEBUG_AGENT_INIT_DXE_CORE                3
#define DEBUG_AGENT_INIT_SMM                     4
#define DEBUG_AGENT_INIT_ENTER_SMI               5
#define DEBUG_AGENT_INIT_EXIT_SMI                6
#define DEBUG_AGENT_INIT_S3                      7
#define DEBUG_AGENT_INIT_DXE_AP                  8
#define DEBUG_AGENT_INIT_PEI                     9
#define DEBUG_AGENT_INIT_DXE_LOAD               10
#define DEBUG_AGENT_INIT_DXE_UNLOAD             11
#define DEBUG_AGENT_INIT_THUNK_PEI_IA32TOX64    12

/**
  Caller provided function to be invoked at the end of InitializeDebugAgent().

  Refer to the description for InitializeDebugAgent() for more details.

  @param[in] Context    The first input parameter of InitializeDebugAgent().

**/
typedef
VOID
(EFIAPI * DEBUG_AGENT_CONTINUE)(
  IN VOID        *Context
  );

//
// Context for PE_COFF_LOADER_IMAGE_CONTEXT
//
typedef struct {
  PHYSICAL_ADDRESS                  ImageAddress;
  CHAR8                             *PdbPointer;
} PE_COFF_LOADER_IMAGE_CONTEXT;

/**
  Performs additional actions after a PE/COFF image has been loaded and relocated.

  If ImageContext is NULL, then ASSERT().

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that has already been loaded and relocated.

**/
VOID
EFIAPI
PeCoffLoaderRelocateImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  );

/**
  Performs additional actions just before a PE/COFF image is unloaded.  Any resources
  that were allocated by PeCoffLoaderRelocateImageExtraAction() must be freed.

  If ImageContext is NULL, then ASSERT().

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that is being unloaded.

**/
VOID
EFIAPI
PeCoffLoaderUnloadImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  );

/**
  Find and report image info to HOST.

  @param  ImageBase        Pointer to the current image base.

**/
VOID
PeCoffFindAndReportImageInfo (
  IN UINT32   ImageBase
  );

/**
  Update IDT entries for DebugAgent whenever IDT base is re-configured

  @param[in] IdtDescriptor      IDT descriptor which needs to be updated
  @param[in] Function           Continue function called by debug agent library

**/
VOID
UpdateDebugAgentIdt (
  IN IA32_DESCRIPTOR       *IdtDescriptor,
  IN DEBUG_AGENT_CONTINUE   Function
  );

/**
  Initialize debug agent.

  This function is used to set up debug environment to support source level debugging.
  If certain Debug Agent Library instance has to save some private data in the stack,
  this function must work on the mode that doesn't return to the caller, then
  the caller needs to wrap up all rest of logic after InitializeDebugAgent() into one
  function and pass it into InitializeDebugAgent(). InitializeDebugAgent() is
  responsible to invoke the passing-in function at the end of InitializeDebugAgent().

  If the parameter Function is not NULL, Debug Agent Library instance will invoke it by
  passing in the Context to be its parameter.

  If Function() is NULL, Debug Agent Library instance will return after setup debug
  environment.

  @param[in] InitFlag     Init flag is used to decide the initialize process.
  @param[in] Context      Context needed according to InitFlag; it was optional.
  @param[in] Function     Continue function called by debug agent library; it was
                          optional.

**/
VOID
EFIAPI
InitializeDebugAgent (
  IN UINT32                InitFlag,
  IN VOID                  *Context, OPTIONAL
  IN DEBUG_AGENT_CONTINUE  Function  OPTIONAL
  );

/**
  Enable/Disable the interrupt of debug timer and return the interrupt state
  prior to the operation.

  If EnableStatus is TRUE, enable the interrupt of debug timer.
  If EnableStatus is FALSE, disable the interrupt of debug timer.

  @param[in] EnableStatus    Enable/Disable.

  @retval TRUE  Debug timer interrupt were enabled on entry to this call.
  @retval FALSE Debug timer interrupt were disabled on entry to this call.

**/
BOOLEAN
EFIAPI
SaveAndSetDebugTimerInterrupt (
  IN BOOLEAN                EnableStatus
  );

#endif
