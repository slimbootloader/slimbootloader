/** @file
  ACPI EcLite Device Specific method implementation.

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  External(\_SB.TPWR, DeviceObj)
  External(\_SB.BTY1, DeviceObj)
  External(\_SB.BTY2, DeviceObj)
  External(\_SB.UBTC.UCEV, MethodObj)

  Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
  {
    //
    //  Intel Proprietary EcLite Event Reporting
    //
    If (LEqual(Arg0, ToUUID("91D936A7-1F01-49C6-A6B4-72F00AD8D8A5")))
    {
      Switch (ToInteger (Arg2))            // Switch to Function Index.
      {
        //
        // Function 0, Query Function - return supported functions BitIndex.
        //
        Case (0)
        {
          // Revision 0
          If (LEqual(Arg1, Zero)) // The current revision is 0
          {
            Return(Buffer(){0x01, 0x20, 0x00, 0x00})
          }
          Else
          {
            Return(0) // Revision mismatch
          }
        }
        //
        // Function  UCSI update [TCPM TASK]
        //
        Case (ECLITE_UCSI_UPDATE_EVENT)
        {
          If (CondRefOf(\_SB.UBTC.UCEV))
          {
          \_SB.UBTC.UCEV()
          }
          Return(0)
        }

        //
        // Power participant notifications
        //
        Case (ToInteger (ECLITE_CHARGER_CONNECT_EVENT))
        {
          // Set Global Power State = Charger Mode.
          Store(ECLITE_AC_PRESENT,PWRS)

          // Perform needed ACPI Notifications.
          If(LAnd(LEqual(DPTF,1), LEqual(PWRE,1))) {
            If (CondRefOf (\_SB.TPWR)) {
            Notify(\_SB.TPWR, 0x81) // notify Power participant
            }
          }

          PNOT()
          Return(0)
        }

        Case (ToInteger (ECLITE_CHARGER_DISCONNECT_EVENT))
        {
          // Set Global Power State = Battery Mode.
          Store(ECLITE_DC_PRESENT,PWRS)

          // Perform needed ACPI Notifications.
          If(LAnd(LEqual(DPTF,1), LEqual(PWRE,1))) {
            If (CondRefOf (\_SB.TPWR)) {
            Notify(\_SB.TPWR, 0x81) // notify Power participant
            }
          }

          PNOT()
          Return(0)
        }

        Case (ToInteger (ECLITE_BATTERY_EVENT))
        {
          If(LAnd(LEqual(DPTF,1), LEqual(PWRE,1)))
          {
            If (CondRefOf (\_SB.TPWR)) {
            Notify(\_SB.TPWR, 0x86) // notify Power participant
            }
          }
          PNOT()
          Return(0)
        }

        Case (ToInteger (ECLITE_PMIC_TEMP_ALERT))
        {
          If(LAnd(LEqual(DPTF,1), LEqual(S1DE,1)))
          {
            If (CondRefOf (\_SB.SEN1)) {
            Notify(\_SB.SEN1, 0x90) // notify Sensor participant
            }
          }
          Return(0)
        }

        Case (ToInteger (ECLITE_SYSTEM0_TEMP_ALERT))
        {
          If(LAnd(LEqual(DPTF,1), LEqual(S2DE,1)))
          {
            If (CondRefOf (\_SB.SEN2)) {
            Notify(\_SB.SEN2, 0x90) // notify Sensor participant
            }
          }
          Return(0)
        }

        Case (ToInteger (ECLITE_SYSTEM1_TEMP_ALERT))
        {
          If(LAnd(LEqual(DPTF,1), LEqual(S3DE,1)))
          {
            If (CondRefOf (\_SB.SEN3)) {
            Notify(\_SB.SEN3, 0x90) // notify Sensor participant
            }
          }
          Return(0)
        }

        Case (ToInteger (ECLITE_BATTERY1_TEMP_ALERT))
        {
          If(LAnd(LEqual(DPTF,1), LEqual(S4DE,1)))
          {
            If (CondRefOf (\_SB.SEN4)) {
            Notify(\_SB.SEN4, 0x90) // notify Sensor participant
            }
          }
          Return(0)
        }

        Case (ToInteger (ECLITE_BATTERY2_TEMP_ALERT))
        {
          If(LAnd(LEqual(DPTF,1), LEqual(S5DE,1)))
          {
            If (CondRefOf (\_SB.SEN5)) {
            Notify(\_SB.SEN5, 0x90) // notify Sensor participant
            }
          }
          Return(0)
        }

        Case (ECLITE_PSRC_CHANGE_EVENT)
        {
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

          If(LAnd(LEqual(DPTF,1), LEqual(PWRE,1)))
          {
            If (CondRefOf (\_SB.TPWR)) {
            Notify(\_SB.TPWR, 0x81) // notify Power participant
            }
          }
          Return(0)
        }

        Case (ECLITE_BATTERY_CYCLE_COUNT_EVENT)
        {
          If(LAnd(LEqual(DPTF,1), LEqual(PWRE,1)))
          {
            If (CondRefOf (\_SB.TPWR)) {
            Notify(\_SB.TPWR, 0x82) // notify Power participant
            }
          }
          Return(0)
        }
        Case (ECLITE_CPU_TEMPERATURE_UPDATE)
        {
          If (CondRefOf (\_TZ.TZ00)) {
          Notify(\_TZ.TZ00,0x80)  // Notify Temprature update event to OS
          }
          Return(0)
        }

        Default
        {
          Return(0) // Function number mismatch but normal return.
        }
      } // End Function Index
    } // Guid Check

    If (LEqual(Arg0, ToUUID ("EEEC56B3-4442-408F-A792-4EDD4D758054")))
    {
      If (LEqual (1, ToInteger (Arg1)))        // Revision 1.
      {
        Switch (ToInteger (Arg2))            // Switch to Function Index.
        {
          //
          // Function 0, Query of supported functions.
          //
          Case (0)
          {
            Return (Buffer () {0xBF, 0x02}) // Total 6 function indices are supported including this.
          }
          //
          // Function 1, BTNL. Button Load Method. No Input/Output.
          //
          Case (1)
          {
            \_SB.HIDD.BTNL ()
          }
          //
          // Function 2, HDMM. HID Driver Mode Method.
          // Input:None
          // Output:HDMM output. See HDMM
          //
          Case (2)
          {
            Return (\_SB.HIDD.HDMM ())
          }
          //
          // Function 3, HDSM. HID Driver Status Method.
          // Input: 0 - The driver is not available. 1 - The driver is available.
          // Output: None
          //
          Case (3)
          {
            \_SB.HIDD.HDSM (DeRefOf (Index (Arg3, 0)))
          }
          //
          // Function 4, HDEM. HID Driver Event Method.
          // Input: None.
          // Output: Package contains Supported Keys (Mode 0)
          //
          Case (4)
          {
            Return (\_SB.HIDD.HDEM ())
          }
          //
          // Function 5 BTNS. Button Status Method.
          // Input: None.
          // Output: Int32 which contains a bit map of Buttons' enable/disable states
          //
          Case (5)
          {
            Return (\_SB.HIDD.BTNS ())
          }
          //
          // Function 7 HEBC. Button implemented state.
          // Input: None
          // Output: Int32 Bit map which shows what buttons are implemented on this system.
          //
          Case (7)
          {
            Return (\_SB.HIDD.HEBC ())
          }
          //
          // Function 9 H2BC. Button implemented state.
          // Input: None
          // Output: Int32 Bit map which shows what buttons are implemented on this system.
          //
          Case (9)
          {
            Return (\_SB.HIDD.H2BC ())
          }
        } // End Switch statement
      }  // End Revision check
    }  // End UUID check

    // If the code falls through to this point, just return a buffer of 0.

    Return (Buffer() {0x00})
  } // End of Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})

