/**@file
  VMD remapped storage description for PCIe SSD remapped under VMD controller

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name (SD3C, 0) // Is device in D3Cold.

External (CBAR, MethodObj)
External (\_SB.PC00.VMD0.VDON, MethodObj)
External (\_SB.PC00.VMD0.VDOF, MethodObj)
External (D3CV, MethodObj)
External (RPD, IntObj)
External (RPF, IntObj)
External (RSPT, IntObj)
External (RSPI, IntObj)
External (_ADR, MethodObj)

//
// Method to find whether SATA port is part of logical volume
//
Method (VS3D) {
  Return (VD3C (_ADR ()))
}


//
// Method to turn ON remapped SATA Port.
//
Method (VSON, 0, Serialized) {

  // If device was not set in D3 cold do not re-initiate.
  If (LEqual (SD3C ,0)) {
    Return ()
  }

  // Turn on power to the remapped SATA port
  \_SB.PC00.VMD0.VDON (RSPT, RSPI)
  Sleep (16)       // Delay for power ramp.

  Store (0, SD3C)
}

//
// Method to turn OFF remapped SATA Port.
//
Method (VSOF, 0, Serialized) {
  // Check if root port supports D3Cold
  // Return if there is no support
  // to avoid L2/L3 ready transition
  If (LNot (D3CV (RSPT, RSPI))) {
    Return ()
  }

  // Turn off power to the remapped SATA port
  \_SB.PC00.VMD0.VDOF (RSPT, RSPI)

  Store (1, SD3C)
}
