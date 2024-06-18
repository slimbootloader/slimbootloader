/** @file
  ACPI EC SSDT table for HID method.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
External (\_SB.HIDD.BTLD, IntObj)
External (\AEAB, IntObj)
External (\S0ID, IntObj)
External (\HEB1, IntObj)
External (\AHDB, IntObj)
External (\HDMM, MethodObj)
External (\HDSM, MethodObj)
External (\HDEM, MethodObj)

Name (BTS1, 0)   // Button Status

//
// HID Button Load Method - called by Platform to say HID driver is capable of receiving 5-button array notifies.
// Input: None
//
// Output: None
//
Method (BTNL, 0, Serialized) // HID Button Enable/Disable Method
{
  //
  // Clear PBST so that we can hide the default power button.
  //
  If (CondRefOf (\_SB.PWRB.PBST)) {
    Store (0, \_SB.PWRB.PBST)
    Notify (\_SB.PWRB, 1) // Device check
  }
  Store (One, \_SB.HIDD.BTLD)

  If (LEqual (\AEAB, 1)) {
    //
    // Enable SCIs for HID driver in EC.
    //
    //   Bits[0]: Power Button
    //   Bits[1]: Windows Button
    //   Bits[2]: Volume Up Button
    //   Bits[3]: Volume Down Button
    //   Bits[4]: Rotation Lock Button
    //   Bits[5:31]: Reserved
    //
    If (LEqual (\S0ID, 0)) {
      //
      // Disable the power button (QD5 and QD6 event) to make the slide to shutdown not pop up if the LowPowerS0Idle is disable
      //
      Store (0x1E, BTS1)
    } Else {
      //
      // Enable all buttons
      //
      Store (0x1F, BTS1)
    }
  } Else {
    //
    // Disable all buttons
    //
    Store (0, BTS1)
  }
  \_SB.PC00.LPCB.H_EC.CBTS (BTS1) //Enable/Disable SCIs from buttons
}

//
// HID Button Enable/Disable Method - called by Platform to disable/enable notification based on button press
// Input:
// Arg0 = Bit mask of buttons to Enable or Disable: 1- Button should be Enabled, 0- Button should be Disabled
//   Bits[0]: Power Button N/A to disable
//   Bits[1]: Windows Button
//   Bits[2]: Volume Up Button
//   Bits[3]: Volume Down Button
//   Bits[4]: Rotation Lock Button
//   Bits[5:31]: Reserved
//
// Output: None
//
Method (BTNE, 1, Serialized) // HID Button Enable/Disable Method
{
  If (LEqual (\AEAB, 1)) {
    Store (Or (And (Arg0, 0x1E), 0x01), BTS1) //Mask off PB Enable/Disable
    \_SB.PC00.LPCB.H_EC.CBTS (BTS1) //Enable/Disable SCIs from buttons
  }
}

//
// HID Button Status - called by Platform to get what buttons are enabled and disabled
// Input: None
//
// Output: Bit mask of buttons' current status: 1- Button is Enabled, 0- Button is Disabled
//   Bits[0]: Power Button N/A to disable
//   Bits[1]: Windows Button
//   Bits[2]: Volume Up Button
//   Bits[3]: Volume Down Button
//   Bits[4]: Rotation Lock Button
//   Bits[5:31]: Reserved
//
Method (BTNS, 0, Serialized)
{
  If (LEqual (\AEAB, 1)) {
    Store (\_SB.PC00.LPCB.H_EC.ERBT (), BTS1) //Button Enable/Disable field in ACPI name space
  }
  Return (BTS1)
}

//
// HID Button Capabilities Method - called by Platform to determine what buttons are supported
// Input: None
//
// Output: Bit mask of buttons supported: 1- Button is Supported, 0- Button is not Supported
//   Bits[0]: Power Button (Must be 1)
//   Bits[1]: Windows Button
//   Bits[2]: Volume Up Button
//   Bits[3]: Volume Down Button
//   Bits[4]: Rotation Lock Button
//   Bits[5:31]: Reserved
//
Method (BTNC, 0, Serialized) // HID Button Capabilities Method
{
  If (LEqual (\AEAB, 1)) {
    Return (0x1F)
  } Else {
    Return (0)
  }
}

//
// HEBC: HID Event Base Capabilities [31:0]- To specify the base button capabilities supported on platform by returning a ULONG value with the following bit level definition
// Input: None
//
// 0 = Button not supported
// 1 = Button supported
// Output:
// Bits [0] - Windows Button (Windows 8.1 supported), Rotation Lock (Windows 8.1 supported), Num Lock, Home, End, Page Up, Page Down
// Bits [1] - Wireless Radio Control
// Bits [2] - System Power Down (Windows 8.1 supported)
// Bits [3] - System Hibernate
// Bits [4] - System Sleep/ System Wake
// Bits [5] - Scan Next Track
// Bits [6] - Scan Previous Track
// Bits [7] - Stop
// Bits [8] - Play/Pause
// Bits [9] - Mute
// Bits [10] - Volume Increment (Windows 8.1 supported)
// Bits [11] - Volume Decrement (Windows 8.1 supported)
// Bits [12] - Display Brightness Increment
// Bits [13] - Display Brightness Decrement
// Bits [14] - Lock Tablet
// Bits [15] - Release Tablet
// Bits [16] - Toggle Bezel
// Bits [17] - 5 button array (Windows 10 supported - Power, Windows Home, Volume Up, Volume Down, Rotation Lock)
// Bits [18] - Button 1
// Bits [19] - Button 2
// Bits [20] - Button 3
// Bits [21] - Button 4
// Bits [22] - Button 5
// Bits [23-31] - reserved
//
//  Modify below table if the target platform has different capabilities. Each bit corresponding the above table definition.
//
Name (HEB2, 0)       // Extended 32bit capability definition for future enhancements.

Method (HEBC, 0, Serialized) {
  If (LEqual (\AHDB, 1)) {
    Return (\HEB1)
  } Else {
    Return (0)
  }
}

Method (H2BC, 0, Serialized) {
  If (LEqual (\AHDB, 1)) {
    Return (\HEB1)
  } Else {
    Return (0)
  }
}

//
// HEEC- Hid Event Extended Capabilities [32:63]
//
Method (HEEC, 0, Serialized) {
  If (LEqual (\AHDB, 1)) {
    Return (HEB2)
  } Else {
    Return (0)
  }
}

//
// HIDD _DSM
// _DSM : Device Specific Method for the Windows Compatible Button Array.
//
// Arg0: UUID Unique function identifier
// Arg1: Integer Revision Level
// Arg2: Integer Function Index
// Arg3: Package Parameters
//
Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
{
  // Compare passed in UUID to supported UUID.

  If (LEqual (Arg0, ToUUID ("EEEC56B3-4442-408F-A792-4EDD4D758054")))
  {
    If (LEqual (1, ToInteger (Arg1)))        // Revision 1.
    {
      Switch (ToInteger (Arg2))            // Switch to Function Index.
      {
        //
        // Function 0, Query of supported functions.
        //
        Case (0)
        {
          Return (Buffer () {0xFF, 0x03}) // Total 9 function indices are supported including this.
        }

        //
        // Function 1, BTNL. Button Load Method. No Input/Output.
        //
        Case (1)
        {
          BTNL ()
        }

        //
        // Function 2, HDMM. HID Driver Mode Method.
        // Input:None
        // Output:HDMM output. See HDMM
        //
        Case (2)
        {
          Return (HDMM ())
        }

        //
        // Function 3, HDSM. HID Driver Status Method.
        // Input: 0 - The driver is not available. 1 - The driver is available.
        // Output: None
        //
        Case (3)
        {
          HDSM (DeRefOf (Index (Arg3, 0)))
        }

        //
        // Function 4, HDEM. HID Driver Event Method.
        // Input: None.
        // Output: Package contains Supported Keys (Mode 0)
        //
        Case (4)
        {
          Return (HDEM ())
        }

        //
        // Function 5 BTNS. Button Status Method.
        // Input: None.
        // Output: Int32 which contains a bit map of Buttons' enable/disable states
        //
        Case (5)
        {
          Return (BTNS ())
        }

        //
        // Function 6 BTNE. Button Enable/Disable Method.
        // Input: Int32 Bit mask of buttons enable/disable control. 1- Button should be Enabled, 0- Button should be Disabled
        // Output: None.
        //
        Case (6)
        {
          BTNE (DeRefOf (Index (Arg3, 0)))
        }

        //
        // Function 7 HEBC. Button implemented state.
        // Input: None
        // Output: Int32 Bit map which shows what buttons are implemented on this system.
        //
        Case (7)
        {
          Return (HEBC ())
        }

        //
        // Function 8 VGBS. Virtual GPIO Button Status.
        // Input: None
        // Output: Intger Bit map which shows what Virtual GPIO Button status. Currently only Dock/Slate modes are supported.
        //
        Case (8)
        {
          Return (\_SB.PC00.LPCB.H_EC.VGBS ())
        }
        //
        // Function 9 H2BC. Button implemented state.
        // Input: None
        // Output: Int32 Bit map which shows what buttons are implemented on this system.
        //
        Case (9)
        {
          Return (H2BC ())
        }
      } // End Switch statement
    }  // End Revision check
  }  // End UUID check

  // If the code falls through to this point, just return a buffer of 0.

  Return (Buffer () {0x00})

}  // End _DSM Method
