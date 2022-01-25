/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#define MODPHY_PG_TIMEOUT_IN_MS      10000
External(SIME)

//
// HSIO Library
Scope(\_SB) {
  //
  // PCIe root port modPHY power gating enable
  // Arg0 - Index of PCIe root port(1 - based)
  //
  Method(PSD3, 1, Serialized) {
    Store(0, Local0) //Time check counter variable

    If (LOr (LEqual (SIME, 1), LNot(IMPS()))) {
      Return ()
    }
    Switch(Add(MODPHY_SPD_GATING_PCIE_RP1, Decrement(Arg0))) {
      Case (MODPHY_SPD_GATING_PCIE_RP1) {
        Store(1, RAA0)
        While (LAnd(Lequal(APA0, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP2) {
        Store(1, RAA1)
        While (LAnd(Lequal(APA1, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP3) {
        Store(1, RAA2)
        While (LAnd(Lequal(APA2, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP4) {
        Store(1, RAA3)
        While (LAnd(Lequal(APA3, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP5) {
        Store(1, RPB0)
        While (LAnd(Lequal(APB0, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP6) {
        Store(1, RPB1)
        While (LAnd(Lequal(APB1, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP7) {
        Store(1, RPB2)
        While (LAnd(Lequal(APB2, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP8) {
        Store(1, RPB3)
        While (LAnd(Lequal(APB3, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP9) {
        Store(1, RPC0)
        While (LAnd(Lequal(APC0, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP10) {
        Store(1, RPC1)
        While (LAnd(Lequal(APC1, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP11) {
        Store(1, RPC2)
        While (LAnd(Lequal(APC2, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP12) {
        Store(1, RPC3)
        While (LAnd(Lequal(APC3, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP13) {
        Store(1, RPD0)
        While (LAnd(Lequal(APD0, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP14) {
        Store(1, RPD1)
        While (LAnd(Lequal(APD1, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP15) {
        Store(1, RPD2)
        While (LAnd(Lequal(APD2, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP16) {
        Store(1, RPD3)
        While (LAnd(Lequal(APD3, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP17) {
        Store(1, RPE0)
        While (LAnd(Lequal(APE0, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP18) {
        Store(1, RPE1)
        While (LAnd(Lequal(APE1, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP19) {
        Store(1, RPE2)
        While (LAnd(Lequal(APE2, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP20) {
        Store(1, RPE3)
        While (LAnd(Lequal(APE3, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
    }
    If(LGreaterEqual(Local0, MODPHY_PG_TIMEOUT_IN_MS)) {
      ADBG("Error: Timeout occurred")
    }
  }

  //
  // PCIe root port modPHY power gating disable
  // Arg0 - Index of PCIe root port(1 - based)
  //
  Method(PSD0, 1, Serialized) {
    Store(0, Local0) //Time check counter variable

    If (LOr (LEqual (SIME, 1), LNot(IMPS()))) {
      Return ()
    }

    Switch(Add(MODPHY_SPD_GATING_PCIE_RP1, Decrement(Arg0))) {
      Case (MODPHY_SPD_GATING_PCIE_RP1) {
        Store(0, RAA0)
        While (LAnd(Lequal(APA0, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP2) {
        Store(0, RAA1)
        While (LAnd(Lequal(APA1, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP3) {
        Store(0, RAA2)
        While (LAnd(Lequal(APA2, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP4) {
        Store(0, RAA3)
        While (LAnd(Lequal(APA3, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP5) {
        Store(0, RPB0)
        While (LAnd(Lequal(APB0, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP6) {
        Store(0, RPB1)
        While (LAnd(Lequal(APB1, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP7) {
        Store(0, RPB2)
        While (LAnd(Lequal(APB2, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP8) {
        Store(0, RPB3)
        While (LAnd(Lequal(APB3, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP9) {
        Store(0, RPC0)
        While (LAnd(Lequal(APC0, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP10) {
        Store(0, RPC1)
        While (LAnd(Lequal(APC1, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP11) {
        Store(0, RPC2)
        While (LAnd(Lequal(APC2, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP12) {
        Store(0, RPC3)
        While (LAnd(Lequal(APC3, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP13) {
        Store(0, RPD0)
        While (LAnd(Lequal(APD0, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP14) {
        Store(0, RPD1)
        While (LAnd(Lequal(APD1, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP15) {
        Store(0, RPD2)
        While (LAnd(Lequal(APD2, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP16) {
        Store(0, RPD3)
        While (LAnd(Lequal(APD3, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP17) {
        Store(0, RPE0)
        While (LAnd(Lequal(APE0, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP18) {
        Store(0, RPE1)
        While (LAnd(Lequal(APE1, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP19) {
        Store(0, RPE2)
        While (LAnd(Lequal(APE2, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP20) {
        Store(0, RPE3)
        While (LAnd(Lequal(APE3, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
    }
    If(LGreaterEqual(Local0, MODPHY_PG_TIMEOUT_IN_MS)) {
      ADBG("Error: Timeout occurred")
    }
  }

  //
  //Controller SPD D3 Method
  //
  // This method sets the appropriate R_CNL_PCH_PWRM_MODPHY_PM_CFG5[MSPDRTREQ],
  // Setting the MSPDRTREQ bit for a controller implies that ASL code provides consent for
  // SPD to be gated for the corresponding controller's lanes (PMC will gate SPD for the
  // corresponding controller's lanes).
  //
  // Arg0 - PCH Controller ENUM
  //
  Method(CSD3, 1, Serialized) {
    Store(0, Local0) //Time check counter variable

    If (LOr (LEqual (SIME, 1), LNot(IMPS()))) {
      Return ()
    }

    Switch (ToInteger(arg0)) {
      Case (MODPHY_SPD_GATING_SATA) {
        Store(1, RSAT)
        While (LAnd(Lequal(ASAT, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_GBE) {
        Store(1, RGBE)
        While (LAnd(Lequal(AGBE, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_XHCI) {
        Store(1, RXHC)
        While (LAnd(Lequal(AXHC, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_XDCI) {
        Store(1, RXDC)
        While (LAnd(Lequal(AXDC, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_UFS) {
        Store(1, RUFS)
        While (LAnd(Lequal(AUFS, 0), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
    }
    If(LGreaterEqual(Local0, MODPHY_PG_TIMEOUT_IN_MS)) {
      ADBG("Error: Timeout occurred")
    }
  }

  //
  // Controller SPD D0 Method
  //
  // This method clears the appropriate R_CNL_PCH_PWRM_MODPHY_PM_CFG5[MSPDRTREQ],
  // Clearing the MSPDRTREQ bit for a controller implies that ASL code does not provide
  // consent for SPD to be gated for the corresponding controller's lanes (PMC will "un-gate"
  // SPD for the corresponding controller's lanes).
  //
  // Arg0 - PCH Controller ENUM
  //
  Method(CSD0, 1, Serialized) {
    Store(0, Local0) //Time check counter variable

    If (LOr (LEqual (SIME, 1), LNot(IMPS()))) {
      Return ()
    }

    Switch (ToInteger(arg0)) {
      Case (MODPHY_SPD_GATING_SATA) {
        Store(0, RSAT)
        While (LAnd(Lequal(ASAT, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_GBE) {
        Store(0, RGBE)
        While (LAnd(Lequal(AGBE, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_XHCI) {
        Store(0, RXHC)
        While (LAnd(Lequal(AXHC, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_XDCI) {
        Store(0, RXDC)
        While (LAnd(Lequal(AXDC, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
      Case (MODPHY_SPD_GATING_UFS) {
        Store(0, RUFS)
        While (LAnd(Lequal(AUFS, 1), LLess(Local0, MODPHY_PG_TIMEOUT_IN_MS))) {
          Increment(Local0)
          Sleep(1)
        }
      }
    }
    If(LGreaterEqual(Local0, MODPHY_PG_TIMEOUT_IN_MS)) {
      ADBG("Error: Timeout occurred")
    }
  }
}

