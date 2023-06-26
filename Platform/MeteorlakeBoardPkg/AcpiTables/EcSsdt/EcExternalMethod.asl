/** @file
  ACPI EC SSDT table for external methods.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
// PPOE is the BIOS setting for PD PS_ON Enable
External (PPOE, IntObj)
External (\ECNO, IntObj)
External (\ECLP, IntObj)
External (UAXL, MethodObj)
External (PGED, IntObj)
Mutex (EHLD, 0) // EC Hold indicator: 0- No one accessing the EC Power State 1- Someone else is accessing the EC Power State

/**
  ERLD        Read EC Lid status.

  @param      None.
  @retval     Return the LID status: 1 - LID open, 0 - LID close.
**/
Method (ERLD)
{
  Return (ECRD (RefOf (LSTE)))
}

/**
  ERVP        Read EC virtual power state.

  @param      None.
  @retval     Return the virtual AC power: 1 - AC present, 0 - AC not present.
**/
Method (ERVP)
{
  Return (ECRD (RefOf (VPWR)))
}

/**
  ERBT        Read EC button enable/disable.

  @param      None.
  @retval     Return button enable/disable:
              bit0    - Power button N/A to disable
              bit1    - Windows button
              bit2    - volume up button
              bit3    - volume down button
              bit4    - rotation lock button
              bit5-7  - reserved
**/
Method (ERBT)
{
  Return (ECRD (RefOf (BTEN)))
}

/**
  ERAP        Read EC AC power state.

  @param      None.
  @retval     Return AC power state: 1 - AC present, 0 - AC not present.

**/
Method (ERAP)
{
  Return (ECRD (RefOf (RPWR)))
}

/**
  ERMT        Read EC max platform temprature.

  @param      None.
  @retval     Return max platform temprature value.

**/
Method (ERMT)
{
  Return (ECRD (RefOf (PLMX)))
}

/**
  ERSP        Read EC sensors value.

  @param      None.
  @retval     Return sensors value in package object:
              Index0      - TSR1 Sensor SEN1
              Index1      - TSR2 Sensor SEN2
              Index2      - TSR3 Sensor SEN3
              Index3      - TSR4 Sensor SEN4
              Index4      - TSR5 Sensor SEN5

**/
Method (ERSP)
{
  Name (TSPK, Buffer (5) {})
  Store (ECRD (RefOf (^TSR1)), Index (TSPK, 0))
  Store (ECRD (RefOf (^TSR2)), Index (TSPK, 1))
  Store (ECRD (RefOf (^TSR3)), Index (TSPK, 2))
  Store (ECRD (RefOf (^TSR4)), Index (TSPK, 3))
  Store (ECRD (RefOf (^TSR5)), Index (TSPK, 4))

  Return (TSPK)
}

/**
  ERPM        Battery 1 Pmax in mW (LSB).

  @param      None.
  @retval     Return Battery 1 Pmax in mW (LSB).

**/
Method (ERPM)
{
  Return (ECRD (RefOf (BMAX)))
}

/**
  ERPD        Read PCH DTS reading from PCH.

  @param      None.
  @retval     Return PCH DTS value from PCH.

**/
Method (ERPD)
{
  Return (ECRD (RefOf (PPDT)))
}

/**
  ERPH        Read CPU PECI reading integer value (unit Celicus).

  @param      None.
  @retval     Return CPU PECI reading integer value (unit Celicus).
**/
Method (ERPH)
{
  Return (ECRD (RefOf (PECH)))
}

/**
  ERBP        Read battery package value.

  @param      None.
  @retval     Return the battery value in package object:
              Index0        - B1DC
              Index1        - B1RC
              Index2        - B1FC
              Index3        - B1FV
              Index4        - B1DI
              Index5        - B1CI
              Index6        - B2RC - 0
              Index7        - B2FC - 0
              Index8        - B2FV - 0
              Index9        - B2DI - 0
              Index10       - B2CI - 0

**/
Method (ERBP)
{
  Name (BTPK, Package () {
    0x80000000, 0x80000000, 0x80000000, 0x80000000,
    0x80000000, 0x80000000, 0x80000000, 0x80000000,
    0x80000000, 0x80000000, 0x80000000
  })
  Store (ECRD (RefOf (B1DC)), Index (BTPK, 0))
  Store (ECRD (RefOf (^B1RC)), Index (BTPK, 1))
  Store (ECRD (RefOf (^B1FC)), Index (BTPK, 2))
  Store (ECRD (RefOf (B1FV)), Index (BTPK, 3))
  Store (ECRD (RefOf (B1DI)), Index (BTPK, 4))
  Store (ECRD (RefOf (B1CI)), Index (BTPK, 5))
  //
  // BAT2 is unsupported, return 0 for BAT2 field
  Store (0, Index (BTPK, 6))
  Store (0, Index (BTPK, 7))
  Store (0, Index (BTPK, 8))
  Store (0, Index (BTPK, 9))
  Store (0, Index (BTPK, 10))
  Return (BTPK)
}

/**
  ERBA        Read battery A maximum.

  @param      None.
  @retval     Return battery A maximum.

**/
Method (ERBA)
{
  Return (ECRD (RefOf (^BMAX)))
}

/**
  ERPN        Read PWM ending value.

  @param      None.
  @retval     Return PWM ending value.

**/
Method (ERPN)
{
  Return (ECRD (RefOf (PENV)))
}

/**
  ERXX        Read DDR fan in rpm.

  @param      None.
  @retval     Return DDR fan in rpm.

**/
Method (ERDF)
{
  Return (ECRD (RefOf (DFSP)))
}

/**
  ERXX        Read GPU fan in rpm.

  @param      None.
  @retval     Return GPU fan in rpm.

**/
Method (ERGF)
{
  Return (ECRD (RefOf (GFSP)))
}

/**
  ERCF        Read CPU fan speed.

  @param      None.
  @retval     Return CPU fan speed.

**/
Method (ERCF)
{
  Return (ECRD (RefOf (^CFSP)))
}

/**
  ERPC        Read PECI repeat pacakges.

  @param      None.
  @retval     Return PECI repeat in buffer object:
              Index0      - PRC1
              Index1      - PRC2
              Index2      - PRC3
              Index3      - PRC4
              Index4      - PRPI
              Index5      - PECN
              Index6      - PCAD
              Index7      - PEWL
              Index8      - PWRL
              Index9      - PECD
              Index10     - PEHI
              Index11     - PECI
              Index12     - PEPL
              Index13     - PEPM
              Index14     - PWFC
              Index15     - PECC
              Index16     - PDT0
              Index17     - PDT1
              Index18     - PDT2
              Index19     - PDT3
              Index20     - PRFC
              Index21     - PRS0
              Index22     - PRS1
              Index23     - PRS2
              Index24     - PRS3
              Index25     - PRS4

**/
Method (ERPC)
{
  Name (PEPK, Buffer (26) {})
  Store (ECRD (RefOf (PRC1)), Index (PEPK, 0))
  Store (ECRD (RefOf (PRC2)), Index (PEPK, 1))
  Store (ECRD (RefOf (PRC3)), Index (PEPK, 2))
  Store (ECRD (RefOf (PRC4)), Index (PEPK, 3))
  Store (ECRD (RefOf (PRPI)), Index (PEPK, 4))
  Store (ECRD (RefOf (PECN)), Index (PEPK, 5))
  Store (ECRD (RefOf (PCAD)), Index (PEPK, 6))
  Store (ECRD (RefOf (PEWL)), Index (PEPK, 7))
  Store (ECRD (RefOf (PWRL)), Index (PEPK, 8))
  Store (ECRD (RefOf (PECD)), Index (PEPK, 9))
  Store (ECRD (RefOf (PEHI)), Index (PEPK, 10))
  Store (ECRD (RefOf (PECI)), Index (PEPK, 11))
  Store (ECRD (RefOf (PEPL)), Index (PEPK, 12))
  Store (ECRD (RefOf (PEPM)), Index (PEPK, 13))
  Store (ECRD (RefOf (PWFC)), Index (PEPK, 14))
  Store (ECRD (RefOf (PECC)), Index (PEPK, 15))
  Store (ECRD (RefOf (PDT0)), Index (PEPK, 16))
  Store (ECRD (RefOf (PDT1)), Index (PEPK, 17))
  Store (ECRD (RefOf (PDT2)), Index (PEPK, 18))
  Store (ECRD (RefOf (PDT3)), Index (PEPK, 19))
  Store (ECRD (RefOf (PRFC)), Index (PEPK, 20))
  Store (ECRD (RefOf (PRS0)), Index (PEPK, 21))
  Store (ECRD (RefOf (PRS1)), Index (PEPK, 22))
  Store (ECRD (RefOf (PRS2)), Index (PEPK, 23))
  Store (ECRD (RefOf (PRS3)), Index (PEPK, 24))
  Store (ECRD (RefOf (PRS4)), Index (PEPK, 25))
  Return (PEPK)
}

/**
  ERPS        Read PECI repeat command status.

  @param      None.
  @retval     Return PECI repeat command status.

**/
Method (ERPS)
{
  Return (ECRD (RefOf (PRCS)))
}

/**
  ERPE        Read PECI error counts.

  @param      None.
  @retval     Return PECI error counts in buffer object:
              Index0      - PEC0: Peci Error Count (LSB)
              Index1      - PEC1: Peci Error Count
              Index2      - PEC2: Peci Error Count
              Index3      - PEC3: Peci Error Count (MSB)

**/
Method (ERPE)
{
  Name (PEPK, Buffer (4) {})
  Store (ECRD (RefOf (PEC0)), Index (PEPK, 0))
  Store (ECRD (RefOf (PEC1)), Index (PEPK, 1))
  Store (ECRD (RefOf (PEC2)), Index (PEPK, 2))
  Store (ECRD (RefOf (PEC3)), Index (PEPK, 3))
  Return (PEPK)
}

/**
  ERPB        Read power battery steady state.

  @param      None.
  @retval     Return power battery steady state.

**/
Method (ERPB)
{
  Return (ECRD (RefOf (^PBSS)))
}

/**
  ERBS        Read battery packages.

  @param      None.
  @retval     Return battery packages:
              Index0    - RBHF: Battery high frequency impedance
              Index1    - VBNL: Battery No-Load Voltage
              Index2    - CMPP: Battery Maximum peak current

**/
Method (ERBS)
{
  Name (BPPK, Package () {
    0x80000000, 0x80000000, 0x80000000
  })
  Store (ECRD (RefOf (^RBHF)), Index (BPPK, 0))
  Store (ECRD (RefOf (^VBNL)), Index (BPPK, 1))
  Store (ECRD (RefOf (^CMPP)), Index (BPPK, 2))
  Return (BPPK)
}

/**
  ERFC        Read fast charging enable state.

  @param      None.
  @retval     Return the enable state of fast charging.

**/
Method (ERFC)
{
  Return (ECRD (RefOf (^FCHG)))
}

/**
  ERSS        Read thermal sensor status.

  @param      None.
  @retval     Return thermal sensor status registers:
              BIT0:   SEN1
              BIT1:   SEN2
              BIT2:   SEN3
              BIT3:   SEN4
              BIT4:   SEN5

**/
Method (ERSS)
{
  Return (ECRD (RefOf (TSSR)))
}

/**
  ERPT        Read EC ram PWRT value.

  @Param      None.
  @retval     Return the PWRT value in EC ram.

**/
Method (ERPT)
{
  Return (ECRD (RefOf (^PWRT)))
}

/**
  ERWP        Read the worst case rest of platform power in (W).

  @param      None.
  @retval     Return 2 bytes value of worst case rst of platform power in (W).

**/
Method (ERWP)
{
  Return (ECRD (RefOf (^PROP)))
}

/**
  ERCT        Read the charge type.

  @param      None.
  @retval     Return 1 byte of charge type, Traditional or Hybrid.

**/
Method (ERCT)
{
  Return (ECRD (RefOf (^CTYP)))
}

/**
  ERAD        Read the AC adpter rating in 10 mW.

  @param      None.
  @retval     Return 2 bytes value of AC adapter rating in 10 mW.
              Max adapter power supported. Because of EC-space constraint
              EC can only send 2bytes=15000mW but system uses 150000mW
              hence bios will multiply this value by 10 and return.

**/
Method (ERAD)
{
  Return (ECRD (RefOf (^ARTG)))
}

/**
  ERPO        Read PPM->OPM message in and CCI indicator.

  @param      None.
  @retval     Return the PPM->OPM message in and CCI indicator in buffer object (20 bytes):
              Index0      - MGI0
              Index1      - MGI1
              Index2      - MGI2
              Index3      - MGI3
              Index4      - MGI4
              Index5      - MGI5
              Index6      - MGI6
              Index7      - MGI7
              Index8      - MGI8
              Index9      - MGI9
              Index10     - MGIA
              Index11     - MGIB
              Index12     - MGIC
              Index13     - MGID
              Index14     - MGIE
              Index15     - MGIF
              Index16     - CCI0
              Index17     - CCI1
              Index18     - CCI2
              Index19     - CCI3

**/
Method (ERPO)
{
  Name (PMBF, Buffer (20) {})
  Store (ECRD (RefOf (MGI0)), Index (PMBF, 0))
  Store (ECRD (RefOf (MGI1)), Index (PMBF, 1))
  Store (ECRD (RefOf (MGI2)), Index (PMBF, 2))
  Store (ECRD (RefOf (MGI3)), Index (PMBF, 3))
  Store (ECRD (RefOf (MGI4)), Index (PMBF, 4))
  Store (ECRD (RefOf (MGI5)), Index (PMBF, 5))
  Store (ECRD (RefOf (MGI6)), Index (PMBF, 6))
  Store (ECRD (RefOf (MGI7)), Index (PMBF, 7))
  Store (ECRD (RefOf (MGI8)), Index (PMBF, 8))
  Store (ECRD (RefOf (MGI9)), Index (PMBF, 9))
  Store (ECRD (RefOf (MGIA)), Index (PMBF, 10))
  Store (ECRD (RefOf (MGIB)), Index (PMBF, 11))
  Store (ECRD (RefOf (MGIC)), Index (PMBF, 12))
  Store (ECRD (RefOf (MGID)), Index (PMBF, 13))
  Store (ECRD (RefOf (MGIE)), Index (PMBF, 14))
  Store (ECRD (RefOf (MGIF)), Index (PMBF, 15))
  Store (ECRD (RefOf (CCI0)), Index (PMBF, 16))
  Store (ECRD (RefOf (CCI1)), Index (PMBF, 17))
  Store (ECRD (RefOf (CCI2)), Index (PMBF, 18))
  Store (ECRD (RefOf (CCI3)), Index (PMBF, 19))
  Return (PMBF)
}

//
// EC write and commands.
//

#define EC_C_UPDATE_UVTH                  0x17
#define EC_C_UPDATE_PWM                   0x1A
#define EC_C_GET_PMIC_VENDOR_ID           0x1B // Internal
#define EC_C_LOW_POWER_ENTRY              0x2C // Internal
#define EC_C_LOW_POWER_EXIT               0x2D // Internal
#define EC_C_PMIC_VOLTAGE                 0x2F // Internal
#define EC_C_SET_BAT_THRESHOLD            0x34 // Internal
#define EC_C_CHARGE_RATE_CHANGE_NOTIFY    0x37
#define EC_C_BTN_SCI_CTL                  0x38 // Internal
#define EC_C_SET_TMP_THRESHOLD            0x4A
#define EC_C_START_PECI_TEST              0x65
#define EC_C_STOP_PECI_TEST               0x66
#define EC_C_ACPI_PORT_WRT_PECI_DATA      0x67
#define EC_C_DIS_PWR_BTN_NOTIFY           0x74
#define EC_C_UCSI_CMD_HNDLR               0xE0
#define EC_C_SX_ENTER                     0xAC

/**
  STSN        Store sequence number.

  @param      Arg0  - 1 byte value to store the sequece number
  @retval     None.

**/
Method (STSN, 1, Serialized)
{
  ECWT (Arg0, ^PBOK)
  ECMD (0x15)
}

/**
  CPDC        Change power delovery capability.

  @param      Arg0  - 2 bytes value to change the power delivery capability.
  @retval     None.

**/
Method (CPDC, 1, Serialized)
{
  ECWT (Arg0, ^UVTH)
  ECMD (EC_C_UPDATE_UVTH)
}

/**
  UPFS        Update the Fan speed in PWM.

  @param      Arg0  - 1 byte value to select the PWM port (low byte).
  @param      Arg1  - 1 byte value to select the PWM port (high byte).
  @param      Arg2  - 1 byte value to change the FAN speed in percentage.
  @retval     None.

**/
Method (UPFS, 3, Serialized)
{
  ECWT (Arg0, RefOf (^PPSL))
  ECWT (Arg1, RefOf (^PPSH))
  ECWT (ERPN (), RefOf (^PINV))
  ECWT (Arg2, RefOf (^PENV))
  ECWT (100, RefOf (^PSTP))
  ECMD (EC_C_UPDATE_PWM)
}

/**
  UPCR        Update the charge rate

  @param      Arg0  - 2 bytes value to change the charge rate.
  @retval     None.

**/
Method (UPCR, 1, Serialized)
{
  ECWT (Arg0, ^CHGR)
  ECMD (EC_C_CHARGE_RATE_CHANGE_NOTIFY)
}

/**
  CBTS        Chanbe button status, enable/disable the button.

  @param      Arg0  - 1 byte state value to change the button state:
                BIT0      - Power Button
                BIT1      - Windows Button
                BIT2      - Volume Up Button
                BIT3      - Volume Down Button
                BIT4      - Rotation Lock  Button
                BIT5-7    - Reserved
  @retval     None.

**/
Method (CBTS, 1, Serialized)
{
  ECWT (Arg0, RefOf (BTEN))
  ECMD (EC_C_BTN_SCI_CTL)
}

/**
  USTP        Update the sensor trip point.

  @param      Arg0  - 1 byte value to select the thermal sensor
  @param      Arg1  - 1 byte value to select Hysteresis.
  @param      Arg2  - 1 byte value to set sensor low trip point
  @retval     None.

**/
Method (USTP, 3, Serialized)
{
  ECWT (Arg0, RefOf (^TSI))
  ECWT (Arg1, RefOf (^HYST))
  ECWT (Arg2, RefOf (^TSLT))
  ECMD (EC_C_SET_TMP_THRESHOLD)
}

/**
  SPET        Start PECI test.

  @param      None.
  @retval     None.

**/
Method (SPET)
{
  ECMD (EC_C_START_PECI_TEST)
}

/**
  EPET        Stop/End PECI test.

  @param      None.
  @param      None.

**/
Method (EPET)
{
  ECMD (EC_C_STOP_PECI_TEST)
}

/**
  WPED        Write PECI data.

  @param      Arg0  - 26 bytes buffer data to write PECI data.
                Index0    - to PRC1
                Index1    - to PRC2
                Index2    - to PRC3
                Index3    - to PRC4
                Index4    - to PRPI
                Index5    - to PECN
                Index6    - to PCAD
                Index7    - to PEWL
                Index8    - to PWRL
                Index9    - to PECD
                Index10   - to PEHI
                Index11   - to PECI
                Index12   - to PEPL
                Index13   - to PEPM
                Index14   - to PWFC
                Index15   - to PECC
                Index16   - to PDT0
                Index17   - to PDT1
                Index18   - to PDT2
                Index19   - to PDT3
                Index20   - to PRFC
                Index21   - to PRS0
                Index22   - to PRS1
                Index23   - to PRS2
                Index24   - to PRS3
                Index25   - to PRS4
  @retval     None.

**/
Method (WPED, 1, Serialized)
{
  ECWT (DeRefOf (Index (Arg0, 0)), RefOf (PRC1))
  ECWT (DeRefOf (Index (Arg0, 1)), RefOf (PRC2))
  ECWT (DeRefOf (Index (Arg0, 2)), RefOf (PRC3))
  ECWT (DeRefOf (Index (Arg0, 3)), RefOf (PRC4))
  ECWT (DeRefOf (Index (Arg0, 4)), RefOf (PRPI))
  ECWT (DeRefOf (Index (Arg0, 5)), RefOf (PECN))
  ECWT (DeRefOf (Index (Arg0, 6)), RefOf (PCAD))
  ECWT (DeRefOf (Index (Arg0, 7)), RefOf (PEWL))
  ECWT (DeRefOf (Index (Arg0, 8)), RefOf (PWRL))
  ECWT (DeRefOf (Index (Arg0, 9)), RefOf (PECD))
  ECWT (DeRefOf (Index (Arg0, 10)), RefOf (PEHI))
  ECWT (DeRefOf (Index (Arg0, 11)), RefOf (PECI))
  ECWT (DeRefOf (Index (Arg0, 12)), RefOf (PEPL))
  ECWT (DeRefOf (Index (Arg0, 13)), RefOf (PEPM))
  ECWT (DeRefOf (Index (Arg0, 14)), RefOf (PWFC))
  ECWT (DeRefOf (Index (Arg0, 15)), RefOf (PECC))
  ECWT (DeRefOf (Index (Arg0, 16)), RefOf (PDT0))
  ECWT (DeRefOf (Index (Arg0, 17)), RefOf (PDT1))
  ECWT (DeRefOf (Index (Arg0, 18)), RefOf (PDT2))
  ECWT (DeRefOf (Index (Arg0, 19)), RefOf (PDT3))
  ECWT (DeRefOf (Index (Arg0, 20)), RefOf (PRFC))
  ECWT (DeRefOf (Index (Arg0, 21)), RefOf (PRS0))
  ECWT (DeRefOf (Index (Arg0, 22)), RefOf (PRS1))
  ECWT (DeRefOf (Index (Arg0, 23)), RefOf (PRS2))
  ECWT (DeRefOf (Index (Arg0, 24)), RefOf (PRS3))
  ECWT (DeRefOf (Index (Arg0, 25)), RefOf (PRS4))
  ECMD (EC_C_ACPI_PORT_WRT_PECI_DATA)
}

/**
  DPBN      Disable power button notify.

  @param    None.
  @retval   None.

**/
Method (DPBN)
{
  ECMD (EC_C_DIS_PWR_BTN_NOTIFY)
}

/**
  WOPM      Write OPM to EC.

  @param    Arg0  - 24 bytes OPM data to write to EC.
              Index0    - MGO0
              Index1    - MGO1
              Index2    - MGO2
              Index3    - MGO3
              Index4    - MGO4
              Index5    - MGO5
              Index6    - MGO6
              Index7    - MGO7
              Index8    - MGO8
              Index9    - MGO9
              Index10   - MGOA
              Index11   - MGOB
              Index12   - MGOC
              Index13   - MGOD
              Index14   - MGOE
              Index15   - MGOF
              Index16   - CTL0
              Index17   - CTL1
              Index18   - CTL2
              Index19   - CTL3
              Index20   - CTL4
              Index21   - CTL5
              Index22   - CTL6
              Index23   - CTL7
  @retval   None.

**/
Method (WOPM, 1, Serialized)
{
  ECWT (DeRefOf (Index (Arg0, 0)), RefOf (MGO0))
  ECWT (DeRefOf (Index (Arg0, 1)), RefOf (MGO1))
  ECWT (DeRefOf (Index (Arg0, 2)), RefOf (MGO2))
  ECWT (DeRefOf (Index (Arg0, 3)), RefOf (MGO3))
  ECWT (DeRefOf (Index (Arg0, 4)), RefOf (MGO4))
  ECWT (DeRefOf (Index (Arg0, 5)), RefOf (MGO5))
  ECWT (DeRefOf (Index (Arg0, 6)), RefOf (MGO6))
  ECWT (DeRefOf (Index (Arg0, 7)), RefOf (MGO7))
  ECWT (DeRefOf (Index (Arg0, 8)), RefOf (MGO8))
  ECWT (DeRefOf (Index (Arg0, 9)), RefOf (MGO9))
  ECWT (DeRefOf (Index (Arg0, 10)), RefOf (MGOA))
  ECWT (DeRefOf (Index (Arg0, 11)), RefOf (MGOB))
  ECWT (DeRefOf (Index (Arg0, 12)), RefOf (MGOC))
  ECWT (DeRefOf (Index (Arg0, 13)), RefOf (MGOD))
  ECWT (DeRefOf (Index (Arg0, 14)), RefOf (MGOE))
  ECWT (DeRefOf (Index (Arg0, 15)), RefOf (MGOF))
  ECWT (DeRefOf (Index (Arg0, 16)), RefOf (CTL0))
  ECWT (DeRefOf (Index (Arg0, 17)), RefOf (CTL1))
  ECWT (DeRefOf (Index (Arg0, 18)), RefOf (CTL2))
  ECWT (DeRefOf (Index (Arg0, 19)), RefOf (CTL3))
  ECWT (DeRefOf (Index (Arg0, 20)), RefOf (CTL4))
  ECWT (DeRefOf (Index (Arg0, 21)), RefOf (CTL5))
  ECWT (DeRefOf (Index (Arg0, 22)), RefOf (CTL6))
  ECWT (DeRefOf (Index (Arg0, 23)), RefOf (CTL7))
  ECMD (EC_C_UCSI_CMD_HNDLR)
}

/**
  Update Timer value in EC mailbox.

  @param  Arg0  - Timer mode
  @param  Arg1  - TimerValue
  @retval None.
**/
Method (UTEC, 2)
{
     If(LNotEqual(Arg1,0xFFFFFFFF)) {
        \_SB.PC00.LPCB.H_EC.ECWT(Arg0, RefOf(\_SB.PC00.LPCB.H_EC.WTMM))
        \_SB.PC00.LPCB.H_EC.ECWT(Arg1, RefOf(\_SB.PC00.LPCB.H_EC.WTMV))
        \_SB.PC00.LPCB.H_EC.ECMD(0x2F)
     }
}

/**
  Cleaning EC PG3 entry/exit bit witn PGED variable enable and battery connected.


**/
Method (PGER, 0, Serialized)
{
  If (LEqual (PGED, 1)) {
    If (LEqual (ECRD (RefOf (RPWR)), 0)) {
      ECWT (0, RefOf (PG3E))
      ECWT (0, RefOf (PG3X))
    }
  }
}
// ECNT (Embedded Controller Notify)
//
// Handle all commands sent to EC by BIOS
//
//  Arguments: (1)
//    Arg0  -4 = Sx Entry Notify
//           3 = Idle Resiliency Entry Notify
//           2 = Idle Resiliency Exit Notify
//           1 = CS Entry Notify
//           0 = CS Exit Notify
//  Return Value:
//    0x00 = Success
//    0xFF = Failure
//
Method (ECNT, 1, Serialized) {
  Switch (ToInteger (Arg0)) {
    Case (0) { // exit CS
      D8XH (0, 0xC5)
      D8XH (1, 0xAA)
      if (LEqual (ECNO, 0x01)) { // Check EC notification enabled in Setup
        Store (Acquire (EHLD, 0xFFFF), Local0) // Wait for Mutex for telling EC to exit Low Power Mode
        if (LEqual (Local0, Zero)) {
          ECMD (0x2D) // Notify EC of CS exit
          If (LEqual (ECLP, 0x1)) {
            Store (One, ECUP)
          }
          Release (EHLD)
        }
      }
      Return (0x00)
    }

    Case (1) { // enter CS
      D8XH (0, 0xC5)
      D8XH (1, 0x01)
      if (LEqual (ECNO, 0x01)) { // Check EC notification enabled in Setup
        ECMD (0x2C) // Notify EC of CS entry
        If (LEqual (ECLP, 0x1)) {
          Store (Zero, ECUP)
        }

      if (LEqual (PPOE, 1)) {
           Store (0, Local0) // Counter
           Store (ECRD (RefOf (EXAC)), Local1)  // Local 1 = EXAC
           While (LEqual (Local1, 0)) {
             If (Lgreater (Local0, 20)) {
               Break
             }
             Sleep (10)
             Increment (Local0) // Increment Timeout.
             Store (ECRD (RefOf (EXAC)), Local1)
           }
           if (LEqual (Local1, 1)) {
             Store (ECRD (RefOf (SCIP)), Local2) // Local2 = SCIP
             if (LEqual (Local2, 1)) {
               UAXL ()
             }
           }
         }
      }
      Return (0x00)
    }

    Case (4) { // enter Sx
        if (LEqual (PPOE, 1)) {
          \_SB.PC00.LPCB.H_EC.ECMD (EC_C_SX_ENTER) // Notify EC of Sx entry
          if (LEqual (ECLP, 0x1)) {
            Store (Zero, ECUP)
          }
          Store (0, Local0) // Counter
          Store (ECRD (RefOf (EXAC)), Local1)  // Local 1 = EXAC
          While (LEqual (Local1, 0)) {
          if (Lgreater (Local0, 20)) {
            Break
          }
          Sleep (10)
          Increment (Local0) // Increment Timeout.
          Store (ECRD (RefOf (EXAC)), Local1)
        }
      }
      Return (0x00)
    }

    Default {
      Return (0xFF)  // Error invalid argument
    }
  }
}

//
// EC write to update the EC ram.
//
/**
  UPCF      Update the cpu fan status in EC ram.

  @param    Arg0  - 1 byte status value to update the cpu fan in EC ram.
              1 - Fan on
              0 - Fan off
  @retval   None.

**/
Method (UPCF, 1, Serialized)
{
  ECWT (Arg0, RefOf (CFAN))
}

/**
  UPTS      Update the thermal sensor status in EC ram.

  @param    Arg0  - 2 bytes value to update the thermal sensor status register in EC ram:
              BIT0:   SEN1
              BIT1:   SEN2
              BIT2:   SEN3
              BIT3:   SEN4
              BIT4:   SEN5
  @retval   None.

**/
Method (UPTS, 1, Serialized)
{
  ECWT (Arg0, RefOf (TSSR))
}
