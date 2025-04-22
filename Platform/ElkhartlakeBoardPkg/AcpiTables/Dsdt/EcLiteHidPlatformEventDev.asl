/** @file
  Intel HID Platform Event Driver ACPI Support

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  Device(\_SB.HIDD)                                       // HID Device.
  {
    Name(_HID,"INT33D5")                            // Intel Ultrabook HID Platform Event Driver.
    Name(_CID,"INT33D5")                            // Generic Motherboard Resources.

    Name (HBSY, 0)   // HID Busy
    Name (HIDX, 0)   // HID Index
    Name (HMDE, 0)   // HID Mode
    Name (HRDY, 0)   // HID Ready
    Name (BTLD, 0)   // Button Driver Loaded

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
      If (CondRefOf(\_SB.PWRB.PBST))
      {
        Store(0, \_SB.PWRB.PBST)
        Notify(\_SB.PWRB, 1) // Device check
      }
      Store(One, \_SB.HIDD.BTLD)
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
      Return (0x0F)
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
    Method (HEBC,0,Serialized) {
      Return (\HEB1)
    }
    Method (H2BC,0,Serialized) {
      Return (\HEB1)
    }
    //
    // Method to Handle Home Button GPE
    //
    Method (HHBE, 0, Serialized) {
      Store (0xF, Local0)
      //
      // Read Home Button GPIO RX State
      // Expecting either value of 0 or 1.
      //
      If (CondRefOf (\_SB.GGIV)) {
        Store (\_SB.GGIV(\GHOM), Local0)
      }
      If (LEqual (Local0, 0xF)) {
        Return ()
      }
      If (LNot (Local0)) {
        //
        // If VirtualButton driver loaded
        //   use VirtualButton driver
        // Else If HID Event Driver loaded
        //   use HID Event Driver
        // Else
        //   use GFX Driver
        //
        If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { // Win10 and Button Driver loaded: Windows Home Press
          Notify(\_SB.HIDD,0xC2) // Notify HID driver that Windows Home button is pressed.
        } Else {
          If (LEqual(\_SB.HIDD.HRDY, 1)){
            \_SB.HIDD.HPEM(1)
          } Else {
            \_SB.PC00.GFX0.IUEH(1)
          }
        }
      } Else {
        If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { // Win10 and Button Driver loaded: Windows Home Button Release
          Notify(\_SB.HIDD,0xC3) // Notify HID driver that Windows Home button is released.
        }
      }
    }
    //
    // Method to Handle Volume Up Button GPE
    //
    Method (HVUE, 0, Serialized) {
      Store (0xF, Local0)
      //
      // Read Volume Up Button GPIO RX State
      // Expecting either value of 0 or 1.
      //
      If (CondRefOf (\_SB.GGIV)) {
        Store (\_SB.GGIV(\GVUP), Local0)
      }
      If (LEqual (Local0, 0xF)) {
        Return ()
      }
      If (LNot (Local0)) {
        //
        // If VirtualButton driver loaded
        //   use VirtualButton driver
        // Else If HID Event Driver loaded
        //   use HID Event Driver
        // Else
        //   use GFX Driver
        //
        If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { // Win10 and Button Driver loaded: Volume Up
          Notify(\_SB.HIDD,0xC4) // Notify HID driver that Volume UP button is pressed.
        } Else {
          If (LEqual(\_SB.HIDD.HRDY, 1)){
            \_SB.HIDD.HPEM(17)
          } Else {
            \_SB.PC00.GFX0.IUEH(2)
          }// End of If (LEqual(\_SB.HIDD.HRDY, 1))
        }
      } Else {
        If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { // Win10 and Button Driver loaded: Volume UP Release
          Notify(\_SB.HIDD,0xC5) // Notify HID driver that Volume UP button is released.
        }
      }
    }
    //
    // Method to Handle Volume Down Button GPE
    //
    Method (HVDE, 0, Serialized) {
      Store (0xF, Local0)
      //
      // Read Volume Down Button GPIO RX State
      // Expecting either value of 0 or 1.
      //
      If (CondRefOf (\_SB.GGIV)) {
        Store (\_SB.GGIV(\GVDN), Local0)
      }
      If (LEqual (Local0, 0xF)) {
        Return ()
      }
      If (LNot (Local0)) {
        //
        // If VirtualButton driver loaded
        //   use VirtualButton driver
        // Else If HID Event Driver loaded
        //   use HID Event Driver
        // Else
        //   use GFX Driver
        //
        If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { // Win10 and Button Driver loaded: Volume Down
          Notify(\_SB.HIDD,0xC6) // Notify HID driver that Volume Down button is pressed.
        } Else {
          If (LEqual(\_SB.HIDD.HRDY, 1)){
            \_SB.HIDD.HPEM(18)
          } Else {
            \_SB.PC00.GFX0.IUEH(3)
          }
        }
      } Else {
        If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { // Win10 and Button Driver loaded: Volume Down Release
          Notify(\_SB.HIDD,0xC7) // Notify HID driver that Volume UP button is released.
        }
      }
    }
    Method (PWPR, 0, Serialized) // Power Button Press Helper Method
    {
      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { // Win10 and Button Driver loaded: Power Button Press
        Notify(\_SB.HIDD,0xCE) // Notify HID driver that Power button is pressed.
      } Else {
        If (LEqual(\_SB.PWRB.PBST, 0x1)) { // Using Control Method Power Button
          \_SB.PWRB.PBDN () // Send press notification to Power Button device if requested
        }
        If (CondRefOf(\_SB.PC00.GFX0.IUER))
        {
          Store(\_SB.PC00.GFX0.IUER, Local0)
          And(Local0, 0xC0, \_SB.PC00.GFX0.IUER)  // Clear 4:0 button events on entry.
          Store(\_SB.PC00.GFX0.IUER, Local0)
          Or(Local0, 0x01, \_SB.PC00.GFX0.IUER)   // Set Power Button Status = pressed.
        }
      } // If Win10
    } // End PWPR Method
    Method (PWRR, 0, Serialized) // Power Button Release Helper Method
    {
      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { // Win10 and Button Driver loaded: Power Button Release
        Notify(\_SB.HIDD,0xCF) // Notify HID driver that Power button is released.
      } Else {
        If (LEqual(\_SB.PWRB.PBST, 0x1)) { // Using Control Method Power Button
          \_SB.PWRB.PBUP () // Send release notification to Power Button device if requested
        }
        If (CondRefOf(\_SB.PC00.GFX0.IUER))
        {
          Store(\_SB.PC00.GFX0.IUER, Local0)
          And(Local0, 0xC0, \_SB.PC00.GFX0.IUER)  // Clear 4:0 button events on entry.
        }
      } // If Win10
    } // End PWRR Method
    //
    // Method to Handle Power Button GPE
    //
    Method (HPBE, 0, Serialized) {
      Store (0xF, Local0)
      //
      // Read Power Button GPIO RX State
      // Expecting either value of 0 or 1.
      //
      If (CondRefOf (\_SB.GGIV)) {
        Store (\_SB.GGIV(\PPBG), Local0)
      }
      If (LEqual (Local0, 0xF)) {
        Return ()
      }
      If (LNot (Local0)) {
        \_SB.HIDD.PWPR ()
      } Else {
        \_SB.HIDD.PWRR ()
      }
    }
  }
