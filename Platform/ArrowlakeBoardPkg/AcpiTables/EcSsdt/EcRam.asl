/** @file
  ACPI EC SSDT table for EC ram data region

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

OperationRegion (ECF2, EmbeddedControl, 0, 0xFF)
Field (ECF2, ByteAcc, Lock, Preserve)
{
Offset(1),
PLMX, 8,      // 1          Max Platform temprature
LTMP, 8,      // 2          Sensor Temperature
RPWR, 1,      // 3.0        Real AC Power (AC Present = 1)
    , 2,      // 3.2:1      Reserved
CFAN, 1,      // 3.3        CPU Fan (Fan On = 1)
    , 2,      // 3.5:4      Reserved
LSTE, 1,      // 3.6        Lid State (Lid Open = 1)
    , 1,      // 3.7        Reserved
MGI0, 8,      // 4          PPM->OPM Message In, 128 bits, 16 bytes
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
MGO0, 8,      // 20         OPM->PPM Message Out, 128 bits, 16 bytes
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
CCI0, 8,      // 36         PPM->OPM CCI indicator, 4 bytes,
CCI1, 8,
CCI2, 8,
CCI3, 8,
Offset(47),
CTMP, 8,      // 47         EC Critical Temperature
    , 1,      // 48.0       Reserved
    , 2,      // 48.2:1     Reserved
PBNS, 1,      // 48.3       Power Button State (0 = Pressed)
VPWR, 1,      // 48.4       Virtual AC Power (AC Present = 1)
    , 3,      // 48.7:5     Reserved
SCAN, 8,      // 49         Hot-Key Scan Code
B1ST, 8,      // 50         Battery 1 Status
Offset(54),
B2ST, 8,      // 54         Battery 2 Status
Offset(58),
CMDR, 8,      // 58         KSC command register
Offset(64),
CFCS, 8,      // 64         Fan capability supported
PPSL, 8,      // 65         PWM Port Select low byte
PPSM, 8,      // 66         Programmable Power Supply Mode
PINV, 8,      // 67         PWM initial value
PENV, 8,      // 68         PWM ending value
PSTP, 8,      // 69         PWM ms delay between steps
CFPM, 8,      // 70         Current fan policy mode
Offset(71),
FSTH, 16,     // 71,72      Fan speed high threshold in RPM
FSTL, 16,     // 73,74      Fan speed low threshold in RPM
FRTS, 8,      // 75         FRTS- Fan RPM trip status register:
              //            Responsible for triggering _QF2:
              //              BIT0:  TFN1
              //              BIT1:  TFN2
              //              BIT2:  TFN3
Offset(78),
PWRT, 8,      // 78         Power Source Type
              //              BIT0:  AC
              //              BIT1:  USB-PD
              //              BIT2:  Programmable Power Supply
PBOK, 8,      // 79         PBOK offset where BIOS will update the Power Delivery State Change Sequence Number
Offset(80),
TSI,  4,      // 80         TemSnsrSelct Select Thermal Sensor
              //              # 0 = SEN1
              //              # 1 = SEN2
              //              # 2 = SEN3
              //              # 3 = SEN4
              //              # 4 = SEN5
              //              # 5 = SEN6
              //              # 6 = SEN7
              //              # 7 = SEN8
              //              # 8 = SEN9
              //              # 9 = SEN10
              //              # A = SEN11
              //              # B = SEN12
              //              # C = SEN13
              //              # D = SEN14
              //              # E = SEN15
              //              # F = SEN16
HYST, 4,      //            Hysteresis selection is global and meant for all sensors
TSHT, 16,     // 81, 82     TempThrshldH Sensor High Trip Point
TSLT, 16,     // 83, 84     TempThrshldL Sensor Low Trip Point
Offset(85),
TSSR, 16,     // 85, 86     TSSR- thermal sensor status register:
              //              Responsible for triggering _QF1:
              //                BIT0:  SEN1
              //                BIT1:  SEN2
              //                BIT2:  SEN3
              //                BIT3:  SEN4
              //                BIT4:  SEN5
Offset(87),
B1DC, 16,     // 87         Battery 1 Design Capacity (mAh)
B1RC, 16,     // 89         Battery 1 Remaining Capacity (mAh)
B1FC, 16,     // 91         Battery 1 Full Charge Capacity (mAh)
B1FV, 16,     // 93         Battery 1 Full Resolution Voltage (mV)
B1DI, 16,     // 95         Battery 1 Full Resolution Discharge Current (mA)
B1CI, 16,     // 97         Battery 1 Full Resolution Charge Current (mA)
Offset(109),
VMIN, 16,     // 109, 110   Min voltage below which platform activates OC protection and shuts down (mV).
PROP, 16,     // 111, 112   Worst case rest of platform power in (W).
DFSP, 16,     // 113, 114   DDR Fan speed in rpm
CFSP, 16,     // 115, 116   CPU Fan speed in rpm
GFSP, 16,     // 117, 118   GPU Fan speed in rpm
Offset(120),  // (0x78)
DLED, 1,      // 120.0      EC Connected Standby Debug LED (CAPS LOCK)
    , 1,      // 120.1      Reserved
    , 1,      // 120.2      Reserved
PG3X, 1,      // 120.3      BIT3: set to indicated PG3 exit after counter expiry.
PG3E, 1,      // 120.4      BIT4: set to indicate PG3 entered successfully.
Offset(122),
AVOL, 16,     // 122, 123   Returns the AC source nominal voltage in (mV).
Offset(125),
ACUR, 16,     // 125, 126   Returns the AC source operational current in (mA).
Offset(129),
PPDT, 8,      // 129        PCH DTS reading from PCH
PECL, 8,      // 130        CPU PECI reading fractional value (1/64 Celcius)
PECH, 8,      // 131        CPU PECI reading integer value (unit Celicus)
Offset(134),
LSOC, 8,      // 134        Long term battery charge is based on c/20 SoC in %
ARTG, 16,     // 135, 136   AC adapter rating in 10 mW. Max adapter power supported. Because of EC-space constraint EC can only send 2bytes=15000mW but system uses 150000mW hence bios will multiply this value by 10 and return.
CTYP, 8,      // 137        Charger type, Traditional or Hybrid
AP01, 8,      // 138        Returns AC source 1ms period percentage overload in 1% unit.
AP02, 8,      // 139        Returns AC source 2ms period percentage overload in 1% unit.
AP10, 8,      // 140        Returns AC source 10ms period percentage overload in 1% unit.
PBSS, 16,     // 141, 142   Max sustained power for battery (mW)
BICC, 16,     // 143, 144   _BIX.Cycle Count
Offset(145),
TSR1, 16,     // 145, 146   Sensor SEN1
TSR2, 16,     // 147, 148   Sensor SEN2
TSR3, 16,     // 149, 150   Sensor SEN3
TSR4, 16,     // 151, 152   Sensor SEN4
TSR5, 16,     // 153, 154   Sensor SEN5
Offset(155),
PRC1, 8,      // 155        Peci Repeat Cycle (LSB)
PRC2, 8,      // 156        Peci Repeat Cycle
PRC3, 8,      // 157        Peci Repeat Cycle
PRC4, 8,      // 158        Peci Repeat Cycle (MSB)
PRPI, 8,      // 159        Peci Repeat Period Interval
PECN, 8,      // 160        Peci Error Count Number
PCAD, 8,      // 161        Peci Clinet Addess
PEWL, 8,      // 162        Write Length
PWRL, 8,      // 163        Read lenght
PECD, 8,      // 164        Command Code
PEHI, 8,      // 165        Host ID
PECI, 8,      // 166        Index
PEPL, 8,      // 167        Parameter (LSB)
PEPM, 8,      // 168        Parameter (MSB)
PWFC, 8,      // 169        Write FCS
PECC, 8,      // 170        Completion code
PDT0, 8,      // 171        Data 0
PDT1, 8,      // 172        Data 1
PDT2, 8,      // 173        Data 2
PDT3, 8,      // 174        Data 3
PRFC, 8,      // 175        Read FCS
PRS0, 8,      // 176        Reserved
PRS1, 8,      // 177        Reserved
PRS2, 8,      // 178        Reserved
PRS3, 8,      // 179        Reserved
PRS4, 8,      // 180        Reserved
Offset(187),
PRCS, 8,      // 187        Peci Repeat Command Status
PEC0, 8,      // 188        Peci Error Count (LSB)
PEC1, 8,      // 189        Peci Error Count
PEC2, 8,      // 190        Peci Error Count
PEC3, 8,      // 191        Peci Error Count (MSB)
Offset(193),  // 193        PD PS_ON (C1)
EXAC, 1,      // uint16_t pson_entry_exit_ack:1
SCIP, 1,      // usbc_src_pwr_sci_pending
TSPW, 14,     // usbc_total_src_pwr:14; Operationg wattage in 10mW granularity
Offset(195),
WTMM, 8,      // 195        Wake timer mode (C3)
WTMV, 32,     // 196-199    Wake time value (C4)
Offset(201),
BTEN, 8,      // 201        Button Enable/Disable
              //              201.0: Power Button N/A to disable
              //              201.1: Windows Button
              //              201.2: Volume Up Button
              //              201.3: Volume Down Button
              //              201.4: Rotation Lock Button
              //              201.5-8: Reserved
CHGA, 16,     // 202, 203   PPS Charge Current (mA)
CHGV, 16,     // 204, 205   PPS Charge Voltage (mV)
Offset(211),
B1DV, 16,     // 211, 212   Battery 1 Design Voltage (mV)
Offset(215),
BMAX, 16,     // 215, 216   Battery A maximum
Offset(219),
BTP1, 8,      // 219        Battery 1 Trip Point in %
B1TL, 8,      // 220        Battery 1 Trip Point lower byte in mAh
B1TH, 8,      // 221        Battery 1 Trip Point higher byte in mAh
KBDB, 8,      // 222        Keyboard brightness Percentage
CHGR, 16,     // 223, 224   Battery Charge Rate
Offset(231),
SCCK, 8,      // 231        Ctrl+Alt+Shit(CAS)+Key Scan Code
Offset(233),
FCHG, 8,      // 233        Fast Charging Enabled.
CTL0, 8,      // 234        OPM->PPM Control message, 8 bytes,
CTL1, 8,      // 235
CTL2, 8,      // 236
CTL3, 8,      // 237
CTL4, 8,      // 238
CTL5, 8,      // 239
CTL6, 8,      // 240
CTL7, 8,      // 241
Offset(242),  // 242        (0xF2)
UCMC, 8,      // 242        USB typeC mailbox command
UCMD, 8,      // 243        USB typeC mailbox data
VBUS, 1,      // 244.1      VBUS Sense
OTGI, 1,      // 244.2      OTG ID
Offset(245),
RBHF, 16,     // 245, 246   Battery high frequency impedance
VBNL, 16,     // 247, 248   Battery No-Load Voltage
CMPP, 16,     // 249, 250   Battery Maximum peak current
Offset(251),
UVTH, 16,     // 251, 252   Under Voltage Threshold
}
