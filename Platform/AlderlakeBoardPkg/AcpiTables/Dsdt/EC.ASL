/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(HGAS, MethodObj)
External(HPFS, MethodObj)
External(HBRT, MethodObj)
External(CHPS, MethodObj)
External(UP1D)
External(SVRF, MethodObj)
External(\_SB.PC00.CTCU, MethodObj)
External(\_SB.PC00.CTCN, MethodObj)
External(\_SB.PC00.CTCD, MethodObj)
External(\_SB.PC00.GFX0.GLID, MethodObj)
External(\_SB.PC00.GFX0.GDCK, MethodObj)
External(\_SB.PC00.GFX0.IUEH, MethodObj)
External(\_SB.PC00.GFX0.GHDS, MethodObj)
External(\_SB.TPWR, DeviceObj)
External(\_SB.BAT1, DeviceObj)
External(\_SB.UBTC, DeviceObj)
External(\_SB.UBTC.MGI0, IntObj)
External(\_SB.UBTC.MGI1, IntObj)
External(\_SB.UBTC.MGI2, IntObj)
External(\_SB.UBTC.MGI3, IntObj)
External(\_SB.UBTC.MGI4, IntObj)
External(\_SB.UBTC.MGI5, IntObj)
External(\_SB.UBTC.MGI6, IntObj)
External(\_SB.UBTC.MGI7, IntObj)
External(\_SB.UBTC.MGI8, IntObj)
External(\_SB.UBTC.MGI9, IntObj)
External(\_SB.UBTC.MGIA, IntObj)
External(\_SB.UBTC.MGIB, IntObj)
External(\_SB.UBTC.MGIC, IntObj)
External(\_SB.UBTC.MGID, IntObj)
External(\_SB.UBTC.MGIE, IntObj)
External(\_SB.UBTC.MGIF, IntObj)
External(\_SB.UBTC.CCI0, IntObj)
External(\_SB.UBTC.CCI1, IntObj)
External(\_SB.UBTC.CCI2, IntObj)
External(\_SB.UBTC.CCI3, IntObj)
External(\_SB.UBTC.STS0, IntObj)
External(\_SB.UBTC.STS1, IntObj)
External(\_SB.UBTC.STS2, IntObj)
External(\_SB.UBTC.STS3, IntObj)
External(\_SB.UBTC.STS4, IntObj)
External(\_SB.UBTC.STS5, IntObj)
External(\_SB.UBTC.STS6, IntObj)
External(\_SB.UBTC.STS7, IntObj)
External(\_SB.PC00.GFX0.IUER, IntObj)
External(\LIDS)
External(\IGDS)
External(\BRTL)
External(\_SB.HIDD.HPEM, MethodObj)
External(\_SB.HIDD.BTLD, MethodObj)
External(\_SB.HIDD.HRDY, MethodObj)

#define POWER_BUTTON         0
#define WINDOWS_BUTTON       1
#define VOLUME_UP_BUTTON     2
#define VOLUME_DOWN_BUTTON   3
#define ROTATION_LOCK_BUTTON 4
#define CONVERTIBLE_BUTTON   6
#define DOCK_INDICATOR       7

Device(H_EC)  // Hitachi Embedded Controller
{
  Name(_HID, EISAID("PNP0C09"))

  Name(_UID,1)

  Name(ECAV, Zero)   // OS Bug Checks if EC OpRegion accessed before Embedded Controller Driver loaded
  Name(ECTK, One)    // ECDT (Embedded Controller Boot Resources Table) Check to correct ECAV flag in the beginning
  Name(BNUM, 0)  // Number Of Batteries Present

  // EC Mutex
  Mutex(ECMT, 0)

  Method(_CRS,0, Serialized)
  {
    Name(BFFR, ResourceTemplate()
    {
      IO(Decode16,0x62,0x62,0,1)  // DIN/DOUT
      IO(Decode16,0x66,0x66,0,1)  // CMD/STS
    })

    Return(BFFR)
  }

  Method(_STA)
  {
    If (LEqual(ECON,1)){
      Return(0x0F)
    }
    Return(0x00)
  }

  OperationRegion(ECF3,EmbeddedControl,0,0xFF)
  Field(ECF3,ByteAcc,Lock,Preserve)
  {
    Offset(4),
    IWCW, 160,    // EC buffer, send data to EC
    Offset(24),
    IWCR, 160,
  }

  OperationRegion(ECF2,EmbeddedControl,0,0xFF)
  Field(ECF2,ByteAcc,Lock,Preserve)
  {
    Offset(1),
    PLMX, 8,      // 1       Max Platform temprature
    LTMP, 8,      // 2       Sensor Temperature
    RPWR, 1,      // 3.0     Real AC Power (AC Present = 1)
        , 2,      // 3.2:1   Reserved
    CFAN, 1,      // 3.3     CPU Fan (Fan On = 1)
        , 2,      // 3.5:4   Reserved
    LSTE, 1,      // 3.6     Lid State (Lid Open = 1)
        , 1,      // 3.7     Reserved
    MGI0, 8,  //Offset 4, PPM->OPM Message In, 128 bits, 16 bytes
    MGI1, 8,
    MGI2, 8,
    MGI3, 8,
    MGI4, 8,
    MGI5, 8,
    MGI6, 8,
    MGI7, 8,
    MGI8, 8,
    MGI9, 8,
    MGIA, 8,
    MGIB, 8,
    MGIC, 8,
    MGID, 8,
    MGIE, 8,
    MGIF, 8,
    MGO0, 8,  //  Offset 20, OPM->PPM Message Out, 128 bits, 16 bytes
    MGO1, 8,
    MGO2, 8,
    MGO3, 8,
    MGO4, 8,
    MGO5, 8,
    MGO6, 8,
    MGO7, 8,
    MGO8, 8,
    MGO9, 8,
    MGOA, 8,
    MGOB, 8,
    MGOC, 8,
    MGOD, 8,
    MGOE, 8,
    MGOF, 8,
    CCI0, 8,  //  Offset 36, PPM->OPM CCI indicator, 4 bytes,
    CCI1, 8,
    CCI2, 8,
    CCI3, 8,
    Offset(47),
    CTMP, 8,      // 47      EC Critical Temperature
        , 1,      // 48.0    Reserved
        , 2,      // 48.2:1  Reserved
    PBNS, 1,      // 48.3    Power Button State (0 = Pressed)
    VPWR, 1,      // 48.4    Virtual AC Power (AC Present = 1)
        , 3,      // 48.7:5  Reserved
    SCAN, 8,      // 49      Hot-Key Scan Code
    B1ST, 8,      // 50      Battery 1 Status
    Offset(54),
    B2ST, 8,      // 54      Battery 2 Status
    Offset(58),
    CMDR, 8,      // 58      KSC command register
    Offset(65),
    PPSL, 8,      // 65      PWM Port Select low byte
    PPSH, 8,      // 66      PWM Port Select high byte
    PINV, 8,      // 67      PWM initial value
    PENV, 8,      // 68      PWM ending value
    PSTP, 8,      // 69      PWM ms delay between steps
    Offset(71),
    CPUP, 16,     // 71,72   CPU Power mW
    BPWR, 16,     // 73,74   Brick Power cW(100ths)
    PPWR, 16,     // 75,76   Platform Power cW(100ths)
    //
    // PwrSrcType Bit[0] = 0 <DC>, Bit[0] = 1 <AC>, Bit[1] = 1 <USB-PD>, Bit[2] = 1 <Wireless Charging>
    // Bit[7:4]  = Power Delivery State Change Sequence Number
    Offset(78),
    PWRT, 8,     // 78
    PBOK, 8,     // 79       PBOK offset where BIOS will update the Power Delivery State Change Sequence Number
    Offset(80),
    TSI,  4,      // 80      TemSnsrSelct Select Thermal Sensor
                  //          # 0 = SEN1
                  //          # 1 = SEN2
                  //          # 2 = SEN3
                  //          # 3 = SEN4
                  //          # 4 = SEN5
                  //          # 5 = SEN6
                  //          # 6 = SEN7
                  //          # 7 = SEN8
                  //          # 8 = SEN9
                  //          # 9 = SEN10
                  //          # A = SEN11
                  //          # B = SEN12
                  //          # C = SEN13
                  //          # D = SEN14
                  //          # E = SEN15
                  //          # F = SEN16
    HYST, 4,      // Hysteresis selection is global and meant for all sensors
    TSHT, 16,     // 81-82   TempThrshldH Sensor High Trip Point
    TSLT, 16,     // 83-84   TempThrshldL Sensor Low Trip Point
    Offset(85),
    TSSR, 16,     // 85- 86  TSSR- thermal sensor status register:
                  //         Responsible for triggering _QF1:
                  //          BIT0:  SEN1
                  //          BIT1:  SEN2
                  //          BIT2:  SEN3
                  //          BIT3:  SEN4
                  //          BIT4:  SEN5
    Offset(87),
    B1DC, 16,     // 87      Battery 1 Design Capacity (mWh)
    B1RC, 16,     // 89      Battery 1 Remaining Capacity (mWh)
    B1FC, 16,     // 91      Battery 1 Full Charge Capacity (mWh)
    B1FV, 16,     // 93      Battery 1 Full Resolution Voltage (mV)
    B1DI, 16,     // 95      Battery 1 Full Resolution Discharge Current (mA)
    B1CI, 16,     // 97      Battery 1 Full Resolution Charge Current (mA)
    B2RC, 16,     // 99      Battery 2 Remaining Capacity (mWh)
    B2FC, 16,     // 101     Battery 2 Full Charge Capacity (mWh)
    B2FV, 16,     // 103     Battery 2 Full Resolution Voltage (mV)
    B2DI, 16,     // 105     Battery 2 Full Resolution Discharge Current (mA)
    B2CI, 16,     // 107     Battery 2 Full Resolution Charge Current (mA)
    VMIN, 16,     // 109,110 Min voltage below which platform activates OC protection and shuts down (mV).
    PROP, 16,     // 111,112 Worst case rest of platform power in (W).
    DFSP, 16,     // 113,114 DDR Fan speed in rpm
    CFSP, 16,     // 115,116 CPU Fan speed in rpm
    GFSP, 16,     // 116,117 GPU Fan speed in rpm
    Offset(120),  // (0x78)
    DLED, 1,      // 120.0   EC Connected Standby Debug LED (CAPS LOCK)
        , 2,      // 120.2:1   Reserved
    DOCO, 1,      // 120.3   Dock Orientation - 1 if normal, 0 for reverse
    Offset(122),
    AVOL, 16,     // 122,123 Returns the AC source nominal voltage in (mV).
    Offset(125),
    ACUR, 16,     // 125,126 Returns the AC source operational current in (mA).
    Offset(128),
    PMAX, 8,      // 128     CPU,MCH & PCH Max temp
    PPDT, 8,      // 129     PCH DTS reading from PCH
    PECL, 8,      // 130     CPU PECI reading fractional value (1/64 Celcius)
    PECH, 8,      // 131     CPU PECI reading integer value (unit Celicus)
    Offset(135),
    ARTG, 16,     // 135,136 AC adapter rating in 10 mW. Max adapter power supported. Because of EC-space constraint EC can only send 2bytes=15000mW but system uses 150000mW hence bios will multiply this value by 10 and return.
    CTYP, 8,      // 137     Charger type, Traditional or Hybrid
    AP01, 8,      // 138     Returns AC source 1ms period percentage overload in 1% unit.
    AP02, 8,      // 139     Returns AC source 2ms period percentage overload in 1% unit.
    AP10, 8,      // 140     Returns AC source 10ms period percentage overload in 1% unit.
    PBSS, 16,     // 141,142 Max sustained power for battery (mW)
    BICC, 16,     // 143,144 _BIX.Cycle Count
    Offset(145),
    TSR1, 16,     // 145,146 Sensor SEN1
    TSR2, 16,     // 147,148 Sensor SEN2
    TSR3, 16,     // 149,150 Sensor SEN3
    TSR4, 16,     // 151,152 Sensor SEN4
    TSR5, 16,     // 153,154 Sensor SEN5
    Offset(155),
    PRC1, 8,      // 155     Peci Repeat Cycle (LSB)
    PRC2, 8,      // 156     Peci Repeat Cycle
    PRC3, 8,      // 157     Peci Repeat Cycle
    PRC4, 8,      // 158     Peci Repeat Cycle (MSB)
    PRPI, 8,      // 159     Peci Repeat Period Interval
    PECN, 8,      // 160     Peci Error Count Number
    PCAD, 8,      // 161     Peci Clinet Addess
    PEWL, 8,      // 162     Write Length
    PWRL, 8,      // 163     Read lenght
    PECD, 8,      // 164     Command Code
    PEHI, 8,      // 165     Host ID
    PECI, 8,      // 166     Index
    PEPL, 8,      // 167     Parameter (LSB)
    PEPM, 8,      // 168     Parameter (MSB)
    PWFC, 8,      // 169     Write FCS
    PECC, 8,      // 170     Completion code
    PDT0, 8,      // 171     Data 0
    PDT1, 8,      // 172     Data 1
    PDT2, 8,      // 173     Data 2
    PDT3, 8,      // 174     Data 3
    PRFC, 8,      // 175     Read FCS
    PRS0, 8,      // 176     Reserved
    PRS1, 8,      // 177     Reserved
    PRS2, 8,      // 178     Reserved
    PRS3, 8,      // 179     Reserved
    PRS4, 8,      // 180     Reserved
    Offset(187),
    PRCS, 8,      // 187     Peci Repeat Command Status
    PEC0, 8,      // 188     Peci Error Count (LSB)
    PEC1, 8,      // 189     Peci Error Count
    PEC2, 8,      // 190     Peci Error Count
    PEC3, 8,      // 191     Peci Error Count (MSB)
    Offset(195),
    WTMM, 8,      // 195     Wake timer mode (C3)
    WTMV, 32,     // 196     Wake time value (C4)

    SPT2, 1,      // 200.0   SATA Por2 - Cable connect power control. 1=ON;0=OFF
    ,7,           // 200.7-1  Reserved
    Offset(201),
    BTEN, 8,      // 201     Button Enable/Disable
                  //          201.0: Power Button N/A to disable
                  //          201.1: Windows Button
                  //          201.2: Volume Up Button
                  //          201.3: Volume Down Button
                  //          201.4: Rotation Lock Button
                  //          201.5-8: Reserved
    Offset(209),
    B2DC, 16,     // 209     Battery 2 Design Capacity (mWh)
    B1DV, 16,     // 211     Battery 1 Design Voltage (mV)
    B2DV, 16,     // 213     Battery 2 Design Voltage (mV)
    Offset(215),
    BMAX, 16,     // 215     Battery A maximum
    B2ML, 8,      // 217     BattBPmaxL - Battery Pack B maximum low byte
    B2MH, 8,      // 218     BattBPmaxH - Battery Pack B maximum high byte
    BTP1, 8,      // 219     Battery 1 Trip Point in %
    B1TL, 8,      // 220     Battery 1 Trip Point lower byte in mAh
    B1TH, 8,      // 221     Battery 1 Trip Point higher byte in mAh
    KBDB, 8,      // 222     Keyboard brightness Percentage
    CHGR, 16,     // 223     Charge Rate
    Offset(231),
    SCCK, 8,      // 231     Ctrl+Alt+Shit(CAS)+Key Scan Code
    PVOL, 8,      // 232     Pmic Vendor ID and Voltage Margining byte
    FCHG, 8,      // 233     Fast Charging Enabled.
    CTL0, 8,      // 234     OPM->PPM Control message, 8 bytes,
    CTL1, 8,
    CTL2, 8,
    CTL3, 8,
    CTL4, 8,
    CTL5, 8,
    CTL6, 8,
    CTL7, 8,
    Offset(244),  // 244 (0xF4)
    VBUS, 1,      // BIT0 -  VBUS Sense
    OTGI, 1,      // BIT1 -  OTG ID
    Offset(245),
    RBHF, 16,     // 245-246 Battery high frequency impedance
    VBNL, 16,     // 247-248 Battery No-Load Voltage
    CMPP, 16,     // 249-250 Battery Maximum peak current
    Offset(251),
    UVTH, 16,     // 251-252 Under Voltage Threshold
  }


  // ECRD (Embedded Read Method)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (1)
  //    Arg0 - Object to Read
  //  Return Value:
  //    Read Value
  //
  Method(ECRD,1,Serialized, 0, IntObj, FieldUnitObj)
  {
    //
    // Check for ECDT support, set ECAV to One if ECDT is supported by OS
    // Only check once at beginning since ECAV might be clear later in certain conditions
    //
    If (ECTK) {
      If (LGreaterEqual (_REV, 2)) {
        Store (One, ECAV)
      }
      Store (Zero, ECTK)   // Clear flag for checking once only
    }

    Store (Acquire(ECMT, 1000), Local0)  // save Acquire result so we can check for Mutex acquired
    If (LEqual(Local0, Zero))  // check for Mutex acquired
    {
      If (ECAV) {
        Store(DerefOf (Arg0), Local1) // Execute Read from EC
        Release(ECMT)
        Return(Local1)
      }
      Else
      {
        Release(ECMT)
      } // If (ECAV)
    } // If EC Acquired
    Return(0)
  }

  // ECWT (Embedded Write Method)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (2)
  //    Arg0 - Value to Write
  //    Arg1 - Object to Write to
  //
  Method(ECWT,2,Serialized,,,{IntObj, FieldUnitObj})
  {
    //
    // Check for ECDT support, set ECAV to One if ECDT is supported by OS
    // Only check once at beginning since ECAV might be clear later in certain conditions
    //
    If (ECTK) {
      If (LGreaterEqual (_REV, 2)) {
        Store (One, ECAV)
      }
      Store (Zero, ECTK)   // Clear flag for checking once only
    }

    Store (Acquire(ECMT, 1000), Local0)  // save Acquire result so we can check for Mutex acquired
    If (LEqual(Local0, Zero))  // check for Mutex acquired
    {
      If (ECAV) {
        Store(Arg0,Arg1) // Execute Write to EC
      } // If (ECAV)
      Release(ECMT)
    } // If EC Acquired
  }

  // ECMD (Embedded Controller Command)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (1)
  //    Arg0 - EC command
  //  Return Value:
  //    0x00 = Success
  //    0xFF = Failure
  //
  Method(ECMD,1,Serialized)
  {
    //
    // Check for ECDT support, set ECAV to One if ECDT is supported by OS
    // Only check once at beginning since ECAV might be clear later in certain conditions
    //
    If (ECTK) {
      If (LGreaterEqual (_REV, 2)) {
        Store (One, ECAV)
      }
      Store (Zero, ECTK)   // Clear flag for checking once only
    }

    If (ECAV) {
      Store (ECRD (RefOf (CMDR)), Local0)
      Store (0, Local1)
      //
      // Time out value is 10 second
      //
      While (LAnd (Local0, LLess (Local1, 5000))) {
        Stall (200)
        Store (ECRD (RefOf (CMDR)), Local0)
        Increment (Local1)
      }

      If (LGreaterEqual (Local1, 5000)) {
        Return (0xFF)
      }

      ECWT (Arg0, RefOf (CMDR))
      Return (0x00)
    }
    Return (0xFF)
  }


  // ECNT (Embedded Controller Notify)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (1)
  //    Arg0 - 3 = Idle Resiliency Entry Notify
  //           2 = Idle Resiliency Exit Notify
  //           1 = CS Entry Notify
  //           0 = CS Exit Notify
  //  Return Value:
  //    0x00 = Success
  //    0xFF = Failure
  //
  Method (ECNT,1,Serialized) {
    Switch (ToInteger(Arg0)) {
      Case (0) { // exit CS
        D8XH (0, 0xC5)
        D8XH (1, 0xAA)
        if (LEqual(ECNO,0x01)) { // Check EC notification enabled in Setup
          Store (Acquire(\EHLD, 0xFFFF), Local0) // Wait for Mutex for telling EC to exit Low Power Mode
          if (LEqual(Local0, Zero)) {
            \_SB.PC00.LPCB.H_EC.ECMD (0x2D) // Notify EC of CS exit
            If (LEqual(ECLP, 0x1)) {
              Store (One,\ECUP)
            }
            Release (\EHLD)
          }
        }
        Return (0x00)
      }

      Case(1) { // enter CS
        D8XH (0, 0xC5)
        D8XH (1, 0x01)

        if (LEqual(ECNO,0x01)) { // Check EC notification enabled in Setup
          \_SB.PC00.LPCB.H_EC.ECMD (0x2C) // Notify EC of CS entry
          If (LEqual(ECLP, 0x1)) {
            Store (Zero,\ECUP)
          }
        }
        Return (0x00)
      }

      Default {
        Return (0xFF)  // Error invalid argument
      }
    }
  }

  // EREG method will be used in _REG (evaluated by OS without ECDT support) or _INI (for OS with ECDT support)
  Method(EREG)
  {
    // Update ECAV Object. ASL should check for this value to be One before accessing EC OpRegion.
    Store(One, ECAV)

    // Turn off the CPU Fan if Active Cooling is disabled.
    If (LEqual(0,ACTT))
    {
      ECWT(0,RefOf(CFAN))
    }

    // Turn off pwm fan so it starts in a known state for legacy thermal zone only.
    If (CondRefOf(\_TZ.ETMD))
    {
      If(LEqual(\_TZ.ETMD,1))
      {
        \_TZ.FN00._OFF()
      }
    }

    // Save the Lid State in global NVS and IGD OpRegion.
    //Store(LSTE,\_SB.PC00.GFX0.CLID)
    If (LEqual(ECRD(RefOf(LSTE)), 0))
    {
      Store(0,\_SB.PC00.GFX0.CLID)
    }
    If (LEqual(ECRD(RefOf(LSTE)), 1))
    {
      Store(3,\_SB.PC00.GFX0.CLID)
    }
    Store(ECRD(RefOf(LSTE)),LIDS)

    // Update the Dock Status
    Store(\DSTS,\_SB.PC00.GFX0.CDCK)

    // Unconditionally fix up the Battery and Power State.

    // Initialize the Number of Present Batteries.
    //  1 = Real Battery 1 is present
    //  2 = Real Battery 2 is present
    //  3 = Real Battery 1 and 2 are present
    Store(0,BNUM)
    Or(BNUM,ShiftRight(And(ECRD(RefOf(B1ST)),0x08),3),BNUM)
    Or(BNUM,ShiftRight(And(ECRD(RefOf(B2ST)),0x08),2),BNUM)


    // Initialize the Power State.
    //  BNUM = 0 = Virtual Power State
    //  BNUM > 0 = Real Power State
    If (LEqual(BNUM,0))
    {
      Store(ECRD(RefOf(VPWR)),PWRS)
    }
    Else
    {
      Store(ECRD(RefOf(RPWR)),PWRS)
    }

    // Perform needed ACPI Notifications.

    SPNT()
  }

  // The _REG Method is needed because communication with the EC
  // before the driver is loaded is prohibited in WIN2000/WINXP.
  // According to ACPI spec, the arguments:
  //   Arg0 - Opertion Region Address space ID
  //   Arg1 - handler connection code
  Method(_REG,2)
  {
    // When OS runs _REG control method with Arg0 = 3 (Embedded Controller Operation Region) and Arg1 = 1 (connect the handler)
    If (LAnd(LEqual(Arg0,3),LEqual(Arg1,1)))
    {
      // Must be running NT 5.0 OS or newer.
      EREG()
    }
  }

  // The _REG Method will not evaluated if ECDT is loaded by OS with ECDT support
  // Uising _INI Method to cover EC initialization done in _REG
  Method(_INI)
  {
    If (LGreaterEqual (_REV, 2)) {
      // Only for OS with ECDT support, such as Vista or newer Windows OS
      EREG()
    }

    If (LEqual(\S0ID,1)) {
      //
      // Enable SCIs in EC to trigger QD5 and QD6 event
      //
      \_SB.PC00.LPCB.H_EC.ECWT(0x01, RefOf(\_SB.PC00.LPCB.H_EC.BTEN)) //Enable EC Power Button in ACPI name space
      \_SB.PC00.LPCB.H_EC.ECMD(0x38)                                  //Enable/Disable SCIs from buttons
    }
  }

  // Update Timer value in EC mailbox
  // Arg0: Timer mode
  // Arg1: TimerValue
  Method(UTEC, 2)
  {
     \_SB.PC00.LPCB.H_EC.ECWT(Arg0, RefOf(\_SB.PC00.LPCB.H_EC.WTMM))
     If(LNotEqual(Arg1,0xFFFFFFFF)) {
        \_SB.PC00.LPCB.H_EC.ECWT(Arg1, RefOf(\_SB.PC00.LPCB.H_EC.WTMV))
        \_SB.PC00.LPCB.H_EC.ECMD(0x2F)
     }
  }

  //
  // The _GPE method needs to be the same as the GPE number assignment in ECDT
  //
  Method(_GPE)
  {
    If (\_SB.PC00.LPCB.ESPI)
    {
      Store (0x6E,  Local0)   // GPI6E for eSPI
    } Else {
        Store (23,  Local0)     // GPI23 for other boards
        If (LNotEqual(\SRSP, 0))
        {
           Store (GGPE(\SRSP), Local0)  // SMC Runtime SCI
        }
    }
    return (Local0)
  }

  // For the below _Qxx Methods, The Runtime SCI has been asserted,
  // the EC Driver performed it's query, and the value returned from
  // the Query = xx.

  Method(_Q30)    // Real Battery AC Insertion Event.
  {
    // Set Global Power State = AC Mode.

    Store(1,PWRS)

    // Perform needed ACPI Notifications.
    If(LAnd(LEqual(\DPTF,1), LEqual(\PWRE,1))) {
       Notify(\_SB.TPWR, 0x81) // notify Power participant
    }

    SPNT()
  }

  Method(_Q31)    // Real Battery AC Removal Event.
  {
    // Set Global Power State = Battery Mode.

    Store(0,PWRS)

    // Perform needed ACPI Notifications.
    If(LAnd(LEqual(\DPTF,1), LEqual(\PWRE,1))) {
       Notify(\_SB.TPWR, 0x81) // notify Power participant
    }

    SPNT()
  }

  Method(_Q32)    // Real Battery Capacity Change.
  {
    // Perform needed ACPI Notifications.
    If(LAnd(LEqual(\DPTF,1), LEqual(\BATR,1))) {
       Notify(\_SB.BAT1, 0x86) // notify Battery participant
    }

    SPNT()
  }

  Method(_Q33)    // Real Battery Insertion/Removal Event.
  {
    // Initialize the Number of Present Batteries.
    //  1 = Real Battery 1 is present
    //  2 = Real Battery 2 is present
    //  3 = Real Battery 1 and 2 are present

    Store(0,BNUM)
    Or(BNUM,ShiftRight(And(ECRD(RefOf(B1ST)),0x08),3),BNUM)
    Or(BNUM,ShiftRight(And(ECRD(RefOf(B2ST)),0x08),2),BNUM)

    // Perform needed ACPI Notifications.
    If(LAnd(LEqual(\DPTF,1), LEqual(\PWRE,1))) {
       Notify(\_SB.TPWR, 0x81) // notify Power participant
    }

    SPNT()
  }

  Method(_Q34)    // PMAX changed by 250mw
  {
    If(LAnd(LEqual(\DPTF,1), LEqual(\BATR,1))) {
      Notify(\_SB.BAT1, 0x80) // notify battery participant
    }
  }


  Method(_Q35)    // PBSS changed by 100mw
  {
    If(LAnd(LEqual(\DPTF,1), LEqual(\BATR,1))) {
      Notify(\_SB.BAT1, 0x83) // notify battery participant
    }
  }

  Method(_Q37)    //PWR_SRC_CHANGE_SCI
  {
    //
    // This SCI is supported by EC for KBL onwards not SKL SDS.
    //

    //PWRT 78d PwrSrcType Bit0=1 if AC, Bit1=1 if USB-PD, Bit2=1 if Wireless Charging
    P8XH(0,0x37)

    And(\_SB.PC00.LPCB.H_EC.PWRT, 0x03, Local0)

    If (LOr(LEqual(Local0, 1), LEqual(Local0, 2))) {
      Store(1, PWRS)     // AC or USB-PD Insertion
    }

    If(LEqual(Local0, 0x0)) {
      Store(0, PWRS)     // AC or USB-PD Removal
    }

    // Perform needed ACPI Notifications.
    If(LAnd(LEqual(\DPTF,1), LEqual(\PWRE,1))) {
       Notify(\_SB.TPWR, 0x81) // notify Power participant
    }
    SPNT()
  }

  Method(_Q38)    //RBHF value is changed over a threshold, 5mOhm.
  {
    If(LAnd(LEqual(\DPTF,1), LEqual(\BATR,1))) {
      Notify(\_SB.BAT1, 0x85) // notify battery participant
    }
  }

  Method(_Q39)    //VBNL value is changed over a threshold, for example, 50mV.
  {
    If(LAnd(LEqual(\DPTF,1), LEqual(\BATR,1))) {
      Notify(\_SB.BAT1, 0x86) // notify battery participant
    }
  }

  Method(_Q3A)    //CMPP value value is changed over a threshold, for example,  100mA.
  {
    If(LAnd(LEqual(\DPTF,1), LEqual(\BATR,1))) {
      Notify(\_SB.BAT1, 0x86) // notify battery participant
    }
  }

  Method(_Q40, 0, Serialized)   // Dock complete Event
  {
//@todo: Waiting for DOCK offect data
//    If (LEqual(ECRD(RefOf(DOCK)), 0)) {
    P8XH(0,0x40)
    Return()
//    }

//@todo: Temporary reomove for fix iasl compiler warning.
//    Sleep(1000) // Delay 1 second for hot docking stability
//    Store(DKSM, \SSMP)
//    Store(1, \DSTS)
//    Sleep(1000) // Delay 1 second for hot docking stability
//    Notify(\_SB.PC00.DOCK, 0x00)
//
//    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
//      Or(PB1E, 0x10, PB1E)   // Set Dock status
//      UPBT(DOCK_INDICATOR, One)
//      Notify(\_SB.HIDD, 0xCA)    // Notify HID Event Filter driver that the system is now in Dock mode.
//      If (LEqual(\CEDS,1)) {
//        If (LEqual(ECRD(RefOf(DOCO)), 1)) {
//          Or(PB1E, 0x08, PB1E) // Set Laptop status
//          UPBT(CONVERTIBLE_BUTTON, One)
//          Notify(\_SB.HIDD, 0xCD)  // Notify HID Event Filter driver that the system is now in Laptop mode.
//        } Else {
//          And(PB1E, Not(0x08), PB1E) // Clear Laptop Status
//          UPBT(CONVERTIBLE_BUTTON, Zero)
//          Notify(\_SB.HIDD, 0xCC)        // Notify HID Event driver that the system is now in Slate mode.
//        }
//      }
//    } Else {
//      If (IGDS)
//      {
//        \_SB.PC00.GFX0.GDCK(1)
//      }
//      If (LEqual(\CEDS,1)) {
//        If (CondRefOf(\_SB.PC00.GFX0.IUEH)) {
//          \_SB.PC00.GFX0.IUEH(6)
//          \_SB.PC00.GFX0.IUEH(7)
//        }
//      }
//    }

  }

  Method(_Q41)    // Surprise Removal
  {
//@todo: Waiting for DOCK offect data
//    If (LEqual(ECRD(RefOf(DOCK)), 1)) {
    P8XH(0,0x41)
    Return()
//    }

//@todo: Temporary reomove for fix iasl compiler warning.
//    Store(0, \DSTS)
//    Sleep(1000) // Delay 1 second for hot un-docking stability
//    Notify(\_SB.PC00.DOCK, 0x01)
//
//    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
//      And(PB1E, Not(0x10), PB1E)   // Clear Dock Status
//      UPBT(DOCK_INDICATOR, Zero)
//      Notify(\_SB.HIDD, 0xCB)          // Notify HID Event Filter driver that the system is now in Undock mode.
//      If (LEqual(\CEDS,1)) {
//        And(PB1E, Not(0x08), PB1E) // Clear Laptop Status
//        UPBT(CONVERTIBLE_BUTTON, Zero)
//        Notify(\_SB.HIDD, 0xCC)        // Notify HID Event Filter driver that the system is now in Slate mode.
//      }
//    } Else {
//      If (IGDS)
//      {
//        \_SB.PC00.GFX0.GDCK(0)
//      }
//      If (LEqual(\CEDS,1)) {
//        If (CondRefOf(\_SB.PC00.GFX0.IUEH)) {
//          \_SB.PC00.GFX0.IUEH(6)
//          \_SB.PC00.GFX0.IUEH(7)
//        }
//      }
//    }

  }

  Method(_Q42)    // Undocking request Event
  {
  }

  Method(_Q43)    // Express Card Presence Changed on Saddlestring (Docking)
  {
    Notify(\_SB.PC00.RP07, 0x0)
  }

  Method(_Q44)  // Virtual Dock Presence Changed
  {
  }

  //
  // SCI handler for USB device Attach/Detach notification
  //

  // XDAT: XDCI device attach/detach
  // Return 1 when device attach
  // Return 0 when device detach
  Method(XDAT)
  {
    If(LEqual(VBUS,1)){
      Return(1)
    }Else{
      Return(0)
    }
  }
  Method(_Q77)
  {
    If(LEqual(XDAT(), 1)){
      Notify(\_SB.PC00.XDCI,0x80)
    }
  }

  Method(UCEV, 0, Serialized)    // Any event on USBTypeC.
  {
      P8XH(0, 0x79)
      Store( MGI0, \_SB.UBTC.MGI0 )
      Store( MGI1, \_SB.UBTC.MGI1 )
      Store( MGI2, \_SB.UBTC.MGI2 )
      Store( MGI3, \_SB.UBTC.MGI3 )
      Store( MGI4, \_SB.UBTC.MGI4 )
      Store( MGI5, \_SB.UBTC.MGI5 )
      Store( MGI6, \_SB.UBTC.MGI6 )
      Store( MGI7, \_SB.UBTC.MGI7 )
      Store( MGI8, \_SB.UBTC.MGI8 )
      Store( MGI9, \_SB.UBTC.MGI9 )
      Store( MGIA, \_SB.UBTC.MGIA )
      Store( MGIB, \_SB.UBTC.MGIB )
      Store( MGIC, \_SB.UBTC.MGIC )
      Store( MGID, \_SB.UBTC.MGID )
      Store( MGIE, \_SB.UBTC.MGIE )
      Store( MGIF, \_SB.UBTC.MGIF )
      Store( CCI0, \_SB.UBTC.CCI0 )
      Store( CCI1, \_SB.UBTC.CCI1 )
      Store( CCI2, \_SB.UBTC.CCI2 )
      Store( CCI3, \_SB.UBTC.CCI3 )

      Notify(\_SB.UBTC,0x80)
  }
  Method(_Q76)    // Any event on USBTypeC.
  {
    UCEV()
  }
  Method(_Q79)    // Any event on USBTypeC.
  {
    UCEV()
  }
// Display Hotkey Control Function
//    Handle display hotkey control functions, such as display switch, display brightness increase/decrease
//
//  Arguments:
//    Arg0: Function Number
//            1 = Display Switch
//            2 = Display Brightness Increase
//            3 = Display Brightness Decrease
//            4+ = Reserved
//
//    Arg1: Sub Function Number
//            if Arg0 == 1, then
//              x = Display Switch Table Index x ( 0<= x <= 3)
//
//  Return Value:
//    None

  Method(DHCF, 2, Serialized)
  {
    Switch(ToInteger(Arg0))
    {
      Case(1) // GMCH SCI hotkey display switch, table index x
      {
        If (LAnd(IGDS, LLess(Arg1, 4)))
        {
          \_SB.PC00.GFX0.GHDS(Arg1)
        }
      }
      Case(2) // Brightness Increase.
      {
        If (CondRefOf(HBRT)) // Send backlight notifications to the DGPU LFP device.
        {
          HBRT(3)
        }
        If (And(4,DSEN))
        {
          //BRTN(0x86)
        } Else {
          // Current brightness is a percentage number. This must be done if ALS is enabled,
          // as the driver will change the brightness based on ambient light.
          Store(\_SB.PC00.GFX0.CBLV, Local0)

          // 1 % will be lost in the conversion, so need to make it up first.
          AND(Add(Local0, 1),0xFE, Local0)

          If (LLessEqual(Local0, 90))
          {
            Add(Local0, 10, Local0)
          }
          Store(Local0, BRTL)
          \_SB.PC00.GFX0.AINT(1, Local0)
        }
      }
      Case(3) // Brightness Decrease.
      {
        If (CondRefOf(HBRT)) // Send backlight notifications to the DGPU LFP device.
        {
          HBRT(4)
        }
        If (And(4,DSEN))
        {
          //BRTN(0x87)
        } Else {
          // Current brightness is a percentage number. This must be done if ALS is enabled,
          // as the driver will change the brightness based on ambient light.
          Store(\_SB.PC00.GFX0.CBLV, Local0)

          // 1 % will be lost in the conversion, so need to make it up first.
          AND(Add(Local0, 1),0xFE, Local0)

          If (LGreaterEqual(Local0, 10))
          {
            Subtract(Local0, 10, Local0)
          }
          Store(Local0, BRTL)
          \_SB.PC00.GFX0.AINT(1, Local0)
        }
      }
    }
  }

  Method(_Q52)    // Hot-Key Event.
  {
    //Return if PS2 Keyboard and Mouse disabled
    If (LEqual(P2MK,0))
    {
      Return
    }

    // This event will be generated whenever a
    // FN+"Next Key" is hit.  This event handler
    // will base code execution on the "Next Key" Scan Code
    // stored in the EC Memory Space.

    Store(ECRD(RefOf(SCAN)),Local0)


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
    If (LEqual(Local0,0x43))
    {
      DHCF(3, 0)
    }

    // F10 Scan Code = 0x44
    ElseIf (LEqual(Local0,0x44))
    {
      DHCF(2, 0)
    }

    // F11 Scan Code = 0x45
    ElseIf (LEqual(Local0,0x45))
    {
      \_SB.HIDD.HPEM(8)         // HID Event Index 8: Airplane Mode/Wireless Radio Button
    }
  } // end Method(_Q52) Fn hot key event

  Method(_Q54)    // Power Button Event for Control method Power Button(10sec PB Override without V-GPIO driver)
  {
    If (LEqual(S0ID,0)) {  // if Connected Standby is not supported

      //
      // Check UAMS(User Absent Mode State) to notify the power button event.
      //
      If (UAMS){ // UAMS has a non-Zero value, means the system is under User Absent Mode. Send Wake up event.
        If (CondRefOf(\_SB.PWRB)){
          Notify(\_SB.PWRB, 0x02)
        }
      } Else { // UAMS is Zero, means the system is ON. Send Sleep event.
        If (CondRefOf(\_SB.PWRB)){
          Notify(\_SB.PWRB, 0x80)
        }
      }
    }
  } // end Method(_Q54) Power button event.

  Name(ABCD,0) // used as counter by hotkeys

  Method(_Q56)    // Hot-Key Event.
  {
    //Return if PS2 Keyboard and Mouse disabled
    If (LEqual(P2MK,0))
    {
      Return
    }

    Store(ECRD(RefOf(SCCK)),Local0)

    // This event will be generated whenever a
    // CRTL+ALT+SHIFT+"Next Key" is hit.  This event handler
    // will base code execution on the "Next Key" Scan Code
    // stored in the EC Memory Space.

    // "BKSP" Scan Code = 0x0E
    If (LEqual(Local0,0x0E))
    {
      \_SB.HIDD.HPEM(14)        // HID Event Index 14: Stop.
    }

    // "Tab" Scan Code = 0x0F
    If (LEqual(Local0,0x0F))
    {
      \_SB.HIDD.HPEM(15)        // HID Event Index 15: Play/Pause.
    }

    // "Q" Scan Code = 0x10
    If (LEqual(Local0,0x10))
    {
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(1)         // HID Event Index 1: Windows Button (Keyboard Left GUI).
    }

    // "W" Scan Code = 0x11
    ElseIf (LEqual(Local0,0x11))
    {
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(2)         // HID Event Index 2: Rotation Lock
    }

    // "E" Scan Code = 0x12
    ElseIf (LEqual(Local0,0x12))
    {
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(3)         // HID Event Index 3: Num Lock
    }

    // "R" Scan Code = 0x13
    ElseIf (LEqual(Local0,0x13))
    {
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(4)         // HID Event Index 4: Home
    }

    // "T" Scan Code = 0x14
    ElseIf (LEqual(Local0,0x14))
    {
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(5)         // HID Event Index 5: End
    }

    // "Y" Scan Code = 0x15
    ElseIf (LEqual(Local0,0x15))
    {
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(6)         // HID Event Index 6: Page Up
    }

    // "U" Scan Code = 0x16
    ElseIf (LEqual(Local0,0x16))
    {
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(7)         // HID Event Index 7: Page Down
    }

    // "I" Scan Code = 0x17
    ElseIf (LEqual(Local0,0x17))
    {
      \_SB.HIDD.HPEM(8)         // HID Event Index 8: Wireless Radio Button
    }

    // "O" Scan Code = 0x18
    ElseIf (LEqual(Local0,0x18))
    {
      \_SB.HIDD.HPEM(9)         // HID Event Index 9: System Power Down
    }

    // HK: "P" Scan Code = 0x19
    ElseIf (LEqual(Local0,0x19))   // CTDP Up
    {
      If (LAnd(CondRefOf(\_SB.PC00.CTCU),LEqual(CTDB,1))){ // only allow this feature if it is enabled in SETUP
        \_SB.PC00.CTCU()
      }
    }

    // HK: "[" Scan Code = 0x1A
    ElseIf (LEqual(Local0,0x1A))   // CTDP Nominal
    {
      If (LAnd(CondRefOf(\_SB.PC00.CTCN),LEqual(CTDB,1))){ // only allow this feature if it is enabled in SETUP
        \_SB.PC00.CTCN()
      }
    }

    // HK: "]" Scan Code = 0x1B
    ElseIf (LEqual(Local0,0x1B))   // CTDP Down
    {
      If (LAnd(CondRefOf(\_SB.PC00.CTCD),LEqual(CTDB,1))){ // only allow this feature if it is enabled in SETUP
        \_SB.PC00.CTCD()
      }
    }

    // Calculator (Enter) Key = 0x1C
    ElseIf (LEqual(Local0,0x1C))
    {
      \_SB.HIDD.HPEM(27)         // HID Event Index 27: System Wake
    }

    // "A" Scan Code = 0x1E
    If (LEqual(Local0,0x1E))
    {
      \_SB.HIDD.HPEM(16)        // HID Event Index 16: Mute.
    }

    // "G" Scan Code = 0x22
    ElseIf (LEqual(Local0,0x22))
    {
      // Virtual Button Event - Ctrl + Alt + Del Screen

      Sleep(1000) // 1sec delay is needed for Powerbutton and Windows Home button

      If (CondRefOf(\_SB.PC00.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        \_SB.PC00.GFX0.IUEH(0)
      }
    }

    // "H" Scan Code = 0x23
    ElseIf (LEqual(Local0,0x23))
    {
      // Virtual Button Event - Windows Button
      Sleep(1000)

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Windows Home
        Notify(\_SB.HIDD,0xC2) // Notify HID driver that Windows Home button is pressed.
        Notify(\_SB.HIDD,0xC3) // Notify HID driver that Windows Home button is released.
      } ElseIf (CondRefOf(\_SB.PC00.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        \_SB.PC00.GFX0.IUEH(1)
      }
    }

    // "J" Scan Code = 0x24
    ElseIf (LEqual(Local0,0x24))
    {
      // Virtual Button Event - Volume Up

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume UP Press
        Notify(\_SB.HIDD,0xC4) // Notify HID driver that Volume UP button is pressed.
        Notify(\_SB.HIDD,0xC5) // Notify HID driver that Volume UP button is released.
      } ElseIf (CondRefOf(\_SB.PC00.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        \_SB.PC00.GFX0.IUEH(2)
      }
    }

    // "K" Scan Code = 0x25
    ElseIf (LEqual(Local0,0x25))
    {
      // Virtual Button Event - Volume Down

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume Down Press
        Notify(\_SB.HIDD,0xC6) // Notify HID driver that Volume Down button is pressed.
        Notify(\_SB.HIDD,0xC7) // Notify HID driver that Volume Down button is released.
      } ElseIf (CondRefOf(\_SB.PC00.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        \_SB.PC00.GFX0.IUEH(3)
      }
    }

    // "L" Scan Code = 0x26
    ElseIf (LEqual(Local0,0x26))
    {
      // Virtual Button Event - Rotation Lock
      Sleep(1000)
      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: RotationLock Press
        Notify(\_SB.HIDD,0xC8) // Notify HID driver that Rotation Lock button is pressed.
        Notify(\_SB.HIDD,0xC9) // Notify HID driver that Rotation Lock button is released.
      } ElseIf (CondRefOf(\_SB.PC00.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        \_SB.PC00.GFX0.IUEH(4)
      }
    }

    // ";" Scan Code = 0x27
    ElseIf (LEqual(Local0,0x27))
    {
      // Virtual Button Event - Convertible Indicator Toggle

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
        Xor(PB1E, 0x08, PB1E) // Toggle Slate/Laptop Lock Status
        If (And(PB1E, 0x08)) // Follower/Laptop Mode changed 0 -> 1 Laptop mode
        {
          UPBT(CONVERTIBLE_BUTTON, One)
          Notify(\_SB.HIDD, 0xCD) // Notify HID Event Filter driver that the system is now in Laptop mode.
        } Else { // Follower/Laptop Mode Changed 1 -> 0 Slate/Tablet Mode
          UPBT(CONVERTIBLE_BUTTON, Zero)
          Notify(\_SB.HIDD, 0xCC) // Notify HID Event Filter driver that the system is now in Slate/Tablet mode.
        }
      } Else {
        If (CondRefOf(\_SB.PC00.GFX0.IUEH)) {
          // Win8 Compatible mode.  Graphics driver takes care of the hot key.
          \_SB.PC00.GFX0.IUEH(6)
        }
      }

    }

    // "'" Scan Code = 0x28
    ElseIf (LEqual(Local0,0x28))
    {
      // Virtual Button Event - Docking Indicator Toggle

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
        Xor(PB1E, 0x10, PB1E) // Toggle Dock/Undock Status
        If (And(PB1E, 0x10)) // Dock/Undock status changed 0 -> 1 Dock mode
        {
          UPBT(DOCK_INDICATOR, One)
          Notify(\_SB.HIDD, 0xCA) // Notify HID Event Filter driver that the system is now in Dock mode.
        } Else { // Dock/Undock status Changed 1 -> 0 Undock mode
          UPBT(DOCK_INDICATOR, Zero)
          Notify(\_SB.HIDD, 0xCB) // Notify HID Event Filter driver that the system is now in undock mode.
        }
      } Else {
        If (CondRefOf(\_SB.PC00.GFX0.IUEH)) {
          // Win8 Compatible mode.  Graphics driver takes care of the hot key.
          \_SB.PC00.GFX0.IUEH(7)
        }
      }

    }

    // F1 Scan Code = 0x3B
    ElseIf (LEqual(Local0,0x3B))
    {
      DHCF(1, 0)              // IGfx Display Switch using Toggle List 1.
    }

    // F2 Scan Code = 0x3C
    ElseIf (LEqual(Local0,0x3C))
    {
      DHCF(1, 1)              // IGfx Display Switch using Toggle List 2.
    }

    // F3 Scan Code = 0x3D
    ElseIf (LEqual(Local0,0x3D))
    {
      DHCF(1, 2)              // IGfx Display Switch using Toggle List 3
    }

    // F4 Scan Code = 0x3E
    ElseIf (LEqual(Local0,0x3E))
    {
      DHCF( 1, 3)             // IGfx Display Switch using Toggle List 4
    }

    // F8 Scan Code = 0x42
    ElseIf (LEqual(Local0,0x42))
    {
      // Panel Fitting Hot Key.
      If (IGDS)
      {
        If (CondRefOf(HPFS))
        {
          HPFS()
        } Else {
          \_SB.PC00.GFX0.AINT(2, 0)
        }
      }
    }

    // F9 Scan Code = 0x43
    ElseIf (LEqual(Local0,0x43))
    {
      DHCF(3, 0)              // Decrease Brightness Level.
    }

    // F10 Scan Code = 0x44
    ElseIf (LEqual(Local0,0x44))
    {
      DHCF(2, 0)              // Increase Brightness Level.
    }

    // F12 Scan Code = 0x58
    ElseIf (LEqual(Local0,0x58))
    {
      \_SB.HIDD.HPEM(11)      // HID Event Index 11: System Sleep
    }

    // Home Scan Code = 0x47
    // Don't use: SDS shares this key with Left Arrow key and should not be used for Hotkey.

    // UpArw Scan Code = 0x48
    ElseIf (LEqual(Local0,0x48))
    {
      \_SB.HIDD.HPEM(17)         // HID Event Index 17: Volume Increment
    }

    // PgUp Scan Code = 0x49
    // Don't use: SDS shares this key with Up Arrow key and should not be used for Hotkey.

    // LftArw Scan Code = 0x4B
    ElseIf (LEqual(Local0,0x4B))
    {
      \_SB.HIDD.HPEM(20)         // HID Event Index 20: Display Brightness Decrement
    }

    // RtArw Scan Code = 0x4D
    ElseIf (LEqual(Local0,0x4D))
    {
      \_SB.HIDD.HPEM(19)         // HID Event Index 19: Display Brightness Increment
    }

    // End Scan Code = 0x4F
    // Don't use: SDS shares this key with Right Arrow key and should not be used for Hotkey.

    // DwnArw Scan Code = 0x50
    ElseIf (LEqual(Local0,0x50))
    {
      \_SB.HIDD.HPEM(18)         // HID Event Index 18: Volume Decrement
    }

    // PgDn Scan Code = 0x51
    // Don't use: SDS shares this key with Down Arrow key and should not be used for Hotkey.

    // Ins Scan Code = 0x52
    ElseIf (LEqual(Local0,0x52))
    {
      \_SB.HIDD.HPEM(13)         // HID Event Index 13: Scan Prev Track
    }

    // Del Scan Code = 0x53
    ElseIf (LEqual(Local0,0x53))
    {
      \_SB.HIDD.HPEM(12)        // HID Event Index 12: Scan Next Track
    }
  } // end Method(_Q56) CAS hot key event

  Method(_QD5)    // 10 second power button press.
  {
    \_SB.PWPR()
  }

  Method(_QD6)    // 10 second power button de-press.
  {
    \_SB.PWRR()
  }

  Method(_Q80)    // Volume Up
  {
    If(LEqual(\VBVP,1))
    {
      // Volume Up.
      // If VirtualButton driver loaded
      //   use VirtualButton driver
      // Else If HID Event Driver loaded
      //   use HID Event Driver
      // Else
      //   use GFX Driver.
      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume UP
        Notify(\_SB.HIDD,0xC4) // Notify HID driver that Volume UP button is pressed.
      } Else {
        If (LEqual(\_SB.HIDD.HRDY, 1)){
          \_SB.HIDD.HPEM(17)
        } Else {
          \_SB.PC00.GFX0.IUEH(2)
        }// End of If (LEqual(\_SB.HIDD.HRDY, 1))
      }
    }
  }

  Method(_Q81)    // Volume Down
  {
    If(LEqual(\VBVD,1))
    {
      // Volume Down.
      // If VirtualButton driver loaded
      //   use VirtualButton driver
      // Else If HID Event Driver loaded
      //   use HID Event Driver
      // Else
      //   use GFX Driver.

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume Down
        Notify(\_SB.HIDD,0xC6) // Notify HID driver that Volume Down button is pressed.
      } Else {
        If (LEqual(\_SB.HIDD.HRDY, 1)){
          \_SB.HIDD.HPEM(18)
        } Else {
          \_SB.PC00.GFX0.IUEH(3)
        }
      }
    }
  }

Method(_Q82)    // Tablet Switch Toggle Slate/Tablet mode
{

  If (LEqual(\SMSS,1)) // SDS, PantherMtn, GrizzlyMtn or StarBrook
  {
    // Rotation Lock button
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    // Else
    //   use GFX Driver.
    ADBG("Toggle Tablet mode ")

    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
      Or(PB1E, 0x08, PB1E) // Toggle Slate Status
      UPBT(CONVERTIBLE_BUTTON, Zero)
      ADBG("Notify 0xCC")
      Notify(\_SB.HIDD, 0xCC) // Notify HID Event Filter driver that the system is now in Slate/Tablet mode.
    } Else {
      \_SB.PC00.GFX0.IUEH(6) // Convertible Indicator lock
    }
  }

}

Method(_Q83)    // Tablet Switch Toggle Laptop/Normal mode
{

  If (LEqual(\SMSS,1)) // SDS, PantherMtn, GrizzlyMtn or StarBrook
  {
    // Rotation Lock button
    // If VirtualButton driver loaded
    //   use VirtualButton driver
    // Else
    //   use GFX Driver.
    ADBG("Toggle Laptop mode")

    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
      Or(PB1E, 0x08, PB1E) // Set Laptop status
      UPBT(CONVERTIBLE_BUTTON, One)
      ADBG("Notify 0xCD")
      Notify(\_SB.HIDD, 0xCD) // Notify HID Event Filter driver that the system is now in Laptop mode.
    } Else {
      \_SB.PC00.GFX0.IUEH(6) // Convertible Indicator lock
    }
  }

}

  Method(_Q85)    // Windows Home button
  {
    If(LEqual(\VBHB,1))
    {
      // Windows Home Button
      // If VirtualButton driver loaded
      //   use VirtualButton driver
      // Else If HID Event Driver loaded
      //   use HID Event Driver
      // Else
      //   use GFX Driver.

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Windows Home Press
        Notify(\_SB.HIDD,0xC2) // Notify HID driver that Windows Home button is pressed.
      } Else {
        If (LEqual(\_SB.HIDD.HRDY, 1)){
          \_SB.HIDD.HPEM(1)
        } Else {
          \_SB.PC00.GFX0.IUEH(1)
        }
      }
    }
  }

  Method(_Q86)    // Rotation Lock press event
  {
    If(LEqual(\VBRL,1))
    {
      // Rotation Lock button
      // If VirtualButton driver loaded
      //   use VirtualButton driver
      // Else If HID Event Driver loaded
      //   use HID Event Driver
      // Else
      //   use GFX Driver.

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: RotationLock Press
        Notify(\_SB.HIDD,0xC8) // Notify HID driver that Rotation Lock button is pressed.
      } Else {
        If (LEqual(\_SB.HIDD.HRDY, 1)){
          \_SB.HIDD.HPEM(2)
        } Else {
          \_SB.PC00.GFX0.IUEH(4)
        }
      }
    }
  }

  Method(_Q87)    // Mode Switch: Clam Shell (Laptop) mode and Slate mode switch
  {
    If (LEqual(\SMSS,1)) // SDS, PantherMtn, GrizzlyMtn or StarBrook
    {
      // Rotation Lock button
      // If VirtualButton driver loaded
      //   use VirtualButton driver
      // Else
      //   use GFX Driver.

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
        Xor(PB1E, 0x08, PB1E) // Toggle Slate/Laptop Lock Status
        If (And(PB1E, 0x08)) // Follower/Laptop Mode changed 0 -> 1 Laptop mode
        {
          UPBT(CONVERTIBLE_BUTTON, One)
          Notify(\_SB.HIDD, 0xCD) // Notify HID Event Filter driver that the system is now in Laptop mode.
        } Else { // Follower/Laptop Mode Changed 1 -> 0 Slate/Tablet Mode
          UPBT(CONVERTIBLE_BUTTON, Zero)
          Notify(\_SB.HIDD, 0xCC) // Notify HID Event Filter driver that the system is now in Slate/Tablet mode.
        }
      } Else {
        \_SB.PC00.GFX0.IUEH(6) // Convertible Indicator lock
      }
    }

  }

  Method(_Q88)    // Volume Up release event
  {
    If(LEqual(\VBVP,1))
    {
      // Volume Up button release event.
      // If VirtualButton driver loaded
      //   use VirtualButton driver
      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume UP Release
        Notify(\_SB.HIDD,0xC5) // Notify HID driver that Volume UP button is released.
      }
    }
  }

  Method(_Q89)    // Volume Down release event
  {
    If(LEqual(\VBVD,1))
    {
      // Volume Down button release event.
      // If VirtualButton driver loaded
      //   use VirtualButton driver

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume Down Release
        Notify(\_SB.HIDD,0xC7) // Notify HID driver that Volume UP button is released.
      }
    }
  }

  Method(_Q8A)    // Windows Home button release event
  {
    If(LEqual(\VBHB,1))
    {
      // Windows Home Button release event
      // If VirtualButton driver loaded
      //   use VirtualButton driver

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Windows Home Button Release
        Notify(\_SB.HIDD,0xC3) // Notify HID driver that Windows Home button is released.
      }
    }
  }

  Method(_Q8B)    // Rotation Lock release event
  {
    If (LEqual(\VBRL,1)) {
      // Windows Rotation Lock release event

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Rotation Lock Release
        Notify(\_SB.HIDD,0xC9) // Notify HID driver that Windows Home button is released.
      }
    }
  }

  Method(_QF0)    // Thermal Event.
  {
    Notify(\_TZ.TZ00,0x80)
  }

  //
  // Subfunctions for VB driver and HID Event Filter driver. Those were all defined under VGBI before but
  // due to HID Event Filter driver integration effort, they are relocated under EC device so that both VGBI and HIDD
  // can use those methods.
  // Relocated methods: VGBS, UPBT
  // Removed method: PBIN (no longer used)
  //
  Name(VBDS,0)
  Name(ONTM,0) // Temporary variable to initialise VBDS only once in the boot
  Method(VGBS,0,Serialized)  // Virtual GPIO Button Status - Refer Bios Arch Spec
  {
    // Bit[7] : Docking Indicatory Status
    // Bit[6] : Convertible/Slate Indicator Status
    // Bit[5] : Reserved
    // Bit[4] : Rotation Lock Button Status
    // Bit[3] : Volume Down Button Status
    // Bit[2] : Volume Up Button Status
    // Bit[1] : Windows Home Button Status
    // Bit[0] : Power Button Status

    If (LEqual(ONTM,0))
    {
      // Initial setup option on VBDS and thereafter hotkeys should be updating the VBDS
      If (LEqual(And(PB1E,0x04),0x04)) // Rotation Lock
      {
        UPBT(ROTATION_LOCK_BUTTON,One)
      }

      If (LEqual(And(PB1E,0x08),0x08)) // Slate/Laptop
      {
        UPBT(CONVERTIBLE_BUTTON,One)
      }

      If (LEqual(And(PB1E,0x10),0x10))  // Undock/Dock
      {
        UPBT(DOCK_INDICATOR,One)
      }
      Store(One,ONTM)
    }
    Return(VBDS)
  }// End of Method

  //
  // UPBT Update Button Status
  //
  // Arg0: Bit location of the target button
  //       0: Power Button
  //       1: Windows Button
  //       2: Volume up Button
  //       3: Volume down Button
  //       4: Rotation Lock Button
  //       5: Reserved
  //       6: Convertible state 0 - Slate, 1 - Notebook
  //       7: Dock Indicator 0 - Undock, 1 - Dock
  //
  // Arg1: On/Off state, 0 - Clear the target bit, 1 - Set the target bit.
  //
  Method(UPBT,2,Serialized)  // Update Button Status
  {
    ShiftLeft(One, Arg0, Local0)
    If (Arg1){                // Button Press/ON
      Or(VBDS, Local0, VBDS)
    } Else {                 // Button Press/OFF
      And(VBDS, Not(Local0),VBDS)
    }
  } // End of UPBT

Device(WDT0)  // WDT Device Resource Consumption
{
  Name(_HID,EISAID("PNP0C02"))

  Name(_UID,3)

  Name(_CRS,ResourceTemplate()
  {
    IO(Decode16,0x6A4,0x6A4,0x1,0x1)  // 1 Byte EC Prv Intfc.
    IO(Decode16,0x6A0,0x6A0,0x1,0x1)  // 1 Byte EC Prv Intfc.
  })
}

/************************************************************************;
;*
;* Name:  CHDK
;*
;* Description: Check DOCK status, returen True if Dock status equal Arg0
;*
;************************************************************************/

  Method(CHDK, 1)
  {
    // If not Mobile Platform then skip the code and return 0
    If (LEqual(ECON,1)){
//@todo: Waiting for DOCK offect data
//      If (LEqual(ECRD(RefOf(DOCK)), Arg0))
      If (LEqual(0, Arg0))
        { // If not docked then it's hot plug
          Return(1)
        }
    }
    Return(0)
  }

  //
  // Hardware Button Array support
  //

  Device(BIND)  // Button Indicators.
  {
     Name(_HID, "INT33D2")
     Name(_CID, "PNP0C40")

     Method(_STA, 0,Serialized)
     {
       If (LAnd(And(IUBE,1), LGreaterEqual(OSYS, 2013)))
       {
         If (And(PB1E, 1)) // 10Sec Power Button is enabled?
         {
           Return(0x000B) // Not visible in UI
         }
         Else
         {
           Return(0x000F)
         }
       }
       Return(0x00)
     }
     //
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

       If (LEqual(Arg0, ToUUID ("DFBCF3C5-E7A5-44E6-9C1F-29C76F6E059C")))
       {
         If (LEqual(0,ToInteger(Arg1)))        // Revision 0.
         {
           Switch (ToInteger(Arg2))            // Switch to Function Index.
           {
             //
             // Function 0, Query of supported functions.
             //

             Case (0)
             {
               Return (Buffer() {0x03})
             }

             //
             // Function 1, Windows Compatible Button Array Power Button Properties.
             //

             Case (1)
             {
               // Only return support if platform enabled via setup.

               If (LEqual(And(PB1E, 1), 1))
               {
                 Return (0x07)
               }

               Return(0x00)

             }
           } // End Switch statement
         }  // End Revision check
       }  // End UUID check

       // If the code falls through to this point, just return a buffer of 0.

       Return (Buffer() {0x00})

    }  // End _DSM Method
  }

  Device(CIND)  // Convertible Indicators.
  {
     Name(_HID, "INT33D3")
     Name(_CID, "PNP0C60")

     Method(_STA, 0,Serialized)
     {
       If (LAnd(And(IUCE,1), LGreaterEqual(OSYS, 2012)))
       {
         Return(0x000F)
       }
       Return(0x00)
     }
  }

  Device(DIND)  // Docking Indicators.
  {
     Name(_HID, "INT33D4")
     Name(_CID, "PNP0C70")
     Method(_STA, 0,Serialized)
     {
       If (LAnd(And(IUDE,1), LGreaterEqual(OSYS, 2012)))
       {
         Return(0x000F)
       }
       Return(0x00)
     }
  }

  // Define a Lid Switch.
  Device(LID0)
  {
    Name(_HID,EISAID("PNP0C0D"))
    Method(_STA)
    {
      If (LEqual(PFLV,FlavorMobile)){
        Return(0x0F)
      }
      Return(0x00)
    }
    Method(_LID,0)
    {
      // 0 = Closed, 1 = Open.
      Return(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.LSTE)))
    }
  }
} // End H_EC

  // System Bus

Scope(\_SB)
{

  // Define an AC Device for ULT, ULX and Halo Platforms.

  Device(ADP1)
  {
    Name(_HID,"ACPI0003")

    Method(_STA)
    {
      If(LEqual(\ADAS,0)){
        Return(0x00)
      }
      If (LEqual(ECON,1)){
        Return(0x0F)
      }
      Return(0x00)
    }

    // Return the value that determines if running
    // from AC or not.

    Method(_PSR,0)
    {
      Return(PWRS)
    }

    // Return that everything runs off of AC.

    Method(_PCL,0)
    {
      Return (
        Package() { _SB }
      )
    }
  }

  // Define a (Control Method) Power Button.

  Device(PWRB)
  {
    Name(_HID,EISAID("PNP0C0C"))

    // The PRW isn't working when
    // placed in any of the logical locations ( PS2K, PS2M,
    // H_EC ), so a Power Button Device was created specifically
    // for the WAKETIME_SCI PRW.

    //
    // Power button status flag used to communicate H_EC.VBDL
    //
    Name(PBST, 1)

    //
    // Up Press Register flag. Set when OS register to recieve the up press of the power button
    //
    Name(UPPS, 0)

    //
    // Status of Power Button Level when EC is in mode where SCI is sent for both press and release of power button
    //
    Name(PBLV, 0)

    Method(PKG2, 2, Serialized) {
      Name (PKG, Package(2){0,0})
      Store(Arg0, Index(PKG,0))
      Store(Arg1, Index(PKG,1))
      Return (PKG)
    }

    Method(_PRW, 0) {
      Name (GPEB, 0)   // GPE status bit #

      // Get GPIO status bit location from PCH lib
      If (LNotEqual(\PPBG, 0))
      {
        Store(GGPE(\PPBG), GPEB)   // PM_PWRBTN is GPD, Pad 3
        Return(PKG2(GPEB, 4))
      }
      Return(Package(){30,4})       //GPI14 = GPE30 = Waketime SCI for Haswell Traditional boards
    }

    Method(_STA, 0)
    {
      If (LAnd(LEqual(ECON,1), PBST)){
        Return(0x0F)
      }
      Return(0x00)
    }

    Method(PBUP, 0) {
      If(UPPS) {
        Notify(\_SB.PWRB, 0xC0) // Send release notification to Power Button device
      }
    }

    //
    // _DSM : Device Specific Method for the Power Button.
    //
    // Arg0: UUID Unique function identifier
    // Arg1: Integer Revision Level
    // Arg2: Integer Function Index
    // Arg3: Package Parameters
    //
    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      // Compare passed in UUID to supported UUID.
      If (LEqual(Arg0, ToUUID ("9C355BCB-35FA-44f7-8A67-447359C36A03")))
      {
        If (LEqual(0,ToInteger(Arg1)))        // Revision 0.
        {
          Switch (ToInteger(Arg2)) {           // Switch to Function Index.
            //
            // Function 0, Query of supported functions.
            //
            Case (0) {
              // Need check for platfroms that support this
              Return (Buffer() {0x07}) // Supports Function 1,2.
            }

            //
            // Function 1, Register for Power Button Release Notify.
            //
            Case (1) {
              Store(0x1, UPPS) // OS has registered to recieve notify on release of power button
              //
              // Place EC into mode where SCI is sent for both press and release of power button
              //
              \_SB.PC00.LPCB.H_EC.ECMD(0x74)
              Return(0x00)
            }

            //
            // Function 2, Power Button Level.
            //
            Case (2) {
              If (UPPS) { // If OS has registered to recieve notify on release of power button
                If (LNot(PBLV)) {
                  return(0x00) // Power button not pressed
                } else {
                  return(0x01) // Power button pressed
                }
              } else {
                If (\_SB.PC00.LPCB.PRBL) {
                  return(0x00) // Power button not pressed
                } else {
                  return(0x01) // Power button pressed
                }
              }
            }
          } // End Switch statement
        } // End Revision check
      } // End UUID check

      //
      // If the code falls through to this point, just return a buffer of 0.
      //
      Return (Buffer() {0x00})
    } // End _DSM Method


  }//end device PWRB

  Method(PWPR, 0, Serialized) // Power Button Press Helper Method
  {
    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Power Button Press
      Notify(\_SB.HIDD,0xCE) // Notify HID driver that Power button is pressed.
    } Else {
      If(LEqual(\_SB.PWRB.PBST, 0x1)) { // Using Control Method Power Button
        Notify(\_SB.PWRB, 0x80) // Send press notification to Power Button device
        Store(1, \_SB.PWRB.PBLV) // Keep track of Power Button Level
      }

      If (CondRefOf(\_SB.PC00.GFX0.IUER))
      {
        Store(\_SB.PC00.GFX0.IUER, Local0)
        And(Local0, 0xC0, \_SB.PC00.GFX0.IUER)  // Clear 4:0 button events on entry.
        Store(\_SB.PC00.GFX0.IUER, Local0)
        Or(Local0, 0x01, \_SB.PC00.GFX0.IUER)   // Set Power Button Status = pressed.
      }

    } //If Win10
  } //End Method

  Method(PWRR, 0, Serialized) // Power Button Release Helper Method
  {
    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Power Button Release
      Notify(\_SB.HIDD,0xCF) // Notify HID driver that Power button is released.
    } Else {
      If(LEqual(\_SB.PWRB.PBST, 0x1)) { // Using Control Method Power Button
        \_SB.PWRB.PBUP() // Send release notification to Power Button device if requested
        Store(0, \_SB.PWRB.PBLV) //// Keep track of Power Button Level
      }

      If (CondRefOf(\_SB.PC00.GFX0.IUER))
      {
        Store(\_SB.PC00.GFX0.IUER, Local0)
        And(Local0, 0xC0, \_SB.PC00.GFX0.IUER)  // Clear 4:0 button events on entry.
      }

    } //If Win10
  } //End Method
}//end scope _SB
