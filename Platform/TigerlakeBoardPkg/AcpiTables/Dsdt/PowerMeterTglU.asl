/** @file
  Power Meter Device Configuration

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.PC00.I2C5)
{
  //
  // PA01 Device: Defines how the PAC193x is connected to the I2C Controller.
  //
  Device(PA01)
  {
    Name(_HID, "MCHP1930")
    Name(_UID, 1)

    // Lowest power D-State supported by the device is D3
    Name (_S0W, 3)

    // Device Status: present, enabled, and functioning properly
    Method(_STA, 0x0, NotSerialized)
    {
      If(POME)
      {
        Switch (ToInteger(PLID)) {
          Case (Package () {BoardIdTglUDdr4, BoardIdTglULp4Type4, BoardIdTglULp4Gcs}) {
            Return(0x0F)
          }
        }
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
        I2CSerialBus(0x0,                   // 7-bit Slave Address
                    ControllerInitiated,    // Slave or master?
                    400000,                 // Connection Speed in hz
                    AddressingMode7Bit,     // 7-bit or 10-bit addressing?
                    "\\_SB.PC00.I2C5",      // I2C Controller to which PAC is connected
                    0,                      // Resource Index
                    ResourceConsumer,       // Consumer or Producer?
                    I2CB)                    // Descriptor name
      })
      CreateWordField(RBUF,I2CB._ADR,BADR)
      Switch (ToInteger(PLID)) {
        Case (BoardIdTglULp4Gcs) {
          Store (0x10, BADR)
        }
        Default {
          Store (0x18, BADR)
        }
      }
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
    //         Either: A Buffer (for Functions 0 and 2, or an error)
    //                 A Package containing EMI resources (rail/resistor pairs)
    //
    // Input Arguments (per _DSM standard):
    //
    //     Arg0: UUID - Function Identifier
    //     Arg1: Integer - Revision
    //     Arg2: Integer - Function Index
    //     Arg3: Package - Parameters (not used in our implementation)
    //
    Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
    {
      //
      //  Is our UUID being invoked?
      //
      if(LNotEqual(Arg0, ToUUID("033771E0-1705-47B4-9535-D1BBE14D9A09")))
      {
        return(Buffer() {0x0})  // incorrect UUID, return NULL for error
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
          switch(ToInteger(Arg1)) // revision check
          {
            case(0)
            {
              return (Buffer() {0x3}) // Revision 0: function 1 is supported
            }
            case(1)
            {
              return (Buffer() {0x7f}) // Revision 1: functions 1->6 are supported
            }
          }
          break;
        }

        // Function 1 returns the channel "Rail Name" and the Resistor Value expressed in milli-Ohms.
        case(1)
        {
          // All Revisions supported (0 and 1 presently defined)
          Name(PKG1, Package()
          {
            Package()
            {
              // Rail Name, Resistor Value
              "VCCIN_PH1",      2, // Channel 1
              "VCCIN_PH2",      2, // Channel 2
              "VCCIN_PH3",      2, // Channel 3
              "V3.3A_DSW_VR",   2  // Channel 4
            },
            Package()
            {
              // Rail Name, Resistor Value
              "AUDIO_CODEC",   10, // Channel 1
              "CAMERA",        10, // Channel 2
              "STORAGE",       5,  // Channel 3
              "CPU_PH1",       1   // Channel 4
            }
          })
          Switch (ToInteger(PLID)) {
            Case (BoardIdTglULp4Gcs) {
              return(DerefOf(Index(PKG1, 1)))
            }
            Default {
              return(DerefOf(Index(PKG1, 0)))
            }
          }
        }

        // Function 2 returns the Resistor values expressed in micro-Ohms.
        case(2)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return enhanced precision resistor values.
          Name(PKG2, Package()
          {
            Package()
            {
              // Value
              2000, // Channel 1
              2000, // Channel 2
              2000, // Channel 3
              2000  // Channel 4
            },
            Package()
            {
              // Value
              10000, // Channel 1
              10000, // Channel 2
              5000,  // Channel 3
              500    // Channel 4
            }
          })
          Switch (ToInteger(PLID)) {
            Case (BoardIdTglULp4Gcs) {
              return(DerefOf(Index(PKG2, 1)))
            }
            Default {
              return(DerefOf(Index(PKG2, 0)))
            }
          }
        }

        // Function 3 returns the EMI enabled/disabled flags
        case(3)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return ON/OFF flags.
          Name(BUF3, Package()
          {
            0xF // driver EMI ENABLE mask, CH1:CH2:CH3:CH4 - 1b=ON, 0b=OFF
            // EMI mask bit set but rail name is NULL -> EMI is not created (private channel)
            // channel name valid but mask bit not set -> EMI is not created (private channel)
          })
          return(BUF3)
        }

        // Function 4 returns the channel bipolar settings
        case(4)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return the target NEG_PWR flags.
          Name(BUF4, Package()
          {
            0x00 // CH1:CH2:CH3:CH4-BIDI : CH1:CH2:CH3:CH4-BIDV
          })
          return(BUF4)
        }

        // Function 5 returns the SPS for ACTIVE and IDLE
        case(5)
        {
        If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          Name(BUF5, Package()
          {
            1024, // ACTIVE sps - accepted values = {1024, 256, 64, 8}
            8 // IDLE sps - accepted values = {1024, 256, 64, 8}
          })
          return(BUF5)
        }

        // Function 6 returns the watchdog interval
        case(6)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          Name(BUF6, Package()
          {
            900 // seconds (min=60, max=900)
          })
          return(BUF6)
        }

      } // switch(Arg2)
      //
      // Return an error (a buffer with a value of zero)
      // if we didn't return anything else above
      //
      return(Buffer() {0x0})
    } // _DSM
  } // PowerMeter Device PA01 Scope End

  //
  // PA02 Device: Defines how the PAC193x is connected to the I2C Controller.
  //
  Device(PA02)
  {
    Name(_HID, "MCHP1930")
    Name(_UID, 2)

    // Lowest power D-State supported by the device is D3
    Name (_S0W, 3)

    // Device Status: present, enabled, and functioning properly
    Method(_STA, 0x0, NotSerialized)
    {
      If(POME)
      {
        Switch (ToInteger(PLID)) {
          Case (Package () {BoardIdTglUDdr4, BoardIdTglULp4Type4, BoardIdTglULp4Gcs}) {
            Return(0x0F)
          }
        }
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
        I2CSerialBus(0x0,                   // 7-bit Slave Address
                    ControllerInitiated,    // Slave or master?
                    400000,                 // Connection Speed in hz
                    AddressingMode7Bit,     // 7-bit or 10-bit addressing?
                    "\\_SB.PC00.I2C5",      // I2C Controller to which PAC is connected
                    0,                      // Resource Index
                    ResourceConsumer,       // Consumer or Producer?
                    I2CB)                   // Descriptor name
      })
      CreateWordField(RBUF,I2CB._ADR,BADR)
      Switch (ToInteger(PLID)) {
        Case (BoardIdTglULp4Gcs) {
          Store (0x12, BADR)
        }
        Default {
          Store (0x11, BADR)
        }
      }
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
    //         Either: A Buffer (for Functions 0 and 2, or an error)
    //                 A Package containing EMI resources (rail/resistor pairs)
    //
    // Input Arguments (per _DSM standard):
    //
    //     Arg0: UUID - Function Identifier
    //     Arg1: Integer - Revision
    //     Arg2: Integer - Function Index
    //     Arg3: Package - Parameters (not used in our implementation)
    //
    Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
    {
      //
      //  Is our UUID being invoked?
      //
      if(LNotEqual(Arg0, ToUUID("033771E0-1705-47B4-9535-D1BBE14D9A09")))
      {
        return(Buffer() {0x0})  // incorrect UUID, return NULL for error
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
          switch(ToInteger(Arg1)) // revision check
          {
            case(0)
            {
              return (Buffer() {0x3}) // Revision 0: function 1 is supported
            }
            case(1)
            {
              return (Buffer() {0x7f}) // Revision 1: functions 1->6 are supported
            }
          }
          break;
        }

        // Function 1 returns the channel "Rail Name" and the Resistor Value expressed in milli-Ohms.
        case(1)
        {
          // All Revisions supported (0 and 1 presently defined)
          Name(PKG1, Package()
          {
            Package()
            {
              // Rail Name, Resistor Value
              "SSD",       5, // Channel 1
              "WLAN",      5, // Channel 2
              "VBATA",     2, // Channel 3
              "BACKLIGHT", 2  // Channel 4
            },
            Package()
            {
              // Rail Name, Resistor Value
              "WIFI",              5,  // Channel 1
              "",                  0,  // Channel 2
              "DISPLAY_PANEL",     10, // Channel 3
              "DISPLAY_BACKLIGHT", 10  // Channel 4
            }
          })
          Switch (ToInteger(PLID)) {
            Case (BoardIdTglULp4Gcs) {
              return(DerefOf(Index(PKG1, 1)))
            }
            Default {
              return(DerefOf(Index(PKG1, 0)))
            }
          }
        }

        // Function 2 returns the Resistor values expressed in micro-Ohms.
        case(2)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return enhanced precision resistor values.
          Name(PKG2, Package()
          {
            Package()
            {
              // Value
              5000, // Channel 1
              5000, // Channel 2
              2000, // Channel 3
              2000  // Channel 4
            },
            Package()
            {
              // Value
              5000,  // Channel 1
              0,     // Channel 2
              10000, // Channel 3
              10000  // Channel 4
            }
          })
          Switch (ToInteger(PLID)) {
            Case (BoardIdTglULp4Gcs) {
              return(DerefOf(Index(PKG2, 1)))
            }
            Default {
              return(DerefOf(Index(PKG2, 0)))
            }
          }
        }

        // Function 3 returns the EMI enabled/disabled flags
        case(3)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return ON/OFF flags.
          Name(BUF3, Package()
          {
            0xF // driver EMI ENABLE mask, CH1:CH2:CH3:CH4 - 1b=ON, 0b=OFF
            // EMI mask bit set but rail name is NULL -> EMI is not created (private channel)
            // channel name valid but mask bit not set -> EMI is not created (private channel)
          })
          return(BUF3)
        }

        // Function 4 returns the channel bipolar settings
        case(4)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return the target NEG_PWR flags.
          Name(BUF4, Package()
          {
            0x00 // CH1:CH2:CH3:CH4-BIDI : CH1:CH2:CH3:CH4-BIDV
          })
          return(BUF4)
        }

        // Function 5 returns the SPS for ACTIVE and IDLE
        case(5)
        {
        If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          Name(BUF5, Package()
          {
            1024, // ACTIVE sps - accepted values = {1024, 256, 64, 8}
            8 // IDLE sps - accepted values = {1024, 256, 64, 8}
          })
          return(BUF5)
        }

        // Function 6 returns the watchdog interval
        case(6)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          Name(BUF6, Package()
          {
            900 // seconds (min=60, max=900)
          })
          return(BUF6)
        }
      } // switch(Arg2)
      //
      // Return an error (a buffer with a value of zero)
      // if we didn't return anything else above
      //
      return(Buffer() {0x0})
    } // _DSM
  } // PowerMeter Device PA02 Scope End

  //
  // PA03 Device: Defines how the PAC193x is connected to the I2C Controller.
  //
  Device(PA03)
  {
    Name(_HID, "MCHP1930")
    Name(_UID, 3)

    // Lowest power D-State supported by the device is D3
    Name (_S0W, 3)

    // Device Status: present, enabled, and functioning properly
    Method(_STA, 0x0, NotSerialized)
    {
      If(POME)
      {
        Switch (ToInteger(PLID)) {
          Case (Package () {BoardIdTglUDdr4, BoardIdTglULp4Type4, BoardIdTglULp4Gcs}) {
            Return(0x0F)
          }
        }
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
        I2CSerialBus(0x0,                   // 7-bit Slave Address
                    ControllerInitiated,    // Slave or master?
                    400000,                 // Connection Speed in hz
                    AddressingMode7Bit,     // 7-bit or 10-bit addressing?
                    "\\_SB.PC00.I2C5",      // I2C Controller to which PAC is connected
                    0,                      // Resource Index
                    ResourceConsumer,       // Consumer or Producer?
                    I2CB)                   // Descriptor name
      })
      CreateWordField(RBUF,I2CB._ADR,BADR)
      Switch (ToInteger(PLID)) {
        Case (BoardIdTglULp4Gcs) {
          Store (0x11, BADR)
        }
        Default {
          Store (0x1E, BADR)
        }
      }
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
    //         Either: A Buffer (for Functions 0 and 2, or an error)
    //                 A Package containing EMI resources (rail/resistor pairs)
    //
    // Input Arguments (per _DSM standard):
    //
    //     Arg0: UUID - Function Identifier
    //     Arg1: Integer - Revision
    //     Arg2: Integer - Function Index
    //     Arg3: Package - Parameters (not used in our implementation)
    //
    Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
    {
      //
      //  Is our UUID being invoked?
      //
      if(LNotEqual(Arg0, ToUUID("033771E0-1705-47B4-9535-D1BBE14D9A09")))
      {
        return(Buffer() {0x0})  // incorrect UUID, return NULL for error
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
          switch(ToInteger(Arg1)) // revision check
          {
            case(0)
            {
              return (Buffer() {0x3}) // Revision 0: function 1 is supported
            }
            case(1)
            {
              return (Buffer() {0x7f}) // Revision 1: functions 1->6 are supported
            }
          }
          break;
        }

        // Function 1 returns the channel "Rail Name" and the Resistor Value expressed in milli-Ohms.
        case(1)
        {
          // All Revisions supported (0 and 1 presently defined)
          Name(PKG1, Package()
          {
            Package()
            {
              // Rail Name, Resistor Value
              "VDD2_CPU",     1, // Channel 1
              "VDD2_MEM",     2, // Channel 2
              "VCCIN_AUX",    2, // Channel 3
              "V1.8A_MCP",    5  // Channel 4
            },
            Package()
            {
              // Rail Name, Resistor Value
              "CPU_PH2",      1, // Channel 1
              "CPU_PH3",      1, // Channel 2
              "SOC",          1, // Channel 3
              "MAINMEM_VDD2", 2  // Channel 4
            }
          })
          Switch (ToInteger(PLID)) {
            Case (BoardIdTglULp4Gcs) {
              return(DerefOf(Index(PKG1, 1)))
            }
            Default {
              return(DerefOf(Index(PKG1, 0)))
            }
          }
        }

        // Function 2 returns the Resistor values expressed in micro-Ohms.
        case(2)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return enhanced precision resistor values.
          Name(PKG2, Package()
          {
            Package()
            {
              // Value
              1000, // Channel 1
              2000, // Channel 2
              2000, // Channel 3
              5000  // Channel 4
            },
            Package()
            {
              // Value
              500, // Channel 1
              500, // Channel 2
              500, // Channel 3
              2000 // Channel 4
            }
          })
          Switch (ToInteger(PLID)) {
            Case (BoardIdTglULp4Gcs) {
              return(DerefOf(Index(PKG2, 1)))
            }
            Default {
              return(DerefOf(Index(PKG2, 0)))
            }
          }
        }

        // Function 3 returns the EMI enabled/disabled flags
        case(3)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return ON/OFF flags.
          Name(BUF3, Package()
          {
            0xF // driver EMI ENABLE mask, CH1:CH2:CH3:CH4 - 1b=ON, 0b=OFF
            // EMI mask bit set but rail name is NULL -> EMI is not created (private channel)
            // channel name valid but mask bit not set -> EMI is not created (private channel)
          })
          return(BUF3)
        }

        // Function 4 returns the channel bipolar settings
        case(4)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return the target NEG_PWR flags.
          Name(BUF4, Package()
          {
            0x00 // CH1:CH2:CH3:CH4-BIDI : CH1:CH2:CH3:CH4-BIDV
          })
          return(BUF4)
        }

        // Function 5 returns the SPS for ACTIVE and IDLE
        case(5)
        {
        If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          Name(BUF5, Package()
          {
            1024, // ACTIVE sps - accepted values = {1024, 256, 64, 8}
               8  // IDLE sps - accepted values = {1024, 256, 64, 8}
          })
          return(BUF5)
        }

        // Function 6 returns the watchdog interval
        case(6)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          Name(BUF6, Package()
          {
            900 // seconds (min=60, max=900)
          })
          return(BUF6)
        }

      } // switch(Arg2)
      //
      // Return an error (a buffer with a value of zero)
      // if we didn't return anything else above
      //
      return(Buffer() {0x0})
    } // _DSM
  } // PowerMeter Device PA03 Scope End

  //
  // PA04 Device: Defines how the PAC193x is connected to the I2C Controller.
  //
  Device(PA04)
  {
    Name(_HID, "MCHP1930")
    Name(_UID, 4)

    // Lowest power D-State supported by the device is D3
    Name (_S0W, 3)

    // Device Status: present, enabled, and functioning properly
    Method(_STA, 0x0, NotSerialized)
    {
      If(POME)
      {
        Switch (ToInteger(PLID)) {
          Case (BoardIdTglULp4Gcs) {
            Return(0x0F)
          }
        }
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
        I2CSerialBus(0x0,                   // 7-bit Slave Address
                    ControllerInitiated,    // Slave or master?
                    400000,                 // Connection Speed in hz
                    AddressingMode7Bit,     // 7-bit or 10-bit addressing?
                    "\\_SB.PC00.I2C5",      // I2C Controller to which PAC is connected
                    0,                      // Resource Index
                    ResourceConsumer,       // Consumer or Producer?
                    I2CB)                   // Descriptor name
      })
      CreateWordField(RBUF,I2CB._ADR,BADR)
      Switch (ToInteger(PLID)) {
        Case (BoardIdTglULp4Gcs) {
          Store (0x13, BADR)
        }
        Default {
          Store (0x0, BADR)
        }
      }
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
    //         Either: A Buffer (for Functions 0 and 2, or an error)
    //                 A Package containing EMI resources (rail/resistor pairs)
    //
    // Input Arguments (per _DSM standard):
    //
    //     Arg0: UUID - Function Identifier
    //     Arg1: Integer - Revision
    //     Arg2: Integer - Function Index
    //     Arg3: Package - Parameters (not used in our implementation)
    //
    Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
    {
      //
      //  Is our UUID being invoked?
      //
      if(LNotEqual(Arg0, ToUUID("033771E0-1705-47B4-9535-D1BBE14D9A09")))
      {
        return(Buffer() {0x0})  // incorrect UUID, return NULL for error
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
          switch(ToInteger(Arg1)) // revision check
          {
            case(0)
            {
              return (Buffer() {0x3}) // Revision 0: function 1 is supported
            }
            case(1)
            {
              return (Buffer() {0x7f}) // Revision 1: functions 1->6 are supported
            }
          }
          break;
        }

        // Function 1 returns the channel "Rail Name" and the Resistor Value expressed in milli-Ohms.
        case(1)
        {
          // All Revisions supported (0 and 1 presently defined)
          Name(PKG1, Package()
          {
            // Rail Name, Resistor Value
            Package () {
              "",     0, // Channel 1
              "",     0, // Channel 2
              "",     0, // Channel 3
              "",     0  // Channel 4
            },
            Package () {
              "MAINMEM_LP4x",          5, // Channel 1
              "MAINMEM_VDD2CPU",       2, // Channel 2
              "MAINMEM_VPP",           5, // Channel 3
              "SYSTEM",                2  // Channel 4
            }
          })
          Switch (ToInteger(PLID)) {
            Case (BoardIdTglULp4Gcs) {
              return(DerefOf(Index(PKG1, 1)))
            }
            Default {
              return(DerefOf(Index(PKG1, 0)))
            }
          }
        }

        // Function 2 returns the Resistor values expressed in micro-Ohms.
        case(2)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return enhanced precision resistor values.
          Name(PKG2, Package()
          {
            // Value
            Package () {
              0, // Channel 1
              0, // Channel 2
              0, // Channel 3
              0  // Channel 4
            },
            Package () {
               5000, // Channel 1
               2000, // Channel 2
               5000, // Channel 3
               2000  // Channel 4
            }
          })
          Switch (ToInteger(PLID)) {
            Case (BoardIdTglULp4Gcs) {
              return(DerefOf(Index(PKG2, 1)))
            }
            Default {
              return(DerefOf(Index(PKG2, 0)))
            }
          }
        }

        // Function 3 returns the EMI enabled/disabled flags
        case(3)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return ON/OFF flags.
          Name(BUF3, Package()
          {
            0xF // driver EMI ENABLE mask, CH1:CH2:CH3:CH4 - 1b=ON, 0b=OFF
            // EMI mask bit set but rail name is NULL -> EMI is not created (private channel)
            // channel name valid but mask bit not set -> EMI is not created (private channel)
          })
          return(BUF3)
        }

        // Function 4 returns the channel bipolar settings
        case(4)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          // Return the target NEG_PWR flags.
          Name(BUF4, Package()
          {
            0x00 // CH1:CH2:CH3:CH4-BIDI : CH1:CH2:CH3:CH4-BIDV
          })
          return(BUF4)
        }

        // Function 5 returns the SPS for ACTIVE and IDLE
        case(5)
        {
        If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          Name(BUF5, Package()
          {
            1024, // ACTIVE sps - accepted values = {1024, 256, 64, 8}
               8  // IDLE sps - accepted values = {1024, 256, 64, 8}
          })
          return(BUF5)
        }

        // Function 6 returns the watchdog interval
        case(6)
        {
          If(LLess(Arg1,1)){ break; } // Revision 0 not supported.
          Name(BUF6, Package()
          {
            900 // seconds (min=60, max=900)
          })
          return(BUF6)
        }

      } // switch(Arg2)
      //
      // Return an error (a buffer with a value of zero)
      // if we didn't return anything else above
      //
      return(Buffer() {0x0})
    } // _DSM
  } // PowerMeter Device PA04 Scope End

} // Scope I2C5
