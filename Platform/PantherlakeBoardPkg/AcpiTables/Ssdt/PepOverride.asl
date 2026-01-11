/** @file
  Pep Override ASL code

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define ROOT_COMPLEX \_SB.PC00
#define ROOT_COMPLEX_STR "\\_SB.PC00"
Include ("PepDevicesOverrideMethodDefinitions.asl")
Include ("PepPcieRpsTypeMethodDefinition.asl")
#undef ROOT_COMPLEX
#undef ROOT_COMPLEX_STR

#define ROOT_COMPLEX \_SB.PC02
#define ROOT_COMPLEX_STR "\\_SB.PC02"
Include ("PepDevicesOverrideMethodDefinitions.asl")
Include ("PepPcieRpsTypeMethodDefinition.asl")
#undef ROOT_COMPLEX
#undef ROOT_COMPLEX_STR

External (\_SB.PEPD, DeviceObj)

Scope (\_SB.PEPD)
{
  //
  // Get PEP OVR Constraints of PCIe Root Port of specific EP type
  // Arguments: (2)
  //    Arg0 - Defualt PEP value for the RP.
  //    Arg1 - PCIE EP Device type
  //         0->Other
  //         1->Storage
  //         2->LAN
  //         3->WLAN
  //         4->GFX
  // Returns: PEP constrant per EP type
  //         0 ->no constraint; 1->D0 F1; 3->D3
  //
  Method(RPCO, 2, Serialized) {
    If (LEqual (Arg1, 1)) {
      ADBG ("PCIE Storage PEP OVR")
      If (LAnd (LNotEqual(Arg0, 0), LGreaterEqual (PTPS, 1))) {
        If (LNotEqual (Arg0, 5)) {
          Return (PTPS)
        } Else {
          Return (Arg0)
        }
      } Else {
        Return (0)
      }
    }

    If (LEqual (Arg1, 2)) {
      ADBG ("PCIE LAN PEP OVR")
      If (LAnd (LNotEqual(Arg0, 0), LGreaterEqual (PTPL, 1))) {
        If (LNotEqual (Arg0, 5)) {
          Return (PTPL)
        } Else {
          Return (Arg0)
        }
      } Else {
        Return (0)
      }
    }

    If (LEqual (Arg1, 3)) {
      ADBG ("PCIE WLAN PEP OVR")
      If (LAnd (LNotEqual(Arg0, 0), LGreaterEqual (PTPW, 1))) {
        If (LNotEqual (Arg0, 5)) {
          Return (PTPW)
        } Else {
          Return (Arg0)
        }
      } Else {
        Return (0)
      }
    }

    If (LEqual (Arg1, 4)) {
      ADBG ("PCIE Graphics PEP OVR")
      If (LAnd (LNotEqual(Arg0, 0), LGreaterEqual (PTPG, 1))) {
        If (LNotEqual (Arg0, 5)) {
          Return (PTPG)
        } Else {
          Return (Arg0)
        }
      } Else {
        Return (0)
      }
    }

    If (LEqual (Arg1, 5)) {
      ADBG ("PCIE DTBT PEP OVR")
      If (LAnd (LNotEqual(Arg0, 0), LEqual (PTTS, 1))) {
        Return (Arg0)
      } Else {
        Return (0)
      }
    }

    If (LEqual (Arg1, 0)) {
      ADBG ("PCIE Others OVR")
      If (LAnd (LNotEqual(Arg0, 0), LNotEqual (PTPO, 0))) {
        If (LNotEqual (Arg0, 5)) {
          Return (PTPO)
        } Else {
          Return (Arg0)
        }
      } Else {
        Return (0)
      }
    }

    ADBG ("PCIE EP NOT present")
    Return (Arg0)
  }

  //
  // Get PEP OVR Constraints of CPU
  // Arguments: (1)
  //    Arg0 - Defualt PEP value for CPU.
  // Return value:
  //    The pep value for CPU.
  //
  Method(CUCO, 1, Serialized) {
    ADBG ("CPU PEP OVR")
    Return (PTCU)
  }

  //
  // Get PEP OVR Constraints of TCSS IPs
  // Arguments: (1)
  //    Arg0 - Defualt PEP value for TCSS.
  // Return value:
  //    The pep value for TCSS.
  //
  Method(TSCO, 1, Serialized) {
    ADBG ("TCSS PEP OVR")
    If (LAnd (LNotEqual (Arg0, 0), LEqual (PTTS, 1))) {
      Return (Arg0)
    } Else {
      Return (0)
    }
  }

}
