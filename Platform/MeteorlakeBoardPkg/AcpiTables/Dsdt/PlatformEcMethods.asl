/** @file
  ACPI DSDT table

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// The file include EC functional APIs
//

External (\LIDS, FieldUnitObj)
External (\_SB.PC00.GFX0.CLID)
External (\PWRS, FieldUnitObj)
External (SPNT, MethodObj)
External (BPNT, MethodObj)
External (\AC0F, FieldUnitObj)
External (\AC1F, FieldUnitObj)
External (\PBSS, FieldUnitObj)

External (\_SB.PC00.LPCB.H_EC, DeviceObj)
External (\_SB.PC00.LPCB.H_EC.UPCF, MethodObj)
External (\_SB.PC00.LPCB.H_EC.BAT0, DeviceObj)
External (\_SB.PC00.LPCB.H_EC.BAT1, DeviceObj)
External (\_SB.PC00.LPCB.H_EC.UPBT, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ONTM, IntObj)
External (\_SB.PC00.LPCB.H_EC.LID0, DeviceObj)
External (\_SB.PC00.LPCB.H_EC.ERVP, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERAP, MethodObj)
External (\_SB.PC00.LPCB.H_EC.UTEC, MethodObj)
External (\_SB.PC00.LPCB.H_EC.PGER, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERMT, MethodObj)
External (\_SB.PC00.LPCB.H_EC.UPFS, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERPH, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERSP, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERPM, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERPD, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERPH, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERBP, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERBA, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERPN, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERCF, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERPC, MethodObj)
External (\_SB.PC00.LPCB.H_EC.WPED, MethodObj)
External (\_SB.PC00.LPCB.H_EC.SPET, MethodObj)
External (\_SB.PC00.LPCB.H_EC.EPET, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERPS, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERPE, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ECNT, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ERLD, MethodObj)
External (\_SB.PC00.LPCB.H_EC.BNUM, IntObj)
External (\_SB.PC00.LPCB.H_EC.XDAT, MethodObj)
External (\_SB.SGOV, MethodObj)
External (\RPSG, FieldUnitObj)

External (\_SB.PWRB, DeviceObj)
External (\_SB.PWRB.PBST, IntObj)
External (\_TZ.TZ00._TMP, MethodObj)

External (\IGDS, FieldUnitObj)
External(DGBA)
External(\_SB.PC00.RP12.CLID)
External(\_SB.PC00.RP12.DLID, MethodObj)

#ifndef CONVERTIBLE_BUTTON
#define CONVERTIBLE_BUTTON   6
#endif
#ifndef DOCK_INDICATOR
#define DOCK_INDICATOR       7
#endif

/**
  Disable the CPU Fan if Active Cooling is disabled.

  @param    Arg0  - Boolean value to indicate if the Active Cooling is enabled
                    or disabled.
  @retval   None.
**/
Method (ACCF, 1, Serialized)
{
  if (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    if (LEqual (0, Arg0)) {
      \_SB.PC00.LPCB.H_EC.UPCF (0)
    }
  }
}

/**
  Notify the batery devices BAT0, BAT1
  Battery Information Changed

  @param    None.
  @retval   None.
**/
Method (NBIX, 0, Serialized)
{
  if (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    Notify (\_SB.PC00.LPCB.H_EC.BAT0, 0x81)       // Eval BAT0 _BIX.
    Notify (\_SB.PC00.LPCB.H_EC.BAT1, 0x81)       // Eval BAT1 _BIX.
  }
}

/**
  Notify the batery devices BAT0, BAT1
  Battery Status Changed

  @param    None.
  @retval   None.
**/
Method (NBST, 0, Serialized)
{
  if (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    Notify (\_SB.PC00.LPCB.H_EC.BAT0, 0x80)       // Eval BAT0 _BST.
    Notify (\_SB.PC00.LPCB.H_EC.BAT1, 0x80)       // Eval BAT1 _BST.
  }
}

/**
  Sleep Wake to update the devices (Button and Lid) status and notify the
  related devices.

  @param    Arg0  - SX value
  @param    Arg1  - GBSX value. Virtual GPIO button Notify Sleep State Change.
  @param    Arg2  - PB1E value. 10sec Power button support.
                    Bit0: 10 sec P-button Enable/Disable
                    Bit1: Internal Flag
                    Bit2: Rotation Lock flag, 0:unlock, 1:lock
                    Bit3: Slate/Laptop Mode Flag, 0: Slate, 1: Laptop
                    Bit4: Undock / Dock Flag, 0: Undock, 1: Dock
                    Bit5: VBDL Flag. 0: VBDL is not called, 1: VBDL is called, Virtual Button Driver is loaded.
                    Bit7-6: Reserved for future use.
  @param    Arg3  - IGDS value. IGD State (Primary Display = 1).
  @retval   None.
**/
Method (SWUB, 4, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    If (LOr (LEqual (Arg0, 3), LEqual (Arg0, 4))) {

      //
      // Update button status.
      //
      If (And (Arg1, 0x40)) {
        If (And (Arg2, 0x08)) {
          \_SB.PC00.LPCB.H_EC.UPBT (CONVERTIBLE_BUTTON, One)
        } Else {
          \_SB.PC00.LPCB.H_EC.UPBT (CONVERTIBLE_BUTTON, Zero)
        }
      }

      If (And (Arg1, 0x80)) {
        If (And (Arg2, 0x10)) {
          \_SB.PC00.LPCB.H_EC.UPBT (DOCK_INDICATOR, One)
        } Else {
          \_SB.PC00.LPCB.H_EC.UPBT (DOCK_INDICATOR, Zero)
        }
      }

      //
      // Prevent reinitializing Virtual GPIO Button status
      //
      Store (1, \_SB.PC00.LPCB.H_EC.ONTM)

      //
      // Update Lid state after S3 or S4 resume
      //
      Store (\_SB.PC00.LPCB.H_EC.ERLD (), LIDS)

      If (Arg3) {
        If (LEqual (LIDS, 0)) {
          Store (0x80000000, \_SB.PC00.GFX0.CLID)
        }
        If (LEqual (LIDS, 1)) {
          Store (0x80000003, \_SB.PC00.GFX0.CLID)
        }
      }
      Notify (\_SB.PC00.LPCB.H_EC.LID0, 0x80)

      If (LEqual (\_SB.PC00.LPCB.H_EC.BNUM,0)) {
        If(LNotEqual(\_SB.PC00.LPCB.H_EC.ERVP (), PWRS)) {
          Store(\_SB.PC00.LPCB.H_EC.ERVP (), PWRS)
          // Perform needed ACPI Notifications.
          SPNT ()
          BPNT ()
        }
      } Else {
        If (LNotEqual (\_SB.PC00.LPCB.H_EC.ERAP (), PWRS)) {
          Store (\_SB.PC00.LPCB.H_EC.ERAP (), PWRS)
          // Perform needed ACPI Notifications.
          SPNT ()
          BPNT ()
        }
      }

      //
      // If Using Control Method Power Button, notify PWRD device with 0x2
      //
      If (LEqual (\_SB.PWRB.PBST, 0x1)) {
        If (PBSS) { //Power Button woke the system
          Notify (\_SB.PWRB, 0x02) // Send release notification to Power Button device 0x02
          Store (1, PBSS)
        }
      }
    }
  }
}

/**
  While the Pseudo G3 is enabled, update the Watch dog registers inside EC
  RAM.

  @param  Arg0  - Wake time value for wake time mode 0x00
  @param  Arg1  - Wake time value for wake time mode 0x01
  @param  Arg2  - Wake time value for wake time mode 0x10
  @param  Arg3  - Wake time value for wake time mode 0x11
  @retval None.
**/
Method (PGWD, 4, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    // 1. Program Timer mode to 0x00 : WADT_AC register
    \_SB.PC00.LPCB.H_EC.UTEC (0x00, Arg0)
    // 2. Program Timer mode to 0x01 : WADT_DC register
    \_SB.PC00.LPCB.H_EC.UTEC (0x01, Arg1)
    // 3. Program Timer mode to 0x02 : WADT_EXP_AC register
    \_SB.PC00.LPCB.H_EC.UTEC (0x02, Arg2)
    // 4. Program Timer mode to 0x03 : WADT_EXP_DC register
    \_SB.PC00.LPCB.H_EC.UTEC (0x03, Arg3)
  }
}

/**

   Clearing PG3E and PG3X when entry S4 or S5 with DC mode (battery connected).

**/
Method (PGCL, 0, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    \_SB.PC00.LPCB.H_EC.PGER ()
  }
}
/**
  Get MAX Platform Temperature.

  @param  None
  @retval return an int value of max platform temperature.
**/
Method (MXTP, 0, Serialized)
{

  //
  // If EC is present, get the max platform temperature from EC.
  //
  if (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    Store (\_SB.PC00.LPCB.H_EC.ERMT (), Local0) // Max Platform temperature
    Add (2732, Multiply (Local0, 10), Local0)
    Return (Local0)
  }

  //
  // return a static value if no source is avaiable.
  //
  Return (3010)
}

/**
  Update the fan speed.

  @param  Arg0  - ETMD value to indicate ETM enable or disable.
  @param  Arg1  - Virtual Fan 0 status.
  @param  Arg2  - Virtual Fan 1 status.
  @retval None.
**/
Method (UPFS, 3, Serialized)
{
  If (LAnd (CondRefOf (\_SB.PC00.LPCB.H_EC), Arg0)) {
    // Virtual Fan0 and Fan1 are On - Set Real Fan to Full Speed
    If (LAnd (LNotEqual (Arg1, 0), LNotEqual (Arg2, 0)))
    {
      \_SB.PC00.LPCB.H_EC.UPFS (0, 0, AC0F) // AC0F
    }
    Else
    {
      //Virtual Fan0 is Off, Virtual Fan1 is On - Set Real Fan to 75% Speed
      If (LAnd (LEqual (Arg1, 0), LNotEqual (Arg2, 0))) {
        \_SB.PC00.LPCB.H_EC.UPFS (0, 0, AC1F) // AC1F
      } Else {
        //Otherwise - Turn the Real Fan Off
        \_SB.PC00.LPCB.H_EC.UPFS (0, 0, 0)
      }
    }
  }
}

//
// PTID Methods start
//

/**
  Temperature Sensor Data Dump.

  @param  Arg0  - TSDB value.
  @param  Arg1  - Temperature Sersor value pacakge object.
  @retval The updated temperature sensor value package.
**/
Method (TSDD, 2, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    Store (Add (Multiply (\_SB.PC00.LPCB.H_EC.ERPH (), 10), 2732), Local0)
    Store (\_SB.PC00.LPCB.H_EC.ERSP (), Local1)
    Store (\_SB.PC00.LPCB.H_EC.ERPM (), Local2)
    Store (\_SB.PC00.LPCB.H_EC.ERPD (), Local3)
    Store (\_SB.PC00.LPCB.H_EC.ERPH (), Local4)

    If (LEqual (Arg0, 1)) {
      Store (Local0, Index (Arg1, 0))
      Store (Local0, Index (Arg1, 1))
      Store (Local0, Index (Arg1, 2))
      Store (Local0, Index (Arg1, 3))
      Store (Local0, Index (Arg1, 4))
      Store (Add (DeRefOf (Index (Local1, 1)), 2732), Index (Arg1, 6))
      Store (Add (DeRefOf (Index (Local1, 2)), 2732), Index (Arg1, 7))
      Store (Add (DeRefOf (Index (Local1, 3)), 2732), Index (Arg1, 8))
      Store (Add (DeRefOf (Index (Local1, 4)), 2732), Index (Arg1, 9))
      Store (Add (Multiply (Local2, 10), 2732), Index (Arg1, 9))
      Store (Add (Multiply (Local3, 10), 2732), Index (Arg1, 10))
      Store (Add (Multiply (Local4, 10), 2732), Index (Arg1, 11))
    }
    Else {
      Store (Local0, Index (Arg1, 0))
      Store (Local0, Index (Arg1, 1))
      Store (Local0, Index (Arg1, 2))
      Store (Local0, Index (Arg1, 3))
      Store (Local0, Index (Arg1, 4))
      Store (Add (DeRefOf (Index (Local1, 0)), 2732), Index (Arg1, 5))
      Store (Add (DeRefOf (Index (Local1, 1)), 2732), Index (Arg1, 6))
      Store (Add (DeRefOf (Index (Local1, 2)), 2732), Index (Arg1, 7))
      Store (Add (DeRefOf (Index (Local1, 3)), 2732), Index (Arg1, 8))
      Store (Add (DeRefOf (Index (Local1, 4)), 2732), Index (Arg1, 9))
      Store (Add (Multiply (Local2, 10), 2732), Index (Arg1, 10))
      Store (Add (Multiply (Local3, 10), 2732), Index (Arg1, 11))
      Store (Add (Multiply (Local4, 10), 2732), Index (Arg1, 12))
    }
  }
  If (LEqual (Arg0, 1)) {
    Store (\_TZ.TZ00._TMP, Index (Arg1, 13))
  }
  Else {
    Store (\_TZ.TZ00._TMP, Index (Arg1, 14))
  }
  Return (Arg1)
}

/**
  Power Sensor Data Dump.

  @param  Arg0  - Platform Power value package object.
  @retval The updated platform power value package.
**/
Method (PSDD, 1, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    Store (0, Index (Arg0, 0))
    Multiply (0, 10, Index (Arg0, 1))
    Multiply (0, 10, Index (Arg0, 2))
    Store (\_SB.PC00.LPCB.H_EC.ERBP (), Local0)
    Store (DeRefOf (Index (Local0, 0)), Index (Arg0, 6))
    Store (DeRefOf (Index (Local0, 1)), Index (Arg0, 7))
    Store (DeRefOf (Index (Local0, 2)), Index (Arg0, 8))
    Store (DeRefOf (Index (Local0, 3)), Index (Arg0, 9))
    Store (DeRefOf (Index (Local0, 4)), Index (Arg0, 10))
    Store (DeRefOf (Index (Local0, 5)), Index (Arg0, 11))
    Store (DeRefOf (Index (Local0, 6)), Index (Arg0, 12))
    Store (DeRefOf (Index (Local0, 7)), Index (Arg0, 13))
    Store (DeRefOf (Index (Local0, 8)), Index (Arg0, 14))
    Store (DeRefOf (Index (Local0, 9)), Index (Arg0, 15))
    Store (DeRefOf (Index (Local0, 10)), Index (Arg0, 16))
    // convert PMAX from signed to unsigned integer
    // PMAX Value = (NOT(OR(FFFF0000, ECValue))+1) * 10.
    Store (\_SB.PC00.LPCB.H_EC.ERBA (), Local0)
    If (Local0){ // don't convert a zero
      Not (Or (0xFFFF0000, Local0, Local0), Local0) // make it a 32 bit value before inverting the bits
      Multiply (Add (1, Local0, Local0), 10, Local0) // add 1 and multiply by 10
    }
    Store (Local0, Index (Arg0, 17))
    // PMAX Value = (NOT(OR(FFFF0000, ECValue))+1) * 10.
    // PMAX is unsupported, return 0 directly.
    Store (0, Index (Arg0, 18))
  }
  Return (Arg0)
}

/**
  Other Sensor Data Dump.

  @param  Arg0  - TSDB value.
  @param  Arg1  - Ohter Sensor Data pacakge object.
  @retval The updated other sensor data package.
**/
Method (OSDD, 2, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    If (LEqual (Arg0, 1)) {
      Store (\_SB.PC00.LPCB.H_EC.ERPN(), Index (Arg1, 0))
      Store (\_SB.PC00.LPCB.H_EC.ERCF(), Index (Arg1, 1))
      Store (DeRefOf (Index (\_SB.PC00.LPCB.H_EC.ERSP (), 2)), Index (Arg1, 2))
    }
    Else {
      Store (\_SB.PC00.LPCB.H_EC.ERPN(), Index (Arg1, 0))
      Store (\_SB.PC00.LPCB.H_EC.ERCF(), Index (Arg1, 1))
    }
    Return (Arg1)
  }
  Return (Package () {})
}

/**
  Read PECI Mailbox Data.

  @param  None.
  @retval 26 bytes PECI data.
**/
Method (RPMD, 0, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    Return (\_SB.PC00.LPCB.H_EC.ERPC ())
  }
  Return (Buffer (26) {})
}

/**
  Write PECI Mailbox Data.

  @param  Arg0 - 26 bytes PECI data to write.
  @retval None.
**/
Method (WPMD, 1, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    \_SB.PC00.LPCB.H_EC.WPED (Arg0)
  }
}

/**
  Issue PECI Command.

  @param  None.
  @retval None.
**/
Method (ISPC, 0, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    \_SB.PC00.LPCB.H_EC.SPET ()
  }
}

/**
  End PECI Command.

  @param  None.
  @retval None.
**/
Method (ENPC)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    \_SB.PC00.LPCB.H_EC.EPET ()
  }
}

/**
  Read PECI Command Status.

  @param  None.
  @retval Return the PECI command status.
**/
Method (RPCS)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    Return (\_SB.PC00.LPCB.H_EC.ERPS ())
  }
  Return (0)
}

/**
  Read PECI Error Count.

  @param  None.
  @retval Return the PECI error count.
**/
Method (RPEC)
{
  Store (0, Local0)
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    Store (\_SB.PC00.LPCB.H_EC.ERPE(), Local1)
    Or(Local0, DeRefOf (Index (Local1, 0)), Local0)
    Or(Local0, ShiftLeft (DeRefOf (Index (Local1, 1)), 8), Local0)
    Or(Local0, ShiftLeft (DeRefOf (Index (Local1, 2)), 16), Local0)
    Or(Local0, ShiftLeft (DeRefOf (Index (Local1, 3)), 24), Local0)
  }
  Return (Local0)
}
//
// PTID Methods end
//

/**
  Update the Dock Indicator.

  @param  Arg0  - Boolean value to indicate enable/disable Dock.
                0 - disable
                1 - enable
  @retval None.
**/
Method (UPDK, 1, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC.UPBT)) {
    \_SB.PC00.LPCB.H_EC.UPBT (0x90, Arg0)
  }
}

/**
  EC CS notify, i.e notify idle resiliency.

  @param  Arg0  - Value to indicate the CS event.
                0 - exit
                1 - entry
  @retval None.
**/
Method (NTIR, 1, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC.ECNT)) {
    \_SB.PC00.LPCB.H_EC.ECNT (Arg0)
  }
}

/**
  Wake event happens, Ec would do blow check and do the related operation:
    1. Check for LID event
    2. Check for Power event

  @param  None.
  @retval None.
**/
Method (WKEC, 0, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    //
    // Check for Virtual Lid Event
    //
    If (LNotEqual (\LIDS, \_SB.PC00.LPCB.H_EC.ERLD ())) {
      //
      // Lid status changed, save Lid status first
      //
      Store (\_SB.PC00.LPCB.H_EC.ERLD (), LIDS)

      If (\IGDS) {
        //
        // Upon waking a lid event may fail if driver is not ready.
        // If it does, set the high bit of the variable to indicate that
        // another notification is required during system callbacks.
        //
        If (\_SB.PC00.GFX0.GLID (LIDS)) {
          Or (0x80000000,\_SB.PC00.GFX0.CLID, \_SB.PC00.GFX0.CLID)
        }
      }
      If(LNotEqual(DGBA, 0))
      {
        // Upon waking a lid event may fail if driver is not ready.
        // If it does, set the high bit of the variable to indicate that
        // another notification is required during system callbacks.
        If(\_SB.PC00.RP12.DLID(LIDS))
        {
          Or(0x80000000,\_SB.PC00.RP12.CLID, \_SB.PC00.RP12.CLID)
        }
      }
      Notify (\_SB.PC00.LPCB.H_EC.LID0, 0x80)
    }
    Else {
      //
      // Check for Virtual Power Event.
      //
      If (LEqual (\_SB.PC00.LPCB.H_EC.BNUM, 0)) {
        If (LNotEqual (\PWRS, \_SB.PC00.LPCB.H_EC.ERVP ()))
        {
          // Update NVS Power State.
          Store (\_SB.PC00.LPCB.H_EC.ERVP (), \PWRS)
          // Perform needed ACPI Notifications.
          SPNT ()
          BPNT ()
        }
      }

      // Make it an attended wake event, no matter what.
      Notify (\_SB.PWRB,0x02)
    }
  }
}

/**
  Notify Retimer Power State.
  1. config high when system is in S0 state
  2. config low when system is Sx/CS state
  @param  Arg0  - Retimer power state
  @retval None
**/
Method (NRTS, 1, Serialized)
{
    If (CondRefOf (RPSG)) {
      If (LNotEqual (RPSG, 0)) { // config Retimer Power State
        \_SB.SGOV (RPSG, Arg0)
      }
    }
}

/**
  XDAT: XDCI device attach/detach

  @param  None
  @retval 1 when device attach; 0 when device detach
**/
Method (XDAT, 0, Serialized)
{
  If (CondRefOf (\_SB.PC00.LPCB.H_EC)) {
    Return (\_SB.PC00.LPCB.H_EC.XDAT ())
  }
  Return (0)
}