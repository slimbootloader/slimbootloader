/** @file
  ACPI RTD3 Common Code for USB.

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Defining PSx methods for informing _psx method of XHCI.RHUB device
  // in PchXHCi.asl file that RTD3 is supported by platform or not. So depending
  // on these function it will Set/clear XHCI bit in MODPHY register. So don't
  // remove this functions.
  //
  Scope(\_SB.PC00.XHCI.RHUB){ //USB XHCI RHUB
    /// PS0X method is called by PS0 method in PchXHCi.asl
    Method(PS0X,0,Serialized)
    {
    }
    ///@defgroup XHCI_rhub_ps0x XHCI RHUB PS0X Method

    /// PS2X method is called by PS2 method in PchXHCIi.asl
    Method(PS2X,0,Serialized)
    {
    } // End of PS2 method
    ///@defgroup XHCI_rhub_ps2x XHCI RHUB PS2X Method

    /// PS3X method is called by _PS3 method in PchXHCi.asl
    Method(PS3X,0,Serialized)
    {
    } // End of PS3 method
    ///@defgroup XHCI_rhub_ps3x XHCI RHUB PS3X Method
  }
