/** @file

  Intel Hid Platform Event Driver ACPI Support

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope (\_SB)
{
  Device (HIDD)                                      // HID Device.
  {
    Name (_HID,"INTC1077")                           // Intel Ultrabook HID Platform Event Driver.

    Name (HBSY, 0)   // HID Busy
    Name (HIDX, 0)   // HID Index
    Name (HMDE, 0)   // HID Mode
    Name (HRDY, 0)   // HID Ready
    Name (BTLD, 0)   // Button Driver Loaded

    Method (_STA,0,Serialized)                       // Status Method.
    {
      If (LAnd (LGreaterEqual (OSYS, 2013), LEqual (HEFE, 1))) { // If OS is Win8.1 or later AND HID event filter is enabled in Setup.
        Return (0x0F)                                // Show device.
      } Else {
        Return (0)                                   // Hide device for Win7/Win8.
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
    Method (HDDM,0,Serialized)
    {
       // Placeholder.
       Name (DPKG, Package (4) {0x11111111, 0x22222222, 0x33333333, 0x44444444})
       Return (DPKG)
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
    Method (HDEM,0,Serialized)
    {
       Store (0,HBSY)                          // Clear HID Busy.
       // Simple Mode is hardcoded for now.  Return Simple Mode HID Index Value.
       If (LEqual (HMDE,0))
       {
          Return (HIDX)
       }
       Return (HMDE)
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
    Method (HDMM,0,Serialized)
    {
       Return (HMDE)               // Return Mode of operation.
    }

    //
    // HID Driver Status Method - called by HID Driver to report platform readiness status.
    // Input: Driver Status.
    // 0 = Driver Unloaded.
    // 1 = Driver Loaded and Ready.
    //
    // Output: None
    //
    Method (HDSM,1,Serialized)
    {
      Store (Arg0,HRDY)              // Store HID Ready Status.
      // Eventually code will communicate to platform the Driver status (enabled/disabled).
    }

    //
    // HID Platform Event Method - called by Platform to communicate HID Event to Driver.
    // Input:
    // Mode 0 = Index of HID Event.
    // Mode 1 = Package containing a complete HID Report.
    //
    Method (HPEM,1,Serialized)                        // HID Platform Event Method.
    {
      Store (1,HBSY)                                  // Set HID Busy.
      // Simple Mode is hardcoded for now.  Simply store HID Index value.
      If (LEqual (HMDE,0))
      {
         Store (Arg0,HIDX)
      } Else {
         Store (Arg0,HIDX)
      }
      Notify (\_SB.HIDD,0xC0)                         // Notify Driver to get HID Event.
      Store (0,Local0)                                // Initialize Local0 as a timeout counter.
      While (LAnd (LLess (Local0,250),HBSY))          // Wait <= 1 second for Driver to ACK success.
      {
        Sleep (4)                                     // Delay 4 ms.
        Increment (Local0)                            // Increment Timeout.
      }
      If (LEqual (HBSY,1))                            // Failure?
      {
        Store (0,HBSY)                                // Yes.  Clear HID Busy Flag.
        Store (0,HIDX)                                // Set HID Simple Mode Index = 0 = Undefined.
        Return (1)                                    // Return Failure.
      } Else {
        Return (0)                                    // Return Success.
      }
    }
  }
}//end scope _SB
