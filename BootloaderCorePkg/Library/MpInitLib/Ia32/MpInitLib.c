/** @file
  MP init library implementation.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <MpInitLibInternal.h>

MP_ASSEMBLY_ADDRESS_MAP            mAddressMap;
ALL_CPU_INFO                       mSysCpuInfo;
volatile ALL_CPU_TASK              mSysCpuTask;
volatile MP_DATA_EXCHANGE_STRUCT   mMpDataStruct;
UINT8                             *mBackupBuffer;
UINT32                             mMpInitPhase = EnumMpInitNull;

/**
  Relocate SMM base for CPU

  @param[in]  Index       CPU index to rebase.
  @param[in]  ApicId      CPU APIC ID.

**/
VOID
SmmRebase (
  IN UINT32  Index,
  IN UINT32  ApicId
)
{
  INT32   Offset;

  Offset = PcdGet32 (PcdSmramTsegSize) - (SMM_BASE_MIN_SIZE + Index * SMM_BASE_GAP);
  if (Offset <= 0) {
    return;
  }

  AsmWriteCr2 (PcdGet32 (PcdSmramTsegBase) + Offset);
  while (!AcquireSpinLockOrFail (&mMpDataStruct.SpinLock)) {
    CpuPause ();
  }
  SendSmiIpi (ApicId);
  mMpDataStruct.SmmRebaseDoneCounter++;
  ReleaseSpinLock (&mMpDataStruct.SpinLock);
  AsmWriteCr2 (0);
}

/**
  Common CPU initialization routine.

  @param[in]  Index       CPU index to initialize.

  @retval EFI_SUCCESS     CPU init has been done successfully.

**/
EFI_STATUS
EFIAPI
CpuInit (
  IN UINT32     Index
  )
{
  UINT32  ApicId;

  ApicId = GetApicId();
  if (Index < PcdGet32 (PcdCpuMaxLogicalProcessorNumber)) {
    mSysCpuInfo.CpuInfo[Index].ApicId = ApicId;
  }

  if (PcdGetBool (PcdSmmRebaseEnabled)) {
    SmmRebase (Index, ApicId);
  }

  return EFI_SUCCESS;
}


/**
  AP initialization routine.

  @param[in]  Index             CPU index to initialize.
  @param[in]  mMpDataStructPtr   MP data structure pointer.

**/
VOID
ApFunc (
  UINT32                   Index,
  MP_DATA_EXCHANGE_STRUCT *mMpDataStructPtr
  )
{
  BOOLEAN            WaitTask;
  CPU_TASK_PROC      ApRunTask;
  volatile UINT32   *State;

  //
  // CPU specific init
  //
  CpuInit (Index);

  //
  // Signal AP completion
  //
  InterlockedIncrement (&mMpDataStructPtr->ApDoneCounter);

  //
  // Enter task loop
  //
  WaitTask = TRUE;
  State  = & (mSysCpuTask.CpuTask[Index].State);
  *State = EnumCpuReady;
  while (WaitTask) {
    while (*State == EnumCpuReady) {
      CpuPause ();
    }
    switch (*State) {
    case EnumCpuEnd:
      WaitTask = FALSE;
      break;

    case EnumCpuStart:
      *State = EnumCpuBusy;
      ApRunTask = (CPU_TASK_PROC)mSysCpuTask.CpuTask[Index].CProcedure;
      if (ApRunTask != NULL) {
        mSysCpuTask.CpuTask[Index].Result = ApRunTask (mSysCpuTask.CpuTask[Index].Argument);
      }
      *State = EnumCpuReady;
      break;

    default:
      break;
    }
  }

  AsmCliHlt();
}


/**
  Run a task function for a specific processor.

  @param[in]  Phase       Initialization phase for MP.

  @retval EFI_SUCCESS     MP init has been done successfully for current phase.

**/
EFI_STATUS
EFIAPI
MpInit (
  IN MP_INIT_PHASE  Phase
  )
{
  UINT8                    *ApBuffer;
  EFI_STATUS                Status;
  UINT8                     TimeOutCounter;
  UINT32                   *Ptr;
  AP_DATA_STRUCT           *ApDataPtr;
  volatile UINT32          *ApCounter;
  UINT32                    CpuCount;
  UINT32                    Index;
  EFI_PHYSICAL_ADDRESS      ApStackTop;

  Status   = EFI_SUCCESS;
  ApBuffer = (UINT8 *)AP_BUFFER_ADDRESS;

  if (Phase == EnumMpInitWakeup) {
    if (mMpInitPhase != EnumMpInitNull) {
      Status = EFI_UNSUPPORTED;
    } else {

      // Init structure for lock
      mMpDataStruct.SmmRebaseDoneCounter = 0;
      InitializeSpinLock (&mMpDataStruct.SpinLock);

      //
      // Allocate 1 K * 16 AP Stack, assume to support max 16 CPUs
      //
      ApStackTop = (EFI_PHYSICAL_ADDRESS) (UINTN)AllocatePages ( \
                   EFI_SIZE_TO_PAGES (PcdGet32 (PcdCpuMaxLogicalProcessorNumber) * AP_STACK_SIZE));
      ZeroMem (&mAddressMap, sizeof (mAddressMap));
      AsmGetAddressMap (&mAddressMap);

      //
      // Allocate backup Buffer for MP waking up
      // It is shared with initial SMBASE region.  So needs to cover at least 32KB.
      mBackupBuffer = AllocatePages (EFI_SIZE_TO_PAGES (AP_BUFFER_SIZE));
      CopyMem (mBackupBuffer, ApBuffer, AP_BUFFER_SIZE);

      //
      // Copy the Rendezvous routine to the memory buffer @ < 1 MB
      //
      CopyMem (ApBuffer, mAddressMap.RendezvousFunnelAddress, mAddressMap.CodeSize);
      ApDataPtr = (AP_DATA_STRUCT *) (ApBuffer + mAddressMap.CodeSize);

      //
      // Patch the GDTR, IDTR and the Segment Selector variables
      //
      AsmGetBspSelectors (& (ApDataPtr->BspSelector));
      AsmReadGdtr ((IA32_DESCRIPTOR *)&ApDataPtr->Gdtr);
      AsmReadIdtr ((IA32_DESCRIPTOR *)&ApDataPtr->Idtr);

      //
      // Patch the Segment/Offset for the far Jump into PMODE code
      //
      Ptr = (UINT32 *) (ApBuffer + mAddressMap.ProtModeJmpPatchOffset);
      Ptr = (UINT32 *) (((UINT8 *) Ptr) + 3);
      *Ptr = ((UINT32)ApBuffer) + (UINT32)mAddressMap.ProtModeStartOffset;
      Ptr = (UINT32 *) (((UINT8 *) Ptr) + 4);
      * ((UINT16 *)Ptr) = ApDataPtr->BspSelector.CSSelector;

      //
      // Patch the BufferStart variable with the address of the buffer
      //
      ApDataPtr->BufferStart = (UINT32)ApBuffer;


      //
      // Patch the Stack size and base
      //
      ApDataPtr->ApStackSize  = AP_STACK_SIZE_SHIFT_BITS;
      ApDataPtr->StackStart   = (UINT32)ApStackTop;

      //
      // Patch the C Procedure variable
      //
      ApDataPtr->CProcedure   = (UINT32)ApFunc;

      //
      // Patch the mMpDataStructure Pointer variable
      //
      ApDataPtr->MpDataStruct = (UINT32)&mMpDataStruct;

      //
      // Initialize the SpinLock
      //
      ApDataPtr->SpinLock = 0;

      //
      // Initialize the ApCounter
      //
      ApDataPtr->ApCounter = 0;

      //
      // Initialize the mMpDataStructure
      //
      mMpDataStruct.ApDoneCounter    = 0;

      //
      // Send Init-SIPI-SIPI to all APs and wait for completion
      // It includes a 200us delay for AP's check-in
      // If it is not long enough, extra delay can be added after this call
      //
      SendInitSipiSipiAllExcludingSelf ((UINT32) ApBuffer);

      CpuInit (0);

      mMpInitPhase = EnumMpInitWakeup;
    }
  }

  if (Phase == EnumMpInitRun) {
    if (mMpInitPhase != EnumMpInitWakeup) {
      Status = EFI_UNSUPPORTED;
    } else {

      //
      // Wait for task done
      //
      ApDataPtr = (AP_DATA_STRUCT *) (ApBuffer + mAddressMap.CodeSize);
      ApCounter = (volatile UINT32 *)&ApDataPtr->ApCounter;
      TimeOutCounter = 0;
      while (TimeOutCounter < AP_TASK_TIMEOUT_CNT) {
        if (mMpDataStruct.ApDoneCounter == *ApCounter) {
          break;
        }
        MicroSecondDelay (AP_TASK_TIMEOUT_UNIT);
        TimeOutCounter++ ;
      }

      CpuCount = (*ApCounter) + 1;
      DEBUG ((DEBUG_INFO, "Detected %d CPU threads\n", CpuCount));
      if (TimeOutCounter == AP_TASK_TIMEOUT_CNT) {
        DEBUG ((DEBUG_INFO, "MPINIT timeout with %d APs completed.\n", mMpDataStruct.ApDoneCounter));
      }

      mSysCpuInfo.CpuCount = CpuCount;
      mSysCpuTask.CpuCount = CpuCount;
      for (Index = 0; Index < CpuCount; Index++) {
        DEBUG ((DEBUG_INFO, " CPU %2d APIC ID: %d\n", Index, mSysCpuInfo.CpuInfo[Index].ApicId));
      }

      if (PcdGetBool (PcdSmmRebaseEnabled)) {
        // Check SMM rebase result
        if (mMpDataStruct.SmmRebaseDoneCounter != CpuCount) {
          CpuHalt ("CPU SMM rebase failed!\n");
        }
      }

      //
      // Restore AP buffer (needed for S3)
      //
      CopyMem (ApBuffer, mBackupBuffer, AP_BUFFER_SIZE);

      mMpInitPhase = EnumMpInitRun;
    }
  }

  if (Phase == EnumMpInitDone) {
    if (mMpInitPhase != EnumMpInitRun) {
      Status = EFI_UNSUPPORTED;
    } else {
      //
      // All APs should be in EnumCpuReady now
      //
      for (Index = 1; Index < mSysCpuTask.CpuCount; Index++) {
        if (mSysCpuTask.CpuTask[Index].State != EnumCpuReady) {
          DEBUG ((DEBUG_ERROR, " CPU %2d is not ready yet! State = %d\n", Index,
                  mSysCpuInfo.CpuInfo[Index].ApicId, mSysCpuTask.CpuTask[Index].State));
        }
      }

      //
      // Send an Init IPI to all the APs to put them back in WFS state
      //
      SendInitIpiAllExcludingSelf();

      mMpInitPhase = EnumMpInitDone;
    }
  }

  return Status;

}


/**
  Get processor info for all CPUs.

  @retval    Pointer to SYS_CPU_INFO structure.

**/
SYS_CPU_INFO *
EFIAPI
MpGetInfo (
  VOID
  )
{
  return (SYS_CPU_INFO *)&mSysCpuInfo;
}


/**
  Get processor task state for all CPUs.

  @retval    Pointer to SYS_CPU_TASK structure containing task info.

**/
SYS_CPU_TASK *
EFIAPI
MpGetTask (
  VOID
  )
{
  return (SYS_CPU_TASK *)&mSysCpuTask;
}


/**
  Run a task function for a specific processor.

  @param[in]  Index       CPU index
  @param[in]  TaskProc    Task function pointer
  @param[in]  Argument    Argument for the task function

  @retval EFI_INVALID_PARAMETER   Invalid Index parameter.
  @retval EFI_NOT_READY           CPU state is not ready for new task yet.
  @retval EFI_SUCCESS             CPU accepted the new task successfully.

**/
EFI_STATUS
EFIAPI
MpRunTask (
  IN  UINT32         Index,
  IN  CPU_TASK_PROC  TaskProc,
  IN  UINT32         Argument
  )
{
  if ((Index >= mSysCpuTask.CpuCount) || (Index == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSysCpuTask.CpuTask[Index].State != EnumCpuReady) {
    return EFI_NOT_READY;
  }

  mSysCpuTask.CpuTask[Index].CProcedure = (UINT32)TaskProc;
  mSysCpuTask.CpuTask[Index].Argument   = (UINT32)Argument;
  mSysCpuTask.CpuTask[Index].State      = EnumCpuStart;

  return EFI_SUCCESS;
}


/**
  Dump MP task running state

  This is a debug function to dump current task running state for each
  processor.


**/
VOID
EFIAPI
MpDumpTask (
  VOID
  )
{
  UINT32 Index;

  for (Index = 0; Index < mSysCpuTask.CpuCount; Index++) {
    DEBUG ((DEBUG_INFO, "CPU%02X: %08X %08X %08X %08X\n",
            Index,
            mSysCpuTask.CpuTask[Index].State,
            mSysCpuTask.CpuTask[Index].CProcedure,
            mSysCpuTask.CpuTask[Index].Argument,
            mSysCpuTask.CpuTask[Index].Result));
  }
  DEBUG ((DEBUG_INFO, "\n"));
}
