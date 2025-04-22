/** @file
  IOTG SIO

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

// This ASL file will support the F81804U HW monitor

Device (TFN2)
{
  Name (_HID, "INTC1048")  // Intel DPTF Fan Device
  Name (_UID, "TFN2")
  Name (_STR, Unicode ("Fan 2"))

  Method (_STA, 0, Serialized)
  {
    // Only report resources to the OS if the SIO Device is present.

    //
    // Steps: 1) If board does not support IT8659E return 0x0000 (no device).
    //        2) If bit set (IT8659E present) aquire mutex and set the SIO
    //           to COM A.
    //        3) Read enable register to Local0.
    //        4) Release mutex and exit config mode.
    //        5) If Local0 set return 0x000f (device present and active).
    //        6) Else return 0x000d (device present, inactive).
    //
    If (LEqual (FND2, 1)) {
      ENFG (0x04)
      Store (IT30, Local0)
      EXFG ()
      If (Local0) {
        Return (0x000F)
      }
      Return (0x000D)
    }

    Return (0x0000)
  }

  OperationRegion (HWEC, SystemIO, 0x295, 0x02)
  Field (HWEC, ByteAcc, Lock, Preserve)
  {
    INDX, 8,
    DATA, 8
  }

  // Use ACPI Defined IndexField so consecutive Index/Data I/Os are
  // assured to be uninterrupted.

  IndexField (INDX, DATA, ByteAcc, Lock, Preserve)
  {
    Offset(0x0A),
    R0AR , 8 ,   //enable SMB and PECI, 1 host clock
    Offset (0x0E),
    R2DR, 8,        // Fan Tachometer 2 Reading Register
    Offset (0x13),
    R13R, 8,        // active high PWM Fan output, default: 6MHz, 23.43KHz
    Offset (0x14),
    R14R, 8,        // active high PWM Fan output, default: 6MHz, 23.43KHz
    Offset (0x16),
    R16R, 8,        // FAN_CTL2 PWM Control Register
    Offset (0x2A),
    R2AR, 8,        // TMPIN2 Temperature Reading Register
    Offset (0x42),
    R42R, 8,        // TMPIN2 High limit register
    R43R, 8,        // TMPIN2 Low limit register
    Offset (0x51),
    R51R, 8,        // set Fan1/Fan2
    Offset (0x53),
    R52R, 8,        // TMPIN2 Thermal Output Limit Register
    Offset (0x56),
    R56R , 8 ,  // offset for the Tmp1 reading
    R57R,  8 ,  //offset for the Tmp2 reading
    Offset (0x5C),
    R5CR, 8,
    Offset (0x68),
    R68R, 8,        // SmartGuardian Automatic Mode Temperature Limit of OFF
    R69R, 8,        // SmartGuardian Automatic Mode Temperature Limit of Start
    R6AR, 8,        // SmartGuardian Automatic Mode Temperature Limit of Full Speed
    R6BR, 8,         // FAN_CTL2 SmartGuardian Automatic Mode start PWM register
    R6CR, 8
  }

  // _FIF (Fan Information)
  //
  // The optional _FIF object provides OSPM with fan device capability information.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //  A Package containing the fan device parameters.
  //
  Method (_FIF)
  {
    Return (Package (){
      0,                          // Revision:Integer
      1,                          // FineGrainControl:Integer Boolean
      2,                          // StepSize:Integer DWORD
      0                           // LowSpeedNotificationSupport:Integer Boolean
    })
  }

  // _FPS (Fan Performance States)
  //
  // Evaluates to a variable-length package containing a list of packages that describe the fan device's performance states.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //  A variable-length Package containing a Revision ID and a list of Packages that describe the fan device's performance states.
  //
  Method (_FPS,,,,PkgObj)
  {
    Return (Package()
    {
      0,      // Revision:Integer
      //        Control,  TripPoint,    Speed,  NoiseLevel, Power
      Package () {100,    0xFFFFFFFF,    255,   500,        5000},
      Package () {95,     0xFFFFFFFF,    240,   475,        4750},
      Package () {90,     0xFFFFFFFF,    220,   450,        4500},
      Package () {85,     0xFFFFFFFF,    200,   425,        4250},
      Package () {80,     0xFFFFFFFF,    180,   400,        4000},
      Package () {70,     0xFFFFFFFF,    150,   350,        3500},
      Package () {60,     0xFFFFFFFF,    130,   300,        3000},
      Package () {50,     0xFFFFFFFF,    110,   250,        2500},
      Package () {40,     0xFFFFFFFF,     90,   200,        2000},
      Package () {30,     0xFFFFFFFF,     70,   150,        1500},
      Package () {25,     0xFFFFFFFF,     50,   125,        1250},
      Package () {0,      0xFFFFFFFF,      0,     0,           0}    // OFF
    })
  }

  Store(0,R16R)       //Set automatic mode off
  Store(144,R6BR)  //Default fan speed value
  Name (FSLV, 50)

  // _FSL (Fan Set Level)
  //
  // The optional _FSL object is a control method that OSPM evaluates to set a fan device's speed (performance state) to a specific level.
  //
  // Arguments: (1)
  //  Arg0 - Level (Integer): conveys to the platform the fan speed level to be set.
  // Return Value:
  //  None
  //
  // Argument Information
  //  Arg0: Level. If the fan supports fine-grained control, Level is a percentage of maximum level (0-100)
  //  that the platform is to engage the fan. If the fan does not support fine-grained control,
  //  Level is a Control field value from a package in the _FPS object's package list.
  //  A Level value of zero causes the platform to turn off the fan.
  //
  Method(_FSL,1,Serialized)
  {
    Store(Arg0, FSLV)
    //
    //Turn off the fan
    //
    If(LEqual(Arg0, 0)) {
      Store(0,R6BR)
    }
    //
    //Fan speed level set by user. The fan speed range based on _FPS package.
    //
    If(LAnd(LGreaterEqual(Arg0, 25),LLess(Arg0, 30))) {
      Store(64,R6BR)
    }
    If(LAnd(LGreaterEqual(Arg0, 30),LLess(Arg0, 40))) {
      Store(80,R6BR)
    }
    If(LAnd(LGreaterEqual(Arg0, 40),LLess(Arg0, 50))) {
      Store(96,R6BR)
    }
    If(LAnd(LGreaterEqual(Arg0, 50),LLess(Arg0, 60))) {
      Store(144,R6BR)
    }
    If(LAnd(LGreaterEqual(Arg0, 60),LLess(Arg0, 70))) {
      Store(160,R6BR)
    }
    If(LAnd(LGreaterEqual(Arg0, 70),LLess(Arg0, 80))) {
      Store(176,R6BR)
    }
    If(LAnd(LGreaterEqual(Arg0, 80),LLess(Arg0, 85))) {
      Store(192,R6BR)
    }
    If(LAnd(LGreaterEqual(Arg0, 85),LLess(Arg0, 90))) {
      Store(208,R6BR)
    }
    If(LAnd(LGreaterEqual(Arg0, 90),LLess(Arg0, 95))) {
      Store(224,R6BR)
    }
    If(LAnd(LGreaterEqual(Arg0, 95),LLess(Arg0, 100))) {
      Store(240,R6BR)
    }
    If(LEqual(Arg0,100)) {
      Store(255,R6BR)
    }
  }

  Name (TFST,Package ()
  {
    0,            // Revision:Integer
    0xFFFFFFFF,   // Control:Integer DWORD
    0xFFFFFFFF    // Speed:Integer DWORD
  })

  //_FST (Fan Status)
  //
  // The optional _FST object provides status information for the fan device.
  //
  // Arguments: (0)
  //  None
  // Return Value:
  //  A Package containing fan device status information
  //  The fan device status information will show on DTT UI
  //
  Method(_FST,0,Serialized,,PkgObj)
  {
    Store(FSLV, Index(TFST, 1))                //Control ID: Show the fan speed level
    Store(Multiply(R6BR,60), Index(TFST, 2))  //PWM Value : Show the fan speed value
    Return(TFST)
  } //End TFN2 device
}
