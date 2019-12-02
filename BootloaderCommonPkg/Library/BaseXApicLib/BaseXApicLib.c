/** @file
  Local APIC Library.

  This local APIC library instance supports xAPIC mode only.

  Copyright (c) 2010 - 2015, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/LocalApicLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/LocalApic.h>

//
// Library internal functions
//
/**
  Retrieve the base address of local APIC.

  @return The base address of local APIC.

**/
UINTN
EFIAPI
GetLocalApicBaseAddress (
  VOID
  )
{
  MSR_IA32_APIC_BASE ApicBaseMsr;

  ApicBaseMsr.Uint64 = AsmReadMsr64 (MSR_IA32_APIC_BASE_ADDRESS);

  return (UINTN) (LShiftU64 ((UINT64) ApicBaseMsr.Bits.ApicBaseHigh, 32)) +
         (((UINTN)ApicBaseMsr.Bits.ApicBaseLow) << 12);
}

/**
  Set the base address of local APIC.

  If BaseAddress is not aligned on a 4KB boundary, then ASSERT().

  @param[in] BaseAddress   Local APIC base address to be set.

**/
VOID
EFIAPI
SetLocalApicBaseAddress (
  IN UINTN                BaseAddress
  )
{
  MSR_IA32_APIC_BASE ApicBaseMsr;

  ASSERT ((BaseAddress & (SIZE_4KB - 1)) == 0);

  ApicBaseMsr.Uint64 = AsmReadMsr64 (MSR_IA32_APIC_BASE_ADDRESS);

  ApicBaseMsr.Bits.ApicBaseLow  = (UINT32) (BaseAddress >> 12);
  ApicBaseMsr.Bits.ApicBaseHigh = (UINT32) (RShiftU64 ((UINT64) BaseAddress, 32));

  AsmWriteMsr64 (MSR_IA32_APIC_BASE_ADDRESS, ApicBaseMsr.Uint64);
}

/**
  Read from a local APIC register.

  This function reads from a local APIC register either in xAPIC or x2APIC mode.
  It is required that in xAPIC mode wider registers (64-bit or 256-bit) must be
  accessed using multiple 32-bit loads or stores, so this function only performs
  32-bit read.

  @param  MmioOffset  The MMIO offset of the local APIC register in xAPIC mode.
                      It must be 16-byte aligned.

  @return 32-bit      Value read from the register.
**/
UINT32
EFIAPI
ReadLocalApicReg (
  IN UINTN  MmioOffset
  )
{
  ASSERT ((MmioOffset & 0xf) == 0);
  ASSERT (GetApicMode () == LOCAL_APIC_MODE_XAPIC);

  return MmioRead32 (GetLocalApicBaseAddress() + MmioOffset);
}

/**
  Write to a local APIC register.

  This function writes to a local APIC register either in xAPIC or x2APIC mode.
  It is required that in xAPIC mode wider registers (64-bit or 256-bit) must be
  accessed using multiple 32-bit loads or stores, so this function only performs
  32-bit write.

  if the register index is invalid or unsupported in current APIC mode, then ASSERT.

  @param  MmioOffset  The MMIO offset of the local APIC register in xAPIC mode.
                      It must be 16-byte aligned.
  @param  Value       Value to be written to the register.
**/
VOID
EFIAPI
WriteLocalApicReg (
  IN UINTN  MmioOffset,
  IN UINT32 Value
  )
{
  ASSERT ((MmioOffset & 0xf) == 0);
  ASSERT (GetApicMode () == LOCAL_APIC_MODE_XAPIC);

  MmioWrite32 (GetLocalApicBaseAddress() + MmioOffset, Value);
}

/**
  Send an IPI by writing to ICR.

  This function returns after the IPI has been accepted by the target processor.

  @param  IcrLow 32-bit value to be written to the low half of ICR.
  @param  ApicId APIC ID of the target processor if this IPI is targeted for a specific processor.
**/
VOID
SendIpi (
  IN UINT32          IcrLow,
  IN UINT32          ApicId
  )
{
  LOCAL_APIC_ICR_LOW IcrLowReg;
  UINT32             IcrHigh;
  BOOLEAN            InterruptState;

  ASSERT (GetApicMode () == LOCAL_APIC_MODE_XAPIC);
  ASSERT (ApicId <= 0xff);

  InterruptState = SaveAndDisableInterrupts ();

  //
  // Save existing contents of ICR high 32 bits
  //
  IcrHigh = ReadLocalApicReg (XAPIC_ICR_HIGH_OFFSET);

  //
  // Wait for DeliveryStatus clear in case a previous IPI
  //  is still being sent
  //
  do {
    IcrLowReg.Uint32 = ReadLocalApicReg (XAPIC_ICR_LOW_OFFSET);
  } while (IcrLowReg.Bits.DeliveryStatus != 0);

  //
  // For xAPIC, the act of writing to the low doubleword of the ICR causes the IPI to be sent.
  //
  WriteLocalApicReg (XAPIC_ICR_HIGH_OFFSET, ApicId << 24);
  WriteLocalApicReg (XAPIC_ICR_LOW_OFFSET, IcrLow);

  //
  // Wait for DeliveryStatus clear again
  //
  do {
    IcrLowReg.Uint32 = ReadLocalApicReg (XAPIC_ICR_LOW_OFFSET);
  } while (IcrLowReg.Bits.DeliveryStatus != 0);

  //
  // And restore old contents of ICR high
  //
  WriteLocalApicReg (XAPIC_ICR_HIGH_OFFSET, IcrHigh);

  SetInterruptState (InterruptState);

}

//
// Library API implementation functions
//

/**
  Get the current local APIC mode.

  If local APIC is disabled, then ASSERT.

  @retval LOCAL_APIC_MODE_XAPIC  current APIC mode is xAPIC.
  @retval LOCAL_APIC_MODE_X2APIC current APIC mode is x2APIC.
**/
UINTN
EFIAPI
GetApicMode (
  VOID
  )
{
  DEBUG_CODE ( {
    MSR_IA32_APIC_BASE ApicBaseMsr;

    ApicBaseMsr.Uint64 = AsmReadMsr64 (MSR_IA32_APIC_BASE_ADDRESS);
    //
    // Local APIC should have been enabled
    //
    ASSERT (ApicBaseMsr.Bits.En != 0);
    ASSERT (ApicBaseMsr.Bits.Extd == 0);
  }
  );
  return LOCAL_APIC_MODE_XAPIC;
}

/**
  Set the current local APIC mode.

  If the specified local APIC mode is not valid, then ASSERT.
  If the specified local APIC mode can't be set as current, then ASSERT.

  @param ApicMode APIC mode to be set.

  @note  This API must not be called from an interrupt handler or SMI handler.
         It may result in unpredictable behavior.
**/
VOID
EFIAPI
SetApicMode (
  IN UINTN  ApicMode
  )
{
  ASSERT (ApicMode == LOCAL_APIC_MODE_XAPIC);
  ASSERT (GetApicMode () == LOCAL_APIC_MODE_XAPIC);
}

/**
  Get the initial local APIC ID of the executing processor assigned by hardware upon power on or reset.

  In xAPIC mode, the initial local APIC ID may be different from current APIC ID.
  In x2APIC mode, the local APIC ID can't be changed and there is no concept of initial APIC ID. In this case,
  the 32-bit local APIC ID is returned as initial APIC ID.

  @return  32-bit initial local APIC ID of the executing processor.
**/
UINT32
EFIAPI
GetInitialApicId (
  VOID
  )
{
  UINT32 ApicId;
  UINT32 MaxCpuIdIndex;
  UINT32 RegEbx;

  ASSERT (GetApicMode () == LOCAL_APIC_MODE_XAPIC);

  //
  // Get the max index of basic CPUID
  //
  AsmCpuid (CPUID_SIGNATURE, &MaxCpuIdIndex, NULL, NULL, NULL);

  //
  // If CPUID Leaf B is supported,
  // Then the initial 32-bit APIC ID = CPUID.0BH:EDX
  // Else the initial 8-bit APIC ID = CPUID.1:EBX[31:24]
  //
  if (MaxCpuIdIndex >= CPUID_EXTENDED_TOPOLOGY) {
    AsmCpuidEx (CPUID_EXTENDED_TOPOLOGY, 0, NULL, NULL, NULL, &ApicId);
    return ApicId;
  }

  AsmCpuid (CPUID_VERSION_INFO, NULL, &RegEbx, NULL, NULL);
  return RegEbx >> 24;
}

/**
  Get the local APIC ID of the executing processor.

  @return  32-bit local APIC ID of the executing processor.
**/
UINT32
EFIAPI
GetApicId (
  VOID
  )
{
  UINT32 ApicId;

  ASSERT (GetApicMode () == LOCAL_APIC_MODE_XAPIC);

  if ((ApicId = GetInitialApicId ()) < 0x100) {
    //
    // If the initial local APIC ID is less 0x100, read APIC ID from
    // XAPIC_ID_OFFSET, otherwise return the initial local APIC ID.
    //
    ApicId = ReadLocalApicReg (XAPIC_ID_OFFSET);
    ApicId >>= 24;
  }
  return ApicId;
}

/**
  Get the value of the local APIC version register.

  @return  the value of the local APIC version register.
**/
UINT32
EFIAPI
GetApicVersion (
  VOID
  )
{
  return ReadLocalApicReg (XAPIC_VERSION_OFFSET);
}

/**
  Send a Fixed IPI to a specified target processor.

  This function returns after the IPI has been accepted by the target processor.

  @param  ApicId   The local APIC ID of the target processor.
  @param  Vector   The vector number of the interrupt being sent.
**/
VOID
EFIAPI
SendFixedIpi (
  IN UINT32          ApicId,
  IN UINT8           Vector
  )
{
  LOCAL_APIC_ICR_LOW IcrLow;

  IcrLow.Uint32 = 0;
  IcrLow.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_FIXED;
  IcrLow.Bits.Level = 1;
  IcrLow.Bits.Vector = Vector;
  SendIpi (IcrLow.Uint32, ApicId);
}

/**
  Send a Fixed IPI to all processors excluding self.

  This function returns after the IPI has been accepted by the target processors.

  @param  Vector   The vector number of the interrupt being sent.
**/
VOID
EFIAPI
SendFixedIpiAllExcludingSelf (
  IN UINT8           Vector
  )
{
  LOCAL_APIC_ICR_LOW IcrLow;

  IcrLow.Uint32 = 0;
  IcrLow.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_FIXED;
  IcrLow.Bits.Level = 1;
  IcrLow.Bits.DestinationShorthand = LOCAL_APIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF;
  IcrLow.Bits.Vector = Vector;
  SendIpi (IcrLow.Uint32, 0);
}

/**
  Send a SMI IPI to a specified target processor.

  This function returns after the IPI has been accepted by the target processor.

  @param  ApicId   Specify the local APIC ID of the target processor.
**/
VOID
EFIAPI
SendSmiIpi (
  IN UINT32          ApicId
  )
{
  LOCAL_APIC_ICR_LOW IcrLow;

  IcrLow.Uint32 = 0;
  IcrLow.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_SMI;
  IcrLow.Bits.Level = 1;
  SendIpi (IcrLow.Uint32, ApicId);
}

/**
  Send a SMI IPI to all processors excluding self.

  This function returns after the IPI has been accepted by the target processors.
**/
VOID
EFIAPI
SendSmiIpiAllExcludingSelf (
  VOID
  )
{
  LOCAL_APIC_ICR_LOW IcrLow;

  IcrLow.Uint32 = 0;
  IcrLow.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_SMI;
  IcrLow.Bits.Level = 1;
  IcrLow.Bits.DestinationShorthand = LOCAL_APIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF;
  SendIpi (IcrLow.Uint32, 0);
}

/**
  Send an INIT IPI to a specified target processor.

  This function returns after the IPI has been accepted by the target processor.

  @param  ApicId   Specify the local APIC ID of the target processor.
**/
VOID
EFIAPI
SendInitIpi (
  IN UINT32          ApicId
  )
{
  LOCAL_APIC_ICR_LOW IcrLow;

  IcrLow.Uint32 = 0;
  IcrLow.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_INIT;
  IcrLow.Bits.Level = 1;
  SendIpi (IcrLow.Uint32, ApicId);
}

/**
  Send an INIT IPI to all processors excluding self.

  This function returns after the IPI has been accepted by the target processors.
**/
VOID
EFIAPI
SendInitIpiAllExcludingSelf (
  VOID
  )
{
  LOCAL_APIC_ICR_LOW IcrLow;

  IcrLow.Uint32 = 0;
  IcrLow.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_INIT;
  IcrLow.Bits.Level = 1;
  IcrLow.Bits.DestinationShorthand = LOCAL_APIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF;
  SendIpi (IcrLow.Uint32, 0);
}

/**
  Send an INIT-Start-up-Start-up IPI sequence to a specified target processor.

  This function returns after the IPI has been accepted by the target processor.

  if StartupRoutine >= 1M, then ASSERT.
  if StartupRoutine is not multiple of 4K, then ASSERT.

  @param  ApicId          Specify the local APIC ID of the target processor.
  @param  StartupRoutine  Points to a start-up routine which is below 1M physical
                          address and 4K aligned.
**/
VOID
EFIAPI
SendInitSipiSipi (
  IN UINT32          ApicId,
  IN UINT32          StartupRoutine
  )
{
  LOCAL_APIC_ICR_LOW IcrLow;

  ASSERT (StartupRoutine < 0x100000);
  ASSERT ((StartupRoutine & 0xfff) == 0);

  SendInitIpi (ApicId);
  MicroSecondDelay (PcdGet32 (PcdCpuInitIpiDelayInMicroSeconds));
  IcrLow.Uint32 = 0;
  IcrLow.Bits.Vector = (StartupRoutine >> 12);
  IcrLow.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_STARTUP;
  IcrLow.Bits.Level = 1;
  SendIpi (IcrLow.Uint32, ApicId);
  MicroSecondDelay (200);
  SendIpi (IcrLow.Uint32, ApicId);
}

/**
  Send an INIT-Start-up-Start-up IPI sequence to all processors excluding self.

  This function returns after the IPI has been accepted by the target processors.

  if StartupRoutine >= 1M, then ASSERT.
  if StartupRoutine is not multiple of 4K, then ASSERT.

  @param  StartupRoutine    Points to a start-up routine which is below 1M physical
                            address and 4K aligned.
**/
VOID
EFIAPI
SendInitSipiSipiAllExcludingSelf (
  IN UINT32          StartupRoutine
  )
{
  LOCAL_APIC_ICR_LOW IcrLow;

  ASSERT (StartupRoutine < 0x100000);
  ASSERT ((StartupRoutine & 0xfff) == 0);

  SendInitIpiAllExcludingSelf ();
  MicroSecondDelay (PcdGet32 (PcdCpuInitIpiDelayInMicroSeconds));
  IcrLow.Uint32 = 0;
  IcrLow.Bits.Vector = (StartupRoutine >> 12);
  IcrLow.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_STARTUP;
  IcrLow.Bits.Level = 1;
  IcrLow.Bits.DestinationShorthand = LOCAL_APIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF;
  SendIpi (IcrLow.Uint32, 0);
  MicroSecondDelay (200);
  SendIpi (IcrLow.Uint32, 0);
}

/**
  Initialize the state of the SoftwareEnable bit in the Local APIC
  Spurious Interrupt Vector register.

  @param  Enable  If TRUE, then set SoftwareEnable to 1
                  If FALSE, then set SoftwareEnable to 0.

**/
VOID
EFIAPI
InitializeLocalApicSoftwareEnable (
  IN BOOLEAN  Enable
  )
{
  LOCAL_APIC_SVR  Svr;

  //
  // Set local APIC software-enabled bit.
  //
  Svr.Uint32 = ReadLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET);
  if (Enable) {
    if (Svr.Bits.SoftwareEnable == 0) {
      Svr.Bits.SoftwareEnable = 1;
      WriteLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET, Svr.Uint32);
    }
  } else {
    if (Svr.Bits.SoftwareEnable == 1) {
      Svr.Bits.SoftwareEnable = 0;
      WriteLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET, Svr.Uint32);
    }
  }
}

/**
  Programming Virtual Wire Mode.

  This function programs the local APIC for virtual wire mode following
  the example described in chapter A.3 of the MP 1.4 spec.

  IOxAPIC is not involved in this type of virtual wire mode.
**/
VOID
EFIAPI
ProgramVirtualWireMode (
  VOID
  )
{
  LOCAL_APIC_SVR      Svr;
  LOCAL_APIC_LVT_LINT Lint;

  //
  // Enable the APIC via SVR and set the spurious interrupt to use Int 00F.
  //
  Svr.Uint32 = ReadLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET);
  Svr.Bits.SpuriousVector = 0xf;
  Svr.Bits.SoftwareEnable = 1;
  WriteLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET, Svr.Uint32);

  //
  // Program the LINT0 vector entry as ExtInt. Not masked, edge, active high.
  //
  Lint.Uint32 = ReadLocalApicReg (XAPIC_LVT_LINT0_OFFSET);
  Lint.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_EXTINT;
  Lint.Bits.InputPinPolarity = 0;
  Lint.Bits.TriggerMode = 0;
  Lint.Bits.Mask = 0;
  WriteLocalApicReg (XAPIC_LVT_LINT0_OFFSET, Lint.Uint32);

  //
  // Program the LINT0 vector entry as NMI. Not masked, edge, active high.
  //
  Lint.Uint32 = ReadLocalApicReg (XAPIC_LVT_LINT1_OFFSET);
  Lint.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_NMI;
  Lint.Bits.InputPinPolarity = 0;
  Lint.Bits.TriggerMode = 0;
  Lint.Bits.Mask = 0;
  WriteLocalApicReg (XAPIC_LVT_LINT1_OFFSET, Lint.Uint32);
}

/**
  Disable LINT0 & LINT1 interrupts.

  This function sets the mask flag in the LVT LINT0 & LINT1 registers.
**/
VOID
EFIAPI
DisableLvtInterrupts (
  VOID
  )
{
  LOCAL_APIC_LVT_LINT LvtLint;

  LvtLint.Uint32 = ReadLocalApicReg (XAPIC_LVT_LINT0_OFFSET);
  LvtLint.Bits.Mask = 1;
  WriteLocalApicReg (XAPIC_LVT_LINT0_OFFSET, LvtLint.Uint32);

  LvtLint.Uint32 = ReadLocalApicReg (XAPIC_LVT_LINT1_OFFSET);
  LvtLint.Bits.Mask = 1;
  WriteLocalApicReg (XAPIC_LVT_LINT1_OFFSET, LvtLint.Uint32);
}

/**
  Read the initial count value from the init-count register.

  @return The initial count value read from the init-count register.
**/
UINT32
EFIAPI
GetApicTimerInitCount (
  VOID
  )
{
  return ReadLocalApicReg (XAPIC_TIMER_INIT_COUNT_OFFSET);
}

/**
  Read the current count value from the current-count register.

  @return The current count value read from the current-count register.
**/
UINT32
EFIAPI
GetApicTimerCurrentCount (
  VOID
  )
{
  return ReadLocalApicReg (XAPIC_TIMER_CURRENT_COUNT_OFFSET);
}

/**
  Initialize the local APIC timer.

  The local APIC timer is initialized and enabled.

  @param DivideValue   The divide value for the DCR. It is one of 1,2,4,8,16,32,64,128.
                       If it is 0, then use the current divide value in the DCR.
  @param InitCount     The initial count value.
  @param PeriodicMode  If TRUE, timer mode is peridoic. Othewise, timer mode is one-shot.
  @param Vector        The timer interrupt vector number.
**/
VOID
EFIAPI
InitializeApicTimer (
  IN UINTN   DivideValue,
  IN UINT32  InitCount,
  IN BOOLEAN PeriodicMode,
  IN UINT8   Vector
  )
{
  LOCAL_APIC_SVR       Svr;
  LOCAL_APIC_DCR       Dcr;
  LOCAL_APIC_LVT_TIMER LvtTimer;
  UINT32               Divisor;

  //
  // Ensure local APIC is in software-enabled state.
  //
  Svr.Uint32 = ReadLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET);
  Svr.Bits.SoftwareEnable = 1;
  WriteLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET, Svr.Uint32);

  //
  // Program init-count register.
  //
  WriteLocalApicReg (XAPIC_TIMER_INIT_COUNT_OFFSET, InitCount);

  if (DivideValue != 0) {
    ASSERT (DivideValue <= 128);
    ASSERT (DivideValue == GetPowerOfTwo32 ((UINT32)DivideValue));
    Divisor = (UINT32) ((HighBitSet32 ((UINT32)DivideValue) - 1) & 0x7);

    Dcr.Uint32 = ReadLocalApicReg (XAPIC_TIMER_DIVIDE_CONFIGURATION_OFFSET);
    Dcr.Bits.DivideValue1 = (Divisor & 0x3);
    Dcr.Bits.DivideValue2 = (Divisor >> 2);
    WriteLocalApicReg (XAPIC_TIMER_DIVIDE_CONFIGURATION_OFFSET, Dcr.Uint32);
  }

  //
  // Enable APIC timer interrupt with specified timer mode.
  //
  LvtTimer.Uint32 = ReadLocalApicReg (XAPIC_LVT_TIMER_OFFSET);
  if (PeriodicMode) {
    LvtTimer.Bits.TimerMode = 1;
  } else {
    LvtTimer.Bits.TimerMode = 0;
  }
  LvtTimer.Bits.Mask = 0;
  LvtTimer.Bits.Vector = Vector;
  WriteLocalApicReg (XAPIC_LVT_TIMER_OFFSET, LvtTimer.Uint32);
}

/**
  Get the state of the local APIC timer.

  @param DivideValue   Return the divide value for the DCR. It is one of 1,2,4,8,16,32,64,128.
  @param PeriodicMode  Return the timer mode. If TRUE, timer mode is peridoic. Othewise, timer mode is one-shot.
  @param Vector        Return the timer interrupt vector number.
**/
VOID
EFIAPI
GetApicTimerState (
  OUT UINTN    *DivideValue  OPTIONAL,
  OUT BOOLEAN  *PeriodicMode  OPTIONAL,
  OUT UINT8    *Vector  OPTIONAL
  )
{
  UINT32 Divisor;
  LOCAL_APIC_DCR Dcr;
  LOCAL_APIC_LVT_TIMER LvtTimer;

  if (DivideValue != NULL) {
    Dcr.Uint32 = ReadLocalApicReg (XAPIC_TIMER_DIVIDE_CONFIGURATION_OFFSET);
    Divisor = Dcr.Bits.DivideValue1 | (Dcr.Bits.DivideValue2 << 2);
    Divisor = (Divisor + 1) & 0x7;
    *DivideValue = ((UINTN)1) << Divisor;
  }

  if (PeriodicMode != NULL || Vector != NULL) {
    LvtTimer.Uint32 = ReadLocalApicReg (XAPIC_LVT_TIMER_OFFSET);
    if (PeriodicMode != NULL) {
      if (LvtTimer.Bits.TimerMode == 1) {
        *PeriodicMode = TRUE;
      } else {
        *PeriodicMode = FALSE;
      }
    }
    if (Vector != NULL) {
      *Vector = (UINT8) LvtTimer.Bits.Vector;
    }
  }
}

/**
  Enable the local APIC timer interrupt.
**/
VOID
EFIAPI
EnableApicTimerInterrupt (
  VOID
  )
{
  LOCAL_APIC_LVT_TIMER LvtTimer;

  LvtTimer.Uint32 = ReadLocalApicReg (XAPIC_LVT_TIMER_OFFSET);
  LvtTimer.Bits.Mask = 0;
  WriteLocalApicReg (XAPIC_LVT_TIMER_OFFSET, LvtTimer.Uint32);
}

/**
  Disable the local APIC timer interrupt.
**/
VOID
EFIAPI
DisableApicTimerInterrupt (
  VOID
  )
{
  LOCAL_APIC_LVT_TIMER LvtTimer;

  LvtTimer.Uint32 = ReadLocalApicReg (XAPIC_LVT_TIMER_OFFSET);
  LvtTimer.Bits.Mask = 1;
  WriteLocalApicReg (XAPIC_LVT_TIMER_OFFSET, LvtTimer.Uint32);
}

/**
  Get the local APIC timer interrupt state.

  @retval TRUE  The local APIC timer interrupt is enabled.
  @retval FALSE The local APIC timer interrupt is disabled.
**/
BOOLEAN
EFIAPI
GetApicTimerInterruptState (
  VOID
  )
{
  LOCAL_APIC_LVT_TIMER LvtTimer;

  LvtTimer.Uint32 = ReadLocalApicReg (XAPIC_LVT_TIMER_OFFSET);
  return (BOOLEAN) (LvtTimer.Bits.Mask == 0);
}

/**
  Send EOI to the local APIC.
**/
VOID
EFIAPI
SendApicEoi (
  VOID
  )
{
  WriteLocalApicReg (XAPIC_EOI_OFFSET, 0);
}

/**
  Get the 32-bit address that a device should use to send a Message Signaled
  Interrupt (MSI) to the Local APIC of the currently executing processor.

  @return 32-bit address used to send an MSI to the Local APIC.
**/
UINT32
EFIAPI
GetApicMsiAddress (
  VOID
  )
{
  LOCAL_APIC_MSI_ADDRESS  MsiAddress;

  //
  // Return address for an MSI interrupt to be delivered only to the APIC ID
  // of the currently executing processor.
  //
  MsiAddress.Uint32             = 0;
  MsiAddress.Bits.BaseAddress   = 0xFEE;
  MsiAddress.Bits.DestinationId = GetApicId ();
  return MsiAddress.Uint32;
}

/**
  Get the 64-bit data value that a device should use to send a Message Signaled
  Interrupt (MSI) to the Local APIC of the currently executing processor.

  If Vector is not in range 0x10..0xFE, then ASSERT().
  If DeliveryMode is not supported, then ASSERT().

  @param  Vector          The 8-bit interrupt vector associated with the MSI.
                          Must be in the range 0x10..0xFE
  @param  DeliveryMode    A 3-bit value that specifies how the recept of the MSI
                          is handled.  The only supported values are:
                            0: LOCAL_APIC_DELIVERY_MODE_FIXED
                            1: LOCAL_APIC_DELIVERY_MODE_LOWEST_PRIORITY
                            2: LOCAL_APIC_DELIVERY_MODE_SMI
                            4: LOCAL_APIC_DELIVERY_MODE_NMI
                            5: LOCAL_APIC_DELIVERY_MODE_INIT
                            7: LOCAL_APIC_DELIVERY_MODE_EXTINT

  @param  LevelTriggered  TRUE specifies a level triggered interrupt.
                          FALSE specifies an edge triggered interrupt.
  @param  AssertionLevel  Ignored if LevelTriggered is FALSE.
                          TRUE specifies a level triggered interrupt that active
                          when the interrupt line is asserted.
                          FALSE specifies a level triggered interrupt that active
                          when the interrupt line is deasserted.

  @return 64-bit data value used to send an MSI to the Local APIC.
**/
UINT64
EFIAPI
GetApicMsiValue (
  IN UINT8    Vector,
  IN UINTN    DeliveryMode,
  IN BOOLEAN  LevelTriggered,
  IN BOOLEAN  AssertionLevel
  )
{
  LOCAL_APIC_MSI_DATA  MsiData;

  ASSERT (Vector >= 0x10 && Vector <= 0xFE);
  ASSERT (DeliveryMode < 8 && DeliveryMode != 6 && DeliveryMode != 3);

  MsiData.Uint64            = 0;
  MsiData.Bits.Vector       = Vector;
  MsiData.Bits.DeliveryMode = (UINT32)DeliveryMode;
  if (LevelTriggered) {
    MsiData.Bits.TriggerMode = 1;
    if (AssertionLevel) {
      MsiData.Bits.Level = 1;
    }
  }
  return MsiData.Uint64;
}
