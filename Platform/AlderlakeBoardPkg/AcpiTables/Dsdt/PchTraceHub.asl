/** @file
  PCH Trace Hub Device

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PchReservedResources.h>
External(PTHM, FieldUnitObj)
#define R_TRACE_HUB_MEM_MTB_SCRPD2     0xE8

Scope (\_SB.PC00)
{
  //
  // Declare PCH TH device for target debugger mode
  //
  Device(PTHT)
  {
    Name(_ADR, 0x001F0007)

    Method(_STA, 0, Serialized)
    {
       If (LEqual (PTHM, 1)) {
         Return (0xF)
       } Else {
         Return (0x0)
       }
    }

    OperationRegion (PTHR, PCI_Config, 0x00, 0x20)
    Field(PTHR, AnyAcc, NoLock, Preserve)
    {
      Offset (0x10),
      MTBL,  32,
      MTBH,  32,
    }

    Method (THEN, 0, Serialized, 0, UnknownObj)
    {
      OperationRegion(RMTB, SystemMemory, Add (ShiftLeft (MTBH, 32), And (MTBL, 0xFFF00000)), 0x100)
      Field(RMTB, AnyAcc, NoLock, Preserve) {
        Offset (R_TRACE_HUB_MEM_MTB_SCRPD2),
        BTTH,  1,
      }
      //
      // Check SCRPD2[0] if BIOS trace to TraceHub is enabled
      //
      If (LEqual (BTTH, 1)) {
        Return (1)
      } Else {
        Return (0)
      }
    }
  }
}

Scope (\_SB) {
  //
  // Declare PCH TH device for host debugger mode
  //
  Device(PTHH)
  {
    Name(_HID, "INTC1001")

    Method(_STA,0,Serialized)
    {
      If (LEqual (PTHM, 2)) {
        Return(0xF)
      } Else {
        Return (0x0)
      }
    }

    Name(_CRS, ResourceTemplate() {
        Memory32Fixed(ReadWrite, PCH_TRACE_HUB_MTB_BASE_ADDRESS, PCH_TRACE_HUB_MTB_MMIO_SIZE) // MTB BAR
        Memory32Fixed(ReadWrite, PCH_TRACE_HUB_SW_BASE_ADDRESS, PCH_TRACE_HUB_SW_MMIO_SIZE)   // SW BAR
        })

    OperationRegion(MTBR, SystemMemory, PCH_TRACE_HUB_MTB_BASE_ADDRESS, 0x100)
    Field(MTBR, AnyAcc, NoLock, Preserve) {
      Offset (R_TRACE_HUB_MEM_MTB_SCRPD2),
      BHTH,  1,
    }

    Method (THEN, 0, Serialized, 0, UnknownObj)
    {
      //
      // Check SCRPD2[0] if BIOS trace to TraceHub is enabled
      //
      If (LEqual (BHTH, 1)) {
        Return (1)
      } Else {
        Return (0)
      }
    }
  }
}
