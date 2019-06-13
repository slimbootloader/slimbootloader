/** @file
  ACPI DSDT table

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


External(\_SB.IETM, DeviceObj)

External(HGAS, MethodObj)
External(HPFS, MethodObj)
External(HBRT, MethodObj)
External(CHPS, MethodObj)
External(UP1D)
External(SVRF, MethodObj)
External(\_SB.PCI0.GFX0.CLID)
External(\_SB.PCI0.GFX0.IUEH, MethodObj)
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
External(\_SB.PCI0.GFX0.IUER, IntObj)

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
    APKP, 16,     // 113,114 Maximum (peak) adapter power output in (mW).
    CFSP, 16,     // 115,116 CPU Fan speed in rpm
    APKT, 16,     // 117,118 Max time the adapter can maintain peak power in (ms).
    Offset(120),  // (0x78)
    DLED, 1,      // 120.0   EC Connected Standby Debug LED (CAPS LOCK)
        , 2,      // 120.3:2   Reserved
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
    Offset(132),
    NPWR, 16,     // 132,133 Actual platform power consumption in (mW)
    LSOC, 8,      // 134     Long term battery charge is based on c/20 SoC (%)
    ARTG, 16,     // 135,136 AC adapter rating in 10 mW. Max adapter power supported. Becasue of EC-space constraint EC can only send 2bytes=15000mW but system uses 150000mW hence bios will multiply this value by 10 and return.
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
    Offset(161),
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
    PEC0, 8,      // 188     Peci Error Count(LSB)
    PEC1, 8,      // 189     Peci Error Count
    PEC2, 8,      // 190     Peci Error Count
    PEC3, 8,      // 191     Peci Error Count(MSB)
    Offset(196),
    WTMS, 8,      // 196     Wake timer Settings
                  //          196.0   Timer Enabled in S3
                  //          196.1   Timer Enabled in S4
                  //          196.2   Timer Enabled in S5
                  //          196.3:6 Reserved
                  //          196.7   Enable/Disable EC Timer
    AWT2, 8,      // 197     ACPIWakeTmrByte2 - Wake timer value (BIT23-16)
    AWT1, 8,      // 198     ACPIWakeTmrByte1 - Wake timer value (BIT15-8)
    AWT0, 8,      // 199     ACPIWakeTmrByte0 - Wake timer value (BIT7-0)
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
    Offset(203),
    CMDM, 8,      // 203     Command Mirror updated by EC whenever it received command through offset 58
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
        If (LEqual(\ECUP,Zero)) {// If EC has been sent to low power mode
          If (LNotEqual(ELPM, 0)) {
            // Set EC Wake GPIO
            \_SB.SGOV(ELPM, 1)
            // check "EC Low Power Exit" pin
            If (LEqual(\_SB.GGOV(ELPS), 0)) {
              Sleep(16)
            }
          }
        }
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
        If (LEqual(\ECUP,Zero)) {// If EC has been sent to low power mode
          if (LNotEqual (ELPM, 0)){
            // Set EC Wake GPIO
            \_SB.SGOV(ELPM, 1)
            // check "EC Low Power Exit" pin
            If (LEqual(\_SB.GGOV(ELPS), 0)) {
              Sleep(16)
            }
          }
        }
        Store(Arg0,Arg1) // Execute Write to EC
      } // If (ECAV)
      Release(ECMT)
    } // If EC Acquired
  }

  // ECWR (Embedded Write Method)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (2)
  //    Arg0 - Value to Write
  //    Arg1 - Object to Write to
  //  Return Value:
  //    None
  //
  Method (ECWR,2,Serialized,,,{IntObj, FieldUnitObj})
  {
    Store (Acquire (ECMT, 1000), Local0)  // save Acquire result so we can check for Mutex acquired
    If (LEqual (Local0, Zero))  // check for Mutex acquired
    {
      If (ECAV) {
        If (LEqual (\ECUP,Zero)) {// If EC has been sent to low power mode
          // Set EC Wake GPIO
          \_SB.SGOV (ELPM, 1)
          // check "EC Low Power Exit" pin
          If (LEqual (\_SB.GGOV (ELPS), 0)) {
            Sleep (16)
          }
        }
        Store (Arg0,Arg1) // Execute Write to EC

        Store (0, Local1)
        While (1) {
          If (LEqual (Arg0, DerefOf (Arg1))){
            break;
          }
          Sleep (1)
          Store (Arg0, Arg1)
          Add (Local1, 1, Local1)
          If (LEqual(Local1, 3)) {
            break
          }
        }

      } // If (ECAV)
      Release (ECMT)
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
      While(ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CMDR))) {Stall (20)} // make sure no command is currently being processed so we don't overwrite it
      ECWT(Arg0, RefOf(\_SB.PCI0.LPCB.H_EC.CMDR))
      Return (0x00)
    }
    Return (0xFF)
  }

  Method (ECM1,1,Serialized) {
    If (ECAV) {
      While (ECRD (RefOf (\_SB.PCI0.LPCB.H_EC.CMDR))) {Stall (20)} // make sure no command is currently being processed so we don't overwrite it
      ECWT (Arg0, RefOf (\_SB.PCI0.LPCB.H_EC.CMDR))
      Store (0, Local1)
      While (1) {
        If (LEqual (Arg0, ECRD (RefOf (\_SB.PCI0.LPCB.H_EC.CMDM)))) {
          break;
        }

        Stall (20)
        If (LEqual (Arg0, ECRD (RefOf(\_SB.PCI0.LPCB.H_EC.CMDM)))) {
          break;
        } else {
          While (ECRD (RefOf (\_SB.PCI0.LPCB.H_EC.CMDR))) {Stall (20)} // make sure no command is currently being processed so we don't overwrite it
          ECWT (Arg0, RefOf (\_SB.PCI0.LPCB.H_EC.CMDR))
        }

        Add (Local1, 1, Local1)
        If (LEqual(Local1, 10)) {
          break
        }
        P8XH (0,0xDC)
      }
      ECWT (0, RefOf (\_SB.PCI0.LPCB.H_EC.CMDM))
    }
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
        ADBG ("EC Exit CS")
        if (LEqual(ECNO,0x01)) { // Check EC notification enabled in Setup
          ADBG ("EC Notify")
          Store (Acquire(\EHLD, 0xFFFF), Local0) // Wait for Mutex for telling EC to exit Low Power Mode
          if (LEqual(Local0, Zero)) {
            \_SB.PCI0.LPCB.H_EC.ECMD (0x2D) // Notify EC of CS exit
            If (LEqual(ECLP, 0x1)) {
              Store (One,\ECUP)
            }
            Release (\EHLD)
          }
          if (LEqual(ECDB,0x01)) {
            ADBG ("EC Debug")
            \_SB.PCI0.LPCB.H_EC.ECWT (0, RefOf(\_SB.PCI0.LPCB.H_EC.DLED)) //Clear EC CS Debug Light (CAPS LOCK)
          }
        }
        Return (0x00)
      }

      Case(1) { // enter CS
        D8XH (0, 0xC5)
        D8XH (1, 0x01)
        ADBG ("EC Enter CS")

        if (LEqual(ECNO,0x01)) { // Check EC notification enabled in Setup
          ADBG ("EC Notify")
          if (LEqual(ECDB,0x01)) {
            ADBG ("EC Debug")
            \_SB.PCI0.LPCB.H_EC.ECWT (1, RefOf(\_SB.PCI0.LPCB.H_EC.DLED)) //Set EC CS Debug Light (CAPS LOCK)
          }
          \_SB.PCI0.LPCB.H_EC.ECMD (0x2C) // Notify EC of CS entry
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
    //Store(LSTE,\_SB.PCI0.GFX0.CLID)
    If (LEqual(ECRD(RefOf(LSTE)), 0))
    {
      Store(0,\_SB.PCI0.GFX0.CLID)
    }
    If (LEqual(ECRD(RefOf(LSTE)), 1))
    {
      Store(3,\_SB.PCI0.GFX0.CLID)
    }
    Store(ECRD(RefOf(LSTE)),LIDS)

    // Update the Dock Status
    Store(\DSTS,\_SB.PCI0.GFX0.CDCK)

    // Unconditionally fix up the Battery and Power State.

    // Initialize the Number of Present Batteries.
    //  1 = Real Battery 1 is present
    //  2 = Real Battery 2 is present
    //  3 = Real Battery 1 and 2 are present
    Store(0,BNUM)
    Or(BNUM,ShiftRight(And(ECRD(RefOf(B1ST)),0x08),3),BNUM)
    Or(BNUM,ShiftRight(And(ECRD(RefOf(B2ST)),0x08),2),BNUM)

    // Save the current Power State for later.
    Store(PWRS,Local0)

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

    PNOT()
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
      //Enable SCIs in EC to trigger QD5 and QD6 event
      //
      \_SB.PCI0.LPCB.H_EC.ECWT(0x01, RefOf(\_SB.PCI0.LPCB.H_EC.BTEN)) //Enable EC Power Button in ACPI name space
      \_SB.PCI0.LPCB.H_EC.ECMD(0x38)                                  //Enable/Disable SCIs from buttons
    }
  }

  //
  // The _GPE method needs to be the same as the GPE number assignment in ECDT
  //
  Method(_GPE)
  {
    If (\_SB.PCI0.LPCB.ESPI)
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

    PNOT()
  }

  Method(_Q31)    // Real Battery AC Removal Event.
  {
    // Set Global Power State = Battery Mode.

    Store(0,PWRS)

    // Perform needed ACPI Notifications.
    If(LAnd(LEqual(\DPTF,1), LEqual(\PWRE,1))) {
       Notify(\_SB.TPWR, 0x81) // notify Power participant
    }

    PNOT()
  }

  Method(_Q32)    // Real Battery Capacity Change.
  {
    // Perform needed ACPI Notifications.
    If(LAnd(LEqual(\DPTF,1), LEqual(\BATR,1))) {
       Notify(\_SB.BAT1, 0x86) // notify Battery participant
    }

    PNOT()
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

    PNOT()
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
   //PWRT 78d PwrSrcType Bit0=1 if AC, Bit1=1 if USB-PD
   ADBG("_Q37 PWR_SRC_CHANGE ")
   P8XH(0,0x37)

   And(\_SB.PCI0.LPCB.H_EC.PWRT, 0x03, Local0)

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
   PNOT()
  }

  Method(_Q40, 0, Serialized)   // Dock complete Event
  {
//@todo: Waiting for DOCK offect data
//    If (LEqual(ECRD(RefOf(DOCK)), 0)) {
    P8XH(0,0x40)
    Return()
//    }

    Sleep(1000) // Delay 1 second for hot docking stability
    Store(DKSM, \SSMP)
    Store(1, \DSTS)
    Sleep(1000) // Delay 1 second for hot docking stability

    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
      Or(PB1E, 0x10, PB1E)   // Set Dock status
      UPBT(DOCK_INDICATOR, One)
      ADBG("Notify 0xCA")
      Notify(\_SB.HIDD, 0xCA)    // Notify HID Event Filter driver that the system is now in Dock mode.
      If (LEqual(\CEDS,1)) {
        If (LEqual(ECRD(RefOf(DOCO)), 1)) {
          Or(PB1E, 0x08, PB1E) // Set Laptop status
          UPBT(CONVERTIBLE_BUTTON, One)
          ADBG("Notify 0xCD")
          Notify(\_SB.HIDD, 0xCD)  // Notify HID Event Filter driver that the system is now in Laptop mode.
        } Else {
          And(PB1E, Not(0x08), PB1E) // Clear Laptop Status
          UPBT(CONVERTIBLE_BUTTON, Zero)
          ADBG("Notify 0xCC")
          Notify(\_SB.HIDD, 0xCC)        // Notify HID Event driver that the system is now in Slate mode.
        }
      }
    } Else {
      If (IGDS)
      {
        \_SB.PCI0.GFX0.GDCK(1)
      }
      If (LEqual(\CEDS,1)) {
        If (CondRefOf(\_SB.PCI0.GFX0.IUEH)) {
          ADBG("IUEH 6")
          \_SB.PCI0.GFX0.IUEH(6)
          ADBG("IUEH 7")
          \_SB.PCI0.GFX0.IUEH(7)
        }
      }
    }

  }

  Method(_Q41)    // Surprise Removal
  {
//@todo: Waiting for DOCK offect data
//    If (LEqual(ECRD(RefOf(DOCK)), 1)) {
    P8XH(0,0x41)
    Return()
//    }

    Store(0, \DSTS)
    Sleep(1000) // Delay 1 second for hot un-docking stability

    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
      And(PB1E, Not(0x10), PB1E)   // Clear Dock Status
      UPBT(DOCK_INDICATOR, Zero)
      ADBG("Notify 0xCB")
      Notify(\_SB.HIDD, 0xCB)          // Notify HID Event Filter driver that the system is now in Undock mode.
      If (LEqual(\CEDS,1)) {
        And(PB1E, Not(0x08), PB1E) // Clear Laptop Status
        UPBT(CONVERTIBLE_BUTTON, Zero)
        ADBG("Notify 0xCC")
        Notify(\_SB.HIDD, 0xCC)        // Notify HID Event Filter driver that the system is now in Slate mode.
      }
    } Else {
      If (IGDS)
      {
        \_SB.PCI0.GFX0.GDCK(0)
      }
      If (LEqual(\CEDS,1)) {
        If (CondRefOf(\_SB.PCI0.GFX0.IUEH)) {
          ADBG("IUEH 6")
          \_SB.PCI0.GFX0.IUEH(6)
          ADBG("IUEH 7")
          \_SB.PCI0.GFX0.IUEH(7)
        }
      }
    }

  }

  Method(_Q42)    // Undocking request Event
  {
    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
    } Else {
      And(PB1E, Not(0x10), PB1E) // Clear Dock Status
      UPBT(DOCK_INDICATOR, Zero)
      If (CondRefOf(\_SB.PCI0.GFX0.IUER))
      {
        Store(\_SB.PCI0.GFX0.IUER, Local0)
        And(Local0, Not(0x80), \_SB.PCI0.GFX0.IUER)
      }
      ADBG("Notify 0xCB")
      Notify(\_SB.HIDD, 0xCB) // Notify HID Event Filter driver that Undocking event happened.
      If (IGDS)
      {
        \_SB.PCI0.GFX0.GDCK(0)
      }
    }

  }

  Method(_Q43)    // Express Card Presence Changed on Saddlestring (Docking)
  {
    Notify(\_SB.PCI0.RP07, 0x0)
  }

  Method(_Q44)  // Virtual Dock Presence Changed
  {
    If (\DSTS)
    {
      Store(0, \DSTS)
    }
    Else
    {
      Store(1, \DSTS)
    }
  }


  Method(_Q51)    // Lid Switch Event.
  {
//@todo: Waiting for DOCK offect data
//    If (LAnd(LEqual(\CEDS,1), LAnd(LEqual(ECRD(RefOf(DOCK)), 1), LEqual(ECRD(RefOf(DOCO)), 0)))) {
//      Return()
//    }
    ADBG("LID Event")
    // Update Lid NVS State.

    Store(ECRD(RefOf(LSTE)),LIDS)

    \_SB.PCI0.GFX0.GLID(LIDS)

    // If ETM is not disabled, notify IETM device
    If (CondRefOf(\_TZ.ETMD))
    {
      If (LEqual(\_TZ.ETMD, 0))
      {
        If (CondRefOf(\_SB.IETM))
        {
          ADBG("Notify IETM")
          Notify(\_SB.IETM,0x83)
        }
      }
    }
    ADBG("Notify LID0")
    Notify(\_SB.PCI0.LPCB.H_EC.LID0,0x80)
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
      ADBG("USB Attach Q")
      Notify(\_SB.PCI0.XDCI,0x80)
    }Else{
      ADBG("USB Detach Q")
      Notify(\_SB.PCI0.XDCI,0x81)
    }
  }

  Method(UCEV, 0, Serialized)    // Any event on USBTypeC.
  {
      P8XH(0, 0x79)
      Store( Timer(), Local0 )
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

      Divide(Subtract(Timer(), Local0), 10000, , Local1)
      ADBG(Local1)
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
          \_SB.PCI0.GFX0.GHDS(Arg1)
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
          BRTN(0x86)
        } Else {
          // Current brightness is a percentage number. This must be done if ALS is enabled,
          // as the driver will change the brightness based on ambient light.
          Store(\_SB.PCI0.GFX0.CBLV, Local0)

          // 1 % will be lost in the conversion, so need to make it up first.
          AND(Add(Local0, 1),0xFE, Local0)

          If (LLessEqual(Local0, 90))
          {
            Add(Local0, 10, Local0)
          }
          Store(Local0, BRTL)
          \_SB.PCI0.GFX0.AINT(1, Local0)
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
          BRTN(0x87)
        } Else {
          // Current brightness is a percentage number. This must be done if ALS is enabled,
          // as the driver will change the brightness based on ambient light.
          Store(\_SB.PCI0.GFX0.CBLV, Local0)

          // 1 % will be lost in the conversion, so need to make it up first.
          AND(Add(Local0, 1),0xFE, Local0)

          If (LGreaterEqual(Local0, 10))
          {
            Subtract(Local0, 10, Local0)
          }
          Store(Local0, BRTL)
          \_SB.PCI0.GFX0.AINT(1, Local0)
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

    ADBG("Fn+SCANCODE:")
    ADBG(ToHexString(Local0))

    // F1 Scan Code = 0x3B
    If (LEqual(Local0,0x3B))
    {
      ADBG("FN+F1")             // N/A
      ADBG("N/A")
    }

    // F2 Scan Code = 0x3C
    ElseIf (LEqual(Local0,0x3C))
    {
      ADBG("FN+F2")             // Hibernate
      ADBG("Hibernate")
    }

    // F3 Scan Code = 0x3D
    ElseIf (LEqual(Local0,0x3D))
    {
      ADBG("FN+F3")             // Display switch
      If(LEqual(\EHK3,1))
      {
        DHCF(1, 0)                // Display Switch using Toggle List 1.
      }
    }

    // F4 Scan Code = 0x3E
    ElseIf (LEqual(Local0,0x3E))
    {
      ADBG("FN+F4")             // KB Bright -
      If(LEqual(\EHK4,1))
      {
        If (LGreaterEqual(ECRD(RefOf(KBDB)),10)){
          Subtract(ECRD(RefOf(KBDB)),10,Local1)
          ECWT(Local1, RefOf(KBDB))
        } Else {
          ECWT(0, RefOf(KBDB))
        }
        ECMD(0x1B)
      }
    }

    // F5 Scan Code = 0x3F
    ElseIf (LEqual(Local0,0x3F))
    {
      ADBG("FN+F5")             // KB Bright +
      If(LEqual(\EHK5,1))
      {
        If (LLessEqual(ECRD(RefOf(KBDB)),90)){
          Add(ECRD(RefOf(KBDB)),10,Local1)
          ECWT(Local1, RefOf(KBDB))
        } Else {
          ECWT(100,RefOf(KBDB))
        }
        ECMD(0x1B)
      }
    }

    // F6 Scan Code = 0x40
    ElseIf (LEqual(Local0,0x40))
    {
      ADBG("FN+F6")             // Bright -
      If (LEqual(\EHK6,1))
      {
        DHCF(3, 0)
      }
    }

    // F7 Scan Code = 0x41
    ElseIf (LEqual(Local0,0x41))
    {
      ADBG("FN+F7")             // Bright +
      If(LEqual(\EHK7,1))
      {
        DHCF(2, 0)
      }
    }

    // F8 Scan Code = 0x42
    ElseIf (LEqual(Local0,0x42))
    {
      If(LEqual(\EHK8,1))
      {
        ADBG("FN+F8")           // Audio Mute
      }
    }

    // F9 Scan Code = 0x43
    ElseIf (LEqual(Local0,0x43))
    {
      ADBG("FN+F9")             // Audio Vlm -
    }

    // F10 Scan Code = 0x44
    ElseIf (LEqual(Local0,0x44))
    {
      ADBG("FN+F10")            // Audio Vlm +
    }

    // F11 Scan Code = 0x45
    ElseIf (LEqual(Local0,0x45))
    {
      ADBG("FN+F11")            // Airplane Mode/Wireless Radio Button
      \_SB.HIDD.HPEM(8)         // HID Event Index 8: Airplane Mode/Wireless Radio Button
    }

    // F12 Scan Code = 0x58
    ElseIf (LEqual(Local0,0x58))
    {
      ADBG("FN+F12")            // Print Screen
    }
  } // end Method(_Q52) Fn hot key event

  Method(_Q54)    // Power Button Event for Control method Power Button(10sec PB Override without V-GPIO driver)
  {
    If (LEqual(S0ID,0)) {  // if Connected Standby is not supported

      //
      // Check UAMS(User Absent Mode State) to notify the power button event.
      //
      If (UAMS){ // UAMS has a non-Zero value, means the system is under User Absent Mode. Send Wake up event.
        ADBG("PB Wake up 0x02")
        If (CondRefOf(\_SB.PWRB)){
          Notify(\_SB.PWRB, 0x02)
        }
      } Else { // UAMS is Zero, means the system is ON. Send Sleep event.
        ADBG("PB Sleep 0x80")
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
    ADBG(Concatenate("CAS+SCNCODE=", ToHexString(Local0)))

    // This event will be generated whenever a
    // CRTL+ALT+SHIFT+"Next Key" is hit.  This event handler
    // will base code execution on the "Next Key" Scan Code
    // stored in the EC Memory Space.

    // "BKSP" Scan Code = 0x0E
    If (LEqual(Local0,0x0E))
    {
      ADBG("CAS+BKSP")
      \_SB.HIDD.HPEM(14)        // HID Event Index 14: Stop.
    }

    // "Tab" Scan Code = 0x0F
    If (LEqual(Local0,0x0F))
    {
      ADBG("CAS+TAB")
      \_SB.HIDD.HPEM(15)        // HID Event Index 15: Play/Pause.
    }

    // "Q" Scan Code = 0x10
    If (LEqual(Local0,0x10))
    {
      ADBG("CAS+Q")
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(1)         // HID Event Index 1: Windows Button (Keyboard Left GUI).
    }

    // "W" Scan Code = 0x11
    ElseIf (LEqual(Local0,0x11))
    {
      ADBG("CAS+W")
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(2)         // HID Event Index 2: Rotation Lock
    }

    // "E" Scan Code = 0x12
    ElseIf (LEqual(Local0,0x12))
    {
      ADBG("CAS+E")
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(3)         // HID Event Index 3: Num Lock
    }

    // "R" Scan Code = 0x13
    ElseIf (LEqual(Local0,0x13))
    {
      ADBG("CAS+R")
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(4)         // HID Event Index 4: Home
    }

    // "T" Scan Code = 0x14
    ElseIf (LEqual(Local0,0x14))
    {
      ADBG("CAS+T")
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(5)         // HID Event Index 5: End
    }

    // "Y" Scan Code = 0x15
    ElseIf (LEqual(Local0,0x15))
    {
      ADBG("CAS+Y")
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(6)         // HID Event Index 6: Page Up
    }

    // "U" Scan Code = 0x16
    ElseIf (LEqual(Local0,0x16))
    {
      ADBG("CAS+U")
      Sleep(1000)               // Index 1-7 are Keyboard/Keypad HIDs.  Add delay.
      \_SB.HIDD.HPEM(7)         // HID Event Index 7: Page Down
    }

    // "I" Scan Code = 0x17
    ElseIf (LEqual(Local0,0x17))
    {
      ADBG("CAS+I")
      \_SB.HIDD.HPEM(8)         // HID Event Index 8: Wireless Radio Button
    }

    // "O" Scan Code = 0x18
    ElseIf (LEqual(Local0,0x18))
    {
      ADBG("CAS+O")
      \_SB.HIDD.HPEM(9)         // HID Event Index 9: System Power Down
    }

    // Calculator (Enter) Key = 0x1C
    ElseIf (LEqual(Local0,0x1C))
    {
      ADBG("CAS+Enter")
      \_SB.HIDD.HPEM(27)         // HID Event Index 27: System Wake
    }

    // "A" Scan Code = 0x1E
    If (LEqual(Local0,0x1E))
    {
      ADBG("CAS+A")
      \_SB.HIDD.HPEM(16)        // HID Event Index 16: Mute.
    }

    // "G" Scan Code = 0x22
    ElseIf (LEqual(Local0,0x22))
    {
      ADBG("CAS+G")
      // Virtual Button Event - Ctrl + Alt + Del Screen

      Sleep(1000) // 1sec delay is needed for Powerbutton and Windows Home button

      If (CondRefOf(\_SB.PCI0.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        ADBG("IUEH 0")
        \_SB.PCI0.GFX0.IUEH(0)
      }
    }

    // "H" Scan Code = 0x23
    ElseIf (LEqual(Local0,0x23))
    {
      ADBG("CAS+H")
      // Virtual Button Event - Windows Button
      Sleep(1000)

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Windows Home
        ADBG("Notify _HID 0xC2")
        Notify(\_SB.HIDD,0xC2) // Notify HID driver that Windows Home button is pressed.
        ADBG("Notify _HID 0xC3")
        Notify(\_SB.HIDD,0xC3) // Notify HID driver that Windows Home button is released.
      } ElseIf (CondRefOf(\_SB.PCI0.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        ADBG("IUEH 1")
        \_SB.PCI0.GFX0.IUEH(1)
      }
    }

    // "J" Scan Code = 0x24
    ElseIf (LEqual(Local0,0x24))
    {
      ADBG("CAS+J")
      // Virtual Button Event - Volume Up

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume UP Press
        ADBG("Notify _HID 0xC4")
        Notify(\_SB.HIDD,0xC4) // Notify HID driver that Volume UP button is pressed.
        ADBG("Notify _HID 0xC5")
        Notify(\_SB.HIDD,0xC5) // Notify HID driver that Volume UP button is released.
      } ElseIf (CondRefOf(\_SB.PCI0.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        ADBG("IUEH 2")
        \_SB.PCI0.GFX0.IUEH(2)
      }
    }

    // "K" Scan Code = 0x25
    ElseIf (LEqual(Local0,0x25))
    {
      ADBG("CAS+K")
      // Virtual Button Event - Volume Down

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume Down Press
        ADBG("Notify _HID 0xC6")
        Notify(\_SB.HIDD,0xC6) // Notify HID driver that Volume Down button is pressed.
        ADBG("Notify _HID 0xC7")
        Notify(\_SB.HIDD,0xC7) // Notify HID driver that Volume Down button is released.
      } ElseIf (CondRefOf(\_SB.PCI0.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        ADBG("IUEH 3")
        \_SB.PCI0.GFX0.IUEH(3)
      }
    }

    // "L" Scan Code = 0x26
    ElseIf (LEqual(Local0,0x26))
    {
      ADBG("CAS+L")
      // Virtual Button Event - Rotation Lock
      Sleep(1000)
      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: RotationLock Press
        ADBG("Notify _HID 0xC8")
        Notify(\_SB.HIDD,0xC8) // Notify HID driver that Rotation Lock button is pressed.
        ADBG("Notify _HID 0xC9")
        Notify(\_SB.HIDD,0xC9) // Notify HID driver that Rotation Lock button is released.
      } ElseIf (CondRefOf(\_SB.PCI0.GFX0.IUEH)) {
        // Win8 Compatible mode.  Graphics driver takes care of the hot key.
        ADBG("IUEH 4")
        \_SB.PCI0.GFX0.IUEH(4)
      }
    }

    // ";" Scan Code = 0x27
    ElseIf (LEqual(Local0,0x27))
    {
      ADBG("CAS+;")
      // Virtual Button Event - Convertible Indicator Toggle

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
        Xor(PB1E, 0x08, PB1E) // Toggle Slate/Laptop Lock Status
        If (And(PB1E, 0x08)) // Slave/Laptop Mode changed 0 -> 1 Laptop mode
        {
          UPBT(CONVERTIBLE_BUTTON, One)
          ADBG("Notify 0xCD")
          Notify(\_SB.HIDD, 0xCD) // Notify HID Event Filter driver that the system is now in Laptop mode.
        } Else { // Slave/Laptop Mode Changed 1 -> 0 Slate/Tablet Mode
          UPBT(CONVERTIBLE_BUTTON, Zero)
          ADBG("Notify 0xCC")
          Notify(\_SB.HIDD, 0xCC) // Notify HID Event Filter driver that the system is now in Slate/Tablet mode.
        }
      } Else {
        If (CondRefOf(\_SB.PCI0.GFX0.IUEH)) {
          // Win8 Compatible mode.  Graphics driver takes care of the hot key.
          ADBG("IUEH 6")
          \_SB.PCI0.GFX0.IUEH(6)
        }
      }

    }

    // "'" Scan Code = 0x28
    ElseIf (LEqual(Local0,0x28))
    {
      ADBG("CAS+'")
      // Virtual Button Event - Docking Indicator Toggle

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
        Xor(PB1E, 0x10, PB1E) // Toggle Dock/Undock Status
        If (And(PB1E, 0x10)) // Dock/Undock status changed 0 -> 1 Dock mode
        {
          UPBT(DOCK_INDICATOR, One)
          ADBG("Notify 0xCA")
          Notify(\_SB.HIDD, 0xCA) // Notify HID Event Filter driver that the system is now in Dock mode.
        } Else { // Dock/Undock status Changed 1 -> 0 Undock mode
          UPBT(DOCK_INDICATOR, Zero)
          ADBG("Notify 0xCB")
          Notify(\_SB.HIDD, 0xCB) // Notify HID Event Filter driver that the system is now in undock mode.
        }
      } Else {
        If (CondRefOf(\_SB.PCI0.GFX0.IUEH)) {
          // Win8 Compatible mode.  Graphics driver takes care of the hot key.
          ADBG("IUEH 7")
          \_SB.PCI0.GFX0.IUEH(7)
        }
      }

    }

    // F1 Scan Code = 0x3B
    ElseIf (LEqual(Local0,0x3B))
    {
      ADBG("CAS+F1")
      DHCF(1, 0)              // IGfx Display Switch using Toggle List 1.
    }

    // F2 Scan Code = 0x3C
    ElseIf (LEqual(Local0,0x3C))
    {
      ADBG("CAS+F2")
      DHCF(1, 1)              // IGfx Display Switch using Toggle List 2.
    }

    // F3 Scan Code = 0x3D
    ElseIf (LEqual(Local0,0x3D))
    {
      ADBG("CAS+F3")
      DHCF(1, 2)              // IGfx Display Switch using Toggle List 3
    }

    // F4 Scan Code = 0x3E
    ElseIf (LEqual(Local0,0x3E))
    {
      ADBG("CAS+F4")
      DHCF( 1, 3)             // IGfx Display Switch using Toggle List 4
    }

    // F8 Scan Code = 0x42
    ElseIf (LEqual(Local0,0x42))
    {
      ADBG("CAS+F8")
      // Panel Fitting Hot Key.
      If (IGDS)
      {
        If (CondRefOf(HPFS))
        {
          HPFS()
        } Else {
          \_SB.PCI0.GFX0.AINT(2, 0)
        }
      }
    }

    // F9 Scan Code = 0x43
    ElseIf (LEqual(Local0,0x43))
    {
      ADBG("CAS+F9")
      DHCF(3, 0)              // Decrease Brightness Level.
    }

    // F10 Scan Code = 0x44
    ElseIf (LEqual(Local0,0x44))
    {
      ADBG("CAS+F10")
      DHCF(2, 0)              // Increase Brightness Level.
    }

    // F12 Scan Code = 0x58
    ElseIf (LEqual(Local0,0x58))
    {
      ADBG("CAS+F12")
      \_SB.HIDD.HPEM(11)      // HID Event Index 11: System Sleep
    }

    // Home Scan Code = 0x47
    // Don't use: SDS shares this key with Left Arrow key and should not be used for Hotkey.

    // UpArw Scan Code = 0x48
    ElseIf (LEqual(Local0,0x48))
    {
      ADBG("CAS+UpArw")
      \_SB.HIDD.HPEM(17)         // HID Event Index 17: Volume Increment
    }

    // PgUp Scan Code = 0x49
    // Don't use: SDS shares this key with Up Arrow key and should not be used for Hotkey.

    // LftArw Scan Code = 0x4B
    ElseIf (LEqual(Local0,0x4B))
    {
      ADBG("CAS+LftArw")
      \_SB.HIDD.HPEM(20)         // HID Event Index 20: Display Brightness Decrement
    }

    // RtArw Scan Code = 0x4D
    ElseIf (LEqual(Local0,0x4D))
    {
      ADBG("CAS+RtArw")
      \_SB.HIDD.HPEM(19)         // HID Event Index 19: Display Brightness Increment
    }

    // End Scan Code = 0x4F
    // Don't use: SDS shares this key with Right Arrow key and should not be used for Hotkey.

    // DwnArw Scan Code = 0x50
    ElseIf (LEqual(Local0,0x50))
    {
      ADBG("CAS+DwnArw")
      \_SB.HIDD.HPEM(18)         // HID Event Index 18: Volume Decrement
    }

    // PgDn Scan Code = 0x51
    // Don't use: SDS shares this key with Down Arrow key and should not be used for Hotkey.

    // Ins Scan Code = 0x52
    ElseIf (LEqual(Local0,0x52))
    {
      ADBG("CAS+Ins")
      \_SB.HIDD.HPEM(13)         // HID Event Index 13: Scan Prev Track
    }

    // Del Scan Code = 0x53
    ElseIf (LEqual(Local0,0x53))
    {
      ADBG("CAS+Del")
      \_SB.HIDD.HPEM(12)        // HID Event Index 12: Scan Next Track
    }
  } // end Method(_Q56) CAS hot key event

  Method(_QD5)    // 10 second power button press.
  {
    ADBG("EC PB press")
    \_SB.PWPR()
  }

  Method(_QD6)    // 10 second power button de-press.
  {
    ADBG("EC PB release")
    \_SB.PWRR()
  }

  Method(_Q80)    // Volume Up
  {
    If(LEqual(\VBVP,1))
    {
      ADBG("Volume Up")
      // Volume Up.
      // If VirtualButton driver loaded
      //   use VirtualButton driver
      // Else If HID Event Driver loaded
      //   use HID Event Driver
      // Else
      //   use GFX Driver.
      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume UP
        ADBG("Notify _HID 0xC4")
        Notify(\_SB.HIDD,0xC4) // Notify HID driver that Volume UP button is pressed.
      } Else {
        If (LEqual(\_SB.HIDD.HRDY, 1)){
          \_SB.HIDD.HPEM(17)
        } Else {
          ADBG("IUEH")
          \_SB.PCI0.GFX0.IUEH(2)
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
      ADBG("Volume Down")

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume Down
        ADBG("Notify _HID 0xC6")
        Notify(\_SB.HIDD,0xC6) // Notify HID driver that Volume Down button is pressed.
      } Else {
        If (LEqual(\_SB.HIDD.HRDY, 1)){
          \_SB.HIDD.HPEM(18)
        } Else {
          ADBG("IUEH")
          \_SB.PCI0.GFX0.IUEH(3)
        }
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
      ADBG("Windows Home")

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Windows Home Press
        ADBG("Notify _HID 0xC2")
        Notify(\_SB.HIDD,0xC2) // Notify HID driver that Windows Home button is pressed.
      } Else {
        If (LEqual(\_SB.HIDD.HRDY, 1)){
          \_SB.HIDD.HPEM(1)
        } Else {
          ADBG("IUEH")
          \_SB.PCI0.GFX0.IUEH(1)
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
      ADBG("Rotation Lock P")

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: RotationLock Press
        ADBG("Notify _HID 0xC8")
        Notify(\_SB.HIDD,0xC8) // Notify HID driver that Rotation Lock button is pressed.
      } Else {
        If (LEqual(\_SB.HIDD.HRDY, 1)){
          \_SB.HIDD.HPEM(2)
        } Else {
          ADBG("IUEH")
          \_SB.PCI0.GFX0.IUEH(4)
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
      ADBG("Convertible Button")

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded. For HID event filter driver
        Xor(PB1E, 0x08, PB1E) // Toggle Slate/Laptop Lock Status
        If (And(PB1E, 0x08)) // Slave/Laptop Mode changed 0 -> 1 Laptop mode
        {
          UPBT(CONVERTIBLE_BUTTON, One)
          ADBG("Notify 0xCD")
          Notify(\_SB.HIDD, 0xCD) // Notify HID Event Filter driver that the system is now in Laptop mode.
        } Else { // Slave/Laptop Mode Changed 1 -> 0 Slate/Tablet Mode
          UPBT(CONVERTIBLE_BUTTON, Zero)
          ADBG("Notify 0xCC")
          Notify(\_SB.HIDD, 0xCC) // Notify HID Event Filter driver that the system is now in Slate/Tablet mode.
        }
      } Else {
        \_SB.PCI0.GFX0.IUEH(6) // Convertible Indicator lock
      }
    }

  }

  Method(_Q88)    // Volume Up release event
  {
    If(LEqual(\VBVP,1))
    {
      ADBG("Vol Up Release")
      // Volume Up button release event.
      // If VirtualButton driver loaded
      //   use VirtualButton driver
      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume UP Release
        ADBG("Notify _HID 0xC5")
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
      ADBG("Vol Down Release")

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Volume Down Release
        ADBG("Notify _HID 0xC7")
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
      ADBG("Win Home release")

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Windows Home Button Release
        ADBG("Notify _HID 0xC3")
        Notify(\_SB.HIDD,0xC3) // Notify HID driver that Windows Home button is released.
      }
    }
  }

  Method(_Q8B)    // Rotation Lock release event
  {
    If (LEqual(\VBRL,1)) {
      // Windows Rotation Lock release event
      ADBG("Rotation Lock R")

      If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Rotation Lock Release
        ADBG("Notify _HID 0xC9")
        Notify(\_SB.HIDD,0xC9) // Notify HID driver that Windows Home button is released.
      }
    }
  }

  Method(_QF0)    // Thermal Event.
  {
      // Only handle the numerous Thermal Events if
      // we are NOT doing ACPI Debugging.
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

    ADBG("VGBS")
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
      Return(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.LSTE)))
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
              \_SB.PCI0.LPCB.H_EC.ECMD(0x74)
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
                If (\_SB.PCI0.LPCB.PRBL) {
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

  Device(HIDD)                                       // HID Device.
  {
    Name(_HID,"INT33D5")                            // Intel Ultrabook HID Platform Event Driver.

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

    //
    // HID Button Load Method - called by Platform to say HID driver is capable of receiving 5-button array notifies.
    // Input: None
    //
    // Output: None
    //
    Method(BTNL,0,Serialized) // HID Button Enable/Disable Load Method
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
      //
      // Enable all buttons
      //
      If(LEqual(\AEAB,1)) {

        //Enable SCIs for HID driver in EC.
        Store(0x1F, BTS1)
        \_SB.PCI0.LPCB.H_EC.ECWT(BTS1, RefOf(\_SB.PCI0.LPCB.H_EC.BTEN)) //Button Enable/Disable field in ACPI name space
        \_SB.PCI0.LPCB.H_EC.ECMD(0x38) //Enable/Disable SCIs from buttons
      } Else {
        Store(0x0, BTS1)
      }
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
        \_SB.PCI0.LPCB.H_EC.ECWT(BTS1, RefOf(\_SB.PCI0.LPCB.H_EC.BTEN)) //Button Enable/Disable field in ACPI name space
        \_SB.PCI0.LPCB.H_EC.ECMD(0x38) //Enable/Disable SCIs from buttons
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
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.BTEN)), BTS1) //Button Enable/Disable field in ACPI name space
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
              Return(\_SB.PCI0.LPCB.H_EC.VGBS())
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

  Method(PWPR, 0, Serialized) // Power Button Press Helper Method
  {
    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Power Button Press
      ADBG("Notify _HID 0xCE")
      Notify(\_SB.HIDD,0xCE) // Notify HID driver that Power button is pressed.
    } Else {
      If(LEqual(\_SB.PWRB.PBST, 0x1)) { // Using Control Method Power Button
        Notify(\_SB.PWRB, 0x80) // Send press notification to Power Button device
        Store(1, \_SB.PWRB.PBLV) // Keep track of Power Button Level
      }

      If (CondRefOf(\_SB.PCI0.GFX0.IUER))
      {
        Store(\_SB.PCI0.GFX0.IUER, Local0)
        And(Local0, 0xC0, \_SB.PCI0.GFX0.IUER)  // Clear 4:0 button events on entry.
        Store(\_SB.PCI0.GFX0.IUER, Local0)
        Or(Local0, 0x01, \_SB.PCI0.GFX0.IUER)   // Set Power Button Status = pressed.
      }

    } //If Win10
  } //End Method

  Method(PWRR, 0, Serialized) // Power Button Release Helper Method
  {
    If (LAnd(LGreaterEqual(OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Power Button Release
      ADBG("Notify _HID 0xCF")
      Notify(\_SB.HIDD,0xCF) // Notify HID driver that Power button is released.
    } Else {
      If(LEqual(\_SB.PWRB.PBST, 0x1)) { // Using Control Method Power Button
        \_SB.PWRB.PBUP() // Send release notification to Power Button device if requested
        Store(0, \_SB.PWRB.PBLV) //// Keep track of Power Button Level
      }

      If (CondRefOf(\_SB.PCI0.GFX0.IUER))
      {
        Store(\_SB.PCI0.GFX0.IUER, Local0)
        And(Local0, 0xC0, \_SB.PCI0.GFX0.IUER)  // Clear 4:0 button events on entry.
      }

    } //If Win10
  } //End Method
}//end scope _SB
