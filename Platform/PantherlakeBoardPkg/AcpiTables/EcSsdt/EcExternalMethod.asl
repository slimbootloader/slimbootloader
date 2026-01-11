/** @file
  ACPI EC SSDT table for external methods.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <EcCommands.h>

// PPOE is the BIOS setting for PD PS_ON Enable
External (PPOE, IntObj)
External (UCMS, IntObj)
External (UAXL, MethodObj)
External (\_SB.PC00.LPCB.NRTS, MethodObj)
Mutex (HLDI, 0) // Hold indicator: 0- No one accessing the Power State 1- Someone else is accessing the Power State

/**
  RLDS        Get Lid status.

  @param      None.
  @retval     Return the LID status: 1 - LID open, 0 - LID close.
**/
Method (RLDS)
{
  Return (ECRD (RefOf (LSTE)))
}

/**
  RVPS        Get virtual power state.

  @param      None.
  @retval     Return the virtual AC power: 1 - AC present, 0 - AC not present.
**/
Method (RVPS)
{
  Return (ECRD (RefOf (VPWR)))
}

/**
  RBED        Get button enable/disable.

  @param      None.
  @retval     Return button enable/disable:
              bit0    - Power button N/A to disable
              bit1    - Windows button
              bit2    - volume up button
              bit3    - volume down button
              bit4    - rotation lock button
              bit5-7  - reserved
**/
Method (RBED)
{
  Return (ECRD (RefOf (BTEN)))
}

/**
  RAPS        Get AC power state.

  @param      None.
  @retval     Return AC power state: 1 - AC present, 0 - AC not present.

**/
Method (RAPS)
{
  Return (ECRD (RefOf (RPWR)))
}

/**
  RMPT        Get max platform temprature.

  @param      None.
  @retval     Return max platform temprature value.

**/
Method (RMPT)
{
  Return (ECRD (RefOf (PLMX)))
}

/**
  RSSV        Get sensors value.

  @param      None.
  @retval     Return sensors value in package object:
              Index0      - TSR1 Sensor SEN1
              Index1      - TSR2 Sensor SEN2
              Index2      - TSR3 Sensor SEN3
              Index3      - TSR4 Sensor SEN4
              Index4      - TSR5 Sensor SEN5

**/
Method (RSSV)
{
  Name (TSPK, Package (5) {0, 0, 0, 0, 0})
  Store (ECRD (RefOf (^TSR1)), Index (TSPK, 0))
  Store (ECRD (RefOf (^TSR2)), Index (TSPK, 1))
  Store (ECRD (RefOf (^TSR3)), Index (TSPK, 2))
  Store (ECRD (RefOf (^TSR4)), Index (TSPK, 3))
  Store (ECRD (RefOf (^TSR5)), Index (TSPK, 4))

  Return (TSPK)
}

/**
  RBPM        Battery 1 Pmax in mW (LSB).

  @param      None.
  @retval     Return Battery 1 Pmax in mW (LSB).

**/
Method (RBPM)
{
  Return (ECRD (RefOf (BMAX)))
}

/**
  RPDP        Get PCH DTS reading from PCH.

  @param      None.
  @retval     Return PCH DTS value from PCH.

**/
Method (RPDP)
{
  Return (ECRD (RefOf (PPDT)))
}

/**
  RCPH        Get CPU PECI reading integer value (unit Celicus).

  @param      None.
  @retval     Return CPU PECI reading integer value (unit Celicus).
**/
Method (RCPH)
{
  Return (ECRD (RefOf (PECH)))
}

/**
  RBPV        Get battery package value.

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
Method (RBPV)
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
  RPNV        Get PWM ending value.

  @param      None.
  @retval     Return PWM ending value.

**/
Method (RPNV)
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
  RCFS        Get CPU fan speed.

  @param      None.
  @retval     Return CPU fan speed.

**/
Method (RCFS)
{
  Return (ECRD (RefOf (^CFSP)))
}

/**
  RPRC        Get PECI repeat pacakges.

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
Method (RPRC)
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
  RPCS         Get PECI repeat command status.

  @param      None.
  @retval     Return PECI repeat command status.

**/
Method (RPCS )
{
  Return (ECRD (RefOf (PRCS)))
}

/**
  RPES        Get PECI error counts.

  @param      None.
  @retval     Return PECI error counts in buffer object:
              Index0      - PEC0: Peci Error Count (LSB)
              Index1      - PEC1: Peci Error Count
              Index2      - PEC2: Peci Error Count
              Index3      - PEC3: Peci Error Count (MSB)

**/
Method (RPES)
{
  Name (PEPK, Buffer (4) {})
  Store (ECRD (RefOf (PEC0)), Index (PEPK, 0))
  Store (ECRD (RefOf (PEC1)), Index (PEPK, 1))
  Store (ECRD (RefOf (PEC2)), Index (PEPK, 2))
  Store (ECRD (RefOf (PEC3)), Index (PEPK, 3))
  Return (PEPK)
}

/**
  RPBS        Get power battery steady state.

  @param      None.
  @retval     Return power battery steady state.

**/
Method (RPBS)
{
  Return (ECRD (RefOf (^PBSS)))
}

/**
  BHFI        Get Battery high frequency impedance

  @param      None.
  @retval     Return Battery high frequency impedance.

**/
Method (BHFI)
{
  Return (ECRD (RefOf (^RBHF)))
}

/**
  BNLV        Get Battery No-Load Voltage.

  @param      None.
  @retval     Return Battery No-Load Voltage.

**/
Method (BNLV)
{
  Return (ECRD (RefOf (^VBNL)))
}

/**
  BMPC        Get Battery Maximum peak current.

  @param      None.
  @retval     Return Battery Maximum peak current.

**/
Method (BMPC)
{
  Return (ECRD (RefOf (^CMPP)))
}

/**
  RFCS        Get fast charging enable state.

  @param      None.
  @retval     Return the enable state of fast charging.

**/
Method (RFCS)
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
  RRPT        Get ram PWRT value.

  @Param      None.
  @retval     Return the PWRT value in ram.

**/
Method (RRPT)
{
  Return (ECRD (RefOf (^PWRT)))
}

/**
  RWPP        Get the worst case rest of platform power in (W).

  @param      None.
  @retval     Return 2 bytes value of worst case rst of platform power in (W).

**/
Method (RWPP)
{
  Return (ECRD (RefOf (^PROP)))
}

/**
  RCTP        Get the charge type.

  @param      None.
  @retval     Return 1 byte of charge type, Traditional or Hybrid.

**/
Method (RCTP)
{
  Return (ECRD (RefOf (^CTYP)))
}

/**
  RADR        Get the AC adpter rating in 10 mW.

  @param      None.
  @retval     Return 2 bytes value of AC adapter rating in 10 mW.
              Max adapter power supported. Because of space constraint
              can only send 2bytes=15000mW but system uses 150000mW
              hence bios will multiply this value by 10 and return.

**/
Method (RADR)
{
  Return (ECRD (RefOf (^ARTG)))
}

/**
  RPOI        Get PPM->OPM message in and CCI indicator.

  @param      None.
  @retval     Return the PPM->OPM message in and CCI indicator in buffer object (24 bytes):
              Index0      - MI00
              Index1      - MI01
              Index2      - MI02
              Index3      - MI03
              Index4      - MI04
              Index5      - MI05
              Index6      - MI06
              Index7      - MI07
              Index8      - MI08
              Index9      - MI09
              Index10     - MI0A
              Index11     - MI0B
              Index12     - MI0C
              Index13     - MI0D
              Index14     - MI0E
              Index15     - MI0F
              Index16     - MI10
              Index17     - MI11
              Index18     - MI12
              Index19     - MI13
              Index20     - CCI0
              Index21     - CCI1
              Index22     - CCI2
              Index23     - CCI3

**/
Method (RPOI)
{
  Name (PMBF, Buffer (24) {})
  Store (ECRD (RefOf (MI00)), Index (PMBF, 0))
  Store (ECRD (RefOf (MI01)), Index (PMBF, 1))
  Store (ECRD (RefOf (MI02)), Index (PMBF, 2))
  Store (ECRD (RefOf (MI03)), Index (PMBF, 3))
  Store (ECRD (RefOf (MI04)), Index (PMBF, 4))
  Store (ECRD (RefOf (MI05)), Index (PMBF, 5))
  Store (ECRD (RefOf (MI06)), Index (PMBF, 6))
  Store (ECRD (RefOf (MI07)), Index (PMBF, 7))
  Store (ECRD (RefOf (MI08)), Index (PMBF, 8))
  Store (ECRD (RefOf (MI09)), Index (PMBF, 9))
  Store (ECRD (RefOf (MI0A)), Index (PMBF, 10))
  Store (ECRD (RefOf (MI0B)), Index (PMBF, 11))
  Store (ECRD (RefOf (MI0C)), Index (PMBF, 12))
  Store (ECRD (RefOf (MI0D)), Index (PMBF, 13))
  Store (ECRD (RefOf (MI0E)), Index (PMBF, 14))
  Store (ECRD (RefOf (MI0F)), Index (PMBF, 15))
  If (CondRefOf (UCMS)) {
    If (LEqual (UCMS, 1)) {
      Store (ECRD (RefOf (CCI0)), Index (PMBF, 16))
      Store (ECRD (RefOf (CCI1)), Index (PMBF, 17))
      Store (ECRD (RefOf (CCI2)), Index (PMBF, 18))
      Store (ECRD (RefOf (CCI3)), Index (PMBF, 19))
      Return (PMBF) // UCSI 1.2
    }
  }
  Store (ECRD (RefOf (MI10)), Index (PMBF, 16))
  Store (ECRD (RefOf (MI11)), Index (PMBF, 17))
  Store (ECRD (RefOf (MI12)), Index (PMBF, 18))
  Store (ECRD (RefOf (MI13)), Index (PMBF, 19))
  Store (ECRD (RefOf (CCI0)), Index (PMBF, 20))
  Store (ECRD (RefOf (CCI1)), Index (PMBF, 21))
  Store (ECRD (RefOf (CCI2)), Index (PMBF, 22))
  Store (ECRD (RefOf (CCI3)), Index (PMBF, 23))
  Return (PMBF) // UCSI 2.x
}

/**
  ERB1        Read the Battery1 state value from EC RAM.
  @param      None.
  @retval     Return Battery 1 state:
              bit2:0    - the battery state defined in ACPI spec for _BST.
              bit3      - the battery 1 device presentation state.
**/
Method (ERB1)
{
  Return (ECRD (RefOf (B1ST)))
}

/**
  RSFC        Get Supported Fan Capability.

  @param      None.
  @retval     Return the supported fan capability.
**/
Method (RSFC)
{
  Return (ECRD (RefOf (^CFCS)))
}

/**
  RFPM        Read Fan Policy Mode.

  @param      None.
  @retval     Return currecnt fan policy mode.
**/
Method (RFPM)
{
  Return (ECRD (RefOf (^CFPM)))
}

/**
  RFTH        Get fan speed threshold in RPM.

  @param      None.
  @retval     Return    FAN RPM high threshold
**/
Method (RFTH)
{
  Return (ECRD (RefOf (^FSTH)))
}

/**
  RFTL        Get fan speed threshold in RPM.

  @param      None.
  @retval     Return    FAN RPM low threshold.
**/
Method (RFTL)
{
  Return (ECRD (RefOf (^FSTL)))
}

/**
  RSTH        Get fan speed threshold Hysterisis.

  @praram     None.
  @retvale    Return    Fan speed threshold Hysterisis.
**/
Method (RSTH)
{
  Return (ECRD (RefOf (^FSHS)))
}

//
// write and commands.
//
/**
  STSN        Store sequence number.

  @param      Arg0  - 1 byte value to store the sequece number
  @retval     None.

**/
Method (STSN, 1, Serialized)
{
  ECWT (Arg0, RefOf (^PBOK))
  ECMD (EC_C_PWR_BOSS_OK)
}

/**
  UPFH        Update the fan speed threshold in RPM.
  @param      Arg0  - conveys to the platform the fan speed High threshold to be set.
  @retval     None.
**/
Method (UPFH, 1, Serialized)
{
  ECWT (Arg0, RefOf (^FSTH))
  ECMD (EC_C_UPDATE_RPM_TRIP_THRESHOLDS)
}

/**
  UPFL        Update the fan speed threshold in RPM.
  @param      Arg0  - conveys to the platform the fan speed Low threshold to be set.
  @retval     None.
**/
Method (UPFL, 1, Serialized)
{
  ECWT (Arg0, RefOf (^FSTL))
  ECMD (EC_C_UPDATE_RPM_TRIP_THRESHOLDS)
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

//
// Local variable under H_EC scope to optimize the PWM updating.
// FNAL - Fan setting already set.
// FANI - Fan Index record.
// LSFS - Last fan speed record.
// RQFS - Requested fan speed value.
//
Name (FNAL, 0)
Name (FANI, 0)
Name (LSFS, 0)
Name (RQFS, 0)
/**
  UPFS        Update the Fan speed in PWM.

  @param      Arg0  - 1 byte value to select the PWM port (low byte).
  @param      Arg1  - 1 byte value to change the FAN speed in percentage.
  @retval     None.

**/
Method (UPFS, 2, Serialized)
{
  If (LEqual (FNAL, 0)) {
    Store (1, FNAL)
    Store (Arg0, FANI)
    Store (RPNV (), LSFS)
    Store (Arg1, RQFS)
    ECWT (Arg0, RefOf (^PPSL))
    ECWT (LSFS, RefOf (^PINV))
    ECWT (Arg1, RefOf (^PENV))
    ECWT (100, RefOf (^PSTP))
    ADBG (Concatenate ("Update fan PWM Index:", ToHexString (Arg0)))
    ADBG (Concatenate ("Update fan PWM Speed:", ToHexString (Arg1)))
    ECMD (EC_C_UPDATE_PWM)
  } Else {
    Store (0, Local0)   // Represent the fan ID change: 1 - fan id changed, 0 - fan id not changed.
    //
    // Update Fan Index if required.
    //
    If (LNotEqual (Arg0, FANI)) {
      ADBG (Concatenate ("Fan Id change to ", ToHexString (Arg0)))
      Store (1, Local0)
      Store (Arg0, FANI)
      ECWT (Arg0, RefOf (^PPSL))
    }
    //
    // Update PWM initial value if required.
    // RQFS now is not update, it is still the value of last request.
    //
    If (LNotEqual (RQFS, LSFS)) {
      ADBG (Concatenate ("Pre Fan PWM Update to ", ToHexString (RQFS)))
      Store (RQFS, LSFS)
      ECWT (RQFS, RefOf (^PINV))
    }
    //
    // Update the PMW and send Ec command if new PWM requested.
    // Update the RQFS to the latest request.
    //
    If (LOr (LNotEqual (Arg1, RQFS), LEqual (Local0, 1))) {
      ADBG (Concatenate ("Update fan speed to ", ToHexString (Arg1)))
      Store (Arg1, RQFS)
      ECWT (Arg1, RefOf (^PENV))
      ECMD (EC_C_UPDATE_PWM)
    }
  }
}

/**
  UPF2        Update the Fan speed in PWM.

  @param      Arg0  - 1 byte value to select the PWM port (low byte).
  @param      Arg1  - 1 byte value to change the FAN speed in percentage.
  @param      Arg2  - 1 byte value to update the previous speed level.
  @retval     None.

**/
Method (UPF2, 3, Serialized)
{
  ECWT (Arg0, RefOf (^PPSL))
  ECWT (Arg2, RefOf (^PINV))
  ECWT (Arg1, RefOf (^PENV))
  ECWT (100, RefOf (^PSTP))
  ECMD (EC_C_UPDATE_PWM)
}

/**
  UPFP        Update Fan Policy Mode.

  @param      Arg0  - 1 byte value to update CFPM
  @retval     None.
**/
Method (UPFP, 1, Serialized)
{
  Store (ECRD (RefOf (^CFCS)), Local0)
  If (And (Local0, 0x2)) {
    ECWT (Arg0, RefOf (^CFPM))
    ECMD (EC_C_SET_FAN_POLICY_MODE)
  }
}

/**
  UPCR        Update the charge rate

  @param      Arg0  - 2 bytes value to change the charge rate.
  @retval     None.

**/
Method (UPCR, 1, Serialized)
{
  ECWT (Arg0, RefOf (^CHGR))
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
  @param      Arg3  - Input to indicate whether it is Low trip point or High trip point
                       0 - Low trip point
                       1 - High trip point
  @retval     None.

**/
Method (USTP, 4, Serialized)
{
  ECWT (Arg0, RefOf (^TSI))
  ECWT (Arg2, RefOf (^HYST))
  If (LEqual (Arg3, 0x00)) {
    ECWT (Arg1, RefOf (^TSLT)) // Sensor Low Trip Point
  } Else {
    ECWT (Arg1, RefOf (^TSHT)) // Sensor High Trip Point
  }
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
  WOPM      Write to ECOPM.

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
    ECWT (Arg0, RefOf(^WTMM))
    ECWT (Arg1, RefOf(^WTMV))
    ECMD (EC_C_P_G3_PROGRAM_COUNTER)
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
//    Arg0  -5 = Sx Exit Notify
//           4 = Sx Entry Notify
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
      ADBG ("EC Exit CS")
      if (LEqual (ECNO, 0x01)) { // Check EC notification enabled in Setup
        ADBG ("EC Notify")
        Store (Acquire (HLDI, 0xFFFF), Local0) // Wait for Mutex for telling to exit Low Power Mode
        if (LEqual (Local0, Zero)) {
          ECMD (EC_C_LOW_POWER_EXIT) // Notify EC of CS exit
          If (LEqual (ECLP, 0x1)) {
            Store (One, ECUP)
          }
          Release (HLDI)
        }
      }
      Return (0x00)
    }

    Case (1) { // enter CS
      D8XH (0, 0xC5)
      D8XH (1, 0x01)
      ADBG ("Enter CS")

      if (LEqual (ECNO, 0x01)) { // Check EC notification enabled in Setup
        ADBG ("EC Notify")
        ECMD (EC_C_LOW_POWER_ENTRY) // Notify EC of CS entry
        If (LEqual (ECLP, 0x1)) {
          Store (Zero, ECUP)
        }

        If (CondRefOf (PPOE)) {
          If (LOr (LEqual (PPOE, 1), LEqual (PPOE, 2))) {
            Store (0, Local0) // Counter
            Store (ECRD (RefOf (EXAC)), Local1)  // Local 1 = EXAC
            While (LEqual (Local1, 0)) {
              ADBG (Concatenate ("EXAC", ToHexString (Local1)))
              If (Lgreater (Local0, 20)) {
                ADBG ("EC PS_ON ACK timeout")
                Break
              }
              Sleep (10)
              Increment (Local0) // Increment Timeout.
              Store (ECRD (RefOf (EXAC)), Local1)
            }
            If (LEqual (Local1, 1)) {
              Store (ECRD (RefOf (SCIP)), Local2) // Local2 = SCIP
              If (LEqual (Local2, 1)) {
                ADBG (Concatenate ("SCIP", ToHexString (Local2)))
                ADBG ("Checking pending PS_ON limit update")
                UAXL ()
              }
            }
          }
        }
      }
      Return (0x00)
    }

    Case (4) { // enter Sx
      ADBG ("EC Enter Sx")

      If (CondRefOf (PPOE)) {
        If (LEqual (PPOE, 2)) {
          If (CondRefOf (\_SB.PC00.LPCB.NRTS)) {
            \_SB.PC00.LPCB.NRTS (0)
          }
        }

        If (LOr (LEqual (PPOE, 1), LEqual (PPOE, 2))) {

          ADBG ("EC Notify")
          \_SB.PARENT_OF_LPCB.LPCB.H_EC.ECMD (EC_C_SX_ENTER) // Notify EC of Sx entry
          If (LEqual (ECLP, 0x1)) {
            Store (Zero, ECUP)
          }
          Store (0, Local0) // Counter
          Store (ECRD (RefOf (EXAC)), Local1)  // Local 1 = EXAC
          While (LEqual (Local1, 0)) {
            ADBG (Concatenate ("EXAC", ToHexString (Local1)))
            If (Lgreater (Local0, 20)) {
              ADBG ("EC PS_ON ACK timeout")
              Break
            }
            Sleep (10)
            Increment (Local0) // Increment Timeout.
            Store (ECRD (RefOf (EXAC)), Local1)
          }
        }
      }
      Return (0x00)
    }

    Case (5) { // exit Sx
      ADBG ("EC Exit Sx")

      If (CondRefOf (PPOE)) {
        If (LOr (LEqual (PPOE, 1), LEqual (PPOE, 2))) {
          ADBG ("EC Notify")
          \_SB.PARENT_OF_LPCB.LPCB.H_EC.ECMD (EC_C_SX_EXIT) // Notify EC of Sx Exit
          If (LEqual (ECLP, 0x1)) {
            Store (Zero, ECUP)
          }
        }

        If (LEqual (PPOE, 2)) {
          If (CondRefOf (\_SB.PC00.LPCB.NRTS)) {
            \_SB.PC00.LPCB.NRTS (1)
          }
        }
      }
      Return (0x00)
    }

    Default {
      Return (0xFF)  // Error invalid argument
    }
  }
}

/**
  UPPM          Update the Programable Power Supply Mode.

  @param        1 byte data represent the PPS mode.
  @retval       0x0           - Success.
  @retval       None.
**/
Method (UPPM, 1, Serialized)
{
  ECWT (Arg0, RefOf (^PPSM))
  ECMD (EC_C_SET_PROGRAMMABLE_POWER_SUPPLY_MODE)
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

/**
  UPLD      Update the Lid status in EC ram.

  @param    Arg0  - 1 byte date to update the Lid status.
  @retval   None.

**/
Method (UPLD, 1, Serialized)
{
  ECWT (Arg0, RefOf (^LSTE))
}