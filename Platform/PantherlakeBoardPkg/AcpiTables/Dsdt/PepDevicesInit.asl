/** @file
  ACPI PEP deep standby entry/exit

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name (PCP0, Package () {0, Package () {0xFF, 0}})
Name (PCP1, Package () {0, Package () {0xFF, 0, 0x81}})
Name (PCP2, Package () {0, Package () {0xFF, 2}})
Name (PCP3, Package () {0, Package () {0xFF, 3}})

Name (DEVC, 0x0) // Device counter
Name (DEVS, 0x0) // DEVY Size
Name (PEPV, 0x0) // Device Constraint value
//
//  PEP deep device init base on the arguments.
//
//  Arguments: (3)
//    Arg0    - Parent scope string.
//    Arg1    - Device name string.
//    Arg2    - Pep constraint value for the devices.
//              0 - No Pep constraint support.
//              1 - PCP1
//              2 - PCP2
//              3 - PCP3
//              4 - PCP0
//              5 - Disable device without change D-state.
//
//  Return Value:
//    None.
//
Method (PDIN, 3, Serialized)
{
  If (LLess (DEVC, DEVS)) {
    If (LNotEqual (SizeOf (Arg1), 0)) {
      Store (Concatenate (Concatenate (Arg0, "."), Arg1), Local0)

      If (LNotEqual (Arg2, 0)) {
        //
        // Check if the device already present or not.
        //
        For (Local1 = Zero, Local1 < DEVC, Local1 += 1) {
          If (LEqual (Local0, DeRefOf (Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 0)))) {
            Break
          }
        }
        If (LEqual (Local1, DEVC)) {
          Store (Local0, Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 0))
        }

        Store (1, Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 1))

        If (LEqual (Arg2, 1)) {
          Store (PCP1, Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 2))
        } ElseIf (LEqual (Arg2, 2)) {
          Store (PCP2, Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 2))
        } ElseIf (LEqual (Arg2, 3)) {
          Store (PCP3, Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 2))
        } ElseIf (LEqual (Arg2, 4)) {
          Store (PCP0, Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 2))
        } ElseIf (LEqual (Arg2, 5)) {
          Store (0, Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 1))
        }

        If (LNotEqual (Arg2, 5)) {
          ADBG (Concatenate ("DEVY entry index :", ToDecimalString (Local1)))
          ADBG (Concatenate ("Device Descriptor                   :", DeRefOf (Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 0))))
          ADBG (Concatenate ("Constraint value (0 - 5, reserved)  :", Arg2))
          ADBG ("------------ ")
          ADBG ("")
        } ElseIf (LNotEqual (Arg0, "Reserved")) {
          ADBG (Concatenate ("DEVY entry index :", ToDecimalString (Local1)))
          ADBG (Concatenate ("Device Descriptor (Disable)         :", DeRefOf (Index (DeRefOf (Index (\_SB.PEPD.DEVY, Local1)), 0))))
        }
        If (LEqual (Local1, DEVC)) {
          Increment (DEVC)
        }
      }
    }
  } Else {
    ADBG (Concatenate ("PEP UPDE Error!!!! DEVY size is excedded, DEVY size = ", ToDecimalString (DEVS)))
  }
}

External (\_SB.PC00.PC2M, MethodObj)
//
// Check if the PCIe device is present or not.
// And return the default PEP constraint value.
//
// Arguments: (2)
//  Arg0    - PCIe Device _ADR (Device[24:20], Function[6:0]).
//  Arg1    - PCIe Device Bus
//  Arg2    - Default Pep constraint value if device present.
//
// Return value:
//  The pep constraint value:
//    0   - disable.
//    1   - PCP1
//    2   - PCP2
//    3   - PCP3
//    4   - PCP0
//    5   - Disable device without change D-state.
//
Method (PFEK, 3, Serialized)
{
  ADBG (Concatenate ("PFEK Adress: ", ToHexString (Arg0)))
  If (CondRefOf (\_SB.PC00.PC2M)) {
    Store (Arg1, Local0)
    OperationRegion (PCFG, SystemMemory, \_SB.PC00.PC2M (Arg0, Local0), 0x10)
    Field (PCFG, AnyAcc, NoLock, Preserve)
    {
      Offset (0),
      VDID, 16,
      DDID, 16,
      Offset (0x0A),
      SCCX, 8,    // Sub Class Code
      BCCX, 8,    // Base Class Code
    }
    If (VDID != 0xFFFF) {
      //
      // Check for the AHCI controller:
      // when VMD is enabled, the device ID of ACHI controller
      // would be change to a dummy value - 0x09AB.
      //
      If (LAnd (LEqual (BCCX, 0x01), LEqual (SCCX, 0x06))) {
        ADBG ("PFEK check for AHCI controller!")
        If (DDID == 0x09AB) {
          Return (0)
        }
      }
      Return (Arg2)
    } Else {
      Return (5)
    }
  } Else {
    ADBG ("FATAL ERROR: PC2M is not support!")
    Return (0)
  }
}

External (\_SB.PEPD.CUCO, MethodObj)
//
// Processor devices PEP constraint initialize.
//
// Arguments: (0)
//  None.
//
// Return Value:
//  None.
//
Method (PRCI, 0, Serialized)
{
  PEPV = 0x01
  If (CondRefOf (\_SB.PEPD.CUCO)) {
    PEPV = \_SB.PEPD.CUCO (PEPV)
  }
  ADBG (Concatenate ("PEP CPU TCNT: ", ToHexString (TCNT)))
  If (LNotEqual (PEPV, 0)) {
    For (Local0 = Zero, Local0 < TCNT, Local0 += 1) {
      If (Local0 < 10) {
        Concatenate ("PR0", ToDecimalString (Local0), Local1)
      } Else {
        Concatenate ("PR", ToDecimalString (Local0), Local1)
      }
      PDIN ("\\_SB", Local1, 4)
    }
  }
}

//
// Update PEP devices.
//
// Arguments: (0)
//  None.
//
// Return Value:
//  None.
//
Method (UPDV, 0, Serialized)
{
  ADBG (Concatenate ("UPDV start, DEVS: ", SizeOf (DEVY)))
  ADBG ("Note: constraint value map:")
  ADBG ("     0 - disable")
  ADBG ("     1 - DOF1")
  ADBG ("     2 - D2")
  ADBG ("     3 - D3")
  ADBG ("     4 - Enable with D0")
  ADBG ("     5 - Disable and keep default setting")
  Store (sizeof (DEVY), DEVS)

  ^PRCI ()

#define ROOT_COMPLEX            \_SB.PC00
#define ROOT_COMPLEX_STR        "\\_SB.PC00"
#define ROOT_COMPLEX_BUS        0
  Include ("PepDevicesList.asl")
#undef ROOT_COMPLEX
#undef ROOT_COMPLEX_STR
#undef ROOT_COMPLEX_BUS

#define ROOT_COMPLEX            \_SB.PC02
#define ROOT_COMPLEX_STR        "\\_SB.PC02"
#define ROOT_COMPLEX_BUS        \PBSB
External (\PBSB, FieldUnitObj)
  Include ("PepDevicesList.asl")
#undef ROOT_COMPLEX
#undef ROOT_COMPLEX_STR
#undef ROOT_COMPLEX_BUS

  //
  // Fill the remain PEP device list with unique name to
  // avoid BSOD.
  //
  For (Local0 = DEVC, Local0 < DEVS, Local0 += 1) {
    PDIN ("Reserved", ToDecimalString (Local0), 5)
  }
}
