/** @file
  ACPI RTD3 SSDT table for XDCI

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#if (FixedPcdGetBool (PcdMtlSSupport) == 1)
#define ROOT_BRIDGE_COMMON_STR1 \_SB.PC02
#else
#define ROOT_BRIDGE_COMMON_STR1 \_SB.PC00
#endif

//
// XDCI - start
//
  If(LEqual(XDCE,1)) {
    Scope(\_SB)
    {
      //
      // Dummy power resource for USB D3 cold support
      //
      PowerResource(USBC, 0, 0)
      {
        Method(_STA) { Return (0xF) }
        Method(_ON) {}
        Method(_OFF) {}
      }
    }

    Scope(ROOT_BRIDGE_COMMON_STR1.XDCI)
    {
      OperationRegion (GENR, SystemMemory, Add(And(XDCB, 0xFFFFFFFFFFFFFF00), 0x10F81C), 0x4)  //AON MMIO - 10F81C: APBFC_U3PMU_CFG5
      Field (GENR, WordAcc, NoLock, Preserve)
      {
            ,   2,
        CPME,   1,    //bit2 core_pme_en
        U3EN,   1,    //bit3 u3_pme_en
        U2EN,   1     //bit4 u2_pme_en
      }

      Method (_PS3, 0, NotSerialized)
      {
        Store (One, CPME)
        Store (One, U2EN)
        Store (One, U3EN)

        ADBG("XDC CSD3")
        \_SB.CSD3(MODPHY_SPD_GATING_XDCI)
      }
      Method (_PS0, 0, NotSerialized)
      {
        Store (Zero, CPME)
        Store (Zero, U2EN)
        Store (Zero, U3EN)

        If(LEqual(DVID,0xFFFF)) {
          Return()
        }

        ADBG("XDC CSD0")
        \_SB.CSD0(MODPHY_SPD_GATING_XDCI)
      }

      Method (_RMV, 0, NotSerialized)  // _RMV: Removal Status
      {
        Return (Zero)
      }

      Method (_PR3, 0, NotSerialized)  // _PR3: Power Resources for D3hot
      {
        Return (Package (0x01)
        {
          USBC // return dummy package
        })
      }
    } // Scope(ROOT_BRIDGE_COMMON_STR1.XDCI)
  } // XDCE
//
// XDCI - end
//
