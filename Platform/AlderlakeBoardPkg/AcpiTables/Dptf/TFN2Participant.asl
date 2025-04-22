/** @file
  Intel ACPI Reference Code for Intel(R) Dynamic Tuning Technology

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.PC00.LPCB.H_EC)
{

  Device(TFN2)
  {
    Name(_HID, "INTC1048")  // Intel DPTF Fan Device
    Name(_UID, "TFN2")
    Name(_STR, Unicode ("DDR Fan"))
    Name(PTYP, 0x04)

    Name(FON, 1) // Indicates if Fan is ON Currently
    // PFLG
    //   0 - Default, participant is on the main board
    //   1 - Participant device is on the docking station
    //   2 - Participant device is on the detachable base
    //   3 - Participant device is an external device (such as a USB device, Intel(R) Dynamic Tuning Technology does not support this type of device today)
    Name(PFLG, 0)

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
    Method(_STA)
    {
      If (LEqual(FND2,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
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
    Method(_FIF)
    {
        Return (Package (){
        0,                            // Revision:Integer
        1,                            // FineGrainControl:Integer Boolean
        2,                            // StepSize:Integer DWORD
        0                             // LowSpeedNotificationSupport:Integer Boolean
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
    Method(_FPS,,,,PkgObj)
    {
        Return (Package()
        {
          0,    // Revision:Integer
          //        Control,  TripPoint,    Speed,  NoiseLevel, Power
          Package(){100,      0xFFFFFFFF,   12000,  500,        5000},
          Package(){ 95,      0xFFFFFFFF,   11600,  475,        4750},
          Package(){ 90,      0xFFFFFFFF,   11200,  450,        4500},
          Package(){ 85,      0xFFFFFFFF,   10500,  425,        4250},
          Package(){ 80,      0xFFFFFFFF,    9800,  400,        4000},
          Package(){ 70,      0xFFFFFFFF,    9300,  350,        3500},
          Package(){ 60,      0xFFFFFFFF,    7400,  300,        3000},
          Package(){ 50,      0xFFFFFFFF,    6300,  250,        2500},
          Package(){ 40,      0xFFFFFFFF,    5100,  200,        2000},
          Package(){ 30,      0xFFFFFFFF,    3800,  150,        1500},
          Package(){ 25,      0xFFFFFFFF,    3200,  125,        1250},
          Package(){  0,      0xFFFFFFFF,       0,    0,           0} // OFF
        })
    }

    Name (FSLV, 0)

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
      If(\_SB.PC00.LPCB.H_EC.ECAV)
      {
        If(LNot(LEqual(Arg0,\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.PENV)))))
        { // EC implementation-specific PWM port control
          \_SB.PC00.LPCB.H_EC.ECWT(1, RefOf(\_SB.PC00.LPCB.H_EC.PPSL))
          \_SB.PC00.LPCB.H_EC.ECWT(0, RefOf(\_SB.PC00.LPCB.H_EC.PPSH))
          \_SB.PC00.LPCB.H_EC.ECWT(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.PENV)), RefOf(\_SB.PC00.LPCB.H_EC.PINV))
          \_SB.PC00.LPCB.H_EC.ECWT(Arg0, RefOf(\_SB.PC00.LPCB.H_EC.PENV))
           Store(Arg0, FSLV)
          \_SB.PC00.LPCB.H_EC.ECWT(100, RefOf(\_SB.PC00.LPCB.H_EC.PSTP))
          \_SB.PC00.LPCB.H_EC.ECMD(0x1a)      // Command to turn the fan on
        }
      }
    }

    Name(TFST, Package()
    {
        0,          // Revision:Integer
        0xFFFFFFFF, // Control:Integer DWORD
        0xFFFFFFFF  // Speed:Integer DWORD
    })

    //_FST (Fan Status)
    //
    // The optional _FST object provides status information for the fan device.
    //
    // Arguments: (0)
    //  None
    // Return Value:
    //  A Package containing fan device status information
    //
    Method(_FST,0,Serialized,,PkgObj)
    {
      If(\_SB.PC00.LPCB.H_EC.ECAV)
      {
        Store(FSLV, Index(TFST, 1))
        Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.DFSP)), Index(TFST, 2))
      }
      Return(TFST)
    }
  } // End TFN1 Device

}// end Scope(\_SB.PC00.LPCB.H_EC)
