/** @file
  This file contains the SSDT ASL code.
  It defines a set of SOC Common APIs

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
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
  External(NEXP, FieldUnitObj)
  External(OSCC, FieldUnitObj)

  #define HOST_BUS \_SB.PC00
  ROOTPORT_EXTERNAL (HOST_BUS, RP01)
  ROOTPORT_EXTERNAL (HOST_BUS, RP02)
  ROOTPORT_EXTERNAL (HOST_BUS, RP03)
  ROOTPORT_EXTERNAL (HOST_BUS, RP04)
  ROOTPORT_EXTERNAL (HOST_BUS, RP05)
  ROOTPORT_EXTERNAL (HOST_BUS, RP06)
  ROOTPORT_EXTERNAL (HOST_BUS, RP07)
  ROOTPORT_EXTERNAL (HOST_BUS, RP08)
  ROOTPORT_EXTERNAL (HOST_BUS, RP09)
  ROOTPORT_EXTERNAL (HOST_BUS, RP10)
  ROOTPORT_EXTERNAL (HOST_BUS, RP11)
  ROOTPORT_EXTERNAL (HOST_BUS, RP12)
  #undef HOST_BUS

  External (\_SB.PC00.TRP0.HPSX, IntObj)
  External (\_SB.PC00.TRP1.HPSX, IntObj)
  External (\_SB.PC00.TRP2.HPSX, IntObj)
  External (\_SB.PC00.TRP3.HPSX, IntObj)
  External (\_SB.PC00.TRP0.HPEX, IntObj)
  External (\_SB.PC00.TRP1.HPEX, IntObj)
  External (\_SB.PC00.TRP2.HPEX, IntObj)
  External (\_SB.PC00.TRP3.HPEX, IntObj)
  External (\_SB.PC00.TRP0.PMEX, IntObj)
  External (\_SB.PC00.TRP1.PMEX, IntObj)
  External (\_SB.PC00.TRP2.PMEX, IntObj)
  External (\_SB.PC00.TRP3.PMEX, IntObj)
  External (\_SB.PC00.TRP0.PMSX, IntObj)
  External (\_SB.PC00.TRP1.PMSX, IntObj)
  External (\_SB.PC00.TRP2.PMSX, IntObj)
  External (\_SB.PC00.TRP3.PMSX, IntObj)

  External (\_SB.PR00, DeviceObj)
  External (\_SB.PR01, DeviceObj)
  External (\_SB.PR02, DeviceObj)
  External (\_SB.PR03, DeviceObj)
  External (\_SB.PR04, DeviceObj)
  External (\_SB.PR05, DeviceObj)
  External (\_SB.PR06, DeviceObj)
  External (\_SB.PR07, DeviceObj)
  External (\_SB.PR08, DeviceObj)
  External (\_SB.PR09, DeviceObj)
  External (\_SB.PR10, DeviceObj)
  External (\_SB.PR11, DeviceObj)
  External (\_SB.PR12, DeviceObj)
  External (\_SB.PR13, DeviceObj)
  External (\_SB.PR14, DeviceObj)
  External (\_SB.PR15, DeviceObj)
  External (\_SB.PR16, DeviceObj)
  External (\_SB.PR17, DeviceObj)
  External (\_SB.PR18, DeviceObj)
  External (\_SB.PR19, DeviceObj)
  External (\_SB.PR20, DeviceObj)
  External (\_SB.PR21, DeviceObj)
  External (\_SB.PR22, DeviceObj)
  External (\_SB.PR23, DeviceObj)
  External (\_SB.PR24, DeviceObj)
  External (\_SB.PR25, DeviceObj)
  External (\_SB.PR26, DeviceObj)
  External (\_SB.PR27, DeviceObj)
  External (\_SB.PR28, DeviceObj)
  External (\_SB.PR29, DeviceObj)
  External (\_SB.PR30, DeviceObj)
  External (\_SB.PR31, DeviceObj)
  External (\_SB.PR32, DeviceObj)
  External (\_SB.PR33, DeviceObj)
  External (\_SB.PR34, DeviceObj)
  External (\_SB.PR35, DeviceObj)
  External (\_SB.PR36, DeviceObj)
  External (\_SB.PR37, DeviceObj)
  External (\_SB.PR38, DeviceObj)
  External (\_SB.PR39, DeviceObj)
  External (\_SB.PR40, DeviceObj)
  External (\_SB.PR41, DeviceObj)
  External (\_SB.PR42, DeviceObj)
  External (\_SB.PR43, DeviceObj)
  External (\_SB.PR44, DeviceObj)
  External (\_SB.PR45, DeviceObj)
  External (\_SB.PR46, DeviceObj)
  External (\_SB.PR47, DeviceObj)
  External (\_SB.PR48, DeviceObj)
  External (\_SB.PR49, DeviceObj)
  External (\_SB.PR50, DeviceObj)
  External (\_SB.PR51, DeviceObj)
  External (\_SB.PR52, DeviceObj)
  External (\_SB.PR53, DeviceObj)
  External (\_SB.PR54, DeviceObj)
  External (\_SB.PR55, DeviceObj)
  External (\_SB.PR56, DeviceObj)
  External (\_SB.PR57, DeviceObj)
  External (\_SB.PR58, DeviceObj)
  External (\_SB.PR59, DeviceObj)
  External (\_SB.PR60, DeviceObj)
  External (\_SB.PR61, DeviceObj)
  External (\_SB.PR62, DeviceObj)
  External (\_SB.PR63, DeviceObj)
  External (PF00, IntObj)
  External (PF01, IntObj)
  External (PF02, IntObj)
  External (PF03, IntObj)
  External (PF04, IntObj)
  External (PF05, IntObj)
  External (PF06, IntObj)
  External (PF07, IntObj)
  External (PF08, IntObj)
  External (PF09, IntObj)
  External (PF10, IntObj)
  External (PF11, IntObj)
  External (PF12, IntObj)
  External (PF13, IntObj)
  External (PF14, IntObj)
  External (PF15, IntObj)
  External (PF16, IntObj)
  External (PF17, IntObj)
  External (PF18, IntObj)
  External (PF19, IntObj)
  External (PF20, IntObj)
  External (PF21, IntObj)
  External (PF22, IntObj)
  External (PF23, IntObj)
  External (PF24, IntObj)
  External (PF25, IntObj)
  External (PF26, IntObj)
  External (PF27, IntObj)
  External (PF28, IntObj)
  External (PF29, IntObj)
  External (PF30, IntObj)
  External (PF31, IntObj)
  External (PF32, IntObj)
  External (PF33, IntObj)
  External (PF34, IntObj)
  External (PF35, IntObj)
  External (PF36, IntObj)
  External (PF37, IntObj)
  External (PF38, IntObj)
  External (PF39, IntObj)
  External (PF40, IntObj)
  External (PF41, IntObj)
  External (PF42, IntObj)
  External (PF43, IntObj)
  External (PF44, IntObj)
  External (PF45, IntObj)
  External (PF46, IntObj)
  External (PF47, IntObj)
  External (PF48, IntObj)
  External (PF49, IntObj)
  External (PF50, IntObj)
  External (PF51, IntObj)
  External (PF52, IntObj)
  External (PF53, IntObj)
  External (PF54, IntObj)
  External (PF55, IntObj)
  External (PF56, IntObj)
  External (PF57, IntObj)
  External (PF58, IntObj)
  External (PF59, IntObj)
  External (PF60, IntObj)
  External (PF61, IntObj)
  External (PF62, IntObj)
  External (PF63, IntObj)
  External (\_SB.PR00.TPSS, PkgObj)
  External (\_SB.PR00.LPSS, PkgObj)
  External (\_SB.CPPC, FieldUnitObj)
  External (\_SB.TCWK, MethodObj)

  External(\PCHS, FieldUnitObj)
  External(\PCHX, IntObj)

  //
  // Notify Hot Plug
  // - Clear Hot Plug SCI Enable on each root port
  //
  Method(NHPG,0,Serialized) {
    // clear the hot plug SCI enable bit
    If (CondRefOf (\_SB.PC00.RP01.HPEX)) {
      Store (0,\_SB.PC00.RP01.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP02.HPEX)) {
      Store (0,\_SB.PC00.RP02.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP03.HPEX)) {
      Store (0,\_SB.PC00.RP03.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP04.HPEX)) {
      Store (0,\_SB.PC00.RP04.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP05.HPEX)) {
      Store (0,\_SB.PC00.RP05.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP06.HPEX)) {
      Store (0,\_SB.PC00.RP06.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP07.HPEX)) {
      Store (0,\_SB.PC00.RP07.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP08.HPEX)) {
      Store (0,\_SB.PC00.RP08.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP09.HPEX)) {
      Store (0,\_SB.PC00.RP09.HPEX)
    }

    If (CondRefOf (\_SB.PC00.TXHC)) {
      Store (0,\_SB.PC00.TRP0.HPEX)
      Store (0,\_SB.PC00.TRP1.HPEX)
      Store (0,\_SB.PC00.TRP2.HPEX)
      Store (0,\_SB.PC00.TRP3.HPEX)
    }

    If (CondRefOf (\_SB.PC00.RP01.HPSX)) {
      Store (1,\_SB.PC00.RP01.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP02.HPSX)) {
      Store (1,\_SB.PC00.RP02.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP03.HPSX)) {
      Store (1,\_SB.PC00.RP03.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP04.HPSX)) {
      Store (1,\_SB.PC00.RP04.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP05.HPSX)) {
      Store (1,\_SB.PC00.RP05.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP06.HPSX)) {
      Store (1,\_SB.PC00.RP06.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP07.HPSX)) {
      Store (1,\_SB.PC00.RP07.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP08.HPSX)) {
      Store (1,\_SB.PC00.RP08.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP09.HPSX)) {
      Store (1,\_SB.PC00.RP09.HPSX)
    }

    If (CondRefOf (\_SB.PC00.TXHC)) {
      Store (1,\_SB.PC00.TRP0.HPSX) // clear the hot plug SCI status bit
      Store (1,\_SB.PC00.TRP1.HPSX) // clear the hot plug SCI status bit
      Store (1,\_SB.PC00.TRP2.HPSX) // clear the hot plug SCI status bit
      Store (1,\_SB.PC00.TRP3.HPSX) // clear the hot plug SCI status bit
    }
  }

  //
  // Notify PME
  // - Clear Power Management SCI Enable on each root port
  //
  Method(NPME,0,Serialized)
  {
    If (CondRefOf (\_SB.PC00.RP01.PMEX)) {
      Store (0,\_SB.PC00.RP01.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP02.PMEX)) {
      Store (0,\_SB.PC00.RP02.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP03.PMEX)) {
      Store (0,\_SB.PC00.RP03.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP04.PMEX)) {
      Store (0,\_SB.PC00.RP04.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP05.PMEX)) {
      Store (0,\_SB.PC00.RP05.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP06.PMEX)) {
      Store (0,\_SB.PC00.RP06.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP07.PMEX)) {
      Store (0,\_SB.PC00.RP07.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP08.PMEX)) {
      Store (0,\_SB.PC00.RP08.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP09.PMEX)) {
      Store (0,\_SB.PC00.RP09.PMEX)
    }

    If (CondRefOf (\_SB.PC00.TXHC)) {
      Store (0,\_SB.PC00.TRP0.PMEX)
      Store (0,\_SB.PC00.TRP1.PMEX)
      Store (0,\_SB.PC00.TRP2.PMEX)
      Store (0,\_SB.PC00.TRP3.PMEX)
    }

    // clear the PME SCI status bit
    If (CondRefOf (\_SB.PC00.RP01.PMSX)) {
      Store (1,\_SB.PC00.RP01.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP02.PMSX)) {
      Store (1,\_SB.PC00.RP02.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP03.PMSX)) {
      Store (1,\_SB.PC00.RP03.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP04.PMSX)) {
      Store (1,\_SB.PC00.RP04.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP05.PMSX)) {
      Store (1,\_SB.PC00.RP05.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP06.PMSX)) {
      Store (1,\_SB.PC00.RP06.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP07.PMSX)) {
      Store (1,\_SB.PC00.RP07.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP08.PMSX)) {
      Store (1,\_SB.PC00.RP08.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP09.PMSX)) {
      Store (1,\_SB.PC00.RP09.PMSX)
    }

    If (CondRefOf (\_SB.PC00.TXHC)) {
      Store (1,\_SB.PC00.TRP0.PMSX) // clear the PME SCI status bit
      Store (1,\_SB.PC00.TRP1.PMSX) // clear the PME SCI status bit
      Store (1,\_SB.PC00.TRP2.PMSX) // clear the PME SCI status bit
      Store (1,\_SB.PC00.TRP3.PMSX) // clear the PME SCI status bit
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
        Store(Zero, \_SB.CPPC) // Return the maximum performance state P0 for available performance range in P0 - Pn.
      }
    } Else {
      If (CondRefof (\_SB.PR00.LPSS)) {
        Store(Zero, \_SB.CPPC) // Return the maximum performance state P0 for available performance range in P0 - Pn.
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
      If (And (\PF16,0x0008)) {
        Notify (\_SB.PR16,0x80)    // Eval PR16 _PPC.
      }
      If (And (\PF17,0x0008)) {
        Notify (\_SB.PR17,0x80)    // Eval PR17 _PPC.
      }
      If (And (\PF18,0x0008)) {
        Notify (\_SB.PR18,0x80)    // Eval PR18 _PPC.
      }
      If (And (\PF19,0x0008)) {
        Notify (\_SB.PR19,0x80)    // Eval PR19 _PPC.
      }
      If (And (\PF20,0x0008)) {
        Notify (\_SB.PR20,0x80)    // Eval PR20 _PPC.
      }
      If (And (\PF21,0x0008)) {
        Notify (\_SB.PR21,0x80)    // Eval PR21 _PPC.
      }
      If (And (\PF22,0x0008)) {
        Notify (\_SB.PR22,0x80)    // Eval PR22 _PPC.
      }
      If (And (\PF23,0x0008)) {
        Notify (\_SB.PR23,0x80)    // Eval PR23 _PPC.
      }
      If (And (\PF24,0x0008)) {
        Notify (\_SB.PR24,0x80)    // Eval PR24 _PPC.
      }
      If (And (\PF25,0x0008)) {
        Notify (\_SB.PR25,0x80)    // Eval PR25 _PPC.
      }
      If (And (\PF26,0x0008)) {
        Notify (\_SB.PR26,0x80)    // Eval PR26 _PPC.
      }
      If (And (\PF27,0x0008)) {
        Notify (\_SB.PR27,0x80)    // Eval PR27 _PPC.
      }
      If (And (\PF28,0x0008)) {
        Notify (\_SB.PR28,0x80)    // Eval PR28 _PPC.
      }
      If (And (\PF29,0x0008)) {
        Notify (\_SB.PR29,0x80)    // Eval PR29 _PPC.
      }
      If (And (\PF30,0x0008)) {
        Notify (\_SB.PR30,0x80)    // Eval PR30 _PPC.
      }
      If (And (\PF31,0x0008)) {
        Notify (\_SB.PR31,0x80)    // Eval PR31 _PPC.
      }
      If (And (\PF32,0x0008)) {
        Notify (\_SB.PR32,0x80)    // Eval PR32 _PPC.
      }
      If (And (\PF33,0x0008)) {
        Notify (\_SB.PR33,0x80)    // Eval PR33 _PPC.
      }
      If (And (\PF34,0x0008)) {
        Notify (\_SB.PR34,0x80)    // Eval PR34 _PPC.
      }
      If (And (\PF35,0x0008)) {
        Notify (\_SB.PR35,0x80)    // Eval PR35 _PPC.
      }
      If (And (\PF36,0x0008)) {
        Notify (\_SB.PR36,0x80)    // Eval PR36 _PPC.
      }
      If (And (\PF37,0x0008)) {
        Notify (\_SB.PR37,0x80)    // Eval PR37 _PPC.
      }
      If (And (\PF38,0x0008)) {
        Notify (\_SB.PR38,0x80)    // Eval PR38 _PPC.
      }
      If (And (\PF39,0x0008)) {
        Notify (\_SB.PR39,0x80)    // Eval PR39 _PPC.
      }
      If (And (\PF40,0x0008)) {
        Notify (\_SB.PR40,0x80)    // Eval PR40 _PPC.
      }
      If (And (\PF41,0x0008)) {
        Notify (\_SB.PR41,0x80)    // Eval PR41 _PPC.
      }
      If (And (\PF42,0x0008)) {
        Notify (\_SB.PR42,0x80)    // Eval PR42 _PPC.
      }
      If (And (\PF43,0x0008)) {
        Notify (\_SB.PR43,0x80)    // Eval PR43 _PPC.
      }
      If (And (\PF44,0x0008)) {
        Notify (\_SB.PR44,0x80)    // Eval PR44 _PPC.
      }
      If (And (\PF45,0x0008)) {
        Notify (\_SB.PR45,0x80)    // Eval PR45 _PPC.
      }
      If (And (\PF46,0x0008)) {
        Notify (\_SB.PR46,0x80)    // Eval PR46 _PPC.
      }
      If (And (\PF47,0x0008)) {
        Notify (\_SB.PR47,0x80)    // Eval PR47 _PPC.
      }
      If (And (\PF48,0x0008)) {
        Notify (\_SB.PR48,0x80)    // Eval PR48 _PPC.
      }
      If (And (\PF49,0x0008)) {
        Notify (\_SB.PR49,0x80)    // Eval PR49 _PPC.
      }
      If (And (\PF50,0x0008)) {
        Notify (\_SB.PR50,0x80)    // Eval PR50 _PPC.
      }
      If (And (\PF51,0x0008)) {
        Notify (\_SB.PR51,0x80)    // Eval PR51 _PPC.
      }
      If (And (\PF52,0x0008)) {
        Notify (\_SB.PR52,0x80)    // Eval PR52 _PPC.
      }
      If (And (\PF53,0x0008)) {
        Notify (\_SB.PR53,0x80)    // Eval PR53 _PPC.
      }
      If (And (\PF54,0x0008)) {
        Notify (\_SB.PR54,0x80)    // Eval PR54 _PPC.
      }
      If (And (\PF55,0x0008)) {
        Notify (\_SB.PR55,0x80)    // Eval PR55 _PPC.
      }
      If (And (\PF56,0x0008)) {
        Notify (\_SB.PR56,0x80)    // Eval PR56 _PPC.
      }
      If (And (\PF57,0x0008)) {
        Notify (\_SB.PR57,0x80)    // Eval PR57 _PPC.
      }
      If (And (\PF58,0x0008)) {
        Notify (\_SB.PR58,0x80)    // Eval PR58 _PPC.
      }
      If (And (\PF59,0x0008)) {
        Notify (\_SB.PR59,0x80)    // Eval PR59 _PPC.
      }
      If (And (\PF60,0x0008)) {
        Notify (\_SB.PR60,0x80)    // Eval PR60 _PPC.
      }
      If (And (\PF61,0x0008)) {
        Notify (\_SB.PR61,0x80)    // Eval PR61 _PPC.
      }
      If (And (\PF62,0x0008)) {
        Notify (\_SB.PR62,0x80)    // Eval PR62 _PPC.
      }
      If (And (\PF63,0x0008)) {
        Notify (\_SB.PR63,0x80)    // Eval PR63 _PPC.
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
      If (LAnd (And (\PF16,0x0008),And (\PF16,0x0010))) {
        Notify (\_SB.PR16,0x81)  // Eval PR16 _CST.
      }
      If (LAnd (And (\PF17,0x0008),And (\PF17,0x0010))) {
        Notify (\_SB.PR17,0x81)  // Eval PR17 _CST.
      }
      If (LAnd (And (\PF18,0x0008),And (\PF18,0x0010))) {
        Notify (\_SB.PR18,0x81)  // Eval PR18 _CST.
      }
      If (LAnd (And (\PF19,0x0008),And (\PF19,0x0010))) {
        Notify (\_SB.PR19,0x81)  // Eval PR19 _CST.
      }
      If (LAnd (And (\PF20,0x0008),And (\PF20,0x0010))) {
        Notify (\_SB.PR20,0x81)  // Eval PR20 _CST.
      }
      If (LAnd (And (\PF21,0x0008),And (\PF21,0x0010))) {
        Notify (\_SB.PR21,0x81)  // Eval PR21 _CST.
      }
      If (LAnd (And (\PF22,0x0008),And (\PF22,0x0010))) {
        Notify (\_SB.PR22,0x81)  // Eval PR22 _CST.
      }
      If (LAnd (And (\PF23,0x0008),And (\PF23,0x0010))) {
        Notify (\_SB.PR23,0x81)  // Eval PR23 _CST.
      }
      If (LAnd (And (\PF24,0x0008),And (\PF24,0x0010))) {
        Notify (\_SB.PR24,0x81)  // Eval PR24 _CST.
      }
      If (LAnd (And (\PF25,0x0008),And (\PF25,0x0010))) {
        Notify (\_SB.PR25,0x81)  // Eval PR25 _CST.
      }
      If (LAnd (And (\PF26,0x0008),And (\PF26,0x0010))) {
        Notify (\_SB.PR26,0x81)  // Eval PR26 _CST.
      }
      If (LAnd (And (\PF27,0x0008),And (\PF27,0x0010))) {
        Notify (\_SB.PR27,0x81)  // Eval PR27 _CST.
      }
      If (LAnd (And (\PF28,0x0008),And (\PF28,0x0010))) {
        Notify (\_SB.PR28,0x81)  // Eval PR28 _CST.
      }
      If (LAnd (And (\PF29,0x0008),And (\PF29,0x0010))) {
        Notify (\_SB.PR29,0x81)  // Eval PR29 _CST.
      }
      If (LAnd (And (\PF30,0x0008),And (\PF30,0x0010))) {
        Notify (\_SB.PR30,0x81)  // Eval PR30 _CST.
      }
      If (LAnd (And (\PF31,0x0008),And (\PF31,0x0010))) {
        Notify (\_SB.PR31,0x81)  // Eval PR31 _CST.
      }
      If (LAnd (And (\PF32,0x0008),And (\PF32,0x0010))) {
        Notify (\_SB.PR32,0x81)  // Eval PR32 _CST.
      }
      If (LAnd (And (\PF33,0x0008),And (\PF33,0x0010))) {
        Notify (\_SB.PR33,0x81)  // Eval PR33 _CST.
      }
      If (LAnd (And (\PF34,0x0008),And (\PF34,0x0010))) {
        Notify (\_SB.PR34,0x81)  // Eval PR34 _CST.
      }
      If (LAnd (And (\PF35,0x0008),And (\PF35,0x0010))) {
        Notify (\_SB.PR35,0x81)  // Eval PR35 _CST.
      }
      If (LAnd (And (\PF36,0x0008),And (\PF36,0x0010))) {
        Notify (\_SB.PR36,0x81)  // Eval PR36 _CST.
      }
      If (LAnd (And (\PF37,0x0008),And (\PF37,0x0010))) {
        Notify (\_SB.PR37,0x81)  // Eval PR37 _CST.
      }
      If (LAnd (And (\PF38,0x0008),And (\PF38,0x0010))) {
        Notify (\_SB.PR38,0x81)  // Eval PR38 _CST.
      }
      If (LAnd (And (\PF39,0x0008),And (\PF39,0x0010))) {
        Notify (\_SB.PR39,0x81)  // Eval PR39 _CST.
      }
      If (LAnd (And (\PF40,0x0008),And (\PF40,0x0010))) {
        Notify (\_SB.PR40,0x81)  // Eval PR40 _CST.
      }
      If (LAnd (And (\PF41,0x0008),And (\PF41,0x0010))) {
        Notify (\_SB.PR41,0x81)  // Eval PR41 _CST.
      }
      If (LAnd (And (\PF42,0x0008),And (\PF42,0x0010))) {
        Notify (\_SB.PR42,0x81)  // Eval PR42 _CST.
      }
      If (LAnd (And (\PF43,0x0008),And (\PF43,0x0010))) {
        Notify (\_SB.PR43,0x81)  // Eval PR43 _CST.
      }
      If (LAnd (And (\PF44,0x0008),And (\PF44,0x0010))) {
        Notify (\_SB.PR44,0x81)  // Eval PR44 _CST.
      }
      If (LAnd (And (\PF45,0x0008),And (\PF45,0x0010))) {
        Notify (\_SB.PR45,0x81)  // Eval PR45 _CST.
      }
      If (LAnd (And (\PF46,0x0008),And (\PF46,0x0010))) {
        Notify (\_SB.PR46,0x81)  // Eval PR46 _CST.
      }
      If (LAnd (And (\PF47,0x0008),And (\PF47,0x0010))) {
        Notify (\_SB.PR47,0x81)  // Eval PR47 _CST.
      }
      If (LAnd (And (\PF48,0x0008),And (\PF48,0x0010))) {
        Notify (\_SB.PR48,0x81)  // Eval PR48 _CST.
      }
      If (LAnd (And (\PF49,0x0008),And (\PF49,0x0010))) {
        Notify (\_SB.PR49,0x81)  // Eval PR49 _CST.
      }
      If (LAnd (And (\PF50,0x0008),And (\PF50,0x0010))) {
        Notify (\_SB.PR50,0x81)  // Eval PR50 _CST.
      }
      If (LAnd (And (\PF51,0x0008),And (\PF51,0x0010))) {
        Notify (\_SB.PR51,0x81)  // Eval PR51 _CST.
      }
      If (LAnd (And (\PF52,0x0008),And (\PF52,0x0010))) {
        Notify (\_SB.PR52,0x81)  // Eval PR52 _CST.
      }
      If (LAnd (And (\PF53,0x0008),And (\PF53,0x0010))) {
        Notify (\_SB.PR53,0x81)  // Eval PR53 _CST.
      }
      If (LAnd (And (\PF54,0x0008),And (\PF54,0x0010))) {
        Notify (\_SB.PR54,0x81)  // Eval PR54 _CST.
      }
      If (LAnd (And (\PF55,0x0008),And (\PF55,0x0010))) {
        Notify (\_SB.PR55,0x81)  // Eval PR55 _CST.
      }
      If (LAnd (And (\PF56,0x0008),And (\PF56,0x0010))) {
        Notify (\_SB.PR56,0x81)  // Eval PR56 _CST.
      }
      If (LAnd (And (\PF57,0x0008),And (\PF57,0x0010))) {
        Notify (\_SB.PR57,0x81)  // Eval PR57 _CST.
      }
      If (LAnd (And (\PF58,0x0008),And (\PF58,0x0010))) {
        Notify (\_SB.PR58,0x81)  // Eval PR58 _CST.
      }
      If (LAnd (And (\PF59,0x0008),And (\PF59,0x0010))) {
        Notify (\_SB.PR59,0x81)  // Eval PR59 _CST.
      }
      If (LAnd (And (\PF60,0x0008),And (\PF60,0x0010))) {
        Notify (\_SB.PR60,0x81)  // Eval PR60 _CST.
      }
      If (LAnd (And (\PF61,0x0008),And (\PF61,0x0010))) {
        Notify (\_SB.PR61,0x81)  // Eval PR61 _CST.
      }
      If (LAnd (And (\PF62,0x0008),And (\PF62,0x0010))) {
        Notify (\_SB.PR62,0x81)  // Eval PR62 _CST.
      }
      If (LAnd (And (\PF63,0x0008),And (\PF63,0x0010))) {
        Notify (\_SB.PR63,0x81)  // Eval PR63 _CST.
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
      #undef HOST_BUS

      If (CondRefOf(\_SB.PC00.TXHC)) {
        \_SB.TCWK(Arg0)
      }

    } // End If S3 or S4 Resume
  } // End SWAK

}
