/** @file
  MP init library implementation.

  Copyright (c) 2015 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <MpInitLibInternal.h>

STATIC UINT32                             mStubCodeSize;
STATIC ALL_CPU_INFO                       mSysCpuInfo;
STATIC volatile ALL_CPU_TASK              mSysCpuTask;
STATIC volatile MP_DATA_EXCHANGE_STRUCT   mMpDataStruct;
STATIC UINT8                             *mBackupBuffer;
STATIC UINT32                             mMpInitPhase = EnumMpInitNull;
STATIC SMMBASE_INFO                      *mSmmBaseInfo;
STATIC MTRR_SETTINGS                      mMtrrTable;
extern UINT8                             *mDefaultSmiHandlerStart;
extern UINT8                             *mDefaultSmiHandlerRet;
extern UINT8                             *mDefaultSmiHandlerEnd;

extern VOID                               RendezvousFunnelProcStart(VOID);
extern VOID                               RendezvousFunnelProcEnd(VOID);

/**
  The CPU task function to program MTRRs.

  @param[in] Arg  Task parameter.

  @retval  0           MTRRs were set successfully.
           MAX_UINT64  Failed to set MTRRs
**/
UINT64
EFIAPI
SetCpuMtrrsTask (
  IN  UINT64   Arg
  )
{
  EFI_STATUS       Status;
  MTRR_SETTINGS   *MtrrSettings;

  MtrrSettings = (MTRR_SETTINGS *)(UINTN)Arg;
  Status = SetCpuMtrrs (MtrrSettings);
  if (!EFI_ERROR(Status)) {
    return 0;
  } else {
    return MAX_UINT64;
  }
}

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
  Sort the CPU entry

  It sort the CPU by the PCD PcdCpuSortMethod. Sort CPU by APIC ID in ascending order could make the debug easy.
  Sort CPU by the APIC ID in descending order might be required by some special case.
  Sort CPU by their thread distances could help fully utilize unshared CPU resources.

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

  // Sort the CPU by APIC ID
  PerformQuickSort (SysCpuInfo->CpuInfo, SysCpuInfo->CpuCount, sizeof (CPU_INFO), CompareCpuApicId, &Temp);

  if (FixedPcdGet32 (PcdCpuSortMethod) == 2) {
    // Sort the CPU by CPU APIC ID in descending order
    for (Idx1 = 0; Idx1 < SysCpuInfo->CpuCount / 2; Idx1++) {
      CopyMem (&Temp, &SysCpuInfo->CpuInfo[Idx1], sizeof(CPU_INFO));
      CopyMem (&SysCpuInfo->CpuInfo[Idx1], &SysCpuInfo->CpuInfo[SysCpuInfo->CpuCount - Idx1 - 1], sizeof(CPU_INFO));
      CopyMem (&SysCpuInfo->CpuInfo[SysCpuInfo->CpuCount - Idx1 - 1], &Temp, sizeof(CPU_INFO));
    }
  }

  if (FixedPcdGet32 (PcdCpuSortMethod) == 0) {
    // Sort the CPU according to their thread distances

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
  MSR_IA32_MTRRCAP_REGISTER MtrrCap;
  UINT8                    *SmmEntry;
  UINT8                    *DefEntry;

  if (SmmBase == 0) {
    SmmBase  = PcdGet32 (PcdSmramTsegBase) + PcdGet32 (PcdSmramTsegSize) - (SMM_BASE_MIN_SIZE + Index * SMM_BASE_GAP);
    SmmEntry = (UINT8 *)(UINTN)SmmBase + 0x8000;
    // Write default handler at new SMM handler entry
    MtrrCap.Uint64 = AsmReadMsr64 (MSR_IA32_MTRRCAP);
    if (MtrrCap.Bits.SMRR == 0) {
      // No SMRR support, fill "RSM" at the entry instead.
      DefEntry = (UINT8 *)&mDefaultSmiHandlerRet;
    } else {
      DefEntry = (UINT8 *)&mDefaultSmiHandlerStart;
    }
    CopyMem (SmmEntry, DefEntry, (UINT8 *)&mDefaultSmiHandlerEnd - DefEntry);
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
  CPU_SMMBASE            *CpuSmmBase;
  UINT32                  ApicId;
  UINT32                  CpuIdx;
  UINT32                  CpuCount;
  PLATFORM_CPU_INIT_HOOK  PlatformCpuInitHook;
  PLD_TO_BL_SMM_INFO      *PldToBlSmmInfo;

  if (FeaturePcdGet (PcdCpuX2ApicEnabled)) {
    // Enable X2APIC if desired
    SetApicMode (LOCAL_APIC_MODE_X2APIC);
    if (Index == 0) {
      DEBUG ((DEBUG_INFO, "APIC Mode: %d\n", GetApicMode ()));
    }
  }

  ApicId = GetApicId();
  if (Index < PcdGet32 (PcdCpuMaxLogicalProcessorNumber)) {
    mSysCpuInfo.CpuInfo[Index].ApicId = ApicId;
  }

  CpuCount = 0;
  if ((GetBootMode() == BOOT_ON_S3_RESUME) && (mSmmBaseInfo != NULL)) {
    // Perform SMM rebasing on S3 if payload provides SMM base info
    if ((PcdGet8(PcdBuildSmmHobs) & BIT1) != 0) {
      PldToBlSmmInfo = (PLD_TO_BL_SMM_INFO *) mSmmBaseInfo;
      if (CompareGuid(&PldToBlSmmInfo->Header.Name, &gPldS3CommunicationGuid)) {
        CpuSmmBase = PldToBlSmmInfo->S3Info.SmmBase;
        CpuCount   = PldToBlSmmInfo->S3Info.CpuCount;
      }
    }

    if ((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) {
      if ((mSmmBaseInfo->SmmBaseHdr.Signature == BL_PLD_COMM_SIG) && (CpuCount == 0)) {
        CpuSmmBase = mSmmBaseInfo->SmmBase;
        CpuCount   = mSmmBaseInfo->SmmBaseHdr.Count;
      }
    }

    for (CpuIdx = 0; CpuIdx < CpuCount; CpuIdx++) {
      if (ApicId == CpuSmmBase[CpuIdx].ApicId) {
        SmmRebase (Index, ApicId, CpuSmmBase[CpuIdx].SmmBase);
        break;
      }
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
  CPU_TASK_FUNC      ApRunTask;
  volatile UINT8     *State;

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
      ApRunTask = (CPU_TASK_FUNC)(UINTN)mSysCpuTask.CpuTask[Index].TaskFunc;
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


  if ((PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE) || (mMpDataStruct.SmmRebaseDoneCounter > 0)) {
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
  AP_DATA_STRUCT           *ApDataPtr;
  volatile UINT32          *ApCounter;
  UINT32                    CpuCount;
  UINT32                    Index;
  EFI_PHYSICAL_ADDRESS      ApStackTop;
  MSR_IA32_MTRRCAP_REGISTER MtrrCap;
  UINT32                    SmrrBase;
  UINT32                    SmrrSize;
  MSR_IA32_MTRR_PHYSMASK_REGISTER SmrrMask;

  Status   = EFI_SUCCESS;
  ApBuffer = (UINT8 *)AP_BUFFER_ADDRESS;

  if (Phase == EnumMpInitWakeup) {
    if (mMpInitPhase != EnumMpInitNull) {
      Status = EFI_UNSUPPORTED;
    } else {
      DEBUG ((DEBUG_INIT, "MP Init%a\n", DebugCodeEnabled() ? " (Wakeup)" : ""));

      // Init structure for lock
      mMpDataStruct.SmmRebaseDoneCounter = 0;
      InitializeSpinLock (&mMpDataStruct.SpinLock);

      //
      // Allocate 1 K * 16 AP Stack, assume to support max 16 CPUs
      //
      ApStackTop = (EFI_PHYSICAL_ADDRESS) (UINTN)AllocatePages ( \
                   EFI_SIZE_TO_PAGES (PcdGet32 (PcdCpuMaxLogicalProcessorNumber) * AP_STACK_SIZE));

      //
      // Allocate backup Buffer for MP waking up
      // It is shared with initial SMBASE region.  So needs to cover at least 32KB.
      mBackupBuffer = AllocatePages (EFI_SIZE_TO_PAGES (AP_BUFFER_SIZE));
      CopyMem (mBackupBuffer, ApBuffer, AP_BUFFER_SIZE);

      //
      // Copy the Rendezvous routine to the memory buffer @ < 1 MB
      //
      mStubCodeSize = (UINT32)((UINT8 *)(UINTN)RendezvousFunnelProcEnd - (UINT8 *)(UINTN)RendezvousFunnelProcStart);
      CopyMem (ApBuffer, (UINT8 *)(UINTN)RendezvousFunnelProcStart, mStubCodeSize);
      ApDataPtr = (AP_DATA_STRUCT *) (ApBuffer + mStubCodeSize);
      ZeroMem (ApDataPtr, sizeof(AP_DATA_STRUCT));
      ApDataPtr->CpuArch = IS_X64;

      //
      // Patch the GDTR, IDTR and the Segment Selector variables
      //
      AsmGetBspSelectors (& (ApDataPtr->BspSelector));
      AsmReadGdtr ((IA32_DESCRIPTOR *)&ApDataPtr->Gdtr);
      AsmReadIdtr ((IA32_DESCRIPTOR *)&ApDataPtr->Idtr);
      ApDataPtr->Cr3 = (UINT32)AsmReadCr3 ();

      // Fill SMRR base and size
      SmrrBase = PcdGet32 (PcdSmramTsegBase);
      SmrrSize = PcdGet32 (PcdSmramTsegSize);
      if (SmrrSize > 0) {
        if ((SmrrSize < SIZE_4KB) ||  (SmrrSize != GetPowerOfTwo32 (SmrrSize)) || ((SmrrBase & ~(SmrrSize - 1)) != SmrrBase)) {
          DEBUG ((DEBUG_INFO, "Invalid SMRR base or size\n"));
        } else {
          MtrrCap.Uint64 = AsmReadMsr64 (MSR_IA32_MTRRCAP);
          if (MtrrCap.Bits.SMRR == 1) {
            // Don't enable SMRR valid yet, it will be done at end of stage2
            SmrrMask.Uint64 = (UINT32)(~(SmrrSize - 1));
            SmrrMask.Bits.V = 0;
            ApDataPtr->SmrrBase = SmrrBase | MSR_IA32_VMX_BASIC_REGISTER_MEMORY_TYPE_WRITE_BACK;
            ApDataPtr->SmrrMask = (UINT32)SmrrMask.Uint64;
            DEBUG ((DEBUG_INFO, "SMRR Base: 0x%08x  Mask: 0x%08x\n", ApDataPtr->SmrrBase, ApDataPtr->SmrrMask));
          }
        }
      }

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
      DEBUG ((DEBUG_INFO, "MP Init (Run)\n"));

      // Wait for task done
      ApDataPtr = (AP_DATA_STRUCT *) (ApBuffer + mStubCodeSize);
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

      if (CpuCount > FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)) {
        CpuCount =  FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber);
        DEBUG ((DEBUG_WARN, "PcdCpuMaxLogicalProcessorNumber is too small !\n", mMpDataStruct.ApDoneCounter));
      }

      mSysCpuInfo.CpuCount = CpuCount;
      mSysCpuTask.CpuCount = CpuCount;
      SortSysCpu (&mSysCpuInfo);

      for (Index = 0; Index < CpuCount; Index++) {
        DEBUG ((DEBUG_INFO, " CPU %2d APIC ID: %d\n", Index, mSysCpuInfo.CpuInfo[Index].ApicId));
      }

      if ((PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE) || (mMpDataStruct.SmmRebaseDoneCounter > 0)) {
        // Check SMM rebase result
        if (mMpDataStruct.SmmRebaseDoneCounter != CpuCount) {
          CpuHalt ("CPU SMM rebase failed!\n");
        } else {
          DEBUG ((DEBUG_INFO, "SMM rebase done on %d CPUs\n", mMpDataStruct.SmmRebaseDoneCounter));
        }
      }

      // Restore AP buffer (needed for S3)
      CopyMem (ApBuffer, mBackupBuffer, AP_BUFFER_SIZE);
      mMpInitPhase = EnumMpInitRun;
    }
  }

  if (Phase == EnumMpInitDone) {
    if (mMpInitPhase == EnumMpInitDone) {
      Status = EFI_UNSUPPORTED;
    } else if (mMpInitPhase != EnumMpInitRun) {
      Status = EFI_UNSUPPORTED;
    } else {
      DEBUG ((DEBUG_INFO, "MP Init (Done)\n"));

      // All APs should be in EnumCpuReady now
      Status = GetCpuMtrrs (&mMtrrTable);
      if (!EFI_ERROR(Status)) {
        for (Index = 1; Index < mSysCpuTask.CpuCount; Index++) {
          if (mSysCpuTask.CpuTask[Index].State == EnumCpuReady) {
            MpRunTask (Index, SetCpuMtrrsTask, (UINT64)(UINTN)&mMtrrTable);
          }
        }
        // Allow MTRR sync to complete
        MicroSecondDelay (100);
      }

      for (Index = 1; Index < mSysCpuTask.CpuCount; Index++) {
        if (mSysCpuTask.CpuTask[Index].State != EnumCpuReady) {
          DEBUG ((DEBUG_ERROR, " CPU %2d is not ready yet! State = %d\n", Index,
                  mSysCpuInfo.CpuInfo[Index].ApicId, mSysCpuTask.CpuTask[Index].State));
        }
      }

      // Send an Init IPI to all the APs to put them back in WFS state
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
  @param[in]  TaskFunc    Task function pointer
  @param[in]  Argument    Argument for the task function

  @retval EFI_INVALID_PARAMETER   Invalid Index parameter.
  @retval EFI_NOT_READY           CPU state is not ready for new task yet.
  @retval EFI_SUCCESS             CPU accepted the new task successfully.

**/
EFI_STATUS
EFIAPI
MpRunTask (
  IN  UINT32         Index,
  IN  CPU_TASK_FUNC  TaskFunc,
  IN  UINT64         Argument
  )
{
  if ((Index >= mSysCpuTask.CpuCount) || (Index == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSysCpuTask.CpuTask[Index].State != EnumCpuReady) {
    return EFI_NOT_READY;
  }

  mSysCpuTask.CpuTask[Index].TaskFunc = (UINT64)(UINTN)TaskFunc;
  mSysCpuTask.CpuTask[Index].Argument   = Argument;
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
    DEBUG ((DEBUG_INFO, "CPU%02X: %02X %010lX %010lX %010lX\n",
            Index,
            mSysCpuTask.CpuTask[Index].State,
            mSysCpuTask.CpuTask[Index].TaskFunc,
            mSysCpuTask.CpuTask[Index].Argument,
            mSysCpuTask.CpuTask[Index].Result));
  }
  DEBUG ((DEBUG_INFO, "\n"));
}
