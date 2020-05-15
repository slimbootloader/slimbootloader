/** @file
  MP init library implementation.

  Copyright (c) 2015 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <MpInitLibInternal.h>

STATIC MP_ASSEMBLY_ADDRESS_MAP            mAddressMap;
STATIC ALL_CPU_INFO                       mSysCpuInfo;
STATIC volatile ALL_CPU_TASK              mSysCpuTask;
STATIC volatile MP_DATA_EXCHANGE_STRUCT   mMpDataStruct;
STATIC UINT8                             *mBackupBuffer;
STATIC UINT32                             mMpInitPhase = EnumMpInitNull;
STATIC SMMBASE_INFO                      *mSmmBaseInfo = NULL;

/**
  The function is called by PerformQuickSort to sort CPU_INFO by ApicId.

  @param[in] Buffer1         The pointer to first buffer.
  @param[in] Buffer2         The pointer to second buffer.

  @retval 0                  Buffer1 ApicId is less than Buffer2 ApicId.
  @retval 1                  Buffer1 ApicId is greater than or equal to Buffer2 ApicId.

**/
STATIC
INTN
EFIAPI
CompareCpuApicId (
  IN CONST VOID                 *Buffer1,
  IN CONST VOID                 *Buffer2
  )
{
  if (((CPU_INFO *)Buffer1)->ApicId < ((CPU_INFO *)Buffer2)->ApicId) {
    return  0;
  } else {
    return  1;
  }
}

/**
  Sort the CPU entry according to their thread distances

  It is required to list CPU thread with further distance first so as to
  fully utilize unshared CPU resources.

  @param[in]  SysCpuInfo     Pointer to the ALL_CPU_INFO structure.

**/
STATIC
VOID
SortSysCpu (
  IN  ALL_CPU_INFO      *SysCpuInfo
)
{
  UINT32         Idx1;
  UINT32         Idx2;
  UINT32         Step;
  ALL_CPU_INFO   OldSysCpuInfo;
  CPU_INFO       Temp;

  if (SysCpuInfo->CpuCount <= 1) {
    return;
  }

  // Sort by APIC ID first
  PerformQuickSort (SysCpuInfo->CpuInfo, SysCpuInfo->CpuCount, sizeof (CPU_INFO), CompareCpuApicId, &Temp);

  // Keep a backup copy
  CopyMem (&OldSysCpuInfo,  SysCpuInfo, sizeof (ALL_CPU_INFO));

  // Rearrange order per thread distance
  Idx2 = 0;
  Step = GetPowerOfTwo32 (SysCpuInfo->CpuCount);
  for (; Step > 0; Step >>= 1) {
    for (Idx1 = 0; Idx1 < SysCpuInfo->CpuCount; Idx1 += Step) {
      if ((OldSysCpuInfo.CpuInfo[Idx1].ApicId != 0xFFFFFFFF) && (Idx2 < SysCpuInfo->CpuCount)) {
        // Fill the CPU_INFO and mark it as consumed
        CopyMem (&SysCpuInfo->CpuInfo[Idx2], &OldSysCpuInfo.CpuInfo[Idx1], sizeof(CPU_INFO));
        OldSysCpuInfo.CpuInfo[Idx1].ApicId = 0xFFFFFFFF;
        Idx2++;
      }
    }
  }
}

/**
  Relocate SMM base for CPU

  @param[in]  Index       CPU index to rebase.
  @param[in]  ApicId      CPU APIC ID.
  @param[in]  SmmBase     SMBASE for the current proc (Index)

**/
VOID
SmmRebase (
  IN UINT32  Index,
  IN UINT32  ApicId,
  IN UINT32  SmmBase
)
{
  if (SmmBase == 0) {
    SmmBase = PcdGet32 (PcdSmramTsegBase) + PcdGet32 (PcdSmramTsegSize) - (SMM_BASE_MIN_SIZE + Index * SMM_BASE_GAP);
    // Write 'RSM' at new SMM handler entry
    *(UINT32 *)(UINTN)(SmmBase + 0x8000) = RSM_SIG;
  }

  AsmWriteCr2 (SmmBase);
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
  IN UINT32               Index
  )
{
  SMMBASE_INFO            *SmmBaseInfo;
  UINT32                  ApicId;
  UINT32                  CpuIdx;
  PLATFORM_CPU_INIT_HOOK  PlatformCpuInitHook;

  ApicId = GetApicId();
  if (Index < PcdGet32 (PcdCpuMaxLogicalProcessorNumber)) {
    mSysCpuInfo.CpuInfo[Index].ApicId = ApicId;
  }

  if (PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE_ON_S3_RESUME_ONLY) {
    if ((GetBootMode() == BOOT_ON_S3_RESUME) && (mSmmBaseInfo != NULL)) {
      SmmBaseInfo = mSmmBaseInfo;
      for (CpuIdx = 0; CpuIdx < SmmBaseInfo->SmmBaseHdr.Count; CpuIdx++) {
        if (ApicId == SmmBaseInfo->SmmBase[CpuIdx].ApicId) {
          SmmRebase (Index, ApicId, SmmBaseInfo->SmmBase[CpuIdx].SmmBase);
          break;
        }
      }
      ASSERT (CpuIdx < SmmBaseInfo->SmmBaseHdr.Count);
    }
  } else if (PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE) {
    SmmRebase (Index, ApicId, 0);
  }

  PlatformCpuInitHook = (PLATFORM_CPU_INIT_HOOK)(UINTN)PcdGet32 (PcdFuncCpuInitHook);
  if (PlatformCpuInitHook != NULL) {
    PlatformCpuInitHook (Index);
  }

  return EFI_SUCCESS;
}


/**
  AP initialization routine.

  @param[in]  Index             CPU index to initialize.
  @param[in]  mMpDataStructPtr   MP data structure pointer.

**/
VOID
EFIAPI
ApFunc (
  UINT32                   Index,
  MP_DATA_EXCHANGE_STRUCT *mMpDataStructPtr
  )
{
  BOOLEAN            WaitTask;
  CPU_TASK_PROC      ApRunTask;
  volatile UINT32   *State;

  // Enable more CPU featurs
  AsmEnableAvx ();

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
      ApRunTask = (CPU_TASK_PROC)(UINTN)mSysCpuTask.CpuTask[Index].CProcedure;
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
  BSP initialization routine.

**/
VOID
BspInit (
  VOID
  )
{

  mSysCpuInfo.CpuCount = 1;
  mSysCpuTask.CpuCount = 1;

  mMpDataStruct.SmmRebaseDoneCounter = 0;

  //
  // CPU specific init
  //
  CpuInit (0);
  DEBUG ((DEBUG_INFO, " BSP APIC ID: %d\n", mSysCpuInfo.CpuInfo[0].ApicId));


  if (PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE) {
    // Check SMM rebase result
    if (mMpDataStruct.SmmRebaseDoneCounter != 1) {
      CpuHalt ("CPU SMM rebase failed!\n");
    }
  }

}


/**
  Multiprocessor Initialization.

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
  UINT32                    TimeOutCounter;
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
      ApDataPtr->Cr3 = (UINT32)AsmReadCr3 ();

      //
      // Patch the Segment/Offset for the far Jump into PMODE code
      //
      Ptr  = (UINT32 *)(ApBuffer + mAddressMap.ProtModeJmpPatchOffset + 3);
      *Ptr = (UINT32)(UINTN)ApBuffer + mAddressMap.ProtModeStartOffset;

      //
      // Patch the BufferStart variable with the address of the buffer
      //
      ApDataPtr->BufferStart = (UINT32)(UINTN)ApBuffer;

      //
      // Patch the Stack size and base
      //
      ApDataPtr->ApStackSize  = AP_STACK_SIZE_SHIFT_BITS;
      ApDataPtr->StackStart   = (UINT32)ApStackTop;

      //
      // Patch the C Procedure variable
      //
      ApDataPtr->CProcedure   = (UINT32)(UINTN)ApFunc;

      //
      // Patch the mMpDataStructure Pointer variable
      //
      ApDataPtr->MpDataStruct = (UINT32)(UINTN)&mMpDataStruct;

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
      // Get SMM Base Info for S3 resume
      //
      if (GetBootMode() == BOOT_ON_S3_RESUME) {
        mSmmBaseInfo = (SMMBASE_INFO *) FindS3Info (SMMBASE_INFO_COMM_ID);
      } else {
        mSmmBaseInfo = NULL;
      }

      //
      // Send Init-SIPI-SIPI to all APs and wait for completion
      // It includes a 200us delay for AP's check-in
      // If it is not long enough, extra delay can be added after this call
      //
      SendInitSipiSipiAllExcludingSelf ((UINT32)(UINTN)ApBuffer);

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
      SortSysCpu (&mSysCpuInfo);

      for (Index = 0; Index < CpuCount; Index++) {
        DEBUG ((DEBUG_INFO, " CPU %2d APIC ID: %d\n", Index, mSysCpuInfo.CpuInfo[Index].ApicId));
      }

      if (PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE) {
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

  mSysCpuTask.CpuTask[Index].CProcedure = (UINT32)(UINTN)TaskProc;
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
