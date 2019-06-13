/**@file
  PAC193x Device of ACPI

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device(PA01)
{
  Name(_HID, "MCHP1930")
  Name(_UID, 1)
  //
  // Lowest power D-State supported by the device is D3
  //
  Name (_S0W, 3)
  //
  // Status:
  // Device is present, enabled, and functioning properly
  //
  Method(_STA, 0x0, NotSerialized)
  {
    If (POME) {
      Return(0xf)
    }
    Return (0x00)
  }

  //
  // Current Resources Settings
  //
  Method(_CRS, 0x0, NotSerialized)
  {
    Name(RBUF, ResourceTemplate()
    {
    //
    // OEM: TODO: Specify the details of how this
    // 139x device is connected to the system.
    //
    I2CSerialBus(0x18, // 7-bit Slave Address
    ControllerInitiated, // Slave or master?
    400000, // Connection Speed in hz
    AddressingMode7Bit, // 7-bit or 10-bit addressing?
    "\\_SB.PCI0.I2C4", // I2C Controller to which PAC is connected
    0, // Resource Index
    ResourceConsumer) // Consumer or Producer?
    })
    Return(RBUF)
  }

  //
  // _DSM - Device Specific Method
  //
  // This method returns configuration information that tells the driver
  // which devices each line in the PAC193x is wired to monitor.
  // Names of the monitored devices must come from the Microsoft-defined
  // power rail taxonomy.
  //
  // The UUID for the Microchip PAC193x's DSM is {033771E0-1705-47B4-9535-D1BBE14D9A09}.
  // This is unique to the device, and must match what the Windows PAC193x driver expects.
  //
  // Returns:
  // Either: A Buffer (for Functions 0 and 2, or an error)
  // A Package containing EMI resources (rail/resistor pairs)
  //
  // Input Arguments (per _DSM standard):
  //
  // Arg0: UUID - Function Identifier
  // Arg1: Integer - Revision
  // Arg2: Integer - Function Index
  // Arg3: Package - Parameters (not used in our implementation)
  //
  Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
  {
    //
    // Is our UUID being invoked?
    //
    if(LNotEqual(Arg0, ToUUID("033771E0-1705-47B4-9535-D1BBE14D9A09")))
    {
      //
      // It's NOT our UUID -- Just return a NULL for error
      //
      return(Buffer() {0x0})
    }
    //
    // Switch based on the function number requested...
    //
    switch(ToInteger(Arg2))
    {
      //
      // Function zero returns a bit-mask of supported functions
      //
      case(0)
      {
        //
        // Revision 0 is the only revision presently defined
        //
        if(LEqual(Arg1, 0x00))
        {
          //
          // Functions 0 and 1 are supported
          //
          return(Buffer() {0x03})
        }
        break;
      }
      //
      // Function one returns the Channel Info Package
      //
      case(1)
      {
        //
        // Revision 0 is the only revision presently defined.
        //
        If(LEqual(Arg1, 0x0))
        {
          //
          // Return channel info with alternating names and resistor values.
          //
          // OEM: TODO: The supplied values ARE ONLY EXAMPLES.
          // Change to match wiring on platform.
          //
          // Each power rail name must be a valid Windows-defined
          // "Metered Hardware Name" or the empty string -- it is
          // not validated by the PAC193x driver. Set the power
          // rail name to the empty string (i.e. "") to indicate
          // that a given channel is Private (i.e. not reported to
          // Windows via the standard EMI interface).
          //
          // Each RSENSE value is expressed in milliohms. If the
          // channel is not used, set the RSENSE value to zero.
          //
          Name(PBUF, Package()
          {
            //
            // Power
            // Rail RSENSE
            // Name Value
            //
            "CPU_SA", 2, // Channel 1
            "CPU_1", 2, // Channel 2
            "CPU_2", 2, // Channel 3
            "STORAGE", 5 // Channel 4
          })
          return(PBUF)
        }
        break;
      }
    } // switch(Arg2)
    //
    // Return an error (a buffer with a value of zero)
    // if we didn't return anything else above
    //
    return(Buffer() {0x0})
  } // _DSM
}

Device(PA02)
{
  Name(_HID, "MCHP1930")
  Name(_UID, 2)
  //
  // Lowest power D-State supported by the device is D3
  //
  Name (_S0W, 3)
  //
  // Status:
  // Device is present, enabled, and functioning properly
  //
  Method(_STA, 0x0, NotSerialized)
  {
    If (POME) {
      Return(0xf)
    }
    Return (0x00)
  }
  //
  // Current Resources Settings
  //
  Method(_CRS, 0x0, NotSerialized)
  {
    Name(RBUF, ResourceTemplate()
    {
    //
    // OEM: TODO: Specify the details of how this
    // 139x device is connected to the system.
    //
    I2CSerialBus(0x19, // 7-bit Slave Address
    ControllerInitiated, // Slave or master?
    400000, // Connection Speed in hz
    AddressingMode7Bit, // 7-bit or 10-bit addressing?
    "\\_SB.PCI0.I2C4", // I2C Controller to which PAC is connected
    0, // Resource Index
    ResourceConsumer) // Consumer or Producer?
    })
    Return(RBUF)
  }

  //
  // _DSM - Device Specific Method
  //
  // This method returns configuration information that tells the driver
  // which devices each line in the PAC193x is wired to monitor.
  // Names of the monitored devices must come from the Microsoft-defined
  // power rail taxonomy.
  //
  // The UUID for the Microchip PAC193x's DSM is {033771E0-1705-47B4-9535-D1BBE14D9A09}.
  // This is unique to the device, and must match what the Windows PAC193x driver expects.
  //
  // Returns:
  // Either: A Buffer (for Functions 0 and 2, or an error)
  // A Package containing EMI resources (rail/resistor pairs)
  //
  // Input Arguments (per _DSM standard):
  //
  // Arg0: UUID - Function Identifier
  // Arg1: Integer - Revision
  // Arg2: Integer - Function Index
  // Arg3: Package - Parameters (not used in our implementation)
  //
  Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
  {
    //
    // Is our UUID being invoked?
    //
    if(LNotEqual(Arg0, ToUUID("033771E0-1705-47B4-9535-D1BBE14D9A09")))
    {
      //
      // It's NOT our UUID -- Just return a NULL for error
      //
      return(Buffer() {0x0})
    }
    //
    // Switch based on the function number requested...
    //
    switch(ToInteger(Arg2))
    {
      //
      // Function zero returns a bit-mask of supported functions
      //
      case(0)
      {
        //
        // Revision 0 is the only revision presently defined
        //
        if(LEqual(Arg1, 0x00))
        {
          //
          // Functions 0 and 1 are supported
          //
          return(Buffer() {0x03})
        }
        break;
      }
      //
      // Function one returns the Channel Info Package
      //
      case(1)
      {
        //
        // Revision 0 is the only revision presently defined.
        //
        If(LEqual(Arg1, 0x0))
        {
          //
          // Return channel info with alternating names and resistor values.
          //
          // OEM: TODO: The supplied values ARE ONLY EXAMPLES.
          // Change to match wiring on platform.
          //
          // Each power rail name must be a valid Windows-defined
          // "Metered Hardware Name" or the empty string -- it is
          // not validated by the PAC193x driver. Set the power
          // rail name to the empty string (i.e. "") to indicate
          // that a given channel is Private (i.e. not reported to
          // Windows via the standard EMI interface).
          //
          // Each RSENSE value is expressed in milliohms. If the
          // channel is not used, set the RSENSE value to zero.
          //
          Name(PBUF, Package()
          {
            //
            // Power
            // Rail RSENSE
            // Name Value
            //
            "DISPLAY_BKLT", 0x0A, // Channel 1
            "MAINMEM_MEMORY", 2, // Channel 2
            "MAINMEM_CPU", 2, // Channel 3
            "", 0,            //Channel 4
          })
          return(PBUF)
        }
        break;
      }
    } // switch(Arg2)
    //
    // Return an error (a buffer with a value of zero)
    // if we didn't return anything else above
    //
    return(Buffer() {0x0})
  } // _DSM
}

Device(PA03)
{
  Name(_HID, "MCHP1930")
  Name(_UID, 3)
  //
  // Lowest power D-State supported by the device is D3
  //
  Name (_S0W, 3)
  //
  // Status:
  // Device is present, enabled, and functioning properly
  //
  Method(_STA, 0x0, NotSerialized)
  {
    If (POME) {
      Return(0xf)
    }
    Return (0x00)
  }
  //
  // Current Resources Settings
  //
  Method(_CRS, 0x0, NotSerialized)
  {
    Name(RBUF, ResourceTemplate()
    {
    //
    // OEM: TODO: Specify the details of how this
    // 139x device is connected to the system.
    //
    I2CSerialBus(0x1A, // 7-bit Slave Address
    ControllerInitiated, // Slave or master?
    400000, // Connection Speed in hz
    AddressingMode7Bit, // 7-bit or 10-bit addressing?
    "\\_SB.PCI0.I2C4", // I2C Controller to which PAC is connected
    0, // Resource Index
    ResourceConsumer) // Consumer or Producer?
    })
    Return(RBUF)
  }

  //
  // _DSM - Device Specific Method
  //
  // This method returns configuration information that tells the driver
  // which devices each line in the PAC193x is wired to monitor.
  // Names of the monitored devices must come from the Microsoft-defined
  // power rail taxonomy.
  //
  // The UUID for the Microchip PAC193x's DSM is {033771E0-1705-47B4-9535-D1BBE14D9A09}.
  // This is unique to the device, and must match what the Windows PAC193x driver expects.
  //
  // Returns:
  // Either: A Buffer (for Functions 0 and 2, or an error)
  // A Package containing EMI resources (rail/resistor pairs)
  //
  // Input Arguments (per _DSM standard):
  //
  // Arg0: UUID - Function Identifier
  // Arg1: Integer - Revision
  // Arg2: Integer - Function Index
  // Arg3: Package - Parameters (not used in our implementation)
  //
  Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
  {
    //
    // Is our UUID being invoked?
    //
    if(LNotEqual(Arg0, ToUUID("033771E0-1705-47B4-9535-D1BBE14D9A09")))
    {
      //
      // It's NOT our UUID -- Just return a NULL for error
      //
      return(Buffer() {0x0})
    }
    //
    // Switch based on the function number requested...
    //
    switch(ToInteger(Arg2))
    {
      //
      // Function zero returns a bit-mask of supported functions
      //
      case(0)
      {
        //
        // Revision 0 is the only revision presently defined
        //
        if(LEqual(Arg1, 0x00))
        {
          //
          // Functions 0 and 1 are supported
          //
          return(Buffer() {0x03})
        }
        break;
      }
      //
      // Function one returns the Channel Info Package
      //
      case(1)
      {
        //
        // Revision 0 is the only revision presently defined.
        //
        If(LEqual(Arg1, 0x0))
        {
          //
          // Return channel info with alternating names and resistor values.
          //
          // OEM: TODO: The supplied values ARE ONLY EXAMPLES.
          // Change to match wiring on platform.
          //
          // Each power rail name must be a valid Windows-defined
          // "Metered Hardware Name" or the empty string -- it is
          // not validated by the PAC193x driver. Set the power
          // rail name to the empty string (i.e. "") to indicate
          // that a given channel is Private (i.e. not reported to
          // Windows via the standard EMI interface).
          //
          // Each RSENSE value is expressed in milliohms. If the
          // channel is not used, set the RSENSE value to zero.
          //
          Name(PBUF, Package()
          {
            //
            // Power
            // Rail RSENSE
            // Name Value
            //
            "DISPLAY_PANEL", 0x0A, // Channel 1
            "GPU_1", 2, // Channel 2
            "GPU_2", 2, // Channel 3
            "SYSTEM_POWER", 2 // Channel 4
          })
          return(PBUF)
        }
        break;
      }
    } // switch(Arg2)
    //
    // Return an error (a buffer with a value of zero)
    // if we didn't return anything else above
    //
    return(Buffer() {0x0})
  } // _DSM
}
