/** @file
  This file contains the SSDT ASL code.
  It defines a set of SOC Common APIs

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define ROOTPORT_NOTIFY(HOSTBUS, ROOT_PORT_OBJECT, RPMASK, RPNUM)      \
    If(LNotEqual(HOSTBUS.ROOT_PORT_OBJECT.VDID,0xFFFFFFFF)) { \
      If(LNotEqual(And(RPMASK,ShiftLeft(1, RPNUM)), 1)) { \
        Notify (HOSTBUS.ROOT_PORT_OBJECT,0) \
      } \
    }

#define ROOTPORT_EXTERNAL(HOSTBUS, ROOT_PORT_OBJECT)   \
  External(HOSTBUS.ROOT_PORT_OBJECT, DeviceObj) \
  External(HOSTBUS.ROOT_PORT_OBJECT.PMEX, FieldUnitObj) \
  External(HOSTBUS.ROOT_PORT_OBJECT.PMSX, FieldUnitObj) \
  External(HOSTBUS.ROOT_PORT_OBJECT.HPEX, FieldUnitObj) \
  External(HOSTBUS.ROOT_PORT_OBJECT.VDID, FieldUnitObj) \
  External(HOSTBUS.ROOT_PORT_OBJECT.HPSX, FieldUnitObj)

DefinitionBlock (
  "SocCommon.aml",
  "SSDT",
  0x02,
  "SocCmn",
  "SocCmn ",
  0x3000
  )
{
  External(\_SB.PC00.TXHC, DeviceObj)
  External(\_SB.PC00.TCON, MethodObj)

  External(TRTD, FieldUnitObj)
  External(TCNT, FieldUnitObj)

  //
  // @todo - move to PchNvs
  //
  External(NEXP, FieldUnitObj)
  External(OSCC, FieldUnitObj)

  #define HOST_BUS \_SB.PC00
  ROOTPORT_EXTERNAL(HOST_BUS, RP01)
  ROOTPORT_EXTERNAL(HOST_BUS, RP02)
  ROOTPORT_EXTERNAL(HOST_BUS, RP03)
  ROOTPORT_EXTERNAL(HOST_BUS, RP04)
  ROOTPORT_EXTERNAL(HOST_BUS, RP05)
  ROOTPORT_EXTERNAL(HOST_BUS, RP06)
  ROOTPORT_EXTERNAL(HOST_BUS, RP07)
  ROOTPORT_EXTERNAL(HOST_BUS, RP08)
  ROOTPORT_EXTERNAL(HOST_BUS, RP09)
  ROOTPORT_EXTERNAL(HOST_BUS, RP10)
  ROOTPORT_EXTERNAL(HOST_BUS, RP11)
  ROOTPORT_EXTERNAL(HOST_BUS, RP12)
  ROOTPORT_EXTERNAL(HOST_BUS, RP13)
  ROOTPORT_EXTERNAL(HOST_BUS, RP14)
  ROOTPORT_EXTERNAL(HOST_BUS, RP15)
  ROOTPORT_EXTERNAL(HOST_BUS, RP16)
  ROOTPORT_EXTERNAL(HOST_BUS, RP17)
  ROOTPORT_EXTERNAL(HOST_BUS, RP18)
  ROOTPORT_EXTERNAL(HOST_BUS, RP19)
  ROOTPORT_EXTERNAL(HOST_BUS, RP20)
  ROOTPORT_EXTERNAL(HOST_BUS, RP21)
  ROOTPORT_EXTERNAL(HOST_BUS, RP22)
  ROOTPORT_EXTERNAL(HOST_BUS, RP23)
  ROOTPORT_EXTERNAL(HOST_BUS, RP24)
  ROOTPORT_EXTERNAL(HOST_BUS, RP25)
  ROOTPORT_EXTERNAL(HOST_BUS, RP26)
  ROOTPORT_EXTERNAL(HOST_BUS, RP27)
  ROOTPORT_EXTERNAL(HOST_BUS, RP28)
  #undef HOST_BUS

  External(\_SB.PC00.TRP0.HPSX, IntObj)
  External(\_SB.PC00.TRP1.HPSX, IntObj)
  External(\_SB.PC00.TRP2.HPSX, IntObj)
  External(\_SB.PC00.TRP3.HPSX, IntObj)
  External(\_SB.PC00.TRP0.HPEX, IntObj)
  External(\_SB.PC00.TRP1.HPEX, IntObj)
  External(\_SB.PC00.TRP2.HPEX, IntObj)
  External(\_SB.PC00.TRP3.HPEX, IntObj)
  External(\_SB.PC00.TRP0.PMEX, IntObj)
  External(\_SB.PC00.TRP1.PMEX, IntObj)
  External(\_SB.PC00.TRP2.PMEX, IntObj)
  External(\_SB.PC00.TRP3.PMEX, IntObj)
  External(\_SB.PC00.TRP0.PMSX, IntObj)
  External(\_SB.PC00.TRP1.PMSX, IntObj)
  External(\_SB.PC00.TRP2.PMSX, IntObj)
  External(\_SB.PC00.TRP3.PMSX, IntObj)
  External(\_SB.PC01.TRP0.HPSX, IntObj)
  External(\_SB.PC01.TRP1.HPSX, IntObj)
  External(\_SB.PC01.TRP2.HPSX, IntObj)
  External(\_SB.PC01.TRP3.HPSX, IntObj)
  External(\_SB.PC01.TRP0.HPEX, IntObj)
  External(\_SB.PC01.TRP1.HPEX, IntObj)
  External(\_SB.PC01.TRP2.HPEX, IntObj)
  External(\_SB.PC01.TRP3.HPEX, IntObj)
  External(\_SB.PC01.TRP0.PMEX, IntObj)
  External(\_SB.PC01.TRP1.PMEX, IntObj)
  External(\_SB.PC01.TRP2.PMEX, IntObj)
  External(\_SB.PC01.TRP3.PMEX, IntObj)
  External(\_SB.PC01.TRP0.PMSX, IntObj)
  External(\_SB.PC01.TRP1.PMSX, IntObj)
  External(\_SB.PC01.TRP2.PMSX, IntObj)
  External(\_SB.PC01.TRP3.PMSX, IntObj)

  External(\_SB.PR00, DeviceObj)
  External(\_SB.PR01, DeviceObj)
  External(\_SB.PR02, DeviceObj)
  External(\_SB.PR03, DeviceObj)
  External(\_SB.PR04, DeviceObj)
  External(\_SB.PR05, DeviceObj)
  External(\_SB.PR06, DeviceObj)
  External(\_SB.PR07, DeviceObj)
  External(\_SB.PR08, DeviceObj)
  External(\_SB.PR09, DeviceObj)
  External(\_SB.PR10, DeviceObj)
  External(\_SB.PR11, DeviceObj)
  External(\_SB.PR12, DeviceObj)
  External(\_SB.PR13, DeviceObj)
  External(\_SB.PR14, DeviceObj)
  External(\_SB.PR15, DeviceObj)

  //
  // @todo - move all CPU related references to seprate CPU FRU
  //
  External(PF00, IntObj)
  External(PF01, IntObj)
  External(PF02, IntObj)
  External(PF03, IntObj)
  External(PF04, IntObj)
  External(PF05, IntObj)
  External(PF06, IntObj)
  External(PF07, IntObj)
  External(PF08, IntObj)
  External(PF09, IntObj)
  External(PF10, IntObj)
  External(PF11, IntObj)
  External(PF12, IntObj)
  External(PF13, IntObj)
  External(PF14, IntObj)
  External(PF15, IntObj)

  External(\_SB.PR00.TPSS, PkgObj)
  External(\_SB.PR00.LPSS, PkgObj)
  External(\_SB.CPPC, FieldUnitObj)
  External(\_SB.TCWK, MethodObj)

  External(\PCHS, FieldUnitObj)
  External(\PCHX, IntObj)

  //
  // Notify Hot Plug
  // - Clear Hot Plug SCI Enable on each root port
  //
  Method(NHPG,0,Serialized) {
    // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP01.HPEX)
    Store(0,\_SB.PC00.RP02.HPEX)
    Store(0,\_SB.PC00.RP03.HPEX)
    Store(0,\_SB.PC00.RP04.HPEX)
    Store(0,\_SB.PC00.RP05.HPEX)
    Store(0,\_SB.PC00.RP06.HPEX)
    Store(0,\_SB.PC00.RP07.HPEX)
    Store(0,\_SB.PC00.RP08.HPEX)
    Store(0,\_SB.PC00.RP09.HPEX)
    Store(0,\_SB.PC00.RP10.HPEX)
    Store(0,\_SB.PC00.RP11.HPEX)
    Store(0,\_SB.PC00.RP12.HPEX)
    If(LEqual(PCHS, PCHX)) {
      Store(0,\_SB.PC00.RP13.HPEX)
      Store(0,\_SB.PC00.RP14.HPEX)
      Store(0,\_SB.PC00.RP15.HPEX)
      Store(0,\_SB.PC00.RP16.HPEX)
      Store(0,\_SB.PC00.RP17.HPEX)
      Store(0,\_SB.PC00.RP18.HPEX)
      Store(0,\_SB.PC00.RP19.HPEX)
      Store(0,\_SB.PC00.RP20.HPEX)
      Store(0,\_SB.PC00.RP21.HPEX)
      Store(0,\_SB.PC00.RP22.HPEX)
      Store(0,\_SB.PC00.RP23.HPEX)
      Store(0,\_SB.PC00.RP24.HPEX)
      Store(0,\_SB.PC00.RP25.HPEX)
      Store(0,\_SB.PC00.RP26.HPEX)
      Store(0,\_SB.PC00.RP27.HPEX)
      Store(0,\_SB.PC00.RP28.HPEX)
    }

    If (CondRefOf(\_SB.PC00.TXHC)) {
      If (CondRefOf(\_SB.PC01)) {
        Store(0,\_SB.PC01.TRP0.HPEX)
        Store(0,\_SB.PC01.TRP1.HPEX)
        Store(0,\_SB.PC01.TRP2.HPEX)
        Store(0,\_SB.PC01.TRP3.HPEX)
      } Else {
        Store(0,\_SB.PC00.TRP0.HPEX)
        Store(0,\_SB.PC00.TRP1.HPEX)
        Store(0,\_SB.PC00.TRP2.HPEX)
        Store(0,\_SB.PC00.TRP3.HPEX)
      }
    }

    Store(1,\_SB.PC00.RP01.HPSX)
    Store(1,\_SB.PC00.RP02.HPSX)
    Store(1,\_SB.PC00.RP03.HPSX)
    Store(1,\_SB.PC00.RP04.HPSX)
    Store(1,\_SB.PC00.RP05.HPSX)
    Store(1,\_SB.PC00.RP06.HPSX)
    Store(1,\_SB.PC00.RP07.HPSX)
    Store(1,\_SB.PC00.RP08.HPSX)
    Store(1,\_SB.PC00.RP09.HPSX)
    Store(1,\_SB.PC00.RP10.HPSX)
    Store(1,\_SB.PC00.RP11.HPSX)
    Store(1,\_SB.PC00.RP12.HPSX)
    If(LEqual(PCHS, PCHX)) {
      Store(1,\_SB.PC00.RP13.HPSX)
      Store(1,\_SB.PC00.RP14.HPSX)
      Store(1,\_SB.PC00.RP15.HPSX)
      Store(1,\_SB.PC00.RP16.HPSX)
      Store(1,\_SB.PC00.RP17.HPSX)
      Store(1,\_SB.PC00.RP18.HPSX)
      Store(1,\_SB.PC00.RP19.HPSX)
      Store(1,\_SB.PC00.RP20.HPSX)
      Store(1,\_SB.PC00.RP21.HPSX)
      Store(1,\_SB.PC00.RP22.HPSX)
      Store(1,\_SB.PC00.RP23.HPSX)
      Store(1,\_SB.PC00.RP24.HPSX)
      Store(1,\_SB.PC00.RP25.HPSX)
      Store(1,\_SB.PC00.RP26.HPSX)
      Store(1,\_SB.PC00.RP27.HPSX)
      Store(1,\_SB.PC00.RP28.HPSX)
    }

    If (CondRefOf(\_SB.PC00.TXHC)) {
      If (CondRefOf(\_SB.PC01)) {
        Store(1,\_SB.PC01.TRP0.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC01.TRP1.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC01.TRP2.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC01.TRP3.HPSX) // clear the hot plug SCI status bit
      } Else {
        Store(1,\_SB.PC00.TRP0.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC00.TRP1.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC00.TRP2.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC00.TRP3.HPSX) // clear the hot plug SCI status bit
      }
    }
  }

  //
  // Notify PME
  // - Clear Power Management SCI Enable on each root port
  //
  Method(NPME,0,Serialized)
  {
    Store(0,\_SB.PC00.RP01.PMEX)
    Store(0,\_SB.PC00.RP02.PMEX)
    Store(0,\_SB.PC00.RP03.PMEX)
    Store(0,\_SB.PC00.RP04.PMEX)
    Store(0,\_SB.PC00.RP05.PMEX)
    Store(0,\_SB.PC00.RP06.PMEX)
    Store(0,\_SB.PC00.RP07.PMEX)
    Store(0,\_SB.PC00.RP08.PMEX)
    Store(0,\_SB.PC00.RP09.PMEX)
    Store(0,\_SB.PC00.RP10.PMEX)
    Store(0,\_SB.PC00.RP11.PMEX)
    Store(0,\_SB.PC00.RP12.PMEX)
    If(LEqual(PCHS, PCHX)) {
      Store(0,\_SB.PC00.RP13.PMEX)
      Store(0,\_SB.PC00.RP14.PMEX)
      Store(0,\_SB.PC00.RP15.PMEX)
      Store(0,\_SB.PC00.RP16.PMEX)
      Store(0,\_SB.PC00.RP17.PMEX)
      Store(0,\_SB.PC00.RP18.PMEX)
      Store(0,\_SB.PC00.RP19.PMEX)
      Store(0,\_SB.PC00.RP20.PMEX)
      Store(0,\_SB.PC00.RP21.PMEX)
      Store(0,\_SB.PC00.RP22.PMEX)
      Store(0,\_SB.PC00.RP23.PMEX)
      Store(0,\_SB.PC00.RP24.PMEX)
      Store(0,\_SB.PC00.RP21.PMEX)
      Store(0,\_SB.PC00.RP22.PMEX)
      Store(0,\_SB.PC00.RP23.PMEX)
      Store(0,\_SB.PC00.RP24.PMEX)
      Store(0,\_SB.PC00.RP25.PMEX)
      Store(0,\_SB.PC00.RP26.PMEX)
      Store(0,\_SB.PC00.RP27.PMEX)
      Store(0,\_SB.PC00.RP28.PMEX)
    }
    If (CondRefOf(\_SB.PC00.TXHC)) {
      If (CondRefOf(\_SB.PC01)) {
        Store(0,\_SB.PC01.TRP0.PMEX)
        Store(0,\_SB.PC01.TRP1.PMEX)
        Store(0,\_SB.PC01.TRP2.PMEX)
        Store(0,\_SB.PC01.TRP3.PMEX)
      } Else {
        Store(0,\_SB.PC00.TRP0.PMEX)
        Store(0,\_SB.PC00.TRP1.PMEX)
        Store(0,\_SB.PC00.TRP2.PMEX)
        Store(0,\_SB.PC00.TRP3.PMEX)
      }
    }

    // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP01.PMSX)
    Store(1,\_SB.PC00.RP02.PMSX)
    Store(1,\_SB.PC00.RP03.PMSX)
    Store(1,\_SB.PC00.RP04.PMSX)
    Store(1,\_SB.PC00.RP05.PMSX)
    Store(1,\_SB.PC00.RP06.PMSX)
    Store(1,\_SB.PC00.RP07.PMSX)
    Store(1,\_SB.PC00.RP08.PMSX)
    Store(1,\_SB.PC00.RP09.PMSX)
    Store(1,\_SB.PC00.RP10.PMSX)
    Store(1,\_SB.PC00.RP11.PMSX)
    Store(1,\_SB.PC00.RP12.PMSX)
    If(LEqual(PCHS, PCHX)) {
      Store(1,\_SB.PC00.RP13.PMSX)
      Store(1,\_SB.PC00.RP14.PMSX)
      Store(1,\_SB.PC00.RP15.PMSX)
      Store(1,\_SB.PC00.RP16.PMSX)
      Store(1,\_SB.PC00.RP17.PMSX)
      Store(1,\_SB.PC00.RP18.PMSX)
      Store(1,\_SB.PC00.RP19.PMSX)
      Store(1,\_SB.PC00.RP20.PMSX)
      Store(1,\_SB.PC00.RP21.PMSX)
      Store(1,\_SB.PC00.RP22.PMSX)
      Store(1,\_SB.PC00.RP23.PMSX)
      Store(1,\_SB.PC00.RP24.PMSX)
      Store(1,\_SB.PC00.RP25.PMSX)
      Store(1,\_SB.PC00.RP26.PMSX)
      Store(1,\_SB.PC00.RP27.PMSX)
      Store(1,\_SB.PC00.RP28.PMSX)
    }

    If (CondRefOf(\_SB.PC00.TXHC)) {
      If (CondRefOf(\_SB.PC01)) {
        Store(1,\_SB.PC01.TRP0.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC01.TRP1.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC01.TRP2.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC01.TRP3.PMSX) // clear the PME SCI status bit
      } Else {
        Store(1,\_SB.PC00.TRP0.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC00.TRP1.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC00.TRP2.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC00.TRP3.PMSX) // clear the PME SCI status bit
      }
    }
  }

  // SOC Power Notification:
  //    Perform all needed OS notifications during a
  //    Power Switch.
  //
  Method(SPNT,0,Serialized) {

    If(And(\PF00,0x0400))
    {
      If (CondRefof (\_SB.PR00.TPSS)) {
        Subtract(SizeOf(\_SB.PR00.TPSS), One, \_SB.CPPC)
      }
    } Else {
      If (CondRefof (\_SB.PR00.LPSS)) {
        Subtract(SizeOf(\_SB.PR00.LPSS), One, \_SB.CPPC)
      }
    }

    //
    // If MP enabled and driver support is present, notify all
    // processors.
    //
    If(LGreater(TCNT, 1))
    {
      If(And(\PF00,0x0008)){
        Notify(\_SB.PR00,0x80)    // Eval PR00 _PPC.
      }
      If(And(\PF01,0x0008)){
        Notify(\_SB.PR01,0x80)    // Eval PR01 _PPC.
      }
      If(And(\PF02,0x0008)){
        Notify(\_SB.PR02,0x80)    // Eval PR02 _PPC.
      }
      If(And(\PF03,0x0008)){
        Notify(\_SB.PR03,0x80)    // Eval PR03 _PPC.
      }
      If(And(\PF04,0x0008)){
        Notify(\_SB.PR04,0x80)    // Eval PR04 _PPC.
      }
      If(And(\PF05,0x0008)){
        Notify(\_SB.PR05,0x80)    // Eval PR05 _PPC.
      }
      If(And(\PF06,0x0008)){
        Notify(\_SB.PR06,0x80)    // Eval PR06 _PPC.
      }
      If(And(\PF07,0x0008)){
        Notify(\_SB.PR07,0x80)    // Eval PR07 _PPC.
      }
      If(And(\PF08,0x0008)){
        Notify(\_SB.PR08,0x80)    // Eval PR08 _PPC.
      }
      If(And(\PF09,0x0008)){
        Notify(\_SB.PR09,0x80)    // Eval PR09 _PPC.
      }
      If(And(\PF10,0x0008)){
        Notify(\_SB.PR10,0x80)    // Eval PR10 _PPC.
      }
      If(And(\PF11,0x0008)){
        Notify(\_SB.PR11,0x80)    // Eval PR11 _PPC.
      }
      If(And(\PF12,0x0008)){
        Notify(\_SB.PR12,0x80)    // Eval PR12 _PPC.
      }
      If(And(\PF13,0x0008)){
        Notify(\_SB.PR13,0x80)    // Eval PR13 _PPC.
      }
      If(And(\PF14,0x0008)){
        Notify(\_SB.PR14,0x80)    // Eval PR14 _PPC.
      }
      If(And(\PF15,0x0008)){
        Notify(\_SB.PR15,0x80)    // Eval PR15 _PPC.
      }
    } Else {
      Notify(\_SB.PR00,0x80)      // Eval _PPC.
    }

    If(LGreater(TCNT, 1)){
      If(LAnd(And(\PF00,0x0008),And(\PF00,0x0010))){
        Notify(\_SB.PR00,0x81)  // Eval PR00 _CST.
      }
      If(LAnd(And(\PF01,0x0008),And(\PF01,0x0010))){
        Notify(\_SB.PR01,0x81)  // Eval PR01 _CST.
      }
      If(LAnd(And(\PF02,0x0008),And(\PF02,0x0010))){
        Notify(\_SB.PR02,0x81)  // Eval PR02 _CST.
      }
      If(LAnd(And(\PF03,0x0008),And(\PF03,0x0010))){
        Notify(\_SB.PR03,0x81)  // Eval PR03 _CST.
      }
      If(LAnd(And(\PF04,0x0008),And(\PF04,0x0010))){
        Notify(\_SB.PR04,0x81)  // Eval PR04 _CST.
      }
      If(LAnd(And(\PF05,0x0008),And(\PF05,0x0010))){
        Notify(\_SB.PR05,0x81)  // Eval PR05 _CST.
      }
      If(LAnd(And(\PF06,0x0008),And(\PF06,0x0010))){
        Notify(\_SB.PR06,0x81)  // Eval PR06 _CST.
      }
      If(LAnd(And(\PF07,0x0008),And(\PF07,0x0010))){
        Notify(\_SB.PR07,0x81)  // Eval PR07 _CST.
      }
      If(LAnd(And(\PF08,0x0008),And(\PF08,0x0010))){
        Notify(\_SB.PR08,0x81)  // Eval PR08 _CST.
      }
      If(LAnd(And(\PF09,0x0008),And(\PF09,0x0010))){
        Notify(\_SB.PR09,0x81)  // Eval PR09 _CST.
      }
      If(LAnd(And(\PF10,0x0008),And(\PF10,0x0010))){
        Notify(\_SB.PR10,0x81)  // Eval PR10 _CST.
      }
      If(LAnd(And(\PF11,0x0008),And(\PF11,0x0010))){
        Notify(\_SB.PR11,0x81)  // Eval PR11 _CST.
      }
      If(LAnd(And(\PF12,0x0008),And(\PF12,0x0010))){
        Notify(\_SB.PR12,0x81)  // Eval PR12 _CST.
      }
      If(LAnd(And(\PF13,0x0008),And(\PF13,0x0010))){
        Notify(\_SB.PR13,0x81)  // Eval PR13 _CST.
      }
      If(LAnd(And(\PF14,0x0008),And(\PF14,0x0010))){
        Notify(\_SB.PR14,0x81)  // Eval PR14 _CST.
      }
      If(LAnd(And(\PF15,0x0008),And(\PF15,0x0010))){
        Notify(\_SB.PR15,0x81)  // Eval PR15 _CST.
      }
    } Else {
      Notify(\_SB.PR00,0x81)      // Eval _CST.
    }
  } // end of Method(SPNT)

  //
  // SOC Prepare to Sleep
  // Designed to be called within platform specific _PTS
  //
  // Arg0 - sleep state - the numeric value of the Sx state
  //
  Method (SPTS, 1) {
    If (CondRefOf(\_SB.PC00.TXHC)) {
      //
      // Bring system out of TC cold before enter Sx
      //
      If (TRTD) {
        //
        // Bring System out of TC cold
        //
        \_SB.PC00.TCON ()
      }
    }
  }


  //
  // SOC Wake
  // Designed to be called within platform specific _WAK
  //
  // Arg0 - sleep state - the numeric value of the sleep state the system is waking from
  // Arg1 - Bit mask with Rp Number that will not recieve notification
  //        Example: Value of 0x180 -> 0000 0000 0000 0000 0000 0001 1000 0000 - will skip RP 7 and 8 notification
  //        Note: BIT0 is ignored
  //
  Method (SWAK, 2) {

    If(NEXP) {
      // Reinitialize the Native PCI Express after resume
      If(And(OSCC,0x01)) {
        NHPG()
      }
      // PME control granted?
      If(And(OSCC,0x04)) {
        NPME()
      }
    }

    If(LOr(LEqual(Arg0,3), LEqual(Arg0,4))) {  // If S3 or S4 Resume
      //
      // For PCI Express Express Cards, it is possible a device was
      // either inserted or removed during an Sx State.  The problem
      // is that no wake event will occur for a given warm inseration
      // or removal, so the OS will not become aware of any change.
      // To get around this, re-enumerate all Express Card slots.
      //
      // If the Root Port is enabled, it may be assumed to be hot-pluggable.
      //
      #define HOST_BUS \_SB.PC00
      ROOTPORT_NOTIFY (HOST_BUS, RP01, Arg1, 1)
      ROOTPORT_NOTIFY (HOST_BUS, RP02, Arg1, 2)
      ROOTPORT_NOTIFY (HOST_BUS, RP03, Arg1, 3)
      ROOTPORT_NOTIFY (HOST_BUS, RP04, Arg1, 4)
      ROOTPORT_NOTIFY (HOST_BUS, RP05, Arg1, 5)
      ROOTPORT_NOTIFY (HOST_BUS, RP06, Arg1, 6)
      ROOTPORT_NOTIFY (HOST_BUS, RP07, Arg1, 7)
      ROOTPORT_NOTIFY (HOST_BUS, RP08, Arg1, 8)
      ROOTPORT_NOTIFY (HOST_BUS, RP09, Arg1, 9)
      ROOTPORT_NOTIFY (HOST_BUS, RP10, Arg1, 10)
      ROOTPORT_NOTIFY (HOST_BUS, RP11, Arg1, 11)
      ROOTPORT_NOTIFY (HOST_BUS, RP12, Arg1, 12)
      If(LEqual(PCHS, PCHX)) {
        ROOTPORT_NOTIFY (HOST_BUS, RP13, Arg1, 13)
        ROOTPORT_NOTIFY (HOST_BUS, RP14, Arg1, 14)
        ROOTPORT_NOTIFY (HOST_BUS, RP15, Arg1, 15)
        ROOTPORT_NOTIFY (HOST_BUS, RP16, Arg1, 16)
        ROOTPORT_NOTIFY (HOST_BUS, RP17, Arg1, 17)
        ROOTPORT_NOTIFY (HOST_BUS, RP18, Arg1, 18)
        ROOTPORT_NOTIFY (HOST_BUS, RP19, Arg1, 19)
        ROOTPORT_NOTIFY (HOST_BUS, RP20, Arg1, 20)
        ROOTPORT_NOTIFY (HOST_BUS, RP21, Arg1, 21)
        ROOTPORT_NOTIFY (HOST_BUS, RP22, Arg1, 22)
        ROOTPORT_NOTIFY (HOST_BUS, RP23, Arg1, 23)
        ROOTPORT_NOTIFY (HOST_BUS, RP24, Arg1, 24)
        ROOTPORT_NOTIFY (HOST_BUS, RP25, Arg1, 25)
        ROOTPORT_NOTIFY (HOST_BUS, RP26, Arg1, 26)
        ROOTPORT_NOTIFY (HOST_BUS, RP27, Arg1, 27)
        ROOTPORT_NOTIFY (HOST_BUS, RP28, Arg1, 28)
      }
      #undef HOST_BUS

      If (CondRefOf(\_SB.PC00.TXHC)) {
        \_SB.TCWK(Arg0)
      }

    } // End If S3 or S4 Resume
  } // End SWAK

}
