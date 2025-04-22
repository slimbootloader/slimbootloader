/** @file
  ACPI RTD3 SSDT table for integrated SATA adapter

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(\_SB.PC00.SAT0.NVM1, DeviceObj)
External(\_SB.PC00.SAT0.NVM2, DeviceObj)
External(\_SB.PC00.SAT0.NVM3, DeviceObj)
External(\_SB.PC00.SAT0.PRT0, DeviceObj)
External(\_SB.PC00.SAT0.PRT1, DeviceObj)
External(\_SB.PC00.SAT0.PRT2, DeviceObj)
External(\_SB.PC00.SAT0.PRT3, DeviceObj)
External(\_SB.PC00.SAT0.PRT4, DeviceObj)
External(\_SB.PC00.SAT0.PRT5, DeviceObj)

Scope(\_SB.PC00.SAT0) {

  OperationRegion(SMIO,PCI_Config,0x24,4)
  Field(SMIO,AnyAcc, NoLock, Preserve) {
    MBR6, 32,     ///- SATA ABAR
  }

  OperationRegion(PCIR, PCI_Config, 0x00, 0x10)
  Field(PCIR, DWordAcc, NoLock, Preserve) {
    Offset(0x0A),
    SUBC,   8  // Link Control register
  }

  //
  // Platform indicates D3Cold support by defining PWRG objects.
  // If PWRG object is defined power resource included in Rtd3SataPort.asl
  // will be visible.
  //
  If(And(RCG1_RTD3_PRT0_ENABLED, RCG1)) {
    Scope(PRT0){
      Name(PBAR, 0x118)
      External(PWRG)
      If(CondRefOf(PWRG)){
        Include("Rtd3SataPort.asl")
      }
    }
  }

  If(And(RCG1_RTD3_PRT1_ENABLED, RCG1)) {
    Scope(PRT1){
      Name(PBAR, 0x198)
      External(PWRG)
      If(CondRefOf(PWRG)){
        Include("Rtd3SataPort.asl")
      }
    }
  }

  If(And(RCG1_RTD3_PRT2_ENABLED, RCG1)) {
    Scope(PRT2){
      Name(PBAR, 0x218)
      External(PWRG)
      If(CondRefOf(PWRG)){
        Include("Rtd3SataPort.asl")
      }
    }
  }

  If(And(RCG1_RTD3_PRT3_ENABLED, RCG1)) {
    Scope(PRT3){
      Name(PBAR, 0x298)
      External(PWRG)
      If(CondRefOf(PWRG)){
        Include("Rtd3SataPort.asl")
      }
    }
  }

  If(And(RCG1_RTD3_PRT4_ENABLED, RCG1)) {
    Scope(PRT4){
      Name(PBAR, 0x318)
      External(PWRG)
      If(CondRefOf(PWRG)){
        Include("Rtd3SataPort.asl")
      }
    }
  }

  If(And(RCG1_RTD3_PRT5_ENABLED, RCG1)) {
    Scope(PRT5){
      Name(PBAR, 0x398)
      External(PWRG)
      If(CondRefOf(PWRG)){
        Include("Rtd3SataPort.asl")
      }
    }
  }
}
