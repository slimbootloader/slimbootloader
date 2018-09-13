/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


// General Purpose Events.  This Scope handles the Run-time and
// Wake-time SCIs.  The specific method called will be determined by
// the _Lxx value, where xx equals the bit location in the General
// Purpose Event register(s).

External(HGLS, MethodObj)
External(HGAS, MethodObj)

Scope(\_GPE)
{
  Method(_L0D, 0) {
    Notify(\_SB.PCI0.XHC, 0x02)
  }

  Method(_L0E, 0) {
    Notify(\_SB.PCI0.HDAS, 0x02)
  }

  // Dummy method for the Tier 1 GPIO SCI enable bit
  Method(_L0F, 0) {}

  Method(_L39){
      // Required for ACPI 5.0 native Windows support
      Notify(\_SB.PCI0.SATA.PRT0, 2) // Device Wake (Windows)
    }

  Method(_L08, 0) {
    \_SB.PCI0.RP02.HPME()
    Notify(\_SB.PCI0.RP02, 0x02)
  }

  Method(_L09, 0) {
    If (LEqual(RP1D,0)) {
      \_SB.PCI0.RP01.HPME()
      Notify(\_SB.PCI0.RP01, 0x02)
    }
    If(LEqual(RP2D,0)) {
      \_SB.PCI0.RP02.HPME()
      Notify(\_SB.PCI0.RP02, 0x02)
    }
    If(LEqual(RP3D,0)) {
      \_SB.PCI0.RP03.HPME()
      Notify(\_SB.PCI0.RP03, 0x02)
    }
    If(LEqual(RP4D,0)) {
      \_SB.PCI0.RP04.HPME()
      Notify(\_SB.PCI0.RP04, 0x02)
    }
    If(LEqual(RP5D,0)) {
      \_SB.PCI0.RP05.HPME()
      Notify(\_SB.PCI0.RP05, 0x02)
    }
    If(LEqual(RP6D,0)) {
      \_SB.PCI0.RP06.HPME()
      Notify(\_SB.PCI0.RP06, 0x02)
    }
  }
}

