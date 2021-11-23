/**@file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PcieRegs.h>
#include <Register/CnviRegs.h>

Scope (\_SB.PC00) {
  Device(CNVW)
  {
    Name(_ADR, 0x00140003)

    //
    // Define a Memory Region that will allow access to the CNVi WiFi PCI Configuration Space
    //
    OperationRegion(CWAR, SystemMemory, Add(\_SB.PC00.GPCB(), 0xA3000), 0x100)
    Field(CWAR, WordAcc, NoLock, Preserve) {
      VDID, 32, // 0x00, VID DID
          ,  1,
      WMSE,  1, // MSE
      WBME,  1, // BME
      Offset(R_PCI_BAR0_OFFSET),
      WBR0, 64, // BAR0
      Offset(R_CNVI_CFG_WIFI_GIO_DEV_CAP),
          , 28,
      WFLR,  1, // Function Level Reset Capable
      Offset(R_CNVI_CFG_WIFI_GIO_DEV_CTRL),
          , 15,
      WIFR,  1, // Init Function Level Reset
      Offset(R_CNVI_CFG_WIFI_PMCSR),
      WPMS, 32,
    }
    Method(_S0W, 0x0, NotSerialized)
    {
      Return (0x3)
    }
    Method(_PRW, 0)
    {
      Return (GPRW(0x6D, 4)) // can wakeup from S4 state
    }

    Method(_PS0, 0, Serialized) {
      // S0i2/3 dynamic switching for CNVi WiFi (Arg0 = 2), active (Arg1 = 1)
      \_SB.S023(2, 1)
    }

    Method(_PS3, 0, Serialized) {
      //
      // Check if:
      //  CNVI.PCR.STAT[5:0](CNVI_STATE) = 0xC
      //  CNVI.PCR.STAT[6](CNVI_STATE_VAL) = 1
      //
      Store (PCRR (\PCNV, R_CNVI_PCR_STAT), Local0)
      If (LEqual(And(Local0, 0x7F), 0x4C)) {
        // S0i2/3 dynamic switching for CNVi WiFi (Arg0 = 2), inactive (Arg1 = 0)
        \_SB.S023(2, 0)
      }
    }

    Method(_DSW, 3) {}

    // Define Platform-level device reset power resource
    PowerResource(WRST, 5, 0)
    {
      // Define the PowerResource for CNVi WiFi
      // PowerResource expects to have _STA, _ON and _OFF Method per ACPI Spec. Not having one of them will cause BSOD

      // Method: Dummy _STA() to comply with ACPI Spec
      Method(_STA)
      {
        Return (0x01)
      }

      // Method: Dummy _ON() to comply with ACPI Spec
      Method(_ON, 0)
      {
      }

      // Method: Dummy _OFF() to comply with ACPI Spec
      Method(_OFF, 0)
      {
      }

      Method(_RST, 0, NotSerialized)
      {
        If(LEqual (WFLR, 1))
        {
          Store (0, WBR0)
          Store (0, WPMS)
          Store (0, WBME)
          Store (0, WMSE)
          Store (1, WIFR)
        }
      }
    } // End WRST

    Name(_PRR, Package(){WRST})
  }

  //
  // CNVi is present
  //
  Method(CNIP)
  {
    If (LNotEqual (\_SB.PC00.CNVW.VDID, 0xFFFFFFFF)){
      Return (0x01)
    } Else {
      Return (0x00)
    }
  }

  //
  // Set BT_EN
  //
  Method(SBTE, 0x1, Serialized)
  {
    //
    // Arg0 - Value to BT_EN
    //
#ifdef PCH_TGL
    If (LEqual (PCHS, PCHL)) { // PCH-LP
      Store (GPIO_VER2_LP_VGPIO0, Local0)
    } ElseIf (LEqual (PCHS, PCH_H)) {
      Store (GPIO_VER2_H_VGPIO0, Local0)
    } Else {
      Return ()
    }
#else
    If (LEqual (PCHS, PCHL)) {
      Store (GPIO_VER1_LP_VGPIO0, Local0)
    } ElseIf (LEqual (PCHS, PCH_N)) {
      Store (GPIO_VER1_N_VGPIO0, Local0)
    } Else {
      Store (GPIO_VER1_H_VGPIO0, Local0)
    }
#endif
    \_SB.SGOV (Local0, Arg0)

    // S0i2/3 dynamic switching for CNVi BT
    \_SB.S023(3, Arg0)
  }

  //
  // Get BT_EN value
  //
  Method(GBTE, 0)
  {
#ifdef PCH_TGL
    If (LEqual (PCHS, PCHL)) { // PCH-LP
      Store (GPIO_VER2_LP_VGPIO0, Local0)
    } ElseIf (LEqual (PCHS, PCH_H)) {
      Store (GPIO_VER2_H_VGPIO0, Local0)
    } Else {
      Return (0)
    }
#else
    If (LEqual (PCHS, PCHL)) {
      Store (GPIO_VER1_LP_VGPIO0, Local0)
    } ElseIf (LEqual (PCHS, PCH_N)) {
      Store (GPIO_VER1_N_VGPIO0, Local0)
    } Else {
      Store (GPIO_VER1_H_VGPIO0, Local0)
    }
#endif
    Return (\_SB.GGOV (Local0))
  }

  //
  // Update BT Audio Offload setting
  //
  Method (AOLX)
  {
    // BT Audio Offload data structure
    Name (AODS, Package () {
      0,    // Revision
      0x12, // DomainType, 0x12:Bluetooth Core
      0     // Audio Offload selection, 0: Disabled, 1:Enabled
      })

    If (LEqual (CBTA, 1)) {
      Store(1, Index (AODS, 2))
    }
    Return (AODS)
  }
}

//
// AOLD (Audio Offload)
//   Arguments:
//     None
//   Return Value:
//     Revision
//     DomainType
//     Audio Offload selection, default value is Disabled
//
If (AND (\_SB.PC00.CNIP (), LEqual (CBTC, 1))) {
  If (LEqual (PCHS, PCH_LP)) {
    Scope(\_SB.PC00.XHCI.RHUB.HS10)
    {
      Method(AOLD)
      {
        Return (AOLX ())
      }
    } // Scope(\_SB.PC00.XHCI.RHUB.HS10)
  } ElseIf (LEqual (PCHS, PCH_H)) {
    Scope(\_SB.PC00.XHCI.RHUB.HS14)
    {
      Method(AOLD)
      {
        Return (AOLX ())
      }
    } // Scope(\_SB.PC00.XHCI.RHUB.HS14)
  } // If (LEqual(PCHS, PCHL))
} // If (AND (\_SB.PC00.CNIP (), LEqual (CBTC, 1)))

