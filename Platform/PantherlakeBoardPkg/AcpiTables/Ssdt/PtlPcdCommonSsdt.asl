/** @file
  This file contains the SSDT ASL code.
  It defines a set of SOC Common APIs

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
  External (\_SB.PC00.TXHC, DeviceObj)
  External (\_SB.PC00.TCON, MethodObj)
  External (\_SB.PC00.TG0N, MethodObj)
  External (\_SB.PC00.TG1N, MethodObj)

  External (TRTD, FieldUnitObj)
  External (ITRT, FieldUnitObj)
  External (TCNT, FieldUnitObj)

  External (\PCHS, FieldUnitObj)
  External (\PCHH, IntObj)
  External (\PCHX, IntObj)

  External (NEXP, FieldUnitObj)
  External (OSCC, FieldUnitObj)

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
  If (LEqual (PCHS, PCHH)) {
    ROOTPORT_EXTERNAL (HOST_BUS, RP11)
    ROOTPORT_EXTERNAL (HOST_BUS, RP12)
  }
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

  External (\_SB.PR00.TPSS, PkgObj)
  External (\_SB.PR00.LPSS, PkgObj)
  External (\_SB.CPPC, FieldUnitObj)
  External (\_SB.TCWK, MethodObj)

  External (\ISCL, FieldUnitObj)
  External (\_SB.ISEN, FieldUnitObj)
  External (\_SB.LL1E, FieldUnitObj)
  External (\_SB.LL2E, FieldUnitObj)
  External (\_SB.TBMD, FieldUnitObj)

  //
  // Notify Hot Plug
  // - Clear Hot Plug SCI Enable on each root port
  //
  Method (NHPG,0,Serialized) {
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
#if FixedPcdGetBool (PcdWclSupport) == 0

    If (CondRefOf (\_SB.PC00.RP07.HPEX)) {
      Store (0,\_SB.PC00.RP07.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP08.HPEX)) {
      Store (0,\_SB.PC00.RP08.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP09.HPEX)) {
      Store (0,\_SB.PC00.RP09.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP10.HPEX)) {
      Store (0,\_SB.PC00.RP10.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP11.HPEX)) {
      Store (0,\_SB.PC00.RP11.HPEX)
    }
    If (CondRefOf (\_SB.PC00.RP12.HPEX)) {
      Store (0,\_SB.PC00.RP12.HPEX)
    }
#endif
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
#if FixedPcdGetBool (PcdWclSupport) == 0

    If (CondRefOf (\_SB.PC00.RP07.HPSX)) {
      Store (1,\_SB.PC00.RP07.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP08.HPSX)) {
      Store (1,\_SB.PC00.RP08.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP09.HPSX)) {
      Store (1,\_SB.PC00.RP09.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP10.HPSX)) {
      Store (1,\_SB.PC00.RP10.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP11.HPSX)) {
      Store (1,\_SB.PC00.RP11.HPSX)
    }
    If (CondRefOf (\_SB.PC00.RP12.HPSX)) {
      Store (1,\_SB.PC00.RP12.HPSX)
    }
#endif
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
  Method (NPME,0,Serialized)
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
#if FixedPcdGetBool (PcdWclSupport) == 0

    If (CondRefOf (\_SB.PC00.RP07.PMEX)) {
      Store (0,\_SB.PC00.RP07.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP08.PMEX)) {
      Store (0,\_SB.PC00.RP08.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP09.PMEX)) {
      Store (0,\_SB.PC00.RP09.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP10.PMEX)) {
      Store (0,\_SB.PC00.RP10.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP11.PMEX)) {
      Store (0,\_SB.PC00.RP11.PMEX)
    }
    If (CondRefOf (\_SB.PC00.RP12.PMEX)) {
      Store (0,\_SB.PC00.RP12.PMEX)
    }
#endif

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
#if FixedPcdGetBool (PcdWclSupport) == 0

    If (CondRefOf (\_SB.PC00.RP07.PMSX)) {
      Store (1,\_SB.PC00.RP07.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP08.PMSX)) {
      Store (1,\_SB.PC00.RP08.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP09.PMSX)) {
      Store (1,\_SB.PC00.RP09.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP10.PMSX)) {
      Store (1,\_SB.PC00.RP10.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP11.PMSX)) {
      Store (1,\_SB.PC00.RP11.PMSX)
    }
    If (CondRefOf (\_SB.PC00.RP12.PMSX)) {
      Store (1,\_SB.PC00.RP12.PMSX)
    }
#endif
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
  Method (SPNT,0,Serialized) {
    //
    // _PPC shall return 0 only when Turbo mode enabled.
    //
    If (CondRefOf (\_SB.TBMD)) {
      If (LEqual (\_SB.TBMD, 1))
      {
        Store(Zero, \_SB.CPPC) // Return the maximum performance state P0 for available performance range from P0 to Pn.
      }
    }

    //
    // If MP enabled and driver support is present, notify all
    // processors.
    //
    If (LGreater (TCNT, 1))
    {
      If (And (\PF00,0x0008)) {
        Notify (\_SB.PR00,0x80)    // Eval PR00 _PPC.
      }
      If (And (\PF01,0x0008)) {
        Notify (\_SB.PR01,0x80)    // Eval PR01 _PPC.
      }
      If (And (\PF02,0x0008)) {
        Notify (\_SB.PR02,0x80)    // Eval PR02 _PPC.
      }
      If (And (\PF03,0x0008)) {
        Notify (\_SB.PR03,0x80)    // Eval PR03 _PPC.
      }
      If (And (\PF04,0x0008)) {
        Notify (\_SB.PR04,0x80)    // Eval PR04 _PPC.
      }
      If (And (\PF05,0x0008)) {
        Notify (\_SB.PR05,0x80)    // Eval PR05 _PPC.
      }
      If (And (\PF06,0x0008)) {
        Notify (\_SB.PR06,0x80)    // Eval PR06 _PPC.
      }
      If (And (\PF07,0x0008)) {
        Notify (\_SB.PR07,0x80)    // Eval PR07 _PPC.
      }
      If (And (\PF08,0x0008)) {
        Notify (\_SB.PR08,0x80)    // Eval PR08 _PPC.
      }
      If (And (\PF09,0x0008)) {
        Notify (\_SB.PR09,0x80)    // Eval PR09 _PPC.
      }
      If (And (\PF10,0x0008)) {
        Notify (\_SB.PR10,0x80)    // Eval PR10 _PPC.
      }
      If (And (\PF11,0x0008)) {
        Notify (\_SB.PR11,0x80)    // Eval PR11 _PPC.
      }
      If (And (\PF12,0x0008)) {
        Notify (\_SB.PR12,0x80)    // Eval PR12 _PPC.
      }
      If (And (\PF13,0x0008)) {
        Notify (\_SB.PR13,0x80)    // Eval PR13 _PPC.
      }
      If (And (\PF14,0x0008)) {
        Notify (\_SB.PR14,0x80)    // Eval PR14 _PPC.
      }
      If (And (\PF15,0x0008)) {
        Notify (\_SB.PR15,0x80)    // Eval PR15 _PPC.
      }
    } Else {
      Notify (\_SB.PR00,0x80)      // Eval _PPC.
    }

    If (LGreater (TCNT, 1)) {
      If (LAnd (And (\PF00,0x0008),And (\PF00,0x0010))) {
        Notify (\_SB.PR00,0x81)  // Eval PR00 _CST.
      }
      If (LAnd (And (\PF01,0x0008),And (\PF01,0x0010))) {
        Notify (\_SB.PR01,0x81)  // Eval PR01 _CST.
      }
      If (LAnd (And (\PF02,0x0008),And (\PF02,0x0010))) {
        Notify (\_SB.PR02,0x81)  // Eval PR02 _CST.
      }
      If (LAnd (And (\PF03,0x0008),And (\PF03,0x0010))) {
        Notify (\_SB.PR03,0x81)  // Eval PR03 _CST.
      }
      If (LAnd (And (\PF04,0x0008),And (\PF04,0x0010))) {
        Notify (\_SB.PR04,0x81)  // Eval PR04 _CST.
      }
      If (LAnd (And (\PF05,0x0008),And (\PF05,0x0010))) {
        Notify (\_SB.PR05,0x81)  // Eval PR05 _CST.
      }
      If (LAnd (And (\PF06,0x0008),And (\PF06,0x0010))) {
        Notify (\_SB.PR06,0x81)  // Eval PR06 _CST.
      }
      If (LAnd (And (\PF07,0x0008),And (\PF07,0x0010))) {
        Notify (\_SB.PR07,0x81)  // Eval PR07 _CST.
      }
      If (LAnd (And (\PF08,0x0008),And (\PF08,0x0010))) {
        Notify (\_SB.PR08,0x81)  // Eval PR08 _CST.
      }
      If (LAnd (And (\PF09,0x0008),And (\PF09,0x0010))) {
        Notify (\_SB.PR09,0x81)  // Eval PR09 _CST.
      }
      If (LAnd (And (\PF10,0x0008),And (\PF10,0x0010))) {
        Notify (\_SB.PR10,0x81)  // Eval PR10 _CST.
      }
      If (LAnd (And (\PF11,0x0008),And (\PF11,0x0010))) {
        Notify (\_SB.PR11,0x81)  // Eval PR11 _CST.
      }
      If (LAnd (And (\PF12,0x0008),And (\PF12,0x0010))) {
        Notify (\_SB.PR12,0x81)  // Eval PR12 _CST.
      }
      If (LAnd (And (\PF13,0x0008),And (\PF13,0x0010))) {
        Notify (\_SB.PR13,0x81)  // Eval PR13 _CST.
      }
      If (LAnd (And (\PF14,0x0008),And (\PF14,0x0010))) {
        Notify (\_SB.PR14,0x81)  // Eval PR14 _CST.
      }
      If (LAnd (And (\PF15,0x0008),And (\PF15,0x0010))) {
        Notify (\_SB.PR15,0x81)  // Eval PR15 _CST.
      }
    } Else {
      Notify (\_SB.PR00,0x81)    // Eval _CST.
    }
  } // end of Method (SPNT)

  //
  // Update the Isys register
  //
  // Arg0 - Integer value represent the AC state:
  //        0       - AC is not present.
  //        others  - AC or other power source plugin.
  //
  Method (UPIS, 1, Serialized) {
    If (CondRefOf (\_SB.ISEN)) {
      If (LEqual (\_SB.ISEN, 1)) {
        If (CondRefOf (\ISCL)) {
          Store (ISCL, Local0)
          //
          // Update Limit L1 Enable (BIT15) and L2 Enable (BIT47) base on the
          // Ac state:  AC      - Clear L1Enable and L2Enable
          //            DC only - Set L1Enable and L2Enable
          //
          If (CondRefOf (\_SB.LL1E)) {
            If (LEqual (\_SB.LL1E, 1)) {
              If (LNotEqual (Arg0, 0)) {
                And (Local0, ~BIT15, Local0)
              } Else {
                Or (Local0, BIT15, Local0)
              }
            }
          }

          If (CondRefOf (\_SB.LL2E)) {
            If (LEqual (\_SB.LL2E, 1)) {
              If (LNotEqual (Arg0, 0)) {
                And (Local0, ~BIT47, Local0)
              } Else {
                Or (Local0, BIT47, Local0)
              }
            }
          }
          Store (Local0, ISCL)
        }
      }
    }
  }


  //
  // SOC Prepare to Sleep
  // Designed to be called within platform specific _PTS
  //
  // Arg0 - sleep state - the numeric value of the Sx state
  //
  Method (SPTS, 1) {
    If (CondRefOf (\_SB.PC00.TXHC)) {
      //
      // Bring system out of TC cold before enter Sx
      //
      If (TRTD) {
        //
        // Bring System out of TC cold
        //
        \_SB.PC00.TCON ()
      }
      If (ITRT) {
        //
        // Bring TBT group0/1 out of D3 cold if it is in D3 cold
        //
        \_SB.PC00.TG0N ()
        \_SB.PC00.TG1N ()
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

    If (NEXP) {
      // Reinitialize the Native PCI Express after resume
      If (And (OSCC,0x01)) {
        NHPG ()
      }

      // PME control granted?
      If (And (OSCC,0x04)) {
        NPME ()
      }
    }

    If (LOr (LEqual (Arg0,3), LEqual (Arg0,4))) {  // If S3 or S4 Resume
      //
      // For PCI Express Cards, it is possible a device was
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
#if FixedPcdGetBool (PcdWclSupport) == 0

      ROOTPORT_NOTIFY (HOST_BUS, RP07, Arg1, 7)
      ROOTPORT_NOTIFY (HOST_BUS, RP08, Arg1, 8)
      ROOTPORT_NOTIFY (HOST_BUS, RP09, Arg1, 9)
      ROOTPORT_NOTIFY (HOST_BUS, RP10, Arg1, 10)
      If (LEqual (PCHS, PCHH)) {
        ROOTPORT_NOTIFY (HOST_BUS, RP11, Arg1, 11)
        ROOTPORT_NOTIFY (HOST_BUS, RP12, Arg1, 12)
      }
#endif
      #undef HOST_BUS

      If (CondRefOf(\_SB.PC00.TXHC)) {
        \_SB.TCWK(Arg0)
      }

    } // End If S3 or S4 Resume
  } // End SWAK

}
