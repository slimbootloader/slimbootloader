/** @file
  Intel ACPI Reference Code for Intel(R) Dynamic Tuning Technology
  Used for driver debugging and validation.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(\_SB.PC00.LPCB.H_EC.CHRG, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.DGPU, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.SEN2, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.SEN3, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.SEN4, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.SEN5, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.TFN1, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.TFN2, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.TFN3, DeviceObj)
External(\_SB.PC00.LPCB.ITE8.TFN1, DeviceObj)
External(\_SB.PC00.LPCB.ITE8.TFN2, DeviceObj)
External(\_SB.PC00.LPCB.ITE8.SEN1, DeviceObj)
External(\_SB.PC00.LPCB.ITE8.SEN2, DeviceObj)
External(\_SB.PC00.LPCB.ITE8.SEN3, DeviceObj)
External(\_SB.TPWR, DeviceObj)

Scope(\_SB.IETM)
{
  // TEVT (Trigger EvenT)
  // Used for Intel(R) Dynamic Tuning Technology driver debugging and validation
  // This function will generate an event of the event code as given in arg1 for the participant given in parameter arg0.
  // Arguments: (2)
  //   Arg0 - The device object name string.
  //   Arg1 - An Integer containing the notification code to be used in Notify.
  // Return Value:
  //   None
  //
  Method(TEVT,2,Serialized,,,{StrObj,IntObj})
  {
    Switch(ToInteger(Arg0))
    {
      case("IETM") {
        Notify(\_SB.IETM, Arg1) // Intel Extended Thermal Manager (Intel(R) Dynamic Tuning Technology Manager)
      }

      case("TCPU") {
        Notify(\_SB.PC00.TCPU, Arg1) // TCPU Participant (SA Thermal Device)
      }

      case("TPCH") {
        Notify(\_SB.TPCH, Arg1) // PCH FIVR Participant
      }
    }

    If (\ECON) {
      Switch(ToInteger(Arg0))
      {
        case("CHRG") {
          Notify(\_SB.PC00.LPCB.H_EC.CHRG, Arg1) // Charger Participant
        }

        case("DGPU") {
          Notify(\_SB.PC00.LPCB.H_EC.DGPU, Arg1) // Sensor Participant
        }

        case("SEN2") {
          Notify(\_SB.PC00.LPCB.H_EC.SEN2, Arg1) // Sensor Participant
        }

        case("SEN3") {
          Notify(\_SB.PC00.LPCB.H_EC.SEN3, Arg1) // Sensor Participant
        }

        case("SEN4") {
          Notify(\_SB.PC00.LPCB.H_EC.SEN4, Arg1) // Sensor Participant
        }

        case("SEN5") {
          Notify(\_SB.PC00.LPCB.H_EC.SEN5, Arg1) // Sensor Participant
        }

        case("TFN1") {
          Notify(\_SB.PC00.LPCB.H_EC.TFN1, Arg1) // Fan Participant
        }

        case("TFN2") {
          Notify(\_SB.PC00.LPCB.H_EC.TFN2, Arg1) // Fan Participant
        }

        case("TFN3") {
          Notify(\_SB.PC00.LPCB.H_EC.TFN3, Arg1) // Fan Participant
        }

        case("TPWR") {
          Notify(\_SB.TPWR, Arg1) // Power Participant
        }
      }
    } Else {
        Switch(ToInteger(Arg0))
      {
        case("SEN1") {
          Notify(\_SB.PC00.LPCB.ITE8.SEN1, Arg1) // Sensor Participant
        }

        case("SEN2") {
          Notify(\_SB.PC00.LPCB.ITE8.SEN2, Arg1) // Sensor Participant
        }

        case("SEN3") {
          Notify(\_SB.PC00.LPCB.ITE8.SEN3, Arg1) // Sensor Participant
        }

        case("TFN1") {
          Notify(\_SB.PC00.LPCB.ITE8.TFN1, Arg1) // Fan1 Participant
        }

        case("TFN2") {
          Notify(\_SB.PC00.LPCB.ITE8.TFN2, Arg1) // Fan2 Participant
        }
      }
    }
  }

} // End Scope(\_SB.IETM)
