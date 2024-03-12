/** @file
  Intel ACPI Sample Code for WWAN Firmware Flash Device

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB) {
  //
  // WWAN Flash Reset Method
  //
  Method(WFRT, 3, Serialized) {
    // Acquire Reset Mutex and save result to check for acquire Mutex
    Store (Acquire (\WWMT, 1000), Local0)
    // Check for Reset Mutex acquired
    If (LEqual (Local0, 0)) {
      Store (1, Local0)
      If (LEqual(WWRP, 1)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP01
      }
      If (LEqual(WWRP, 2)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP02
      }
      If (LEqual(WWRP, 3)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP03
      }
      If (LEqual(WWRP, 4)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP04
      }
      If (LEqual(WWRP, 5)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP05
      }
      If (LEqual(WWRP, 6)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP06
      }
      If (LEqual(WWRP, 7)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP07
      }
      If (LEqual(WWRP, 8)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP08
      }
      If (LEqual(WWRP, 9)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP09
      }
      If (LEqual(WWRP, 13)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP13
      }
      If (LEqual(WWRP, 17)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP17
      }
      If (LEqual(WWRP, 21)) {
        #undef WWAN_PCIE_ROOT_PORT
        #define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP21
      }
      If (LEqual (Local0, 1)) {
        Release (\WWMT)
      }
    }
  }

  Device(MFFD) {
    //
    // Modem Firmware Flash Device
    //
    Name(_HID, "INTC1073")
    Name(_STR, Unicode ("Modem Firmware Flash Device"))

    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA) {
      //
      // The WWAN Firmware Flash Device can support when WWAN support is enabled.
      //
      If (LEqual(WFFD, 1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    //--------------------------------------------------
    //  Modem Firmware Flash Device _DSM Method
    //--------------------------------------------------
    //
    // _DSM (Device Specific Method)
    //
    // This optional object is a control method that enables devices to provide device specific control
    // functions that are consumed by the device driver.
    //
    // Arguments: (4)
    //   Arg0 - A Buffer containing a UUID
    //   Arg1 - An Integer containing the Revision ID
    //   Arg2 - An Integer containing the Function Index
    //   Arg3 - A Package that contains function-specific arguments
    // Return Value:
    //   If Function Index = 0, a Buffer containing a function index bit-field which has set of supported function indexes.
    //   Otherwise, the return feature value based on set of supported function type which depends on the UUID and revision ID.
    //
    Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj}) {
      If(LEqual(Arg0, ToUUID("0F3D57AA-6046-47B2-8A9A-88F1021D3865"))) {
        //
        // Function 0 : Query Function/Get supported functions (based on revision)
        //              For Revision 0,
        //              -- Return value would contain one bit for each function index. Starting with zero.
        //              -- Bit 0 indicates whether there is support for any functions other than function 0 for the specified UUID and Revision ID.
        //              -- If set to zero, no functions are supported.
        //
        If(LEqual(Arg2, Zero)) {
          // Revision 0
          If(LEqual(Arg1, Zero)){ // The current revision is 0
            //
            // Below are functions defined.
            // Bit0    - Indicates whether its support for any other functions
            // Bit1    - Warm Reset (Toggle RESET# Signals)
            // Bit2    - Cold Reset (Full Power Cycle)
            // Bit31:3 - Reserved for future options
            Return(Buffer(){0x07})
          } Else {
            Return(Buffer(){0}) // No supported functions.
          }
        }

        //
        // Function 1 : Warm Reset (Toggle RESET# Signals - FLDR)
        //
        If(LEqual(Arg2, One)) {
          WFRT (0, DeRefOf (Index (Arg3, 0)), DeRefOf (Index (Arg3, 1))) // Perform Modem Warm Reset Flow
          Return(Buffer(){0})
        }

        //
        // Function 2 : Cold Reset (Full Power Cycle - PLDR)
        //
        If(LEqual(Arg2, 2)) {
          WFRT (1, DeRefOf (Index (Arg3, 0)), DeRefOf (Index (Arg3, 1))) // Perform Modem Full Cold Reset (PLDR) Flow
          Return(Buffer(){0})
        }

        //
        // No other supported functions, but normal return.
        //
        Return(0)
      } Else {
        Return(Buffer(){0}) // Guid mismatch
      }
    }
  } // End of Device(MFFD)
}