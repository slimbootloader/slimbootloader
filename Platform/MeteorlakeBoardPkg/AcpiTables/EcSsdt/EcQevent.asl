/** @file
  ACPI Ec device methods and variables.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\PWRS)
External (\_SB.DPTF, IntObj)
External (\_SB.PWRE, IntObj)
External (\_SB.IETM.TPWR, DeviceObj)
External (\_SB.BATR, IntObj)
External (\_SB.IETM.BAT1, DeviceObj)
External (\BPNT, MethodObj)
External (\SPNT, MethodObj)

External (\P8XH, MethodObj)

External (\_SB.PC00.DOCK, DeviceObj)
External (\_SB.PC00.GFX0.IUER)
External (\IGDS, IntObj) // in SaNvs
External (\_SB.PC00.GFX0.GDCK, MethodObj)

External (\_SB.PC00.RP07, DeviceObj)
External (\DSTS, IntObj)

External (\LIDS)
External (\_SB.PC00.GFX0.GLID, MethodObj)
External (\_TZ.ETMD)
External (\_SB.IETM, DeviceObj)

External (\_SB.PC00.XDCI, DeviceObj)
External (\_SB.UBTC, DeviceObj)
External (\_SB.UBTC.MGI0, IntObj)
External (\_SB.UBTC.MGI1, IntObj)
External (\_SB.UBTC.MGI2, IntObj)
External (\_SB.UBTC.MGI3, IntObj)
External (\_SB.UBTC.MGI4, IntObj)
External (\_SB.UBTC.MGI5, IntObj)
External (\_SB.UBTC.MGI6, IntObj)
External (\_SB.UBTC.MGI7, IntObj)
External (\_SB.UBTC.MGI8, IntObj)
External (\_SB.UBTC.MGI9, IntObj)
External (\_SB.UBTC.MGIA, IntObj)
External (\_SB.UBTC.MGIB, IntObj)
External (\_SB.UBTC.MGIC, IntObj)
External (\_SB.UBTC.MGID, IntObj)
External (\_SB.UBTC.MGIE, IntObj)
External (\_SB.UBTC.MGIF, IntObj)
External (\_SB.UBTC.CCI0, IntObj)
External (\_SB.UBTC.CCI1, IntObj)
External (\_SB.UBTC.CCI2, IntObj)
External (\_SB.UBTC.CCI3, IntObj)

External (\HBRT, MethodObj)
External (\DSEN, IntObj)
External (\VBVP, IntObj)
External (\VBVD, IntObj)
External (\SMSS, IntObj)
External (\VBHB, IntObj)
External (\VBRL, IntObj)
External (\BRTN, MethodObj)
External (\_SB.PC00.GFX0.CBL1)
External (\_SB.PC00.GFX0.CBL2)
External (\_SB.PC00.GFX0.AINT, MethodObj)
External (\BRTL, IntObj)

External (\SCMK, IntObj)

External (\S0ID)
External (\UAMS)
External (\_SB.PWRB, DeviceObj)
External (\_SB.PC00.CTCU, MethodObj)
External (\CTDB, IntObj)
External (\_SB.PC00.CTCN, MethodObj)
External (\_SB.PC00.CTCD, MethodObj)
External (\_SB.PC00.TCPU, DeviceObj)
External (\_SB.PC00.TCPU.SPUR, MethodObj)
External (\_SB.PC00.TCPU.MAXT, IntObj)
External (\_SB.CLVL, FieldUnitObj)
External (\_SB.ODV0, IntObj)
External (\_SB.ODV1, IntObj)
External (\_SB.ODV2, IntObj)
External (\_SB.ODV3, IntObj)
External (\_SB.ODV4, IntObj)
External (\_SB.ODV5, IntObj)
External (\_SB.IETM.ODVP, MethodObj)

External (\OSYS)
External (\_SB.HIDD, DeviceObj)
External (\_SB.HIDD.BTLD)
External (\_SB.HIDD.HRDY, IntObj)
External (\_SB.HIDD.HPEM, MethodObj)
External (\_SB.PC00.GFX0.IUEH, MethodObj)

External (\_SB.WCCD, DeviceObj)
External (\DEVM, IntObj)

External (\_TZ.TZ00, DeviceObj)
External (\GPRV, MethodObj)             // PS_ON control method
External (PPOE, IntObj)
External (\_SB.IETM.DTNS, MethodObj)
External (\_SB.IETM.CHRG, DeviceObj)
External (\_SB.CHGE, IntObj)            // EnableChargerParticipant
External (\_SB.IETM.TFN1, DeviceObj)
External (\_SB.IETM.TFN2, DeviceObj)
External (\_SB.IETM.TFN3, DeviceObj)
External(\_SB.PC00.RP12.CBLV)
External(\_SB.PC00.RP12.DLID, MethodObj)
External(\_SB.PC00.RP12.HBRT, MethodObj)
External(\_SB.PC00.RP12.DINT, MethodObj)
External(DGBA)
External(DBRL)
External(DGDS)

#define CONVERTIBLE_BUTTON   6
#define DOCK_INDICATOR       7

//
// For the below _Qxx Methods, The Runtime SCI has been asserted,
// the EC Driver performed it's query, and the value returned from
// the Query = xx.
//
Method (_Q30)    // Real Battery AC Insertion Event.
{
  // Set Global Power State = AC Mode.

  Store (1, \PWRS)

  SPNT ()
  BPNT ()
}

Method (_Q31)    // Real Battery AC Removal Event.
{
  // Set Global Power State = Battery Mode.

  Store (0, \PWRS)

  SPNT ()
  BPNT ()
}

Method (_Q32)    // Real Battery Capacity Change.
{
  SPNT ()
  BPNT ()
}

Method (_Q33)    // Real Battery Insertion/Removal Event.
{
  // Initialize the Number of Present Batteries.
  //  1 = Real Battery 1 is present
  //  2 = Real Battery 2 is present
  //  3 = Real Battery 1 and 2 are present

  Store (0, BNUM)
  Or (BNUM, ShiftRight (And (ECRD (RefOf (B1ST)), 0x08), 3), BNUM)

  SPNT ()
  BPNT ()
}

Method (_Q37)    // Power Source Change SCI
{
  // PWRT (Power source type)
  // Bit0=1 if AC,
  // Bit1=1 if USB-PD,
  // Bit2=1 if Programmable Power Supply Enable
  P8XH (0, 0x37)

  And (ECRD (RefOf (PWRT)), 0x03, Local0)
  And (ECRD (RefOf (PWRT)), 0x04, Local1)

  If (LOr (LEqual (Local0, 1), LEqual (Local0, 2))) {
    Store (1, \PWRS)     // AC or USB-PD Insertion
  } ElseIf (LEqual (Local0, 0x0)) {
    Store (0, \PWRS)     // AC or USB-PD Removal
  }

  SPNT ()
  BPNT ()
}

Method (_Q40, 0, Serialized)   // Dock complete Event
{
  P8XH (0, 0x40)
  Return ()
}

Method (_Q41)    // Surprise Removal
{
  P8XH (0, 0x41)
  Return ()
}

Method (_Q42)    // Undocking request Event
{
  Notify (\_SB.PC00.DOCK, 0x01)

  If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
  } Else {
    And (\PB1E, Not (0x10), \PB1E) // Clear Dock Status
    UPBT (DOCK_INDICATOR, Zero)
    If (CondRefOf (\_SB.PC00.GFX0.IUER))
    {
      Store (\_SB.PC00.GFX0.IUER, Local0)
      And (Local0, Not (0x80), \_SB.PC00.GFX0.IUER)
    }
    Notify (\_SB.HIDD, 0xCB) // Notify HID Event Filter driver that Undocking event happened.
    If (\IGDS)
    {
      \_SB.PC00.GFX0.GDCK (0)
    }
  }
}

Method (_Q43)    // Express Card Presence Changed on Saddlestring (Docking)
{
  Notify (\_SB.PC00.RP07, 0x0)
}

Method (_Q44)  // Virtual Dock Presence Changed
{
  If (\DSTS)
  {
    Notify (\_SB.PC00.DOCK, 1) //UnDocked
    Store (0, \DSTS)
  }
  Else
  {
    Notify (\_SB.PC00.DOCK, 0) //Docked
    Store (1, \DSTS)
  }
}

Method (_Q51)    // Lid Switch Event.
{
  // Update Lid NVS State.

  Store (ECRD (RefOf (LSTE)), LIDS)

  \_SB.PC00.GFX0.GLID (LIDS)
  // Call DLID method to update DG LFP Lid status if DG is present
  If (LNotEqual(DGBA, 0))
  {
    \_SB.PC00.RP12.DLID(LIDS)
  }

  // If ETM is not disabled, notify IETM device
  If (CondRefOf (\_TZ.ETMD))
  {
    If (LEqual (\_TZ.ETMD, 0))
    {
      If (CondRefOf (\_SB.IETM))
      {
        Notify (\_SB.IETM, 0x83)
      }
    }
  }
  Notify (LID0, 0x80)
}

Name (UCPL, 450) // USBC Max power limit in 4.5 W in the system. A granularity unit is 10mW.
//
// USBC Power budget allocation
// This method will be called by EC _Q76 event to update current USBC total power used by the system.
// It will be updated to reflect last successfully negociated value.
//
// Arg0 - New requested auxiliary power from the EC in 10mW.
//
Method (UPBA, 1, Serialized) {
  If (LGreater (Arg0, UCPL)) {
    GPRV (2, 1) // Block core power removal
  } Else {
    GPRV (2, 0) // Allow for core power removal
  }
}

Method (UAXL, 0, Serialized) {
  Store (ECRD (RefOf (TSPW)), Local0)
  UPBA (Local0) // USBC Power budget allocation
}

 Method (_Q76)
{
    // PPOE is the BIOS setting for PD PS_ON Enable
    If (LEqual (PPOE, 1)) {
      UAXL ()
    }
}

//
// SCI handler for USB device Attach/Detach notification
//

// XDAT: XDCI device attach/detach
// Return 1 when device attach
// Return 0 when device detach
Method (XDAT)
{
  // 0 = Detached, 1 = Attached.
  Return (ECRD (RefOf (VBUS)))
}

Method (_Q77)
{
  If (LEqual (XDAT (), 1)){
    Notify (\_SB.PC00.XDCI, 0x80)
  }
}

Method (UCEV, 0, Serialized)    // Any event on USBTypeC.
{
    P8XH (0, 0x79)
    Store ( Timer (), Local0 )
    Store ( MGI0, \_SB.UBTC.MGI0 )
    Store ( MGI1, \_SB.UBTC.MGI1 )
    Store ( MGI2, \_SB.UBTC.MGI2 )
    Store ( MGI3, \_SB.UBTC.MGI3 )
    Store ( MGI4, \_SB.UBTC.MGI4 )
    Store ( MGI5, \_SB.UBTC.MGI5 )
    Store ( MGI6, \_SB.UBTC.MGI6 )
    Store ( MGI7, \_SB.UBTC.MGI7 )
    Store ( MGI8, \_SB.UBTC.MGI8 )
    Store ( MGI9, \_SB.UBTC.MGI9 )
    Store ( MGIA, \_SB.UBTC.MGIA )
    Store ( MGIB, \_SB.UBTC.MGIB )
    Store ( MGIC, \_SB.UBTC.MGIC )
    Store ( MGID, \_SB.UBTC.MGID )
    Store ( MGIE, \_SB.UBTC.MGIE )
    Store ( MGIF, \_SB.UBTC.MGIF )
    Store ( CCI0, \_SB.UBTC.CCI0 )
    Store ( CCI1, \_SB.UBTC.CCI1 )
    Store ( CCI2, \_SB.UBTC.CCI2 )
    Store ( CCI3, \_SB.UBTC.CCI3 )

    Local0 = Timer () - Local0
    Notify (\_SB.UBTC, 0x80)
}
Method (_Q79)    // Any event on USBTypeC.
{
  UCEV ()
}

// Display Hotkey Control Function
//    Handle display hotkey control functions, such as display switch, display brightness increase/decrease
//
//  Arguments:
//    Arg0: Function Number
//            1 = Reserved
//            2 = Display Brightness Increase
//            3 = Display Brightness Decrease
//            4+ = Reserved
//
//  Return Value:
//    None
Method (DHCF, 1, Serialized)
{
  Switch (ToInteger (Arg0))
  {
    Case (2) // Brightness Increase.
    {
      If (CondRefOf (HBRT)) // Send backlight notifications to the DGPU LFP device.
      {
        HBRT (3)
      }
      If (And (4, \DSEN))
      {
        BRTN (0x86)
      } Else {
        // Current brightness is a percentage number. This must be done if ALS is enabled,
        // as the driver will change the brightness based on ambient light.
        Store (\_SB.PC00.GFX0.CBL1, Local0)

        // 1 % will be lost in the conversion, so need to make it up first.
        And (Add (Local0, 1), 0xFE, Local0)

        If (LLessEqual (Local0, 90))
        {
          Add (Local0, 10, Local0)
        }
        Store (Local0, \BRTL)
        \_SB.PC00.GFX0.AINT (1, Local0)
      }

      If (LNotEqual(DGBA, 0))
        {
          If (CondRefOf(\_SB.PC00.RP12.HBRT)) // Send backlight notifications to the DGPU LFP device.
          {
            \_SB.PC00.RP12.HBRT(3)
          }
          If (And(4,DGDS))
          {
            BRTN(0x86)
          } Else {
            // Current brightness is a percentage number. This must be done if ALS is enabled,
            // as the driver will change the brightness based on ambient light.
            Store(\_SB.PC00.RP12.CBLV, Local0)

            // 1 % will be lost in the conversion, so need to make it up first.
            AND(Add(Local0, 1),0xFE, Local0)

            If (LLessEqual(Local0, 90))
            {
              Add(Local0, 10, Local0)
            }
            Store(Local0, DBRL)
            \_SB.PC00.RP12.DINT(1, Local0)
          }
        }
    }
    Case (3) // Brightness Decrease.
    {
      If (CondRefOf (HBRT)) // Send backlight notifications to the DGPU LFP device.
      {
        HBRT (4)
      }
      If (And (4, \DSEN))
      {
        BRTN (0x87)
      } Else {
        // Current brightness is a percentage number. This must be done if ALS is enabled,
        // as the driver will change the brightness based on ambient light.
        Store (\_SB.PC00.GFX0.CBL1, Local0)

        // 1 % will be lost in the conversion, so need to make it up first.
        And (Add (Local0, 1), 0xFE, Local0)

        If (LGreaterEqual (Local0, 10))
        {
          Subtract (Local0, 10, Local0)
        }
        Store (Local0, \BRTL)
        \_SB.PC00.GFX0.AINT (1, Local0)
      }

      If (LNotEqual(DGBA, 0))
        {
          If (CondRefOf(\_SB.PC00.RP12.HBRT)) // Send backlight notifications to the DGPU LFP device.
          {
            \_SB.PC00.RP12.HBRT(4)
          }
          If (And(4,DGDS))
          {
            BRTN(0x87)
          } Else {
            // Current brightness is a percentage number. This must be done if ALS is enabled,
            // as the driver will change the brightness based on ambient light.
            Store(\_SB.PC00.RP12.CBLV, Local0)

            // 1 % will be lost in the conversion, so need to make it up first.
            AND(Add(Local0, 1),0xFE, Local0)

            If (LGreaterEqual(Local0, 10))
            {
              Subtract(Local0, 10, Local0)
            }
            Store(Local0, DBRL)
            \_SB.PC00.RP12.DINT(1, Local0)
          }
        }
    }
  }
}

Method (_Q52)    // Hot-Key Event.
{
  //Return if Scan Matrix Keyboard Support disabled
  If (LEqual (\SCMK, 0))
  {
    Return
  }

  // This event will be generated whenever a
  // FN+"Next Key" is hit.  This event handler
  // will base code execution on the "Next Key" Scan Code
  // stored in the EC Memory Space.

  Store (ECRD (RefOf (SCAN)), Local0)

  // Fn + F1 - F8 & F12, EC would send the Scan code to OS directly
  // Function key | OS behavior           | EC behavior
  // -------------+-----------------------+---------------------------------------------------------------------
  // F1           | Mute                  | Send scan code to OS directly (press: 0xE0 0x20, release: 0xE0 0xA0)
  // F2           | Volume down           | Send scan code to OS directly (press: 0xE0 0x2E, release: 0xE0 0xAE)
  // F3           | Volume up             | Send scan code to OS directly (press: 0xE0 0x30, release: 0xE0 0xB0)
  // F4           | Play/Pause            | Send scan code to OS directly (press: 0xE0 0x22, release: 0xE0 0xA2)
  // F5           | Insert                | Send scan code to OS directly (press: 0xE0 0x52, release: 0xE0 0xD2)
  // F6           | Print Screen          | Send scan code to OS directly (press: 0xE0 0x37, release: 0xE0 0xB7)
  // F7           | Blank                 | Nothing
  // F8           | NumLock               | Send scan code to OS directly (press: 0x45, release: 0xC5)
  // F9           | Display brightness -  | Send SCI: Q52 with scan code 0x43
  // F10          | Display brightness +  | Send SCI: Q52 with scan code 0x44
  // F11          | Airplane mode         | Send SCI: Q52 with scan code 0x45
  // F12          | Scroll lock           | Send scan code to OS directly (press: 0x46, release: 0xC6)

  // F9 Scan Code = 0x43
  If (LEqual (Local0, 0x43))
  {
    DHCF (3)
  }

  // F10 Scan Code = 0x44
  ElseIf (LEqual (Local0, 0x44))
  {
    DHCF (2)
  }

  // F11 Scan Code = 0x45
  ElseIf (LEqual (Local0, 0x45))
  {
    \_SB.HIDD.HPEM (8)         // HID Event Index 8: Airplane Mode/Wireless Radio Button
  }
} // end Method (_Q52) Fn hot key event

Method (_Q54)    // Power Button Event for Control method Power Button (10sec PB Override without V-GPIO driver)
{
  If (LEqual (\S0ID, 0)) {  // if Connected Standby is not supported

    //
    // Check UAMS (User Absent Mode State) to notify the power button event.
    //
    If (\UAMS){ // UAMS has a non-Zero value, means the system is under User Absent Mode. Send Wake up event.
      If (CondRefOf (\_SB.PWRB)){
        Notify (\_SB.PWRB, 0x02)
      }
    } Else { // UAMS is Zero, means the system is ON. Send Sleep event.
      If (CondRefOf (\_SB.PWRB)){
        Notify (\_SB.PWRB, 0x80)
      }
    }
  }
} // end Method (_Q54) Power button event.

// Battery Test Method:
//    This method updates the Virtual Battery percentages and
//    notifies the OS of these changes.
//
//  Arguments:
//    Arg0: The desired Virtual Battery Percentage.
//
//  Return Value:
//    None
Method (BTTM, 1, Serialized)
{
  If (\PWRS)
  {
    If (LGreaterEqual (Arg0, B0SC))
    {
      Store (Arg0, B0SC)
      Notify (BAT0, 0x80)
    }
  }
  Else
  {
    If (LLessEqual (Arg0, B0SC))
    {
      Store (Arg0, B0SC)
      Notify (BAT0, 0x80)
    }
  }
}

//
// Virtual Battery Hot Keys:
//    This method passes the hot key scan code to the BTTM method.
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method (VBHK, 0, Serialized)
{
  // 1/! Scan Code = 0x02

  Store (ECRD (RefOf (SCCK)), Local0)

  If (LEqual (Local0, 0x02)) //  Battery = 10%.
  {
    BTTM (10)
  }

  // 2/@ Scan Code = 0x03

  If (LEqual (Local0, 0x03)) //  Battery = 20%.
  {
    BTTM (20)
  }

  // 3/# Scan Code = 0x04

  If (LEqual (Local0, 0x04)) //  Battery = 30%.
  {
    BTTM (30)
  }

  // 4/$ Scan Code = 0x05

  If (LEqual (Local0, 0x05)) //  Battery = 40%.
  {
    BTTM (40)
  }

  // 5/% Scan Code = 0x06

  If (LEqual (Local0, 0x06)) //  Battery = 50%.
  {
    BTTM (50)
  }

  // 6/^ Scan Code = 0x07

  If (LEqual (Local0, 0x07)) //  Battery = 60%.
  {
    BTTM (60)
  }

  // 7/& Scan Code = 0x08

  If (LEqual (Local0, 0x08)) //  Battery = 70%.
  {
    BTTM (70)
  }

  // 8/* Scan Code = 0x09

  If (LEqual (Local0, 0x09)) //  Battery = 80%.
  {
    BTTM (80)
  }

  // 9/( Scan Code = 0x0A

  If (LEqual (Local0, 0x0A)) //  Battery = 90%.
  {
    BTTM (90)
  }

  // 0/) Scan Code = 0x0B

  If (LEqual (Local0, 0x0B)) // Battery = 100%.
  {
    BTTM (100)
  }

  // _/- Scan Code = 0x0C

  If (LEqual (Local0, 0x0C)) // Battery = Battery - 2%.
  {
    If (LGreaterEqual (B0SC, 2))
    {
      BTTM (Subtract (B0SC, 2))
    }
  }

  // +/= Scan Code = 0x0D

  If (LEqual (Local0, 0x0D)) // Battery = Battery + 2%.
  {
    If (LLessEqual (B0SC, 98))
    {
      BTTM (Add (B0SC, 2))
    }
  }

  // F5 Scan Code = 0x3F

  If (LEqual (Local0, 0x3F))   // Virtual Power State Change.
  {
  // Check for Virtual Battery Mode.  If the CRB is
  // running on real batteries, skip this whole section.

    If (LEqual (BNUM, 0))
    {
      // Toggle the Virtual Power State.
      Xor (\PWRS, 1, \PWRS)

      // Perform needed ACPI Notifications.
      SPNT ()
      BPNT ()
    }
  }
}

Name (ABCD, 0) // used as counter by hotkeys

Method (_Q56)    // Hot-Key Event.
{
  //Return if Scan Matrix Keyboard Support disabled
  If (LEqual (\SCMK, 0))
  {
    Return
  }

  Store (ECRD (RefOf (SCCK)), Local0)
  // This event will be generated whenever a
  // CRTL+ALT+SHIFT+"Next Key" is hit.  This event handler
  // will base code execution on the "Next Key" Scan Code
  // stored in the EC Memory Space.

  // The scan code is between 1 - 0(0x02-0x0B), and -/+(0x0C, 0x0D).
  If (LAnd (LGreaterEqual (Local0, 0x02), LLessEqual (Local0, 0x0D)))
  {
    // Virtual Battery Hot Keys (VBHK) - VBHK uses keys 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -, + and F5.
    If (LEqual (BNUM, 0))  // Check for Virtual Battery Mode before testing the hot-keys specific to Virtual Battery Support.
    {
      VBHK ()  //  remove this call if no virtual battery is needed.
    }
  }

  // "BKSP" Scan Code = 0x0E
  If (LEqual (Local0, 0x0E))
  {
    \_SB.HIDD.HPEM (14)        // HID Event Index 14: Stop.
  }

  // "Tab" Scan Code = 0x0F
  If (LEqual (Local0, 0x0F))
  {
    \_SB.HIDD.HPEM (15)        // HID Event Index 15: Play/Pause.
  }

  // "Q" Scan Code = 0x10
  If (LEqual (Local0, 0x10))
  {
    Sleep (1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
    \_SB.HIDD.HPEM (1)         // HID Event Index 1: Windows Button (Keyboard Left GUI).
  }

  // "W" Scan Code = 0x11
  ElseIf (LEqual (Local0, 0x11))
  {
    Sleep (1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
    \_SB.HIDD.HPEM (2)         // HID Event Index 2: Rotation Lock
  }

  // "E" Scan Code = 0x12
  ElseIf (LEqual (Local0, 0x12))
  {
    Sleep (1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
    \_SB.HIDD.HPEM (3)         // HID Event Index 3: Num Lock
  }

  // "R" Scan Code = 0x13
  ElseIf (LEqual (Local0, 0x13))
  {
    Sleep (1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
    \_SB.HIDD.HPEM (4)         // HID Event Index 4: Home
  }

  // "T" Scan Code = 0x14
  ElseIf (LEqual (Local0, 0x14))
  {
    Sleep (1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
    \_SB.HIDD.HPEM (5)         // HID Event Index 5: End
  }

  // "Y" Scan Code = 0x15
  ElseIf (LEqual (Local0, 0x15))
  {
    Sleep (1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
    \_SB.HIDD.HPEM (6)         // HID Event Index 6: Page Up
  }

  // "U" Scan Code = 0x16
  ElseIf (LEqual (Local0, 0x16))
  {
    Sleep (1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
    \_SB.HIDD.HPEM (7)         // HID Event Index 7: Page Down
  }

  // "I" Scan Code = 0x17
  ElseIf (LEqual (Local0, 0x17))
  {
    \_SB.HIDD.HPEM (8)         // HID Event Index 8: Wireless Radio Button
  }

  // "O" Scan Code = 0x18
  ElseIf (LEqual (Local0, 0x18))
  {
    \_SB.HIDD.HPEM (9)         // HID Event Index 9: System Power Down
  }

  // HK: "P" Scan Code = 0x19
  ElseIf (LEqual (Local0, 0x19))   // CTDP Up
  {
    If (LAnd (CondRefOf (\_SB.PC00.CTCU), LEqual (\CTDB, 1))){ // only allow this feature if it is enabled in SETUP
      \_SB.PC00.CTCU ()
    }
  }

  // HK: "[" Scan Code = 0x1A
  ElseIf (LEqual (Local0, 0x1A))   // CTDP Nominal
  {
    If (LAnd (CondRefOf (\_SB.PC00.CTCN), LEqual (\CTDB, 1))){ // only allow this feature if it is enabled in SETUP
      \_SB.PC00.CTCN ()
    }
  }

  // HK: "]" Scan Code = 0x1B
  ElseIf (LEqual (Local0, 0x1B))   // CTDP Down
  {
    If (LAnd (CondRefOf (\_SB.PC00.CTCD), LEqual (\CTDB, 1))){ // only allow this feature if it is enabled in SETUP
      \_SB.PC00.CTCD ()
    }
  }

  // Calculator (Enter) Key = 0x1C
  ElseIf (LEqual (Local0, 0x1C))
  {
    \_SB.HIDD.HPEM (27)         // HID Event Index 27: System Wake
  }

  // "A" Scan Code = 0x1E
  If (LEqual (Local0, 0x1E))
  {
    \_SB.HIDD.HPEM (16)        // HID Event Index 16: Mute.
  }

  // HK: "S" Scan Code = 0x1F
  ElseIf (LEqual (Local0, 0x1F)) // cycle _PUR
  {
    Increment (ABCD)
    If (LGreater  (ABCD, 3))
    {
      Store (0, ABCD)
    }
    \_SB.PC00.TCPU.SPUR (ABCD)
  }

  // "G" Scan Code = 0x22
  ElseIf (LEqual (Local0, 0x22))
  {
    // Virtual Button Event - Ctrl + Alt + Del Screen

    Sleep (1000) // 1sec delay is needed for Powerbutton and Windows Home button

    If (CondRefOf (\_SB.PC00.GFX0.IUEH)) {
      // Win8 Compatible mode.  Graphics driver takes care of the hot key.
      \_SB.PC00.GFX0.IUEH (0)
    }
  }

  // "H" Scan Code = 0x23
  ElseIf (LEqual (Local0, 0x23))
  {
    // Virtual Button Event - Windows Button
    Sleep (1000)

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Windows Home
      Notify (\_SB.HIDD, 0xC2) // Notify HID driver that Windows Home button is pressed.
      Notify (\_SB.HIDD, 0xC3) // Notify HID driver that Windows Home button is released.
    } ElseIf (CondRefOf (\_SB.PC00.GFX0.IUEH)) {
      // Win8 Compatible mode.  Graphics driver takes care of the hot key.
      \_SB.PC00.GFX0.IUEH (1)
    }
  }

  // "J" Scan Code = 0x24
  ElseIf (LEqual (Local0, 0x24))
  {
    // Virtual Button Event - Volume Up

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume UP Press
      Notify (\_SB.HIDD, 0xC4) // Notify HID driver that Volume UP button is pressed.
      Notify (\_SB.HIDD, 0xC5) // Notify HID driver that Volume UP button is released.
    } ElseIf (CondRefOf (\_SB.PC00.GFX0.IUEH)) {
      // Win8 Compatible mode.  Graphics driver takes care of the hot key.
      \_SB.PC00.GFX0.IUEH (2)
    }
  }

  // "K" Scan Code = 0x25
  ElseIf (LEqual (Local0, 0x25))
  {
    // Virtual Button Event - Volume Down

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume Down Press
      Notify (\_SB.HIDD, 0xC6) // Notify HID driver that Volume Down button is pressed.
      Notify (\_SB.HIDD, 0xC7) // Notify HID driver that Volume Down button is released.
    } ElseIf (CondRefOf (\_SB.PC00.GFX0.IUEH)) {
      // Win8 Compatible mode.  Graphics driver takes care of the hot key.
      \_SB.PC00.GFX0.IUEH (3)
    }
  }

  // "L" Scan Code = 0x26
  ElseIf (LEqual (Local0, 0x26))
  {
    // Virtual Button Event - Rotation Lock
    Sleep (1000)
    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: RotationLock Press
      Notify (\_SB.HIDD, 0xC8) // Notify HID driver that Rotation Lock button is pressed.
      Notify (\_SB.HIDD, 0xC9) // Notify HID driver that Rotation Lock button is released.
    } ElseIf (CondRefOf (\_SB.PC00.GFX0.IUEH)) {
      // Win8 Compatible mode.  Graphics driver takes care of the hot key.
      \_SB.PC00.GFX0.IUEH (4)
    }
  }

  // ";" Scan Code = 0x27
  ElseIf (LEqual (Local0, 0x27))
  {
    // Virtual Button Event - Convertible Indicator Toggle

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
      Xor (\PB1E, 0x08, \PB1E) // Toggle Slate/Laptop Lock Status
      If (And (\PB1E, 0x08)) // Slate/Laptop Mode changed 0 -> 1 Laptop mode
      {
        UPBT (CONVERTIBLE_BUTTON, One)
        Notify (\_SB.HIDD, 0xCD) // Notify HID Event Filter driver that the system is now in Laptop mode.
      } Else { // Slate/Laptop Mode Changed 1 -> 0 Slate Mode
        UPBT (CONVERTIBLE_BUTTON, Zero)
        Notify (\_SB.HIDD, 0xCC) // Notify HID Event Filter driver that the system is now in Slate/Tablet mode.
      }
    } Else {
      If (CondRefOf (\_SB.PC00.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        \_SB.PC00.GFX0.IUEH (6)
      }
    }

  }

  // "'" Scan Code = 0x28
  ElseIf (LEqual (Local0, 0x28))
  {
    // Virtual Button Event - Docking Indicator Toggle

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
      Xor (\PB1E, 0x10, \PB1E) // Toggle Dock/Undock Status
      If (And (\PB1E, 0x10)) // Dock/Undock status changed 0 -> 1 Dock mode
      {
        UPBT (DOCK_INDICATOR, One)
        Notify (\_SB.HIDD, 0xCA) // Notify HID Event Filter driver that the system is now in Dock mode.
      } Else { // Dock/Undock status Changed 1 -> 0 Undock mode
        UPBT (DOCK_INDICATOR, Zero)
        Notify (\_SB.HIDD, 0xCB) // Notify HID Event Filter driver that the system is now in undock mode.
      }
    } Else {
      If (CondRefOf (\_SB.PC00.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        \_SB.PC00.GFX0.IUEH (7)
      }
    }

  }

//
// Reserved for Intel(R) Dynamic Tuning Technology validation
//

  // HK: "M" Scan Code = 0x32
  ElseIf (LEqual (Local0, 0x32))
  {
    Increment (\_SB.PC00.TCPU.MAXT)  // increment TDPC
    If (LGreaterEqual (\_SB.PC00.TCPU.MAXT, \_SB.CLVL)){  // take care of wrap around condition
      Store (0, \_SB.PC00.TCPU.MAXT)
    }
    P8XH (0, \_SB.PC00.TCPU.MAXT)
    Notify (\_SB.PC00.TCPU, 0x84) // hotkey for processor participant
  }

  // F5 Scan Code = 0x3F
  ElseIf (LEqual (Local0, 0x3F))
  {
    // Virtual Battery Hot Keys (VBHK) - VBHK uses keys 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -, + and F5.
    If (LEqual (BNUM, 0))      // Check for Virtual Battery Mode before testing the hot-keys specific to Virtual Battery Support.
    {
      VBHK ()                //  remove this call if no virtual battery is needed.
    }
  }

  // F9 Scan Code = 0x43
  ElseIf (LEqual (Local0, 0x43))
  {
    DHCF (3)              // Decrease Brightness Level.
  }

  // F10 Scan Code = 0x44
  ElseIf (LEqual (Local0, 0x44))
  {
    DHCF (2)              // Increase Brightness Level.
  }

  // F12 Scan Code = 0x58
  ElseIf (LEqual (Local0, 0x58))
  {
    \_SB.HIDD.HPEM (11)      // HID Event Index 11: System Sleep
  }

  // Home Scan Code = 0x47
  // Don't use: SDS shares this key with Left Arrow key and should not be used for Hotkey.

  // UpArw Scan Code = 0x48
  ElseIf (LEqual (Local0, 0x48))
  {
    \_SB.HIDD.HPEM (17)         // HID Event Index 17: Volume Increment
  }

  // PgUp Scan Code = 0x49
  // Don't use: SDS shares this key with Up Arrow key and should not be used for Hotkey.

  // LftArw Scan Code = 0x4B
  ElseIf (LEqual (Local0, 0x4B))
  {
    \_SB.HIDD.HPEM (20)         // HID Event Index 20: Display Brightness Decrement
  }

  // RtArw Scan Code = 0x4D
  ElseIf (LEqual (Local0, 0x4D))
  {
    \_SB.HIDD.HPEM (19)         // HID Event Index 19: Display Brightness Increment
  }

  // End Scan Code = 0x4F
  // Don't use: SDS shares this key with Right Arrow key and should not be used for Hotkey.

  // DwnArw Scan Code = 0x50
  ElseIf (LEqual (Local0, 0x50))
  {
    \_SB.HIDD.HPEM (18)         // HID Event Index 18: Volume Decrement
  }

  // PgDn Scan Code = 0x51
  // Don't use: SDS shares this key with Down Arrow key and should not be used for Hotkey.

  // Ins Scan Code = 0x52
  ElseIf (LEqual (Local0, 0x52))
  {
    \_SB.HIDD.HPEM (13)         // HID Event Index 13: Scan Prev Track
  }

  // Del Scan Code = 0x53
  ElseIf (LEqual (Local0, 0x53))
  {
    \_SB.HIDD.HPEM (12)        // HID Event Index 12: Scan Next Track
  }
} // end Method (_Q56) CAS hot key event

Method (_QD5)    // 10 second power button press.
{
  \_SB.PWPR ()
}

Method (_QD6)    // 10 second power button de-press.
{
  \_SB.PWRR ()
}

Method (_Q80)    // Volume Up
{
  If (LEqual (VBVP, 1))
  {
    // Volume Up.
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    // Else If HID Event Driver loaded
    //   use HID Event Driver
    // Else
    //   use GFX Driver.
    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume UP
      Notify (\_SB.HIDD, 0xC4) // Notify HID driver that Volume UP button is pressed.
    } Else {
      If (LEqual (\_SB.HIDD.HRDY, 1)){
        \_SB.HIDD.HPEM (17)
      } Else {
        \_SB.PC00.GFX0.IUEH (2)
      }// End of If (LEqual (\_SB.HIDD.HRDY, 1))
    }
  }
}

Method (_Q81)    // Volume Down
{
  If (LEqual (VBVD, 1))
  {
    // Volume Down.
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    // Else If HID Event Driver loaded
    //   use HID Event Driver
    // Else
    //   use GFX Driver.
    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume Down
      Notify (\_SB.HIDD, 0xC6) // Notify HID driver that Volume Down button is pressed.
    } Else {
      If (LEqual (\_SB.HIDD.HRDY, 1)){
        \_SB.HIDD.HPEM (18)
      } Else {
        \_SB.PC00.GFX0.IUEH (3)
      }
    }
  }
}

Method (_Q82)    // Tablet Switch Toggle Slate/Tablet mode
{
  If (LEqual (SMSS, 1)) // SDS, PantherMtn, GrizzlyMtn or StarBrook
  {
    // Rotation Lock button
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    // Else
    //   use GFX Driver.

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
      Or (\PB1E, 0x08, \PB1E) // Toggle Slate Status
      UPBT (CONVERTIBLE_BUTTON, Zero)
      Notify (\_SB.HIDD, 0xCC) // Notify HID Event Filter driver that the system is now in Slate/Tablet mode.
    } Else {
      \_SB.PC00.GFX0.IUEH (6) // Convertible Indicator lock
    }
  }
}

Method (_Q83)    // Tablet Switch Toggle Laptop/Normal mode
{
  If (LEqual (SMSS, 1)) // SDS, PantherMtn, GrizzlyMtn or StarBrook
  {
    // Rotation Lock button
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    // Else
    //   use GFX Driver.

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
      Or (\PB1E, 0x08, \PB1E) // Set Laptop status
      UPBT (CONVERTIBLE_BUTTON, One)
      Notify (\_SB.HIDD, 0xCD) // Notify HID Event Filter driver that the system is now in Laptop mode.
    } Else {
      \_SB.PC00.GFX0.IUEH (6) // Convertible Indicator lock
    }
  }
}

Method (_Q85)    // Windows Home button
{
  If (LEqual (VBHB, 1))
  {
    // Windows Home Button
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    // Else If HID Event Driver loaded
    //   use HID Event Driver
    // Else
    //   use GFX Driver.

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Windows Home Press
      Notify (\_SB.HIDD, 0xC2) // Notify HID driver that Windows Home button is pressed.
    } Else {
      If (LEqual (\_SB.HIDD.HRDY, 1)){
        \_SB.HIDD.HPEM (1)
      } Else {
        \_SB.PC00.GFX0.IUEH (1)
      }
    }
  }
}

Method (_Q86)    // Rotation Lock press event
{
  If (LEqual (VBRL, 1))
  {
    // Rotation Lock button
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    // Else If HID Event Driver loaded
    //   use HID Event Driver
    // Else
    //   use GFX Driver.

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: RotationLock Press
      Notify (\_SB.HIDD, 0xC8) // Notify HID driver that Rotation Lock button is pressed.
    } Else {
      If (LEqual (\_SB.HIDD.HRDY, 1)){
        \_SB.HIDD.HPEM (2)
      } Else {
        \_SB.PC00.GFX0.IUEH (4)
      }
    }
  }
}

Method (_Q87)    // Mode Switch: Clam Shell (Laptop) mode and Slate mode switch
{
  If (LEqual (SMSS, 1)) // SDS, PantherMtn, GrizzlyMtn or StarBrook
  {
    // Rotation Lock button
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    // Else
    //   use GFX Driver.

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
      Xor (\PB1E, 0x08, \PB1E) // Toggle Slate/Laptop Lock Status
      If (And (\PB1E, 0x08)) // Slate/Laptop Mode changed 0 -> 1 Laptop mode
      {
        UPBT (CONVERTIBLE_BUTTON, One)
        Notify (\_SB.HIDD, 0xCD) // Notify HID Event Filter driver that the system is now in Laptop mode.
      } Else { // Slate/Laptop Mode Changed 1 -> 0 Slate Mode
        UPBT (CONVERTIBLE_BUTTON, Zero)
        Notify (\_SB.HIDD, 0xCC) // Notify HID Event Filter driver that the system is now in Slate/Tablet mode.
      }
    } Else {
      \_SB.PC00.GFX0.IUEH (6) // Convertible Indicator lock
    }
  }

}

Method (_Q88)    // Volume Up release event
{
  If (LEqual (VBVP, 1))
  {
    // Volume Up button release event.
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume UP Release
      Notify (\_SB.HIDD, 0xC5) // Notify HID driver that Volume UP button is released.
    }
  }
}

Method (_Q89)    // Volume Down release event
{
  If (LEqual (VBVD, 1))
  {
    // Volume Down button release event.
    // If VirtualButton driver loaded
    //   use VirtualButton driver

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume Down Release
      Notify (\_SB.HIDD, 0xC7) // Notify HID driver that Volume UP button is released.
    }
  }
}

Method (_Q8A)    // Windows Home button release event
{
  If (LEqual (VBHB, 1))
  {
    // Windows Home Button release event
    // If VirtualButton driver loaded
    //   use VirtualButton driver

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Windows Home Button Release
      Notify (\_SB.HIDD, 0xC3) // Notify HID driver that Windows Home button is released.
    }
  }
}

Method (_Q8B)    // Rotation Lock release event
{
  If (LEqual (VBRL, 1)) {
    // Windows Rotation Lock release event

    If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Rotation Lock Release
      Notify (\_SB.HIDD, 0xC9) // Notify HID driver that Windows Home button is released.
    }
  }
}

//
// The method, _Q90, is just placeholder method for reference only when using EC to monitor device mode. OEM/Customer
// can make their own way to get device mode and notify the driver.
//
Method (_Q90) // Device mode change event from EC.
{
  If (CondRefOf (DEVM)) {
    // Get device mode
    Store (2, DEVM) // Device Mode 2

    If (CondRefOf (\_SB.WCCD)) {
      Notify (\_SB.WCCD, 0x80)
    }
  }
}

Method (_QF0)    // Thermal Event.
{
  Notify (\_TZ.TZ00, 0x80)
}

// Virtual Battery Switch Event:
//    This method handles the Virtual Battery switch on the CRB.
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method (VBSE, 0, Serialized) // called from EC.asl
{
  // Check for Virtual Battery Mode.  If the CRB is
  // running on real batteries, skip this whole section.

  If (LEqual (BNUM, 0))
  {
    // Check that the current and NVS Power States
    // are different.  The 2 may be out of sync due to
    // Virtual Battery hot-key support.

    If (LNotEqual (ECRD (RefOf (VPWR)), \PWRS))
    {
      // Update NVS Power State.

      Store (ECRD (RefOf (VPWR)), \PWRS)

      // Perform needed ACPI Notifications.
      SPNT ()
      BPNT ()
    }
  }
}

Method (_Q53)
{
  VBSE ()    // Virtual Battery Switch Event, remove if not needed.
}

// _QF1 (Query - Embedded Controller Query F1)
//
// Handler for EC generated SCI number F1.
//
// Arguments: (0)
//   None
// Return Value:
//   None
//
Method (_QF1)
{
  // Thermal sensor threshold crossing event handler
  Store (ECRD (RefOf (TSSR)), Local0)

  While (Local0) // Ensure that events occuring during execution
  {             // of this handler are not dropped
    ECWT (0, RefOf (TSSR)) // clear all status bits
    If (CondRefOf (\_SB.IETM.DTNS))
    {
      \_SB.IETM.DTNS (Local0)
    }

    Store (ECRD (RefOf (TSSR)), Local0)
  }
}

// _QF2 (Query - Embedded Controller Query F2)
//
// Handler for EC generated SCI number F2.
//
// Arguments: (0)
//   None
// Return Value:
//   None
//
Method(_QF2)
{ // Fan RPM threshold crossing event handler
  // EC should generate an SCI interrupt when the fan speed in RPM is greater than the threshold.
  // This causes BIOS to generate an ACPI notification to the Fan Participant ACPI device.
  Store(ECRD(RefOf(FRTS)), Local0)
  While(Local0) // Ensure that events occuring during execution
  {             // of this handler are not dropped
    ECWT(0, RefOf(FRTS)) // clear all status bits
    If(And(Local0, 0x4))
    { // BIT2: Fan3 RPM Threshold Crossed
      Notify(\_SB.IETM.TFN3, 0x90)
    }
    If(And(Local0, 0x2))
    { // BIT1: Fan2 RPM Threshold Crossed
      Notify(\_SB.IETM.TFN2, 0x90)
    }
    If(And(Local0, 0x1))
    { // BIT0: Fan1 RPM Threshold Crossed
      Notify(\_SB.IETM.TFN1, 0x90)
    }
    Store(ECRD(RefOf(FRTS)), Local0)
  }
}

