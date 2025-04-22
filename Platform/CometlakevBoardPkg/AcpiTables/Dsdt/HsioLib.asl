/**@file

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// HSIO Library
//
Scope(\_SB) {
  //
  // PCIe root port modPHY power gating enable
  // Arg0 - Index of PCIe root port(1 - based)
  //
  Method(PSD3, 1, Serialized) {

  }

  //
  // PCIe root port modPHY power gating disable
  // Arg0 - Index of PCIe root port(1 - based)
  //
  Method(PSD0, 1, Serialized) {

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

  }
}
