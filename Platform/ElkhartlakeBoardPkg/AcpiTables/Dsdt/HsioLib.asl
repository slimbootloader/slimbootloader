/**@file

 Copyright (c) 2015 - 2020, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/
External(SIME)

//
// HSIO Library
//
Scope(\_SB) {
  //
  // PCIe root port modPHY power gating enable
  // Arg0 - Index of PCIe root port(1 - based)
  //
  Method(PSD3, 1, Serialized) {
    If (LEqual (SIME, 1)) {
      Return ()
    }
    Switch(Add(MODPHY_SPD_GATING_PCIE_RP1, Decrement(Arg0))) {
      Case (MODPHY_SPD_GATING_PCIE_RP1) {
        Store(1, RAA0)
        While(Lequal(APA0, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP2) {
        Store(1, RAA1)
        While(Lequal(APA1, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP3) {
        Store(1, RAA2)
        While(Lequal(APA2, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP4) {
        Store(1, RAA3)
        While(Lequal(APA3, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP5) {
        Store(1, RPB0)
        While(Lequal(APB0, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP6) {
        Store(1, RPB1)
        While(Lequal(APB1, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP7) {
        Store(1, RPB2)
        While(Lequal(APB2, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP8) {
        Store(1, RPB3)
        While(Lequal(APB3, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP9) {
        Store(1, RPC0)
        While(Lequal(APC0, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP10) {
        Store(1, RPC1)
        While(Lequal(APC1, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP11) {
        Store(1, RPC2)
        While(Lequal(APC2, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP12) {
        Store(1, RPC3)
        While(Lequal(APC3, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP13) {
        Store(1, RPD0)
        While(Lequal(APD0, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP14) {
        Store(1, RPD1)
        While(Lequal(APD1, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP15) {
        Store(1, RPD2)
        While(Lequal(APD2, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP16) {
        Store(1, RPD3)
        While(Lequal(APD3, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP17) {
        Store(1, RPE0)
        While(Lequal(APE0, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP18) {
        Store(1, RPE1)
        While(Lequal(APE1, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP19) {
        Store(1, RPE2)
        While(Lequal(APE2, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP20) {
        Store(1, RPE3)
        While(Lequal(APE3, 0)) {
          Stall(10)
        }
      }
    }
  }

  //
  // PCIe root port modPHY power gating disable
  // Arg0 - Index of PCIe root port(1 - based)
  //
  Method(PSD0, 1, Serialized) {
    If (LEqual (SIME, 1)) {
      Return ()
    }
    Switch(Add(MODPHY_SPD_GATING_PCIE_RP1, Decrement(Arg0))) {
      Case (MODPHY_SPD_GATING_PCIE_RP1) {
        Store(0, RAA0)
        While(Lequal(APA0, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP2) {
        Store(0, RAA1)
        While(Lequal(APA1, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP3) {
        Store(0, RAA2)
        While(Lequal(APA2, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP4) {
        Store(0, RAA3)
        While(Lequal(APA3, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP5) {
        Store(0, RPB0)
        While(Lequal(APB0, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP6) {
        Store(0, RPB1)
        While(Lequal(APB1, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP7) {
        Store(0, RPB2)
        While(Lequal(APB2, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP8) {
        Store(0, RPB3)
        While(Lequal(APB3, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP9) {
        Store(0, RPC0)
        While(Lequal(APC0, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP10) {
        Store(0, RPC1)
        While(Lequal(APC1, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP11) {
        Store(0, RPC2)
        While(Lequal(APC2, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP12) {
        Store(0, RPC3)
        While(Lequal(APC3, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP13) {
        Store(0, RPD0)
        While(Lequal(APD0, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP14) {
        Store(0, RPD1)
        While(Lequal(APD1, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP15) {
        Store(0, RPD2)
        While(Lequal(APD2, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP16) {
        Store(0, RPD3)
        While(Lequal(APD3, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP17) {
        Store(0, RPE0)
        While(Lequal(APE0, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP18) {
        Store(0, RPE1)
        While(Lequal(APE1, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP19) {
        Store(0, RPE2)
        While(Lequal(APE2, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_PCIE_RP20) {
        Store(0, RPE3)
        While(Lequal(APE3, 1)) {
          Stall(10)
        }
      }
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
    If (LEqual (SIME, 1)) {
      Return ()
    }
    Switch (ToInteger(arg0)) {
      Case (MODPHY_SPD_GATING_SATA) {
        Store(1, RSAT)
        While(Lequal(ASAT, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_GBE) {
        Store(1, RGBE)
        While(Lequal(AGBE, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_XHCI) {
        Store(1, RXHC)
        While(Lequal(AXHC, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_XDCI) {
        Store(1, RXDC)
        While(Lequal(AXDC, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_UFS0) {
        Store(1, RUF0)
        While(Lequal(AUF0, 0)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_UFS1) {
        Store(1, RUF1)
        While(Lequal(AUF1, 0)) {
          Stall(10)
        }
      }
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
    If (LEqual (SIME, 1)) {
      Return ()
    }
    Switch (ToInteger(arg0)) {
      Case (MODPHY_SPD_GATING_SATA) {
        Store(0, RSAT)
        While(Lequal(ASAT, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_GBE) {
        Store(0, RGBE)
        While(Lequal(AGBE, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_XHCI) {
        Store(0, RXHC)
        While(Lequal(AXHC, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_XDCI) {
        Store(0, RXDC)
        While(Lequal(AXDC, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_UFS0) {
        Store(0, RUF0)
        While(Lequal(AUF0, 1)) {
          Stall(10)
        }
      }
      Case (MODPHY_SPD_GATING_UFS1) {
        Store(0, RUF1)
        While(Lequal(AUF1, 1)) {
          Stall(10)
        }
      }
    }
  }
}

