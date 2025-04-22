/** @file
  Debug Agent Library instance implementition in Slim Bootloader.

  Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/MemoryAllocationLib.h>
#include <Service/PlatformService.h>
#include "DebugAgent.h"

GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN  mSkipBreakpoint = FALSE;

typedef struct {
  UINT32                  InitFlag;
  VOID                    *Context;
  DEBUG_AGENT_CONTINUE    Function;
} DEBUG_AGENT_PHASE2_CONTEXT;

/**
  Caller provided function to be invoked at the end of DebugPortInitialize().

  Refer to the description for DebugPortInitialize() for more details.

  @param[in] Context           The first input argument of DebugPortInitialize().
  @param[in] DebugPortHandle   Debug port handle created by Debug Communication Library.

**/
VOID
EFIAPI
InitializeDebugAgentPhase2 (
  IN VOID                  *Context,
  IN DEBUG_PORT_HANDLE     DebugPortHandle
  );

/**
  Check if debug agent support multi-processor.

  @retval TRUE    Multi-processor is supported.
  @retval FALSE   Multi-processor is not supported.

**/
BOOLEAN
MultiProcessorDebugSupport (
  VOID
  )
{
  return FALSE;
}

/**
  Read the Attach/Break-in symbols from the debug port.

  @param[in]  Handle         Pointer to Debug Port handle.
  @param[out] BreakSymbol    Returned break symbol.

  @retval EFI_SUCCESS        Read the symbol in BreakSymbol.
  @retval EFI_NOT_FOUND      No read the break symbol.

**/
EFI_STATUS
DebugReadBreakSymbol (
  IN  DEBUG_PORT_HANDLE      Handle,
  OUT UINT8                  *BreakSymbol
  )
{
  EFI_STATUS                 Status;
  DEBUG_PACKET_HEADER        DebugHeader;
  UINT8                      *Data8;

  *BreakSymbol = 0;
  //
  // If Debug Port buffer has data, read it till it was break symbol or Debug Port buffer empty.
  //
  Data8 = (UINT8 *) &DebugHeader;
  while (TRUE) {
    //
    // If start symbol is not received
    //
    if (!DebugPortPollBuffer (Handle)) {
      //
      // If no data in Debug Port, exit
      //
      break;
    }
    //
    // Try to read the start symbol
    //
    DebugAgentReadBuffer (Handle, Data8, 1, 0);
    if (*Data8 == DEBUG_STARTING_SYMBOL_ATTACH) {
      *BreakSymbol = *Data8;
      DebugAgentMsgPrint (DEBUG_AGENT_INFO, "Debug Timer attach symbol received %x", *BreakSymbol);
      return EFI_SUCCESS;
    }
    if (*Data8 == DEBUG_STARTING_SYMBOL_NORMAL) {
      Status = ReadRemainingBreakPacket (Handle, &DebugHeader);
      if (Status == EFI_SUCCESS) {
        *BreakSymbol = DebugHeader.Command;
        DebugAgentMsgPrint (DEBUG_AGENT_INFO, "Debug Timer break symbol received %x", *BreakSymbol);
        return EFI_SUCCESS;
      }
      if (Status == EFI_TIMEOUT) {
        break;
      }
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Get the pointer to location saved Mailbox pointer from IDT entry.

**/
VOID *
GetLocationSavedMailboxPointerInIdtEntry (
  VOID
  )
{
  UINTN                     *MailboxLocation;

  MailboxLocation = (UINTN *) GetExceptionHandlerInIdtEntry (DEBUG_MAILBOX_VECTOR);
  //
  // *MailboxLocation is the pointer to Mailbox
  //
  VerifyMailboxChecksum ((DEBUG_AGENT_MAILBOX *) (*MailboxLocation));
  return MailboxLocation;
}

/**
  Set the pointer of Mailbox into IDT entry before memory is ready.

  @param[in]  MailboxLocation    Pointer to location saved Mailbox pointer.

**/
VOID
SetLocationSavedMailboxPointerInIdtEntry (
  IN VOID                  *MailboxLocation
  )
{
  SetExceptionHandlerInIdtEntry (DEBUG_MAILBOX_VECTOR, MailboxLocation);
}

/**
  Get the location of Mailbox pointer from the GUIDed HOB.

  @return Pointer to the location saved Mailbox pointer.

**/
UINT64 *
GetMailboxLocationFromHob (
  VOID
  )
{
  EFI_HOB_GUID_TYPE        *GuidHob;

  GuidHob = GetFirstGuidHob (&gEfiDebugAgentGuid);
  if (GuidHob == NULL) {
    return NULL;
  }
  return (UINT64 *) (GET_GUID_HOB_DATA(GuidHob));
}

/**
  Get Debug Agent Mailbox pointer.

  @return Mailbox pointer.

**/
DEBUG_AGENT_MAILBOX *
GetMailboxPointer (
  VOID
  )
{
  UINT64               DebugPortHandle;
  UINT64               *MailboxLocationInIdt;
  UINT64               *MailboxLocationInHob;
  DEBUG_AGENT_MAILBOX  *Mailbox;

  //
  // Get mailbox from IDT entry firstly
  //
  MailboxLocationInIdt = GetLocationSavedMailboxPointerInIdtEntry ();
  Mailbox = (DEBUG_AGENT_MAILBOX *)(UINTN)(*MailboxLocationInIdt);
  //
  // Cannot used GetDebugFlag() to get Debug Flag to avoid GetMailboxPointer() nested
  //
  if (Mailbox->DebugFlag.Bits.CheckMailboxInHob != 1 ||
      Mailbox->DebugFlag.Bits.InitArch != DEBUG_ARCH_SYMBOL) {
    //
    // If mailbox was setup in SEC or the current CPU arch is different from the init arch
    // Debug Agent initialized, return the mailbox from IDT entry directly.
    // Otherwise, we need to check the mailbox location saved in GUIDed HOB further.
    //
    return Mailbox;
  }

  MailboxLocationInHob = GetMailboxLocationFromHob ();
  //
  // Compare mailbox in IDT enry with mailbox in HOB,
  // need to fix mailbox location if HOB moved by PEI CORE
  //
  if (MailboxLocationInHob != MailboxLocationInIdt && MailboxLocationInHob != NULL) {
    Mailbox = (DEBUG_AGENT_MAILBOX *)(UINTN)(*MailboxLocationInHob);
    //
    // Fix up Debug Port handler and save new mailbox in IDT entry
    //
    Mailbox = (DEBUG_AGENT_MAILBOX *)((UINTN)Mailbox + ((UINTN)(MailboxLocationInHob) - (UINTN)MailboxLocationInIdt));
    DebugPortHandle = (UINTN)Mailbox->DebugPortHandle + ((UINTN)(MailboxLocationInHob) - (UINTN)MailboxLocationInIdt);
    UpdateMailboxContent (Mailbox, DEBUG_MAILBOX_DEBUG_PORT_HANDLE_INDEX, DebugPortHandle);
    *MailboxLocationInHob = (UINT64)(UINTN)Mailbox;
    SetLocationSavedMailboxPointerInIdtEntry (MailboxLocationInHob);
    //
    // Clean CheckMailboxInHob flag
    //
    Mailbox->DebugFlag.Bits.CheckMailboxInHob = 0;
    UpdateMailboxChecksum (Mailbox);
  }
  return Mailbox;
}

/**
  Get debug port handle.

  @return Debug port handle.

**/
DEBUG_PORT_HANDLE
GetDebugPortHandle (
  VOID
  )
{
  DEBUG_AGENT_MAILBOX    *DebugAgentMailbox;

  DebugAgentMailbox = GetMailboxPointer ();

  return (DEBUG_PORT_HANDLE) (UINTN)(DebugAgentMailbox->DebugPortHandle);
}

/**
  Update IDT entries for DebugAgent whenever IDT base is re-configured

  @param[in] IdtDescriptor      IDT descriptor which needs to be updated
  @param[in] Function           Continue function called by debug agent library

**/
VOID
UpdateDebugAgentIdt (
  IN IA32_DESCRIPTOR       *IdtDescriptor,
  IN DEBUG_AGENT_CONTINUE   Function
  )
{
  IA32_DESCRIPTOR              Idtr;
  IA32_IDT_GATE_DESCRIPTOR    *IdtEntry;
  UINT64                      *MailboxLocationPointer;
  DEBUG_AGENT_MAILBOX         *Mailbox;

  if (IsDebugAgentInitialzed() != TRUE) {
    if (Function != NULL) {
      Function (IdtDescriptor);
    }
    DEBUG ((DEBUG_WARN, "Debug Agent: The DebugAgent is not initialized yet!\n"));
    return;
  }

  //
  // Disable Debug Timer interrupt
  //
  SaveAndSetDebugTimerInterrupt (FALSE);
  //
  // Save and disable original interrupt status
  //
  SaveAndDisableInterrupts ();

  //
  // Save current IDT descriptor
  //
  AsmReadIdtr (&Idtr);

  if (Function != NULL) {
    Function (IdtDescriptor);
  }

  IdtDescriptor = &Idtr;

  IdtEntry = (IA32_IDT_GATE_DESCRIPTOR *)(IdtDescriptor->Base);
  MailboxLocationPointer = (UINT64 *) ((UINTN) IdtEntry[DEBUG_MAILBOX_VECTOR].Bits.OffsetLow +
                                      ((UINTN) IdtEntry[DEBUG_MAILBOX_VECTOR].Bits.OffsetHigh << 16));
  Mailbox = (DEBUG_AGENT_MAILBOX *) (UINTN)(*MailboxLocationPointer);
  //
  // Mailbox should valid and setup before executing thunk code
  //
  VerifyMailboxChecksum (Mailbox);
  //
  // Set up IDT entries
  //
  InitializeDebugIdt ();
  //
  // Update IDT entry to save location pointer saved the mailbox pointer
  //
  SetLocationSavedMailboxPointerInIdtEntry (MailboxLocationPointer);

  //
  // Enable Debug Timer interrupt
  //
  SaveAndSetDebugTimerInterrupt (TRUE);
  //
  // Enable CPU interrupts so debug timer interrupts can be delivered
  //
  EnableInterrupts ();
}

/**
  Initialize debug agent.

  This function is used to set up debug environment for all stages.

  If InitFlag is DEBUG_AGENT_INIT_PREMEM_SEC, it will overirde IDT table entries
  and initialize debug port. It will enable interrupt to support break-in feature.
  It will set up debug agent Mailbox in cache-as-ramfrom. It will be called before
  physical memory is ready.
  If InitFlag is DEBUG_AGENT_INIT_POSTMEM_SEC, debug agent will build one GUIDed
  HOB to copy debug agent Mailbox. It will be called after physical memory is ready.

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
  )
{
  DEBUG_AGENT_MAILBOX              *Mailbox;
  DEBUG_AGENT_MAILBOX              *NewMailbox;
  DEBUG_AGENT_MAILBOX              MailboxInStack;
  DEBUG_AGENT_PHASE2_CONTEXT       Phase2Context;
  UINT64                           MailboxLocation;
  UINT64                           *MailboxLocationPointer;
  UINT32                           DebugTimerFrequency;
  BOOLEAN                          CpuInterruptState;

  //
  // Disable interrupts and save current interrupt state
  //
  CpuInterruptState = SaveAndDisableInterrupts();

  switch (InitFlag) {
  case DEBUG_AGENT_INIT_PREMEM_SEC:
    if (IsDebugAgentInitialzed() == TRUE) {
      DEBUG ((DEBUG_WARN, "Debug Agent: The DebugAgent has already been enabled!\n"));
      break;
    }

    InitializeDebugIdt ();

    MailboxLocation = (UINT64)(UINTN)&MailboxInStack;
    Mailbox = &MailboxInStack;
    ZeroMem ((VOID *) Mailbox, sizeof (DEBUG_AGENT_MAILBOX));
    //
    // Get and save debug port handle and set the length of memory block.
    //
    SetLocationSavedMailboxPointerInIdtEntry (&MailboxLocation);
    //
    // Force error message could be printed during the first shakehand between Target/HOST.
    //
    SetDebugFlag (DEBUG_AGENT_FLAG_PRINT_ERROR_LEVEL, DEBUG_AGENT_ERROR);
    //
    // Save init arch type when debug agent initialized
    //
    SetDebugFlag (DEBUG_AGENT_FLAG_INIT_ARCH, DEBUG_ARCH_SYMBOL);
    //
    // Initialize Debug Timer hardware and save its frequency
    //
    InitializeDebugTimer (&DebugTimerFrequency, TRUE);
    UpdateMailboxContent (Mailbox, DEBUG_MAILBOX_DEBUG_TIMER_FREQUENCY, DebugTimerFrequency);

    Phase2Context.InitFlag = InitFlag;
    Phase2Context.Context  = Context;
    Phase2Context.Function = Function;
    DebugPortInitialize ((VOID *) &Phase2Context, InitializeDebugAgentPhase2);
    //
    // If reaches here, it means Debug Port initialization failed.
    //
    DEBUG ((DEBUG_ERROR, "Debug Agent: Debug port initialization failed.\n"));
    break;

  case DEBUG_AGENT_INIT_POSTMEM_SEC:
    Mailbox = GetMailboxPointer ();
    //
    // Memory has been ready
    //
    SetDebugFlag (DEBUG_AGENT_FLAG_MEMORY_READY, 1);
    if (IsHostAttached ()) {
      //
      // Trigger one software interrupt to inform HOST
      //
      TriggerSoftInterrupt (MEMORY_READY_SIGNATURE);
    }
    //
    // Fix up Debug Port handle address and mailbox address
    //
    NewMailbox = (DEBUG_AGENT_MAILBOX *) AllocatePool (sizeof (DEBUG_AGENT_MAILBOX));
    MailboxLocationPointer = (UINT64 *) AllocatePool (sizeof (UINT64));
    CopyMem (NewMailbox, Mailbox, sizeof (DEBUG_AGENT_MAILBOX));
    UpdateMailboxContent (NewMailbox, DEBUG_MAILBOX_DEBUG_PORT_HANDLE_INDEX, (UINT64)(UINTN)(Mailbox->DebugPortHandle + 1));
    *MailboxLocationPointer = (UINT64)(UINTN)NewMailbox;
    //
    // Update IDT entry to save the location saved mailbox pointer
    //
    SetLocationSavedMailboxPointerInIdtEntry (MailboxLocationPointer);
    break;

  // TBD
  case DEBUG_AGENT_INIT_DXE_LOAD:
    break;

  // TBD
  case DEBUG_AGENT_INIT_DXE_UNLOAD:
    break;

  // TBD
  case DEBUG_AGENT_INIT_S3:
    break;

  default:
    //
    // Only DEBUG_AGENT_INIT_PREMEM_SEC and DEBUG_AGENT_INIT_POSTMEM_SEC are allowed for this
    // Debug Agent library instance.
    //
    DEBUG ((DEBUG_ERROR, "Debug Agent: The InitFlag value is not allowed!\n"));
    CpuDeadLoop ();
    break;
  }

  if (InitFlag == DEBUG_AGENT_INIT_POSTMEM_SEC) {
    //
    // Restore CPU Interrupt state and keep debug timer interrupt state as is
    // in DEBUG_AGENT_INIT_POSTMEM_SEC case
    //
    SetInterruptState (CpuInterruptState);
  } else {
    //
    // Enable Debug Timer interrupt
    //
    SaveAndSetDebugTimerInterrupt (TRUE);
    //
    // Enable CPU interrupts so debug timer interrupts can be delivered
    //
    EnableInterrupts ();
  }
  //
  // If Function is not NULL, invoke it always whatever debug agent was initialized sucesssfully or not.
  //
  if (Function != NULL) {
    Function (Context);
  }
}

/**
  Caller provided function to be invoked at the end of DebugPortInitialize().

  Refer to the description for DebugPortInitialize() for more details.

  @param[in] Context           The first input argument of DebugPortInitialize().
  @param[in] DebugPortHandle   Debug port handle created by Debug Communication Library.

**/
VOID
EFIAPI
InitializeDebugAgentPhase2 (
  IN VOID                  *Context,
  IN DEBUG_PORT_HANDLE     DebugPortHandle
  )
{
  DEBUG_AGENT_PHASE2_CONTEXT *Phase2Context;
  UINT64                     *MailboxLocation;
  DEBUG_AGENT_MAILBOX        *Mailbox;
  UINT64                     NewDebugPortHandle;

  Phase2Context = (DEBUG_AGENT_PHASE2_CONTEXT *) Context;
  MailboxLocation = GetLocationSavedMailboxPointerInIdtEntry ();
  Mailbox = (DEBUG_AGENT_MAILBOX *)(UINTN)(*MailboxLocation);
  NewDebugPortHandle = (UINT64)(UINTN)DebugPortHandle;
  UpdateMailboxContent (Mailbox, DEBUG_MAILBOX_DEBUG_PORT_HANDLE_INDEX, NewDebugPortHandle);

  //
  // Trigger one software interrupt to inform HOST
  //
  TriggerSoftInterrupt (SYSTEM_RESET_SIGNATURE);

  if (Phase2Context->InitFlag == DEBUG_AGENT_INIT_PREMEM_SEC) {
    //
    // Enable Debug Timer interrupt
    //
    SaveAndSetDebugTimerInterrupt (TRUE);
    //
    // Enable CPU interrupts so debug timer interrupts can be delivered
    //
    EnableInterrupts ();
    //
    // Call continuation function if it is not NULL.
    //
    Phase2Context->Function (Phase2Context->Context);
  }
}
