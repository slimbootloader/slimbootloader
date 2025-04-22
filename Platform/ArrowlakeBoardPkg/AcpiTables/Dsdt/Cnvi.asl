/**@file
  CNVi Common ACPI definitions

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PcieRegs.h>
#include <Register/CnviRegs.h>

External (\_SB.BTRK, MethodObj)
External (\_SB.GBTR, MethodObj)
External (\CNMT)

Scope (\_SB.PC00) {
  Device (CNVW)
  {
    Name (_ADR, 0x00140003)

    // Reset Type
    //   0: Core Reset
    //   1: Product Reset
    Name (RSTT, 0)

    // Last _PRR Status
    //   0: Non-PLDR executed [Default]
    //   1: Core PLDR Completed successfully
    //   2: Product PLDR Completed successfully
    //   3: Previous PLDR Not Completed
    //   4: Product PLDR timeout
    Name (PRRS, 0)

    Name (PLRB, 0)

    //
    // Define a Memory Region that will allow access to the CNVi WiFi PCI Configuration Space
    //
    OperationRegion (CWAR, SystemMemory, Add (\_SB.PC00.GPCB (), 0xA3000), 0x100)
    Field (CWAR, WordAcc, NoLock, Preserve) {
      VDID, 32, // 0x00, VID DID
          ,  1,
      WMSE,  1, // MSE
      WBME,  1, // BME
      Offset (R_PCI_BAR0_OFFSET),
      WBR0, 64, // BAR0
      Offset (R_CNVI_CFG_WIFI_GIO_DEV_CAP),
          , 28,
      WFLR,  1, // Function Level Reset Capable
      Offset (R_CNVI_CFG_WIFI_GIO_DEV_CTRL),
          , 15,
      WIFR,  1, // Init Function Level Reset
      Offset (R_CNVI_CFG_WIFI_PMCSR),
      WPMS, 32,
    }
    Method (_S0W, 0x0, NotSerialized)
    {
      Return (0x3)
    }
    Method (_PRW, 0)
    {
      Return (GPRW (0x6D, 4)) // can wakeup from S4 state
    }

    Method (_PS0, 0, Serialized) {
      If (LNotEqual (VDID, 0xFFFFFFFF)) {
        // S0i2/3 dynamic switching for CNVi WiFi (Arg0 = 2), active (Arg1 = 1)
        \_SB.S023 (2, 1)
      }
    }

    Method (_PS3, 0, Serialized) {
      If (LNotEqual (VDID, 0xFFFFFFFF)) {
        //
        // Check if:
        //  CNVI.PCR.STAT[5:0](CNVI_STATE) = 0xC
        //  CNVI.PCR.STAT[6](CNVI_STATE_VAL) = 1
        //
        Store (PCRR (\PCNV, R_CNVI_PCR_STAT), Local0)
        If (LEqual (And (Local0, 0x7F), 0x4C)) {
          // S0i2/3 dynamic switching for CNVi WiFi (Arg0 = 2), inactive (Arg1 = 0)
          \_SB.S023 (2, 0)
        }
      }
    }

    Method (_DSW, 3) {}

    // Define Platform-level device reset power resource
    PowerResource (WRST, 5, 0) {
      // Define the PowerResource for CNVi WiFi
      // PowerResource expects to have _STA, _ON and _OFF Method per ACPI Spec. Not having one of them will cause BSOD

      // Method: Dummy _STA () to comply with ACPI Spec
      Method (_STA) {
        Return (0x01)
      }

      // Method: Dummy _ON () to comply with ACPI Spec
      Method (_ON, 0) {
      }

      // Method: Dummy _OFF () to comply with ACPI Spec
      Method (_OFF, 0) {
      }

      Method (_RST, 0, NotSerialized) {
        Store (Acquire (\CNMT, 1000), Local0) // save Acquire result so we can check for Mutex acquired
        If (LEqual (Local0, Zero)) // check for Mutex acquired
        {
          CFLR () // WiFi FLR
          Store (1, \_SB.PC00.CNVW.PRRS) // Set Core PLDR Completed successfully

          // Perform WiFi PLDR if RSTT = 1
          If (LAnd (CondRefOf (\_SB.PC00.CNVW.RSTT), LEqual (\_SB.PC00.CNVW.RSTT, 1))) {

            If (LEqual (PCHS, PCH_S)) {
              Store (R_CNVI_VER4_PCR_CNVI_PLDR_ABORT, PLRB)
            } Else {
              Store (R_CNVI_VER2_PCR_CNVI_PLDR_ABORT, PLRB)
            }

            // Query previous PLDR completed by checking PLDR_ABORT_REQUEST = 0
            If (LEqual (And (PCRR (\PCNV, PLRB), B_CNVI_PCR_CNVI_PLDR_ABORT_REQUEST), Zero)) {
              // If BT RF-Kill deasserted, assert BT RF-Kill and wait for 160 ms
              If (LEqual (\_SB.GBTR (), 1)) {
                \_SB.BTRK (0x00) // assert W_DISABLE2#
                Sleep (160)
                Store (1, Local2)
              }

              // Enable CNVi PLDR request and wait for 10 ms
              PCRO (\PCNV, PLRB, Or (B_CNVI_PCR_CNVI_PLDR_ABORT_ENABLE, B_CNVI_PCR_CNVI_PLDR_ABORT_REQUEST))
              Sleep (10)
              Store (PCRR (\PCNV, PLRB), Local1)

              // Check PLDR request is successful if PLDR_ABORT_REQUEST = 0 & CNVB_CNVI_READY = 1
              If (LAnd (LEqual (And (Local1, B_CNVI_PCR_CNVI_PLDR_ABORT_REQUEST), 0), And (Local1, B_CNVI_PCR_SCU_CNVB_CNVI_READY))) {
                Store (2, \_SB.PC00.CNVW.PRRS) // Set Product PLDR Completed successfully
                // Deassert BT RF-Kill and wait for 160 ms
                If (LEqual (Local2, 1)) {
                  \_SB.BTRK (0x01) // deassert W_DISABLE2#
                  Sleep (160)
                }
              } Else {
                // PLDR timeout
                Store (4, \_SB.PC00.CNVW.PRRS) // Set Product PLDR timeout
              }
            } Else {
              // Previous PLDR not completed
              Store (3, \_SB.PC00.CNVW.PRRS) // Set Previous PLDR Not Completed
            }
          }
          Release (\CNMT)
        }
      }
    } // End WRST

    Name (_PRR, Package () {WRST})

    Method (CFLR) {
      If (LEqual (WFLR, 1)) {
        Store (1, WIFR)
      }
    }
  }

  //
  // CNVi is present
  //
  Method (CNIP)
  {
    If (LOr (LNotEqual (\_SB.PC00.CNVW.VDID, 0xFFFFFFFF), LEqual (CRFP, 1))) {
      Return (0x01)
    } Else {
      Return (0x00)
    }
  }

  //
  // Set BT_EN
  //
  Method (SBTE, 0x1, Serialized)
  {
    //
    // Arg0 - Value to BT_EN
    //
    If (LNotEqual (GBTP, 0)) {
      Store (GBTP, Local0)
      \_SB.SGOV (Local0, Arg0)

      // S0i2/3 dynamic switching for CNVi BT
      \_SB.S023 (3, Arg0)
    }
  }

  //
  // Get BT_EN value
  //
  Method (GBTE, 0)
  {
    If (LNotEqual (GBTP, 0)) {
      Store (GBTP, Local0)
      Return (\_SB.GGOV (Local0))
    } Else {
      Return (0)
    }
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
      Store (1, Index (AODS, 2))
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
If (LAnd (\_SB.PC00.CNIP (), LEqual (CBTC, 1))) {
  If (LOr (LEqual (PCHS, PCH_LP), LOr (LEqual (PCHS, PCH_P), LEqual (PCHS, PCH_M)))) {
    Scope (\_SB.PC00.XHCI.RHUB.HS10)
    {
      Method (AOLD)
      {
        Return (AOLX ())
      }
    }
  } ElseIf (LOr (LEqual (PCHS, PCH_H), LEqual (PCHS, PCH_S))) {
    Scope (\_SB.PC00.XHCI.RHUB.HS14)
    {
      Method (AOLD)
      {
        Return (AOLX ())
      }
    }
  } ElseIf (LEqual (PCHS, PCH_N)) {
    Scope (\_SB.PC00.XHCI.RHUB.HS08)
    {
      Method (AOLD)
      {
        Return (AOLX ())
      }
    }
  }
}



