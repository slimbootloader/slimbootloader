/** @file
  Intel Processor Power Management ACPI Code.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CpuPowerMgmt.h"

DefinitionBlock(
  "CPU0TST.aml",
  "SSDT",
  0x02,
  "PmRef",
  "Cpu0Tst",
  0x3000
  )
{
  External(\_PR.CPU0, DeviceObj)
  External(PDC0)
  External(\_PR.CFGD, IntObj)
  External(_PSS)
  External (NLPC)

  Scope(\_PR.CPU0)
  {
    Name(_TPC, 0)  // All T-States are available

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      //
      // IF OSPM is capable of direct access to MSR
      //    Report MSR interface
      // ELSE
      //    Report I/O interface
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //
      If(And(PDC0, 0x0004)) {

        Return(Package() {
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)}
        })
      }

      Return(Package() {
        ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)},
        ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)}
      })
    }

    //
    // _TSS package for fine-grained T-State control.
    // "Power" fields are replaced with real values by the first
    // call of _TSS method.
    //
    Name(TSMF, Package() {
        Package(){100, 1000, 0, 0x00, 0},
        Package(){ 94,  940, 0, 0x1F, 0},
        Package(){ 88,  880, 0, 0x1E, 0},
        Package(){ 82,  820, 0, 0x1D, 0},
        Package(){ 75,  760, 0, 0x1C, 0},
        Package(){ 69,  700, 0, 0x1B, 0},
        Package(){ 63,  640, 0, 0x1A, 0},
        Package(){ 57,  580, 0, 0x19, 0},
        Package(){ 50,  520, 0, 0x18, 0},
        Package(){ 44,  460, 0, 0x17, 0},
        Package(){ 38,  400, 0, 0x16, 0},
        Package(){ 32,  340, 0, 0x15, 0},
        Package(){ 25,  280, 0, 0x14, 0},
        Package(){ 19,  220, 0, 0x13, 0},
        Package(){ 13,  160, 0, 0x12, 0},
        Package(){  7,  100, 0, 0x11, 0}
    })

    //
    // _TSS package for T-State control (Coarse grained)
    // "Power" fields are replaced with real values by the first
    // call of _TSS method.
    //
    Name(TSMC, Package() {
        Package(){100, 1000, 0, 0x00, 0},
        Package(){ 88,  875, 0, 0x1E, 0},
        Package(){ 75,  750, 0, 0x1C, 0},
        Package(){ 63,  625, 0, 0x1A, 0},
        Package(){ 50,  500, 0, 0x18, 0},
        Package(){ 38,  375, 0, 0x16, 0},
        Package(){ 25,  250, 0, 0x14, 0},
        Package(){ 13,  125, 0, 0x12, 0}
    })

    Name(TSSF, 0)  // Flag for TSIF/TSIC/TSMF/TSMC initialization
    Mutex(TSMO, 0) // Mutex object to ensure the _TSS initalization code is only executed once

    Method(_TSS, 0, Serialized)
    {
      //
      // Update "Power" fields of TSIC or TSIF or TSMC or TSMF with the LFM
      // power data IF _PSS is available
      //    Power caluclation:
      //               n - Number of T-states available
      //              _TSS(x).power = LFM.Power * (n-x)/n
      //
      IF (LAnd(LNot(TSSF),CondRefOf(_PSS)))
      {
        //
        // Acquire Mutex to make sure the initialization happens only once.
        //
        Acquire (TSMO, 0xFFFF)
        //
        // Only one thread will be able to acquire the mutex at a time, but the other threads which have acquired the mutex previously, will eventually try to execute the TSS initalization code.
        // So, let's check if TSS has already been initalized once again. If its initalized, skip the initalization.
        //
        IF (LAnd(LNot(TSSF),CondRefOf(_PSS)))
        {
          Name (LFMI, 0)
          Store (_PSS, Local0)
          Store (SizeOf(Local0), LFMI)
          Decrement(LFMI)    // Index of LFM entry in _PSS
          Name ( LFMP, 0) //LFM Power from _PSS
          Store ( DerefOf(Index(DerefOf(Index(Local0,LFMI)),1)) , LFMP)
          Store (0, Local0)

          //
          // Copy reference of appropiate TSS package based on Fine grained T-state support
          // We'll update the power in the package directly (via the reference variable Local1)
          //
          // If Fine Grained T-states is enabled
          //      TSMF
          //    ELSE
          //      TSMC
          //
          If(And(\_PR.CFGD,PPM_TSTATE_FINE_GRAINED))
          {
            Store ( RefOf(TSMF), Local1 )
            Store ( SizeOf(TSMF),Local2 )
          }
          Else
          {
            Store ( RefOf(TSMC), Local1 )
            Store ( SizeOf(TSMC),Local2 )
          }

          While(LLess(Local0, Local2))
          {
            Store(Divide(Multiply(LFMP, Subtract(Local2, Local0)), Local2),
                  Local4)    // Power for this entry
            Store(Local4,Index(DerefOf(Index(DerefOf(Local1),Local0)),1))
            Increment(Local0)
          }

          Store(Ones, TSSF)    // Set flag to indicate TSS table initalization is complete
        }

        Release (TSMO)

      }
      //
      //  If Fine Grained T-states is enabled
      //    Report TSMF
      //  ELSE
      //    Report TSMC
      //
      If(And(\_PR.CFGD, PPM_TSTATE_FINE_GRAINED))
      {
        Return(TSMF)
      }
      Else
      {
        Return(TSMC)
      }
    }

    //
    // _TDL (T-state Depth Limit)
    //
    // This optional object evaluates to the _TSS entry number of the lowest power throttling state that OSPM may use.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the Throttling Depth Limit _TSS entry number:
    //   0 - throttling disabled.
    //   1 - state 1 is the lowest power T-state available.
    //   2 - state 2 is the lowest power T-state available.
    //   ...
    //   n - state n is the lowest power T-state available.
    //
    Method(_TDL, 0)
    {
      Store ("Cpu0: _TDL Called", Debug)
      If(And(\_PR.CFGD, PPM_TSTATE_FINE_GRAINED))
      {
        Return(Subtract(SizeOf(TSMF),1))
      }
      Else
      {
        Return(Subtract(SizeOf(TSMC),1))
      }
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF !(direct access to MSR)
      //    Report SW_ANY as the coordination type
      // ELSE
      //   Report SW_ALL as the coordination type
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //
      If (LNot(And(PDC0,4))) {
        Return(Package(){     // SW_ANY
          Package(){
            5,                // # entries.
            0,                // Revision.
            0,                // Domain #.
            0xFD,             // Coord Type- SW_ANY
            NLPC              // # processors.
          }
        })
      }
      Return(Package(){       // SW_ALL
        Package(){
          5,                  // # entries.
          0,                  // Revision.
          0,                  // Domain #.
          0xFC,               // Coord Type- SW_ALL
          1                   // # processors.
        }
      })
    }
  }
} // End of Definition Block

