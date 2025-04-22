/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB)
{
  Device(HIDD)                                       // HID Device.
  {
    Method (_HID) {                                  // Intel Ultrabook HID Platform Event Driver.
      Return("INTC1070")
    }

    Name (HBSY, 0)   // HID Busy
    Name (HIDX, 0)   // HID Index
    Name (HMDE, 0)   // HID Mode
    Name (HRDY, 0)   // HID Ready
    Name (BTLD, 0)   // Button Driver Loaded
    Name (BTS1, 0)   // Button Status

    Method(_STA,0,Serialized)                       // Status Method.
    {
      If (LAnd(LGreaterEqual(OSYS, 2013), LEqual(HEFE, 1))) { // If OS is Win8.1 or later AND HID event filter is enabled in Setup.
        Return(0x0F)                                // Show device.
      } Else {
        Return(0)                                   // Hide device for Win7/Win8.
      }
    }

    //
    // HID Driver Descriptor Method - Called by HID Driver during initialization
    // to obtain HID Descriptor information.
    //
    // Input: None
    //
    // Output:  Package containing a complete HID Descriptor information.
    //
    Method(HDDM,0,Serialized)
    {
       // Placeholder.
       Name(DPKG, Package(4) {0x11111111, 0x22222222, 0x33333333, 0x44444444})
       Return(DPKG)
    }

    //
    // HID Driver Event Method - Called by HID Driver to get the specific
    // platform event.
    //
    // Input: None
    //
    // Output: Platform HID Event.
    // Mode 0 = Index of HID Input Report, per pre-defined Table.
    // Mode 1 = Package containing a complete HID Input Report.
    //
    Method(HDEM,0,Serialized)
    {
       Store(0,HBSY)                          // Clear HID Busy.
       // Simple Mode is hardcoded for now.  Return Simple Mode HID Index Value.
       If (LEqual(HMDE,0))
       {
          Return(HIDX)
       }
       Return(HMDE)
    }

    //
    // HID Driver Mode Method - Called by HID Driver during initialization to get
    // the platform mode of operation.
    //
    // Input: None
    //
    // Output:  Mode the platform is running in.
    // 0 = Simple Mode.
    // 1 = Advanced Mode.
    //
    Method(HDMM,0,Serialized)
    {
       Return(HMDE)               // Return Mode of operation.
    }

    //
    // HID Driver Status Method - called by HID Driver to report platform readiness status.
    // Input: Driver Status.
    // 0 = Driver Unloaded.
    // 1 = Driver Loaded and Ready.
    //
    // Output: None
    //
    Method(HDSM,1,Serialized)
    {
      Store(Arg0,HRDY)              // Store HID Ready Status.
      // Eventually code will communicate to platform the Driver status (enabled/disabled).
    }

    //
    // HID Platform Event Method - called by Platform to communicate HID Event to Driver.
    // Input:
    // Mode 0 = Index of HID Event.
    // Mode 1 = Package containing a complete HID Report.
    //
    Method(HPEM,1,Serialized)                        // HID Platform Event Method.
    {
      Store(1,HBSY)                                 // Set HID Busy.
      // Simple Mode is hardcoded for now.  Simply store HID Index value.
      If (LEqual(HMDE,0))
      {
         Store(Arg0,HIDX)
      } Else {
         Store(Arg0,HIDX)
      }
      Notify(\_SB.HIDD,0xC0)                         // Notify Driver to get HID Event.
      Store(0,Local0)                                // Initialize Local0 as a timeout counter.
      While(LAnd(LLess(Local0,250),HBSY))            // Wait <= 1 second for Driver to ACK success.
      {
        Sleep(4)                                     // Delay 4 ms.
        Increment(Local0)                            // Increment Timeout.
      }
      If (LEqual(HBSY,1))                             // Failure?
      {
        Store(0,HBSY)                                // Yes.  Clear HID Busy Flag.
        Store(0,HIDX)                                // Set HID Simple Mode Index = 0 = Undefined.
        Return(1)                                    // Return Failure.
      } Else {
        Return(0)                                    // Return Success.
      }
    }

If (LEqual(ECON,1)) {
    //
    // HID Button Load Method - called by Platform to say HID driver is capable of receiving 5-button array notifies.
    // Input: None
    //
    // Output: None
    //
    Method(BTNL,0,Serialized) // HID Button Enable/Disable Method
    {
      //
      // Clear PBST so that we can hide the default power button.
      //
#if FixedPcdGetBool(PcdAdlNSupport) == 0
      If (CondRefOf(\_SB.PWRB.PBST))
      {
        Store(0, \_SB.PWRB.PBST)
        Notify(\_SB.PWRB, 1) // Device check
      }
#endif
      Store(One, \_SB.HIDD.BTLD)

      If(LEqual(\AEAB,1)) {
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
        If (LEqual(\S0ID,0)) {
          //
          // Disable the power button (QD5 and QD6 event) to make the slide to shutdown not pop up if the LowPowerS0Idle is disable
          //
          Store(0x1E, BTS1)
        } Else {
          //
          // Enable all buttons
          //
          Store(0x1F, BTS1)
        }
      } Else {
        //
        // Disable all buttons
        //
        Store(0, BTS1)
      }
#if FixedPcdGetBool(PcdAdlNSupport) == 0
      \_SB.PC00.LPCB.H_EC.ECWT(BTS1, RefOf(\_SB.PC00.LPCB.H_EC.BTEN)) //Button Enable/Disable field in ACPI name space
      \_SB.PC00.LPCB.H_EC.ECMD(0x38) //Enable/Disable SCIs from buttons
#endif
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
    Method(BTNE,1,Serialized) // HID Button Enable/Disable Method
    {
      If(LEqual(\AEAB,1)) {
        Store(Or(And(Arg0, 0x1E),0x01), BTS1) //Mask off PB Enable/Disable
#if FixedPcdGetBool(PcdAdlNSupport) == 0
        \_SB.PC00.LPCB.H_EC.ECWT(BTS1, RefOf(\_SB.PC00.LPCB.H_EC.BTEN)) //Button Enable/Disable field in ACPI name space
        \_SB.PC00.LPCB.H_EC.ECMD(0x38) //Enable/Disable SCIs from buttons
#endif
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
    Method(BTNS,0,Serialized)
    {
      If(LEqual(\AEAB,1)) {
#if FixedPcdGetBool(PcdAdlNSupport) == 0
        Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.BTEN)), BTS1) //Button Enable/Disable field in ACPI name space
#endif
      }
      Return(BTS1)
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
    Method(BTNC,0,Serialized) // HID Button Capabilities Method
    {
      If(LEqual(\AEAB,1)) {
        Return(0x1F)
      } Else {
        Return(0)
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

    Method (HEBC,0,Serialized) {
      If(LEqual(\AHDB,1)) {
        Return (\HEB1)
      } Else {
        Return (0)
      }
    }

    Method (H2BC,0,Serialized) {
      If(LEqual(\AHDB,1)) {
        Return (\HEB1)
      } Else {
        Return (0)
      }
    }

    //
    // HEEC- Hid Event Extended Capabilities [32:63]
    //
    Method (HEEC,0,Serialized) {
      If(LEqual(\AHDB,1)) {
        Return(HEB2)
      } Else {
        Return(0)
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

      If (LEqual(Arg0, ToUUID ("EEEC56B3-4442-408F-A792-4EDD4D758054")))
      {
        If (LEqual(1,ToInteger(Arg1)))        // Revision 1.
        {
          Switch (ToInteger(Arg2))            // Switch to Function Index.
          {
            //
            // Function 0, Query of supported functions.
            //
            Case (0)
            {
              Return (Buffer() {0xFF, 0x03}) // Total 9 function indices are supported including this.
            }

            //
            // Function 1, BTNL. Button Load Method. No Input/Output.
            //
            Case (1)
            {
              BTNL()
            }

            //
            // Function 2, HDMM. HID Driver Mode Method.
            // Input:None
            // Output:HDMM output. See HDMM
            //
            Case (2)
            {
              Return(HDMM())
            }

            //
            // Function 3, HDSM. HID Driver Status Method.
            // Input: 0 - The driver is not available. 1 - The driver is available.
            // Output: None
            //
            Case (3)
            {
              HDSM(DeRefOf(Index(Arg3, 0)))
            }

            //
            // Function 4, HDEM. HID Driver Event Method.
            // Input: None.
            // Output: Package contains Supported Keys (Mode 0)
            //
            Case (4)
            {
              Return(HDEM())
            }

            //
            // Function 5 BTNS. Button Status Method.
            // Input: None.
            // Output: Int32 which contains a bit map of Buttons' enable/disable states
            //
            Case (5)
            {
              Return(BTNS())
            }

            //
            // Function 6 BTNE. Button Enable/Disable Method.
            // Input: Int32 Bit mask of buttons enable/disable control. 1- Button should be Enabled, 0- Button should be Disabled
            // Output: None.
            //
            Case (6)
            {
              BTNE(DeRefOf(Index(Arg3, 0)))
            }

            //
            // Function 7 HEBC. Button implemented state.
            // Input: None
            // Output: Int32 Bit map which shows what buttons are implemented on this system.
            //
            Case (7)
            {
              Return(HEBC())
            }

            //
            // Function 8 VGBS. Virtual GPIO Button Status.
            // Input: None
            // Output: Intger Bit map which shows what Virtual GPIO Button status. Currently only Dock/Slate modes are supported.
            //
            Case (8)
            {
#if FixedPcdGetBool(PcdAdlNSupport) == 0
              Return(\_SB.PC00.LPCB.H_EC.VGBS())
#else
              Return(0)
#endif
            }
            //
            // Function 9 H2BC. Button implemented state.
            // Input: None
            // Output: Int32 Bit map which shows what buttons are implemented on this system.
            //
            Case (9)
            {
              Return(H2BC())
            }
          } // End Switch statement
        }  // End Revision check
      }  // End UUID check

      // If the code falls through to this point, just return a buffer of 0.

      Return (Buffer() {0x00})

    }  // End _DSM Method
  }
  }
}//end scope _SB
