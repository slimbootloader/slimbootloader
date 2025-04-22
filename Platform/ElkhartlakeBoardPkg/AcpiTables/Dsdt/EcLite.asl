/** @file
  ACPI Support for EcLite Mail Box Driver.

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "EcLite.h"

Scope (\_SB)
{
  Device (ECLT)
  {
    Name (_HID, "INTC1035")
    Name (_CID, "INTC1035")
    Name (_DDN, "Intel EC-Lite Device")
    Name (_UID, 0)

    Name (CMOA, 0)
    NAme (DAOA, 0)
    Name (ECLA, 0) // Until the EcLite driver is ready, the control methods cannot access the operation region
    Name (BNUM, 0) // Number Of Batteries Present

    Method (_STA, 0x0) {
      Return(0x000F)
    }
    //
    // EC-Lite device should be loaded only after PSE device is available
    //
    Name (_DEP, Package() {\_SB.PC00.PSED})
    // ECLite Mutex
    Mutex(ELMT, 0)

    // ECLite driver command operation region
    OperationRegion(CMOR, 0x9F, 0x0, 0x10)
    Field (CMOR, DWordAcc, NoLock, Preserve)
    {
      CMND, 32, // Command: Read=1, Write=2
      OFST, 32, // Offset for data buffer
      LNTH, 32, // Length of data buffer
      COMP, 32, // Type of Command/Op-region
    }

    // ECLite driver data operation region
    OperationRegion(DAOR, 0x9E, 0x0, 0x180)
    Field (DAOR, AnyAcc, NoLock, Preserve)
    {
      //
      // FG1 - BAT1
      //
      Offset (0),
      B1ST, 16,  // 000: Battery State
      B1PR, 16,  // 002: Battery Present Rate
      B1RC, 16,  // 004: Battery Remaining Capacity
      B1PV, 16,  // 006: Battery present voltage
      B1DC, 16,  // 008: Design Capacity
      B1FC, 16,  // 010: Full charge capacity
      B1DV, 16,  // 012: Design Voltage
      B1CC, 16,  // 014: Cycle count
      B1CW, 16,  // 016: Design Capacity of warning
      B1LW, 16,  // 018: Design Capacity of Low
      B1T0, 16,  // 020: Battery Trip Point[0]
      B1T1, 16,  // 022: Battery Trip Point[1]
      B1DR, 16,  // 024: Battery discharge Rate[0]
      B1WL, 16,  // 026: Battery Warning Level
      B1LL, 16,  // 028: Battery Low Level
                 // 030-037: Reserved

      //
      // FG1 - BAT2
      //
      Offset (38),
      B2ST, 16,  // 038: Battery State
      B2PR, 16,  // 040: Battery Present Rate
      B2RC, 16,  // 042: Battery Remaining Capacity
      B2PV, 16,  // 044: Battery present voltage
      B2DC, 16,  // 046: Design Capacity
      B2FC, 16,  // 048: Full charge capacity
      B2DV, 16,  // 050: Design Voltage
      B2CC, 16,  // 052: Cycle count
      B2CW, 16,  // 054: Design Capacity of warning
      B2LW, 16,  // 056: Design Capacity of Low
      B2T0, 16,  // 058: Battery Trip Point[0]
      B2T1, 16,  // 060: Battery Trip Point[1]
      B2DR, 16,  // 062: Battery discharge Rate[0]
      B2WL, 16,  // 064: Battery Warning Level
      B2LL, 16,  // 066: Battery Low Level
                 // 068-075: Reserved

      //
      // DPTF
      //
      Offset (76),
      S0T1, 16, // 076: System0 Temperature
      S1T1, 16, // 078: System1 Temperature
      S2T1, 16, // 080: System2 Temperature
      S3T1, 16, // 082: System3 Temperature
                // 084-099: Reserved
      Offset (100),
      S0A3, 16, // 100: Sys0_ALERT3(high)
      S0A1, 16, // 102: Sys0_ALERT1(low)
      S1A3, 16, // 104: Sys1_ALERT3(high)
      S1A1, 16, // 106: Sys1_ALERT1(low)
      S2A3, 16, // 108: Sys2_ALERT3(high)
      S2A1, 16, // 110: Sys2_ALERT1(low)
      S3A3, 16, // 112: Sys3_ALERT3(high)
      S3A1, 16, // 114: Sys3_ALERT1(low)
                // 116-147: Reserved
      Offset (148),
      S0CT, 16, // 148: SYS0_CRIT_TEMP_THR
      S1CT, 16, // 150: SYS1_CRIT_TEMP_THR
      S2CT, 16, // 152: SYS2_CRIT_TEMP_THR
      S3CT, 16, // 154: SYS2_CRIT_TEMP_THR
                // 156-173: Reserved
      Offset (174),
      CTCT, 16, // 174: CPU_CRIT_TEMP_THR
      CTHT, 16, // 176: CPUTherm Temp

      //
      // CHARGER 1
      //
      Offset (178),
      C1CC, 16, // 178: Charger 1 CC
      C1CV, 16, // 180: Charger 1 CV
      C1IN, 16, // 182: Charger 1 Charger Inlimit

      //
      // CHARGER 2
      //
      Offset (184),
      C2CC, 16, // 184: Charger 2 CC
      C2CV, 16, // 186: Charger 2 CV
      C2IN, 16, // 188: Charger 2 Charger Inlimit
                // 190-207: Reserved

      //
      // POWER BOSS
      //
      Offset(208),
                // 208-209: Reserved
      Offset(210),
      PSRC, 16, // 210: Current power source
                // 212-223: Reserved

      //
      // Processor Participants
      //
      Offset(224),
                  // 224-227: Reserved

      //
      // BATTERY 1
      //
      Offset(228),
                // 228 - 243: Reserved

      //
      // BATTERY 2
      //
      Offset(244),
                // 244 - 259: Reserved

      //
      // UCSI
      //
      Offset (260),
      MGO1, 32, // 260: MessageOut1
      MGO2, 32, // 264: MessageOut2
      MGO3, 32, // 268: MessageOut3
      MGO4, 32, // 272: MessageOut4
      CTL1, 32, // 276: Control1
      CTL2, 32, // 280: Control2
      MGI1, 32, // 284: MessageIn1
      MGI2, 32, // 288: MessageIn2
      MGI3, 32, // 292: MessageIn3
      MGI4, 32, // 296: MessageIn4
      CCI,  32, // 300: CCI
      UCRV, 16, // 304: UCSI Revision
                // 306-320: Reserved

      //
      // USB Mode
      //
      Offset (322),
      USBM, 8,  // 322: Host/Device mode
                // 323-323: Reserved

      //
      // Haptics
      //
      Offset (324),
                // 324-325: Reserved

      //
      // BIOS Mode
      //
      Offset (326),
                // 326-327: Reserved

      //
      // FAN Control
      //
      Offset (328),
      FPWM, 16,  // 328: FAN PWM
      TACO, 16,  // 330: TACHO
                 // 332-332: Reserved

      //
      // ECLite Event Notify Config
      //
      Offset (333),
      ENCG, 8,  // 333: ECLite Event Notify Config

      //
      // Reserved
      //
      Offset (334),
                // 334 -384: Reserved
    }

    Method(_REG,2)
    {
      If (LAnd(LEqual(Arg0,0x9F), LEqual(Arg1,1)))
      {
        Store(One, CMOA) // EcLite Command Opregion available
      }

      If (LAnd(LEqual(Arg0,0x9F), LEqual(Arg1,0)))
      {
        Store(Zero, CMOA) // EcLite Command Opregion not available
      }

      If (LAnd(LEqual(Arg0,0x9E), LEqual(Arg1,1)))
      {
        Store(One, DAOA) // EcLite Data Opregion available
      }

      If (LAnd(LEqual(Arg0,0x9E), LEqual(Arg1,0)))
      {
        Store(Zero, DAOA) // EcLite Data Opregion not available
      }

      And(CMOA, DAOA, ECLA)

      //
      // Ec Lite early initilization and notfications
      //
      If (ECLA)
      {
        ECLI()
      }
    }
    //
    // Name: ECLI
    // Description: Function for EC Lite early initilization
    // Input: None
    // Return: Data read from EcLite opregion for the given Field Unit
    //
    Method(ECLI)
    {
      //
      // Notify Firmware to start generating Host HECI Contifcations
      //
      \_SB.ECLT.ECLW( 1, RefOf(\_SB.ECLT.ENCG))
      \_SB.ECLT.ECLC(ECLITE_WRITE_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_ENCG_OFFSET, ECLITE_BYTES_COUNT_1)
      // Initialize the BNUM variable with total number if batteries present.
      //  1 = Real Battery 1 is present
      //  2 = Real Battery 2 is present
      //  3 = Real Battery 1 and 2 are present
      Store(0,BNUM)
      \_SB.ECLT.ECLC(ECLITE_READ_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_B1ST_OFFSET, ECLITE_BYTES_COUNT_2)
      Or(BNUM, ShiftRight(And( \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1ST)), 0x08),3),BNUM)
      \_SB.ECLT.ECLC(ECLITE_READ_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_B2ST_OFFSET, ECLITE_BYTES_COUNT_2)
      Or(BNUM,ShiftRight(And(\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B2ST)),0x08),2),BNUM)

      \_SB.ECLT.ECLC(ECLITE_READ_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_PSRC_OFFSET, ECLITE_BYTES_COUNT_2)
      Store(\_SB.ECLT.ECLR(RefOf(\_SB.ECLT.PSRC)), Local0)

      If(LNotEqual(And(ToInteger(Local0),ECLITE_PSRC_BIT_MASK),ECLITE_DC_PRESENT))
      {
        Store(ECLITE_AC_PRESENT,PWRS)
      }
      Else
      {
        Store(ECLITE_DC_PRESENT,PWRS)
      }

      // Perform needed ACPI Notifications.
      PNOT()
    }
    //
    // Name: ECLR
    // Description: Function to read the ECLite Opregion data
    // Input: Arg0 -> Operation region Field Unit
    // Return: Data read from ECLite opregion for the given Field Unit
    //
    Method(ECLR,1,Serialized, 0, IntObj, FieldUnitObj)
    {
      Store (Acquire(ELMT, 1000), Local0)  // save Acquire result so we can check for Mutex acquired
      If (LEqual(Local0, Zero))  // check for Mutex acquired
      {
        If (ECLA)
        {
          Store(DerefOf (Arg0), Local1) // Execute Read from ECLite Opregion
          Release(ELMT)
          Return(Local1)
        }
        Else
        {
          Release(ELMT)
        }
      }
      Return(0)
    } // End of Method(ECLR,1,Serialized, 0, IntObj, FieldUnitObj)

    //
    // Name: ECLW
    // Description: Function to write into the ECLite Opregion
    // Input: Arg0 -> Operation region Field Unit
    //        Arg1 -> Data
    // Return: Nothing
    //
    Method(ECLW,2,Serialized,,,{IntObj, FieldUnitObj})
    {
      Store (Acquire(ELMT, 1000), Local0)  // save Acquire result so we can check for Mutex acquired
      If (LEqual(Local0, Zero))  // check for Mutex acquired
      {
        If (ECLA)
        {
          Store(Arg0,Arg1) // Execute Write to ECLite opregion
        }
        Release(ELMT)
      }
    } // End of Method(ECLW,2,Serialized,,,{IntObj, FieldUnitObj})

    //
    // Name: ECLC
    // Description: Function to send command into the ECLite Opregion.
    //              READ command: This command is issued from bios to ECLite driver to initiate the Batch read operation from PSE to ECLite opregion
    //              WRITE command: This command is issued from bios to ECLite driver to initiate the Batch write operation from ECLite opregion to PSE
    // Input: Arg0 -> Command
    //        Arg1 -> Type Of Command/Opregion
    //        Arg2 -> Starting Offset
    //        Arg3 -> Length of data
    // Return: Nothing
    //
    Method(ECLC,4,Serialized)
    {
      Store (Acquire(ELMT, 1000), Local0)  // save Acquire result so we can check for Mutex acquired
      If (LEqual(Local0, Zero))  // check for Mutex acquired
      {
        If (ECLA)
        {
          Store(Arg2,OFST)
          Store(Arg3,LNTH)
          Store(Arg1,COMP)
          Store(Arg0,CMND)
          //
          // Delay for 10 milli-seconds as PSE roundtrip latency for READ/WRITE commands identied as 6 milli-seconds
          // Note: If ECLITE driver takes care of write-completion this code is optional from bios side
          //
          Sleep(10)
        }
        Release(ELMT)
      }
    } // End of Method(ECLC,3,Serialized)

    //
    // EcLite Event Reporting via DSM method
    //
    Include ("EcLiteDsm.asl")
  } // End of Device (ECLT)
} // End of Scope (\_SB)
